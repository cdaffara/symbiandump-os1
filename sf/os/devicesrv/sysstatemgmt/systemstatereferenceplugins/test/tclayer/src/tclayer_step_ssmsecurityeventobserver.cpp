// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32property.h>
#include <w32std.h>
#include "ssmdebug.h"
#include "strtsecphaseobserver.h"
#include "ssmsecurityeventobserver.h"
#include "ssmsecuritynotecontroller.h"
#include "ssmuiproviderdll.h"
#include "ssmrefcustomcmdcommon.h"
#include "strtsecuritynotetype.h"
#include "ssmsecuritychecknotifier.h"
#include "tclayer_step_ssmsecurityeventobserver.h"
#include "strtsecnoterequestqueue.h"
#include "strtsecphaseobserver.h"

//Exe name which defines security state PS keys
_LIT (KExeToDefineSecurityStatePS, "\\sys\\bin\\definesecuritystateps.exe");
const TUint32 KMiscPluginPropertyKey = 0x2000E658;
const TUid KPropertyCategory={0x2000D75B};

CLayerTestSsmEventObserver::CLayerTestSsmEventObserver()
	{
	}

CLayerTestSsmEventObserver::~CLayerTestSsmEventObserver()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	}

TVerdict CLayerTestSsmEventObserver::doTestStepPreambleL()
	{
	_LIT(KTESTLOG, "doTestStepPreambleL");
	INFO_PRINTF1(KTESTLOG);

	//Create and install active scheduler
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;

	//Needed fro calling calback for stopping active scheduler
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);

	//Start the test exe which defines security state related property keys
	RProcess processHandle;
	CleanupClosePushL(processHandle);
	processHandle.Create(KExeToDefineSecurityStatePS, KNullDesC);
	processHandle.Resume();
	TRequestStatus status;
    processHandle.Rendezvous(status);
    User::WaitForRequest(status);
	        
    TInt retVal = status.Int();
    INFO_PRINTF2(_L("retVal = %d"),retVal);
    TEST(KErrNone == retVal);
    // leave if the process has not started properly
    User::LeaveIfError(retVal);
    CleanupStack::PopAndDestroy();
	//processHandle.Close();
    
    TInt error = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
    TInt error1 = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
    INFO_PRINTF3(_L("Defining and setting property returned %d & %d"), error, error1);

	return CTestStep::doTestStepPreambleL();
	}

TVerdict CLayerTestSsmEventObserver::doTestStepPostambleL()
	{
    TInt error = RProperty::Delete(KPropertyCategory, KMiscPluginPropertyKey);
	return CTestStep::doTestStepPostambleL();
	}

static TInt CallBackL(TAny* aLayerTestSsmEventObserver)
	{
	//Call back function to stop active scheduler
	CLayerTestSsmEventObserver* test = reinterpret_cast<CLayerTestSsmEventObserver*>(aLayerTestSsmEventObserver);
	test->CallBackRunL();
	return KErrNone;
	}

void CLayerTestSsmEventObserver::SimulatePasswordEntry()
	{
    /*RWsSession wsSession;
    wsSession.Connect();*/
    
	TRawEvent eventDown;
	TRawEvent eventUp;

	//Simulate the key press ,(comma) in to pin notifier dialogue
	eventDown.Set(TRawEvent::EKeyDown, EStdKeyComma);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EKeyUp, EStdKeyComma);
	UserSvr::AddEvent(eventUp);
	User::After(1000000);

/*    eventDown.Set(TRawEvent::EKeyDown, EStdKeyEnter);
    UserSvr::AddEvent(eventDown);
    eventUp.Set(TRawEvent::EKeyUp, EStdKeyEnter);
    UserSvr::AddEvent(eventUp);*/
    
	eventDown.Set(TRawEvent::EButton1Down, 60, 600);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EButton1Up, 60, 600);
	UserSvr::AddEvent(eventUp);
	User::After(1000000);
	
   /* wsSession.Flush();
    wsSession.Close();*/
	}

void CLayerTestSsmEventObserver::CallBackRunL()
	{
	//Stop the active scheduler if the request is completed else
	//simulate keys and callback once again
	if(iRequest)
		{
		if((*iRequest) != KRequestPending)
			{
			iActiveSchedulerWait->AsyncStop();
			}
		else
			{
			iAsyncStopScheduler->CallBack();
			SimulatePasswordEntry();
			}
		}
	else
		{
		iActiveSchedulerWait->AsyncStop();
		}
	}


void CLayerTestSsmEventObserver::TestStartSecurityEventObserverSchedulerL()
	{
	TCallBack stop(CallBackL, this);
	iAsyncStopScheduler->Set(stop);

	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	}

/**
Old Test CaseID 		AFSS-CLAYER-0026
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0026
 */	

void CLayerTestSsmEventObserver::TestSecurityEventObserverL()
	{
	TestSecurityEventObserver1L();
	TestSecurityEventObserver2L();
	TestSecurityEventObserver3L();
	TestSecurityEventObserver4L();
	TestSecurityEventObserver5L();
	
	TestSecNoteRequestQueueL();
	TestSecPhaseObserverL();
	TestSecurityNoteControllerL();
	}

// Release the CStrtSecurityEventObserver
void ReleaseSecurityEventObserver(TAny* aPtr)
	{
	CStrtSecurityEventObserver* self = static_cast<CStrtSecurityEventObserver*> (aPtr);
	self->Release();
	}

void CLayerTestSsmEventObserver::TestSecurityEventObserver1L()
	{
	_LIT(KTESTLOG, "TestSecurityEventObserver1L");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecurityEventObserver* securityEventObserver = static_cast<CStrtSecurityEventObserver*> (CStrtSecurityEventObserver::NewL());
	CleanupStack::PushL(TCleanupItem(ReleaseSecurityEventObserver, securityEventObserver));
	securityEventObserver->InitializeL();

	securityEventObserver->iEvent = RMobilePhone::ENoICCFound;
	securityEventObserver->ProcessSecurityEvent();

	securityEventObserver->iEvent = RMobilePhone::EPin1Required;
	securityEventObserver->iSecPhaseObserver->iValue = EStarterSecurityPhaseUninitialized;
	securityEventObserver->ProcessSecurityEvent();
	TBool codeOk = securityEventObserver->iController->iSecurityNote->IsCodeAccepted();
	TEST(!codeOk);

	securityEventObserver->iSecPhaseObserver->iValue = EStarterSecurityPhaseSimOk;
	iRequest = &(securityEventObserver->iController->iStatus);
	securityEventObserver->ProcessSecurityEvent();

	TestStartSecurityEventObserverSchedulerL();

	codeOk = securityEventObserver->iController->iSecurityNote->IsCodeAccepted();
	TEST(codeOk);

	//Send verified event
	securityEventObserver->iEvent = RMobilePhone::EPin1Verified;
	securityEventObserver->ProcessSecurityEvent();
	CleanupStack::PopAndDestroy(securityEventObserver);
	}

void CLayerTestSsmEventObserver::TestSecurityEventObserver2L()
	{
	_LIT(KTESTLOG, "TestSecurityEventObserver2L");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecurityEventObserver* securityEventObserver = static_cast<CStrtSecurityEventObserver*> (CStrtSecurityEventObserver::NewL());
	CleanupStack::PushL(TCleanupItem(ReleaseSecurityEventObserver, securityEventObserver));
	securityEventObserver->InitializeL();

	securityEventObserver->iEvent = RMobilePhone::EPhonePasswordRequired;
	securityEventObserver->iSecPhaseObserver->iValue = EStarterSecurityPhaseUninitialized;
	securityEventObserver->ProcessSecurityEvent();
	TBool codeOk = securityEventObserver->iController->iSecurityNote->IsCodeAccepted();
	TEST(!codeOk);

	securityEventObserver->iSecPhaseObserver->iValue = EStarterSecurityPhaseSecOk;
	iRequest = &(securityEventObserver->iController->iStatus);
	securityEventObserver->ProcessSecurityEvent();
	TestStartSecurityEventObserverSchedulerL();

	codeOk = securityEventObserver->iController->iSecurityNote->IsCodeAccepted();
	TEST(codeOk);

	//Send verified event
	securityEventObserver->iEvent = RMobilePhone::EPhonePasswordVerified;
	securityEventObserver->ProcessSecurityEvent();
	CleanupStack::PopAndDestroy(securityEventObserver);
	}

void CLayerTestSsmEventObserver::TestSecurityEventObserverHelperL(RMobilePhone::TMobilePhoneSecurityEvent aSecurityEvent, 
																	RMobilePhone::TMobilePhoneSecurityEvent aSecurityVerificationEvent)
	{
	CStrtSecurityEventObserver* securityEventObserver = static_cast<CStrtSecurityEventObserver*> (CStrtSecurityEventObserver::NewL());
	CleanupStack::PushL(TCleanupItem(ReleaseSecurityEventObserver, securityEventObserver));
	securityEventObserver->InitializeL();

	securityEventObserver->iEvent = aSecurityEvent;
	TBool codeOk = securityEventObserver->iController->iSecurityNote->IsCodeAccepted();
	TEST(!codeOk);

	securityEventObserver->iSecPhaseObserver->iValue = EStarterSecurityPhaseSecOk;
	iRequest = &(securityEventObserver->iController->iStatus);
	securityEventObserver->ProcessSecurityEvent();
	TestStartSecurityEventObserverSchedulerL();

	codeOk = securityEventObserver->iController->iSecurityNote->IsCodeAccepted();
	TEST(codeOk);

	//Send verified event 
	securityEventObserver->iEvent = aSecurityVerificationEvent;
	securityEventObserver->ProcessSecurityEvent();
	CleanupStack::PopAndDestroy(securityEventObserver);
	}

void CLayerTestSsmEventObserver::TestSecurityEventObserver3L()
	{
	_LIT(KTESTLOG, "TestSecurityEventObserver3L");
	INFO_PRINTF1(KTESTLOG);
	TestSecurityEventObserverHelperL(RMobilePhone::EUSIMAppPinRequired, RMobilePhone::EUSIMAppPinVerified);
	TestSecurityEventObserverHelperL(RMobilePhone::EPuk1Required, RMobilePhone::EPuk1Verified);
	TestSecurityEventObserverHelperL(RMobilePhone::EPin2Required, RMobilePhone::EPin2Verified);
	TestSecurityEventObserverHelperL(RMobilePhone::EPuk2Required, RMobilePhone::EPuk2Verified);
	TestSecurityEventObserverHelperL(RMobilePhone::ESecondUSIMAppPinRequired, RMobilePhone::ESecondUSIMAppPinVerified);
	TestSecurityEventObserverHelperL(RMobilePhone::EUniversalPinRequired, RMobilePhone::EUniversalPinVerified);
	TestSecurityEventObserverHelperL(RMobilePhone::EUniversalPukRequired, RMobilePhone::EUniversalPukVerified);
	}

void CLayerTestSsmEventObserver::TestSecurityEventObserver4L()
	{
	_LIT(KTESTLOG, "TestSecurityEventObserver4L");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecurityEventObserver* securityEventObserver = static_cast<CStrtSecurityEventObserver*> (CStrtSecurityEventObserver::NewL());
	CleanupStack::PushL(TCleanupItem(ReleaseSecurityEventObserver, securityEventObserver));
	securityEventObserver->InitializeL();

	securityEventObserver->iEvent = RMobilePhone::EUSIMAppPinRequired;
	iRequest = &(securityEventObserver->iController->iStatus);
	securityEventObserver->StartL();
	securityEventObserver->Cancel();
	TestStartSecurityEventObserverSchedulerL();

	//Release will panic in case of any pending AO objects
	CleanupStack::PopAndDestroy(securityEventObserver);
	}

void CLayerTestSsmEventObserver::TestSecurityEventObserver5L()
	{
	_LIT(KTESTLOG, "TestSecurityEventObserver5L");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecurityEventObserver* securityEventObserver = static_cast<CStrtSecurityEventObserver*> (CStrtSecurityEventObserver::NewL());
	CleanupStack::PushL(TCleanupItem(ReleaseSecurityEventObserver, securityEventObserver));
	securityEventObserver->InitializeL();

	securityEventObserver->iEvent = RMobilePhone::EUSIMAppPinRequired;
	iRequest = &(securityEventObserver->iController->iStatus);

	securityEventObserver->iStatus = KRequestPending;
	TRequestStatus* status = &(securityEventObserver->iStatus);
	User::RequestComplete(status, KErrNone);
	securityEventObserver->SetActive();
	TestStartSecurityEventObserverSchedulerL();
	TEST(KErrNotSupported == securityEventObserver->iStatus.Int());

	//Release will panic in case of any pending AO objects
	CleanupStack::PopAndDestroy(securityEventObserver);
	}

void CLayerTestSsmEventObserver::TestSecNoteRequestQueueL()
	{
	_LIT(KTESTLOG, "TestSecNoteRequestQueueL");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecNoteRequestQueue* secNoteRequestQueue = CStrtSecNoteRequestQueue::NewL();

	secNoteRequestQueue->Remove(ESecCodePIN1);
	TInt err = secNoteRequestQueue->Add(ESecCodePIN1);
	TEST(KErrNone == err);

	err = secNoteRequestQueue->Add(ESecCodePIN1);
	TEST(KErrNone == err);

	secNoteRequestQueue->Remove(ESecCodePIN1);
	delete secNoteRequestQueue;
	}

void CLayerTestSsmEventObserver::TestSecPhaseObserverL()
	{
	_LIT(KTESTLOG, "TestSecPhaseObserverL");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecPhaseObserver* secPhaseObserver = CStrtSecPhaseObserver::NewL();

	TEST(! secPhaseObserver->IsSecurityPhaseSimOk());

	secPhaseObserver->iValue = EStarterSecurityPhaseSimOk;
	TEST(secPhaseObserver->IsSecurityPhaseSimOk());

	secPhaseObserver->iValue = EStarterSecurityPhaseSecOk;
	TEST(secPhaseObserver->IsSecurityPhaseSimOk());

	secPhaseObserver->iValue = EStarterSecurityPhaseSecNok;
	TEST(secPhaseObserver->IsSecurityPhaseSimOk());

	secPhaseObserver->Cancel();
	secPhaseObserver->iStatus = KRequestPending;
	TRequestStatus* status = &(secPhaseObserver->iStatus);
	User::RequestComplete(status, KErrGeneral);
	secPhaseObserver->SetActive();

	TestStartSecurityEventObserverSchedulerL();

	TEST(KErrGeneral == secPhaseObserver->iStatus.Int());
	delete secPhaseObserver;
	}

void CLayerTestSsmEventObserver::TestSecurityNoteControllerL()
	{
	_LIT(KTESTLOG, "TestSecurityNoteControllerL");
	INFO_PRINTF1(KTESTLOG);
	CStrtSecurityNoteController* securityNoteController = CStrtSecurityNoteController::NewL();

	//Should not show any dialogue asking for code
	securityNoteController->CompleteSelf();
	securityNoteController->SecurityNoteRequested(ESecCodePIN1);

	delete securityNoteController;
	}
TVerdict CLayerTestSsmEventObserver::doTestStepL()
	{
	__UHEAP_MARK;
	TestSecurityEventObserverL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
