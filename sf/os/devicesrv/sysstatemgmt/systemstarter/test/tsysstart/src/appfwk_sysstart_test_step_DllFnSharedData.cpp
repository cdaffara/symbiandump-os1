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

#include "appfwk_sysstart_test_step_DllFnSharedData.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0029
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0029
 */

void CAppfwkSysStartTestStepDllFnSharedData::TestDllFnSharedDataL()
	{
	TTestDllResults expectedTestDllResults;
	
	INFO_PRINTF1(_L("Test - TestDllFn13"));
	expectedTestDllResults.count=1; 
	expectedTestDllResults.arg1 =210;
	expectedTestDllResults.arg2 =211;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn13ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn14"));
	TestDllResultsL(KTDllFn14ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn15"));
	expectedTestDllResults.count=1; 
	expectedTestDllResults.arg1 =310;
	expectedTestDllResults.arg2 =311;
	expectedTestDllResults.arg3 =312;
	TestDllResultsL(KTDllFn15ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn16"));
	TestDllResultsL(KTDllFn16ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn17"));
	expectedTestDllResults.count=1; 
	expectedTestDllResults.arg1 =410;
	expectedTestDllResults.arg2 =0;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn17ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn18"));
	TestDllResultsL(KTDllFn18ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllFnSharedData::~CAppfwkSysStartTestStepDllFnSharedData()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllFnSharedData::CAppfwkSysStartTestStepDllFnSharedData()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllFnSharedData);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllFnSharedData::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0029: DllFnSharedData - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllFnSharedDataL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0029: DllFnSharedData - Finished"));
	
	return TestStepResult();
	}

