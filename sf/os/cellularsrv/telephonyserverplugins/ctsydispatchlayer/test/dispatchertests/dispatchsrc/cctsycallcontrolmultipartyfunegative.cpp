// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// when support for the various APIs are disabled in the dispatch layer.
//



/**
 @file	The TEFUnit test suite for SmsControl in the Common TSY
*/

#include "cctsycallcontrolmultipartyfunegative.h"

#include <etelmm.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolmultipartyinterface.h>

#include <etel.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>

#include <test/tmockltsydata.h>
#include "mockltsyindicatorids.h"

#include "config.h"


CTestSuite* CCTsyCallControlMultipartyFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFUNegative, TestHangUpL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFUNegative, TestAddCallL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFUNegative, TestCreateConferenceL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFUNegative, TestSwapL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFUNegative, TestGoOneToOneL);
	END_SUITE;
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-NEGATIVE-UN0001
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the Hangup API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCallControlMultipartyFUNegative::TestHangUpL()
	{
	// Note: based on original non-negative test void CCTsyCallControlMultipartyFU::TestUseCase0003L()
	
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallL(mobileService, callId1, callId2);

	DriverHangUpCallL(callId1, KErrGsmCCNormalCallClearing, EFalse);
	DriverHangUpCallL(callId2, KErrGsmCCNormalCallClearing);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	RMobileLine mobileLine2;
	CleanupClosePushL(mobileLine2);
	RMobileCall mobileCall2;
	CleanupClosePushL(mobileCall2);

	ClientDialAndHoldFirstCallDialSecondCallL(mobileService, mobileCall, mobileLine,
			mobileCall2, mobileLine2);

	RMobileConferenceCall confCall;
	CleanupClosePushL(confCall);

	TRequestStatus notifyCall1Status;
	RMobileCall::TMobileCallStatus call1Status;
	mobileCall.NotifyMobileCallStatusChange(notifyCall1Status, call1Status);
	
	ClientCreateConferenceCallL(confCall);

	User::WaitForRequest(notifyCall1Status);
	ASSERT_EQUALS(KErrNone, notifyCall1Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, call1Status);
	
	TRequestStatus hangupStatus;
	confCall.HangUp(hangupStatus);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNotSupported, hangupStatus.Int());

	// Boths calls should still be connected
	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);
	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	// Hang up the calls
	ClientHangUpCallL(mobileCall);
	ClientHangUpCallL(mobileCall2);
	
	User::After(100000); // Allow the lifetimer to be updated before ending the test
	
	mobileCall.Close();
	mobileCall2.Close();

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(8, &config);	//  confCall, this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-NEGATIVE-UN0002
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the AddCall API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCallControlMultipartyFUNegative::TestAddCallL()
	{
	// Note: based on non-negative test CCTsyCallControlMultipartyFU::TestUseCase0007L()  
	
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallAndHoldItL(mobileService, callId1, callId2);

	TInt callId3 = 3;
	DriverDialCallL(callId3, mobileService);

	DriverHangUpCallL(callId1, KErrGsmCCNormalCallClearing, EFalse);
	DriverHangUpCallL(callId2, KErrGsmCCNormalCallClearing, EFalse);
	DriverHangUpCallL(callId3, KErrGsmCCNormalCallClearing);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	RMobileLine mobileLine2;
	CleanupClosePushL(mobileLine2);
	RMobileCall mobileCall2;
	CleanupClosePushL(mobileCall2);

	ClientDialAndHoldFirstCallDialSecondCallL(mobileService, mobileCall, mobileLine,
			mobileCall2, mobileLine2);

	RMobileConferenceCall confCall;
	CleanupClosePushL(confCall);

	TRequestStatus notifyCall1Status;
	RMobileCall::TMobileCallStatus call1Status;
	mobileCall.NotifyMobileCallStatusChange(notifyCall1Status, call1Status);
	
	ClientCreateConferenceCallL(confCall);

	User::WaitForRequest(notifyCall1Status);
	ASSERT_EQUALS(KErrNone, notifyCall1Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, call1Status);
	
	mobileCall.NotifyMobileCallStatusChange(notifyCall1Status, call1Status);
	
	TRequestStatus notifyCall2Status;
	RMobileCall::TMobileCallStatus call2Status;
	mobileCall2.NotifyMobileCallStatusChange(notifyCall2Status, call2Status);
	
	ClientSwapConferenceCallL(confCall, RMobileConferenceCall::EConferenceSwapped,
			RMobileConferenceCall::EConferenceHold);

	User::WaitForRequest(notifyCall1Status);
	ASSERT_EQUALS(KErrNone, notifyCall1Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, call1Status);
	
	User::WaitForRequest(notifyCall2Status);
	ASSERT_EQUALS(KErrNone, notifyCall2Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, call2Status);
	
	// Dial a new call and attempt to add it to the conference
	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientDialCallL(mobileLine3, mobileCall3, mobileService);

	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	ASSERT_EQUALS(KErrNone, mobileCall3.GetMobileCallInfo(callInfoPckg));

	TRequestStatus status;
	confCall.AddCall(status, callInfo.iCallName);
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNotSupported);

	// Hang up the calls
	ClientHangUpCallL(mobileCall);
	ClientHangUpCallL(mobileCall2);
	ClientHangUpCallL(mobileCall3);

    User::After(100000); // Allow the lifetimer to be updated before ending the test	
	
	mobileCall.Close();
	mobileCall2.Close();
	mobileCall3.Close();

	AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(9, &config);
	}



/**
 * @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-NEGATIVE-UN0003
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the CreateConference API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCallControlMultipartyFUNegative::TestCreateConferenceL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Set up correct conditions to be able to create a conference
	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialAndHoldFirstCallDialSecondCallL(mobileService, callId1, callId2);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	RMobileLine mobileLine2;
	CleanupClosePushL(mobileLine2);
	RMobileCall mobileCall2;
	CleanupClosePushL(mobileCall2);

	ClientDialAndHoldFirstCallDialSecondCallL(mobileService, mobileCall, mobileLine,
			mobileCall2, mobileLine2);

	RMobileConferenceCall confCall;
	CleanupClosePushL(confCall);
	ASSERT_EQUALS(KErrNone, confCall.Open(iPhone));

	// Hang up cause for normal hang up
	DriverHangUpCallL(callId1, KErrGsmCCNormalCallClearing, EFalse);
	DriverHangUpCallL(callId2, KErrGsmCCNormalCallClearing);
	
	TRequestStatus status;
	confCall.CreateConference(status);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	// Hang up the calls
	ClientHangUpCallL(mobileCall);
	ClientHangUpCallL(mobileCall2);
	
	User::After(100000); // Allow the lifetimer to be updated before ending the test
	
	mobileCall.Close();
	mobileCall2.Close();

	AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(8, &config);
	}



/**
 * @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-NEGATIVE-UN0004
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the Swap API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCallControlMultipartyFUNegative::TestSwapL()
	{
	// Note: Equiv non-negative test is CCTsyCallControlMultipartyFU::TestUseCase0006L
	
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallAndHoldItL(mobileService, callId1, callId2);

	TInt callId3 = 3;
	DriverDialCallL(callId3, mobileService);
	
	DriverHangUpCallL(callId1, KErrGsmCCNormalCallClearing, EFalse);
	DriverHangUpCallL(callId2, KErrGsmCCNormalCallClearing, EFalse);
	DriverHangUpCallL(callId3, KErrGsmCCNormalCallClearing);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	RMobileLine mobileLine2;
	CleanupClosePushL(mobileLine2);
	RMobileCall mobileCall2;
	CleanupClosePushL(mobileCall2);

	RMobileConferenceCall confCall;
	CleanupClosePushL(confCall);

	ClientCreateConferenceCallAndHoldItL(mobileService, mobileCall,
			mobileLine, mobileCall2, mobileLine2, confCall);

	TRequestStatus notifyCall1Status;
	RMobileCall::TMobileCallStatus call1Status;
	mobileCall.NotifyMobileCallStatusChange(notifyCall1Status, call1Status);
	
	TRequestStatus notifyCall2Status;
	RMobileCall::TMobileCallStatus call2Status;
	mobileCall2.NotifyMobileCallStatusChange(notifyCall2Status, call2Status);
	
	// Dial a new call and attempt to swap the conference with it
	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientDialCallL(mobileLine3, mobileCall3, mobileService);

	User::WaitForRequest(notifyCall1Status);
	ASSERT_EQUALS(KErrNone, notifyCall1Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, call1Status);
	
	mobileCall.NotifyMobileCallStatusChange(notifyCall1Status, call1Status);
	
	TRequestStatus swapStatus;
	confCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrNotSupported, swapStatus.Int());

	User::WaitForRequest(notifyCall1Status);
	ASSERT_EQUALS(KErrNone, notifyCall1Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, call1Status);
	
	User::WaitForRequest(notifyCall2Status);
	ASSERT_EQUALS(KErrNone, notifyCall2Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, call2Status);
	
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	ASSERT_EQUALS(KErrNone, confCall.GetConferenceStatus(confStatus));
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceHold, confStatus);

	// Hang up the calls
	ClientHangUpCallL(mobileCall);
	ClientHangUpCallL(mobileCall2);
	ClientHangUpCallL(mobileCall3);
	
	User::After(100000); // Allow the lifetimer to be updated before ending the test
    
	mobileCall.Close();
	mobileCall2.Close();
	mobileCall3.Close();

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(10, &config);
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-NEGATIVE-UN0005
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GoOneToOne API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCallControlMultipartyFUNegative::TestGoOneToOneL()
	{
	// Note: Equiv non-negative test CCTsyCallControlMultipartyFU::TestUseCase0008L()
	
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallL(mobileService, callId1, callId2);

    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
    
	TMockLtsyData1<RArray<TInt> > ltsyData(callIds);
	data.Close();
	ltsyData.SerialiseL(data);			
	
    iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, KErrNone);

    DriverCompleteSuccessfulHangUpNotificationsL(callId1, EFalse, KErrGsmCCNormalCallClearing, EFalse);
    DriverCompleteSuccessfulHangUpNotificationsL(callId2, EFalse, KErrGsmCCNormalCallClearing, ETrue);

	// Client Side Test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	RMobileLine mobileLine2;
	CleanupClosePushL(mobileLine2);
	RMobileCall mobileCall2;
	CleanupClosePushL(mobileCall2);

	ClientDialAndHoldFirstCallDialSecondCallL(mobileService, mobileCall, mobileLine,
			mobileCall2, mobileLine2);

	RMobileConferenceCall confCall;
	CleanupClosePushL(confCall);

	TRequestStatus notifyCall1Status;
	RMobileCall::TMobileCallStatus call1Status;
	mobileCall.NotifyMobileCallStatusChange(notifyCall1Status, call1Status);
	
	ClientCreateConferenceCallL(confCall);

	User::WaitForRequest(notifyCall1Status);
	ASSERT_EQUALS(KErrNone, notifyCall1Status.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, call1Status);
	
	TRequestStatus oneToOneStatus;
	mobileCall2.GoOneToOne(oneToOneStatus);
	User::WaitForRequest(oneToOneStatus);
	ASSERT_EQUALS(KErrNotSupported, oneToOneStatus.Int());

	TInt numCalls;
	ASSERT_EQUALS(KErrNone, confCall.EnumerateCalls(numCalls));
	ASSERT_EQUALS(2, numCalls);

	TRequestStatus hangupStatus;
	confCall.HangUp(hangupStatus);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int());
	
	mobileCall.Close();
	mobileCall2.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(9, &config);
	}


//
// Helper functions
//

void CCTsyCallControlMultipartyFUNegative::DriverCreateConferenceCallL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	DriverDialAndHoldFirstCallDialSecondCallL(aMobileService, aCallId1, aCallId2);

	TMockLtsyData2<TInt, TInt> createConfData(aCallId1, aCallId2);
	createConfData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, data);

    RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
    		aCallId1, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
    		aCallId2, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData2.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

    CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlMultipartyFUNegative::DriverCreateConferenceCallL

void CCTsyCallControlMultipartyFUNegative::DriverCreateConferenceCallAndHoldItL(
	RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	DriverCreateConferenceCallL(aMobileService, aCallId1, aCallId2);

	// Swapping a conference call when there isn't another call causes hold
	// IPC to go to the dispatcher.
	DriverHoldCallL(aCallId1, KErrNone);

	// Call 2 which is part of the conference will go on hold
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusHold;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
		aCallId2, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlMultipartyFUNegative::DriverCreateConferenceCallAndHoldItL

void CCTsyCallControlMultipartyFUNegative::ClientCreateConferenceCallL(RMobileConferenceCall& aConfCall)
	{
	ASSERT_EQUALS(KErrNone, aConfCall.Open(iPhone));

	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	TRequestStatus notifyConfStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	aConfCall.NotifyConferenceStatusChange(notifyConfStatus, confStatus);

	TRequestStatus createConfStatus;
	aConfCall.CreateConference(createConfStatus);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, confEvent); // Call 1 when call status change notification completes
	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int()); // Call 2 added when call status change notification completes
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, confEvent);
	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceBuilt, confEvent);

	User::WaitForRequest(notifyConfStatus);
	ASSERT_EQUALS(KErrNone, notifyConfStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, confStatus);

	User::WaitForRequest(createConfStatus);
	ASSERT_EQUALS(KErrNone, createConfStatus.Int());
	} // CCTsyCallControlMultipartyFUNegative::ClientCreateConferenceCallL

void CCTsyCallControlMultipartyFUNegative::ClientSwapConferenceCallL(RMobileConferenceCall& aConfCall,
	RMobileConferenceCall::TMobileConferenceEvent aExpectedConfEvent, RMobileConferenceCall::TMobileConferenceStatus aExpectedConfStatus)
	{
	TRequestStatus notifyConfStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	aConfCall.NotifyConferenceStatusChange(notifyConfStatus, confStatus);

	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	TRequestStatus swapStatus;
	aConfCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrNone, swapStatus.Int());

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(aExpectedConfEvent, confEvent);

	User::WaitForRequest(notifyConfStatus);
	ASSERT_EQUALS(KErrNone, notifyConfStatus.Int());
	ASSERT_EQUALS(aExpectedConfStatus, confStatus);
	} // CCTsyCallControlMultipartyFUNegative::ClientSwapConferenceCallL

void CCTsyCallControlMultipartyFUNegative::ClientCreateConferenceCallAndHoldItL(
	RMobilePhone::TMobileService aMobileService, RMobileCall& aMobileCall,
	RMobileLine& aMobileLine, RMobileCall& aMobileCall2, RMobileLine& aMobileLine2,
	RMobileConferenceCall& aConfCall)
	{
    ClientDialAndHoldFirstCallDialSecondCallL(aMobileService, aMobileCall, aMobileLine, aMobileCall2, aMobileLine2);
    ClientCreateConferenceCallL(aConfCall);
    ClientSwapConferenceCallL(aConfCall, RMobileConferenceCall::EConferenceSwapped, RMobileConferenceCall::EConferenceHold);
	} // CCTsyCallControlMultipartyFUNegative::ClientCreateConferenceCallAndHoldItL

