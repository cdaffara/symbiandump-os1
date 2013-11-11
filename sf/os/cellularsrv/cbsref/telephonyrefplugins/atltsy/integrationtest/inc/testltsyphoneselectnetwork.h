//  TestLtsySelectNetwork.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneSelectNetwork
//
#ifndef TESTLTSYPHONESELECTNETWORK_H_
#define TESTLTSYPHONESELECTNETWORK_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneSelectNetwork : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneSelectNetwork();
	CTestLtsyPhoneSelectNetwork(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneSelectNetwork,   "TestLtsyPhoneSelectNetwork");
#endif /*TESTLTSYPHONESELECTNETWORK_H_*/
