//  TestLtsyPhoneSetNetworkSelectionSetting.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneSetNetworkSelectionSetting

// user include
#include "TestLtsyPhoneSetNetworkSelectionSetting.h"


/**
* Constructor
* 
*/
CTestLtsyPhoneSetNetworkSelectionSetting::CTestLtsyPhoneSetNetworkSelectionSetting( CTestLtsyModel& aTestModel) 
    :CTestLtsyPhoneStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPhoneSetNetworkSelectionSetting::~CTestLtsyPhoneSetNetworkSelectionSetting()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPhoneSetNetworkSelectionSetting::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,SetNetworkSelectionSetting());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	
	return TestStepResult();
	}

	
// end of file
