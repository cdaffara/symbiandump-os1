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

#include <etelmm.h>
#include <centralrepository.h> 
#include <ssm/ssmuiproviderdll.h>

// Include the header file for the class under test
#include "ssmsimstatusobserver.h"
#include "cmdpublishsimownedandchanged.h"

#include "ssmclayer.h"

#include "ssmdebug.h"

#include "tclayer_step_ssmsimstatusobserver.h"

//Include the test wrappers/helpers
#include "adaptationcliwrapper.h"
#include "ssmcmnwrapper.h"
#include "activestopper.h"

/**
 * Constructs a complete CCLayerTestSsmSimStatusObserver object
 * @internalComponent
 */
CCLayerTestSsmSimStatusObserver::CCLayerTestSsmSimStatusObserver()
    :iAsyncStopScheduler(NULL),
    iActiveScheduler(NULL)  
	{
	
	}

/**
 * Releases any resources associated with this test
 * @internalComponent
 */
CCLayerTestSsmSimStatusObserver::~CCLayerTestSsmSimStatusObserver()
	{
	
	}

/**
 * Test step preamble
 * @internalComponent
 */
TVerdict CCLayerTestSsmSimStatusObserver::doTestStepPreambleL()
	{
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install (iActiveScheduler);

    //Needed for calling callback for stopping active scheduler
    iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	return CTestStep::doTestStepPreambleL();
	}
/**
 * CallBackL
 * @internalComponent
 */
static TInt CallBackL(TAny* aCCLayerTestSsmSimStatusObserver)
    {
    //Call back function to stop active scheduler
    CCLayerTestSsmSimStatusObserver* test = reinterpret_cast<CCLayerTestSsmSimStatusObserver*>(aCCLayerTestSsmSimStatusObserver);
    test->CallBackRunL();
    return KErrNone;
    }
/**
 * 
 * CallBackRunL
 * @internalComponent
 */
void CCLayerTestSsmSimStatusObserver::CallBackRunL()
    {
    //Stop the active scheduler if the request is completed else
    //callback once again
    if(iRequestStatus != KRequestPending)
        {
        CActiveScheduler::Stop();
        }
    else
        {
        iAsyncStopScheduler->CallBack();
        }
    }

/**
 * Test step - a single test step performs a series of tests.
 * 
 * See the individual doTest... methods on this class for details of the tests.
 * 
 * @internalComponent
 */
TVerdict CCLayerTestSsmSimStatusObserver::doTestStepL()
	{
	INFO_PRINTF1(_L("Entering test for compatibility layer SSM SIM status observer"));
	
	__UHEAP_MARK;
	
	TRAPD(err, doTestSimChangedAndOwnedL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestInitalizationAndDestructionL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestSimEventsL());
	TEST(err == KErrNone);
	
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Leaving test for compatibility layer SSM SIM status observer"));
	return TestStepResult();
	}

/**
 * Test step postamble
 * @internalComponent
 */
TVerdict CCLayerTestSsmSimStatusObserver::doTestStepPostambleL()
	{
	delete iActiveScheduler;
    delete iAsyncStopScheduler;
   	return CTestStep::doTestStepPostambleL();
	}
	
/**
Old Test CaseID 		AFSS-CLAYER-0023
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0023
 */	

void CCLayerTestSsmSimStatusObserver::doTestInitalizationAndDestructionL()
	{
	INFO_PRINTF1(_L("Entering test for intialization and destruction of SSM SIM status observer"));
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Testing constructor for SSM SIM status observer"));
	
	CSsmSimStatusObserver* simObserver = new (ELeave) CSsmSimStatusObserver();
	CleanupStack::PushL(simObserver);
	
	// Now cleanup
	simObserver->Release();
	CleanupStack::Pop(simObserver);
	simObserver = NULL;
	
	INFO_PRINTF1(_L("Testing exported NewL() for SSM SIM status observer"));
	
	MSsmUtility* ssmUtility = CSsmSimStatusObserver::NewL();
	CleanupStack::PushL(ssmUtility);
	
	// Now cleanup
	ssmUtility->Release();
	CleanupStack::Pop(ssmUtility);
	ssmUtility = NULL;
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for intialization and destruction of SSM SIM status observer"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0024
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0023
 */	

void CCLayerTestSsmSimStatusObserver::doTestSimChangedAndOwnedL()
	{
	INFO_PRINTF1(_L("Entering test for change and owned parts of SSM SIM status observer/cmd"));
	__UHEAP_MARK;
	
	// Delete any existing defines, ignore return value
	TInt err1 = RProperty::Delete(CSsmUiSpecific::StartupPSUid(), KPSSimChanged);
	TInt err2 = RProperty::Delete(CSsmUiSpecific::StartupPSUid(), KPSSimOwned);
	INFO_PRINTF3(_L("Value of err1 & err2 = %d & %d"), err1, err2);
	TInt err = KErrNone;
	
	INFO_PRINTF1(_L("Constructing SSM SIM publish cmd"));	
	CCustomCmdPublishSimOwnedAndChanged* simCustomCmd = CCustomCmdPublishSimOwnedAndChanged::NewL();
	CleanupReleasePushL(*simCustomCmd);	
	simCustomCmd->Initialize(NULL);
	CleanupClosePushL(*simCustomCmd);
	
	INFO_PRINTF1(_L("Executing SSM SIM publish cmd without defining SIM properties"));
	TBuf8<1> emptyBuf;
	simCustomCmd->Execute(emptyBuf, iRequestStatus);
	TEST(iRequestStatus == KRequestPending);

	INFO_PRINTF1(_L("Adding callBack"));
	TCallBack stop(CallBackL, this);
    iAsyncStopScheduler->Set(stop);
    
    iAsyncStopScheduler->CallBack();
	INFO_PRINTF1(_L("Testing running active scheduler for publish startup mode without property defined"));
	iActiveScheduler->Start();
	User::WaitForRequest(iRequestStatus);
	
	TEST(iRequestStatus == KErrNotFound);
	
	INFO_PRINTF1(_L("Checking execute cancel does nothing"));
	simCustomCmd->ExecuteCancel();
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM publish cmd"));
	
	// Close simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	// Release simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	simCustomCmd = NULL;
		
	INFO_PRINTF1(_L("Constructing SSM SIM publish cmd"));	
	simCustomCmd = CCustomCmdPublishSimOwnedAndChanged::NewL();
	CleanupReleasePushL(*simCustomCmd);
	simCustomCmd->Initialize(NULL);
	CleanupClosePushL(*simCustomCmd);
	
	INFO_PRINTF1(_L("Initializing SSM SIM status observer without defining SIM changed property"));
	err = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KPSSimOwned, RProperty::EInt);
	INFO_PRINTF2(_L("Define returns err = %d") ,err);
	TEST(err == KErrNone);
	simCustomCmd->Execute(emptyBuf, iRequestStatus);
	
	INFO_PRINTF1(_L("Adding callback"));
	iAsyncStopScheduler->CallBack();
	INFO_PRINTF1(_L("Testing running active scheduler for publish startup mode without defining SIM changed property"));
	iActiveScheduler->Start();
	User::WaitForRequest(iRequestStatus);
	TEST(iRequestStatus == KErrNotFound);
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM publish cmd"));
	
	// Close simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	// Release simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	simCustomCmd = NULL;

	INFO_PRINTF1(_L("Constructing SSM SIM publish cmd"));	
	simCustomCmd = CCustomCmdPublishSimOwnedAndChanged::NewL();
	CleanupReleasePushL(*simCustomCmd);
	simCustomCmd->Initialize(NULL);
	CleanupClosePushL(*simCustomCmd);
	
	INFO_PRINTF1(_L("Initializing SSM SIM status observer with defined SIM properties, both false"));
	AdaptationCliWrapper::SetSimOwned(EFalse);
	AdaptationCliWrapper::SetSimChanged(EFalse);
	err = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KPSSimChanged, RProperty::EInt);
	TEST(err == KErrNone);
	simCustomCmd->Execute(emptyBuf, iRequestStatus);
	
	INFO_PRINTF1(_L("callback"));
    iAsyncStopScheduler->CallBack();
	INFO_PRINTF1(_L("Testing running active scheduler for publish startup mode with defined SIM properties, both false"));
	iActiveScheduler->Start();	
	User::WaitForRequest(iRequestStatus);
	
	
	TEST(iRequestStatus == KErrPermissionDenied);
	TInt simChangedValue = 0;
	TInt simOwnedValue = 0;
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimChanged, simChangedValue);
	TEST(err == KErrNone);
	TEST(simChangedValue == ESimChanged);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimOwned, simOwnedValue);
	TEST(err == KErrNone);
	TEST(simOwnedValue == ESimNotOwned);
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM publish cmd"));
	// Close simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	// Release simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	simCustomCmd = NULL;

	INFO_PRINTF1(_L("Constructing SSM SIM publish cmd"));	
	simCustomCmd = CCustomCmdPublishSimOwnedAndChanged::NewL();
	CleanupReleasePushL(*simCustomCmd);
	simCustomCmd->Initialize(NULL);
	CleanupClosePushL(*simCustomCmd);
	
	INFO_PRINTF1(_L("Initializing SSM SIM status observer with get owned failure"));
	AdaptationCliWrapper::SetSimOwned(ETrue);
	AdaptationCliWrapper::SetSimOwnedReturnValue(KErrNotSupported);
	simCustomCmd->Execute(emptyBuf, iRequestStatus);
	
	INFO_PRINTF1(_L("callback"));
    iAsyncStopScheduler->CallBack();
	INFO_PRINTF1(_L("Testing running active scheduler for publish startup mode with get owned failure"));
	iActiveScheduler->Start();	
	User::WaitForRequest(iRequestStatus);
	
	TEST(iRequestStatus == KErrPermissionDenied);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimChanged, simChangedValue);
	TEST(err == KErrNone);
	TEST(simChangedValue == ESimChanged);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimOwned, simOwnedValue);
	TEST(err == KErrNone);
	TEST(simOwnedValue == ESimNotOwned);
	AdaptationCliWrapper::SetSimOwnedReturnValue(KErrNone);
	AdaptationCliWrapper::SetSimOwned(EFalse);
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM publish cmd"));
	// Close simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	// Release simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	simCustomCmd = NULL;

	INFO_PRINTF1(_L("Constructing SSM SIM publish cmd"));	
	simCustomCmd = CCustomCmdPublishSimOwnedAndChanged::NewL();
	CleanupReleasePushL(*simCustomCmd);
	simCustomCmd->Initialize(NULL);
	CleanupClosePushL(*simCustomCmd);
	
	INFO_PRINTF1(_L("Initializing SSM SIM status observer with get changed failure"));
	AdaptationCliWrapper::SetSimChangedReturnValue(KErrNotSupported);
	simCustomCmd->Execute(emptyBuf, iRequestStatus);
	
	INFO_PRINTF1(_L("callback"));
    iAsyncStopScheduler->CallBack();
	INFO_PRINTF1(_L("Testing running active scheduler for publish startup mode SIM status observer with get changed failure"));
	iActiveScheduler->Start();	
	User::WaitForRequest(iRequestStatus);
	
	TEST(iRequestStatus == KErrPermissionDenied);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimChanged, simChangedValue);
	TEST(err == KErrNone);
	TEST(simChangedValue == ESimChanged);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimOwned, simOwnedValue);
	TEST(err == KErrNone);
	TEST(simOwnedValue == ESimNotOwned);
	AdaptationCliWrapper::SetSimChangedReturnValue(KErrNone);
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM publish cmd"));
	// Close simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	// Release simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	simCustomCmd = NULL;

	INFO_PRINTF1(_L("Constructing SSM SIM publish cmd"));	
	simCustomCmd = CCustomCmdPublishSimOwnedAndChanged::NewL();
	CleanupReleasePushL(*simCustomCmd);
	simCustomCmd->Initialize(NULL);
	CleanupClosePushL(*simCustomCmd);
	
	INFO_PRINTF1(_L("Initializing SSM SIM status observer with defined SIM properties, both true"));
	AdaptationCliWrapper::SetSimOwned(ETrue);
	AdaptationCliWrapper::SetSimChanged(ETrue);
	simCustomCmd->Execute(emptyBuf, iRequestStatus);
	
	INFO_PRINTF1(_L("callback"));
	iAsyncStopScheduler->CallBack();
	INFO_PRINTF1(_L("Testing running active scheduler for publish startup mode with defined SIM properties, both true"));
	iActiveScheduler->Start();	
	User::WaitForRequest(iRequestStatus);	
		
	TEST(iRequestStatus == KErrPermissionDenied);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimChanged, simChangedValue);
	TEST(err == KErrNone);
	TEST(simChangedValue == ESimChanged);
	err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSimOwned, simOwnedValue);
	TEST(err == KErrNone);
	TEST(simOwnedValue == ESimOwned);
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM publish cmd"));
	// Close simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	// Release simCustomCmd
	CleanupStack::PopAndDestroy(simCustomCmd);
	simCustomCmd = NULL;
		
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for change and owned parts of SSM SIM status observer/cmd"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0025
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0023
 */	

void CCLayerTestSsmSimStatusObserver::doTestSimEventsL()
	{
	INFO_PRINTF1(_L("Entering test for SIM events parts of SSM SIM status observer"));
	__UHEAP_MARK;
		
	INFO_PRINTF1(_L("Constructing SSM SIM status observer"));	
	CSsmSimStatusObserver* simObserver = new (ELeave) CSsmSimStatusObserver();
	CleanupStack::PushL(simObserver);
	
	INFO_PRINTF1(_L("Initializing SSM SIM status observer with defined SIM properties, both true"));
	TRAPD(err, simObserver->InitializeL());
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("Adding active stopper"));
	CActiveStopper* stopper = new (ELeave) CActiveStopper();
	CleanupStack::PushL(stopper);
	    
	INFO_PRINTF1(_L("Starting SSM SIM status observer"));
	simObserver->StartL();
	
	TPckgBuf<TSsmSimEventType> simEventPckg;
	INFO_PRINTF1(_L("Testing with event ESsmSimUsable"));
	simEventPckg() = ESsmSimUsable;
	AdaptationCliWrapper::GetSimEventPckg()->Copy(simEventPckg);
	// Notify the waiting observer
	TRequestStatus* statusPtr = AdaptationCliWrapper::GetSimNotifyStatus();
	User::RequestComplete(statusPtr, KErrNone);
	iActiveScheduler->Start();
	TSsmSwp lastSwp(SsmCmnWrapper::GetLastSwpTransition());
	TEST(lastSwp.Key() == CSsmUiSpecific::SimStatusPropertyKey());
	TEST(lastSwp.Value() == ESimUsable);
	
	INFO_PRINTF1(_L("Testing with event ESsmSimReadable"));
	simEventPckg() = ESsmSimReadable;
	AdaptationCliWrapper::GetSimEventPckg()->Copy(simEventPckg);
	// Notify the waiting observer
	statusPtr = AdaptationCliWrapper::GetSimNotifyStatus();
	User::RequestComplete(statusPtr, KErrNone);
	iActiveScheduler->Start();
	lastSwp = SsmCmnWrapper::GetLastSwpTransition();
	TEST(lastSwp.Key() == CSsmUiSpecific::SimStatusPropertyKey());
	TEST(lastSwp.Value() == ESimReadable);
	
	INFO_PRINTF1(_L("Testing with event ESsmSimNotReady"));
	simEventPckg() = ESsmSimNotReady;
	AdaptationCliWrapper::GetSimEventPckg()->Copy(simEventPckg);
	// Notify the waiting observer
	statusPtr = AdaptationCliWrapper::GetSimNotifyStatus();
	User::RequestComplete(statusPtr, KErrNone);
	iActiveScheduler->Start();
	lastSwp = SsmCmnWrapper::GetLastSwpTransition();
	TEST(lastSwp.Key() == CSsmUiSpecific::SimStatusPropertyKey());
	TEST(lastSwp.Value() == ESimNotReady);
		
	INFO_PRINTF1(_L("Testing with event ESsmSimRemoved"));
	simEventPckg() = ESsmSimRemoved;
	AdaptationCliWrapper::GetSimEventPckg()->Copy(simEventPckg);
	// Notify the waiting observer
	statusPtr = AdaptationCliWrapper::GetSimNotifyStatus();
	User::RequestComplete(statusPtr, KErrNone);
	iActiveScheduler->Start();
	lastSwp = SsmCmnWrapper::GetLastSwpTransition();
	TEST(lastSwp.Key() == CSsmUiSpecific::SimStatusPropertyKey());
	TEST(lastSwp.Value() == ESimNotPresent);
	
	INFO_PRINTF1(_L("Cleaning up active stopper"));
	CleanupStack::PopAndDestroy(stopper);
	
	INFO_PRINTF1(_L("Cleaning up SSM SIM status observer"));
	simObserver->Release();
	CleanupStack::Pop(simObserver);
	simObserver = NULL;

			
	__UHEAP_MARKEND;	
	INFO_PRINTF1(_L("Leaving test for SIM events parts of SSM SIM status observer"));	
	}
