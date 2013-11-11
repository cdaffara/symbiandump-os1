// TestTelephonyGetPhoneTime.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyGetPhoneTime

//system include
#include <etelmm.h>
#include <e32std.h>

//user include

#include "TestLtsyGetPhoneBatteryInfo.h"


/** Supported battery information capabilities. The supported 
* capabilities are returned to the client when it requests them by calling
* CMmPhoneTsy::GetBatteryCaps() 
*/
/**
 * Constructor
 * 
 */
CTestLtsyGetPhoneBatteryInfo::CTestLtsyGetPhoneBatteryInfo(CTestLtsyModel& aModel) 
    : CTestLtsyPhoneStepBase(aModel)
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyGetPhoneBatteryInfo::~CTestLtsyGetPhoneBatteryInfo()
	{
	}

/**
 * Invoked by the test execute framework for exercising functionality under test
 * @return	EPass: The test passed succesfully;
 EFail: The test failed;
 EInconclusive: The result of the test was inconclusive;
 ETestSuiteError: Error in the test suite;
 EAbort: The test was aborted;
 EIgnore: The test was unexecuted
 */
TVerdict CTestLtsyGetPhoneBatteryInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyGetPhoneInfo::doTestStepL Begin"));
	if (TestStepResult() == EPass)
		{
		GetBatteryCaps();
		TRequestStatus reqStatus;
		SharedPhone().GetBatteryInfo(reqStatus, iBatteryInfo);
		User::WaitForRequest(reqStatus);
		if (reqStatus==KErrNone)
			{
			INFO_PRINTF2(_L("Battery status : %d\n"), iBatteryInfo.iStatus);
			INFO_PRINTF2(_L("Battery charge level : %d\n"), iBatteryInfo.iChargeLevel);
			}
		else
			{
			INFO_PRINTF2(_L("Battery charge, Error %d returned.\n"), reqStatus.Int());
			}
		}
	return TestStepResult();
	}

/**
 * 
 * Get battery caps
 * 
 */
void CTestLtsyGetPhoneBatteryInfo::GetBatteryCaps()
	{
	TUint32 caps;
	TInt res = SharedPhone().GetBatteryCaps(caps);
	if (res != KErrNone)
		{
		ERR_PRINTF1(_L("Failed to Check return value."));
		}
	}

/**
 * 
 * Notify battery info change or not
 * 
 */
void CTestLtsyGetPhoneBatteryInfo::NotifyBatteryInfoChangeL()
	{
	TRequestStatus status;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	TUint twentyCharge = 20;
	SharedPhone().NotifyBatteryInfoChange(status, batInfo);

	User::WaitForRequest(status);
	if(batInfo.iStatus != RMobilePhone::EBatteryConnectedButExternallyPowered)
		{
		INFO_PRINTF1(_L("Failed to Check Phone battery status."));
		}
	if(batInfo.iChargeLevel != twentyCharge)
		{
		INFO_PRINTF1(_L("Failed to Check Phone battery charge level."));
		}
	}
