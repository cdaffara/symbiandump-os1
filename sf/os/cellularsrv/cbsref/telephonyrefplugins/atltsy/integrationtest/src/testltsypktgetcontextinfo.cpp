// TestLtsyPktGetContextInfo.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktGetContextInfo

// 	user include
#include "TestLtsyPktGetContextInfo.h"

/**
 * Constructor
 */
CTestLtsyPktGetContextInfo::CTestLtsyPktGetContextInfo(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktGetContextInfo::~CTestLtsyPktGetContextInfo()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktGetContextInfo::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,DisplayContextInfo());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
