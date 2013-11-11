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
// Definition of classes related event queues
// 
//

#if !defined(__EVQUEUE_H__)
#define __EVQUEUE_H__

#include <e32std.h>
#include <e32base.h>
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "W32STD.H"

class CWsClient;
class CWsWindowRedraw;
class CWsGraphicDrawer;

enum TEventQueueWalkRet
	{
	EEventQueueWalkOk,
	EEventQueueWalkDeleteEvent,
	EEventQueueWalkRestart,
	};
class TWsEvent;

typedef TEventQueueWalkRet (*EventQueueWalk)(TAny* aPtr, TWsEvent* aEvent);

enum TWservEventPriorities
	{
	EEventPriorityHigh,
	EEventPriorityLow,
	};

struct TRedraw
	{
	CWsWindowRedraw* iRedraw;
	TUint iPriority;
	};

class CEventBase : public CBase
	{
private:
	enum TEventSignalledState
		{
		EEventFlagSignalled=0x1,
		EEventFlagCancelled=0x2
		};
public:
	CEventBase(CWsClient* aOwner);
	~CEventBase();
	void CancelRead();
	void EventReadyCheck();
	virtual void EventReady(const RMessagePtr2& aEventMsg);
	void GetData(TAny* aData, TInt aLen);
protected:
	void SignalEvent(TInt aCode = KErrNone);
	inline TBool IsEventCancelled();
protected:
	TInt iEventSignalledState;
	CWsClient* iWsOwner;
	RMessagePtr2 iEventMsg;
	};

class CEventQueue : public CEventBase
	{
private:
	enum {EExtraQueueSize=16};
	enum {EMaxQueueSize=32};
	enum {EMinQueueSize=2};
	enum {EQueueGranularity=4};
	enum TCompressMode
		{
		ECompressNoPurge,	// Compress only free space
		ECompressPurge1,	// Purge from only non-foreground queues
		ECompressPurge2,	// Purge from all queues
		};
// Functions
public:
	CEventQueue(CWsClient* aOwner);
	~CEventQueue();
	void ConstructL();
	TBool QueueEvent(const TWsEvent &event);
	TBool QueueEvent(const TWsEvent &event, TWservEventPriorities aPriority);
	TBool QueueEvent(TUint32 aTarget, TInt aEvent, TInt aIntVal = 0);
	TBool CheckRoom();
	void GetData();
	void UpdateLastEvent(const TWsEvent &event);
	TBool UpdateLastPointerEvent(const TWsEvent &event);
	void RemoveEvent(TInt index);
	void PurgePointerEvents();
	const TWsEvent* PeekLastEvent();
	void WalkEventQueue(EventQueueWalk aFunc, TAny* aFuncParam);
	void MoveToFront();
	static void Wait();
	static void Signal();
	static void InitStaticsL();
	static void DeleteStaticsL();
	//void LogUpDownEvents(TChar aChar);
public:// from CEventBase
	void EventReady(const RMessagePtr2& aEventMsg);
private:
	static void EventCopy(TWsEvent* aStart, TWsEvent* aEnd, TInt aNumEvents);
	static TInt RequiredQueueSize(TInt aNumConnections);
	void AdjustQueueSizeL(TInt aNewSize);
	void IncreaseQueueSize(TInt aNumSpaces);
	TInt FollowingGap() const;
	void Purge();
	TInt PurgeInactiveEvents(const TInt& aSizeRequired);
	void Compress(TCompressMode aCompressMode);
	void AddQueueL();
	void RemoveQueue();
	TBool Expand(TWservEventPriorities aPriority);
	TBool doExpand(TCompressMode aCompressMode);
	TInt SqueezeUp();
	void SqueezeDown();
	void MoveUp(TInt aMove);
	void MoveDown(TInt aMove);
	TBool MoveDownAndExpand(TDblQueIter<CEventQueue> &aIter, TInt aExpand);
#if defined(_DEBUG)
	void CheckQueue();
	void ZapEvent(TWsEvent* aTarget);
	void ZapEvents(TWsEvent* aTarget, TInt aLen);
#endif
private:
	void QueueActive();
	TWsEvent* EventPtr(TInt index);
	void RemoveEvent(TWsEvent* aEvToRemove);
	inline void IncEventPointer(TWsEvent*& aEventPtr);	
	inline void DecEventPointer(TWsEvent*& aEventPtr);	
	void PurgeEventPairs(TWsEvent* aEventToPurge, TPointerEvent::TType aMatchingType, TPointerEvent::TType aSearchTerminator);
	TWsEvent* NextPointerEvent(TWsEvent* aBaseEvent);
	TBool CheckPurgePointerEvent(TWsEvent* aEventToPurge);
private:
    /** Position of head in local queue relative to iEventPtr. 
        First event to send to the Client. */ 
	TInt iHead;
	
	/** Number of events waiting in local queue */
    TInt iCount;
    
    /** Size of local queue = maximum number of events */
    TInt iQueueSize;
    
    /** Beginning of local queue */
    TWsEvent* iEventPtr;
    
    /** Used by iQueueList to link local queues */
	TDblQueLink iLink;
	
	/** Mutex for operations on global event queue */
	static RMutex iMutex;
	
	/** Always filled with zeros */
	static TWsEvent iNullEvent;
	
	/** List of event queues */
	static TDblQue<CEventQueue> iQueueList;
	
	/** Global queue that keeps all events queues */
	static TWsEvent* iGlobalEventQueue;
	
	/** Size of global queue */
	static TInt iGlobalEventQueueSize;
	
	/** Number of event queues */
	static TInt iNumConnections;
	};

class TWsRedrawEvent;

class CRedrawQueue : public CEventBase
	{
// Functions
public:
	CRedrawQueue(CWsClient *aOwner);
	~CRedrawQueue();
	void ConstructL();
	TBool TriggerRedraw();
	void ReCalcOrder();
	void AddInvalid(CWsWindowRedraw *redrawWin);
	void RemoveInvalid(CWsWindowRedraw *redrawWin);
	void GetData();
	TUint RedrawPriority(CWsWindowRedraw *aRedrawWin);
public:// from CEventBase
	void EventReady(const RMessagePtr2& aEventMsg);
private:
	void DeleteFromQueue(TInt aIndex);
	void QueueActive();
	TBool FindOutstandingRedrawEvent(CWsWindowRedraw& aRedraw, TWsRedrawEvent& aEvent);
	TBool FindWindowNeedingRedrawEvent(TWsRedrawEvent& aEvent);
private:
// Data
	CArrayFixSeg<TRedraw> *iRedraws;	// List of windows and their areas that require redraws
	TBool iAllocError;
	TBool iRedrawTrigger;
	TKeyArrayFix *iKeyPriority;
	TKeyArrayFix *iKeyWindow;
	static TWsRedrawEvent iNullRedrawEvent;
	};

class CWsGraphicMessageQueue: public CEventBase
	{
public:
	class CMessage: public CWsMessageData
		{
	public:
		static CMessage* New(const TDesC8& aData);
		~CMessage();
	public: // From CWsMessageData
		TPtrC8 Data() const;
		void Release();
	private:
		CMessage(const TDesC8& aData);
	private:
		TPtr8 iData;
		};
public:
	CWsGraphicMessageQueue(CWsClient *aOwner);
	~CWsGraphicMessageQueue();
	void GetGraphicMessage();
	void AbortMessage(TInt aError);
	void EventReady(const RMessagePtr2& aEventMsg);
	void Queue(CWsMessageData* aMessage);
	TInt TopClientHandle() const;
private:
	CWsMessageData* iHead;
	CWsMessageData* iTail;
	CWsMessageData* Pop();
	void GetDataWithHeader(TUint aHeader, const TDesC8& aData, TInt aDataLen);
	};

#endif
