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

#include "appfwk_sysstart_test_step_ProcFailRetry1Ignore.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0037
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0037
 */


void CAppfwkSysStartTestStepProcFailRetry1Ignore::TestProcFailRetry1IgnoreL()
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
	TestAppStartCount(2, app5Results.iStartCount); 
	TestAppRendCount(0, app5Results.iRendCount);
	TestAppRendFailCount(0, app5Results.iRendFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepProcFailRetry1Ignore::~CAppfwkSysStartTestStepProcFailRetry1Ignore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepProcFailRetry1Ignore::CAppfwkSysStartTestStepProcFailRetry1Ignore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepProcFailRetry1Ignore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepProcFailRetry1Ignore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0037: ProcFailRetry1Ignore - Started"));

 	__UHEAP_MARK;
 	
	TestProcFailRetry1IgnoreL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0037: ProcFailRetry1Ignore - Finished"));
	
	return TestStepResult();
	}

