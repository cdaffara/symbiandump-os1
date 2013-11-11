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
// Rfcomm muxer
// 
//

#include <bluetooth/logger.h>

#include <bt_sock.h>

#include "rfcommmuxer.h"
#include "rfcomm.h"
#include "rfcommutil.h"
#include "rfcommframe.h"
#include "rfcommtypes.h"
#include "rfcommfcs.h"
#include "rfcommsap.h"

#include "rfcommflow.h"
#include "rfcommmuxchannel.h"
#include "AsyncErrorKicker.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

// Disable int to char warnings
#ifdef __WINS__
#pragma warning( disable : 4244 )
#endif

#ifdef __FLOG_ACTIVE
_LIT(KCommandText, "command");
_LIT(KResponseText, "response");
#endif //__FLOG_ACTIVE

const TUint8 KBitsForNumberInTInt = 32 - 1; // -1 is because we don't count the sign bit

CRfcommMuxer* CRfcommMuxer::NewL(CRfcommProtocol& aProt, CProtocolBase& aL2CAP,
								CMuxChannelStateFactory& aFactory)
	/**
	Factory function for CRfcommMuxer - called when local device initiates the connection.

	Note the CProtocolBase passed in so that the muxer can create its own L2CAP SAP
	**/
	{
	CRfcommMuxer* mux= new (ELeave) CRfcommMuxer(aProt,KInitiationDirectionOutgoing);
	CleanupStack::PushL(mux);
	mux->ConstructL(aFactory, aL2CAP);
	CleanupStack::Pop();
	return mux;
	}

CRfcommMuxer* CRfcommMuxer::NewL(CRfcommProtocol& aProt, CServProviderBase* aSAP,
								CMuxChannelStateFactory& aFactory)
	/**
	Factory function for CRfcommMuxer - called when remote device initiates the connection.

	Note the CServProviderBase passed in, which becomes the L2CAP SAP used by this muxer.

	@param aSAP		 This is a connected L2CAP sap
	@param aFactory  The Mux channel state factory
	@return          A new Muxer in the right state
	**/
	{
	CRfcommMuxer* mux= new (ELeave) CRfcommMuxer(aProt,KInitiationDirectionIncoming);
	CleanupStack::PushL(mux);
	mux->ConstructL(aFactory, aSAP);
	CleanupStack::Pop();
	return mux;
	}


CRfcommMuxer::CRfcommMuxer(CRfcommProtocol& aProt, TInitiationDirection aDirection)
	: iSAPs(_FOFF(CRfcommSAP, iLink)),
	  iOutboundQ(_FOFF(CRfcommFrame, iLink)),
	  iResponseQ(_FOFF(CRfcommFrame, iLink)),
	  iOutboundQLength(0),
	  iProtocol(aProt),
	  iDirection(aDirection),
	  iMuxIdleTimeout(KRfcommMuxIdleTimeoutOpening),	  
	  iCanSend(ETrue),
	  iBlockedSAPs(_FOFF(CRfcommSAP, iLink)),
	  iDataFramesSent(0),
	  iL2CAPSendBlocked(EFalse),
	  iTriedCBFC(EFalse),
	  iCurrentCredit(0),
	  iInitialTxCredit(0),
	  iFlowStrategy(0)
	{
	TCallBack cb(IdleTimerExpired, this);
	iIdleTimerEntry.Set(cb);
	}

void CRfcommMuxer::ConstructL(CMuxChannelStateFactory& aFactory, CProtocolBase& aL2CAP)
	{ 
	// Create a SAP for this to use
	iBoundSAP=aL2CAP.NewSAPL(KSockSeqPacket);
	iMuxChannel = new (ELeave) CRfcommMuxChannel(aFactory, *this, *iBoundSAP, CMuxChannelStateFactory::EClosed);

	//	We now have a bound L2Cap SAP.
	//  Use it to setup L2Cap config for RFComm.
	TL2CapConfigPkg configPkg;
	iProtocol.SetL2CapConfig(configPkg);
	User::LeaveIfError(iBoundSAP->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configPkg));

	CommonConstructL();
	}

void CRfcommMuxer::ConstructL(CMuxChannelStateFactory& aFactory, CServProviderBase* aSAP)
	{ 
	// SAP has been provided for this to use, so start it
	__ASSERT_DEBUG(aSAP!=NULL, Panic(ERfcommNullSAPForMuxer));
	iBoundSAP=aSAP;
	iBoundSAP->Start();
	TBTSockAddr btAddr;
	aSAP->RemName(btAddr);
	iRemoteAddr=btAddr.BTAddr();
	iMuxChannel = new (ELeave) CRfcommMuxChannel(aFactory, *this, *iBoundSAP, CMuxChannelStateFactory::ELinkUp);
	CommonConstructL();
	}

void CRfcommMuxer::CommonConstructL()
	{
	iBoundSAP->SetNotify(this);
	TCallBack cb(TryToSendCallbackStatic,this);
	iSendCallback=new (ELeave) CAsyncCallBack(cb, ECAsyncImmediatePriority);
	iNextPacket=HBufC8::NewL(KRfcommDefaultMTU);  // This will do for now
#ifdef NO_CBFC
	iFlowStrategy = TRfcommFlowStrategyNonCreditBased::NewL(*this);
#else
	if(iProtocol.CBFCDisallowed())
		{
		SetFlowType(CRfcommFlowStrategyFactory::EFlowNonCreditBased);
		iTriedCBFC = ETrue;
		}
	else
		{
		SetFlowType(CRfcommFlowStrategyFactory::EFlowInitial);
		}
#endif
	}

CRfcommMuxer::~CRfcommMuxer()
	{
	LOG(_L("CRfcommMuxer::~CRfcommMuxer"));

	DequeIdleTimer();
	DeleteQueuedFrames();
	
	// remove the references to this muxer in any SAPs it is/was linked to
	TDblQueIter<CRfcommSAP> iter(iSAPs);
	CRfcommSAP* sap;
	while(iter)
		{
		sap=iter++;
		if (sap)
			{
			sap->iMux = NULL;
			sap->iLink.Deque();
			}
		}

	
	
	delete iSendCallback;
	delete iBoundSAP;
	delete iNextPacket;
	delete iMuxChannel;
	}

/***************************************************************************/
/*
  Commands from the protocol.
  
*/
  

void CRfcommMuxer::Bind(TBTDevAddr& aAddr)
	/**
	   Bind this mux to the given remote address
	   
	   Calling this function implies that this end is to be the
	   initiator (direction bit = 1).  The bound sap will be used to
	   create an L2CAP link to the remote end over which the RFCOMM
	   frames will flow.  The muxchannel will then run this channel.
	**/
	{
	iRemoteAddr=aAddr;
	iMuxChannel->SetAddress(aAddr);
	}

void CRfcommMuxer::AddSAP(CRfcommSAP& aSAP)
	/**
	   Adds a sap to the Q for this mux.
	**/
	{
	DequeIdleTimer();
	iSAPs.AddFirst(aSAP);
	if(iMuxChannel->IsOpen())
		{
		aSAP.MuxUp();
		}
	else if (iMuxChannel->IsErrored())
		{
		aSAP.Error(KErrRfcommMuxChannelErrored, CRfcommSAP::EErrorFatal);
		}
	else
		{
		iMuxChannel->Open();  // Eventually calls back
		}
	}

void CRfcommMuxer::DetachSAP(CRfcommSAP& aSAP)
	/**
	   Detach this sap.

	   If it's the last one, shut down.  We clean out the outbound and
	   response queues as we no longer want these frames since the sap
	   has gone.
	**/
	{
	aSAP.iLink.Deque();
	aSAP.iMux=0;
	
	ClearOutboundQueue(aSAP);
	ClearResponseQueue(aSAP);
	CheckForIdle(ETrue);
	}

void CRfcommMuxer::GetInboundServerChannelsInUse(TFixedArray<TBool, KMaxRfcommServerChannel>& aChannelInUse)
	/**
	   Identify inbound server channels in use for connected SAPs.
	   
	   aChannelInUse is an array indexed by (server channel - 1) and on return 
	   aChannelInUse will be updated with any inbound server channels that
	   are currently in use by connected SAPs.
	**/
	{
	TDblQueIter<CRfcommSAP> iter(iSAPs);
	CRfcommSAP* sap;

	// Only interested in cloned SAPs as this identifies if the SAP is for 
	// an inbound connection
	while(iter)
		{
		sap=iter++;		
		if (sap->IsCloned())
			{
			aChannelInUse[sap->ServerChannel() - 1] = ETrue;
			}
		}

	iter=iBlockedSAPs;
	while(iter)
		{
		sap=iter++;
		if (sap->IsCloned())
			{
			aChannelInUse[sap->ServerChannel() - 1] = ETrue;
			}
		}	
	}

/****************************************************************************/
/*
  Notifications from the MSocketNotify interface
	
*/

void CRfcommMuxer::NewData(TUint aCount)
	/**
	   Called when new data is available.
	   This is called each time a new packet of data arrives from L2CAP.
	   We get to the data by calling GetData on the SAP.

	   Frames never span a L2CAP packet, and are one per packet.

	   This assumes a packet interface from L2CAP

	   @param aCount  Number of new packets waiting
	**/
	{
 	if (aCount == KNewDataEndofData)
 		{
		LOG(_L("RFCOMM: L2CAP signalled EndOfData"));
 		Disconnect();
 		}
	else
		{
		LOG1(_L("RFCOMM: New data, count %d"), aCount);
		//BLOG: KBlogNewData
		iPacketsWaiting+=aCount;
		while(CanProcessNewData() && iPacketsWaiting)
			{
			// Get a packet into the buffer
			TPtr8 data=iNextPacket->Des();
			data.SetMax();
			iBoundSAP->GetData(data, 0);
			// process the new data
			ProcessFrame();
			iPacketsWaiting--;
			}
		}
	}


void CRfcommMuxer::CanSend()
	/**
	   Notification that we can now send data to the lower layer.
	**/
	{
	L2CAPBlocked(EFalse);
	TryToSend();
	}

//CBFC
CRfcommFlowStrategyFactory::TFlowStrategies CRfcommMuxer::FlowType()
	{
	return iFlowStrategy->FlowType();
	}

#ifdef NO_CBFC
TBool CRfcommMuxer::FlowType(CRfcommFlowStrategyFactory::TFlowStrategies /*aFlow*/)
	{
	return EFalse;
	}
#else
TBool CRfcommMuxer::SetFlowType(CRfcommFlowStrategyFactory::TFlowStrategies aFlow)
	{
	if(iTriedCBFC)
		{
		LOG(_L("RFCOMM: Requesting new flow strategy again: DISALLOWED"));
		return EFalse;
		}

	switch(aFlow)
		{
	case CRfcommFlowStrategyFactory::EFlowCreditBased:
		LOG(_L("RFCOMM: Requesting CBFC at SAP level"));
		iTriedCBFC = ETrue;				// stop the test being done again
		iFlowStrategy = &(iProtocol.FlowStrategyFactory()->GetFlowStrategy(aFlow));
		break;
	case CRfcommFlowStrategyFactory::EFlowNonCreditBased:
		LOG(_L("RFCOMM: Not requesting CBFC at SAP level"));
		iTriedCBFC = ETrue;				// stop the test being done again
		iFlowStrategy = &(iProtocol.FlowStrategyFactory()->GetFlowStrategy(aFlow));
		break;
	default:
		iFlowStrategy = &(iProtocol.FlowStrategyFactory()->GetFlowStrategy(CRfcommFlowStrategyFactory::EFlowInitial));
		break;
		}
	return ETrue;
	}
#endif

void CRfcommMuxer::DisallowCBFC()
	{
	iCBFCDisallowed = ETrue;
	}

void CRfcommMuxer::AllowCBFC()
	{
	iCBFCDisallowed = EFalse;
	}

TRfcommFlowStrategy* CRfcommMuxer::FlowStrategy()
	{
	return iFlowStrategy;
	}



void CRfcommMuxer::ConnectComplete(const TDesC8& /*aConnectData*/)
	/**
	   Version with connection data.

	   Ignore the data (since L2CAP should never provide this!)
	**/
	{
	ConnectComplete();
	}

void CRfcommMuxer::ConnectComplete(CServProviderBase& /*aSSP*/)
	/**
	   Incoming connection completed on listen socket.
	**/
	{
	Panic(ERfcommIncomingNotSupported);
	}

void CRfcommMuxer::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	Panic(ERfcommIncomingNotSupported);
	}

void CRfcommMuxer::CanClose(TDelete /*aDelete*/)
	/**
	   We must have asked the socket to shutdown, which means we're dying.
	**/
	{
	LOG1(_L("RFCOMM: CanClose from L2CAP for mux %08x"), this);
	iMuxChannel->CanClose();
	}

void CRfcommMuxer::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	Panic(ERfcommDisconnectDataNotSupported);
	}

void CRfcommMuxer::Error(TInt aError,TUint aOperationMask)
	/**
	   Something's gone wrong.

	   We get told whether it's just this operation or all that are
	   affected, but we'll just assume that all of them are bust for
	   now.
	**/
	{
	LOG1(_L("RFCOMM: Error on L2CAP sap %d"), aError);
	if(aOperationMask & MSocketNotify::EErrorIoctl)
		{
		// This was an ioctl problem, so pass up to the saps
		PropagateIoctlCompletion(aError, iIoctlLevel, iIoctlName, NULL);
		iIoctlName=0;
		iIoctlLevel=0;
		}

	// Pass this on to the Mux channel as well
	iMuxChannel->Error(aError, aOperationMask);
	}

void CRfcommMuxer::MuxChannelError(TBool aFatal, TInt aError)
	/**
	   Something's gone wrong on the mux channel

	   If this is a fatal error it means we need a new muxchannel,
	   else we can live with it.  Either way we need to error all the
	   existing saps.
	**/
	{
	LOG1(_L("RFCOMM: Error on Mux %d"), aError)
    
	TDblQueIter<CRfcommSAP> iter(iSAPs);
	CRfcommSAP* sap;

	// Erroring the saps will remove them from us when we signal a 
	// general error
	while(iter)
		{
		sap=iter++;
		sap->Error(aError, CRfcommSAP::EErrorGeneral);
		}
	iter=iBlockedSAPs;
	while(iter)
		{
		sap=iter++;
		sap->Error(aError, CRfcommSAP::EErrorGeneral);
		}

	if(aFatal)
		{
		// The channel is now useless, and so is the mux
		// Set the address to 0 to stop any more saps attaching
		DeleteQueuedFrames();
		iRemoteAddr=TInt64(0);
		return;
		}
	CheckForIdle(ETrue);
	}

const TBTDevAddr& CRfcommMuxer::RemoteBTAddr() const
	{
	return iRemoteAddr;
	}

void CRfcommMuxer::MuxChannelDown()
	/**
	   The mux channel has gone down.
	   Pass this on to the saps	
	**/
	{
	CloseSAPs();
	DeleteQueuedFrames(); // since we can't send or receive any more
	CheckForIdle(ETrue);
	}

void CRfcommMuxer::CloseSAPs()
	{
	TDblQueIter<CRfcommSAP> iter(iSAPs);
	CRfcommSAP* sap;

	while(iter)
		{
		sap=iter++;
		sap->LinkDown();
		}
	iter=iBlockedSAPs;
	while(iter)
		{
		sap=iter++;
		sap->LinkDown();
		}
	}

void CRfcommMuxer::MuxChannelUp()
	/**
	   The mux channel has come up.

	   We need to allocate the buffer for incoming packets.  Note this
	   may not be the same as the RFCOMM MTU since we can't guarantee
	   that the other end won't send us omore in an L2CAP packet.
	**/
	{

	TPckgBuf<TInt> buf;
	// Find out what the max data size is.
	iBoundSAP->GetOption(KSolBtL2CAP, KL2CAPInboundMTU, buf);
	TInt incoming = buf();

	TRAPD(err, iNextPacket=iNextPacket->ReAllocL(incoming));
	if(err != KErrNone)
		{
		// We can't do anything here, so close & error
		iMuxChannel->Close();
		MuxChannelError(ETrue, err);
		}
	else
		{
		TDblQueIter<CRfcommSAP> iter(iSAPs);
		CRfcommSAP* sap;
		
		while(iter)
			{
			sap=iter++;
			sap->MuxUp();
			}
		iter=iBlockedSAPs;
		while(iter)
			{
			sap=iter++;
			sap->MuxUp();
			}
		}
	}
 
void CRfcommMuxer::Disconnect()
	/**
	   The L2CAP link has disconnected.

	   
	**/
	{
	iMuxChannel->Disconnect();
	}

void CRfcommMuxer::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	Disconnect();
	}

void CRfcommMuxer::IoctlComplete(TDesC8* aBuf)
	/**
	   An ioctl has completed.

	   Broadcast it to all the saps that might be interested
	**/
	{
	PropagateIoctlCompletion(KErrNone, iIoctlLevel, iIoctlName, aBuf);
	iIoctlName=0;
	iIoctlLevel=0;
	}

void CRfcommMuxer::PropagateIoctlCompletion(TInt aError, TUint aIoctlLevel, TUint aIoctlName, TDesC8* aBuf)
	{
	TDblQueIter<CRfcommSAP> iter(iSAPs);
	CRfcommSAP* sap;
	while(iter)
		{
		sap=iter++;
		sap->IoctlComplete(aError, aIoctlLevel, aIoctlName, aBuf);
		}
	iter=iBlockedSAPs;
	while(iter)
		{
		sap=iter++;
		sap->IoctlComplete(aError, aIoctlLevel, aIoctlName, aBuf);
		}
	}

void CRfcommMuxer::ConnectComplete()
	/**
	   Signal from bound sap that connection has occurred.
	   
	   Part of the MSocketNotify interface. This is called when L2CAP
	   has brought up the lower layer link to the remote device.
	   Pass this through to the muxchannel.
	**/
	{
	FTRACE(TSockAddr addr;
		   iBoundSAP->RemName(addr);
		   LOG1(_L("RFCOMM: Connect complete rcid %d"),
		   	  addr.Port());
		   iBoundSAP->LocalName(addr);
		   LOG1(_L("RFCOMM: Connect complete lcid %d"),
		   	  addr.Port());
		);
	iMuxChannel->ConnectComplete();
	}

/**********************************************************************/
/*
  Commands from the SAP.
*/

TUint8 CRfcommMuxer::MakeDLCI(TUint8 aServerChannel, TUint8 aDirectionBit)
	{
	TUint8 addr = aServerChannel;
	addr <<= 1;
	addr |= aDirectionBit;
	return addr;
	}

TUint8 CRfcommMuxer::MakeOutboundDLCI(TUint8 aServerChannel)
	/**
	 Given a server channel, add the direction bit for an outbound connection
	 **/
	{
	// Use the inverse of our direction bit
	return MakeDLCI(aServerChannel, ~iDirection & KDirectionMask);
	}
	
TUint8 CRfcommMuxer::MakeInboundDLCI(TUint8 aServerChannel)
	/**
	 Given a server channel, add the direction bit for an inbound connection
	 **/
	{
	return MakeDLCI(aServerChannel, iDirection);
	}
	
TUint8 CRfcommMuxer::MakeServerChannel(TUint8 aDLCI)
	/**
	Given a DLCI, turn that back into a server channel
	**/
	{
	return aDLCI>>1;	//	Lose the bottom (direction) bit.
	}

TInt CRfcommMuxer::GetMaxDataSize() const
	/**
	   Returns the current max data size allowed
	**/
	{
	return iMuxChannel->MaxDataSize();
	}


void CRfcommMuxer::Donate(CRfcommSAP& aSAP, TUint8 aCredit)
	/**
	   Send an empty UIH data frame, cos the other end has been caught short
	**/
	{
	//Try to find another "Donate" frame in the outbound Q and attach to that...
	TDblQueIter<CRfcommFrame> iter(iOutboundQ);
	CRfcommFrame* frmInQ;
	
	while(iter)
		{
		frmInQ=iter;
 		if(frmInQ->Ctrl()==KUIHCBFCCtrlField && !(frmInQ->DataLength()))
 		    {
 		    if (frmInQ->SAP() && 
 		       (frmInQ->SAP()->RemoteAddress() == aSAP.RemoteAddress() && 
 		       (frmInQ->SAP()->DLCI()) == aSAP.DLCI()))
 		        {
                LOG1(_L("RFCOMM: Old credit in donate frame: %d"), frmInQ->Credit());
                TUint8 revisedDonation = aCredit+frmInQ->Credit();
                frmInQ->SetCredit(revisedDonation);
                LOG1(_L("RFCOMM: New credit in donate frame: %d"), frmInQ->Credit());
                break;
 			}
		}
		iter++;
		}

	if(!iter)
		//If no other "Donate" frame in the outbound Q.....
		{
		CRfcommDataFrame* frm=0;
		
		frm=NewDataFrame(aSAP.DLCI(), 0, aCredit, &aSAP);
		if(!frm)
			{
			LOG1(_L("RFCOMM: OOM when writing for SAP %08x, queueing SAP Error callback"), &aSAP);
			aSAP.iErrorKicker->SetError(KErrNoMemory, CRfcommSAP::EErrorOperation);
			aSAP.iErrorKicker->Call();
			}
		if(!frm)
			{
			SetSendBlocked(aSAP, ETrue);
			return;
			}
		
		EnqueFrame(frm);
		}

	FlowStrategy()->ReviseDonatedCredits(aSAP, aCredit); //Rx credit
	}

TInt CRfcommMuxer::Write(CRfcommSAP& aSAP, TUint8 aCredit, const TDesC8& aData)
	/**
	   Write some data.

	   We attempt to create a datapacket on the Q for this packet.  If
	   this fails or the data Q is too long then we return less than
	   the length of the data.  In which case we mark the SAP as
	   needing a CanSend, and provide one at a suitable later date.
	**/
	{
	__ASSERT_DEBUG(aData.Length() <= iMuxChannel->MaxDataSize(), Panic(ERfcommDataTooLong));
	CRfcommDataFrame* frm=0;

	if(iOutboundQLength < KMaxOutboundQLength)
		{
		frm=NewDataFrame(aSAP.DLCI(), aData.Length(), aCredit, &aSAP);

		if(!frm)
			{
			LOG1(_L("RFCOMM: OOM when writing for SAP %08x, queueing SAP Error callback"), &aSAP);
			aSAP.iErrorKicker->SetError(KErrNoMemory, CRfcommSAP::EErrorOperation);
			aSAP.iErrorKicker->Call();
			}
		}
	

	if(!frm)
		{
		SetSendBlocked(aSAP, ETrue);
		return 0;
		}
	if(!aData.Length())
		{
		LOG(_L("RFCOMM: Write has sent ZERO length data."));
		}

	frm->PutData(aData);
	LOG1(_L("RFCOMM: Writing 0x%x"),/*frm->Data()*/aData[0]);
	//BLOG: (KBlogWriteData, aData[0])
	
	FlowStrategy()->ReviseTransmittedCredits(aSAP); //TxCredit
	FlowStrategy()->ReviseDonatedCredits(aSAP, aCredit); //Rx credit
	EnqueFrame(frm);
	return aData.Length();
	}

TInt CRfcommMuxer::Ioctl(TUint aLevel, TUint aName, TDes8* aOption)
	/**
	   A request to do an ioctl onto our lower level sap.
	   
	   If one's already in progress, simply return KErrInUse
	**/
	{
	if(iIoctlLevel)
		{
		return KErrInUse;
		}
	else
		{
		iIoctlLevel=aLevel;
		iIoctlName=aName;
		iBoundSAP->Ioctl(aLevel, aName, aOption);
		}
	return KErrNone;
	}

void CRfcommMuxer::CancelIoctl(TUint aLevel, TUint aName)
	/**
	One of the RFCOMM SAPs has been asked to cancel an Ioctl which
	was being run at the L2CAP level.
	**/
	{
	if(aLevel==iIoctlLevel && iIoctlName==aName)
		{
		iBoundSAP->CancelIoctl(aLevel, aName);
		iIoctlLevel=0;
		iIoctlName=0;
		}
	}

TInt CRfcommMuxer::SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption)
	// Handle SetOption passed down from SAP
	{
	return iBoundSAP->SetOption(aLevel, aName, aOption);
	}

TInt CRfcommMuxer::GetOption(TUint aLevel, TUint aName, TDes8 &aOption)
	// Handle SetOption passed down from SAP
	{
	return iBoundSAP->GetOption(aLevel, aName, aOption);
	}
		
void CRfcommMuxer::SetCanHandleData(CRfcommSAP& aSAP, TBool aCanReceive)
	/**
	   Inform the muxer whether this sap can handle more data arriving.

	   The mux uses this information to see if it is safe to read more
	   data up from L2CAP.
	   
	   @param aCanReceive False if this sap can't handle any more
	**/
	{
	LOG2(_L("RFCOMM: CanHandleData sap %08x, State %d"),
				  &aSAP, aCanReceive);
		
	//FC
	// Update the bitmask that controls whether we will take new data from L2CAP
	SetNoFreeSpace(aSAP.DLCI(), !aCanReceive);
	
	if(CanProcessNewData() && iPacketsWaiting)
		{
		NewData(0);  // Tickle ourselves
		}
	}

TInt CRfcommMuxer::SendSABM(CRfcommSAP& aSAP)
	/**
	   Send a SABM for this SAP
	**/
	{
	LOG2(_L("RFCOMM: SendSABM for SAP %08x, Addr %d"), &aSAP, aSAP.DLCI());
	return(TransmitSABM(aSAP.DLCI(), &aSAP));
	}

TInt CRfcommMuxer::SendMSC(CRfcommSAP& aSAP, TUint8 aFlags)
	/**
	   Send a MSC command for this SAP.
	**/
	{
	//FC
	LOG1(_L("RFCOMM: Sending MSC command for sap %08x"), &aSAP);
	return TransmitMSC(aSAP.DLCI(), ETrue, aFlags, &aSAP);
	}

TInt CRfcommMuxer::SendMSCRsp(CRfcommSAP& aSAP, TUint8 aFlags)
	/**
	   Send a MSC response for this SAP.
	**/
	{
	LOG1(_L("RFCOMM: Sending MSC response for sap %08x"), &aSAP);
	return TransmitMSC(aSAP.DLCI(), EFalse, aFlags, &aSAP);
	}

TInt CRfcommMuxer::SendRLS(CRfcommSAP& aSAP, TUint8 aStatus)
	/**
	   Send a Remote Line Status command for this SAP.
	**/
	{
	LOG1(_L("RFCOMM: Sending RLS cmd for sap %08x"), &aSAP);
	return TransmitRLS(ETrue, aSAP.DLCI(), aStatus, &aSAP);
	}

TInt CRfcommMuxer::SendRLSRsp(CRfcommSAP& aSAP, TUint8 aStatus)
	/**
	   Send a Remote Line Status response for this SAP.
	**/
	{
	LOG1(_L("RFCOMM: Sending RLS resp for sap %08x"), &aSAP);
	return TransmitRLS(EFalse, aSAP.DLCI(), aStatus, &aSAP);
	}

TInt CRfcommMuxer::SendRPN(CRfcommSAP& aSAP, TBool aCommand, TUint8 aLength, 
						   const TRfcommRPNTransaction& aRPNTransaction)
	/**
	   Send a RPN frame to L2CAP for this SAP.
	**/
	{
	return TransmitRPN(aSAP.DLCI(), aCommand, aLength, aRPNTransaction, &aSAP);
	}

TInt CRfcommMuxer::SendUA(CRfcommSAP& aSAP)
	/**
	   Send a UA frame on behalf of the SAP
	**/
	{
	return TransmitUA(aSAP.DLCI(), &aSAP);
	}

TInt CRfcommMuxer::SendUA(TUint8 aDLCI)
	/**
	   Send a UA frame on for the DLC
	**/
	{
	return TransmitUA(aDLCI);
	}

TInt CRfcommMuxer::SendPN(CRfcommSAP& aSAP, const TRfcommPortParams& aParams)
	/**
	   Send a PN frame on behalf of the SAP
	   This function takes the proposed maximum frame size parameter from the SAP itself.
	**/
	{
	return TransmitPN(aSAP.DLCI(), ETrue, aParams, &aSAP);
	}

TInt CRfcommMuxer::SendPNResponse(CRfcommSAP& aSAP, const TRfcommPortParams& aParams)
	/**
		Send a PN Response frame on behalf of the SAP
	**/
	{
	return TransmitPN(aSAP.DLCI(), EFalse, aParams, &aSAP);
	}

void CRfcommMuxer::SendDISC(CRfcommSAP& aSAP)
	/**
	   Disconnect this sap and associated DLC.
	**/
	{
	TransmitDISC(aSAP.DLCI(), &aSAP);
	}

void CRfcommMuxer::SendDISC(TUint8 aDLCI)
	/**
	   Disconnect the associated DLC.
	**/
	{
	TransmitDISC(aDLCI);
	}

void CRfcommMuxer::SendDM(TUint8 aDLCI)
	{
	TransmitDM(aDLCI,ETrue);
	}

TInt CRfcommMuxer::SendFCon()
	/**
	Sends a FCon command to the peer RFCOMM entity.

	Used only for internal testing purposes.
	**/
	{
	//FC
	return TransmitFCon(ETrue,NULL);
	}

TInt CRfcommMuxer::SendFCoff()
	/**
	Sends a FCoff command to the peer RFCOMM entity.

	Used only for internal testing purposes.
	**/
	{
	//FC
	return TransmitFCoff(ETrue,NULL);
	}

/*
  Timeout from the frames
*/

void CRfcommMuxer::FrameResponseTimeout(CRfcommFrame* aFrm)
	/*
	  Called when a frame that needs a response times out

	*/
	{
	//	I could code the next bit more efficiently, but don't like obfuscation!
	TBool done=EFalse;
	if(aFrm->SAP()!=NULL)
		done=aFrm->SAP()->HandleFrameResponseTimeout();

	if(!done)
		iMuxChannel->FrameTimeout(aFrm);
	//	The frame will be cleaned up either by the mux error handling or by the SAP 
	//	going into the closed or error state.
	}

void CRfcommMuxer::ClearOutboundQueue(CRfcommSAP& aSAP)
	/**
	   Clear any frames on the outbound queue that point to this SAP.

	   This prevents any frame timeout getting back to a deleted SAP.
	   If a SAP wants to send a frame to the remote end after its
	   death, it must make sure that the iSAP member of the frame does
	   not point to it (ie is null).
	**/
	{
	TDblQueIter<CRfcommFrame> iter(iOutboundQ);
	CRfcommFrame* frm;
	
	while(iter)
		{
		frm=iter++;
		if(frm->SAP() == &aSAP)
			{
			--iOutboundQLength;
			delete frm;
			}
		}
	}

void CRfcommMuxer::ClearResponseQueue(CRfcommSAP& aSAP)	
	/**
	Clear the response queue of all frames associated with the SAP.

	The association is based on the frame pointing to the sap, or
	being for a DLCI which matches that of the SAP.  Frames on DLCI 0
	are not matched by the DLCI matching.
	
	This is generally called in response to a SAP being closed or
	entering an error state.  Clearly since the sap is no longer
	around, it makes no sense to keep the frames that are awaiting a
	response for timeout purposes.
	**/
	{
	TDblQueIter<CRfcommFrame> iter(iResponseQ);
	CRfcommFrame* frm=NULL;
	TUint8 frmDLCI=KMuxDLCI;

	TUint8 dlci=aSAP.iDLCI;
	if(dlci == KMuxDLCI)
		dlci=KMaxRfcommDLCI+1;  // This is an impossible DLCI that will never match
	while(iter)
		{
		frm=iter++;
		
		//	determine the DLCI the frame is associated with
		if(frm->Type() == KMuxCtrlFrameType)
			frmDLCI=static_cast<CRfcommMuxCtrlFrame*>(frm)->DLCI();
		else
			frmDLCI=frm->Address();

		if(frmDLCI==dlci || frm->SAP() == &aSAP)
			{
			LOG2(_L("RFCOMM: Clearing response for DLCI %d, sap 0x%08x"), frmDLCI, &aSAP);
			delete frm;
			break;
			}
		}
	}

/**********************************************************************/
/*
  Internal functions.
*/

void CRfcommMuxer::SetNoFreeSpace(TInt aDLCI, TBool aVal)
	/**
		Sets or resets this DLCI as having no space

		@param aVal   True if no space
	**/
	{
	__ASSERT_DEBUG(aDLCI <= KMaxRfcommDLCI, Panic(ERfcommInvalidDLCI));
	if(aVal)
		iSAPNoFreeSpaceMask[aDLCI >> 5] |= 1 << (aDLCI % 32);
	else
		iSAPNoFreeSpaceMask[aDLCI >> 5] &= ~(1 << (aDLCI % 32));
	}

TBool CRfcommMuxer::CanProcessNewData() const
	{
	return iFlowStrategy->CanProcessNewData(!iSAPNoFreeSpaceMask[0] && !iSAPNoFreeSpaceMask[1]);
	}

void CRfcommMuxer::SetSendBlocked(CRfcommSAP& aSAP, TBool aIsBlocked)
	/**
	   Mark the state of this SAP as able/unable to send

	   This is done by moving the SAP from the active to the blocked
	   Q.  To ensure all saps get a chance, we wake saps from the
	   front of the blocked Q, but add them to the end, thus
	   round-robining the chance to send.
	**/
	{
	
	aSAP.iLink.Deque();
	if(aIsBlocked)
		/*AddFirst - this helps avoid an infinite loop.
		For example SignalSAPsCanSend() unblocks a SAP 
		then can cause it to be reblocked when it calls 
		sap->CanSend().
		*/
		iBlockedSAPs.AddFirst(aSAP); 
	else
		iSAPs.AddLast(aSAP);
	}

void CRfcommMuxer::ProcessFrame()
	/**
	   Process the frame that is at the start data buffer.

	   When this is called, a frame is in the data buffer.  The
	   contents should be verified and processed appropriately.
	**/
	{
	if(iNextPacket->Length() < KMinFrameLength)
		{
		LOG(_L("RFCOMM:  ** ERROR ** received frame too short"));
		return;
		}
	LOG(_L("1st five bytes of incoming frame...."));
	LOGHEXRAW(&(*iNextPacket)[0], 5);
	
	TUint8  addr=DecodeDLCI((*iNextPacket)[KFrameAddrOffset]);
	TUint8  ctrl=(*iNextPacket)[KFrameCtrlOffset];
	// Is this really necessary ?
	TBool   poll = ((ctrl & KPollFinalBitmask) != 0)?ETrue:EFalse;
	//TBool   poll = ctrl & KPollFinalBitmask;
	ctrl = static_cast<TUint8>(ctrl & ~KPollFinalBitmask); // zero the poll/final bit

	//LOG1(_L("RFCOMM: Frame for dlci %d"), addr);
	if(addr!=0 && !iMuxChannel->IsOpen())
		{
		LOG1(_L("RFCOMM: Error: Frame for dlci %d before mux channel up - junking"), addr);
		return;
		}
	DecodeLengthAndCredit((poll && ctrl == KUIHCtrlField && addr != KMuxDLCI));  // get the length and the credit if correct packet

	// Frame length is header+data+fcs (fcs is one byte)
	if(iCurrentDataLength+iCurrentHeaderLength+1 != iNextPacket->Length())
		{
		if(iCurrentDataLength+iCurrentHeaderLength+1 < iNextPacket->Length())
			{
			LOG(_L("RFCOMM: Frame shorter than L2CAP packet, junking"));
			}
		else
			{
			LOG(_L("RFCOMM: Frame longer than L2CAP packet, junking"));
			}
		return;
		}
	
	TUint8  fcs=(*iNextPacket)[iCurrentDataLength+iCurrentHeaderLength];
		
	if(CheckFCS(fcs,ctrl))
		{
	switch (ctrl)
			{
		case KSABMCtrlField:
			if(poll)
				{
				LOG(_L("Rx: SABM"));
				HandleSABM(addr);
				}
			else
				{
				// Must ignore SABM with poll bit 0, TS07.10 5.4.4.1
				LOG(_L("RFCOMM: SABM with P/F = 0, ignoring"));
				}
			break;
			
		case KUACtrlField:
			LOG(_L("Rx: UA"));
			HandleUA(addr);
			break;
			
		case KDMCtrlField:
			LOG(_L("Rx: DM"));
			HandleDM(addr);
			break;
			
		case KDISCCtrlField:
			if(poll)
				{
				LOG(_L("Rx: DISC"));
				HandleDISC(addr);
				}
			else
				{
				// Must ignore DISC with poll bit 0, TS07.10 5.4.4.1
				LOG(_L("RFCOMM: DISC with P/F = 0, ignoring"));
				}
			break;
			
		case KUIHCtrlField:
			if(addr == KMuxDLCI)
				{
				LOG(_L("Rx: UIH Ctrl Frame"));
				TRAPD(err, ParseCtrlMessageL());
				if(err !=KErrNone)
					{
					LOG(_L("RFCOMM: Error in parsing the ctrl frames"));
					}
				}
			else
				{
				LOG(_L("Rx: Simple UIH"));
				ProcessDataFrame(addr, poll);
				}
			break;
			
		default:
			{
			LOG(_L("Error: RFCOMM: Unexpected frame ctrl field"));
			}
			}
		}
	else
		{
		LOG(_L("RFCOMM: Frame failed checksum"));
		}
	}

TUint8 CRfcommMuxer::DecodeDLCI(TUint8 aAddr)
	/**
	   Strip off the EA and C/R bits and return an TS07.10 DLCI.

	   Note that this is not the same as the RFCOMM server channel,
	   which is 5 bits + 1 direction bit.
	**/
	{
	return TUint8(aAddr >> 2);
	}

void CRfcommMuxer::DecodeLengthAndCredit(TBool aCBFC)
	/**
	   Decode the length of a frame.

	   Return the overall length of the header.  This can be one or
	   two bytes in length.
	**/
	{
	if((*iNextPacket)[2] & 1)
		{
		// Single byte length
		iCurrentDataLength=(*iNextPacket)[2] >> 1;
		iCurrentHeaderLength= KShortFrameHeaderLength;
		}
	else 
		{
		iCurrentDataLength=((*iNextPacket)[2] >> 1) +
			((*iNextPacket)[3] << 7);
		iCurrentHeaderLength= KLongFrameHeaderLength;
		}

	//if doing CBFC updates credit buffer and revises header length buffer
	//(iCurrentHeaderLength and iCurrentCredit)
	iFlowStrategy->DecodeLength(aCBFC, *iNextPacket, 
						iCurrentCredit, iCurrentHeaderLength);
	}

void CRfcommMuxer::HandleSABM(TUint8 aDLCI)
	/**
		Handles an incoming SABM command.
		This function basically dispatches the SABM to either the
		mux channel or an appropriate SAP.
	**/
	{
	if(iCurrentDataLength != 0)
		{
		LOG1(_L("RFCOMM: SABM frame with length %d"), iCurrentDataLength);
		}
// want this for UPF test verification - need the whole of a SABM in log file	
	LOG1(_L("RFCOMM: SABM command received, DLCI %d"), aDLCI);
	LOGHEXRAW(&((iNextPacket->Des())[0]), (iCurrentHeaderLength+iCurrentDataLength));
	
	if(aDLCI == KMuxDLCI)
		//	A SABM intended to initialise the Mux channel
		iMuxChannel->SABM();
	else
		{
		//	A SABM intended for a SAP
		CRfcommSAP* mySAP=FindConnectedOrListeningSAP(aDLCI);
		if(mySAP)
			mySAP->SABM(*this, aDLCI);
		else
			{
			//	There is nothing out there to handle this SABM.
			//	Send a DM back as response.
			SendDM(aDLCI);
			}
		}
	}

void CRfcommMuxer::HandleDISC(TUint8 aAddr)
	/**
	   Handles an incoming DISConnect request.

	   If this is for the mux channel then we should shutdown totally.
	   Otherwise we signal the appropriate SAP and then send a UA
	   frame to acknowledge.
	**/
	{
	LOG1(_L("RFCOMM: DISC frame for DLCI %d"), aAddr);
	if(iCurrentDataLength !=0)
		{
		LOG(_L("RFCOMM: DISC frame length !=0"));
		}

	CRfcommSAP* sap;
	
	if(aAddr != KMuxDLCI)
		{
		sap=FindSAP(aAddr);
		if(!sap)
			{
			LOG(_L("RFCOMM: DISC for non-existant sap"));
			TransmitDM(aAddr, ETrue);
			}
		else
			{
			sap->DISC();
			}
		}
	else
		{
		iMuxChannel->DISC();
		}
	}

void CRfcommMuxer::HandleDM(TUint8 aAddr)
	/**
	   Handles an incoming DM frame.

	   We must remove the equivalent SABM or DISC from the response Q
	**/
	{
	LOG1(_L("RFCOMM: DM Frame for DLCI %d"), aAddr);
	if(aAddr != KMuxDLCI)
		{
		CRfcommSAP* sap=FindSAP(aAddr);
		if(!sap)
			{
			LOG(_L("RFCOMM: DM for unanticipated DLCI!"));
			}
		else
			{
			sap->DM();
			}
		}
	else
		{
		iMuxChannel->DM();
		}
	if(!CtrlFrameResponse(aAddr))
		MuxCtrlResponseDM(aAddr);
	}

void CRfcommMuxer::HandleUA(TUint8 aAddr)
	/**
	   Handles an incoming UA frame.

	   This represents an acknowledgement of a command on this channel.
	**/
	{
	LOG1(_L("RFCOMM: UA Frame for addr %d"), aAddr);
	if(aAddr != KMuxDLCI)
		{
		CRfcommSAP* sap=FindSAP(aAddr);
		if(!sap)
			{
			LOG(_L("RFCOMM: UA for invalid DLCI, ignoring"));
			}
		else
			{
			sap->UA();
			}
		}
	else
		{
		iMuxChannel->UA();
		}
	//	Attempt to find the matching control frame - ignore errors.
	(void)CtrlFrameResponse(aAddr);
	}

// TRY_CBFC is this used for PNresponses ? got to check if our CBFC overture has been
// accepted. Appears only to be used for DM and UA responses where no action is needed
TInt CRfcommMuxer::CtrlFrameResponse(TUint8 aAddr)
	/**
	   Remove the ctrl frame that caused this response from the response pending Q

	   Responses are UA and DM, which may both be caused by SABM &
	   DISC frames.  However, there should only ever be one SABM/DISC
	   outstanding for a given address, so we simply look for a
	   matching address.
	**/
	{
	TDblQueIter<CRfcommFrame> iter(iResponseQ);
	CRfcommFrame* frm;

	while(iter)
		{
		frm=iter++;
		if(frm->Type() == KCtrlFrameType && DecodeDLCI(frm->Address()) == aAddr)
			{
			LOG(_L("RFCOMM: Found matching Ctrl command for response"));
			delete frm;
			break;
			}
		}
	return(iter?KErrNone:KErrNotFound);
	}

void CRfcommMuxer::ProcessDataFrame(TUint8 aAddr, TBool aPoll)
	/**
	   Send this data up to the sap.
	**/
	{
	__ASSERT_DEBUG(iNextPacket, Panic(ERfcommNoPacket));
	LOG(_L("CRfcommMuxer::ProcessDataFrame"));
	CRfcommSAP* sap=FindSAP(aAddr);
	if(sap)
		{
		if(iFlowStrategy->ProcessDataFrameReviseCredits(*sap, aPoll, iCurrentCredit))
			//returning'ETrue' => sap needs unblocking
			{
			LOG(_L("RFCOMM: CBFC unblocking"));
			SetSendBlocked(*sap, EFalse);
			sap->CanSend();
			}
		if (iCurrentDataLength)
			{
			sap->Data(iNextPacket->Mid(iCurrentHeaderLength, iCurrentDataLength));
			}
		else
			{
			LOG(_L("RFCOMM: Data frame with no data - probably a CBFC credit"));
			}
		}
	else
		{
		LOG1(_L("RFCOMM: Data for unknown sap %d"), aAddr);
		}
	}

void CRfcommMuxer::ParseCtrlMessageL()
	/**
	   Parse the control messages for the mux.

	   The data on the channel has the following format.

	   | Type | Length | Value 1 | Value 2 | ... | Value n |
	   unless this is PN frame then CL bits may indicate CBFC
	   and the K bits indicate the initial credit

	**/
	{
	TInt offset=0; // Number of bytes we've parsed.
	TInt x=0;
	TUint8 type=0;
	TInt length;
	TBool command;  // command or response

	if(!iMuxChannel->IsOpen())
		{
		LOG(_L("RFCOMM: Error - mux ctrl message before muxchannel open! Junking."));
//		return;  // MB - fixme
		}
	
	while(offset+2 <= iCurrentDataLength)
		{
		command=(*iNextPacket)[iCurrentHeaderLength+offset] & KCRBitmask;
		offset+=GetTypeFieldL(x, offset);
		// Type must be 8 bits or less for us to deal with
		if(x > 255 || x < 0)
			{
			User::Leave(KErrArgument);
			}
		type=static_cast<TUint8>(x);
		offset+=GetLengthFieldL(length, offset);
		if(offset+length > iCurrentDataLength)
			{
			User::Leave(KErrArgument);
			}
		
		LOG2(_L("RFCOMM: Incoming Packet, %d, %d"),type, length);

		// Now parse the individual types.  Offset points to the data
		switch(type)
			{
		case KTestType:
			HandleTest(command, iCurrentHeaderLength+offset, length);
			break;
		
		case KFConType:
			//FC
			if(length!=0)
				{
				LOG(_L("RFCOMM: FCon with non-zero length!"));
				}
			HandleFCon(command);
			//
			break;
		
		case KFCoffType:
			//FC
			if(length!=0)
				{
				LOG(_L("RFCOMM: FCoff with non-zero length!"));
				}
			HandleFCoff(command);
			//
			break;

		case KMSCType:
			{
			LOG(_L("RFCOMM: MSC:"));
			LOGHEXRAW(&((iNextPacket->Des())[0]), (iCurrentHeaderLength+iCurrentDataLength));

			LOG2(_L("RFCOMM: MSC Hex: Incoming Packet, %d, %d"), (*iNextPacket)[0], 80);
			
			// See TS 7.10, pg 29 for details
			// Length must be exactly 2, or 3 if 'Break Signals' are present
			if(length < 2 || length > 3)
				{
				LOG1(_L("RFCOMM: MSC with erroneous length %d"), length);
				break;
				}
			// Next byte is DLCI...
			TUint8 dlci = static_cast<TUint8>((*iNextPacket)[iCurrentHeaderLength+offset] >> 2);
			LOG1(_L("RFCOMM: DLCI %d"), dlci);
			// ...and the next is the V.24 signals
			TUint8 signals = static_cast<TUint8>((*iNextPacket)[iCurrentHeaderLength+offset+1] >> 1);
			LOG3(_L("RFCOMM: MSC %S received dlci %d signals 0x%02x"),
				(command?&KCommandText:&KResponseText), dlci, signals);
			
			if(!command)
				{
				LOG(_L("RFCOMM: Response"));
				}
			else
				{
				LOG(_L("RFCOMM: Command"));
				}
			LOG1(_L("RFCOMM: Signals 0x%02x"), signals);

			HandleMSC(command, dlci, signals);
			break;
			}

		case KRPNType:
		    {
			// RPN frame received from L2CAP
			LOG(_L("RFCOMM: RPN received"));
			if(length != KRPNRequestLength && 
			   length != KRPNCommandLength &&
			   length != KRPNResponseLength)
				{
				LOG(_L("RFCOMM: Ignoring RPN with bad length"));
				break;
				}
			TUint8 dlci=0;
			if(length == KRPNRequestLength)
				{
				// There's no parameter data
				ParseRPN(iCurrentHeaderLength+offset, length, dlci);
				HandleRPN(command, dlci); 
				}
			else // There was data so need to parse this too
				{
				TRfcommRPNTransaction rpnTransaction;
				ParseRPN(iCurrentHeaderLength+offset, length, dlci, 
						 &rpnTransaction);
				HandleRPN(command, dlci, &rpnTransaction);
				}
			break;
			}

		case KPNType:
		    {
			LOG(_L("RFCOMM: PN received"));
			LOGHEXRAW(&((iNextPacket->Des())[0]), (iCurrentHeaderLength+iCurrentDataLength));

			TRfcommPortParams portparams;
			TUint8 dlci;
			ParsePNL(iCurrentHeaderLength+offset, length, dlci, portparams);
			HandlePN(command, dlci, portparams);
			break;
			}
		
		case KNSCType:
			LOG(_L("RFCOMM: NSC received"));
			break;

		case KRLSType:
			{
			LOG(_L("RFCOMM: RLS received"));
			// See TS 7.10, pgs 34/35 for details
			const TUint8 KRLSLength = 2; // Length *must* be exactly 2
			if(length != KRLSLength)
				{
				LOG1(_L("RFCOMM: RLS with erroneous length %d"),
							  length);
				break;
				}
			// First byte is DLCI...
			TUint8 dlci = TUint8((*iNextPacket)[iCurrentHeaderLength+offset]) >> 2;
			// ...and the next is the status octet
			TUint8 status = TUint8((*iNextPacket)[iCurrentHeaderLength+offset+1]);
			LOG3(_L("RFCOMM: RLS %S received dlci %d status 0x%02x"),
					(command?&KCommandText:&KResponseText), dlci, status);
			HandleRLS(command, dlci, status);
			break;		
			}
		default:
			LOG1(_L("RFCOMM: Unknown type 0x%02x"), type);
			TransmitNSC(command, type);
			break;
			}
		offset+=length;
		}
	}

void CRfcommMuxer::ParsePNL(TInt aOffset, TInt aLen, TUint8& aDLCI,
						    TRfcommPortParams& aParams)
	/**
	   Parse the PN field at the specified offset in the frame.
	   Format is:
	   @verbatim
	   Value Octet Bit 1 Bit 2 Bit 3 Bit 4 Bit 5 Bit 6 Bit 7 Bit 8
	       1        D1    D2    D3    D4    D5    D6    0     0
    	   2        I1    I2    I3    I4    CL1   CL2   CL3   CL4
	       3        P1    P2    P3    P4    P5    P6    0     0
    	   4        T1    T2    T3    T4    T5    T6    T7    T8
    	   5        N1    N2    N3    N4    N5    N6    N7    N8
	       6        N9    N10   N11   N12   N13   N14   N15   N16
    	   7        NA1   NA2   NA3   NA4   NA5   NA6   NA7   NA8
     	   8        K1    K2    K3    0     0     0     0     0

	   Where:
	     D1-6  : DLCI
		 I1-4  : frame type (must be zero)
		 CL1-4 : Convergence layer (if CBFC 0xf for command, 0xe for response...
									...otherwise must be zero)
		 P1-6  : Priority
		 T1-8  : Acknowledgement timer T1 (must be zero)
		 N1-16 : Max frame size (default 127)
		 NA1-8 : Max retransmissions (must be zero)
		 K1-3  : either Initial CBFC credit or Error recovery window (must be zero)
		 @endverbatim
	**/
	{
	// Length must be exactly 8
	const TUint8 KPNLength = 8;
	if(aLen < KPNLength)
		{
		LOG(_L("RFCOMM: Invalid PN packet length"));
		User::Leave(KErrGeneral);
		}

	HBufC8& packet = *iNextPacket;

	// Constants for offsets into packet
	// DLCI is at start of byte, (offset = 0)
	// Ignore frame type		 (offset = 1)
	const TUint8 KConvergenceLayerOffset = 1; //used by CBFC
	const TUint8 KPriorityOffset		 = 2;
	// Ignore Ack timer			 (offset = 3)
	const TUint8 KMaxFrameLowerOffset	 = 4;
	const TUint8 KMaxFrameUpperOffset	 = 5;
	const TUint8 KErrorRecoveryWindow    = 7; //used by CBFC
	// Ignore the rest

	// DLCI - only want lowest 6 bits of byte
	aDLCI = packet[aOffset] & KMaxRfcommDLCI;
	// Priority - only want lowest 6 bits of byte
	aParams.iPriority = packet[aOffset+KPriorityOffset] & 0x3f;
	// Frame size is 2 bytes. Lower byte first...
	aParams.iMaxFrameSize = TUint16(packet[aOffset+KMaxFrameLowerOffset]);
	// ...then upper byte
	aParams.iMaxFrameSize += TUint16(packet[aOffset+KMaxFrameUpperOffset] << 8);
	aParams.iCreditIndicator = TUint8(packet[aOffset + KConvergenceLayerOffset] >> 4);
	aParams.iInitialCredit = TUint8(packet[aOffset + KErrorRecoveryWindow] & 7);
	LOG2(_L("RFCOMM: PN Rx(CBFC) CL 0x%02x, K 0x%02x"), aParams.iCreditIndicator, aParams.iInitialCredit);
	}

void CRfcommMuxer::ParseRPN(TInt aOffset, TInt aLen, TUint8& aDLCI, 
							TRfcommRPNTransaction* aRPNTransactionPtr)
	/**

	Parse an incoming RPN frame.  
	aOffset points to the start of the data octets on entry

	@verbatim
	Format is:
	Value Octet Bit 1 Bit 2 Bit 3 Bit 4 Bit 5 Bit 6 Bit 7 Bit 8
	     0       EA    1               DLCI
		 1       B1    B2    B3    B4    B5    B6    B7    B8
		 2       D1    D2    S     P     PT1   PT2   res   res
		 3       FLC1  FLC2  FLC3  FLC4  FLC5  FLC6  res   res
		 4       XON1  XON2  XON3  XON4  XON5  XON6  XON7  XON8
		 5       XOF1  XOF2  XOF3  XOF4  XOF5  XOF6  XOF7  XOF8
		 6       PM1   PM2   PM3   PM4   PM5   PM6   PM7   PM8
		 7       PM9   PM10  PM11  PM12  PM13  PM14  PM15  PM16

	Where:
	   B1-8   :  Baud rate
       D1-2   :  data bits
	   S      :  Stop bits
	   P      :  Parity
	   PT1-2  :  Parity Type
	   FLC1-6 :  Flow ctrl type
	   XON1-8 :  Xon character
	   XOFF1-8:  Xoff character
	   PM1-16 :  Which values are to be set/accepted
   @endverbatim
	See TS07.10 5.4.6.3.9 for more details
	**/
	{
	// Wrap a descriptor over the data we want
	TPtrC8 valueOctets=iNextPacket->Des().Mid(aOffset, aLen);
	
	// Get the DLCI
	aDLCI = valueOctets[0] >> 2;
	LOG1(_L("RFCOMM: RPN frame, DLCI %d"), aDLCI);

	// Now parse out the valid fields if necessary
	if(aRPNTransactionPtr)
		{
		LOG(_L("Parsing full RPN data\n"));
		// Local references into the transaction structure
		TRfcommRemotePortParams& portParams = aRPNTransactionPtr->iPortParams;
		TUint16& paramMask = aRPNTransactionPtr->iParamMask;
		// construct the parameter mask out of the last two value octets
		paramMask = TUint16(valueOctets[7]);
		paramMask = TUint16((paramMask << 8) + valueOctets[6]);
		
		// Bit rate
		if(paramMask & EPMBitRate)
		{
			TUint8 bitRate=valueOctets[1];
			switch(bitRate)
			{
			// Magic numbers because enums are not the same 
			// values as the ones specified in the spec and 
			// adding even more enums would confuse matters
			case 0:
				portParams.SetBitRate(EBps2400);
				break;
			case 1:
				portParams.SetBitRate(EBps4800);
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				portParams.SetBitRate(static_cast<TBps>(EBps7200+(bitRate-2)));
				break;
			default:
				{
				LOG(_L("RFCOMM: RPN with invalid bitrate field, ignoring"));
				}
			}
		}
		
		// Value octet 2 is compound
		// Masks for bit position within octet
		//							Bit number:	   76543210
		const TUint8 KDataBitsPos		= 0x03; // 000000XX
		const TUint8 KStopBitPos		= 0x04;	// 00000X00
		const TUint8 KParityBitPos		= 0x08;	// 0000X000
		const TUint8 KParityTypeBitsPos = 0x30; // 00XX0000
		// 2 most significant bits are reserved (always 0)

		const TUint8 KStopBitOffset		= 2; // offset for 00000X00
		const TUint8 KParityBitOffset	= 3; // offset for 0000X000
		const TUint8 KParityTypeOffset	= 4; // offset for 00XX0000
			
		if(paramMask & EPMDataBits)
			{
			TUint8 dataBits = TUint8(valueOctets[2] & KDataBitsPos);
			// Data bits
			switch(dataBits)
				{
				// Magic numbers because enums are not the same 
				// values as the ones specified in the spec and 
				// adding even more enums would confuse matters
				case 0:
					portParams.SetDataBits(EData5);
					break;
				case 1:
					portParams.SetDataBits(EData7);
					break;
				case 2:
					portParams.SetDataBits(EData6);
					break;
				case 3:
					portParams.SetDataBits(EData8);
					break;
				default:
					__DEBUGGER()
					LOG(_L("How did we get more than 3 with 2 bits?!\n"));
				}
			}

		if(paramMask & EPMStopBit)
			{
			TUint8 stopBit = TUint8((valueOctets[2] & KStopBitPos)>>KStopBitOffset);
			portParams.SetStopBit(static_cast<TStopBits>(stopBit));
			}

		// The Parity and ParityType bits have a combined effect..
		
		if(paramMask & EPMParity)
			{
			TUint8 parityBit = TUint8((valueOctets[2] & KParityBitPos)>>KParityBitOffset);
			
			if(!parityBit)
				{ 
				portParams.SetParity(EParityNone);
				}
			}

		if(paramMask & EPMParityType)
			{
			TParity parityType = EParitySpace; //Just to stop the compiler warnings

			//It is intended that this 'GetParity' will only update 'parityType' 
			//if 'SetParity' was called in the previous 'if' clause.
			portParams.GetParity(parityType);

			if(parityType == EParityNone)
				{
				// Can't have a parity type if there's no parity set
				LOG(_L("RFCOMM: WARNING!: Tried to set parity type when parity is none or invalid"));
				}
			else
				{	
				// Parity Type is in first two bits of upper nibble
				// Need to move it down to the lower nibble
				switch((valueOctets[2] & KParityTypeBitsPos)>>KParityTypeOffset)
					{
				case 0:
					portParams.SetParity(EParityOdd);
					break;
				case 1:
					portParams.SetParity(EParityMark);
					break;
				case 2:
					portParams.SetParity(EParityEven);
					break;
				case 3:
					portParams.SetParity(EParitySpace);
					break;
				default:
					break;
					}
				}
			}

		if(paramMask & EPMXOnChar)
			{
			portParams.SetXOnChar(valueOctets[4]);
			}

		if(paramMask & EPMXOffChar)
			{
			portParams.SetXOffChar(valueOctets[5]);
			}	

		// Flow control is only in the first 6 bits of this value octet
		TUint8 flowCtrl = TUint8(valueOctets[3] & 0x3f);
		portParams.UpdateWholeFlowCtrl(paramMask, flowCtrl);
		}
	else
		{	
		LOG(_L("Query request received - package has not been filled out\n"));
		}
	}


void CRfcommMuxer::HandleRLS(TBool aCommand, TUint8 aDLCI, TUint8 aStatus)
	/**
		Handle a RLS frame from remote dev

		If it's a command, respond then pass the data up to the user
		If it's a response, just ignore it - it'll just be what we sent
	**/
	{
	CRfcommSAP* sap=FindSAP(aDLCI);
	if(!sap)
		{
		LOG1(_L("RFCOMM: RLS for unknown dlci %d"), aDLCI);
		if(aCommand)
			SendDM(aDLCI);
		return;
		}

	if(aCommand)
		{
		LOG(_L("RFCOMM: RLS command received"));
		// Send a response to the remote device
		SendRLSRsp(*sap, aStatus);
		// Pass status up to the user
		sap->RLS(aStatus); 
		}
	else
		{
		// Response to our command
		LOG(_L("RFCOMM: RLS response received"));
		MuxCtrlResponse(KRLSType);
		}
	}


void CRfcommMuxer::HandleRPN(const TBool& aCommand, const TUint8& aDLCI, 
							 const TRfcommRPNTransaction* aRPNTransactionPtr)
	/**
	   Handle an RPN frame from remote dev

	   If it's valid, just pass it on to the SAP
	**/
	{	
	LOG1(_L("RFCOMM: Handling RPN for dlci %d"), aDLCI);

	if(aDLCI == KMuxDLCI)
		{
		LOG(_L("RFCOMM: RPN for Mux channel. Ignoring"));
		return;
		}

	if(!aCommand && aRPNTransactionPtr == 0)
		{
		LOG(_L("RFCOMM: Corrupt RPN response. Ignoring"));
		return;
		}
	
	// Check to see if there's a sap to pass this onto
	CRfcommSAP* sap=FindConnectedOrListeningSAP(aDLCI);
	
	if(sap == 0)// No listening or connected SAP available
		{
		if(aCommand)
			{
			LOG(_L("RFCOMM: RPN command for unknown sap - Sending DM"));
			SendDM(aDLCI);
			}
		else
			{
			LOG(_L("RFCOMM: RPN response for unknown sap. Ignoring"));
			}
		return;
		}

	// Still here? Everything's OK to pass the RPN on to the SAP
	if(aCommand)
		sap->RPN(aRPNTransactionPtr, *this, aDLCI);
	else // It's a valid response
		{
		// So we're no longer waiting for a response
		MuxCtrlResponse(KRPNType);
		sap->RPNRsp(*aRPNTransactionPtr);
		}
	}

void CRfcommMuxer::HandlePN(TBool aCommand, TUint8 aDLCI, TRfcommPortParams& aParams)
	/**
	   Handles PN commands & responses.
	   
	   Hands over to the SAP, but checks the max length first and
	   truncates if necessary before passing on.  We may also get a PN
	   for the mux channel, which is setting the frame size.
	**/
	{
	LOG2(_L("RFCOMM: Handling PN for dlci %d (MTU=%d)"), aDLCI, aParams.iMaxFrameSize);
	if(aDLCI == KMuxDLCI)
		{
		iMuxChannel->PN(aCommand, aParams);
		}
	else
		{
		CRfcommSAP* sap=FindConnectedOrListeningSAP(aDLCI);
		TBool useCBFC = aCommand?(aParams.iCreditIndicator == (KCBFCCommandFlag >> 4)):
								 (aParams.iCreditIndicator == (KCBFCResponseFlag >> 4));
		if(!sap)
			{
			if(aCommand)
				{
				//	Means no suitable listening SAP is around.
				LOG(_L("RFCOMM: PN for unknown sap - incoming connection request with no listening SAP"));
				TransmitDM(aDLCI, ETrue);	//	DM for specified DLCI with P/F bit set
				}
			else
				{
				// unexpected response
				LOG(_L("RFCOMM: PN response for unknown sap, ignoring"));
				}
			}
		else
			{// Found a SAP for this PN
			if (!iTriedCBFC)
				{
				if(useCBFC)
					{
					SetFlowType(CRfcommFlowStrategyFactory::EFlowCreditBased);
					}
				else
					{
					SetFlowType(CRfcommFlowStrategyFactory::EFlowNonCreditBased);
					}
				}

			if(aParams.iCreditIndicator == (KCBFCCommandFlag >> 4))
			//done for each PN
				{
				// need to set a one time initial credit in case
				// subsequent DLCs don't have a PN frame
				iInitialTxCredit = aParams.iInitialCredit;
				}

			if(aCommand)
				{
				sap->PN(aParams,*this,aDLCI);
				}
			else
				{
				sap->PNResp(aParams);
				}
			}
		}
	// FIXME not sure about this, 
	// if this is a SAP PN frame will it be in the muxctrl queue ?
	if(!aCommand)
		MuxCtrlResponse(KPNType);
	}


void CRfcommMuxer::HandleMSC(TBool aCommand, TUint8 aDLCI, TUint8 aSignals)
	/**
		Handles a MSC command & response from the remote device
	   
		If it's a command, send response and filter up to SAP
		If it's a response, we're no longer expecting a response (!)
	**/
	{
	CRfcommSAP* sap=FindSAP(aDLCI);
	if(aCommand)
		{
		LOG(_L("RFCOMM: MSC command received"));
		if(sap)
			{
			// Send a response to remote device
			SendMSCRsp(*sap, aSignals);
			// Pass signals up to user
			sap->MSC(aSignals); 
			}
		else
			{
			LOG1(_L("RFCOMM: MSC for unknown dlci %d"), aDLCI);
			SendDM(aDLCI);
			}
		}
	else
		{ 
		LOG(_L("RFCOMM: MSC response received"));
		MuxCtrlResponse(KMSCType);
		}
	}

void CRfcommMuxer::HandleTest(TBool aCommand, TInt aOffset, TInt aLen)
	/**
	   Handles a test command.

	   If this is a command, then respond with the same data bytes as
	   were sent to us.  Else ignore it.
	**/
	{
	if(aCommand)
		{
		LOG(_L("RFCOMM: Test command received"));
		__ASSERT_DEBUG(aLen < iMuxChannel->MaxDataSize(), Panic(ERfcommTestDataTooLong));
		TransmitTest(!aCommand, TPtr8(&iNextPacket->Des()[aOffset], aLen, aLen));
		}
	else
		{
		MuxCtrlResponse(KTestType);
		LOG(_L("RFCOMM: Test response received"));
		}
	}

void CRfcommMuxer::HandleFCon(TBool aCommand)
	/**
	   Handle a FCon instruction.

	   If this is a command, then we need to respond and set flow ctrl
	   to OK.  Otherwise it's a response to our FCon command, and we
	   can expect the remote end to start sending.
	**/
	{
	if(aCommand)
		{
		LOG(_L("RFCOMM: FCon command received"));
		if(ClearToSend())
			{
			LOG(_L("RFCOMM: Received FCon when state was ON"));
			}
		SetCanSend(ETrue);
		if(TransmitFCon(EFalse) != KErrNone)
			{
			LOG(_L("RFCOMM: Failed to send FCon response!"));
			}
		TryToSend();  // Try to send even if we couldn't respond
		}
	else
		{
		LOG(_L("RFCOMM: FCon response received"));
		MuxCtrlResponse(KFConType);
		}
	}

void CRfcommMuxer::HandleFCoff(TBool aCommand)
	/**
	   Handles a FCoff command.

	   If this is a command, then set our state to flow ctrl off, and
	   send a response.  If it was a reply, then we should now not
	   expect any more data packets till we send a FCon.
	**/
	{
	//FC
	if(aCommand)
		{
		LOG(_L("RFCOMM: FCoff command received"));
		if(!ClearToSend())
			{
			LOG(_L("RFCOMM: Received FCoff when state was off"));
			}
		SetCanSend(EFalse);
		TransmitFCoff(EFalse);
		}
	else
		{
		LOG(_L("RFCOMM: FCoff response received"));
		MuxCtrlResponse(KFCoffType);
		}
	//
	}

TInt CRfcommMuxer::GetTypeFieldL(TInt& aType, TInt aPos)
	/**
	   Parse the type field.  Due to EA bits this is of unknown length.

	   Type field looks like this :
	   @verbatim
       | EA | C/R | T1 | T2 | T3 | T4 | T5 | T6 |

	   If EA is 0, then the following bytes are extensions:

	   | EA | T7 | T8 | T9 | T10 | T11 | T12 | T13 |
	   @endverbatim
	   @param aType The type is returned in this
	   @param aPos  The start position in frame's data
	   @return The length of the type field in bytes.
	**/
	{
	const TUint8 KBitsInFirstTypeFieldByte = 6;
	const TUint8 KBitsInSubsequentTypeFieldBytes = 7;
	
	TInt readptr = iCurrentHeaderLength + aPos;
	aType = (*iNextPacket)[readptr] >> 2; // drop the EA and C/R bit (any good reason the latter?)
	TInt offset = 1; // in bytes
	TUint8 bitshift = KBitsInFirstTypeFieldByte;
	
	if(!((*iNextPacket)[readptr] & KEABitmask))
		{
		// Multiple length type field
		LOG(_L("RFCOMM: Multi-length type field in mux channel"));
		do
			{
			if(((readptr + offset - iCurrentHeaderLength) > iCurrentDataLength)
				|| ((bitshift + KBitsInSubsequentTypeFieldBytes) > KBitsForNumberInTInt))
				{
				// We enter here if the EA's are such that we cannot handle the field, by either
				// 1) They indicated there was another type field byte which isn't present.
				// 2) They indicate a type field value that cannot be represented in a TInt.
				User::Leave(KErrArgument);
				}
			// we should also check that we don't have too extensions
			// (i.e. we have a number larger than a TInt)
			aType += ((*iNextPacket)[readptr+offset] >> 1) << bitshift;
			offset++;
			bitshift += KBitsInSubsequentTypeFieldBytes;
			}
		while (!((*iNextPacket)[readptr+offset-1] & KEABitmask));
		}
	
	return offset;
	}
	
TInt CRfcommMuxer::GetLengthFieldL(TInt& aLen, TInt aPos)
	/**
	   Parse the length field.  Due to EA bits this is of unknown length.

	   Type field looks like this :

       | EA | L1 | L2 | L3 | L4 | L5 | L6 | L7 |

	   @param aLen The length is returned in this
	   @param aPos The start position in the frame's data
	   @return The length of the length field.
	**/
	{
	const TUint8 KBitsInLengthFieldByte = 7;
	
	TInt readptr = iCurrentHeaderLength + aPos;
	aLen = (*iNextPacket)[readptr] >> 1; // discard the EA bit
	TInt offset = 1;
	TUint8 bitshift = KBitsInLengthFieldByte;
	
	if(!((*iNextPacket)[readptr] & KEABitmask))
		{
		// Multiple length Len field
		LOG(_L("RFCOMM: Multi-length length field in mux channel"));
		do
			{
			// Initially we do some checking to ensure field isn't malformed.
			// Part 1 of 2.
			if((readptr + offset - iCurrentHeaderLength) > iCurrentDataLength)
				{
				// We enter here if the EAs indicate that there is another length field byte
				// when there isn't.
                User::Leave(KErrArgument);
				}

			// Now we know that there is sufficient data in the packet, we can get the next length value
			TUint8 value((*iNextPacket)[readptr+offset] >> 1);
			
			// Do some more checking of the field
			// Part 2 of 2.
			if((bitshift + KBitsInLengthFieldByte) > KBitsForNumberInTInt)
				{
				// We enter here if the EA's indicate that the length value the field represents is
				// potentially larger than a TInt.
				// However it may be that the overflowed bits are just trailing zeros, meaning that
				// we can represent it.
				TUint8 trailingZeroMask(0xff);
				trailingZeroMask <<= (KBitsForNumberInTInt - bitshift);
				trailingZeroMask &= value;
				if(trailingZeroMask != 0x00)
					{
					// We enter here if there are bits which would overflow a TInt
					User::Leave(KErrArgument);
					}
				}

			// If here the byte appears valid, so update the length as appropriate.
			aLen += value << bitshift;
			offset++;
			bitshift += KBitsInLengthFieldByte;
			}
		while (!((*iNextPacket)[readptr+offset-1] & KEABitmask));
		}
	
	return offset;
	}

void CRfcommMuxer::MuxCtrlResponse(TUint8 aType)
	/**
	   Remove the mux ctrl frame that caused this response from the
	   response pending Q

	   To fully match a response, one would need to look at all the
	   parameters to the command.  However, we cheat by just matching
	   the command type.  This may lead us to remove the wrong one,
	   but since any lack of a match brings down the link, we go for
	   the easy option.
	**/
	{
	TDblQueIter<CRfcommFrame> iter(iResponseQ);
	CRfcommFrame* frm;

	while(iter)
		{
		frm=iter++;
		if(frm->Type() == KMuxCtrlFrameType)
			{
			CRfcommMuxCtrlFrame* muxframe=static_cast<CRfcommMuxCtrlFrame*>(frm);
			if(muxframe->CommandType() == aType)
				{
				LOG(_L("RFCOMM: Found matching muxctrl command for response"));
				delete frm;
				break;
				}
			}
		}
	}

void CRfcommMuxer::MuxCtrlResponseDM(TUint8 aDLCI)
	/**
	   Remove the mux ctrl frame that caused this DM response from the
	   response pending Q

	   All we have to work on is the DLCI of the channel which issued the
	   original mux control frame which elicited the DM response. We
	   can search through the response queue until we find a frame sent 
	   out for this DLCI.
    **/
	{
	TDblQueIter<CRfcommFrame> iter(iResponseQ);
	CRfcommFrame* frm;

	while(iter)
		{
		frm=iter++;
		if(frm->Type() == KMuxCtrlFrameType)
			{
			CRfcommMuxCtrlFrame* muxframe=static_cast<CRfcommMuxCtrlFrame*>(frm);
			if(muxframe->DLCI() == aDLCI)
				{
				LOG(_L("RFCOMM: Found matching muxctrl command for DM response"));
				delete frm;
				break;
				}
			}
		}
	}

TInt CRfcommMuxer::TransmitSABM(TUint8 aDLCI, CRfcommSAP* aSAP)
	/**
	   Send a SABM command for specified DLCI.
	   This command is used to start the connection of a channel,
	   including the control channel.
	   @verbatim
	   Frame format is:

	   Address | Control | Length | FCS

	   Address format is:

	   | 1  |  2  | 3 | 4   5   6   7   8 |
	   | EA | C/R | D |  Server channel   |

	   Where aDLCI already includes the D & Server channel bits.
	   @endverbatim

	   We always set the P/F bit to one in a SABM frame.
	   
	   @param aDLCI This value is the server channel plus direction
	   bit part of the DLCI.  This will have the E/A & C/R bits added.

	   @return Error code
	**/
	{
	LOG1(_L("RFCOMM: Sending SABM for DLCI %d"), aDLCI); 
	// We always expect a response for a SABM, so this frame should
	// be associated with a SAP or be sent by the mux channel.
	__ASSERT_DEBUG(aSAP || (aDLCI == KMuxDLCI), Panic(ERfCommNothingToHandleResponse));
	
	CRfcommCtrlFrame* frm=NewFrame(aSAP);
	
	if(!frm)
		return KErrNoMemory;

	frm->SetResponseNeeded(ETrue);
	frm->SetAddress(BuildAddr(aDLCI, ETrue));
	frm->SetControl(KSABMCtrlField | KPollFinalBitmask);
	/*
	BLOG: The frame has now been prepared therefore can show all bits. 
	//BLOG: Can show all the information about this frame in TrytoSend() just as it is physically about
	//to go over the air - Bluetooth...wooohooooo!
	//Mind you probably should blog DLCI here? 
	//BLOG KBlogDLCI
	BlogTransmitCtrlFrame(frm, aDLCI, aSAP)
	*/	
	EnqueFrame(frm);
	
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitUA(TUint8 aDLCI, CRfcommSAP* aSAP)
	{
	LOG1(_L("RFCOMM: Sending UA for DLCI %d"), aDLCI); 
	CRfcommCtrlFrame* frm=NewFrame(aSAP);
	if(!frm)
		return KErrNoMemory;

	// UA always has Final set
	frm->SetControl(KUACtrlField | KPollFinalBitmask);
	frm->SetAddress(BuildAddr(aDLCI, EFalse));
	EnqueFrame(frm);
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitDM(TUint8 aDLCI, TBool aPFBit, CRfcommSAP* aSAP)
	{
	LOG1(_L("RFCOMM: Sending DM for DLCI %d"), aDLCI); 
	CRfcommCtrlFrame* frm=NewFrame(aSAP);
	if(!frm)
		return KErrNoMemory;

	if(aPFBit)
		frm->SetControl(KDMCtrlField | KPollFinalBitmask);
	else
		frm->SetControl(KDMCtrlField);
	frm->SetAddress(BuildAddr(aDLCI, EFalse));
	EnqueFrame(frm);
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitDISC(TUint8 aDLCI, CRfcommSAP* aSAP)
	{
	LOG1(_L("RFCOMM: Sending DISC for DLCI %d"), aDLCI); 
	// In setting the response needed flag we assume that only SAPs 
	// will send a DISC, not the mux channel.  If this changes in the 
	// future then the condition on setting the response needed flag 
	// needs reconsidering to deal with this.
	__ASSERT_DEBUG(aDLCI != KMuxDLCI, Panic(ERfCommDiscSentOnMuxDlci));

	CRfcommCtrlFrame* frm=NewFrame(aSAP);
	if(!frm)
		return KErrNoMemory;

	// We only wait for a response for a DISC if there's going to 
	// be something still around to receive it, otherwise we'll just 
	// ignore the response when it comes in.
	frm->SetResponseNeeded(aSAP ? ETrue : EFalse);
	
	// DISC always has Final set
	frm->SetControl(KDISCCtrlField | KPollFinalBitmask);
	frm->SetAddress(BuildAddr(aDLCI, ETrue));  // C/R set
	EnqueFrame(frm);
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitRPN(TUint8 aDLCI, TBool aCommand, TUint8 aLen,
							   const TRfcommRPNTransaction& aRPNTransaction, CRfcommSAP* aSAP)
	/**

	Transmit an RPN frame. 

	@verbatim
	Format is:
	Value Octet Bit 1 Bit 2 Bit 3 Bit 4 Bit 5 Bit 6 Bit 7 Bit 8
		 1		 E/A   C/R	---------------DLCI----------------
		 2       B1    B2    B3    B4    B5    B6    B7    B8
		 3       D1    D2    S     P     PT1   PT2   res   res
		 4       FLC1  FLC2  FLC3  FLC4  FLC5  FLC6  res   res
		 5       XON1  XON2  XON3  XON4  XON5  XON6  XON7  XON8
		 6       XOF1  XOF2  XOF3  XOF4  XOF5  XOF6  XOF7  XOF8
		 7       PM1   PM2   PM3   PM4   PM5   PM6   PM7   PM8
		 8       PM9   PM10  PM11  PM12  PM13  PM14  PM15  PM16

	Where:
	   B1-8   :  Baud rate
       D1-2   :  data bits
	   S      :  Stop bits
	   P      :  Parity
	   PT1-2  :  Parity Type
	   FLC1-6 :  Flow ctrl type
	   XON1-8 :  Xon character
	   XOFF1-8:  Xoff character
	   PM1-16 :  Which values are to be set/accepted
   
	See TS07.10 5.4.6.3.9 for more details
	@endverbatim
	**/
	{
	CRfcommMuxCtrlFrame* frm = 0;
	
	__ASSERT_DEBUG(aLen == KRPNCommandLength || aLen == KRPNRequestLength || 
				   aLen == KRPNResponseLength,	Panic(ERfcommInvalidRPNLength));
	// We never expect an RPN on the mux channel
	__ASSERT_DEBUG(aDLCI != KMuxDLCI, Panic(ERfCommUnexpectedCommandOnMuxChannel));
	
	// We always expect a response for an RPN command frame, so this frame should
	// be associated with a SAP.
	__ASSERT_DEBUG(!aCommand || aSAP, Panic(ERfCommNothingToHandleResponse));

	LOG(_L("RFCOMM: Creating RPN frame"));
	frm=NewSignalFrame(aLen, aCommand, aSAP);
	if(!frm)
		return KErrNoMemory;
	
	frm->SetDLCI(aDLCI);	//	For use when matching DM responses to mux control command frames
	frm->SetCommandType(KRPNType, aCommand);
	frm->SetResponseNeeded(aCommand);

	// Now the only parameter that's always present, the DLCI
	// Make sure the DLCI is only 6 bits long
	__ASSERT_DEBUG(aDLCI <= KMaxRfcommDLCI, Panic(ERfcommInvalidDLCI));
	aDLCI &= KMaxRfcommDLCI;
	frm->PutByte((aDLCI << 2) | 0x03); // Add E/A and C/R (both 1)

	TUint8 valueOctet;

	// If we're creating a negotiation request or a response of any kind,
	// fill in the rest of the value octets 
	if(aLen == KRPNCommandLength || aLen == KRPNResponseLength)
		{
		const TRfcommRemotePortParams& portParams = aRPNTransaction.iPortParams;
	
		// Second octet carries Baud Rate (AKA 'Bit Rate')
		TBps bitRate = EBps9600; //Just to stop the compiler warnings
		if(portParams.GetBitRate(bitRate)) // Sets bitRate if valid in portParams
			{
			valueOctet = 0;
			switch(bitRate)
				{
			// Magic numbers because enums are not the same 
			// values as the ones specified in the spec and 
			// adding even more enums would confuse matters
			case EBps2400:
				valueOctet = 0;
				break;
			case EBps4800:
				valueOctet = 1;
				break;
			case EBps7200:
			case EBps9600:
			case EBps19200:
			case EBps38400:
			case EBps57600:
			case EBps115200:
			case EBps230400:
				valueOctet = (bitRate - EBps7200) + 2;
				break;
			default:
				LOG(_L("RFCOMM:[rfcommmuxer.cpp] Invalid bitrate, ignoring"));
				__DEBUGGER(	);
				}
			}
		else
			{
			valueOctet = 0;
			}
		frm->PutByte(valueOctet);			
		
		// Third octet is compound. Each part is offset from 
		// the beginning of the octet.  
		//								Bit No. 76543210
		// Data Bits are stored at start:		000000XX
		const TUint8 KStopBitOffset		= 2; // 00000X00
		const TUint8 KParityBitOffset	= 3; // 0000X000
		const TUint8 KParityTypeOffset	= 4; // 00XX0000
		
		// Now it gets complicated - here we go...
		// Start with the 2 data bits
		TDataBits dataBits = EData5; //Just to stop the compiler warnings
		if(portParams.GetDataBits(dataBits))
			{
			switch (dataBits)
				{
				// Magic numbers because enums are not the same 
				// values as the ones specified in the spec and 
				// adding even more enums would confuse matters
				case EData5:	
					valueOctet = 0;
					break;
				case EData6:
					valueOctet = 2;
					break;
				case EData7:
					valueOctet = 1;
					break;
				case EData8:
					valueOctet = 3;
					break;
				default:
					__DEBUGGER();
					LOG(_L("TransmitRPN - Dodgy Data Bits. Assuming 0\n"));
					valueOctet = 0;
				}
			}
		else
			valueOctet = 0;

		// Set the stop bit	if necessary
		// EStop1 corresponds to leaving the stop bit clear
		// EStop2 corresponds to setting the stop bit
		TStopBits stopBit = EStop1; //Just to stop the compiler warnings
		if(portParams.GetStopBit(stopBit))
			{
			if(stopBit == EStop2)
				valueOctet |=  (1 << KStopBitOffset);
			}

		TParity parity = EParityNone; //Just to stop the compiler warnings
		if(portParams.GetParity(parity))
			{
			// Set the parity bit by default
			valueOctet |= (1 << KParityBitOffset);
			// Set Parity Type (and clear parity bit if necessary)
			switch(parity)
				{
				// Magic numbers because enums are not the same 
				// values as the ones specified in the spec and 
				// adding even more enums would confuse matters
				case EParityNone:
					// Clear the parity bit
					valueOctet &= ~(1 << KParityBitOffset); 
					break;
				case EParityOdd:
					// Do nothing, this parity type is stored as 0
					break;
				case EParityEven:
					valueOctet |= (2 << KParityTypeOffset);
					break;
				case EParityMark:
					valueOctet |= (1 << KParityTypeOffset); 
					break;
				case EParitySpace:
					valueOctet |= (3 << KParityTypeOffset); 
					break;
				}
			}
		// The reserved bits are already 0 so ignore them
		// Finally finished building Octet 3, now store it
		frm->PutByte(valueOctet);

		// Octet 4 contains 6 flow control bits and 
		// 2 reserved bits (both zero)
		if(portParams.GetFlowCtrl(valueOctet))
			{
			// Make sure the Flow Control is only 6 bits
			if (valueOctet >> 6 != 0)
				{
				LOG(_L("Invalid Flow Control\n"));
				__DEBUGGER();
				delete frm;
				
				return KErrArgument;
				}
			frm->PutByte(valueOctet);
			}
		else
			{
			frm->PutByte(0);
			}


		// Octet 5 contains 8 XON bits
		if(portParams.GetXOnChar(valueOctet))
			{
			frm->PutByte(valueOctet);
			}
		else
			{
			frm->PutByte(0);
			}
		
		// Octet 6 contains 8 XOFF bits
		if(portParams.GetXOffChar(valueOctet))
			{
			frm->PutByte(valueOctet);
			}
		else
			{
			frm->PutByte(0);
			}
		
		// Octets 7 and 8 are the Parameter Mask
		// Lowest byte first...
		valueOctet = TUint8(aRPNTransaction.iParamMask & 0xff);
		frm->PutByte(valueOctet);
		// ...then the upper byte
		valueOctet = TUint8(aRPNTransaction.iParamMask >> 8);
		frm->PutByte(valueOctet);
		}

	// Finished building frame, so send it	
	EnqueFrame(frm);
	
	return KErrNone;
	}


TInt CRfcommMuxer::TransmitPN(TUint8 aDLCI, TBool aCommand,
							  const TRfcommPortParams& aParams, CRfcommSAP* aSAP)
	/**
	   Format is:
	   @verbatim
	   Value Octet Bit 1 Bit 2 Bit 3 Bit 4 Bit 5 Bit 6 Bit 7 Bit 8
	       1        D1    D2    D3    D4    D5    D6    0     0
    	   2        I1    I2    I3    I4    CL1   CL2   CL3   CL4
	       3        P1    P2    P3    P4    P5    P6    0     0
    	   4        T1    T2    T3    T4    T5    T6    T7    T8
    	   5        N1    N2    N3    N4    N5    N6    N7    N8
	       6        N9    N10   N11   N12   N13   N14   N15   N16
    	   7        NA1   NA2   NA3   NA4   NA5   NA6   NA7   NA8
     	   8        K1    K2    K3    0     0     0     0     0
	   Where:
	     D1-6  : DLCI
		 I1-4  : frame type (must be zero)
		 CL1-4 : Convergence layer 
				 (if CBFC, set to 0x0f in request, 0x0e in response....
				  .....otherewise must be zero)
		 P1-6  : Priority
		 T1-8  : Acknowledgement timer T1 (must be zero)
		 N1-16 : Max frame size (default 127)
		 NA1-8 : Max retransmissions (must be zero)
		 K1-3  : Error recovery window 
				 (initial credit (0-7) for CBFC or set to 0 if fallback to FCon/off)
	  @endverbatim
	 **/
	{
	LOG3(_L("RFCOMM: Sending PN %S for dlci %d (MTU=%d)"), 
		(aCommand?&KCommandText:&KResponseText) , aDLCI, aParams.iMaxFrameSize);
	// We never expect to send a PN on the mux channel, however to aid interop 
	// with shoddy remotes we tolerate responding to one
	__ASSERT_DEBUG(((aDLCI != KMuxDLCI) || !aCommand), Panic(ERfCommUnexpectedCommandOnMuxChannel));
	
	// We always expect a response for a PN command frame, so this frame should
	// be associated with a SAP.
	__ASSERT_DEBUG(!aCommand || aSAP, Panic(ERfCommNothingToHandleResponse));

	CRfcommMuxCtrlFrame* frm=NewSignalFrame(KRPNCommandLength, aCommand, aSAP);

	if(!frm)
		return KErrNoMemory;

	frm->SetDLCI(aDLCI);	//	For use when matching DM responses to mux control command frames
	frm->SetCommandType(KPNType, aCommand);
	frm->SetResponseNeeded(aCommand);
	// Now the 8 parameters
	__ASSERT_DEBUG(aDLCI <= KMaxRfcommDLCI, Panic(ERfcommInvalidDLCI));
	aDLCI &= KMaxRfcommDLCI;
	frm->PutByte(aDLCI);
	//TRY_CBFC ... NB used to do some negotiating here - but in current version all
	//				   such negotiation is performed in HandlePN
#ifdef NO_CBFC
	frm->PutByte(0x00);
#else
	frm->PutByte(iFlowStrategy->PNConvergenceLayer(aCommand));
#endif
	LOG2(_L("RFCOMM: CBFC Tried 0x%x, Type 0x%x"), iTriedCBFC, FlowType());

	__ASSERT_DEBUG(aParams.iPriority <= KMaxRfcommPriority, Panic(ERfcommInvalidDLCI));
	TUint8 params = aParams.iPriority & KMaxRfcommPriority;
	frm->PutByte(params);
	frm->PutByte(0x00); // T bits
	frm->PutLittleEndian16(aParams.iMaxFrameSize);
	frm->PutByte(0x00); // NA bits
#ifdef NO_CBFC
	frm->PutByte(0x00);
#else
	frm->PutByte(aParams.iInitialCredit);
#endif
	EnqueFrame(frm);
	
	return KErrNone;
	}


TInt CRfcommMuxer::TransmitFCon(TBool aCommand, CRfcommSAP* aSAP)
	/**
	   Send a FCon command frame.

	   If aCommand is True then this should be a command, else it is a
	   response.
	**/
	{
	LOG(_L("RFCOMM: Trying to send FCon"));
	if(!(iFlowStrategy->AllowFCOnOff(aCommand)))
		return KErrNotSupported; //Occurs if CBFC is on.

	LOG1(_L("RFCOMM: Sending FCon %S"),aCommand?&KCommandText:&KResponseText);
	//FC
	CRfcommMuxCtrlFrame* frm=NewSignalFrame(KFConCommandLength, aCommand, aSAP);
	if(!frm)
		return KErrNoMemory;

	frm->SetCommandType(KFConType, aCommand);
	frm->SetResponseNeeded(aCommand);
	EnqueFrame(frm);
	//
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitNSC(TBool aCommand, TUint8 aType)
	/**
	   Send a NSC command frame.

	**/
	{
	LOG(_L("RFCOMM: Sending NSC"));
	
	CRfcommMuxCtrlFrame* frm=NewSignalFrame(KNSCCommandLength, EFalse);
	if(!frm)
		return KErrNoMemory;

	frm->SetCommandType(KNSCType, EFalse);
	aCommand = (aCommand) ? 1 : 0;
	frm->PutByte(aType | (aCommand << 1));
	EnqueFrame(frm);
	
	return KErrNone;
	}


TInt CRfcommMuxer::TransmitFCoff(TBool aCommand, CRfcommSAP* aSAP)
	/**
	   Send a FCoff command frame.

	   If aCommand is True then this should be a command, else it is a
	   response.
	**/
	{
	LOG(_L("RFCOMM: Trying to send FCoff"));
	if(!(iFlowStrategy->AllowFCOnOff(aCommand)))
		return KErrNotSupported; //Occurs if CBFC is on.

	LOG1(_L("RFCOMM: Sending FCoff %S"),aCommand?&KCommandText:&KResponseText);
	//FC
	CRfcommMuxCtrlFrame* frm=NewSignalFrame(KFConCommandLength, aCommand, aSAP);
	if(!frm)
		return KErrNoMemory;

	frm->SetCommandType(KFCoffType, aCommand);
	frm->SetResponseNeeded(aCommand);
	EnqueFrame(frm);
	//
	return KErrNone;
	}


TInt CRfcommMuxer::TransmitMSC(TUint8 aDLCI, TBool aCommand, TUint8 aSignals, CRfcommSAP* aSAP)
	/**
	   Send out a MSC signalling command (or response) which travels in a UIH frame

	   Format is (TS07.10 5.4.6.3.7):

	   | Command | Length | DLCI | Signals |
	**/
	{
	LOG3(_L("RFCOMM: SendMSC %S DLCI %d, Signals %x"),
		(aCommand?&KCommandText:&KResponseText), aDLCI, aSignals);
	
	// We never expect an MSC on the mux channel
	__ASSERT_DEBUG(aDLCI != KMuxDLCI, Panic(ERfCommUnexpectedCommandOnMuxChannel));
	
	// We always expect a response for an MSC command frame, so this frame should
	// be associated with a SAP.
	__ASSERT_DEBUG(!aCommand || aSAP, Panic(ERfCommNothingToHandleResponse));
	
	CRfcommMuxCtrlFrame* frm=NewSignalFrame(KMSCCommandLength, aCommand, aSAP);
	if(!frm)
		return KErrNoMemory;

	frm->SetDLCI(aDLCI);	//	For use when matching DM responses to mux control command frames
	frm->SetCommandType(KMSCType, aCommand);
	frm->SetResponseNeeded(aCommand);
	// DLCI
	frm->PutByte((aDLCI << 2) | 0x03); // dlci, 1, EA
	// Signals
	iFlowStrategy->OutgoingMSCReviseSignals(aCommand, aSignals); //FC-bit to zero if CBFC.
	frm->PutByte((aSignals << 1) | 0x01); // signals, EA
	EnqueFrame(frm);
	
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitTest(TBool aCommand, const TDesC8& aData, CRfcommSAP* aSAP)
	/**
	   Send out a Test signalling command
	**/
	{
	LOG1(_L("RFCOMM: Sending Test %S"),(aCommand?&KCommandText:&KResponseText));
	
	CRfcommMuxCtrlFrame* frm=NewSignalFrame(aData.Length(), aCommand, aSAP);
	if(!frm)
		return KErrNoMemory;

	frm->SetCommandType(KTestType, aCommand);
	frm->SetResponseNeeded(aCommand);
	// Put the data in
	frm->PutData(aData);
	EnqueFrame(frm);
	
	return KErrNone;
	}

TInt CRfcommMuxer::TransmitRLS(TBool aCommand, TUint8 aDLCI, TUint8 aStatus,
							   CRfcommSAP* aSAP)
	/**
	   Send out a RLS signalling command
	**/
	{
	// We never expect an RLS on the mux channel
	__ASSERT_DEBUG(aDLCI != KMuxDLCI, Panic(ERfCommUnexpectedCommandOnMuxChannel));
	
	// We always expect a response for an RLS command frame, so this frame should
	// be associated with a SAP.
	__ASSERT_DEBUG(!aCommand || aSAP, Panic(ERfCommNothingToHandleResponse));
	
	CRfcommMuxCtrlFrame* frm=NewSignalFrame(KRLSCommandLength, aCommand, aSAP);
	if(!frm)
		return KErrNoMemory;

	frm->SetDLCI(aDLCI);	//	For use when matching DM responses to mux control command frames
	frm->SetCommandType(KRLSType, aCommand);
	frm->SetResponseNeeded(aCommand);
	// Now the DLCI 
	frm->PutByte(aDLCI << 2 | 0x03);  // C/R & EA both 1
	// Now the status
	frm->PutByte(aStatus);
	EnqueFrame(frm);
	
	return KErrNone;
	}


TUint8 CRfcommMuxer::BuildAddr(TUint8 aDLCI, TBool aCommand)
	/**
	   Add C/R and EA bits to an existing DLCI to create an address field.

	   Format of address is:

	   EA | C/R | DLCI |

	   where C/R depends on whether this is a command frame and what
	   the direction bit is set to.
	**/

	{
	aDLCI<<=2; // Shift up for C/R and EA bits
	aDLCI |= 0x01; // EA is 1
	if(aCommand)
		{
		aDLCI |= iDirection << 1; // C/R is same as direction for commands
		}
	else
		{
		aDLCI |= (~iDirection & 1) << 1;  // Response has C/R =!Direction
		}
	return aDLCI;
	}

CRfcommMuxCtrlFrame* CRfcommMuxer::NewSignalFrame(TInt aCommandLength, TBool aCommand, CRfcommSAP* aSAP)
/**
	Create a new UIH frame for a mux command

	NB The command length excludes the Type octet and the Length octet(s)
**/
	{
	CRfcommMuxCtrlFrame* frm=NULL;
	TRAPD(err, frm=CRfcommMuxCtrlFrame::NewL(aCommandLength, *this, aSAP));
	if(!err)
		{
		// The C/R bit for the UIH frame is always 1 for the initiator
		// and 0 for the responder, which is the same as for commands.
		frm->SetAddress(BuildAddr(KMuxDLCI, ETrue));
		frm->SetControl(KUIHCtrlField);
		frm->SetResponseNeeded(aCommand);  // Commands need responses
		}
	return frm;	//	NB frm will be NULL if NewL failed.
	}

CRfcommCtrlFrame* CRfcommMuxer::NewFrame(CRfcommSAP* aSAP)
	/**
	   Returns a ctrl frame (SABM, DISC, DM, UA)
	**/
	{
	return new CRfcommCtrlFrame(*this, aSAP);
	}

CRfcommDataFrame* CRfcommMuxer::NewDataFrame(TUint8 aDLCI, TInt aLen, 
											 TUint8 aCredit, CRfcommSAP* aSAP)
	/**
	   Returns a new data (UIH) frame for the specified DLCI

	   The C/R bit for the UIH frame is always 1 for the initiator and
	   0 for the responder, which is the same as for commands.
	**/
	{
	//	NB should be NULL if CRfcommDataFrame was not created
	return iFlowStrategy->NewDataFrame(*this, BuildAddr(aDLCI, ETrue), aLen, aCredit, aSAP);
	}

TBool CRfcommMuxer::CheckFCS(TUint8 aFCS, TUint8 aCtrl)
	/**
	   Check the FCS on the incoming frame.

	   FCS calculated over addr & ctrl for UIH, and addr, ctrl & len
	   for the others.
	**/
	{
	TUint8* data=&iNextPacket->Des()[0];
	TInt len;
	
	if(aCtrl == KUIHCtrlField)
		len=2;  // addr & ctrl fields
	else
		len=iCurrentHeaderLength;

	return ::CheckFCS(data, len, aFCS);
	}

CRfcommSAP* CRfcommMuxer::FindSAP(const TUint8 aDLCI)
	/**
	   Find the SAP that is on this channel.

	   If no such sap exists, return 0
	**/
	{
	TDblQueIter<CRfcommSAP> iter(iSAPs);
	CRfcommSAP* sap;

	while(iter)
		{
		sap=iter++;
		if(sap->DLCI() == aDLCI)
			return sap;
		}

	iter=iBlockedSAPs;
	while(iter)
		{
		sap=iter++;
		if(sap->DLCI() == aDLCI)
			return sap;
		}

	return 0;
	}

CRfcommSAP* CRfcommMuxer::FindConnectedOrListeningSAP(const TUint8 aDLCI)
	/**
	Find a SAP that is on this channel, or get a listening SAP from the protocol.

	If there is neither a SAP on this channel or a listening SAP then return NULL.
	**/
	{
	CRfcommSAP* mySAP=FindSAP(aDLCI);
	if(mySAP==NULL)
		{
		//	No SAP is currently in existance for this DLCI. 
		//	Find a listening SAP if possible.
		TBTSockAddr addr;
		addr.SetBTAddr(iRemoteAddr);
		addr.SetPort(aDLCI>>1);	//	i.e. convert from DLCI to server channel
		mySAP=iProtocol.FindIdleSAP(addr);
		}

	return mySAP;
	}

void CRfcommMuxer::EnqueFrame(CRfcommFrame* aFrm)
	/**
	   Add this frame to the outbound Q.

	   Exactly where the frame is added depends on the type of frame,
	   frames where Priority==ETrue are added just behind any other 
	   priority frames in the queue, others are added right at the end.
	**/
	{
	//FC
	if(!aFrm->Priority())
		{
		//	Not a high priority frame. To the back of the entire queue!
		iOutboundQ.AddLast(*aFrm);
		}
	else
		{
		//	A priority frame. Jump to the back of the priority queue!
		
		//	First, we need to find the back of the priority queue...
		TDblQueIter<CRfcommFrame> iter(iOutboundQ);
		while(iter && (*iter).Priority())
			{
			iter++;
			}

		if(iter)
			{
			//	We found a non-priority frame - jump into queue before this one
			aFrm->iLink.AddBefore(&(*iter).iLink);
			}
		else
			{
			//	We ran out of frames, so either entire Q is made up of priority
			//	frames, or the Q is empty. Either way, we can add to the end
			//	of the queue.
			iOutboundQ.AddLast(*aFrm);
			}
		}
	iOutboundQLength++;
	TryToSend();
	}

void CRfcommMuxer::TryToSend()
	/**
		Just Q an async callback
	**/
	{
	iSendCallback->CallBack();
	}
		
TInt CRfcommMuxer::TryToSendCallbackStatic(TAny* aMux)
	{
	(static_cast<CRfcommMuxer*>(aMux))->TryToSendCallback();
	return EFalse;
	}

#ifdef __FLOG_ACTIVE
void CRfcommMuxer::LogMuxCommand(CRfcommSAP* aSAP, CRfcommMuxer* /*aMux*/, TUint8 aCommand)
	{
	TUint8 signals;

	switch(aCommand)
		{
	case KTestType:
			LOG(_L("TestCommand")); 
			break;
		case KPNType:
			LOG(_L("PN"));//
			break;
		case KRPNType:
			LOG(_L("RPN"));//
			break;
		case KFConType:
			//may need length byte value
			LOG(_L("FcOn"));//
			break;
		case KFCoffType:
			LOG(_L("FcOff"));//
			break;
		case KMSCType:
			if (aSAP)
				{
				signals = aSAP->Signals();//V.24 signals in MSC
				LOG1(_L("MSC %d"),signals);//
				}
			else
				{
				LOG(_L("RFCOMM: Unable to get signals for KMSCType command, unknown SAP"));
				}
			break;
		case KNSCType:
			LOG(_L("NSC"));//
			break;
		case KRLSType:
			LOG(_L("RLS"));//
		};

	}

void CRfcommMuxer::ExplainOutgoingFrame(CRfcommFrame* aFrm, CRfcommMuxer* aMux )
	{
#ifndef TCI
	//Show what type of frame we have sent and the various parts of the frame which are important.

	TInt frametype = aFrm->Type(); //Is it a Ctrl Frame, Data Frame, CreditDataFrame or Mux Ctrl Frame ?
	TUint8 ctrlfield = aFrm->Ctrl();
	TUint8 ctrl = ctrlfield&~KPollFinalBitmask; //tells whether SABM, DISC, UA, DM
	TUint8 addressfield = aFrm->Address(); //Contains EA, CR, DLCI
//	TUint8 dlci = aMux->DecodeDLCI(addressfield); //extracts dlci
	TBool EA = addressfield & KEABitmask; //Is the EA bit set? 
	TBool CR = addressfield & KCRBitmask; //Is the CR bit set?
	TBool poll = ctrlfield & KPollFinalBitmask; //Is the p/f bit set?
	
	switch(frametype)
		{
		case KCtrlFrameType:
			{
			//CRfcommCtrlFrame* ctrlfrm=static_cast<CRfcommCtrlFrame*>(aFrm);
			//TUint16 ctrlframelength = ctrlfrm->DataLength();
			LOG(_L("Tx:"));
			
			if(ctrl==KSABMCtrlField)
				{
				LOG(_L("Tx: SABM"));
				}
			if(ctrl==KUACtrlField)
				{
				LOG(_L("Tx: UA"));
				}
			if(ctrl==KDMCtrlField)
				{
				LOG(_L("Tx: DM"));
				}
			if(ctrl==KDISCCtrlField)
				{
				LOG(_L("Tx: DISC"));
				}
			}
			break;
		case KDataFrameType: //CRfCommUIHFrame
			{
			CRfcommUIHFrame* uihfrm=static_cast<CRfcommUIHFrame*>(aFrm);
			TUint16 uihframelength = uihfrm->DataLength();

			if(poll)
				LOG(_L("Tx: UIH credit data frame"));
			
			if (uihframelength<=127)
				{
				LOG(_L("Tx: UIH simple data frame"));
				}
			else
				{
				LOG(_L("Tx: UIH simple data frame"));
				}
			}
			break;
		case KCreditDataFrameType: //CRfcommCreditDataFrame
			{
			CRfcommCreditDataFrame* creditfrm=static_cast<CRfcommCreditDataFrame*>(aFrm);
			TUint8 credits = creditfrm->Credit();
			//BLOG Credits
			TUint16 length = creditfrm->DataLength();
			
			if (length<=127)
				{
				LOG1(_L("Tx: Short UIH credit data frame with %d credits"), credits);
				}
			else
				{
				LOG1(_L("Tx :Long UIH credit data frame with %d credits"), credits);
				}
			}
			break;
		case KMuxCtrlFrameType: //CRfcommMuxCtrlFrame containing muxer messages/commands
			{
			//#ifndef TCI
			CRfcommMuxCtrlFrame* muxfrm=static_cast<CRfcommMuxCtrlFrame*>(aFrm);
			TUint8 muxdlci = muxfrm->iDLCI;
			CRfcommSAP* sap = aMux->FindSAP(muxdlci); //Find the SAP that is on this dlci for aMux 
			TUint8 command = muxfrm->CommandType(); 
			if (sap)
				{
				LogMuxCommand(sap, aMux, command);
				}
			else
				{
				LOG1(_L("RFCOMM: Outgoing frame for unknown SAP: command = %d"),command);
				LogMuxCommand(NULL, aMux, command);
				}
			//#endif
			}
			break;
		default: //CRfcommDataFrame
			break;
		};
	LOG1(_L("         P/F bit = %d"), poll);
	LOG1(_L("         EA  bit = %d"), EA);
	LOG1(_L("         C/R bit = %d"), CR);

	LOG1(_L("Frame of type %d"), frametype);
#endif
	}
#endif	//FLOG_ACTIVE

void CRfcommMuxer::TryToSendCallback()
	/**
	   Attempt to send a frame off the bottom of the Q.

		This is called back via an Async callback to break 
		"up to the top and down to the bottom from the RunL"	

	   Currently assumes that a frame will always fit in an L2CAP
	   packet.
	**/
	{
	LOG1(_L("RFCOMM: Send Callback with Q len %d"), iOutboundQLength);
	CRfcommFrame* frm;

	__ASSERT_DEBUG(!(iOutboundQLength !=0 && iOutboundQ.IsEmpty()),User::Panic(_L("Debug"),1));

	while	(	!iOutboundQ.IsEmpty() && !L2CAPBlocked() && 
				((iOutboundQ.First()->Priority()) || ClearToSend())	)
		//	i.e. We have frames to send, L2CAP isn't blocked and we are EITHER clear to send
		//	OR we have priority frames to send.
		{
		LOG1(_L("RFCOMM: Sending @ Q len %d"), iOutboundQLength);
		frm=iOutboundQ.First();
		
		FTRACE(ExplainOutgoingFrame(frm, this));
#ifdef _DEBUG
		//FOR DEBUG PURPOSES ONLY
		if(frm->Address() != KMuxDLCI)
			{
			CRfcommSAP* sap = FindSAP(DecodeDLCI(frm->Address()));
			if(sap)
				{
				switch(frm->Ctrl())
					{
				case KUIHCtrlField:
				case KUIHCBFCCtrlField:
					if(frm->DataLength())
						{
						iDataFramesSent++;
						LOG1(_L("RFCOMM: Total data frames transmitted by mux (=> caused Tx decrement): %d"), iDataFramesSent);
						}
					break;
				default:
					break;
					}
				}
			}
#endif

		// Datagram send, so 0 or length of packet returned

		if(!iBoundSAP->Write(frm->Data(), 0))
			{
			LOG(_L("RFCOMM: L2CAP send failed, restoring frame"));
			L2CAPBlocked(ETrue);
			}
		else
			{
			// Managed to send this packet, so either move to responseQ or delete
			frm->iLink.Deque();
			if(frm->ResponseNeeded())
				{
				iResponseQ.AddLast(*frm);
				frm->QueResponseTimer();
				}
			else
				{
				delete frm;
				}
			iOutboundQLength--;
			}
		}
	
	// We may have made room for some new sap to send, so let them know
	SignalSAPsCanSend();
	
	// Equally, we may have sent all the remaining queued frames, and have no SAPs
	// left to service.
	CheckForIdle();
	}


void CRfcommMuxer::SignalSAPsCanSend()
	/**
	   We may be able to let some saps send, so start signalling them
	**/
	{
	CRfcommSAP* sap;
	TDblQueIter<CRfcommSAP> iter(iBlockedSAPs);

	while(iter && !L2CAPBlocked() && ClearToSend())
		{
		sap = iter++;
		SetSendBlocked(*sap, EFalse);
		sap->CanSend();
		}
	}

void CRfcommMuxer::CheckForIdle(TBool aClosing)
	/**
	   Check to see if we're still needed.  If not, Q a delayed delete.
	**/
	{
	if (aClosing)
		{
		// If we could be in the position where we have no SAPs then the next
		// time we set the idle timer we should use the closing value.
		iMuxIdleTimeout = KRfcommMuxIdleTimeoutClosing;
		}
		
	if(iOutboundQLength==0 && iSAPs.IsEmpty() && iBlockedSAPs.IsEmpty())
		{
		QueIdleTimer();
		}
	}

void CRfcommMuxer::QueIdleTimer()
	/**
	   Queues the idle timer if necessary
	**/
	{
	if(!iIdleTimerQueued)
		{
		LOG2(_L("RFCOMM: Mux 0x%08x Q idle timer for %d microsecs"), this, iMuxIdleTimeout);
		iIdleTimerQueued=ETrue;
		BTSocketTimer::Queue(iMuxIdleTimeout, iIdleTimerEntry);
		}
	}

void CRfcommMuxer::DequeIdleTimer()
	/**
	   Deques idle timer if necessary
	**/
	{
	if(iIdleTimerQueued)
		{
		LOG1(_L("RFCOMM: Mux 0x%08x deQ idle timer"), this);
		BTSocketTimer::Remove(iIdleTimerEntry);
		iIdleTimerQueued=EFalse;
		}
	}

TInt CRfcommMuxer::IdleTimerExpired(TAny* aMux)
	/**
	   Static idle callback.

	   This is entered after all our saps have gone away.
	   
	**/
	{
 	LOG1(_L("RFCOMM: Mux %08x idle timer expired"), aMux);
	// Start the delete process
	CRfcommMuxer* mux = static_cast<CRfcommMuxer*>(aMux);
	mux->iIdleTimerQueued=EFalse;  // Obviously...

	__ASSERT_DEBUG(mux->iSAPs.IsEmpty() && mux->iBlockedSAPs.IsEmpty(),
		           Panic(ERfcommIdleTimeoutWhenNotIdle));
	
	mux->iMuxChannel->Close();	//	May call back syncronously or asynchronously
								//	into MuxChannelClosed
	return FALSE;
	}

void CRfcommMuxer::MuxChannelClosed()
	{
	// remove this muxer
	iProtocol.MuxDown(*this);	
	}

void CRfcommMuxer::DeleteQueuedFrames()
	/**
		Delete all frames that are on either Q.
	**/	
	{
	TDblQueIter<CRfcommFrame> iter(iOutboundQ);
	CRfcommFrame* frm;
	
	while(iter)
		{
		frm=iter++;
		delete frm;
		}
	iOutboundQLength=0;
	
	iter=iResponseQ;
	while(iter)
		{
		frm=iter++;
		delete frm;
		}
	}

