// TestLtsyPktSetContextConfig.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktSetContextConfig

// 	user include
#include "TestLtsyPktSetContextConfig.h"

/**
 * Constructor
 */
CTestLtsyPktSetContextConfig::CTestLtsyPktSetContextConfig(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktSetContextConfig);
	}

/**
 * Destructor
 */
CTestLtsyPktSetContextConfig::~CTestLtsyPktSetContextConfig()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktSetContextConfig::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsContextSetConfigTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
