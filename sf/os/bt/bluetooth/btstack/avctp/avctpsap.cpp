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
// Implements the code for AVCTP SAPs
// 
//

/**
 @file
 @internalTechnology
*/

#include <bluetooth/logger.h>
#include <es_mbuf.h>
#include "Avctp.h"
#include "avctpsap.h"
#include "avctpmuxer.h"
#include "avctpcommon.h"
#include "avctputils.h"
#include "avctppacket.h"
#include "avctpPacketMgr.h"
#include "AvctpMessageParameters.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

#ifdef _DEBUG
PANICCATEGORY("avctpsap");
#endif

using namespace SymbianAvctp;

/** 
Avctp Sap Security policy  
We don't need to check LocalServices as BluetoothSAP does that for us
*/
_LIT_SECURITY_POLICY_S0(KAvctpSapSecurityPolicy, KDefaultSidforPids);

/** Diagnostic string identifying this module when calling security policy checking methods,
in builds without platsec diagnostics this will be NULL */
const char * const KAvctpSapSecurityPolicyDiagnostic = __PLATSEC_DIAGNOSTIC_STRING("Avctp Sap");

/**
Static AVCTP SAP factory function

  @internalComponent
  @leave KErrNoMemory if the SAP object could not be allocated
  @param aProt Lower protocol object
  @return A pointer to the new SAP
*/
CAvctpSap* CAvctpSap::NewL(CAvctpProtocol& aProt)
	{
	LOG_STATIC_FUNC

	CAvctpSap* sap = new(ELeave) CAvctpSap(aProt);
	CleanupStack::PushL(sap);
	sap->ConstructL();
	CleanupStack::Pop(sap);
	return sap;
	}

/**
Second-phase construction of a SAP

Set up the async. callback.

  @internalComponent
*/
void CAvctpSap::ConstructL()
	{
	LOG_FUNC

	CBluetoothSAP::ConstructL();

	TCallBack cb(SendAsyncCallBack, this);
	iSendAsyncCallBack = new(ELeave) CAsyncCallBack(cb, EActiveHighPriority);
	}

/**
Constructor for an AVCTP SAP

  @internalComponent
  @param aProt The AVCTP protocol object
*/
CAvctpSap::CAvctpSap(CAvctpProtocol& aProtocol)
	: CBluetoothSAP(aProtocol.SecMan(), aProtocol.CodMan()),
	  iProtocol(aProtocol),
	  iOutgoingSdus(),
	  iIncomingSdus(_FOFF(HAvctpIncomingSdu, iQueLink)),
	  iChannel(KAvctpInvalidChannel),
	  iIsInList(EFalse)
	{
	LOG_FUNC
	// we set the channel because "0" is a real channel, so we want to know that iChannel is unset at time of construction
	}


/**
Destructor for an AVCTP SAP

Called when ESock deletes the SAP, when user app calls RSocket::Close()

  @internalAll
*/
CAvctpSap::~CAvctpSap()
	{
	LOG_FUNC
	
	// Delete any remaining packets that the sap owns
	TDblQueIter<HAvctpOutgoingSdu> outgoingIter = iOutgoingSdus.Iter();
	while (outgoingIter)
		{
		delete outgoingIter++; // deques sdu
		}
	TDblQueIter<HAvctpIncomingSdu> incomingIter(iIncomingSdus);
	while (incomingIter)
		{
		delete incomingIter++; // deques sdu
		}	
	
	iSendAsyncCallBack->Cancel();
	delete iSendAsyncCallBack;
	}

MSocketNotify& CAvctpSap::Socket() const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSocket, Panic(EAvctpDataSAPNullSocket));
	return *iSocket;
	}

/****************************************************************************/
/* Implementation of CControlledServProvider methods */
/****************************************************************************/


/**
Set the local name to anAddr.

@internalAll
@param anAddr The address with the PID
@return KErrNone if successful
*/
TInt CAvctpSap::SetLocalName(TSockAddr& aAddr)
	{
	LOG_FUNC

	TAvctpSockAddr avctpAddr = TAvctpSockAddr::Cast(aAddr);
	iChannel = avctpAddr.Channel(); 
	__ASSERT_DEBUG(iChannel == KAvctpPrimaryChannel || iChannel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannel));
	
	TInt err = KErrNone;
	if (iChannel != KAvctpPrimaryChannel && iChannel != KAvctpSecondaryChannel)
		{
		err = KErrInvalidChannel;
		}
	if (err == KErrNone)
		{
		err = SetPid(avctpAddr.Pid());
		}
	if (err == KErrNone)
		{
		err = iProtocol.AddSap(*this);
		}
	return err;
	}
	
/**
Read the Local Name into anAddr.

AVCTP knows only the PID on which the device is
registered but this is OK since that's all they want.

  @internalAll
  @param anAddr The address to read into
*/
void CAvctpSap::LocalName(TSockAddr& aAddr) const
	{
	LOG_FUNC

	// Copy iPid into TSockAddr and return
	TAvctpSockAddr avctpAddr(aAddr);
	avctpAddr.SetChannel(iChannel);
	avctpAddr.SetPid(iPid);
	aAddr = avctpAddr;
	}

/**
Read the remote name into anAddr.

AVCTP knows only the PID on which the device is
registered but this is OK since that's all they want.

  @internalAll
  @param anAddr The address to read into
*/
void CAvctpSap::RemName(TSockAddr& aAddr) const
	{
	LOG_FUNC
	// no BT Addr to fill on for data saps :-)
	TAvctpSockAddr avctpAddr(aAddr);
	avctpAddr.SetPid(iPid);
	aAddr = avctpAddr;   // Convert back
	}

/**
Set the remote name to anAddr.

  @internalAll
  @param anAddr The address to set
*/
TInt CAvctpSap::SetRemName(TSockAddr& aAddr)
	{
	LOG_FUNC

	// Copy this over
	TAvctpSockAddr avctpAddr = TAvctpSockAddr::Cast(aAddr);

	iChannel = avctpAddr.Channel();
	__ASSERT_DEBUG(iChannel == KAvctpPrimaryChannel || iChannel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannel));
	
	TInt err = KErrNone;
	if (iChannel != KAvctpPrimaryChannel && iChannel != KAvctpSecondaryChannel)
		{
		err = KErrInvalidChannel;
		}
	
	if (err == KErrNone)
		{
		err = SetPid(avctpAddr.Pid());
		}

	return err;
	}

/**
Auto bind from ESock.

We're not expecting this as a connectionless protocol so panic

  @internalAll
*/
void CAvctpSap::AutoBind()
	{
	LOG_FUNC
	__PANIC_UNEXPECTED_CALL
	}

/**
Get a socket option.

AVCTP doesn't support socket options, so fail it if it's for AVCTP.

  @internalAll
  @param aLevel The socket option level
  @param aName The socket option name
  @param aOption The socket option data
*/
TInt CAvctpSap::GetOption(TUint /*aLevel*/, TUint /*aName*/, TDes8& /*aOption*/) const
	{
	LOG_FUNC
	
	return KErrNotSupported;
	}

/**
Set a socket option. (BluetoothSAP version)

AVCTP doesn't support socket options, so fail it if it's for AVCTP.

  @internalAll
  @param aLevel The socket option level
  @param aName The socket option name
  @param aOption The socket option data
*/
TInt CAvctpSap::SAPSetOption(TUint /*aLevel*/, TUint /*aName*/, const TDesC8& /*aOption*/)
	{
	LOG_FUNC
	return KErrNotSupported;
	}


// start-up/Shutdown

// NB: All ActiveOpen / PassiveOpen / Start calls require a connection-
//     oriented socket, so we just panic!

/**
Active open an AVCTP socket...

Wait a minute!  AVCTP is connectionless, so panic!

  @internalAll
*/
void CAvctpSap::ActiveOpen()
	{
	LOG_FUNC
	Panic(EActiveOpenNotSupported); // control sap does this
	}

/**
Active open an AVCTP socket (data overload)...

Wait a minute!  AVCTP is connectionless, so panic!

  @internalAll
  @param aConnectionData Data to send on connection
*/
void CAvctpSap::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	Panic(EActiveOpenNotSupported);
	}

/**
Passive open an AVCTP socket...

Wait a minute, passive opens are only for connection-oriented protocols too! Panic.

  @internalAll
  @param aQueSize How many connections to complete before they are Accept()ed...
  @return KErrNotSupported, obviously...
*/
TInt CAvctpSap::PassiveOpen(TUint /*aQueSize*/)
	{
	LOG_FUNC
	Panic(EPassiveOpenNotSupported);
	return KErrNotSupported;
	}

/**
Passive open an AVCTP socket (data overload)...

Wait a minute, passive opens are only for connection-oriented protocols too! Panic.

  @internalAll
  @param aQueSize How many connections to complete before they are Accept()ed...
  @param aConnectionData Data to send on connection
  @return KErrNotSupported, obviously...
*/
TInt CAvctpSap::PassiveOpen(TUint /*aQueSize*/, const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	Panic(EPassiveOpenNotSupported);
	return KErrNotSupported;
	}

/**
We ignore this.

  @internalAll
*/
void CAvctpSap::Start()
	{
	LOG_FUNC
	}

/**
Close the SAP down.

  @internalAll
  @param aCloseType How fast we're going down
*/
void CAvctpSap::Shutdown(TCloseType aCloseType)
	{
	LOG_FUNC
	LOG1(_L("option = %d"), aCloseType);

	if (aCloseType != EImmediate)
		{
		// We need to remember we've been asked to close so:
		iIsClosing = ETrue;
		
		// Normal Shutdown - ESOCK will wait for us to say we can end		
		// Esock is no longer interested in our incoming packets so delete them
		TDblQueIter<HAvctpIncomingSdu> incomingIter(iIncomingSdus);
		while (incomingIter)
			{
			delete incomingIter++; // deques sdu
			}	
			
		CheckForCanClose();	
		}
	else
		{
		// Note: Esock doesn't require us to call CanClose on an EImmediate Shutdown
		iClosed = ETrue;
		
		iProtocol.RemoveSap(*this);
		}
	}

/**
Close the SAP down (data overload).

  @internalAll
  @param aCloseType How fast we're going down
  @param aDisconnectionData Data to send on disconnect
*/
void CAvctpSap::Shutdown(TCloseType /*aCloseType*/, const TDesC8& /*aDisconnectionData*/)
	{
	LOG_FUNC
	Panic(EDisconnectDataNotSupported);
	}

/**
Send some data.

Just pass this over to our state.

  @internalAll
  @param aDesc The data to send
  @param aOptions The message parameters
  @param aAddr The address to send to
  @return 0 if the write failed or 1 if we took ownership of the data
*/
TInt CAvctpSap::Write(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr)
	{
	LOG_FUNC

	// We've been asked to close so esock shouldn't be giving us any more data
	__ASSERT_ALWAYS(!iIsClosing, Panic(EUnexpectedEsockEvent));
	
	TUint ret = 0;
	
	if (iOutgoingSdus.Count() < KSapOutboundQHighMark)
		{
		TAvctpMessageParameters parameters(aOptions);
		TAvctpNormalHeaderInfo headerInfo = TAvctpNormalHeaderInfo(
												parameters.iTransaction, 
												ENormalPkt, 
												parameters.Type(),
												ETrue, // hasValidPid
												Pid());
												
		HAvctpOutgoingSdu* sdu = NULL;
		
		TRAPD(err,sdu = HAvctpOutgoingSdu::NewL(headerInfo, 
												TAvctpSockAddr(*aAddr).BTAddr(),
												aData));
		if (err == KErrNone)
			{
			iOutgoingSdus.Insert(*sdu);
			StartSendAsyncCallBack();
			ret = 1; //one sdu
			}
		}

	if (!ret)
		{
		iSendBlocked = ETrue;
		iPendingSendAddress = TAvctpSockAddr::Cast(*aAddr).BTAddr();
		}
	return ret;
	}
	
/**
This function is called on the Sap by Esock when it wants to receive the
data we told it about in NewData upcalls from us.

Rather than letting Esock call the descriptor overload, this function
deals with the request directly for two reasons. The intention is to 
implement AVCTP in terms of MBufs in future but also because the descriptor 
overload doesn't get called correctly by esock since it doesn't know how
long our datagrams will be and so presents us with a TDes8 which is too 
short.

  @internalAll
  @param aData The data to be received
  @param aOptions The message parameters
  @param aAddr The address the data is received from
  @return the error associated with the read. Could be KErrNoMBufs (a likely case) in which case esock 
  		   would attempt an allocation for us and would then try again. If it's some other error then it's up to us 
  		   to tell esock when to retry (by signalling with NewData(0)). If it was some error that we couldn't
  		   handle at all then we should have set the error upon the socket  	
*/
TInt CAvctpSap::GetData(RMBufChain& aData, TUint /*aLength*/, TUint /*aOptions*/, TSockAddr* aAddr)
	{
	LOG_FUNC

	// We've been asked to close so esock shouldn't be asking us for more data
	__ASSERT_ALWAYS(!iIsClosing, Panic(EUnexpectedEsockEvent));
	__ASSERT_DEBUG(!iClosed, Panic(EAvctpSapClosed));
	
	HAvctpIncomingSdu* sdu = iIncomingSdus.First();
	__ASSERT_ALWAYS(sdu, Panic(ENullPacket));
	
	aData.Assign(const_cast<RMBufChain&>(sdu->Data()));
	
	if (aAddr)
		{
		TAvctpSockAddr avctpAddr = TAvctpSockAddr::Cast(*aAddr);
		avctpAddr.SetPid(iPid);
		avctpAddr.SetBTAddr(sdu->BTAddr());
		avctpAddr.SetChannel(iChannel);
		(*aAddr) = avctpAddr;   // Convert back
		}	
	delete sdu; // deques pkt in ~HAvctpIncomingSdu				
	
	return KErrNone;	
	}

/**
Perform an Ioctl.

  @internalAll
  @param aLevel The Ioctl level
  @param aName The Ioctl name
  @param aOption The Ioctl data
*/
void CAvctpSap::Ioctl(TUint /*aLevel*/, TUint /*aName*/, TDes8* /*aOption*/)
	{
	LOG_FUNC
	Socket().Error(KErrNotSupported, MSocketNotify::EErrorIoctl);
	__ASSERT_DEBUG(NULL, Panic(EIoctlsNotSupported));
	}

/**
Cancel an Ioctl.

Just pass it to the state machine.

  @internalAll
  @param aLevel The Ioctl level
  @param aName The Ioctl name
*/
void CAvctpSap::CancelIoctl(TUint /*aLevel*/, TUint /*aName*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(NULL, Panic(EIoctlsNotSupported));
	}
	
TInt CAvctpSap::SecurityCheck(MProvdSecurityChecker* aSecurityChecker)
	{
	LOG_FUNC

	// Store for later use when asked to bind to a particular PID.
	iSecurityChecker = aSecurityChecker;
	return KErrNone;
	}

/************************************************************************

  Events interface (between muxer and Sap via Protocol)

 ************************************************************************/
																			
/**
There is new data from the muxer for us.
We take ownership if the packet is suitable for us otherwise ignore

  @internalComponent
  @param aData The new data from the muxer
  @param aMux The mux this data came from
  @return ETrue if we took ownership of the data, or EFalse otherwise
*/

TBool CAvctpSap::NewData(HAvctpIncomingSdu* aSdu, TInt aChannel)
	{
	LOG_FUNC
	LOG1(_L("New Sdu 0x%08x"), aSdu);	
	
	__ASSERT_DEBUG(!iClosed, Panic(EAvctpSapClosed));
	
	TBool acceptsPdu = EFalse;

	if (SymbianAvctp::Pid(CAvctpPacket::GetHeader(aSdu->Data())) == Pid()
					&& aChannel == iChannel)
				{
				if (!iIsClosing)
					{
					iIncomingSdus.AddLast(*aSdu);
					Socket().NewData(1);
					acceptsPdu = ETrue;
					}
				}

	return acceptsPdu;
	}
	



TBool CAvctpSap::HasDataFor(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC

	TBool ans = EFalse;
	
	TDblQueIter<HAvctpOutgoingSdu> iter = iOutgoingSdus.Iter();
	HAvctpOutgoingSdu* sdu;

	while (iter)
		{
		sdu = iter++;
		if (sdu->BTAddr() == aRemoteAddr)
			{
			ans = ETrue;
			break;
			}
		}

	LOG1(_L("result %d"), ans);
	return ans;
	}

/**
This function is called when a muxer goes down and is used to get rid of
any packets that are now stale. If a packet remains in the system after 
the muxer to it's remote device goes down, it'll cause a reconnection
attempt which we don't want.

We also need to delete any partial incoming packets from this muxer since
they obviously won't be competed.
*/
void CAvctpSap::MuxerDown(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC

	HAvctpOutgoingSdu* sdu;

	TDblQueIter<HAvctpOutgoingSdu> iter = iOutgoingSdus.Iter();
	while (iter)
		{
		sdu = iter++;
		if (sdu->BTAddr() == aRemoteAddr)
			{
			delete sdu;
			}
		}
		
	if (!iIsClosing && iSendBlocked && iPendingSendAddress == aRemoteAddr)
		{
		Socket().Error(KErrDisconnected, MSocketNotify::EErrorSend);
		}
		
	CheckForCanSend();
	CheckForCanClose();
	}

/****************************************************************************/
/* Implementation of private methods */
/****************************************************************************/

/**
This function checks the Secure ID of the client process that is trying to 
use an RAvctp object on aPid. Currently all PIDs are checked against the 
default SID provided in avctpconstants.h

If a licensee wanted to change behaviour, they would have to branch the stack
to do so. This is obviously undesirable though they should question why they
aren't using the RemCon server in the first place.

This function should only be called once when the Pid of the Sap is first set.

TODO: provide a resource / SPD file matching PIDs <-> SIDs, using KRemConSrvUid as the default SID
*/
TInt CAvctpSap::CheckPidAllowed(TPid /*aPid*/)
	{
	LOG_FUNC
	// iSecurityChecker should've been provided in SecurityCheck by now
	ASSERT_DEBUG(iSecurityChecker);
	TBool ret = iSecurityChecker->CheckPolicy(KAvctpSapSecurityPolicy, KAvctpSapSecurityPolicyDiagnostic);
	iSecurityChecker = NULL; // we're finished with it and it's not owned by us
	return ret;
	}

void CAvctpSap::CheckForCanSend()
	{
	LOG_FUNC

	if (!iIsClosing && iOutgoingSdus.Count() < KSapOutboundQLowMark)
		{
		Socket().CanSend(); // Okay to call at any time from Esock's point of view
		iSendBlocked = EFalse;
		}	
	}

void CAvctpSap::CheckForCanClose()
	{
	LOG_FUNC

	if ((iIsClosing && iOutgoingSdus.Count() == 0) && !iClosed)
		{
		iClosed = ETrue;	// Ensure CanClose() is only called once, otherwise it hits an assert
		iProtocol.RemoveSap(*this);
		Socket().CanClose();
		}
	}
		
/**
Asynchronous callback function to deal with actually sending stuff to 
the muxers for delivery.

Attempts to send all packets that it can from the iOutgoingSdus Q

  @internalComponent
  @param aSap The SAP being called back
  @return EFalse - i.e. the callback should not be called again
*/
/*static*/ TInt CAvctpSap::SendAsyncCallBack(TAny* aSap)
	{
	LOG_STATIC_FUNC
	LOG1(_L("for Sap: 0x%08x"), aSap);
	
	CAvctpSap& sap = *static_cast<CAvctpSap*>(aSap);
	
	TDblQueIter<HAvctpOutgoingSdu> iter = sap.iOutgoingSdus.Iter();
	HAvctpOutgoingSdu* sdu = NULL;
	MAvctpSDUSender* sduSender = NULL;
	
	while (iter)
		{
		sdu = iter++;
		const TBTDevAddr& addr = sdu->BTAddr();
		// Get a SDU Sender (tied to transport) if we don't have one yet
		if (!sduSender)
			{
			sduSender = sap.iProtocol.GetSDUSender(addr, sap.iChannel);
			}
		
		if (!sduSender)	// means the muxer has not been created 
			{
			delete sdu;	// I cannot send it due to a muxer creation failure
			continue;
			}
		
		// Try to send the sdu
		if (sduSender->IsClearToSend(sap.iChannel))
			{
			// Note if the write succeeds the ownership of the sdu is transferred and the sdu will
			// be deleted which will deque the sdu from the sap's outboundQ. If the write fails
			// the sap retains ownership and the packet will remain on the outboundQ.
			// Hence we don't need to check the return value
			
			(void)sduSender->Write(sdu, sap.iChannel);
			}
		else // may as well skip all sdus that are for the same transport
			{
			while (iter)
				{
				sdu = iter++;
				if (sdu->BTAddr() != addr)
					{
					iter--; // don't want to skip this sdu when we go round the outer loop again
					sduSender = NULL; // we're finished with this muxer
					break; // inner while
					}
				}
			}
		}

	sap.CheckForCanSend();
	sap.CheckForCanClose();

	return EFalse;
	}

/**
Set the PID for this SAP & add us to the Protocol's Q

  @internalComponent
  @param aPid The SAP's PID
  @return KErrInUse if there is already a Sap on aPid, otherwise KErrNone
*/
TInt CAvctpSap::SetPid(TPid aPid)
	{
	LOG_FUNC
	LOG1(_L("to PID: 0x%x"), aPid);

	__ASSERT_DEBUG(iPid == 0 || iPid == aPid, Panic(ESapAlreadyBound));
	
	TInt err = KErrNone;
	if (iPid == 0 || iPid == aPid)
		{
		err = CheckPidAllowed(aPid);
		if (err == KErrNone)
			{
			iPid = aPid;
			}
		}
	else
		{
		err = KErrSapAlreadyBound;
		}

	return err;
	}	


// Pure virtuals From MSocketNotify
// It's not used in favour of the public overloaded NewData()
void CAvctpSap::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(NULL, Panic(ENewDataNotSupported));
	}

/**
Notification from a muxer that we can send again.
*/
void CAvctpSap::CanSend()
	{
	LOG_FUNC
	StartSendAsyncCallBack();
	}

// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::ConnectComplete()
	{
	LOG_FUNC
	}

// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	}

// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	}
	
// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	}
	
// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	}

// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	}
	
void CAvctpSap::Error(TInt aError, TUint aOperationMask)
	{
	iSocket->Error(aError, aOperationMask);
	}

// Not implemented because CAvctpSap is a connectionless sap	
void CAvctpSap::Disconnect()
	{
	LOG_FUNC
	}

// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	}

// Not implemented because CAvctpSap is a connectionless sap
void CAvctpSap::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	// if we forward ioctls (NO!) then we'd expect this
	}

void CAvctpSap::NoBearer(const TDesC8& /*aConnectionInfo*/) {}
void CAvctpSap::Bearer(const TDesC8& /*aConnectionInfo*/) {}

CAvctpControlSAP* CAvctpControlSAP::NewL(CAvctpProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	return new (ELeave) CAvctpControlSAP(aProtocol);
	}

CAvctpControlSAP::CAvctpControlSAP(CAvctpProtocol& aProtocol)
:CBluetoothSAP(aProtocol.SecMan(), aProtocol.CodMan())
,iProtocol(aProtocol),
iIndicationQueue(_FOFF(HQueuedIndication, iLink))
	{
	LOG_FUNC
	}

/**
 This method is called by CAvctpControlSAP::Ioctl() when the aName is an active request
 (i.e. an attach request, a detach request, for both channels). In this case we want to set
 an oustanding ioctl to be completed later. And it has priority on other incoming events, so we
 want it to be served first. We can have only one oustanding ioctl. This method creates it and put 
 it at first in the queue.
 **/
void CAvctpControlSAP::SetOutstandingIndication(const TControlIoctlMessage& aMessage)
	{
	LOG_FUNC
	HQueuedIndication* ind = HQueuedIndication::New(aMessage);
	if (ind)
		{
		iIndicationQueue.AddFirst(*ind);
		}
	else
		{
		Error(KErrNoMemory);
		}
	}

void CAvctpControlSAP::CompleteIoctlWithError(const TControlIoctlMessage& aMessage)
	{
	LOG_FUNC
	TPckgC<TControlIoctlMessage> pck(aMessage);
	iSocket->IoctlComplete(&pck);
	ClearIoctl();
	}

/**
 In some cases the Ioctl (i.e. an AttachAgreement) does not wait for a reply. So, 
 AvctpRemoteDevices, after sending an AgreeAttachment, cancels the ioctl and submit a new Listen one.
 Hence, we need to queued the error completion, and when the listening ioctl comes we complete it with
 the queued error. It is queued with priority as we want it to be the first completed.
 */
void CAvctpControlSAP::QueueErrorIoctlComplete(const TControlIoctlMessage& aMessage)
	{
	SetOutstandingIndication(aMessage);
	}

/**
 Dequeues an iocl and if it is an oustanding iocl (and not an incoming indication) then it 
 completes it with the error code passed on.
 In some cases the outstanding ioctl will be already completed and hence not queued anymore.
 for example, if we are trying to attach to ourself (so the address passed in the attach request
 is the local device's address) the transport immediately notify an error up that completes the
 outstanding ioctl. Then the transport is destroyed, and so iProtocol.ProvideTransport returns an
 error. So this method is called but it won't find the outstanding ioctl because already completed
 but NotifyError(). In this case we do nothing as the client has been already notified.
 */
void CAvctpControlSAP::CompleteQueuedIoctlWithError(TInt aError)
	{
	LOG_FUNC
	if (!iIndicationQueue.IsEmpty())
		{
		HQueuedIndication* ind = iIndicationQueue.First();
		SymbianAvctp::TControlIoctls ioctl = ind->Indication().iIoctl;
		
		if (ioctl == EPrimaryChannelAttachToTransport || 
			ioctl == ESecondaryChannelAttachToTransport ||
			ioctl == EPrimaryChannelDetachFromTransport ||
			ioctl == ESecondaryChannelDetachFromTransport)
			{
			iIndicationQueue.Remove(*ind);
			ind->Indication().iError = aError;
			CompleteIoctlWithError(ind->Indication());
			delete ind;
			ind = NULL;
			}
			
		}
	}

/**
 This method process the queue and it complete the ioctl.
 In some cases it changes the indication type (i.e. from ELinkUp to EAttachIndicate).
 If the ioctl type is EError it doesn't do anything because we want the error to be propagated to 
 the client.
 */
void CAvctpControlSAP::ProcessIoctlQueue()
	{
	LOG_FUNC
	if (!iIndicationQueue.IsEmpty())
		{
		HQueuedIndication* ind = iIndicationQueue.First();
		iIndicationQueue.Remove(*ind);
		
		switch(ind->Indication().iIoctl)
			{
			case ELinkUp:
				ind->Indication().iIoctl = EAttachIndicate;
				break;
			case ELinkDown:
				ind->Indication().iIoctl = EDetachIndicate;
				break;
			default:
				break;	// propagate it
			}
		
		
		TPckgC<TControlIoctlMessage> pck(ind->Indication());
		iSocket->IoctlComplete(&pck);
		delete ind;
		ClearIoctl();
		}
	}

TBool CAvctpControlSAP::IsActiveRequest(TUint aName)
	{
	LOG_FUNC
	return (aName == EPrimaryChannelAttachToTransport || 
			aName == ESecondaryChannelAttachToTransport ||
			aName == EPrimaryChannelDetachFromTransport || 
			aName == ESecondaryChannelDetachFromTransport);
	}

void CAvctpControlSAP::Ioctl(TUint aLevel,TUint aName, TDes8* aOption)
	{
	LOG_FUNC
	
	TInt err = KErrNone;

	TControlIoctlMessage msg;
	TPckg<TControlIoctlMessage> pck(msg);
	pck.Copy(*aOption);
	
	TBTDevAddr addr = pck().iAddr.BTAddr();
	TPid pid = pck().iAddr.Pid();
	TInt channel = pck().iAddr.Channel();
	
	if (aLevel == KSolBtAVCTP)
		{
		iIoctlLevel = aLevel;
		iIoctlName = aName;
		
		if (IsActiveRequest(aName))
			{
			SetOutstandingIndication(pck());
			}
		
		switch (aName)
			{
			case EPrimaryChannelAttachToTransport:
				{
				__ASSERT_DEBUG(channel == KAvctpPrimaryChannel, Panic(EAvctpInvalidChannelIoctl));
				// we should complete the ioctl with an error if this function returns an error
				if ((err = iProtocol.ProvideTransport(addr, pid, *this)) != KErrNone)
					{
					CompleteQueuedIoctlWithError(err);
					}
				}
			break;
			case EAwaitProvidedTransport:
				{
				__ASSERT_DEBUG(channel == KAvctpPrimaryChannel, Panic(EAvctpInvalidChannelIoctl));
				iProtocol.AwaitTransport(pid, *this);
				}
			break;
			case EPrimaryChannelAgreeAttachment:
				{
				__ASSERT_DEBUG(channel == KAvctpPrimaryChannel, Panic(EAvctpInvalidChannelIoctl));
				LOG(_L("EPrimaryChannelAgreeAttachment, then reset aName"))
				if ((err = iProtocol.PrimaryChannelAgreeAttachment(pid, addr)) == KErrNone)
					{
					ClearIoctl();				// this ioctl is not expecting an answer
					iIoctlName = EUndefinedIoctl;	// just used to not execute the iIoctlName update
					}
				else
					{
					pck().iError = err;
					QueueErrorIoctlComplete(pck());
					}
				}
			break;
			case ESecondaryChannelAttachToTransport:
				{
				__ASSERT_DEBUG(channel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannelIoctl));
				// note this SAP is only managing the transport: the demuxing of control and second AVCTP data is handled
				// by having different data SAPs for each traffic flow
				// rare that we'd actively connect the second channel, but using at least for test purposes
				
				// should return the error through the confirm completion than via Error()?
				if ((err = iProtocol.ActiveExtendTransport(addr, pid, *this)) != KErrNone)
					{
					CompleteQueuedIoctlWithError(err);
					}
				}
			break;
			case ESecondaryChannelAgreeAttachment:
				{
				__ASSERT_DEBUG(channel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannelIoctl));
				if ((err = iProtocol.SecondaryChannelAgreeAttachment(pid, addr)) == KErrNone)
					{
					ClearIoctl();				// this ioctl is not expecting an answer
					iIoctlName = EUndefinedIoctl;	// just used to not execute the iIoctlName update
					}
				else
					{
					pck().iError = err;
					QueueErrorIoctlComplete(pck());
					}
				}
			break;
			case EAwaitExtendedTransport:
				{
				__ASSERT_DEBUG(channel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannelIoctl));
				iProtocol.AwaitForExtendedTransport(pid, *this);
				}
			break;
			case EPrimaryChannelRefuseAttach:
				{
				__ASSERT_DEBUG(channel == KAvctpPrimaryChannel, Panic(EAvctpInvalidChannelIoctl));
				iProtocol.PrimaryChannelRefuseAttach(addr, pid);
				}
			break;
			case ESecondaryChannelRefuseAttach:
				{
				__ASSERT_DEBUG(channel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannelIoctl));
				iProtocol.SecondaryChannelRefuseAttach(addr, pid);
				}
			break;
			case EPrimaryChannelDetachFromTransport:
				{
				__ASSERT_DEBUG(channel == KAvctpPrimaryChannel, Panic(EAvctpInvalidChannelIoctl));
				if ((err = iProtocol.ReleaseTransport(addr, pid)) != KErrNone)
					{
					CompleteQueuedIoctlWithError(err);		
					} 
				}
			break;
			case ESecondaryChannelDetachFromTransport:
				{
				__ASSERT_DEBUG(channel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannelIoctl));
				if ((err = iProtocol.ReleaseExtendedTransport(addr, pid)) != KErrNone)
					{
					CompleteQueuedIoctlWithError(err);		
					} 
				}
			break;
			default:
				{
				iSocket->Error(KErrNotSupported, MSocketNotify::EErrorIoctl);		
				}
			break;
			}
		
		if (err == KErrNone && iIoctlName != EUndefinedIoctl)
			{
			if (!IsActiveRequest(aName))
				{
				ProcessIoctlQueue();
				}
			}
		else
			{
			LOG(_L("No outstanding ioctl"));
			}
		}
	}

void CAvctpControlSAP::CancelIoctl(TUint/* aLevel*/,TUint /*aName*/)
	{
	LOG_FUNC
	// do nothing for now, though should in future inform protocol that a transport is not needed
	HQueuedIndication* ind = iIndicationQueue.First();
	if (ind && IsActiveRequest(ind->Indication().iIoctl))
		{
		iIndicationQueue.Remove(*ind);
		delete ind;
		ind = NULL;
		}
	ClearIoctl();
	}

TInt CAvctpControlSAP::SetRemName(TSockAddr& /*aAddr*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return KErrNone;
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::ActiveOpen()
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TInt CAvctpControlSAP::PassiveOpen(TUint /*aQueueSize*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return KErrNotSupported;
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TInt CAvctpControlSAP::PassiveOpen(TUint /*aQueueSize*/,const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return KErrNotSupported;
	}


// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented or left empty cause called by the framework
void CAvctpControlSAP::Start()
	{
	LOG_FUNC
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::LocalName(TSockAddr& /*aAddr*/) const
	{
	LOG_FUNC
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TInt CAvctpControlSAP::SetLocalName(TSockAddr& /*aAddr*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return KErrNotSupported;
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::RemName(TSockAddr& /*aAddr*/) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

TInt CAvctpControlSAP::GetOption(TUint aLevel,TUint aName, TDes8& aOption) const
	{
	LOG_FUNC
	TInt err = KErrNotSupported;
	
	TOptionMessage msg;
	TPckg<TOptionMessage> pck(msg);
	pck.Copy(aOption);
	
	TInt mtu;
	if (aLevel == KSolBtAVCTP)
		{
		switch(aName)
			{
			case KAvctpBaseOutboundMTU:
				{
				err = iProtocol.GetChannelMtu(KAvctpPrimaryChannel, pck().iAddr, mtu);
				break;
				}
			case KAvctpExtendOutboundMTU:
				{
				err = iProtocol.GetChannelMtu(KAvctpSecondaryChannel, pck().iAddr, mtu);
				break;
				}
			default:
				break;
			};
		}

	if (err == KErrNone)
		{
		pck().iMtu = mtu;
		}
	aOption = pck;
	return err;
	}

void CAvctpControlSAP::Shutdown(CServProviderBase::TCloseType aOption)
	{
	LOG_FUNC
	
	if (aOption != EImmediate)
		{
		
		iSocket->CanClose(); 
		}
	else
		{
		// If immediate don't need to call back
		}
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::Shutdown(CServProviderBase::TCloseType /*aOption*/, const TDesC8& /*aDisconnectionData*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::AutoBind()
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TUint CAvctpControlSAP::Write(const TDesC8& /*aDesc*/,TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return 0;
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TInt CAvctpControlSAP::GetData(RMBufChain& /*aData*/, TUint /*aLength*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return KErrNotSupported;
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TInt CAvctpControlSAP::SecurityCheck(MProvdSecurityChecker* /*aSecurityChecker*/)
	{
	LOG_FUNC
	return KErrNone;
	}

// CAvctpControlSAP inherited from CBluetoothSAP to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
TInt CAvctpControlSAP::SAPSetOption(TUint /*aLevel*/,TUint /*aName*/,const TDesC8& /*aOption*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	return KErrNotSupported;
	}

// Pure virtuals From MSocketNotify

// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::CanSend()
	{
	LOG_FUNC
	// the AVCTP Transport should not have given us tghis signal
	// only data saps for this
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::ConnectComplete()
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}
	
// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}
	
// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}
	
// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}
	
void CAvctpControlSAP::Error(TInt aError, TUint aOperationMask)
	{
	LOG_FUNC
	iSocket->Error(aError, aOperationMask);
	}
	
// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::Disconnect()
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

// CAvctpControlSAP inherited from MSocketNotify to take advantage of the socket paradigm, 
// but it is used just as control channel between the client side and the server side.
// so, many socket functions are not implemented
void CAvctpControlSAP::Disconnect(TDesC8& /*aDisconnectData*/)
	{	
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EAvctpControlSAPOpetationNotSupported));
	}

/*inline*/ void CAvctpControlSAP::ClearIoctl()
	{
	LOG_FUNC
	iIoctlLevel = 0;
	iIoctlName = 0;
	}

/**
 Create an HQueuedIndication object with the aInd argument (incoming device address)
 and put it at the end of the queue. It is used just for the incoming Ioctl complete 
 **/
void CAvctpControlSAP::QueueIncomingIndication(const TControlIoctlMessage& aMessage)
	{
	LOG_FUNC
	HQueuedIndication* ind = HQueuedIndication::New(aMessage);
	if (ind)
		{
		LOG1(_L("Adding Indication to queue: "), aMessage.iIoctl);
		iIndicationQueue.AddLast(*ind);
		}
	else
		{
		// OOM'd - better tell client...they've lost an indication
		Error(KErrNoMemory);
		}
	}

/**
 This method is called to notify back something to the client. both remote indications and 
 confirmation for actions requested from the client
 */
void CAvctpControlSAP::IoctlComplete(TDesC8* aBuf)
	{
	LOG_FUNC
	
	TControlIoctlMessage msg;
	TPckg<TControlIoctlMessage> pck(msg);
	
	pck.Copy(*aBuf);
	
	TBTDevAddr addr = pck().iAddr.BTAddr();
	
	// we can have the following cases:
	// 1) We are not waiting for a completion
	//	  Action: queue the indication to be served later
	// 2) The queue is not empty and there is an outstanding ioctl 
	//	  a) we had asked for a connection to a remote device
	//		I)  the ioctl complete is from the device we had requested.
	//		II) this ioctl complete is from another device
	//	  b) we were listening on an incoming connection
	// 3) we were listening on an incoming connection
	if (!iIoctlName)	// case 1
		{
		LOG(_L("iIoctlName is empty"));
		QueueIncomingIndication(msg);
		}
	else if (!iIndicationQueue.IsEmpty() )	// case 2
		{
		HQueuedIndication* ind = iIndicationQueue.First();
		
		__ASSERT_DEBUG(addr != TBTDevAddr(0), Panic(EAvctpInvalidAddress));
		
		if (ind->Indication().iAddr.BTAddr() == addr)	// case 2aI
			{
			iIndicationQueue.Remove(*ind);
			switch(ind->Indication().iIoctl)
				{
				case EPrimaryChannelAttachToTransport:
				case ESecondaryChannelAttachToTransport:
					__ASSERT_DEBUG(pck().iIoctl == ELinkUp || pck().iIoctl == EAttachConfirm || pck().iIoctl == EError, Panic(EAvctpUnexpectedIoctlCompletion));
					pck().iIoctl = EAttachConfirm;
					break;
				case EPrimaryChannelDetachFromTransport:
				case ESecondaryChannelDetachFromTransport:
					__ASSERT_DEBUG(pck().iIoctl == EDetachConfirm, Panic(EAvctpUnexpectedIoctlCompletion));
					break;	// do nothing the event is the same that goes back
				default:
					__ASSERT_DEBUG(EFalse, Panic(EAvctpUnexpectedIoctlCompletion));
					break;
				}
			iSocket->IoctlComplete(&pck);
			delete ind;
			}
		else	// case 2aII
			{
			LOG(_L("iIoctlName complete is for another device"));
			QueueIncomingIndication(msg);
			}	
		}
	else	// case 3
		{
		// tell client straight away
        if (iSocket)
            {
            TControlIoctls ioctlValue = pck().iIoctl;
            
            switch(pck().iIoctl)
            	{
            	case ELinkUp:
            		pck().iIoctl = EAttachIndicate;	// notify an indication
            		break;
            	case ELinkDown:
            		pck().iIoctl = EDetachIndicate;	// notify an indication
            		break;
            	default:
            		break;	// pass it as it is
            	}
            iSocket->IoctlComplete(&pck);
            }
		}
	ClearIoctl();
	}

HQueuedIndication* HQueuedIndication::New(const TControlIoctlMessage& aMessage)
    {
	LOG_STATIC_FUNC
    HQueuedIndication* ind = new HQueuedIndication;
    if (ind)
    	{
    	TRAPD(err, ind->ConstructL(aMessage));
    	if (err)
    	    {
    	    delete ind;
    	    ind=NULL;
    	    }
    	}
    return ind;
    }
	
void HQueuedIndication::ConstructL(const TControlIoctlMessage& aMessage)
    {
	LOG_FUNC
	iMessage = new (ELeave) TControlIoctlMessage(aMessage);
    }
    
HQueuedIndication::~HQueuedIndication()
    {
	LOG_FUNC
	delete iMessage;
    }

// Not used for avctp
void CAvctpControlSAP::NoBearer(const TDesC8& /*aConnectionInfo*/) {}
// Not used for avctp
void CAvctpControlSAP::Bearer(const TDesC8& /*aConnectionInfo*/) {}

