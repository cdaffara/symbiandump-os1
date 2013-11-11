// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the avdtp signalling channel
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpSignallingChannel.h"
#include "avdtp.h"
#include "avdtputil.h"
#include "avdtpSignallingTransaction.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

//Macro to just return (and enter debugger in debug builds) if we get an unsolicited response packet
#define RETURN_IF_NO_TRANSACTION(transaction) 	if (!transaction) {	LOG(_L("No transaction found"));return;}
#define RETURN_IF_SIGNAL_BAD(transaction, signal) if (!CheckSignal(*transaction, signal)) {LOG1(_L("Transaction has different signal (%d)"), signal); return;}

// check that sessions have replied with an AVDTP error in error cases
#define	CHECK_ERROR(result) if (!(result<=KErrAvdtpSignallingErrorBase || result==KErrNone || result==KErrNotFound)) __DEBUGGER();

CSignallingChannel* CSignallingChannel::NewL(CAvdtpProtocol& aProtocol,
											 CLogicalChannelFactory& aChannelFactory,
											 const TBTDevAddr& aRemoteAddress)
	{
	LOG_STATIC_FUNC
	CSignallingChannel* s = CSignallingChannel::NewLC(aProtocol, aChannelFactory, aRemoteAddress);
	CleanupStack::Pop();
	return s;
	}

CSignallingChannel* CSignallingChannel::NewLC(CAvdtpProtocol& aProtocol,
											  CLogicalChannelFactory& aChannelFactory,
											  const TBTDevAddr& aRemoteAddress)
	{
	LOG_STATIC_FUNC
	CSignallingChannel* s = new(ELeave)	CSignallingChannel(aProtocol, aChannelFactory, aRemoteAddress);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}
	
CSignallingChannel::CSignallingChannel(CAvdtpProtocol& aProtocol,
									   CLogicalChannelFactory& aChannelFactory,
									   const TBTDevAddr& aRemoteAddress)
: iProtocol(aProtocol),
  iLogicalChannelFactory(aChannelFactory),
  iPermanentUsers(_FOFF(XAvdtpSignalReceiver,iSignalReceiverEmbeddedLink)),
  iTransactions(_FOFF(CSignallingTransaction,iLink)),
  iRemoteAddress(aRemoteAddress),
  iInboundMessage(*this),
  iDraftMessages(_FOFF(CAvdtpOutboundSignallingMessage,iLink)),
  iQueuedMessages(_FOFF(CAvdtpOutboundSignallingMessage,iLink))
	{
	LOG_FUNC
	SetBlocked(EFalse);
	
	TCallBack cb(TryToClose, this);
	iIdleTimerEntry.Set(cb);
	}
	
void CSignallingChannel::ConstructL()
	{
	LOG_FUNC
	TCallBack cb(CSignallingChannel::TryToSendCallback, this);
	iTryToSendCallback = new (ELeave)CAsyncCallBack(cb, CActive::EPriorityStandard);
	}

CSignallingChannel::~CSignallingChannel()
	{
	LOG_FUNC
	CancelIdleTimer();

	// cancel possible outstanding factory request
	iLogicalChannelFactory.Cancel(iLogicalChannelRequest);
	
	// remove from protocol's knowledge
	iProtocol.SignallingChannelDown(*this);
	
	// any preauthorisation for device this represents is now void
	if (!IsListening())
		{
		iProtocol.RemoteSEPCache().InvalidateSEPs(iRemoteAddress);
		(void)iProtocol.SetPreauthorisation(iRemoteAddress, EFalse);
		}
		
	if (iBearer)
		{
		// tell l2cap
		iBearer->Shutdown(CServProviderBase::EImmediate);
		}
		
	delete iBearer;
	delete iTryToSendCallback;
	}

void CSignallingChannel::StartTryToSendCallback()
	{
	LOG_FUNC
	if (!iTryToSendCallback->IsActive())
		{
		iTryToSendCallback->CallBack();
		}
	// else already running and will callback anyway
	}

void CSignallingChannel::CancelTryToSendCallback()
	{
	LOG_FUNC
	iTryToSendCallback->Cancel();
	}

/*static*/ TInt CSignallingChannel::TryToSendCallback(TAny *aSigCh)
	{
	LOG_STATIC_FUNC
	return (reinterpret_cast<CSignallingChannel*>(aSigCh))->TryToSendCallback();
	}

void CSignallingChannel::ObtainMTU()
	{
	LOG_FUNC
	TPckg<TInt> mtubuf(iBearerMTU);
	TInt err=iBearer->GetOption(KSolBtL2CAP,KL2CAPOutboundMTUForBestPerformance,mtubuf);
	if (err!=KErrNone)
		{
		Error(err);
		}
	}

/*
Does post-packet-send processing
*/
void CSignallingChannel::PacketSent(CSignallingTransaction& aTransaction)
	{
	LOG_FUNC
	// We can't delete the transaction yet, because it looks after timer.
		
	switch (aTransaction.SentAction())
		{
		case EKeepSetRTX:
			{
			aTransaction.StartTimer();
			break;
			}
		case EDiscard:
			{
			// these transactions can be cleared up now
			// we don't support caching of responses if they get lost - they'll be regenerated if needed
			aTransaction.iLink.Deque();
			delete &aTransaction;
			break;
			}
		case EKeepDontSetRTX:
		// do nothing!
			break;
		default:
			__ASSERT_DEBUG(0, Panic(EAvdtpUnknownPostSendAction));
		}
	}
	
void CSignallingChannel::CheckOutboundQueue()
	{
	// check to see if the outbound queue is empty but we have an outstanding
	// request to service it
	if (iQueuedMessages.IsEmpty() && iTryToSendCallback->IsActive())
		{
		CancelTryToSendCallback();
		}				
	}
	
void CSignallingChannel::ServiceOutboundQueue()
	{
	LOG_FUNC
	if (!Blocked())
		{
		// we can send!
		// get fragments to send if we don't have any from before
		if(iOutgoingSignallingMessage.IsEmpty())
			{
			// get the head message
			CAvdtpOutboundSignallingMessage& message = *iQueuedMessages.First();
			
			// find the transaction pertaining to it (and make it the current transaction)
			iCurrentTransaction = FindTransaction(message.TransactionLabel());
			
			__ASSERT_DEBUG(iCurrentTransaction!=NULL, Panic(EAvdtpSignallingChannelDrainingFaulty));
			
			// see whether this requires fragmenting, and get next fragment
			// note reference return parameter
			TRAPD(ret, iMoreFrags = message.GetNextOutboundFragmentL(iOutgoingSignallingMessage, iBearerMTU));
			if (ret!=KErrNone)
				{
				iMoreFrags = EFalse;
				iOutgoingSignallingMessage.Free();
				// error the user
				iCurrentTransaction->Error(ret);
				return;
				}
			}
			

		// write outgoing fragments to bearer
		TInt sent = iBearer->Write(iOutgoingSignallingMessage,NULL);
		
		if (!sent)
			{
			// leave fragment there for next time writing
			SetBlocked(ETrue);
			}
		else
			{
			iOutgoingSignallingMessage.Free();
			if (!iMoreFrags)
				{
				// that message has been completely sent
				PacketSent(*iCurrentTransaction);
				}
			}

		if (!iQueuedMessages.IsEmpty())
			{
			// more to send, send asynchronously.
			StartTryToSendCallback();
			}
		else if (iTryToSendCallback->Priority()==KAvdtpReleaseAcceptPriority)
            {
            // cancel the Release send boost (actually might want to use boosting
            // to clear remaining fragments of partially sent commands)
            iTryToSendCallback->SetPriority(CActive::EPriorityStandard);
            }
   		}
	}

/*static*/ TInt CSignallingChannel::TryToSendCallback()
	{
	LOG_STATIC_FUNC
	ServiceOutboundQueue();
	return EFalse;
	}

TBTDevAddr CSignallingChannel::RemoteAddress()
	{
	LOG_FUNC
	if (iBearer && IsListening())
		{
		TL2CAPSockAddr remote;
		iBearer->RemName(remote);		
		iRemoteAddress = remote.BTAddr();
		}
	if (iBearer)
		{
		return iRemoteAddress;
		}
	else
		{
		return TBTDevAddr(0);
		}
	}

void CSignallingChannel::LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket aTicket, TInt aError)
	{
	LOG_FUNC
	
	if (aError == KErrNone)
		{
		iLogicalChannelRequest = aTicket;

		TLogicalChannelRecord rec = aTicket.GetLogicalChannel();
		
		// we take this "out" of the logical channel for performance reasons
		// (the pending logical channel will then die at some point later)
		iBearer = rec.iLogicalChannelSAP;
		iBearer->SetNotify(this);
		
		// get the MTU now
		ObtainMTU();

		// decide on our address (we may have been passive)
		// to let clients know we signal all the Signal Receivers
		TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	
		while (iter)
			{
			iter++->SignallingChannelReady(*this);
			}
			
		// migrate all the current listening sessions, to this new channel
		// if their clients not happy they can detach and relisten for new connection
		// just as if they got an inbound connection - which this is like
		// whether it really is passive l2cap, or was initiated by any one session
		
		iProtocol.ConnectSignallingListeners(*this);
		
		// process any already received inbound data
		if (rec.iDataCount)
			{
			NewData(rec.iDataCount);
			}
		if (rec.iEndOfData)
			{
			NewData(KNewDataEndofData);
			Disconnect();
			}
		}
	else
		{
		// to let clients know we signal all the Signal Receivers
		TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	
		while (iter)
			{
			iter++->SignallingChannelError(aError);
			}
		if (IsIdle())
			{
			IdledD();
			}
		}
	}
	
TInt CSignallingChannel::AttachSignallingUser(XAvdtpSignalReceiver& aUser)
	{
	LOG_FUNC
	// must check they're not already in the queue
#ifdef _DEBUG
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	while (iter)
		{
		if (iter++ == &aUser)
			{
			Panic(EAvdtpBadSignallingUserQueue);
			}
		}
#endif

	TBool isFirstUser = iPermanentUsers.IsEmpty();
	TInt ret = KErrNone;
	
	iPermanentUsers.AddFirst(aUser);
	if (iBearer)
		{
		aUser.SignallingChannelReady(*this);
		}
	else if(isFirstUser)
		{
		// kick off the need for a signalling channel - we have a user
		
		// form a request
		if (!IsListening())
			{
			TRAP(ret, iLogicalChannelRequest = iLogicalChannelFactory.CreateSignallingLogicalChannelL(iRemoteAddress, *this));
			}
		else
			{
			TRAP(ret, iLogicalChannelRequest = iLogicalChannelFactory.ExpectSignallingLogicalChannelL(*this));
			}
		}
	
	return ret;
	}

void CSignallingChannel::DetachSignallingUser(XAvdtpSignalReceiver& aUser)
	{
	LOG_FUNC
	aUser.iSignalReceiverEmbeddedLink.Deque();
	
	// cleanup any requests they had outstanding
	RemoveTransactions(aUser);

	if (IsIdle())
		{
		if(!IsListening())
			{
			QueIdleTimer();
			}
		else
			{
			//listener just dies
			IdledD();
			}
		}
	}

TBool CSignallingChannel::IsIdle() const
	{
	LOG_FUNC

	// idle if permanent users empty, and transient users haven't got stuff queued
	return (iPermanentUsers.IsEmpty() && iTransactions.IsEmpty());
	}
	
void CSignallingChannel::QueIdleTimer()
	{
	LOG_FUNC
	CancelIdleTimer();
	iIdleTimerActive = ETrue;	
	BTSocketTimer::Queue(KAvdtpSignallingChannelIdleTimeout, iIdleTimerEntry);
	}
	
void CSignallingChannel::CancelIdleTimer()
	{
	LOG_FUNC
	if(iIdleTimerActive)
		{
		// there's something to cancel
		iIdleTimerActive = EFalse;
		BTSocketTimer::Remove(iIdleTimerEntry);
		}
	}

/*static*/ TInt CSignallingChannel::TryToClose(TAny* aSignallingChannel)
	{
	LOG_STATIC_FUNC
	// check if still idle
	CSignallingChannel* sigch = reinterpret_cast<CSignallingChannel*>(aSignallingChannel);
	if (sigch->IsIdle())
		{
		sigch->IdledD();
		}
	return EFalse;
	}

void CSignallingChannel::IdledD()
	{
	LOG_FUNC
	// we are till idle
	__ASSERT_ALWAYS(iDraftMessages.IsEmpty(), Panic(EAvdtpSignallingChannelDyingWithDraftMessagesOnQueue));
	__ASSERT_ALWAYS(iTransactions.IsEmpty(), Panic(EAvdtpSignallingChannelDyingWithTransactionsOnQueue));
	__ASSERT_ALWAYS(iQueuedMessages.IsEmpty(), Panic(EAvdtpSignallingChannelDyingWithMessagesOnQueue));

	delete this;
	}
	
/*
Remove transactions held by a user
*/
void CSignallingChannel::RemoveTransactions(XAvdtpSignalReceiver& aUser)
	{
	LOG_FUNC
	TDblQueIter<CSignallingTransaction> iter(iTransactions);
	while (iter)
		{
		CSignallingTransaction* transaction = iter++;
		if (transaction->User() == &aUser)
			{
			transaction->iLink.Deque();
			delete transaction;
			
			// outbound queue may now be empty
			CheckOutboundQueue();			
			}
		}
	}

void CSignallingChannel::NewData(TUint aCount)
	{
	LOG_FUNC
	// read out now (could make this async though)
	// just give to inbound message
	TInt res = iInboundMessage.NewData(aCount);
	
	if (res!=KErrNone)
		{
		// have to error this channel
		Error(res);
		}
	}
	

/**
Called by packet as it builds itself up - get stuff out of bearer now
The service provided is always to fetch an MTU's worth
*/
TInt CSignallingChannel::GetData(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr)
	{
	LOG_FUNC
	return iBearer->GetData(aData,iBearerMTU,aOptions,aAddr);
	}
	

/**
Upcall from L2CAP - it is now ready to send
*/	
void CSignallingChannel::CanSend()
	{
	LOG_FUNC
	SetBlocked(EFalse);
	StartTryToSendCallback();
	}
	
void CSignallingChannel::ConnectComplete()
	{
	LOG_FUNC
	// the factory would have done this by now
	}
	
void CSignallingChannel::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// the factory would have done this by now
	}
	
void CSignallingChannel::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	// the factory would have done this by now
	}
	
void CSignallingChannel::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// the factory would have done this by now
	}
	
void CSignallingChannel::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	// only support immediate shutdown so shouldnt be called
	}
	
void CSignallingChannel::CanClose(const TDesC8& /*aDisconnectData*/,TDelete aDelete)
	{
	LOG_FUNC
	// l2cap shouldnt upcall this, but just in case
	CanClose(aDelete);
	}
	
void CSignallingChannel::Error(TInt aError,TUint /*aOperationMask*/)
	{
	LOG_FUNC
	// from logical channel
	ErrorPermanentUsers(aError);
	ErrorServiceRequesters(aError);
	delete this;
	}
	
void CSignallingChannel::ErrorPermanentUsers(TInt aError)
	{
	LOG_FUNC
	// error all users
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	while (iter)
		{
		iter++->SignallingChannelError(aError);
		}
	}

void CSignallingChannel::ErrorServiceRequesters(TInt aError)
	{
	LOG_FUNC
	// complete all outstanding services
	TDblQueIter<CSignallingTransaction> iter(iTransactions);
	while (iter)
		{
		CSignallingTransaction* transaction = iter++;

		if (transaction->User())
			{
			// it was a command, tell the user it failed
			// This will cause the transaction to be tidied
			// up.
			transaction->Error(aError);
			}
		else
			{
			// Just tidy up, there's no user to inform.
			RemoveTransaction(*transaction);
			}					
		}
	}
	
void CSignallingChannel::CancelTransactions(XAvdtpSignalReceiver& aUser)
	{
	LOG_FUNC
	RemoveTransactions(aUser);
	}
	
void CSignallingChannel::Disconnect()
	{
	LOG_FUNC
	Error(KErrDisconnected);
	}
	
void CSignallingChannel::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	// shouldnt be supported by l2cap, but just in case
	Disconnect();
	}
	
void CSignallingChannel::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	// we don't issue ioctls - drop
	}
	
void CSignallingChannel::NoBearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// do nothing
	}
	
void CSignallingChannel::Bearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// do nothing	
	}
		
/**
Find a transaction given a transaction label - there shuold be one
@param aLabel the transaction label in the packet for which a transaction is required
@internalComponent
*/
CSignallingTransaction* CSignallingChannel::FindTransaction(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TDblQueIter<CSignallingTransaction> iter(iTransactions);
	while (iter)
		{
		CSignallingTransaction* t = iter;
		if (t->Label() == aLabel)
			{
			return t;
			}
		iter++;
		}
	//__DEBUGGER();// FLOG
	return NULL;
	}

/**
Relinquish a transaction - may not be found if race with client cancelling
@param aLabel the label of the transaction to relinquish
@internalComponent
**/	
void CSignallingChannel::RemoveTransaction(CSignallingTransaction& aTransaction)
	{
	LOG_FUNC

	// This will cancel the timer and remove the transaction from the queue
	delete &aTransaction;

	// outbound queue may now be empty
	CheckOutboundQueue();			
	
	if (IsIdle())
		{
		QueIdleTimer();
		}
	}
	
void CSignallingChannel::DiscoverIndication(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	// create transaction ready for the response
	CSignallingTransaction* transaction = PrepareSignallingResponse(EResponseAccept, EAvdtpDiscover, aLabel);
	
	if (transaction)
		{
		// go and ask sessions to contribute to packet
		TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
		
		while (iter)
			{
			// sessions MUST process this synchronously.
			(void)(iter++)->DiscoverIndication(aLabel, transaction->Message());
			}
		
		// ..as we now deem the packet constructed, and ready to send
		(void)EnqueueMessage(*transaction);
		// if Enque failed (it can't at present for Responses) remote would timeout
		}
	}
	
	

void CSignallingChannel::DiscoverConfirm(TAvdtpTransactionLabel aLabel,
										  TInt aResult,
										  const TAvdtpInternalDiscoverConfirm* const aConfirm)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);	
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpDiscover);
		
	transaction->User()->DiscoverConfirm(aResult, aConfirm);
	
	RemoveTransaction(*transaction); //clears RTX timer
	}

void CSignallingChannel::GetCapsIndication(TAvdtpTransactionLabel aLabel, TSEID aSEID)
	{
	LOG_FUNC
	// create transaction ready for the response
	CSignallingTransaction* transaction = PrepareSignallingResponse(EResponseAccept, EAvdtpGetCapabilities, aLabel);
	if (transaction)
		{
		// go and ask sessions to contribute to packet
		TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
		TInt result = KErrNotFound;

		while (iter)
			{
			// sessions MUST process this synchronously.
			result = (iter++)->GetCapsIndication(aLabel, aSEID, transaction->Message());
			if (result!=KErrNotFound)
				{
				// some error occured that meant we need not bother asking other sessions
				break;
				}
			}
	
		// ..as we now deem the packet constructed, and ready to send
		if (result==KErrNone)
			{
			(void)EnqueueMessage(*transaction);
			// if Enque failed (it can't at present for Responses) remote would timeout
			}
		else
			{
			delete transaction;
			
			if (result==KErrNotFound)
				{
				SendReject(aLabel, EAvdtpGetCapabilities, EAvdtpBadACPSEID);
				}
			// else dump, and leave to time out				
			}
		}
	else
		{
		// leave to timeout - no apt error
		}
	}

void CSignallingChannel::GetCapsConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, const HBufC8* aCaps/*=NULL*/)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpGetCapabilities);
	
	TSEID seid(reinterpret_cast<TUint>(transaction->Cookie()));
	if (aResult==KErrNone)
		{
		//Parse the SEID info, updating the protocol's sep cache and
		//generating a descriptor to pass to the client.
		
		// tricky to get this to be efficient
		// we just tell cache - it'll parse if it chooses to :o)
		iProtocol.RemoteSEPCache().SetCapabilities(iRemoteAddress,
												   seid,
												   const_cast<HBufC8*>(aCaps));
														   	
			
		// to make life for the user we tell the the categories we did see
		// if they do want to investigate more they can begin a deep parse
		TAvdtpServiceCategories  cats;
		TRAPD(err, cats = iProtocol.RemoteSEPCache().GetCapabilitiesL(
											iRemoteAddress,
											seid));
											
		transaction->User()->GetCapsConfirm(err, seid,cats());
		}
	else
		{
		transaction->User()->GetCapsConfirm(aResult,seid, NULL);
		}
	RemoveTransaction(*transaction); //clears RTX timer
	}


void CSignallingChannel::SetConfigIndication(TAvdtpTransactionLabel aLabel,
											  TSEID aACPSEID,
											  TSEID aINTSEID,
											  RBuf8& aConfigData)
	{		
	LOG_FUNC
	// ownership of aConfigData transferred
	// a consuming session will take owership
	// if it is unclaimed we tidyup
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
	
	while (iter)
		{
		result = (iter++)->SetConfigIndication(aLabel, aACPSEID, aINTSEID, aConfigData);
		if (result!=KErrNotFound)
			{
			break;
			}
		}
			
	if (result!=KErrNone && result!=KErrNoMemory)
		{
		CHECK_ERROR(result);
		TAvdtpSignallingErrorCode code = AvdtpInternalUtils::SymbianErrorToAvdtpError(result);
		
		// as per contract, the session has given extended info back to us
		SendSetConfigurationReject(aLabel, code, static_cast<TAvdtpServiceCategory>(aConfigData[0]));
		
		aConfigData.Close();
		}
	else if (result == KErrNoMemory)
		{
		// if no memory then leave for remote to timeout
		aConfigData.Close();
		}
	}
		

void CSignallingChannel::SetConfigConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, TAvdtpServiceCategory aErrorCategory)
	{
	LOG_FUNC
	ConfigConfirm(aLabel, aResult, aErrorCategory, EFalse);
	}


void CSignallingChannel::GetConfigIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{	
	LOG_FUNC
	// create transaction ready for the response
	CSignallingTransaction* transaction = PrepareSignallingResponse(EResponseAccept, EAvdtpGetConfiguration, aLabel);
	
	if(transaction)
		{
		// go and ask sessions to contribute to packet
		TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
		TInt result = KErrNotFound;

		while (iter)
			{
			// sessions MUST process this synchronously.
			result = (iter++)->GetConfigIndication(aLabel, aACPSEID, transaction->Message());
			if (result!=KErrNotFound)
				{
				break;
				}
			}
		
		// ..as we now deem the packet constructed, and ready to send
		if (result==KErrNone)
			{
			(void)EnqueueMessage(*transaction);
			// leave remote to timeout
			}
		else if (result!=KErrNoMemory)
			{
			delete transaction;
			SendReject(aLabel, EAvdtpGetConfiguration, AvdtpInternalUtils::SymbianErrorToAvdtpError(result));
			}
		// else leave to timeout, no suitable response to send
		}
	// else leave to timeout, no suitable response to send
	}


void CSignallingChannel::GetConfigConfirm(TAvdtpTransactionLabel /*aLabel*/, TInt /*aResult*/)
	{
	LOG_FUNC
	// shouldnt get one of these as we wont send GetConf yet
	// drop
	}


void CSignallingChannel::ReconfigIndication(TAvdtpTransactionLabel aLabel,
										TSEID aACPSEID,
										 RBuf8& aConfigData)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
		
	while (iter)
		{
		result = (iter++)->ReconfigIndication(aLabel, aACPSEID, aConfigData);
		if (result==KErrNone)
			{
			break;
			}
		}
	
	if (result!=KErrNone && result!=KErrNoMemory)
		{
		CHECK_ERROR(result);
	
		TAvdtpSignallingErrorCode code = AvdtpInternalUtils::SymbianErrorToAvdtpError(result);
		// as per contract, the session has given extended info back to us
		SendReconfigureReject(aLabel, code, static_cast<TAvdtpServiceCategory>(aConfigData[0]));
		
		aConfigData.Close();
		}
	else if (result != KErrNone)
		{
		// clean up the config data if we failed - it hasn't been passed on
		aConfigData.Close();
		}
	// else leave remote to timeout
	}

void CSignallingChannel::ReconfigConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, TAvdtpServiceCategory aErrorCategory)
	{
	LOG_FUNC
	ConfigConfirm(aLabel, aResult, aErrorCategory, ETrue);
	}

TInt CSignallingChannel::SendOpenStream(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID)
	{
	LOG_FUNC
	if (!iBearer)
		{
		return KErrNotReady;
		}

	TInt result = KErrNoMemory;
	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpOpen);
	
	if (transaction)
		{
		result = AvdtpSignallingMessageOpen::Command::Format(transaction->Message(), aACPSEID);
		if (result==KErrNone)
			{
			transaction->SetCookie(reinterpret_cast<TAny*>(aACPSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
	return result;
	}
	
void CSignallingChannel::OpenIndication(TAvdtpTransactionLabel aLabel, TSEID aSEID)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
		
	while (iter)
		{
		result = (iter++)->OpenIndication(aSEID); 
		if (result!=KErrNotFound)
			{
			break;
			}
		}	
	
	if (result==KErrNone)
		{
		result = SendAccept(aLabel, EAvdtpOpen);
		}
	else if (result!=KErrNone && result!=KErrNoMemory)
		{
		SendReject(aLabel, EAvdtpOpen, AvdtpInternalUtils::SymbianErrorToAvdtpError(result));
		}

	if (result==KErrNone)
		{
		// now we are happy to pre-authorise remote for more incoming logical channels
		iProtocol.SetPreauthorisation(iRemoteAddress, ETrue);
		}
//#pragma message("would be nice to error session that consumed")
	}


void CSignallingChannel::OpenConfirm(TAvdtpTransactionLabel aLabel, TInt aResult)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpOpen);
			
	transaction->User()->OpenConfirm(aResult, TSEID(reinterpret_cast<TUint>(transaction->Cookie())));
	RemoveTransaction(*transaction); //clears RTX timer
	}


void CSignallingChannel::StartIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
	
	while (iter)
		{
		result = (iter++)->StartIndication(aLabel, aACPSEID);
		if (result!=KErrNotFound)
			{
			break;
			}
		}

	if (result != KErrNone && result!=KErrNoMemory)
		{
		CHECK_ERROR(result);		
		
		// this one has to error slightly differently - we tell remote of the first bad SEID
		// which would be this one.
		TPckg<TUint8> seidPckg(aACPSEID.PacketValue());
		SendReject(aLabel, EAvdtpStart, AvdtpInternalUtils::SymbianErrorToAvdtpError(result), &seidPckg);
		}
	//else leave remote to timeout
	}

void CSignallingChannel::StartConfirm(TAvdtpTransactionLabel aLabel, TInt aResult)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpStart);
	
	transaction->User()->StartConfirm(aResult, TSEID(reinterpret_cast<TUint>(transaction->Cookie())));
	RemoveTransaction(*transaction); //clears RTX timer
	}

void CSignallingChannel::SuspendConfirm(TAvdtpTransactionLabel aLabel, TInt aResult)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpSuspend);
		
	transaction->User()->SuspendConfirm(aResult, TSEID(reinterpret_cast<TUint>(transaction->Cookie())));
	RemoveTransaction(*transaction); //clears RTX timer
	}


void CSignallingChannel::ReleaseIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
	
	while (iter)
		{
		result = (iter++)->ReleaseIndication(aLabel, aACPSEID);
		if (result!=KErrNotFound)
			{
			break;
			}
		}

	// session must send accept otherwise we may send OK after it has released transport channels	
	if (result!=KErrNone && result!=KErrNoMemory)
		{
		result = SendReject(aLabel, EAvdtpRelease, AvdtpInternalUtils::SymbianErrorToAvdtpError(result));
		}
	// else leave remote to timeout
	if (result!=KErrNone)
		{
//	#pragma message("would be nice to just error the session that consumed")
//			iSignallingChannelError(err);
		}
	}

void CSignallingChannel::ReleaseConfirm(TAvdtpTransactionLabel aLabel, TInt aResult)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpRelease);
	
	// We take the details we need and remove transaction before
	// passing the signal on for processing as ReleaseConfirm may
	// lead to the stream, and all its transactions, being 
	// destroyed.
	XAvdtpSignalReceiver* user = transaction->User();
	TSEID seid(reinterpret_cast<TUint>(transaction->Cookie()));
	RemoveTransaction(*transaction); //clears RTX timer

	if (user)
		{
		// it was an "normal" Release - forward confirm
		user->ReleaseConfirm(aResult, seid);
		}
	}


void CSignallingChannel::SuspendIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
	
	while (iter)
		{
		result = (iter++)->SuspendIndication(aLabel, aACPSEID);
		/* if we've found and suspended we don't need to go round again
		   if we've run out of memory or anything then...
		*/
		if (result!=KErrNotFound)
			{
			break;
			}
		}

	/* if we've not broken out then we will come out with notfound
	   which is a bad sepid */
	if (result!=KErrNone && result!=KErrNoMemory)
		{
		CHECK_ERROR(result);		
		
		// this one has to error slightly differently - we tell remote of the first bad SEID
		// which would be this one.
		TPckg<TUint8> seidPckg(aACPSEID.PacketValue());
		SendReject(aLabel, EAvdtpSuspend, AvdtpInternalUtils::SymbianErrorToAvdtpError(result), &seidPckg);
		}
	// else leave remote to timeout
	}


void CSignallingChannel::AbortIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
	
	while (iter)
		{
		result = (iter++)->AbortIndication(aLabel, aACPSEID);
		if (result!=KErrNotFound)
			{
			break;
			}
		}
		
	if (result == KErrNone)
		{
		(void)SendAccept(aLabel, EAvdtpAbort);
		}
	// else no error path for Abort. Just ignore.			
	}

void CSignallingChannel::AbortConfirm(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpAbort);

	// We take the details we need and remove transaction before
	// passing the signal on for processing as AbortConfirm may
	// lead to the stream, and all its transactions, being 
	// destroyed.
	XAvdtpSignalReceiver* user = transaction->User();
	TSEID seid(reinterpret_cast<TUint>(transaction->Cookie()));
	RemoveTransaction(*transaction); //clears RTX timer

	if (user)
		{
		// it was an "immediate" Abort - noone wants confirm
		user->AbortConfirm(seid);
		}
	}


void CSignallingChannel::SecurityControlIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID, const HBufC8* aSecurityData)
	{
	LOG_FUNC
	TDblQueIter<XAvdtpSignalReceiver> iter(iPermanentUsers);
	TInt result = KErrNotFound;
	
	while (iter)
		{
		result = (iter++)->SecurityControlIndication(aLabel, aACPSEID, aSecurityData);
		if (result!=KErrNotFound)
			{
			break;
			}
		}

	if (result!=KErrNone && result!=KErrNoMemory)
		{
		CHECK_ERROR(result);		
		
		// destroy HBuf
		delete (const_cast<HBufC8*>(aSecurityData));
		SendReject(aLabel, EAvdtpSecurityControl, AvdtpInternalUtils::SymbianErrorToAvdtpError(result));
		}
	// else ownership of HBufC8 transferred
	}

void CSignallingChannel::SecurityControlConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, const TDesC8& aResponseData)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);	
	RETURN_IF_SIGNAL_BAD(transaction, EAvdtpSecurityControl);

	transaction->User()->SecurityControlConfirm(aResult, TSEID(reinterpret_cast<TUint>(transaction->Cookie())), aResponseData);
	RemoveTransaction(*transaction); //clears RTX timer
	}


/**
Send primitives
**/


TInt CSignallingChannel::SendDiscoverSEPs(XAvdtpSignalReceiver& aReceiver)
	{
	LOG_FUNC
	if (!iBearer)
		{
		return KErrNotReady;
		}
		
	CSignallingTransaction*	transaction = PrepareSignallingCommand(aReceiver, EAvdtpDiscover);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result=EnqueueMessage(*transaction);
		}
	if (result!=KErrNone)
		{
		delete transaction;
		}
		
	return result;
	}
	
TInt CSignallingChannel::SendGetCapabilities(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID)
	{
	LOG_FUNC
	if (!aACPSEID.IsValid() || aACPSEID.IsLocal())
		{
		return KErrArgument;
		}
	if (!iBearer)
		{
		return KErrNotReady;
		}

	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpGetCapabilities);
	TInt result = KErrNoMemory;

	if (transaction)
		{
		result = AvdtpSignallingMessageGetCapabilities::Command::Format(transaction->Message(), aACPSEID);
		if (result==KErrNone)
			{
			//Stuff the seid in the CSignallingTransaction as AVDTP spec forgets it
			transaction->SetCookie(reinterpret_cast<TAny*>(aACPSEID.Value()));			
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
	return result;
	}
	
TInt CSignallingChannel::SendReleaseAccept(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
    // want to turn this around asap to avoid remote hanging, and races with local l2cap shutting down
    // so one-shot bump the priority

 	if(!iTryToSendCallback->IsActive())
 	    {
		iTryToSendCallback->SetPriority(KAvdtpReleaseAcceptPriority);	
    	}
    else if(iTryToSendCallback->Priority()!=KAvdtpReleaseAcceptPriority)
   		{
   		//Callback is active trying to send a low priority item
   		//Cancel it and set the priority to high. The priority will remain high until the
   		//callback has run enough times to empty the message queue.
   		iTryToSendCallback->Cancel();
    	iTryToSendCallback->SetPriority(KAvdtpReleaseAcceptPriority);
    	}
    //Else, do nothing. The priority will remain high until the message queue has been
    //cleared, so the EAvdtpRelease about to be added should be sent whilst the priority
    //is still high
    	
    return SendAccept(aLabel, EAvdtpRelease);
	}
	
	
TInt CSignallingChannel::SendSetConfigurationAccept(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	return SendAccept(aLabel, EAvdtpSetConfiguration);
	}
									  
TInt CSignallingChannel::SendSetConfigurationReject(TAvdtpTransactionLabel aLabel,
											TBluetoothAvDistributionError aResult,
									  		TAvdtpServiceCategory aCategory)
	{
	LOG_FUNC
	// packet size of errored category is 8bits
	TPckg<TUint8> cat(static_cast<TUint8>(aCategory));
	return SendReject(aLabel, EAvdtpSetConfiguration, aResult, &cat);
	}

TInt CSignallingChannel::SendReconfigureAccept(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	return SendAccept(aLabel, EAvdtpReconfigure);
	}
	
TInt CSignallingChannel::SendReconfigureReject(TAvdtpTransactionLabel aLabel,
											TBluetoothAvDistributionError aResult,
									  		TAvdtpServiceCategory aCategory)
	{
	LOG_FUNC
	// packet size of errored category is 8bits
	TPckg<TUint8> cat(static_cast<TUint8>(aCategory));
	return SendReject(aLabel, EAvdtpReconfigure, aResult, &cat);
	}

TInt CSignallingChannel::SendSecurityControlAccept(TAvdtpTransactionLabel aLabel, const TDesC8* aOptionalData)
	{
	LOG_FUNC
	return SendAccept(aLabel, EAvdtpSecurityControl, aOptionalData);	
	}
	
TInt CSignallingChannel::SendSecurityControlReject(TAvdtpTransactionLabel aLabel, TBluetoothAvDistributionError aResult)
	{
	LOG_FUNC
	return SendReject(aLabel, EAvdtpSecurityControl, aResult, NULL);
	}
	
TInt CSignallingChannel::SendRelease(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID)
	{
	LOG_FUNC
	if (!aACPSEID.IsValid())
		{
		return KErrArgument;
		}
	if (!iBearer)
		{
		return KErrNotReady;
		}

	// create transaction
	TInt result = KErrNoMemory;
	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpRelease);
	
	if (transaction)
		{
		result = AvdtpSignallingMessageStreamRelease::Command::Format(transaction->Message(), aACPSEID);
		if (result==KErrNone)
			{
			transaction->SetCookie(reinterpret_cast<TAny*>(aACPSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
			
	// create packet
	return result;	
	}
	
TInt CSignallingChannel::SendSetConfiguration(XAvdtpSignalReceiver& aReceiver,
										  TSEID aINTSEID,
										  TSEID aACPSEID,
										  const RBuf8& aConfiguration)
	{
	LOG_FUNC
	if (!aACPSEID.IsValid() || !aINTSEID.IsValid())
		{
		return KErrArgument;
		}

	if (!iBearer)
		{
		return KErrNotReady;
		}

	// create transaction
	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpSetConfiguration);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result = AvdtpSignallingMessageSetConfiguration::Command::Format(transaction->Message(), aACPSEID, aINTSEID, aConfiguration);
		if (result==KErrNone)
			{
			transaction->SetCookie(reinterpret_cast<TAny*>(aACPSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
			
	// create packet
	return result;	
	}


TInt CSignallingChannel::SendReconfigure(XAvdtpSignalReceiver& aReceiver,
										  TSEID aACPSEID,
										  const RBuf8& aConfiguration)
	{
	LOG_FUNC
	if (!aACPSEID.IsValid())
		{
		return KErrArgument;
		}
	if (!iBearer)
		{
		return KErrNotReady;
		}

	// create transaction
	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpReconfigure);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result = AvdtpSignallingMessageReconfigure::Command::Format(transaction->Message(), aACPSEID, aConfiguration);
		
		if (result==KErrNone)
			{
			transaction->SetCookie(reinterpret_cast<TAny*>(aACPSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
			
	// create packet
	return result;	
	}


TInt CSignallingChannel::SendStartStream(XAvdtpSignalReceiver& aReceiver, TSEID aRemoteSEID)
	{
	LOG_FUNC
	if (!iBearer)
		{
		return KErrNotReady;
		}

	// create transaction
	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpStart);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result = AvdtpSignallingMessageStart::Command::Format(transaction->Message(), aRemoteSEID);
		if (result==KErrNone)
			{
			transaction->SetCookie(reinterpret_cast<TAny*>(aRemoteSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
			
	// create packet
	return result;	
	}


TInt CSignallingChannel::SendSuspendStream(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID)
	{
	LOG_FUNC
	if (!iBearer)
		{
		return KErrNotReady;
		}

	// create transaction
	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpSuspend);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result=AvdtpSignallingMessageSuspend::Command::Format(transaction->Message(), aACPSEID);
		if (result==KErrNone)
			{
			transaction->SetCookie(reinterpret_cast<TAny*>(aACPSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
			
	// create packet
	return result;	
	}
		


TInt CSignallingChannel::SendAbort(XAvdtpSignalReceiver& aReceiver, TSEID aRemoteSEID)
	{
	LOG_FUNC
	if (!aRemoteSEID.IsValid())
		{
		return KErrArgument;
		}
	if (!iBearer)
		{
		return KErrNotReady;
		}

	CSignallingTransaction*	transaction = PrepareSignallingCommand(aReceiver, EAvdtpAbort);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result = AvdtpSignallingMessageAbort::Command::Format(transaction->Message(), aRemoteSEID);
		if (result==KErrNone)
			{
			//Stuff the seid in the CSignallingTransaction as AVDTP forgets it
			transaction->SetCookie(reinterpret_cast<TAny*>(aRemoteSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
	return result;
	}
	

TInt CSignallingChannel::SendSecurityControl(XAvdtpSignalReceiver& aReceiver, 
									    TSEID aRemoteSEID,
									    const TDesC8& aSecurityData)
	{
	LOG_FUNC
	if (!aRemoteSEID.IsValid())
		{
		return KErrArgument;
		}
	if (!iBearer)
		{
		return KErrNotReady;
		}

	CSignallingTransaction* transaction = PrepareSignallingCommand(aReceiver, EAvdtpSecurityControl);
	TInt result = KErrNoMemory;
	
	if (transaction)
		{
		result = AvdtpSignallingMessageSecurityControl::Command::Format(transaction->Message(), aRemoteSEID, aSecurityData);
			
		if (result==KErrNone)
			{
			//Stuff the seid in the CSignallingTransaction as AVDTP spec forgets it
			transaction->SetCookie(reinterpret_cast<TAny*>(aRemoteSEID.Value()));
			result=EnqueueMessage(*transaction);
			}
		if (result!=KErrNone)
			{
			delete transaction;
			}
		}
		
	return result;
	}
	
	
TInt CSignallingChannel::SendStartAccept(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	return SendAccept(aLabel, EAvdtpStart);
	}
	
TInt CSignallingChannel::SendStartReject(TAvdtpTransactionLabel aLabel,
											TBluetoothAvDistributionError aResult,
											TSEID aBadSEID)
	{
	LOG_FUNC
	TPckg<TUint8> badSEID(aBadSEID.PacketValue());
	return SendReject(aLabel, EAvdtpStart, aResult, &badSEID);
	}
	
TInt CSignallingChannel::SendSuspendAccept(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	return SendAccept(aLabel, EAvdtpSuspend);
	}

TInt CSignallingChannel::SendSuspendReject(TAvdtpTransactionLabel aLabel,
											TBluetoothAvDistributionError aResult,
											TSEID aBadSEID)
	{
	LOG_FUNC
	TPckg<TUint8> badSEID(aBadSEID.PacketValue());
	return SendReject(aLabel, EAvdtpSuspend, aResult, &badSEID);
	}
	
/**
Places a message (defined by the transaction) into the outbound queue
At this point the transaction label for commands is determined
Certain commands (such as Abort) may be placed at the head of the queue - it is this function that decides
If a packet is added to the queue, and the bearer is not blocked, then async send callback is kicked
@internalComponent
**/
TInt CSignallingChannel::EnqueueMessage(CSignallingTransaction& aTransaction)
	{
	LOG_FUNC
	// from transaction get the message
	CAvdtpOutboundSignallingMessage& message = aTransaction.Message();
	
	TInt result=KErrNone;

	//OutBound Signalling message should never be EReserved
	__ASSERT_DEBUG(message.iSignal!= EReserved,Panic(EAvdtpInvalidReservedValueInOutboundSignallingMessage));
	if (message.iMessageType==ECommand)
		{
		result = message.AllocateTransactionLabel(iLabelGen);
		LOG1(_L("Allocated label %d"), message.TransactionLabel());
		}
	
	// remove from draft que, whether error occurred or not
	message.Deque(); 					
	
	if (result==KErrNone)
		{
		// remember the packet label in the transaction object
		aTransaction.SetLabel(message.TransactionLabel());
		// ok to send, figure out priority		
		if (aTransaction.Signal()==EAvdtpAbort)
			{
			// add to outbound queue (high priority)
			iQueuedMessages.AddFirst(message);
			}
		else
			{
			// add to outbound queue	
			iQueuedMessages.AddLast(message);
			}
		// kick off sending
		if (!Blocked())
			{
			StartTryToSendCallback();
			}	
		}
	
	return result;	
	}

/**
Creates a reponse packet
@param aMessageType whether accept or reject
@param aMessage the signalling response to be put in the packet
@param aLabel the transaction label - typically that used in the inbound command
@internalComponent
**/
CSignallingTransaction* CSignallingChannel::PrepareSignallingResponse(
												TAvdtpMessageType aMessageType,
												TAvdtpMessage aMessage,
												TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = PrepareSignallingPacket(aMessageType, aMessage);
	if (transaction)
		{		
		transaction->Message().SetTransactionLabel(aLabel);
		}
	return transaction;
	}

/**
Creates a command packet and returns a transaction to the caller
@param aReceiver a reference to the caller - used for determining who sent the packet
@param aMessage the signalling command to be put in the packet
@internalComponent
**/
CSignallingTransaction* CSignallingChannel::PrepareSignallingCommand(
												XAvdtpSignalReceiver& aReceiver,
												TAvdtpMessage aMessage)
	{
	LOG_FUNC
	CSignallingTransaction* transaction = PrepareSignallingPacket(ECommand, aMessage);
	if (transaction)
		{
		transaction->SetUser(&aReceiver);
		}
	return transaction;
	}


/*
common private method
ownership not returned - transaction remains in signalling channel transaction queue
@internalComponent
*/
CSignallingTransaction* CSignallingChannel::PrepareSignallingPacket(TAvdtpMessageType aMessageType,
													TAvdtpMessage aMessage)
	{
	LOG_FUNC
	//todo: KErrInUse if SAP already preparing a message?
	CSignallingTransaction* transaction = CSignallingTransaction::New(*this,
												aMessage,
												aMessageType);
	
	if (transaction)
		{
		// until the packet is finalised, set to be on draft message queue
		iDraftMessages.AddLast(transaction->Message());
		iTransactions.AddLast(*transaction);
	
		// see what post-send action is required (timer etc - see GAVDP spec)
		transaction->SetSentAction();
		}
	return transaction;
	}

/*
Helper that factors similarity between SetConfiguration and Reconfigure confirms
@param aCategory this may be meaningless if aResult==KErrNone
@internalComponent
*/
void CSignallingChannel::ConfigConfirm(TAvdtpTransactionLabel aLabel,
											  TInt aResult,
											  TAvdtpServiceCategory aCategory,
											  TBool aReconfigure)
	{	
	LOG_FUNC
	CSignallingTransaction* transaction = FindTransaction(aLabel);
	RETURN_IF_NO_TRANSACTION(transaction);
	RETURN_IF_SIGNAL_BAD(transaction, aReconfigure ? EAvdtpReconfigure : EAvdtpSetConfiguration);
	
	// get acp seid from cookie
	TSEID seid(reinterpret_cast<TUint>(transaction->Cookie()));
	
	if (!aReconfigure)
		{
		transaction->User()->SetConfigConfirm(aResult, seid, aCategory);
   		// return label back to pool
   		// for error the setconfigconfirm will destroy the stream which in turn clears the transaction already
   		// don't want to try a double clean up - it doesn't work well :-)
 		// It is possible that SetConfigConfirm was an error and causes stream release
 		// which deletes all the streams transactions - including the one we
 		// just found. To guard against that check it exists (again)
 		transaction = FindTransaction(aLabel);
 		if ((transaction) && (aResult == KErrNone))
			{
			RemoveTransaction(*transaction); //clears RTX timer
			}
		}
	else
		{
		transaction->User()->ReconfigConfirm(aResult, seid, aCategory);
		// return label back to pool
		RemoveTransaction(*transaction); //clears RTX timer
		}	
	}


/**
Sends a rejection response
The response can be to any of the signalling messages.
@param aError The error code to place in the packet
@param aRejectionData Trailing extended error information
@internalComponent
*/
TInt CSignallingChannel::SendReject(TAvdtpTransactionLabel aLabel,
									TAvdtpMessage aMessage,
									TBluetoothAvDistributionError aError,
									const TDesC8* aRejectionData)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iBearer, Panic(EAvdtpSignallingChannelLogicalChannelNotReady));	
	__ASSERT_DEBUG(!(aMessage==EAvdtpDiscover && aError==EAvdtpBadACPSEID), Panic(EAvdtpBadErrorCase));
	
	if (!iBearer)
		{
		return KErrNotReady;
		}
	
	CSignallingTransaction* transaction = NULL;
	TInt result = KErrNone;
	
	//If its a valid signal identifier then send the 'Reject Response' otherwise send 'General Reject' 
	if (aMessage < EAvdtpLargestValidSignalIndentifier)
		{
		transaction = PrepareSignallingResponse(EResponseReject, aMessage, aLabel);	
		// now need to append more data for certain packets													 
		switch (aMessage)
			{
			// those that just have error code
			case EAvdtpDiscover:
			case EAvdtpGetCapabilities:
			case EAvdtpGetConfiguration:
			case EAvdtpOpen:
			case EAvdtpRelease:
			case EAvdtpSecurityControl:
				{
				__ASSERT_DEBUG(aRejectionData == NULL, Panic(EAvdtpSignallingMessageResponseNoTrailingDataExpected));
				result = transaction ? AvdtpSignallingMessage::Reject::Format(transaction->Message(), aError) : KErrNoMemory;
				}
			break;
			
			// those with extra rejection info
			case EAvdtpSetConfiguration:
			case EAvdtpReconfigure:
			case EAvdtpSuspend:
			case EAvdtpStart:
				{
				__ASSERT_DEBUG(aRejectionData != NULL, Panic(EAvdtpSignallingMessageResponseTrailingDataExpected));
				// need to append data to the packet (SEID etc)
				result = transaction ? AvdtpSignallingMessage::Reject::Format(transaction->Message(), aError, aRejectionData) : KErrNoMemory;
				}
			break;
			default:
				break;			// unsupported, just leave to time out
			}
		}
	else
		{
		__ASSERT_DEBUG(aRejectionData == NULL, Panic(EAvdtpSignallingMessageResponseNoTrailingDataExpected));
		//For General Reject send the invalid signal identifier which is causing this general reject	
		transaction = PrepareSignallingResponse(EGeneralReject, aMessage, aLabel);
		}

	if (!transaction)
		{
		result = KErrNoMemory;
		}

	if (result==KErrNone)
		{
		(void)EnqueueMessage(*transaction);
		}
	else
		{
		delete transaction;
		}
	return result;
	}

TInt CSignallingChannel::SendAccept(TAvdtpTransactionLabel aLabel, TAvdtpMessage aMessage, const TDesC8* aOptionalData /*= NULL*/)
	{
	LOG_FUNC
	if (!iBearer)
		{
		return KErrNotReady;
		}
	
	TInt result = KErrNone;
	CSignallingTransaction*	transaction = PrepareSignallingResponse(EResponseAccept, aMessage, aLabel);
	if (transaction)
		{
		if (aOptionalData)
			{
			// no other messages have data in accept in avdtp 1.0
			__ASSERT_DEBUG(aMessage==EAvdtpSecurityControl, Panic(EAvdtpSignallingMessageResponseNoTrailingDataExpected));
			result = AvdtpSignallingMessageSecurityControl::Accept::Format(transaction->Message(), aOptionalData);
			}
		}
	else
		{
		result = KErrNoMemory;
		}
	
	if (result == KErrNone)
		{
		result=EnqueueMessage(*transaction);
		}
	else
		{
		delete transaction;
		}
		
	return result;	
	}

TBool CSignallingChannel::CheckSignal(const CSignallingTransaction& aTransaction, TAvdtpMessage aSignal) const
	{
	LOG_FUNC
	return aTransaction.Signal() == aSignal;
	}


