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
 
#ifndef __TSSMSWPPOLICY_STEP_RSWPPOLICYSESS_H__
#define __TSSMSWPPOLICY_STEP_RSWPPOLICYSESS_H__

#include <test/testexecutestepbase.h>

#include "ssmswppolicysrv.h"
#include "ssmswppolicycli.h"

#include "tssmswppolicy_swppolicycli.h"

_LIT(KTSsmSwpRSwpPolicySessionStep,"SsmSwpRSwpPolicySessionStep");

class CSsmSwpRSwpPolicySessionTest;

enum TestCondition
	{
	// This is used to test HandleCleReturnValue when it completes request with error
	EErrorKTestRSsmSwpPolicySession = 1,
	// This is used to test HandleCleReturnValue when it panics
	EPanicKTestRSsmSwpPolicySession,
	// This is used to test HandleCleReturnValue when it completes request with KErrNone
	EKErrNone,
	// This is used to test HandleCleReturnValue when it completes request with KErrNone and stop the scheduler
	EKErrNoneAndStopScheduler
	};

// Enum used to test different asychronous (call and cancel) methods of the ssmswppolicyserver.
enum TTestType
	{
	ETestCompletePolicySuccessfully = 1,
	ETestSwpPolicyInitialize,
	ETestSwpPolicyPrepareCommandList,
	ETestSwpPolicyCallCommandList,
	ETestSwpPolicyHandleCleRetVal,
	ETestCancelSwpPolicyTest,
	ETestSwpPolicyCancelInitialize,
	ETestSwpPolicyCancelPrepareCommandList,
	ETestSwpPolicyCancelHandleCleRetVal
	};

// Active object which is used for testing the functionality of ssmswppolicyserver
class CSwpPolicyStepTest : public CActive
	{
enum TStep
	{
	//Different steps involved during the request for swp change
	ENull = 0,
	EInitialize,
	EPrepareCommandList,
	ECallCommandList,
	EHandleCleReturnValue
	};
public:
	static CSwpPolicyStepTest* NewL(CSsmSwpRSwpPolicySessionTest& aSwpRSwpPolicySessionTest, RSsmSwpPolicySessionTest& aSsmSwpPolicyTestCli, const TSsmSwp& aSwp, TInt aLibraryHandle);
	~CSwpPolicyStepTest();
	void StartSwpPolicyTest(TestCondition aTestCondition, TTestType aTestType);
	void StartInitialize();
	void StartPrepareCommandList();
	void StartHandleCleReturnValue(TInt aError, TInt aSeverity);

private:
	CSwpPolicyStepTest(CSsmSwpRSwpPolicySessionTest& aSwpRSwpPolicySessionTest, RSsmSwpPolicySessionTest& aSsmSwpPolicyTestCli, const TSsmSwp& aSwp, TInt aLibraryHandle);

protected:
	void RunL();
	void DoCancel();
	TInt RunError();

private:
	CSsmSwpRSwpPolicySessionTest& iSwpRSwpPolicySessionTest;
	RSsmSwpPolicySessionTest& iSsmSwpPolicyTestCli;
	TSsmSwp iSwp;
	TStep iStep;
	TestCondition iTestCondition;
	RThread iThread;
	TTestType iTestType;
	TInt iLibraryHandle;
	TInt iFailRate;
	};
	
class CSsmSwpRSwpPolicySessionTest : public CTestStep 
	{
public:
	CSsmSwpRSwpPolicySessionTest();
	~CSsmSwpRSwpPolicySessionTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

	void DoTestResult(TInt aActualResult, TInt aExpectedResult, TInt aStep);
	void GetSsmSwpServerThreadHandleL(RThread& aThread);

private:
	void GetPolicyResolverL(RLibrary& aLibrary, const TDesC& aLibraryFilename);

	void DoTestSsmSwpPolicyCliL(TTestType aTestType);
	void DoTestSsmSwpPolicyForMemLeaksL();
	void DoTestSsmSwpPolicyForOOML();
	void DoTestErrorCondForSsmSwpClient();
	void DoTestErrorCondL(TTestType aTestType);
	
private:
	RSsmSwpPolicySessionTest iSsmSwpPolicyCli;
	RLibrary iLibrary;
	CActiveScheduler* iActiveScheduler;
public:
	TBool iOOM; //Flag to indicate if the test is OOM test or not
	};

#endif	//__TSSMSWPPOLICY_STEP_RSWPPOLICYSESS_H__
