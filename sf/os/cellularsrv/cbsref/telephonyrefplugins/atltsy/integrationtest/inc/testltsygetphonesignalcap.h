// TestLtsyGetPhoneSignalCap.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestLtsyGetPhoneSignalCap

#ifndef TESTTELEPHONYGETPHONESIGNALCAP_H_
#define TESTTELEPHONYGETPHONESIGNALCAP_H_

//user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the function to get signal strength
 * 
 */
class CTestLtsyGetPhoneSignalCap : public CTestLtsyPhoneStepBase  
/**
@internalAll
@test
*/
{
public:
	CTestLtsyGetPhoneSignalCap(CTestLtsyModel& aModel);
	virtual ~CTestLtsyGetPhoneSignalCap();	
	virtual TVerdict doTestStepL();
};

_LIT(KTestLtsyGetPhoneSingalCap,          "TestLtsyGetPhoneSingalCap");

#endif /*TESTTELEPHONYGETPHONESIGNALCAP_H_*/


