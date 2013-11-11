// TestLtsyPktDeleteContext.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktDeleteContext

// 	user include
#include "TestLtsyPktDeleteContext.h"

/**
 * Constructor
 */
CTestLtsyPktDeleteContext::CTestLtsyPktDeleteContext(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktDeleteContext);
	}

/**
 * Destructor
 */
CTestLtsyPktDeleteContext::~CTestLtsyPktDeleteContext()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktDeleteContext::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsContextDeleteTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
