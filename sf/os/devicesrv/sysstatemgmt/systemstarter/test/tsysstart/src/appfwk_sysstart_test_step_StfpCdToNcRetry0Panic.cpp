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

#include "appfwk_sysstart_test_step_StfpCdToNcRetry0Panic.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0009
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0009
 */


void CAppfwkSysStartTestStepStfpCdToNcRetry0Panic::TestStfpCdToNcRetry0PanicL()
	{
	GetStfpResultsL();
	TInt csCount=1;
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
	
	// test to see if there was a panic in the last test run
	TestForPanicL();
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepStfpCdToNcRetry0Panic::~CAppfwkSysStartTestStepStfpCdToNcRetry0Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepStfpCdToNcRetry0Panic::CAppfwkSysStartTestStepStfpCdToNcRetry0Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepStfpCdToNcRetry0Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepStfpCdToNcRetry0Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0009: StfpCdToNcRetry0Panic - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestStfpCdToNcRetry0PanicL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0009: StfpCdToNcRetry0Panic - Finished"));
	
	return TestStepResult();
	}

