// TestLtsyPktGetContextQoSProfile.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktGetContextQoSProfile

// 	user include
#include "TestLtsyPktGetContextQoSProfile.h"

/**
 * Constructor
 */
CTestLtsyPktGetContextQoSProfile::CTestLtsyPktGetContextQoSProfile(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktGetContextQoSProfile);
	}

/**
 * Destructor
 */
CTestLtsyPktGetContextQoSProfile::~CTestLtsyPktGetContextQoSProfile()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktGetContextQoSProfile::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GprsGetQoSProfileTest());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
