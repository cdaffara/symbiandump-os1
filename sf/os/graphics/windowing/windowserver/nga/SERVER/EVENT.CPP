// Copyright (c) 1994-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "W32STD.H"
#include <e32uid.h>
#include <hal.h>
#include <w32adll.h>
#include "W32CLICK.H"
#include "server.h"
#include "windowgroup.h"
#include "KEYCLICK.H"
#include "wstop.h"
#include "panics.h"
#include "screen.h"
#include "inifile.h"
#include "password.h"
#include "pointer.h"
#include "debugbar.h"
#include "advancedpointereventhelper.h"
#include "graphics/wsgraphicdrawerinternal.h"
#include "debughelper.h"

GLREF_D CDebugLogBase *wsDebugLog;

GLREF_C void StateDump();
GLREF_C void HeapDump();

_LIT(KDefaultKeyRouterPluginName, "keyrouter.dll");
_LIT(KWSERVIniFileVarKeyRouterPlugin, "KEYROUTERPLUGIN");

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
CKeyEventRouter* TWindowServerEvent::iKeyEventRouter;
RLibrary TWindowServerEvent::iKeyEventRouterLibrary;
CWsHotKey *TWindowServerEvent::iHotKeys;
TInt TWindowServerEvent::iModifierState;
CRawEventReceiver *TWindowServerEvent::iEventReceiver;
RArray<TWindowServerEvent::TRawEventHandler> TWindowServerEvent::iEventHandlers;
CArrayFixFlat<SNotificationHandler> *TWindowServerEvent::iNotificationHandlers;
TInt TWindowServerEvent::iPotentialEventHandlers=0;
TUint32 TWindowServerEvent::iBinaryFlags=0x00;
RArray<TDrawerHandler>* TWindowServerEvent::iDrawerHandlers;
RArray<TWsEventHandler> TWindowServerEvent::iWsEventHandlers;
TInt TWindowServerEvent::iEventHandlerCount=0;
TRepeatKey CKeyboardRepeat::iCurrentRepeat;
TRepeatKey CKeyboardRepeat::iAlternateRepeat;
TRepeatKey CKeyboardRepeat::iLongRepeat;
TInt CKeyboardRepeat::iRepeatRollover=1;
CKeyboardRepeat::TRepeatType CKeyboardRepeat::iRepeating=ERepeatNone;
CKeyboardRepeat *CKeyboardRepeat::iThis=NULL;
TTimeIntervalMicroSeconds32 CKeyboardRepeat::iInitialTime;
TTimeIntervalMicroSeconds32 CKeyboardRepeat::iTime;
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
	delete iKeyEventRouter;
	iKeyEventRouterLibrary.Close();
	CKeyboardRepeat::Destroy();
	delete iKeyTranslator;
	delete iEventReceiver;
	iEventHandlers.Close();
	delete iNotificationHandlers;
	iDrawerHandlers->Close();
	delete iDrawerHandlers;
	iWsEventHandlers.Close();
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

	// CCaptureKeys is no longer used but a dummy object is required for
	// calls to CKeyTranslator::TranslateKey() until capture functionality
	// has been removed from ektran.dll.
	iCaptureKeys=new(ELeave) CCaptureKeys;
	iCaptureKeys->Construct();

	// Load the key event routing plug-in. The DLL name may be overridden
	// by setting the keyword KEYROUTERPLUGIN in wsini.ini.
	TPtrC pluginName(KDefaultKeyRouterPluginName);
	WsIniFile->FindVar(KWSERVIniFileVarKeyRouterPlugin, pluginName);
	const TUidType uidType(KDynamicLibraryUid, KKeyRouterPluginUid);
	TInt err = iKeyEventRouterLibrary.Load(pluginName, uidType);

	if (wsDebugLog)
		{
		TLogMessageText buf;

		if (err == KErrNone)
			{
			_LIT(KLogLoadOk, "Loaded plugin '%S' UID3 0x%x");
			const TFileName& pluginPathname = iKeyEventRouterLibrary.FileName();
			const TUid uid3 = iKeyEventRouterLibrary.Type()[2];
			buf.Format(KLogLoadOk, &pluginPathname, uid3.iUid);
			}
		else
			{
			_LIT(KLogLoadError, "Failed to load plugin '%S' (error %d)");
			buf.Format(KLogLoadError, &pluginName, err);
			}

		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant, buf);
		}

	if (err != KErrNone)
		{
#ifdef _DEBUG
		_LIT(KLoadError, "WServ: failed to load plugin '%S' (error %d)");
		RDebug::Print(KLoadError, &pluginName, err);
#endif
		User::Leave(err);
		}

	// Create the key event router
	typedef CKeyEventRouter* (*TCreateFunc)();
	TCreateFunc newL = reinterpret_cast<TCreateFunc>(iKeyEventRouterLibrary.Lookup(1));
	if (newL == NULL)
		{
		User::Leave(KErrNotFound);
		}
	iKeyEventRouter = (*newL)();

	for (TInt index=0;index<TWindowServerEvent::ENumHotKeys;index++)
		ConstructDefaultHotKeyL(index,DefaultHotKeys[index]);
	CKeyboardRepeat::NewL();
	CKeyboardRepeat::SetRepeatTime(EDefaultInitialRepeatTime, EDefaultRepeatTime);
	iEventHandlers=RArray<TRawEventHandler>(2);
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
	captureKey.priority = 0;
	hotKey->ConstructLD(captureKey);
//
	LinkHotKey(hotKey);
	}

void TWindowServerEvent::AddEventHandler(MEventHandler *aEventHandler, TBool aAdvancedPointersEnabled)
	{
#if defined(_DEBUG)
	TRAPD(err,iEventHandlers.AppendL(TRawEventHandler(aEventHandler, aAdvancedPointersEnabled)));
	WS_ASSERT_DEBUG(err==KErrNone, EWsPanicEventHandlerInconsistency);
#else
	iEventHandlers.AppendL(TRawEventHandler(aEventHandler, aAdvancedPointersEnabled));	//Shouldn't leave
#endif
#ifdef LOG_WSERV_EVENTS
	RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::AddEventHandler Added handler = %d AdvancedPointerEnabled = %d", iEventHandlers.Count(),aAdvancedPointersEnabled);
#endif
	}

void TWindowServerEvent::RemoveEventHandler(const MEventHandler *aEventHandler)
	{
	TInt count=iEventHandlers.Count();
	TInt ii;
	for(ii=0;ii<count;++ii)
		{
		if (iEventHandlers[ii].iEventHandler==aEventHandler)
			{
#ifdef LOG_WSERV_EVENTS
		    RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::RemoveEventHandler Removed handler = %d",ii);
#endif
			if (iEventHandlerCount>0)  
				{
				iBinaryFlags |= ERemovedEventHandlerWhileProcessingRawEvents;
				iEventHandlers[ii].iEventHandler=NULL; // replace the Handler with null to keep size of the array
				}
			else 
				{
				iEventHandlers.Remove(ii);
				}
			return;
			}
		}
	}

void TWindowServerEvent::PotentialEventHandlerL(TInt aNum)
	{
	iPotentialEventHandlers+=aNum;
	WS_ASSERT_DEBUG(iPotentialEventHandlers>=iEventHandlers.Count(), EWsPanicEventHandlerInconsistency);
	TRAPD(err,iEventHandlers.Reserve(iPotentialEventHandlers));
	if (err!=KErrNone)
		{
		if (aNum>0)
			User::Leave(err);
		}
	else if (iPotentialEventHandlers==0)
		iEventHandlers.Compress();
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

TEventQueueWalkRet OverrideDisplayChangedEvent(TAny *aNewEvent, TWsEvent *aOldEvent)
	{
	TWsEvent* newEvent = static_cast<TWsEvent*>(aNewEvent);
	if (aOldEvent->Type() == EEventDisplayChanged && 
			aOldEvent->DisplayChanged()->iDisplayNumber == newEvent->DisplayChanged()->iDisplayNumber)
		{
		aOldEvent->SetTimeNow();
		aOldEvent->DisplayChanged()->iConfigurationChangeId = newEvent->DisplayChanged()->iConfigurationChangeId;
		aOldEvent->DisplayChanged()->iResolutionListChangeId = newEvent->DisplayChanged()->iResolutionListChangeId;
		newEvent->DisplayChanged()->iDisplayNumber = KErrNotFound; //So the new event won't be placed on event queue again
		}
	return EEventQueueWalkOk;
	}

TBool TWindowServerEvent::SendDisplayChangedEvents(CWsClient* aWsClient, TInt aDisplayNumber, TInt aConfigurationChangeId, TInt aResolutionListChangeId)
	{
	CEventQueue *queue = aWsClient->EventQueue();
	TWsEvent event;
	event.SetType(EEventDisplayChanged);
	event.SetTimeNow();
	
    // fill in the handle otherwise CONE will discard the notification
    CWsObjectIx* clientObjList = aWsClient->ObjectIndex();
    const TWsObject* ptr=clientObjList->FirstObject();
    const TWsObject* end=ptr+clientObjList->Length();
    while(++ptr<end)    // first one should always have a NULL object
        {
        const CWsObject* obj=ptr->iObject;
        if (obj && obj->Type()==WS_HANDLE_GROUP_WINDOW)
            {
            event.SetHandle(ptr->iHandle);
            break;
            }
        }	
	
	TWsDisplayChangedEvent* dispEvent = event.DisplayChanged();
	dispEvent->iDisplayNumber = aDisplayNumber;
	dispEvent->iConfigurationChangeId = aConfigurationChangeId;
	dispEvent->iResolutionListChangeId = aResolutionListChangeId;
	queue->WalkEventQueue(&OverrideDisplayChangedEvent, &event);
	//place the new event on the queue only when its display number is valid (!=KErrNotFound)
	if(event.DisplayChanged()->iDisplayNumber >= 0)
		{
		return queue->QueueEvent(event);
		}
	return ETrue;
	}

void TWindowServerEvent::QueueKeyEvent(CWsWindowGroup *aWin, TWsEvent &aEvent, TWservEventPriorities aPriority)
	{
#ifdef LOG_WSERV_EVENTS
    RDebug::Print(_L("_WSEVENT_KEY: TWindowServerEvent::QueueKeyEvent, Queuing event name %S for application read, window handle: %d"), &WsEventName(aEvent), CWsTop::FocusWindowGroup()->ClientHandle());
#endif
	aEvent.SetTimeNow();
	aWin->EventQueue()->QueueEvent(aEvent, aPriority);
	}

/**
Process a key press event.

This function is called for every input key event and uses the Key Event
Routing plug-in to check for short and long key capture and determine the
destination window group for the queued event(s).
Window server hotkeys are also processed.
Note that the key repeat timer is started here but the key repeat events
generated by the timer go directly to QueueKeyPress().

@param	aKeyEvent		Input key event
@param	aCheckRepeat	Check for key repeat and long key capture
@param	aRepeats		Repeat count
*/
void TWindowServerEvent::ProcessKeyPress(const TKeyEvent& aKeyEvent, TBool aCheckRepeat, TInt aRepeats)
 	{
	CWsWindowGroup* focusWin = CWsTop::FocusWindowGroup();
	TUid focusAppUid = focusWin ? TUid::Uid(focusWin->Client()->SecureId().iId) : KNullUid;

	// Route the key event and check for short key capture.
	// Note that the Key Routing plugin may translate or block key events.
	TKeyEventRouterInput input(ECaptureTypeKey, aKeyEvent, focusWin, focusAppUid);
	TKeyEventRouterOutput output;

#ifdef _DEBUG
	// RouteKey() must not fail. Check for leaves in case the plug-in
	// is badly behaved.
	TRAPD(err, iKeyEventRouter->RouteKey(input, output));
	WS_ASSERT_DEBUG(err == KErrNone, EWsPanicKeyEventRouterLeave);
#else
	iKeyEventRouter->RouteKey(input, output);
#endif

	WS_ASSERT_DEBUG(output.iResult == ERouted || output.iResult == ECaptured || output.iResult == EConsumed, EWsPanicKeyEventRouterBadResult);

	if (output.iResult == EConsumed)
		{
		focusWin = NULL;
		}
	else
		{
		focusWin = static_cast<CWsWindowGroup*>(output.iWindowGroup);
		}
	WS_ASSERT_DEBUG((focusWin == NULL || focusWin->Type() == WS_HANDLE_GROUP_WINDOW) && (output.iResult != ERouted || focusWin == CWsTop::FocusWindowGroup()), EWsPanicKeyEventRouterBadWindowGroup);

	// Ensure that short event is not marked with EModifierLongKey
	output.iKeyEvent.iModifiers &= ~EModifierLongKey;

	// Generate key click unless the event is consumed. This is consistent
	// with the behaviour when CKeyTranslator::TranslateKey() yields no
	// translation for a particular scan code. (Click events for key up/down
	// will still be generated by QueueKeyUpDown()). Note however that a long
	// key press may still be captured even if the short event is consumed.
	if (CClick::IsHandler() && output.iResult != EConsumed)
		{
		output.iKeyEvent.iRepeats = aRepeats;
		CClick::KeyEvent(EEventKey, output.iKeyEvent);
		}

	if (output.iResult == ECaptured)
		{
		if (output.iWindowGroup == NULL)	// Captured by Wserv itself
			{
			_LIT(KWSERVDebugLogCapturedKey,"WSERV Captured Key");
			CScreen* focusScreen=CWsTop::CurrentFocusScreen();
			TInt screenNo=focusScreen->ScreenNumber();
			
			if (wsDebugLog)
				wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogCapturedKey);
			CWsHotKey *hotKey=iHotKeys;
			while(hotKey)
				{
				if (hotKey->KeyHandle() == reinterpret_cast<TInt>(output.iCaptureHandle))
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

		_LIT(KWSERVDebugLogKeyCapturedByApp,"Key captured by app %d");
		if (wsDebugLog)
			wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogKeyCapturedByApp,focusWin->Identifier());
		if (CWsPassword::PasswordModeActive() && focusWin!=CWsPassword::PasswordWindow()->WinGroup())
			return;
		}

	CWsCaptureLongKey* longCapture = NULL;
	TKeyEventRouterOutput longOutput;
	if (aCheckRepeat)
		{
		// Check for long key capture.
		// Note that a long key event can only result from capture, there is
		// no default detection or routing of long events.
		input.iType = ECaptureTypeLongKey;
#ifdef _DEBUG
		TRAPD(err, iKeyEventRouter->RouteKey(input, longOutput));
		WS_ASSERT_DEBUG(err == KErrNone, EWsPanicKeyEventRouterLeave);
#else
		iKeyEventRouter->RouteKey(input, longOutput);
#endif

		if (longOutput.iResult == ECaptured)
			{
			longCapture = static_cast<CWsCaptureLongKey*>(longOutput.iCaptureHandle);

			// Mark long key events with EModifierLongKey so that applications
			// can easily distinguish short and long events.
			longOutput.iKeyEvent.iModifiers |= EModifierLongKey;

			// Start timer to detect long key press
			CKeyboardRepeat::StartRepeat(aKeyEvent.iScanCode, output, &longOutput);
			}
		else if (output.iResult != EConsumed && output.iKeyEvent.iModifiers & EModifierAutorepeatable)
			{
			// Start timer for key repeat
			CKeyboardRepeat::StartRepeat(aKeyEvent.iScanCode, output, NULL);
			}
		}

	// Queue the short event
	if (!longCapture || longCapture->iFlags & ELongCaptureShortEventImmediately)
		{
		QueueKeyPress(output, EFalse, aRepeats);
		}
	}

/**
Queue a key press event.

This function is called for each key event produced by ProcessKeyPress(),
for every key repeat and long key event generated by the timer and also for
delayed short key events from KeyUp().

@param	aOutput			Output key event from routing plug-in
@param	aIsRepeat		Event is due to key repeat
@param	aRepeats		Repeat count
*/
void TWindowServerEvent::QueueKeyPress(const TKeyEventRouterOutput& aOutput, TBool aIsRepeat, TInt aRepeats)
 	{
	if (aOutput.iResult == EConsumed)
		{
		// Don't deliver this key
		return;
		}

	TWsEvent event;
	TKeyEvent& keyEvent = *event.Key();
	keyEvent = aOutput.iKeyEvent;
	keyEvent.iRepeats = aRepeats;

	CWsWindowGroup* focusWin = static_cast<CWsWindowGroup*>(aOutput.iWindowGroup);
	WS_ASSERT_DEBUG(focusWin == NULL || focusWin->Type() == WS_HANDLE_GROUP_WINDOW, EWsPanicKeyEventRouterBadWindowGroup);

	if (aIsRepeat && aOutput.iResult != ECaptured && focusWin != CWsTop::FocusWindowGroup())
		CKeyboardRepeat::CancelRepeat(NULL);		// Repeat is going to different window so cancel it and don't deliver this key
	else if (focusWin != NULL && focusWin->CheckForPriorityKey(keyEvent) == EFalse)
		{
		event.SetType(EEventKey);
		event.SetHandle(focusWin->ClientHandle());
		if (aRepeats!=0)
			{
			CEventQueue* queue=focusWin->EventQueue();
			queue->Wait();
			const TWsEvent* prev=queue->PeekLastEvent();
			if (prev != NULL && prev->Type() == EEventKey && prev->Key()->iRepeats > 0 && prev->Key()->iCode == keyEvent.iCode)
				{
				prev->Key()->iRepeats += aRepeats;
				queue->Signal();
				if (CClick::IsHandler())
					CClick::KeyEvent(EEventKeyRepeat, *prev->Key());
				return;
				}
			queue->Signal();
			if (CClick::IsHandler())
				CClick::KeyEvent(EEventKeyRepeat,keyEvent);
			}
		QueueKeyEvent(focusWin, event, EEventPriorityLow);
		}
	}

/**
Queue a key up/down event.

@param	aRawEvent		Raw event
*/
void TWindowServerEvent::QueueKeyUpDown(const TRawEvent &aRawEvent)
 	{
#ifdef LOG_WSERV_EVENTS
    RDebug::Print(_L("_WSEVENT_KEY: TWindowServerEvent::QueueKeyUpDown, Event Name: %S, Scan code: %d"), &RawEventName(aRawEvent), aRawEvent.ScanCode());
#endif
	TEventCode type = aRawEvent.Type() == TRawEvent::EKeyUp ? EEventKeyUp : EEventKeyDown;

	// Check for key up/down capture
	TKeyEvent keyEvent;
	keyEvent.iScanCode = aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE;
#if defined(__WINS__)
	keyEvent.iCode = __WINS_CHARCODE(aRawEvent.ScanCode());
#else
	keyEvent.iCode = 0;
#endif
	keyEvent.iModifiers = iModifierState;
	keyEvent.iRepeats = 0;

	CWsWindowGroup* focusWin = CWsTop::FocusWindowGroup();
	TUid focusAppUid = focusWin ? TUid::Uid(focusWin->Client()->SecureId().iId) : KNullUid;

	TKeyEventRouterInput input(ECaptureTypeKeyUpDown, keyEvent, focusWin, focusAppUid);
	TKeyEventRouterOutput output;
#ifdef _DEBUG
	TRAPD(err, iKeyEventRouter->RouteKey(input, output));
	WS_ASSERT_DEBUG(err == KErrNone, EWsPanicKeyEventRouterLeave);
#else
	iKeyEventRouter->RouteKey(input, output);
#endif

	if (output.iResult == EConsumed)
		{
		// Don't deliver this key. A key click is still generated for the
		// input event.
		if (CClick::IsHandler())
			{
			CClick::KeyEvent(type, keyEvent);
			}
		return;
		}
	WS_ASSERT_DEBUG(output.iResult == ERouted || output.iResult == ECaptured, EWsPanicKeyEventRouterBadResult);

	focusWin = static_cast<CWsWindowGroup*>(output.iWindowGroup);
	WS_ASSERT_DEBUG((focusWin == NULL || focusWin->Type() == WS_HANDLE_GROUP_WINDOW) && (output.iResult != ERouted || focusWin == CWsTop::FocusWindowGroup()), EWsPanicKeyEventRouterBadWindowGroup);
#if defined(__WINS__)
	if (focusWin && !focusWin->WsOwner()->RemoveKeyCode())
		{
		// Restore WINS character code
		output.iKeyEvent.iScanCode |= output.iKeyEvent.iCode;
		}
	output.iKeyEvent.iCode = 0;
#endif

	output.iKeyEvent.iRepeats = 0;
	if (CClick::IsHandler())
		{
		CClick::KeyEvent(type, output.iKeyEvent);
		}

	TWsEvent event;
	*event.Key() = output.iKeyEvent;
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
	case TRawEvent::EPointer3DOutOfRange:
		aType=TPointerEvent::EOutOfRange;
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
		TPoint3D point3D(0,0,0);
		if (type != TPointerEvent::EOutOfRange)
			{
			point3D = aRawEvent.Pos3D();
			}
		TWsEvent event;
		TAdvancedPointerEventHelper::InitAdvancedPointerEvent(event, type,iKeyTranslator->GetModifierState(),point3D,aRawEvent.PointerNumber());
		TWsPointer::ProcessWsEvent(event, aGroupWin, ETrue);
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
				TWsEvent event;
				TAdvancedPointerEventHelper::InitAdvancedPointerEvent(event, type, 0, aRawEvent.Pos3D(), TPoint(), aRawEvent.PointerNumber());
				TAdvancedPointerEvent& pointerEvent = *event.Pointer();
				CClick::PointerEvent(pointerEvent.iPosition,pointerEvent);
				}
			}
			break;
		default:
			break;
		}
	}

/*
Process a raw event

@param	aRawEvent	Raw event
*/
void TWindowServerEvent::ProcessRawEvent(const TRawEvent& aRawEvent)
//
// Event has completed.
//
	{
	TRawEvent::TType eventType = aRawEvent.Type();
	TBool isPointerEvent = TWsPointer::IsPointerEventType(eventType);
	if (isPointerEvent)
		{
#ifdef LOG_WSERV_EVENTS
        RDebug::Print(_L("_WSEVENT_POINTER: TWindowServerEvent::ProcessRawEvent EventName = %S PointerNumber = %d PrimaryPointerNumber = %d Coordinates = ( %d, %d )"), 
                &RawEventName(aRawEvent),aRawEvent.PointerNumber(),TWsPointer::PrimaryPointer(),aRawEvent.Pos().iX,aRawEvent.Pos().iY); 
#endif
        TWsPointer::UpdatePrimaryPointer(aRawEvent);
		}
	TInt count=iEventHandlers.Count();
	TInt ii;
	TBool eventHandled = EFalse;
	iEventHandlerCount++;
	for(ii=0;ii<count;++ii)
		{
		TRawEventHandler &handler = iEventHandlers[ii];
		if (handler.iEventHandler != NULL &&
			(!isPointerEvent ||
			 handler.iAdvancedPointersEnabled ||
			 aRawEvent.PointerNumber() == TWsPointer::PrimaryPointer()) &&
			handler.iEventHandler->OfferRawEvent(aRawEvent))
			{
			if (CClick::IsHandler())
				{
#ifdef LOG_WSERV_EVENTS
				RDebug::Print(_L("_WSEVENT_KEY: Send event %S for Key Click"), &RawEventName(aRawEvent));
#endif
				SendEventToKeyClick(aRawEvent);
				}
			eventHandled = ETrue;
#ifdef LOG_WSERV_EVENTS
            RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::ProcessRawEvent Event Consumed by ANIM.dll Handler No = %d Advanced Pointer Enabled = %d",ii,handler.iAdvancedPointersEnabled);
#endif
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
				if (iEventHandlers[ii].iEventHandler==NULL) iEventHandlers.Remove(ii);
				}
			}
		}
	if (eventHandled)
		{
#ifdef LOG_WSERV_EVENTS
		RDebug::Printf("_WSEVENT: Event is already handled by anim dll not by window server");
		// This is to determine when we press the power button which bring power dialog
		// whether it is a pointer event or key event
		// Also when we plugin the charging cable this is to determine whether it is a pointer event or key event
		RDebug::Print(_L("_WSEVENT: RawEvent Name = %S"), &RawEventName(aRawEvent));
#endif
		if (isPointerEvent)
			{
#ifdef LOG_WSERV_EVENTS
			RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::ProcessRawEvent Pointer Number= %d  State = %x XY(%d,%d)",TWsPointer::iPointers[0].iNumber,TWsPointer::iPointers[0].iState,TWsPointer::iPointers[0].iPos.iX,TWsPointer::iPointers[0].iPos.iY);
		    RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::ProcessRawEvent Pointer Number= %d  State = %x XY(%d,%d)",TWsPointer::iPointers[1].iNumber,TWsPointer::iPointers[1].iState,TWsPointer::iPointers[1].iPos.iX,TWsPointer::iPointers[1].iPos.iY);
#endif
              //Prevention of the phone pointer event "dead lock". 
              TPointerEvent::TType type;
              TBool handled = ETrue;
              GetPointerEvent(type, aRawEvent, handled);
              switch(type)
                  {
                  case TPointerEvent::EButton1Down:
                  case TPointerEvent::EButton2Down:
                  case TPointerEvent::EButton3Down:
                      TWsPointer::iPointers[aRawEvent.PointerNumber()].iState = TWsPointer::EPointerStateDown;
                      break;
                  case TPointerEvent::EButton1Up:
                  case TPointerEvent::EButton2Up:
                  case TPointerEvent::EButton3Up:
                      TWsPointer::iPointers[aRawEvent.PointerNumber()].iState = TWsPointer::EPointerStateUp;
                      break;
                  case TPointerEvent::EOutOfRange:
                      TWsPointer::iPointers[aRawEvent.PointerNumber()].iState = TWsPointer::EPointerStateOutOfRange;
                      break;
                      default:
                      break;
                  }
			}
		return;
		}
	
	switch(eventType)
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
			if (eventType==TRawEvent::ESwitchOn)
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
			TBool switchOff=(eventType==TRawEvent::ESwitchOff);
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
#ifdef LOG_WSERV_EVENTS
			RDebug::Printf("_WSEVENT_KEY: TRawEvent::EKeyDown");
#endif
			_LIT(KWSERVDebugLogKeyDownArrival,"Key down arrives %d");
			CScreen* screen = CWsTop::Screen();
			WS_ASSERT_ALWAYS(screen, EWsPanicNoScreen);
			if(CDebugBar* dbg = screen->DebugBar())
				dbg->OnKeyEvent();
			if (wsDebugLog)
				wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVDebugLogKeyDownArrival,aRawEvent.ScanCode());
			CKeyboardRepeat::KeyDown();
			TKeyData keyData;
			// Note iCaptureKeys is needed as dummy arg only. Key capture is
			// now handled in ProcessKeyPress().
			TBool translated=iKeyTranslator->TranslateKey(aRawEvent.ScanCode(), EFalse,*iCaptureKeys,keyData);
			ProcessModifierChanges();
			QueueKeyUpDown(aRawEvent);
			if (translated)
				{
				TKeyEvent keyEvent;
				keyEvent.iScanCode = aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE;
				keyEvent.iCode = keyData.iKeyCode;
				keyEvent.iModifiers = keyData.iModifiers;
				ProcessKeyPress(keyEvent, ETrue, 0);
				}
			}
			break;
		case TRawEvent::EKeyUp:
			{
#ifdef LOG_WSERV_EVENTS
			RDebug::Printf("_WSEVENT_KEY: TRawEvent::EKeyUp");
#endif
			_LIT(KWSERVDebugLogKeyUpArrival,"Key up arrives %d");
			CScreen* screen = CWsTop::Screen();
			WS_ASSERT_ALWAYS(screen, EWsPanicNoScreen);
			if(CDebugBar* dbg = screen->DebugBar())
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
				TKeyEvent keyEvent;
				keyEvent.iScanCode = aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE;
				keyEvent.iCode = keyData.iKeyCode;
				keyEvent.iModifiers = keyData.iModifiers;
				ProcessKeyPress(keyEvent, EFalse, 0);
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
		case TRawEvent::EPointer3DOutOfRange:
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
			TKeyEvent keyEvent;
			keyEvent.iScanCode = aRawEvent.ScanCode() __REMOVE_WINS_CHARCODE;
			keyEvent.iCode = aRawEvent.ScanCode();
 			keyEvent.iModifiers = iKeyTranslator->GetModifierState();
			ProcessKeyPress(keyEvent, EFalse, aRawEvent.Repeats());
 			}
 			break;
		default:
			break;
		}
#ifdef LOG_WSERV_EVENTS
    RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::ProcessRawEvent Number= %d  State = %x XY(%d,%d)",TWsPointer::iPointers[0].iNumber,TWsPointer::iPointers[0].iState,TWsPointer::iPointers[0].iPos.iX,TWsPointer::iPointers[0].iPos.iY);
    RDebug::Printf("_WSEVENT_POINTER: TWindowServerEvent::ProcessRawEvent Number= %d  State = %x XY(%d,%d)",TWsPointer::iPointers[1].iNumber,TWsPointer::iPointers[1].iState,TWsPointer::iPointers[1].iPos.iX,TWsPointer::iPointers[1].iPos.iY);
#endif
	}

void TWindowServerEvent::ProcessKeyEvent(const TKeyEvent &aKeyEvent,TInt aRepeats)
	{
#ifdef LOG_WSERV_EVENTS
    RDebug::Printf("_WSEVENT_KEY: TWindowServerEvent::ProcessKeyEvent, key code: %d, repeat: %d", aKeyEvent.iCode, aRepeats);
#endif
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
	ProcessKeyPress(aKeyEvent, aRepeats == 0, aRepeats);
	}

void TWindowServerEvent::AddCaptureKeyL(const TKeyCaptureRequest& aRequest)
	{
	iKeyEventRouter->AddCaptureKeyL(aRequest);
	}

void TWindowServerEvent::UpdateCaptureKeyL(const TKeyCaptureRequest& aRequest)
	{
	iKeyEventRouter->UpdateCaptureKeyL(aRequest);
	}

void TWindowServerEvent::CancelCaptureKey(TKeyCaptureType aType, TAny* aHandle)
	{
	iKeyEventRouter->CancelCaptureKey(aType, aHandle);
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
			eventHandlerCount -= 1;			//Update the counters
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
#ifdef LOG_WSERV_EVENTS
    RDebug::Printf("_WSEVENT_KEY: CRawEventReceiver::RunL Entry point for event receiver");
#endif
//__PROFILE_START(11);
	if (TWsPointer::PreProcessDriverEvent(iEventBuf.Event()
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

/**
Process timer events.

Called when the key repeat timer expires, this function generates the
appropriate long key or repeated key event. If the timer was started for
normal key repeat or if the long key event was captured with the automatic
repeat option specified then the timer is restarted.
*/
void CKeyboardRepeat::RunL()
	{
	User::ResetInactivityTime();
	WS_ASSERT_DEBUG(iRepeating != ERepeatNone, EWsPanicKeyRepeat);
	TBool timer=ETrue;
	if (iRepeating>=ERepeatLong)
		{
		// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong
		WS_ASSERT_DEBUG(iLongCapture != NULL, EWsPanicKeyRepeat);
		if (iLongCapture)
			{
			iCurrentRepeat = iLongRepeat;
			timer = iLongCapture->iFlags & ELongCaptureRepeatEvents;
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

	TWindowServerEvent::QueueKeyPress(iCurrentRepeat.iOutput, ETrue, 1);
	}

/**
Start key repeat and long key press timer

@param	aInputScanCode	Original scan code (before routing)
@param	aShortEvent		Short key event (routing plug-in output)
@param	aLongEvent		Pointer to long key event (routing plug-in output)
						or NULL if none.

Note: When aLongEvent != NULL, iCurrentRepeat reflects the short key event
until the timer has expired. This is necessary to allow a delayed short key
event to be delivered by KeyUp(). CancelRepeat() must therefore examine
iCurrentRepeat or iLongRepeat according to the repeat type in iRepeat.
*/
void CKeyboardRepeat::StartRepeat(TInt aInputScanCode, const TKeyEventRouterOutput& aShortEvent, const TKeyEventRouterOutput* aLongEvent)
	{
	TTimeIntervalMicroSeconds32 time;
	iCurrentRepeat.iInputScanCode = aInputScanCode;
	iCurrentRepeat.iOutput = aShortEvent;

	if (aLongEvent)
		{
		iRepeating = ERepeatLong;
		iLongRepeat.iInputScanCode = aInputScanCode;
		iLongRepeat.iOutput = *aLongEvent;
		iLongCapture = static_cast<CWsCaptureLongKey*>(aLongEvent->iCaptureHandle);
		time = iLongCapture->iDelay;
		}
	else
		{
		iLongCapture = NULL;
		iRepeating=ERepeatNormal;
		time=iInitialTime;
		}
	iThis->After(time);
	}

/**
Cancel key repeat processing
*/
void CKeyboardRepeat::doCancelRepeat()
	{
	iRepeating=ERepeatNone;
	iThis->Cancel();
	}

/**
Cancel any key repeat associated with the specified window group

@param	aRepeatFocus	Destination window group or NULL for all
*/
void CKeyboardRepeat::CancelRepeat(CWsWindowGroup *aRepeatFocus)
	{
	if (iRepeating != ERepeatNone)
		{
		if (aRepeatFocus == NULL ||
			(iRepeating == ERepeatNormal) && (aRepeatFocus == iCurrentRepeat.iOutput.iWindowGroup) ||
			(iRepeating >= ERepeatLong) && (aRepeatFocus == iLongRepeat.iOutput.iWindowGroup))
			{
			doCancelRepeat();
			iAlternateRepeatExists=EFalse;
			}
		}
	}

/**
Cancel any key repeat associated with the specified capture handle

@param	aCaptureHandle		Handle to capture request
@param	aLongCaptureFlag	ETrue for long key capture, EFalse for normal key
*/
void CKeyboardRepeat::CancelRepeat(const TAny* aCaptureHandle, TBool aLongCaptureFlag)
	{
	if (aLongCaptureFlag)
		{
		// Cancel repeat for long capture key
		if (iRepeating >= ERepeatLong && aCaptureHandle == iLongRepeat.iOutput.iCaptureHandle)
			{
			doCancelRepeat();
			iAlternateRepeatExists=EFalse;
			}
		}
	else
		{
		// Cancel repeat for normal capture key
		if (iRepeating == ERepeatNormal && aCaptureHandle == iCurrentRepeat.iOutput.iCaptureHandle)
			{
			doCancelRepeat();
			iAlternateRepeatExists=EFalse;
			}
		}
	}
	
/**
Process a key down event during key repeat.
The current repeat data is saved for possible restoration after rollover.
*/
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

/**
Process a key up event during key repeat.
Send delayed short key event if necessary for long key event processing.
Switch to alternate repeat if rollover key was released.

@param	aScanCode	Scan code
*/
void CKeyboardRepeat::KeyUp(TInt aScanCode)
	{
	if (iAlternateRepeatExists && iAlternateRepeat.iInputScanCode == aScanCode)
		iAlternateRepeatExists=EFalse;
	if (iRepeating != ERepeatNone && iCurrentRepeat.iInputScanCode == aScanCode)
		{
		if (iRepeating==ERepeatLong)
			{
			// Defensive programming - iLongCapture should never be NULL if iRepeating >= ERepeatLong			
			WS_ASSERT_DEBUG(iLongCapture != NULL, EWsPanicKeyRepeat);
			if (iLongCapture && !(iLongCapture->iFlags & ELongCaptureShortEventImmediately))
				{
				TWindowServerEvent::QueueKeyPress(iCurrentRepeat.iOutput, EFalse, 0);
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

//
//CEventQueueRetry//
//
CEventQueueRetry* CEventQueueRetry::NewL()
	{
	CEventQueueRetry* self = new (ELeave) CEventQueueRetry;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
      
void CEventQueueRetry::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}
 
CEventQueueRetry::CEventQueueRetry() : CActive(EPriorityStandard), iRetrySpinner(1)
	{
	
	}

CEventQueueRetry::~CEventQueueRetry()
	{
	Cancel();
	iTimer.Close();
	iClientArray.Close();
	}

void CEventQueueRetry::DoCancel()
	{
	iTimer.Cancel();
	iRetrySpinner = 0;
	iClientArray.Reset();
	}

void CEventQueueRetry::RunL()
	{
	//some clients may be no longer interested in listening
	//so we need to refresh the client list each round of retry
	iClientArray.Reset();
	TInt err = iOwner->GetNotificationClients(iClientArray);
	if(err != KErrNone)
		{
		iClientArray.Reset(); //so the retry won't kick off
		}
	if(iClientArray.Count() > 0)
		{
		TBool eventOnAllQueues = ETrue;
		for(TInt i = 0; i < iClientArray.Count(); i++)
			{
			if(iClientArray[i]->RetryEvent(EEventDisplayChanged))
				{
				if(TWindowServerEvent::SendDisplayChangedEvents(iClientArray[i], iOwner->ScreenNumber(),
						iOwner->ConfigSpinner(), iOwner->DisplaySpinner()))
					{
					iClientArray[i]->RemoveRetryFlag(EEventDisplayChanged);
					}
				else
					{
					eventOnAllQueues = EFalse;
					}
				}
			}
		
		if(!eventOnAllQueues)
			{//another retry needed, but with increased time interval
			iRetrySpinner++;
			Retry(KRetryInitialDelay*iRetrySpinner);
			}
		}
	
	}

void CEventQueueRetry::Retry(TInt aDelay)
	{
	//the retry might be infinite, with an increasing interval, as delivery of the event is garuanteed
	//if aDelay is greater than max TInt, it will be negative number so we reset it to KRetryInitialDelay
	if(aDelay < 0)
		{
		aDelay = KRetryInitialDelay;
		}
	iTimer.After(iStatus, aDelay);
	SetActive();
	}

void CEventQueueRetry::Init(CScreen *aOwner)
	{
	iOwner = aOwner;
	iRetrySpinner = 0;
	
	}

void CEventQueueRetry::CancelRetry()
	{
	Cancel();
	}
