// testltsypktgetnwkregistrationstatus.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyPktNetworkRegistrationStatus

// 	user include
#include "testltsypktgetnwkregistrationstatus.h"

/**
 * Constructor
 */
CTestLtsyPktNetworkRegistrationStatus::CTestLtsyPktNetworkRegistrationStatus(CTestLtsyModel& aModel)
	:CTestLtsyPktTestStepBase(aModel)
	{
	SetTestStepName(KTestLtsyPktNetworkRegistrationStatus);
	}

/**
 * Destructor
 */
CTestLtsyPktNetworkRegistrationStatus::~CTestLtsyPktNetworkRegistrationStatus()
	{
	}

/**
 * It implements the required functionality of the test step.
 * This step will open a shared phone
 * @return TVerdict - result of the test step
 * @leave - System wide error codes
 */
TVerdict CTestLtsyPktNetworkRegistrationStatus::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
    	TRAPD(err,DisplayNetworkRegistrationStatus());
    	if(err != KErrNone)
    		{
    		INFO_PRINTF2(_L("The error was returned %d!! when open Context"),err);
    	    SetTestStepResult(EFail);
    		}
		}
	return TestStepResult();
	}

// end of file
