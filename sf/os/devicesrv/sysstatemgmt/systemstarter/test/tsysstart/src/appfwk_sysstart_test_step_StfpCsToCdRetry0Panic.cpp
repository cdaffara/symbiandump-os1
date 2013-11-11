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

#include "appfwk_sysstart_test_step_StfpCsToCdRetry0Panic.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0004
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0004
 */


void CAppfwkSysStartTestStepStfpCsToCdRetry0Panic::TestStfpCsToCdRetry0PanicL()
	{
	GetStfpResultsL();
	TInt csCount=1;
	TestCsCount(csCount);
	
	TInt csFailCount=0;
	TestCsFailCount(csFailCount);
	
	TInt cdCount=1;
	TestCdCount(cdCount);
	
	TInt cdFailCount=1;
	TestCdFailCount(cdFailCount);
	
	TInt ncCount=0;
	TestNcCount(ncCount);
	
	TInt ncFailCount=0;
	TestNcFailCount(ncFailCount);
	
	TestForPanicL();
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepStfpCsToCdRetry0Panic::~CAppfwkSysStartTestStepStfpCsToCdRetry0Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepStfpCsToCdRetry0Panic::CAppfwkSysStartTestStepStfpCsToCdRetry0Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepStfpCsToCdRetry0Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepStfpCsToCdRetry0Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0004: StfpCsToCdRetry0Panic - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestStfpCsToCdRetry0PanicL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0004: StfpCsToCdRetry0Panic - Finished"));
	
	return TestStepResult();
	}

