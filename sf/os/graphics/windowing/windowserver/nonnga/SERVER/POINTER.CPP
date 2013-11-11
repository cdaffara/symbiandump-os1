// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Pointer functions
// 
//

#include <e32std.h>
#include <e32hal.h>
#include "W32CLICK.H"
#include "pointer.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "KEYCLICK.H"
#include "ScrDev.H"
#include "EVENT.H"
#include "panics.h"
#include "wstop.h"
#include "inifile.h"
#include "graphics/pointereventdata.h"

TPoint WsPointer::iCurrentPos;
TBool WsPointer::iPointerDown;
const CWsWindow *WsPointer::iCurrentWindow;
const CWsWindow *WsPointer::iActualWinPointerIsOver;
const CWsWindow *WsPointer::iGrabWindow;
const CWsWindow *WsPointer::iPrevClickWindow;
TPoint WsPointer::iPrevClickPos;
TTime WsPointer::iPrevClickTime;
TPointerEvent::TType WsPointer::iPrevClickEventType;
TTimeIntervalMicroSeconds32 WsPointer::iDoubleClickMaxInterval;
TInt WsPointer::iDoubleClickMaxDistance;
CWsPointerCursor *WsPointer::iCursorSprite;
CWsPointerTimer *WsPointer::iRepeatTimer=NULL;
TPointerCursorMode WsPointer::iPointerCursorMode=EPointerCursorNormal;
CWsWindow *WsPointer::iRepeatWindow=NULL;
TRect WsPointer::iRepeatRect;
TXYInputType  WsPointer::iXyInputType;
TUint WsPointer::iLastUnmatchedDown1;
TUint WsPointer::iLastUnmatchedDown2;
TUint WsPointer::iLastUnmatchedDown3;
TBool WsPointer::iTimerQueued;
TBool WsPointer::iUpdateRequired;
CPeriodic *WsPointer::iPeriodicTimer;
CWsRootWindow* WsPointer::iRootWindow;

TBool CWsPointerBuffer::iSignalled=EFalse;
CWsPointerBuffer *CWsPointerBuffer::iCurrentBuffer=NULL;
CCirBuf<TPoint> *CWsPointerBuffer::iPointerBuffer=NULL;
TSglQue<CWsPointerBuffer> CWsPointerBuffer::iList(_FOFF(CWsPointerBuffer,iQue));

void WsPointer::InitStaticsL()
	{
	iRepeatTimer=new(ELeave) CWsPointerTimer();
	iRepeatTimer->ConstructL();
	TMachineInfoV1Buf machineInfo;
	UserHal::MachineInfo(machineInfo);
	iXyInputType=machineInfo().iXYInputType;

	iRootWindow = CWsTop::Screen()->RootWindow();

	iCurrentWindow=MovesAvailable() ? iRootWindow : NULL;
	iPeriodicTimer=CPeriodic::NewL(EPointerCursorPriority);
	}

void WsPointer::DeleteStatics()
	{
	SetPointerCursorMode(EPointerCursorNone);
	UpdatePointerCursor();
	delete iRepeatTimer;
	delete iPeriodicTimer;
	}

void WsPointer::SetPointerCursorPos(TPoint aPos)
	{
	RestrictPos(aPos,EFalse);
	iCurrentPos=aPos;
	ReLogCurrentWindow();
	UpdatePointerCursor();
	}

void WsPointer::SendEnterExitEvent(TEventCode aType)
	{
	if (iCurrentWindow
		&& !(iCurrentWindow->PointerFilter()&EPointerFilterEnterExit)
		&& !iCurrentWindow->ShutDownInProgress())
		iCurrentWindow->QueueEvent(aType);
	}

void WsPointer::SetCurrentWindow(const CWsWindow *aWin)
	{
	if (aWin!=iCurrentWindow)
		{
		SendEnterExitEvent(EEventPointerExit);
		iCurrentWindow=aWin;
		SendEnterExitEvent(EEventPointerEnter);
		}
	}

void WsPointer::ReLogCurrentWindow(TPoint &aPos, TPoint &aParentPos, const CWsWindowGroup *aForceInGroup)
//
// Relog the current pointer window, can be used to set a new iCurrentPos or when the window layout has changed.
// Sets iCurrentPos to aPos and modifys aPos to the relative position within the new current window
//
	{
	if (iRootWindow)
		SetCurrentWindow(iRootWindow->PointerWindow(iCurrentPos,&aPos,&aParentPos,iGrabWindow,iActualWinPointerIsOver
																										,aForceInGroup));
	else
		iCurrentWindow=NULL;
	}

void WsPointer::ReLogCurrentWindow()
//
// Relog the current pointer window when the window layout has changed.
//
	{
	if (iCurrentWindow)	// NULL iCurrentWindow means pointer is up so don't relog it
		SetCurrentWindow(iRootWindow->PointerWindow(iCurrentPos,NULL,NULL,iGrabWindow,iActualWinPointerIsOver,NULL));
	}

void WsPointer::ReLogWindow(const CWsWindow *aWin)
//
// Called when a window has changed it's filter state, will trigger a 'Enter' message if the window
// is the current window
//
	{
	if (aWin==iCurrentWindow)
		SendEnterExitEvent(EEventPointerEnter);
	}

void WsPointer::UnmatchedDownPurged(TPointerEvent::TType aPointerType, TUint aHandle)
	{
	if (aPointerType==TPointerEvent::EButton1Up && iGrabWindow && iGrabWindow->ClientHandle()==aHandle)
		{
		if (iGrabWindow && iGrabWindow->ClientHandle()==aHandle)
			iGrabWindow=NULL;
		if (iRepeatWindow && iRepeatWindow->ClientHandle()==aHandle)
			CancelPointerRepeatEventRequest();
		}
	switch(aPointerType)
		{
		case TPointerEvent::EButton1Up:
			iLastUnmatchedDown1=aHandle;
			break;
		case TPointerEvent::EButton2Up:
			iLastUnmatchedDown2=aHandle;
			break;
		case TPointerEvent::EButton3Up:
			iLastUnmatchedDown3=aHandle;
			break;
		default:;
		}
	}

void WsPointer::WindowDisconected(const CWsWindow *deletedWindow)
	{
	if (iRepeatWindow==deletedWindow)
		CancelPointerRepeatEventRequest();
	if (iGrabWindow==deletedWindow)
		iGrabWindow=NULL;
	if (iCurrentWindow==deletedWindow)
		{
		ReLogCurrentWindow();
		UpdatePointerCursor();
		}
	}

TEventQueueWalkRet RemovePointerUpFunc(TAny *aHandle, TWsEvent *aEvent)
//
// Callback function pointer for up event remove event queue walk
//
	{
	if (aEvent->Type()==EEventPointer && aEvent->Pointer()->iType==TPointerEvent::EButton1Up && (*(TUint *)aHandle)==aEvent->Handle())
		return(EEventQueueWalkDeleteEvent);
	return(EEventQueueWalkOk);
	}

void WsPointer::ClaimGrab(const CWsWindow *aWindow,TBool aSendUpEvent)
//
// If the pointer is down claim grab in aWindow as though the down event had gone to this window
// also send an up event to the window (if any) that would receive it the pointer was released now
//
	{
	TInt modState=TWindowServerEvent::GetModifierState();
	TWsEvent event;
	TPointerEvent& pointerEvent=*event.Pointer();
	pointerEvent.iModifiers=modState;
	pointerEvent.iPosition=iCurrentPos;
	if (iPointerDown)
		{
		if (iCurrentWindow!=aWindow)
			{
			if (aSendUpEvent)
				ProcessEvent(TPointerEvent::EButton1Up,iCurrentPos,modState,NULL,EFalse);
			else // If up event already in queue purge it
				{
				TUint handle=iCurrentWindow->ClientHandle();
				iCurrentWindow->EventQueue()->WalkEventQueue(&RemovePointerUpFunc,&handle);
				}
			iPointerDown=ETrue;
			if (aWindow->HasPointerGrab())
				iGrabWindow=aWindow;
			ReLogCurrentWindow(pointerEvent.iPosition,pointerEvent.iParentPosition,NULL);
			pointerEvent.iType=TPointerEvent::EDrag;
			ProcessPointerEvent(event);
			}
		}
	else
		{
		const CWsWindow *current=iCurrentWindow;
		iCurrentWindow=aWindow;
		WS_ASSERT_DEBUG(iGrabWindow==NULL, EWsPanicPointerClaimGrab);
		iGrabWindow=aWindow;	// Force the up event to be sent to aWindow
		ReLogCurrentWindow(pointerEvent.iPosition,pointerEvent.iParentPosition,NULL);
		pointerEvent.iType=TPointerEvent::EButton1Up;
		ProcessPointerEvent(event);
		iGrabWindow=NULL;
		iCurrentWindow=current;
		}
	}

TBool WsPointer::CheckDownEventPurged(TPointerEvent::TType aType)
	{
	switch(aType)
		{
		TUint lastUnmatchedDown;
		case TPointerEvent::EButton1Up:
			lastUnmatchedDown=iLastUnmatchedDown1;
			iLastUnmatchedDown1=0;
			goto lastUnmatchedDownCheck;
		case TPointerEvent::EButton2Up:
			lastUnmatchedDown=iLastUnmatchedDown2;
			iLastUnmatchedDown2=0;
			goto lastUnmatchedDownCheck;
		case TPointerEvent::EButton3Up:
			lastUnmatchedDown=iLastUnmatchedDown3;
			iLastUnmatchedDown3=0;
lastUnmatchedDownCheck:
		if (lastUnmatchedDown==iCurrentWindow->ClientHandle())
			return ETrue; // Don't deliver the event as we've already thrown away the down
		default:		//Should never get to default
			break;
		}
	return EFalse;
	}

TBool WsPointer::QueuePointerEvent(const CWsWindow *aWindow, TWsEvent &aEvent)
	{
	CWsClient *client=aWindow->WsOwner();
	if (client)
		{
		CEventQueue *queue=aWindow->EventQueue();
		aEvent.SetHandle(aWindow->ClientHandle());
		if (aEvent.Handle()!=0)
			{
			if (aEvent.Pointer()->iType==TPointerEvent::EMove || aEvent.Pointer()->iType==TPointerEvent::EDrag)
				{
				queue->Wait();
				const TWsEvent *prev=queue->PeekLastEvent();
				if (prev!=NULL && prev->Type()==EEventPointer && prev->Handle()==aEvent.Handle() && prev->Pointer()->iType==aEvent.Pointer()->iType)
					{
					queue->UpdateLastEvent(aEvent);
					return EFalse;
					}
				queue->Signal();
				}
			TWservEventPriorities priority=EEventPriorityLow;
			switch (aEvent.Pointer()->iType)
				{
				case TPointerEvent::EButton1Up:
				case TPointerEvent::EButton2Up:
				case TPointerEvent::EButton3Up:
					if (CheckDownEventPurged(aEvent.Pointer()->iType))
						return ETrue;
					if (queue->CheckRoom())
						{
						if (CheckDownEventPurged(aEvent.Pointer()->iType))
							return ETrue;
						} 
					/*Fall Through if an event was not purged*/
				case TPointerEvent::EButton1Down:
				case TPointerEvent::EButton2Down:
				case TPointerEvent::EButton3Down:
					priority=EEventPriorityHigh;
					break;
				default:;
				}
			queue->QueueEvent(aEvent,priority);
			}
		}
	return EFalse;
	}

void WsPointer::ProcessForegroundCheck()
	{
	CWsWindowGroup *group=iCurrentWindow->WinGroup();
	if (group->iFlags&CWsWindowGroup::EGroupFlagAutoForeground)
		group->SetOrdinalPosition(0);
	}

void WsPointer::ProcessPointerEvent(TWsEvent& aEvent)
	{
	if (iCurrentWindow && iCurrentWindow!=iRootWindow)
		{
		aEvent.SetType(EEventPointer);
		aEvent.SetTimeNow();
		TPointerEvent::TType type=aEvent.Pointer()->iType;
		switch(type)
			{
			//TUint lastUnmatchedDown;
			case TPointerEvent::EButton1Down:
				ProcessForegroundCheck(); 
				/*Fall Through*/
			case TPointerEvent::EButton2Down:
			case TPointerEvent::EButton3Down:
				{
				TPoint& pos=aEvent.Pointer()->iPosition;
				if (iCurrentWindow==iPrevClickWindow &&
					type==iPrevClickEventType &&
					(Abs(pos.iX-iPrevClickPos.iX)+Abs(pos.iY-iPrevClickPos.iY))<iDoubleClickMaxDistance &&
					aEvent.Time()<(iPrevClickTime+iDoubleClickMaxInterval))
					{
					aEvent.Pointer()->iModifiers|=EModifierDoubleClick;
					iPrevClickWindow=NULL;	// Set to NULL to block a double double click
					}
				else
					iPrevClickWindow=iCurrentWindow;
				iPrevClickEventType=type;
				iPrevClickPos=pos;
				iPrevClickTime=aEvent.Time();
				}
				break;
			default:
				break;
			}
		if (iRepeatWindow)
			{
			if (PointerEventRepeatCheck(&aEvent, iCurrentWindow->ClientHandle()))
				return;
			CancelPointerRepeatEventRequest();
			}
		if (QueuePointerEvent(iCurrentWindow, aEvent))
			return;
		if (iCurrentWindow->DragDropCapture())
			{
			aEvent.SetType(EEventDragDrop);
			QueuePointerEvent(iActualWinPointerIsOver, aEvent);
			}
		}
	}

TInt PointerTimerCallBack(TAny *)
	{
	WsPointer::TimerExpired();
	return(KErrNone);
	}

void WsPointer::RestrictPos(TPoint& aPos,TBool aWithinDrawableArea/*=ETrue*/)
	{
	CScreen* screen = iRootWindow->Screen();
	WS_ASSERT_DEBUG(screen->IsValidScreenSizeMode(screen->ScreenSizeMode()), EWsPanicInvalidScreenSizeMode);
#if defined(__WINS__)
	if (aWithinDrawableArea)
		{
		if (!DeltaMouse() && !TRect(screen->DrawableArea()).Contains(aPos))
			{
			return;			//Not in the drawable area so user may be trying to click on facia button.
			}
		}
#endif
	TRect validRect=screen->GetPointerCursorArea(screen->ScreenSizeMode());
	if (aPos.iX<validRect.iTl.iX)
		aPos.iX=validRect.iTl.iX;
	else if (aPos.iX>=validRect.iBr.iX)
		aPos.iX=validRect.iBr.iX-1;
	if (aPos.iY<validRect.iTl.iY)
		aPos.iY=validRect.iTl.iY;
	else if (aPos.iY>=validRect.iBr.iY)
		aPos.iY=validRect.iBr.iY-1;
	}

#if defined(__WINS__)
TBool WsPointer::PreProcessEvent(TRawEvent &aRawEvent,TBool aFromHardware/*=EFlase*/)
#else
TBool WsPointer::PreProcessEvent(TRawEvent &aRawEvent)
#endif
	{
#if defined(__WINS__)
	WS_ASSERT_DEBUG(TRawEvent::EPointerMove==1, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EPointerMove+1==TRawEvent::EPointerSwitchOn, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EPointerSwitchOn+8==TRawEvent::EButton1Down, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EButton1Down+5==TRawEvent::EButton3Up, EWsPanicRawEventsTypeChanged);
#endif
	TRawEvent::TType type=aRawEvent.Type();
	if (type<TRawEvent::EPointerMove || (type>TRawEvent::EPointerSwitchOn && type<TRawEvent::EButton1Down)
																						|| type>TRawEvent::EButton3Up)
		return ETrue;
	if (!XyInput())
		return EFalse;
	TPoint xy=aRawEvent.Pos();
	if (DeltaMouse())
		{
	#if defined(__WINS__)
		if (aFromHardware)
			return EFalse;
	#endif
		if (type==TRawEvent::EPointerMove)
			{
			xy+=iCurrentPos;
			RestrictPos(xy);
			}
		else
			xy=iCurrentPos;
		}
	else
		{
		CScreen* screen=iRootWindow->Screen();
	#if !defined(__WINS__)
		TSize screenSize=screen->ScreenDevice()->SizeInPixels()-TSize(1,1);             //This is in the current rotation
		switch (screen->Orientation())
			{
			case CFbsBitGc::EGraphicsOrientationRotated90:
				xy.SetXY(xy.iY,screenSize.iHeight-xy.iX);
				break;
			case CFbsBitGc::EGraphicsOrientationRotated180:
				xy=-(xy-screenSize);
				break;
			case CFbsBitGc::EGraphicsOrientationRotated270:
				xy.SetXY(screenSize.iWidth-xy.iY,xy.iX);
				break;
			default:;		//To stop warning
			}
	#endif
		// Move the raw event position by shifting it by Origin and scale
		xy=screen->PhysicalToLogical(xy);
		RestrictPos(xy);
		}
	aRawEvent.Set(type,xy.iX,xy.iY);
	return ETrue;
	}

void WsPointer::ProcessEvent(TPointerEvent::TType aType, const TPoint &aPos, TUint aModifiers
																				,const CWsWindowGroup *aForceInGroup,TBool aNatural)
	{
	iCurrentPos=aPos;
	if (aType==TPointerEvent::EMove && !MovesAvailable() && !iPointerDown)
		return;
	TPoint pos(iCurrentPos);	// We need a non-const TPoint for 'ReLogCurrentWindow'
	TPoint parPos;
	ReLogCurrentWindow(pos,parPos,aForceInGroup);
	TWsEvent event;
	TPointerEvent& pointerEvent=*event.Pointer();
	pointerEvent.iModifiers=aModifiers;
	pointerEvent.iPosition=pos;
	pointerEvent.iParentPosition=parPos;
	switch(aType)
		{
		case TPointerEvent::EButton1Down:
			iPointerDown=ETrue;
			if (iGrabWindow==NULL && iCurrentWindow->HasPointerGrab())
				iGrabWindow=iCurrentWindow;
			if (!MovesAvailable() && iCurrentWindow->PointerFilter()&EPointerGenerateSimulatedMove)
				{
				pointerEvent.iType=TPointerEvent::EMove;
				ProcessEvent(event);
				}
			break;
		case TPointerEvent::EButton1Up:
			iPointerDown=EFalse;
			iGrabWindow=NULL;
			break;
		case TPointerEvent::EMove:
			if (iPointerDown)
				aType=TPointerEvent::EDrag;
			break;
		default:;
		}
	pointerEvent.iType=aType;
	if (aNatural && CClick::IsHandler())
		{
		CClick::PointerEvent(iCurrentPos,pointerEvent);
		TPointerEventData params;
		params.iVersion=0;
		params.iCurrentPos=iCurrentPos;
		params.iPointerEvent.iType 		= pointerEvent.iType;
		params.iPointerEvent.iModifiers = pointerEvent.iModifiers;
		params.iPointerEvent.iPosition  = pointerEvent.iPosition;
		params.iPointerEvent.iParentPosition = pointerEvent.iParentPosition;
		params.iClientHandle=iCurrentWindow->ClientHandle();
		params.iWindowOrigin=iCurrentWindow->Origin();
		CWsWindowGroup* groupWin=iCurrentWindow->WinGroup();
		params.iWindowGroupId=groupWin ? groupWin->Identifier() : 0;
		params.iSource=TPointerEventData::EUnspecified;
		CClick::OtherEvent(EEventPointer,&params);
		}
	ProcessEvent(event);
	}

void WsPointer::ProcessEvent(TWsEvent& aEvent)
	{
	TUint filter=iCurrentWindow->PointerFilter();
	TPointerEvent::TType type=aEvent.Pointer()->iType;
	if ((type!=TPointerEvent::EMove || !(filter&EPointerFilterMove)) &&
		 (type!=TPointerEvent::EDrag || !(filter&EPointerFilterDrag)))
		{
		TPoint pos=aEvent.Pointer()->iPosition;
		if ((type==TPointerEvent::EMove || type==TPointerEvent::EDrag) && iCurrentWindow->UsingPointerBuffer())
			CWsPointerBuffer::PointerEvent((CWsClientWindow *)iCurrentWindow,pos);
		else if (!WsKeyboardEmulator::PointerEvent(type,pos,iCurrentWindow->PointerKeyList()))
			ProcessPointerEvent(aEvent);
		}
	if (!MovesAvailable() && (type==TPointerEvent::EButton1Up || type==TPointerEvent::ESwitchOn))
		iCurrentWindow=NULL;
	PointerCursorUpdateCheck();
	}

void WsPointer::TimerExpired()
	{
	WS_ASSERT_DEBUG(iTimerQueued, EWsPanicPointerTimer);
	if (iUpdateRequired)
		{
		UpdatePointerCursor();
		iUpdateRequired=EFalse;
		}
	else
		{
		iTimerQueued=EFalse;
		iPeriodicTimer->Cancel();
		}
	}

void WsPointer::GetDoubleClickSettings(TTimeIntervalMicroSeconds32 &aTime, TInt &aDistance)
	{
	aTime=iDoubleClickMaxInterval;
	aDistance=iDoubleClickMaxDistance;
	}

void WsPointer::SetDoubleClick(const TTimeIntervalMicroSeconds32 &aTime, TInt aDistance)
	{
	iDoubleClickMaxInterval=aTime;
	iDoubleClickMaxDistance=aDistance;
	}

void WsPointer::PointerCursorUpdateCheck()
	{
	CWsPointerCursor *sprite=CalculatePointerCursor();
	if (iCursorSprite || sprite)	// If there either was, or is a pointer cursor we need an update
		{
		if (!iTimerQueued)
			{
			UpdatePointerCursorTo(sprite);
			iPeriodicTimer->Start(TTimeIntervalMicroSeconds32(EPointerUpdateGapInMicroSeconds),
									TTimeIntervalMicroSeconds32(EPointerUpdateGapInMicroSeconds),
									  TCallBack(PointerTimerCallBack,NULL));
			iTimerQueued=ETrue;
			}
		else
			iUpdateRequired=ETrue;
		}
	}

void WsPointer::UpdatePointerCursor()
	{
//__PROFILE_START(3);
	CWsPointerCursor *sprite=CalculatePointerCursor();
	UpdatePointerCursorTo(sprite);
//__PROFILE_END(3);
	}

void WsPointer::UpdatePointerCursorTo(CWsPointerCursor* aNewCursor)
	{
	CScreen* screen=NULL;
	if (iCursorSprite!=aNewCursor)
		{
		if (iCursorSprite)
			{
			iCursorSprite->Deactivate();
			screen=iCursorSprite->Screen();		//This will need changing ##
			}
		iCursorSprite=aNewCursor;
		if (iCursorSprite)
			{
			iCursorSprite->SetPos(iCurrentPos);
			iCursorSprite->Activate();
			}
		goto Update;
		}
	else if (iCursorSprite)
		{
		iCursorSprite->SetPos(iCurrentPos);
	Update:
		if (!screen)
			screen=iCursorSprite->Screen();
		screen->Update();
		}
	}

CWsPointerCursor* WsPointer::CalculatePointerCursor()
	{
	CWsPointerCursor *sprite=NULL;
	if (iCurrentWindow && (iPointerCursorMode==EPointerCursorNormal || iPointerCursorMode==EPointerCursorWindow))
		{
		const CWsWindowBase* window=iCurrentWindow;
		do	{
			sprite=window->PointerCursor();
			if (window->WinType()!=EWinTypeClient)
				break;
			window=window->BaseParent();
			} while (!sprite);
		}
	if (!sprite && iCurrentWindow && (iPointerCursorMode==EPointerCursorFixed || iPointerCursorMode==EPointerCursorNormal))
		sprite=CWsClient::DefaultSystemPointerCursor();
	return sprite;
	}

TEventQueueWalkRet PointerRepeatPurgeFunc(TAny *, TWsEvent *aEvent)
//
// Callback function for event queue walk
//
	{
	return(WsPointer::PointerRepeatPurgeCheck(aEvent));
	}

TBool WsPointer::PointerEventRepeatCheck(const TWsEvent *aEvent, TUint32 aHandle)
//
// Return ETrue if this pointer event is consumed by the pointer repeat
//
	{
	TPointerEvent *pntEvent=aEvent->Pointer();
	if (aHandle==iRepeatWindow->ClientHandle() && 
		 (pntEvent->iType==TPointerEvent::EDrag || pntEvent->iType==TPointerEvent::EMove) &&
		  iRepeatRect.Contains(pntEvent->iPosition))
		return(ETrue);
	return(EFalse);
	}

TEventQueueWalkRet WsPointer::PointerRepeatPurgeCheck(const TWsEvent *aEvent)
	{
	if (iRepeatWindow && aEvent->Type()==EEventPointer)
		{
		if (PointerEventRepeatCheck(aEvent,aEvent->Handle()))
			return(EEventQueueWalkDeleteEvent);	// Purge the event as it is a move/drag within the repeat rect
		CancelPointerRepeatEventRequest();
		}
	return(EEventQueueWalkOk);
	}

void WsPointer::RequestPointerRepeatEvent(CWsWindow *aWindow, TTimeIntervalMicroSeconds32 aTime,const TRect &aRect)
	{
	CancelPointerRepeatEventRequest();
	iRepeatWindow=aWindow;
	iRepeatRect=aRect;
	iRepeatTimer->After(aTime);
	aWindow->EventQueue()->WalkEventQueue(&PointerRepeatPurgeFunc,NULL);
	if (iRepeatWindow && !iRepeatRect.Contains(iCurrentPos-iRepeatWindow->Origin()))
		CancelPointerRepeatEventRequest();
	}

void WsPointer::CancelPointerRepeatEventRequest()
	{
	if (iRepeatWindow)
		{
		iRepeatWindow=NULL;
		iRepeatTimer->Cancel();
		}
	}

void WsPointer::RepeatTimerCompleted()
	{
	TWsEvent event;
	event.SetType(EEventPointer);
	event.SetTimeNow();
	event.Pointer()->iModifiers=TWindowServerEvent::GetModifierState();
	event.Pointer()->iPosition=iCurrentPos-iRepeatWindow->Origin();
	event.Pointer()->iParentPosition=iCurrentPos-iRepeatWindow->BaseParent()->Origin();
	event.Pointer()->iType=TPointerEvent::EButtonRepeat;
	QueuePointerEvent(iRepeatWindow, event);
	iRepeatWindow=NULL;
	}

#if defined(__WINS__)
void WsPointer::SetXyInputType(TXYInputType aXyInputType)
	{
	if (iXyInputType>EXYInputPointer && aXyInputType<EXYInputMouse && !iPointerDown)
		{
		iCurrentWindow=NULL;
		UpdatePointerCursor();
		}
	else if (iXyInputType<EXYInputMouse && aXyInputType>EXYInputPointer && !iPointerDown)
		{
		TPoint pos(iCurrentPos);
		TPoint parPos;
		ReLogCurrentWindow(pos,parPos,NULL);
		UpdatePointerCursor();
		}
	iXyInputType=aXyInputType;
	}
#endif

//

CWsPointerTimer::CWsPointerTimer() : CTimer(EPointerRepeatPriority)
	{}

void CWsPointerTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CWsPointerTimer::RunL()
	{
	User::ResetInactivityTime();
	WS_ASSERT_DEBUG(iStatus.Int()==KErrNone, EWsPanicPointerRepeatTimerStatus);
	WsPointer::RepeatTimerCompleted();
	}

//

CWsPointerBuffer::~CWsPointerBuffer()
	{
	if (this == iCurrentBuffer)
		{
		// We're about to be destroyed - don't want to be pointed at any more.
		iCurrentBuffer = NULL;
		}
	iList.Remove(*this);
	}

void CWsPointerBuffer::ConnectL(CWsClientWindow *aWindow, TInt aMaxPoints, TUint aFlags)
	{
	CWsPointerBuffer *pb=NULL;
	for(TSglQueIter<CWsPointerBuffer> iter(iList);(pb=iter++)!=NULL;)
		if (pb->iWindow==aWindow)
			User::Leave(KErrInUse);
	CWsPointerBuffer *pbuf=new(ELeave) CWsPointerBuffer;
	pbuf->iWindow=aWindow;
	pbuf->iMaxPoints=aMaxPoints;
	pbuf->iFlags=aFlags;
	iList.AddFirst(*pbuf);
	CleanupStack::PushL(pbuf);
	AdjustMaxSizeL();
	CleanupStack::Pop();
	}

void CWsPointerBuffer::Disconnect(CWsClientWindow *aWindow)
	{
	CWsPointerBuffer *pb=NULL;
	for(TSglQueIter<CWsPointerBuffer> iter(iList);(pb=iter++)!=NULL;)
		{
		if (pb->iWindow==aWindow)
			{
			delete pb; // Note that the destructor also sets iCurrentBuffer to NULL if it is pointing at pb
			TRAP_IGNORE(AdjustMaxSizeL());	// Shouldn't fail, but doesn't matter if it does as we simply have a larger buffer than needed
			break; // from for loop
			}
		}
	}

void CWsPointerBuffer::Reset()
	{
	iSignalled=EFalse;
	iPointerBuffer->Reset();
	}

void CWsPointerBuffer::SignalBufferReady()
	{
	if (!iSignalled)
		if (iCurrentBuffer && iCurrentBuffer->iWindow->QueueEvent(EEventPointerBufferReady))
			iSignalled=ETrue;
	}

void CWsPointerBuffer::PointerEvent(CWsClientWindow *aWindow,const TPoint &aPoint)
	{
	if (iCurrentBuffer==NULL || aWindow!=iCurrentBuffer->iWindow)
		{
		Reset();
		CWsPointerBuffer *pb=NULL;
		for(TSglQueIter<CWsPointerBuffer> iter(iList);(pb=iter++)!=NULL;)
			{
			if (pb->iWindow==aWindow)
				{
				iCurrentBuffer=pb;
				break; // from for loop
				}
			}
		}
	iPointerBuffer->Add(&aPoint);
	SignalBufferReady();
	}

void CWsPointerBuffer::RetrievePointerMoveBuffer(CWsClientWindow *aWindow,TInt aMaxPoints)
	{
	enum {KPointerMoveBufferSize=32};	// Puts 256 bytes on the stack
	if (iCurrentBuffer && aWindow==iCurrentBuffer->iWindow)
		{
		iSignalled=EFalse;
		TInt max=Min(aMaxPoints,iPointerBuffer->Count());
		TInt buflen=0;
		aWindow->WsOwner()->SetReply(max);
		TPoint point;
		TBuf8<KPointerMoveBufferSize*sizeof(TPoint)> pnts;
		for(TInt index=0;index<max;index++)
			{
			iPointerBuffer->Remove(&point);
			pnts.Append((TUint8 *)&point,sizeof(TPoint));
			buflen++;
			if (buflen==KPointerMoveBufferSize)
				{
				CWsClient::ReplyBuf(pnts);
				pnts.Zero();
				buflen=0;
				}
			}
		if (buflen>0)
			CWsClient::ReplyBuf(pnts);
		if (iPointerBuffer->Count())
			SignalBufferReady();
		}
	}

void CWsPointerBuffer::DiscardPointerMoveBuffer(TUint aHandle)
	{
	if (iCurrentBuffer && aHandle==iCurrentBuffer->iWindow->ClientHandle())
		Reset();
	}

void CWsPointerBuffer::DiscardPointerMoveBuffer(CWsClientWindow *aWindow)
	{
	if (iCurrentBuffer && aWindow==iCurrentBuffer->iWindow)
		Reset();
	}

void CWsPointerBuffer::AdjustMaxSizeL()
	{
	TInt max=0;
	CWsPointerBuffer *pb=NULL;
	for(TSglQueIter<CWsPointerBuffer> iter(iList);(pb=iter++)!=NULL;)
		if (pb->iMaxPoints>max)
			max=pb->iMaxPoints;
	if (max==0)
		{
		delete iPointerBuffer;
		iPointerBuffer=NULL;
		}
	else if (!iPointerBuffer)
		{
		CCirBuf<TPoint> *pointerBuffer=new(ELeave) CCirBuf<TPoint>;
		CleanupStack::PushL(pointerBuffer);
		pointerBuffer->SetLengthL(max);
		CleanupStack::Pop();
		iPointerBuffer=pointerBuffer;
		}
	else
		iPointerBuffer->SetLengthL(max);
	}
