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
// Top level window server code
// 
//

#include "EVENT.H"

#include <hal.h>
#include <w32adll.h>
#include <w32click.h>
#include "server.h"
#include "windowgroup.h"
#include "KEYCLICK.H"
#include "wstop.h"
#include "panics.h"
#include "screen.h"
#include "inifile.h"
#include "password.h"
#include <w32std.h>
#include "pointer.h"
#include "debugbar.h"
#include "wstraces.h"
#include "Graphics/wsgraphicdrawerinternal.h"


GLREF_D CDebugLogBase *wsDebugLog;

GLREF_C void StateDump();
GLREF_C void HeapDump();

#define IMPOSSIBLE 0xFFFFFFFF

const TWsWinCmdCaptureKey ImpossibleKeyPress=
	{
	IMPOSSIBLE,	// Impossible to hit key combination, used for disabling Hot Keys
	IMPOSSIBLE,
	IMPOSSIBLE};

const TWsWinCmdCaptureKey DefaultHotKeys[TWindowServerEvent::ENumHotKeys]={
	{	// Enable logging
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	5}, // E
	{	// Disable logging
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	4}, // D
	{	// Window server internal dump to log
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	23},// W
	{	// The key of death
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	11},// K
	{	// Shutdown window server
#if defined(_DEBUG)
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	24},// X
#else
	IMPOSSIBLE,	// Impossible to hit key combination, effectively disables shutdown key in release builds
	IMPOSSIBLE,
	IMPOSSIBLE},
#endif
	{	// Heap dump
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	8}, // H
	{	// Inc Contrast
	0,
	0,
	EKeyIncContrast},
	{	// Dec Contrast
	0,
	0,
	EKeyDecContrast},
	{	// Off
	0,
	0,
	EKeyOff},
	{	// Backlight on 
	0,
	0,
	EKeyBacklightOn},
	{	// Backlight off
	0,
	0,
	EKeyBacklightOff},
	{	// Backlight toggle
	0,
	0,
	EKeyBacklightToggle},
	{	// Screen Dimension Change
	0,
	0,
	EKeyScreenDimension0},
	{
	0,
	0,
	EKeyScreenDimension1},
	{
	0,
	0,
	EKeyScreenDimension2},
	{
	0,
	0,
	EKeyScreenDimension3},
#if defined(_DEBUG)
	{	// Display mode cycle
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	21},// U
	{	// Orientation cycle
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	15},// O
#else
	{	// Display mode cycle
	IMPOSSIBLE,	// Impossible to hit key combination
	IMPOSSIBLE,
	IMPOSSIBLE},
	{	// Orientation cycle
	IMPOSSIBLE,	// Impossible to hit key combination
	IMPOSSIBLE,
	IMPOSSIBLE},
#endif
	{	// Inc Brightness
	0,
	0,
	EKeyIncBrightness},
	{	// Dec Brightness
	0,
	0,
	EKeyDecBrightness},
	{	// Cycle focus screen
	EModifierFunc|EModifierCtrl|EModifierShift,
	EModifierFunc|EModifierCtrl|EModifierShift,
	9}, // I
	};

CKeyTranslator *TWindowServerEvent::iKeyTranslator=NULL;
TEventRequestQueue TWindowServerEvent::iSwitchOnQueue;
TEventRequestQueue TWindowServerEvent::iErrorMessageQueue;
TEventRequestQueue TWindowServerEvent::iModifierChangedQueue;
TEventRequestQueue TWindowServerEvent::iGroupChangedQueue;
TEventRequestQueue TWindowServerEvent::iFocusChangedQueue;
TEventRequestQueue TWindowServerEvent::iGroupListChangedQueue;
TEventRequestQueue TWindowServerEvent::iScreenDeviceChangedQueue;
TTime TWindowServerEvent::iPrevOomMessageTime;
CCaptureKeys *TWindowServerEvent::iCaptureKeys;
CWsHotKey *TWindowServerEvent::iHotKeys;
TInt TWindowServerEvent::iModifierState;
CRawEventReceiver *TWindowServerEvent::iEventReceiver;
CArrayPtrFlat<MEventHandler> *TWindowServerEvent::iEventHandlers;
CArrayFixFlat<SNotificationHandler> *TWindowServerEvent::iNotificationHandlers;
TInt TWindowServerEvent::iPotentialEventHandlers=0;
TUint32 TWindowServerEvent::iBinaryFlags=0x00;
RArray<TDrawerHandler>* TWindowServerEvent::iDrawerHandlers;
RArray<TWsEventHandler> TWindowServerEvent::iWsEventHandlers;
TInt TWindowServerEvent::iEventHandlerCount=0;
TRepeatKey CKeyboardRepeat::iCurrentRepeat;
TRepeatKey CKeyboardRepeat::iAlternateRepeat;
TInt CKeyboardRepeat::iRepeatRollover=1;
CKeyboardRepeat::TRepeatType CKeyboardRepeat::iRepeating=ERepeatNone;
CKeyboardRepeat *CKeyboardRepeat::iThis=NULL;
TTimeIntervalMicroSeconds32 CKeyboardRepeat::iInitialTime;
TTimeIntervalMicroSeconds32 CKeyboardRepeat::iTime;
CWsWindowGroup *CKeyboardRepeat::iFocus=NULL;
TBool CKeyboardRepeat::iAlternateRepeatExists=EFalse;
CWsCaptureLongKey* CKeyboardRepeat::iLongCapture=NULL;


void TWindowServerEvent::DeleteHotKeys()
	{
	CWsHotKey *hotKey=iHotKeys;
	while(hotKey)
		{
		CWsHotKey *next=hotKey->iNext;
		delete hotKey;
		hotKey=next;
		}
	iHotKeys=NULL;
	}

void TWindowServerEvent::DeleteStatics()
	{
	DeleteHotKeys();
	delete iCaptureKeys;
	CKeyboardRepeat::Destroy();
	delete iKeyTranslator;
	delete iEventReceiver;
	delete iEventHandlers;
	delete iNotificationHandlers;
	iDrawerHandlers->Close();
	delete iDrawerHandlers;
	}

void TWindowServerEvent::InitStaticsL()
//
// Create the CEvent active object.
//
	{
#if defined(__WINS__)
	WS_ASSERT_DEBUG(TWindowServerEvent::ENumHotKeys==EHotKeyLastKeyType+1, EWsPanicUnknownCaptureKey);
#endif
	iEventReceiver=new(ELeave) CRawEventReceiver(EEventPriority);
	iEventReceiver->ConstructL();
	iKeyTranslator=CKeyTranslator::New();
	User::LeaveIfNull(iKeyTranslator);

//  Change keyboard mapping according to information the HAL
	TInt keyboardIndex;
	if (HAL::Get(HALData::EKeyboardIndex,keyboardIndex)==KErrNone)
		{
		_LIT(KLitKeyDataDllName,"EKDATA.%02d");
		TBuf<16> keyDataDllName;
		keyDataDllName.Format(KLitKeyDataDllName,keyboardIndex);
		iKeyTranslator->ChangeKeyData(keyDataDllName);
		}

	iCaptureKeys=new(ELeave) CCaptureKeys;
	iCaptureKeys->Construct();
	for (TInt index=0;index<TWindowServerEvent::ENumHotKeys;index++)
		ConstructDefaultHotKeyL(index,DefaultHotKeys[index]);
	CKeyboardRepeat::NewL();
	CKeyboardRepeat::SetRepeatTime(EDefaultInitialRepeatTime, EDefaultRepeatTime);
	iEventHandlers=new(ELeave) CArrayPtrFlat<MEventHandler>(2);
	iNotificationHandlers=new(ELeave) CArrayFixFlat<SNotificationHandler>(2);
	iDrawerHandlers = new(ELeave) RArray<TDrawerHandler>(4);
	}

void TWindowServerEvent::LinkHotKey(CWsHotKey *aWsHotKey)
	{
	aWsHotKey->SetLink(iHotKeys);
	iHotKeys=aWsHotKey;
	}

void TWindowServerEvent::ConstructDefaultHotKeyL(TInt aHotKey, const TWsWinCmdCaptureKey &aSystemKey)
	{
	CWsHotKey* hotKey=new(ELeave) CWsHotKey(aHotKey, ETrue);
	// hotKey is pushed onto the cleanup stack in method ConstructLD.
	hotKey->ConstructLD(aSystemKey);
	LinkHotKey(hotKey);
	}

CWsHotKey* TWindowServerEvent::ClearHotKeysL(TInt aHotKey)
	{
	if (aHotKey>ENumHotKeys)
		{
		User::Leave(KErrArgument);
		}
	CWsHotKey** pHotKey= &iHotKeys;
	CWsHotKey* defaultHotKey=NULL;
	while(*pHotKey)
		{
		TBool unlinked=EFalse;
		if ((*pHotKey)->HotKeyType()==aHotKey)
			{
			CWsHotKey *free=*pHotKey;
			if (free->IsDefault())
				{
				free->SetL(ImpossibleKeyPress);
				defaultHotKey=free;
				}
			else
				{
				*pHotKey=(*pHotKey)->iNext;
				delete free;
				unlinked=ETrue;
				}
			}
		if (!unlinked)
			{
			pHotKey=&(*pHotKey)->iNext;
			}
		}
	return(defaultHotKey);
	}

void TWindowServerEvent::ResetDefaultHotKeyL(TInt aHotKey)
	{
	if ((aHotKey<0) || (aHotKey>=ENumHotKeys))
		{
		User::Leave(KErrArgument);
		}
	CWsHotKey* defaultHotKey=ClearHotKeysL(aHotKey);
	WS_ASSERT_DEBUG(defaultHotKey, EWsPanicDefaultHotKeyNotFound);
	defaultHotKey->SetL(DefaultHotKeys[aHotKey]);
	}

void TWindowServerEvent::SetHotKeyL(const TWsClCmdSetHotKey &aHotKey)
	{
	if (aHotKey.type>ENumHotKeys)
		User::Leave(KErrArgument);
//
	CWsHotKey *hotKey=new(ELeave) CWsHotKey(aHotKey.type, EFalse);
//
	TWsWinCmdCaptureKey captureKey;
	captureKey.modifiers=aHotKey.modifiers;
	captureKey.modifierMask=aHotKey.modifierMask;
	captureKey.key=aHotKey.keycode;
	hotKey->ConstructLD(captureKey);
//
	LinkHotKey(hotKey);
	}

void TWindowServerEvent::AddEventHandler(MEventHandler *aEventHandler)
	{
#if defined(_DEBUG)
	TRAPD(err,iEventHandlers->AppendL(aEventHandler));
	WS_ASSERT_DEBUG(err==KErrNone, EWsPanicEventHandlerInconsistency);
#else
	iEventHandlers->AppendL(aEventHandler);		//Shouldn't leave
#endif
	}

void TWindowServerEvent::RemoveEventHandler(const MEventHandler *aEventHandler)
	{
	TInt count=iEventHandlers->Count();
	TInt ii;
	for(ii=0;ii<count;++ii)
		{
		if ((*iEventHandlers)[ii]==aEventHandler)
			{
			if (iEventHandlerCount>0)  
				{
				iBinaryFlags |= ERemovedEventHandlerWhileProcessingRawEvents;
				(*iEventHandlers)[ii]=NULL; // replace the Handler with null to keep size of the array
				}
			else 
				{
				iEventHandlers->Delete(ii);
				}
			return;
			}
		}
	}

void TWindowServerEvent::PotentialEventHandlerL(TInt aNum)
	{
	iPotentialEventHandlers+=aNum;
	WS_ASSERT_DEBUG(iPotentialEventHandlers>=iEventHandlers->Count(), EWsPanicEventHandlerInconsistency);
	TRAPD(err,iEventHandlers->SetReserveL(iPotentialEventHandlers));
	if (err!=KErrNone)
		{
		if (aNum>0)
			User::Leave(err);
		}
	else if (iPotentialEventHandlers==0)
		iEventHandlers->Compress();
	}

void SendSwitchOnEvent(TEventRequestItem *aQptr, TInt aEvent, TInt )
	{
	aQptr->iWindow->QueueEvent(aEvent);
	}

/*void SendSwitchOffEvent(TEventRequestItem *aQptr, TInt , TInt )
	{
	aQptr->iWindow->QueueEvent(EEventSwitchOff);
	}*/

void SendErrorMessage(TEventRequestItem *aQptr, TInt aCategory, TInt aError)
	{
	TWsEvent event;
	event.SetType(EEventErrorMessage);
	event.SetHandle(aQptr->iWindow->ClientHandle());
	event.ErrorMessage()->iErrorCategory=(TWsErrorMessage::TErrorCategory)aCategory;
	event.ErrorMessage()->iError=aError;
	event.SetTimeNow();
	aQptr->iWindow->EventQueue()->QueueEvent(event,EEventPriorityHigh);
	}

void SendModifierChangedEvent(TEventRequestItem *aQptr, TInt aChanged, TInt )
	{
	TInt tmpChanged=aChanged&aQptr->iParam;
	if (tmpChanged)
		{
		TWsEvent event;
		event.SetType(EEventModifiersChanged);
		event.SetHandle(aQptr->iWindow->ClientHandle());
		event.ModifiersChanged()->iChangedModifiers=tmpChanged;
		event.ModifiersChanged()->iModifiers=TWindowServerEvent::GetStoredModifierState();
		event.SetTimeNow();
		aQptr->iWindow->EventQueue()->QueueEvent(event,EEventPriorityHigh);
		}
	}

void TWindowServerEvent::ProcessEventQueue(TEventRequestQueue &aQueue, TSendEventFunc aFunc, TInt aParam1, TInt aParam2)
	{
	TSglQueIter<TEventRequestItem> iter(aQueue.Queue());
	TEventRequestItem *qPtr;
	CWsWindowGroup *focusWin=CWsTop::FocusWindowGroup();
	while((qPtr=iter++)!=NULL)
		{
		if (qPtr->iCircumstances==EEventControlAlways || 
			(qPtr->iCircumstances==EEventControlOnlyWithKeyboardFocus && qPtr->iWindow->WinGroup()==focusWin) ||
			(qPtr->iCircumstances==EEventControlOnlyWhenVisible && !qPtr->iWindow->TreeIsObscured()))
			aFunc(qPtr, aParam1, aParam2);
		}
	}

void TWindowServerEvent::NotifyOom()
	{
	TTime now;
	now.UniversalTime();
	TTimeIntervalSeconds interval;
	TInt err=now.SecondsFrom(iPrevOomMessageTime,interval);
	if (err!=KErrNone || interval.Int()<0 || interval.Int()>EOomEventSecondGap)
		{
		ProcessErrorMessages(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
		iPrevOomMessageTime=now;
		}
	}

TBool TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::TErrorCategory aCategory, TInt aError)
	{
	if (aError!=KErrNone)
		{
		ProcessEventQueue(iErrorMessageQueue, SendErrorMessage, aCategory, aError);
		return ETrue;
		}
	return EFalse;
	}

void TWindowServerEvent::ProcessModifierChanges()
	{
	TInt newState=iKeyTranslator->GetModifierState();
	if (newState!=iModifierState)
		{
		TInt changed=iModifierState^newState;
		iModifierState=newState;
		ProcessEventQueue(iModifierChangedQueue, SendModifierChangedEvent, changed, 0);
		}
	}

TEventQueueWalkRet FindScreenDeviceChangedEvent(TAny *aHandlePtr, TWsEvent *aEvent)
	{
	if (aEvent->Type()==EEventScreenDeviceChanged && aEvent->Handle()==*(TUint *)aHandlePtr)
		*(TUint *)aHandlePtr=0;	// Indicates event found
	return(EEventQueueWalkOk);
	}
	
void TWindowServerEvent::SendScreenDeviceChangedEvents(CScreen* aScreen)
	{
	TSglQueIter<TEventRequestItem> iter(iScreenDeviceChangedQueue .Queue());
	TEventRequestItem *qPtr;
	while((qPtr=iter++)!=NULL)
		SendScreenDeviceChangedEvent(qPtr->iWindow);
	if(CClick::IsHandler())
		{
		TClickMakerData clickMakerData;
		clickMakerData.screenDeviceMode=aScreen->ScreenSizeMode();
		CClick::OtherEvent(EEventScreenDeviceChanged, &clickMakerData);
		}
	TWsEvent wsEvent;
	wsEvent.SetType(EEventScreenDeviceChanged);
	TWindowServerEvent::PublishNotification(wsEvent);
	TWservCrEvent crEvent(TWservCrEvent::EScreenSizeModeChanged,aScreen->ScreenSizeMode());
	TWindowServerEvent::NotifyDrawer(crEvent);
	}

void TWindowServerEvent::SendScreenDeviceChangedEvent(const CWsWindowBase *aWindow)
	{
	CEventQueue *queue=aWindow->EventQueue();
	TUint32 handle=aWindow->ClientHandle();
	queue->WalkEventQueue(&FindScreenDeviceChangedEvent,&handle);
	if (handle!=NULL)	// Indicates event not found
		queue->QueueEvent(handle, EEventScreenDeviceChanged);
	}

TEventQueueWalkRet FindGroupChangedEvent(TAny *aHandlePtr, TWsEvent *aEvent)
	{
	if (aEvent->Type()==EEventWindowGroupsChanged && aEvent->Handle()==*(TUint *)aHandlePtr)
		{
		*(TUint *)aHandlePtr=0;	// Indicates event found
		}
	return(EEventQueueWalkOk);
	}
	
void TWindowServerEvent::SendGroupChangedEvents()
	{
	TSglQueIter<TEventRequestItem> iter(iGroupChangedQueue.Queue());
	TEventRequestItem *qPtr;
	while((qPtr=iter++)!=NULL)
		{
		const CWsWindowBase *win=qPtr->iWindow;
		CEventQueue *queue=win->EventQueue();
		TUint32 handle=win->ClientHandle();
		queue->WalkEventQueue(&FindGroupChangedEvent,&handle);
		if (handle!=NULL)	// Indicates event not found
			{
			queue->QueueEvent(handle, EEventWindowGroupsChanged);
			}
		}
	}	

TEventQueueWalkRet FindFocusChangedEvent(TAny *aHandlePtr, TWsEvent *aEvent)
	{
	if (aEvent->Type()==EEventFocusGroupChanged && aEvent->Handle()==*(TUint *)aHandlePtr)
		{
		*(TUint *)aHandlePtr=0;	// Indicates event found
		}
	return(EEventQueueWalkOk);
	}
	
void TWindowServerEvent::SendFocusChangedEvents()
	{
	TInt identifier=0;	// Zero Identifier indicates, currently there is no focused window group
	CScreen* currentFocusScreen=CWsTop::CurrentFocusScreen();	
	TInt screenNumber=currentFocusScreen->ScreenNumber();
	CWsWindowGroup* currentFocusWG=currentFocusScreen->FocusWindowGroup();
	WS_ASSERT_DEBUG(currentFocusWG, EWsPanicNoScreen);
	if(currentFocusWG)
		{
		identifier=currentFocusWG->Identifier();
		}
	TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EWindowGroupChanged,
		screenNumber, reinterpret_cast<TAny*>(identifier)));
		 
	TSglQueIter<TEventRequestItem> iter(iFocusChangedQueue.Queue());
	TEventRequestItem *qPtr;
	while((qPtr=iter++)!=NULL)
		{
		const CWsWindowBase *win=qPtr->iWindow;
		CEventQueue *queue=win->EventQueue();
		TUint32 handle=win->ClientHandle();
		queue->WalkEventQueue(&FindFocusChangedEvent,&handle);
		if (handle!=NULL)	// Indicates event not found
			{
			queue->QueueEvent(handle, EEventFocusGroupChanged);
			}
		}
	}

TEventQueueWalkRet FindGroupListChangedEvent(TAny *aHandlePtr, TWsEvent *aEvent)
	{
	if (aEvent->Type()==EEventWindowGroupListChanged && aEvent->Handle()==*(TUint *)aHandlePtr)
		{
		*(TUint *)aHandlePtr=0;	// Indicates event found
		}
	return(EEventQueueWalkOk);
	}
	
void TWindowServerEvent::SendGroupListChangedEvents()
	{
	TSglQueIter<TEventRequestItem> iter(iGroupListChangedQueue.Queue());
	TEventRequestItem *qPtr;
	while((qPtr=iter++)!=NULL)
		{
		const CWsWindowBase *win=qPtr->iWindow;
		CEventQueue *queue=win->EventQueue();
		TUint32 handle=win->ClientHandle();
		queue->WalkEventQueue(&FindGroupListChangedEvent,&handle);
		if (handle!=NULL)	// Indicates event not found
			{
			queue->QueueEvent(handle, EEventWindowGroupListChanged);
			}
		}
	}	

TEventQueueWalkRet OverrideVisibilityChangedEvent(TAny *aNewEvent, TWsEvent *aOldEvent)
	{
	// This replaces the first visibility event it finds for the given window with the
	// one given.  This is fine, so long as the meaning of all visibility events remains
	// independent of the ones before.
	TWsEvent* newEvent = static_cast<TWsEvent*>(aNewEvent);
	if (aOldEvent->Type()==EEventWindowVisibilityChanged && aOldEvent->Handle()==newEvent->Handle())
		{
		aOldEvent->SetTimeNow();
		aOldEvent->VisibilityChanged()->iFlags = newEvent->VisibilityChanged()->iFlags;
		newEvent->SetHandle(NULL);
		}
	return EEventQueueWalkOk;
	}

void TWindowServerEvent::SendVisibilityChangedEvents(CWsWindowBase* aWin, TUint aFlags)
	{
	CEventQueue *queue=aWin->EventQueue();
	TWsEvent event;
	event.SetType(EEventWindowVisibilityChanged);
	event.SetHandle(aWin->ClientHandle());
	event.SetTimeNow();
	TWsVisibilityChangedEvent* visevent = event.VisibilityChanged();
	visevent->iFlags = aFlags;
	queue->WalkEventQueue(&OverrideVisibilityChangedEvent,&event);
	if (event.Handle()!=NULL)
		{
		queue->QueueEvent(event);
		}
	}

void TWindowServerEvent::QueueKeyEvent(CWsWindowGroup *aWin, TWsEvent &aEvent, TWservEventPriorities aPriority)
	{
	aEvent.SetTimeNow();
	aWin->EventQueue()->QueueEvent(aEvent, aPriority);
	}

void TWindowServerEvent::QueueKeyPress(const TKeyData& aKey, TInt aScanCode, CWsWindowGroup* aRepeatFocus, TBool aCheckRepeat,TInt aRepeats)
 	{
	CWsWindowGroup* focusWin=CWsTop::FocusWindowGroup();
	TWsEvent event;
	TKeyEvent& keyEvent=*event.Key();
	keyEvent.iCode=aKey.iKeyCode;
	keyEvent.iScanCode=aScanCode;
	keyEvent.iModifiers=aKey.iModifiers;
	keyEvent.iRepeats=aRepeats;
	if (!aRepeatFocus && CClick::IsHandler())
		CClick::KeyEvent(EEventKey,keyEvent);
	CWsCaptureLongKey* longCapture=NULL;
	if (aCheckRepeat)
		longCapture=CWsCaptureLongKey::CheckForCapture(aKey.iKeyCode, aKey.iModifiers);
	if (aKey.iIsCaptureKey)
		{
		if (aKey.iApp==NULL)	// Captured by Wserv itself
			{
			_LIT(KWSERVDebugLogCapturedKey,"WSERV Captured Key");
			CScreen* focusScreen=CWsTop::CurrentFocusScreen();
			TInt screenNo=focusScreen->ScreenNumber();
			
			if (wsDebugLog)
				wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogCapturedKey);
			CWsHotKey *hotKey=iHotKeys;
			while(hotKey)
				{
				if (hotKey->KeyHandle()==aKey.iHandle)
					{
					switch(hotKey->HotKeyType())
						{
						case EHotKeyEnableLogging:
							CWsTop::EnableLogging();
							break;
						case EHotKeyDisableLogging:
							CWsTop::DisableLogging();
							break;
						case EHotKeyStateDump:
							StateDump();
							break;
						case EHotKeyHeapDump:
							HeapDump();
							break;
						case EHotKeyOfDeath:
							if (!CWsPassword::PasswordModeActive())
								{
								const TBool currentJustInTimeValue=User::JustInTime();
								if (currentJustInTimeValue)
									{
									User::SetJustInTime(EFalse);
									}
								CWsTop::KillForegroundSession();
								if (currentJustInTimeValue)
									{
									User::SetJustInTime(ETrue);
									}
								}
							break;
						case EHotKeyShutDown:
							CWsTop::Exit();
							break;
						case EHotKeyIncContrast:
							focusScreen->IncContrast();
							break;
						case EHotKeyDecContrast:
							focusScreen->DecContrast();
							break;
						case EHotKeyOff:
							CWsTop::HandleSwitchOff(EEventKeySwitchOff,ETrue);
							break;
						case EHotKeyBacklightToggle:
							{
							TInt state;
							if (!ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Get(screenNo,HALData::EBacklightState,state)))
								ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Set(screenNo,HALData::EBacklightState,!state));
							}
							break;
						case EHotKeyBacklightOn:
							ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Set(screenNo,HALData::EBacklightState,ETrue));
							break;
						case EHotKeyBacklightOff:
							ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Set(screenNo,HALData::EBacklightState,EFalse));
							break;
						case EHotKeyScreenDimension0:
						case EHotKeyScreenDimension1:
						case EHotKeyScreenDimension2:
						case EHotKeyScreenDimension3:
							focusScreen->doSetScreenMode(hotKey->HotKeyType()-EHotKeyScreenDimension0);
							break;
						case EHotKeyCycleDisplaySize:
							focusScreen->CycleDisplaySize();
							break;
						case EHotKeyCycleOrientation:
							focusScreen->CycleOrientation();
							break;
						case EHotKeyIncBrightness:
							focusScreen->IncBrightness();
							break;
						case EHotKeyDecBrightness:
							focusScreen->DecBrightness();
							break;
						case EHotKeyCycleFocusScreen:
							CWsTop::SetCurrentFocusScreen((CWsTop::CurrentFocusScreen()->ScreenNumber()+1)%CWsTop::NumberOfScreens());
							break;
						}
					return;
					}
				hotKey=hotKey->iNext;
				}
			WS_PANIC_ALWAYS(EWsPanicUnknownCaptureKey);
			return;
			}
		focusWin=((CWsWindowGroup *)aKey.iApp);
		_LIT(KWSERVDebugLogKeyCapturedByApp,"Key captured by app %d");
		if (wsDebugLog)
			wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogKeyCapturedByApp,focusWin->Identifier());
		if (CWsPassword::PasswordModeActive() && focusWin!=CWsPassword::PasswordWindow()->WinGroup())
			return;
		}
	if (aRepeatFocus && aRepeatFocus!=focusWin)
		CKeyboardRepeat::CancelRepeat(NULL);		// Repeat is going to different window so cancel it and don't deliver this key
	else if (focusWin!=NULL && focusWin->CheckForPriorityKey(aKey,aScanCode)==EFalse)
		{
		if (longCapture || (aCheckRepeat && !aRepeatFocus && aKey.iModifiers&EModifierAutorepeatable))
			{
			if (CKeyboardRepeat::StartRepeat(aKey,aScanCode,focusWin,longCapture))
				return;
			}
		event.SetType(EEventKey);
		event.SetHandle(focusWin->ClientHandle());
		if (aRepeats!=0)
			{
			CEventQueue* queue=focusWin->EventQueue();
			queue->Wait();
			const TWsEvent* prev=queue->PeekLastEvent();
			if (prev!=NULL && prev->Type()==EEventKey && prev->Key()->iRepeats>0)
				{
			//WS_ASSERT_DEBUG(prev->Key()->iScanCode==aScanCode, EWsPanicKeyRepeat); //This ASSERT can be triggered by using new functionality, need to find a way to make it tell the difference.
				event= *prev;
				event.Key()->iRepeats+=aRepeats;
				queue->UpdateLastEvent(event);
				if (CClick::IsHandler())
					CClick::KeyEvent(EEventKeyRepeat,*event.Key());
				return;
				}
			queue->Signal();
			event.Key()->iRepeats=aRepeats;
			if (CClick::IsHandler())
				CClick::KeyEvent(EEventKeyRepeat,keyEvent);
			}
		QueueKeyEvent(focusWin, event, EEventPriorityLow);
		}
	}

void TWindowServerEvent::QueueKeyUpDown(const TRawEvent &aRawEvent)
 	{
	CWsWindowGroup *focusWin=CWsCaptureKeyUpsAndDowns::CheckForCapture(aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE, iModifierState);
	if (!focusWin)	// If not captured
		focusWin=CWsTop::FocusWindowGroup();
	TWsEvent event;
	TEventCode type=aRawEvent.Type()==TRawEvent::EKeyUp ? EEventKeyUp : EEventKeyDown;
	event.Key()->iCode=0;
#if defined(__WINS__)
	if (focusWin && !focusWin->WsOwner()->RemoveKeyCode())
		event.Key()->iScanCode=aRawEvent.ScanCode();
	else
#endif
	event.Key()->iScanCode=aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE;
	event.Key()->iModifiers=iModifierState;
	event.Key()->iRepeats=0;
	if (CClick::IsHandler())
		CClick::KeyEvent(type,*event.Key());
	if (focusWin!=NULL)
		{
		event.SetType(type);
		event.SetHandle(focusWin->ClientHandle());
		QueueKeyEvent(focusWin, event, EEventPriorityHigh);
		}
	}

LOCAL_D void GetPointerEvent(TPointerEvent::TType& aType, const TRawEvent &aRawEvent, TBool& aHandled)
	{
	aHandled=ETrue;
	switch(aRawEvent.Type())
		{
	case TRawEvent::EButton1Down:
		aType=TPointerEvent::EButton1Down;
		break;
	case TRawEvent::EButton1Up:
		aType=TPointerEvent::EButton1Up;
		break;
	case TRawEvent::EButton2Down:
		aType=TPointerEvent::EButton2Down;
		break;
	case TRawEvent::EButton2Up:
		aType=TPointerEvent::EButton2Up;
		break;
	case TRawEvent::EButton3Down:
		aType=TPointerEvent::EButton3Down;
		break;
	case TRawEvent::EButton3Up:
		aType=TPointerEvent::EButton3Up;
		break;
	case TRawEvent::EPointerMove:
		aType=TPointerEvent::EMove;
		break;
	case TRawEvent::EPointerSwitchOn:
		aType=TPointerEvent::ESwitchOn;
		break;
	default:
		aHandled=EFalse;
		}
	}

TBool TWindowServerEvent::MousePress(const TRawEvent &aRawEvent, const CWsWindowGroup *aGroupWin)
	//
	//Return EFalse if known not to be a Mouse Event
	//
	{
	TBool handled=ETrue;
	TPointerEvent::TType type;
	GetPointerEvent(type, aRawEvent, handled);
	if (handled)
		{
		TPoint xy(aRawEvent.Pos());
		WsPointer::ProcessEvent(type, xy, iKeyTranslator->GetModifierState(), aGroupWin, ETrue);
		}
	return handled;
	}

LOCAL_D void SendEventToKeyClick(const TRawEvent& aRawEvent)
	{
	switch(aRawEvent.Type())
		{
		case TRawEvent::EKeyDown:
		case TRawEvent::EKeyUp:
			{
			TKeyEvent keyEvent;
			keyEvent.iCode=0;
			keyEvent.iScanCode=aRawEvent.ScanCode();
			keyEvent.iModifiers=0;
			keyEvent.iRepeats=0;
			CClick::KeyEvent(EEventKey,keyEvent);
			}
			break;
		case TRawEvent::EButton1Down:
		case TRawEvent::EButton1Up:
		case TRawEvent::EButton2Down:
		case TRawEvent::EButton2Up:
		case TRawEvent::EButton3Down:
		case TRawEvent::EButton3Up:
		case TRawEvent::EPointerMove:
		case TRawEvent::EPointerSwitchOn:
			{
			TBool handled=ETrue;
			TPointerEvent::TType type;
			GetPointerEvent(type, aRawEvent, handled);
			if (handled)
				{
				TPointerEvent pointerEvent;
				pointerEvent.iType=type;
				pointerEvent.iModifiers=0;
				pointerEvent.iPosition=aRawEvent.Pos();
				pointerEvent.iParentPosition=TPoint(KMinTInt32,KMinTInt32);
				CClick::PointerEvent(pointerEvent.iPosition,pointerEvent);
				}
			}
			break;
		default:
			break;
		}
	}

void TWindowServerEvent::ProcessRawEvent(const TRawEvent& aRawEvent)
//
// Event has completed.
//
	{
	WS_TRACE_SERVER_PROCESSRAWEVENT();
	TInt count=iEventHandlers->Count();
	TInt ii;
	TBool eventHandled = EFalse;
	iEventHandlerCount++;
	for(ii=0;ii<count;++ii)
		{
		if ((*iEventHandlers)[ii] != NULL && (*iEventHandlers)[ii]->OfferRawEvent(aRawEvent))
			{
			if (CClick::IsHandler())
				{
				SendEventToKeyClick(aRawEvent);
				}
			eventHandled = ETrue;
			break;
			}
		}
	if (--iEventHandlerCount == 0)
		{
		if (ERemovedEventHandlerWhileProcessingRawEvents & iBinaryFlags) // Handler was deleted while previous loop
			{ 
			iBinaryFlags &= ~ERemovedEventHandlerWhileProcessingRawEvents;
			for(ii=count-1;ii>=0;--ii)
				{
				if ((*iEventHandlers)[ii]==NULL) iEventHandlers->Delete(ii);
				}
			}
		}
	if (eventHandled)
		{
		return;
		}
	switch(aRawEvent.Type())
		{
		case TRawEvent::ERedraw:
			CWsTop::RedrawScreens();
			break;
		case TRawEvent::ESwitchOn:
		case TRawEvent::ECaseOpen:
			{
			TInt event=EEventCaseOpened;
			CKeyboardRepeat::CancelRepeat(NULL);
			CWsPassword::SwitchOn();
			if (aRawEvent.Type()==TRawEvent::ESwitchOn)
				{
				UserSvr::WsSwitchOnScreen();
				HAL::Set(HALData::EDisplayState,1);
				event=EEventSwitchOn;
				}
			ProcessEventQueue(iSwitchOnQueue, SendSwitchOnEvent, event, 0);
			break;
			}
		case TRawEvent::ESwitchOff:
		case TRawEvent::ECaseClose:
			{
			TBool switchOff=(aRawEvent.Type()==TRawEvent::ESwitchOff);
			CWsTop::HandleSwitchOff(switchOff? EEventSwitchOff:EEventCaseClosed,switchOff);
			break;
			}
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP			
		case TRawEvent::ERestartSystem:
			{ /* restart event being handled */
			CWsTop::HandleSwitchOff(EEventRestartSystem,ETrue);
			break;
			}
#endif			
		case TRawEvent::EInactive:
#ifndef __WINS__
			CWsTop::WindowServer()->AnimationScheduler()->OnInactive();
#endif
			CKeyboardRepeat::CancelRepeat(NULL);
			break;
		case TRawEvent::EActive:
#ifndef __WINS__
			CWsTop::WindowServer()->AnimationScheduler()->OnActive();
#endif
			break;
		case TRawEvent::EKeyDown:
			{
			_LIT(KWSERVDebugLogKeyDownArrival,"Key down arrives %d");
			if(CDebugBar* dbg = CWsTop::Screen()->DebugBar())
				dbg->OnKeyEvent();
			if (wsDebugLog)
				wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogKeyDownArrival,aRawEvent.ScanCode());
			CKeyboardRepeat::KeyDown();
			TKeyData keyData;
			TBool translated=iKeyTranslator->TranslateKey(aRawEvent.ScanCode(), EFalse,*iCaptureKeys,keyData);
			ProcessModifierChanges();
			QueueKeyUpDown(aRawEvent);
			if (translated)
				QueueKeyPress(keyData,aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE,NULL,ETrue,0);
			}
			break;
		case TRawEvent::EKeyUp:
			{
			_LIT(KWSERVDebugLogKeyUpArrival,"Key up arrives %d");
			if(CDebugBar* dbg = CWsTop::Screen()->DebugBar())
				dbg->OnKeyEvent();
			if (wsDebugLog)
				wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogKeyUpArrival,aRawEvent.ScanCode());
			TKeyData keyData;
			CKeyboardRepeat::KeyUp(aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE);
			TBool translated=iKeyTranslator->TranslateKey(aRawEvent.ScanCode(), ETrue,*iCaptureKeys,keyData);
			ProcessModifierChanges();
			QueueKeyUpDown(aRawEvent);
			if (translated)
				{
				CKeyboardRepeat::CancelRepeat(NULL);
				QueueKeyPress(keyData,aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE,NULL,EFalse,0);
				}
			}
			break;
		case TRawEvent::EButton1Down:
		case TRawEvent::EButton2Down:
		case TRawEvent::EButton3Down:
		case TRawEvent::EPointerSwitchOn:
#ifndef __WINS__
			CWsTop::WindowServer()->AnimationScheduler()->OnActive();
#endif
			// fall through
		case TRawEvent::EButton1Up:
		case TRawEvent::EButton2Up:
		case TRawEvent::EButton3Up:
		case TRawEvent::EPointerMove:
			#if defined(_DEBUG)
				WS_ASSERT_DEBUG(MousePress(aRawEvent,NULL), EWsPanicEventType);
			#else
				MousePress(aRawEvent,NULL);
			#endif
			break;
		case TRawEvent::EUpdateModifiers:
			iKeyTranslator->UpdateModifiers(aRawEvent.Modifiers());
			break;
		case TRawEvent::EKeyRepeat:
 			{
 			_LIT(KWSERVDebugLogRepeatingKeyArrival,"Repeating key arrives %d");
 			if (wsDebugLog)
 				wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogRepeatingKeyArrival,aRawEvent.ScanCode());
 			TKeyData keyData;
 			keyData.iModifiers=iKeyTranslator->GetModifierState();
			keyData.iApp=0;
			keyData.iHandle=0;
			keyData.iIsCaptureKey=EFalse;
			keyData.iKeyCode=aRawEvent.ScanCode(); 
			iCaptureKeys->ProcessCaptureKeys(keyData);
			QueueKeyPress(keyData, aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE,NULL,EFalse,aRawEvent.Repeats());
 			}
 			break;
		default:
			break;
		}
	}

void TWindowServerEvent::ProcessKeyEvent(const TKeyEvent &aKeyEvent,TInt aRepeats)
	{
	TKeyData keyData;
	keyData.iModifiers=aKeyEvent.iModifiers;
	keyData.iApp=0;
	keyData.iHandle=0;
	keyData.iIsCaptureKey=EFalse;
	keyData.iKeyCode=aKeyEvent.iCode;
	if (CKeyboardRepeat::IsAreadyActive())
		{
		CKeyboardRepeat::CancelRepeat(NULL);
		}
	iCaptureKeys->ProcessCaptureKeys(keyData);
	QueueKeyPress(keyData,aKeyEvent.iScanCode,NULL,aRepeats==0,aRepeats);
	}

void TWindowServerEvent::AddCaptureKeyL(const TCaptureKey &aCaptureKey)
	{
	iCaptureKeys->AddCaptureKeyL(aCaptureKey,aCaptureKey.iKeyCodePattern.iFiller);
	}

void TWindowServerEvent::SetCaptureKey(TUint32 aHandle, const TCaptureKey &aCaptureKey)
	{
	iCaptureKeys->SetCaptureKey(aHandle, aCaptureKey,aCaptureKey.iKeyCodePattern.iFiller);
	}

void TWindowServerEvent::CancelCaptureKey(TUint32 aHandle)
	{
	iCaptureKeys->CancelCaptureKey(aHandle);
	}

TInt TWindowServerEvent::GetModifierState()
	{
	return(iKeyTranslator->GetModifierState());
	}

void TWindowServerEvent::SetModifierState(TEventModifier aModifier,TModifierState aState)
	{
	iKeyTranslator->SetModifierState(aModifier,aState);
	}

TInt TWindowServerEvent::AddNotificationHandler(CAnim* aAnim, TUint32 aNotifications)
	{
	SNotificationHandler notif;
	notif.iAnim = aAnim;
	notif.iNotifications = aNotifications;

	// update the entry if the anim is already in the array
	TInt count=iNotificationHandlers->Count();
	TInt ii;
	for(ii=0;ii<count;++ii)
		{
		if ((*iNotificationHandlers)[ii].iAnim==aAnim)
			{
			(*iNotificationHandlers)[ii]=notif;
			return KErrNone;
			}
		}
	
	// otherwise add it to the array
	TRAPD(err,iNotificationHandlers->AppendL(notif));
	return err;
	}

void TWindowServerEvent::RemoveNotificationHandler(CAnim* aAnim)
	{
	TInt count=iNotificationHandlers->Count();
	TInt ii;
	for(ii=0;ii<count;++ii)
		{
		if ((*iNotificationHandlers)[ii].iAnim==aAnim)
			{
			iNotificationHandlers->Delete(ii);
			return;
			}
		}
	}

void TWindowServerEvent::PublishNotification(const TWsEvent& aWsEvent)
	{
	TInt count=iNotificationHandlers->Count();
	TInt ii;
	for(ii=0;ii<count;++ii)
		{
		SNotificationHandler notif = (*iNotificationHandlers)[ii];
		switch (aWsEvent.Type())
			{
		case EEventDirectScreenAccessBegin:
		case EEventDirectScreenAccessEnd:
			if (notif.iNotifications & EDirectScreenAccess)
				{
				notif.iAnim->HandleNotification(aWsEvent);
				}
			break;
		case EEventHeartbeatTimerStateChange:
			if (notif.iNotifications & EHeartbeatTimer)
				{
				notif.iAnim->HandleNotification(aWsEvent);
				}
			break;
		case EEventScreenDeviceChanged:
			if (notif.iNotifications & EScreenDeviceChange)
				{
				notif.iAnim->HandleNotification(aWsEvent);
				}
			break;
		default:
			break;
			}
		}

	}

TBool TWindowServerEvent::DrawerCompareFunc(const TDrawerHandler& lhs, const TDrawerHandler& rhs)
	{
	return lhs.iDrawer == rhs.iDrawer;	
	}

TInt TWindowServerEvent::RegisterDrawerHandler(CWsGraphicDrawer* aDrawer, TUint32 aEvents)
	{
	TInt idx = iDrawerHandlers->Find(TDrawerHandler(aDrawer, aEvents),
		TIdentityRelation<TDrawerHandler>(TWindowServerEvent::DrawerCompareFunc));
	if (idx != KErrNotFound)
		{
		// replace event mask for this drawer
		(*iDrawerHandlers)[idx].iEvents = aEvents;
		idx = KErrNone;
		}
	else
		idx = iDrawerHandlers->Append(TDrawerHandler(aDrawer,aEvents));
	
	return idx;
	}

TInt TWindowServerEvent::UnregisterDrawerHandler(CWsGraphicDrawer* aDrawer)
	{
	TInt idx = iDrawerHandlers->Find(TDrawerHandler(aDrawer,0),
		TIdentityRelation<TDrawerHandler>(TWindowServerEvent::DrawerCompareFunc));
	if (idx == KErrNotFound)
		return idx;
	(*iDrawerHandlers)[idx].iDrawer = NULL;		//NotifyDrawer() will clean up the array
	return KErrNone;
	}

TInt TWindowServerEvent::RegisterWsEventHandler(MWsEventHandler * aHandler, TUint32 aEvents)
	{
	TWsEventHandler handler(aHandler, aEvents);
	TInt idx = iWsEventHandlers.Find(handler, TIdentityRelation<TWsEventHandler>(TWsEventHandler::CompareHandler));
	if (idx < 0)
		{
		TInt err = iWsEventHandlers.Append(handler);
		return err;
		}
	else
		{
		iWsEventHandlers[idx].iEvents = aEvents;
		return KErrNone;
		}
	}
	
TInt TWindowServerEvent::UnregisterWsEventHandler(MWsEventHandler * aHandler)
	{
	TWsEventHandler handler(aHandler, 0);
	TInt idx = iWsEventHandlers.Find(handler, TIdentityRelation<TWsEventHandler>(TWsEventHandler::CompareHandler));
	if (idx < 0)
		return idx;
	iWsEventHandlers[idx].iEvents = NULL;	//NotifyDrawer() will clean up the array
	return KErrNone;
	}


void TWindowServerEvent::NotifyDrawer(const TWservCrEvent& aEvent)
	{
	TInt drawerCount = iDrawerHandlers->Count();
	for (TInt idx = 0; idx < drawerCount; idx++)
		{
		TDrawerHandler hd = (*iDrawerHandlers)[idx]; 
		if (!hd.iDrawer) 
			{                					//If the handler has been removed 
			iDrawerHandlers->Remove(idx);       //Remove from the array 
			drawerCount -= 1;   				//Update the counters 
			idx -= 1; 
			} 
		else 
			{ 
			if (hd.iEvents & aEvent.Type()) 
				{ 
			    hd.iDrawer->HandleEvent(aEvent); 
			    } 
			} 
		}
	
	TInt eventHandlerCount = iWsEventHandlers.Count();
	for (TInt idx = 0; idx < eventHandlerCount; ++idx)
		{
		TWsEventHandler* eh = &iWsEventHandlers[idx];
		if (!eh->iEvents)
			{								//If the handler has been removed
			iWsEventHandlers.Remove(idx);	//Remove from the array
			drawerCount -= 1;				//Update the counters
			idx -= 1;
			}
		else
			{
			if (eh->iEvents & aEvent.Type())
				{
				eh->iHandler->DoHandleEvent(aEvent);
				}
			}
		}
	}

void TWindowServerEvent::NotifyScreenDrawingEvent(const TRegion* aRegion)
	{
	if (aRegion && !aRegion->IsEmpty())
		{
		TWservCrEvent event(TWservCrEvent::EScreenDrawing,0,const_cast<TRegion*>(aRegion));
		NotifyDrawer(event);
		}
	}

void TWindowServerEvent::NotifyScreenDrawingEvent(const TRect& aRect)
	{
	TRegionFix<1> reg(aRect);
	TWservCrEvent event(TWservCrEvent::EScreenDrawing,0,&reg);
	NotifyDrawer(event);
	}

//
// CRawEventReceiver //
//

CRawEventReceiver::CRawEventReceiver(TInt aPriority) : CActive(aPriority)
//
// Constructor
//
	{
	__DECLARE_NAME(_S("CRawEventReceiver"));
	}

CRawEventReceiver::~CRawEventReceiver()
	{
	CActive::Cancel();
	}

void CRawEventReceiver::ConstructL()
	{
	CActiveScheduler::Add(this);
	UserSvr::CaptureEventHook();
	Request();
	}

void CRawEventReceiver::Request()
//
// Issue a request for the next event.
//
	{
	UserSvr::RequestEvent(iEventBuf,iStatus);
	SetActive();
	}

void CRawEventReceiver::DoCancel()
//
// Cancel a pending event.
//
	{
	UserSvr::RequestEventCancel();
	}

void CRawEventReceiver::RunL()
	{
//__PROFILE_START(11);
	if (WsPointer::PreProcessEvent(iEventBuf.Event()
#if defined(__WINS__)
													,ETrue
#endif
														  ))
		TWindowServerEvent::ProcessRawEvent(iEventBuf.Event());
	Request();
//__PROFILE_END(11);
	}

//
// TEventRequestQueue //
//

TEventRequestQueue::TEventRequestQueue() : iQueue(_FOFF(TEventRequestItem,iQue))
	{}

inline TSglQue<TEventRequestItem> &TEventRequestQueue::Queue()
	{return(iQueue);}

TEventRequestItem *TEventRequestQueue::FindInEventRequestQueueList(const CWsWindowBase &aWindow)
//
// Return a pointer to the link in the queue for the window, or NULL if not in the queue
//
	{
	TSglQueIter<TEventRequestItem> iter(iQueue);
	TEventRequestItem *qPtr;
	while((qPtr=iter++)!=NULL)
		if (qPtr->iWindow==&aWindow)
			break;
	return(qPtr);
	}

void TEventRequestQueue::AddToEventRequestListL(const CWsWindowBase &aWindow, TInt aParam, TEventControl aCircumstances)
//
// Add a link to the on event list
//
	{
	TEventRequestItem *item=FindInEventRequestQueueList(aWindow);
	if (!item)
		{
		item=new(ELeave) TEventRequestItem;
		item->iWindow= &aWindow;
		item->iParam=aParam;
		item->iCircumstances=aCircumstances;
		iQueue.AddFirst(*item);
		}
	item->iCircumstances=aCircumstances;
	item->iParam=aParam;	// Just update the parameter if already exists
	}

void TEventRequestQueue::RemoveFromEventRequestListL(const CWsWindowBase &aWindow)
//
// Remove a link from the on event list
//
	{
	TEventRequestItem *qPtr=FindInEventRequestQueueList(aWindow);
	if (qPtr)
		{
		iQueue.Remove(*qPtr);
		delete qPtr;
		}
	}

//
// Keyboard auto repeat class //
//

CKeyboardRepeat::CKeyboardRepeat() : CTimer(EKeyRepeatPriority)
	{}

void CKeyboardRepeat::NewL()
	{
	iThis=new(ELeave) CKeyboardRepeat();
	iThis->ConstructL();
	CActiveScheduler::Add(iThis);
	_LIT(KWSERVIniFileVarRepeatRollover,"REPEATROLLOVER");
	WsIniFile->FindVar(KWSERVIniFileVarRepeatRollover,iRepeatRollover);
	}

void CKeyboardRepeat::Destroy()
	{
	delete iThis;
	}

void CKeyboardRepeat::GetRepeatTime(TTimeIntervalMicroSeconds32 &aInitialTime, TTimeIntervalMicroSeconds32 &aTime)
	{
	aInitialTime=iInitialTime;
	aTime=iTime;
	}

void CKeyboardRepeat::SetRepeatTime(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime)
	{
	iInitialTime=aInitialTime;
	iTime=aTime;
	}

void CKeyboardRepeat::RunL()
	{
	User::ResetInactivityTime();
	//WS_ASSERT_DEBUG(iRepeating!=ERepeatNone, EWsPanicTemp);
	TBool timer=ETrue;
	if (iRepeating>=ERepeatLong)
		{
		// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong
		if (iLongCapture)
			{
			iCurrentRepeat.iKey.iApp=REINTERPRET_CAST(TUint32,iLongCapture->iWindowGroup);
			iCurrentRepeat.iKey.iHandle=0;
			iCurrentRepeat.iKey.iIsCaptureKey=ETrue;
			iCurrentRepeat.iKey.iKeyCode=iLongCapture->iData.outputKey;
			timer=iLongCapture->iData.flags&ELongCaptureRepeatEvents;
			iRepeating=ERepeatLongRepeated;
			}
		else
			{
			// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong
			// Stop key repeat if this incorrect condition occurs
			timer=EFalse; 
			}		
		}
	if (timer)
		After(iTime);
	else
		iRepeating=ERepeatNone;
	TWindowServerEvent::QueueKeyPress(iCurrentRepeat.iKey,iCurrentRepeat.iScanCode,iFocus,EFalse,1);
	}

TBool CKeyboardRepeat::StartRepeat(const TKeyData &aKey, TInt aScanCode, CWsWindowGroup *aRepeatFocus, CWsCaptureLongKey* aLongCapture)
	{
	TTimeIntervalMicroSeconds32 time;
	TBool ret=EFalse;
	iCurrentRepeat.iScanCode=aScanCode;
	iCurrentRepeat.iKey=aKey;
	iFocus=aRepeatFocus;
	if (aLongCapture)
		{
		iLongCapture=aLongCapture;
		iRepeating=ERepeatLong;
		time=aLongCapture->iData.delay;
		ret=!(aLongCapture->iData.flags&ELongCaptureShortEventImmediately);
		}
	else
		{
		iRepeating=ERepeatNormal;
		time=iInitialTime;
		}
	iThis->After(time);
	return ret;
	}

void CKeyboardRepeat::doCancelRepeat()
	{
	iRepeating=ERepeatNone;
	iThis->Cancel();
	}

void CKeyboardRepeat::CancelRepeat(CWsWindowGroup *aRepeatFocus)
	{
	if (aRepeatFocus==NULL || aRepeatFocus==iFocus)
		{
		if (iRepeating)
			doCancelRepeat();
		iAlternateRepeatExists=EFalse;
		}
	else if (iRepeating >= ERepeatLong)
		{
		// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong
		if (iLongCapture && iLongCapture->iWindowGroup == aRepeatFocus)
			{
			doCancelRepeat();
			iAlternateRepeatExists=EFalse;
			}
		}
	}

void CKeyboardRepeat::CancelRepeat(CWsWindowGroup *aRepeatFocus,TUint aScanCode,TBool aLongCaptureFlag,TUint aModifiers)
	{
	// aLongCaptureFlag indicates if CancelRepeat caused by call to CancelCaptureLongKey()
	if (aLongCaptureFlag)
		{
		// long capture key is cancelled
		if (iRepeating >= ERepeatLong && iCurrentRepeat.iScanCode==aScanCode)			
				{
				// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong
				if (iLongCapture && aRepeatFocus == iLongCapture->iWindowGroup &&
					(aModifiers & iLongCapture->iData.modifierMask) == iLongCapture->iData.modifiers)
					{
					doCancelRepeat();
					iAlternateRepeatExists=EFalse;
					}
				}
		}
	else
		{
		// normal capture key is cancelled
		if (aRepeatFocus==iFocus)
			{
			if (iRepeating>=ERepeatNormal && iCurrentRepeat.iScanCode==aScanCode)
				{
				doCancelRepeat();
				}
			iAlternateRepeatExists=EFalse;
			}
		}
	}
	
void CKeyboardRepeat::KeyDown()
	{
	if (iRepeating!=ERepeatNone)
		{
		if (iRepeating==ERepeatNormal && iRepeatRollover>0) // 1 Allow key repeat rollover
			{
			iAlternateRepeat=iCurrentRepeat;
			iAlternateRepeatExists=ETrue;
			}
		doCancelRepeat();
		}
	}

void CKeyboardRepeat::KeyUp(TInt aScanCode)
	{
	if (iAlternateRepeatExists && iAlternateRepeat.iScanCode==aScanCode)
		iAlternateRepeatExists=EFalse;
	if (iRepeating!=ERepeatNone && iCurrentRepeat.iScanCode==aScanCode)
		{
		if (iRepeating==ERepeatLong)
			{
			// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong		
			if (iLongCapture && !(iLongCapture->iData.flags&ELongCaptureShortEventImmediately))
				{
				TWindowServerEvent::QueueKeyPress(iCurrentRepeat.iKey,iCurrentRepeat.iScanCode,NULL,EFalse,0);
				}
			}
		if (iAlternateRepeatExists)
			{
			iAlternateRepeatExists=EFalse;
			iCurrentRepeat=iAlternateRepeat;
			iRepeating=ERepeatNormal;
			}
		else
			doCancelRepeat();
		}
	}

//
// CWsHotKey //
//

CWsHotKey::CWsHotKey(TInt aHotKeyType, TBool aIsDefault) : 
	iHotKeyType(aHotKeyType),
	iIsDefault(aIsDefault)
	{
	}

CWsHotKey::~CWsHotKey()
	{
	delete iCaptureKey;
	}

void CWsHotKey::ConstructLD(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	CleanupStack::PushL(this);
	iCaptureKey=new(ELeave) CWsCaptureKey(NULL);
	iCaptureKey->ConstructL(aCaptureKey);
	CleanupStack::Pop();
	}

void CWsHotKey::SetL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	iCaptureKey->SetL(aCaptureKey);
	}


