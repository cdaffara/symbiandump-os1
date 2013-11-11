// TestLtsyPktDetachNetwork.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktDetachNetwork

// 	user include
#include "TestLtsyPktDetachNetwork.h"

/**
 * Constructor
 */
CTestLtsyPktDetachNetwork::CTestLtsyPktDetachNetwork(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyPktDetachNetwork::~CTestLtsyPktDetachNetwork()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktDetachNetwork::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsContextDetachTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
