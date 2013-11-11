//  TestLtsyPhoneGetHomeNetwork.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetHomeNetwork

// user include
#include "TestLtsyPhoneGetHomeNetwork.h"


/**
* Constructor
* 
*/
CTestLtsyPhoneGetHomeNetwork::CTestLtsyPhoneGetHomeNetwork( CTestLtsyModel& aTestModel) 
    :CTestLtsyPhoneStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPhoneGetHomeNetwork::~CTestLtsyPhoneGetHomeNetwork()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPhoneGetHomeNetwork::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GetHomeNetwork());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	
	return TestStepResult();
	}

	
// end of file
