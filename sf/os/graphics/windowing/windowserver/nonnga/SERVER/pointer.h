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
// WsPointer and associated classes definitions
// 
//

#ifndef __POINTER_H__
#define __POINTER_H__

#include "server.h"

class CWsPointerTimer : public CTimer
	{
public:
	CWsPointerTimer();
	void ConstructL();
private:
	void RunL();
	};

class WsPointer
	{
	enum {EPointerUpdateGapInMicroSeconds=50000};
public:
	static void InitStaticsL();
	static void DeleteStatics();
	static inline TPointerCursorMode PointerCursorMode();
	static inline void SetPointerCursorMode(TPointerCursorMode aMode);
	static inline TPoint PointerCursorPos();
	static void SetPointerCursorPos(TPoint aPos);
	static void ReLogCurrentWindow();
	static void ReLogCurrentWindow(TPoint &aPos, TPoint &aParentPos, const CWsWindowGroup *aForceInGroup);
	static void ReLogWindow(const CWsWindow *aWin);
	static void WindowDisconected(const CWsWindow *deletedWindow);
	static void ClaimGrab(const CWsWindow *aWindow, TBool aSendUpEvent);
#if defined(__WINS__)
	static TBool PreProcessEvent(TRawEvent &aRawEvent,TBool aFromHardware=EFalse);
#else
	static TBool PreProcessEvent(TRawEvent &aRawEvent);
#endif
	static void ProcessEvent(TPointerEvent::TType type, const TPoint &aPos, TUint aModifiers
																				,const CWsWindowGroup *aForceInGroup,TBool aNatural);
	static void GetDoubleClickSettings(TTimeIntervalMicroSeconds32 &aTime, TInt &aDistance);
	static void SetDoubleClick(const TTimeIntervalMicroSeconds32 &aTime, TInt aDistance);
	static void UpdatePointerCursor();
	static void RequestPointerRepeatEvent(CWsWindow *aWindow, TTimeIntervalMicroSeconds32 aTime,const TRect &aRect);
	static void CancelPointerRepeatEventRequest();
	static void RepeatTimerCompleted();
	static TEventQueueWalkRet PointerRepeatPurgeCheck(const TWsEvent *aEvent);
	static void UnmatchedDownPurged(TPointerEvent::TType aPointerType, TUint aHandle);
	static void TimerExpired();
	static inline TBool XyInput();
#if defined(__WINS__)
	static void SetXyInputType(TXYInputType aXyInputType);
#endif
private:
	static void PointerCursorUpdateCheck();
	static void ProcessForegroundCheck();
	static void RestrictPos(TPoint& aPos,TBool aWithinDrawableArea=ETrue);
	static void ProcessEvent(TWsEvent& aEvent);
	static TBool PointerEventRepeatCheck(const TWsEvent *aEvent, TUint32 aHandle);
	static void SendEnterExitEvent(TEventCode aType);
	static void SetCurrentWindow(const CWsWindow *aWin);
	static void ProcessPointerEvent(TWsEvent& aEvent);
	static TBool QueuePointerEvent(const CWsWindow *aWindow, TWsEvent &event);
	static TBool CheckDownEventPurged(TPointerEvent::TType aType);
	static void UpdatePointerCursorTo(CWsPointerCursor* aNewCursor);
	static CWsPointerCursor* CalculatePointerCursor();
	static inline TBool MovesAvailable();
	static inline TBool DeltaMouse();
private:	
	static TPoint iCurrentPos;
	static TBool iPointerDown;	// Maps onto button1 down on multi button systems
	static TXYInputType iXyInputType;
	static const CWsWindow *iCurrentWindow;
	static const CWsWindow *iGrabWindow;
	static const CWsWindow *iActualWinPointerIsOver;
	static const CWsWindow *iPrevClickWindow;
	static TPoint iPrevClickPos;
	static TTime iPrevClickTime;
	static TPointerEvent::TType iPrevClickEventType;
	static TTimeIntervalMicroSeconds32 iDoubleClickMaxInterval;
	static TInt iDoubleClickMaxDistance;
	static CWsPointerCursor *iCursorSprite;
	static CWsPointerTimer *iRepeatTimer;
	static TPointerCursorMode iPointerCursorMode;
	static CWsWindow *iRepeatWindow;
	static TRect iRepeatRect;
	static TUint iLastUnmatchedDown1;
	static TUint iLastUnmatchedDown2;
	static TUint iLastUnmatchedDown3;
	static TBool iTimerQueued;
	static TBool iUpdateRequired;
	static CPeriodic *iPeriodicTimer;
	static CWsRootWindow* iRootWindow;	// uses
	};

class CWsPointerBuffer : public CBase
	{
public:
	~CWsPointerBuffer();
	static void ConnectL(CWsClientWindow *aWindow, TInt aMaxPoints, TUint aFlags);
	static void Disconnect(CWsClientWindow *aWindow);
	static void PointerEvent(CWsClientWindow *aWindow,const TPoint &aPoint);
	static void RetrievePointerMoveBuffer(CWsClientWindow *aWindow,TInt aMaxPoints);
	static void DiscardPointerMoveBuffer(CWsClientWindow *aWindow);
	static void DiscardPointerMoveBuffer(TUint aHandle);
private:
	static void AdjustMaxSizeL();
	static void Reset();
	static void SignalBufferReady();
private:
	CWsClientWindow *iWindow;
	TInt iMaxPoints;
	TInt iFlags;
	TSglQueLink iQue;
	static TBool iSignalled;
	static CWsPointerBuffer *iCurrentBuffer;
	static CCirBuf<TPoint> *iPointerBuffer;
	static TSglQue<CWsPointerBuffer> iList;
	};


inline void WsPointer::SetPointerCursorMode(TPointerCursorMode aMode)
	{
	iPointerCursorMode=aMode;
	}
	
inline TPointerCursorMode WsPointer::PointerCursorMode()
	{
	return iPointerCursorMode;
	}
	
inline TPoint WsPointer::PointerCursorPos()
	{
	return iCurrentPos;
	}

inline TBool WsPointer::XyInput()
	{
	return iXyInputType!=EXYInputNone;
	}

inline TBool WsPointer::MovesAvailable()
	{
	return (iXyInputType!=EXYInputNone && iXyInputType!=EXYInputPointer);
	}

inline TBool WsPointer::DeltaMouse()
	{
	return (iXyInputType==EXYInputDeltaMouse);
	}

#endif
