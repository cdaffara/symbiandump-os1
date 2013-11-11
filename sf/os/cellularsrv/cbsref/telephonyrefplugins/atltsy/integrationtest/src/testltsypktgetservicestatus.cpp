// TestLtsyPktGetServiceStatus.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktGetServiceStatus

// 	user include
#include "testltsypktgetservicestatus.h"

/**
 * Constructor
 */
CTestLtsyPktGetServiceStatus::CTestLtsyPktGetServiceStatus(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktGetServiceStatus::~CTestLtsyPktGetServiceStatus()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktGetServiceStatus::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,DisplayPacketServiceStatus());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
