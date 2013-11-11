// TestLtsyPktGetDynamicCapabilities.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktGetDynamicCapabilities

// 	user include
#include "TestLtsyPktGetDynamicCapabilities.h"

/**
 * Constructor
 */
CTestLtsyPktGetDynamicCapabilities::CTestLtsyPktGetDynamicCapabilities(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktGetDynamicCapabilities::~CTestLtsyPktGetDynamicCapabilities()
	{
	}
/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktGetDynamicCapabilities::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		TRAPD(err,DisplayDynamicCapabilities());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
