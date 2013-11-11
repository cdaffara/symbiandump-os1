// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the AVCTP protocol object
// 
//

/**
 @file
 @internalTechnology
*/

#include <e32def.h>
#include <bluetooth/logger.h>
#include <bluetoothav.h>

#include "Avctp.h"
#include "avctpsap.h"
#include "avctpmuxerstates.h"
#include "avctpconstants.h" 
#include "IncomingConnListener.h"
#include "avctpPacketMgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

#ifdef _DEBUG
PANICCATEGORY("avctp");
#endif

using namespace SymbianAvctp;

#include "avctpmuxer.h"



#ifdef __FLOG_ACTIVE
#define LOG_SAPS LogSaps();
#define LOG_MUXERS LogMuxers();
#define LOG_SAPLINKSMGRS LogSapLinkMgrs();
#else
#define LOG_SAPS
#define LOG_MUXERS
#define LOG_SAPLINKSMGRS
#endif

/** 
Protocol object constructor.

@see CBTSecMan
@param aSecMan The Bluetooth security manager
@param aControlPlane the object which the protocol can use to pass control plane messages
@param aCodMan the CoD manager for the protocol
@return A pointer to the AVCTP protocol object
@internalComponent
*/
CAvctpProtocol::CAvctpProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
:	CBluetoothProtocolBase(aSecMan, aControlPlane, aCodMan),
	iSaps(_FOFF(CAvctpSap, iQueLink)),
	iTransports(_FOFF(CAvctpTransport, iQueLink)),
	iIpidResponses(_FOFF(HAvctpOutgoingSdu, iQueLink))
	{
	LOG_FUNC
	
	TCallBack callBack(TryToClose, this);
	iIdleTimerEntry.Set(callBack);
	}

/** Static protocol object factory function.

Leaves the protocol object on the cleanup stack.

@see CBTSecMan
@param aSecMan The Bluetooth security manager
@param aControlPlane the object which the protocol can use to pass control plane messages
@param aCodMan the CoD manager for the protocol
@return A pointer to the AVCTP protocol object
@leave KErrNoMemory if the protocol object could not be created
@internalComponent
*/
CAvctpProtocol* CAvctpProtocol::NewLC(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
	CONNECT_LOGGER
	LOG_STATIC_FUNC
	CAvctpProtocol* self = new(ELeave) CAvctpProtocol(aSecMan, aControlPlane, aCodMan);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/** Static protocol object factory function.

@see CBTSecMan
@param aSecMan The Bluetooth security manager
@param aControlPlane the object which the protocol can use to pass control plane messages
@param aCodMan the CoD manager for the protocol
@return A pointer to the AVCTP protocol object
@leave KErrNoMemory if the protocol object could not be created
@internalComponent
*/
CAvctpProtocol* CAvctpProtocol::NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
	LOG_STATIC_FUNC
	CAvctpProtocol* self = CAvctpProtocol::NewLC(aSecMan, aControlPlane, aCodMan);
	CleanupStack::Pop(self);
	return self;
	}

/** Protocol object second-phase construction.

@internalComponent
*/
void CAvctpProtocol::ConstructL()
	{
	LOG_FUNC
	iMuxerStateFactory = CAvctpMuxerStateFactory::NewL();

	TCallBack cb(IpidAsyncCallBack, this);
	iIpidResponseAsyncCallBack = new (ELeave)CAsyncCallBack(cb, EActiveHighPriority);
	
	}

/** Protocol object destructor.

We don't clear up the SAPs, or the Muxer Collections as they are owned by ESock
and it is up to it to destroy them.

 muxers are owned by the protocol and so should be destroyed by it.

@internalComponent
*/
CAvctpProtocol::~CAvctpProtocol()
	{
	LOG_FUNC
	
	DequeIdleTimer();

	if (LowerProtocol())
		{
		LowerProtocol()->Close();  // Matches the bind
		}

	CancelIpidAsyncCallBack();
	delete iIpidResponseAsyncCallBack;

	TDblQueIter<HAvctpOutgoingSdu> sduIter(iIpidResponses);
	while(sduIter)
		{
		delete sduIter++; // deques sdu
		}				
	TDblQueIter<CAvctpTransport> muxerIter(iTransports);
	while(muxerIter)
		{
		delete muxerIter++; // deques muxer
		}	

	delete iMuxerStateFactory;
	// iListener is deleted through CBluetoothProtocolBase
	delete iSecondChannelListener;
	iClientItems.Close();
	
	
	}

/** Pre-binding initialise.

Alloc any stuff we need. This will only ever be called once during the lifetime of this protocol.

@internalAll
@leave KErrNoMemory if the state factory cannot be created
@param aTag The strin   g identifier for the protocol from the ESock ini file
*/
void CAvctpProtocol::InitL(TDesC& /*aTag*/)
	{
	LOG_FUNC
	}

/** Binding complete.

@internalAll
*/
void CAvctpProtocol::StartL()
	{
	LOG_FUNC

	// Should check that we're bound now.
	if (!iLowerProtocol)
		{
		User::Leave(ENotBound);
		}
	User::LeaveIfError(IncrementListeners());
	}

/** Request by Protocol Mgr to bind to the specified protocol. 

We can only be bound to one lower layer protocol, so the function leaves if we are already bound.

@internalAll
@leave KErrProtocolAlreadyBound if we are already bound, and any other leave from the lower protocol's BindL()
@param aProtocol The protocol we need to bind to.
*/
void CAvctpProtocol::BindToL(CProtocolBase* aProtocol)
	{
	LOG_FUNC

	if(!iLowerProtocol)
		{
#ifdef _DEBUG
		TServerProtocolDesc prtDesc;
		aProtocol->Identify(&prtDesc);

		if(prtDesc.iAddrFamily != KBTAddrFamily ||
		   prtDesc.iProtocol != KL2CAP)
			{
			User::Leave(KErrBtEskError);
			}
#endif

		iLowerProtocol=static_cast<CBluetoothProtocolBase*>(aProtocol);
		iLowerProtocol->BindL(this, 0); // id not used
		iLowerProtocol->Open();
		}
	else
		{
	    User::Leave(KErrProtocolAlreadyBound);
		}
	}

/** Create a new SAP.

The SAP returned is owned by the caller -- this protocol will not clean it up.
ESock uses this function to create a new SAP, and ESock will delete when it is
finished with it.

@internalAll
@leave KErrNotSupported if aSockType is not KSockDatagram, KErrNoMemory if a
     new SAP could not be created
@param aSockType The socket type for the SAP: AVCTP only supports KSockDatagram
@return A pointer to a new SAP
*/
CServProviderBase* CAvctpProtocol::NewSAPL(TUint aSockType)
	{
	LOG_FUNC

	CBluetoothSAP* sap = NULL;
	
	switch(aSockType)
		{
	case KSockDatagram:    // AVCTP uses datagrams for data SAPs
		sap = CAvctpSap::NewL(*this);
		break;
	case KSockRaw:    // AVCTP uses Raw for control SAPs
		sap = CAvctpControlSAP::NewL(*this);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return sap;
	}
	

/** Identify the protocol.

The descriptor is filled in to identify the protocol to ESock.

@internalAll
@param aDesc A pointer to the descriptor to be filled in
*/
void CAvctpProtocol::Identify(TServerProtocolDesc* aDesc) const
	{
	LOG_FUNC

	ProtocolIdentity(aDesc);
	}

/** Fill in the protocol descriptor.

This is a static utility function to fill in the protocol details.

@internalComponent
@param aDesc A pointer to the descriptor to be filled in
*/
void CAvctpProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc)
	{
	LOG_STATIC_FUNC

	_LIT(KAvctpProtocolName,"AVCTP"); 
	aDesc->iName			= KAvctpProtocolName;
	aDesc->iAddrFamily		= KBTAddrFamily;
	aDesc->iSockType		= KUndefinedSockType; // can support datagram AND raw
	aDesc->iProtocol		= KAVCTP;
	aDesc->iVersion			= TVersion(KBTMajor,KBTMinor,KBTBuild);
	aDesc->iByteOrder		= ELittleEndian;
	aDesc->iServiceInfo		= KServiceInfo;
	aDesc->iNamingServices	= NULL;
	aDesc->iSecurity		= KSocketNoSecurity; // cause we do our own
	aDesc->iMessageSize		= KSocketMessageSizeNoLimit; // we can fragment to our heart's content
	aDesc->iServiceTypeInfo	= KServiceTypeInfo;
	aDesc->iNumSockets		= KMaxNumSocketsInProtocol;
	}

/** Our protocol reference in esock is now zero, so start to close.

We don't actually close, merely Q a timer for a later close down.
This close can be pre-empted by another open.

@internalAll
*/
void CAvctpProtocol::CloseNow()
	{
	LOG_FUNC

	iClosePending = ETrue;
	
	// We can only assert there are no saps cause the sap links mgrs are asynchronously deleted
	// and so may still be around at this point. However they should've been deleted by the
	// time TryToClose is called so we assert IsIdle() there.
	ASSERT_DEBUG(iSaps.IsEmpty());
	
	QueIdleTimer();
	}

/** Request to open the protocol.

The protocol may be repeatedly opened and closed.  The order of calls is
InitL, [Open *n , Close * n,  CloseNow] * m etc.

@internalAll
*/
void CAvctpProtocol::Open()
	{
	LOG_FUNC

	iClosePending = EFalse;
	DequeIdleTimer();
	CProtocolBase::Open();
	}

/**
   This is one session closing.
*/
void CAvctpProtocol::Close()

	{
	LOG_FUNC
	CProtocolBase::Close();
	}


/**
Helper to actually start listening. This protocol doesn't use the base class
implementation as it needs to do a bit more.
@internalComponent
*/
void CAvctpProtocol::DoStartAvctpListeningL()
	{	
	LOG_FUNC

	const TUint KDefaultMtu = 335;
	
	CServProviderBase* sap =iLowerProtocol->NewSAPL(KSockSeqPacket);
	CleanupStack::PushL(sap);
	
	TBTSockAddr localAddr;
	localAddr.SetPort(KAVCTP);

	// the security settings are:	
	// (though see :Preauthorise() for the authentication exceptions due to avdtp authentication)
	TBTServiceSecurity sec;
	sec.SetAuthentication(KInboundAuthenticationDefault);
	sec.SetAuthorisation(KInboundAuthoristationDefault);
	sec.SetEncryption(KInboundEncryptionDefault);
	sec.SetDenied(EFalse);
	sec.SetUid(KAvctpServiceUid);
	localAddr.SetSecurity(sec);
	
	CleanupStack::Pop(sap);
	iListener = CIncomingConnectionListener::NewL(*this, sap, localAddr, KProtocolListeningQueueSize);										
	
	CServProviderBase* sap2 =iLowerProtocol->NewSAPL(KSockSeqPacket);
	CleanupStack::PushL(sap2);
	
	TPckgBuf<TL2CapConfig> config;
	config().SetMaxTransmitUnitSize(KAvctpSecondaryChannelInboundMTU);
	config().SetMinMTU(KDefaultMtu);
	config().SetMaxReceiveUnitSize(KAvctpSecondaryChannelInboundMTU);
	config().SetMinMRU(KDefaultMtu);
	config().ConfigureReliableChannel(TL2CapConfig::EDefaultRetransmission);
	config().SetLegacyModesDisallowed(ETrue);
	
	__DEBUG_ONLY(TInt err =) sap2->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, config); 
	__ASSERT_DEBUG(err == KErrNone, Panic(ESetOptionError));
	
	// security for the secondary channel is not required
	sec.SetAuthentication(KSecondaryChannelAuthenticationDefault);
	sec.SetAuthorisation(KSecondaryChannelAuthoristationDefault);
	sec.SetEncryption(KInboundEncryptionDefault);
	sec.SetDenied(EFalse);
	sec.SetUid(KAvctpServiceUid);	
	localAddr.SetSecurity(sec);
	localAddr.SetPort(KAvctpSecondChannelPSM);
	
	CleanupStack::Pop(sap2);
	iSecondChannelListener  = CIncomingConnectionListener::NewL(*this, sap2, localAddr, KProtocolListeningQueueSize);										
	}

/**
AVCTP connection listener connect complete function (cloned SAP overload).

Part of the MSocketNotify interface.

Called by the listening L2CAP SAP when an incoming connection occurs.

This checks with the protocol to check for an existing muxer - if there
is one, an error is returned and the new L2CAP SAP is shut down (the 
remote device should be using the existing muxer). If not, the SAP is
given to the muxer.

@internalAll
@param aSap The SAP for the new lower layer connection
*/
TInt CAvctpProtocol::BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSSP)
	{
	LOG_FUNC
	
	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address; 
		aAddr.GetReadable(address);
		)
		
	LOG1(_L("from BT Device 0x%S"), &address);
	
	// NB: If no error occurs, ownership of aSSP passes to the muxer.
	// If an error does occur then the CIncomingConnectionListener will take care of aSSP for us
	TInt err = AttachInboundConnectionToTransport(aAddr, aSSP);
	
	return err;
	}



/**
Control plane message delivery system between protocols
@return error as a result of processing or not consuming the control message
@param aMessage the message
@param aParam arbitrary data for message - knowledge of aMessage allows casting
@see CBTProtocolFamily
@internalComponent
*/
TInt CAvctpProtocol::ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam)
	{
	LOG_FUNC

	// only ones applicable to this protocol at present are 
	// for preauthorising a device - must have come from AVCTP, with PID=RCP
	TInt ret = KErrNotSupported;
	
	switch (aMessage)
		{
		case EPreauthoriseDevice:
			{
			__ASSERT_DEBUG(aParam, Panic(EProtocolReceivingBadlyFormedControlMessage));
			const TOverrideAuthorise& override = *reinterpret_cast<const TOverrideAuthorise*>(aParam);
			__ASSERT_DEBUG(override.iAuthorisingProtocol == KAVDTP, Panic(EProtocolReceivingControlFromUnexpectedProtocol));
			
			SetPreauthorisation(override.iPreauthorisedRemoteAddress,
								override.iPreauthorise);
			ret = KErrNone;							
			break;
			}
		default:
			__ASSERT_DEBUG(aParam, Panic(EProtocolReceivingBadlyFormedControlMessage));
		}
	return ret;
	}

/**
Helper to hide the need to supply the socket level to which preauthorisation pertains
@param aPreauthoriseAddress the address of the device to preauthorise
@param aSetPreauthorisation ETrue if the device is allowed to be authorised for other AVDTP/AVCTP connection, EFalse to cancel
@internalComponent
*/
TInt CAvctpProtocol::SetPreauthorisation(const TBTDevAddr& aPreauthoriseAddress, TBool aSetPreauthorisation)
	{
	LOG_FUNC

	TInt ret = KErrNone;
	if (IsListening())
		{
		if (aSetPreauthorisation && !Listener().IsPreauthorised(KSolBtL2CAP, aPreauthoriseAddress))
			{
			Listener().SetPreauthorisation(KSolBtL2CAP, aPreauthoriseAddress, ETrue);

			// tell AVDTP
			TOverrideAuthorise override;
			override.iAuthorisingProtocol = KAVCTP;
			override.iAuthorisingPort = KAvrcpPid;			
			override.iPreauthorise = aSetPreauthorisation;
			override.iPreauthorisedRemoteAddress = aPreauthoriseAddress;
			
			ControlPlane().Preauthorise(KAVDTP, override);

			}
		else if (!aSetPreauthorisation)
			{
			Listener().SetPreauthorisation(KSolBtL2CAP, aPreauthoriseAddress, EFalse);			
			}
		// else do nothing
		}
	else
		{
		ret = KErrNotReady;
		}
	return ret;
	}



//
// Interface to all Saps provided for the muxers

/**
  Used by a muxer to tell the saps that it is now in a position to 
  send data
	
  @param aRemoteAddr the remote address which they an now send to.
  @internalComponent
*/
void CAvctpProtocol::SignalCanSendToSaps(CAvctpPacketMgr& IF_FLOGGING(aPacketMgr))
	{
	LOG_FUNC
	LOG1(_L("from Packetgr 0x%08x"), &aPacketMgr);
	
	// kick off the ipid sending process.
	StartSendIpidAsyncCallBack();
	
	TDblQueIter<CAvctpSap> sapIter(iSaps);
	CAvctpSap* sap;
	while(sapIter)
		{
		sap = sapIter++;
		sap->CanSend(); 
		}	
	}
	 
/**
  Find out whether there are any saps with data for the given remote address
  @param aRemoteAddr the remote device which might have data for it
  @return ETrue if there is such a sap, EFalse otherwise
  @internalComponent
**/	
TBool CAvctpProtocol::SapsHaveDataFor(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC
	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address;
		aRemoteAddr.GetReadable(address);
		)

	LOG1(_L("from BT Device 0x%S"), &address);

	TDblQueIter<CAvctpSap> iter(iSaps);
	CAvctpSap* sap;
	TBool ans = EFalse;
	
	while(iter)
		{
		sap = iter++;
		if(sap->HasDataFor(aRemoteAddr))
			{
			ans = ETrue;
			break;
			}
		}

	LOG1(_L("result %d"), ans);
	return ans;
	}
	
/**
Note the a TAvctpHeaderInfo could be provided as a parameter to this function
but since the HAvctpIncomingSdu data contains this info too, there's a danger
that the TAvctpHeaderInfo would not match the actual sdu. Hence the header
info class isn't provided. This isn't a big cost cause it's quick to pull 
the transaction label, PID etc out of the sdu itself.
*/
void CAvctpProtocol::SignalNewDataToSaps(HAvctpIncomingSdu* aSdu, TInt aChannel)
	{
	LOG_FUNC

	// Give data to each sap in turn to see if they want the PDU
	TDblQueIter<CAvctpSap> iter(iSaps);
	CAvctpSap* sap;
	TBool sapAcceptedData = EFalse;
	while(iter)
		{
		sap = iter++;
		sapAcceptedData = sap->NewData(aSdu, aChannel);
		if (sapAcceptedData)
			{
			break; // Once one accepts it, there's no point giving the data to the others
			}
		}					  	 

	if (!sapAcceptedData)
		{
		// This is not for a registered PID.  If this is a command we should
		// return an IPID response, if not we just dump it.  Crazy remote!
		TAvctpStartHeaderInfo headerInfo;
		TInt err = CAvctpPacket::ParseHeader(aSdu->Data(), headerInfo);
		
		if(!err && headerInfo.iMsgType == SymbianAvctp::ECommand)
			{
			HAvctpOutgoingSdu* ipidSdu = NULL;
			TRAPD(ret, ipidSdu = HAvctpOutgoingSdu::NewIpidResponseL(*aSdu, aChannel));
			if (ret == KErrNone)
				{
				iIpidResponses.AddLast(*ipidSdu);
				StartSendIpidAsyncCallBack();
				}
			}
		// else - we've OOM & we'll have to break the AVCTP spec & not send an IPID response :(
		// or it's a response that we can't re-respond to.

		// No one took the data so we need to clean it up
		delete aSdu;
		}
	}

/**
This function is called when a muxer goes down and is used to inform Saps
of this fact so they can get rid of any packets that are now stale.
*/
void CAvctpProtocol::SignalMuxerDownToSaps(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC

	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address;
		aRemoteAddr.GetReadable(address);
		)

	LOG1(_L("from BT Device 0x%S"), &address);
	
	TDblQueIter<CAvctpSap> iter(iSaps);
	CAvctpSap* sap;
	
	while(iter)
		{
		sap = iter++;
		sap->MuxerDown(aRemoteAddr);
		}					  	 
	}

//
// Que Management Functions	 

/** 
Adds a SAP to their queue.
Ensures that there will only ever be one sap on the Q for each PID

@internalComponent
@param aSap The SAP to be added
@return KErrInUse if there is already a Sap on aSap's PID, otherwise KErrNone
*/   
TInt CAvctpProtocol::AddSap(CAvctpSap& aSap)
	{
	LOG_FUNC
	LOG2(_L("Adding 0x%08x on Pid 0x%x"), &aSap, aSap.Pid());
	LOG_SAPS
	
	// The given sap must have a valid Pid to be on the protocol's Q
	__ASSERT_DEBUG(aSap.Pid() != 0, Panic(ENullPid));
	
	TInt ret = KErrNone;
	
	CAvctpSap* sap = FindSap(aSap.Pid());
	
	if (!sap && aSap.iChannel == KAvctpSecondaryChannel)
		{
		ret = KErrNotReady;
		}
	else if (sap && aSap.iChannel == KAvctpPrimaryChannel)
		{
		// Bit of a hack to support two SAPs doing one PID
		ret = KErrAlreadyExists;
		}
	else
		{
		// tell the sap that has been inserted in the list. Infact a sap that is not inserted in the
		// list is shutdown from esock. In that process CAvctpProtocol::RemoveSap() is called and 
		// would remove the "PID's client" from the iClientItems list. But it has to remove it only if that
		// sap has been inserted in the list.
		aSap.iIsInList = ETrue;
		iSaps.AddFirst(aSap);
		DequeIdleTimer();
		}
	
	return ret;
	}
	
/** 
Removes a SAP from their queue.

@internalComponent
@param aSap The SAP to be removed
*/   	
void CAvctpProtocol::RemoveSap(CAvctpSap& aSap)
	{
	LOG_FUNC
	LOG2(_L("Removing 0x%08x from Pid 0x%x"), &aSap, aSap.Pid());
	LOG_SAPS

	if (aSap.IsInList())
		{
		aSap.iQueLink.Deque(); // safe to Deque even if it's already been done
		
		TDblQueIter<CAvctpTransport> muxerIter(iTransports);
		CAvctpTransport* transport = NULL;
		
		
		while(muxerIter)
			{
			transport = muxerIter++;
			//Sap pid is used as clientId key to uniquely identify the client RAvctp
			if (transport->HasClient(aSap.Pid()))
				{
				if (aSap.Channel() == KAvctpPrimaryChannel)
					{
					transport->RemovePrimaryChannelRef(aSap.Pid());
					}
				else
					{
					transport->RemoveSecondaryChannelRef(aSap.Pid());
					}
				}
			}
	
		// as we are removing the sap it means the RAvctp for this Pid is being closing
		// so we must remove the TClientItem from the protocol. To avoid removing it twice
		// (it should be safe though) we only remove it for the primary channel sap
				
		if (aSap.Channel() == KAvctpPrimaryChannel)
			{
			__DEBUG_ONLY(TInt err = ) iClientItems.Remove(aSap.Pid());
			__ASSERT_DEBUG(err == KErrNone, Panic(EAvctpClientNotFound));
			}
		
		CheckForIdle();
		}
	}

/** Find the SAP that is on this PID.

If no such SAP exists, return NULL. Note this function doesn't transfer ownership.

There should be no Sap with a zero PID on the Q

@internalComponent
@param aClientId The client Id to look for
@return The SAP connected on the specified PID, if any, otherwise NULL. 
*/
CAvctpSap* CAvctpProtocol::FindSap(TUint16 aClientId)
	{
	LOG_FUNC

	TDblQueIter<CAvctpSap> iter(iSaps);
	CAvctpSap* sap = NULL;
	CAvctpSap* foundSap = NULL;

	while (iter)
		{
		sap = iter++;
		__ASSERT_DEBUG(sap->Pid() != 0, Panic(ENullPid));
		if(sap->Pid() == aClientId)
			{
			foundSap = sap;
			break;
			}
		}

	return foundSap;
	}
	
/** 
Adds a Muxer to their queue which has been assigned a remote address. 
Does the transfer from the blank muxer Q for the muxer.

@internalComponent
@param aMuxer The Muxer to be added
@return KErrInUse if there is already a Muxer on aMuxer's BTDevAddr, otherwise KErrNone
*/   
TInt CAvctpProtocol::AddTransport(CAvctpTransport& aTransport)
	{
	LOG_FUNC

	LOG_MUXERS
	
	// The given muxer must have a valid remote addr to be on the protocol's Q
	__ASSERT_DEBUG(aTransport.DevAddr() != TBTDevAddr(0), Panic(ENullTBTDevAddr));
	
	TInt ret = KErrNone;
	CAvctpTransport* transport = FindTransport(aTransport.DevAddr());
	if (transport)
		{
		ret = KErrInUse;
		}
	else
		{
		iTransports.AddFirst(aTransport);
		}
	
	return ret;
	}

/** 
Removes a Muxer from their queue.

@internalComponent
@param aMuxer The Muxer to be removed
*/   	 
void CAvctpProtocol::RemoveTransport(CAvctpTransport& aTransport)
	{
	LOG_FUNC
	LOG_MUXERS
	  
	aTransport.iQueLink.Deque(); // safe to Deque even if it's already been done
	}

/** Find the Muxer that is on this remote address.
If no such Muxer exists, return NULL. Note this function doesn't transfer ownership.

There should be no Muxer with a zero BTDevAddr on the Q
@internalComponent
@param aDevAddr The remote address to look for
@return The Muxer connected on the specified remote address, if any, otherwise NULL.
*/
CAvctpTransport* CAvctpProtocol::FindTransport(const TBTDevAddr& aDevAddr)
	{
	LOG_FUNC

	TDblQueIter<CAvctpTransport> iter(iTransports); 
	CAvctpTransport* transport = NULL;
	CAvctpTransport* foundTransport = NULL;

	while (iter)
		{
		transport = iter++;

		if(transport->DevAddr() == aDevAddr)
			{
			foundTransport = transport;
			break;
			}
		}

	return foundTransport;
	}

CAvctpTransport* CAvctpProtocol::FindOrCreateTransport(const TBTDevAddr& aDevAddr, TInt aChannel, CServProviderBase* aL2CAPConSAP)
	{
	LOG_FUNC
	TInt err = KErrNone;
	CAvctpTransport* transport = FindTransport(aDevAddr);
	
	if (!transport)
		{
		// we create the transport but, in this case, we don't have PIDs attached and 
		// we cannot notify them
		TRAP(err, transport = CAvctpTransport::NewL(*this));
		if (err == KErrNone)
			{
			iTransports.AddLast(*transport);
			}
		
		if (err == KErrNone)
			{
			if (aL2CAPConSAP)
				{
				err = transport->StartIncoming(aDevAddr, aL2CAPConSAP);
				}
			else
				{
				err = transport->Start(aDevAddr, KUndefinedPid);
				}
			
			if (err == KErrNone)
				{
				// It's possible that the muxer had to synchronously delete itself
				// and wasn't able to return a synchronous error.
				// In this case the muxer will have removed itself from the protocol's Q
				if (!FindTransport(aDevAddr))
					{
					transport = NULL; // since it's already dead
					}
				}
			}
		}
	else if (aChannel == KAvctpSecondaryChannel)	// secondary channel
		{
		if (!transport->HasSecondChannel())
			{
			CServProviderBase* sap = NULL;
			TRAP(err, sap = iLowerProtocol->NewSAPL(KSockSeqPacket));
			if (err == KErrNone)
				{
				err = transport->AddSecondChannel(*sap);
				if (err!=KErrNone)
					{
					delete sap;
					sap = NULL;
					}
				}
			}
		}
	return transport;
	}

void CAvctpProtocol::AwaitTransport(TUint16 aClientId, MSocketNotify& aControlSocket)
	{
	LOG_FUNC
	AddClientItem(aClientId, aControlSocket);
	}

/**
 It is submitted by the client saying it is happy to accept an incoming indication (for the primary channel).
 When the client receive an attach indication it can decide to accept or refuse it. 
 If accepts it sends back an agree attachment ioctl that results in this call.
 @param aClientId is the RAvctp pid value
 @param aRemoteAddr is the remote device's address which has been connected
 */
TInt CAvctpProtocol::PrimaryChannelAgreeAttachment(TUint16 aClientId, const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC
	TInt err = KErrMuxerNotFound;
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	if (transport)
		{
		TClientItem* item = iClientItems.Find(aClientId);
		__ASSERT_DEBUG(item, Panic(EAvctpClientNotFound));
		err = transport->AddPrimaryChannelRef(item); // it returns KErrNone or KErrNoMemory
		}
	return err;
	}

/**
 It is submitted by the client saying it is happy to accept an incoming indication (for the secondary channel).
 When the client receive an attach indication it can decide to accept or refuse it. 
 If accepts it sends back an agree attachment ioctl that results in this call.
 @param aClientId is the RAvctp pid value
 @param aRemoteAddr is the remote device's address which has been connected
 */
TInt CAvctpProtocol::SecondaryChannelAgreeAttachment(TUint16 aClientId, const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC
	TInt err = KErrMuxerNotFound;
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	if (transport)
		{
		TClientItem* item = iClientItems.Find(aClientId);
		__ASSERT_DEBUG(item, Panic(EAvctpClientNotFound));
		transport->AddSecondaryChannelRef();
		err = KErrNone;
		}
	return err;
	}

/**
 If not already present it adds a TClientItem to the hashtable. Despite it was already present or not
 it always calls AttachPrimaryChannel to update the MSocketNotify.
 That is because once the TClientItem is added to the hastable it remains alive until the protocol 
 is alive. So we need to update the socket observer because it can be changed. i.e an RAvctp is created
 and used, then it is destroyed, and created again. The "clientId" remains the same, but the 
 MSocketNofity passed through it is probably changed. The item will be found (already added to the
 protocol hashtable) but the MSocketNotify& must be updated.
 @param aClientId is the unique identifier of the RAvctp client
 @param aNotify the socket observer for the ioctl messages (the primary channel CAvctpControlSAp)
 */
const TClientItem* CAvctpProtocol::AddClientItem(TUint16 aClientId, MSocketNotify& aNotify) // it doesn't pass the ownership
	{
	LOG_FUNC
	TClientItem* pitem = iClientItems.Find(aClientId);
	if (!pitem)
		{
		// pitem is null so, proceed creating it and inserting in the hashtable
		TClientItem newItem(aClientId);
		newItem.AttachPrimaryChannel(aNotify);
		if (iClientItems.Insert(aClientId, newItem) == KErrNone)
			{
			// insertion succesfully, so we assign the inserted item to pitem
			pitem = iClientItems.Find(aClientId);
			}
		// if an error occurred (out of memory) pitem is still null
		}
	
	if (pitem)	// pitem can be null if the insertion failed 
		{
		// we update aNotify anyway, so it is always updated to the correct reference
		pitem->AttachPrimaryChannel(aNotify);
		}
	
	return const_cast<const TClientItem*>(pitem);	// it doesn't pass the ownership
	}

const TClientItem* CAvctpProtocol::ClientItem(TUint16 aClientId) const // it doesn't pass ownership
	{
	LOG_FUNC
	const TClientItem* pitem = iClientItems.Find(aClientId);
	return pitem;
	}

void CAvctpProtocol::NotifyLinkUp(const TBTDevAddr& aAddr, TBool aIsSecondChannel)
	{
	LOG_FUNC
	TControlIoctlMessage msg(ELinkUp, aAddr);
	TPckgC<TControlIoctlMessage> pck(msg);
	
	THashMapIter<TInt, TClientItem> iter(iClientItems);
	while(iter.NextKey())
		{
		TClientItem* pitem = iter.CurrentValue();
		MSocketNotify* socket = aIsSecondChannel ? pitem->SecondaryChannel() : pitem->PrimaryChannel();

		__DEBUG_ONLY
			(
			if (!aIsSecondChannel)
				{
				__ASSERT_DEBUG(socket, Panic(EAvctpInvalidChannelNotify));
				}
			)

		if (socket)
			{
			socket->IoctlComplete(&pck);
			}
		}
	}

/** Gets a muxer for the given BT device address, using the L2CAP connection provided.
@internalComponent
*/	
TInt CAvctpProtocol::StartProtocolListening()
	{
	LOG_FUNC

	// because we do non-default security we override the base class
	// and new the incoming listener ourselves
	TRAPD(err, DoStartAvctpListeningL());
	return err;
	}
	
/** Gets a muxer for the given BT device address.
	
If it finds the muxer or successfully creates a new muxer, it returns 
with KErrNone and a valid pointer to the correct muxer. On a failure, 
it will return with a standard error, without a pointer to  a muxer.
This flavour of GetMuxer is used when a muxer is being created in response 
to a local request to connect to a remote device (i.e. no existing 
L2CAP connection exists) or when someone just wants a muxer on the given 
address whether or not one exists.

Note that because a synchronous error could occur during the Start call, 
aMuxer might be NULL at the end of this function. Note thought this
depends on whether aMuxer was able to asynchronously delete itself or not.
Note that there could then be two error paths, one via the muxer sending
a ConnectComplete (with error) & the return from this function. If this is
the case, KErrSynchronousMuxerError will be returned which should be voided
so that the client doesn't get two errors.

@internalComponent
@param aRemoteAddr The remote address for which we want a muxer
@param aMuxer A muxer will be created by this function if one doesn't exist and a connection attempt start. 
			  If there is an existing muxer it is returned aMuxer will be not be owned by the caller
@param aLinksMgr a possible control client for the newly created muxer
@return KErrNone if the muxer is successfully returned, 
		KErrSynchronousMuxerError if the muxer failed immediately on the address given, or
		KErrNoMemory or
		any other error code from CAvctpTransport::NewL
*/ 
TInt CAvctpProtocol::ProvideTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId,
									  MSocketNotify& aControlSocket)
	{
	LOG_FUNC
	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address;
		aRemoteAddr.GetReadable(address);
		)

	LOG1(_L("from BT Device 0x%S"), &address);
	
	TInt err = KErrNone;
	
	// first of all I add the Client Id to my list and get a pointer to it.
	const TClientItem* item = AddClientItem(aClientId, aControlSocket);
	
	if (item)	// we were able to allocate the memory for it
		{
		CAvctpTransport* transport = FindTransport(aRemoteAddr);
	
		if (!transport)
			{
			TRAP(err,transport = CAvctpTransport::NewL(*this));
			if (err == KErrNone)
				{
				err = transport->AddPrimaryChannelRef(item);
				if (err == KErrNone)
					{
					iTransports.AddLast(*transport);
					}
				else
					{
					// it is safe to call it on a non-attached clientId and it is the only
					// way we have to ask the transport to destroying itself
					// as we just created the transport we know calling this method will destroy it
					transport->RemovePrimaryChannelRef(aClientId);
					}
				}
			}
		else
			{
			err = transport->AddPrimaryChannelRef(item);
			// in this case we already had the transport, probably with some clients attached.
			// we can't destroy it, and it is working for the attached clients. So, we simply
			// return an error to the client is asking to be attached.
			}
		
		if (err == KErrNone)
			{
			err = transport->Start(aRemoteAddr, aClientId);
	
			if (err == KErrNone)
				{
				// It's possible that the muxer had to synchronously delete itself
				// and wasn't able to return a synchronous error (e.g. if the client
				// tries to connect to itself and the async deletion attempt OOMs)
				// In this case the muxer will have removed itself from the protocol's Q
				if (!FindTransport(aRemoteAddr))
					{
					transport = NULL; // since it's already dead
					err = KErrSynchronousMuxerError;
					}
				}
			}
		}
	else
		{
		err = KErrNoMemory;
		}
	
	return err;
	}

TInt CAvctpProtocol::ActiveExtendTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket)
	{
	LOG_FUNC
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	TInt err = KErrMuxerNotFound;
	if (transport)
		{
		if (!transport->HasSecondChannel())
			{
			// get a new SAP and ask transport to connect it up
			CServProviderBase* sap = NULL;
			TRAP(err, sap = iLowerProtocol->NewSAPL(KSockSeqPacket));
			err = transport->AddSecondChannel(*sap);
			if (err!=KErrNone)
				{
				delete sap;
				sap = NULL;
				}
			else
				{
				transport->SetSecondChannelCtrlNotify(aClientId, aSecondChannelControlSocket);
				transport->AddSecondaryChannelRef();
				}
			}
		else
			{
			transport->SetSecondChannelCtrlNotify(aClientId, aSecondChannelControlSocket);
			transport->AddSecondaryChannelRef();
			transport->NotifyAttachConfirm(aClientId, KErrNone, ETrue);
			}
		}
	return err;
	}

void CAvctpProtocol::SetSecondChannelCtrlNotify(TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket)
	{
	LOG_FUNC
	TClientItem* item = iClientItems.Find(aClientId);
	__ASSERT_DEBUG(item, Panic(EAvctpClientNotFound));
	item->AttachSecondaryChannel(aSecondChannelControlSocket);
	}

void CAvctpProtocol::AwaitForExtendedTransport(TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket)
	{
	LOG_FUNC
	SetSecondChannelCtrlNotify(aClientId, aSecondChannelControlSocket);
	}

void CAvctpProtocol::PrimaryChannelRefuseAttach(const TBTDevAddr& aRemoteAddr, TUint16 aClientId)
	{
	LOG_FUNC
	
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	
	// when this method is called it means the client refused to be attached to the transport
	// and didn't call the AgreeAttachment. It means that we don't have the TClientItem attached to
	// the transport.
	// However we call transport->RemovePrimaryChannelRef anyway. It is safe to call it even if the
	// aClientId item is not attached, and it does the reference count control so that if there
	// are no more clients attached it destroys the transport.
	// this method can also be called on the PrimaryChannelCancelAttach invocation client side.
	// In this case the client will be found and removed
	if(transport)
		{
		// NOTE: if transport has no more clients attached it deletes itself.
		// so, don't rely on transport after this call because it could have been deleted
		transport->RemovePrimaryChannelRef(aClientId);
		}
	}

void CAvctpProtocol::SecondaryChannelRefuseAttach(const TBTDevAddr& aRemoteAddr, TUint16 aClientId)
	{
	LOG_FUNC

	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	
	// when this method is called it means the client refused to be attached to the transport
	// and didn't call the AgreeAttachment. It means that we don't have the TClientItem attached to
	// the transport.
	// However we call transport->RemovePrimaryChannelRef anyway. It is safe to call it even if the
	// aClientId item is not attached, and it does the reference count control so that if there
	// are no more clients attached it destroys the transport.
	
	if(transport)
		{
		// we must call AddSecondaryChannelRef() first, that increments the reference count
		// because then we call RemoveSecondaryChannelRef() that decrements it.
		// we want to call RemoveSecondaryChannelRef() because it manages the zero reference which
		// drives the secondary l2cap channel destruction 
		transport->AddSecondaryChannelRef();
		transport->RemoveSecondaryChannelRef(aClientId);
		}
	}

TInt CAvctpProtocol::GetChannelMtu(TInt aChannel, const TBTDevAddr& aAddr, TInt& aMtu)
	{
	LOG_FUNC
	TInt err = KErrNotReady;
	CAvctpTransport* t = FindTransport(aAddr);
	if(t)
		{
		err = t->GetChannelMtu(aChannel, aMtu);
		}
	return err;
	}

TInt CAvctpProtocol::ReleaseTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId)
	{
	LOG_FUNC
	TInt err = KErrMuxerNotFound;
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	if(transport)
		{
		err = KErrNone;
		transport->NotifyDetachConfirm(aClientId, err);
		// NOTE: if there are no more pid attached to the transport it will delete itself.
		// so, don't rely on the transport after this call because it could have been deleted
		transport->RemovePrimaryChannelRef(aClientId);
		}
	return err;
	}

TInt CAvctpProtocol::ReleaseExtendedTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId)
	{
	LOG_FUNC
	TInt err = KErrMuxerNotFound;
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	if(transport)
		{
		err = KErrNone;
		transport->NotifyDetachConfirm(aClientId, err, ETrue);
		transport->RemoveSecondaryChannelRef(aClientId);
		}
	return err;
	}

void CAvctpProtocol::PrimaryChannelIncomingRemoteDisconnection(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC
	CAvctpTransport* transport = FindTransport(aRemoteAddr);
	if(transport)
		{
		// the secondary channel, if present, has been already destroyed.
		transport->NotifyLinkDown(aRemoteAddr, KAvctpPrimaryChannel);	// we are destroying the transport 
		}
	}

/**
Gets a muxer for the given BT device address, using the L2CAP connection provided.

If it is found that a muxer for the specified BT device already exists, 
KErrAlreadyExists will be returned. If a new muxer is successfully created, 
KErrNone will be returned along with a valid pointer to the correct muxer in aMuxer 

This flavour of GetMuxer is used when a muxer is being created in response to a 
remote request to connect to the local device (i.e. an L2CAP SAP has been created 
due to an incoming connection).

Note that because a synchronous error could occur during the Start call, 
aMuxer might be NULL at the end of this function. Note thought this
depends on whether aMuxer was able to asynchronously delete itself or not.
Note that there could then be two error paths, one via the muxer sending
a ConnectComplete (with error) & the return from this function. If this is
the case, KErrSynchronousMuxerError will be returned which should be voided
so that the client doesn't get two errors.

@internalComponent
@param aRemoteAddr The remote address for which we want a muxer
@param aL2CAPConSAP This is a non null, L2CAP sap for the incoming connection from aRemoteAddr
@param aMuxer A muxer created by this function to hold aL2CAPConSAP (assuming there wasn't one already for aRemoteAddr). aMuxer will be not be owned by the caller
@return KErrNone if the muxer is successfully returned, 
		KErrSynchronousMuxerError if the muxer failed immediately on the address give, or
		any other error code from CAvctpTransport::NewL
*/
TInt CAvctpProtocol::AttachInboundConnectionToTransport(const TBTDevAddr& aRemoteAddr,
														CServProviderBase* aL2CAPConSAP)
	{
	LOG_FUNC
	__DEBUG_ONLY
		(
		TBuf<KBTAddressLength> address;
		aRemoteAddr.GetReadable(address);
		LOG1(_L("from BT Device 0x%S"), &address);
		
		//	Check that the device address and the device of the L2CAP remote address match!!
		TBTSockAddr addr;
		aL2CAPConSAP->RemName(addr);
		__ASSERT_DEBUG(addr.BTAddr()==aRemoteAddr,Panic(EMismatchedAddressAndSap));
		)

	TL2CAPPort localPSM;
	TPckg<TL2CAPPort> localPSMBuf(localPSM);
	TInt err = aL2CAPConSAP->GetOption(KSolBtL2CAP, KL2CAPLocalPSM, localPSMBuf);

	__ASSERT_DEBUG(err == KErrNone, Panic(EGetOptionError));
	
	//first try to find the transport with a given address: if it exists then this inbound link could be the second channel
	CAvctpTransport* transport = FindTransport(aRemoteAddr);

	if (transport)
		{
		// ok so try finding a listener transport for this link (which should be control)
		if (localPSM == KAvctpSecondChannelPSM)
			{
			transport->AddSecondChannel(*aL2CAPConSAP);
			}
		else
			{
			// remote was being daft
			return KErrGeneral;
			}
		}
	else
		{
		
		if (localPSM != KAVCTP)
			{
			return KErrGeneral; // the other type of daft remote
			}
				
		// else remote sensibly connected first channel to the right PSM, for which we should have a listening transport on bdaddr==0
		transport = FindOrCreateTransport(aRemoteAddr, KAvctpPrimaryChannel, aL2CAPConSAP); 
		if (transport)
			{
			// It's possible that the muxer had to synchronously delete itself
			// and wasn't able to return a synchronous error (e.g. if the client
			// tries to connect to itself and the async deletion attempt OOMs)
			// In this case the muxer will have removed itself from the protocol's Q
			if (!FindTransport(aRemoteAddr))
				{
				transport = NULL; // since it's already dead
				err = KErrSynchronousMuxerError;
				}
			else
				{	// notify link up
				NotifyLinkUp(aRemoteAddr, EFalse);
				}
			}
		else
			{
			err = KErrNotReady;
			}
		}

	return err;
	}

/** 
Provide interface to class to send AVCTP SDUs (if one for the needed address exists
*/
MAvctpSDUSender* CAvctpProtocol::GetSDUSender(const TBTDevAddr& aRemoteAddr, TInt aChannel)
	{
	LOG_FUNC

	CAvctpTransport* transport = FindOrCreateTransport(aRemoteAddr, aChannel);
	return transport ? &static_cast<MAvctpSDUSender&>(transport->PacketMgr()) : NULL;
	}

/**
Check to see if we're still needed.  If not, Q a delayed delete.

  @internalComponent
*/
void CAvctpProtocol::CheckForIdle()
	{
	LOG_FUNC

	if (IsIdle())
		{
		QueIdleTimer();
		}
	}

/** 
Called to check whether we can close down. I.e. are there any clients remaining on the
other side of Esock.

We could be idle even if there are Avctp muxers alive since Esock won't know about these

@internalComponent
@return ETrue if the protocol can close, EFalse if not
*/
TBool CAvctpProtocol::IsIdle()
	{
	LOG_FUNC
	LOG_SAPS
	
	return (iClosePending && 
			iSaps.IsEmpty());
	}
	
/**
Queues the idle timer if necessary

  @internalComponent
*/
void CAvctpProtocol::QueIdleTimer()
	{
	LOG_FUNC

	if (!iIdleTimerQueued)
		{
		LOG(_L("Queued idle timer"));

		iIdleTimerQueued = ETrue;
		BTSocketTimer::Queue(KProtocolIdleTimeout, iIdleTimerEntry);
		}
	}

/**
Deques idle timer if necessary

  @internalComponent
*/
void CAvctpProtocol::DequeIdleTimer()
	{
	LOG_FUNC

	if (iIdleTimerQueued)
		{
		LOG(_L("Dequeued idle timer"));

		iIdleTimerQueued = EFalse;
		BTSocketTimer::Remove(iIdleTimerEntry);
		}
	}

	
/** Asynchronous callback function.

We check if should close then close.

@internalComponent
@param aProtocol The protocol object
@return EFalse to indicate the callback does not need to be reissued
*/
TInt CAvctpProtocol::TryToClose(TAny* aProtocol)
	{
	LOG_STATIC_FUNC

	CAvctpProtocol* protocol	= static_cast<CAvctpProtocol*>(aProtocol);
	protocol->iIdleTimerQueued	= EFalse;
	__ASSERT_DEBUG(protocol->IsIdle(), Panic(EIdleTimeoutWhenNotIdle));
	protocol->CanClose();

	return EFalse; // don't try to callback again
	}

TInt CAvctpProtocol::IpidAsyncCallBack(TAny* aProtocol)
	{
	LOG_STATIC_FUNC

	__ASSERT_DEBUG(aProtocol, Panic(ENullAvctpProtocol));
	
	CAvctpProtocol& protocol = *static_cast<CAvctpProtocol*>(aProtocol);

	TDblQueIter<HAvctpOutgoingSdu> iter(protocol.iIpidResponses);
	HAvctpOutgoingSdu* sdu = NULL;
	MAvctpSDUSender* sender = NULL;
	
	TInt err;
	
	while(iter)
		{
		sdu = iter++;
		sender = protocol.GetSDUSender(sdu->BTAddr(), sdu->Channel());

		if (sender)
			{
			LOG1(_L("Attempt to write IPID response 0x%08x in IpidAsyncCallBack"), &sdu);
			if ((err = sender->WriteIpid(sdu)) != KErrNone)
				{
				if (err == KErrRemoteSentTooManyIpidSdus)
					{
					CAvctpTransport* transport = protocol.FindTransport(sdu->BTAddr());
					if (transport)
						{
						transport->Shutdown(KErrRemoteSentTooManyIpidSdus);
						}
					}
				else if (err == KErrMuxerBlocked)
					{
					//got blocked
					//it will be called again asyncronously
					}
				}
			// else send completed and sdu ownership transferred to the sender
			}
		else
			{
			// can't send the IPID since we don't have a data path to send it over
			delete sdu; // will deque the sdu
			}
		}	
	return EFalse;	
	}
	
#ifdef __FLOG_ACTIVE	
void CAvctpProtocol::LogSaps()
	{
	LOG_FUNC
	TDblQueIter<CAvctpSap> iter(iSaps);
	CAvctpSap* sap = NULL;
	while (iter)
		{
		sap = iter++;
		LOG2(_L("0x%08x is a sap on PID %d"), sap, sap->Pid());
		}	
	}
	
void CAvctpProtocol::LogMuxers()
	{
	LOG_FUNC
	TDblQueIter<CAvctpTransport> iter(iTransports);
	CAvctpTransport* transport = NULL;
	TBuf<KBTAddressLength> address;
	while (iter)
		{
		transport= iter++;
		transport->DevAddr().GetReadable(address);
		LOG2(_L("0x%08x is a transport on BT Device %S"), transport, &address);
		}	
	}

#endif

// TClientItem

TClientItem::TClientItem(TUint16 aClientId) : 
	iClientId(aClientId),
	iPrimaryChannel(NULL),
	iSecondaryChannel(NULL),
	iIsSecondaryChannelAttached(EFalse),
	iRefCount(0)
	{
	LOG_FUNC
	}

void TClientItem::AttachPrimaryChannel(MSocketNotify& aNotify)
	{
	LOG_FUNC
	iPrimaryChannel = &aNotify;
	iIsSecondaryChannelAttached = EFalse;
	}

void TClientItem::AttachSecondaryChannel(MSocketNotify& aNotify)
	{
	LOG_FUNC
	iSecondaryChannel = &aNotify;
	iIsSecondaryChannelAttached = ETrue;
	}

TUint16 TClientItem::ClientId() const
	{
	LOG_FUNC
	return iClientId;
	}

MSocketNotify* TClientItem::PrimaryChannel() const
	{
	LOG_FUNC
	return iPrimaryChannel;
	}

MSocketNotify* TClientItem::SecondaryChannel() const
	{
	LOG_FUNC
	return iSecondaryChannel;
	}

TBool TClientItem::IsSecondaryChannelAttached() const
	{
	LOG_FUNC
	return iIsSecondaryChannelAttached;
	}

void TClientItem::DetachSecondaryChannel()
	{
	LOG_FUNC
	iSecondaryChannel = NULL;
	iIsSecondaryChannelAttached = EFalse;
	}
