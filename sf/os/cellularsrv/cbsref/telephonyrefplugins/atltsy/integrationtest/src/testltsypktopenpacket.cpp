//  TestLtsyPktOpenPPacket.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktOpenPacket

// user include

#include "TestLtsyPktOpenPacket.h"
_LIT(KTsyModuleName, "PhoneTsy");
/**
* Constructor
* 
*/
CTestLtsyPktOpenPacket::CTestLtsyPktOpenPacket( CTestLtsyModel& aTestModel) 
    :CTestLtsyPktTestStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPktOpenPacket::~CTestLtsyPktOpenPacket()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktOpenPacket::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		if (!iModel.SharedEnv().PhoneOpened())
			{
			iModel.SharedEnv().OpenPhoneL(KTsyModuleName);
			}
    	TRAPD(err,OpenPacketobject());
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("The error was returned %d!! when open Phone"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	
	return TestStepResult();
	}

	
// end of file
