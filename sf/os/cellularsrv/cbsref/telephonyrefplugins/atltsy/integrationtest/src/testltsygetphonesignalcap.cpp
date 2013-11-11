// TestLtsyGetPhoneSignalCap.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyGetPhoneSignalCap

//system include
#include <etelmm.h>

//user include

#include "TestLtsyGetPhoneSignalCap.h"

/**
 * Constructor
 * 
 */
CTestLtsyGetPhoneSignalCap::CTestLtsyGetPhoneSignalCap(CTestLtsyModel& aModel)
    :CTestLtsyPhoneStepBase(aModel)
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyGetPhoneSignalCap::~CTestLtsyGetPhoneSignalCap()
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

TVerdict CTestLtsyGetPhoneSignalCap::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestTelePhonyGetPhoneInfo::doTestStepL Begin"));
	if (TestStepResult() == EPass)
		{
    	TRAPD(err,GetSignalCaps());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

