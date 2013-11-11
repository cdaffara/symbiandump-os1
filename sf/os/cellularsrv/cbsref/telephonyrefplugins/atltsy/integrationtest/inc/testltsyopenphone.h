//  TestTelephonyOpenPhone.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyOpenPhone
//
#ifndef TESTTELEPHONYOPENPHONE_H_
#define TESTTELEPHONYOPENPHONE_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyOpenPhone : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyOpenPhone();
	CTestLtsyOpenPhone(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyOpenPhone,   "TestLtsyOpenPhone");
#endif /*TESTTELEPHONYOPENPHONE_H_*/
