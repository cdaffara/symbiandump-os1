//  TestLtsyRegisterCellInfoChangeNotification.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPhoneRegisterCellInfoChangeNotification
//
#ifndef TESTLTSYPHONEREGCELLINFOCHANGENOTIFICTION_H_
#define TESTLTSYPHONEREGCELLINFOCHANGENOTIFICTION_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the funcition to open a shared phone.
 * 
 */
class CTestLtsyPhoneRegisterCellInfoChangeNotification : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyPhoneRegisterCellInfoChangeNotification();
	CTestLtsyPhoneRegisterCellInfoChangeNotification(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyPhoneRegisterCellInfoChangeNotification,   "TestLtsyPhoneRegisterCellInfoChangeNotification");
#endif /*TESTLTSYPHONEREGCELLINFOCHANGENOTIFICTION_H_*/
