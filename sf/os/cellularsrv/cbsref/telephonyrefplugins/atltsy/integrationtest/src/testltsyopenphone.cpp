//  TestTelephonyOpenPhone.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyOpenPhone

// user include
#include "TestLtsyOpenPhone.h"
#include "TestLtsyCallSharedData.h"
#include "TestLtsySmsSharedData.h"
#include "TestLtsyPktSharedData.h"
#include "TestLtsyPhoneStoreSharedData.h"


/**
* Constructor
* 
*/
CTestLtsyOpenPhone::CTestLtsyOpenPhone( CTestLtsyModel& aTestModel) 
    :CTestLtsyPhoneStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyOpenPhone::~CTestLtsyOpenPhone()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyOpenPhone::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,TestOpenPhoneL());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	
	return TestStepResult();
	}

	
// end of file
