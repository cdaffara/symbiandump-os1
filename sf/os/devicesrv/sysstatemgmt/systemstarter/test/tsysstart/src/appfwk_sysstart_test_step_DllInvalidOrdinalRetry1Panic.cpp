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

#include "appfwk_sysstart_test_step_DllInvalidOrdinalRetry1Panic.h"
#include "appfwk_sysstart_test_dll.h"


/**
Old Test CaseID 		APPFWK-SYSSTART-0021
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0021
 */


void CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic::TestDllInvalidOrdinalRetry1PanicL()
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
	// the proof that the attempts to execute the invalid ordinal were
	// made
	// NOTE: Even though the retries have been set to 1 the expected behaviour
	//       is only 1 attempt is made to call a non-existent ordinal
	INFO_PRINTF1(_L("Test - Invalid Ordinal"));
	TInt expectedCount=1;
	TestEpocWindResultsL(KDllResultFile, expectedCount);
	
	// check to make sure ordinal 4 was not called (which is
	// called after the invalid ordinal to ensure the boot sequence
	// did not continue)
	INFO_PRINTF1(_L("Test - TestDllFn4"));
	TestResultFileDoesNotExist(KTDllFn4ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic::~CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic::CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0021: DllInvalidOrdinalRetry1Panic - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllInvalidOrdinalRetry1PanicL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0021: DllInvalidOrdinalRetry1Panic - Finished"));
	
	return TestStepResult();
	}

