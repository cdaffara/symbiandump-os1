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

#include "appfwk_sysstart_test_step_DefRendFailIgnore.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"


/**
Old Test CaseID 		APPFWK-SYSSTART-0073
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0073
 */


void CAppfwkSysStartTestStepDefRendFailIgnore::TestDefRendFailIgnoreL()
	{
	TAppfwkSysstartTestAppResults app22Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp22ResultFileName, app22Results);
	TAppfwkSysstartTestAppResults app4Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp4ResultFileName, app4Results);
	
	INFO_PRINTF1(_L("Test App 22 Results"));
	TestAppStartCount(1, app22Results.iStartCount); 
	TestAppRendCount(0, app22Results.iRendCount);
	TestAppRendFailCount(1, app22Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 4 Results"));
	TestAppStartCount(1, app4Results.iStartCount); 
	TestAppRendCount(0, app4Results.iRendCount);
	TestAppRendFailCount(1, app4Results.iRendFailCount);
	
	TAppfwkSysstartTestAppResults app5Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp5ResultFileName, app5Results);
	
	INFO_PRINTF1(_L("Test App 5 Results"));
	TestAppStartCount(1, app5Results.iStartCount); 
	TestAppRendCount(0, app5Results.iRendCount);
	TestAppRendFailCount(0, app5Results.iRendFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefRendFailIgnore::~CAppfwkSysStartTestStepDefRendFailIgnore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefRendFailIgnore::CAppfwkSysStartTestStepDefRendFailIgnore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefRendFailIgnore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefRendFailIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0073: DefRendFailIgnore - Started"));

 	__UHEAP_MARK;
 	
	TestDefRendFailIgnoreL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0073: DefRendFailIgnore - Finished"));
	
	return TestStepResult();
	}

