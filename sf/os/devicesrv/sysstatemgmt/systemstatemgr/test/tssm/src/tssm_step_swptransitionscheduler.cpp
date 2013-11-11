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
#include "tssm_step_swptransitionscheduler.h"

#include "tssm_swppolicy_generic.h"
#include "tssm_swppolicy_generic.hrh"

// Keys and values used for testing the transition engines
const TUint KSwpTestKey1=0xE5325221; 

/**
 This function is wrapped in  Callback and used to stop the CActiveScheduler
 when all other activities have been peformed
 */
static TInt StopScheduler(TAny* aSsmSwpRequestSchedulerTest)
	{
	CSsmSwpTransitionSchedulerTest*  ssmSwpRequestSchedulerTest = static_cast<CSsmSwpTransitionSchedulerTest*>(aSsmSwpRequestSchedulerTest);
	ssmSwpRequestSchedulerTest->StopActiveScheduler();
	return KErrNone;
	}

//----------------- CTestStep implemenation --------------------

/**
 Construct and register test class name with TEF
 */
CSsmSwpTransitionSchedulerTest::CSsmSwpTransitionSchedulerTest( )
	{
	SetTestStepName (KTSsmSwpTransitionSchedulerStep );
	}

/**
 Destroy common resources created at beginning of test
 */
CSsmSwpTransitionSchedulerTest::~CSsmSwpTransitionSchedulerTest( )
	{
	delete iActiveScheduler;
	}

/**
 Create common resources for this test 
 Install CActiveScheduler necessary for the tests
 */
TVerdict CSsmSwpTransitionSchedulerTest::doTestStepPreambleL( )
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler );

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set (stop );

	return CTestStep::doTestStepPreambleL ( );
	}

/**
Old Test CaseID 		APPFWK-SSM-0009
New Test CaseID 		DEVSRVS-SSMA-SSM-0009
 */

TVerdict CSsmSwpTransitionSchedulerTest::doTestStepL( )
	{
	INFO_PRINTF1(_L("CSsmSwpTransitionSchedulerTest started...."));
	
	__UHEAP_MARK;
	
	doTestSwpTransitionSchedulerTestForMemoryLeaksL();
	doTestQueueL();
	doTestRequestSimpleL();
	doTestRequestMultipleL();
	doTestCancelL();
	
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpTransitionSchedulerTest completed!!"));
	return TestStepResult();
	}

/**
 Clean up if necessary
 */
TVerdict CSsmSwpTransitionSchedulerTest::doTestStepPostambleL( )
	{
	return CTestStep::doTestStepPostambleL ( );
	}

void CSsmSwpTransitionSchedulerTest::StopActiveScheduler()
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

void CSsmSwpTransitionSchedulerTest::doTestSwpTransitionSchedulerTestForMemoryLeaksL( )
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmSwpTransitionScheduler"));


	// CSsmSwpTransitionScheduler
	INFO_PRINTF1(_L("CSsmSwpTransitionScheduler construction under OOM"));
	CSsmSwpTransitionScheduler* scheduler=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, scheduler = CSsmSwpTransitionScheduler::NewL());
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(scheduler==NULL);
	        }
	    else
	        {
	        TEST(scheduler!=NULL);
			delete scheduler;
	        scheduler = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);


	// CreateAndSubmitEngineL 
	INFO_PRINTF1(_L("CSsmSwpTransitionScheduler create and submit engine under OOM"));
	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret,
	    	{
			CSsmSwpTransitionScheduler* scheduler = CSsmSwpTransitionScheduler::NewLC();
			TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);
			CreateAndSubmitEngineL (scheduler, goodValue );
			CleanupStack::PopAndDestroy(scheduler);
	    	})

        __UHEAP_MARKEND;

        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for create and submit engine: %d."),failRate-1);
	}


/** 
 Test construction and destruction of transition queue 
 */

/**
 Test queueing by submitting a and cancelling multiple transitions
 This should cancel any running transitions but more importnatly it
 will empty the queue and destory any entries
 */
void CSsmSwpTransitionSchedulerTest::doTestQueueL( )
	{
	INFO_PRINTF1(_L("Checking scheduler queue"));

	ResetCounters ( );

	__UHEAP_MARK;

	CSsmSwpTransitionScheduler* scheduler = CSsmSwpTransitionScheduler::NewLC();
	// add monitor for tracking counts
	scheduler->SetTransactionMonitor (this );

	TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, goodValue );

	scheduler->Cancel(NULL);

	//Verify that the expected calls were made 
	AssertQueue();

	CleanupStack::PopAndDestroy (scheduler );

	__UHEAP_MARKEND;

	}

void CSsmSwpTransitionSchedulerTest::AssertQueue( )
	{
	TEST(iNumTransitionsQueued == 3);
	TEST(iNumTransitionsStarted == 1);
	TEST(iNumTransitionsCompleted == 0);
	}

/**
 Test submitting and running of a single transition
 Create a scheduler and submit one transition
 One should start and run to completion and none shoudl be queued
 */
void CSsmSwpTransitionSchedulerTest::doTestRequestSimpleL( )
	{
	INFO_PRINTF1(_L("Checking scheduler with a single engine"));

	ResetCounters ( );

	__UHEAP_MARK;

	CSsmSwpTransitionScheduler* scheduler = CSsmSwpTransitionScheduler::NewLC ( );
	// add monitor for tracking counts
	scheduler->SetTransactionMonitor (this );

	TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);
	CreateAndSubmitEngineL (scheduler, goodValue );

	iExpectedNoOfTransitions = 1;
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack ( );
	//run engine
	iActiveScheduler->Start ( );

	//Verify that the expected calls were made 
	AssertRequestSimple();

	CleanupStack::PopAndDestroy (scheduler );

	__UHEAP_MARKEND;
	}

void CSsmSwpTransitionSchedulerTest::AssertRequestSimple( )
	{
	TEST(iNumTransitionsQueued == 0);
	if (iNumTransitionsQueued != 0)
		INFO_PRINTF3(_L("iNumTransitionsQueued \t expected %d got %d"), 0 , iNumTransitionsQueued);

	TEST(iNumTransitionsStarted == 1);
	if (iNumTransitionsStarted != 1)
		INFO_PRINTF3(_L("iNumTransitionsStarted \t expected %d got %d"), 1 , iNumTransitionsStarted);

	TEST(iNumTransitionsCompleted == 1);
	if (iNumTransitionsCompleted != 1)
		INFO_PRINTF3(_L("iNumTransitionsCompleted \t expected %d got %d"), 1 , iNumTransitionsCompleted);

	TEST(iNumTransitionsFailed == 0);
	if (iNumTransitionsFailed != 0)
		INFO_PRINTF3(_L("iNumTransitionsFailed \t expected %d got %d"), 0 , iNumTransitionsFailed);
	}

/**
 Test submitting, queuing and running of a multiple transitions
 Create a scheduler and submit three transition
 three should start and run to completion and two will be queued
 */
void CSsmSwpTransitionSchedulerTest::doTestRequestMultipleL( )
	{
	INFO_PRINTF1(_L("Checking scheduler with multiple engines"));

	ResetCounters ( );

	__UHEAP_MARK;

	CSsmSwpTransitionScheduler* scheduler = CSsmSwpTransitionScheduler::NewLC ( );
	// add monitor for tracking counts
	scheduler->SetTransactionMonitor (this );

	TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);
	TSsmSwp badValue(KSwpTestKey1, ESwpGenericBadValueNotAllowed);
	
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, badValue );
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, badValue );
	CreateAndSubmitEngineL (scheduler, goodValue );

	iExpectedNoOfTransitions = 4;
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack ( );
	//run engine
	iActiveScheduler->Start ( );

	//Verify that the expected calls were made 
	AssertRequestMultiple();

	CleanupStack::PopAndDestroy (scheduler );

	__UHEAP_MARKEND;
	}

void CSsmSwpTransitionSchedulerTest::AssertRequestMultiple( )
	{
	TEST(iNumTransitionsQueued == 5);
	if(iNumTransitionsQueued != 5)
		INFO_PRINTF3(_L("iNumTransitionsQueued \t expected %d got %d"), 5 , iNumTransitionsQueued);
	
	TEST(iNumTransitionsStarted == 6);
	if (iNumTransitionsStarted != 6)
		INFO_PRINTF3(_L("iNumTransitionsStarted \t expected %d got %d"), 6 , iNumTransitionsStarted);
	
	TEST(iNumTransitionsCompleted == 4);
	if (iNumTransitionsCompleted != 4)
		INFO_PRINTF3(_L("iNumTransitionsCompleted \t expected %d got %d"), 4 , iNumTransitionsCompleted);
	
	TEST(iNumTransitionsFailed == 2);
	if (iNumTransitionsFailed != 2)
		INFO_PRINTF3(_L("iNumTransitionsFailed \t expected %d got %d"), 2 , iNumTransitionsFailed);
	
	}

/**
 Test cancel after start
 Add four transitions and cancel before starting the ActiveSceduler
 One should start, three should be queued, and none should complete
 */
void CSsmSwpTransitionSchedulerTest::doTestCancelL( )
	{
	INFO_PRINTF1(_L("Checking scheduler cancel after start"));

	ResetCounters ( );

	__UHEAP_MARK;

	CSsmSwpTransitionScheduler* scheduler = CSsmSwpTransitionScheduler::NewLC ( );
	// add monitor for tracking counts
	scheduler->SetTransactionMonitor (this );

	TSsmSwp goodValue(KSwpTestKey1, ESwpGenericDoNothing);
	TSsmSwp badValue(KSwpTestKey1, ESwpGenericBadValueNotAllowed);

	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, badValue );
	CreateAndSubmitEngineL (scheduler, goodValue );

	scheduler->Cancel(NULL);

	CreateAndSubmitEngineL (scheduler, goodValue );
	CreateAndSubmitEngineL (scheduler, badValue );
	CreateAndSubmitEngineL (scheduler, badValue );
	CreateAndSubmitEngineL (scheduler, goodValue );

	iExpectedNoOfTransitions = 2;
	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack ( );
	//run engine
	iActiveScheduler->Start ( );

	//Verify that the expected calls were made 
	AssertCancel();

	CleanupStack::PopAndDestroy (scheduler );

	__UHEAP_MARKEND;
	}

void CSsmSwpTransitionSchedulerTest::AssertCancel()
	{
	TEST(iNumTransitionsQueued == 6);
	if (iNumTransitionsQueued != 6)
		INFO_PRINTF3(_L("iNumTransitionsQueued \t expected %d got %d"), 4 , iNumTransitionsQueued);
	
	TEST(iNumTransitionsStarted == 5);
	if (iNumTransitionsStarted != 5)
		INFO_PRINTF3(_L("iNumTransitionsStarted \t expected %d got %d"), 6 , iNumTransitionsStarted);
	
	TEST(iNumTransitionsCompleted == 2);
	if (iNumTransitionsCompleted != 2)
		INFO_PRINTF3(_L("iNumTransitionsCompleted \t expected %d got %d"), 2 , iNumTransitionsCompleted);
	
	TEST(iNumTransitionsFailed == 2);
	if (iNumTransitionsFailed != 2)
		INFO_PRINTF3(_L("iNumTransitionsFailed \t expected %d got %d"), 2 , iNumTransitionsFailed);
	
	}


void CSsmSwpTransitionSchedulerTest::CreateAndSubmitEngineL(CSsmSwpTransitionScheduler* aScheduler, TSsmSwp& aSwp )
	{
	CSsmSwpPolicyFrame* frame = GetSwpPolicyL (aSwp );
	CleanupStack::PushL (frame );

	CSsmSwpTransitionEngine* engine = CSsmSwpTransitionEngine::NewL (aSwp );
	engine->SetCleSessionProxy (this );
	engine->SetSwpPolicyFrame (frame );
	engine->PerformCommandListValidation(EFalse);

	CleanupStack::Pop (frame );
	CleanupStack::PushL (engine );
	aScheduler->SubmitL (engine );
	CleanupStack::Pop (engine );
	}

/**
 reset internal test counters
 */
void CSsmSwpTransitionSchedulerTest::ResetCounters( )
	{
	iExpectedNoOfTransitions =0;
	iNumTransitionsQueued = 0;
	iNumTransitionsStarted = 0;
	iNumTransitionsCompleted = 0;
	iNumTransitionsFailed = 0;
	}

//----------------- MPolicyResolverProxy interface implementation --------------------

CSsmSwpPolicyFrame* CSsmSwpTransitionSchedulerTest::CreatePolicyLC(const RLibrary& aLibrary) const
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
CSsmSwpPolicyFrame* CSsmSwpTransitionSchedulerTest::GetSwpPolicyL(const TSsmSwp& aSwp)
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
void CSsmSwpTransitionSchedulerTest::ExecuteCommandList(
		const CSsmCommandList& /*aCmdList*/, TRequestStatus& aStatus, TCmdErrorSeverity& /*aSeverity*/ )
	{
	TRequestStatus* tmp = &aStatus;
	User::RequestComplete (tmp, KErrNone );
	}

void CSsmSwpTransitionSchedulerTest::ExecuteCommandListCancel( )
	{
	}

/** Function needed when using real SysCle but not needed for this test*/
void CSsmSwpTransitionSchedulerTest::ConnectL( )
	{
	}
void CSsmSwpTransitionSchedulerTest::ReleaseCle( )
	{
	}
void CSsmSwpTransitionSchedulerTest::Close()
	{
	}
TBool CSsmSwpTransitionSchedulerTest::IsConnected() 
	{
	return EFalse;
	}

MCleSessionProxy* CSsmSwpTransitionSchedulerTest::CreateNewProxyL()
	{
	return this;
	}

//--------------------- MSwpSchedulerMonitor interface implementation -----------------------
/**
 count the number of started transitions
 */
void CSsmSwpTransitionSchedulerTest::SwpTransitionQueued( )
	{
	iNumTransitionsQueued++;
	}

/**
 count the number of started transitions
 */
void CSsmSwpTransitionSchedulerTest::SwpTransitionStarted( )
	{
	iNumTransitionsStarted++;
	}

/**
 count the number of completed transitions
 */
void CSsmSwpTransitionSchedulerTest::SwpTransitionCompleted(TUint aNumberCompleted)
	{
	(void)aNumberCompleted;
	iNumTransitionsCompleted++;
	}

void CSsmSwpTransitionSchedulerTest::SwpTransitionFailed(TUint aNumberFailed)
	{
	(void)aNumberFailed;
	iNumTransitionsFailed++;
	}


