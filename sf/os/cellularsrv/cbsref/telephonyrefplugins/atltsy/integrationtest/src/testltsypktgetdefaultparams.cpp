// TestLtsyPktGetDefaultParams.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktGetDefaultParams

// 	user include
#include "TestLtsyPktGetDefaultParams.h"

/**
 * Constructor
 */
CTestLtsyPktGetDefaultParams::CTestLtsyPktGetDefaultParams(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktGetDefaultParams);
	}

/**
 * Destructor
 */
CTestLtsyPktGetDefaultParams::~CTestLtsyPktGetDefaultParams()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktGetDefaultParams::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsGetDefaultContextParamsTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
