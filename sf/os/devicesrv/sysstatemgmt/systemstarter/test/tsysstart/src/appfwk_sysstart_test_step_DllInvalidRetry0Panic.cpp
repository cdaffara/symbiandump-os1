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

#include "appfwk_sysstart_test_step_DllInvalidRetry0Panic.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0016
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0016
 */


void CAppfwkSysStartTestStepDllInvalidRetry0Panic::TestDllInvalidRetry0PanicL()
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
	
	// check to make sure ordinal 4 was not called (which is
	// called after the invalid dll to ensure the boot sequence
	// did not continue)
	INFO_PRINTF1(_L("Test - TestDllFn4"));
	TestResultFileDoesNotExist(KTDllFn4ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllInvalidRetry0Panic::~CAppfwkSysStartTestStepDllInvalidRetry0Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllInvalidRetry0Panic::CAppfwkSysStartTestStepDllInvalidRetry0Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllInvalidRetry0Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllInvalidRetry0Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0016: DllInvalidRetry0Panic - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllInvalidRetry0PanicL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0016: DllInvalidRetry0Panic - Finished"));
	
	return TestStepResult();
	}

