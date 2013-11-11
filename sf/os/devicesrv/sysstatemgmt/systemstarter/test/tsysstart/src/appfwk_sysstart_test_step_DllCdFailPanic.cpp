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

#include "appfwk_sysstart_test_step_DllCdFailPanic.h"
#include "appfwk_sysstart_test_dll.h"


/**
Old Test CaseID 		APPFWK-SYSSTART-0027
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0027
 */


void CAppfwkSysStartTestStepDllCdFailPanic::TestDllCdFailPanicL()
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
	expectedTestDllResults.arg1 =1000;
	TestDllResultsL(KTDllFn8ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn9"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =2000;
	expectedTestDllResults.arg2 =2001;
	TestDllResultsL(KTDllFn9ResultFileName, expectedTestDllResults);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart Panic"));
	TInt expectedCount=1;
	TestEpocWindResultsL(KDllResultFile, expectedCount);
	
	// checks to make sure ordinal 10,11,12 were not called 
	INFO_PRINTF1(_L("Test - TestDllFn10"));
	TestResultFileDoesNotExist(KTDllFn10ResultFileName);
	INFO_PRINTF1(_L("Test - TestDllFn11"));
	TestResultFileDoesNotExist(KTDllFn11ResultFileName);
	INFO_PRINTF1(_L("Test - TestDllFn12"));
	TestResultFileDoesNotExist(KTDllFn12ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllCdFailPanic::~CAppfwkSysStartTestStepDllCdFailPanic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllCdFailPanic::CAppfwkSysStartTestStepDllCdFailPanic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllCdFailPanic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllCdFailPanic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0027: DllCdFailPanic - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllCdFailPanicL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0027: DllCdFailPanic - Finished"));
	
	return TestStepResult();
	}

