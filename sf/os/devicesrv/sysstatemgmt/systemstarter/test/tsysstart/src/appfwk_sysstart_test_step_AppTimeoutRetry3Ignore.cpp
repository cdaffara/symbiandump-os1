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

#include "appfwk_sysstart_test_step_AppTimeoutRetry3Ignore.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0049
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0049
 */


void CAppfwkSysStartTestStepAppTimeoutRetry3Ignore::TestAppTimeoutRetry3IgnoreL()
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
	
	INFO_PRINTF1(_L("Test App 6 Results"));
	TestAppStartCount(1, app6Results.iStartCount); 
	TestAppRendCount(0, app6Results.iRendCount);
	TestAppRendFailCount(0, app6Results.iRendFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepAppTimeoutRetry3Ignore::~CAppfwkSysStartTestStepAppTimeoutRetry3Ignore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepAppTimeoutRetry3Ignore::CAppfwkSysStartTestStepAppTimeoutRetry3Ignore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepAppTimeoutRetry3Ignore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepAppTimeoutRetry3Ignore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0049: AppTimeoutRetry3Ignore - Started"));

 	__UHEAP_MARK;
 	
	TestAppTimeoutRetry3IgnoreL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0049: AppTimeoutRetry3Ignore - Finished"));
	
	return TestStepResult();
	}

