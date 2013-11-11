// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "appfwk_sysstart_test_step_DllCsFnMultipleRetry.h"
#include "appfwk_sysstart_test_dll.h"


/**
Old Test CaseID 		APPFWK-SYSSTART-0031
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0031
 */


void CAppfwkSysStartTestStepDllCsFnMultipleRetry::TestDllCsFnMultipleRetryL()
	{
	TTestDllResults expectedTestDllResults;
	
	INFO_PRINTF1(_L("Test - TestDllFn2"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =100;
	expectedTestDllResults.arg2 =0;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn2ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn11"));
	expectedTestDllResults.count=3; // 2 retry
	expectedTestDllResults.arg1 =200;
	expectedTestDllResults.arg2 =201;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn11ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn4"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =300;
	expectedTestDllResults.arg2 =301;
	expectedTestDllResults.arg3 =302;
	TestDllResultsL(KTDllFn4ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllCsFnMultipleRetry::~CAppfwkSysStartTestStepDllCsFnMultipleRetry()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllCsFnMultipleRetry::CAppfwkSysStartTestStepDllCsFnMultipleRetry()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllCsFnMultipleRetry);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllCsFnMultipleRetry::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0031: DllCsFnMultipleRetry - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllCsFnMultipleRetryL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0031: DllCsFnMultipleRetry - Finished"));
	
	return TestStepResult();
	}

