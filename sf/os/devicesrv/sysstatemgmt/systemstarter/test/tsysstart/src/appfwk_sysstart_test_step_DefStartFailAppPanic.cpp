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

#include "appfwk_sysstart_test_step_DefStartFailAppPanic.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0066
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0066
 */


void CAppfwkSysStartTestStepDefStartFailAppPanic::TestDefStartFailAppPanicL()
	{
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart epocwind log check"));
	TestEpocWindResultsL(KTAppPanicResultFile,1);
	
	// check to make sure t_app5 result file does not exist
	INFO_PRINTF1(_L("Test - t_app5 result file does not exist"));
	TestResultFileDoesNotExistL(KTApp5ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefStartFailAppPanic::~CAppfwkSysStartTestStepDefStartFailAppPanic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefStartFailAppPanic::CAppfwkSysStartTestStepDefStartFailAppPanic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefStartFailAppPanic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefStartFailAppPanic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0066: DefStartFailAppPanic - Started"));

 	__UHEAP_MARK;
 	
	TestDefStartFailAppPanicL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0066: DefStartFailAppPanic - Finished"));
	
	return TestStepResult();
	}

