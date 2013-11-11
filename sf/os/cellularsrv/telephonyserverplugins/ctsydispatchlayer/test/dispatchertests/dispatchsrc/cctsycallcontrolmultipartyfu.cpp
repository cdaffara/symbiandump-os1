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
// The TEFUnit test suite for CallControlMultipartyControl in the Common TSY.
//



/**
 @file
*/

#include "cctsycallcontrolmultipartyfu.h"
#include "mockltsyindicatorids.h"

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolmultipartyinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>

CTestSuite* CCTsyCallControlMultipartyFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUseCase0011L);

	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlMultipartyFU, TestUnit0001L);

	END_SUITE;
	}


//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for creating a multiparty conference call
@SYMTestPriority High
@SYMTestActions Invokes dialling two calls and creating a conference call
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallL(mobileService, callId1, callId2);

	RBuf8 data;
	CleanupClosePushL(data);
	
    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
    
	TMockLtsyData1<RArray<TInt> > ltsyData(callIds);
	ltsyData.SerialiseL(data);
    			
    iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, KErrNone);

    DriverCompleteSuccessfulHangUpNotificationsL(callId1, EFalse, KErrGsmCCNormalCallClearing, EFalse);
    DriverCompleteSuccessfulHangUpNotificationsL(callId2, EFalse, KErrGsmCCNormalCallClearing);

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

	ClientCreateConferenceCallL(confCall);

	TRequestStatus notifyConfStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatus, confStatus);

	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	TRequestStatus hangupStatus;
	confCall.HangUp(hangupStatus);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);

	User::WaitForRequest(notifyConfStatus);
	ASSERT_EQUALS(KErrNone, notifyConfStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, confStatus);

	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int());

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0001L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0002
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to create a conference call
 @SYMTestPriority High
 @SYMTestActions Invokes dialling two calls and attempting to create a conference call
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialAndHoldFirstCallDialSecondCallL(mobileService, callId1, callId2);
	TMockLtsyData2<TInt, TInt> createConfData(callId1, callId2);
	createConfData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, KErrGeneral);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	TMockLtsyData2<TInt, TInt> mockData1(callId1, hangUpCause);
	data.Close();
	mockData1.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	TMockLtsyData2<TInt, TInt> mockData2(callId2, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

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
	ASSERT_EQUALS(KErrNone, confCall.Open(iPhone));

	TRequestStatus createConfStatus;
	confCall.CreateConference(createConfStatus);
	User::WaitForRequest(createConfStatus);
	ASSERT_EQUALS(KErrGeneral, createConfStatus.Int());

	// Both calls should still be in their original state (call 1 - held, call 2 - connected)
	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);
	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0002L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0003
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to hang up a conference call
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileConferenceCall::HangUp
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0003L()
	{
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
	ltsyData.SerialiseL(data);
					
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, KErrGeneral);

	// Clean up when subsessions are closed
	DriverCloseTwoRemotePartyConferenceAndCallSubsessionsL(callId1, callId2, callIds);

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

	ClientCreateConferenceCallL(confCall);

	TRequestStatus hangupStatus;
	confCall.HangUp(hangupStatus);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrGeneral, hangupStatus.Int());

	// Boths calls should still be connected
	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);
	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0003L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0004
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for hang up of remote party in conference call
 @SYMTestPriority High
 @SYMTestActions Invokes creation of a 2 remote party conference call with one party hanging up therefore terminating the conference.
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallL(mobileService, callId1, callId2);

	TInt delayBeforeRemotePartyHangsUp = 5;
	DriverCompleteSuccessfulHangUpNotificationsL(callId1, EFalse, KErrGsmCCNormalCallClearing, EFalse, delayBeforeRemotePartyHangsUp);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	TMockLtsyData2<TInt, TInt> mockData2(callId2, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

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

	ClientCreateConferenceCallL(confCall);

	TRequestStatus notifyConfStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatus, confStatus);

	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	// MockSY should complete queued the hang up call status change notifications now
	// If it doesn't, delay to simulate this above needs to be increased.

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);

	User::WaitForRequest(notifyConfStatus);
	ASSERT_EQUALS(KErrNone, notifyConfStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, confStatus);

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0004L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0005
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for swapping a conference call
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileConferenceCall::Swap and RMobileConferenceCall::AddCall
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// This use case demonstrates swapping of conference calls.
	// It does the following:
	//  - Set up a conference call with two remote parties
	//  - Swap the conference call (when it is the only active call)
	//  - The conference call goes on hold
	//  - Dial a third call and add it to the conference
	//  - The conference gets connected again with all three calls being a part of it
	//  - Swap the conference call to put it back on hold
	//  - Dial a fourth call
	//  - Swap the conference call making it active and the fourth call on hold
	//  - Close down

	TInt callId1 = 1;
	TInt callId2 = 2;
	TInt callId3 = 3;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceWithThreeRemotePartiesL(mobileService, callId1, callId2, callId3);

	// Swapping conference call when there is no held call is the same as
	// holding the conference call
	DriverHoldCallL(callId1, KErrNone);

	// Other calls in the conference all go on hold too
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusHold;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData6(
		callId2, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData6.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData7(
		callId3, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData7.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TInt callId4 = 4;
	DriverDialCallL(callId4, mobileService);

	TMockLtsyData2<TInt, TInt> mockCallData12(callId1, callId4);
	data.Close();
	mockCallData12.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId, KErrNone);

	// Call 4 goes on hold, the other three which are part of the conference become connected
	callStatus = RMobileCall::EStatusHold;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData8(
		callId4, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData8.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData9(
		callId1, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData9.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData10(
		callId2, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData10.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData11(
		callId3, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData11.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
    callIds.AppendL( callId3 );
    
	TMockLtsyData1<RArray<TInt> > ltsyData(callIds);
	data.Close();
	ltsyData.SerialiseL(data);
	
	// Tidy up at the end of the test
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, KErrNone);

    DriverCompleteSuccessfulHangUpNotificationsL(callId1, EFalse, KErrGsmCCNormalCallClearing, EFalse);
    DriverCompleteSuccessfulHangUpNotificationsL(callId2, EFalse, KErrGsmCCNormalCallClearing, EFalse);
    DriverCompleteSuccessfulHangUpNotificationsL(callId3, EFalse, KErrGsmCCNormalCallClearing, EFalse);

    DriverCompleteSuccessfulHangUpNotificationsL(callId4, EFalse, KErrGsmCCNormalCallClearing, ETrue);

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

	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientCreateConferenceCallWithThreeRemotePartiesL(mobileService, mobileCall,
			mobileLine, mobileCall2, mobileLine2, mobileCall3, mobileLine3, confCall);

	ClientSwapConferenceCallL(confCall, RMobileConferenceCall::EConferenceSwapped,
					RMobileConferenceCall::EConferenceHold);

	RMobileLine mobileLine4;
	CleanupClosePushL(mobileLine4);
	RMobileCall mobileCall4;
	CleanupClosePushL(mobileCall4);

	ClientDialCallL(mobileLine4, mobileCall4, mobileService);

	ClientSwapConferenceCallL(confCall, RMobileConferenceCall::EConferenceSwapped,
				RMobileConferenceCall::EConferenceActive);

	// Call 4 should now be held
	ASSERT_EQUALS(KErrNone, mobileCall4.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	// Tidy up at the end of the test
	TRequestStatus hangupStatus;
	confCall.HangUp(hangupStatus);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(12, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0005L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0006
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to swap a conference call
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileConferenceCall::Swap
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0006L()
	{
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

	TMockLtsyData2<TInt, TInt> mockCallData12(callId1, callId3);
	data.Close();
	mockCallData12.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId, KErrGeneral);

    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
	
	DriverCloseThreeRemotePartyConferenceAndCallSubsessionsL(callId1, callId2, callId3, callIds);

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

	// Dial a new call and attempt to swap the conference with it
	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientDialCallL(mobileLine3, mobileCall3, mobileService);

	TRequestStatus swapStatus;
	confCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrGeneral, swapStatus.Int());

	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	ASSERT_EQUALS(KErrNone, confCall.GetConferenceStatus(confStatus));
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceHold, confStatus);

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);
	mobileCall3.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(10, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0006L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0007
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to add a call to the conference
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileConferenceCall::AddCall
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0007L()
	{
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

	TMockLtsyData2<TInt, TInt> mockData(callId3, callId1);
	data.Close();
	mockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId, KErrGeneral);

	// Tidy up at end of test
	
    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
	
	DriverCloseThreeRemotePartyConferenceAndCallSubsessionsL(callId1, callId2, callId3, callIds);

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

	ClientCreateConferenceCallL(confCall);

	ClientSwapConferenceCallL(confCall, RMobileConferenceCall::EConferenceSwapped,
			RMobileConferenceCall::EConferenceHold);

	// Dial a new call and attempt to add it to the conference
	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientDialCallL(mobileLine3, mobileCall3, mobileService);

	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	ASSERT_EQUALS(KErrNone, mobileCall3.GetMobileCallInfo(callInfoPckg));

	TRequestStatus addCallStatus;
	confCall.AddCall(addCallStatus, callInfo.iCallName);
	User::WaitForRequest(addCallStatus);
	ASSERT_EQUALS(KErrGeneral, addCallStatus.Int());

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);
	mobileCall3.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(10, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0007L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0008
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for going one to one in a conference call with two remote parties
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileCall::GoOneToOne
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallL(mobileService, callId1, callId2);

	TMockLtsyData1<TInt> oneToOneData(callId2);
	oneToOneData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, data);

	TMockLtsyData1<TInt> oneToOneMockData(callId2);
	data.Close();
	oneToOneMockData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, KErrNone, data);

	// Call 2 is connected and call 1 goes on hold
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusHold;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
		callId1, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
		callId2, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	TMockLtsyData2<TInt, TInt> mockData2(callId1, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	TMockLtsyData2<TInt, TInt> mockData3(callId2, hangUpCause);
	data.Close();
	mockData3.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

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

	ClientCreateConferenceCallL(confCall);

	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	TRequestStatus confCallReqStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(confCallReqStatus, confStatus);

	TRequestStatus oneToOneStatus;
	mobileCall2.GoOneToOne(oneToOneStatus);
	User::WaitForRequest(oneToOneStatus);
	ASSERT_EQUALS(KErrNone, oneToOneStatus.Int());

	User::WaitForRequest(confCallReqStatus);
	ASSERT_EQUALS(KErrNone, confCallReqStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, confStatus);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceSplit, confEvent);

	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);
	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0008L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0009
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for going one to one in a conference call with three remote parties
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileCall::GoOneToOne
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	TInt callId3 = 3;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceWithThreeRemotePartiesL(mobileService, callId1, callId2, callId3);

	TMockLtsyData1<TInt> oneToOneData(callId2);
	oneToOneData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, data);

	TMockLtsyData1<TInt> oneToOneMockData(callId2);
	data.Close();
	oneToOneMockData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, KErrNone, data);

	// Call 2 is connected and call 1, 3 go on hold
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusHold;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
		callId1, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
			callId3, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(
		callId2, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData3.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
	
	// Tidy up at end of test        
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	TMockLtsyData2<TInt, TInt> mockData1(callId1, hangUpCause);
	data.Close();
	mockData1.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	TMockLtsyData2<TInt, TInt> mockData2(callId2, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);
	
	TMockLtsyData2<TInt, TInt> mockData3(callId3, hangUpCause);
	data.Close();
	mockData3.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

    RArray<TInt> callIds;
    CleanupClosePushL( callIds ); // Empty call ids list since we remove the conference call after calls have hung up.

	TMockLtsyData1<RArray<TInt> > ltsyData((RArray<TInt>&) callIds);
	ltsyData.SerialiseL(data);
		
	// Clean up when subsessions are closed (conference call only, no longer contains any calls)
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, data);	
	
	// Client Side Test

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

	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientCreateConferenceCallWithThreeRemotePartiesL(mobileService, mobileCall,
			mobileLine, mobileCall2, mobileLine2, mobileCall3, mobileLine3, confCall);
	
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	TRequestStatus confCallReqStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(confCallReqStatus, confStatus);
	
	TRequestStatus oneToOneStatus;
	mobileCall2.GoOneToOne(oneToOneStatus);
	User::WaitForRequest(oneToOneStatus);
	ASSERT_EQUALS(KErrNone, oneToOneStatus.Int());

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, confEvent);
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceSplit, confEvent);
	
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);
	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);
	ASSERT_EQUALS(KErrNone, mobileCall3.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);
		
// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);
	mobileCall3.Close(); 
	User::After(50000);
	confCall.Close();	
	User::After(50000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(10, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0009L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0010
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failing to go one-to-one with a call in a conference
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileConferenceCall::GoOneToOne
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceCallL(mobileService, callId1, callId2);

	TMockLtsyData1<TInt> oneToOneData(callId2);
	oneToOneData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, data);

	TMockLtsyData1<TInt> oneToOneMockData(callId2);
	data.Close();
	oneToOneMockData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId, KErrGeneral, data);

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

	ClientCreateConferenceCallL(confCall);

	TRequestStatus oneToOneStatus;
	mobileCall2.GoOneToOne(oneToOneStatus);
	User::WaitForRequest(oneToOneStatus);
	ASSERT_EQUALS(KErrGeneral, oneToOneStatus.Int());

	TInt numCalls;
	ASSERT_EQUALS(KErrNone, confCall.EnumerateCalls(numCalls));
	ASSERT_EQUALS(2, numCalls);

	TRequestStatus hangupStatus;
	confCall.HangUp(hangupStatus);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0010L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UC0011
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for sending DTMF through a conference call
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SendDTMFTones
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlMultipartyFU::TestUseCase0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId1 = 1;
	TInt callId2 = 2;
	TInt callId3 = 3;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateConferenceWithThreeRemotePartiesL(mobileService, callId1, callId2, callId3);

	// When DTMF digits are sent and the active call is a conference call, the call ID
	// of one of the calls taking part in the conference is sent to the LTSY.
	TBuf<5> tones = _L("A123B");
	data.Close();
	TMockLtsyData2<TInt, TBuf<5> > toneLtsyData(callId1, tones);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
    callIds.AppendL( callId3 );

	// Tidy up at end of test
	DriverCloseThreeRemotePartyConferenceAndCallSubsessionsL(callId1, callId2, callId3, callIds);

	// Client Side Test

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

	RMobileLine mobileLine3;
	CleanupClosePushL(mobileLine3);
	RMobileCall mobileCall3;
	CleanupClosePushL(mobileCall3);

	ClientCreateConferenceCallWithThreeRemotePartiesL(mobileService, mobileCall,
			mobileLine, mobileCall2, mobileLine2, mobileCall3, mobileLine3, confCall);

	TRequestStatus sendStatus;
	iPhone.SendDTMFTones(sendStatus, tones);
	User::WaitForRequest(sendStatus);
	ASSERT_EQUALS(KErrNone, sendStatus.Int());

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 	
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);
	mobileCall3.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(10, this);
	} // CCTsyCallControlMultipartyFU::TestUseCase0011L


//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROLMULTIPARTY-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::CreateConference
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::CreateConference
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlMultipartyFU::TestUnit0001L()
	{
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

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInt, TInt> createConfData(callId1, callId2);
	createConfData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, data, KErrGeneral);

	TRequestStatus createConfStatus;
	confCall.CreateConference(createConfStatus);
	User::WaitForRequest(createConfStatus);
	ASSERT_EQUALS(KErrGeneral, createConfStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, KErrGeneral);

	confCall.CreateConference(createConfStatus);
	User::WaitForRequest(createConfStatus);
	ASSERT_EQUALS(KErrGeneral, createConfStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::CreateConference() when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, data);

    RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
    		callId1, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
    		callId2, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData2.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	confCall.CreateConference(createConfStatus);
	User::WaitForRequest(createConfStatus);
	ASSERT_EQUALS(KErrNone, createConfStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileConferenceCall::CreateConference()
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus terminatedStatus;
	iMockLTSY.NotifyTerminated(terminatedStatus);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId, KErrNone);

	User::WaitForRequest(terminatedStatus);

    RArray<TInt> callIds;
    CleanupClosePushL( callIds );
    callIds.AppendL( callId1 );
    callIds.AppendL( callId2 );
	
	// Clean up when subsessions are closed
	DriverCloseTwoRemotePartyConferenceAndCallSubsessionsL(callId1, callId2, callIds);

// NOTE: Adopting User::After(50000) in here is because of test harness failure in the SMP (naviengine).
//       The ETel server is closing a tel object by an Active Object (CAsyncOneShot), there can be out of ordered closes in SMP.
//       Since the MockLtsy is expecting exact order of test procedure, this can be regarded as an error but it is not a part of intended testing. 
//       Therefore this waiting function calls are required to close the tel objects in perfect order. 
	confCall.Close();
	User::After(50000);
	mobileCall.Close();
	User::After(50000);
	mobileCall2.Close();
	User::After(50000);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this);
	} // CCTsyCallControlMultipartyFU::TestUnit0001L


//
// Helper functions
//

void CCTsyCallControlMultipartyFU::DriverCreateConferenceCallL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2)
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
	} // CCTsyCallControlMultipartyFU::DriverCreateConferenceCallL

void CCTsyCallControlMultipartyFU::DriverCreateConferenceCallAndHoldItL(
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
	} // CCTsyCallControlMultipartyFU::DriverCreateConferenceCallAndHoldItL

void CCTsyCallControlMultipartyFU::DriverCreateConferenceWithThreeRemotePartiesL(
	RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2, TInt aCallId3)
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

	DriverDialCallL(aCallId3, aMobileService);

	TMockLtsyData2<TInt, TInt> mockData(aCallId3, aCallId1);
	data.Close();
	mockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId, KErrNone);

	// When a call is added to the conference, all the calls end up connected
	callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(
		aCallId1, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData3.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(
		aCallId2, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData4.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData5(
		aCallId3, RMobilePhone::EServiceUnspecified, callStatus);
	data.Close();
	mockCallData5.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlMultipartyFU::DriverCreateConferenceWithThreeRemotePartiesL

void CCTsyCallControlMultipartyFU::DriverCloseTwoRemotePartyConferenceAndCallSubsessionsL(TInt aCallId1, TInt aCallId2, const RArray<TInt>& aCallIds)
	{
	RBuf8 data;
	CleanupClosePushL(data);
    
	TMockLtsyData1<RArray<TInt> > ltsyData((RArray<TInt>&) aCallIds);
	ltsyData.SerialiseL(data);
		
	// Clean up when subsessions are closed (conference call and the two calls in the conference)
	iMockLTSY.ExpectL(MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId, data);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	TMockLtsyData2<TInt, TInt> mockData1(aCallId1, hangUpCause);
	data.Close();
	mockData1.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	TMockLtsyData2<TInt, TInt> mockData2(aCallId2, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlMultipartyFU::DriverCloseConferenceAndCallSubsessionsL

void CCTsyCallControlMultipartyFU::DriverCloseThreeRemotePartyConferenceAndCallSubsessionsL(TInt aCallId1, TInt aCallId2, TInt aCallId3, const RArray<TInt>& aCallIds)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	DriverCloseTwoRemotePartyConferenceAndCallSubsessionsL(aCallId1, aCallId2, aCallIds);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	TMockLtsyData2<TInt, TInt> mockData2(aCallId3, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlMultipartyFU::DriverCloseThreeRemotePartyConferenceAndCallSubsessionsL

void CCTsyCallControlMultipartyFU::ClientCreateConferenceCallL(RMobileConferenceCall& aConfCall)
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
	} // CCTsyCallControlMultipartyFU::ClientCreateConferenceCallL

void CCTsyCallControlMultipartyFU::ClientSwapConferenceCallL(RMobileConferenceCall& aConfCall,
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
	} // CCTsyCallControlMultipartyFU::ClientSwapConferenceCallL

void CCTsyCallControlMultipartyFU::ClientCreateConferenceCallAndHoldItL(
	RMobilePhone::TMobileService aMobileService, RMobileCall& aMobileCall,
	RMobileLine& aMobileLine, RMobileCall& aMobileCall2, RMobileLine& aMobileLine2,
	RMobileConferenceCall& aConfCall)
	{
    ClientDialAndHoldFirstCallDialSecondCallL(aMobileService, aMobileCall, aMobileLine, aMobileCall2, aMobileLine2);
    ClientCreateConferenceCallL(aConfCall);
    ClientSwapConferenceCallL(aConfCall, RMobileConferenceCall::EConferenceSwapped, RMobileConferenceCall::EConferenceHold);
	} // CCTsyCallControlMultipartyFU::ClientCreateConferenceCallAndHoldItL

void CCTsyCallControlMultipartyFU::ClientCreateConferenceCallWithThreeRemotePartiesL(
	RMobilePhone::TMobileService aMobileService, RMobileCall& aMobileCall,
	RMobileLine& aMobileLine, RMobileCall& aMobileCall2, RMobileLine& aMobileLine2,
	RMobileCall& aMobileCall3, RMobileLine& aMobileLine3,
	RMobileConferenceCall& aConfCall)
	{
	ClientCreateConferenceCallAndHoldItL(aMobileService, aMobileCall,
		aMobileLine, aMobileCall2, aMobileLine2, aConfCall);

	// Dial a new call and add it to the conference
	ClientDialCallL(aMobileLine3, aMobileCall3, aMobileService);

	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	ASSERT_EQUALS(KErrNone, aMobileCall3.GetMobileCallInfo(callInfoPckg));
	
	TRequestStatus addCallStatus;
	aConfCall.AddCall(addCallStatus, callInfo.iCallName);
	User::WaitForRequest(addCallStatus);
	ASSERT_EQUALS(KErrNone, addCallStatus.Int());

	TRequestStatus notifyConfStatus;
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	aConfCall.NotifyConferenceStatusChange(notifyConfStatus, confStatus);

	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName callName;
	TRequestStatus notifyConfEventStatus;
	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, callName);

	User::WaitForRequest(notifyConfEventStatus);
	ASSERT_EQUALS(KErrNone, notifyConfEventStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, confEvent);

	User::WaitForRequest(notifyConfStatus);
	ASSERT_EQUALS(KErrNone, notifyConfStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, confStatus);

	TInt numCalls;
	ASSERT_EQUALS(KErrNone, aConfCall.EnumerateCalls(numCalls));
	ASSERT_EQUALS(3, numCalls);
	} // CCTsyCallControlMultipartyFU::ClientCreateConferenceCallWithThreeRemotePartiesL



