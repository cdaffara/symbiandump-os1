//  TestLtsyPktClosePacket.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktClosePacket

// user include

#include "TestLtsyPktClosePacket.h"

/**
* Constructor
* 
*/
CTestLtsyPktClosePacket::CTestLtsyPktClosePacket( CTestLtsyModel& aTestModel) 
    :CTestLtsyPktTestStepBase(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPktClosePacket::~CTestLtsyPktClosePacket()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktClosePacket::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
//		if (iModel.SharedEnv().PhoneOpened())
//			{
//			iModel.SharedEnv().ClosePhone();	
//			}
    	ClosePacketobject();
		}
	
	return TestStepResult();
	}

	
// end of file
