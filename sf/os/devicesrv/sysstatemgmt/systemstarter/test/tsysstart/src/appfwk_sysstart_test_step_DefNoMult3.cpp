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

#include "appfwk_sysstart_test_step_DefNoMult3.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0056
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0056
 */


void CAppfwkSysStartTestStepDefNoMult3::TestDefNoMult3L()
	{
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart Panic"));
	TestEpocWindResultsL(KTAppPanicResultFile,1);
	
	// check to make sure that t_app7,8,9,10,11 result files were not created,
	// i.e. this part of the startup resource file was not reached
	TestResultFileDoesNotExistL(KTApp7ResultFileName);
	TestResultFileDoesNotExistL(KTApp8ResultFileName);
	TestResultFileDoesNotExistL(KTApp9ResultFileName);
	TestResultFileDoesNotExistL(KTApp10ResultFileName);
	TestResultFileDoesNotExistL(KTApp11ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefNoMult3::~CAppfwkSysStartTestStepDefNoMult3()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefNoMult3::CAppfwkSysStartTestStepDefNoMult3()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefNoMult3);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefNoMult3::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0056: DefNoMult3 - Started"));

 	__UHEAP_MARK;
 	
	TestDefNoMult3L();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0056: DefNoMult3 - Finished"));
	
	return TestStepResult();
	}

