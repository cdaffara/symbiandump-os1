// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// TWsPointer and associated classes definitions
// 
//

#ifndef __POINTER_H__
#define __POINTER_H__

#include "server.h"
#include "advancedpointereventhelper.h"

class TWsPointer;
class CWsPointerTimer : public CTimer
	{
public:
	class MPointerTimerCallback
		{
	public:
		virtual void RepeatTimerCompleted() =0;
		};	
	
	static CWsPointerTimer* NewL(MPointerTimerCallback& aWsPointer);
private:
	CWsPointerTimer(MPointerTimerCallback& aPointerTimerCallback);
	void ConstructL();
	void RunL();
private:
	MPointerTimerCallback& iPointerTimerCallback;
	};

class TWsPointer : public CWsPointerTimer::MPointerTimerCallback
	{
friend class TWindowServerEvent;
	enum {EPointerUpdateGapInMicroSeconds=50000};
	enum TPointerState
		{
		EPointerStateOutOfRange=0,
		EPointerStateUp,
		EPointerStateDown
		};
public:
	static void InitStaticsL();
	static void Stop();
	static void DeleteStatics();
	static inline TPointerCursorMode PointerCursorMode();
	static inline void SetPointerCursorMode(TPointerCursorMode aMode);
	static inline TPoint PointerCursorPos();
	static void SetPointerCursorPos(TPoint aPos);
	static void ReLogPointersCurrentWindows();
	static void ReLogWindow(const CWsWindow* aWin);
	static void WindowDisconnected(const CWsWindow* deletedWindow);
	static TInt ClaimGrab(const CWsWindow* aWindow,const TWsWinCmdGrabControl& aGrabControl);	
#if defined(__WINS__)
	static TBool PreProcessDriverEvent(TRawEvent &aRawEvent,TBool aFromHardware=EFalse);
#else
	static TBool PreProcessDriverEvent(TRawEvent &aRawEvent);
#endif
	static void TranslateCoordsOnRotation(TPoint& aPoint);
	static TBool PreProcessClientEvent(TRawEvent &aRawEvent, TBool aAdvancedPointersEnabled);
	static TBool IsPointerEventType(TRawEvent::TType aType);
	static void ProcessWsEvent(TWsEvent& aEvent,const CWsWindowGroup* aForceInGroup,TBool aNatural);	
	static void GetDoubleClickSettings(TTimeIntervalMicroSeconds32 &aTime, TInt &aDistance);
	static void SetDoubleClick(const TTimeIntervalMicroSeconds32 &aTime, TInt aDistance);
	static void UpdatePointerCursor();
	static TInt RequestPointerRepeatEvent(CWsWindow* aWindow, const TWsWinCmdRequestPointerRepeatEvent& aRequest);
	static TInt CancelPointerRepeatEventRequest(const TWsWinCmdCancelPointerRepeatEventRequest& aCancel);
	void RepeatTimerCompleted(); // from CWsPointerTimer::MPointerTimerCallback	
	static TEventQueueWalkRet PointerRepeatPurgeCheck(TWsEvent* aEvent, TUint8 aPointerNumber);
	static inline void UnmatchedEventPurged(TPointerEvent::TType aPointerType, TWsEvent* aEventPurged);
	static void TimerExpired();
	static inline TBool XyInput();
#if defined(__WINS__)
	static void SetXyInputType(TXYInputType aXyInputType);
#endif
	static void UpdatePrimaryPointer(const TRawEvent& aRawEvent);
	static inline void RollbackPrimaryPointer();
	static TInt SetCloseProximityThresholds(TInt aEnterCloseProximityThreshold, TInt aExitCloseProximityThreshold);
	static TInt GetEnterCloseProximityThreshold();
	static TInt GetExitCloseProximityThreshold();
	static TInt SetHighPressureThresholds(TInt aEnterHighPressureThreshold, TInt aExitHighPressureThreshold);
	static TInt GetEnterHighPressureThreshold();
	static TInt GetExitHighPressureThreshold();
	static inline TInt PrimaryPointer();
private:
	/** Clears all fields to default values, state goes to EPointerStateOutOfRange */
	void Clear();
	void SendEnterExitEvent(TEventCode aType);
	void SetCurrentWindow(const CWsWindow* aWin);
	void ReLogCurrentWindow();
	void ReLogCurrentWindow(TPoint &aPos, TPoint &aParentPos, const CWsWindowGroup* aForceInGroup);
	void ProcessForegroundCheck();
	CWsPointerCursor* CalculatePointerCursor();
	void ProcessEvent(TWsEvent& aEvent,const CWsWindowGroup* aForceInGroup,TBool aNatural);	
	void ProcessEvent(TWsEvent& aEvent, TBool aNatural);
	void ProcessPointerEvent(TWsEvent& aEvent);
	void ProcessOutOfRangeEvent(TWsEvent& aEvent,const CWsWindowGroup* aForceInGroup, TBool aNatural);
	void UnmatchedEventPurged(TPointerEvent::TType aPointerType, TUint aHandle);
	TBool CheckMatchingEventPurged(TPointerEvent::TType aType);
	TBool QueuePointerEvent(const CWsWindow* aWindow, TWsEvent &event);
	void ClaimGrab(const CWsWindow* aWindow, TBool aSendUpEvent);
	void PointerCursorUpdateCheck();
	void NotifyCClick(TAdvancedPointerEvent& aPointerEvent);
	
	static void RestrictPos(TPoint& aPos,TBool aWithinDrawableArea=ETrue);
	void RequestRepeatEvent(CWsWindow* aWindow, const TWsWinCmdRequestPointerRepeatEvent& aRequest);
	void CancelPointerRepeatEventRequest();
	TBool PointerEventRepeatCheck(const TWsEvent* aEvent, TUint32 aHandle);
	static void UpdatePointerCursorTo(CWsPointerCursor* aNewCursor);
	static inline TBool MovesAvailable();
	static inline TBool DeltaMouse();
	static inline CWsWindow*& RepeatWindow(TUint8 aPointerNumber);
	static inline TRect& RepeatRect(TUint8 aPointerNumber);
	static inline CWsPointerTimer*& RepeatTimer(TUint8 aPointerNumber);
	static inline TBool PointerNumberInRange(TUint8 aPointerNumber);
	static void ShiftYCoordinate(TInt& aY);
private:
	/** Pointer number used to identify this pointer */
	TUint iNumber;
	
	/** State of the pointer - Up, Down or OutOfRange */
	TPointerState iState;
	
	/** 2D position of the pointer. Valid if iState != EPointerStateOutOfRange. */
	TPoint iPos;
	
	/** Combined pressure and proximity values for the pointer */
	TInt iPressureProximity;
	
	/** Where the pointer is 'Logged' taking into account position, grabbing and capturing */
	const CWsWindow* iCurrentWindow;
	
	/** Window under pointer */
	const CWsWindow* iActualWinPointerIsOver;
	
	/** Window that is grabbing this pointer's events (related to Drag events) */
	const CWsWindow* iGrabWindow;
	
	TUint iLastUnmatchedDown1;
	TUint iLastUnmatchedDown2;
	TUint iLastUnmatchedDown3;
	TUint iLastUnmatchedEnterHighPressure;
	
	/** Window which received last click, for double click detection */
	const CWsWindow* iPrevClickWindow;
	
	/** Position of last click, for double click detection */
	TPoint iPrevClickPos;
	
	/** Time of last click, for double click detection */ 
	TTime iPrevClickTime;
	
	/** Which button has been clicked last time, for double click detection */ 
	TPointerEvent::TType iPrevClickEventType;
	
	/** State upon which firing of EEnterCloseProximity and EExitCloseProximity
	events is based. */
	TBool iInCloseProximity;
	
	/** State upon which firing of EEnterHighPressure and EExitHighPressure
	events is based. */
	TBool iInHighPressure;
	
	/** Timer for pointer event repeating */
	CWsPointerTimer* iRepeatTimer;
	
	CWsWindow* iRepeatWindow;
	
	TRect iRepeatRect;
	
	/** Pointing device's type */
	static TXYInputType iXyInputType;

	/** Maximum number of pointers handled at the same time. Always at least 1. */ 
	static TInt iMaxPointers;
	
	/** Main array of TWsPointer objects, can keep up to iMaxPointers objects */
	static RArray<TWsPointer> iPointers;
	
	/** Index of Primary Pointer in iPointers array. */
	static TInt iPrimaryPointer;
	
	/** Previous primary pointer, before last run of UpdatePrimaryPointer(TRawEvent). */
	static TInt iPreviousPrimaryPointer;
	
	static CWsRootWindow* iRootWindow;
	
	/** Global setting for all pointers */
	static TTimeIntervalMicroSeconds32 iDoubleClickMaxInterval;

	/** Global setting for all pointers */
	static TInt iDoubleClickMaxDistance;
	
	static CWsPointerCursor* iCursorSprite;
	
	static TPointerCursorMode iPointerCursorMode;
	
	/** If pointer cursor update is required */
	static TBool iUpdateRequired;
	
	/** Timer for pointer cursor updates */
	static CPeriodic* iPeriodicTimer;
	
	/** For iPeriodicTimer */
	static TBool iTimerQueued;
	
	static TBool iIs3DPointer;
	
	/** Threshold upon which firing of EEnterCloseProximity event is based */
	static TInt iEnterCloseProximityThreshold;
	
	/** Threshold upon which firing of EExitCloseProximity event is based */
	static TInt iExitCloseProximityThreshold;

	/** Threshold upon which firing of EEnterHighPressure event is based */
	static TInt iEnterHighPressureThreshold;

	/** Threshold upon which firing of EExitHighPressure event is based */
	static TInt iExitHighPressureThreshold;
	
	/** Used to offset the y pointer */
	static TInt iYOffset;
	
#if defined(__WINS__)
	/** Tell whether to rotate pointer coords in _WINS_ builds */
	static TBool iEmulatorRotatePointerCoords;
#endif
	};

class CWsPointerBuffer : public CBase
	{
public:
	~CWsPointerBuffer();
	static void ConnectL(CWsClientWindow* aWindow, TInt aMaxPoints, TUint aFlags);
	static void Disconnect(CWsClientWindow* aWindow);
	static void PointerEvent(CWsClientWindow* aWindow,const TPoint &aPoint);
	static void RetrievePointerMoveBuffer(CWsClientWindow* aWindow,TInt aMaxPoints);
	static void DiscardPointerMoveBuffer(CWsClientWindow* aWindow);
	static void DiscardPointerMoveBuffer(TUint aHandle);
private:
	static void AdjustMaxSizeL();
	static void Reset();
	static void SignalBufferReady();
private:
	CWsClientWindow* iWindow;
	TInt iMaxPoints;
	TInt iFlags;
	TSglQueLink iQue;
	static TBool iSignalled;
	static CWsPointerBuffer* iCurrentBuffer;
	static CCirBuf<TPoint>* iPointerBuffer;
	static TSglQue<CWsPointerBuffer> iList;
	};


inline void TWsPointer::SetPointerCursorMode(TPointerCursorMode aMode)
	{iPointerCursorMode=aMode;}
	
inline TPointerCursorMode TWsPointer::PointerCursorMode()
	{return iPointerCursorMode;}
	
inline TPoint TWsPointer::PointerCursorPos()
	{return iPointers[iPrimaryPointer].iPos;}

inline void TWsPointer::UnmatchedEventPurged(TPointerEvent::TType aPointerType, TWsEvent* aEventPurged)
	{iPointers[TAdvancedPointerEventHelper::PointerNumber(*aEventPurged)].UnmatchedEventPurged(aPointerType, aEventPurged->Handle());}

inline TBool TWsPointer::XyInput()
	{return iXyInputType!=EXYInputNone;}

/*
EXYInputNone => no pointer, no moves.
EXYInputPointer && !iIs3DPointer => drags only, no moves (allows mouse to behave like pen for emulator)
EXYInputPointer && iIs3DPointer => drags and moves
EXYInputMouse => drags and moves  
EXYInputDeltaMouse => drags and moves
*/
inline TBool TWsPointer::MovesAvailable()
	{return (iXyInputType!=EXYInputNone && ((iXyInputType!=EXYInputPointer)||iIs3DPointer));}

inline TBool TWsPointer::DeltaMouse()
	{return (iXyInputType==EXYInputDeltaMouse);}

inline TInt TWsPointer::PrimaryPointer()
	{return iPrimaryPointer;}

/**
Sets primary pointer to the one which was present before last run of UpdatePrimaryPointer() method.
*/
inline void TWsPointer::RollbackPrimaryPointer()
	{iPrimaryPointer = iPreviousPrimaryPointer;}

inline CWsWindow*& TWsPointer::RepeatWindow(TUint8 aPointerNumber)
	{return iPointers[aPointerNumber].iRepeatWindow;}

inline TRect& TWsPointer::RepeatRect(TUint8 aPointerNumber)
	{return iPointers[aPointerNumber].iRepeatRect;}

inline CWsPointerTimer*& TWsPointer::RepeatTimer(TUint8 aPointerNumber)
	{return iPointers[aPointerNumber].iRepeatTimer;}

inline TBool TWsPointer::PointerNumberInRange(TUint8 aPointerNumber)
	{return (aPointerNumber<iMaxPointers);}

#endif
