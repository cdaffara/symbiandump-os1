//  TestLtsyGetHomeNetwork.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetHomeNetwork
//
#ifndef TESTLTSYPHONEGETHOMENWK_H_
#define TESTLTSYPHONEGETHOMENWK_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetHomeNetwork : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetHomeNetwork();
	CTestLtsyPhoneGetHomeNetwork(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetHomeNetwork,   "TestLtsyPhoneGetHomeNetwork");
#endif /*TESTLTSYPHONEGETHOMENWK_H_*/
