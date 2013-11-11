//  TestLtsyPhoneGetPhoneID.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetPhoneID
//
#ifndef TESTLTSYPHONEGETPHONEID_H_
#define TESTLTSYPHONEGETPHONEID_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetPhoneID : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetPhoneID();
	CTestLtsyPhoneGetPhoneID(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetPhoneID,   "TestLtsyPhoneGetPhoneID");
#endif /*TESTLTSYPHONEGETPHONEID_H_*/
