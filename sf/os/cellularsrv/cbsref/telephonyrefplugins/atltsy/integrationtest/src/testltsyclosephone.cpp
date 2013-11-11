//  TestTelephonyClosePhone.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyClosePhone

// user include
#include "TestLtsyClosePhone.h"

// member functions

/**
* Constructor
* 
*/
CTestLtsyClosePhone::CTestLtsyClosePhone( CTestLtsyModel& aTestModel) 
    :CTestLtsyPhoneStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyClosePhone::~CTestLtsyClosePhone()
	{
	
	}

/**
 * It implements the required functionality of the test step.
 * This step will close the shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyClosePhone::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TestClosePhone();
		}

	return TestStepResult();
	}

// end of file
