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
// The TEFUnit test suite for Indicator in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyindicatorfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyIndicatorFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestGetIndicator0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestGetIndicator0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestGetIndicator0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestGetIndicatorCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestNotifyIndicatorChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestNotifyIndicatorChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIndicatorFU, TestNotifyIndicatorChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-IND-MGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIndicator
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIndicator
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestGetIndicator0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of RMobilePhone::GetIndicator.
 	//-------------------------------------------------------------------------
	TRequestStatus reqStatus;
	TUint32 indicator(0);
	iPhone.GetIndicator(reqStatus, indicator);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	TUint32 expectedIndicator(0);
	ASSERT_EQUALS(expectedIndicator, indicator);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-IND-MGI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetIndicator
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetIndicator
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestGetIndicator0002L()
	{

// This test should test cancellation of GetIndicator
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetIndicator
 	//------------------------------------------------------------------------- 	
	TRequestStatus reqStatus;
	TUint32 indicator(0);
	iPhone.GetIndicator(reqStatus, indicator);

	iPhone.CancelAsyncRequest(EMobilePhoneGetIndicator);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-IND-MGI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetIndicator
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetIndicator
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestGetIndicator0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetIndicator
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TUint32 indicator(0);
	iPhone.GetIndicator(reqStatus, indicator);

	TRequestStatus reqStatus2;
	TUint32 indicator2(0);
	phone2.GetIndicator(reqStatus2, indicator2);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-IND-MGIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIndicatorCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIndicatorCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestGetIndicatorCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RMobilePhone::GetIndicatorCaps.
 	//-------------------------------------------------------------------------
	TUint32 actionCaps;
	TUint32 indCaps;
	iPhone.GetIndicatorCaps(actionCaps, indCaps);
	TUint32 expectedActionCaps = RMobilePhone::KCapsGetIndicator | 
            RMobilePhone::KCapsNotifyIndicatorChange;
	TUint32 expectedIndCaps = RMobilePhone::KIndChargerConnected | 
            RMobilePhone::KIndNetworkAvailable | 
            RMobilePhone::KIndCallInProgress;
	ASSERT_EQUALS(actionCaps, expectedActionCaps);
	ASSERT_EQUALS(indCaps, expectedIndCaps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-IND-MNIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIndicatorChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIndicatorChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestNotifyIndicatorChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RMobilePhone::NotifyIndicatorChange.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TUint32 indicator(0);
	iPhone.NotifyIndicatorChange(reqStatus, indicator);

	// invoke battery info change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	batInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	batInfo.iChargeLevel = 20;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> batInfoData(batInfo);
	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	TUint32 expectedIndicator(RMobilePhone::KIndChargerConnected);
	ASSERT_EQUALS(expectedIndicator, indicator);

 	//-------------------------------------------------------------------------
	// TEST: Increase coverage.
 	//-------------------------------------------------------------------------

	iPhone.NotifyIndicatorChange(reqStatus, indicator);

	// invoke network info change
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	RMobilePhone::TMobilePhoneRegistrationStatus
	                               registrationStatus(RMobilePhone::ERegisteredRoaming);
	RMobilePhone::TMobilePhoneNetworkStatus
	                               networkStatus(RMobilePhone::ENetworkStatusAvailable);
	TMockLtsyData2<RMobilePhone::TMobilePhoneRegistrationStatus,
	               RMobilePhone::TMobilePhoneNetworkStatus>
	                                    netInfoData(registrationStatus, networkStatus);
	data.Close();
	netInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// invoke network info change again
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	registrationStatus = RMobilePhone::ERegisteredOnHomeNetwork;
	data.Close();
	netInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	expectedIndicator = RMobilePhone::KIndChargerConnected|
	                    RMobilePhone::KIndNetworkAvailable;
	ASSERT_EQUALS(expectedIndicator, indicator);
	//----------------------------------------------------------
	

 	//-------------------------------------------------------------------------
	// TEST: Increase coverage.
 	//-------------------------------------------------------------------------
	iPhone.NotifyIndicatorChange(reqStatus, indicator);

	// invoke call info change
	RLine line;
	RCall call;
	CleanupClosePushL(call);

    TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, err);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TInt callId = 1;
	TName incomingCallName;

	err = CreateIncomingCallL(line, callId, KMmTsyVoice1LineName, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);

    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);

 	// close call
 	CloseIncomingCallL(call, callId, mobileService);

	// invoke call info change again
	mobileCallStatus = RMobileCall::EStatusConnected;
	err = CreateIncomingCallL(line, callId, KMmTsyVoice1LineName, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);

    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	expectedIndicator = RMobilePhone::KIndChargerConnected|
	                    RMobilePhone::KIndNetworkAvailable|
	                    RMobilePhone::KIndCallInProgress;
	ASSERT_EQUALS(expectedIndicator, indicator);

 	// close call
 	CloseIncomingCallL(call, callId, mobileService);

	CleanupStack::PopAndDestroy(2); // line, call

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-IND-MNIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyIndicatorChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyIndicatorChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestNotifyIndicatorChange0002L()
	{

// This test should test cancellation of NotifyIndicatorChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyIndicatorChange
 	//-------------------------------------------------------------------------
	TRequestStatus reqStatus;
	TUint32 indicator(0);
	iPhone.NotifyIndicatorChange(reqStatus, indicator);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyIndicatorChange);

	// invoke battery info change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	batInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	batInfo.iChargeLevel = 20;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> batInfoData(batInfo);
	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-IND-MNIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyIndicatorChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyIndicatorChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIndicatorFU::TestNotifyIndicatorChange0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyIndicatorChange
 	//-------------------------------------------------------------------------
	TRequestStatus reqStatus;
	TUint32 indicator(0);
	iPhone.NotifyIndicatorChange(reqStatus, indicator);

	TRequestStatus reqStatus2;
	TUint32 indicator2(0);
	phone2.NotifyIndicatorChange(reqStatus2, indicator2);

	// invoke battery info change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	batInfo.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
	batInfo.iChargeLevel = 20;
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> batInfoData(batInfo);
	batInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyBatteryInfoChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	TUint32 expectedIndicator(RMobilePhone::KIndChargerConnected);
	ASSERT_EQUALS(expectedIndicator, indicator);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(expectedIndicator, indicator2);

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
