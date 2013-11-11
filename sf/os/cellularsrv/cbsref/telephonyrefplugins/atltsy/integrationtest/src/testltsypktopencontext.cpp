// TestLtsyPktOpenContext.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktOpenContext

// 	user include
#include "TestLtsyPktOpenContext.h"

/**
 * Constructor
 */
CTestLtsyPktOpenContext::CTestLtsyPktOpenContext(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktOpenContext::~CTestLtsyPktOpenContext()
	{
	}
/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktOpenContext::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		if (!IsOpenedPktService())
			{
			OpenPacketobject();
			}
    	TRAPD(err,OpenNewContext());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
