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

#include "appfwk_sysstart_test_step_DllCdFailIgnore.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0024
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0024
 */


void CAppfwkSysStartTestStepDllCdFailIgnore::TestDllCdFailIgnoreL()
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
	
	INFO_PRINTF1(_L("Test - TestDllFn10"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =3000;
	expectedTestDllResults.arg2 =3001;
	expectedTestDllResults.arg3 =3002;
	TestDllResultsL(KTDllFn10ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn11"));
	expectedTestDllResults.count=2; // 1 retry
	expectedTestDllResults.arg1 =2000;
	expectedTestDllResults.arg2 =2001;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn11ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn12"));
	expectedTestDllResults.count=1; // 0 retry
	expectedTestDllResults.arg1 =1000;
	expectedTestDllResults.arg2 =0;
	TestDllResultsL(KTDllFn12ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllCdFailIgnore::~CAppfwkSysStartTestStepDllCdFailIgnore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllCdFailIgnore::CAppfwkSysStartTestStepDllCdFailIgnore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllCdFailIgnore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllCdFailIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0024: DllCdFailIgnore - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllCdFailIgnoreL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0024: DllCdFailIgnore - Finished"));
	
	return TestStepResult();
	}

