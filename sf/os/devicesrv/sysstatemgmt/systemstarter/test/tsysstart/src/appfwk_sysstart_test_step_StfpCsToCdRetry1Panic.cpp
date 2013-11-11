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

#include "appfwk_sysstart_test_step_StfpCsToCdRetry1Panic.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0005
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0005
 */


void CAppfwkSysStartTestStepStfpCsToCdRetry1Panic::TestStfpCsToCdRetry1PanicL()
	{
	GetStfpResultsL();
	TInt csCount=1;
	TestCsCount(csCount);
	
	TInt csFailCount=0;
	TestCsFailCount(csFailCount);
	
	TInt cdCount=2;
	TestCdCount(cdCount);
	
	TInt cdFailCount=2;
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
CAppfwkSysStartTestStepStfpCsToCdRetry1Panic::~CAppfwkSysStartTestStepStfpCsToCdRetry1Panic()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepStfpCsToCdRetry1Panic::CAppfwkSysStartTestStepStfpCsToCdRetry1Panic()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepStfpCsToCdRetry1Panic);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepStfpCsToCdRetry1Panic::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0005: StfpCsToCdRetry1Panic - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestStfpCsToCdRetry1PanicL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0005: StfpCsToCdRetry1Panic - Finished"));
	
	return TestStepResult();
	}

