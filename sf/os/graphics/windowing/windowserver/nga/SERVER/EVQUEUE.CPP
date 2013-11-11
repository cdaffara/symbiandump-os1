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
#include "advancedpointereventhelper.h"
#include "debughelper.h"

GLREF_D CDebugLogBase* wsDebugLog;

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
TWsEvent* CEventQueue::iGlobalEventQueue=NULL;
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

CEventBase::CEventBase(CWsClient* aOwner) : iWsOwner(aOwner)
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

/**
If there is an outstanding read cancel it.
*/
void CEventBase::CancelRead()
	{
	if (!iEventMsg.IsNull())
		{
		iEventMsg.Complete(KErrCancel);
		}
	iEventSignalledState|=EEventFlagCancelled;
	}

void CEventBase::GetData(TAny* aData, TInt aLen)
	{
	if (!(iEventSignalledState&EEventFlagSignalled))
		{
		iWsOwner->PPanic(EWservPanicUnsignalledEventData);
		}
	iEventSignalledState&=~EEventFlagSignalled;
	CWsClient::ReplyBuf(aData,aLen);
	}

/**
Queue a read of an event notification
*/
void CEventBase::EventReadyCheck()
	{
	if ((iEventSignalledState&EEventFlagSignalled && !(iEventSignalledState&EEventFlagCancelled)) || !iEventMsg.IsNull())
		{
#ifdef LOG_WSERV_EVENTS
		RDebug::Printf("_WSEVENT: CEventBase::EventReadyCheck, Event is not ready");
#endif
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
	CEventQueue* qptr;
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
		TWsEvent* ev=EventPtr(index);
		WS_ASSERT_DEBUG(ev->Type()!=EEventMarkInvalid, EWsPanicCheckEventQueue);
		}
	}

void CEventQueue::ZapEvent(TWsEvent* aTarget)
	{
	aTarget->SetType(EEventMarkInvalid);
	aTarget->SetHandle(555);
	}

void CEventQueue::ZapEvents(TWsEvent* aTarget, TInt aLen)
	{
	for(TInt index=0;index<aLen;index++)
		ZapEvent(aTarget+index);
	}

#endif

void CEventQueue::EventCopy(TWsEvent* aTarget, TWsEvent* aSource, TInt aNumEvents)
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
		CEventQueue* qptr=iQueueList.Last();
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

inline void CEventQueue::IncEventPointer(TWsEvent*& aEventPtr)
	{
	// iEventPtr[iQueueSize] is the element beyond the array, used for efficient bounds checking of the circular buffer only, do not access!!
	if(++aEventPtr==&iEventPtr[iQueueSize])
		{
		aEventPtr=iEventPtr;
		}
	}

inline void CEventQueue::DecEventPointer(TWsEvent*& aEventPtr)
	{
	if(aEventPtr--==iEventPtr)
		{
		aEventPtr=&iEventPtr[iQueueSize - 1];
		}
	}

/*
Starting from aEventToPurge searches the queue for matching event by iterating towards end of queue.
Matching event will be a pointer event with the same window handle and pointer number
as aEventToPurge, but will have type aMatchingType. If matching event is found, it will 
be removed from the queue and search will finish.

Search will be stopped if an event of type aSearchTerminator is found with the same pointer number 
as aEventToPurge.

If search is not stopped by aSearchTerminator and matching event is not found, TWsPointer
class is notified that matching event has not been removed, so if it arrives in the future,
TWsPointer class will be able to ignore it.
*/
void CEventQueue::PurgeEventPairs(TWsEvent* aEventToPurge, TPointerEvent::TType aMatchingType, TPointerEvent::TType aSearchTerminator)
		{
		TWsEvent* eventToMatch = aEventToPurge;
		TWsEvent* lastEvent = EventPtr(iCount);
		for(IncEventPointer(eventToMatch);eventToMatch!=lastEvent;IncEventPointer(eventToMatch))
			{
			if ( (eventToMatch->Type()==EEventPointer)  // Must be checked first to ensure it is pointer data checked later
					&&	(TAdvancedPointerEventHelper::PointerNumber(*eventToMatch) == TAdvancedPointerEventHelper::PointerNumber(*aEventToPurge))) // same pointer 
				{
				if ((eventToMatch->Pointer()->iType==aMatchingType) // correct event type
						&& (eventToMatch->Handle()==aEventToPurge->Handle())) // same window
					{
					*eventToMatch=iNullEvent;
					return;
					}
				else if (eventToMatch->Pointer()->iType==aSearchTerminator) // stop searching for mathing type
					{
					return;
					}
				}
			};
		TWsPointer::UnmatchedEventPurged(aMatchingType, aEventToPurge);
		}

/**
Starting from pointer event aBasePointerEvent searches the queue for next pointer event for 
the same pointer by iterating towards end of queue.

@param aBasePointerEvent must be of type EEventPointer
@return Next pointer event in the queue for the same pointer as aBasePointerEvent 
        if it has the same handle as aBasePointerEvent or NULL if it has different handle.
        NULL if there is no next pointer event for the same pointer in the queue.
*/
TWsEvent* CEventQueue::NextPointerEvent(TWsEvent* aBasePointerEvent)
	{
	WS_ASSERT_DEBUG(aBasePointerEvent->Type() == EEventPointer, EWsPanicCheckEventQueue);
	TWsEvent* currentEvent = aBasePointerEvent;
	TWsEvent* lastEvent = EventPtr(iCount);
	TUint8 pointerNumber = TAdvancedPointerEventHelper::PointerNumber(*aBasePointerEvent);
	TUint handle = aBasePointerEvent->Handle();
	for(IncEventPointer(currentEvent);currentEvent!=lastEvent;IncEventPointer(currentEvent))
		{
		if ((currentEvent->Type() == EEventPointer) && 
			(TAdvancedPointerEventHelper::PointerNumber(*currentEvent) == pointerNumber))
			{
			if (currentEvent->Handle() == handle)
				{
				return currentEvent;
				}
			else
				{
				return NULL;
				}
			}
		};
	return NULL;
	}
	
/**
Checks if aEventToPurge should be purged. If it can be purged, then events that should
be purged together with aEventToPurge (matching events) are overwritten with iNullEvent.

@return ETrue if aEventToPurge should be purged, EFalse otherwise.
*/
TBool CEventQueue::CheckPurgePointerEvent(TWsEvent* aEventToPurge)
	{
	switch(aEventToPurge->Pointer()->iType)
		{
		case TPointerEvent::EDrag:
		case TPointerEvent::EMove:
		case TPointerEvent::EButtonRepeat:
		case TPointerEvent::ESwitchOn:
			return ETrue;
		case TPointerEvent::EButton1Down:
			PurgeEventPairs(aEventToPurge,TPointerEvent::EButton1Up, TPointerEvent::ENullType);
			return ETrue;
		case TPointerEvent::EButton2Down:
			PurgeEventPairs(aEventToPurge,TPointerEvent::EButton2Up, TPointerEvent::ENullType);
			return ETrue;
		case TPointerEvent::EButton3Down:
			PurgeEventPairs(aEventToPurge,TPointerEvent::EButton3Up, TPointerEvent::ENullType);
			return ETrue;
		case TPointerEvent::EEnterHighPressure:
			PurgeEventPairs(aEventToPurge,TPointerEvent::EExitHighPressure, TPointerEvent::EButton1Up);
			return ETrue;
		case TPointerEvent::EEnterCloseProximity:
			{
			TWsEvent* nextEvent = NextPointerEvent(aEventToPurge);
			if (nextEvent != NULL)
				{
				switch(nextEvent->Pointer()->iType)
					{
					case TPointerEvent::EExitCloseProximity: 
						*nextEvent = iNullEvent;
					case TPointerEvent::EOutOfRange:
						return ETrue;
					}
				}
			break;
			}
		case TPointerEvent::EExitCloseProximity:
			{
			TWsEvent* nextEvent = NextPointerEvent(aEventToPurge);
			if (nextEvent != NULL)
				{
				switch(nextEvent->Pointer()->iType)
					{
					case TPointerEvent::EEnterCloseProximity: 
						*nextEvent = iNullEvent;
					case TPointerEvent::EOutOfRange:
						return ETrue;
					}
				}
			break;
			}
		case TPointerEvent::EOutOfRange:
			{
			TWsEvent* nextEvent = NextPointerEvent(aEventToPurge);
			if ((nextEvent != NULL) &&
				(nextEvent->Pointer()->iType == TPointerEvent::EOutOfRange))
				{
				return ETrue;
				}
			break;
			}
		case TPointerEvent::EExitHighPressure:
			{
			TWsEvent* nextEvent = NextPointerEvent(aEventToPurge);
			if ((nextEvent != NULL) &&
				(nextEvent->Pointer()->iType == TPointerEvent::EButton1Up))
				{
				return ETrue;
				}
			break;
			}
		case TPointerEvent::EButton1Up:
		case TPointerEvent::EButton2Up:
		case TPointerEvent::EButton3Up:
			break;
		}
	return EFalse;
	}

/** Purgable events are:
	Pointer Up/Down pairs belonging to the same pointer, button and window
    Pointer moves & drags
    Key messages
    Key Up/Down pairs
    Key Ups if not foreground connection
    Focus lost/gained pairs
    EEnterHighPressure/EExitHighPressure pairs belonging to the same pointer
    EEnterCloseProximity/EExitCloseProximity pairs belonging to the same pointer
    Lone EEnterHighPressure, EExitHighPressure if followed by Up for the same pointer
    Lone EEnterCloseProximity, EExitCloseProximity if followed by EOutOfRange for the same pointer
    EOutOfRange if followed by another EOutOfRange for the same pointer

    Events that must not be purged:
    Key ups for foreground connections queue
    Lone pointer ups, must be delivered to match preceeding pointer down
    Lone EExitHighPressure if not followed by Up, must be delivered to match preceeding EEnterHighPressure
    Lone EEnterCloseProximity, EExitCloseProximity not followed by EOutOfRange for the same pointer
    Lone focus lost/gained messages
*/	
void CEventQueue::Purge()
	{
	TWsEvent* eventToPurge;
	TWsEvent* eventToMatch;
	TInt indexToMatch;
	TInt indexToPurge=iCount;
	while(indexToPurge>0)
		{
		eventToPurge=EventPtr(--indexToPurge);
		switch(eventToPurge->Type())
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
#ifdef LOG_WSERV_EVENTS
				RDebug::Print(_L("_WSEVENT: CEventQueue::Purge(), The event to be purged is %S"), &WsEventName(*eventToPurge));
#endif
				RemoveEvent(indexToPurge);
				return;
			case EEventKeyUp:
				if (iQueueList.First()!=this)
					goto breakLoopAndRemoveEvent;
				break;
			case EEventKeyDown:
				if (iQueueList.First()!=this)
					goto breakLoopAndRemoveEvent;
				for (indexToMatch=indexToPurge+1;indexToMatch<iCount;indexToMatch++)
					{
					eventToMatch=EventPtr(indexToMatch);
					if (eventToMatch->Type()==EEventKeyUp && eventToMatch->Key()->iScanCode==eventToPurge->Key()->iScanCode)
						{
						*eventToMatch=iNullEvent;
						goto breakLoopAndRemoveEvent;
						}
					}
				break;
			case EEventModifiersChanged:
				for (indexToMatch=indexToPurge;indexToMatch>0;)
					{
					eventToMatch=EventPtr(--indexToMatch);
					if (eventToMatch->Type()==EEventModifiersChanged)
						{
						eventToPurge->ModifiersChanged()->iChangedModifiers|=eventToMatch->ModifiersChanged()->iChangedModifiers;
						indexToPurge=indexToMatch;
						goto breakLoopAndRemoveEvent;
						}
					}
				break;
			case EEventPointerBufferReady:
				CWsPointerBuffer::DiscardPointerMoveBuffer(eventToPurge->Handle());
				goto breakLoopAndRemoveEvent;
			case EEventFocusLost:
			case EEventFocusGained:
				if ((indexToPurge+1)<iCount)
					{
					eventToMatch=EventPtr(indexToPurge+1);
					if (eventToMatch->Type()==EEventFocusLost || eventToMatch->Type()==EEventFocusGained)
						{
						*eventToMatch=iNullEvent;
						goto breakLoopAndRemoveEvent;
						}
					}
				break;
			case EEventSwitchOn:
				if ((indexToPurge+1)<iCount && EventPtr(indexToPurge+1)->Type()==EEventSwitchOn)
					goto breakLoopAndRemoveEvent;
				break;
			case EEventPointer:
				if (CheckPurgePointerEvent(eventToPurge))
					{
					goto breakLoopAndRemoveEvent;
					}
				break;
			}
		}
	}

void CEventQueue::PurgePointerEvents()
	{
	TWsEvent* eventToPurge;
	TInt indexToPurge=iCount;
	while(indexToPurge>0)
		{
		eventToPurge=EventPtr(--indexToPurge);
		switch(eventToPurge->Type())
			{
			case EEventPointerBufferReady:
				CWsPointerBuffer::DiscardPointerMoveBuffer(eventToPurge->Handle());
				RemoveEvent(indexToPurge);
				break;
			case EEventPointer:
				if (CheckPurgePointerEvent(eventToPurge))
					{
					RemoveEvent(indexToPurge);
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
		TWsEvent* head=EventPtr(0);
		TWsEvent* tail=EventPtr(iCount);
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
	CEventQueue* qptr;
	iter.Set(*(CEventQueue *)this);
	iter++;
	TWsEvent* end;
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
	CEventQueue* qptr=iter;
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
	CEventQueue* qptr;
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

CEventQueue::CEventQueue(CWsClient* aOwner) : CEventBase(aOwner)
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

TWsEvent* CEventQueue::EventPtr(TInt index)	
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
	TBool ret=ETrue;
	Wait();
	if (iCount==iQueueSize && !Expand(aPriority))
		{
		ret=EFalse;
#ifdef LOG_WSERV_EVENTS
		RDebug::Printf("WSEVENT: CEventQueue::QueueEvent(): 0x%X:  Queue Full!!!!!, iCount = %d, iQueueSize = %d", this, iCount, iQueueSize);
		RDebug::Print(_L("WSEVENT: CEventQueue::QueueEvent(): 0x%X:  Queue Full!!!!! TWsEvent.Type() = %S"), this, &WsEventName(event));
#endif
		}
	else
		{
		if (!iEventMsg.IsNull())
			{
			SignalEvent();
			}
#ifdef LOG_WSERV_EVENTS
		RDebug::Printf("_WSEVENT: CEventQueue::QueueEvent, Right before adding the event");
#endif
		*EventPtr(iCount++)=event;
#ifdef LOG_WSERV_EVENTS
		RDebug::Print(_L("_WSEVENT: CEventQueue::QueueEvent, Event %S successfully queued"), &WsEventName(event));
#endif
		}
	Signal();
	return(ret);
	}

TBool CEventQueue::QueueEvent(TUint32 aTarget, TInt aEvent, TInt aIntVal)
	{
	TWsEvent event;
	event.SetType(aEvent);
	event.SetHandle(aTarget);
	event.SetTimeNow();
	*(event.Int()) = aIntVal;
	return(QueueEvent(event));
	}

void CEventQueue::UpdateLastEvent(const TWsEvent &event)
	{
	WS_ASSERT_DEBUG(iCount>0, EWsPanicQueueUpdateCount);
	Mem::Copy(EventPtr(iCount-1)->EventData(),event.EventData(),TWsEvent::EWsEventDataSize);
	}

/*
Replaces last pointer event related to particular pointer with new one.

While searching for event to replace this method considers all events on the
queue except EMove and EDrag pointer events from pointers different than aEvent. 
If the last of these events under consideration:
(1) is a pointer event, 
(2) has the same type as aEvent,
(3) its type is either EMove or EDrag and
(4) has the same window handle as aEvent,
then it is removed from the queue and aEvent is put at the end of the queue.

@return ETrue if event on the queue has been replaced with aEvent, EFalse otherwise. 
*/
TBool CEventQueue::UpdateLastPointerEvent(const TWsEvent &aEvent)
	{
	if (aEvent.Pointer()->iType == TPointerEvent::EMove || aEvent.Pointer()->iType == TPointerEvent::EDrag)
		{
		Wait();
		
		if (iCount == 0)
			{
			Signal();
			return EFalse;
			}
		
		// loop through all events on the queue starting from the last one
		TWsEvent* evToUpdate = EventPtr(iCount);
		TWsEvent* evOnHead   = &iEventPtr[iHead]; 
		while (evToUpdate != evOnHead)
			{
			DecEventPointer(evToUpdate);
			
			// conditions that stop searching
			if (   (evToUpdate->Type() != EEventPointer)	// found non-pointer event
				|| (evToUpdate->Pointer()->iType != TPointerEvent::EMove && evToUpdate->Pointer()->iType != TPointerEvent::EDrag)	// pointer event but wrong type
				|| (   (TAdvancedPointerEventHelper::PointerNumber(*evToUpdate) == TAdvancedPointerEventHelper::PointerNumber(aEvent))
				    && (   (evToUpdate->Handle() != aEvent.Handle())					// good number & bad handle         
				        || (evToUpdate->Pointer()->iType != aEvent.Pointer()->iType)))) // good number & bad type
				{
				Signal();
				return EFalse;
				}
			else if (TAdvancedPointerEventHelper::PointerNumber(*evToUpdate) == TAdvancedPointerEventHelper::PointerNumber(aEvent))
				{
				// we found event to update: evToUpdate is pointer event with right type, pointer number
				// and window handle
				
				if (evToUpdate == EventPtr(iCount - 1))
					{
					UpdateLastEvent(aEvent);
					}
				else
					{
					RemoveEvent(evToUpdate);
					*EventPtr(iCount++) = aEvent;
					}
				Signal();
				return ETrue;
				}
			
			// evToUpdate is EMove or EDrag pointer event with different pointer id,
			// continue to loop through the queue
			}
		Signal();
		}
	return EFalse;
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
#ifdef LOG_WSERV_EVENTS
		RDebug::Printf("_WSEVENT: CEventQueue::GetData(), TWsEvent.Type() = %d", (iEventPtr+iHead)->Type());
#endif
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
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT: CEventQueue::RemoveEvent(index), Remove event index %d in the queue", index);
#endif
	WS_ASSERT_DEBUG(index < iCount, EWsPanicCheckEventQueue);
	RemoveEvent(EventPtr(index));
	}

void CEventQueue::RemoveEvent(TWsEvent* aEvToRemove)
//
// Remove event in the queue, this event MUST exist in the queue
//
	{
#ifdef LOG_WSERV_EVENTS
	RDebug::Print(_L("_WSEVENT: CEventQueue::RemoveEvent(aEvToRemove), Remove event %S in the queue"), &WsEventName(*aEvToRemove));
#endif
	iCount--;
	TWsEvent* last = EventPtr(iCount);
	TWsEvent* prev;
	while(aEvToRemove!=last)
		{
		prev = aEvToRemove;
		IncEventPointer(aEvToRemove);
		*prev = *aEvToRemove;
		}
	__ZAP_EVENT(last);
	}

const TWsEvent* CEventQueue::PeekLastEvent()
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

void CEventQueue::WalkEventQueue(EventQueueWalk aFunc, TAny* aFuncParam)
	{
	Wait();
restart:
	for (TInt index=0;index<iCount;index++)
		{
		TWsEvent* event=EventPtr(index);
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
