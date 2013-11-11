// TestTelephonyGetPhoneBatteryInfo.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyGetPhoneBatteryCap


#ifndef TESTTELEPHONYGETPHONEBATTERYCAP_H_
#define TESTTELEPHONYGETPHONEBATTERYCAP_H_

//user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the function to get battery info
 * 
 */
class CTestLtsyGetPhoneBatteryCap : public CTestLtsyPhoneStepBase  
/**
@internalAll
@test
*/
{
public:
	CTestLtsyGetPhoneBatteryCap(CTestLtsyModel& aModel);
	virtual ~CTestLtsyGetPhoneBatteryCap();
	virtual TVerdict doTestStepL();

};

_LIT(KTestLtsyGetPhoneBatteryCap,           "TestLtsyGetPhoneBatteryCap");

#endif /*TESTTELEPHONYGETPHONEBATTERYCAP_H_*/
