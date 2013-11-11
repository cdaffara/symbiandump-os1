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

#include "appfwk_sysstart_test_step_DefStartAllState.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0057
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0057
 */


void CAppfwkSysStartTestStepDefStartAllState::TestDefStartAllStateL()
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
	
	TAppfwkSysstartTestAppResults app12Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp12ResultFileName, app12Results);
	TAppfwkSysstartTestAppResults app13Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp13ResultFileName, app13Results);
	TAppfwkSysstartTestAppResults app14Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp14ResultFileName, app14Results);
	TAppfwkSysstartTestAppResults app15Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp15ResultFileName, app15Results);
	TAppfwkSysstartTestAppResults app16Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp16ResultFileName, app16Results);
	
	INFO_PRINTF1(_L("Test App 12 Results"));
	TestAppStartCount(1, app12Results.iStartCount); 
	TestAppRendCount(1, app12Results.iRendCount);
	TestAppRendFailCount(0, app12Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 13 Results"));
	TestAppStartCount(1, app13Results.iStartCount); 
	TestAppRendCount(1, app13Results.iRendCount);
	TestAppRendFailCount(0, app13Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 14 Results"));
	TestAppStartCount(1, app14Results.iStartCount); 
	TestAppRendCount(1, app14Results.iRendCount);
	TestAppRendFailCount(0, app14Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 15 Results"));
	TestAppStartCount(1, app15Results.iStartCount); 
	TestAppRendCount(1, app15Results.iRendCount);
	TestAppRendFailCount(0, app15Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 16 Results"));
	TestAppStartCount(1, app16Results.iStartCount); 
	TestAppRendCount(0, app16Results.iRendCount);
	TestAppRendFailCount(0, app16Results.iRendFailCount);
	
	TAppfwkSysstartTestAppResults app17Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp17ResultFileName, app17Results);
	TAppfwkSysstartTestAppResults app18Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp18ResultFileName, app18Results);
	TAppfwkSysstartTestAppResults app19Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp19ResultFileName, app19Results);
	TAppfwkSysstartTestAppResults app20Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp20ResultFileName, app20Results);
	TAppfwkSysstartTestAppResults app21Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp21ResultFileName, app21Results);
	
	INFO_PRINTF1(_L("Test App 17 Results"));
	TestAppStartCount(1, app17Results.iStartCount); 
	TestAppRendCount(1, app17Results.iRendCount);
	TestAppRendFailCount(0, app17Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 18 Results"));
	TestAppStartCount(1, app18Results.iStartCount); 
	TestAppRendCount(1, app18Results.iRendCount);
	TestAppRendFailCount(0, app18Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 19 Results"));
	TestAppStartCount(1, app19Results.iStartCount); 
	TestAppRendCount(1, app19Results.iRendCount);
	TestAppRendFailCount(0, app19Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 20 Results"));
	TestAppStartCount(1, app20Results.iStartCount); 
	TestAppRendCount(1, app20Results.iRendCount);
	TestAppRendFailCount(0, app20Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 21 Results"));
	TestAppStartCount(1, app21Results.iStartCount); 
	TestAppRendCount(0, app21Results.iRendCount);
	TestAppRendFailCount(0, app21Results.iRendFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDefStartAllState::~CAppfwkSysStartTestStepDefStartAllState()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDefStartAllState::CAppfwkSysStartTestStepDefStartAllState()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDefStartAllState);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDefStartAllState::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0057: DefStartAllState - Started"));

 	__UHEAP_MARK;
 	
	TestDefStartAllStateL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0057: DefStartAllState - Finished"));
	
	return TestStepResult();
	}

