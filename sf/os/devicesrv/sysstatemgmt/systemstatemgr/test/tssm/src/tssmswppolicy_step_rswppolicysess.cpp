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
#include "ssmdebug.h"
#include "ssmserverpanic.h"
#include "ssmswppolicyresolver.h"
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmcommandlist.h>
#include "ssmatest_utils.h"

#include "tssmswppolicy_step_rswppolicysess.h"

#include <e32property.h>

const TUint KSwpTestKey = 10;
const TInt KSwpTestKey_Value = -512;	// Random value for the swp key, no specific reason of being a -ve no.

const TInt KTestRSsmSwpPolicySessionErr = -1234;	//Random no. used as error code
const TInt KTestRSsmSwpPolicySessionPanic = 1011;	//Same Error code as used in tssm_swppolicy_common (ESwpCommonTestFailed2)
const TInt KSwpSeverity = -512;	// Random no.

_LIT(KTestSwpPolicyCommonFile, "ssm.swp.policy.test.common.dll");
_LIT(KSsmSwpPolicyTestServerName, "TestSsmSwpPolicyServer");

CSwpPolicyStepTest* CSwpPolicyStepTest::NewL(CSsmSwpRSwpPolicySessionTest& aSwpRSwpPolicySessionTest, RSsmSwpPolicySessionTest& aSsmSwpPolicyTestCli, const TSsmSwp& aSwp, TInt aLibraryHandle)
	{
	CSwpPolicyStepTest* self = new(ELeave)CSwpPolicyStepTest(aSwpRSwpPolicySessionTest, aSsmSwpPolicyTestCli, aSwp, aLibraryHandle);
	return self;
	}

CSwpPolicyStepTest::CSwpPolicyStepTest(CSsmSwpRSwpPolicySessionTest& aSwpRSwpPolicySessionTest, RSsmSwpPolicySessionTest& aSsmSwpPolicyTestCli, const TSsmSwp& aSwp, TInt aLibraryHandle)
			: CActive(EPriorityStandard), iSwpRSwpPolicySessionTest(aSwpRSwpPolicySessionTest), iSsmSwpPolicyTestCli(aSsmSwpPolicyTestCli), iSwp(aSwp), iStep(ENull),iLibraryHandle(aLibraryHandle),iFailRate(0)
	{
	CActiveScheduler::Add(this);
	}

CSwpPolicyStepTest::~CSwpPolicyStepTest()
	{
	Cancel();
	iThread.Close();
	}

void CSwpPolicyStepTest::StartSwpPolicyTest(TestCondition aTestCondition, TTestType aTestType)
	{
	iTestCondition = aTestCondition;
	iTestType = aTestType;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CSwpPolicyStepTest::StartInitialize()
	{
	iStep = EInitialize;
	if(iSwpRSwpPolicySessionTest.iOOM)
		{
		iSsmSwpPolicyTestCli.HeapReset();
		iSsmSwpPolicyTestCli.SetHeapFailure(++iFailRate);
		}
	iSsmSwpPolicyTestCli.CallSetDllHandleAndInitialize(iLibraryHandle, iStatus);
	SetActive();
	}

void CSwpPolicyStepTest::StartPrepareCommandList()
	{
	iStep = EPrepareCommandList;
	iSsmSwpPolicyTestCli.CallPrepareCommandList(iSwp, iStatus);
	SetActive();
	}

void CSwpPolicyStepTest::StartHandleCleReturnValue(TInt aError, TInt aSeverity)
	{
	iStep = EHandleCleReturnValue;
	iSsmSwpPolicyTestCli.CallHandleCleReturnValue(iSwp, aError, aSeverity, iStatus);
	SetActive();
	}

void CSwpPolicyStepTest::RunL()
	{
	if (iStep == ENull)
		{
		iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNone, ENull);
		StartInitialize();
		if (iTestType == ETestSwpPolicyCancelInitialize)
			{
			Cancel();
			}
		}
	else if(iStep == EInitialize)
		{
		if((iStatus == KErrNoMemory) && iSwpRSwpPolicySessionTest.iOOM )
			{
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNoMemory, EInitialize);
			StartInitialize();
			if (iTestType == ETestSwpPolicyCancelInitialize)
				{
				Cancel();
				}
			}
		else
			{
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNone, EInitialize);
			StartPrepareCommandList();
			if (iTestType == ETestSwpPolicyCancelPrepareCommandList)
				{
				Cancel();
				}
			}
		}
	else if(iStep == EPrepareCommandList)
		{
		if((iStatus == KErrNoMemory) && iSwpRSwpPolicySessionTest.iOOM)
			{
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNoMemory, EPrepareCommandList);
			//Cancel the current request; reset the state to ENull and start from Initialise
			Cancel();
			StartInitialize();
			return;
			}
		iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNone, EPrepareCommandList);
		CSsmCommandList* ssmCommandList = iSsmSwpPolicyTestCli.CallCommandListL();
		if (ssmCommandList == NULL && iSwpRSwpPolicySessionTest.iOOM)
			{
			//Cancel the current request; reset the state to ENull and start from Initialise
			iSwpRSwpPolicySessionTest.DoTestResult(KErrNoMemory, KErrNoMemory, ECallCommandList);
			Cancel();
			StartInitialize();
			return;
			}
		const TInt count = ssmCommandList->Count();
		// We are expecting only one command in the commandlist
		if (count != 1)
			{
			User::Leave(KErrArgument);
			}
		delete ssmCommandList;
		iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNone, ECallCommandList);

		if (iTestCondition == EPanicKTestRSsmSwpPolicySession)
			{
			iSwpRSwpPolicySessionTest.GetSsmSwpServerThreadHandleL(iThread);
			StartHandleCleReturnValue(KTestRSsmSwpPolicySessionPanic, KSwpSeverity);
			}
		else if (iTestCondition == EErrorKTestRSsmSwpPolicySession)
			{
			StartHandleCleReturnValue(KTestRSsmSwpPolicySessionErr, KSwpSeverity);
			}
		else if (iTestCondition == EKErrNone || iTestCondition == EKErrNoneAndStopScheduler)
			{
			StartHandleCleReturnValue(KErrNone, KSwpSeverity);
			}

		if (iTestType == ETestSwpPolicyCancelHandleCleRetVal)
			{
			Cancel();
			}
		}
	else if(iStep == EHandleCleReturnValue)
		{
		if((iStatus == KErrNoMemory) && iSwpRSwpPolicySessionTest.iOOM)
			{
			//Cancel the current request; reset the state to ENull and start from Initialise
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNoMemory, EHandleCleReturnValue);
			Cancel();
			StartInitialize();
			return;
			}
		if (iTestCondition == EErrorKTestRSsmSwpPolicySession)
			{
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KTestRSsmSwpPolicySessionErr, EHandleCleReturnValue);
			}
		else if(iTestCondition == EKErrNone || iTestCondition == EKErrNoneAndStopScheduler)
			{
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNone, EHandleCleReturnValue);
			if (iTestCondition == EKErrNoneAndStopScheduler)
				{
				CActiveScheduler::Stop();
				}
			}
		else if(iTestCondition == EPanicKTestRSsmSwpPolicySession)
			{
			// In test condition where the HandleCleReturnValue panic's, the ssmswppolicyserver
			// will be terminated
			iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrServerTerminated, EHandleCleReturnValue);
			// In case the swppolicy panics ssmswppolicyserver returns KErrServerTerminated
			const TInt exitReason = iThread.ExitReason();
			iSwpRSwpPolicySessionTest.DoTestResult(exitReason, KTestRSsmSwpPolicySessionPanic, EHandleCleReturnValue);
			
			// We are stopping the scheduler here, as this is the last test case.
			CActiveScheduler::Stop();
			}

		iStep = ENull;
		}
	else
		{
		iStep = ENull;
		}
	}

void CSwpPolicyStepTest::DoCancel()
	{
	if(iStep == EInitialize)
		{
		iSsmSwpPolicyTestCli.CallInitializeCancel();
		}
	else if(iStep == EPrepareCommandList)
		{
		iSsmSwpPolicyTestCli.CallPrepareCommandListCancel();
		}
	else if(iStep == EHandleCleReturnValue)
		{
		iSsmSwpPolicyTestCli.CallHandleCleReturnValueCancel();
		}
	iStep = ENull;
	// We are stopping the scheduler here, as this is the last test case for testing cancel methods
	if (iTestType == ETestSwpPolicyCancelHandleCleRetVal)
		CActiveScheduler::Stop();
	}

TInt CSwpPolicyStepTest::RunError()
	{
	iSwpRSwpPolicySessionTest.DoTestResult(iStatus.Int(), KErrNone, iStep);
	return KErrNone;
	}

CSsmSwpRSwpPolicySessionTest::~CSsmSwpRSwpPolicySessionTest()
	{
	iLibrary.Close();
	delete iActiveScheduler;
	}

CSsmSwpRSwpPolicySessionTest::CSsmSwpRSwpPolicySessionTest()
	{
	iOOM = 0;
	SetTestStepName(KTSsmSwpRSwpPolicySessionStep);
	}

TVerdict CSsmSwpRSwpPolicySessionTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmSwpRSwpPolicySessionTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CSsmSwpRSwpPolicySessionTest::DoTestResult(TInt aActualResult, TInt aExpectedResult, TInt aStep)
	{
	INFO_PRINTF4(_L("For %d Step : Actual Result : %d - Expected Result : %d"), aStep, aActualResult, aExpectedResult);
	TEST(aActualResult == aExpectedResult);
	}

// In the cases where ssmswpserver would panic, we need to get the handle to the server
// in order to test the exit reason.
void CSsmSwpRSwpPolicySessionTest::GetSsmSwpServerThreadHandleL(RThread& aThread)
	{
	TPtrC ptr(KSsmSwpPolicyTestServerName());
	TFullName procSerchTerm(_L("*"));
	procSerchTerm += ptr;
	ptr.Set(ptr.Ptr(), KSsmSwpPolicyTestServerName().Length());
	procSerchTerm += _L("*");

	TFindThread find(procSerchTerm);
	TFullName name;
	TInt err = find.Next(name);
	TEST(err == KErrNone);
	User::LeaveIfError(err);

	err = aThread.Open(find);
	TEST(err == KErrNone);
	User::LeaveIfError(err);
	}

void CSsmSwpRSwpPolicySessionTest::GetPolicyResolverL(RLibrary& aLibrary, const TDesC& aLibraryFilename)
	{
	const TInt fileErr = aLibrary.Load(aLibraryFilename);
	if (fileErr != KErrNone)
		{
		INFO_PRINTF3(_L("Failed to load library file %S, file error-code: %d"), &aLibraryFilename, fileErr);
		User::Leave(fileErr);
		}
	if (aLibrary.Type()[1] != KSsmSwpPolicyDllTypeUid)
		{
		INFO_PRINTF4(_L("Wrong type (uid2) in swp policy library dll %S. Expected %x found %x"),
					&aLibraryFilename, KSsmSwpPolicyDllTypeUid, aLibrary.Type()[1]);
		User::Leave(KErrNotSupported);
		}
	}

void CSsmSwpRSwpPolicySessionTest::DoTestSsmSwpPolicyCliL(TTestType aTestType)
	{
	RSsmSwpPolicySessionTest ssmSwpPolicyCli;
	INFO_PRINTF1(_L("Connect first RSsmSwpPolicyCli client to server"));
	ssmSwpPolicyCli.ConnectL(KSsmSwpPolicyTestServerName);
	CleanupClosePushL(ssmSwpPolicyCli);

	TSsmSwp swp(KSwpTestKey, KSwpTestKey_Value);
	INFO_PRINTF1(_L("Create first active object to test the methods of RSsmSwpPolicySession API"));
	CSwpPolicyStepTest* swpPolicyStepTest = CSwpPolicyStepTest::NewL(*this, ssmSwpPolicyCli, swp, iLibrary.Handle());
	CleanupStack::PushL(swpPolicyStepTest);

	RSsmSwpPolicySessionTest ssmSwpPolicyCli1;
	INFO_PRINTF1(_L("Connect second RSsmSwpPolicyCli client to server"));
	ssmSwpPolicyCli1.ConnectL(KSsmSwpPolicyTestServerName);
	CleanupClosePushL(ssmSwpPolicyCli1);

	INFO_PRINTF1(_L("Create second active object to test the methods of RSsmSwpPolicySession API"));
	CSwpPolicyStepTest* swpPolicyStepTest1 = CSwpPolicyStepTest::NewL(*this, ssmSwpPolicyCli1, swp, iLibrary.Handle());
	CleanupStack::PushL(swpPolicyStepTest1);

	RSsmSwpPolicySessionTest ssmSwpPolicyCli2;
	INFO_PRINTF1(_L("Connect third RSsmSwpPolicyCli client to server"));
	ssmSwpPolicyCli2.ConnectL(KSsmSwpPolicyTestServerName);
	CleanupClosePushL(ssmSwpPolicyCli2);

	INFO_PRINTF1(_L("Create third active object to test the methods of RSsmSwpPolicySession API"));
	CSwpPolicyStepTest* swpPolicyStepTest2 = CSwpPolicyStepTest::NewL(*this, ssmSwpPolicyCli2, swp, iLibrary.Handle());
	CleanupStack::PushL(swpPolicyStepTest2);

	if (aTestType == ETestCompletePolicySuccessfully)
		{
		INFO_PRINTF1(_L("Test HandleCleReturnValue method which completes with KErrNone"));
		swpPolicyStepTest->StartSwpPolicyTest(EKErrNone, ETestCompletePolicySuccessfully);
	
		INFO_PRINTF1(_L("Test HandleCleReturnValue method which completes with error"));
		swpPolicyStepTest1->StartSwpPolicyTest(EErrorKTestRSsmSwpPolicySession, ETestCompletePolicySuccessfully);
	
		INFO_PRINTF1(_L("Test HandleCleReturnValue method which completes with panic"));
		swpPolicyStepTest2->StartSwpPolicyTest(EPanicKTestRSsmSwpPolicySession, ETestCompletePolicySuccessfully);
		}
	else
		{
		INFO_PRINTF1(_L("Test CallInitialize and CallInitializeCancel methods"));
		swpPolicyStepTest->StartSwpPolicyTest(EKErrNone, ETestSwpPolicyCancelInitialize);
	
		INFO_PRINTF1(_L("Test CallPrepareCommandList and CallPrepareCommandListCancel methods"));
		swpPolicyStepTest1->StartSwpPolicyTest(EKErrNone, ETestSwpPolicyCancelPrepareCommandList);
	
		INFO_PRINTF1(_L("Test CallHandleCleReturnValue and CallHandleCleReturnValueCancel methods"));
		swpPolicyStepTest2->StartSwpPolicyTest(EKErrNone, ETestSwpPolicyCancelHandleCleRetVal);
		}
	iActiveScheduler->Start();
	
	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();

	//For cleanup of the server resources
	User::After(1*1000*1000);
	CleanupStack::PopAndDestroy(6, &ssmSwpPolicyCli);
	}

void CSsmSwpRSwpPolicySessionTest::DoTestErrorCondL(TTestType aTestType)
	{
	RSsmSwpPolicySessionTest ssmSwpPolicyCli;
	ssmSwpPolicyCli.ConnectL(KSsmSwpPolicyTestServerName);
	CleanupClosePushL(ssmSwpPolicyCli);

	RThread thread;
	CleanupClosePushL(thread);
	GetSsmSwpServerThreadHandleL(thread);

	TRequestStatus status;
	TSsmSwp swp(KSwpTestKey, KSwpTestKey_Value);

	if (aTestType == ETestSwpPolicyInitialize)
		{
		// We are setting the swppolicy libraryhandle as NULL and calling the initialize method
		ssmSwpPolicyCli.CallSetDllHandleAndInitialize(0, status);
		TEST(status.Int() == KRequestPending);
		User::WaitForRequest(status);
		}
	else if (aTestType == ETestSwpPolicyPrepareCommandList)
		{
		// Call preparecommandlist without setting the swppolicy libraryhandle and initializing the swppolicy
		ssmSwpPolicyCli.CallPrepareCommandList(swp, status);
		TEST(status.Int() == KRequestPending);
		User::WaitForRequest(status);
		}
	else if (aTestType == ETestSwpPolicyCallCommandList || aTestType == ETestSwpPolicyHandleCleRetVal)
		{
		// Set the swppolicy libraryhandle and initialize the policy
		ssmSwpPolicyCli.CallSetDllHandleAndInitialize(iLibrary.Handle(), status);
		TEST(status.Int() == KRequestPending);
		User::WaitForRequest(status);

		if (aTestType == ETestSwpPolicyCallCommandList)
			{
			// Call callcommandlist without calling the preparecommandlist
			// Since this call panics the server, ssmCommandList would be NULL, so no need to delete it
			CSsmCommandList* ssmCommandList = ssmSwpPolicyCli.CallCommandListL();
			delete ssmCommandList;
			}
		else
			{
			// Call preparecommandlist
			ssmSwpPolicyCli.CallPrepareCommandList(swp, status);
			TEST(status.Int() == KRequestPending);
			User::WaitForRequest(status);

			// Call HandleCleReturnValue without calling the command list.
			ssmSwpPolicyCli.CallHandleCleReturnValue(swp, KErrNone, 100, status);
			TEST(status.Int() == KRequestPending);
			User::WaitForRequest(status);
			}
		}

	const TInt exitReason = thread.ExitReason();
	if (aTestType == ETestSwpPolicyInitialize)
		{
		const TExitType exitType = thread.ExitType();
		// ssmswppolicyserver panics when Initialize method is called without setting the swppolicy library handle
		TEST(exitReason == 0 && exitType == EExitPanic);
		}
//These are testing Assert_Debug statements, so should be run only debug builds.
#ifdef _DEBUG
	else if (aTestType == ETestSwpPolicyPrepareCommandList)
		{
		// ssmswppolicyserver panics when PrepareCommandList method is called without setting the swppolicy library handle
		// and initializing the swppolicy
		TEST(exitReason == ESsmSwpPolicyServerError2);
		}
	else if (aTestType == ETestSwpPolicyCallCommandList)
		{
		// ssmswppolicyserver panics when CallCommandList method is called without preparing the commandlist
		TEST(exitReason == ESsmSwpPolicySrvStepError3);
		}
	else if (aTestType == ETestSwpPolicyHandleCleRetVal)
		{
		// ssmswppolicyserver panics when HandleCleReturnValue method is called without calling 
		// CallCommandlistL method.
		TEST(exitReason == ESsmSwpPolicySrvStepError4);
		}
	
#endif	// _DEBUG
	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();

	CleanupStack::PopAndDestroy(2, &ssmSwpPolicyCli);
	}

void CSsmSwpRSwpPolicySessionTest::DoTestErrorCondForSsmSwpClient()
	{
	INFO_PRINTF1(_L("Call Initialize method without setting the PolicyDll Handle"));
	TRAPD(err , DoTestErrorCondL(ETestSwpPolicyInitialize));
	TEST(err == KErrNone);

	INFO_PRINTF1(_L("Call PrepareCommandList method without setting the PolicyDll Handle and"));
	INFO_PRINTF1(_L("initializing the swp policy"));
	TRAP(err , DoTestErrorCondL(ETestSwpPolicyPrepareCommandList));
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("Call CallCommandList method without preparing the commandlist"));
	TRAP(err, DoTestErrorCondL(ETestSwpPolicyCallCommandList));
	TEST(err == KErrNone);

	INFO_PRINTF1(_L("Call HandleCleReturnValue without calling the command list in the swp policy"));
	TRAP(err, DoTestErrorCondL(ETestSwpPolicyHandleCleRetVal));
	TEST(err == KErrNone);
	}

void CSsmSwpRSwpPolicySessionTest::DoTestSsmSwpPolicyForMemLeaksL()
	{
	INFO_PRINTF1(_L("Test RSsmSwpPolicySession memory leaks"));
	RSsmSwpPolicySessionTest ssmSwpPolicyCli;
	INFO_PRINTF1(_L("Connect RSsmSwpPolicyCli client to server"));
	ssmSwpPolicyCli.ConnectL(KSsmSwpPolicyTestServerName);
	CleanupClosePushL(ssmSwpPolicyCli);

	ssmSwpPolicyCli.HeapMark();

	TSsmSwp swp(KSwpTestKey, KSwpTestKey_Value);
	INFO_PRINTF1(_L("Create active object to test the methods of RSsmSwpPolicySession API"));
	CSwpPolicyStepTest* swpPolicyStepTest = CSwpPolicyStepTest::NewL(*this, ssmSwpPolicyCli, swp, iLibrary.Handle());
	CleanupStack::PushL(swpPolicyStepTest);

	INFO_PRINTF1(_L("Test HandleCleReturnValue method which completes with KErrNone"));
	swpPolicyStepTest->StartSwpPolicyTest(EKErrNoneAndStopScheduler, ETestCompletePolicySuccessfully);

	iActiveScheduler->Start();

	ssmSwpPolicyCli.HeapMarkEnd();
	CleanupStack::PopAndDestroy(2, &ssmSwpPolicyCli);	
	}

void CSsmSwpRSwpPolicySessionTest::DoTestSsmSwpPolicyForOOML()
	{
	INFO_PRINTF1(_L("Test RSsmSwpPolicySession for out of memory"));
	
	RSsmSwpPolicySessionTest ssmSwpPolicyCli;
	INFO_PRINTF1(_L("Connect RSsmSwpPolicyCli client to server"));
	ssmSwpPolicyCli.ConnectL(KSsmSwpPolicyTestServerName);
	CleanupClosePushL(ssmSwpPolicyCli);
	iOOM = ETrue;
	
	TSsmSwp swp(KSwpTestKey, KSwpTestKey_Value);
	INFO_PRINTF1(_L("Create active object to test the methods of RSsmSwpPolicySession API"));
	CSwpPolicyStepTest* swpPolicyStepTest = CSwpPolicyStepTest::NewL(*this, ssmSwpPolicyCli, swp, iLibrary.Handle());
	CleanupStack::PushL(swpPolicyStepTest);

	swpPolicyStepTest->StartSwpPolicyTest(EKErrNoneAndStopScheduler, ETestCompletePolicySuccessfully);
	iActiveScheduler->Start();

	iOOM = EFalse;
	CleanupStack::PopAndDestroy(2, &ssmSwpPolicyCli);
	}

TVerdict CSsmSwpRSwpPolicySessionTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpRSwpPolicySessionTest started....."));

	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	__UHEAP_MARK;

	RProperty property;
	CleanupClosePushL(property);
	// Define and set the rproperty
	TEST(KErrNone == property.Define(RProcess().SecureId(), KSwpTestKey, RProperty::EInt));
	TEST(KErrNone == property.Set(RProcess().SecureId(), KSwpTestKey, 0));

	// Load the swppolicy library
	GetPolicyResolverL(iLibrary, KTestSwpPolicyCommonFile());
	// Making process as the owner of the swppolicy library, as this library
	// has to be accessed by the ssmswppolicy thread.
	TInt err = iLibrary.Duplicate(RThread(), EOwnerProcess);
	TEST(err == KErrNone);
	
	// Starting all test cases...
	TRAP(err, DoTestSsmSwpPolicyCliL(ETestCompletePolicySuccessfully));
	TEST(err == KErrNone);

	TRAP(err, DoTestSsmSwpPolicyCliL(ETestCancelSwpPolicyTest));
	TEST(err == KErrNone);

	DoTestErrorCondForSsmSwpClient();
	
	TRAP(err, DoTestSsmSwpPolicyForMemLeaksL());
	TEST(err == KErrNone);

	//OOM test case
	INFO_PRINTF1(_L("Start DoTestSsmSwpPolicyForOOML method "));
	TRAP(err, DoTestSsmSwpPolicyForOOML());
	INFO_PRINTF2(_L("DoTestSsmSwpPolicyForOOML method completes with %d"),err);
	TEST(err == KErrNone);
	
	TEST(KErrNone == property.Delete(RProcess().SecureId(), KSwpTestKey));
	iLibrary.Close();
	CleanupStack::PopAndDestroy(&property);

	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("....CSsmSwpRSwpPolicySessionTest finished!!!"));

	return TestStepResult();
	}



