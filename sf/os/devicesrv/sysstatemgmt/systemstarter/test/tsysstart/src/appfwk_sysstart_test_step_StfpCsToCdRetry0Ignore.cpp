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

#include "appfwk_sysstart_test_step_StfpCsToCdRetry0Ignore.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0001
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0001
 */


void CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore::TestStfpCsToCdRetry0IgnoreL()
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
	
	TInt cdFailCount=1;
	TestCdFailCount(cdFailCount);
	
	TInt ncCount=1;
	TestNcCount(ncCount);
	
	TInt ncFailCount=0;
	TestNcFailCount(ncFailCount);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore::~CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore::CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepStfpCsToCdRetry0Ignore);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0001: StfpCsToCdRetry0Ignore - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestStfpCsToCdRetry0IgnoreL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0001: StfpCsToCdRetry0Ignore - Finished"));
	
	return TestStepResult();
	}

