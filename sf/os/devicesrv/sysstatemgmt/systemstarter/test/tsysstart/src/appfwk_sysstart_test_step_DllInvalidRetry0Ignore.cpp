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

#include "appfwk_sysstart_test_step_DllInvalidRetry0Ignore.h"
#include "appfwk_sysstart_test_dll.h"


/**
Old Test CaseID 		APPFWK-SYSSTART-0014
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0014
 */


void CAppfwkSysStartTestStepDllInvalidRetry0Ignore::TestDllInvalidRetry0IgnoreL()
	{
	TTestDllResults expectedTestDllResults;
	
	// check to make sure that ordinal 2 was called
	INFO_PRINTF1(_L("Test - TestDllFn2"));
	expectedTestDllResults.count=1;
	expectedTestDllResults.arg1 =100;
	expectedTestDllResults.arg2 =0;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn2ResultFileName, expectedTestDllResults);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that the attempts to load the invalid dll were
	// made
	INFO_PRINTF1(_L("Test - Invalid DLL"));
	TInt expectedCount=1;
	TestEpocWindResultsL(KDllResultFile, expectedCount);
	
	// check to make sure ordinal 4 was called (which is
	// called after the invalid dll to ensure the boot sequence
	// did continue
	INFO_PRINTF1(_L("Test - TestDllFn4"));
	expectedTestDllResults.arg1 =300;
	expectedTestDllResults.arg2 =301;
	expectedTestDllResults.arg3 =302;
	TestDllResultsL(KTDllFn4ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllInvalidRetry0Ignore::~CAppfwkSysStartTestStepDllInvalidRetry0Ignore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllInvalidRetry0Ignore::CAppfwkSysStartTestStepDllInvalidRetry0Ignore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllInvalidRetry0Ignore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllInvalidRetry0Ignore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0014: DllInvalidRetry0Ignore - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllInvalidRetry0IgnoreL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0014: DllInvalidRetry0Ignore - Finished"));
	
	return TestStepResult();
	}

