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
// The TEFUnit test suite for PhonePower in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyphonepowerfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>

#include <rmockpower.h>
#include <rmockpowerstate.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <hwrm/hwrmpowercommands.h>
const TInt KTwoMins = 120000000;


CTestSuite* CCTsyPhonePowerFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestGetBatteryCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestGetBatteryInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestGetBatteryInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestNotifyBatteryInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestNotifyBatteryInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestNotifyBatteryInfoChange0004L);

	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestHWRMGetBatteryInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestHWRMGetBatteryInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestHWRMNotifyBatteryInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestHWRMNotifyBatteryInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonePowerFU, TestHWRMNotifyBatteryInfoChange0004L);

	END_SUITE;
	}

//
// Actual test cases
// HWRM is not supported
//


/**
@SYMTestCaseID BA-CTSY-PP-MGBC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetBatteryCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetBatteryCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestGetBatteryCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetBatteryCaps when result is not cached.
 	//-------------------------------------------------------------------------
	TUint32 caps;
	TInt res = iPhone.GetBatteryCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(KBatteryCaps, caps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PP-MGBI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetBatteryInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetBatteryInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestGetBatteryInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetBatteryInfo when result is not cached.
 	//-------------------------------------------------------------------------
 	TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
 	
 	iPhone.GetBatteryInfo(requestStatus, batteryInfo);
 	
 	User::WaitForRequest(requestStatus);
 	
 	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	ASSERT_EQUALS((TUint)0x00, batteryInfo.iChargeLevel)
 	ASSERT_EQUALS(RMobilePhone::EPowerStatusUnknown, batteryInfo.iStatus)
 	 	
 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetBatteryInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
 	// change phone info first
	TRequestStatus status;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo2;
	const TUint twentyCharge = 20;

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	batInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	batInfo.iChargeLevel = twentyCharge;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> batInfoData(batInfo);
	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);

	// get cached battery info now
 	iPhone.GetBatteryInfo(status, batInfo2);
 	
	User::WaitForRequest(status); 	
 	
	ASSERT_EQUALS(KErrNone, status.Int());
	ASSERT_EQUALS(RMobilePhone::EBatteryConnectedButExternallyPowered, batInfo2.iStatus);
	ASSERT_EQUALS(twentyCharge, batInfo2.iChargeLevel);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PP-MGBI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetBatteryInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetBatteryInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestGetBatteryInfo0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetBatteryInfo
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.
	
 	TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
 	TRequestStatus requestStatus2;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo2;
 	
 	iPhone.GetBatteryInfo(requestStatus, batteryInfo);
 	iPhone.GetBatteryInfo(requestStatus2, batteryInfo2);
 	
 	User::WaitForRequest(requestStatus);
 	User::WaitForRequest(requestStatus2);
 	
 	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	ASSERT_EQUALS((TUint)0, batteryInfo.iChargeLevel)
 	ASSERT_EQUALS(RMobilePhone::EPowerStatusUnknown, batteryInfo.iStatus)
 	
 	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
 	ASSERT_EQUALS((TUint)0, batteryInfo2.iChargeLevel)
 	ASSERT_EQUALS(RMobilePhone::EPowerStatusUnknown, batteryInfo2.iStatus)
 	
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PP-MNBIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyBatteryInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestNotifyBatteryInfoChange0001L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// TEST C: LTSY successfully completing request of
	// RMobilePhone::NotifyBatteryInfoChange
 	//-------------------------------------------------------------------------

	TRequestStatus status;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	const TUint twentyCharge = 20;

	// Check basic expected operation
	iPhone.NotifyBatteryInfoChange(status, batInfo);

	batInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	batInfo.iChargeLevel = twentyCharge;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> batInfoData(batInfo);
	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data);

	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	ASSERT_EQUALS(RMobilePhone::EBatteryConnectedButExternallyPowered, batInfo.iStatus);
	ASSERT_EQUALS(twentyCharge, batInfo.iChargeLevel);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of notification from LTSY->CTSY
 	//-------------------------------------------------------------------------

	const TUint fiftyCharge = 50;
	batInfo.iStatus = RMobilePhone::ENoBatteryConnected;
	batInfo.iChargeLevel = fiftyCharge;
	data.Close();
	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrGeneral, data);

	iPhone.NotifyBatteryInfoChange(status, batInfo);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrGeneral, status.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); //data, this
	}


/**
@SYMTestCaseID BA-CTSY-PP-MNBIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyBatteryInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestNotifyBatteryInfoChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test A: Test cancelling of RMobilePhone::NotifyBatteryInfoChange
 	//-------------------------------------------------------------------------

	// EMobilePhoneNotifyBatteryInfoChange does not get passed down
	// to LTSY so don't need to tell mock LTSY to expect it.
	RMobilePhone::TMobilePhoneBatteryInfoV1 retBatInfo;
	retBatInfo.iStatus = RMobilePhone::ENoBatteryConnected;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> retBatInfoData(retBatInfo);
	retBatInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data, 2);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	iPhone.NotifyBatteryInfoChange(reqStatus, batInfo);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange);

	// Check status is KErrCancel
	ASSERT_EQUALS(KErrCancel,reqStatus.Int());

	// Wait for completion of iMockLTSY.CompleteL
	User::WaitForRequest(mockLtsyStatus); // Uncomment in actual test!
	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PP-MNBIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyBatteryInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestNotifyBatteryInfoChange0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus status, status2;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo2;


	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyBatteryInfoChange
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.

	// Set up Data for Client 1.
	const TUint ZeroCharge = 0;
	batInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	batInfo.iChargeLevel = ZeroCharge;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> batInfoData(batInfo);

	const TUint TenCharge = 0;
	batInfo2.iStatus = RMobilePhone::ENoBatteryConnected;
	batInfo.iChargeLevel = TenCharge;

	// send call from client 1
	iPhone.NotifyBatteryInfoChange(status, batInfo);

	// send call from client 2
	phone2.NotifyBatteryInfoChange(status2, batInfo2);

	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data,0);

	// Wait for the complete on the first notification
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int())
	ASSERT_EQUALS(RMobilePhone::EBatteryConnectedButExternallyPowered, batInfo.iStatus);
	ASSERT_EQUALS(ZeroCharge, batInfo.iChargeLevel);

	// Wait for the complete on the first notification
	User::WaitForRequest(status2);
	ASSERT_EQUALS(KErrNone, status2.Int())
	ASSERT_EQUALS(RMobilePhone::EBatteryConnectedButExternallyPowered, batInfo2.iStatus);
	ASSERT_EQUALS(ZeroCharge, batInfo2.iChargeLevel);

	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PPHWRM-MGBI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetBatteryInfo using HWRM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetBatteryInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestHWRMGetBatteryInfo0001L()
	{
	_LIT( KTSYHWRMTestStart, "TSY test: start testing tsy with HWRM Battery API.");
	Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, KTSYHWRMTestStart);
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetBatteryInfo. 
 	//-------------------------------------------------------------------------

    TInt delay(1);
    RMockPowerState mockPowerState ;
	User::LeaveIfError(mockPowerState.Connect());
	CleanupClosePushL(mockPowerState);
	mockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusNotConnected,delay);
    
    RMockPower mockPower;
	User::LeaveIfError(mockPower.Connect());
	CleanupClosePushL(mockPower);
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryInfoCmdId, KNullDesC8);
	
	HWRMPowerCommand::TBatteryConsumptionResponseData batteryConsumptionReport;
	batteryConsumptionReport.iErrorCode = KErrNone;
	batteryConsumptionReport.iNominalCapacity = 0;
	batteryConsumptionReport.iRemainingCapacity = 0;
	batteryConsumptionReport.iRemainingPercentageCapacity = 10;
	HWRMPowerCommand::TBatteryConsumptionResponsePackage batteryConsumptionReportPackg(batteryConsumptionReport);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryInfoCmdId, delay, batteryConsumptionReportPackg);
	
	// CTSY BatteryInfoObserver is polling batteryinfo changes at 2 minutes intervals.
    User::After(KTwoMins);

 	TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
 	iPhone.GetBatteryInfo(requestStatus, batteryInfo);
 	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	ASSERT_EQUALS((TUint)batteryConsumptionReport.iRemainingPercentageCapacity, batteryInfo.iChargeLevel);
 	ASSERT_EQUALS(RMobilePhone::EPoweredByBattery, batteryInfo.iStatus);
 	
 	CleanupStack::PopAndDestroy(&mockPower);
 	CleanupStack::PopAndDestroy(&mockPowerState);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSY-PPHWRM-MGBI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetBatteryInfo with HWRM
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetBatteryInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestHWRMGetBatteryInfo0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion multiple client requests of
	// RMobilePhone::GetBatteryInfo. 
 	//-------------------------------------------------------------------------

    RMockPower mockPower;
	User::LeaveIfError(mockPower.Connect());
	CleanupClosePushL(mockPower);
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryInfoCmdId, KNullDesC8);

	HWRMPowerCommand::TBatteryConsumptionResponseData batteryConsumptionReport;
	batteryConsumptionReport.iErrorCode = KErrNone;
	batteryConsumptionReport.iNominalCapacity = 0;
	batteryConsumptionReport.iRemainingCapacity = 0;
	batteryConsumptionReport.iRemainingPercentageCapacity = 20;
    TInt delay(1);
	HWRMPowerCommand::TBatteryConsumptionResponsePackage batteryConsumptionReportPackg(batteryConsumptionReport);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryInfoCmdId, delay, batteryConsumptionReportPackg);

    RMockPowerState mockPowerState ;
	User::LeaveIfError(mockPowerState.Connect());
	CleanupClosePushL(mockPowerState);
	mockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusCharging, delay);

    // CTSY BatteryInfoObserver is polling batteryinfo changes at 2 minutes intervals.
    User::After(KTwoMins);

 	TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
 	iPhone.GetBatteryInfo(requestStatus, batteryInfo);
 	User::WaitForRequest(requestStatus);
 	
 	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	ASSERT_EQUALS((TUint)batteryConsumptionReport.iRemainingPercentageCapacity, batteryInfo.iChargeLevel);
 	ASSERT_EQUALS(RMobilePhone::EBatteryConnectedButExternallyPowered, batteryInfo.iStatus);

 	TRequestStatus requestStatus2;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo2;
    phone2.GetBatteryInfo(requestStatus2, batteryInfo2);
 	User::WaitForRequest(requestStatus2);

 	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
 	ASSERT_EQUALS((TUint)batteryConsumptionReport.iRemainingPercentageCapacity, batteryInfo2.iChargeLevel);
 	ASSERT_EQUALS(RMobilePhone::EBatteryConnectedButExternallyPowered, batteryInfo2.iStatus);

 	CleanupStack::PopAndDestroy(&mockPowerState);
 	CleanupStack::PopAndDestroy(&mockPower);
 	CleanupStack::PopAndDestroy(&phone2);
 	CleanupStack::PopAndDestroy(&telServer2);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSY-PPHWRM-MNBIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyBatteryInfoChange with HWRM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestHWRMNotifyBatteryInfoChange0001L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// TEST C1: LTSY successfully completing request of
	// RMobilePhone::NotifyBatteryInfoChange.
	// iChargeLevel is changed.
 	//-------------------------------------------------------------------------

 	TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
    iPhone.NotifyBatteryInfoChange(requestStatus, batteryInfo);

    RMockPower mockPower;
    User::LeaveIfError(mockPower.Connect());
    CleanupClosePushL(mockPower);
    mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryInfoCmdId, KNullDesC8);
    HWRMPowerCommand::TBatteryConsumptionResponseData batteryConsumptionReport;
	batteryConsumptionReport.iErrorCode = KErrNone;
	batteryConsumptionReport.iNominalCapacity = 0;
	batteryConsumptionReport.iRemainingCapacity = 0;
	batteryConsumptionReport.iRemainingPercentageCapacity = 30;
    TInt delay(1);
   	HWRMPowerCommand::TBatteryConsumptionResponsePackage batteryConsumptionReportPackg(batteryConsumptionReport);
    mockPower.CompleteL(HWRMPowerCommand::EGetBatteryInfoCmdId, delay, batteryConsumptionReportPackg);
    
    User::WaitForRequest(requestStatus);
    
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	ASSERT_EQUALS((TUint)batteryConsumptionReport.iRemainingPercentageCapacity, batteryInfo.iChargeLevel);

    CleanupStack::PopAndDestroy(&mockPower);

	//-------------------------------------------------------------------------
	// TEST C2: LTSY successfully completing request of
	// RMobilePhone::NotifyBatteryInfoChange
	// when charging status is changed
 	//-------------------------------------------------------------------------

    batteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
    batteryInfo.iChargeLevel = 0;
    iPhone.NotifyBatteryInfoChange(requestStatus, batteryInfo);

    RMockPowerState mockPowerState;
	User::LeaveIfError(mockPowerState.Connect());
    CleanupClosePushL(mockPowerState);
    mockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusNotConnected, delay);

    User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobilePhone::EPoweredByBattery, batteryInfo.iStatus);
	
    CleanupStack::PopAndDestroy(&mockPowerState);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSY-PPHWRM-MNBIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyBatteryInfoChange with HWRM
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestHWRMNotifyBatteryInfoChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test A: Test cancelling of RMobilePhone::NotifyBatteryInfoChange
 	//-------------------------------------------------------------------------

    RMockPowerState mockPowerState;
	User::LeaveIfError(mockPowerState.Connect());
    CleanupClosePushL(mockPowerState);
    TInt delay(1);
	mockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusChargingComplete,delay);

 	TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;

	iPhone.NotifyBatteryInfoChange(requestStatus, batteryInfo);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange);

    User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    CleanupStack::PopAndDestroy(&mockPowerState);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSY-PPHWRM-MNBIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyBatteryInfoChange with HWRM
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhonePowerFU::TestHWRMNotifyBatteryInfoChange0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A1: Test multiple clients requesting
	// RMobilePhone::NotifyBatteryInfoChange
	// when iChargeLevel is changed.
 	//-------------------------------------------------------------------------
 	
    TRequestStatus requestStatus;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
 	TRequestStatus requestStatus2;
 	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo2;
	iPhone.NotifyBatteryInfoChange(requestStatus, batteryInfo);
    phone2.NotifyBatteryInfoChange(requestStatus2, batteryInfo2);

    RMockPower mockPower;
    User::LeaveIfError(mockPower.Connect());
    CleanupClosePushL(mockPower);
    mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryInfoCmdId, KNullDesC8);
    HWRMPowerCommand::TBatteryConsumptionResponseData batteryConsumptionReport;
	batteryConsumptionReport.iErrorCode = KErrNone;
	batteryConsumptionReport.iNominalCapacity = 0;
	batteryConsumptionReport.iRemainingCapacity = 0;
	batteryConsumptionReport.iRemainingPercentageCapacity = 20;
    TInt delay(1);
   	HWRMPowerCommand::TBatteryConsumptionResponsePackage batteryConsumptionReportPackg(batteryConsumptionReport);
    mockPower.CompleteL(HWRMPowerCommand::EGetBatteryInfoCmdId, delay, batteryConsumptionReportPackg);
    
    User::WaitForRequest(requestStatus);
    User::WaitForRequest(requestStatus2);
   
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	ASSERT_EQUALS((TUint)batteryConsumptionReport.iRemainingPercentageCapacity, batteryInfo.iChargeLevel)

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
 	ASSERT_EQUALS((TUint)batteryConsumptionReport.iRemainingPercentageCapacity, batteryInfo2.iChargeLevel)

    CleanupStack::PopAndDestroy(&mockPower);
    
    //-------------------------------------------------------------------------
	// Test A2: Test multiple clients requesting
	// RMobilePhone::NotifyBatteryInfoChange 
	// when charging status is changed
 	//-------------------------------------------------------------------------
    
    batteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
    batteryInfo.iChargeLevel = 0;
    batteryInfo2.iStatus = RMobilePhone::EPowerStatusUnknown;
    batteryInfo2.iChargeLevel = 0;
    iPhone.NotifyBatteryInfoChange(requestStatus, batteryInfo);
    phone2.NotifyBatteryInfoChange(requestStatus2, batteryInfo2);
    
    RMockPowerState mockPowerState;
	User::LeaveIfError(mockPowerState.Connect());
    CleanupClosePushL(mockPowerState);
	mockPowerState.CompleteL(KHWRMChargingStatus, EChargingStatusNotConnected,delay);
    
    User::WaitForRequest(requestStatus);
    User::WaitForRequest(requestStatus2);
    
    ASSERT_EQUALS(RMobilePhone::EPoweredByBattery, batteryInfo.iStatus);
    ASSERT_EQUALS(RMobilePhone::EPoweredByBattery, batteryInfo2.iStatus);
    
    CleanupStack::PopAndDestroy(&mockPowerState);
	CleanupStack::PopAndDestroy(&phone2);
	CleanupStack::PopAndDestroy(&telServer2);
	CleanupStack::PopAndDestroy(this);
	}
	
