// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server event queue handling
// 
//

#include "EVQUEUE.H"

#include "server.h"
#include "wstop.h"
#include "panics.h"
#include "pointer.h"
#include "wstraces.h"

GLREF_D CDebugLogBase *wsDebugLog;

#if defined(_DEBUG)
#define __CHECK_QUEUE() CheckQueue()
#define __ZAP_EVENTS(pointer,len) ZapEvents(pointer,len)
#define __ZAP_EVENT(pointer) ZapEvent(pointer)
#else
#define __CHECK_QUEUE()
#define __ZAP_EVENTS(pointer,len)
#define __ZAP_EVENT(pointer)
#endif

const TInt ECopyBufSize=4;

TDblQue<CEventQueue> CEventQueue::iQueueList(_FOFF(CEventQueue,iLink));
TWsEvent *CEventQueue::iGlobalEventQueue=NULL;
TInt CEventQueue::iGlobalEventQueueSize=0;
TInt CEventQueue::iNumConnections=0;
RMutex CEventQueue::iMutex;
TWsEvent CEventQueue::iNullEvent;

// CEventBase

CEventBase::~CEventBase()
	{
	if (!iEventMsg.IsNull() && CWsTop::ShuttingDown())
		{
		iEventMsg.Complete(KErrServerTerminated);
		}
	}

CEventBase::CEventBase(CWsClient *aOwner) : iWsOwner(aOwner)
	{
	}

void CEventBase::SignalEvent(TInt aCode)
	{
	if (wsDebugLog)
		wsDebugLog->SignalEvent(iWsOwner->ConnectionHandle());
	iEventMsg.Complete(aCode);
	iEventSignalledState|=EEventFlagSignalled;
	}

inline TBool CEventBase::IsEventCancelled()
	{
	return (iEventSignalledState & EEventFlagCancelled);
	}

void CEventBase::CancelRead()
//
// If there is an outstanding read cancel it.
//
	{
	if (!iEventMsg.IsNull())
		{
		iEventMsg.Complete(KErrCancel);
		}
	iEventSignalledState|=EEventFlagCancelled;
	}

void CEventBase::GetData(TAny *aData, TInt aLen)
	{
	if (!(iEventSignalledState&EEventFlagSignalled))
		{
		iWsOwner->PPanic(EWservPanicUnsignalledEventData);
		}
	iEventSignalledState&=~EEventFlagSignalled;
	CWsClient::ReplyBuf(aData,aLen);
	}

void CEventBase::EventReadyCheck()
//
// Queue a read of an event notification
//
	{
	if ((iEventSignalledState&EEventFlagSignalled && !(iEventSignalledState&EEventFlagCancelled)) || !iEventMsg.IsNull())
		{
		iWsOwner->PPanic(EWservPanicReadOutstanding);
		}
	}

void CEventBase::EventReady(const RMessagePtr2& aEventMsg)
	{
	EventReadyCheck();
	iEventMsg=aEventMsg;
	}

// CEventQueue - Queue organisation functions

#if defined(_DEBUG)
void CEventQueue::CheckQueue()
	{
	TDblQueIter<CEventQueue> iter(iQueueList);
	CEventQueue *qptr;
	iter.SetToFirst();
	while((qptr=iter++)!=NULL)
		{
		WS_ASSERT_DEBUG(qptr->iQueueSize>=0 && qptr->iQueueSize<=iGlobalEventQueueSize, EWsPanicCheckEventQueue);
		WS_ASSERT_DEBUG(qptr->iQueueSize==0 || qptr->iHead<qptr->iQueueSize, EWsPanicCheckEventQueue);
		WS_ASSERT_DEBUG(qptr->iCount<=qptr->iQueueSize, EWsPanicCheckEventQueue);
		WS_ASSERT_DEBUG(qptr->iEventPtr>=iGlobalEventQueue && qptr->iEventPtr<=(iGlobalEventQueue+iGlobalEventQueueSize), EWsPanicCheckEventQueue);
		}
	for(TInt index=0;index<iCount;index++)
		{
		TWsEvent *ev=EventPtr(index);
		WS_ASSERT_DEBUG(ev->Type()!=EEventMarkInvalid, EWsPanicCheckEventQueue);
		}
	}

void CEventQueue::ZapEvent(TWsEvent *aTarget)
	{
	aTarget->SetType(EEventMarkInvalid);
	aTarget->SetHandle(555);
	}

void CEventQueue::ZapEvents(TWsEvent *aTarget, TInt aLen)
	{
	for(TInt index=0;index<aLen;index++)
		ZapEvent(aTarget+index);
	}

#endif

void CEventQueue::EventCopy(TWsEvent *aTarget, TWsEvent *aSource, TInt aNumEvents)
	{
	WS_ASSERT_DEBUG(aTarget>=iGlobalEventQueue, EWsPanicEventQueueCopy);
	WS_ASSERT_DEBUG((aTarget+aNumEvents)<=(iGlobalEventQueue+iGlobalEventQueueSize), EWsPanicEventQueueCopy);
	Mem::Copy(aTarget,aSource,aNumEvents*sizeof(TWsEvent));
	}

TInt CEventQueue::RequiredQueueSize(TInt aNumConnections)
	{
	return(aNumConnections*EMinQueueSize+EMaxQueueSize+EExtraQueueSize);
	}

/**
Adjust the Global Queue Size.
@param aNewSize the size for event queue. 
*/
void CEventQueue::AdjustQueueSizeL(TInt aNewSize)
	{
	TWsEvent* oldQ=iGlobalEventQueue;
	if (aNewSize < iGlobalEventQueueSize)
		{
		// Re-alloc wont move the cell down to a lower address, this means once this cell is
		// high up in memory it may never move down again, thus wasting loads of global heap space.
		const CEventQueue* last=iQueueList.Last();
		if ((last->iEventPtr + last->iQueueSize) >= (iGlobalEventQueue + aNewSize))
			{
			return;
			}
		TInt allocSize = aNewSize * sizeof(TWsEvent);
		iGlobalEventQueue=static_cast<TWsEvent*>(User::AllocL(allocSize));
		Mem::Copy(iGlobalEventQueue,oldQ,allocSize);
		User::Free(oldQ);
		}
	else
		{
		iGlobalEventQueue = static_cast<TWsEvent*>(User::ReAllocL(iGlobalEventQueue, aNewSize * sizeof(TWsEvent)));
		}
	__ZAP_EVENTS(iGlobalEventQueue + iGlobalEventQueueSize, aNewSize - iGlobalEventQueueSize); 
	iGlobalEventQueueSize = aNewSize; 
	// coverity[use_after_free]
	TInt diff=(reinterpret_cast<TInt8*>(iGlobalEventQueue)-reinterpret_cast<TInt8*>(oldQ));
	if (diff)
		{
		TDblQueIter<CEventQueue> iter(iQueueList);
		CEventQueue* qptr;
		while((qptr=iter++)!=NULL)
			{
			qptr->iEventPtr=reinterpret_cast<TWsEvent*>(reinterpret_cast<TInt8*>(qptr->iEventPtr)+diff);
			}
		}
	}

void CEventQueue::AddQueueL()
	{
	Wait();
	if ((iNumConnections%EQueueGranularity)==0)
		{
		const TInt newSize = RequiredQueueSize(iNumConnections + EQueueGranularity);
		TRAPD(err,AdjustQueueSizeL(newSize));
		if (err!=KErrNone)
			{
			__CHECK_QUEUE();
			Signal();
			User::Leave(err);
			}
		}
	iNumConnections++;
	if (iQueueList.IsEmpty())
		iEventPtr=iGlobalEventQueue;
	else
		{
		CEventQueue *qptr=iQueueList.Last();
		iEventPtr=qptr->iEventPtr+qptr->iQueueSize;
		}
	iQueueList.AddLast(*this);
	
	//Initialize the queue size to at least EMinQueueSize
	//Check the queue by doing standard queue compression.
	TBool isExpanded = ETrue;
	do {isExpanded = Expand(EEventPriorityLow);} while ((iQueueSize < EMinQueueSize) && isExpanded);
	while (iQueueSize < EMinQueueSize)
		{
		//Cannot get enough spaces by doing standard queue compression,
		//try to grow the global queue.
		TInt sizeRequired = EMinQueueSize - iQueueSize;
		const TInt newSize = iGlobalEventQueueSize + sizeRequired;
		TRAPD(err, AdjustQueueSizeL(newSize));
		if (err != KErrNone)
			{
			//Cannot get enough spaces by growing global queue.
			//try to purge the oldest events from inactive clients.				
			TInt numEventCleared = PurgeInactiveEvents(sizeRequired);
			if (numEventCleared == 0)
				{
				__CHECK_QUEUE();
				Signal();
				User::Leave(err);
				}
			}
		while (doExpand(ECompressNoPurge)) {};
		}
	__CHECK_QUEUE();
	Signal();
	}

void CEventQueue::RemoveQueue()
	{
	Wait();
	if (iEventPtr)	// If this is still NULL this class hasn't been linked into the Q list
		{
		__ZAP_EVENTS(iEventPtr, iQueueSize);
		iLink.Deque();
		if (--iNumConnections==0)
			{
			User::Free(iGlobalEventQueue);
			iGlobalEventQueue=NULL;
			iGlobalEventQueueSize=0;
			}
		else if ((iNumConnections%EQueueGranularity)==0)
			{
			TDblQueIter<CEventQueue> iter(iQueueList);
			CEventQueue* qptr=iter++;
			qptr->Compress(ECompressNoPurge);
			while((qptr=iter++)!=NULL)
				{
				qptr->SqueezeDown();
				}
			const TInt newSize = RequiredQueueSize(iNumConnections);
			TRAP_IGNORE(AdjustQueueSizeL(newSize));
			// Can easily leave as we need to allocate the new smaller queue before freeing
			// the old queue. But if it does it doesn't matter as we'll simply be left with
			// a larger than necessary buffer until the next realloc
			}
		iCount=0;
		}
	__CHECK_QUEUE();
	Signal();
	}

void CEventQueue::IncreaseQueueSize(TInt aNumSpaces)
	{
	if ((iQueueSize+aNumSpaces)>EMaxQueueSize)
		aNumSpaces=EMaxQueueSize-iQueueSize;
	EventCopy(iEventPtr+iHead+aNumSpaces, iEventPtr+iHead, iQueueSize-iHead);
	__ZAP_EVENTS(iEventPtr+iHead, aNumSpaces);
	iQueueSize+=aNumSpaces;
	iHead=(iHead+aNumSpaces)%iQueueSize;
	}

TBool CEventQueue::Expand(TWservEventPriorities aPriority)
	{
	if (iQueueSize==EMaxQueueSize)
		{
		if (aPriority==EEventPriorityHigh)
			{
			Purge();
			if (iCount<iQueueSize)
				return(ETrue);	// Success!
			}
		}
	else if (doExpand(ECompressNoPurge) ||
			  doExpand(ECompressPurge1) ||
			   (aPriority==EEventPriorityHigh && doExpand(ECompressPurge2)))
		return(ETrue);
	return(EFalse);				// Failure
	}

TBool CEventQueue::doExpand(TCompressMode aCompressMode)
	{
	TDblQueIter<CEventQueue> iter(iQueueList);
	iter.SetToLast();
	CEventQueue* qptr=NULL;
	TInt spare=0;
	
	// while loop from last queue till current queue, moving all queues up 
	// to get all the space from between them
	while((qptr=iter)!=this)
		{
		spare=qptr->SqueezeUp();
		if (spare==0)
			qptr->Compress(aCompressMode);
		iter--;
		}

	// current queue, if we have space then expand the same and return
	spare=FollowingGap();
	if (spare>0)
		{
		IncreaseQueueSize(spare);
		__CHECK_QUEUE();
		return(ETrue);
		}

	// while loop from current queue till first queue, looking for a gap 
	// between queues and using the first one it finds
	iter--;
	TInt expanded=0;
	while((qptr=iter)!=NULL)
		{
		expanded=qptr->SqueezeUp();
		if (expanded>0)
			{
			return MoveDownAndExpand(iter, expanded);
			}
		else
			qptr->Compress(aCompressMode);
		iter--;
		}

	// Even by doing all the above if we did not find space then check if first 
	// queue has some space before it. If so then make use of it and movedown all the 
	// queue till the current queue and then use the space for expansion
	iter.SetToFirst();
	qptr=iter;
	if (qptr->iEventPtr>iGlobalEventQueue)
		{
		return MoveDownAndExpand(iter, qptr->iEventPtr-iGlobalEventQueue);
		}

	__CHECK_QUEUE();
	return(EFalse);	// Failed to expand enough room
	}

// This function moves the queue down by given amount and repeats this until
// the current queue and then expand the same
TBool CEventQueue::MoveDownAndExpand(TDblQueIter<CEventQueue> &aIter, TInt aExpand)
	{
	CEventQueue* qptr=NULL;
	FOREVER
		{
		qptr=aIter++;
		qptr->MoveDown(aExpand);
		if (qptr==this)
			{
			IncreaseQueueSize(aExpand);
			__CHECK_QUEUE();
			break;
			}
		}
	return ETrue;
	}

void CEventQueue::MoveDown(TInt aMove)
	{
	if (!aMove)
		{
		return;
		}
	EventCopy(iEventPtr-aMove,iEventPtr,iQueueSize);
	iEventPtr-=aMove;
	}

/*void CEventQueue::LogUpDownEvents(TChar aChar)
	{
	TWsEvent *event;
	TBuf<128> buf;
	buf.Zero();
	buf.Append(aChar);
	buf.Append('#');
	buf.Append('#');
	TBool some=EFalse;
	TInt index;

	for (index=0;index<iCount;index++)
		{
		event=EventPtr(index);
		if (event->Type()==EEventPointer)
			{
			if (event->Pointer()->iType==TPointerEvent::EButton1Down
										|| event->Pointer()->iType==TPointerEvent::EButton1Up)
				{
				some=ETrue;
				if (event->Pointer()->iType==TPointerEvent::EButton1Down)
					buf.Append('D');
				else
					buf.Append('U');
				buf.AppendNum(index);
				}
			}
		}
	if (wsDebugLog)
		wsDebugLog->MiscMessage(ELogImportant,buf);
	}*/

void CEventQueue::Purge()
	{
// Purgable events are:
//	Pointer Up/Down pairs
//  Pointer moves & drags
//  Key messages
//  Key Up/Down pairs
//  Key Ups if not foreground connection
//  Focus lost/gained pairs
//
// Events that must no be purged
//  Key ups for foreground connections queue
//  Lone pointer ups, must be delivered to match preceeding pointer down
//  Lone focus lost/gained messages
//	
	TWsEvent *event;
	TWsEvent *event2;
	TInt index2;
	TInt index=iCount;
	while(index>0)
		{
		event=EventPtr(--index);
		switch(event->Type())
			{
			case EEventPassword:
				break;
			case EEventMarkInvalid:
#if defined(_DEBUG)
				WS_PANIC_DEBUG(EWsPanicCheckEventQueue);
#endif
			case EEventNull:
			case EEventKey:
			case EEventPointerEnter:
			case EEventPointerExit:
			case EEventDragDrop:
breakLoopAndRemoveEvent:
				RemoveEvent(index);
				return;
			case EEventKeyUp:
				if (iQueueList.First()!=this)
					goto breakLoopAndRemoveEvent;
				break;
			case EEventKeyDown:
				if (iQueueList.First()!=this)
					goto breakLoopAndRemoveEvent;
				for (index2=index+1;index2<iCount;index2++)
					{
					event2=EventPtr(index2);
					if (event2->Type()==EEventKeyUp && event2->Key()->iScanCode==event->Key()->iScanCode)
						{
						*event2=iNullEvent;
						goto breakLoopAndRemoveEvent;
						}
					}
				break;
			case EEventModifiersChanged:
				for (index2=index;index2>0;)
					{
					event2=EventPtr(--index2);
					if (event2->Type()==EEventModifiersChanged)
						{
						event->ModifiersChanged()->iChangedModifiers|=event2->ModifiersChanged()->iChangedModifiers;
						index=index2;
						goto breakLoopAndRemoveEvent;
						}
					}
				break;
			case EEventPointerBufferReady:
				CWsPointerBuffer::DiscardPointerMoveBuffer(event->Handle());
				goto breakLoopAndRemoveEvent;
			case EEventFocusLost:
			case EEventFocusGained:
				if ((index+1)<iCount)
					{
					event2=EventPtr(index+1);
					if (event2->Type()==EEventFocusLost || event2->Type()==EEventFocusGained)
						{
						*event2=iNullEvent;
						goto breakLoopAndRemoveEvent;
						}
					}
				break;
			case EEventSwitchOn:
				if ((index+1)<iCount && EventPtr(index+1)->Type()==EEventSwitchOn)
					goto breakLoopAndRemoveEvent;
				break;
			case EEventPointer:
				{
				TPointerEvent::TType upType;
				switch(event->Pointer()->iType)
					{
					case TPointerEvent::EDrag:
					case TPointerEvent::EMove:
					case TPointerEvent::EButtonRepeat:
					case TPointerEvent::ESwitchOn:
						goto breakLoopAndRemoveEvent;
					case TPointerEvent::EButton1Down:
						upType=TPointerEvent::EButton1Up;
						goto purgeDownUp;
					case TPointerEvent::EButton2Down:
						upType=TPointerEvent::EButton2Up;
						goto purgeDownUp;
					case TPointerEvent::EButton3Down:
						upType=TPointerEvent::EButton3Up;
purgeDownUp:			for (index2=index+1;index2<iCount;index2++)
							{
							event2=EventPtr(index2);
							if (event2->Type()==EEventPointer && event2->Handle()==event->Handle() && event2->Pointer()->iType==upType)
								{
								*event2=iNullEvent;
								goto breakLoopAndRemoveEvent;
								}
							}
						WsPointer::UnmatchedDownPurged(upType, event->Handle());
						goto breakLoopAndRemoveEvent;
					case TPointerEvent::EButton1Up:
					case TPointerEvent::EButton2Up:
					case TPointerEvent::EButton3Up:
						break;
					}
				}
				break;
			}
		}
	}

void CEventQueue::PurgePointerEvents()
	{
	TWsEvent *event;
	TWsEvent *event2;
	TInt index2;
	TInt index=iCount;
	while(index>0)
		{
		event=EventPtr(--index);
		switch(event->Type())
			{
			case EEventPointerBufferReady:
				CWsPointerBuffer::DiscardPointerMoveBuffer(event->Handle());
				RemoveEvent(index);
				break;
			case EEventPointer:
				{
				TPointerEvent::TType upType;
				switch(event->Pointer()->iType)
					{
					case TPointerEvent::EDrag:
					case TPointerEvent::EMove:
					case TPointerEvent::EButtonRepeat:
					case TPointerEvent::ESwitchOn:
						RemoveEvent(index);
						break;
					case TPointerEvent::EButton1Down:
						upType=TPointerEvent::EButton1Up;
						goto purgeDownUp2;
					case TPointerEvent::EButton2Down:
						upType=TPointerEvent::EButton2Up;
						goto purgeDownUp2;
					case TPointerEvent::EButton3Down:
						upType=TPointerEvent::EButton3Up;
purgeDownUp2:			for (index2=index+1;index2<iCount;index2++)
							{
							event2=EventPtr(index2);
							if (event2->Type()==EEventPointer && event2->Handle()==event->Handle() && event2->Pointer()->iType==upType)
								{
								*event2=iNullEvent;
								goto purgedUp;
								}
							}
						WsPointer::UnmatchedDownPurged(upType, event->Handle());
purgedUp:				RemoveEvent(index);
						break;
					case TPointerEvent::EButton1Up:
					case TPointerEvent::EButton2Up:
					case TPointerEvent::EButton3Up:
						break;
					}
				}
				break;
			}
		}
	}

/**
Purge requested number of oldest events from inactive event queue.
@param aSizeRequired the total events required to be cleared. 
@return The number of events cleared.
*/
TInt CEventQueue::PurgeInactiveEvents(const TInt& aSizeRequired)
	{
	TInt numEventsCleared = 0;
	CEventQueue* qptr;
	TBool isRemoved;
	do	{
		TDblQueIter<CEventQueue> iter(iQueueList);
		isRemoved = EFalse;
		while ((qptr = iter++) != NULL && (aSizeRequired > numEventsCleared))
			{
			if ((qptr->IsEventCancelled() || (qptr->iEventMsg.IsNull() && !qptr->iEventSignalledState)) &&
				(qptr->iQueueSize > EMinQueueSize))
				{
				// we have a client that is not listening with a size larger than min queue size.
				// so lets remove it's oldest event until the number of removed events meet the requirement.
				qptr->RemoveEvent(0);
				numEventsCleared++;
				isRemoved = ETrue;
				}
			}
		} while ((aSizeRequired > numEventsCleared) && isRemoved);
	return numEventsCleared;
	}

void CEventQueue::Compress(TCompressMode aCompressMode)
	{
//
// The different purge modes are
//
// ECompressNoPurge,	// Don't purge anything
// ECompressPurge1,		// Don't purge foreground queue
// ECompressPurge2,		// Purge all queues
//
	if (aCompressMode==ECompressPurge2 ||
		(this!=iQueueList.First() && aCompressMode==ECompressPurge1))
		Purge();
	TInt compress=iQueueSize-(iCount>EMinQueueSize?iCount:EMinQueueSize);
	if (compress>0)
		{
		compress=(compress+1)/2;	// Compress half the free space in the queue
		TWsEvent *head=EventPtr(0);
		TWsEvent *tail=EventPtr(iCount);
		if (head>tail)
			{
			EventCopy(iEventPtr+compress,iEventPtr,tail-iEventPtr);
			iHead-=compress;
			}
		else
			{
			EventCopy(iEventPtr+compress,head,iCount);
			iHead=0;
			}
		iEventPtr+=compress;
		iQueueSize-=compress;
		}
	}

void CEventQueue::MoveUp(TInt aNumEvents)
	{
	if (!aNumEvents)
		{
		return;
		}
	EventCopy(iEventPtr+aNumEvents,iEventPtr,iQueueSize);
	iEventPtr+=aNumEvents;
	}

TInt CEventQueue::FollowingGap() const
	{
	TDblQueIter<CEventQueue> iter(iQueueList);
	CEventQueue *qptr;
	iter.Set(*(CEventQueue *)this);
	iter++;
	TWsEvent *end;
	if ((qptr=iter)!=NULL)
		end=qptr->iEventPtr;
	else
		end=iGlobalEventQueue+iGlobalEventQueueSize;
	return(end-(iEventPtr+iQueueSize));
	}

TInt CEventQueue::SqueezeUp()
	{
	TInt gap=FollowingGap();
	MoveUp(gap);
	return(gap);
	}

void CEventQueue::SqueezeDown()
	{
	TDblQueIter<CEventQueue> iter(iQueueList);
	iter.Set(*this);
	iter--;
	CEventQueue *qptr=iter;
	if (qptr!=NULL)
		{
		Compress(ECompressNoPurge);
		TInt gap=qptr->FollowingGap();
		MoveDown(gap);
		}
	}

void CEventQueue::MoveToFront()
	{
	if (this==iQueueList.First())
		return;
	Wait();
	CEventQueue *qptr;
	TInt gap=0;
	TDblQueIter<CEventQueue> iter(iQueueList);
	iter.SetToLast();
	while((qptr=iter--)!=NULL)
		{
		if (gap<iQueueSize)
			qptr->Compress(ECompressNoPurge);
		gap=qptr->SqueezeUp();
		}
	if (gap>=iQueueSize)
		EventCopy(iGlobalEventQueue,iEventPtr,iQueueSize);
	else
		{
		EventCopy(iGlobalEventQueue,iEventPtr,gap);
		iEventPtr+=gap;
		TWsEvent copyBuf[ECopyBufSize];	// temp buffer, can copy upto ECopyBufSize events at a time
		TInt eventsToGo=iQueueSize-gap;
		iQueueSize=gap;
		do
			{
			TInt copy=Min(eventsToGo,ECopyBufSize);
			Mem::Copy(&copyBuf[0],iEventPtr,copy*sizeof(TWsEvent));
			iter.Set(*this);
			iter--;
			while((qptr=iter--)!=NULL)
				qptr->MoveUp(copy);
			EventCopy(iGlobalEventQueue+iQueueSize,&copyBuf[0],copy);
			iQueueSize+=copy;
			eventsToGo-=copy;
			iEventPtr+=copy;
			} while(eventsToGo>0);
		}
	iEventPtr=iGlobalEventQueue;
	this->iLink.Deque();
	iQueueList.AddFirst(*this);
	__CHECK_QUEUE();
	Signal();
	}

// CEventQueue

CEventQueue::CEventQueue(CWsClient *aOwner) : CEventBase(aOwner)
	{
	__DECLARE_NAME(_S("CEventQueue"));
	}

CEventQueue::~CEventQueue()
	{
	RemoveQueue();
	}

void CEventQueue::InitStaticsL()
	{
	User::LeaveIfError(iMutex.CreateLocal());
	}

void CEventQueue::DeleteStaticsL()
	{
	iMutex.Close();
	}

void CEventQueue::ConstructL()
	{
	AddQueueL();
	Mem::FillZ(&iNullEvent,sizeof(iNullEvent));
	}

TWsEvent *CEventQueue::EventPtr(TInt index)	
	{
	return(iEventPtr+((iHead+index)%iQueueSize));
	}

TBool CEventQueue::QueueEvent(const TWsEvent &event)
	{
	TWservEventPriorities priority=EEventPriorityLow;
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP
	if (event.Type()==EEventPassword || event.Type()==EEventSwitchOff || 
		event.Type()==EEventKeySwitchOff || event.Type()==EEventRestartSystem)
#else
	if (event.Type()==EEventPassword || event.Type()==EEventSwitchOff || event.Type()==EEventKeySwitchOff)
#endif
		{
		priority=EEventPriorityHigh;
		}
	return(QueueEvent(event,priority));
	}

TBool CEventQueue::CheckRoom()
//
// If the queue is full and room is created return ETrue
//
	{
	TBool ret=EFalse;
	Wait();
	if (iCount==iQueueSize && Expand(EEventPriorityHigh))
		ret=ETrue;
	Signal();
	return(ret);
	}

TBool CEventQueue::QueueEvent(const TWsEvent &event, TWservEventPriorities aPriority)
//
// Queue an event, returns ETrue if queued or delivered, EFalse if the queue was full.
//
	{
	WS_TRACE_SERVER_QUEUEEVENT();
	TBool ret=ETrue;
	Wait();
	if (iCount==iQueueSize && !Expand(aPriority))
		ret=EFalse;
	else
		{
		if (!iEventMsg.IsNull())
			{
			SignalEvent();
			}
		*EventPtr(iCount++)=event;
		}
	Signal();
	return(ret);
	}

TBool CEventQueue::QueueEvent(TUint32 aTarget, TInt aEvent)
	{
	TWsEvent event;
	event.SetType(aEvent);
	event.SetHandle(aTarget);
	event.SetTimeNow();
	return(QueueEvent(event));
	}

void CEventQueue::UpdateLastEvent(const TWsEvent &event)
	{
	WS_ASSERT_DEBUG(iCount>0, EWsPanicQueueUpdateCount);
	Mem::Copy(EventPtr(iCount-1)->EventData(),event.EventData(),TWsEvent::EWsEventDataSize);
	Signal();
	}

void CEventQueue::GetData()
//
// If there is an outstanding event in the queue, reply with it's data and remove it from the Q
//
	{
	if (iCount>0)
		{
		WS_ASSERT_DEBUG((iEventPtr+iHead)->Type()!=EEventMarkInvalid, EWsPanicCheckEventQueue);
		CEventBase::GetData(iEventPtr+iHead,sizeof(*iEventPtr));
		__ZAP_EVENT(iEventPtr+iHead);
		iHead=(iHead+1)%iQueueSize;
		iCount--;
		}
	else
		CEventBase::GetData(&iNullEvent,sizeof(iNullEvent));
	}

void CEventQueue::EventReady(const RMessagePtr2& aEventMsg)
//
// Queue a read of an event notification
//
	{
	EventReadyCheck();
	Wait();
	iEventMsg=aEventMsg;
	if (iCount>0)
		SignalEvent();
	Signal();
	}

void CEventQueue::RemoveEvent(TInt index)
//
// Remove event 'index' in the queue, this event MUST exist in the queue
//
	{
	WS_ASSERT_DEBUG(index < iCount, EWsPanicCheckEventQueue);
	iCount--;
	for(;index<iCount;index++)
		*EventPtr(index)= *EventPtr(index+1);
	__ZAP_EVENT(EventPtr(iCount));
	}

const TWsEvent *CEventQueue::PeekLastEvent()
//
// Return a read only pointer to the last event in the queue (or NULL if no event)
//
	{
	if (iCount==0)
		return(NULL);
	return(EventPtr(iCount-1));
	}

void CEventQueue::Wait()
	{
	iMutex.Wait();
	}

void CEventQueue::Signal()
	{
	iMutex.Signal();
	}

void CEventQueue::WalkEventQueue(EventQueueWalk aFunc, TAny *aFuncParam)
	{
	Wait();
restart:
	for (TInt index=0;index<iCount;index++)
		{
		TWsEvent *event=EventPtr(index);
		switch((aFunc)(aFuncParam,event))
			{
			case EEventQueueWalkDeleteEvent:
				RemoveEvent(index--);
			case EEventQueueWalkOk:
				break;
			case EEventQueueWalkRestart:
				goto restart;
			}
		}
	Signal();
	}
