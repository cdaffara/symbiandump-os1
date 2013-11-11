//  TestLtsyGetCellInfo.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneGetCellInfo
//
#ifndef TESTLTSYPHONEGETCELLINFO_H_
#define TESTLTSYPHONEGETCELLINFO_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneGetCellInfo : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneGetCellInfo();
	CTestLtsyPhoneGetCellInfo(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneGetCellInfo,   "TestLtsyPhoneGetCellInfo");
#endif /*TESTLTSYPHONEGETCELLINFO_H_*/
