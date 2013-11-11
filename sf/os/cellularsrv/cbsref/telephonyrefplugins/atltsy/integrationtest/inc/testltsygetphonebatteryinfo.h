// TestTelephonyGetPhoneBatteryInfo.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyGetPhoneBatteryInfo


#ifndef TESTTELEPHONYGETPHONEBATTERYINFO_H_
#define TESTTELEPHONYGETPHONEBATTERYINFO_H_

//user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the function to get battery info
 * 
 */
class CTestLtsyGetPhoneBatteryInfo : public CTestLtsyPhoneStepBase  
/**
@internalAll
@test
*/
{
public:
	CTestLtsyGetPhoneBatteryInfo(CTestLtsyModel& aModel);
	virtual ~CTestLtsyGetPhoneBatteryInfo();
	virtual TVerdict doTestStepL();
private:
	void GetBatteryCaps();
	void NotifyBatteryInfoChangeL();

private:
	RMobilePhone::TMobilePhoneBatteryInfoV1      iBatteryInfo;
};

_LIT(KTestLtsyGetPhoneBatteryInfo,           "TestLtsyGetPhoneBatteryInfo");

#endif /*TESTTELEPHONYGETPHONEBATTERYINFOSTEP_H_*/
