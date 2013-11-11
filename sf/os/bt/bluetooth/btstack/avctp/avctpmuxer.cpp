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
// AVCTP muxer
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <es_mbuf.h>

#include "avctpmuxer.h"
#include "avctpcommon.h"
#include "avctpmuxerstates.h"
#include "Avctp.h"
#include "avctputils.h"
#include "avctppacket.h"
#include "avctpsap.h"
#include "avctpPacketMgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

using namespace SymbianAvctp;

#ifdef __FLOG_ACTIVE	
#define LOG_CLIENTS LogClients();
#else
#define LOG_CLIENTS
#endif

/**
Factory function for CAvctpTransport - called when local device initiates
the connection.

Note the CProtocolBase passed in so that the muxer can create its own L2CAP SAP

  @internalComponent
  @leave KErrNoMemory if the muxer could not be allocated
  @param aProtocol The AVCTP protocol object, used to make callbacks on the protocol
  @return A new muxer in the right state
*/
CAvctpTransport* CAvctpTransport::NewL(CAvctpProtocol& aProtocol)
	{
	LOG_STATIC_FUNC

	CAvctpTransport* transport = new(ELeave) CAvctpTransport(aProtocol);
	CleanupStack::PushL(transport);
	transport->ConstructL();
	CleanupStack::Pop(transport);
	return transport;
	}

/**
AVCTP SubConnection Provider Constructor

Set up the async. callback.

  @internalComponent
  @param aProt The protocol object
*/
CAvctpTransport::CAvctpTransport(CAvctpProtocol& aProt) :
	iProtocol(aProt),
	iSecondChannelSocket(*this),
	iState(&(aProt.MuxerStateFactory().GetState(CAvctpMuxerStateFactory::EClosed))),
	iAddress(TBTDevAddr(0))
	{
	LOG_FUNC

	TCallBack cb(IdleTimerExpired, this);
	iIdleTimerEntry.Set(cb);
	};

/**
Second phase muxer construction.

  @internalComponent
*/
void CAvctpTransport::ConstructL()
	{
	LOG_FUNC

	TCallBack cb(SecondChannelNewDataAsyncCallBack, this);
	iNewDataAsyncCallBack = new (ELeave)CAsyncCallBack(cb, EActiveHighPriority);
	iPacketMgr = CAvctpPacketMgr::NewL(*this, iProtocol);

	SetSendBlocked(KAvctpPrimaryChannel);
	SetSendBlocked(KAvctpSecondaryChannel);
	CheckForIdle();
	}

/**
Muxer d'tor

We only Shutdown the lower protocol sap in an immediate way because in normal
circumstances we will have decided to die after idling for a while which means
no-one had data to send via this muxer. Hence there should be little chance
of losing data that could otherwise have been left in the L2CAP sap. If we did a
graceful close we would then have had to wait for the CanClose response which is
an unnecessary overhead if we've already used a timeout to decide no-one wants
the link.

If the muxer died gracefully, there will be no data in the system that needs
to be sent over this muxer, however if the muxer goes down suddenly, e.g. the
remote disconnects, a connection attempt fails or a Shutdown is called there
will be data left in the protocol as a whole that would attempt a reconnection
to the remote device of this muxer. Hence we tell the saps we've gone down to 
let them decide whether or not their packets have become stale or not.

@internalComponent
*/
CAvctpTransport::~CAvctpTransport()
	{
	LOG_FUNC

	DequeIdleTimer();
	
	iProtocol.SignalMuxerDownToSaps(DevAddr());
	iProtocol.RemoveTransport(*this);  // it's okay to deque the link even if that's already been done

	// the first channel (index 0) is the most important, so better closing from the last one
	
	TInt index;
	for(index = iChannelSAPs.Count()-1;index>=0;index--)
		{
		if (iChannelSAPs[index])
			{
			iChannelSAPs[index]->Shutdown(CServProviderBase::EImmediate);
			delete iChannelSAPs[index];
			}
		}
	
	CancelSecondChannelNewDataAsyncCallBack();
	delete iNewDataAsyncCallBack;
	
	delete iPacketMgr;
	iClientItems.Close();
	}


TInt CAvctpTransport::AddSecondChannel(CServProviderBase& aSAP)
	{
	LOG_FUNC
	return iState->AddSecondChannel(*this, aSAP);
	}

TBool CAvctpTransport::HasSecondChannel() const
	{
	LOG_FUNC
	return iChannelSAPs[KAvctpSecondaryChannel] != NULL ? ETrue : EFalse;
	}

void CAvctpTransport::RemoveSecondChannel()
	{
	LOG_FUNC
	iState->RemoveSecondChannel(*this);
	}

TInt CAvctpTransport::GetChannelMtu(TInt aChannel, TInt& aMtu) const
	{
	LOG_FUNC
	
	TInt err = KErrNotReady;
	if(iChannelSAPs[aChannel])
		{
		TPckgBuf<TInt> mtuBuf;
		err = iChannelSAPs[aChannel]->GetOption(KSolBtL2CAP, KL2CAPNegotiatedOutboundMTU, mtuBuf);
		if (err == KErrNone)
			{
			// MTU for AVCTP clients is L2CAP MTU - AVCTP Header
			aMtu = mtuBuf() - SymbianAvctp::ENormalHeaderLength;
			}
		}

	return err;
	}


/****************************************************************************/
/*
  Notifications from the MSocketNotify interface
  I.e calls from iChannelSAPs[KAvctpPrimaryChannel]
*/

/**
Called when new data is available.

This is called each time a new packet of data arrives from L2CAP.
We get to the data by calling GetData on the L2CAP SAP.

This assumes a packet interface from L2CAP.

  @internalComponent
  @param aCount Number of new packets waiting
*/
void CAvctpTransport::NewData(TUint aCount)
	{
	LOG_FUNC

	LOG1(_L("%d packets available"), aCount);

	if (aCount == KNewDataEndofData)
		{
		Disconnect();
		}
	else 
		{
		// for latency reasons, we process channel 1 data synchronously		
		// Only need to do this once rather than for each NewData call
		// because these calls are all synchronous and hence the MTU
		// of L2CAP can't change in-between GetData calls (changing
		// the MTU requires negotiation with a remote device
		// which is naturally asynchronous)
		TInt mtu = GetCurrentInboundMtu(KAvctpPrimaryChannel);
		
		if(mtu >= 0)
			{
			__DEBUG_ONLY(TInt err;)
			while (aCount--)
				{
				if ((__DEBUG_ONLY(err =) iState->NewData(*this, mtu, 0)) != KErrNone)
					{
					__ASSERT_DEBUG(err == KErrMuxerShutDown, Panic(EUnexpectedErrorCode));
					break;
					}
				}
			}
		}
	}


/**
Notification that we can now send data to the lower layer.

  @internalComponent
*/
void CAvctpTransport::CanSend()
	{
	LOG_FUNC
	iState->CanSend(*this, KAvctpPrimaryChannel); //channel is implicitly primary one because this upcall from the sap1
	}

/**
Signal from the lower protocol SAP that a connection has occurred.
	   
Part of the MSocketNotify interface. This is called when L2CAP
has brought up the lower layer link to the remote device.

  @internalComponent
*/
void CAvctpTransport::ConnectComplete()
	{
	LOG_FUNC

	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address;
		iRemoteAddr.GetReadable(address);		
		)

	LOG1(_L("from BT Device 0x%S"), &address);
	
	iState->ConnectComplete(*this);
	}

/**
Version with connection data.

Ignore the data (since L2CAP should never provide this!)

  @internalComponent
  @param aConnectData The data received on connection
*/
void CAvctpTransport::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	LOG(_L("ConnectComplete with aConnectData"));
	ConnectComplete();
	}

/**
Incoming connection completed on listening socket.

  @internalComponent
  @param aSSP The new SAP for the completed connection
*/
void CAvctpTransport::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	LOG(_L("ConnectComplete with aSSP"));
	ConnectComplete();
	}

/**
Incoming connection completed on listening socket with connection data.

  @internalComponent
  @param aSSP The new SAP for the completed connection
*/
void CAvctpTransport::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	LOG(_L("ConnectComplete with aSSP and aConnectData"));
	ConnectComplete();
	}

/**
We must have asked the lower protocol socket to shutdown, which means we're dying.
However we only ask for an immediate shutdown so we shouldn't get a CanClose callback

  @internalComponent
  @param aDelete How to shutdown
*/
void CAvctpTransport::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	Panic(ELowerProtocolSapCanClose);
	}

/**
The lower socket can close, with disconnection data.

We don't support this so panic

  @internalComponent
  @param aDisconnectData Data from the disconnecting socket
  @param aDelete How to shutdown
*/
void CAvctpTransport::CanClose(const TDesC8& /*aDisconnectData*/, TDelete /*aDelete*/)
	{
	LOG_FUNC
	Panic(ELowerProtocolSapCanClose);
	}

/**
Something has gone wrong.

We get told whether it's just this operation or others that are
affected.  We use this to decide what to do.

  @internalComponent
  @param aError The error code
  @param aOperationMask The operation(s) affected
*/
void CAvctpTransport::Error(TInt aError, TUint aOperationMask)
	{
	LOG_FUNC

	LOG2(_L("Error %d, OperationMask 0b%b"), aError, aOperationMask);
	iState->Error(*this, aError, aOperationMask, KAvctpPrimaryChannel);
	}

/**
The L2CAP link has disconnected. This is the result of a remote device disconnecting

  @internalComponent
*/
void CAvctpTransport::Disconnect()
	{
	LOG_FUNC
	iState->Disconnect(*this);
	}

/**
Disconnect with disconnection data.

Ignore the data (since L2CAP should never provide this!)

  @internalComponent
  @param aDisconnectData Data from the disconnecting socket
*/
void CAvctpTransport::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	LOG(_L("Disconnect with aDisconnectData"));
	Disconnect();
	}

/**
IOCTL forwarding not support, and none initiated by AVCTP so NOP
*/
void CAvctpTransport::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	__PANIC_UNEXPECTED_CALL
	}
 
/**
This function allows the muxer to be shutdown irrespective of whether 
it is idle. This is intended to be used to punish a remote device that
has done something naughty.

  @internalComponent
*/
void CAvctpTransport::Shutdown(TInt aError)
	{
	LOG_FUNC
	NotifyLinkDown(iAddress, KAvctpPrimaryChannel, aError);
	iState->Shutdown(*this, aError);
	}

TBool CAvctpTransport::HasDataToSend()
	{
	LOG_FUNC
	
	TBool ret = PacketMgr().WouldLikeToSend();
	if (ret == EFalse)
		{
		ret = iProtocol.SapsHaveDataFor(DevAddr());
		}
	return ret;
	}

/**********************************************************************/
/*
  Commands from the AVCTP SAP.
*/


TInt CAvctpTransport::Start(const TBTDevAddr& aAddr, TUint16 aClientId)		
	{
	LOG_FUNC

	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address;
		aAddr.GetReadable(address);
		)

	LOG1(_L("from BT Device 0x%S"), &address);
	
	iAddress = aAddr;

	return iState->Start(*this, aAddr, aClientId);
	}

TInt CAvctpTransport::StartIncoming(const TBTDevAddr& aAddr, CServProviderBase* aL2CAPConSAP)
	{
	LOG_FUNC
	LOG1(_L("CServProviderBase* aL2CAPConSAP 0x%08x"), aL2CAPConSAP);
	
	iAddress = aAddr;

	TInt err = iState->StartIncoming(*this, aAddr, aL2CAPConSAP);
	return err;
	}


/**********************************************************************/
/*
  Internal functions.
*/

/**
Set the BTDevAddr for this Muxer & add us to the Protocol's Q

  @internalComponent
  @param aDevAddr The Muxer's new remote address
  @return KErrInUse if there is already a Muxer on aRemoteAddr, otherwise KErrNone
*/
void CAvctpTransport::AssignToDevice(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC

	// Note the below assumes that there is at most one data client (or they're all on the same PID)
	__ASSERT_DEBUG(iRemoteAddr == TBTDevAddr(0) || iRemoteAddr == aRemoteAddr, Panic(EMuxerAlreadyBound));

	if (iRemoteAddr == TBTDevAddr(0))
		{
		iRemoteAddr = aRemoteAddr;
		}
	}

/**
 New data on the secondary channel is async because we want to be responsive while processing a 
 potentially big amount of data
 */
/*static*/ TInt CAvctpTransport::SecondChannelNewDataAsyncCallBack(TAny* aTransport)
	{
	LOG_STATIC_FUNC

	CAvctpTransport* t = static_cast<CAvctpTransport*>(aTransport);
	
	// Only need to do this once rather than for each NewData call
	// because these calls are all synchronous and hence the MTU
	// of L2CAP can't change in-between GetData calls (changing
	// the MTU requires negotiation with a remote device
	// which is naturally asynchronous)
	TInt mtu = t->GetCurrentInboundMtu(1);
	__DEBUG_ONLY(TInt err;)
	if(mtu >= 0)
		{
		while (t->iSecondChannelPacketsWaiting)
			{
			if ((__DEBUG_ONLY(err =) t->iState->NewData(*t, mtu, KAvctpSecondaryChannel)) != KErrNone)
				{
				__ASSERT_DEBUG(err == KErrMuxerShutDown, Panic(EUnexpectedErrorCode));
				break;
				}
			--(t->iSecondChannelPacketsWaiting);
			}
		}
	// else we have an error getting the MTU, this is probably because the 
	// L2CAP channel is in the process of shutting down.  We leave the data
	// there where it'll be cleared up later.  If not we'll pick this data
	// up next time we get signalled NewData from the lower protocol.

	return EFalse;
	}
	
/**
Check to see if we're still needed.  If not, Q a delayed delete.

  @internalComponent
*/
void CAvctpTransport::CheckForIdle()
	{
	LOG_FUNC

	if ( IsIdle())
		{
		QueIdleTimer();
		}
	}

/**
@return System wide error code if the value could not be retrieved, otherwise the current
		inbound MTU.
*/
TInt CAvctpTransport::GetCurrentInboundMtu(TInt aChannel)
	{
	LOG_FUNC
	TPckgBuf<TInt> buf;
	
	__ASSERT_DEBUG(iChannelSAPs[aChannel], Panic(ENullLowerProtocolSap));
	
	TInt err = iChannelSAPs[aChannel]->GetOption(KSolBtL2CAP, KL2CAPInboundMTU, buf);
	if(!err)
		{
		return buf();
		}
	else
		{
		return err;
		}
	}

/**
@return System wide error code if the value could not be retrieved, otherwise the current
		outbound MTU.
*/
TInt CAvctpTransport::GetCurrentOutboundMtu(TInt aChannel)
	{
	LOG_FUNC
	TPckgBuf<TInt> buf;
	
	__ASSERT_DEBUG(iChannelSAPs[aChannel], Panic(ENullLowerProtocolSap));
	TInt err = iChannelSAPs[aChannel]->GetOption(KSolBtL2CAP, KL2CAPOutboundMTUForBestPerformance, buf);
	if(!err)
		{
		return buf();
		}
	else
		{
		return err;
		}
	}
	
TInt CAvctpTransport::DoWrite(RMBufChain& aPDU, TInt aChannel)
	{
	LOG_FUNC
	
	__ASSERT_DEBUG(iChannelSAPs[aChannel], Panic(ENullLowerProtocolSap));
	return iChannelSAPs[aChannel]->Write(aPDU,0);
	}

/**
Check to see if the Muxer is idle. 
Note this subConProvider shouldn't have any direct data clients.
It's only important if there are control clients or there are saps that have data to send
through the muxer. So a data client could effectively keep us non idle by queueing data 
for us.

  @internalComponent
*/
TBool CAvctpTransport::IsIdle()
	{
	LOG_FUNC
	return iState->IsIdle(*this);
	}

/**
Queues the idle timer if necessary

  @internalComponent
*/
void CAvctpTransport::QueIdleTimer()
	{
	LOG_FUNC

	if (!iIdleTimerQueued)
		{
		LOG(_L("Queued idle timer"));

		iIdleTimerQueued = ETrue;
		BTSocketTimer::Queue(KTransportIdleTimeout, iIdleTimerEntry);
		}
	}

/**
Deques idle timer if necessary

  @internalComponent
*/
void CAvctpTransport::DequeIdleTimer()
	{
	LOG_FUNC

	if (iIdleTimerQueued)
		{
		LOG(_L("Dequeued idle timer"));

		iIdleTimerQueued = EFalse;
		BTSocketTimer::Remove(iIdleTimerEntry);
		}
	}

/**
Static idle callback.

This is entered if we remain idle (as defined by IsIdle()) for the duration of the timer.
  @internalComponent
  @param aMux The muxer that is idle
  @return EFalse - do no reissue callback
*/
TInt CAvctpTransport::IdleTimerExpired(TAny* aTransport)
	{
	LOG_STATIC_FUNC
	LOG1(_L("on Transport 0x%08x"), aTransport);
	
	CAvctpTransport* transport = static_cast<CAvctpTransport*>(aTransport);
	
	__ASSERT_DEBUG(transport, Panic(ENullMuxer));
	// If the following panics it means we've not cancelled our IdleTimer at some previous point
	__ASSERT_DEBUG(transport->IsIdle(), Panic(EIdleTimeoutWhenNotIdle));

	transport->iIdleTimerQueued = EFalse;  // Obviously...

	transport->iState->IdleTimerExpired(*transport);
	
	return EFalse;
	}

void CAvctpTransport::NotifyLinkState(const TBTDevAddr& aAddr, TControlIoctls aIotcl, TInt aChannel, TInt aError)
	{
	LOG_FUNC
	TControlIoctlMessage msg(aIotcl, aAddr, aError);
	TPckgC<TControlIoctlMessage> pck(msg);
	
	TClientItemIter iter(iClientItems);
	while(iter.NextKey())
		{
		const TClientItem** pitem = iter.CurrentValue();
		MSocketNotify* socket = aChannel == KAvctpSecondaryChannel ? (*pitem)->SecondaryChannel() : (*pitem)->PrimaryChannel();
		if(socket)
			{
			socket->IoctlComplete(&pck);
			}
		}
	}

void CAvctpTransport::NotifyLinkUp(const TBTDevAddr& aAddr, TBool aIsSecondChannel)
	{
	LOG_FUNC
	iProtocol.NotifyLinkUp(aAddr, aIsSecondChannel);
	}

void CAvctpTransport::NotifyLinkDown(const TBTDevAddr& aAddr, TInt aChannel, TInt aError)
	{
	LOG_FUNC
	NotifyLinkState(aAddr, ELinkDown, aChannel, aError);
	}

void CAvctpTransport::NotifyAttachConfirm(TInt aError, TBool aIsSecondChannel)
	{
	LOG_FUNC
	TControlIoctlMessage msg(SymbianAvctp::EAttachConfirm, iRemoteAddr, aError);
	TPckgC<TControlIoctlMessage> pck(msg);
	
	TClientItemIter iter(iClientItems);
	while(iter.NextKey())
		{
		const TClientItem** pitem = iter.CurrentValue();
		MSocketNotify* socket = aIsSecondChannel ? (*pitem)->SecondaryChannel() : (*pitem)->PrimaryChannel();
		__ASSERT_DEBUG(socket, Panic(EAvctpInvalidChannelNotify));
		socket->IoctlComplete(&pck);
		}
	}

void CAvctpTransport::NotifyAttachConfirm(TUint16 aClientId, TInt aError, TBool aIsSecondChannel)
	{
	LOG_FUNC
	TPtrC8 addr(iRemoteAddr.Des());
	TControlIoctlMessage msg(SymbianAvctp::EAttachConfirm, iRemoteAddr, aError);
	TPckgC<TControlIoctlMessage> pck(msg);
	
	const TClientItem** pitem = iClientItems.Find(aClientId);
	if (pitem)
		{
		MSocketNotify* socket = aIsSecondChannel ? (*pitem)->SecondaryChannel() : (*pitem)->PrimaryChannel();
		if (aIsSecondChannel && !(*pitem)->IsSecondaryChannelAttached())
			return;
		
		__ASSERT_DEBUG(socket, Panic(EAvctpInvalidChannelNotify));
		if (socket)
			{
			socket->IoctlComplete(&pck);
			}
		}
	}

void CAvctpTransport::NotifyDetachConfirm(TUint16 aClientId, TInt aError, TBool aIsSecondChannel)
	{
	LOG_FUNC
	TPtrC8 addr(iRemoteAddr.Des());
	TControlIoctlMessage msg(SymbianAvctp::EDetachConfirm, iRemoteAddr, aError);
	TPckgC<TControlIoctlMessage> pck(msg);
	
	const TClientItem** pitem = iClientItems.Find(aClientId);
	if (*pitem)
		{
		MSocketNotify* socket = aIsSecondChannel ? (*pitem)->SecondaryChannel() : (*pitem)->PrimaryChannel();
		__ASSERT_DEBUG(socket, Panic(EAvctpInvalidChannelNotify));
		socket->IoctlComplete(&pck);
		}
	}

void CAvctpTransport::NotifyLinkError(TInt aError, TBool aIsSecondChannel)
	{
	LOG_FUNC
	TPtrC8 addr(iRemoteAddr.Des());
	TControlIoctlMessage msg(SymbianAvctp::EError, iRemoteAddr, aError);
	TPckgC<TControlIoctlMessage> pck(msg);
	
	TClientItemIter iter(iClientItems);
	while(iter.NextKey())
		{
		const TClientItem** pitem = iter.CurrentValue();
		MSocketNotify* notify = aIsSecondChannel ? (*pitem)->SecondaryChannel() : (*pitem)->PrimaryChannel();
		if (notify)
			{
			notify->IoctlComplete(&pck);
			}
		}
	}

void CAvctpTransport::MbpsnNewData(TUint aCount)
	{
	// new data from the second channel

	LOG_FUNC

	LOG1(_L("%d packets available"), aCount);

	if (aCount == KNewDataEndofData)
		{
		this->MbpsnDisconnect();
		}
	else 
		{
		iSecondChannelPacketsWaiting += aCount;
		StartSecondChannelNewDataAsyncCallBack();
		}

	}

void CAvctpTransport::MbpsnCanSend()
	{
	LOG_FUNC
	iState->CanSend(*this, KAvctpSecondaryChannel); //channel is implicitly the second channel with this upcall
	}

void CAvctpTransport::MbpsnConnectComplete()
	{
	LOG_FUNC
	
	IF_FLOGGING
		(
		TBuf<KBTAddressLength> address;
		iRemoteAddr.GetReadable(address);
		)

	LOG1(_L("from BT Device 0x%S"), &address);
	
	// state machine should be able to resolve the difference in saps completing
	iState->ConnectComplete(*this);
	}

void CAvctpTransport::MbpsnConnectComplete(CServProviderBase& aSAP)
	{
	LOG_FUNC
	// it is an incoming connection and we don't know the pid. 
	// but that's fine because it will be ignored in the state machine.
	iState->AddSecondChannel(*this, aSAP);
	}

//only initiate immediate shutdowns so there should be no canclose callback
void CAvctpTransport::MbpsnCanClose(MSocketNotify::TDelete /*aDelete*/)
	{
	LOG_FUNC
	Panic(ELowerProtocolSapCanClose);
	}

void CAvctpTransport::MbpsnError(TInt aError, TUint aOperationMask)
	{
	LOG_FUNC

	LOG2(_L("Error %d, OperationMask 0b%b"), aError, aOperationMask);
	
	iState->Error(*this, aError, aOperationMask, KAvctpSecondaryChannel);
	}

void CAvctpTransport::MbpsnDisconnect()
	{
	LOG_FUNC
	iState->SecondChannelRemoved(*this);
	}

void CAvctpTransport::SetSecondChannelCtrlNotify(TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket)
	{
	LOG_FUNC
	iProtocol.SetSecondChannelCtrlNotify(aClientId, aSecondChannelControlSocket);
	}


TInt CAvctpTransport::AddPrimaryChannelRef(const TClientItem* item)
	{
	LOG_FUNC
	DequeIdleTimer();	// adding a client reference means not to be idle anymore
	TUint16 clientId = item->ClientId();
	return iClientItems.Insert(clientId, item);
	}

void CAvctpTransport::AddSecondaryChannelRef()
	{
	iSecondaryChannelClientRefCount++;
	}

void CAvctpTransport::RemovePrimaryChannelRef(TUint16 aClientId)
	{
	LOG_FUNC
	
	TClientItem** item = const_cast<TClientItem**>(iClientItems.Find(aClientId));
	if (item && *item)
		{
		iClientItems.Remove(aClientId);
		}
	if (iClientItems.Count() == 0 && IsIdle())
		{
		iProtocol.RemoveTransport(*this);
		delete this;
		}
	}

/**
 This method is called from ReleaseExtendedTransport() and RemoveSap().
 When called from RemoveSap (that is called when the sap is shutdown) the secondary channel
 may or may not exist. If the muxer state is, for example, Open then the secondary channel
 does not exist. In this case, HasSecondChannel() return false and the method exits.
 If the muxer is in SecondChannelPending state HasSecondChannel returns true. we can have a
 situation where 
 */
void CAvctpTransport::RemoveSecondaryChannelRef(TUint16 aClientId)
	{
	LOG_FUNC
	
	if (HasSecondChannel())
		{
		TClientItem** item = const_cast<TClientItem**>(iClientItems.Find(aClientId));
		__ASSERT_DEBUG(item, Panic(EAvctpClientNotFound));
		
		if (iSecondaryChannelClientRefCount > 0)
			{
			iSecondaryChannelClientRefCount--;
			}
		
		if (iSecondaryChannelClientRefCount == 0)
			{
			RemoveSecondChannel();
			}
		}
	}

TInt CAvctpTransport::ClientCount()
	{
	return iClientItems.Count();
	}

TInt CAvctpTransport::ClientSecondaryChannelCount()
	{
	TInt count = 0;
	TClientItemIter iter(iClientItems);
	while(iter.NextKey())
		{
		const TClientItem** pitem = iter.CurrentValue();
		if ((*pitem)->IsSecondaryChannelAttached())
			{
			count++;
			}
		}
	return count;
	}

TBool CAvctpTransport::HasClient(TUint16 aClientId)
	{
	LOG_FUNC
	return (iClientItems.Find(aClientId) != NULL) ? ETrue : EFalse;
	}

void CAvctpTransport::BindSecondaryChannelSap(CServProviderBase& aSAP)
	{
	iChannelSAPs[KAvctpSecondaryChannel] = &aSAP;
	iChannelSAPs[KAvctpSecondaryChannel]->SetNotify(&iSecondChannelSocket);
	}

//pseudosocket stuff
TAvctpSecondChannelPseudoSocket::TAvctpSecondChannelPseudoSocket(MSecondChannelPseudoSocketNotify& aNotify)
:iNotify(aNotify)
	{
	LOG_FUNC
	}


void TAvctpSecondChannelPseudoSocket::NewData(TUint aCount)
	{
	LOG_FUNC
	iNotify.MbpsnNewData(aCount);
	}
	
void TAvctpSecondChannelPseudoSocket::CanSend()
	{
	LOG_FUNC
	iNotify.MbpsnCanSend();
	}
	
void TAvctpSecondChannelPseudoSocket::ConnectComplete()
	{
	LOG_FUNC
	iNotify.MbpsnConnectComplete();
	}
	
void TAvctpSecondChannelPseudoSocket::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	/*drop*/
	}
	
void TAvctpSecondChannelPseudoSocket::ConnectComplete(CServProviderBase& aSSP)
	{
	LOG_FUNC
	iNotify.MbpsnConnectComplete(aSSP);
	}
	
void TAvctpSecondChannelPseudoSocket::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	/*drop*/
	}
	
void TAvctpSecondChannelPseudoSocket::CanClose(TDelete aDelete)
	{
	LOG_FUNC
	iNotify.MbpsnCanClose(aDelete);
	}
	
void TAvctpSecondChannelPseudoSocket::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	/*drop*/
	}
	
void TAvctpSecondChannelPseudoSocket::Error(TInt aError, TUint aOperationMask)
	{
	LOG_FUNC
	iNotify.MbpsnError(aError, aOperationMask);
	}
	
void TAvctpSecondChannelPseudoSocket::Disconnect()
	{
	LOG_FUNC
	iNotify.MbpsnDisconnect();
	}

void TAvctpSecondChannelPseudoSocket::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	}
