// TestTelephonyGetPhoneSingalStrength.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyGetPhoneSingalStrength

#ifndef TESTTELEPHONYGETPHONESIGNALSTRENGTH_H_
#define TESTTELEPHONYGETPHONESIGNALSTRENGTH_H_

//user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the function to get signal strength
 * 
 */
class CTestLtsyGetPhoneSignalStrength : public CTestLtsyPhoneStepBase  
/**
@internalAll
@test
*/
{
public:
	CTestLtsyGetPhoneSignalStrength(CTestLtsyModel& aModel);
	virtual ~CTestLtsyGetPhoneSignalStrength();	
	virtual TVerdict doTestStepL();
private:
	void   GetSignalCaps();
	void   NotifySignalStrengthChangeL();

private:
	TInt32                                       iSignalStrength;
	TInt8                                        iBar;
};

_LIT(KTestLtsyGetPhoneSingalStrength,          "TestLtsyGetPhoneSingalStrength");

#endif /*TESTTELEPHONYGETPHONESIGNALSTRENGTH_H_*/


