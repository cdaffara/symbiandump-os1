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

#include "appfwk_sysstart_test_step_DefTimeoutProcIgnore.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0071
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0071
 */

void CAppfwkSysStartTestStepDefTimeoutProcIgnore::TestDefTimeoutProcIgnoreL()
	{
	TAppfwkSysstartTestAppResults app22Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp22ResultFileName, app22Results);
	TAppfwkSysstartTestAppResults app23Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp23ResultFileName, app23Results);
	TAppfwkSysstartTestAppResults app5Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp5ResultFileName, app5Results);
	
	INFO_PRINTF1(_L("Test App 22 Results"));
	TestAppStartCount(1, app22Results.iStartCount); 
	TestAppRendCount(0, app22Results.iRendCount);
	TestAppRendFailCount(0, app22Results.iRendFailCount);
	
	INFO_PRINTF1(_L("Test App 23 Results"));
	TestAppStartCount(1, app23Results.iStartCount); 
	TestAppRendCount(1, app23Results.iRendCount);
	TestAppRendFailCount(0, app23Results.iRendFailCount);
	
	INFO_PRINTF1(_L("Test App 5 Results"));
	TestAppStartCount(1, app5Results.iStartCount); 
	TestAppRendCount(0, app5Results.iRendCount);
	TestAppRendFailCount(0, app5Results.iRendFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefTimeoutProcIgnore::~CAppfwkSysStartTestStepDefTimeoutProcIgnore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefTimeoutProcIgnore::CAppfwkSysStartTestStepDefTimeoutProcIgnore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefTimeoutProcIgnore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefTimeoutProcIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0071: DefTimeoutProcIgnore - Started"));

 	__UHEAP_MARK;
 	
	TestDefTimeoutProcIgnoreL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0071: DefTimeoutProcIgnore - Finished"));
	
	return TestStepResult();
	}

