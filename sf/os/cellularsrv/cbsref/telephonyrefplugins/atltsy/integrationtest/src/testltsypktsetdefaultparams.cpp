// CTestLtsyPktSetDefaultParams.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktSetDefaultParams

// 	user include
#include "TestLtsyPktSetDefaultParams.h"

/**
 * Constructor
 */
CTestLtsyPktSetDefaultParams::CTestLtsyPktSetDefaultParams(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktSetDefaultParams);
	}

/**
 * Destructor
 */
CTestLtsyPktSetDefaultParams::~CTestLtsyPktSetDefaultParams()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktSetDefaultParams::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsSetDefaultContextParamsTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
