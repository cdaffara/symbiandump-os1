//  TestLtsyPhoneGetNetworkRegistrationStatus.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetNetworkRegistrationStatus
//
#ifndef TESTLTSYPHONEGETNWKREGISTSTATUS_H_
#define TESTLTSYPHONEGETNWKREGISTSTATUS_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetNetworkRegistrationStatus : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetNetworkRegistrationStatus();
	CTestLtsyPhoneGetNetworkRegistrationStatus(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetNetworkRegistrationStatus,   "TestLtsyPhoneGetNetworkRegistrationStatus");
#endif /*TESTLTSYPHONEGETNWKREGISTSTATUS_H_*/
