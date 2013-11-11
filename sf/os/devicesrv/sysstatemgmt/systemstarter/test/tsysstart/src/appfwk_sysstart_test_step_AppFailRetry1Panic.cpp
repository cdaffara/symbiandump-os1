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

#include "appfwk_sysstart_test_step_AppFailRetry1Panic.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0048
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0048
 */


void CAppfwkSysStartTestStepAppFailRetry1Panic::TestAppFailRetry1PanicL()
	{
	// read in result files
	TAppfwkSysstartTestAppResults app4Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp4ResultFileName, app4Results);
	TAppfwkSysstartTestAppResults app5Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp5ResultFileName, app5Results);
	
	INFO_PRINTF1(_L("Test App 4 Results"));
	TestAppStartCount(2, app4Results.iStartCount);  
	TestAppRendCount(0, app4Results.iRendCount);
	TestAppRendFailCount(2, app4Results.iRendFailCount);
	
	INFO_PRINTF1(_L("Test App 5 Results"));
	TestAppStartCount(1, app5Results.iStartCount); 
	TestAppRendCount(0, app5Results.iRendCount);
	TestAppRendFailCount(0, app5Results.iRendFailCount);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart Panic"));
	TestEpocWindResultsL(KTAppPanicResultFile,1);
	
	// check to make sure that t_app1 result file was not created,
	// i.e. this part of the startup resource file was not reached
	TestResultFileDoesNotExistL(KTApp1ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepAppFailRetry1Panic::~CAppfwkSysStartTestStepAppFailRetry1Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepAppFailRetry1Panic::CAppfwkSysStartTestStepAppFailRetry1Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepAppFailRetry1Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepAppFailRetry1Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0048: AppFailRetry1Panic - Started"));

 	__UHEAP_MARK;
 	
	TestAppFailRetry1PanicL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0048: AppFailRetry1Panic - Finished"));
	
	return TestStepResult();
	}

