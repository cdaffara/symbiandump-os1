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

#include "appfwk_sysstart_test_step_DefRendFailAppPanic.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0074
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0074
 */


void CAppfwkSysStartTestStepDefRendFailAppPanic::TestDefRendFailAppPanicL()
	{
	TAppfwkSysstartTestAppResults app22Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp22ResultFileName, app22Results);
	TAppfwkSysstartTestAppResults app23Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp23ResultFileName, app23Results);
	
	INFO_PRINTF1(_L("Test App 22 Results"));
	TestAppStartCount(1, app22Results.iStartCount); 
	TestAppRendCount(0, app22Results.iRendCount);
	TestAppRendFailCount(1, app22Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 23 Results"));
	TestAppStartCount(1, app23Results.iStartCount); 
	TestAppRendCount(1, app23Results.iRendCount);
	TestAppRendFailCount(0, app23Results.iRendFailCount);
	
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
CAppfwkSysStartTestStepDefRendFailAppPanic::~CAppfwkSysStartTestStepDefRendFailAppPanic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefRendFailAppPanic::CAppfwkSysStartTestStepDefRendFailAppPanic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefRendFailAppPanic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefRendFailAppPanic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0074: DefRendFailAppPanic - Started"));

 	__UHEAP_MARK;
 	
	TestDefRendFailAppPanicL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0074: DefRendFailAppPanic - Finished"));
	
	return TestStepResult();
	}

