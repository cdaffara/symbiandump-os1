//  TestTelephonyOpenPhone.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetFdnStatus
//
#ifndef TESTLTSYPHONEGETFDNSTATUS_H_
#define TESTLTSYPHONEGETFDNSTATUS_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetFdnStatus : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetFdnStatus();
	CTestLtsyPhoneGetFdnStatus(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetFdnStatus,   "TestLtsyPhoneGetFdnStatus");
#endif /*TESTLTSYPHONEGETFDNSTATUS_H_*/
