// TestLtsyPktCloseContext.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktCloseContext

// 	user include
#include "TestLtsyPktCloseContext.h"

/**
 * Constructor
 */
CTestLtsyPktCloseContext::CTestLtsyPktCloseContext(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktCloseContext::~CTestLtsyPktCloseContext()
	{
	}
/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktCloseContext::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		CloseExistContext();
		}
	return TestStepResult();
	}

// end of file
