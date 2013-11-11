// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the Protocol object
// 
//

#include <bluetooth/logger.h>
#include <e32def.h>
#include "rfcomm.h"
#include "rfcommstates.h"
#include "rfcommmuxer.h"
#include "rfcommmuxchannel.h"
#include "rfcommconsts.h" 
#include "bt.h"
#include "l2cap.h"
#include "IncomingConnListener.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

CRfcommProtocol::CRfcommProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
:	CBluetoothProtocolBase(aSecMan, aControlPlane, aCodMan),
	iMuxes(_FOFF(CRfcommMuxer, iLink)),
	iListeningSAPs(_FOFF(CRfcommSAP, iListeningLink)),
	iBoundSAPs(_FOFF(CRfcommSAP,iBoundLink)),
	iCBFCDisallowed(EFalse)
	{
	LOG_FUNC
	TCallBack cb(TryToClose, this);
	iIdleTimerEntry.Set(cb);
	}

CRfcommProtocol::~CRfcommProtocol()
	/**
	   Destructor.
	   We don't clear up the SAPs as they are owned by
	   ESOCK and it is up to it to destroy them.
	**/
	{
	LOG_FUNC
	RemoveIdleTimerEntry();
	if(LowerProtocol())
		LowerProtocol()->Close();  // Matches the bind
	delete iStateFactory;
	delete iFlowStrategyFactory;
	delete iMuxChannelStateFactory;
#ifdef __FLOG_ACTIVE
	CLOSE_LOGGER
#endif
	}

CRfcommProtocol* CRfcommProtocol::NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
#ifdef __FLOG_ACTIVE
	CONNECT_LOGGER
#endif
	LOG_STATIC_FUNC
	CRfcommProtocol* i=new (ELeave) CRfcommProtocol(aSecMan, aControlPlane, aCodMan);
	CleanupStack::PushL(i);
	i->ConstructL();
	CleanupStack::Pop(i);
	return i;
	}

void CRfcommProtocol::ConstructL()
	{
	LOG_FUNC
	}

void CRfcommProtocol::InitL(TDesC& /*aTag*/)
/**
	Pre-binding initialise.

	Alloc any stuff we need.
	This will only ever be called once during the lifetime of this
	protocol.
**/
	{
	LOG_FUNC
	// Create all the things we need now
	iStateFactory=CRfcommStateFactory::NewL();
	iFlowStrategyFactory=CRfcommFlowStrategyFactory::NewL();
	iMuxChannelStateFactory=CMuxChannelStateFactory::NewL();
	}

void CRfcommProtocol::StartL()
/**
	Binding complete.
**/
	{
	LOG_FUNC
	  // Should check that we're bound now.
	  if(!LowerProtocol())
	    {
		User::Leave(KErrRfcommNotBound);
	    }
	}


void CRfcommProtocol::BindToL(CProtocolBase* aProtocol)
  /***
      Request by Protocol Mgr to bind to the specified protocol.
      We can only be bound to one lower layer protocol, so the function leaves
      if we are already bound.
      @param aProtocol The protocol we need to bind to.
  **/
	{
	LOG_FUNC
	if(!LowerProtocol())
		{
#ifdef _DEBUG
		TServerProtocolDesc prtDesc;
		aProtocol->Identify(&prtDesc);

		if(prtDesc.iAddrFamily!=KBTAddrFamily ||
		   prtDesc.iProtocol!=KL2CAP)
			{
			User::Leave(KErrBtEskError);
			}
#endif

		iLowerProtocol=static_cast<CBluetoothProtocolBase*>(aProtocol);
		LowerProtocol()->BindL(this, 0); // id not used
		LowerProtocol()->Open();
		}
	else
		{
	    User::Leave(KErrRfcommAlreadyBound);
		}
	}

// Factory functions

CServProviderBase* CRfcommProtocol::NewSAPL(TUint aSockType)
  /** 
      Create a new SAP.
      The SAP returned is owned by the caller -- this protocol will
      not clean it up.  esock uses this function to create a new SAP,
      and esock will delete when it is finished with it.
  **/
	{
	LOG_FUNC

	CRfcommSAP* sap=0;
	
	switch(aSockType)
		{
	case KSockStream:    // the connected type
		sap=CRfcommSAP::NewL(*this);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	LOG1(_L("RFCOMM: NewSAPL created SAP %08x"), sap);
	return sap;
	}


TInt CRfcommProtocol::StartProtocolListening()
/**
	A SAP has wanted to start us listening...we look after the values to do this
**/
	{
	LOG_FUNC
	TInt rerr = CBluetoothProtocolBase::StartListening(KRFCOMMPSM, KSockSeqPacket, KRFCOMMIncomingConnQueueSize, KUidServiceRfcomm);
	if(rerr == KErrNone)
		//	We should now have an L2Cap listener. 
		//  Use it to setup L2Cap config for RFComm.
		{
		TL2CapConfigPkg configPkg;
		SetL2CapConfig(configPkg);
		rerr = Listener().SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configPkg);
		if(rerr!=KErrNone)
			{
			StopProtocolListening(); //clean up
			}
		}
	return rerr;
	}

// Query functions

void CRfcommProtocol::SetL2CapConfig(TL2CapConfigPkg& aConfigPkg)
/*
	L2Cap configuration required by RFComm
*/
	{
	LOG_FUNC
	aConfigPkg().ConfigureReliableChannel(TL2CapConfig::EDefaultRetransmission);
	aConfigPkg().SetMaxReceiveUnitSize(KMaxL2CAPMTUAllowed);
	}

void CRfcommProtocol::Identify(TServerProtocolDesc *aDesc)const
//
// Identify request from SOCKET server
//
	{
	LOG_FUNC
	CRfcommProtocol::ProtocolIdentity(aDesc);
	}

void CRfcommProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc)
	{
	LOG_STATIC_FUNC
	_LIT(name,"RFCOMM");
	aDesc->iProtocol=KRFCOMM;

	aDesc->iName=name;
	aDesc->iAddrFamily=KBTAddrFamily;
	aDesc->iSockType=KSockStream;
	
	aDesc->iVersion=TVersion(KBTMajor,KBTMinor,KBTBuild);
	aDesc->iByteOrder=ELittleEndian;
	aDesc->iServiceInfo=KRFCOMMStreamServiceInfo;
	aDesc->iNamingServices=0;
	aDesc->iSecurity=KSocketNoSecurity;
	aDesc->iMessageSize=KSocketMessageSizeIsStream;
	aDesc->iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains|EPreferDescriptors|EUseCanSend;
	aDesc->iNumSockets=100;
	}

void CRfcommProtocol::CloseNow()
	/**
	   Our reference is now zero, so start to close.

	   We don't actually close, merely Q a timer for a later close
	   down.  This close can be prempted by another open.
	**/
	{
	LOG_FUNC
	iClosePending = ETrue;
	QueIdleTimerEntry();
	}

void CRfcommProtocol::Open()
  /**
     Request to open the protocol.
     The protocol may be repeatedly opened and closed.  The order of calls is
     InitL, [Open *n , Close * n,  CloseNow] * m etc.
  **/
     {
	LOG_FUNC
	 iClosePending = EFalse;
	 RemoveIdleTimerEntry();
	 CProtocolBase::Open();
     }
     
void CRfcommProtocol::Close()
	/**
	   This is one session closing.
	**/
	{
	LOG_FUNC
	CProtocolBase::Close();
	}

CRfcommFlowStrategyFactory* CRfcommProtocol::FlowStrategyFactory() const
	{
	LOG_FUNC
	return iFlowStrategyFactory;
	}

CRfcommStateFactory* CRfcommProtocol::StateFactory() const
	{
	LOG_FUNC
	return iStateFactory;
	}

TInt CRfcommProtocol::GetMux(TBTSockAddr& aRemoteAddr, CRfcommMuxer*& aMuxer)
	/**
	Gets a muxer for the given BT device address.
	
	If it finds the muxer or successfully creates a new muxer, it returns 
	with KErrNone and a valid pointer to the correct muxer. On a failure, 
	it will return with a standard error, without a pointer to  a muxer.
	This flavour of GetMux is used when a muxer is being created in response 
	to a local request to connect to a remote device (i.e. no existing 
	L2CAP connection exists).
	**/
	{
	LOG_FUNC
	TBTDevAddr devAddr=aRemoteAddr.BTAddr();
	aMuxer=FindMux(devAddr);

	if(!aMuxer)
		{
		// Need to create one
		TRAPD(err, aMuxer=CRfcommMuxer::NewL(*this, *LowerProtocol(), *iMuxChannelStateFactory));
		if(err != KErrNone)
			{
			return err;
			}

		LOG1(_L("RFCOMM: (Outgoing) Creating new mux %08x"), aMuxer);
		LOG6(_L(" for BT Device %02x %02x %02x %02x %02x %02x"), TUint8(devAddr[0]), TUint8(devAddr[1]), TUint8(devAddr[2]), TUint8(devAddr[3]), TUint8(devAddr[4]), TUint8(devAddr[5]));
		// Add mux to the Q
		iMuxes.AddFirst(*aMuxer);
		aMuxer->Bind(devAddr);  // Sets the remote addr
		}
	
	return KErrNone;
	}

TInt CRfcommProtocol::BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP)
	{
	LOG_FUNC
	CRfcommMuxer* muxer = NULL; // don't care about getting pointer to muxer
	return GetMux(aAddr, aSAP, muxer);	//	NB If no error occurs, ownership of aSSP passes to the muxer.
	}

const TBTDevAddr& CRfcommProtocol::LocalBTAddr() const
	{
	LOG_FUNC
	return static_cast<const CL2CAPProtocol*>(iLowerProtocol)->LocalBTAddr();
	}

TInt CRfcommProtocol::GetMux(const TBTDevAddr& aRemoteAddr, CServProviderBase* aL2CAPConSAP, CRfcommMuxer*& aMuxer)
	/**
	Gets a muxer for the given BT device address, using the L2CAP connection provided.
	
	If it is found that a muxer for the specified BT device already exists, 
	KErrAlreadyExists will be returned. If a new muxer is successfully created, 
	KErrNone will be returned along with a valid pointer to the correct muxer in aMuxer 

	This flavour of GetMux is used when a muxer is being created in response to a 
	remote request to connect to the local device (i.e. an L2CAP SAP has been created 
	due to an incoming connection).
	**/
	{
	LOG_FUNC
#ifdef _DEBUG
	//	Check that the device address and the device of the L2CAP remote address match!!
	TBTSockAddr addr;
	aL2CAPConSAP->RemName(addr);
	__ASSERT_DEBUG(addr.BTAddr()==aRemoteAddr,Panic(ERfcommMismatchedAddressAndSAP));
#endif//_DEBUG

	aMuxer=FindMux(aRemoteAddr);
	TInt err=KErrNone;
	if(aMuxer!=NULL)	// i.e. a muxer already exists
		err=KErrAlreadyExists;
	else
		{
		TRAP(err, aMuxer=CRfcommMuxer::NewL(*this, aL2CAPConSAP, *iMuxChannelStateFactory));
		LOG1(_L("RFCOMM: (Incoming) Creating new mux %08x"), aMuxer);
		LOG6(_L(" for BT Device %02x %02x %02x %02x %02x %02x"),
					  TUint8(aRemoteAddr[0]), TUint8(aRemoteAddr[1]), TUint8(aRemoteAddr[2]), TUint8(aRemoteAddr[3]),
					  TUint8(aRemoteAddr[4]), TUint8(aRemoteAddr[5]));
		
		if(!err) 		// Add mux to the Q
			iMuxes.AddFirst(*aMuxer);
		}
	return err;
	}

CRfcommMuxer* CRfcommProtocol::FindMux(const TBTDevAddr& aAddr)
	/**
	   Find the existing muxer for this address.
	   @return  Pointer to a muxer for this address, or NULL if it doesn't exist.
	**/	
	{
	LOG_STATIC_FUNC
	TDblQueIter<CRfcommMuxer> iter(iMuxes);
	CRfcommMuxer* mux;

	while(iter)
		{
		mux=iter++;
		// Find a mux aimed at the same addr, then check it's not on its way down
		if((mux->iRemoteAddr == aAddr) && mux->iMuxChannel->CanAttachSAP())
			{
			// We have a mux, so return it
			return mux;
			}
		}
	// No mux
	return NULL;
	}

void CRfcommProtocol::MuxDown(CRfcommMuxer& aMux)
	/**
	   Called by the mux when it's dying.

	   We clean up and delete it.
	**/
	{
	LOG_FUNC
	LOG1(_L("RFCOMM: Mux %08x down"), &aMux);
	aMux.iLink.Deque();
	delete &aMux;

	if(CheckForClose())
		QueIdleTimerEntry();
	}

void CRfcommProtocol::AddIdleSAP(CRfcommSAP& aSAP)
	/**
	Used to inform the protocol of a SAP which has been told to listen
	**/
	{
	LOG_FUNC
#if defined(_DEBUG) || defined(__FLOG_ACTIVE)
	TDblQueIter<CRfcommSAP> iter(iListeningSAPs);
#endif

#ifdef _DEBUG
	//	Check that the SAP about to be added is not already on the listening list
	CRfcommSAP* mySAP=NULL;
	TBool found(EFalse);
	while(iter && !found)
		{
		mySAP=iter++;
		if(mySAP==&aSAP)	
			found=ETrue;
		}
	if(found)
		Panic(ERfcommRequeuingListeningSAP);
#endif

	iListeningSAPs.AddLast(aSAP);

#ifdef __FLOG_ACTIVE
	iter.SetToFirst();
	while(iter)
		{
		LOG1(_L("RFCOMM: Listening on channel %d"),(iter++)->ServerChannel());
		};
#endif //__FLOG_ACTIVE
	}

void CRfcommProtocol::AddBoundSAP(CRfcommSAP& aSAP)
	/**
	Used to inform the protocol of a SAP which has been told to bind
	**/
	{
	LOG_FUNC
	#ifdef _DEBUG
	TDblQueIter<CRfcommSAP> iter(iBoundSAPs);
	//	Check that the SAP about to be added is not already on the Bound list
	CRfcommSAP* mySAP=NULL;
	TBool found(EFalse);
	while(iter && !found)
		{
		mySAP=iter++;
		if(mySAP==&aSAP)	
			found=ETrue;
		}
	if(found)
		Panic(ERfcommRequeuingBoundSAP);
	#endif
	
	iBoundSAPs.AddLast(aSAP);
	}
	
void CRfcommProtocol::RemoveIdleSAP(CRfcommSAP& aSAP)
	/**
	Used to inform the protocol of a listening SAP which is about to be destroyed
	**/
	{
	LOG_FUNC
#ifdef _DEBUG
	//	Should check that the SAP about to be dequed is actually on the listening list
	CRfcommSAP* SAP=NULL;
	TBool found(EFalse);
	TDblQueIter<CRfcommSAP> iter(iListeningSAPs);
	while(iter && !found)
		{
		SAP=iter++;
		if(SAP==&aSAP)	
			found=ETrue;
		}
	if(!found)
		Panic(ERfcommDequeingBadListeningSAP);
#endif//_DEBUG

	aSAP.iListeningLink.Deque();
	}
	
void CRfcommProtocol::RemoveBoundSAP(CRfcommSAP& aSAP)
	/**
	Used to inform the protocol of a bound SAP which is about to be destroyed
	**/
	{
	LOG_FUNC
	//	check that the SAP about to be dequeued is actually on the bound list	
	__ASSERT_DEBUG(SAPIsBound(aSAP),Panic(ERfcommDequeuingBadBoundSAP));

	aSAP.iBoundLink.Deque();
	}
	
TBool CRfcommProtocol::SAPIsBound(const CRfcommSAP& aSAP)
	/**
	Used to inform the protocol if a SAP is bound  
	**/
	{
	LOG_FUNC
	CRfcommSAP* SAP=NULL;
	TBool found(EFalse);
	TDblQueIter<CRfcommSAP> iter(iBoundSAPs);
	
	while(iter && !found)
		{
		SAP=iter++;
		if(SAP==&aSAP)	
			found=ETrue;
		}

	return found;
	}

CRfcommSAP* CRfcommProtocol::FindInboundConnectedSAP(const TBTSockAddr& aSockAddr)
	/**
	Used to enquire of the protocol whether there is a cloned SAP created from a SAP told to
	listen on the specified address. This can be used to determine if a local Server Channel
	is in use even though the original listening SAP no longer exists.

	Returns NULL if no such SAP exists.
	**/
	{
	LOG_FUNC
	CRfcommSAP* SAP=NULL;
	CRfcommMuxer* mux=NULL;
	TUint8 dlci=0;

	// Search for matching SAP across all Muxes	
	TDblQueIter<CRfcommMuxer> iter(iMuxes);
	while(iter && !SAP)
		{
		mux=iter++;
		dlci=mux->MakeInboundDLCI(aSockAddr.Port());

		SAP=mux->FindSAP(dlci);
		}
	return SAP;	
	}
	
CRfcommSAP* CRfcommProtocol::FindIdleSAP(const TBTSockAddr& aSockAddr)
	/**
	Used to enquire of the protocol whether there is a SAP which has been told to listen on the specified address.

	Returns NULL if no such SAP exists.
	**/
	{
	LOG_FUNC
	CRfcommSAP* SAP=NULL;
	TDblQueIter<CRfcommSAP> iter(iListeningSAPs);
	while(iter)
		{
		SAP=iter++;
		if(SAP->ListeningTo(aSockAddr))
			//	Found a SAP which is interested...
			return SAP;
		}
	return NULL;
	}
	
CRfcommSAP* CRfcommProtocol::FindBoundSAP(const TBTSockAddr& aSockAddr)
	{
	LOG_FUNC
	CRfcommSAP* SAP=NULL;
	TDblQueIter<CRfcommSAP> iter(iBoundSAPs);
	while(iter)
		{
		SAP=iter++;
		if(SAP->BoundTo(aSockAddr))
			//	Found a SAP which is interested...
			return SAP;
		}
	return NULL;
	}

TInt CRfcommProtocol::FindFreeServerChannel()
	/**
	Returns a server channel on which this device is not currently bound or listening
	**/
	{
	LOG_FUNC
	// Array indexed from 0 to 29 but valid server channels are 1 to 30 so
	// valid server channel = index + 1
	TFixedArray<TBool, KMaxRfcommServerChannel> channelInUse;

	TDblQueIter<CRfcommSAP> iterListening(iListeningSAPs);
	TDblQueIter<CRfcommSAP> iterBound(iBoundSAPs);
	TDblQueIter<CRfcommMuxer> iterMuxes(iMuxes);
	
	for(TInt i = 0; i < KMaxRfcommServerChannel; i++)
		{
		channelInUse[i] = EFalse;
		}

	while(iterListening)
		{
		channelInUse[(*iterListening++).ServerChannel() - 1] = ETrue;
		}
		
	while(iterBound)
		{
		channelInUse[(*iterBound++).ServerChannel() - 1] = ETrue;
		}

	// Need to check Muxes for cloned SAPs that are not taken account of already 
	// because the associated listening SAP has been removed
	while(iterMuxes)
		{
		(*iterMuxes++).GetInboundServerChannelsInUse(channelInUse);
		}

	TInt index = 0;
	while(index < KMaxRfcommServerChannel && channelInUse[index++])
		{
		// Do nothing, index will be set appropriately when loop exits
		}

	return index < KMaxRfcommServerChannel ? index : KErrInUse;
	}

void CRfcommProtocol::DisallowCBFC()
	{
	LOG_FUNC
	iCBFCDisallowed = ETrue;
	}

void CRfcommProtocol::AllowCBFC()
	{
	LOG_FUNC
	iCBFCDisallowed = EFalse;
	}

TBool CRfcommProtocol::CBFCDisallowed()
	{
	LOG_FUNC
	return iCBFCDisallowed;
	}


TBool CRfcommProtocol::CheckForClose()
	/**
	   Called to check whether we can close down
	**/
	{
	LOG_FUNC
	return (iClosePending && iMuxes.IsEmpty());
	}

TInt CRfcommProtocol::TryToClose(TAny* aProtocol)
	{
	LOG_STATIC_FUNC
	CRfcommProtocol* p=static_cast<CRfcommProtocol*>(aProtocol);
	p->iIdleEntryQueued = EFalse;
	if (p->iClosePending && p->iMuxes.IsEmpty())
		{

		LOG(_L("RFCOMM: protocol can close now"));
		p->CanClose();
		}
	return EFalse;
	}
   
void CRfcommProtocol::RemoveIdleTimerEntry()
	/**
	   Takes us off the idle timer Q, if we're on it.
	**/
	{
	LOG_FUNC
	if(iIdleEntryQueued)
		{
		LOG(_L("RFCOMM: Protocol removing timer entry"));
		BTSocketTimer::Remove(iIdleTimerEntry);
		iIdleEntryQueued=EFalse;
		}
	}

void CRfcommProtocol::QueIdleTimerEntry()
	/**
	   Q a timer to delete us.
	**/
	{
	LOG_FUNC
	if(!iIdleEntryQueued)
		{
		LOG(_L("RFCOMM: Protocol adding timer entry"));
		BTSocketTimer::Queue(KRfcommIdleTimeout, iIdleTimerEntry);
		iIdleEntryQueued =  ETrue;
		}
	}

