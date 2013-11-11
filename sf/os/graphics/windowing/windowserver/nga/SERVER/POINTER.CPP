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
#include <hal.h>
#include "advancedpointereventhelper.h"
#include "graphics/pointereventdata.h"
#include "debughelper.h"

TTimeIntervalMicroSeconds32 TWsPointer::iDoubleClickMaxInterval;
TInt 			   TWsPointer::iDoubleClickMaxDistance;
CWsPointerCursor*  TWsPointer::iCursorSprite;
TPointerCursorMode TWsPointer::iPointerCursorMode=EPointerCursorNormal;
TXYInputType       TWsPointer::iXyInputType;
TBool 			   TWsPointer::iTimerQueued;
TBool 			   TWsPointer::iUpdateRequired;
CPeriodic* 		   TWsPointer::iPeriodicTimer;
CWsRootWindow*     TWsPointer::iRootWindow;
TInt			   TWsPointer::iMaxPointers;
TBool			   TWsPointer::iIs3DPointer;
RArray<TWsPointer> TWsPointer::iPointers;
TInt 			   TWsPointer::iPrimaryPointer = TAdvancedPointerEvent::EDefaultPointerNumber;
TInt			   TWsPointer::iPreviousPrimaryPointer;
TInt			   TWsPointer::iEnterCloseProximityThreshold;
TInt			   TWsPointer::iExitCloseProximityThreshold;
TInt			   TWsPointer::iEnterHighPressureThreshold;
TInt			   TWsPointer::iExitHighPressureThreshold;
TBool			   CWsPointerBuffer::iSignalled=EFalse;
CWsPointerBuffer*  CWsPointerBuffer::iCurrentBuffer=NULL;
CCirBuf<TPoint>*   CWsPointerBuffer::iPointerBuffer=NULL;
TSglQue<CWsPointerBuffer> CWsPointerBuffer::iList(_FOFF(CWsPointerBuffer,iQue));
TInt                TWsPointer::iYOffset;
#if defined(__WINS__)
TBool               TWsPointer::iEmulatorRotatePointerCoords;
#endif

static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_SwEvent,ECapabilitySwEvent);

void TWsPointer::InitStaticsL()
	{
	//This iYOffset setting is specific for capacitive touch screens, where user's finger is the pointer device.
	//This is typically used so that the pointer event location is more inline with where the user perceives their 
	//finger to be on the screen (for example, due to refraction and the relatively large touch area of a finger).
	iYOffset = 0;
    _LIT( KWSERVIniFileVarYShifting, "YSHIFTING");
    TBool fetchingSucceeded = WsIniFile->FindVar(KWSERVIniFileVarYShifting, iYOffset);
    WS_ASSERT_ALWAYS(iYOffset>=0, EWsPanicInvalidPointerOffset);
    if ( !fetchingSucceeded )
       {
       iYOffset = 0;
       }

#if defined(__WINS__)    
	//An emulator may or may not deploy a renderchain or displaydriver that supports rotated drawing.
	//On a real device target the coordinate system is always rotated together with wserv's screendevice.
	_LIT( KWSERVIniFileVarEmulatorRotPointCoords, "EMULATOR_ROTATE_POINTER_COORDS");
	iEmulatorRotatePointerCoords = WsIniFile->FindVar(KWSERVIniFileVarEmulatorRotPointCoords);
#endif

	const CScreen* screen = CWsTop::Screen();
	WS_ASSERT_ALWAYS(screen, EWsPanicNoScreen);
	iRootWindow = screen->RootWindow();
	
	TMachineInfoV1Buf machineInfo;
	UserHal::MachineInfo(machineInfo);
	iXyInputType=machineInfo().iXYInputType;
	
	// Read EPointerMaxPointers from HAL and check if reported value is consistent
	// with iXyInputType.
	// Even if HAL reports that device doesn't support any pointer, WSERV
	// always has to support at least one pointer for compatibility reasons
	// and for keeping state of pointer cursor.
	if(HAL::Get(HALData::EPointerMaxPointers,iMaxPointers)!=KErrNone)
		{
		iMaxPointers = 1;
		}
	else
		{
		WS_ASSERT_ALWAYS(iMaxPointers >= 0 && iMaxPointers <= TAdvancedPointerEvent::EMaximumWServNumberOfPointers,
						 EWsPanicMaxPointersOutOfRange);
		WS_ASSERT_ALWAYS(( XyInput() && iMaxPointers >  0) || (!XyInput() && iMaxPointers == 0),
						 EWsPanicMaxPointersInconsistent);
		if (iMaxPointers == 0)
			{
			iMaxPointers = 1;
			}
		}
	
	//** Log the number of pointers here i,e iMaxPointers
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: Number of pointers system supports %d", iMaxPointers);
#endif
	
	// Does device support Z coordinate of the pointers?
	if(HAL::Get(HALData::EPointer3D,iIs3DPointer)!=KErrNone)
		{
		iIs3DPointer=EFalse; // No API, then no 3D pointers
		}
	WS_ASSERT_ALWAYS(!iIs3DPointer || XyInput(), EWsPanicPointer3DInconsistent);
	
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: Z coordinate supported %d", iIs3DPointer);
#endif	
	
	// Initialize thresholds for EEnterCloseProximity, EExitCloseProximity,
	// EEnterHighPressure and EExitHightPressure events.
	if(HAL::Get(HALData::EPointer3DEnterCloseProximityThreshold,
			iEnterCloseProximityThreshold) != KErrNone)
		{
		iEnterCloseProximityThreshold = KMaxTInt;
		}
	if(HAL::Get(HALData::EPointer3DExitCloseProximityThreshold,
			iExitCloseProximityThreshold) != KErrNone)
		{
		iExitCloseProximityThreshold  = KMinTInt;
		}
	if(HAL::Get(HALData::EPointer3DEnterHighPressureThreshold,
			iEnterHighPressureThreshold)  != KErrNone)
		{
		iEnterHighPressureThreshold   = KMaxTInt;
		}
	if(HAL::Get(HALData::EPointer3DExitHighPressureThreshold,
			iExitHighPressureThreshold)   != KErrNone)
		{
		iExitHighPressureThreshold    = KMinTInt;
		}
	
	iPointers = RArray<TWsPointer>(iMaxPointers);
	TWsPointer emptyPointer;	
	emptyPointer.iRepeatTimer = NULL;
	emptyPointer.Clear();
	for (TInt ii = 0; ii < iMaxPointers; ii++)
		{
		emptyPointer.iNumber = ii;
		User::LeaveIfError(iPointers.Append(emptyPointer));
		RepeatTimer(ii) = CWsPointerTimer::NewL(iPointers[ii]);
		}
	
	iPeriodicTimer=CPeriodic::NewL(EPointerCursorPriority);
	}

void TWsPointer::Clear()
	{
	iState = EPointerStateOutOfRange;
	iPos.iX = 0;
	iPos.iY = 0;
	iPressureProximity = 0;
	iCurrentWindow=MovesAvailable() ? iRootWindow : NULL;
	iActualWinPointerIsOver = NULL;
	iGrabWindow = NULL;
	iLastUnmatchedDown1 = NULL;
	iLastUnmatchedDown2 = NULL;
	iLastUnmatchedDown3 = NULL;
	iLastUnmatchedEnterHighPressure = NULL;
	iPrevClickWindow = NULL;
	iInCloseProximity = EFalse;
	iInHighPressure = EFalse;
	CancelPointerRepeatEventRequest();	
	}

/**
Turns off pointer cursor, deletes permenently pointer cursor update timer and pointer event repeat timer.
*/
void TWsPointer::Stop()
	{
	SetPointerCursorMode(EPointerCursorNone);
	UpdatePointerCursor();
	delete iPeriodicTimer;
	}

void TWsPointer::DeleteStatics()
	{
	for (TInt ii = 0; ii < iMaxPointers; ii++)
		{
		delete RepeatTimer(ii);	
		}		
	iPointers.Close();
	}

void TWsPointer::SetPointerCursorPos(TPoint aPos)
	{
	RestrictPos(aPos,EFalse);
	iPointers[iPrimaryPointer].iPos=aPos; 
	iPointers[iPrimaryPointer].ReLogCurrentWindow();
	UpdatePointerCursor();
	}

void TWsPointer::SendEnterExitEvent(TEventCode aType)
	{
	if (iCurrentWindow
		&& (iState != EPointerStateOutOfRange)
		&& (iNumber == iPrimaryPointer || iCurrentWindow->AdvancedPointersEnabled())
		&& !(iCurrentWindow->PointerFilter()&EPointerFilterEnterExit)
		&& !iCurrentWindow->ShutDownInProgress())
		{
		iCurrentWindow->QueueEvent(aType, iNumber);
		}
	}

void TWsPointer::SetCurrentWindow(const CWsWindow* aWin)
	{
	if (aWin!=iCurrentWindow)
		{
		SendEnterExitEvent(EEventPointerExit);
		iCurrentWindow=aWin;
		SendEnterExitEvent(EEventPointerEnter);
		}
	}

/**
Relogs the current window (sets iCurrentWindow) for this pointer. 
Retrieves pointer relative position in current window and pointer position in current 
window's parent window. 
*/
void TWsPointer::ReLogCurrentWindow(TPoint &aPos, TPoint &aParentPos, const CWsWindowGroup* aForceInGroup)
	{
	if (iRootWindow)
		{
		SetCurrentWindow(iRootWindow->PointerWindow(iPos,&aPos,&aParentPos,iGrabWindow,
				                                    iActualWinPointerIsOver,aForceInGroup));
		}
	else
		{
		iCurrentWindow=NULL;
		}
	}

/**
Relog all pointer's current windows when the window layout has changed.
*/
void TWsPointer::ReLogPointersCurrentWindows()
	{
	for (TInt ii = 0; ii < iMaxPointers; ii++)
		{
		TWsPointer& pointer = iPointers[ii];
		pointer.ReLogCurrentWindow();
		}
	}

/*
Relog this pointer's current pointer window when the window layout has changed.
Works similarly to ReLogCurrentWindow(TPoint &aPos, TPoint &aParentPos, const CWsWindowGroup* aForceInGroup),
but doesn't set relative positions.
*/
void TWsPointer::ReLogCurrentWindow()
	{
	if (iCurrentWindow)
		{
		SetCurrentWindow(iRootWindow->PointerWindow(iPos,NULL,NULL,iGrabWindow,iActualWinPointerIsOver,NULL));
		}
	}

/*
Called when a window has changed it's filter state, will trigger a 'Enter' message if the window
is the current window
*/
void TWsPointer::ReLogWindow(const CWsWindow* aWin)
	{
	for (TInt ii = 0; ii < iMaxPointers; ii++)
		{
		if (aWin == iPointers[ii].iCurrentWindow)
			{
			iPointers[ii].SendEnterExitEvent(EEventPointerEnter);
			}
		}
	}

void TWsPointer::UnmatchedEventPurged(TPointerEvent::TType aPointerType, TUint aHandle)
	{
	if (aPointerType==TPointerEvent::EButton1Up)
		{
		if (iGrabWindow && iGrabWindow->ClientHandle()==aHandle)
			{
			iGrabWindow=NULL;
			}
		if (iRepeatWindow && iRepeatWindow->ClientHandle()==aHandle)
			{
			CancelPointerRepeatEventRequest();
			}
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
		case TPointerEvent::EExitHighPressure:
			iLastUnmatchedEnterHighPressure=aHandle;
			break;
		default:;
		}
	}

void TWsPointer::WindowDisconnected(const CWsWindow* deletedWindow)
	{
	for (TInt pointerNum = 0; pointerNum < iMaxPointers; pointerNum++)
		{
		TWsPointer& pointer = iPointers[pointerNum];
		if (pointer.iRepeatWindow==deletedWindow)
			{
			pointer.CancelPointerRepeatEventRequest();
			}		
		if (pointer.iGrabWindow==deletedWindow)
			{
			pointer.iGrabWindow=NULL;
			}
		if (pointer.iCurrentWindow==deletedWindow)
			{
			pointer.ReLogCurrentWindow();
			if (pointerNum == iPrimaryPointer)
				{
				UpdatePointerCursor();
				}
			}
		}
	}

/*
Callback function pointer for up event remove event queue walk
*/
TEventQueueWalkRet RemovePointerUpFunc(TAny* aHandle, TWsEvent* aEvent)
	{
	if (aEvent->Type()==EEventPointer && aEvent->Pointer()->iType==TPointerEvent::EButton1Up && (*(TUint *)aHandle)==aEvent->Handle())
		{
		return(EEventQueueWalkDeleteEvent);
		}
	return(EEventQueueWalkOk);
	}

/*
If the specified pointer is down claim grab in aWindow as though the down event had 
gone to this window. Also send an up event to the window (if any) that would receive it if the 
pointer was released now.
If no pointer is specifed do the above for the primary pointer.

@return	KErrNone if successful,
		KErrNotFound if pointernumber out of range,
		KErrNotSupported if incorrect pointer grab claimed for window in emulation mode,
		KErrPermissionDenied if trying to grab from a different window owner without the required capability.
*/
TInt TWsPointer::ClaimGrab(const CWsWindow *aWindow,const TWsWinCmdGrabControl& aGrabControl)
	{
	TInt pointerNumber(aGrabControl.HasPointerNumber() ? aGrabControl.pointerNumber : iPrimaryPointer);
	TBool advancedPointersEnabled(aWindow->AdvancedPointersEnabled());
	TInt errNo(KErrNone);

	if(!advancedPointersEnabled	&& aGrabControl.HasPointerNumber() && (TAdvancedPointerEvent::EDefaultPointerNumber!=pointerNumber))
		{
		// The window is in emulation mode, and cannot get events for anything other than the primary pointer		
		errNo=KErrNotSupported;
		}
	else if(advancedPointersEnabled && ((pointerNumber<0) || (pointerNumber>=iMaxPointers)))
		{
		// Unknown pointer number
		errNo=KErrNotFound;
		}
	else if(aGrabControl.HasPointerNumber()	&& (aWindow->WsOwner()!=iPointers[pointerNumber].iCurrentWindow->WsOwner())
			&& !KSecurityPolicy_SwEvent().CheckPolicy(aWindow->WsOwner()->ClientMessage(), __PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWindowBase::ClaimPointerGrab")))
		{
		// Trying to grab from a different window owner requires the relevant permission
		//     - Only for the multipointer API, RWindowBase::ClaimPointerGrab(TInt, TBool), which provides a pointer number
		//     - Doing so for the legacy non-multipointer API, RWindowBase::ClaimPointerGrab(TBool), would be a compatibility break
		errNo=KErrPermissionDenied;
		}
	else
		{
		iPointers[pointerNumber].ClaimGrab(aWindow,aGrabControl.CheckFlags(TWsWinCmdGrabControl::ESendUpEvent));		
		}	
	
	return errNo;
	}

/*
If this pointer is down claim grab in aWindow as though the down event had 
gone to this window. Also send an up event to the window (if any) that would receive it if the 
pointer was released now.
*/
void TWsPointer::ClaimGrab(const CWsWindow* aWindow,TBool aSendUpEvent)
	{	
	TInt modState=TWindowServerEvent::GetModifierState();
	TWsEvent event;
	TAdvancedPointerEvent& pointerEvent=*event.Pointer();
	TAdvancedPointerEventHelper::InitAdvancedPointerEvent(event, TPointerEvent::EButton1Up,modState,TPoint3D(iPos.iX,iPos.iY,iPressureProximity),iNumber);
	if (iState == EPointerStateDown)
		{
		if (iCurrentWindow!=aWindow)
			{
			if (aSendUpEvent)
				{
				ProcessEvent(event,NULL,EFalse);
				}
			else // If up event already in queue purge it
				{
				CEventQueue* eventQueue = iCurrentWindow->EventQueue();
				if (eventQueue)
					{
					TUint handle=iCurrentWindow->ClientHandle();
					eventQueue->WalkEventQueue(&RemovePointerUpFunc,&handle);
					}
				}
			iState = EPointerStateDown;
			if (aWindow->HasPointerGrab())
				{
				iGrabWindow=aWindow;
				}
			ReLogCurrentWindow(pointerEvent.iPosition,pointerEvent.iParentPosition,NULL);
			pointerEvent.iType=TPointerEvent::EDrag;
			ProcessPointerEvent(event);
			}
		}
	else if (iState == EPointerStateUp)
		{
		const CWsWindow *current=iCurrentWindow;
		iCurrentWindow=aWindow;
		WS_ASSERT_DEBUG(iGrabWindow==NULL, EWsPanicPointerClaimGrab);
		iGrabWindow=aWindow;	// Force the up event to be sent to aWindow
		ReLogCurrentWindow(pointerEvent.iPosition,pointerEvent.iParentPosition,NULL);
		ProcessPointerEvent(event);
		iGrabWindow=NULL;
		iCurrentWindow=current;
		}
	}

/**
@return ETrue if matching event has been purged, so current event should not be delivered,
        EFalse otherwise.
*/
TBool TWsPointer::CheckMatchingEventPurged(TPointerEvent::TType aType)
	{
	switch(aType)
		{
		TUint lastUnmatchedDown;
		case TPointerEvent::EButton1Up:
			lastUnmatchedDown=iLastUnmatchedDown1;
			iLastUnmatchedDown1=0;
			iLastUnmatchedEnterHighPressure=0;
			goto lastUnmatchedDownCheck;
		case TPointerEvent::EButton2Up:
			lastUnmatchedDown=iLastUnmatchedDown2;
			iLastUnmatchedDown2=0;
			goto lastUnmatchedDownCheck;
		case TPointerEvent::EButton3Up:
			lastUnmatchedDown=iLastUnmatchedDown3;
			iLastUnmatchedDown3=0;
			goto lastUnmatchedDownCheck;
		case TPointerEvent::EExitHighPressure:
			lastUnmatchedDown=iLastUnmatchedEnterHighPressure;
			iLastUnmatchedEnterHighPressure=0;
lastUnmatchedDownCheck:
		if (lastUnmatchedDown==iCurrentWindow->ClientHandle())
			{
			return ETrue; // Don't deliver the event as we've already thrown away matching event
			}
		default:		//Should never get to default
			break;
		}
	return EFalse;
	}

TBool TWsPointer::QueuePointerEvent(const CWsWindow* aWindow, TWsEvent &aEvent)
	{
	if (aWindow->WsOwner() &&
		(aWindow->AdvancedPointersEnabled() || 
		TAdvancedPointerEventHelper::PointerNumber(aEvent) == iPrimaryPointer))
		{
		CEventQueue* queue=aWindow->EventQueue();
		aEvent.SetHandle(aWindow->ClientHandle());
#ifdef LOG_WSERV_EVENTS
		RDebug::Printf("_WSEVENT_POINTER: TWsPointer::QueuePointerEvent with AdvancedPointerEnabled");
#endif
		if (aEvent.Handle()!=0)
			{
 			if(!aWindow->AdvancedPointersEnabled())
 				{
 				// Re-assign from WServ primary pointer number, to EDefaultPointerNumber for Cone and other clients.
 				// This should not get confused with any real pointer of the same number due to the TWsEvents' window handle
 				// being different.
 				TAdvancedPointerEventHelper::SetPointerNumber(aEvent,TAdvancedPointerEvent::EDefaultPointerNumber);
 				aEvent.Pointer()->iModifiers&=~EModifierAdvancedPointerEvent;  // Clear the advanced pointer flag
 				}
			if (queue->UpdateLastPointerEvent(aEvent))
				{
				return EFalse;
				}
			TWservEventPriorities priority=EEventPriorityLow;
			switch (aEvent.Pointer()->iType)
				{
				case TPointerEvent::EButton1Up:
				case TPointerEvent::EButton2Up:
				case TPointerEvent::EButton3Up:
				case TPointerEvent::EExitHighPressure:
					if (CheckMatchingEventPurged(aEvent.Pointer()->iType))
						{
						#ifdef LOG_WSERV_EVENTS
						RDebug::Printf("_WSEVENT_POINTER: Check matching event has been purged so no addition of event 01");
						#endif
						return ETrue;
						}
					if (queue->CheckRoom())
						{
						if (CheckMatchingEventPurged(aEvent.Pointer()->iType))
							{
							#ifdef LOG_WSERV_EVENTS
							RDebug::Printf("_WSEVENT_POINTER: Check matching event has been purged so no addition of event 02");
							#endif
							return ETrue;
							}
						}
					/*Fall Through if an event was not purged*/
				case TPointerEvent::EButton1Down:
				case TPointerEvent::EButton2Down:
				case TPointerEvent::EButton3Down:
				case TPointerEvent::EEnterHighPressure:
				case TPointerEvent::EOutOfRange:
					priority=EEventPriorityHigh;
					break;
				default:;
				}
#ifdef LOG_WSERV_EVENTS
			RDebug::Printf("_WSEVENT_POINTER: TWsPointer::QueuePointerEvent After adding event to clientqueue Event State %d ", iState);
#endif
			queue->QueueEvent(aEvent,priority);
			}
		}
	return EFalse;
	}

/*
Moves the window group which contains this pointer's iCurrentWindow on top.
*/
void TWsPointer::ProcessForegroundCheck()
	{
	CWsWindowGroup* group=((CWsTopClientWindow *)iCurrentWindow)->TopClientWindow()->Parent();
	if (group->iFlags&CWsWindowGroup::EGroupFlagAutoForeground)
		{
		group->SetOrdinalPosition(0);
		}
	}

/*
Pointer Event Processing - stage 3 of 3:
- setting event's time
- auto foreground check
- double clicks detection
- pointer repeats
- add event to client's queue
- drag&drop capturing
*/
void TWsPointer::ProcessPointerEvent(TWsEvent& aEvent)
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
					{
					iPrevClickWindow=iCurrentWindow;
					}
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
				{
				return;
				}
			CancelPointerRepeatEventRequest();
			}
		if (QueuePointerEvent(iCurrentWindow, aEvent))
			{
			return;
			}
		if (iCurrentWindow->DragDropCapture())
			{
			aEvent.SetType(EEventDragDrop);
			QueuePointerEvent(iActualWinPointerIsOver, aEvent);
			}
		}
	}

TInt PointerTimerCallBack(TAny *)
	{
	TWsPointer::TimerExpired();
	return(KErrNone);
	}

void TWsPointer::RestrictPos(TPoint& aPos,TBool aWithinDrawableArea/*=ETrue*/)
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
TBool TWsPointer::PreProcessDriverEvent(TRawEvent &aRawEvent,TBool aFromHardware/*=EFlase*/)
#else
TBool TWsPointer::PreProcessDriverEvent(TRawEvent &aRawEvent)
#endif
	{
	TRawEvent::TType type=aRawEvent.Type();
	
	if (!IsPointerEventType(type)) 
		return ETrue;
	
	if (!XyInput())
		{
		return EFalse;
		}
	
	//** Log the type, pointer number, and its coordinates
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: Pointer number = %d RawEvent Type = %d Coordinates [%d, %d]", 
					aRawEvent.PointerNumber(), type, aRawEvent.Pos().iX, aRawEvent.Pos().iY);
#endif
	
	// check correctness of aRawEvent.PointerNumber()
	if (iMaxPointers > 1)
		{
		if (aRawEvent.PointerNumber() >= iMaxPointers)
			{
			return EFalse;
			}
		}
	else
		{
		aRawEvent.SetPointerNumber(0);
		}
	
	if (type != TRawEvent::EPointer3DOutOfRange)
		{
		// operations on coordinates are valid for all types except EPointer3DOutOfRange
		TPoint xy=aRawEvent.Pos();
		if (DeltaMouse())
			{
	#if defined(__WINS__)
			if (aFromHardware)
				return EFalse;
	#endif
			if (type==TRawEvent::EPointerMove)
				{
				xy+=iPointers[aRawEvent.PointerNumber()].iPos;
				ShiftYCoordinate(xy.iY);
				RestrictPos(xy);
				}
			else
				xy=iPointers[aRawEvent.PointerNumber()].iPos;
			}
		else
			{
	#if !defined(__WINS__)
			TranslateCoordsOnRotation(xy);
	#else
			if(iEmulatorRotatePointerCoords)
				{
				//emulators that support rotated drawing and touch may want to enable 
				//rotation of pointer events in wsini.
				TranslateCoordsOnRotation(xy);
				}
	#endif
			CScreen* screen=iRootWindow->Screen();
			ShiftYCoordinate(xy.iY);
			// Move the raw event position by shifting it by Origin and scale
			xy=screen->PhysicalToLogical(xy);
			RestrictPos(xy);
			}
		aRawEvent.Set(type, xy.iX, xy.iY, 
					  iIs3DPointer ? aRawEvent.Pos3D().iZ : 0);
		}
	
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: Coordinates after Rotation and shift [%d, %d]", 
							aRawEvent.Pos().iX, aRawEvent.Pos().iY);
#endif
	return ETrue;
	}

void TWsPointer::TranslateCoordsOnRotation(TPoint& aPoint)
	{
	CScreen* screen=iRootWindow->Screen();
	TSize screenSize=screen->SizeInPixels()-TSize(1,1);     //This is in the current rotation
	switch (screen->Orientation())
		{
		case CFbsBitGc::EGraphicsOrientationRotated90:
			aPoint.SetXY(aPoint.iY,screenSize.iHeight-aPoint.iX);
			break;
		case CFbsBitGc::EGraphicsOrientationRotated180:
			aPoint=-(aPoint-screenSize);
			break;
		case CFbsBitGc::EGraphicsOrientationRotated270:
			aPoint.SetXY(screenSize.iWidth-aPoint.iY,aPoint.iX);
			break;
		default:;       //To stop warning
		}
	}

/**
 * Validates events sent to the Window Server by its Client (Anim or Window Group).
 * May overwrite aRawEvent's Z coordinate and/or pointer number if Client or digitizer driver
 * doesn't support them.
 * @param aRawEvent event to validate
 * @param aAdvancedPointersEnabled ETrue if Client supports advanced pointer's data (Z coordiante
 *                                 and pointer number); EFalse otherwise.
 * @return ETrue if aRawEvent should be processed by Window Server. EFalse if it should be ignored.
 */
TBool TWsPointer::PreProcessClientEvent(TRawEvent &aRawEvent, TBool aAdvancedPointersEnabled)
	{
	TRawEvent::TType type=aRawEvent.Type();
	
	if (!IsPointerEventType(type)) 
		return ETrue;
	
	// validate pointer number
	if (aAdvancedPointersEnabled)
		{
		// ignore event if both digitizer driver and Client support pointer numbers, but number is invalid
		if (aRawEvent.PointerNumber() >= iMaxPointers)
			{
			return EFalse;
			}		
		}
	else
		{
		// set to iPrimaryPointer if Client doesn't support pointer numbers
		aRawEvent.SetPointerNumber(iPrimaryPointer);
		}
	
	// validate Z coordinate for all pointer events except EPointer3DOutOfRange
	if (type != TRawEvent::EPointer3DOutOfRange)
		{
		if (!iIs3DPointer)
			{
			// set Z to 0 for all events when the digitizer driver doesn't support Z coordinate
			TPoint3D xyz=aRawEvent.Pos3D();
			aRawEvent.Set(type, xyz.iX, xyz.iY, 0);
			}
		else if (!aAdvancedPointersEnabled)
			{
			// set Z to actual value if the digitizer driver does support Z coordinate but Client doesn't 
			TPoint3D xyz=aRawEvent.Pos3D();
			aRawEvent.Set(type, xyz.iX, xyz.iY, iPointers[aRawEvent.PointerNumber()].iPressureProximity);
			}
		}
	
	return ETrue;
	}

TBool TWsPointer::IsPointerEventType(TRawEvent::TType aType)
	{
#if defined(__WINS__)
	WS_ASSERT_DEBUG(TRawEvent::EPointerMove==1, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EPointerMove+1==TRawEvent::EPointerSwitchOn, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EPointerSwitchOn+8==TRawEvent::EButton1Down, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EButton1Down+5==TRawEvent::EButton3Up, EWsPanicRawEventsTypeChanged);
	WS_ASSERT_DEBUG(TRawEvent::EButton3Up+6==TRawEvent::EPointer3DOutOfRange, EWsPanicRawEventsTypeChanged);
#endif
	return (aType == TRawEvent::EPointerMove) ||
		   (aType == TRawEvent::EPointerSwitchOn) ||
		   (aType >= TRawEvent::EButton1Down && aType <= TRawEvent::EButton3Up) ||
		   (aType == TRawEvent::EPointer3DOutOfRange);
	}

/*
Pointer Event processing.

This method redirects pointer event processing to proper TWsPointer object.
*/
void TWsPointer::ProcessWsEvent(TWsEvent& aEvent,const CWsWindowGroup* aForceInGroup,TBool aNatural)
	{
	if(iPrimaryPointer!=iPreviousPrimaryPointer)
		{
		// The primary pointer may be updated while the TRawEvent is being processed, but this TRawEvent may be
		// then be consumed by an anim.
		// If it hasn't then we can leave any repeat request.
		CancelPointerRepeatEventRequest(iPreviousPrimaryPointer);
		}
	
	if (aEvent.Pointer()->iType == TPointerEvent::EOutOfRange)
		{
		iPointers[TAdvancedPointerEventHelper::PointerNumber(aEvent)].ProcessOutOfRangeEvent(aEvent, aForceInGroup, aNatural);
		}
	else
		{
		iPointers[TAdvancedPointerEventHelper::PointerNumber(aEvent)].ProcessEvent(aEvent, aForceInGroup, aNatural);
		}
	}

/*
Pointer Event Processing - stage 1 of 3:
- updating this pointer's state: coordinates and grabbing window
- updating coordinates of event
- updating event type to Drag
- simulated moves
- generating events: EEnterCloseProximity, EExitCloseProximity, EEnterHighPressure, EExitHighPressure
*/
void TWsPointer::ProcessEvent(TWsEvent& aEvent,const CWsWindowGroup* aForceInGroup,TBool aNatural)
 	{
	TAdvancedPointerEvent& pointerEvent = *aEvent.Pointer();
	TPointerEvent::TType eventType=pointerEvent.iType;
	
 	if (iState == EPointerStateOutOfRange)
 		{
 		// new pointer comes in range, so clear information after previous one 		
 		Clear();
 		}
 	
	// update coordinates
	iPos=pointerEvent.iPosition;
	iPressureProximity=TAdvancedPointerEventHelper::Z(aEvent);
	
	if (eventType == TPointerEvent::EMove && !MovesAvailable() && iState != EPointerStateDown)
		{
		return;
		}
	
	// re-log and update parent position
	TPoint parPos;
	ReLogCurrentWindow(pointerEvent.iPosition,parPos,aForceInGroup);
	pointerEvent.iParentPosition=parPos;
	
	
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: TWsPointer::ProcessEvent Event to be sent to this window %U", reinterpret_cast<TUint32>(iCurrentWindow));
	RDebug::Print(_L("_WSEVENT_POINTER: TWsPointer::ProcessEvent EventName %S and Event State %d "), &WsEventName(aEvent), iState);
#endif
	// update state
	switch(eventType)
		{
		case TPointerEvent::EButton1Down:
			if (iGrabWindow==NULL && iCurrentWindow->HasPointerGrab())
				{
				iGrabWindow=iCurrentWindow;
				}
			if (!MovesAvailable() && iCurrentWindow->PointerFilter()&EPointerGenerateSimulatedMove)
				{
				pointerEvent.iType=TPointerEvent::EMove;
				ProcessEvent(aEvent, EFalse);
				pointerEvent.iType=TPointerEvent::EButton1Down;
				}
			switch(iState)
				{
				case EPointerStateOutOfRange:
					iState = EPointerStateDown;
					SendEnterExitEvent(EEventPointerEnter);
					// intentional lack of break statement
				case EPointerStateUp:
					iState = EPointerStateDown;
					ProcessEvent(aEvent, aNatural);
					if (iPressureProximity >= iEnterHighPressureThreshold)
						{
						iInHighPressure = ETrue;
						pointerEvent.iType = TPointerEvent::EEnterHighPressure;
						ProcessEvent(aEvent, EFalse);
						}
					else
						{
						iInHighPressure = EFalse;
						}
					break;
				case EPointerStateDown:
					if (iInHighPressure && iPressureProximity < iExitHighPressureThreshold) 
						{
						iInHighPressure = EFalse;
						eventType = TPointerEvent::EExitHighPressure;
						}
					else if (!iInHighPressure && iPressureProximity >= iEnterHighPressureThreshold)
						{
						iInHighPressure = ETrue;
						eventType = TPointerEvent::EEnterHighPressure;
						}
					ProcessEvent(aEvent, aNatural);
					break;
				}
			break;
		case TPointerEvent::EButton1Up:
			iGrabWindow=NULL;
			switch(iState)
				{
				case EPointerStateDown:
					iState = EPointerStateUp;
					ProcessEvent(aEvent, aNatural);
					if (iPressureProximity < iExitCloseProximityThreshold)
						{
						pointerEvent.iType = TPointerEvent::EExitCloseProximity;
						iInCloseProximity = EFalse;
						ProcessEvent(aEvent, EFalse);
						}
					else
						{
						iInCloseProximity = ETrue;
						}
					break;
				case EPointerStateOutOfRange:
					iState = EPointerStateUp;
					SendEnterExitEvent(EEventPointerEnter);
					// intentional lack of break statement
				case EPointerStateUp:
					if (iInCloseProximity &&
						iPressureProximity < iExitCloseProximityThreshold)
						{
						iInCloseProximity = EFalse;
						pointerEvent.iType = TPointerEvent::EExitCloseProximity;
						}
					else if (!iInCloseProximity &&
							 iPressureProximity >= iEnterCloseProximityThreshold)
						{
						iInCloseProximity = ETrue;
						pointerEvent.iType = TPointerEvent::EEnterCloseProximity;
						}
					ProcessEvent(aEvent, aNatural);
					break;
				}
			break;
		case TPointerEvent::EMove:
			switch(iState)
				{
				case EPointerStateDown:
					if (iInHighPressure && 
						iPressureProximity < iExitHighPressureThreshold) 
						{
						iInHighPressure = EFalse;
						pointerEvent.iType = TPointerEvent::EExitHighPressure;
						}
					else if (!iInHighPressure &&
							iPressureProximity >= iEnterHighPressureThreshold)
						{
						iInHighPressure = ETrue;
						pointerEvent.iType = TPointerEvent::EEnterHighPressure;
						}
					else
						{
						pointerEvent.iType = TPointerEvent::EDrag;
						}
					break;
				case EPointerStateUp:
					if (iInCloseProximity &&
						iPressureProximity < iExitCloseProximityThreshold)
						{
						iInCloseProximity = EFalse;
						pointerEvent.iType = TPointerEvent::EExitCloseProximity;
						}
					else if (!iInCloseProximity &&
							 iPressureProximity >= iEnterCloseProximityThreshold)
						{
						iInCloseProximity = ETrue;
						pointerEvent.iType = TPointerEvent::EEnterCloseProximity;
						}
					break;
				case EPointerStateOutOfRange:
					iState = EPointerStateUp;
					SendEnterExitEvent(EEventPointerEnter);
					if (iPressureProximity >= iEnterCloseProximityThreshold)
						{
						iInCloseProximity = ETrue;
						pointerEvent.iType = TPointerEvent::EEnterCloseProximity;
						}
					break;
				}
			ProcessEvent(aEvent, aNatural);
			break;
		default:
			ProcessEvent(aEvent, aNatural);
			break;
		}
	}

/**
Processes OutOfRange events:
- injects event to key click plugin
- directs event to the last current window
- sends Exit event
*/
void TWsPointer::ProcessOutOfRangeEvent(TWsEvent& aEvent,const CWsWindowGroup* aForceInGroup, TBool aNatural)
	{
	if (iState != EPointerStateOutOfRange)
		{
#ifdef LOG_WSERV_EVENTS
       RDebug::Printf("_WSEVENT_POINTER: TWsPointer::ProcessOutOfRangeEvent Pointer Number = %d, iState =%d ", iNumber, iState); 
#endif

		// OutOfRange event generated by driver doesn't contain correct coordinates,
		// we update them from last state in order to deliver event to the proper window.
		SendEnterExitEvent(EEventPointerExit);
		
		iState = EPointerStateOutOfRange;
		
		TAdvancedPointerEventHelper::SetPointerNumber(aEvent, iNumber);
		TAdvancedPointerEventHelper::SetZ(aEvent, iPressureProximity);
		
		TAdvancedPointerEvent& pointerEvent = *aEvent.Pointer();		
		iCurrentWindow=iRootWindow->PointerWindow(iPos,&pointerEvent.iPosition,&pointerEvent.iParentPosition,iGrabWindow,
			                   					  iActualWinPointerIsOver,aForceInGroup);
		ProcessEvent(aEvent, aNatural);
		}
	}

void TWsPointer::NotifyCClick(TAdvancedPointerEvent& aPointerEvent)
	{
	if (CClick::IsHandler())
		{
		CClick::PointerEvent(iPos,aPointerEvent);
		TPointerEventData params;
		params.iVersion=0;
		params.iCurrentPos=iPos;
		TAdvancedPointerEventHelper::Copy(aPointerEvent, params.iPointerEvent);
		params.iClientHandle=iCurrentWindow->ClientHandle();
		params.iWindowOrigin=iCurrentWindow->Origin();
		CWsWindowGroup* groupWin=iCurrentWindow->WinGroup();
		params.iWindowGroupId=groupWin ? groupWin->Identifier() : 0;
		params.iSource=TPointerEventData::EUnspecified;
		CClick::OtherEvent(EEventPointer,&params);
		}
	}

/*
Pointer Event Processing - stage 2 of 3:
- injecting events to key click plugin
- pointer filtering
- injection to event buffer
- injection to keyboard emulator
- clearing iCurrentWindow if going up and !MovesAvailable()
- updating pointer cursor
*/
void TWsPointer::ProcessEvent(TWsEvent& aEvent, TBool aNatural)
	{
	if (aNatural)
		{
		NotifyCClick(*aEvent.Pointer());
		}
	
	TUint filter=iCurrentWindow->PointerFilter();
	TPointerEvent::TType type=aEvent.Pointer()->iType;
	if ((type!=TPointerEvent::EMove || !(filter&EPointerFilterMove)) &&
		 (type!=TPointerEvent::EDrag || !(filter&EPointerFilterDrag)))
		{
		if (iNumber == iPrimaryPointer)
			{
			TPoint pos=aEvent.Pointer()->iPosition;
			if ((type==TPointerEvent::EMove || type==TPointerEvent::EDrag) && iCurrentWindow->UsingPointerBuffer())
				{
				CWsPointerBuffer::PointerEvent((CWsClientWindow *)iCurrentWindow,pos);
				}
			else if ((type==TPointerEvent::EEnterCloseProximity || type==TPointerEvent::EExitCloseProximity || 
					  type==TPointerEvent::EEnterHighPressure   || type==TPointerEvent::EExitHighPressure) && 
					 iCurrentWindow->UsingPointerBuffer())
				{
				CWsPointerBuffer::PointerEvent((CWsClientWindow *)iCurrentWindow,pos);
				ProcessPointerEvent(aEvent);
				}
			else if (!WsKeyboardEmulator::PointerEvent(type,pos,iCurrentWindow->PointerKeyList()))
				{
#ifdef LOG_WSERV_EVENTS
				RDebug::Printf("_WSEVENT_POINTER: Calling ProcessPointerEvent for primary pointer");
#endif
				ProcessPointerEvent(aEvent);
				}
			}
		else if (!iCurrentWindow->UsingPointerBuffer() || (type != TPointerEvent::EMove && type != TPointerEvent::EDrag))
			{
#ifdef LOG_WSERV_EVENTS
			RDebug::Printf("_WSEVENT_POINTER: Calling ProcessPointerEvent for non primary pointer");
#endif
			ProcessPointerEvent(aEvent);
			}
		}
	if (!MovesAvailable() && (type==TPointerEvent::EButton1Up || 
			                  type==TPointerEvent::ESwitchOn  ||
			                  type==TPointerEvent::EOutOfRange))
		{
		iCurrentWindow=NULL;
		}
	if (iNumber == iPrimaryPointer)
		{
		PointerCursorUpdateCheck();
		}
	}

void TWsPointer::TimerExpired()
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

void TWsPointer::GetDoubleClickSettings(TTimeIntervalMicroSeconds32 &aTime, TInt &aDistance)
	{
	aTime=iDoubleClickMaxInterval;
	aDistance=iDoubleClickMaxDistance;
	}

void TWsPointer::SetDoubleClick(const TTimeIntervalMicroSeconds32 &aTime, TInt aDistance)
	{
	iDoubleClickMaxInterval=aTime;
	iDoubleClickMaxDistance=aDistance;
	}

void TWsPointer::PointerCursorUpdateCheck()
	{
	CWsPointerCursor* sprite=CalculatePointerCursor();
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
			{
			iUpdateRequired=ETrue;
			}
		}
	}

void TWsPointer::UpdatePointerCursor()
	{
//__PROFILE_START(3);
	CWsPointerCursor* sprite=iPointers[iPrimaryPointer].CalculatePointerCursor();
	UpdatePointerCursorTo(sprite);
//__PROFILE_END(3);
	}

void TWsPointer::UpdatePointerCursorTo(CWsPointerCursor* aNewCursor)
	{
	if (iCursorSprite!=aNewCursor)
		{
		if (iCursorSprite)
			{
			iCursorSprite->Deactivate();
			}
		iCursorSprite=aNewCursor;
		if (iCursorSprite)
			{
			iCursorSprite->SetPos(iPointers[iPrimaryPointer].iPos);
			iCursorSprite->Activate();
			}
		}
	else if (iCursorSprite)
		{
		iCursorSprite->SetPos(iPointers[iPrimaryPointer].iPos);
		}
	}

CWsPointerCursor* TWsPointer::CalculatePointerCursor()
	{
	CWsPointerCursor* sprite=NULL;
	if (iCurrentWindow && (iPointerCursorMode==EPointerCursorNormal || iPointerCursorMode==EPointerCursorWindow))
		{
		const CWsWindowBase* window=iCurrentWindow;
		do	{
			sprite=window->PointerCursor();
			if (window->WinType()!=EWinTypeClient)
				{
				break;
				}
			window=window->BaseParent();
			} while (!sprite);
		}
	if (!sprite && iCurrentWindow && (iPointerCursorMode==EPointerCursorFixed || iPointerCursorMode==EPointerCursorNormal))
		{
		sprite=CWsClient::DefaultSystemPointerCursor();
		}
	return sprite;
	}

/*
Callback function for event queue walk
*/
TEventQueueWalkRet PointerRepeatPurgeFunc(TAny* aReqPtrNum, TWsEvent* aQueueEvent)
	{
	return(TWsPointer::PointerRepeatPurgeCheck(aQueueEvent, reinterpret_cast<TUint>(aReqPtrNum)));
	}

TBool TWsPointer::PointerEventRepeatCheck(const TWsEvent* aEvent, TUint32 aHandle)
//
// Return ETrue if this pointer event is consumed by the pointer repeat
//
	{
	// Must be a pointer event type in order to get the pointer number,
	// which is needed to check the repeat window.
	WS_ASSERT_DEBUG(aEvent->Type()==EEventPointer,EWsPanicEventType);
	
	const TAdvancedPointerEvent* pntEvent=aEvent->Pointer();
	if (	TAdvancedPointerEventHelper::PointerNumber(*aEvent)==iNumber &&
			aHandle==iRepeatWindow->ClientHandle())
		{
		switch(pntEvent->iType)
			{
			case TPointerEvent::EDrag: // deliberate drop-through
			case TPointerEvent::EMove: // deliberate drop-through
			case TPointerEvent::EEnterCloseProximity: // deliberate drop-through
			case TPointerEvent::EExitCloseProximity: // deliberate drop-through
			case TPointerEvent::EEnterHighPressure: // deliberate drop-through
			case TPointerEvent::EExitHighPressure:
				{
				if(iRepeatRect.Contains(pntEvent->iPosition))
					{
					return(ETrue);
					}
				break;
				}
			default:
				// do nothing and drop through
				break;
			}		
		}
	return(EFalse);
	}

TEventQueueWalkRet TWsPointer::PointerRepeatPurgeCheck(TWsEvent* aQueueEvent, TUint8 aReqPtrNum)
	{
	// Return value is "WalkOK", unless a repeated event is found that needs to be deleted.
	TEventQueueWalkRet eventQueueWalkRet(EEventQueueWalkOk);	
	
	// Check the WSEvent Type	
	if (aQueueEvent->Type()==EEventPointer) // aEvent is a pointer event
		{		
		// It is a pointer event, so we can get the pointer number
		// to check if there is a repeat request for that pointer.
		const TInt eventPtrNum(TAdvancedPointerEventHelper::PointerNumber(*aQueueEvent));		
		// If aEvent's pointer has an active repeat request,
		// then it'll have a repeat window.
		if ((eventPtrNum == aReqPtrNum) && RepeatWindow(eventPtrNum))
			{
			// There is a repeat request for the pointer.
			// Is there a queued repeated event to be deleted?
			TWsPointer& wsPointer = iPointers[eventPtrNum];
			if (wsPointer.PointerEventRepeatCheck(aQueueEvent,aQueueEvent->Handle()))
				{
				// Update the return value to purge the event
				// as it is a move/drag within the repeat rect
				eventQueueWalkRet=EEventQueueWalkDeleteEvent;
				}		
			else
				{
				// No queued repeated event was found, therefore the
				// request is still pending and needs to be cancelled.
				wsPointer.CancelPointerRepeatEventRequest();
				}
			}
		}
	return eventQueueWalkRet;
	}

void TWsPointer::RequestRepeatEvent(CWsWindow* aWindow, const TWsWinCmdRequestPointerRepeatEvent& aRequest)
	{
	CancelPointerRepeatEventRequest();
	iRepeatWindow=aWindow;
	iRepeatRect=aRequest.rect;
	iRepeatTimer->After(aRequest.time);
	aWindow->EventQueue()->WalkEventQueue(&PointerRepeatPurgeFunc,reinterpret_cast<TInt*>(iNumber));
	if (iRepeatWindow && !iRepeatRect.Contains(iPos-iRepeatWindow->Origin()))
		{
		CancelPointerRepeatEventRequest();
		}	
	}

TInt TWsPointer::RequestPointerRepeatEvent(CWsWindow* aWindow, const TWsWinCmdRequestPointerRepeatEvent& aRequest)
	{
	TInt errNo = KErrNone;
	TUint8 pointerNum = aRequest.HasPointerNumber() ? aRequest.pointerNumber : iPrimaryPointer;
	if(PointerNumberInRange(pointerNum))
		{
		iPointers[pointerNum].RequestRepeatEvent(aWindow,aRequest);
		}
	else
		{
		errNo=KErrArgument;
		}
	return errNo;
	}

void TWsPointer::CancelPointerRepeatEventRequest()
	{
	if (iRepeatWindow)
		{
		iRepeatWindow=NULL;
		if(iRepeatTimer)
			iRepeatTimer->Cancel();
		}
	}

TInt TWsPointer::CancelPointerRepeatEventRequest(const TWsWinCmdCancelPointerRepeatEventRequest& aRequest)
	{
	TInt errNo = KErrNone;	
	TUint8 pointerNum = aRequest.HasPointerNumber() ? aRequest.pointerNumber : iPrimaryPointer;
	if(PointerNumberInRange(pointerNum))
		{
		iPointers[pointerNum].CancelPointerRepeatEventRequest();
		}
	else
		{
		errNo=KErrArgument;
		}
	return errNo;
	}

void TWsPointer::RepeatTimerCompleted()
	{
	TWsEvent event;
	event.SetType(EEventPointer);
	event.SetTimeNow();
	TPoint3D point3D(iPos-iRepeatWindow->Origin());
	point3D.iZ=iPressureProximity;
	TAdvancedPointerEventHelper::InitAdvancedPointerEvent(event,
														  TPointerEvent::EButtonRepeat,
														  TWindowServerEvent::GetModifierState(),
														  point3D,
														  (iPos-iRepeatWindow->BaseParent()->Origin()),
														  iNumber);
	QueuePointerEvent(iRepeatWindow, event);
	iRepeatWindow=NULL;
	}

#if defined(__WINS__)
void TWsPointer::SetXyInputType(TXYInputType aXyInputType)
	{
	if (iXyInputType>EXYInputPointer && aXyInputType<EXYInputMouse)
		{
		// change from Mouse types to Pointer/None
		for (TInt ii = 0; ii < iMaxPointers; ii++)
			{
			if (iPointers[ii].iState != EPointerStateDown)
				{
				iPointers[ii].iCurrentWindow=NULL;
				}
			}
		UpdatePointerCursor();
		}
	else if (iXyInputType<EXYInputMouse && aXyInputType>EXYInputPointer)
		{
		// change from Pointer/None types to Mouse types
		for (TInt ii = 0; ii < iMaxPointers; ii++)
			{
			if (iPointers[ii].iState != EPointerStateDown)
				{
				TPoint pos(iPointers[ii].iPos);
				TPoint parPos;
				iPointers[ii].ReLogCurrentWindow(pos,parPos,NULL);
				}
			}
		UpdatePointerCursor();
		}
	iXyInputType=aXyInputType;
	}
#endif

/**
Updates Primary Pointer before aRawEvent is processed. Only events related to Primary
Pointer will be sent to Clients which require single pointer environment.

This method implements single pointer environment emulation rules (see design
documentation for more details).

@param aRawEvent Incoming event used to update the Primary Pointer. It must be a pointer event,
                 as defined by TWsPointer::IsPointerEventType(TRawEvent::TType).
*/
void TWsPointer::UpdatePrimaryPointer(const TRawEvent& aRawEvent)
	{
	iPreviousPrimaryPointer = iPrimaryPointer;
	
	TRawEvent::TType type=aRawEvent.Type();
	TInt pointerNumber = aRawEvent.PointerNumber();
    
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: TWsPointer::UpdatePrimaryPointer Current Primary pointer = %d",iPrimaryPointer);
	for(TInt i=0; i< iMaxPointers; i++)
		{
		RDebug::Printf("_WSEVENT_POINTER: TWsPointer::UpdatePrimaryPointer Pointer Number= %d  State = %x XY(%d,%d)",iPointers[i].iNumber,iPointers[i].iState,iPointers[i].iPos.iX,iPointers[i].iPos.iY);
		}
#endif
	
	// If primary pointer is out of range, then the first pointer that will 
	// start being detected (come back in range) will become primary.
	if (iPointers[iPrimaryPointer].iState == EPointerStateOutOfRange)
		{
		if (type != TRawEvent::EPointer3DOutOfRange && iPointers[pointerNumber].iState == EPointerStateOutOfRange)
			{
			iPrimaryPointer = pointerNumber;
#ifdef LOG_WSERV_EVENTS
	        RDebug::Printf("_WSEVENT_POINTER: TWsPointer::UpdatePrimaryPointer New Primary pointer(OutRange) = %d",iPrimaryPointer);
#endif
			}
		return;
		}
	
	// if non-primary pointer sends EButton1Down event, and actual primary pointer
	// is not down, then the pointer which has sent EButton1Down becomes primary.
	if (type == TRawEvent::EButton1Down && 
		iPointers[iPrimaryPointer].iState != EPointerStateDown)
		{
		iPrimaryPointer = pointerNumber;
#ifdef LOG_WSERV_EVENTS
        RDebug::Printf("_WSEVENT_POINTER: TWsPointer::UpdatePrimaryPointer New Primary pointer(ButtonDown) = %d",iPrimaryPointer);
#endif
		return;
		}
	}

/** Sets Z coordinate threshold values for TPointerEvent::EEnterCloseProximity 
and TPointerEvent::EExitCloseProximity events.
@return KErrNone if successful, 
        KErrNotSupported if the device doesn't support threshold values,
        KErrArgument if aEnterCloseProximityThreshold is less than aExitCloseProximityThreshold 
@see RWsSession::SetCloseProximityThresholds which calls this method
*/
TInt TWsPointer::SetCloseProximityThresholds(TInt aEnterCloseProximityThreshold, TInt aExitCloseProximityThreshold)
	{
	if (aEnterCloseProximityThreshold < aExitCloseProximityThreshold)
		{
		return KErrArgument;
		}
	
	TInt ret = HAL::Set(HALData::EPointer3DEnterCloseProximityThreshold, aEnterCloseProximityThreshold);
	if (ret != KErrNone)
		{
		return ret;
		}
	
	ret = HAL::Set(HALData::EPointer3DExitCloseProximityThreshold, aExitCloseProximityThreshold);
	WS_ASSERT_DEBUG(ret == KErrNone, EWsPanicProxThresholdsInconsist);
	if (ret != KErrNone)
		{
		HAL::Set(HALData::EPointer3DEnterCloseProximityThreshold, iEnterCloseProximityThreshold);
		return ret;
		}
	
	iEnterCloseProximityThreshold = aEnterCloseProximityThreshold;
	iExitCloseProximityThreshold  = aExitCloseProximityThreshold; 
	return KErrNone;
	}

/**
@return Z coordinate threshold value for TPointerEvent::EEnterCloseProximity events
@see RWsSession::GetEnterCloseProximityThreshold which calls this method
*/
TInt TWsPointer::GetEnterCloseProximityThreshold()
	{
	return iEnterCloseProximityThreshold;
	}

/**
@return Z coordinate threshold value for TPointerEvent::EExitCloseProximity events
@see RWsSession::GetExitCloseProximityThreshold which calls this method
*/
TInt TWsPointer::GetExitCloseProximityThreshold()
	{
	return iExitCloseProximityThreshold;
	}

/** Sets Z coordinate threshold value for TPointerEvent::EEnterHighPressure and 
TPointerEvent::EExitHighPressure events.
@return KErrNone if successful, 
        KErrNotSupported if the device doesn't support threshold values,
        KErrArgument if aEnterHighPressureThreshold is less than aExitHighPressureThreshold 
@see RWsSession::SetHighPressureThresholds which calls this method
*/
TInt TWsPointer::SetHighPressureThresholds(TInt aEnterHighPressureThreshold, TInt aExitHighPressureThreshold)
	{
	if (aEnterHighPressureThreshold < aExitHighPressureThreshold)
		{
		return KErrArgument;
		}
	
	TInt ret = HAL::Set(HALData::EPointer3DEnterHighPressureThreshold, aEnterHighPressureThreshold);
	if (ret != KErrNone)
		{
		return ret;
		}
	
	ret = HAL::Set(HALData::EPointer3DExitHighPressureThreshold, aExitHighPressureThreshold);
	WS_ASSERT_DEBUG(ret == KErrNone, EWsPanicPressThresholdsInconsist);
	if (ret != KErrNone)
		{
		HAL::Set(HALData::EPointer3DEnterHighPressureThreshold, iEnterHighPressureThreshold);
		return ret;
		}
	
	iEnterHighPressureThreshold = aEnterHighPressureThreshold;
	iExitHighPressureThreshold  = aExitHighPressureThreshold;
	return KErrNone;
	}

/**
@return Z coordinate threshold value for TPointerEvent::EEnterHighPressure events
@see RWsSession::GetEnterHighPressureThreshold which calls this method
*/
TInt TWsPointer::GetEnterHighPressureThreshold()
	{
	return iEnterHighPressureThreshold;
	}

/**
@return Z coordinate threshold value for TPointerEvent::EExitHighPressure events
@see RWsSession::GetExitHighPressureThreshold which calls this method
*/
TInt TWsPointer::GetExitHighPressureThreshold()
	{
	return iExitHighPressureThreshold;
	}


/**
This function is specific for capacitive touch screens, where user's finger is the pointer device. 
Usability studies have shown that the user's perception of the location of the pointer hit is always 
away from few pixels north of the actual hit centre as detected by the digitizer device. So, this function
will shift all pointer events by a specified Y displacement.

@param aY Current y coordinate pointer position.
 
 */
void TWsPointer::ShiftYCoordinate(TInt& aY)
    {
    WS_ASSERT_DEBUG(iYOffset>=0, EWsPanicInvalidPointerOffset);
    if (aY >= iYOffset)
        {
        aY -=iYOffset;
        }
    else
        {
        aY=0;
        }
    }
//
CWsPointerTimer::CWsPointerTimer(MPointerTimerCallback& aPointerTimerCallback)
: CTimer(EPointerRepeatPriority), iPointerTimerCallback(aPointerTimerCallback)
	{}

void CWsPointerTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

CWsPointerTimer* CWsPointerTimer::NewL(MPointerTimerCallback& aWsPointer)
	{
	CWsPointerTimer* self = new(ELeave) CWsPointerTimer(aWsPointer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CWsPointerTimer::RunL()
	{
	User::ResetInactivityTime();
	WS_ASSERT_DEBUG(iStatus.Int()==KErrNone, EWsPanicPointerRepeatTimerStatus);
	iPointerTimerCallback.RepeatTimerCompleted();
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

void CWsPointerBuffer::ConnectL(CWsClientWindow* aWindow, TInt aMaxPoints, TUint aFlags)
	{
	CWsPointerBuffer* pb=NULL;
	for(TSglQueIter<CWsPointerBuffer> iter(iList);(pb=iter++)!=NULL;)
		{
		if (pb->iWindow==aWindow)
			{
			User::Leave(KErrInUse);
			}
		}
	CWsPointerBuffer* pbuf=new(ELeave) CWsPointerBuffer;
	pbuf->iWindow=aWindow;
	pbuf->iMaxPoints=aMaxPoints;
	pbuf->iFlags=aFlags;
	iList.AddFirst(*pbuf);
	CleanupStack::PushL(pbuf);
	AdjustMaxSizeL();
	CleanupStack::Pop();
	}

void CWsPointerBuffer::Disconnect(CWsClientWindow* aWindow)
	{
	CWsPointerBuffer* pb=NULL;
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
		{
		if (iCurrentBuffer && iCurrentBuffer->iWindow->QueueEvent(EEventPointerBufferReady))
			{
			iSignalled=ETrue;
			}
		}
	}

void CWsPointerBuffer::PointerEvent(CWsClientWindow* aWindow,const TPoint &aPoint)
	{
	if (iCurrentBuffer==NULL || aWindow!=iCurrentBuffer->iWindow)
		{
		Reset();
		CWsPointerBuffer* pb=NULL;
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

void CWsPointerBuffer::RetrievePointerMoveBuffer(CWsClientWindow* aWindow,TInt aMaxPoints)
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
			{
			CWsClient::ReplyBuf(pnts);
			}
		if (iPointerBuffer->Count())
			{
			SignalBufferReady();
			}
		}
	}

void CWsPointerBuffer::DiscardPointerMoveBuffer(TUint aHandle)
	{
	if (iCurrentBuffer && aHandle==iCurrentBuffer->iWindow->ClientHandle())
		{
		Reset();
		}
	}

void CWsPointerBuffer::DiscardPointerMoveBuffer(CWsClientWindow* aWindow)
	{
	if (iCurrentBuffer && aWindow==iCurrentBuffer->iWindow)
		{
		Reset();
		}
	}

void CWsPointerBuffer::AdjustMaxSizeL()
	{
	TInt max=0;
	CWsPointerBuffer* pb=NULL;
	for(TSglQueIter<CWsPointerBuffer> iter(iList);(pb=iter++)!=NULL;)
		{
		if (pb->iMaxPoints>max)
			{
			max=pb->iMaxPoints;
			}
		}
	if (max==0)
		{
		delete iPointerBuffer;
		iPointerBuffer=NULL;
		}
	else if (!iPointerBuffer)
		{
		CCirBuf<TPoint>* pointerBuffer=new(ELeave) CCirBuf<TPoint>;
		CleanupStack::PushL(pointerBuffer);
		pointerBuffer->SetLengthL(max);
		CleanupStack::Pop();
		iPointerBuffer=pointerBuffer;
		}
	else
		{
		iPointerBuffer->SetLengthL(max);
		}
	}
