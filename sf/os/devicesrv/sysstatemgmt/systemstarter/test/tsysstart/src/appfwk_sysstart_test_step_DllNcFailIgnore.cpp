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

#include "appfwk_sysstart_test_step_DllNcFailIgnore.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0025
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0025
 */


void CAppfwkSysStartTestStepDllNcFailIgnore::TestDllNcFailIgnoreL()
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
	expectedTestDllResults.arg1 =10000;
	TestDllResultsL(KTDllFn8ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn9"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =20000;
	expectedTestDllResults.arg2 =20001;
	TestDllResultsL(KTDllFn9ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn10"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =30000;
	expectedTestDllResults.arg2 =30001;
	expectedTestDllResults.arg3 =30002;
	TestDllResultsL(KTDllFn10ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn11"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =20000;
	expectedTestDllResults.arg2 =20001;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn11ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn12"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =10000;
	expectedTestDllResults.arg2 =0;
	TestDllResultsL(KTDllFn12ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllNcFailIgnore::~CAppfwkSysStartTestStepDllNcFailIgnore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllNcFailIgnore::CAppfwkSysStartTestStepDllNcFailIgnore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllNcFailIgnore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllNcFailIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0025: DllNcFailIgnore - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllNcFailIgnoreL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0025: DllNcFailIgnore - Finished"));
	
	return TestStepResult();
	}

