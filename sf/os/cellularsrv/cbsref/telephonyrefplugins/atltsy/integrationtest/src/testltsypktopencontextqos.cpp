// TestLtsyPktOpenContextQos.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktOpenContextQos

// 	user include
#include "TestLtsyPktOpenContextQos.h"

/**
 * Constructor
 */
CTestLtsyPktOpenContextQos::CTestLtsyPktOpenContextQos(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktOpenContextQos::~CTestLtsyPktOpenContextQos()
	{
	}
/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktOpenContextQos::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,OpenNewContextQos());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
