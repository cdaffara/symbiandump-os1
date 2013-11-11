// TestLtsyGetPhoneBatteryCap.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyGetPhoneBatteryCap

//system include
#include <etelmm.h>
#include <e32std.h>

//user include

#include "TestLtsyGetPhoneBatteryCap.h"


/** Supported battery information capabilities. The supported 
* capabilities are returned to the client when it requests them by calling
* CMmPhoneTsy::GetBatteryCaps() 
*/
/**
 * Constructor
 * 
 */
CTestLtsyGetPhoneBatteryCap::CTestLtsyGetPhoneBatteryCap(CTestLtsyModel& aModel) 
    : CTestLtsyPhoneStepBase(aModel)
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyGetPhoneBatteryCap::~CTestLtsyGetPhoneBatteryCap()
	{
	}

/**
 * Invoked by the test execute framework for exercising functionality under test
 * @return	EPass: The test passed succesfully;
 EFail: The test failed;
 EInconclusive: The result of the test was inconclusive;
 ETestSuiteError: Error in the test suite;
 EAbort: The test was aborted;
 EIgnore: The test was unexecuted
 */
TVerdict CTestLtsyGetPhoneBatteryCap::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyGetPhoneInfo::doTestStepL Begin"));
	if (TestStepResult() == EPass)
		{
    	TRAPD(err,GetBatteryCaps());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

