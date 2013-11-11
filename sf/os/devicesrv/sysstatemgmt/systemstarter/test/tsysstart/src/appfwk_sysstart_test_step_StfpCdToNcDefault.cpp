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

#include "appfwk_sysstart_test_step_StfpCdToNcDefault.h"


/**
Old Test CaseID 		APPFWK-SYSSTART-0007
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0007
 */


void CAppfwkSysStartTestStepStfpCdToNcDefault::TestStfpCdToNcDefaultL()
	{
	GetStfpResultsL();

	TInt csCount = 0;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	// Critical Static count will be 2 when sysstart is started using sysstatemgr.exe, one cs for sysstart and 
	// second one is for sysstatemgr.exe as it would be in critical static state once sysstart completes.
	csCount = 2;
#else
	csCount = 1;
#endif
	TestCsCount(csCount);
	
	TInt csFailCount=0;
	TestCsFailCount(csFailCount);
	
	TInt cdCount=1;
	TestCdCount(cdCount);
	
	TInt cdFailCount=0;
	TestCdFailCount(cdFailCount);
	
	TInt ncCount=1;
	TestNcCount(ncCount);
	
	TInt ncFailCount=1;
	TestNcFailCount(ncFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepStfpCdToNcDefault::~CAppfwkSysStartTestStepStfpCdToNcDefault()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepStfpCdToNcDefault::CAppfwkSysStartTestStepStfpCdToNcDefault()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepStfpCdToNcDefault);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepStfpCdToNcDefault::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0007: StfpCdToNcDefault - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestStfpCdToNcDefaultL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0007: StfpCdToNcDefault - Finished"));
	
	return TestStepResult();
	}

