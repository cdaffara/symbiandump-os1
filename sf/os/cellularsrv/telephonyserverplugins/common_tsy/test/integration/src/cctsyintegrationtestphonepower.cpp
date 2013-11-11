// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Test step definitions for the PhonePower functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestphonepower.h"
#include "cetelsessionmgr.h"


	
CCTSYIntegrationTestPhonePowerBase::CCTSYIntegrationTestPhonePowerBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iPhoneTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhonePowerBase::~CCTSYIntegrationTestPhonePowerBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPhonePower0001::CCTSYIntegrationTestPhonePower0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhonePowerBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhonePower0001::GetTestStepName());
	}

CCTSYIntegrationTestPhonePower0001::~CCTSYIntegrationTestPhonePower0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhonePower0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PP-0001
 * @SYMFssID BA/CTSY/PP-0001
 * @SYMTestCaseDesc Get battery capabilities and information. Verify capabilities are correct and charge level is a valid percentage
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetBatteryCaps, RMobilePhone::GetBatteryInfo
 * @SYMTestExpectedResults Pass - Battery caps and information returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify capabilities are correct and charge level is a valid percentage.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// Get the battery capabilities and information.
	// Verify capabilities are correct and charge level is a valid percentage.

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetBatteryCaps returns caps in set of KCapsGetBatteryInfo | KCapsNotifyBatteryInfoChange
	TUint32 batteryCaps;
	ASSERT_EQUALS(mobilePhone.GetBatteryCaps(batteryCaps), KErrNone, 
					_L("RMobilePhone::GetBatteryCaps returned error"))
	TUint32 expectedCaps = 
							  RMobilePhone::KCapsGetBatteryInfo
					        | RMobilePhone::KCapsNotifyBatteryInfoChange;
	TUint32 unwantedCaps = 0;
					
	ASSERT_BITS_SET(batteryCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetBatteryCaps returned incorrect caps"))
	
	TExtEtelRequestStatus requestStatus(mobilePhone,EMobilePhoneGetBatteryInfo);	
	RMobilePhone::TMobilePhoneBatteryInfoV1  batteryInfo;

	// Check RMobilePhone::GetBatteryInfo returns aInfo.iStatus = EPoweredByBattery
	// Check RMobilePhone::GetBatteryInfo returns aInfo.iChargeLevel > 0 but <= 100
	mobilePhone.GetBatteryInfo(requestStatus, batteryInfo);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestStatus, ETimeShort), 
			KErrNone, _L("RMobilePhone::GetBatteryInfo did not complete."))
    ASSERT_EQUALS(requestStatus.Int(),KErrNone , 
    		_L("RMobilePhone::GetBatteryInfo completed with incorrect error"))
    ASSERT_EQUALS(batteryInfo.iStatus,RMobilePhone::EPoweredByBattery, 
    		_L("RMobilePhone::GetBatteryInfo returned unexpected battery status"))
    ASSERT_TRUE(batteryInfo.iChargeLevel > 0 && batteryInfo.iChargeLevel <= 100 , 
    		_L("RMobilePhone::GetBatteryInfo returned incorrect charge level"))

	
	//
	// TEST END
	//

    StartCleanup();
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhonePower0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhonePower0001");
	}



CCTSYIntegrationTestPhonePower0002::CCTSYIntegrationTestPhonePower0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhonePowerBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhonePower0002::GetTestStepName());
	}

CCTSYIntegrationTestPhonePower0002::~CCTSYIntegrationTestPhonePower0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhonePower0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PP-0002
 * @SYMFssID BA/CTSY/PP-0002
 * @SYMTestCaseDesc Plug and unplug the phone charger.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetBatteryInfo
 * @SYMTestExpectedResults Pass - Battery info changes when charger is plugged and unplugged.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify battery info change notifications received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RMobilePhone::GetBatteryCaps returns caps in set of KCapsGetBatteryInfo | KCapsNotifyBatteryInfoChange 
	TUint32 caps;

	CHECK_EQUALS_L(mobilePhone.GetBatteryCaps(caps), KErrNone, _L("RMobilePhone::GetBatteryCaps return value is not KErrNone"));
	
	TUint32 expectedCaps = RMobilePhone::KCapsGetBatteryInfo | RMobilePhone::KCapsNotifyBatteryInfoChange;
	CHECK_BITS_SET_L(caps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetBatteryCaps returned wrong caps"));
	
	// Ensure charger is plugged into the phone. 
	TBuf<120> plugPrompt(_L("Ensure charger is plugged into the phone"));
	DisplayUserInteractionPromptL(plugPrompt); // 30 seconds
		
	// Ensure RMobilePhone::GetBatteryInfo returns aInfo.iStatus = EBatteryConnectedButExternallyPowered
	TExtEtelRequestStatus getBatteryInfoStatus(mobilePhone, EMobilePhoneGetBatteryInfo);
	CleanupStack::PushL(getBatteryInfoStatus);
	RMobilePhone::TMobilePhoneBatteryInfoV1 info;
	
	mobilePhone.GetBatteryInfo(getBatteryInfoStatus, info);
	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getBatteryInfoStatus, ETimeShort), KErrNone, _L("RMobilePhone::GetBatteryInfo timed-out"));
	CHECK_EQUALS_L(getBatteryInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetBatteryInfo returned with an error"));
	CHECK_EQUALS_L(info.iStatus, RMobilePhone::EBatteryConnectedButExternallyPowered, _L("RMobilePhone::GetBatteryInfo idServiceStatus is not RMobilePhone::EBatteryConnectedButExternallyPowered"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for RMobilePhone::NotifyBatteryInfoChange
	TExtEtelRequestStatus notifyBatteryInfoChangeStatus(mobilePhone, EMobilePhoneNotifyBatteryInfoChange);
	CleanupStack::PushL(notifyBatteryInfoChangeStatus);
	RMobilePhone::TMobilePhoneBatteryInfoV1 notifyInfo;
	mobilePhone.NotifyBatteryInfoChange(notifyBatteryInfoChangeStatus, notifyInfo);
	
	// Unplug the phone charger	
	TBuf<120> unplugPrompt(_L("Ensure charger is unplugged from the phone"));
	DisplayUserInteractionPromptL(unplugPrompt); // 30 seconds

	// Check RMobilePhone::NotifyBatteryInfoChange completes with aInfo.iStatus = EPoweredByBattery
	// Check RMobilePhone::NotifyBatteryInfoChange completes with aInfo.iChargeLevel > 0 but <= 100
	RMobilePhone::TMobilePhoneBatteryInfoV1 expectedInfo = notifyInfo;
	expectedInfo.iStatus = RMobilePhone::EPoweredByBattery;
	
	TCmpBase<RMobilePhone::TMobilePhoneBatteryInfoV1> phoneBatteryInfoComp(notifyInfo, expectedInfo, *this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyBatteryInfoChange(mobilePhone,
											  notifyBatteryInfoChangeStatus,
											  phoneBatteryInfoComp,
											  KErrNone);
	
	ASSERT_TRUE(notifyInfo.iChargeLevel>0 && notifyInfo.iChargeLevel<=100, _L("RMobilePhone::GetBatteryInfo aInfo.iChargeLevel is not > 0 and <= 100"));
	
	// Check RMobilePhone::GetBatteryInfo returns aInfo.iStatus = EPoweredByBattery
	mobilePhone.GetBatteryInfo(getBatteryInfoStatus, info);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBatteryInfoStatus, ETimeShort), KErrNone, _L("RMobilePhone::GetBatteryInfo timed-out"));
	ASSERT_EQUALS(getBatteryInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetBatteryInfo returned with an error"));
	ASSERT_EQUALS(info.iStatus, RMobilePhone::EPoweredByBattery, _L("RMobilePhone::GetBatteryInfo idServiceStatus is not RMobilePhone::EPoweredByBattery"));
	
	// Check RMobilePhone::GetBatteryInfo returns aInfo.iChargeLevel > 0 but <= 100
	ASSERT_TRUE(info.iChargeLevel>0 && info.iChargeLevel<=100, _L("RMobilePhone::GetBatteryInfo aInfo.iChargeLevel is not > 0 and <= 100"));
	
	// Repost notification for RMobilePhone::NotifyBatteryInfoChange
	mobilePhone.NotifyBatteryInfoChange(notifyBatteryInfoChangeStatus, notifyInfo);
	
	// Plug in the phone charger 
	DisplayUserInteractionPromptL(plugPrompt); // 30 seconds

	// Check RMobilePhone::NotifyBatteryInfoChange completes with aInfo.iStatus = EBatteryConnectedButExternallyPowered
	// Check RMobilePhone::NotifyBatteryInfoChange completes with aInfo.iChargeLevel > 0 but <= 100
	expectedInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	
	TCmpBase<RMobilePhone::TMobilePhoneBatteryInfoV1> phoneBatteryInfoComp2(notifyInfo, expectedInfo, *this);
	iPhoneTsyTestHelper.WaitForMobilePhoneNotifyBatteryInfoChange(mobilePhone,
											  notifyBatteryInfoChangeStatus,
											  phoneBatteryInfoComp2,
											  KErrNone);
	
	ASSERT_TRUE(notifyInfo.iChargeLevel>0 && notifyInfo.iChargeLevel<=100, _L("RMobilePhone::GetBatteryInfo aInfo.iChargeLevel is not > 0 and <= 100"));
	
	// Check RMobilePhone::GetBatteryInfo returns aInfo.iStatus = EBatteryConnectedButExternallyPowered
	mobilePhone.GetBatteryInfo(getBatteryInfoStatus, info);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBatteryInfoStatus, ETimeShort), KErrNone, _L("RMobilePhone::GetBatteryInfo timed-out"));
	ASSERT_EQUALS(getBatteryInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetBatteryInfo returned with an error"));
	ASSERT_EQUALS(info.iStatus, RMobilePhone::EBatteryConnectedButExternallyPowered, _L("RMobilePhone::GetBatteryInfo idServiceStatus is not RMobilePhone::EBatteryConnectedButExternallyPowered"));

	// Check RMobilePhone::GetBatteryInfo returns aInfo.iChargeLevel > 0 but <= 100
	ASSERT_TRUE(info.iChargeLevel>0 && info.iChargeLevel<=100, _L("RMobilePhone::GetBatteryInfo aInfo.iChargeLevel is not > 0 and <= 100"));
	
	//
	// TEST END
	//

    StartCleanup();

    CleanupStack::PopAndDestroy(&notifyBatteryInfoChangeStatus);
    CleanupStack::PopAndDestroy(&getBatteryInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhonePower0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhonePower0002");
	}



