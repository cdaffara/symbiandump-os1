// TestLtsyPktCloseContextQos.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktCloseContextQos

// 	user include
#include "TestLtsyPktCloseContextQos.h"

/**
 * Constructor
 */
CTestLtsyPktCloseContextQos::CTestLtsyPktCloseContextQos(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	}

/**
 * Destructor
 */
CTestLtsyPktCloseContextQos::~CTestLtsyPktCloseContextQos()
	{
	}
/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktCloseContextQos::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	CloseExistContextQos();
		}
	return TestStepResult();
	}

// end of file
