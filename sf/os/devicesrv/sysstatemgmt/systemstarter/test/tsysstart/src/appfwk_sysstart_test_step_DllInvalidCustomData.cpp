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

#include "appfwk_sysstart_test_step_DllInvalidCustomData.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0032
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0032
 */


void CAppfwkSysStartTestStepDllInvalidCustomData::TestDllInvalidCustomDataL()
	{
	// check to make sure ordinal 2 was not called (which is
	// called before the invalid custom data to ensure the boot sequence
	// did not start - as it should be killed when the resource
	// file is read in)
	INFO_PRINTF1(_L("Test - TestDllFn2"));
	TestResultFileDoesNotExist(KTDllFn2ResultFileName);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that the attempts to execute the ordinal with
	// the invalid custom data were made
	INFO_PRINTF1(_L("Test - Invalid Custom Data"));
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
CAppfwkSysStartTestStepDllInvalidCustomData::~CAppfwkSysStartTestStepDllInvalidCustomData()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllInvalidCustomData::CAppfwkSysStartTestStepDllInvalidCustomData()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllInvalidCustomData);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllInvalidCustomData::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0032: DllInvalidCustomData - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllInvalidCustomDataL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0032: DllInvalidCustomData - Finished"));
	
	return TestStepResult();
	}

