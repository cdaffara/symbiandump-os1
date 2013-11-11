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

#include "appfwk_sysstart_test_step_DefStartCd.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0051
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0051
 */


void CAppfwkSysStartTestStepDefStartCd::TestDefStartCdL()
	{
	TAppfwkSysstartTestAppResults app7Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp7ResultFileName, app7Results);
	TAppfwkSysstartTestAppResults app8Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp8ResultFileName, app8Results);
	TAppfwkSysstartTestAppResults app9Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp9ResultFileName, app9Results);
	TAppfwkSysstartTestAppResults app10Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp10ResultFileName, app10Results);
	TAppfwkSysstartTestAppResults app11Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp11ResultFileName, app11Results);
	
	INFO_PRINTF1(_L("Test App 7 Results"));
	TestAppStartCount(1, app7Results.iStartCount); 
	TestAppRendCount(1, app7Results.iRendCount);
	TestAppRendFailCount(0, app7Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 8 Results"));
	TestAppStartCount(1, app8Results.iStartCount); 
	TestAppRendCount(1, app8Results.iRendCount);
	TestAppRendFailCount(0, app8Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 9 Results"));
	TestAppStartCount(1, app9Results.iStartCount); 
	TestAppRendCount(1, app9Results.iRendCount);
	TestAppRendFailCount(0, app9Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 10 Results"));
	TestAppStartCount(1, app10Results.iStartCount); 
	TestAppRendCount(1, app10Results.iRendCount);
	TestAppRendFailCount(0, app10Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 11 Results"));
	TestAppStartCount(1, app11Results.iStartCount); 
	TestAppRendCount(0, app11Results.iRendCount);
	TestAppRendFailCount(0, app11Results.iRendFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefStartCd::~CAppfwkSysStartTestStepDefStartCd()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefStartCd::CAppfwkSysStartTestStepDefStartCd()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefStartCd);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefStartCd::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0051: DefStartCd - Started"));

 	__UHEAP_MARK;
 	
	TestDefStartCdL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0051: DefStartCd - Finished"));
	
	return TestStepResult();
	}

