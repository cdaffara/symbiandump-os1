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

#include "appfwk_sysstart_test_step_DefTimeoutProcPanic.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0064
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0064
 */


void CAppfwkSysStartTestStepDefTimeoutProcPanic::TestDefTimeoutProcPanicL()
	{
	TAppfwkSysstartTestAppResults app22Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp22ResultFileName, app22Results);
	TAppfwkSysstartTestAppResults app23Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp23ResultFileName, app23Results);
	
	INFO_PRINTF1(_L("Test App 22 Results"));
	TestAppStartCount(1, app22Results.iStartCount); 
	TestAppRendCount(0, app22Results.iRendCount);
	TestAppRendFailCount(0, app22Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 23 Results"));
	TestAppStartCount(1, app23Results.iStartCount); 
	TestAppRendCount(1, app23Results.iRendCount);
	TestAppRendFailCount(0, app23Results.iRendFailCount);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart Panic"));
	TestEpocWindResultsL(KTAppPanicResultFile,1);
	
	// check to make sure that t_app5 result file was not created,
	// i.e. this part of the startup resource file was not reached
	TestResultFileDoesNotExistL(KTApp5ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefTimeoutProcPanic::~CAppfwkSysStartTestStepDefTimeoutProcPanic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefTimeoutProcPanic::CAppfwkSysStartTestStepDefTimeoutProcPanic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefTimeoutProcPanic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefTimeoutProcPanic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0064: DefTimeoutProcPanic - Started"));

 	__UHEAP_MARK;
 	
	TestDefTimeoutProcPanicL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0064: DefTimeoutProcPanic - Finished"));
	
	return TestStepResult();
	}

