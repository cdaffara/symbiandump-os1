//  TestLtsyGetDetectedNetworksCancel.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetDetectedNetworksCancel
//
#ifndef TESTLTSYPHONEGETDETTECTEDNWKSCANCEL_H_
#define TESTLTSYPHONEGETDETTECTEDNWKSCANCEL_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetDetectedNetworksCancelCancel : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetDetectedNetworksCancelCancel();
	CTestLtsyPhoneGetDetectedNetworksCancelCancel(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetDetectedNetworksCancelCancel,   "TestLtsyPhoneGetDetectedNetworksCancel");
#endif /*TESTLTSYPHONEGETDETTECTEDNWKSCANCEL_H_*/
