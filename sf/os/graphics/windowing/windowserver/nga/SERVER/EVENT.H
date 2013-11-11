// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of classes related event handling
// 
//


#ifndef __EVENT_H__
#define __EVENT_H__

#include "EVQUEUE.H"
#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>
#include "W32STD.H"
#include "w32cmd.h"
#include "EVQUEUE.H"
#include <w32adll.h>
#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/wskeyrouter.h>

class CWsClient;
class CWsWindowBase;
class CWsWindowGroup;
class MEventHandler;
class CWsHotKey;
class CWsCaptureLongKey;
class CScreen;
class DWsScreenDevice;

const TInt KRetryInitialDelay = 100000; //0.01 second

struct TEventRequestItem
	{
	const CWsWindowBase *iWindow;
	TInt iParam;
	TEventControl iCircumstances;
	TSglQueLink iQue;
	};

class TEventRequestQueue
	{
public:
	TEventRequestQueue();
	TEventRequestItem *FindInEventRequestQueueList(const CWsWindowBase &aWindow);
	void AddToEventRequestListL(const CWsWindowBase &aWindow, TInt aParam, TEventControl aCircumstances);
	void RemoveFromEventRequestListL(const CWsWindowBase &aWindow);
	inline TSglQue<TEventRequestItem> &Queue();
private:
	TSglQue<TEventRequestItem> iQueue;
	};

class CRawEventReceiver : public CActive
	{
public:
	CRawEventReceiver(TInt aPriority);
	~CRawEventReceiver();
	void ConstructL();
	inline TBool IsReadyToRun() const {return IsActive() && iStatus!=KRequestPending;}
protected:
	void Request();
	void DoCancel();
	void RunL();
private:
	TRawEventBuf iEventBuf;
	};

class TRepeatKey
	{
public:
	TKeyEventRouterOutput iOutput;
	TInt iInputScanCode;
	};

class CKeyboardRepeat : public CTimer
	{
public:
	CKeyboardRepeat();
	static void NewL();
	static void Destroy();
	static void KeyDown();
	static void KeyUp(TInt aScanCode);
	static void StartRepeat(TInt aInputScanCode, const TKeyEventRouterOutput& aShortEvent, const TKeyEventRouterOutput* aLongEvent);
	static void CancelRepeat(CWsWindowGroup *aRepeatFocus);
	static void CancelRepeat(const TAny* aCaptureHandle, TBool aLongCaptureFlag);
	static void SetRepeatTime(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime);
	static void GetRepeatTime(TTimeIntervalMicroSeconds32 &aInitialTime, TTimeIntervalMicroSeconds32 &aTime);
	static inline TBool IsAreadyActive();
private:
	enum TRepeatType
		{
		ERepeatNone,
		ERepeatNormal,
		ERepeatLong,
		ERepeatLongRepeated,
		};
private:
	static void doCancelRepeat();
	void RunL();
private:
	static CKeyboardRepeat *iThis; // Needed as CTimer derived parts can't be static
	static TTimeIntervalMicroSeconds32 iInitialTime;
	static TTimeIntervalMicroSeconds32 iTime;
	static TRepeatType iRepeating;
	static TRepeatKey iCurrentRepeat;
	static TRepeatKey iAlternateRepeat;
	static TRepeatKey iLongRepeat;
	static TBool iAlternateRepeatExists;
	static CWsCaptureLongKey* iLongCapture;
	static TInt iRepeatRollover;
	};

struct SNotificationHandler {CAnim* iAnim; TUint32 iNotifications;};
struct TDrawerHandler
	{
	TDrawerHandler(CWsGraphicDrawer *aDrawer, TUint32 aEvents): iDrawer(aDrawer), iEvents(aEvents) {}
	CWsGraphicDrawer* iDrawer;
	TUint32 iEvents;
	};

struct TWsEventHandler
	{
	TWsEventHandler(MWsEventHandler *aHandler, TUint32 aEvents): iHandler(aHandler), iEvents(aEvents) {}
	static TBool CompareHandler(const TWsEventHandler& lhs, const TWsEventHandler& rhs) { return lhs.iHandler == rhs.iHandler; }
	MWsEventHandler* iHandler;
	TUint32 iEvents;
	};

class CEventQueueRetry : public CActive
	{
public:
	static CEventQueueRetry* NewL();
	~CEventQueueRetry();
	void Init(CScreen *aOwner);
	void Retry(TInt aDelay);
	void CancelRetry();
private:
	CEventQueueRetry();
	void ConstructL();
	// From CActive:
	void RunL();
	void DoCancel();
	
private:
	RTimer iTimer;
	CScreen *iOwner;
	TInt iRetrySpinner;
	RPointerArray<CWsClient> iClientArray;
	};

class TWindowServerEvent
	{
	#define EDefaultInitialRepeatTime TTimeIntervalMicroSeconds32(300000)
	#define EDefaultRepeatTime TTimeIntervalMicroSeconds32(100000)

	typedef void (*TSendEventFunc)(TEventRequestItem *aQptr, TInt aParam1, TInt aParam2);
public:
	enum {ENumHotKeys=21};
	enum {EOomEventSecondGap=150};	// Don't resend OOM messages unless at least 150 seconds has passed
	enum {ERemovedEventHandlerWhileProcessingRawEvents=0x02};
public:
	static void InitStaticsL();
	static void DeleteStatics();
	static CWsHotKey* ClearHotKeysL(TInt aHotKey);
	static void ResetDefaultHotKeyL(TInt aHotKey);
	static void SetHotKeyL(const TWsClCmdSetHotKey &aHotKey);
	static void AddCaptureKeyL(const TKeyCaptureRequest& aRequest);
	static void UpdateCaptureKeyL(const TKeyCaptureRequest& aRequest);
	static void CancelCaptureKey(TKeyCaptureType aType, TAny* aHandle);
	static void ClientDestroyed(CWsClient *aClient);
	static inline void AddToSwitchOnEventListL(const CWsWindowBase &aWindow, TEventControl aCircumstances);
	static inline void RemoveFromSwitchOnEventList(const CWsWindowBase &aWindow);
	static inline void AddToErrorMessageListL(const CWsWindowBase &aWindow, TEventControl aCircumstances);
	static inline void RemoveFromErrorMessageList(const CWsWindowBase &aWindow);
	static inline void AddToModifierChangedEventListL(const CWsWindowBase &aWindow, TInt aModifierMask, TEventControl aCircumstances);
	static inline void RemoveFromModifierChangedEventList(const CWsWindowBase &aWindow);
	static inline void AddToGroupChangeEventListL(const CWsWindowBase &aWindow);
	static inline void RemoveFromGroupChangeEventEventList(const CWsWindowBase &aWindow);
	static inline void AddToFocusChangeEventListL(const CWsWindowBase &aWindow);
	static inline void RemoveFromFocusChangeEventEventList(const CWsWindowBase &aWindow);
	static inline void AddToGroupListChangeEventListL(const CWsWindowBase &aWindow);
	static inline void RemoveFromGroupListChangeEventEventList(const CWsWindowBase &aWindow);
	static inline void AddToScreenDeviceChangeEventListL(const CWsWindowBase &aWindow);
	static inline void RemoveFromScreenDeviceChangeEventList(const CWsWindowBase &aWindow);
	static TInt GetModifierState();
	static inline TInt GetStoredModifierState();
	static void SetModifierState(TEventModifier aModifier,TModifierState aState);
	static void ProcessRawEvent(const TRawEvent& aRawEvent);
	static void ProcessKeyEvent(const TKeyEvent& aKeyEvent,TInt aRepeats);
	static TBool MousePress(const TRawEvent &aRawEvent, const CWsWindowGroup *aGroupWin);
	static void SendGroupChangedEvents();
	static void SendFocusChangedEvents();
	static void SendGroupListChangedEvents();
	static void SendVisibilityChangedEvents(CWsWindowBase* aWin, TUint aFlags);
	static TBool SendDisplayChangedEvents(CWsClient *aWsClient, TInt aDisplayNumber, TInt aConfigurationChangeId, TInt aResolutionListChangeId);
	static void SendScreenDeviceChangedEvents(CScreen* aScreen);
	static void SendScreenDeviceChangedEvent(const CWsWindowBase *aWindow);
	static TBool ProcessErrorMessages(TWsErrorMessage::TErrorCategory aCategory, TInt aError);
	static void NotifyOom();
	static void ProcessKeyPress(const TKeyEvent& aKeyEvent, TBool aCheckRepeat,TInt aRepeats);
	static void QueueKeyPress(const TKeyEventRouterOutput& aOutput, TBool aIsRepeat, TInt aRepeats);
	static void AddEventHandler(MEventHandler *aEventHandler, TBool aAdvancedPointersEnabled);
	static void RemoveEventHandler(const MEventHandler *aEventHandler);
	static void PotentialEventHandlerL(TInt aNum);
	static TInt AddNotificationHandler(CAnim* aAnim, TUint32 aNotifications);
	static void RemoveNotificationHandler(CAnim* aAnim);
	static void PublishNotification(const TWsEvent& aWsEvent);
	static TInt RegisterDrawerHandler(CWsGraphicDrawer* aDrawer, TUint32 aEvents);
	static TInt UnregisterDrawerHandler(CWsGraphicDrawer* aDrawer);
	static TInt RegisterWsEventHandler(MWsEventHandler * aHandler, TUint32 aEvents);
	static TInt UnregisterWsEventHandler(MWsEventHandler * aHandler);
	static void NotifyDrawer(const TWservCrEvent& aEvent);
	static void NotifyScreenDrawingEvent(const TRegion* aRegion);
	static void NotifyScreenDrawingEvent(const TRect& aRect);
	static inline const CRawEventReceiver* EventReceiver() {return iEventReceiver;}
private:
	class TRawEventHandler
		{
		public:
			inline TRawEventHandler(MEventHandler *aEventHandler, TBool aAdvancedPointersEnabled);
		public:
			MEventHandler *iEventHandler;
			TBool iAdvancedPointersEnabled;
		};
	static void ProcessEventQueue(TEventRequestQueue &aQueue, TSendEventFunc aFunc, TInt aParam1, TInt aParam2);
	static void DeleteHotKeys();
	static void QueueKeyEvent(CWsWindowGroup *aWin, TWsEvent &aEvent, TWservEventPriorities aPriority);
	static void QueueKeyUpDown(const TRawEvent &aRawEvent);
	static void ConstructDefaultHotKeyL(TInt aHotKey, const TWsWinCmdCaptureKey &aSystemKey);
	static void ProcessModifierChanges();
	static void LinkHotKey(CWsHotKey *aWsHotKey);
	static TBool DrawerCompareFunc(const TDrawerHandler& lhs, const TDrawerHandler& rhs);
private:
	static CKeyTranslator *iKeyTranslator;
	static TEventRequestQueue iSwitchOnQueue;
	static TEventRequestQueue iErrorMessageQueue;
	static TEventRequestQueue iModifierChangedQueue;
	static TEventRequestQueue iGroupChangedQueue;
	static TEventRequestQueue iFocusChangedQueue;
	static TEventRequestQueue iGroupListChangedQueue;
	static TEventRequestQueue iScreenDeviceChangedQueue;
	static TTime iPrevOomMessageTime;
	static CCaptureKeys *iCaptureKeys;
	static CKeyEventRouter *iKeyEventRouter;
	static RLibrary iKeyEventRouterLibrary;
	static CWsHotKey *iHotKeys;
	static TInt iModifierState;
	static CRawEventReceiver *iEventReceiver;
	static RArray<TRawEventHandler> iEventHandlers;
	static CArrayFixFlat<SNotificationHandler> *iNotificationHandlers;
	static TInt iPotentialEventHandlers;
	static RArray<TDrawerHandler>* iDrawerHandlers;
	static RArray<TWsEventHandler> iWsEventHandlers;
	static TUint32 iBinaryFlags;
	static TInt iEventHandlerCount;
	};


//
// inlines			//
//

inline TBool CKeyboardRepeat::IsAreadyActive()
	{
	return iThis->IsActive();
	}

inline TWindowServerEvent::TRawEventHandler::TRawEventHandler(MEventHandler *aEventHandler, TBool aAdvancedPointersEnabled)
: iEventHandler(aEventHandler), iAdvancedPointersEnabled(aAdvancedPointersEnabled)
	{ }

inline void TWindowServerEvent::AddToSwitchOnEventListL(const CWsWindowBase &aWindow, TEventControl aCircumstances)
	{iSwitchOnQueue.AddToEventRequestListL(aWindow, 0, aCircumstances);}
inline void TWindowServerEvent::AddToErrorMessageListL(const CWsWindowBase &aWindow, TEventControl aCircumstances)
	{iErrorMessageQueue.AddToEventRequestListL(aWindow, 0, aCircumstances);}
inline void TWindowServerEvent::AddToModifierChangedEventListL(const CWsWindowBase &aWindow, TInt aModifierMask, TEventControl aCircumstances)
	{iModifierChangedQueue.AddToEventRequestListL(aWindow, aModifierMask, aCircumstances);}
inline void TWindowServerEvent::AddToGroupChangeEventListL(const CWsWindowBase &aWindow)
	{iGroupChangedQueue.AddToEventRequestListL(aWindow, 0, EEventControlAlways);}
inline void TWindowServerEvent::AddToFocusChangeEventListL(const CWsWindowBase &aWindow)
	{iFocusChangedQueue.AddToEventRequestListL(aWindow, 0, EEventControlAlways);}
inline void TWindowServerEvent::AddToGroupListChangeEventListL(const CWsWindowBase &aWindow)
	{iGroupListChangedQueue.AddToEventRequestListL(aWindow, 0, EEventControlAlways);}
inline void TWindowServerEvent::AddToScreenDeviceChangeEventListL(const CWsWindowBase &aWindow)
	{iScreenDeviceChangedQueue.AddToEventRequestListL(aWindow, 0, EEventControlAlways);}

inline void TWindowServerEvent::RemoveFromSwitchOnEventList(const CWsWindowBase &aWindow)
	{iSwitchOnQueue.RemoveFromEventRequestListL(aWindow);}
inline void TWindowServerEvent::RemoveFromErrorMessageList(const CWsWindowBase &aWindow)
	{iErrorMessageQueue.RemoveFromEventRequestListL(aWindow);}
inline void TWindowServerEvent::RemoveFromModifierChangedEventList(const CWsWindowBase &aWindow)
	{iModifierChangedQueue.RemoveFromEventRequestListL(aWindow);}
inline void TWindowServerEvent::RemoveFromGroupChangeEventEventList(const CWsWindowBase &aWindow)
	{iGroupChangedQueue.RemoveFromEventRequestListL(aWindow);}
inline void TWindowServerEvent::RemoveFromFocusChangeEventEventList(const CWsWindowBase &aWindow)
	{iFocusChangedQueue.RemoveFromEventRequestListL(aWindow);}
inline void TWindowServerEvent::RemoveFromGroupListChangeEventEventList(const CWsWindowBase &aWindow)
	{iGroupListChangedQueue.RemoveFromEventRequestListL(aWindow);}
inline void TWindowServerEvent::RemoveFromScreenDeviceChangeEventList(const CWsWindowBase &aWindow)
	{iScreenDeviceChangedQueue.RemoveFromEventRequestListL(aWindow);}
inline TInt TWindowServerEvent::GetStoredModifierState()
	{return(iModifierState);}

#endif
