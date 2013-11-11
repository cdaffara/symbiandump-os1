//  TestLtsySetNetworkSelectionSetting.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneSetNetworkSelectionSetting
//
#ifndef TESTLTSYPHONESETNWKSETTING_H_
#define TESTLTSYPHONESETNWKSETTING_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneSetNetworkSelectionSetting : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneSetNetworkSelectionSetting();
	CTestLtsyPhoneSetNetworkSelectionSetting(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneSetNetworkSelectionSetting,   "TestLtsyPhoneSetNetworkSelectionSetting");
#endif /*TESTLTSYPHONESETNWKSETTING_H_*/
