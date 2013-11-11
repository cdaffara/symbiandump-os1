// TestLtsyPktSetContextQoSProfile.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktSetContextQoSProfile

// 	user include
#include "TestLtsyPktSetContextQoSProfile.h"

/**
 * Constructor
 */
CTestLtsyPktSetContextQoSProfile::CTestLtsyPktSetContextQoSProfile(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktSetContextQoSProfile);
	}

/**
 * Destructor
 */
CTestLtsyPktSetContextQoSProfile::~CTestLtsyPktSetContextQoSProfile()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktSetContextQoSProfile::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsSetQoSProfileTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
