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


#include <ssm/ssmstate.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstatetransition.h>
#include "ssmserverpanic.h"
#include "ssmstatepolicyresolver.h"
#include "ssmstatepolicyframe.h"
#include "ssmstatetransitionengine.h"
#include "clesessionproxy.h"

#include "ssmatest_utils.h"
#include "tssm_step_validstatelist.h"
#include "tssm_statepolicy_invalidlist.h"


const TInt KTestInvalidPanicCategory = -988;


//----------------------------------------------------------------------------------------------------------------
//---------------------------------------- Thread creation framework ---------------------------------------------
//----------------------------------------------------------------------------------------------------------------


/**
Test framework to check for panic scenarios
It requires to create a separate thread so we can check the panic category and code.
*/
TInt StartStateInvalidListInThreadL(TInt aFunction)
	{
	RThread thread;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNamePrefix, "SsmTestThread");
	RBuf threadName;
	CleanupClosePushL(threadName);
	threadName.CreateL(KThreadNamePrefix().Length() + 6); // 6 digit thread number
	threadName = KThreadNamePrefix;
	threadName.AppendNumFixedWidth(aFunction, EDecimal, 6);
	const TInt KMinHeapSize = 0xc800; // 50k - NOTE just an arbitrary value, please feel free to change it
	const TInt KMaxHeapSize = 0x19000; // 100k - NOTE just an arbitrary value, please feel free to change it
	User::LeaveIfError(thread.Create(threadName, ThreadStartStateInvalidListFn, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, (TAny*)aFunction));
	CleanupStack::PopAndDestroy(&threadName);
	TRequestStatus status;
	thread.Logon(status);
	TBool jit =	User::JustInTime();
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(status);

	// always expecting a state transition engine panic
	TExitCategoryName category = thread.ExitCategory();
	if (category.Compare(KPanicSysStateMgr) != 0)
		{
		User::Leave(KTestInvalidPanicCategory);
		}
	const TInt exitReason = thread.ExitReason();
	thread.Close();
	User::SetJustInTime(jit);

	// return the exit reason for the caller to verify the expected panic value
	return exitReason;
	}

TInt ThreadStartStateInvalidListFn(TAny* aPtr)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	TInt function = (TInt)aPtr;
	TRAPD(err, ThreadDispatchStateInvalidListFunctionL(function));

	delete trapCleanup;
	return err;
	}


void ThreadDispatchStateInvalidListFunctionL(TInt aFunction)
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// Create a state policy resolver and load the initial policy
	CSsmStatePolicyResolver* resolver = CSsmStatePolicyResolver::NewL();
	CleanupStack::PushL(resolver);
	const TSsmState state = TSsmState(ESsmTestInvalidListState, KSsmAnySubState);
	resolver->GetStatePolicyL(state);

	// Create a cle session proxy
	CCleSessionProxy* cleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(cleSession);

	// Create the state transition engine and let it take ownership of resolver and cleSession
	CSsmStateTransitionEngine* stateTransitionEngine = CSsmStateTransitionEngine::NewL(resolver, cleSession);
	stateTransitionEngine->PerformCommandListValidation(ETrue);
	CleanupStack::Pop(cleSession);
	CleanupStack::Pop(resolver);
	CleanupStack::PushL(stateTransitionEngine);

	//Request transition according to the reason action, which will define the invalid list
	const TInt reason = aFunction;
	const TSsmStateTransition request(state, reason);
	stateTransitionEngine->SubmitL(request);

	sched->Start();

	CleanupStack::PopAndDestroy(2, sched);
	}



//---------------- CSsmValidStateListTest step ---------------------------------------


CSsmValidStateListTest::~CSsmValidStateListTest()
	{
	}

CSsmValidStateListTest::CSsmValidStateListTest()
	{
	SetTestStepName(KTSsmValidStateListStep);
	}

TVerdict CSsmValidStateListTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmValidStateListTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0011	
New Test CaseID 		DEVSRVS-SSMA-SSM-0011
 */

TVerdict CSsmValidStateListTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmValidStateListTest started...."));

	__UHEAP_MARK;

	INFO_PRINTF1(_L("Checking cmd list without a Publish System State command"));
	TInt exitReason(0);
	TRAPD(err, exitReason = StartStateInvalidListInThreadL(EStateCmdWithoutPublishState));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartStateInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESsmStateEngineError9);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESsmStateEngineError9);

	INFO_PRINTF1(_L("Checking cmd list with more than one Publish System State command"));
	exitReason=0;
	TRAP(err, exitReason = StartStateInvalidListInThreadL(EStateCmdTwoPublishState));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartStateInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESsmStateEngineError9);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESsmStateEngineError9);

	INFO_PRINTF1(_L("Checking cmd list with a Publish System-Wide Property"));
	exitReason=0;
	TRAP(err, exitReason = StartStateInvalidListInThreadL(EStateCmdPublishSwp));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartStateInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESsmStateEngineError9);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESsmStateEngineError9);

	INFO_PRINTF1(_L("Checking cmd list with no Multiple Wait command and more than one deferred commands"));
	exitReason=0;
	TRAP(err, exitReason = StartStateInvalidListInThreadL(EStateCmdNoMultipleWait));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartStateInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESsmStateEngineError9);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESsmStateEngineError9);

	INFO_PRINTF1(_L("Checking cmd list with too many Multiple Wait command and no deferred commands - Validation should be OK - Panic comes from GetNextState"));
	exitReason=0;
	TRAP(err, exitReason = StartStateInvalidListInThreadL(EStateCmdTooManyMultipleWaitErr1));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == KSsmTestAppRvError);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, KSsmTestAppRvError);

	INFO_PRINTF1(_L("Checking cmd list with too many Multiple Wait command and no deferred commands - Validation should be OK - Panic originates in GetNextState returning an unknown state"));
	exitReason=0;
	TRAP(err, exitReason = StartStateInvalidListInThreadL(EStateCmdTooManyMultipleWaitErr2));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESsmStateEngineError7);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESsmStateEngineError7);

	INFO_PRINTF1(_L("Checking cmd list which leaves with an error when it is being prepared"));
	exitReason=0;
	TRAP(err, exitReason = StartStateInvalidListInThreadL(EStateCmdPrepareCmdListFailed));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESsmStateEngineError8);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESsmStateEngineError8);

	__UHEAP_MARKEND;

	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();

	INFO_PRINTF1(_L("....CSsmValidStateListTest completed!!"));
	return TestStepResult();
	}


