// TestLtsyPktGetContextConfig.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktGetContextConfig

// 	user include
#include "TestLtsyPktGetContextConfig.h"

/**
 * Constructor
 */
CTestLtsyPktGetContextConfig::CTestLtsyPktGetContextConfig(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktGetContextConfig);
	}

/**
 * Destructor
 */
CTestLtsyPktGetContextConfig::~CTestLtsyPktGetContextConfig()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktGetContextConfig::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsContextGetConfigTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
