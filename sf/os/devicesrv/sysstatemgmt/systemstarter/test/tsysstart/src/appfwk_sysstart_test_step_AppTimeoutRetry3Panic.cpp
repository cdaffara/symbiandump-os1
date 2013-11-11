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

#include "appfwk_sysstart_test_step_AppTimeoutRetry3Panic.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0050
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0050
 */


void CAppfwkSysStartTestStepAppTimeoutRetry3Panic::TestAppTimeoutRetry3PanicL()
	{
	// read in result files
	TAppfwkSysstartTestAppResults app1Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp1ResultFileName, app1Results);
	TAppfwkSysstartTestAppResults app2Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp2ResultFileName, app2Results);
	TAppfwkSysstartTestAppResults app3Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp3ResultFileName, app3Results);
	TAppfwkSysstartTestAppResults app6Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp6ResultFileName, app6Results);
	
	INFO_PRINTF1(_L("Test App 1 Results"));
	TestAppStartCount(1, app1Results.iStartCount);  
	TestAppRendCount(1, app1Results.iRendCount);
	TestAppRendFailCount(0, app1Results.iRendFailCount);
	
	INFO_PRINTF1(_L("Test App 2 Results"));
	TestAppStartCount(1, app2Results.iStartCount);  
	TestAppRendCount(1, app2Results.iRendCount);
	TestAppRendFailCount(0, app2Results.iRendFailCount);
	
	INFO_PRINTF1(_L("Test App 3 Results"));
	TestAppStartCount(4, app3Results.iStartCount);  
	TestAppRendCount(0, app3Results.iRendCount);
	TestAppRendFailCount(0, app3Results.iRendFailCount);
	
	// check the results obtained from the epocwind log, i.e.
	// the proof that sysstart performed a panic
	INFO_PRINTF1(_L("Test - SysStart Panic"));
	TestEpocWindResultsL(KTAppPanicResultFile,1);
	
	// check to make sure that t_app6 result file was not created,
	// i.e. this part of the startup resource file was not reached
	TestResultFileDoesNotExistL(KTApp6ResultFileName);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepAppTimeoutRetry3Panic::~CAppfwkSysStartTestStepAppTimeoutRetry3Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepAppTimeoutRetry3Panic::CAppfwkSysStartTestStepAppTimeoutRetry3Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepAppTimeoutRetry3Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepAppTimeoutRetry3Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0050: AppTimeoutRetry3Panic - Started"));

 	__UHEAP_MARK;
 	
	TestAppTimeoutRetry3PanicL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0050: AppTimeoutRetry3Panic - Finished"));
	
	return TestStepResult();
	}

