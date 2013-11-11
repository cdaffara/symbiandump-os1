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
//


#include <commschan.h>
#include <cflog.h>
#include "../inc/chaninternals.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemCommsFwcmsch, "ElemCommsFwcmsch");
#endif

using namespace CommsFW;

CChannelDrain* CChannelDrain::NewL(RCFChannelRawBase& aChannel, RMsgQueueBase* aQueues, TInt aPriorities)
	{
	// CChannelDrain objects have the array of queue drains at the end, making their size dependent
	// upon the number of queues. Hence the use of in-place construction: the type-safe templated
	// approach injects too much code into the modules using it
	const TInt alignedCQueueDrainSize = (sizeof(CQueueDrain) + 3) & ~3;
	const TInt channelDrainObjSize = sizeof(CChannelDrain) + (aPriorities - 1) * alignedCQueueDrainSize;
	CChannelDrain* self = (CChannelDrain*) User::AllocL(channelDrainObjSize);
	(void) new(self) CChannelDrain(aChannel, aQueues, aPriorities);
	for(TInt q = aPriorities - 1; q >= 0; --q)
		{
		if(q > 0)
			{
			// All drains past the first need explicit in-place construction
			(void) new(&self->iDrains[q]) CQueueDrain;
			}
		self->iDrains[q].SetParentChannel(*self);
		CActiveScheduler::Add(&self->iDrains[q]);
		}
	return self;
	}

CChannelDrain::CChannelDrain(RCFChannelRawBase& aChannel, RMsgQueueBase* aQueues, TInt aPriorities)
: iChannel(aChannel),
  iQueues(aQueues),
  iPriorities(aPriorities)
	{
	}
CChannelDrain::~CChannelDrain()
	{
	if(iPriorities > 1)
		{
		for(TInt q = iPriorities - 1; q >= 0; --q)
			{
			CQueueDrain* p = &iDrains[q];
			p->Cancel();
			if(q > 0)
				{
				// All drains past the first need explicit in-place destruction
				p->~CQueueDrain();	//lint !e424	// lint thinks we should use [] on the whole array, but CBase doesn't override new[]+delete[]
				}
			}
		}
	}
	
void CChannelDrain::QueueNotificationCompleted(TInt aStatus)
	{
	if(iClientNotify)
		{
		// NB: NULLs the client notification ptr for us
		User::RequestComplete(iClientNotify, aStatus);
		}
	else
		{
		// Cancels should only arise where the client used RCFChannel::CancelDataAvailable(),
		// in which case we've already cancelled any client request. So here we ignore them
		if(aStatus != KErrCancel)
			{
			++iOutstandingNotifications;
			}
		}
	}
void CChannelDrain::QueueNotificationCancelled(CChannelDrain::CQueueDrain* aCancelQueue)
	{
	TInt q = aCancelQueue - iDrains;
	__ASSERT_DEBUG(q < iPriorities, User::Panic(KCFChannelPanic, ECFChanBadQueue));
	iQueues[q].CancelDataAvailable();
	}

void CChannelDrain::NotifyDataAvailable(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(!iClientNotify, User::Panic(KCFChannelPanic, ECFChanNestedRequest));
	iClientNotify = &aStatus;
	// Check for ability to complete instantly
	if(iOutstandingNotifications > 0)
		{
		// NB: NULLs the client notification ptr for us
		User::RequestComplete(iClientNotify, aStatus.Int());
		--iOutstandingNotifications;
		}
	else
		{
		// Request notification from any queue not already requested
		*iClientNotify = KRequestPending;
		for(TInt q = iPriorities - 1; q >= 0; --q)
			{
			if(!iDrains[q].IsActive())
				{
				iDrains[q].NotifyDataAvailable(iQueues[q]);
				}
			}
		}
	}
void CChannelDrain::CancelDataAvailable()
	{
	iOutstandingNotifications = 0;
	if(iClientNotify)
		{
		User::RequestComplete(iClientNotify, KErrCancel);
 		}
	// Cancel all active notifications
	for(TInt q = iPriorities - 1; q >= 0; --q)
		{
		if(iDrains[q].IsActive())
			{
			iQueues[q].CancelDataAvailable();
			}
		}
	}
			
CChannelDrain::CQueueDrain::CQueueDrain()
: CActive(EPriorityStandard)
	{
	}
		
void CChannelDrain::CQueueDrain::SetParentChannel(CChannelDrain& aChannelDrain)
	{
	iChannelDrain = &aChannelDrain;
	}
void CChannelDrain::CQueueDrain::RunL()
	{
	iChannelDrain->QueueNotificationCompleted(iStatus.Int());
	}
void CChannelDrain::CQueueDrain::DoCancel()
	{
	iChannelDrain->QueueNotificationCancelled(this);
	}
void CChannelDrain::CQueueDrain::NotifyDataAvailable(RMsgQueueBase& aQueue)
	{
	aQueue.NotifyDataAvailable(iStatus);
	SetActive();
	}


//

EXPORT_C RCFChannelRawBase::RCFChannelRawBase()
: iDrain(NULL)
	{
	}

EXPORT_C RCFChannelRawBase::~RCFChannelRawBase()
	{
	__ASSERT_DEBUG(iDrain == 0, User::Panic(KCFChannelPanic, ECFChanOrphanedDrain));
	}

EXPORT_C TInt RCFChannelRawBase::Create(TInt aSlots, TInt aSize, RMsgQueueBase* aQueues, TInt aPriorities)
	{
	__ASSERT_DEBUG(aQueues[0].Handle() == 0, User::Panic(KCFChannelPanic, ECFChanMultiCreate));

	if(aPriorities > 1)
		{
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
		TRAP_IGNORE(iDrain = CChannelDrain::NewL(*this, aQueues, aPriorities));
		if (!iDrain)
			return KErrNoMemory;
#else
		TRAPD(err, (iDrain = CChannelDrain::NewL(*this, aQueues, aPriorities)));
		if (err)
            {
			return err;
            }
#endif
		}
	for(TInt q = 0; q < aPriorities; ++q)
		{
		TInt ret = aQueues[q].CreateLocal(aSlots, aSize);
#ifdef __CFLOG_ACTIVE
		TName nam(RThread().Name());
	    __CFLOG_3(KLogCommsFw, KLogEvent, _L("<%S> created local queue 0x%X with result 0x%X."), &nam, aQueues[q].Handle(), ret);
#endif
		if (ret != KErrNone)
			return ret;
		}
	return KErrNone;
	}

EXPORT_C TInt RCFChannelRawBase::Create(const RMsgQueueBase* aForeignQueues, TOwnerType aOwner, RMsgQueueBase* aQueues, TInt aPriorities)
	{
	__ASSERT_DEBUG(aQueues[0].Handle() == 0, User::Panic(KCFChannelPanic, ECFChanMultiCreate));
	if(aPriorities > 1)
		{
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
		TRAP_IGNORE(iDrain = CChannelDrain::NewL(*this, aQueues, aPriorities));
		if (!iDrain)
			return KErrNoMemory;
#else
		TRAPD(err, (iDrain = CChannelDrain::NewL(*this, aQueues, aPriorities)));
		if (err)
            {
			return err;
            }
#endif
		}
	for(TInt q = 0; q < aPriorities; ++q)
		{
		aQueues[q] = aForeignQueues[q];
		TInt ret = aQueues[q].Duplicate(RThread(), aOwner);
#ifdef __CFLOG_ACTIVE
		TName nam(RThread().Name());
	    __CFLOG_3(KLogCommsFw, KLogEvent, _L("<%S> created duplicated queue 0x%X with result 0x%X."), &nam, aQueues[q].Handle(), ret);
#endif
		if(ret != KErrNone)
			return ret;
		}
	return KErrNone;
	}

EXPORT_C void RCFChannelRawBase::GetMsgQueues(RMsgQueueBase* aForeignQueues, const RMsgQueueBase* aQueues, TInt aPriorities) const
	{
	for(TInt q = 0; q < aPriorities; ++q)
		{
		aForeignQueues[q] = aQueues[q];
#ifdef __CFLOG_ACTIVE
		TName nam(RThread().Name());
	    __CFLOG_2(KLogCommsFw, KLogEvent, _L("<%S> got queue 0x%X."), &nam, aForeignQueues[q].Handle());
#endif
		}
	}

EXPORT_C TBool RCFChannelRawBase::IsOpen(const RMsgQueueBase* aQueues) const
	{
	return aQueues[0].Handle() != 0;
	}


EXPORT_C void RCFChannelRawBase::Close(RMsgQueueBase* aQueues, TInt aPriorities)
	{
	delete iDrain;
	iDrain = NULL;
	for(TInt q = 0; q < aPriorities; ++q)
		{
#ifdef __CFLOG_ACTIVE
		TName nam(RThread().Name());
	    __CFLOG_2(KLogCommsFw, KLogEvent, _L("<%S> closed queue 0x%X."), &nam, aQueues[q].Handle());
#endif
		aQueues[q].Close();
		}
	}

#ifdef __CFLOG_ACTIVE
// Decoding of different message types - note the sweeping assumption that if the
// message size matches that of the standard TCFMessageBase then it should be decoded as
// such. In the fully-templated form this was of course selected by type, but in the absence
// of type info this is about the best _quick_ thing we can do without introducing cost. 
// Future notion: migrate this descriptive ability to the templated class and add a default
// overload (...) for the simple case?

static void DebugDescribeMessage(TDes& aBuf, const TAny* aMsg, TInt aMsgSize)
	{
	if(aMsgSize == sizeof(TCFMessage))
		{
		const TCFMessage* cfMsg = reinterpret_cast<const TCFMessage*>(aMsg);
		aBuf.AppendFormat(_L("Code: %d, Dump: 0x%X 0x%X 0x%X 0x%X"), cfMsg->Code(), 
			((int*)(cfMsg->ParamBlock()))[0], ((int*)(cfMsg->ParamBlock()))[1], 
			((int*)(cfMsg->ParamBlock()))[2], ((int*)(cfMsg->ParamBlock()))[3]);
		}
	else
		{
		aBuf.AppendFormat(_L("Dump: 0x%X 0x%X"), ((int*)(aMsg))[0], ((int*)(aMsg))[1]);
		}
	}

#endif

EXPORT_C TInt RCFChannelRawBase::Send(const TAny* aMsg, TInt aPriority, TInt aSize, RMsgQueueBase* aQueues)
	{
#ifdef __CFLOG_ACTIVE
	TTime homeTime;
	TBuf<16> buf;
	homeTime.HomeTime();
	_LIT(KFormatTxt,"@%Cus");
	TRAPD(res, homeTime.FormatL(buf,KFormatTxt)); 

	res = aQueues[aPriority].Send(aMsg, aSize);

	TName nam(RThread().Name());
	TBuf<128> msgDesc;
	DebugDescribeMessage(msgDesc, aMsg, aSize);
	if(KErrNone == res)
		{
	    __CFLOG_4(KLogCommsFw, KLogMsg, _L("TX MSG %S on 0x%X in <%S>. %S"),
			&buf, aQueues[aPriority].Handle(), &nam, &msgDesc);
		}
	else
		{
	    __CFLOG_5(KLogCommsFw, KLogMsg, _L("TX MSG %S on 0x%X in <%S> FAILED with result %d. %S"), 
			&buf, aQueues[aPriority].Handle(), &nam, res, &msgDesc);
		}
	return res;
#else
	return aQueues[aPriority].Send(aMsg, aSize);
#endif
	}

EXPORT_C void RCFChannelRawBase::NotifySpaceAvailable(TRequestStatus& aStatus, TInt aPriority, RMsgQueueBase* aQueues)
	{
	__CFLOG_2(KLogCommsFw, KLogMsg, _L("NotifySpaceAvailable: status 0x%X, priority %d"), &aStatus, aPriority);
	aStatus = KRequestPending;
	aQueues[aPriority].NotifySpaceAvailable(aStatus);
	}

EXPORT_C void RCFChannelRawBase::CancelSpaceAvailable(TInt aPriority,  RMsgQueueBase* aQueues)
	{
	__CFLOG_1(KLogCommsFw, KLogMsg, _L("CancelSpaceAvailable: priority %d"), &aPriority);
	aQueues[aPriority].CancelSpaceAvailable();
	}

EXPORT_C TInt RCFChannelRawBase::Receive(TAny* aMsg, TInt aSize, RMsgQueueBase* aQueues, TInt aPriorities)
	{
	// Find the highest-priority waiting message
	TInt ret = KErrUnderflow;
	for(TInt q = aPriorities - 1; ret != KErrNone && q >= 0; --q)
		{
		ret = aQueues[q].Receive(aMsg, aSize);
#ifdef __CFLOG_ACTIVE
		TName nam(RThread().Name());
		TTime homeTime;
		TBuf<16> buf;
		homeTime.HomeTime();
		_LIT(KFormatTxt,"@%Cus");
		TRAP_IGNORE(homeTime.FormatL(buf,KFormatTxt));
		TBuf<128> msgDesc;
		DebugDescribeMessage(msgDesc, aMsg, aSize);
		if(KErrNone == ret)
			{
			__CFLOG_4(KLogCommsFw, KLogMsg, _L("RX MSG %S on 0x%X in <%S>. %S"), 
					  &buf, aQueues[q].Handle(), &nam, &msgDesc);
			}
		else
			{
			__CFLOG_4(KLogCommsFw, KLogMsg, _L("RX MSG %S on 0x%X FAILED in <%S> with result %d."), &buf, aQueues[q].Handle(), &nam, ret);
			}
#endif
		}
	return ret;
	}

EXPORT_C void RCFChannelRawBase::NotifyDataAvailable(TRequestStatus& aStatus, RMsgQueueBase* aQueues)
	{
	__CFLOG_1(KLogCommsFw, KLogMsg, _L("NotifyDataAvailable: status 0x%X"), &aStatus);
	if(!iDrain)
		{
		aStatus = KRequestPending;
		aQueues[0].NotifyDataAvailable(aStatus);
		}
	else
		{
		iDrain->NotifyDataAvailable(aStatus);
		}
	}

EXPORT_C void RCFChannelRawBase::CancelDataAvailable(RMsgQueueBase* aQueues)
	{
	__CFLOG(KLogCommsFw, KLogMsg, _L("CancelDataAvailable"));
	if(!iDrain)
		{
		aQueues[0].CancelDataAvailable();
		}
	else
		{
		iDrain->CancelDataAvailable();
		}
	}



//



//

EXPORT_C CCFChannelPairRawBase::CCFChannelPairRawBase(TInt aAOPriority, RMsgQueueBase* aRxQueues, RCFChannelRawBase& aRxChannel, 
									RMsgQueueBase* aTxQueues, RCFChannelRawBase& aTxChannel, TInt aSize, TInt aChannelPriorities)
: CActive(aAOPriority),
  iRxQueues(aRxQueues),
  iRxChannel(aRxChannel),
  iTxQueues(aTxQueues),
  iTxChannel(aTxChannel),
  iSize((TUint16) aSize),
  iPriorities((TUint8) aChannelPriorities)
//  iCloseTimer(NULL)
	{
	__ASSERT_DEBUG(aChannelPriorities <= 255, User::Panic(KSpecAssert_ElemCommsFwcmsch, 1));
	}

EXPORT_C CCFChannelPairRawBase::~CCFChannelPairRawBase()
	{
	CCFChannelPairRawBase::DoCancel();
	}

EXPORT_C TInt CCFChannelPairRawBase::CreateRecv(const RMsgQueueBase* aForeignQueues, TOwnerType aOwner)
	{
	return iRxChannel.Create(aForeignQueues, aOwner, iRxQueues, iPriorities);
	}

EXPORT_C TInt CCFChannelPairRawBase::CreateRecv(TInt aSlots)
	{
	return iRxChannel.Create(aSlots, iSize, iRxQueues, iPriorities);
	}

EXPORT_C TInt CCFChannelPairRawBase::CreateSend(const RMsgQueueBase* aForeignQueues, TOwnerType aOwner)
	{
	return iTxChannel.Create(aForeignQueues, aOwner, iTxQueues, iPriorities);
	}

EXPORT_C TInt CCFChannelPairRawBase::CreateSend(TInt aSlots)
	{
	return iTxChannel.Create(aSlots, iSize, iTxQueues, iPriorities);
	}

EXPORT_C TInt CCFChannelPairRawBase::Send(const TAny* aMsg, TInt aPriority)
	{
	__ASSERT_DEBUG(!TxHalted(), User::Panic(KSpecAssert_ElemCommsFwcmsch, 2));
	return iTxChannel.Send(aMsg, aPriority, iSize, iTxQueues);
	}

//void CCFChannelPairRawBase::NotifySpaceAvailable(CActive& aRequester, TInt aPriority)
EXPORT_C void CCFChannelPairRawBase::NotifySpaceAvailable(TRequestStatus& aStatus, TInt aPriority)
	{
	__ASSERT_DEBUG(!TxHalted(), User::Panic(KSpecAssert_ElemCommsFwcmsch, 3));
	iTxChannel.NotifySpaceAvailable(aStatus, aPriority, iTxQueues);
	}

EXPORT_C void CCFChannelPairRawBase::CancelSpaceAvailable(TInt aPriority)
	{
	iTxChannel.CancelSpaceAvailable(aPriority, iTxQueues);
	}

TInt CCFChannelPairRawBase::RawReceive(TAny* aMsg)
	{
	if(RxHalted())
		{
		if(TxHalted())
			return KErrChannelFullyClosed;
		else
			return KErrChannelHalfClosed;
		}

	TInt ret = iRxChannel.Receive(aMsg, iSize, iRxQueues, iPriorities);

	return ret;
	}

EXPORT_C TInt CCFChannelPairRawBase::Receive(TAny* aMsg)
	{
	if(RxHalted())
		{
		if(TxHalted())
			return KErrChannelFullyClosed;
		else
			return KErrChannelHalfClosed;
		}

	TInt ret = iRxChannel.Receive(aMsg, iSize, iRxQueues, iPriorities);
	if(ret == KErrNone)
		{
		// See if we've received a PIG
		if(IsMsgPIG(aMsg))
			{
			HaltRx();
			// If we've already closed Tx then have sent a PIG, hence
			// go to fully-closed state
			if (TxHalted())
				{
				iRxChannel.Close(iRxQueues, iPriorities);
				iTxChannel.Close(iTxQueues, iPriorities);
				ret = KErrChannelFullyClosed;
				}
			else
				{
				ret = KErrChannelHalfClosed;
				}
			}
		}
	return ret;
	}

//void CCFChannelPairRawBase::NotifyDataAvailable(CActive& aRequester)
EXPORT_C void CCFChannelPairRawBase::NotifyDataAvailable(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(!RxHalted(), User::Panic(KSpecAssert_ElemCommsFwcmsch, 4));
	iRxChannel.NotifyDataAvailable(aStatus, iRxQueues);
	}

EXPORT_C void CCFChannelPairRawBase::CancelDataAvailable()
	{
	iRxChannel.CancelDataAvailable(iRxQueues);
	}

EXPORT_C TInt CCFChannelPairRawBase::CloseGracefully(TInt aGracefulCloseTimeout)
	{
	if(!TxHalted())
		{
		TBool sent = SendPIG();
		if(sent && RxHalted())
			{
			iRxChannel.Close(iRxQueues, iPriorities);
			return KErrChannelFullyClosed;
			}
		else
			{
			iCloseTimer = CPeriodic::New(Priority());
			if(!iCloseTimer)
				return KErrNoMemory;
			const TInt KMaxInterval = 2147483647;
			iCloseTimer->Start(aGracefulCloseTimeout, KMaxInterval, TCallBack(TimerElapsed, this));
			}
		}
	else if(RxHalted())
		{
		iRxChannel.Close(iRxQueues, iPriorities);
		return KErrChannelFullyClosed;
		}
	return KErrChannelHalfClosed;
	}

EXPORT_C void CCFChannelPairRawBase::CloseNonGracefully()
	{
	HaltTx();
	if(iTxChannel.IsOpen(iTxQueues))
		iTxChannel.Close(iTxQueues, iPriorities);
	HaltRx();
	if(iRxChannel.IsOpen(iRxQueues))
		{
		iRxChannel.CancelDataAvailable(iRxQueues);
		iRxChannel.Close(iRxQueues, iPriorities);
		}
	}

EXPORT_C void CCFChannelPairRawBase::GetRecvMsgQueues(RMsgQueueBase* aForeignQueues) const
	{
	iRxChannel.GetMsgQueues(aForeignQueues, iRxQueues, iPriorities);
	}

EXPORT_C void CCFChannelPairRawBase::GetSendMsgQueues(RMsgQueueBase* aForeignQueues) const
	{
	iTxChannel.GetMsgQueues(aForeignQueues, iTxQueues, iPriorities);
	}

EXPORT_C TBool CCFChannelPairRawBase::IsMsgPIG(const TAny*)
	{
	// Classes that implement graceful close must override this
	return EFalse;
	}

EXPORT_C TBool CCFChannelPairRawBase::SendPIG()
	{
	// Classes that implement graceful close must override this
	return EFalse;
	}

EXPORT_C TBool CCFChannelPairRawBase::DoSendPIG(const TAny* aMsgPIG)
	{
	__ASSERT_DEBUG(aMsgPIG, User::Panic(KSpecAssert_ElemCommsFwcmsch, 5));
	TInt ret = iTxChannel.Send(aMsgPIG, 0, iSize, iTxQueues);
	if(ret == KErrOverflow && !IsAdded())	// PIG is always lowest priority
		{
		CancelSpaceAvailable(0);
		CActiveScheduler::Add(this);
		SetActive();
		iTxChannel.NotifySpaceAvailable(iStatus, 0, iTxQueues);
		}
	else if(ret == KErrNone)
		{
		HaltTx();
		iTxChannel.Close(iTxQueues, iPriorities);
		}
	return ret == KErrNone;
	}

TInt CCFChannelPairRawBase::TimerElapsed(TAny* aAnyThis)
	{
	CCFChannelPairRawBase* self = reinterpret_cast<CCFChannelPairRawBase*>(aAnyThis);
	self->DoCancel();
	return KErrNone;
	}

EXPORT_C void CCFChannelPairRawBase::RunL()
	{
	// Space available notification while trying to send a PIG
	if(SendPIG())
		{
		Deque();
		iTxChannel.Close(iTxQueues, iPriorities);
		}
	else
		{
		// Failed again; since we just told there was space the only plausible explanation
		// is that we're OOM so descendant class couldn't create the PIG msg. So closing
		// non-gracefully is the best we can do
		CloseNonGracefully();
		}
	if(iCloseTimer)
		{
		delete iCloseTimer;
		iCloseTimer = NULL;
		}
	}

EXPORT_C void CCFChannelPairRawBase::DoCancel()
	{
	if(iCloseTimer)
		{
		delete iCloseTimer;
		iCloseTimer = NULL;
		// Tx is halted once the PIG is sent; if we haven't halted it then we're waiting for space
		if(!TxHalted())
			{
			CancelSpaceAvailable(0);
			}
		else
			{
			CloseNonGracefully();
			}
		}
	}

void CCFChannelPairRawBase::HaltRx()
	{
	iFlags |= KRxHalted;
	}

TBool CCFChannelPairRawBase::RxHalted()
	{
	return iFlags & KRxHalted;
	}

void CCFChannelPairRawBase::HaltTx()
	{
	iFlags |= KTxHalted;
	}

TBool CCFChannelPairRawBase::TxHalted()
	{
	return iFlags & KTxHalted;
	}

