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
//

/**
 @file
 @internalTechnology
*/


#include <cflog.h>
#include <e32hashtab.h>
#include <ecom/ecom.h>
#include <elements/interfacetable.h>
#include "../inc/cftransportmsg.h"
#include "../inc/cfhookinternals.h"
#include <elements/metatype.h>
#include <cfutil.h>
#include <elements/nm_signals.h>
#include <elements/nm_log.h>
#include <elements/nm_address_internal.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemComsFwCfTrnC, "ElemComsFwCfTrnC");
#endif


using namespace CommsFW;
using namespace Messages;
using NetInterfaces::TInterfaceControl;
using Meta::CMetaDataVirtualCtorInPlace;
using Meta::SMetaData;

#ifdef SYMBIAN_TRACE_ENABLE
	_LIT8(KLogTransportSubTag, "cftransport");
#endif

class MMessageDispatcher
	{
public:
	/** Dispatch a received message from the chipper to its destination
	*/
	virtual void DispatchLegacyL(TCFMessage& aMessage, TWorkerId aSenderId, TInt aFirstDispatchLeaveReason) = 0;
	virtual void DispatchInterfaceL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, const TDesC8& aMessage, TInt aFirstDispatchLeaveReason) = 0;
	};

EXPORT_C void CommsFW::Panic(TCFTransportPanics aReason)
	{
	User::Panic(KCFTransportPanic, aReason);
	}


EXPORT_C void TWorkerThreadDataBase::Init(TWorkerId aWorkerId, TBool aHoldingReference)
	{
	iWorkerId = aWorkerId;
	iThreadId = RThread().Id();
	iHeap = &User::Heap();
	iHoldingReference = aHoldingReference;
	if(iHoldingReference)
		{
		VERIFY_RESULT(iThread.Open(iThreadId, EOwnerProcess), KErrNone);
		iHeap->Open();
		}
	}

EXPORT_C void TWorkerThreadDataBase::Clear()
	{
	iWorkerId = KInvalidWorkerId;
	iThreadId = KNullThreadId;
	if(iHoldingReference)
		{
		iThread.Close();
		iHeap->Close();
		}
	iHeap = NULL;
	}


//

const TInt KReassemblyStoreSize = KMaxNumChips * TCFMessage::KMaxRawBlockSize;


// Handling received chips

class TChipReceiver
	{
public:
	TChipReceiver(MMessageDispatcher& aDispatcher, TWorkerId aPeerId);
	TCFMessage& NextChipStore()
		{
		return reinterpret_cast<TCFMessage&>(*const_cast<TUint8*>(iNextChipStore.Ptr()));
		}
	void ProcessChipL(TInt aFirstDispatchLeaveReason);

	TWorkerId ReceiverWorkerId() const
		{
		return iPeerId;
		}

	TInt ChipsRemaining() const {
		return iChipsRemaining;
	}
private:
	void ResetNextChipStore()
		{
		iNextChipStore.Set(const_cast<TUint8*>(iReassemblyStore.Ptr()), iReassemblyStore.MaxLength(), iReassemblyStore.MaxLength());	//@TODO-TG surely a nicer way?
		}
private:
	MMessageDispatcher& iDispatcher;
	TWorkerId iPeerId;
	TInt iChipsRemaining;
	TPtr8 iNextChipStore;
	TBuf8<KReassemblyStoreSize> iReassemblyStore;
	};

TChipReceiver::TChipReceiver(MMessageDispatcher& aDispatcher, TWorkerId aPeerId)
: iDispatcher(aDispatcher),
  iPeerId(aPeerId),
  iChipsRemaining(0),
  iNextChipStore(NULL, 0)	// initialised properly below
	{
	ResetNextChipStore();
	}

void TChipReceiver::ProcessChipL(TInt aFirstDispatchLeaveReason)
	{
	TCFMessage& chip = NextChipStore();
	if(iChipsRemaining == 0)
		{
		//Not currently in a chipped message, so address is valid
		if (TCFLegacyMessagePacker::IsLegacy(chip))
			{
			//Legacy TCFMessage
			iDispatcher.DispatchLegacyL(chip, iPeerId, aFirstDispatchLeaveReason);
			return;
			}

		if(TCFMessage2::ChipsFollowing(chip) == 0)
			{
			//New TCFMessage2, not chipped
			TRuntimeCtxId* postedFrom;
			TRuntimeCtxId* postedTo;;
			TUint8 offset = static_cast<TCFMessage2&>(chip).ParseGetOffset(postedFrom, postedTo);
			TPtrC8 message(chip.RawBlock() + offset, sizeof(TCFMessage2) - offset);
			iDispatcher.DispatchInterfaceL(*postedFrom, *postedTo, message, aFirstDispatchLeaveReason);
			return;
			}

		//New TCFMessage2, chipped
		iChipsRemaining = TCFMessage2::ChipsFollowing(chip) + 1;
		}

	__ASSERT_DEBUG(iChipsRemaining > 0, User::Panic(KSpecAssert_ElemComsFwCfTrnC, 1));	// single-chip messages all handled above
	if(iChipsRemaining > 1)
		{
		iNextChipStore.Set(iNextChipStore.MidTPtr(TCFMessage::KMaxRawBlockSize));
		}
	else
		{
		TCFMessage2& firstChip = *reinterpret_cast<TCFMessage2*>(const_cast<TUint8*>(iReassemblyStore.Ptr()));
		TRuntimeCtxId* postedFrom;
		TRuntimeCtxId* postedTo;;
		TUint8* msgPtr = firstChip.RawBlock() + firstChip.ParseGetOffset(postedFrom, postedTo);
		TPtrC8 message(msgPtr, iNextChipStore.Ptr() + TCFMessage::KMaxRawBlockSize - msgPtr);
		iDispatcher.DispatchInterfaceL(*postedFrom, *postedTo, message, aFirstDispatchLeaveReason);
		// Only once dispatch hasn't left is it proper to reset for the next message
		ResetNextChipStore();
		}
	// Decrement the remaining chip count only once past the possibility of leaving, so that the state is intact for a post-leave redispatch
	--iChipsRemaining;
	}


void MCommsTransportSender::PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage)
	{
	TUint8 buf[CommsFW::KMaxInlineMessageSize];
	TPtr8 args(buf, sizeof(buf));
	TInt err = aMessage.Store(args);
	__ASSERT_ALWAYS(err == KErrNone, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));	// Metadata too big or corrupt


#ifdef SYMBIAN_TRACE_ENABLE
	// Note: this log output is used by SVG tools - careful when changing it
	NM_LOG_START_BLOCK(KLogTransportSubTag, _L8("Post"));
	NM_LOG_ADDRESS_EXT(KLogTransportSubTag, aPostFrom, _L8("From:"));
	NM_LOG_ADDRESS_EXT(KLogTransportSubTag, aPostTo, _L8("To:"));
	NM_LOG_MESSAGE_EXT(KLogTransportSubTag, args, _L8("Msg:"));
	NM_LOG_END_BLOCK(KLogTransportSubTag, KNullDesC8);
#endif

	PostMessage(aPostFrom, aPostTo, TPtrC8(buf, args.Length()));
	}


class MBindingAwareTransportSender : public MCommsTransportSender
	{
public:
	virtual TBool TransportUsesBinding() const = 0;
	virtual void SetDropTransportPending(TBool aDropPending) = 0;
	virtual TBool IsDropTransportPending() const = 0;
	};


#ifndef SYMBIAN_NETWORKING_INTERTHREAD_TRANSPORT_ONLY

// Self-sending

class CTransportSelfSender : public CActive, public MBindingAwareTransportSender
	{
public:
	enum
		{
		KSelfSenderInitialQueueLength = 20, /** Initial size of self-sender queue */
		KSelfSenderQueuePreAllocationGrowthStep = 20, /** Additionl space to reserve each time we pre-allocate some queue space to avoind too much re-allocation */
		KSelfSenderQueueGrowthStep = 20 /** Increment of self-sender queue size when full. PostMessage() functions panic if growth required and fails */
		};

	static CTransportSelfSender* NewL(MMessageDispatcher& aDispatcher, TWorkerId aSelfId);

private:
	//Legacy & helpers
	void PostMessage(const TCFMessage& aMessage);
	void PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
	// From MTransportSender
	virtual TInt ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage);

	//Legacy & helpers
	TInt ImpatientPostMessage(const TCFMessage& aMessage);
	TInt ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);
#endif

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
	void SendMessageSync(const TCFMessage& aMessage);
#endif

public:
	// From MBindingAwareTransportSender
	virtual TBool TransportUsesBinding() const
		{
		return EFalse;
		}
	virtual void SetDropTransportPending(TBool /*aDropPending*/)
		{
		// self-transports aren't droppable
		}
	virtual TBool IsDropTransportPending() const
		{
		return EFalse;
		}
	
	void PreallocateQueueSpaceL(TInt aMinUndeliveredMessages);

	~CTransportSelfSender();
protected:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	CTransportSelfSender(MMessageDispatcher& aDispatcher, TWorkerId aSelfId);
	void ConstructL();
	TInt DoPostMessage(const TCFMessage& aMessage);
	TInt DoPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);
	TInt ProcessMessage(TCFMessage2& aMessage, TInt aFirstDispatchLeaveReason);
	void MaybeTriggerReceipt();
	void ResizeQueueL(TInt aAdditionalChips);
private:
	TChipReceiver iReceiver;
	CCirBuf<TCFMessage>* iBuf;
	};

CTransportSelfSender::CTransportSelfSender(MMessageDispatcher& aDispatcher, TWorkerId aSelfId)
: CActive(EPriorityUserInput),
  iReceiver(aDispatcher, aSelfId)
	{
	}

CTransportSelfSender::~CTransportSelfSender()
	{
	Cancel();
	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("~CTransportSelfSender() - selfQ maxlen = %d"), iBuf->Length() ));
	delete iBuf;
	}

void CTransportSelfSender::ConstructL()
	{
	iBuf = new CCirBuf<TCFMessage>;
	iBuf->SetLengthL(KSelfSenderInitialQueueLength);
	}

CTransportSelfSender* CTransportSelfSender::NewL(MMessageDispatcher& aDispatcher, TWorkerId aSelfId)
	{
	CTransportSelfSender* self = new(ELeave) CTransportSelfSender(aDispatcher, aSelfId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop(self);
	return self;
	}
/*
 * Preemptively allocate self sender queue length, given by aMinUndeliveredMessages.
 * Note the current logic for queue enlargement is rather coarse, preferring linear growth by a fixed amount.
 * A better approach would have been to implement a logarithmic increase for the queue length, which would have saved a bit more memory when the  
 */
void CTransportSelfSender::PreallocateQueueSpaceL(TInt aMinUndeliveredMessages)
	{	
	TInt currentLength = iBuf->Length();
	if (aMinUndeliveredMessages > currentLength)
		{
		TInt resizeBy = aMinUndeliveredMessages - currentLength;
		__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("CTransportSelfSender::PreallocateQueueSpaceL(%d) Trying to enlarge queue from %d to %d chips"), aMinUndeliveredMessages, currentLength, currentLength + resizeBy + KSelfSenderQueuePreAllocationGrowthStep ));
		ResizeQueueL(resizeBy + KSelfSenderQueuePreAllocationGrowthStep);
		__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("CTransportSelfSender::PreallocateQueueSpaceL queue enlargement successful") ));
		}
	}

void CTransportSelfSender::ResizeQueueL(TInt aAdditionalChips)
	{
	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("CTransportSelfSender::ResizeQueueL(%d)"), aAdditionalChips ));
	
	TInt newLen = iBuf->Length() + aAdditionalChips;
	__ASSERT_ALWAYS(newLen > iBuf->Count(), Panic(ECFInvalidQueueSize));
				
	if (iBuf->Count() == 0)
		{
		iBuf->SetLengthL(newLen);
		return;
		}

	// Create a new queue of the requisite size, copy the elements, and swap for the original
	// (there's no safe way to resize in-place).
	CCirBuf<TCFMessage>* newBuf = new (ELeave) CCirBuf<TCFMessage>;
	CleanupStack::PushL(newBuf);
	newBuf->SetLengthL(newLen);
	CleanupStack::Pop(newBuf);
	
	TCFMessage entry;
	while(iBuf->Remove(&entry))
		{
		VERIFY( newBuf->Add(&entry) );
		}
	delete iBuf;
	iBuf = newBuf;
	}

void CTransportSelfSender::PostMessage(const TCFMessage& aMessage)
	{
	TInt ret = DoPostMessage(aMessage);
	if(ret != KErrNone)
		{
		TRAP_IGNORE(ResizeQueueL(KSelfSenderQueueGrowthStep));
		ret = DoPostMessage(aMessage);
		}
	__ASSERT_ALWAYS(ret == KErrNone, Panic(ECFTransPeerDrainFailure));	// true, even though peer is us...
	}

void CTransportSelfSender::PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	__ASSERT_ALWAYS(aPostFrom.Size() + aPostTo.Size() + aMessage.Length() <= CommsFW::KMaxInlineMessageSize, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	TInt ret = DoPostMessage(aPostFrom, aPostTo, aMessage);
	if(ret != KErrNone)
		{
		TRAP_IGNORE(ResizeQueueL(KSelfSenderQueueGrowthStep));
		ret = DoPostMessage(aPostFrom, aPostTo, aMessage);
		}
	__ASSERT_ALWAYS(ret == KErrNone, Panic(ECFTransPeerDrainFailure));	// true, even though peer is us...
	}


#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
/** Needs more testing before deployment
@internalTechnology
*/
TInt CTransportSelfSender::ImpatientPostMessage(const TCFMessage& aMessage)
	{
	return DoPostMessage(aMessage);
	}

/** Needs more testing before deployment
@internalTechnology
*/
TInt CTransportSelfSender::ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	return DoPostMessage(aPostFrom, aPostTo, aMessage);
	}
#endif


TInt CTransportSelfSender::DoPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
#ifdef _DEBUG
	//Do not send a message to a NULL interface, if you need to address a thread, use
	//proper tier manager
	const TNodeId* nid = address_cast<TNodeId>(&aPostTo);
	if (nid && nid->Ptr()==NULL)
		{
		__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("ERROR: CTransportSelfSender::DoPostMessage() posting to NULL") ));
		User::Panic(KCFTransportPanic,ECFTransInvalidCookie);
		}
#endif

	__ASSERT_ALWAYS(aPostFrom.Size() + aPostTo.Size() + aMessage.Length() <= CommsFW::KMaxInlineMessageSize, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));	// Too big for chipper; send as blob
	TPtrC8 chipPtr(aMessage);
	TCFMessage2 msg(chipPtr, aPostFrom, aPostTo);

	if(chipPtr.Size() > 0)
		{
		TUint following = (chipPtr.Size() + TCFMessage::KMaxRawBlockSize - 1) / TCFMessage::KMaxRawBlockSize;
		TCFMessage2::SetChipsFollowing(following, msg);
		}

	const TCFMessage* msgPtr = &msg;
	TInt dataCarried;
	do
		{
		TInt err = DoPostMessage(*msgPtr);
		if(err != KErrNone)
			{
			// If en-queueing fails partway through chipping we can't punt the problem back to the caller to resolve
			// as they see fit since the half-sent sequence of chips is corrupt (will eat some following message or
			// underflow) and there's no (documentedly-safe) way to manipulate the ring buffer so as to erase the sent
			// chips. Our only hope is to try growing the buffer here; if this fails then the only option left is
			// panicking - to do better we need our own Deque so we can remove those chips
			if(msgPtr == &msg)
				{
				__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("ERROR: CTransportSelfSender::DoPostMessage() queue overflow") ));

				// First chip, safe to error
				return KErrOverflow;
				}
			else
				{
				TRAP_IGNORE(ResizeQueueL(KSelfSenderQueueGrowthStep));
				err = DoPostMessage(*msgPtr);
				if(err != KErrNone)
					{
					Panic(ECFTransPeerDrainFailure);	// true, even though peer is us... should try growing ring buf
					}
				}
			}

		dataCarried = Min(chipPtr.Size(), sizeof(*msgPtr));
		// By strict definition all of the (raw) data in a TCFMessage is payload and word-aligned so the following
		// cast must be safe, except when moving on from the final chip, in which case msgPtr could be unaligned but
		// will never be derefenced further since the loop then terminates
		msgPtr = reinterpret_cast<const TCFMessage*>(chipPtr.Ptr());
		chipPtr.Set(chipPtr.Mid(dataCarried));
		} while(dataCarried > 0);
	return KErrNone;
	}

TInt CTransportSelfSender::DoPostMessage(const TCFMessage& aMessage)
	{
#ifdef SYMBIAN_CFTRANSPORT_ALL_OWN_THREAD_MSGS_DIRECT_DISPATCH
#pragma message("!!KLUDGE ALERT!! SYMBIAN_CFTRANSPORT_ALL_OWN_THREAD_MSGS_DIRECT_DISPATCH")
	// The receiver gets its own modifiable copy of the message
	TCFMessage2& msg = iReceiver.NextChipStore();
	msg = aMessage;
	TRAPD(err, iReceiver.ProcessChipL(KErrNone));
	if(err != KErrNone)
		{
		TRAP(err, iReceiver.ProcessChipL(KErrNone));	// we ignore err; this is already their error handler
		}
	return KErrNone;
#else
	if(!iBuf->Add(&aMessage))
		{
		return KErrOverflow;
		}
	if(!IsActive())
		{
		MaybeTriggerReceipt();
		}
	return KErrNone;
#endif
	}

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
void CTransportSelfSender::SendMessageSync(const TCFMessage& aMessage)
	{
	// The receiver gets its own modifiable copy of the message
	iReceiver.NextChipStore() = aMessage;

	TRAPD(err, iReceiver.ProcessChipL(KErrNone));
	if(err != KErrNone)
		{
		TRAP(err, iReceiver.ProcessChipL(KErrNone));	// we ignore err; this is already their error handler
		}
	}
#endif


void CTransportSelfSender::RunL()
	{
	VERIFY_RESULT(iBuf->Remove(&iReceiver.NextChipStore()), 1);
	iReceiver.ProcessChipL(KErrNone);
	MaybeTriggerReceipt();
	}

TInt CTransportSelfSender::RunError(TInt aError)
	{
	TRAPD(err, iReceiver.ProcessChipL(aError));
	if(err != KErrNone)
		{
		// This is a fatal error because a common requirement for handling the first leave is sending
		// an error signal; leaving again suggests that the code is not handling the possibility of a Leave()
		// properly, rather than explicitly opting to ignore it.
		Panic(ECFTransLeaveFromLeaveHandler);
		}
	MaybeTriggerReceipt();
	return KErrNone;
	}

void CTransportSelfSender::DoCancel()
	{
	if(IsActive())
		{
		iStatus = KRequestPending;
		TRequestStatus* selfStatus = &iStatus;
		User::RequestComplete(selfStatus, KErrCancel);
		}
	}

void CTransportSelfSender::MaybeTriggerReceipt()
	{
	if(!IsActive() && iBuf->Count() > 0)
		{
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* selfStatus = &iStatus;
		User::RequestComplete(selfStatus, KErrNone);
		}
	}

#endif

// One per peer binding
class CTransportChipper : public CActive, public MBindingAwareTransportSender
/**
@internalComponent
*/
	{
public:
	enum {
		KMessageSendTimeout = 15 * 1000 * 1000,
		KMessageSendSleepInterval = 500 * 1000
		};

	static CTransportChipper* NewL(TWorkerId aPeerId, const RCFChannel::TMsgQueues& aInboundQueues,
		const RCFChannel::TMsgQueues& aOutboundQueues, MMessageDispatcher& aDispatcher, MWorkerThreadRegister& aThreadRegister);
	~CTransportChipper();

private:
	//Legacy & helpers
	virtual void PostMessage(const TCFMessage& aMessage);
	virtual void PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
	virtual TInt ImpatientPostMessage(const TCFMessage& aMessage);
	virtual TInt ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);
#endif
#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
	/** The caller intends to block for some out-of-band synchronisation mechanism (eg a semaphore) and so dispatch
	must be done immediately as part of this sending call if the peer object is running in the same worker thread, so
	that the recipient can signal the caller if appropriate. Such synchronisation mechanisms are strongly discouraged
	but exist in some legacy code.

	@deprecated
	*/
	virtual void SendMessageSync(const TCFMessage& aMessage);
#endif

public:
	// From MBindingAwareTransportSender
	virtual TBool TransportUsesBinding() const
		{
		return ETrue;
		}
	virtual void SetDropTransportPending(TBool aDropPending)
		{
		iDropTransportPending = aDropPending;
		}
	virtual TBool IsDropTransportPending() const
		{
		return iDropTransportPending;
		}

	inline TWorkerId PeerId() const
		{
		return iReceiver.ReceiverWorkerId();
		}

	inline void SetRequestId(CommsFW::TId aId)
		{
		iRequestId = aId;
		}

	inline CommsFW::TId GetRequestId() const
		{
		return iRequestId;
		}

	inline static TInt LinkOffset()
		{
		return _FOFF(CTransportChipper, iLink);
		}
private:
	CTransportChipper(TWorkerId aPeerId, MMessageDispatcher& aDispatcher, MWorkerThreadRegister& aThreadRegister);
	void ConstructL(const CommsFW::RCFChannel::TMsgQueues& aRxQueues,
					const CommsFW::RCFChannel::TMsgQueues& aTxQueues);
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void RequestDataNotification();
	void DoPostMessage(const TCFMessage& aMessage);
private:
	class TFetchedChip : public CommsFW::TCFMessage2
		{
	public:
		TSglQueLink iLink;
		};
	typedef TSglQue<TFetchedChip> TFetchedChipQue;

	RCFChannelPair iChannelPair;
	MWorkerThreadRegister& iThreadRegister;
	CommsFW::TId iRequestId;
	TFetchedChipQue iFetchedChipQue;
	RTimer iTimer;
	TId iMessageIdentifier; // ID of message from RootServer to Bind/Unbind, used when sending completion
	TBool iDropTransportPending;
	TCFMessage2 iCurrChip;
	TChipReceiver iReceiver;
	TSglQueLink iLink;
	};

CTransportChipper* CTransportChipper::NewL(TWorkerId aPeerId, const RCFChannel::TMsgQueues& aInboundQueues,
		const RCFChannel::TMsgQueues& aOutboundQueues, MMessageDispatcher& aDispatcher, MWorkerThreadRegister& aThreadRegister)
	{
	CTransportChipper* self = new(ELeave) CTransportChipper(aPeerId, aDispatcher, aThreadRegister);
	CleanupStack::PushL(self);
	self->ConstructL(aInboundQueues, aOutboundQueues);
	CleanupStack::Pop(self);
	return self;
	}

CTransportChipper::CTransportChipper(TWorkerId aPeerId, MMessageDispatcher& aDispatcher, MWorkerThreadRegister& aThreadRegister)
: CActive(EPriorityStandard),
  iThreadRegister(aThreadRegister),
  iFetchedChipQue(_FOFF(TFetchedChip, iLink)),
  iReceiver(aDispatcher, aPeerId)
	{
	}

void CTransportChipper::ConstructL(const CommsFW::RCFChannel::TMsgQueues& aRxQueues, const CommsFW::RCFChannel::TMsgQueues& aTxQueues)
	{
	TInt ret;
	if((ret = iChannelPair.CreateRecv(aRxQueues)) != KErrNone ||
	   (ret = iChannelPair.CreateSend(aTxQueues)) != KErrNone ||
	   (ret = iTimer.CreateLocal()) != KErrNone)
		{
		User::Leave(ret);
		}
	CActiveScheduler::Add(this);
	RequestDataNotification();
	}

CTransportChipper::~CTransportChipper()
	{
	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("~CTransportChipper(%d)"), iReceiver.ReceiverWorkerId()));
	Cancel();
	iChannelPair.CloseNonGracefully();
	iTimer.Close();
	}


void CTransportChipper::RunL()
	{
	// Either the inbound queue has signalled data available or the fetched message queue still has entries
	if(iFetchedChipQue.IsEmpty())
		{
		/**
		   Due to a problem with fragmentation happening on a layer above session control, we have to control
		   how fragmented messages are received, as to receive them as normal could, in unfortunate cases,
		   trigger the session control due to the values of the initial bytes of the message.

		   The nicer solution would be to move fragmentation down into the same layer as session control, thus
		   removing the possible conflicts between the two.
		 */
		if (iReceiver.ChipsRemaining() > 0)
			{
			TInt r = iChannelPair.RawReceive(iReceiver.NextChipStore());
			VERIFY_RESULT(r, KErrNone);
			}
		else
			{
			TInt r = iChannelPair.Receive(iReceiver.NextChipStore());
			VERIFY_RESULT(r, KErrNone);
			}
		iReceiver.ProcessChipL(KErrNone);
		}
	else
		{
		TFetchedChip* chip = iFetchedChipQue.First();
		iFetchedChipQue.Remove(*chip);
		iReceiver.NextChipStore() = *chip;
		delete chip;
		iReceiver.ProcessChipL(KErrNone);
		}

	if(iFetchedChipQue.IsEmpty())
		{
		RequestDataNotification();
		}
	else
		{
		// Self-trigger to keep draining the fetched messages
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* selfStatus = &iStatus;
		User::RequestComplete(selfStatus, KErrNone);
		}
	}

TInt CTransportChipper::RunError(TInt aError)
	{
	TRAPD(err, iReceiver.ProcessChipL(aError));	// give the recipient the chance to make whatever amends they might need
	if(err != KErrNone)
		{
		// This is a fatal error because a common requirement for handling the first leave is sending
		// an error signal; leaving again suggests that the code is not handling the possibility of a Leave()
		// properly, rather than explicitly opting to ignore it.
		Panic(ECFTransLeaveFromLeaveHandler);
		}
	RequestDataNotification();	// the show must go on
	return KErrNone;
	}

void CTransportChipper::DoCancel()
	{
 	iChannelPair.CancelDataAvailable();
	}


/**
Makes the object instance active, waiting for incoming data.
*/
void CTransportChipper::RequestDataNotification()
	{
	SetActive();
	iChannelPair.NotifyDataAvailable(*this);
	}

void CTransportChipper::PostMessage(const TCFMessage& aMessage)
	{
	DoPostMessage(aMessage);
	}

void CTransportChipper::PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
#ifdef _DEBUG
	//Do not send a message to a NULL interface, if you need to address a thread, use
	//proper tier manager
	const TNodeId* nid = address_cast<TNodeId>(&aPostTo);
	if (nid && nid->Ptr()==NULL)
		{
		__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("ERROR: CTransportChipper::DoPostMessage() posting to NULL") ));
		User::Panic(KCFTransportPanic,ECFTransInvalidCookie);
		}
#endif

	__ASSERT_ALWAYS(aPostFrom.Size() + aPostTo.Size() + aMessage.Length() <= CommsFW::KMaxInlineMessageSize, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));	// Too big for chipper; send as blob
	__ASSERT_DEBUG(aPostTo.Thread() == PeerId(), User::Panic(KSpecAssert_ElemComsFwCfTrnC, 2));
	TPtrC8 chipPtr(aMessage);
	TCFMessage2 msg(chipPtr, aPostFrom, aPostTo);

	if(chipPtr.Size() > 0)
		{
		TUint following = (chipPtr.Size() + TCFMessage::KMaxRawBlockSize - 1) / TCFMessage::KMaxRawBlockSize;
		TCFMessage2::SetChipsFollowing(following, msg);
		}

	const TCFMessage* msgPtr = &msg;
	TInt dataCarried;
	do
		{
		DoPostMessage(*msgPtr);
		dataCarried = Min(chipPtr.Size(), sizeof(*msgPtr));
		// By strict definition all of the (raw) data in a TCFMessage is payload and word-aligned so the following
		// cast must be safe, except when moving on from the final chip, in which case msgPtr could be unaligned but
		// will never be derefenced further since the loop then terminates
		msgPtr = reinterpret_cast<const TCFMessage*>(chipPtr.Ptr());
		chipPtr.Set(chipPtr.Mid(dataCarried));
		} while(dataCarried > 0);
	}

void CTransportChipper::DoPostMessage(const TCFMessage& aMessage)
	{
	TInt sendRet = iChannelPair.Send(aMessage);
	if(sendRet == KErrOverflow)
		{
		// The peer has allowed their inbound queue to become full. This could be due to:
		//	(a) they aren't being scheduled enough
		//	(b) they're doing a lengthy operation
		//	(c) they're trying to send to us and our inbound queue is full, ie we're mirroring one another
		// For (a) + (b) we start waiting for space on their inbound queue (our outbound) and sleeping during
		// the wait. For (c) we drain our inbound queue to the fetched queue.
		TRequestStatus spaceAvailStatus;
		iChannelPair.NotifySpaceAvailable(spaceAvailStatus);
		TRequestStatus timeoutStatus;
		TFetchedChip* fetched = new TFetchedChip;
		const TInt KWaitLimit = KMessageSendTimeout / KMessageSendSleepInterval;
		TInt sleepCycle = KWaitLimit;
		while(sendRet == KErrOverflow && --sleepCycle >= 0)
			{
			if(fetched == NULL)
				{
				fetched = new TFetchedChip;
				}

			// Fetch any inbound pending and queue for later processing
			while(fetched && iChannelPair.Receive(*fetched) == KErrNone)
				{
				iFetchedChipQue.AddLast(*fetched);
				fetched = new TFetchedChip;
				}

			iTimer.After(timeoutStatus, KMessageSendSleepInterval);
			User::WaitForRequest(spaceAvailStatus, timeoutStatus);
			if(spaceAvailStatus != KRequestPending)
				{
				iTimer.Cancel();
				User::WaitForRequest(timeoutStatus);
				if(spaceAvailStatus == KErrNone)
					{
					sendRet = iChannelPair.Send(aMessage);
					__ASSERT_DEBUG(sendRet != KErrOverflow, User::Panic(KSpecAssert_ElemComsFwCfTrnC, 3));	// only plausible cause is someone else writing to same queue; mortal sin
					}
				}
			}
		if(sendRet == KErrOverflow)
			{
			iChannelPair.CancelSpaceAvailable();
			User::WaitForRequest(spaceAvailStatus);
			// Request the thread registry to terminate the rogue thread - eventually our caller will be notified of their death
			// and can clean-up and unwind as best it can
			iThreadRegister.PanicWorker(PeerId(), KCFTransportPanic, ECFTransPeerDrainFailure);
			}
		delete fetched;
		}
	}

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
/** Needs more testing before deployment
@internalTechnology
*/
TInt CTransportChipper::ImpatientPostMessage(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	__ASSERT_DEBUG(TCFLegacyMessagePacker::WorkerId(aMessage) == PeerId(), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	return iChannelPair.Send(aMessage);
	}

/** Needs more testing before deployment
@internalTechnology
*/
TInt CTransportChipper::ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	__ASSERT_ALWAYS(aPostFrom.Size() + aPostTo.Size() + aMessage.Length() <= CommsFW::KMaxInlineMessageSize, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));	// too big for chipper
	__ASSERT_DEBUG(aPostTo.Thread() == PeerId(), User::Panic(KSpecAssert_ElemComsFwCfTrnC, 4));
	TCFMessage2 msg(aPostFrom, aPostTo, aMessage);
	return iChannelPair.Send(msg);
	}
#endif

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
void CTransportChipper::SendMessageSync(const TCFMessage& aMessage)
	{
	// Between threads there's no need for dispatch to complete before send returns, so just post
	PostMessage(aMessage);
	}
#endif

namespace CommsFW
	{

class CCommsTransportImpl : public CAsyncOneShot, public MCommsTransportSender, public MTransportReceiver, public MMessageDispatcher
	{
	friend class CCommsTransport;
public:
	static CCommsTransportImpl* NewL(MWorkerThreadRegister& aThreadRegister, const CMetaDataVirtualCtorInPlace* aVirtCtor);
	~CCommsTransportImpl();

	// From MTransportReceiver
	virtual void RegisterAddress(TRuntimeCtxId& aCookie);
	virtual void RegisterLegacyInterface(MLegacyMessageReceiver* aLegacyInterface);
	virtual void DeregisterAddress(TRuntimeCtxId& aCookie);
	virtual TInt RegisteredCount() const;
	virtual TInt VerifyDestination(const TRuntimeCtxId& aDestination);

private:
	//Legacy & helpers
	virtual void PostMessage(const TCFMessage& aMessage);
	virtual void PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
	virtual TInt ImpatientPostMessage(const TCFMessage& aMessage);
	virtual TInt ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage);
#endif
#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
	virtual void SendMessageSync(const TCFMessage& aMessage);
#endif

public:
	// Bind and unbind to peer threads
	TInt EstablishTransportToPeer(TWorkerId aPeerId,
		const CommsFW::RCFChannelMsgQueues& aInboundQueues, const CommsFW::RCFChannelMsgQueues& aOutboundQueues);
	TInt DropTransportToPeer(TWorkerId aPeerId);
	TBool PeerReachable(TWorkerId aPeerId) const
		{
		return GetSender(aPeerId) != NULL;
		}

	void SetLastRequestIdConcerningPeer(TWorkerId aPeerId, CommsFW::TId aRequestId)
		{
		static_cast<CTransportChipper*>(GetSenderMandatory(aPeerId))->SetRequestId(aRequestId);
		}

	CommsFW::TId LastRequestIdConcerningPeer(TWorkerId aPeerId) const
		{
		return static_cast<CTransportChipper*>(GetSenderMandatory(aPeerId))->GetRequestId();
		}
	void SetDropTransportPending(TWorkerId aPeerId, TBool aDropPending)
		{
		GetSenderMandatory(aPeerId)->SetDropTransportPending(aDropPending);
		}

	TBool IsDropTransportPending(TWorkerId aPeerId) const
		{
		return GetSenderMandatory(aPeerId)->IsDropTransportPending();
		}
	
	void PreallocateQueueSpaceL(TInt aMinUndeliveredMessages);

	void RunL();	// for deleting chippers posted to death row
private:
	CCommsTransportImpl(MWorkerThreadRegister& aThreadRegister, const CMetaDataVirtualCtorInPlace* aVirtCtor)
		: CAsyncOneShot(EPriorityLow),
		  iChipperDeathRow(CTransportChipper::LinkOffset()),
	      iThreadRegister(aThreadRegister),
		  iVirtCtor(aVirtCtor)
#ifdef SYMBIAN_TRACE_ENABLE
		  , iLastTime(0)
#endif
		{
		}
	void ConstructL();

	MBindingAwareTransportSender* GetSender(TWorkerId aPeerId) const;
	MBindingAwareTransportSender* GetSender(const TRuntimeCtxId& aPeerCookie) const;
	MBindingAwareTransportSender* GetSenderMandatory(TWorkerId aPeerId) const;
	MBindingAwareTransportSender* GetSenderMandatory(const TRuntimeCtxId& aPeerCookie) const;

	// MMessageDispatcher
	virtual void DispatchLegacyL(TCFMessage& aMessage, TWorkerId aSenderId, TInt aFirstDispatchLeaveReason);
	virtual void DispatchInterfaceL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, const TDesC8& aMessage, TInt aFirstDispatchLeaveReason);

	TWorkerId WorkerId() const
		{
		return iSelfThreadId;
		}
	void PurgeDeathRow();
#ifdef SYMBIAN_TRACE_ENABLE
	void FormMarkerString(TDes8& aBuf);
#endif

private:
	typedef TSglQue<CTransportChipper> TChipperList;
	TChipperList iChipperDeathRow;

private:
	MWorkerThreadRegister& iThreadRegister;
	MLegacyMessageReceiver* iLegacyReceiver;
	const CMetaDataVirtualCtorInPlace* iVirtCtor;
	TWorkerId iSelfThreadId;
	TWorkerId iUpperThreadId;
	CTransportChipper** iChippers;

#ifndef SYMBIAN_NETWORKING_INTERTHREAD_TRANSPORT_ONLY
	mutable CTransportSelfSender* iSelfSender;
#endif
	TInt iPointerSalt;
	TInt iRegisteredCount;
#ifdef SYMBIAN_TRACE_ENABLE
	TInt iDispatchCount;
	TTime iLastTime;
	RHashSet<TIntPtr> iIntfRegister;
	TBool iIntfRegisterOOMed:1;
#endif
	};

	} //namespace CommsFW

CCommsTransportImpl::~CCommsTransportImpl()
	{
	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("~CCommsTransportImpl()")));
	PurgeDeathRow();
	for(TWorkerId chipper = iUpperThreadId; chipper != KInvalidWorkerId; --chipper)
		{
		if(DropTransportToPeer(chipper) == KErrNone)
			{
			__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("~CCommsTransportImpl(): chipper for W%d still present"), chipper));
			__DEBUGGER();	// draw attention
			}
		}

#ifndef SYMBIAN_NETWORKING_INTERTHREAD_TRANSPORT_ONLY
	delete iSelfSender;
#endif
	delete iChippers;

#ifdef SYMBIAN_TRACE_ENABLE
	// If we were unable to allocate an entry for an address we can no longer rely on the register containing
	// all remaining interfaces.
	if(!iIntfRegisterOOMed)
		{
		__ASSERT_DEBUG(iRegisteredCount == iIntfRegister.Count(), User::Panic(KSpecAssert_ElemComsFwCfTrnC, 5));
		}

	// Leaked interfaces prevent the transport from unbinding thus preventing a module bound to one or more
	// other modules from unloading gracefully.  This causes long timeouts during the ESock unload process
	// before the unloader forcibly unbinds and unloads the module using the immediate unload type.  If
	// that happens, iRegisteredCount will not be zero and a list of the leaked interfaces will be printed
	// to the log.
	if(iRegisteredCount != 0)
		{
		NM_LOG((KLogTransportSubTag, _L8("~CCommsTransportImpl(): %d interfaces still registered - this will prevent the transport from unbinding:"), iRegisteredCount));

		RHashSet<TIntPtr>::TIter it(iIntfRegister);
		for(it.Next(); it.Current(); it.Next())
			{
			NM_LOG((KLogTransportSubTag, _L8("\t(%08x)"), *it.Current()));
			}

		__CF_BREAK_IF_DEBUGGER();
		}

	iIntfRegister.Close();
#endif
	}

CCommsTransportImpl* CCommsTransportImpl::NewL(MWorkerThreadRegister& aThreadRegister, const CMetaDataVirtualCtorInPlace* aVirtCtor)
	{
	CCommsTransportImpl* self = new (ELeave) CCommsTransportImpl(aThreadRegister, aVirtCtor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCommsTransportImpl::ConstructL()
	{
	iSelfThreadId = iThreadRegister.SelfWorkerId();
	iUpperThreadId = iThreadRegister.UpperBoundWorkerId();
	iChippers = (CTransportChipper**) User::AllocZL(sizeof(CTransportChipper*) * (iUpperThreadId + 1));
#ifndef SYMBIAN_NETWORKING_INTERTHREAD_TRANSPORT_ONLY
	iSelfSender = CTransportSelfSender::NewL(*this, iThreadRegister.SelfWorkerId());
#endif
	}


void CCommsTransportImpl::RegisterAddress(TRuntimeCtxId& aCookie)
	{
	TNodeId& nodeId = address_cast<TNodeId>(aCookie); //This transport implementation currently only registers TNodeId
	__ASSERT_ALWAYS(nodeId.Ptr(), Panic(ECFTransBadRegistration));
	__ASSERT_ALWAYS(nodeId.IsNull(), Panic(ECFInterfaceRegistrationOverwrite));
	++iRegisteredCount;

	// code below relies upon one byte of salt - would be nice to __COMPILE_ASSERT()
	iPointerSalt = ((iPointerSalt + 1) & 0xFF);
#ifdef _DEBUG
	// Cookie salt protects against re-use; zero is magic value to simply match by pointer
	if(iPointerSalt == 0)
		{
		++iPointerSalt;
		}
#endif
	TRuntimeCtxIdOp op(nodeId);
	op.Set(iPointerSalt, iThreadRegister.SelfWorkerId());
//TODO - it would be better to print the whole address (aCookie.Size() bytes starting from &aCookie == aCookie.Printable();)
	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("RegisterItf(%08x) => %08x %08x"), nodeId.Ptr(), *reinterpret_cast<TUint*>(&aCookie), *(reinterpret_cast<TUint*>(&aCookie) + 1)));

#ifdef SYMBIAN_TRACE_ENABLE
	__ASSERT_ALWAYS(!iIntfRegister.Find(TIntPtr(nodeId.Ptr())), Panic(ECFTransBadRegistration));

	if(iIntfRegister.Insert(TIntPtr(nodeId.Ptr())) != KErrNone)
		{
		// If we were unable to allocate an entry for an address we can't rely on the contents of the interface
		// register to identify bad deregistrations.
		iIntfRegisterOOMed = ETrue;
		}
#endif
	}

void CCommsTransportImpl::RegisterLegacyInterface(MLegacyMessageReceiver* aLegacyInterface)
	{
	__ASSERT_ALWAYS(iLegacyReceiver == NULL, User::Panic(KCFTransportPanic, ECFTransDuplicateHandler));	// only one legacy handler can be registered at a time
	iLegacyReceiver = aLegacyInterface;
	}

/** Relinquish an interface cookie. Note that this is also required for cookies other than object handles for
logging, debug support, and code sanity reasons
*/
void CCommsTransportImpl::DeregisterAddress(TRuntimeCtxId& aCookie)
	{
	//This transport implementation currently only registers TNodeId
//TODO - it would be better to print the whole address (aCookie.Size() bytes starting from &aCookie == aCookie.Printable();)
	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("DeregisterItf(%08x) => %08x %08x"), address_cast<TNodeId>(aCookie).Ptr(), *reinterpret_cast<const TUint*>(&aCookie), *(reinterpret_cast<const TUint*>(&aCookie) + 1)));
	--iRegisteredCount;
	__ASSERT_ALWAYS(iRegisteredCount >= 0, Panic(ECFExcessInterfaceDeregistration));
	aCookie.SetNull();

#ifdef SYMBIAN_TRACE_ENABLE
	TNodeId& nodeId = address_cast<TNodeId>(aCookie); //This transport implementation currently only registers TNodeId

	TInt err = iIntfRegister.Remove(TIntPtr(nodeId.Ptr()));

	// Check to make sure the interface register holds all of the registered addresses or else
	// we must not perform the check to make sure the address being deregistered was registered.
	if(!iIntfRegisterOOMed)
		{
		__ASSERT_ALWAYS(err == KErrNone, Panic(ECFInterfaceNotRegistered));
		}
#endif
	}

TInt CCommsTransportImpl::RegisteredCount() const
	{
	return iRegisteredCount;
	}

void CCommsTransportImpl::DispatchLegacyL(TCFMessage& aMessage, TWorkerId aSenderId, TInt aFirstDispatchLeaveReason)
	{
	__ASSERT_ALWAYS(iLegacyReceiver != NULL, User::Panic(KCFTransportPanic, ECFTransLegacyHandlerAbsent));	// no legacy handler registered, or already deregistered with messages from peer still to dequeue

	TCFMessage unpacked = aMessage; //We need to copy the original message since on Leave from this fn the original chip must not be modified
	TCFLegacyMessagePacker::UnPack(unpacked);
	if(aFirstDispatchLeaveReason == KErrNone)
		{
		iLegacyReceiver->DispatchL(unpacked, aSenderId);
		}
	else
		{
		iLegacyReceiver->OnDispatchLeave(unpacked, aSenderId, aFirstDispatchLeaveReason);
		}
	}

TInt CCommsTransportImpl::VerifyDestination(const TRuntimeCtxId& /*aDestination*/)
	{
    return KErrNone;
   	}

void CCommsTransportImpl::DispatchInterfaceL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, const TDesC8& aMessage, TInt aFirstDispatchLeaveReason)
	{
	TBuf8<KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead> msgBuf;
	TPtrC8 params(aMessage);
	TSignalBase* msg = static_cast<TSignalBase*>(iVirtCtor->New(params, msgBuf));	// A panic stemming from here probably means that the concrete T-message class
																						// has too many unserialised members (or that the serialised form is markedly
																						// more compressed. Consider removing members or sending a pointer to object
	__ASSERT_ALWAYS(msg != NULL, Panic(ECFInterfaceNotRegistered));	// No virtual constructor found. Likely causes:
																	//	(1) Forgot to register it through the CMetaDataVirtualCtorInPlace used when constructing the transport
																	//	(2) T-class being dispatched isn't in the virtual ctor table at all
																	//	(3) Ctor table deregistered prematurely
	//TAny* itf = TCookieAccessor(aDestination).Ptr();
    if (aFirstDispatchLeaveReason)
        {
    	msg->Error(aSender, aRecipient, aFirstDispatchLeaveReason);
        }
    else
        {
    	TInt n = VerifyDestination(aRecipient);
    	if (n==KErrNone)
    	    {
#ifdef SYMBIAN_TRACE_ENABLE
            const TInt KMaxMarkerString = sizeof("DispatchMessage w00.0000000000 00:00:00");
            TBuf8<KMaxMarkerString> buf;

            FormMarkerString(buf);

            // Output to utrace log.
        	// Note: this log output is used by SVG tools - careful when changing it
            _LIT8(KLogTransportSubTag, "cftransport");
    	    NM_LOG_START_BLOCK(KLogTransportSubTag, buf);
            NM_LOG_ADDRESS_EXT(KLogTransportSubTag, aSender, _L8("From:"));
            NM_LOG_ADDRESS_EXT(KLogTransportSubTag, aRecipient, _L8("To:"));
            NM_LOG_MESSAGE_EXT(KLogTransportSubTag, aMessage, _L8("Msg:"));
    	    NM_LOG_END_BLOCK(KLogTransportSubTag, KNullDesC8);

    	    // Output to CDU log (only for outputting marker, not message details - see FormMarkerString()).
    		__CFLOG_VAR(( KLogCommsFw, KLogFwMarker, buf));

#endif

        	msg->DispatchL(aSender, aRecipient);
    	    }
#if defined(__CFLOG_ACTIVE)
        else
           	__CFLOG_VAR(( KLogCommsFw, KLogFwTransport, _L8("CCommsTransportImpl::DispatchInterfaceL message not delivered-ignored by transport")));
#endif
        }
	}


#ifdef SYMBIAN_TRACE_ENABLE

void CCommsTransportImpl::FormMarkerString(TDes8& aBuf)
/**
Populate the descriptor passed with a marker string.

We output a unique marker with each dispatched message so that we can match a position in the utrace log
output with a corresponding position in the traditional CDU log output.  This marker is output to both logs.

NOTE: This is a necessary hack because we have to retain the CDU logging as the utrace mechanism does not have enough
functionality to support moving over entirely to utrace.  Also, there is no mechanism in utrace to provide
for time stamps, so this has to be implemented here.

@param aBuf buffer to fill with marker string
*/
	{
	// Add marker string.  This is formed of worker id and an incrementing message dispatch counter
	// (CCommsTransportImpl is per worker).  For example "w6.142".
	_LIT8(KMarkerString, "Dispatch w%d:%d");
	aBuf.AppendFormat(KMarkerString(), WorkerId(), ++iDispatchCount);

	static const TInt64 KTimestampDelta = 100000;	// minimum interval between timestamps
													// number of message dispatches
													// last timestamp output

	// Add timestamp - first time around and subsequently after KTimestampDelta microseconds.  We are really
	// compensating for the fact that utrace does not provide a timestamp as CDU used to do, and this is the
	// most convenient location to do this.
	TTime time;
	time.HomeTime();
	TTimeIntervalMicroSeconds interval;
	if (iLastTime != TTime(0))				// (TTime(0) == first time around)
		{
		interval = time.MicroSecondsFrom(iLastTime);
		}
	else
		{
		interval = KTimestampDelta;
		}

	if (interval >= KTimestampDelta)
		{
	    _LIT8(KTimeString, " %02d:%02d:%02d.%d");
	    iLastTime = time;
	    TDateTime dateTime = time.DateTime();
		const TInt KMicrosecondsPerTenthSecond = 100000;
	    aBuf.AppendFormat(KTimeString(), dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond() / KMicrosecondsPerTenthSecond);
		}
	}

#endif


MBindingAwareTransportSender* CCommsTransportImpl::GetSender(TWorkerId aPeerId) const
	{
	__ASSERT_ALWAYS(aPeerId != KInvalidWorkerId && aPeerId <= iUpperThreadId, User::Panic(KCFTransportPanic, ECFTransInvalidWorkerId));	// peer worker id out of range
#ifndef SYMBIAN_NETWORKING_INTERTHREAD_TRANSPORT_ONLY
	if(aPeerId == WorkerId())
		{
		return iSelfSender;
		}
	else
#endif
		{
		return iChippers[aPeerId];
		}
	}

MBindingAwareTransportSender* CCommsTransportImpl::GetSenderMandatory(TWorkerId aPeerId) const
	{
	MBindingAwareTransportSender* chipper = GetSender(aPeerId);
	__ASSERT_ALWAYS(chipper != NULL, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));		// no transport to peer; use GetSender() to probe for presence
	return chipper;
	}

MBindingAwareTransportSender* CCommsTransportImpl::GetSender(const TRuntimeCtxId& aPeerCookie) const
	{
	return GetSender(aPeerCookie.Thread());
	}

MBindingAwareTransportSender* CCommsTransportImpl::GetSenderMandatory(const TRuntimeCtxId& aPeerCookie) const
	{
	MBindingAwareTransportSender* chipper = GetSender(aPeerCookie);
	__ASSERT_ALWAYS(chipper != NULL, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));		// no transport to peer - stale cookie?
	return chipper;
	}

void CCommsTransportImpl::PostMessage(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	GetSenderMandatory(TCFLegacyMessagePacker::WorkerId(aMessage))->PostMessage(aMessage);
	}

void CCommsTransportImpl::PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	__ASSERT_DEBUG(!aPostTo.IsNull(), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));	// null/uninitialised cookie
	__ASSERT_DEBUG(!aPostFrom.IsNull(), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));	// null/uninitialised cookie
	GetSenderMandatory(aPostTo)->PostMessage(aPostFrom, aPostTo, aMessage);
	}

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
/** Needs more testing before deployment
@internalTechnology
*/
TInt CCommsTransportImpl::ImpatientPostMessage(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	return GetSenderMandatory(TCFLegacyMessagePacker::WorkerId(aMessage))->ImpatientPostMessage(aMessage);
	}

/** Needs more testing before deployment
@internalTechnology
*/
TInt CCommsTransportImpl::ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	__ASSERT_DEBUG(aPostFrom.IsNull(), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));	// null/uninitialised cookie
	__ASSERT_DEBUG(aPostTo.IsNull(), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));	// null/uninitialised cookie
	return GetSenderMandatory(aPeerCookie)->ImpatientPostMessage(aPostFrom, aPostTo, aMessage);
	}
#endif

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
void CCommsTransportImpl::SendMessageSync(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	return GetSenderMandatory(TCFLegacyMessagePacker::WorkerId(aMessage))->SendMessageSync(aMessage);
	}
#endif

TInt CCommsTransportImpl::EstablishTransportToPeer(TWorkerId aPeerId,
		const CommsFW::RCFChannelMsgQueues& aInboundQueues, const CommsFW::RCFChannelMsgQueues& aOutboundQueues)
	{
	TInt err;
	if(!PeerReachable(aPeerId))
		{
		TRAP(err, iChippers[aPeerId] = CTransportChipper::NewL(aPeerId, aInboundQueues, aOutboundQueues, *this, iThreadRegister));
		}
	else
		{
		err = KErrAlreadyExists;
		}
	return err;
	}

TInt CCommsTransportImpl::DropTransportToPeer(TWorkerId aPeerId)
	{
	TInt err = KErrNotFound;	// Tolerating unbind in absence of bind simplifies handling for callers (akin to Close(), Cancel() being safe ops)
	if(WorkerId() != aPeerId)
		{
		CTransportChipper* chipper = static_cast<CTransportChipper*>(GetSender(aPeerId));
		if(chipper)
			{
			iChipperDeathRow.AddLast(*chipper);
			iChippers[aPeerId] = NULL;
			if(!IsActive())
				{
				Call();
				}
			err = KErrNone;
			}
		}
	return err;
	}

void CCommsTransportImpl::RunL()
	{
	PurgeDeathRow();
	}

void CCommsTransportImpl::PurgeDeathRow()
	{
	while(!iChipperDeathRow.IsEmpty())
		{
		CTransportChipper* chipper = iChipperDeathRow.First();
		iChipperDeathRow.Remove(*chipper);
		delete chipper;
		}
	}

void CCommsTransportImpl::PreallocateQueueSpaceL(TInt aMinUndeliveredMessages)
	{
#ifndef SYMBIAN_NETWORKING_INTERTHREAD_TRANSPORT_ONLY
	iSelfSender->PreallocateQueueSpaceL(aMinUndeliveredMessages);
#else
	(void)aSize; // Do nothing
#endif
	}

//

EXPORT_C CCommsTransport* CCommsTransport::NewL(MWorkerThreadRegister& aThreadRegister, const CMetaDataVirtualCtorInPlace* aVirtCtor, CCFTransportHooks* /*aHooksWalker*/)
	{
	CCommsTransport* self = new(ELeave) CCommsTransport;
	CleanupStack::PushL(self);
	self->iImpl = CCommsTransportImpl::NewL(aThreadRegister, aVirtCtor);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCommsTransport::~CCommsTransport()
	{
	delete iImpl;
	}

EXPORT_C void CCommsTransport::RegisterAddress(TRuntimeCtxId& aCookie)
	{
	iImpl->RegisterAddress(aCookie);
	}

EXPORT_C void CCommsTransport::RegisterLegacyInterface(MLegacyMessageReceiver* aLegacyInterface)
	{
	iImpl->RegisterLegacyInterface(aLegacyInterface);
	}

EXPORT_C void CCommsTransport::DeregisterAddress(TRuntimeCtxId& aCookie)
	{
	iImpl->DeregisterAddress(aCookie);
	}

EXPORT_C TInt CCommsTransport::RegisteredCount() const
	{
	return iImpl->RegisteredCount();
	}

EXPORT_C void CCommsTransport::PostMessage(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	iImpl->PostMessage(aMessage);
	}

EXPORT_C void CCommsTransport::PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	iImpl->PostMessage(aPostFrom, aPostTo, aMessage);
	}

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_IMPATIENCE
/** Needs more testing before deployment
@internalTechnology
*/
EXPORT_C TInt CCommsTransport::ImpatientPostMessage(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	return iImpl->ImpatientPostMessage(aMessage);
	}

/** Needs more testing before deployment
@internalTechnology
*/
EXPORT_C TInt CCommsTransport::ImpatientPostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aMessage)
	{
	return iImpl->ImpatientPostMessage(aPostFrom, aPostTo, aMessage);
	}
#endif

#ifdef SYMBIAN_CFTRANSPORT_SUPPORT_SYNC_SEND
EXPORT_C void CCommsTransport::SendMessageSync(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(TCFLegacyMessagePacker::IsLegacy(aMessage), User::Panic(KCFTransportPanic, ECFTransInvalidCookie));
	iImpl->SendMessageSync(aMessage);
	}
#endif

EXPORT_C TInt CCommsTransport::EstablishTransportToPeer(TWorkerId aPeerId,
	const CommsFW::RCFChannelMsgQueues& aInboundQueues, const CommsFW::RCFChannelMsgQueues& aOutboundQueues)
	{
	return iImpl->EstablishTransportToPeer(aPeerId, aInboundQueues, aOutboundQueues);
	}

EXPORT_C TInt CCommsTransport::DropTransportToPeer(TWorkerId aPeerId)
	{
	return iImpl->DropTransportToPeer(aPeerId);
	}

EXPORT_C void CCommsTransport::SetLastRequestIdConcerningPeer(TWorkerId aPeerId, CommsFW::TId aRequestId)
	{
	iImpl->SetLastRequestIdConcerningPeer(aPeerId, aRequestId);
	}

EXPORT_C CommsFW::TId CCommsTransport::LastRequestIdConcerningPeer(TWorkerId aPeerId) const
	{
	return iImpl->LastRequestIdConcerningPeer(aPeerId);
	}

EXPORT_C TBool CCommsTransport::PeerReachable(TWorkerId aPeerId) const
	{
	return iImpl->PeerReachable(aPeerId);
	}

EXPORT_C MTransportSender& CCommsTransport::GetSender()
	{
	__ASSERT_DEBUG(iImpl, User::Panic(KSpecAssert_ElemComsFwCfTrnC, 6));
	return *iImpl;
	}

EXPORT_C MTransportReceiver& CCommsTransport::GetReceiver()
	{
	__ASSERT_DEBUG(iImpl, User::Panic(KSpecAssert_ElemComsFwCfTrnC, 7));
	return *iImpl;
	}

EXPORT_C TWorkerId CCommsTransport::WorkerId() const
	{
	__ASSERT_DEBUG(iImpl, User::Panic(KSpecAssert_ElemComsFwCfTrnC, 8));
	return iImpl->WorkerId();
	}

EXPORT_C void CCommsTransport::SetDropTransportPending(TWorkerId aPeerId, TBool aDropPending)
	{
	iImpl->SetDropTransportPending(aPeerId, aDropPending);
	}

EXPORT_C TBool CCommsTransport::IsDropTransportPending(TWorkerId aPeerId) const
	{
	return iImpl->IsDropTransportPending(aPeerId);
	}

EXPORT_C void CCommsTransport::PreallocateQueueSpaceL(TInt aMinUndeliveredMessages)
	{
	iImpl->PreallocateQueueSpaceL(aMinUndeliveredMessages);
	}

//

TCFMessage2::TCFMessage2(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TDesC8& aData)
	{
	*RawBlock() = 0; //The first sizeof(TInt) contains chips and other data
	TPtr8 buf(RawBlock() + sizeof(TInt), sizeof(*this) - sizeof(TInt));
	__ASSERT_ALWAYS(KErrNone == aPostTo.Store(buf), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	__ASSERT_ALWAYS(KErrNone == aPostFrom.Store(buf), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	__ASSERT_ALWAYS(aData.Size() <= sizeof(*this) - buf.Size(), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	buf.Append(aData);
	}

TCFMessage2::TCFMessage2(TPtrC8& aData, const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo)
	{
	*RawBlock() = 0; //The first sizeof(TInt) contains chips and other data
	TPtr8 buf(RawBlock() + sizeof(TInt), sizeof(*this) - sizeof(TInt));
	__ASSERT_ALWAYS(KErrNone == aPostTo.Store(buf), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	__ASSERT_ALWAYS(KErrNone == aPostFrom.Store(buf), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	TInt dataCarried = Min<TInt>(buf.MaxSize() - buf.Size(), aData.Size());
	buf.Append(aData.Ptr(), dataCarried);
	aData.Set(aData.Mid(dataCarried));
	}

TUint8 TCFMessage2::ParseGetOffset(TRuntimeCtxId*& aPostedFrom, TRuntimeCtxId*& aPostedTo)
	{
	TUint8 offset = sizeof(TInt); //The first sizeof(TInt) contains chips and other data
	aPostedTo = reinterpret_cast<TRuntimeCtxId*>(RawBlock() + offset);
	__ASSERT_ALWAYS(!aPostedTo->IsNull(), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));

	__ASSERT_DEBUG(aPostedTo->Size() < sizeof(*this) - offset, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	__ASSERT_DEBUG(aPostedTo->Size() <= TRuntimeCtxId::KMaxInlineAddressSize, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));

	offset += aPostedTo->Size();
	aPostedFrom = reinterpret_cast<TRuntimeCtxId*>(RawBlock() + offset);

	__ASSERT_DEBUG(aPostedFrom->Size() < sizeof(*this) - offset, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	__ASSERT_DEBUG(aPostedFrom->Size() <= TRuntimeCtxId::KMaxInlineAddressSize, User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	return offset + aPostedFrom->Size();
	}



