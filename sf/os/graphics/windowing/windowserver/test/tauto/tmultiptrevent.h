// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TMULTIPTREVENT_H_
#define TMULTIPTREVENT_H_

#include <e32std.h>
#include <e32cmn.h>
#include "w32std.h"
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "AUTODLL.H"
#include "../TClick/multiptrclick.h"
#include "TGraphicsHarness.h"

class CTMultiPtrEventTest;
class CTMultiPtrEventClient;

/*
 * CTEvent derived class which acts as eventhandler and eventbuffer 
 * Stores the events in buffer, which is used for comparing events received from wserv.
 */ 
class CTMultiPtrEventBuffer : public CTEvent
	{
	enum {EEventBufferSize=40, EMovePtsBuffer=10};
public:
	CTMultiPtrEventBuffer(RWsSession *aWs, CTMultiPtrEventTest *aTest, CTMultiPtrEventClient* aClient);
	void ConstructL();
	void AddExpectedEvent(TWsEvent &aEvent);
	void AddExpectedMovePtEvent(TPoint &aMovePt);
	TInt EventsRemaining();
	void SetEventCount(TInt aCount) {iEventCount = aCount;}
	void SetNestedLoopState(TBool aLoopStart) {iNestedLoopStarted = aLoopStart;}
	TBool NestedLoopState() {return iNestedLoopStarted;}
	void GetMoveBufferAndCompareL();
protected:
	// Pure virtual from CTEventBase
	void doRunL();
private:
	void TestL(TInt aTest);
	void TestL(TInt aTest, TInt aVal1, TInt aVal2,const char *oper, const char *aTestStr, 
	           const TWsEvent *aEvent, const TWsEvent *aExpected, const char *aFile, TUint aLine);
    void TestL(TInt aTest, TPoint aVal1, TPoint aVal2,const char *oper, const char *aTestStr, 
               const TWsEvent *aEvent, const TWsEvent *aExpected, const char *aFile, TUint aLine);
private:
	CCirBuf<TWsEvent> iEventBuffer;
	CCirBuf<TPoint> iMovePtsBuffer;
	CTMultiPtrEventTest* iTest;
	TInt iEventCount;
	CTMultiPtrEventClient* iClient;
	TBool iNestedLoopStarted;
	};


/* 
 * CTClient derived class which gives Wserv client environment 
 * i,e iWs, iScreen, iGc, iGroup, iEventHandler etc...
 * Owns CTMultiPtrEventBuffer and stores in iEventhandler of its base class
 * Accepts the event from test class and passes them on to buffer class
 */ 
class CTMultiPtrEventClient : public CTClient
	{
public:
	CTMultiPtrEventClient(CTMultiPtrEventTest *aTest, TBool aSecondaryClient = EFalse);
	~CTMultiPtrEventClient();
	void ConstructL();
	
	inline CTBlankWindow* ParentWin() { return iParentWin; }
	inline CTBlankWindow* ChildWin() { return iChildWin; }
	inline CTMultiPtrEventBuffer* EventBuffer() {return static_cast<CTMultiPtrEventBuffer*>(iEventHandler);}
	inline void SetExpectNonAdvancedPointerEvents() {iExpectNonAdvancedPointerEvents=ETrue;};
	inline void ClearExpectNonAdvancedPointerEvents() {iExpectNonAdvancedPointerEvents=EFalse;};

	TBool IsSecondaryClient() {return iSecondaryClient;}
	
	void AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TUint8 aPointerNumber, TUint aHandle = 0);
	void AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle = 0);
	void AddExpectedWsEvent(TEventCode aType, TInt aPointerNumber = 0, TUint aHandle = 0);
	void AddExpectedMovePoint(TPoint aPos);
	void CalculatePtrPosAndSet3Ddata(TWsEvent& aEvent, TPointerEvent::TType aType, TPoint aPos, TUint aModifiers, TInt aZ, TUint8 aPointerNumber, TUint aHandle);
	void ConstructGroupBlankWinL();
	void DeleteGroupBlankWin();
	TBool CheckOrdinalPosition(TInt aWinAutoFocus1);
	
	// Virtual from CTClient
	void ConstructEventHandlerL();
	
private:
	CTMultiPtrEventTest* iTest;
	TSize iParentWinSize;
	TPoint iParentWinPos;
	TSize iChildWinSize;
	TPoint iChildWinPos;
	CTBlankWindow* iParentWin;
	CTBlankWindow* iChildWin;

	TBool iSecondaryClient;
	
	// For AutoFocus test
	CTWindowGroup* iGroupWinAutoFocus1;
	CTWindowGroup* iGroupWinAutoFocus2;
	CTBlankWindow* iWinAutoFocus1;
	CTBlankWindow* iWinAutoFocus2;
	
	TSize iWinSizeAutoFocus1;
	TPoint iWinPosAutoFocus1;
	TSize iWinSizeAutoFocus2;
	TPoint iWinPosAutoFocus2;
	TBool iExpectNonAdvancedPointerEvents;
	};

class RMultiPointerAnim : public RTestAnim
	{
public:
	static RMultiPointerAnim* NewL(RWindowBase* aWin, RAnimDll* aDll);
	RMultiPointerAnim(RAnimDll* aDll) :RTestAnim(*aDll) {}
	TInt AddExpectedAnimPointerEvent(TRawEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TBool aEat = ETrue);
	};

/*
 * CTWsGraphicsBase derived class which implements RunTestCaseL pure virtual.
 * Runs all the testcases for multi poniter events.
 * For each test case it creates CTMultiPtrEventClient object and creates a nested activescheduler
 * Calls NextSetOfEventsL() which in turn calls respective tests depending upong the testcase number
 * For each test simulates the events and adds the same event to CTMultiPtrEventBuffer
 * When all the tests for a particular testcase completes stops activescheduler
 * Repeats the same for all testcases
 */ 
class CTMultiPtrEventTest : public CTWsGraphicsBase
	{
public:
	CTMultiPtrEventTest(CTestStep* aStep);
	~CTMultiPtrEventTest();
	void ConstructL();
	void NextSetOfEventsL();
	void Failed();
	
	// Functions for simulating events, which use UserSvr::AddEvent
	void SimulatePointerEvent(TRawEvent::TType aType, TInt aX, TInt aY, TUint8 aPointerNumber);
	void SimulatePointerEvent(TRawEvent::TType aType, TInt aX, TInt aY, TInt aZ, TUint8 aPointerNumber);
	void SimulatePointerDownUp(TInt aX, TInt aY, TInt aZ, TUint8 aPointerNumber);
	
	// Functions for adding event, calls the client's AddExpectedPointer
	void AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TUint8 aPointerNumber, TUint aHandle = 0);
	void AddExpectedPointerEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle = 0, TBool aSecondaryClient = EFalse);
	void AddExpectedPointerDownUp(TPoint aPos, TInt aZ, TUint8 aPointerNumber);
	void AddExpectedWsEvent(TEventCode aType, TBool aSecondaryClient = EFalse, TInt aPointerNumber = 0, TUint aHandle = 0);
	void AddExpectedMultiPtrClickEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle = 0);
	void AddExpectedMultiPtrClickOtherEvent(TPointerEvent::TType aType, TPoint aPos, TInt aZ, TUint aModifier, TUint8 aPointerNumber, TUint aHandle);
	
	void AddExpectedKey(TInt aEventCode, TInt aScanCode, TInt aCode/*=0*/, TInt aRepeats/*=0*/, TUint aModifiers/*=0*/);
	void AddExpectedKeyDownUp(TInt aScanCode, TInt aCode=0, TInt aRepeats=0, TUint aModifiers=0);	
	
	// Supplementary function for tests
	void TestDragForMultiPointer(TPoint aPtrPos, TInt aPrValue, TUint8 aPointerNumber);
	void SimulateAndAddLoneUpEvents(TInt aNumSimulation, TInt aNumAddition, TPoint aPos);
	void TestAndDisplayAnimError();
	void GetRemainingEventsFromSecondaryClient();
	void TestAndDisplayPtrClickError();
	void CreatePointerClickPluginL();
	void SetAutoFlush();
	void ResetFlushStopAS();
	void TestFilteringForMultiPointer(TBool aFiltering);
	void TestGrabbingForMultiPointer(TBool aGrabbing);
	void CreateGroupWinL();
	void DeleteGroupWin();
	void CreateGroupWinForCapturingL();
	void DeleteGroupWinForCapturing();
	void CreateAnimDllAndAnimL();
	void DestroyAnimDllAndAnim();
	void TestCloseProximityHighPressureAPIsL();
	void TestCapturingForMultiPointer(TBool aAllGroups);
	void SetThresholdValues(TInt aEnterCloseProx, TInt aExitCloseProx, TInt aEnterHighPres, TInt aExitHighPres);
	void NegativeTestingOfProximityPressureAPIsL();
	void TestErrorCodeL(TInt aExpErrCode, TInt aActualErrCode);
	void SimulateProximityAndPressureEvents(TInt aCloseProxEnterValue, TInt aCloseProxExitValue, TInt aHighPresEnterValue, TInt aHighPresExitValue);
	void SetDefaultScreenMode();
	TPoint GetPointerPostionOnRotatedMode(TPoint aPointerPos, CFbsBitGc::TGraphicsOrientation aRotation);
	void LogEventsAdded();
	
	// All tests
	void MultiPointerEvents();
	void PurgingMultiPointerEvents();
	void MultiPointerEventsForAnimsL();
	void MultiClientMultiPointerEvents();
	void MultiPointerEventsForPointerClickL();
	void MultiPointerEventsInDiffScreenModes();
	void FilteringMultiPointerEvents();
	void GrabbingMultiPointerEvents();
	void VirtualKeyBoardForMultiplePointers();
	void DoubleClickForMultiplePointers();
	void MultiPointerOutOfRangeEvents();
	void AutoForeGroundForMultiplePointersL();
	void EmulationRulesForMultiplePointers();
    void CloseProximityAndHighPressureEventsL();
	void OutOfBoundValueTesting();
	void CapturingMultiPointerEvents();
	void NegativeTestsForProximityPressureEventsL();
	void PointerRepeatEventForMultiplePointers();
	void SetExpectNonAdvancedPointerEvents(TBool aSecondaryClient=EFalse);
	void ClearExpectNonAdvancedPointerEvents(TBool aSecondaryClient=EFalse);
	
protected:
	void RunTestCaseL(TInt aCurTestCase);
private:
	void RunTestsL();
private:
	TInt iEventSet;			// Set of events for one particular test case
	TBool iFailed;
	TBool iActiveSchedulerStarted;
	TBool iTheClientFlush;
	TBool iClientFlush;
	TBool iSecClientFlush;
	TSize iPhysicalScreenSize;
	TRect iOrigPtrCursorArea;
	
	TUint iPointerNumber;
	TInt iMaxDevPointers;
	TInt iMaxUiPointers;
	TInt iMaxPressure;
	TInt iPressureStep;
	TInt iMaxProximity;
	TInt iProximityStep;

	TInt iEnterCloseProximityThreshold;
	TInt iExitCloseProximityThreshold;
	TInt iEnterHighPressureThreshold;
	TInt iExitHighPressureThreshold;	
	 
	CTMultiPtrEventClient *iMultiPtrEventClient;
	CTMultiPtrEventClient *iSecMultiPtrEventClient;
	
	RAnimDll* iAnimDll;
	RMultiPointerAnim* iAnim;
	
	RSoundPlugIn iClick;
	TBool iPtrPluginLoaded;
	RChunk iChunk;
	RHeap* iHeap;
	TAny* iErrDesc;
	
	TInt iCurrentScreenMode;
	TBool iChangeScreenMode;
	
	CTWindowGroup* iGroupWinForEmul;
	CTBlankWindow* iWinForEmul;
	
	CTWindowGroup* iGroupWinForCapturing;
	CTBlankWindow* iWinForCapturing;
	
	TInt iYOffset;
	};

class CTMultiPtrEventTestStep : public CTGraphicsStep
	{
public:
	CTMultiPtrEventTestStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMultiPtrEventTestStep, "TMultiPtrEventTest");

#endif /*TMULTIPTREVENT_H_*/
