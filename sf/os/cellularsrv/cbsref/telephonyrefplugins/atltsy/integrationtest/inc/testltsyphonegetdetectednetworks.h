//  TestLtsyGetDetectedNetworks.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetDetectedNetworks
//
#ifndef TESTLTSYPHONEGETDETTECTEDNWKS_H_
#define TESTLTSYPHONEGETDETTECTEDNWKS_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetDetectedNetworks : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetDetectedNetworks();
	CTestLtsyPhoneGetDetectedNetworks(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetDetectedNetworks,   "TestLtsyPhoneGetDetectedNetworks");
#endif /*TESTLTSYPHONEGETDETTECTEDNWKS_H_*/
