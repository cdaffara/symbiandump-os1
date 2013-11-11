// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __TEVENT_H__
#define __TEVENT_H__

#include <e32std.h>
#include <e32cmn.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "AUTODLL.H"
#include "../TClick/CLICK.H"
#include "TGraphicsHarness.h"
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA 
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA 

const TInt EMaxEventQueueSize=32;
const TInt EMinQueueSize = 2;
const TUint KModifiersIgnore = 0xffffffff;


class CTEventTest;
class CTQueueClient;

class TWsQTEvent : public TWsEvent
	{
	friend class CTEventTest;
public:
	inline TTime& Time() {return iTime;}
	};

class CTQueueTestEvent : public CTEvent
	{
	enum {EEventBufferSize=40};
public:
	CTQueueTestEvent(RWsSession *aWs, CTEventTest *aTest);
	void ConstructL();
	void AddExpectedEvent(TWsEvent &aEvent);
	inline void ResetCount(TInt aSet) {iEventCount=0;iEventSet=aSet;iLastEventError=EFalse;}
	TInt EventsLeft();
	inline void SetCheckTimeStamp(TBool aCheck) {iCheckTimeStamp = aCheck;}
protected:
	void doRunL();
private:
	void TestL(TInt aTest);
private:
	CCirBuf<TWsEvent> iEventBuffer;
	CTEventTest *iTest;
	TInt iEventCount;
	TInt iEventSet;
	TBool iCheckTimeStamp;
	TPartialRedrawType iRedrawType;
	TBool iLastEventError;
	};

const TInt EWinPositionX=6;
const TInt EWinPositionY=9;
const TInt EWinWidth=100;
const TInt EWinHeight=50;
// (6,9)
// +----------------------+
// |   (31,21)            |
// |   +--------------+   |
// |   |              |   |
// |   |              |   |
// |   +--------------+   |
// |            (81,46)   |
// +----------------------+ (106,59)

class CTQueueClient : public CTClient
	{
private:
	enum {EMaxKeysDown=50};
public:
	CTQueueClient(CTEventTest *aTest);
	~CTQueueClient();
	void ConstructL(TBool aCreateChild);
	void ConstructEventHandlerL();
	void AddExpectedEvent(TWsEvent &aEvent);
	TInt WindowGroupIdentifier();
	CTWinBase* GetHandle(TPoint& aPos);
	void SetChildCapture(TInt aCaptureFlags);
	inline void ResetCount(TInt aSet) {EventQueue()->ResetCount(aSet);}
	inline void EnableFocusChangeEvents() {iGroup->GroupWin()->EnableFocusChangeEvents();}
	inline void DisableFocusChangeEvents() {iGroup->GroupWin()->DisableFocusChangeEvents();}
	inline void EnableGroupListChangeEvents() {iGroup->GroupWin()->EnableGroupListChangeEvents();}
	inline void DisableGroupListChangeEvents() {iGroup->GroupWin()->DisableGroupListChangeEvents();}
	inline CTQueueTestEvent* EventQueue() {return STATIC_CAST(CTQueueTestEvent*,iEventHandler);}
	inline TPoint ChildTopLeft() {return TPoint(EWinWidth/4,EWinHeight/4);}
	inline TSize ChildSize() {return TSize(EWinWidth/2,EWinHeight/2);}
	void CreateEventTestGroupOnSecondScreenL();
	void CloseEventTestGroupOnSecondScreen();
private:
	inline TPoint ParentTopLeft() {return TPoint(EWinPositionX,EWinPositionY);}
	inline TPoint ChildAbsTopLeft() {return ParentTopLeft()+ChildTopLeft();}
private:
	TInt iKeysDown[EMaxKeysDown];
	TInt iNumKeysDown;
	CTEventTest *iTest;
public:
	CTBackedUpWin *iChildWin;
	CTBackedUpWin *iWin;
	TBool iChildCapturing;
	TBool iCapturing;
	CWsScreenDevice* iSndScreen;
	RWindowGroup* iWinGp1;
	RWindowGroup* iWinGp2;
	};

class REventAnimBase : public RTestAnim
	{
public:
	enum EAnimationType
		{
		EEventAnim,
		EEventPostingAnim
		};
	REventAnimBase(RAnimDll* aDll) :RTestAnim(*aDll) {}
	virtual void Destroy();
	void Reset();
	TInt ErrorCode();
	TInt NumberOfEventsReceived();
	TInt AddExpectedEvent(TBool aEat, const TRawEvent& aEvent);
	TInt AddExpectedEvent(const TAnimRawEvent& aEvent);	
	static RAnimDll* LoadL();
	};

/**
 Client side handle to server-side anim class used for Anim Event tests
 @internalComponent
 @test
 */
class REventAnim : public REventAnimBase
	{
public:
	static REventAnim* NewL(RWindowBase* aWin, RAnimDll* aDll, REventAnimBase::EAnimationType aType=EEventAnim);
	inline REventAnim(RAnimDll* aDll) : REventAnimBase(aDll) {}
	inline TInt AddEvent(const TAnimRawEvent& aEvent) {return REventAnimBase::AddExpectedEvent(aEvent);};
	inline TInt GetError() {return REventAnimBase::ErrorCode();};
	inline TInt GetNumEvents() {return REventAnimBase::NumberOfEventsReceived();};
	TInt TestFail(TInt aNoEvents);
	};

/**
 Client side handle to server-side anim class used for Event Handler Removal tests
 
 RRemovableAnim is an anim which removes itself from the event handler list 
 after receiving n events, where n is the 'lifetime' of the anim measured in events received.
 
 @internalComponent
 @test
 */
class RRemovableAnim : public REventAnimBase
	{
public:
	static RRemovableAnim* NewLC(RWindowBase* aWin, RAnimDll* aDll);
	inline RRemovableAnim(RAnimDll* aDll) : REventAnimBase(aDll) {}
	inline TInt AddExpectedEvent(const TRawEvent& aEvent) {return REventAnimBase::AddExpectedEvent(EFalse, aEvent);};
	TInt SetEventHandlerLifetime(TInt aEventCount);
	TInt EventHandlerLifetime();
	TInt AddToEventHandlers();
	TInt RemoveFromEventHandlers();
	TBool TestPassed();
	};

class CTEventTest : public CTWsGraphicsBase
	{
	enum {
		EKeyboardInitialRepeat1=1234567,
		EKeyboardRepeat1=555666,
		EKeyboardInitialRepeat2=2121212,
		EKeyboardRepeat2=20000,
		EDoubleClickTime1=900000,
		EDoubleClickGap1=10,
		EDoubleClickTime2=4000000,
		EDoubleClickGap2=5,
		ENumVisWins=2,
		EPointerCursorUpdateGap=50000,		//0.05secs Same as WsPointer::EPointerUpdateGapInMicroSeconds
		EKeyboardInitialRepeat3=2147483447, // Max value
		EKeyboardRepeat3=8000, // for repeat 3
		EKeyboardInitialRepeat4=-2, // Min value
		EKeyboardRepeat4=6000, // for repeat 4
		EKeyboardRepeat5=7000, // for repeat 4z
		EKeyboardRepeat6=-30000, // for repeat 4
		EKeyboardRepeat7=2147483447, // for repeat 4
		};
public:
	CTEventTest(CTestStep* aStep);
	~CTEventTest();
	//TestState DoTestL();
	void ConstructL();
	void General();
	void General2();
	void Failed();
	//void QueuePurgingL();
	//void SendEventL();
	//void InvisibleWindowL();
	//void JavaAdditionsL();
	void WindowCapBugL();
	//void XyInputTypeL();
	//void MovingPointerCursorL();
	//void RotatedModeL();
	//void AnimEvent();
	void CheckTimeInterval(const TTimeIntervalMicroSeconds32 &aTime,TInt EKeyboardRepeat1);
	void KeyRepeatRate(TInt aInitialRepeat, TInt aRepeat);
	void KeyRepeatRateNegTest(TInt aInitialRepeat, TInt aRepeat);
	void DoubleClickSettings(TInt aInterval, TInt aDistance);
	void NextSetOfEventsL();
	inline void ResetCount(TInt aSet) {iQueueClient->ResetCount(aSet);}
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	enum
		{
		EDoRot0  =0x01,
		EDoRot90 =0x02,
		EDoRot180=0x04,
		EDoScale=0x80,
		};
	enum TClients
		{
		ETheClient,
		EQueueClient,
		};
	enum TCaptureKeyApis
		{
		ECaptureKey1,
		ECaptureKey2,
		ECaptureKeyUpAndDowns1,
		ECaptureKeyUpAndDowns2,
		ECaptureLongKey1,
		ECaptureLongKey2,
		// Add additional enum values before this line
		EMaxCaptureKeyApis 
		};
	enum TCancelCaptureKeyApis
		{
		ECancelCaptureKey,		
		ECancelCaptureKeyUpAndDowns,		
		ECancelCaptureLongKey,
		// Add additional enum values before this line
		EMaxCancelCaptureKeyApis
		};
	enum TLongkeyCapPriorityTests
		{
		ELongkeyCaptureSamePriority,
		ELongkeyCaptureDiffPriority
		};
private:
	void RunTestsL(TBool aNeedChildWindows=EFalse);
	void RunTestsRestoreAreaL(TBool aNeedChildWindows);
	void QueuePurge_NextSetOfEventsL();
	void InitializeQueueSizeTestL(TBool aEventReadyCancel);
	void SendEventTest_NextSetOfEvents();
	void InvisibleWindow_NextSetOfEvents();
	void JavaAdditions_NextSetOfEvents();
	void XyInputType_NextSetOfEvents();
	void MovingPointerCursor_NextSetOfEventsL();
	void WinCap_NextSetOfEvents();
	void RotatedMode_NextSetOfEventsL();
	void ScreenScaling_NextSetOfEventsL();
	void AnimEvent_NextSetOfEventsL();
	void FocusChanged_NextSetOfEventsL();
	void StopEvents_NextSetOfEventsL();
	void VirtualKeyboard_NextSetOfEventsL();
	void KeyClicks_NextSetOfEvents();
	void CaptureLong_NextSetOfEventsL();
	void Password_NextSetOfEvents();
	void GroupListChanged_NextSetOfEventsL();
	void VisibilityChanged_NextSetOfEventsL();
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA  	
	void KeyEventTranslation_NextSetOfEventsL();
	void KeyEventBlocking_NextSetOfEventsL();
	void KeyEventAppRestriction_NextSetOfEventsL();
	void KeyEventAppPriority_NextSetOfEventsL();
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS	
	void SurfaceVisibilityChanged1_NextSetOfEventsL();
	void SurfaceVisibilityChanged2_NextSetOfEventsL();
	void SurfaceVisibilityChanged3_NextSetOfEventsL();
#endif //SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	void CheckTimeStamp_NextSetOfEventsL();
	void RepeatableKeysL();
	void PointerCapture_NextSetOfEventsL();
	void PointerBufferPurge_NextSetOfEventsL();
	void MismatchedCapture_NextSetOfEventsL();	
	void EventHandlerRemoval_NextSetOfEventsL();
	void EventHandlerRemoval_CreateAnimsL(TInt aAnimCount);
	void EventHandlerRemoval_PrepareAnims(const TInt* aLifeTimes, TInt aAnimCount);
	void EventHandlerRemoval_CreateExpectedEvents(TInt aEventCount, TInt aLiveAnimCount);
	void EventHandlerRemoval_AddExpectedEvent(TRawEvent aEvent, TInt aLiveAnimCount);
	void EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
	void EventHandlerRemoval_CheckRecursionOfProcessRawEvent();
	void EventHandlerRemoval_ManuallyRemoveAllAnims();
	void EventHandlerRemoval_ResetAllAnims();
	void EventHandlerRemoval_RemoveAndResetAllAnims();
	void EventHandlerRemoval_DestroyAllAnims();
	void EventHandlerRemoval_AddEventHandlerMultipleTimes();
	void CheckPointerCursorInDifferentScreenMode(TSizeMode aMode,TPoint aOrigin);
	void SimulateAndCheck(TPoint aOrigin,TSize aScale,TInt aXOffset,TInt aYOffset,TPointerEvent::TType aEventType,TRect aPointerCursorArea);
	void AddExpectedKey(TInt aEventCode, TInt aScanCode, TInt aCode=0, TInt aRepeats=0, TUint aModifiers=KModifiersIgnore);
	void AddExpectedKeyDownUp(TInt aScanCode, TInt aCode=0, TInt aRepeats=0, TUint aModifiers=KModifiersIgnore);
	void AddExpectedEvent(TInt aEventCode, CTWinBase* aWin);
	void AddExpectedEvent(TInt aEventCode);
	void AddExpectedEvent(TInt aEventCode,RWindowGroup* aWindow);
	void AddExpectedErrorMessage(TWsErrorMessage::TErrorCategory aCategory, TInt aError);
	void CalculatePointerEvent(TWsEvent& aEvent,TPointerEvent::TType aType, TPoint aPos);
	void CalculatePointerCaptureEvent(TWsEvent& aEvent,TPointerEvent::TType aType, TPoint aPos, TInt aHandle=0);
	void AddExpectedPointer(TPointerEvent::TType aType, TPoint aPos);
	void AddExpectedPointerCapture(TPointerEvent::TType aType, TPoint aPos, TInt aHandle=0);
	void AddExpectedPointerClick(TPointerEvent::TType aType, TPoint aPos);
	void AddExpectedPointerDownUp(TPoint aPos);
	inline void AddExpectedPointerScr(TPointerEvent::TType aType, TPoint aPos);
	inline void AddExpectedPointerDownUpScr(TPoint aPos);
	void AddExpectedModifiersChanged(TInt aChangedModifiers, TInt aModifiers);
	void AddExpectedEnterExit(TInt aEventCode, TPoint aPos);
	void AddExpectedKeyAnim(TRawEvent::TType aEventCode, TInt aScanCode,TBool aEat);
	void AddExpectedPointerAnim(TRawEvent::TType aEventCode, TPoint aPoint, TBool aEat=ETrue);
	void AddExpectedVisibilityChange(TUint aFlags);
	void AddExpectedVisibilityChange(TUint aFlags, CTBlankWindow* aWindow);
	inline void SimulatePointerWin(TRawEvent::TType aType, TInt aX, TInt aY);
	inline void SimulatePointerDownUpWin(TInt aX, TInt aY);
	inline void SimulatePointer(TRawEvent::TType aType, TInt aX, TInt aY);
	void doSendEvent(TWsEvent &aEvent);
	void SendKey(TInt aEventCode, TInt aScanCode, TInt aCode=0, TInt iRepeats=0);
	void SendEvent(TInt aEventCode);
	void CreateEventTestGroup();
	void CloseEventTestGroup();
	void CreateAndCloseEventTestGroup();
	void TriggerOomMessage(TTimeIntervalMinutes aPeriod);
	TBool SetClickType(TClickOutputModes aType);
	CTBlankWindow* NewBlueForgroundWindowL();
	CTBlankWindow* NewBlueForgroundWindow();
	TInt KeyRepeatTime() const;
	void EventQueuePtrCrashTest();
	TInt ClickCommandReply(TInt aCommand,TInt aData);
	TInt CreateGroupWindow(RWindowGroup& aWinGp,TClients aClient,TInt& aWinGpId);
	void CloseGroupWindow(RWindowGroup& aWinGp,TInt aWinGpId);
	void CloseWindow(RWindowBase& aWin,TInt aHandle,TInt aWinGpId);
	void EventQueueExpansionTestL();
	void RawEventRepeatTest_NextSetOfEventsL();
	void SimulateRepeatEvent(TInt aScanCode, TInt aRepeats=0);
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
	void SimulateButton1DownWithSimulatedMove(TInt aX, TInt aY);
#endif
	void TestCaptureAndCancelCapturePair(TCaptureKeyApis aCaptureApi, TCancelCaptureKeyApis aCancelCaptureApi);
	static TInt GenerateAnEvent(TAny* aEventTest);
	void CheckLongkeyCaptureWithPriority(TLongkeyCapPriorityTests aTestType);
	void DelayForRepeatEvents(TInt aNumeratorFracVal, TInt aDenominatorFracVal);
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA 	
	void SpawnCaptureAppL(RProcess& aProcess);
#endif
private:
	CTBlankWindow* iBlankWin;
	RBlankWindow iBackingWindow;
	TSize iPhysicalScreenSize;
	TSize iWinSize;
	RWindowGroup iEventTestGroup;
	CTWindowGroup *iEventTestGroup2;
	CTWindowGroup *iEventTestGroup3;
	CTWindowGroup *iEventTestGroup4;
	TInt iEventSet;
	CTQueueClient *iQueueClient;
	TBool iFailed;
	REventAnim* iAnim;
	TUint iScreenModeTests; //currently contains supported rotation and if screen scaling is possible
	TInt iNumScrModes;
	RSoundPlugIn iClick;
	TBool iAddToClick;
	TInt iCaptureKey;
	TBool iTurnBackOnCapsLock;
	TInt iCurrentMode;
	TSizeMode iSizeMode;
	TBool iTheClientFlush;
	TBool iQuequeClientFlush;
	CTBlankWindow* iVisWins1;
	CTBlankWindow* iVisWins2;
	CTBlankWindow* iVisWins3;
	CTransWindow* iTransWin;
	RAnimDll* iAnimDll;
	RPointerArray<RRemovableAnim>* iRemovableAnims;
	TBool iIs3DPointer;
	TBool iPtrPluginLoaded;
	TInt iYOffset;
	TTimeIntervalMicroSeconds32 iKeyBoardRepeatInitialDelay;
	TTimeIntervalMicroSeconds32 iKeyBoardRepeatNextDelay;
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA 	
	RProcess iCaptureApp1;
	RProcess iCaptureApp2;
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS	
	TSurfaceId iSurfaceId;
	RSurfaceManager iSurfaceManager;
	CPeriodic* iTimeOutCallback;
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	};

class CTQueueWindowGroup : public CTWindowGroup
	{
public:
	CTQueueWindowGroup(CTClient *aClient);
	void ConstructL();
private:
	TInt iNumOnEvents;
	};

class CTEventTestStep : public CTGraphicsStep
	{
public:
	CTEventTestStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTEventTestStep,"TEventTest");


#endif
