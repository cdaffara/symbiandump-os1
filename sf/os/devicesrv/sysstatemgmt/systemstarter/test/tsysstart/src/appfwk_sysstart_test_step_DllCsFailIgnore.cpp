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

#include "appfwk_sysstart_test_step_DllCsFailIgnore.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0023
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0023
 */


void CAppfwkSysStartTestStepDllCsFailIgnore::TestDllCsFailIgnoreL()
	{
	TTestDllResults expectedTestDllResults;
	
	INFO_PRINTF1(_L("Test - TestDllFn7"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =0;
	expectedTestDllResults.arg2 =0;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn7ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn8"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =100;
	TestDllResultsL(KTDllFn8ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn9"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =200;
	expectedTestDllResults.arg2 =201;
	TestDllResultsL(KTDllFn9ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn10"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =300;
	expectedTestDllResults.arg2 =301;
	expectedTestDllResults.arg3 =302;
	TestDllResultsL(KTDllFn10ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn11"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =200;
	expectedTestDllResults.arg2 =201;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn11ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn12"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =100;
	expectedTestDllResults.arg2 =0;
	TestDllResultsL(KTDllFn12ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllCsFailIgnore::~CAppfwkSysStartTestStepDllCsFailIgnore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllCsFailIgnore::CAppfwkSysStartTestStepDllCsFailIgnore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllCsFailIgnore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllCsFailIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0023: DllCsFailIgnore - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllCsFailIgnoreL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0023: DllCsFailIgnore - Finished"));
	
	return TestStepResult();
	}

