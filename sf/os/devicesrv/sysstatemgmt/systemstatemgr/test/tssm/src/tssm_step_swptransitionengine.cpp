// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmatest_utils.h"
#include "tssm_step_swptransitionengine.h"
#include "tssm_swppolicy_generic.h"
#include "tssm_swppolicy_generic.hrh"

#include <e32debug.h>

// Keys and values used for testing the transition engines
static const TUint KSwpTestKey1 = 0xE5325221;

const TInt KTestErrorBadCategory = -911;
const TInt KTestErrorBadFunction = -912;

/**
*/
static TInt StopScheduler(TAny* aSsmSwpTransitionEngineTest)
	{
	CSsmSwpTransitionEngineTest*  ssmSwpRequestEngineTest = static_cast<CSsmSwpTransitionEngineTest*>(aSsmSwpTransitionEngineTest);
	ssmSwpRequestEngineTest->StopActiveScheduler();
	return KErrNone;
	}

//--------------------- Thread creation framework ---------------------

/**
Test framework to check for panic scenarios
It requires to create a separate thread so we can check the panic category and code.
*/
TInt StartTestInThreadL(TInt aFunction)
	{
	RThread thread;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNamePrefix, "SsmTestThread");
	RBuf threadName;
	CleanupClosePushL(threadName);
	threadName.CreateL(KThreadNamePrefix().Length() + 4); // 4 digit thread number
	threadName = KThreadNamePrefix;
	threadName.AppendNumFixedWidth(aFunction, EDecimal, 4);
	const TInt KMinHeapSize = 0xc800; // 50k - NOTE just an arbitrary value, please feel free to change it
	const TInt KMaxHeapSize = 0x19000; // 100k - NOTE just an arbitrary value, please feel free to change it
	User::LeaveIfError(thread.Create(threadName, ThreadStartFn, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, (TAny*)aFunction));
	CleanupStack::PopAndDestroy(&threadName);
	TRequestStatus status;
	thread.Logon(status);
	TBool jit =	User::JustInTime();
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(status);

	// always expecting a transition engine panic
	TExitCategoryName category = thread.ExitCategory();
	RDebug::Print(_L("***** Exit Category = %S *****"),&category);
	if (category.Compare(KPanicSysStateMgr) != 0)
		{
		User::Leave(KTestErrorBadCategory);
		}
	const TInt exitReason = thread.ExitReason();
	thread.Close();
	User::SetJustInTime(jit);

	// return the exit reason for the caller to verify the expected panic value
	return exitReason;
	}

TInt ThreadStartFn(TAny* aPtr)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	TInt function = (TInt)aPtr;
	TRAPD(err, ThreadDispatchFunctionL(function));

	delete trapCleanup;
	return err;
	}


void ThreadDispatchFunctionL(TInt aFunction)
	{
	switch (aFunction)
		{
		case EThreadSubmitTwice:
			ThreadSubmitTwiceL();
			break;
		default:
			User::Leave(KTestErrorBadFunction);
			break;
		}
	}


void ThreadSubmitTwiceL()
	{
	CSsmSwpTransitionEngineTest* step = new (ELeave) CSsmSwpTransitionEngineTest();

	CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(activeScheduler);
	CActiveScheduler::Install(activeScheduler);

	CAsyncCallBack* asyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, step);
	asyncStopScheduler->Set(stop);
	CleanupStack::Pop(activeScheduler);

	// ssmswppolicyproxy is created and set by the individualtransitionscheduler, since
	// we are submitting the request directly to the engine the ssmswppolicyproxy should be
	// created and set in the engine
	CSsmSwpPolicyCliSession* ssmSwpPolicySession = CSsmSwpPolicyCliSession::NewL();
	CleanupStack::PushL(ssmSwpPolicySession);

	// Setting the cle session as we have to execute the publishswp command.
	// published swp value is used to stop the scheduler.
	CCleSessionProxy* cleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(cleSession);

	const TSsmSwp swpValue(KSwpTestKey1, ESwpGenericDoNothing);
	CSsmSwpPolicyFrame* policy = step->GetSwpPolicyL(swpValue);
	CleanupStack::PushL(policy);

	CSsmSwpTransitionEngine* engine = CSsmSwpTransitionEngine::NewL(swpValue);
	engine->SetCleSessionProxy(cleSession);
	engine->SetSwpPolicyFrame(policy);
	engine->SetSsmSwpPolicySession(ssmSwpPolicySession);
	CleanupStack::Pop(policy);

	CleanupStack::PushL(engine);
	engine->Submit();
	engine->Submit();
	// etc, etc, etc..
	// don't bother to continue here. All being well the second SubmitL will cause a panic!
	//Cleanup test
	CleanupStack::PopAndDestroy(3, ssmSwpPolicySession);	// and engine
	}


//----------------- CTestStep implemenation --------------------

/**
*/
CSsmSwpTransitionEngineTest::~CSsmSwpTransitionEngineTest()
	{
	iProperty.Close();
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	}

/**
*/
CSsmSwpTransitionEngineTest::CSsmSwpTransitionEngineTest()
	{
	SetTestStepName(KTSsmSwpTransitionEngineStep);
	}

/**
*/
TVerdict CSsmSwpTransitionEngineTest::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set(stop);

	return CTestStep::doTestStepPreambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0007
New Test CaseID 		DEVSRVS-SSMA-SSM-0007
 */

TVerdict CSsmSwpTransitionEngineTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpTransitionEngineTest started...."));

	__UHEAP_MARK;

	doTestSwpTransitionEngineTestForMemoryLeaksL();
	doTestRequestSimpleL();
	doTestRequestCancelL();
	doTestRequestSubmitTwiceL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpTransitionEngineTest completed!!"));
	return TestStepResult();
	}

/**
*/
TVerdict CSsmSwpTransitionEngineTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
*/
void CSsmSwpTransitionEngineTest::doTestSwpTransitionEngineTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmSwpTransitionEngine"));

	__UHEAP_MARK;

	// ssmswppolicyproxy is created and set by the individualtransitionscheduler, since
	// we are submitting the request directly to the engine the ssmswppolicyproxy should be
	// created and set in the engine
	CSsmSwpPolicyCliSession* ssmSwpPolicySession = CSsmSwpPolicyCliSession::NewL();
	CleanupStack::PushL(ssmSwpPolicySession);

	// Setting the cle session as we have to execute the publishswp command.
	// published swp value is used to stop the scheduler.
	CCleSessionProxy* cleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(cleSession);

	const TSsmSwp swpValue(KSwpTestKey1, ESwpGenericDoNothing);
	CSsmSwpPolicyFrame* policy = this->GetSwpPolicyL(swpValue);
	CleanupStack::PushL(policy);

	CSsmSwpTransitionEngine* engine = CSsmSwpTransitionEngine::NewL(swpValue);
	engine->SetCleSessionProxy(cleSession);
	engine->SetSwpPolicyFrame(policy);
	engine->SetSsmSwpPolicySession(ssmSwpPolicySession);
	CleanupStack::Pop(policy);
	
	//Cleanup test
	CleanupStack::PopAndDestroy(2, ssmSwpPolicySession);
	delete engine;

	__UHEAP_MARKEND;
	}

/**
Takes CSsmSwpTransitionEngine through the simple swp transition.
*/
void CSsmSwpTransitionEngineTest::doTestRequestSimpleL()
	{
	INFO_PRINTF1(_L("Checking requesthandler with a single request"));

	__UHEAP_MARK;

	// ssmswppolicyproxy is created and set by the individualtransitionscheduler, but
	// we are submitting the request directly to the engine the ssmswppolicyproxy should be
	// created and set in the engine
	CSsmSwpPolicyCliSession* ssmSwpPolicySession = CSsmSwpPolicyCliSession::NewL();
	CleanupStack::PushL(ssmSwpPolicySession);

	// Setting the cle session as we have to execute the publishswp command.
	// published swp value is used to stop the scheduler.
	CCleSessionProxy* cleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(cleSession);

	// Define and set the rproperty
	TInt err = (iProperty.Define(RProcess().SecureId(), KSwpTestKey1, RProperty::EInt));
	INFO_PRINTF2(_L("doTestRequestSimpleL :: Define KSwpTestKey1 returns : %d "), err);
	TEST ((KErrNone == err) ||(KErrAlreadyExists == err));
	TEST(KErrNone == iProperty.Set(RProcess().SecureId(), KSwpTestKey1, 0));

	TInt value;
	TEST(KErrNone == iProperty.Get(RProcess().SecureId(), KSwpTestKey1, value));
	TEST(value == 0);

	const TSsmSwp swpValue(KSwpTestKey1, ESwpGenericDoNothing);
	CSsmSwpPolicyFrame* policy = this->GetSwpPolicyL(swpValue);
	CleanupStack::PushL(policy);
	
	CSsmSwpTransitionEngine* engine = CSsmSwpTransitionEngine::NewL(swpValue);
	engine->SetCleSessionProxy(cleSession);
	engine->SetSwpPolicyFrame(policy);
	engine->SetSsmSwpPolicySession(ssmSwpPolicySession);
	engine->PerformCommandListValidation(EFalse);
	CleanupStack::Pop(policy);

	CleanupStack::PushL(engine);
	engine->Submit();

	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack();
	//run engine
	iActiveScheduler->Start();

	TEST(KErrNone == iProperty.Get(RProcess().SecureId(), KSwpTestKey1, value));
	TEST(value == ESwpGenericDoNothing);

	TEST(KErrNone == iProperty.Delete(RProcess().SecureId(), KSwpTestKey1));

	//Cleanup test
	CleanupStack::PopAndDestroy(3, ssmSwpPolicySession);	// and engine

	__UHEAP_MARKEND;
	}

/**
Takes CSsmSwpTransitionEngine through the simple swp transition and cancels it.
*/
void CSsmSwpTransitionEngineTest::doTestRequestCancelL()
	{
	INFO_PRINTF1(_L("Checking requesthandler with a cancel request"));

	__UHEAP_MARK;

	// ssmswppolicyproxy is created and set by the individualtransitionscheduler, but
	// we are submitting the request directly to the engine the ssmswppolicyproxy should be
	// created and set in the engine
	CSsmSwpPolicyCliSession* ssmSwpPolicySession = CSsmSwpPolicyCliSession::NewL();
	CleanupStack::PushL(ssmSwpPolicySession);

	// Setting the cle session as we have to execute the publishswp command.
	// published swp value is used to stop the scheduler.
	CCleSessionProxy* cleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(cleSession);

	// Define and set the rproperty
	TEST(KErrNone == iProperty.Define(RProcess().SecureId(), KSwpTestKey1, RProperty::EInt));
	TEST(KErrNone == iProperty.Set(RProcess().SecureId(), KSwpTestKey1, -5));

	TInt value;
	TEST(KErrNone == iProperty.Get(RProcess().SecureId(), KSwpTestKey1, value));
	TEST(value == -5);

	const TSsmSwp swpValue(KSwpTestKey1, ESwpGenericDoNothing);
	CSsmSwpPolicyFrame* policy = this->GetSwpPolicyL(swpValue);
	CleanupStack::PushL(policy);

	CSsmSwpTransitionEngine* engine = CSsmSwpTransitionEngine::NewL(swpValue);
	engine->SetCleSessionProxy(cleSession);
	engine->SetSwpPolicyFrame(policy);
	engine->SetSsmSwpPolicySession(ssmSwpPolicySession);
	engine->PerformCommandListValidation(EFalse);
	CleanupStack::Pop(policy);

	CleanupStack::PushL(engine);
	engine->Submit();
	engine->Cancel();

	//this callback will not run until the test is finished because it has priority idle
	iAsyncStopScheduler->CallBack();
	//run engine
	iActiveScheduler->Start();

	TEST(KErrNone == iProperty.Get(RProcess().SecureId(), KSwpTestKey1, value));
	TEST(value == -5);

	TEST(KErrNone == iProperty.Delete(RProcess().SecureId(), KSwpTestKey1));

	//Cleanup test
	CleanupStack::PopAndDestroy(3, ssmSwpPolicySession);	// and engine

	__UHEAP_MARKEND;
	}

void CSsmSwpTransitionEngineTest::doTestRequestSubmitTwiceL()
	{
	INFO_PRINTF1(_L("Checking requesthandler with two consecutive requests"));
	TInt exitReason(0);
	TRAPD(err, exitReason = StartTestInThreadL(EThreadSubmitTwice));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    3-- StartTestInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError3);
	INFO_PRINTF3(_L("    3-- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError3);

	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();
	}

void CSsmSwpTransitionEngineTest::StopActiveScheduler()
	{
	// We would stop the sceduler once all the required transitions are completed
	//Verify that the expected calls were made
	TInt value;
	TEST(KErrNone == iProperty.Get(RProcess().SecureId(), KSwpTestKey1, value));

	// We have to stop the scheduler in case of Cancel test code, so testing property's initial value.
	if (value == ESwpGenericDoNothing || value == -5)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		}
	}

//----------------- MPolicyResolverProxy implementation --------------------

CSsmSwpPolicyFrame* CSsmSwpTransitionEngineTest::CreatePolicyLC(const RLibrary& aLibrary) const
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
CSsmSwpPolicyFrame* CSsmSwpTransitionEngineTest::GetSwpPolicyL(const TSsmSwp& aSwp)
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




