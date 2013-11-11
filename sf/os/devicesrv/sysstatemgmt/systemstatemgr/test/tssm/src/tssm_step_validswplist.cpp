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
#include "ssmserverpanic.h"
#include "ssmswppolicyframe.h"
#include "ssmswppolicyresolver.h"
#include "ssmswptransitionengine.h"
#include "ssmswptransitionscheduler.h"
#include "ssmswprequesthandler.h"
#include "clesessionproxy.h"

#include "ssmatest_utils.h"
#include "tssm_step_validswplist.h"
#include "tssm_swppolicy_invalidlist.h"


const TInt KTestInvalidPanicCategory = -988;
// property uids should be real uids (use these temporarily for test purposes)
const TUint key={0x01};
_LIT(KSsmSwpPolicyServerName, "TestSsmSwpPolicyServer");

//----------------------------------------------------------------------------------------------------------------
//---------------------------------------- Thread creation framework ---------------------------------------------
//----------------------------------------------------------------------------------------------------------------


/**
Test framework to check for panic scenarios
It requires to create a separate thread so we can check the panic category and code.
*/
TInt StartSwpInvalidListInThreadL(CSsmValidSwpListTest* aSsmValidSwpListTest)
	{
	RThread thread;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNamePrefix, "SsmTestThread");

	RBuf threadName;
	CleanupClosePushL(threadName);
	threadName.CreateL(KThreadNamePrefix().Length() + 6); // 6 digit thread number
	threadName = KThreadNamePrefix;
	threadName.AppendNumFixedWidth(aSsmValidSwpListTest->Function(), EDecimal, 6);
	const TInt KMinHeapSize = 0xc800; // 50k - NOTE just an arbitrary value, please feel free to change it
	const TInt KMaxHeapSize = 0x19000; // 100k - NOTE just an arbitrary value, please feel free to change it
	User::LeaveIfError(thread.Create(threadName, ThreadStartSwpInvalidListFn, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, aSsmValidSwpListTest));
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

TInt ThreadStartSwpInvalidListFn(TAny* aPtr)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CSsmValidSwpListTest* ssmValidSwpListTest = static_cast<CSsmValidSwpListTest*>(aPtr);
	TRAPD(err, ThreadDispatchSwpInvalidListFunctionL(ssmValidSwpListTest));

	delete trapCleanup;
	return err;
	}

void ThreadDispatchSwpInvalidListFunctionL(CSsmValidSwpListTest* aSsmValidSwpListTest)
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// Create a swp policy resolver and register our property
	CSsmSwpPolicyResolver* resolver = CSsmSwpPolicyResolver::NewL();
	CleanupStack::PushL(resolver);
	resolver->RegisterSwpMappingL(key, KTestSwpPolicyInvalidListFile);


	// Create a cle session proxy
	CCleSessionProxy* cleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(cleSession);
	cleSession->ConnectL();

	// Create the swp request handler and wire it up to the proxies
	CSsmSwpRequestHandler* handler = CSsmSwpRequestHandler::NewL();
	handler->SetSwpPolicyResolverProxy(resolver);
	handler->SetCleSessionProxy(cleSession);
	CleanupStack::Pop(cleSession);
	CleanupStack::Pop(resolver);
	CleanupStack::PushL(handler);

	//To get the handle to testssmswppolicyserver, since we have to test the exit reason
	if (aSsmValidSwpListTest->Function() == ESwpCmdTooManyMultipleWaitErr1)
		aSsmValidSwpListTest->GetSsmSwpPolicyServerThreadIdL();

	//Request transition according to the reason action, which will define the invalid list
	const TSsmSwp swp(key, aSsmValidSwpListTest->Function());
	
	handler->SubmitRequestL(swp);

	sched->Start();

	CleanupStack::PopAndDestroy(3, sched);
	}



//---------------- CSsmValidSwpListTest step ---------------------------------------


CSsmValidSwpListTest::~CSsmValidSwpListTest()
	{
	iThread.Close();
	}

CSsmValidSwpListTest::CSsmValidSwpListTest()
	{
	SetTestStepName(KTSsmValidSwpListStep);
	}

TVerdict CSsmValidSwpListTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmValidSwpListTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TInt CSsmValidSwpListTest::Function()
	{
	return iFunction;
	}

void CSsmValidSwpListTest::SetFunction(TInt aFunction)
	{
	iFunction = aFunction;
	}

void CSsmValidSwpListTest::GetSsmSwpPolicyServerThreadIdL()
	{
	TPtrC ptr(KSsmSwpPolicyServerName());
	TFullName procSerchTerm(_L("*"));
	procSerchTerm += ptr;
	ptr.Set(ptr.Ptr(), KSsmSwpPolicyServerName().Length());
	procSerchTerm += _L("*");

	TFindThread find(procSerchTerm);
	TFullName name;
	TInt err = find.Next(name);
	TEST(err == KErrNone);
	User::LeaveIfError(err);

	err = iThread.Open(find);
	TEST(err == KErrNone);
	User::LeaveIfError(err);
	}

/**
Old Test CaseID 		APPFWK-SSM-0012
New Test CaseID 		DEVSRVS-SSMA-SSM-0012
 */
TVerdict CSsmValidSwpListTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmValidSwpListTest started...."));

	__UHEAP_MARK;

	TInt err = RProperty::Define(RProcess().SecureId(), key, RProperty::EInt);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	INFO_PRINTF1(_L("Checking cmd list without a Publish System Swp command"));
	TInt exitReason(0);
	SetFunction(ESwpCmdWithoutPublishSwp);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError16);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError16);

	INFO_PRINTF1(_L("Checking cmd list with more than one Publish System Swp command"));
	exitReason=0;
	SetFunction(ESwpCmdTwoPublishSwp);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError16);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError16);

	INFO_PRINTF1(_L("Checking cmd list with a Publish System State"));
	exitReason=0;
	SetFunction(ESwpCmdPublishState);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError16);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError16);

	INFO_PRINTF1(_L("Checking cmd list with no Multiple Wait command and more than one deferred commands"));
	exitReason=0;
	SetFunction(ESwpCmdNoMultipleWait);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError16);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError16);

	INFO_PRINTF1(_L("Checking cmd list with too many Multiple Wait command and and no deferred commands - Validation should be OK - Panic comes from HandleCleReturnValue"));
	exitReason=0;
	SetFunction(ESwpCmdTooManyMultipleWaitErr1);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	const TInt threadExitReason = iThread.ExitReason();
	TBuf<16> threadExitCategory = iThread.ExitCategory();
	// This is the exit reason for the ssmswppolicyserver
	TEST(threadExitReason == KSsmTestAppRvError);
	// this is the exit reason for the engine.
	TEST(exitReason == ESwpTransitionEngineError20);
	iThread.Close();
	INFO_PRINTF4(_L("    -- received panic '%d', category '%S', expected was '%d'."), threadExitReason, &threadExitCategory, KSsmTestAppRvError);

	INFO_PRINTF1(_L("Checking cmd list with too many Multiple Wait command and and no deferred commands - Validation should be OK - Panic originates in HandleCleReturnValue returning an error"));
	exitReason=0;
	SetFunction(ESwpCmdTooManyMultipleWaitErr2);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError20);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError20);

	INFO_PRINTF1(_L("Checking cmd list which leaves with an error when it is being prepared"));
	exitReason=0;
	SetFunction(ESwpCmdPrepareCmdListFailed);
	TRAP(err, exitReason = StartSwpInvalidListInThreadL(this));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("    -- StartSwpInvalidListInThreadL method completed with '%d'."), err);
	TEST(exitReason == ESwpTransitionEngineError20);
	INFO_PRINTF3(_L("    -- received panic '%d', expected was '%d'."), exitReason, ESwpTransitionEngineError20);
	
	err = RProperty::Delete(RProcess().SecureId(), key);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();

	INFO_PRINTF1(_L("....CSsmValidSwpListTest completed!!"));
	return TestStepResult();
	}


