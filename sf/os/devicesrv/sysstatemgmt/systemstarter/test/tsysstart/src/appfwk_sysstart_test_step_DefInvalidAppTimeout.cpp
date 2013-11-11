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

#include "appfwk_sysstart_test_step_DefInvalidAppTimeout.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0059
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0059
 */


void CAppfwkSysStartTestStepDefInvalidAppTimeout::TestDefInvalidAppTimeoutL()
	{
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart Panic"));
	TestEpocWindResultsL(KTAppPanicResultFile,1);
	
	// check to make sure that t_app22,23,24 result files were not created,
	// i.e. this part of the startup resource file was not reached
	TestResultFileDoesNotExistL(KTApp22ResultFileName);
	TestResultFileDoesNotExistL(KTApp23ResultFileName);
	TestResultFileDoesNotExistL(KTApp24ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefInvalidAppTimeout::~CAppfwkSysStartTestStepDefInvalidAppTimeout()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefInvalidAppTimeout::CAppfwkSysStartTestStepDefInvalidAppTimeout()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefInvalidAppTimeout);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefInvalidAppTimeout::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0059: DefInvalidAppTimeout - Started"));

 	__UHEAP_MARK;
 	
	TestDefInvalidAppTimeoutL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0059: DefInvalidAppTimeout - Finished"));
	
	return TestStepResult();
	}

