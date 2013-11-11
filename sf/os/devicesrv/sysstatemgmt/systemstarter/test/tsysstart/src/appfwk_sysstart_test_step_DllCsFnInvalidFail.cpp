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

#include "appfwk_sysstart_test_step_DllCsFnInvalidFail.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0030
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0030
 */


void CAppfwkSysStartTestStepDllCsFnInvalidFail::TestDllCsFnInvalidFailL()
	{
	// check to make sure ordinal 2 was not called (which is
	// called before the invalid fail policy to ensure the boot sequence
	// did not start - as it should be killed when the resource
	// file is read in)
	INFO_PRINTF1(_L("Test - TestDllFn2"));
	TestResultFileDoesNotExist(KTDllFn2ResultFileName);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that the attempts to execute the invalid fail
	// policy
	INFO_PRINTF1(_L("Test - Invalid Fail Policy"));
	TInt expectedCount=1;
	TestEpocWindResultsL(KDllResultFile, expectedCount);
	
	// check to make sure ordinal 4 was not called (which is
	// called after the invalid fail policy to ensure the boot sequence
	// did not continue)
	INFO_PRINTF1(_L("Test - TestDllFn4"));
	TestResultFileDoesNotExist(KTDllFn4ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllCsFnInvalidFail::~CAppfwkSysStartTestStepDllCsFnInvalidFail()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllCsFnInvalidFail::CAppfwkSysStartTestStepDllCsFnInvalidFail()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllCsFnInvalidFail);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllCsFnInvalidFail::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0030: DllCsFnInvalidFail - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllCsFnInvalidFailL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0030: DllCsFnInvalidFail - Finished"));
	
	return TestStepResult();
	}

