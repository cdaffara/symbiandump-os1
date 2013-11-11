//  TestLtsyPhoneGetPhoneID.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetPhoneID

// user include
#include "TestLtsyPhoneGetPhoneID.h"


/**
* Constructor
* 
*/
CTestLtsyPhoneGetPhoneID::CTestLtsyPhoneGetPhoneID( CTestLtsyModel& aTestModel) 
    :CTestLtsyPhoneStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPhoneGetPhoneID::~CTestLtsyPhoneGetPhoneID()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPhoneGetPhoneID::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GetPhoneId());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	
	return TestStepResult();
	}

	
// end of file
