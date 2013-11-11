//  TestLtsyPhoneGetDetectedNetworksCancel.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetDetectedNetworksCancel

// user include
#include "TestLtsyPhoneGetDetectedNetworksCancel.h"


/**
* Constructor
* 
*/
CTestLtsyPhoneGetDetectedNetworksCancelCancel::CTestLtsyPhoneGetDetectedNetworksCancelCancel( CTestLtsyModel& aTestModel) 
    :CTestLtsyPhoneStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPhoneGetDetectedNetworksCancelCancel::~CTestLtsyPhoneGetDetectedNetworksCancelCancel()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPhoneGetDetectedNetworksCancelCancel::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,GetDetectedNetworksCancelL());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	
	return TestStepResult();
	}

	
// end of file
