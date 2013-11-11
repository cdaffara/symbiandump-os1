// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ssm/ssmcommandlist.h>

#include "ssmswppolicyframe.h"
#include "ssmswptransitionengine.h"
#include "ssmswptransitionscheduler.h"
#include "ssmswprequesthandler.h"
#include "tssm_step_swprequesthandler.h"

#include "tssm_swppolicy_generic.h"
#include "tssm_swppolicy_generic.hrh"

// Keys and values used for testing the transition engines (Using ssm.swp.policy.test.generic.dll)
const TUint KSwpTestKey1=0xE5325221; 

/**
 This function is wrapped in  Callback and used to stop the CActiveScheduler
 when all other activities have been peformed
 */
static TInt StopScheduler(TAny* aSsmSwpRequestHandlerTest)
	{
	CSsmSwpRequestHandlerTest*  ssmSwpRequestHandlerTest = static_cast<CSsmSwpRequestHandlerTest*>(aSsmSwpRequestHandlerTest);
	ssmSwpRequestHandlerTest->StopActiveScheduler();
	return KErrNone;
	}

//----------------- CTestStep implemenation --------------------

/**
 Construct and register test class name with TEF
 */
CSsmSwpRequestHandlerTest::CSsmSwpRequestHandlerTest()
	{
	SetTestStepName (KTSsmSwpRequestHandlerStep);
	}

/**
 Destroy common resources created at beginning of test
 */
CSsmSwpRequestHandlerTest::~CSsmSwpRequestHandlerTest()
	{
	delete iActiveScheduler;
	}

/**
 Create common resources for this test 
 Install CActiveScheduler necessary for the tests
 */
TVerdict CSsmSwpRequestHandlerTest::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set (stop);

	return CTestStep::doTestStepPreambleL ();
	}
/**
Old Test CaseID 		APPFWK-SSM-0008
New Test CaseID 		DEVSRVS-SSMA-SSM-0008
 */

TVerdict CSsmSwpRequestHandlerTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpRequestHandlerTest started...."));
	
	__UHEAP_MARK;
	
	doTestSwpRequestHandlerTestForMemoryLeaksL ();
	doTestSimpleSubmissionL ();
	doTestMultipleSubmissionL ();
	doTestCancelSubmissionL ();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpRequestHandlerTest completed!!"));
	return TestStepResult();
	}

/**
 Clean up if necessary
 */
TVerdict CSsmSwpRequestHandlerTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL ();
	}

void CSsmSwpRequestHandlerTest::StopActiveScheduler()
	{
	// We would stop the sceduler once all the required transitions are completed
	if (iExpectedNoOfTransitions == iNumTransitionsCompleted)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		}
	}

/**

 */
void CSsmSwpRequestHandlerTest::doTestSwpRequestHandlerTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmSwpRequestHandler"));

	// CSsmSwpRequestHandler
	INFO_PRINTF1(_L("CSsmSwpRequestHandler construction under OOM"));
	CSsmSwpRequestHandler* handler=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, handler = CSsmSwpRequestHandler::NewL());
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(handler==NULL);
	        }
	    else
	        {
	        TEST(handler!=NULL);
			delete handler;
	        handler = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);
	
	// CSsmSwpRequestHandler
	INFO_PRINTF1(_L("CSsmSwpRequestHandler simple submission under OOM"));
	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	
		//INFO_PRINTF2(_L("  #will now fail %d allocation"),failRate);

	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, 
	    	{
	    	CSsmSwpRequestHandler* handler = CSsmSwpRequestHandler::NewLC();
			handler->SetSwpHandlerMonitor(this);
			handler->SetSwpPolicyResolverProxy(this);
			handler->SetCleSessionProxy(this);
			TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);
			handler->SubmitRequestL(goodValue);
			CleanupStack::PopAndDestroy(handler);
	    	})
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;
	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for simple submission: %d."),failRate-1);
	}

/**
 Test a single submission to the scheduler
 */
void CSsmSwpRequestHandlerTest::doTestSimpleSubmissionL()
	{
	INFO_PRINTF1(_L("Checking a single submission to the scheduler"));

	ResetCounters ();

	__UHEAP_MARK;

	// prepare participants
	CSsmSwpRequestHandler* handler = CSsmSwpRequestHandler::NewLC ();
	// add monitor for tracking counts
	handler->SetSwpHandlerMonitor (this);
	handler->SetSwpPolicyResolverProxy (this);
	handler->SetCleSessionProxy (this);
	handler->PerformCommandListValidation(EFalse);

	// get scheduler and add a monitor
	CSsmSwpTransitionScheduler& scheduler = handler->SwpTransitionScheduler ();
	scheduler.SetTransactionMonitor (this);

	TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);

	handler->SubmitRequestL (goodValue);

	iExpectedNoOfTransitions = 1;
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack ();
	//run engine
	iActiveScheduler->Start ();

	//Verify that the expected calls were made 
	AssertSimpleSubmission();

	CleanupStack::PopAndDestroy (handler);

	__UHEAP_MARKEND;

	}

void CSsmSwpRequestHandlerTest::AssertSimpleSubmission()
	{
	TEST(iNumTransitionsQueued == 0);
	TEST(iNumTransitionsStarted == 1);
	TEST(iNumTransitionsCompleted == 1);
	TEST(iNumTransitionsFailed == 0);
	TEST(iNumRequestsMade == 1);
	}

/**
 Test multiple submissions to the scheduler
 */
void CSsmSwpRequestHandlerTest::doTestMultipleSubmissionL()
	{
	INFO_PRINTF1(_L("Checking multiple submissions to the scheduler"));

	ResetCounters ();

	__UHEAP_MARK;

	// prepare participants
	CSsmSwpRequestHandler* handler = CSsmSwpRequestHandler::NewLC ();
	// add monitor for tracking counts
	handler->SetSwpHandlerMonitor (this);
	handler->SetSwpPolicyResolverProxy (this);
	handler->SetCleSessionProxy (this);
	handler->PerformCommandListValidation(EFalse);

	// get scheduler and add a monitor
	CSsmSwpTransitionScheduler& scheduler = handler->SwpTransitionScheduler ();
	scheduler.SetTransactionMonitor (this);

	// prepare values
	TSsmSwp swpValueGood(KSwpTestKey1, ESwpGenericDoNothing);
	TSsmSwp swpValueBad(KSwpTestKey1, ESwpGenericBadValueNotAllowed);

	handler->SubmitRequestL (swpValueGood);
	handler->SubmitRequestL (swpValueGood);
	handler->SubmitRequestL (swpValueBad);
	handler->SubmitRequestL (swpValueGood);
	handler->SubmitRequestL (swpValueGood);
	handler->SubmitRequestL (swpValueBad);
	handler->SubmitRequestL (swpValueGood);
	handler->SubmitRequestL (swpValueGood);

	iExpectedNoOfTransitions = 6;
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack ();
	//run engine
	iActiveScheduler->Start ();

	//Verify that the expected calls were made 
	AssertMultipleSubmission();

	CleanupStack::PopAndDestroy (handler);

	__UHEAP_MARKEND;
	}

void CSsmSwpRequestHandlerTest::AssertMultipleSubmission()
	{
	TESTE(iNumTransitionsQueued == 7, iNumTransitionsQueued);
	TESTE(iNumTransitionsStarted == 8, iNumTransitionsStarted);
	TESTE(iNumTransitionsCompleted == 6, iNumTransitionsCompleted);
	TESTE(iNumTransitionsFailed == 2, iNumTransitionsFailed);
	TESTE(iNumRequestsMade == 8, iNumRequestsMade);
	}


/**
 Test Cancel submissions to the scheduler
 */
void CSsmSwpRequestHandlerTest::doTestCancelSubmissionL()
	{
	INFO_PRINTF1(_L("Checking cancel submissions to the scheduler"));

	ResetCounters ();

	__UHEAP_MARK;

	// prepare participants
	CSsmSwpRequestHandler* handler = CSsmSwpRequestHandler::NewLC ();
	// add monitor for tracking counts
	handler->SetSwpHandlerMonitor (this);
	handler->SetSwpPolicyResolverProxy (this);
	handler->SetCleSessionProxy (this);
	handler->PerformCommandListValidation(EFalse);

	// get scheduler and add a monitor
	CSsmSwpTransitionScheduler& scheduler = handler->SwpTransitionScheduler ();
	scheduler.SetTransactionMonitor (this);

	// prepare values
	TSsmSwp swpValueGood(KSwpTestKey1, ESwpGenericDoNothing);
	TSsmSwp swpValueBad(KSwpTestKey1, ESwpGenericBadValueNotAllowed);

	handler->SubmitRequestL (swpValueGood); //A
	handler->SubmitRequestL (swpValueGood); //B
	handler->SubmitRequestL (swpValueBad);	//C
	handler->SubmitRequestL (swpValueGood); //D

	handler->Cancel(NULL);

	handler->SubmitRequestL (swpValueGood); //E
	handler->SubmitRequestL (swpValueBad);	//F
	handler->SubmitRequestL (swpValueGood); //G
	handler->SubmitRequestL (swpValueGood); //H

	iExpectedNoOfTransitions = 3;
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack ();
	//run engine
	iActiveScheduler->Start ();

	//Verify that the expected calls were made 
	AssertCancelSubmission();

	CleanupStack::PopAndDestroy (handler);

	__UHEAP_MARKEND;
	}

void CSsmSwpRequestHandlerTest::AssertCancelSubmission()
	{
	//Assert that all 8 Submissions were reported
	TEST(iNumRequestsMade == 8); //ABCDEFGH
	
	//Assert that from the 4 requests that was run, 3 are good and 1 is bad
	TEST(iNumTransitionsCompleted == 3);//EGH
	TEST(iNumTransitionsFailed == 1);	//F
	
	//Assert that all but initial requests are queued
	TESTE(iNumTransitionsQueued == 6, iNumTransitionsQueued);	//BCDEGH
	
	//Assert the number of requests that have their engine AO SetActive()
	TESTE(iNumTransitionsStarted == 5, iNumTransitionsStarted);	//AEFGH	
	}

/**
 reset internal test counters
 */
void CSsmSwpRequestHandlerTest::ResetCounters()
	{
	iNumTransitionsQueued = 0;
	iNumTransitionsStarted = 0;
	iNumTransitionsCompleted = 0;
	iNumTransitionsFailed = 0;
	iNumRequestsMade = 0;
	}

//----------------- MPolicyResolverProxy interface implementation --------------------

CSsmSwpPolicyFrame* CSsmSwpRequestHandlerTest::CreatePolicyLC(const RLibrary& aLibrary) const
	{
	__ASSERT_DEBUG( KNullHandle != aLibrary.Handle(), PanicNow(KPanicSysStateMgr, ESwpPolicyResolverLibraryNotLoaded));
	
	CSsmSwpPolicyFrame* frame = NULL;
	TRAPD(err, frame = CSsmSwpPolicyFrame::NewL(aLibrary.Lookup(1)));
	if (err != KErrNone)
		{
		TFileName name = aLibrary.FileName();
		DEBUGPRINT3(_L("Error %d when calling first function in State Policy DLL %S."), err, &name);
		}
	User::LeaveIfError(err);
	CleanupStack::PushL(frame);
	return frame;
	}
/**
 */
CSsmSwpPolicyFrame* CSsmSwpRequestHandlerTest::GetSwpPolicyL(const TSsmSwp& aSwp)
	{
	RLibrary library;
	const TInt fileErr = library.Load(KTestSwpPolicyGenericFile);
	if (fileErr != KErrNone)
		{
		RDebug::Print(_L("Failed to load library file %S, file error-code: %d"), &KTestSwpPolicyGenericFile(), fileErr);
		User::Leave(fileErr);
		}

	// make a process-owned copy of the handle as it has to be used by SsmSwpPolicyServer, 
	// which runs in a separate thread
	User::LeaveIfError(library.Duplicate(RThread(), EOwnerProcess));

	CSsmSwpPolicyFrame* policy = CreatePolicyLC(library);
	CleanupStack::Pop(policy);
	policy->SetLibrary(library); // takes ownership of open library handle
	policy->SetSwpKey(aSwp.Key());

	RDebug::Print(_L("Created Swp Policy DLL %S"), &KTestSwpPolicyGenericFile());

	return policy;
	}

//--------------------- MCleSessionProxy interface implementation -----------------------
/**
 */
void CSsmSwpRequestHandlerTest::ExecuteCommandList(const CSsmCommandList& /*aCmdList*/, TRequestStatus& aStatus, TCmdErrorSeverity& /*aSeverity*/)
	{
	TRequestStatus* tmp = &aStatus;
	User::RequestComplete (tmp, KErrNone);
	}

void CSsmSwpRequestHandlerTest::ExecuteCommandListCancel()
	{
	}

/** Function needed when using real SysCle but not needed for this test*/
void CSsmSwpRequestHandlerTest::ConnectL()
	{
	}
void CSsmSwpRequestHandlerTest::ReleaseCle()
	{
	}
void CSsmSwpRequestHandlerTest::Close()
	{
	}
TBool CSsmSwpRequestHandlerTest::IsConnected() 
	{
	return EFalse;
	}

MCleSessionProxy* CSsmSwpRequestHandlerTest::CreateNewProxyL()
	{
	return this;
	}

//--------------------- MSwpSchedulerMonitor interface implementation -----------------------
/**
 count the number of started transitions
 */
void CSsmSwpRequestHandlerTest::SwpTransitionQueued()
	{
	iNumTransitionsQueued++;
	}

/**
 count the number of started transitions
 */
void CSsmSwpRequestHandlerTest::SwpTransitionStarted()
	{
	iNumTransitionsStarted++;
	}

/**
 count the number of completed transitions
 */
void CSsmSwpRequestHandlerTest::SwpTransitionCompleted(TUint aNumberCompleted)
	{
	(void)aNumberCompleted;
	iNumTransitionsCompleted++;
	}

void CSsmSwpRequestHandlerTest::SwpTransitionFailed(TUint aNumberFailed)
	{
	(void)aNumberFailed;
	iNumTransitionsFailed++;
	}

//--------------------- MSwpHandlerMonitor interface implementation -----------------------
/**
 count the number of started transitions
 */
void CSsmSwpRequestHandlerTest::SwpRequestMade(const TSsmSwp& aRequest)
	{
	(void) aRequest;
	iNumRequestsMade++;
	}




