//  TestLtsyGetCurrentNetworkInfo.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetCurrentNetworkInfo
//
#ifndef TESTLTSYPHONEGETCURRENTNWKINFO_H_
#define TESTLTSYPHONEGETCURRENTNWKINFO_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetCurrentNetworkInfo : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetCurrentNetworkInfo();
	CTestLtsyPhoneGetCurrentNetworkInfo(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetCurrentNetworkInfo,   "TestLtsyPhoneGetCurrentNetworkInfo");
#endif /*TESTLTSYPHONEGETCURRENTNWKINFO_H_*/
