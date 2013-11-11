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

#include "appfwk_sysstart_test_step_InitApparc.h"
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0078
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0078
 */


void CAppfwkSysStartTestStepInitApparc::TestInitApparcL()
	{
	// read in result files
	TAppfwkSysstartTestAppResults app1Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp1ResultFileName, app1Results);
	TAppfwkSysstartTestAppResults app2Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp2ResultFileName, app2Results);
	TAppfwkSysstartTestAppResults app3Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp3ResultFileName, app3Results);
	
	INFO_PRINTF1(_L("Test App 1 Results"));
	// check to make sure t_app1 started
	TestAppStartCount(1, app1Results.iStartCount); 
	INFO_PRINTF1(_L("Test App 2 Results"));
	// check to make sure t_app2 started, AND
	// sysstart did not wait on t_app1 i.e. fire and forget
	TestAppStartCount(1, app2Results.iStartCount); 
	// check to make sure that t_app2 did rendezvous
	TestAppRendCount(1, app2Results.iRendCount);
	// t_app2 did not cause any rendezvous to fail
	TestAppRendFailCount(0, app2Results.iRendFailCount);
	INFO_PRINTF1(_L("Test App 3 Results"));
	// check to make sure t_app3 started, AND
	// that sysstart did adhere to the waitforstart on
	// t_app2
	TestAppStartCount(1, app3Results.iStartCount);
	TestAppRendCount(1, app3Results.iRendCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepInitApparc::~CAppfwkSysStartTestStepInitApparc()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepInitApparc::CAppfwkSysStartTestStepInitApparc()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepInitApparc);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepInitApparc::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0078: InitApparc - Started"));

 	__UHEAP_MARK;
 	
	TestInitApparcL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0078: InitApparc - Finished"));
	
	return TestStepResult();
	}

