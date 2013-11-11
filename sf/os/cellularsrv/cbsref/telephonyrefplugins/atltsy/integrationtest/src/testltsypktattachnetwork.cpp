// TestLtsyPktAttachNetwork.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktAttachNetwork

// 	user include
#include "TestLtsyPktAttachNetwork.h"

/**
 * Constructor
 */
CTestLtsyPktAttachNetwork::CTestLtsyPktAttachNetwork(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktAttachNetwork::~CTestLtsyPktAttachNetwork()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktAttachNetwork::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsContextAttachTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
