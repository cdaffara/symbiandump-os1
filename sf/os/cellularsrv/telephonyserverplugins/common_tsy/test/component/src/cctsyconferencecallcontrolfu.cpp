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
// The TEFUnit test suite for ConferenceCallControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyconferencecallcontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cmmconferencecalltsy.h"
#include <sys/unistd.h>
#include <etelmmerr.h>

CTestSuite* CCTsyConferenceCallControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGoOneToOne0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGoOneToOne0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGoOneToOne0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGoOneToOne0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGoOneToOne0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGoOneToOne00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestEnumerateCalls0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestCreateConference0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestCreateConference0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestCreateConference0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestCreateConference0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestBreakDownConference0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestBreakDownConference0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestBreakDownConference0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGhostConference0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyCapsChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGetConferenceStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyConferenceEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyConferenceEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyConferenceEvent0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyConferenceStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyConferenceStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestNotifyConferenceStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestSwap0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestSwap0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestSwap0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestSwap0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestHangUp0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestHangUp0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestHangUp0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestHangUp0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestHangUp0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestAddCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestAddCall0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestAddCall0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestAddCall0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestAddCall0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGetCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGetMobileCallInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyConferenceCallControlFU, TestGetMobileCallInfo0003L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CONC-CGOTO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GoOneToOne for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GoOneToOne for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGoOneToOne0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RArray<TInt> callsInConference;
	CleanupClosePushL(callsInConference);
	
	// open new line for calls
	RLine line;
	OpenLineLC(line, KMmTsyVoice2LineName);
	
	// open 3d call
	RMobileCall call3;
	OpenNewCallLC(call3, line);
	
	//-------------------------------------------------------------------------
	// TEST: send request when there is no conference
 	//-------------------------------------------------------------------------
	
	TRequestStatus reqStatus;
	call3.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: send request when conference is not active
 	//-------------------------------------------------------------------------
	
	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	// test
	call3.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: send request for call that is not in conference (different call mode)
 	//-------------------------------------------------------------------------

	RLine line2;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line2, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	callsInConference.AppendL(1); // call1
	callsInConference.AppendL(2); // call2
	
	// dial 3rd call (status idle)
	_LIT(KPhoneNumber3, "1632960000");
	DialL(call3, 3, RMobilePhone::EAuxVoiceService, KPhoneNumber3);

	// test
	call3.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-----------------------------------------------------------------------------------
	// TEST: send request for call that is not in conference (call mode is EVoiceService)
	//-----------------------------------------------------------------------------------
	call3.Close();
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
		
    TName callName3;
	OpenNewCallLC(call3, line2, &callName3);
	CleanupStack::Pop(&call3); // call3 has already been put into the cleanup stack.
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	ChangeCallStatusL(3, RMobileCall::EStatusConnecting);
	ChangeCallStatusL(3, RMobileCall::EStatusConnected);

	// test
	call3.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// add call3 to conference
	AddCallL(conferenceCall, 3, callName3, callsInConference);	
	callsInConference.AppendL(3);  // call3	
	
	//-----------------------------------------------------------------------------------
	// TEST: place call on hold after requesting go one-to-one
	//-----------------------------------------------------------------------------------
	// test
	TMockLtsyCallData0 emptyCallData3(3, RMobilePhone::EVoiceService);
	emptyCallData3.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	call3.GoOneToOne(reqStatus);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotReady, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: send request for call that is not in conference (status hold)
 	//-------------------------------------------------------------------------
	
	// open 4th call
	RMobileCall call4;
	TName callName4;
	OpenNewCallLC(call4, line2, &callName4);

	// dial 4th call (status hold)
	_LIT(KPhoneNumber4, "1472583690");
	DialL(call4, 4, RMobilePhone::EVoiceService, KPhoneNumber4);
	
	// first put the conference call (call1, call2 and call3) on hold
	ChangeCallStatusInOrderL(1, 3, RMobileCall::EStatusHold);
	
    ChangeCallStatusL(4, RMobileCall::EStatusConnecting);
    ChangeCallStatusL(4, RMobileCall::EStatusConnected);    
	
	// test
	call4.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	
	// add call4 to conference
	AddCallL(conferenceCall, 4, callName4, callsInConference);
	callsInConference.AppendL(4); // call4
	// add call5 to conference
	RMobileCall call5;
	_LIT(KPhoneNumber5, "9999999990");
	MakeCallAndAddToConferenceLC(call5, line2, conferenceCall, 5, KPhoneNumber5, callsInConference); 
	callsInConference.AppendL(5); // call5
	// test
	TMockLtsyCallData0 emptyCallData4(4, RMobilePhone::EVoiceService);
	emptyCallData4.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data, KErrNotSupported);
	
	call4.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::GoOneToOne.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	call4.GoOneToOne(reqStatus);
	
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	ChangeCallStatusL(5, RMobileCall::EStatusHold);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::GoOneToOne
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobileCallGoOneToOne, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: increase coverage
 	//-------------------------------------------------------------------------

	// make conference active (add 4th call to the hold conference)
	AddCallL(conferenceCall, 4, callName4, callsInConference);

    // test
    data.Close();
	emptyCallData4.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	call4.GoOneToOne(reqStatus);
	
    ChangeCallStatusL(5, RMobileCall::EStatusHold);
    ChangeCallStatusL(1, RMobileCall::EStatusHold);
    ChangeCallStatusL(3, RMobileCall::EStatusHold);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    ChangeCallStatusL(2, RMobileCall::EStatusHold);	
    
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST: increase coverage
 	//-------------------------------------------------------------------------

    // make conference active (add 4th call to the hold conference)
    AddCallL(conferenceCall, 4, callName4, callsInConference);
    
    // test
    data.Close();
    TMockLtsyCallData0 emptyCallData5(5, RMobilePhone::EVoiceService);
    emptyCallData5.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
    call5.GoOneToOne(reqStatus);
    
    // test
	ChangeCallStatusL(5, RMobileCall::EStatusDisconnecting);
	
    // GoOneToOne failed because call5 is disconnecting
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileCallGoOneToOne, KErrGeneral, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	// call5 is in idle state
	ChangeCallStatusL(5, RMobileCall::EStatusIdle);	

	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(4, RMobileCall::EStatusHold);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

    // close call5 object
	CleanupStack::PopAndDestroy(&call5);

	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// swap conference to active
	data.Close();
	TMockLtsyCallData0 emptyCallData1(1, RMobilePhone::EVoiceService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallResume, data);
	conferenceCall.Swap(reqStatus);

    ChangeCallStatusL(3, RMobileCall::EStatusConnected);
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);
	ChangeCallStatusL(4, RMobileCall::EStatusConnected);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// test
	data.Close();
	emptyCallData4.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	iMockLTSY.CompleteL(EMobileCallGoOneToOne, KErrGeneral, data);
	
	call4.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(4, RMobileCall::EStatusHold);

	RMobileCall::TMobileCallCapsV1 caps;
	RMobileCall::TMobileCallCapsV1Pckg capsDes(caps);
	TInt res = call4.GetMobileCallCaps(capsDes);
	ASSERT_EQUALS(KErrNone, res);

    //GoOneToOne operation is not be supported for call4. This is because
    //call4 is already in connected state and other calls are in hold state.
    TUint32 oneToOneNotSupported = 0;
    ASSERT_EQUALS(oneToOneNotSupported, caps.iCallControlCaps & RMobileCall::KCapsOneToOne);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(10, this); // call4, call2, call, line2, conferenceCall, call3, line, callsInConference, data, this

	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGOTO-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::GoOneToOne for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::GoOneToOne for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGoOneToOne0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	RLine line;
	RMobileCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// open 3rd call for coverage
	RCall call3;
	OpenNewCallLC(call3, line);
	_LIT(KPhoneNumber3, "3333333330");   	
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	ChangeCallStatusL(3, RMobileCall::EStatusDialling);
	
	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::GoOneToOne
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	
	// post request
	TRequestStatus reqStatus;
	call.GoOneToOne(reqStatus);
	
	// cancel
	call.CancelAsyncRequest(EMobileCallGoOneToOne);
	
	// invoke request completion from mockLtsy
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	
	User::WaitForRequest(reqStatus);
	// CTSY has no cancel for this ipc, so request completes with KErrNone
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // call3, call2, call, line, conferenceCall, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGOTO-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::GoOneToOne for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::GoOneToOne for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGoOneToOne0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RArray<TInt> callsInConference;
	CleanupClosePushL(callsInConference);
		
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	RLine line;
	RMobileCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	callsInConference.AppendL(1); // call1
	callsInConference.AppendL(2); // call2
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);

	// add 3rd call to conference from 2nd client
	RLine line2;
	TInt res = line2.Open(phone2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(line);

	RMobileCall call3;
	_LIT(KPhoneNumber3, "1632960000");
	MakeCallAndAddToConferenceLC(call3, line2, conferenceCall2, 3, KPhoneNumber3, callsInConference);
	callsInConference.AppendL(3); // call3
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::GoOneToOne
 	//-------------------------------------------------------------------------

	// post 1st request
	TMockLtsyCallData0 emptyCallData1(1, RMobilePhone::EVoiceService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	TRequestStatus reqStatus;
	call.GoOneToOne(reqStatus);
	// check that EMobileCallGoOneToOne ipc request is passed to ltsy
	AssertMockLtsyStatusL();

	// post 2nd request
	TRequestStatus reqStatus2;
	call3.GoOneToOne(reqStatus2);
	// check that request is not passed to ltsy
	AssertMockLtsyStatusL();

	// notify CTSY about changes in calls' statuses from mockLtsy
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNotSupported, reqStatus2.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(12, this); // call3, line2, conferenceCall2, phone2, telServer2, call2, call, line, conferenceCall, callsInConference, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGOTO-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GoOneToOne with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GoOneToOne and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGoOneToOne0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	RLine line;
	RMobileCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileCall::GoOneToOne
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	
	TRequestStatus reqStatus;
	call.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGOTO-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GoOneToOne for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GoOneToOne for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGoOneToOne0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open new line for calls
	RLine line;
	TInt res = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(line);
	
	// open call
	RMobileCall call;
	res = call.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(call);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::GoOneToOne when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	call.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGOTO-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GoOneToOne for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GoOneToOne for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGoOneToOne00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open new line for calls
	RLine line;
	TInt res = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(line);
	
	// open call
	RMobileCall call;
	res = call.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(call);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::GoOneToOne when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	call.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::EnumerateCalls
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::EnumerateCalls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestEnumerateCalls0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RArray<TInt> callsInConference;
	CleanupClosePushL(callsInConference);
 	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::EnumerateCalls when result is not cached.
 	//-------------------------------------------------------------------------

	TInt count;
	TInt res = conferenceCall.EnumerateCalls(count);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(0, count);
	AssertMockLtsyStatusL();

	// create conference
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	callsInConference.AppendL(1); // call1
	callsInConference.AppendL(2); // call2
	
	// calls enumeration request again
	res = conferenceCall.EnumerateCalls(count);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(2, count);
	AssertMockLtsyStatusL();

	// add call to conference
	RCall call3;
	_LIT(KPhoneNumber3, "3333333330");
	MakeCallAndAddToConferenceLC(call3, line, conferenceCall, 3, KPhoneNumber3, callsInConference);
	callsInConference.AppendL(3); // call3
	
	// calls enumeration request again
	res = conferenceCall.EnumerateCalls(count);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(3, count);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this); // call3, call2, call, line, callsInConference, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::CreateConference
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::CreateConference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestCreateConference0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
 	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference, KErrNotSupported);

	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	iMockLTSY.CompleteL(EMobileConferenceCallCreateConference, KErrGeneral);

	conferenceCall.CreateConference(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::CreateConference.
 	//-------------------------------------------------------------------------

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");
	_LIT(KPhoneNumber2, "1234567890");
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileConferenceCall::CreateConference
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobileConferenceCallCreateConference, KErrNone);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::CreateConference
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::CreateConference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestCreateConference0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::CreateConference
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	iMockLTSY.CompleteL(EMobileConferenceCallCreateConference, KErrNone, 2);

	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);

	conferenceCall.CancelAsyncRequest(EMobileConferenceCallCreateConference);
	
	User::WaitForRequest(reqStatus);
	// CTSY has no cancel for this ipc, so request completes with KErrNone
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::CreateConference
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::CreateConference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestCreateConference0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	// open new line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	OpenNewCallLC(call, line);

	// dial first call (make them hold)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);

	// open second call
	RCall call2;
	OpenNewCallLC(call2, line);

	// dial second call (make them connected)
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// --------------------Open second client------------------
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::CreateConference
	//-------------------------------------------------------------------------

	// first request
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	AssertMockLtsyStatusL();

	// second request
	TRequestStatus reqStatus2;
	conferenceCall2.CreateConference(reqStatus2);
	AssertMockLtsyStatusL();

	// connect first call from mockLtsy
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	// refresh second call status from mockLtsy
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNotReady, reqStatus2.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); // conferenceCall2, phone2, telServer2, call2, call, line, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COCC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::CreateConference with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::CreateConference and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestCreateConference0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileConferenceCall::CreateConference
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference, KErrNone);

	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COBDC-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for break down conference from LTSY for active conference
@SYMTestPriority High
@SYMTestActions Invokes break down conference from LTSY for active conference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestBreakDownConference0001aL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	// open line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	TName callName;
	OpenNewCallLC(call, line, &callName);
	// dial first call (status connected)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	
	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	
	// update first call status
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);

	// open and connect second call exactly here for increase coverage
	RCall call2;
	OpenNewCallLC(call2, line);
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);
	
	// conference is created here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for break down conference 
	// from LTSY for active conference
	//--------------------------------
	
	TRequestStatus statChangeStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(statChangeStatus, status);

	TRequestStatus notifyEventStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName notifyCallName;
	conferenceCall.NotifyConferenceEvent(notifyEventStatus, event, notifyCallName);
	
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	
	// notify requests complete here (actual test)
	User::WaitForRequest(notifyEventStatus);
	ASSERT_EQUALS(KErrNone, notifyEventStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, event);

	User::WaitForRequest(statChangeStatus);
	ASSERT_EQUALS(KErrNone, statChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, status);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COBDC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for break down conference from LTSY for hold conference
@SYMTestPriority High
@SYMTestActions Invokes break down conference from LTSY for hold conference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestBreakDownConference0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	// open line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	TName callName;
	OpenNewCallLC(call, line, &callName);

	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	
	// open call, dial (status connected) and make them idle for coverage
	// (conference caps changes back to zero)
	RCall someCall;
	OpenNewCallLC(someCall, line);
	_LIT(KSomePhoneNumber, "1632960000");
	DialL(someCall, 8, RMobilePhone::EVoiceService, KSomePhoneNumber);
	ChangeCallStatusL(8, RMobileCall::EStatusConnected);// here conf.caps become KCapsCreate
	ChangeCallStatusL(8, RMobileCall::EStatusIdle);// zero conf.caps

	// open second call
	RCall call2;
	OpenNewCallLC(call2, line);
	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	
	// connect first call
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	// update second call status
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// conference is created here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// swap conference
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data);
	conferenceCall.Swap(reqStatus);
	// if swap request did not pass to Ltsy the following assert will fail the test
	AssertMockLtsyStatusL();

	// set calls 1 and 2 on hold from MockLtsy
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);

	// CTSY completes swap request here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	//--------------------------------
	// TEST: Test support in CTSY for break down conference 
	// from LTSY for hold conference
	//--------------------------------
	
	TRequestStatus statChangeStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(statChangeStatus, status);

	TRequestStatus notifyEventStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName notifyCallName;
	conferenceCall.NotifyConferenceEvent(notifyEventStatus, event, notifyCallName);
	
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	
	// notify requests complete here (actual test)
	User::WaitForRequest(notifyEventStatus);
	ASSERT_EQUALS(KErrNone, notifyEventStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, event);

	User::WaitForRequest(statChangeStatus);
	ASSERT_EQUALS(KErrNone, statChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, status);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // call2, someCall, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COBDC-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for break down conference from LTSY for active conference with hold call
@SYMTestPriority High
@SYMTestActions Invokes break down conference from LTSY for active conference with hold call
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestBreakDownConference0001cL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	// open line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	TName callName;
	OpenNewCallLC(call, line, &callName);
	// dial first call (status connected)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	
	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	
	// update first call status
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);

	// open and connect second call exactly here for increase coverage
	RCall call2;
	OpenNewCallLC(call2, line);
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);
	
	// conference is created here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// create hold call for coverage
	RCall call3;
	OpenNewCallLC(call3, line);
	_LIT(KPhoneNumber3, "3333333330");
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);

	//--------------------------------
	// TEST: Test support in CTSY for break down conference 
	// from LTSY for active conference with hold call
	//--------------------------------
	
	TRequestStatus statChangeStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(statChangeStatus, status);

	TRequestStatus notifyEventStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName notifyCallName;
	conferenceCall.NotifyConferenceEvent(notifyEventStatus, event, notifyCallName);
	
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	
	// notify request completes here (actual test)
	User::WaitForRequest(notifyEventStatus);
	ASSERT_EQUALS(KErrNone, notifyEventStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, event);

	User::WaitForRequest(statChangeStatus);
	ASSERT_EQUALS(KErrNone, statChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, status);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // call3, call2, call, line, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COGCO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for ghost operations from LTSY with conference
@SYMTestPriority High
@SYMTestActions Invokes ghost operations from LTSY with conference
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGhostConference0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	// open line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RMobileCall call;
	TName callName;
	OpenNewCallLC(call, line, &callName);

	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	
	// open second call
	RCall call2;
	TName callName2;
	OpenNewCallLC(call2, line, &callName2);

	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	AssertMockLtsyStatusL();

	// The following tests are to cover CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent()
	
	//--------------------------------
	// TEST: Test support in CTSY for ghost conference creation from LTSY
	// in case when conference is creating through Etel too (just for increase coverage)
	//--------------------------------
	
	// send request through Etel
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);

	// failing attempt to invoke starting of ghost conference creation
	// (covering code)
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	TInt completeConfEvent = RMobileConferenceCall::EConferenceBuilt;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData1<TInt> completeGhostOperationData(1, mobileService, completeConfEvent);
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    // then fail no more necessary etel request
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallCreateConference, KErrGeneral);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//--------------------------------
	// TEST: Test support in CTSY for ghost add call to conference operation from LTSY
	// over ghost add call to conference operation
	// when conference is idle
	//--------------------------------

	// failing attempt to invoke starting of ghost add call to conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeConfEvent = RMobileConferenceCall::EConferenceCallAdded;
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//--------------------------------
	// TEST: Test support in CTSY for ghost swap operation from LTSY
	// when conference is idle
	//--------------------------------

	// failing attempt to invoke starting of ghost swap conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeConfEvent = RMobileConferenceCall::EConferenceSwapped;
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//--------------------------------
	// TEST: Test support in CTSY for ghost conference creation from LTSY 
	//--------------------------------
	
	// post conference status change notify request
	TRequestStatus statusChangeStatus;
	RMobileConferenceCall::TMobileConferenceStatus mobileConfStatus;
	conferenceCall.NotifyConferenceStatusChange(statusChangeStatus, mobileConfStatus);

	// post conference event notify request
	TRequestStatus confEventStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName notifyCallName;
	conferenceCall.NotifyConferenceEvent(confEventStatus, event, notifyCallName);
	
	// invoke starting of ghost conference creation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeConfEvent = RMobileConferenceCall::EConferenceBuilt;
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// connect first call
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);

	// notify conference event completes here (intermediate test)
	User::WaitForRequest(confEventStatus);
	ASSERT_EQUALS(KErrNone, confEventStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);
	// repost conference event notify request
	conferenceCall.NotifyConferenceEvent(confEventStatus, event, notifyCallName);

	// update second call status
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// notify conference event completes here (intermediate test)
	User::WaitForRequest(confEventStatus);
	ASSERT_EQUALS(KErrNone, confEventStatus.Int());
	ASSERT_EQUALS(callName2, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	// here completes ghost conference creation
	// notify conference status change completes here (actual test)
	User::WaitForRequest(statusChangeStatus);
	ASSERT_EQUALS(KErrNone, statusChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, mobileConfStatus);

	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for ghost add call to conference operation from LTSY
	// over ghost build conference operation
	// when call is adding through Etel (just for increase coverage)
	//--------------------------------

	// open 3rd call
	RCall call3;
	TName callName3;
	OpenNewCallLC(call3, line, &callName3);

	// dial 3rd call (status hold)
	_LIT(KPhoneNumber3, "3333333330");
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	// First put the conference call (call1 and call2) on hold
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusHold);
	// Then change the status of call3 in correct order so that right capabilities can be assigned
	ConnectAndPutCallOnHoldInRightOrderL(3);
	
	// send add call request through Etel
	TMockLtsyCallData0 emptyCallDataServiceUnspecified3(3, RMobilePhone::EServiceUnspecified);
	data.Close();
	emptyCallDataServiceUnspecified3.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);
	conferenceCall.AddCall(reqStatus, callName3);

	// failing attempt to invoke starting of ghost add call operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    // then fail no more necessary etel request
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallAddCall, KErrGeneral);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//--------------------------------
	// TEST: Test support in CTSY for ghost add call to conference operation from LTSY
	// over ghost build conference operation
	//--------------------------------
	
	// post conference event notify request
	conferenceCall.NotifyConferenceEvent(confEventStatus, event, notifyCallName);
	
	// invoke starting of ghost add call to conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // connect conference calls
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusConnected);
	// connect 3rd call
	ChangeCallStatusL(3, RMobileCall::EStatusConnected);
	
	// notify conference event completes here (actual test)
	User::WaitForRequest(confEventStatus);
	ASSERT_EQUALS(KErrNone, confEventStatus.Int());
	ASSERT_EQUALS(callName3, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for ghost add call to conference operation from LTSY
	// over ghost add call to conference operation
	// when call is adding through Etel (just for increase coverage)
	//--------------------------------

	// open 4th call
	RCall call4;
	TName callName4;
	OpenNewCallLC(call4, line, &callName4);

	// dial 4th call (status hold)
	_LIT(KPhoneNumber4, "4444444440");
	DialL(call4, 4, RMobilePhone::EVoiceService, KPhoneNumber4);
	// put the conference call (call1, call2 and call3) on hold
	ChangeCallStatusInOrderL(1, 3, RMobileCall::EStatusHold);
	ConnectAndPutCallOnHoldInRightOrderL(4);

	// send add call request through Etel
	TMockLtsyCallData0 emptyCallDataServiceUnspecified4(4, RMobilePhone::EServiceUnspecified);
	data.Close();
	emptyCallDataServiceUnspecified4.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);
	conferenceCall.AddCall(reqStatus, callName4);

	// failing attempt to invoke starting of ghost add call operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeConfEvent = RMobileConferenceCall::EConferenceCallAdded;
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    // then fail no more necessary etel request
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallAddCall, KErrGeneral);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//--------------------------------
	// TEST: Test support in CTSY for ghost add call to conference operation from LTSY
	// over ghost add call to conference operation
	//--------------------------------

	// post conference event notify request
	conferenceCall.NotifyConferenceEvent(confEventStatus, event, notifyCallName);
	
	// invoke starting of ghost add call to conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // make the conference(call1, call2 and call3) call active
    ChangeCallStatusInOrderL(1, 3, RMobileCall::EStatusConnected);
	// connect 4th call
	ChangeCallStatusL(4, RMobileCall::EStatusConnected);
	
	// notify conference event completes here (actual test)
	User::WaitForRequest(confEventStatus);
	ASSERT_EQUALS(KErrNone, confEventStatus.Int());
	ASSERT_EQUALS(callName4, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for ghost GoOneToOne operation from LTSY
	// when GoOneToOne is called through Etel (just for increase coverage)
	//--------------------------------

	// call GoOneToOne through Etel
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	data.Close();
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallGoOneToOne, data);
	call.GoOneToOne(reqStatus);

	// failing attempt to invoke starting of ghost go one to one operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeConfEvent = RMobileConferenceCall::EConferenceSplit;
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // then fail no more necessary etel request
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	emptyCallData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallGoOneToOne, KErrGeneral, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//--------------------------------
	// TEST: Test support in CTSY for ghost GoOneToOne operation from LTSY
	//--------------------------------

	// post conference status change notify request
	do
		{
		conferenceCall.NotifyConferenceStatusChange(statusChangeStatus, mobileConfStatus);
		}
	while ( statusChangeStatus.Int() != KRequestPending );

	// post conference event notify request
	conferenceCall.NotifyConferenceEvent(confEventStatus, event, notifyCallName);

	// invoke starting of ghost go one to one operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// make 1st call the only connected call
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	ChangeCallStatusL(4, RMobileCall::EStatusHold);
	
	// notify conference event completes here (actual test)
	User::WaitForRequest(confEventStatus);
	ASSERT_EQUALS(KErrNone, confEventStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, event);
	// notify conference status change completes here (actual test)
	User::WaitForRequest(statusChangeStatus);
	ASSERT_EQUALS(KErrNone, statusChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceHold, mobileConfStatus);

	// repost conference event notify request
	conferenceCall.NotifyConferenceEvent(confEventStatus, event, notifyCallName);
	// notify conference event completes here (actual test)
	User::WaitForRequest(confEventStatus);
	ASSERT_EQUALS(KErrNone, confEventStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceSplit, event);

	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for ghost swap operation from LTSY
	// when swap is called through Etel (just for increase coverage)
	//--------------------------------

	// call Swap through Etel
	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	conferenceCall.Swap(reqStatus);
	
	// failing attempt to invoke starting of ghost swap conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	data.Close();
	completeConfEvent = RMobileConferenceCall::EConferenceSwapped;
	completeGhostOperationData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    // then fail no more necessary etel request
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallSwap, KErrGeneral);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for ghost swap operation from LTSY
	// when conference is hold
	//--------------------------------

	// post conference status change notify request
	conferenceCall.NotifyConferenceStatusChange(statusChangeStatus, mobileConfStatus);

	// invoke starting of ghost swap conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// make 1st call the only hold call
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);
	ChangeCallStatusL(3, RMobileCall::EStatusConnected);
	ChangeCallStatusL(4, RMobileCall::EStatusConnected);
	
	// notify conference status change completes here (actual test)
	User::WaitForRequest(statusChangeStatus);
	ASSERT_EQUALS(KErrNone, statusChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, mobileConfStatus);

	AssertMockLtsyStatusL();

	//--------------------------------
	// TEST: Test support in CTSY for ghost swap operation from LTSY
	// when conference is active
	//--------------------------------

	// post conference status change notify request
	conferenceCall.NotifyConferenceStatusChange(statusChangeStatus, mobileConfStatus);

	// invoke starting of ghost swap conference operation
	// (covering code)
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallNotifyConferenceEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// make 1st call the only connected call
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	ChangeCallStatusL(4, RMobileCall::EStatusHold);
	
	// notify conference status change completes here (actual test)
	User::WaitForRequest(statusChangeStatus);
	ASSERT_EQUALS(KErrNone, statusChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceHold, mobileConfStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); // call4, call3, call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::NotifyCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::NotifyCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::NotifyCapsChange.
 	//-------------------------------------------------------------------------

	// post request
	TRequestStatus notifyStatus;
	TUint32 caps;
	conferenceCall.NotifyCapsChange(notifyStatus, caps);

	// open new line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	OpenNewCallLC(call, line);

	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);

	// open second call
	RCall call2;
	OpenNewCallLC(call2, line);

	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);
	
	// completion of notify request (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobileConferenceCall::KCapsCreate), caps);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: Increase coverage (lets create conference)
 	//-------------------------------------------------------------------------

	// repost request
	conferenceCall.NotifyCapsChange(notifyStatus, caps);

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);

	// make first call connected
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	// update second call status (connected)
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// conference is created
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// completion of notify request (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobileConferenceCall::KCapsHangUp |
	                                   RMobileConferenceCall::KCapsSwap), caps);
	
	//-------------------------------------------------------------------------
	// TEST: Increase coverage (lets hangup conference)
 	//-------------------------------------------------------------------------

	// repost request
	conferenceCall.NotifyCapsChange(notifyStatus, caps);

	// hangup conference
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	conferenceCall.HangUp(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	ChangeCallStatusL(2, RMobileCall::EStatusIdle);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// completion of notify request (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(0), caps);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::NotifyCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::NotifyCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::NotifyCapsChange
 	//-------------------------------------------------------------------------
 	
	// post request
	TRequestStatus notifyStatus;
	TUint32 caps;
	conferenceCall.NotifyCapsChange(notifyStatus, caps);

	// cancel
	conferenceCall.CancelAsyncRequest(EMobileConferenceCallNotifyCapsChange);

	// completion of notify request 
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCancel, notifyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::NotifyCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::NotifyCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyCapsChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::NotifyCapsChange
 	//-------------------------------------------------------------------------

	// post request
	TRequestStatus notifyStatus;
	TUint32 caps;
	conferenceCall.NotifyCapsChange(notifyStatus, caps);

	TRequestStatus notifyStatus2;
	TUint32 caps2;
	conferenceCall2.NotifyCapsChange(notifyStatus2, caps2);

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// completion of 1st notify request
	User::WaitForRequest(notifyStatus);
	// completion of 2nd notify request
	User::WaitForRequest(notifyStatus2);

	// check results
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobileConferenceCall::KCapsCreate), caps);

	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobileConferenceCall::KCapsCreate), caps2);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this); // call2, call, line, conferenceCall2, phone2, telServer2, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COGCS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::GetConferenceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::GetConferenceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGetConferenceStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::GetConferenceStatus.
 	//-------------------------------------------------------------------------

	RMobileConferenceCall::TMobileConferenceStatus status;
	TInt res = conferenceCall.GetConferenceStatus(status);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, status);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// RMobileConferenceCall::GetConferenceStatus with other Caps value.
 	//-------------------------------------------------------------------------

	// invoke conference status change
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// test
	res = conferenceCall.GetConferenceStatus(status);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, status);
	AssertMockLtsyStatusL();

	// invoke conference status change (swap conference)
	TMockLtsyCallData0 emptyCallData(1, RMobilePhone::EVoiceService);
	emptyCallData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data);
	TRequestStatus reqStatus;
	conferenceCall.Swap(reqStatus);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// test again
	res = conferenceCall.GetConferenceStatus(status);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceHold, status);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); // call2, call, line, conferenceCall, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::NotifyConferenceEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::NotifyConferenceEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyConferenceEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::NotifyConferenceEvent.
 	//-------------------------------------------------------------------------

	// post request
	TRequestStatus notifyStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName notifyCallName;
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);

	// open new line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	TName callName;
	OpenNewCallLC(call, line, &callName);

	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);

	// open second call
	RCall call2;
	TName callName2;
	OpenNewCallLC(call2, line, &callName2);

	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);

	// make first call connected
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	//-------------------------------------------------------------------------
	// TEST: Increase coverage (conference creation continues)
 	//-------------------------------------------------------------------------
	
	// repost tested request
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);

	// update second call status (connected)
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(callName2, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	//-------------------------------------------------------------------------
	// TEST: Increase coverage (conference creation continues)
 	//-------------------------------------------------------------------------

	// repost tested request
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);

	// conference creation request completes here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	TName callsInConference;
	callsInConference.Zero ();
	callsInConference.Append (callName2);
	callsInConference.Append (KDelimiter);
	callsInConference.Append (callName);
	ASSERT_EQUALS(callsInConference, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceBuilt, event);

	//-------------------------------------------------------------------------
	// TEST: Increase coverage (lets hangup conference)
 	//-------------------------------------------------------------------------

	// repost tested request
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);

	// hangup conference
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	conferenceCall.HangUp(reqStatus);

	// make 1st call idle
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, event);

	//-------------------------------------------------------------------------
	// TEST: Increase coverage (conference hangup continues)
 	//-------------------------------------------------------------------------

	// repost tested request
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);

	// make 2nd call idle
	ChangeCallStatusL(2, RMobileCall::EStatusIdle);

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(callName2, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, event);

	// here completes hangup request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::NotifyConferenceEvent
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::NotifyConferenceEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyConferenceEvent0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::NotifyConferenceEvent
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName callName;
	conferenceCall.NotifyConferenceEvent(reqStatus, event, callName);

	conferenceCall.CancelAsyncRequest(EMobileConferenceCallNotifyConferenceEvent);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::NotifyConferenceEvent
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::NotifyConferenceEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyConferenceEvent0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::NotifyConferenceEvent
 	//-------------------------------------------------------------------------

	// post request
	TRequestStatus notifyStatus;
	RMobileConferenceCall::TMobileConferenceEvent event;
	TName notifyCallName;
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);

	TRequestStatus notifyStatus2;
	RMobileConferenceCall::TMobileConferenceEvent event2;
	TName notifyCallName2;
	conferenceCall2.NotifyConferenceEvent(notifyStatus2, event2, notifyCallName2);

	// open new line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	TName callName;
	OpenNewCallLC(call, line, &callName);

	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);

	// open second call
	RCall call2;
	TName callName2;
	OpenNewCallLC(call2, line, &callName2);

	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);

	// make first call connected
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(callName, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS(callName, notifyCallName2);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event2);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: Increase coverage (conference creation continues)
 	//-------------------------------------------------------------------------

	// repost tested request
	conferenceCall.NotifyConferenceEvent(notifyStatus, event, notifyCallName);
	conferenceCall2.NotifyConferenceEvent(notifyStatus2, event2, notifyCallName2);

	// update second call status (connected)
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// notify request completes here (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(callName2, notifyCallName);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event);

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS(callName2, notifyCallName2);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallAdded, event2);
	AssertMockLtsyStatusL();

	// conference creation request completes here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this); // call2, call, line, conferenceCall2, phone2, telServer2, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::NotifyConferenceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::NotifyConferenceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyConferenceStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::NotifyConferenceStatusChange.
 	//-------------------------------------------------------------------------

	// post request
	TRequestStatus notifyStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(notifyStatus, status);

	// invoke conference status change
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// completion of notify request
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, status);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// RMobileConferenceCall::NotifyConferenceStatusChange with other Caps value.
 	//-------------------------------------------------------------------------

	// post request
	conferenceCall.NotifyConferenceStatusChange(notifyStatus, status);

	// hangup conference
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	TRequestStatus reqStatus;
	conferenceCall.HangUp(reqStatus);
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	ChangeCallStatusL(2, RMobileCall::EStatusIdle);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// completion of notify request (actual test)
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, status);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::NotifyConferenceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::NotifyConferenceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyConferenceStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::NotifyConferenceStatusChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus notifyStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(notifyStatus, status);

	conferenceCall.CancelAsyncRequest(EMobileConferenceCallNotifyConferenceStatusChange);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCancel, notifyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CONCSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::NotifyConferenceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::NotifyConferenceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestNotifyConferenceStatusChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::NotifyConferenceStatusChange
 	//-------------------------------------------------------------------------

	// post request
	TRequestStatus notifyStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(notifyStatus, status);

	// post request
	TRequestStatus notifyStatus2;
	RMobileConferenceCall::TMobileConferenceStatus status2;
	conferenceCall2.NotifyConferenceStatusChange(notifyStatus2, status2);

	// invoke conference status change
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// completion of notify request
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, status);

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, status2);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this); // call2, call, line, conferenceCall2, phone2, telServer2, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::Swap
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::Swap
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestSwap0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	//-------------------------------------------------------------------------
	// TEST: failure to swap not created conference call
 	//-------------------------------------------------------------------------
	
	TRequestStatus reqStatus;
	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotReady, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A1: failure to dispatch request to LTSY
	// when conference is active and no hold calls.
 	//-------------------------------------------------------------------------

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data, KErrNotSupported);

	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B1: failure on completion of pending request from LTSY->CTSY
	// when conference is active and no hold calls.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallHold, data);

	iMockLTSY.CompleteL(EMobileConferenceCallSwap, KErrGeneral);

	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C1: Successful completion request of
	// RMobileConferenceCall::Swap
	// when conference is active and no hold calls.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallHold, data);
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A2: failure to dispatch request to LTSY
	// when conference is hold and no active calls.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallResume, data, KErrNotSupported);
	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B2: failure on completion of pending request from LTSY->CTSY
	// when conference is hold and no active calls.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallResume, data);
	iMockLTSY.CompleteL(EMobileConferenceCallSwap, KErrGeneral);
	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMobileConferenceCall::Swap
	// when conference is hold and no active calls.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallResume, data);
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A3: failure to dispatch request to LTSY
	// when conference is active and there is a hold call.
 	//-------------------------------------------------------------------------

	// dial 3rd call (status hold)
	RCall call3;
	OpenNewCallLC(call3, line);
	_LIT(KPhoneNumber3, "3333333330");   	
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);

	// open 4th call for increase coverage objectives 
	RCall call4;
	OpenNewCallLC(call4, line);

	// test
	iMockLTSY.ExpectL(EMobileConferenceCallSwap, KErrNotSupported);

	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B3: failure on completion of pending request from LTSY->CTSY
	// when conference is active and there is a hold call.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	iMockLTSY.CompleteL(EMobileConferenceCallSwap, KErrGeneral);

	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C3: Successful completion request of
	// RMobileConferenceCall::Swap
	// when conference is active and there is a hold call.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusConnected);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A4: failure to dispatch request to LTSY
	// when conference is hold and there is an active call.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(EMobileConferenceCallSwap, KErrNotSupported);

	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B4: failure on completion of pending request from LTSY->CTSY
	// when conference is hold and there is an active call.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	iMockLTSY.CompleteL(EMobileConferenceCallSwap, KErrGeneral);

	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C4: Successful completion request of
	// RMobileConferenceCall::Swap
	// when conference is hold and there is an active call.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileConferenceCall::Swap
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallSwap, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C3-2: Successful completion request of
	// RMobileConferenceCall::Swap
	// when conference is active and there is a hold call.
	// This time hold call become idle
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusIdle);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: swap conference without swap request and without ghost swap operation  
	// increase coverage
 	//-------------------------------------------------------------------------

	TRequestStatus statChangeStatus;
	RMobileConferenceCall::TMobileConferenceStatus status;
	conferenceCall.NotifyConferenceStatusChange(statChangeStatus, status);

	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected); // conference become active

	// completion of notify request
	User::WaitForRequest(statChangeStatus);
	ASSERT_EQUALS(KErrNone, statChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceActive, status);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: swap conference without swap request and without ghost swap operation  
	// this time conference is active
	// increase coverage
 	//-------------------------------------------------------------------------

	conferenceCall.NotifyConferenceStatusChange(statChangeStatus, status);

	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold); // conference become hold 

	// completion of notify request
	User::WaitForRequest(statChangeStatus);
	ASSERT_EQUALS(KErrNone, statChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceHold, status);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: make conference idle when there is requested a swap  
 	//-------------------------------------------------------------------------

	conferenceCall.NotifyConferenceStatusChange(statChangeStatus, status);

	data.Close();
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallResume, data);
	conferenceCall.Swap(reqStatus);
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);

	// completion of notify request
	User::WaitForRequest(statChangeStatus);
	ASSERT_EQUALS(KErrNone, statChangeStatus.Int());
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceIdle, status);

	// swap request completes with error
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrMMEtelCallTerminated, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this); // call4, call3, call2, call, line, conferenceCall, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::Swap
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::Swap
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestSwap0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::Swap
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data);

	// post request
	TRequestStatus reqStatus;
	conferenceCall.Swap(reqStatus);

	// cancel
	conferenceCall.CancelAsyncRequest(EMobileConferenceCallSwap);

	// invoke request completion from mockLtsy
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);

	User::WaitForRequest(reqStatus);
	// CTSY has no cancel for this ipc, so request completes with KErrNone
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); // call2, call, line, conferenceCall, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::Swap
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::Swap
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestSwap0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::Swap
 	//-------------------------------------------------------------------------

	// prepare and post 1st request
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data);

	// set calls on hold when request passes to Ltsy after delay
	data.Close();
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusHold;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData1(1, mobileService, callStatus);
	completeCallStatusData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	data.Close();
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData2(2, mobileService, callStatus);
	completeCallStatusData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	TRequestStatus reqStatus;
	conferenceCall.Swap(reqStatus);
	
	// prepare and post 2nd request
	data.Close();
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallResume, data);

	// connect calls when request passes to Ltsy after delay
	data.Close();
	callStatus = RMobileCall::EStatusConnected;
	completeCallStatusData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	data.Close();
	completeCallStatusData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	TRequestStatus reqStatus2;
	conferenceCall2.Swap(reqStatus2);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(9, this); // conferenceCall2, phone2, telServer2, call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::Swap with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::Swap and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestSwap0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileConferenceCall::Swap
 	//-------------------------------------------------------------------------

	TMockLtsyCallData0 emptyCallData1(1, RMobilePhone::EVoiceService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data);

	TRequestStatus reqStatus;
	conferenceCall.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); // call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COHU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::HangUp
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::HangUp
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestHangUp0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RArray<TInt> callsInConference;
	CleanupClosePushL(callsInConference);
	
	//-------------------------------------------------------------------------
	// TEST: failure to hangup not created conference call
 	//-------------------------------------------------------------------------
	
	TRequestStatus reqStatus;
	conferenceCall.HangUp(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrAccessDenied, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	callsInConference.AppendL(1); // call1
	callsInConference.AppendL(2); // call2
	
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp, KErrNotSupported);

	conferenceCall.HangUp(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	iMockLTSY.CompleteL(EMobileConferenceCallHangUp, KErrGeneral);

	conferenceCall.HangUp(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::CreateConference.
 	//-------------------------------------------------------------------------

	// add more calls to conference for coverage
	RCall call3;
	_LIT(KPhoneNumber3, "3333333330");
	MakeCallAndAddToConferenceLC(call3, line, conferenceCall, 3, KPhoneNumber3, callsInConference); 
	callsInConference.AppendL(3); // call3
	
	RCall call4;
	_LIT(KPhoneNumber4, "4444444440");   	
	MakeCallAndAddToConferenceLC(call4, line, conferenceCall, 4, KPhoneNumber4, callsInConference); 
	callsInConference.AppendL(4); // call4
	
	RCall call5;
	_LIT(KPhoneNumber5, "5555555550");   	
	MakeCallAndAddToConferenceLC(call5, line, conferenceCall, 5, KPhoneNumber5, callsInConference); 
	callsInConference.AppendL(5); // call5
	
	// create a hold call out of the conference
	RCall call6;
	OpenNewCallLC(call6, line);
	_LIT(KPhoneNumber6, "6666666660");   	
	DialL(call6, 6, RMobilePhone::EVoiceService, KPhoneNumber6);
	// Put the conference call (call1, call2, call3, call4 and call5) on hold
	ChangeCallStatusInOrderL(1, 5, RMobileCall::EStatusHold);
	// Connect call6 in right order
	ConnectAndPutCallOnHoldInRightOrderL(6);

	// now test begins
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	conferenceCall.HangUp(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusDisconnecting); // just for coverage
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	ChangeCallStatusL(2, RMobileCall::EStatusIdle);
	ChangeCallStatusL(3, RMobileCall::EStatusIdle);
	ChangeCallStatusL(4, RMobileCall::EStatusIdle);
	ChangeCallStatusL(5, RMobileCall::EStatusIdle);

	// hangup completes here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileConferenceCall::CreateConference
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// send completion
	iMockLTSY.CompleteL(EMobileConferenceCallHangUp, KErrNone);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	
    //
    // Added for DEF139341 (Unexpected error note pops up after creating emergency call)
    // Tests that CTSY treats KErrGsmReleaseByUser as a normal return code, not as an error
    // (the client request should complete with KErrNone).
    //

   // open conference object
    RMobileConferenceCall conferenceCall2;
    OpenConferenceLC(conferenceCall2, iPhone);
    
    RArray<TInt> callsInConference2;
    CleanupClosePushL(callsInConference2);
    
    RLine line2;
    RCall call21;
    RCall call22;
    CreateConferenceLC(iPhone, conferenceCall2, line2, call21, 21, KPhoneNumber1, call22, 22, KPhoneNumber2);
    callsInConference2.AppendL(21);
    callsInConference2.AppendL(22);
    
    iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
    iMockLTSY.CompleteL(EMobileConferenceCallHangUp, KErrGsmReleaseByUser);
    conferenceCall2.HangUp(reqStatus);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    
	
	CleanupStack::PopAndDestroy(15, this); // call22, call21, line2, callsInConference2, conferenceCall2, call6, call5, call4, call3, call2, call, line, callsInConference, conferenceCall, this
	
	}

/**
@SYMTestCaseID BA-CTSY-CONC-COHU-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for events related to RMobileConferenceCall::HangUp
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::HangUp. Checks events are triggered correctly. 
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestHangUp0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	RLine line;
	TRequestStatus reqStatus;
	TRequestStatus hangupStatus;
	RMobileConferenceCall::TMobileConferenceEvent whatStatus;
	TName callName;
	RCall::TCallInfo callInfo;

	// add calls to conference
	RCall call1;
	_LIT(KPhoneNumber1, "1111111110");
	RCall call2;
	_LIT(KPhoneNumber2, "22222222220");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call1, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	// now test begins
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	conferenceCall.NotifyConferenceEvent(hangupStatus, whatStatus, callName);
	conferenceCall.HangUp(reqStatus);
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	ChangeCallStatusL(2, RMobileCall::EStatusIdle);

	// hangup completes here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	//Three Events are to be expected. One EConferenceCallRemoved for each call 
	//in the conference and one EConferenceTerminated.
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, whatStatus);
	call1.GetInfo(callInfo);
	ASSERT_EQUALS(callInfo.iCallName, callName);
	
	conferenceCall.NotifyConferenceEvent(hangupStatus, whatStatus, callName);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceCallRemoved, whatStatus);
	call2.GetInfo(callInfo);
	ASSERT_EQUALS(callInfo.iCallName, callName);
	
	conferenceCall.NotifyConferenceEvent(hangupStatus, whatStatus, callName);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(RMobileConferenceCall::EConferenceTerminated, whatStatus);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call2, call1, line, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COHU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::HangUp
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::HangUp
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestHangUp0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RArray<TInt> callsInConference;
	CleanupClosePushL(callsInConference);
	
	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");
	_LIT(KPhoneNumber2, "1234567890");
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	callsInConference.AppendL(1); // call1
	callsInConference.AppendL(2); // call2
	
	// add more calls to conference for coverage issues
	RCall call3;
	_LIT(KPhoneNumber3, "3333333330");
	MakeCallAndAddToConferenceLC(call3, line, conferenceCall, 3, KPhoneNumber3, callsInConference); 
	callsInConference.AppendL(3); // call3
	
	RCall call4;
	_LIT(KPhoneNumber4, "4444444440");
	MakeCallAndAddToConferenceLC(call4, line, conferenceCall, 4, KPhoneNumber4, callsInConference); 
	callsInConference.AppendL(4); // call4
	
	RCall call5;
	_LIT(KPhoneNumber5, "5555555550");
	MakeCallAndAddToConferenceLC(call5, line, conferenceCall, 5, KPhoneNumber5, callsInConference); 
	callsInConference.AppendL(5); // call5
	
	// swap conference for coverage
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 emptyCallData1(1, mobileService);
	emptyCallData1.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallHold, data);
	TRequestStatus reqStatus;
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	ChangeCallStatusL(4, RMobileCall::EStatusHold);
	ChangeCallStatusL(5, RMobileCall::EStatusHold);

	// swap completes here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::HangUp
 	//-------------------------------------------------------------------------
 	
	// send request
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);
	conferenceCall.HangUp(reqStatus);
	
	// cancel
	conferenceCall.CancelAsyncRequest(EMobileConferenceCallHangUp);

	// mockLtsy completes request
	ChangeCallStatusL(1, RMobileCall::EStatusIdle);
	ChangeCallStatusL(2, RMobileCall::EStatusIdle);
	ChangeCallStatusL(3, RMobileCall::EStatusIdle);
	ChangeCallStatusL(4, RMobileCall::EStatusIdle);
	
	//Change call5 status to idle
	RMobileCall::TMobileCallStatus callStatus5 = RMobileCall::EStatusIdle;
	RMobilePhone::TMobileService mobileService5 = RMobilePhone::EVoiceService;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData5(5, mobileService5, callStatus5);
	data.Close();
	mockData5.SerialiseL(data);   
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);      
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
	 
	User::WaitForRequest(mockLtsyStatus); 	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());   	

	// hangup completes here
	User::WaitForRequest(reqStatus);
	// CTSY has no cancel for this ipc, so request completes with KErrNone
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(10, this); // call5, call4, call3, call2, call, line, callsInConference, conferenceCall, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COHU-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::HangUp
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::HangUp
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestHangUp0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// Open second client
	RTelServer telServer2;
	TInt res = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	res = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(phone2);

	// open conference object
	RMobileConferenceCall conferenceCall2;
	OpenConferenceLC(conferenceCall2, phone2);

	TRequestStatus reqStatus;
	TRequestStatus reqStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::HangUp
 	//-------------------------------------------------------------------------

	// prepare and send 1st request
	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);

	// complete request from mockLtsy when request passes to ltsy with delay
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusIdle;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData1(1, mobileService, callStatus);
	completeCallStatusData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	data.Close();
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData2(2, mobileService, callStatus);
	completeCallStatusData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	conferenceCall.HangUp(reqStatus);
	
	// send 2nd request
	conferenceCall2.HangUp(reqStatus2);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(9, this); // conferenceCall2, phone2, telServer2, call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COHU-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::HangUp with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::HangUp and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestHangUp0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileConferenceCall::HangUp
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallHangUp);

	TRequestStatus reqStatus;
	conferenceCall.HangUp(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COAC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::AddCall
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::AddCall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestAddCall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RArray<TInt> callsInConference;
	CleanupClosePushL(callsInConference);
	
	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall,iPhone);
	
	// open line for calls
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	OpenNewCallLC(call, line);
	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");   	
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	
	// create a call to add to the conference
	RCall call3;
	TName callName3;
	OpenNewCallLC(call3, line, &callName3);
	
	// create a hold call out of the conference (just for increase coverage)
	RCall call4;
	TName callName4;
	OpenNewCallLC(call4, line, &callName4);
	_LIT(KPhoneNumber4, "4444444440");   	
	DialL(call4, 4, RMobilePhone::EVoiceService, KPhoneNumber4);
	ChangeCallStatusL(4, RMobileCall::EStatusHold);
	
	// open second call
	RMobileCall call2;
	TName callName2;
	OpenNewCallLC(call2, line, &callName2);
	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");   	
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// TEST: add idle call
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	conferenceCall.AddCall(reqStatus, callName3);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: add call when conference is idle
 	//-------------------------------------------------------------------------

	// dial call3 (status hold)
	_LIT(KPhoneNumber3, "3333333330");   	
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	ConnectAndPutCallOnHoldInRightOrderL(3);

	// test
	conferenceCall.AddCall(reqStatus, callName3);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	conferenceCall.CreateConference(reqStatus);
	
	// connect first call
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	// update second call status
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// conference is created here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// test
	TMockLtsyCallData0 emptyCallDataServiceUnspecified3(3, RMobilePhone::EServiceUnspecified);
	emptyCallDataServiceUnspecified3.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data, KErrNotSupported);

	conferenceCall.AddCall(reqStatus, callName3);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);
	iMockLTSY.CompleteL(EMobileConferenceCallAddCall, KErrGeneral);

	conferenceCall.AddCall(reqStatus, callName3);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::AddCall.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);
    conferenceCall.AddCall(reqStatus, callName3);

    RMobileCall::TMobileCallEvent completeEvent(RMobileCall::ERemoteTerminated);
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TMockLtsyCallData1<RMobileCall::TMobileCallEvent> callEventData(2, mobileService, completeEvent);
	
    // this call event completion is just for increase coverage
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    data.Close();
    callEventData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // this complete is just for increase coverage
    ChangeCallStatusL(2, RMobileCall::EStatusIdle);

    // this complete is just for increase coverage
	ChangeCallStatusL(4, RMobileCall::EStatusIdle);

	// this complete is just for increase coverage
	ChangeCallStatusL(3, RMobileCall::EStatusHold);

	ChangeCallStatusL(3, RMobileCall::EStatusConnected);

	// now only call1 and call3 are in conference
	callsInConference.AppendL(1); // call1
	callsInConference.AppendL(3); // call3
	
	// add call request completes here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	TInt count;
	TInt res = conferenceCall.EnumerateCalls(count);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(2, count);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileConferenceCall::AddCall
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMobileConferenceCallAddCall, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
    //-------------------------------------------------------------------------
    // TEST F: Tests that a call that connect without connecting can be added to a conference.
    //-------------------------------------------------------------------------
    RCall callOnlyConnect; 
    TName callNameOnlyConnect;
    _LIT(KPhoneNumberOnlyConnect, "8888888881");
    ChangeConferenceCallStatusL(callsInConference, RMobileCall::EStatusHold);
    
    // Then open the call and dial
    OpenNewCallLC(callOnlyConnect, line, &callNameOnlyConnect);
    DialL(callOnlyConnect, 9, RMobilePhone::EVoiceService, KPhoneNumberOnlyConnect);
    // Apply only connected status changes
    ChangeCallStatusL(9, RMobileCall::EStatusConnected);

    data.Close();
    TMockLtsyCallData0 emptyCallDataServiceUnspecified9(9, RMobilePhone::EServiceUnspecified);
    emptyCallDataServiceUnspecified9.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data, KErrNone);
    iMockLTSY.CompleteL(EMobileConferenceCallAddCall, KErrNone);

    // Try to add the call to the conference. 
    conferenceCall.AddCall(reqStatus, callNameOnlyConnect);
    User::WaitForRequest(reqStatus);
    TInt ttt = reqStatus.Int();
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    ChangeConferenceCallStatusL(callsInConference, RMobileCall::EStatusConnected);
    
    AddCallL(conferenceCall, 9, callNameOnlyConnect, callsInConference);
    callsInConference.Append(9);
    
	//-------------------------------------------------------------------------
	// TEST G: Tests that a call with lack of KCapsJoin capability cannot be added to a conference.
	//-------------------------------------------------------------------------
	// call2 doesn't have join capability since it is in idle mode.
	conferenceCall.AddCall(reqStatus, callName2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
		
	// KCapsJoin capability is not assigned to a new call if 5 calls are present in an ongoing conference.
	RCall call6; // 4th call in the conference
	_LIT(KPhoneNumber6, "6666666660");
	MakeCallAndAddToConferenceLC(call6, line, conferenceCall, 6, KPhoneNumber6, callsInConference);
	callsInConference.AppendL(6); // call6
	
	RCall call7; // 5th call in the conference
	_LIT(KPhoneNumber7, "7777777770");
	MakeCallAndAddToConferenceLC(call7, line, conferenceCall, 7, KPhoneNumber7, callsInConference);
	callsInConference.AppendL(7); // call7
	
	RCall call8; // 6th call in the conference
	TName callName8;
	_LIT(KPhoneNumber8, "8888888880");
	// First put the conference call on hold
	ChangeConferenceCallStatusL(callsInConference, RMobileCall::EStatusHold);
	// Then open the call and dial
	OpenNewCallLC(call8, line, &callName8);
	DialL(call8, 8, RMobilePhone::EVoiceService, KPhoneNumber8);
	// Apply all expected status changes so that capabilities can be assigned correctly
	ChangeCallStatusL(8, RMobileCall::EStatusConnecting);
	ChangeCallStatusL(8, RMobileCall::EStatusConnected);
	
	// Try to add the 6th call to the conference. This operations should fail with KErrArgument as a conference can have maximum 5 members.
	conferenceCall.AddCall(reqStatus, callName8);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrMMEtelMaxReached, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(13, this); // call8, call7, call6, callOnlyConnect, call4, call3, call2, call, line, conferenceCall, callsInConference, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COAC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileConferenceCall::AddCall
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileConferenceCall::AddCall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestAddCall0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// open 3rd call
	RCall call3;
	TName callName3;
	OpenNewCallLC(call3, line, &callName3);

	// dial 3rd call (status hold)
	_LIT(KPhoneNumber3, "1632960000");
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	// put the conference call on hold
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusHold);
	ConnectAndPutCallOnHoldInRightOrderL(3);
	// make the conference call active
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileConferenceCall::AddCall
 	//-------------------------------------------------------------------------
 	
	// send request
	TMockLtsyCallData0 emptyCallDataServiceUnspecified3(3, RMobilePhone::EServiceUnspecified);
	emptyCallDataServiceUnspecified3.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);
	TRequestStatus reqStatus;
	conferenceCall.AddCall(reqStatus, callName3);
	
	// cancel
	conferenceCall.CancelAsyncRequest(EMobileConferenceCallAddCall);

	// mockLtsy completes request
	ChangeCallStatusL(3, RMobileCall::EStatusConnected);

	// check results
	User::WaitForRequest(reqStatus);
	// there is no cancel for this ipc in ctsy
	// so status is KErrNone
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // call3, call2, call, line, conferenceCall, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-COAC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::AddCall with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::AddCall with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestAddCall0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobileConferenceCall::AddCall
 	//-------------------------------------------------------------------------

	TName name(KNullDesC);
	
	TRequestStatus reqStatus;
	conferenceCall.AddCall(reqStatus, name);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // conferenceCall, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COAC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileConferenceCall::AddCall
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileConferenceCall::AddCall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestAddCall0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);
	
	// open 3rd call
	RCall call3;
	TName callName3;
	OpenNewCallLC(call3, line, &callName3);

	// dial 3rd call (status hold)
	_LIT(KPhoneNumber3, "1632960000");
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	// put the conference call on hold
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusHold);
	ConnectAndPutCallOnHoldInRightOrderL(3);
	// make the conference call active
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusConnected);
	
	// ------------------- Open second client ------------------------
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// open new line for calls
	RLine secLine;
	TInt res = secLine.Open(phone2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(secLine);
	
	// open call
	RCall secCall;
	TName secCallName;
	OpenNewCallLC(secCall, secLine, &secCallName);

	// dial call
	_LIT(KSecPhoneNumber1, "1632960000");
	DialL(secCall, 4, RMobilePhone::EVoiceService, KSecPhoneNumber1);
	ChangeCallStatusL(4, RMobileCall::EStatusConnecting);
	ChangeCallStatusL(4, RMobileCall::EStatusConnected);

	// open conference object
	RMobileConferenceCall secConferenceCall;
	OpenConferenceLC(secConferenceCall, phone2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileConferenceCall::AddCall
 	//-------------------------------------------------------------------------

	// first request
	TMockLtsyCallData0 emptyCallDataServiceUnspecified3(3, RMobilePhone::EServiceUnspecified);
	emptyCallDataServiceUnspecified3.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);

	data.Close();
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData3(3, RMobilePhone::EVoiceService, callStatus);
	completeCallStatusData3.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	TRequestStatus reqStatus;
	conferenceCall.AddCall(reqStatus, callName3);

	// second request
	TMockLtsyCallData0 emptyCallDataServiceUnspecified4(4, RMobilePhone::EServiceUnspecified);
	data.Close();
	emptyCallDataServiceUnspecified4.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);

	data.Close();
	RMobileCall::TMobileCallStatus secCallStatus = RMobileCall::EStatusConnected;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> secCompleteCallStatusData1(4, RMobilePhone::EVoiceService, secCallStatus);
	secCompleteCallStatusData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

	TRequestStatus secReqStatus;
	secConferenceCall.AddCall(secReqStatus, secCallName);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(secReqStatus);
	ASSERT_EQUALS(KErrNone, secReqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(12, this); // secConferenceCall, secCall, secLine, phone2, telServer2, call3, call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COAC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::AddCall with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::AddCall and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestAddCall0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// open conference object
	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	// open 3rd call
	RCall call3;
	TName callName3;
	OpenNewCallLC(call3, line, &callName3);

	// dial 3rd call (status hold)
	_LIT(KPhoneNumber3, "1632960000");
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	// put the conference call on hold
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusHold);
	ConnectAndPutCallOnHoldInRightOrderL(3);
	// make the conference call active
	ChangeCallStatusInOrderL(1, 2, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileConferenceCall::AddCall
 	//-------------------------------------------------------------------------

	TMockLtsyCallData0 emptyCallDataServiceUnspecified3(3, RMobilePhone::EServiceUnspecified);
	emptyCallDataServiceUnspecified3.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);

	TRequestStatus reqStatus;
	conferenceCall.AddCall(reqStatus, callName3);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this); // call3, call2, call, line, conferenceCall, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CONC-COGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::GetCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::GetCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGetCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileConferenceCall::GetCaps.
 	//-------------------------------------------------------------------------

	TUint32 caps;
	TInt res = conferenceCall.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(static_cast<TUint32>(0), caps);

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// RMobileConferenceCall::GetCaps with other Caps value.
 	//-------------------------------------------------------------------------

	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");   	
	_LIT(KPhoneNumber2, "1234567890");   	
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	res = conferenceCall.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(static_cast<TUint32>(RMobileConferenceCall::KCapsSwap |
            RMobileConferenceCall::KCapsHangUp), caps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGMCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::GetMobileCallInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::GetMobileCallInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGetMobileCallInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);
	
	//-------------------------------------------------------------------------
	// TEST: Send request of RMobileConferenceCall::GetMobileCallInfo
	// when conference is idle
 	//-------------------------------------------------------------------------
	
	RMobileCall::TMobileCallInfoV1 info; 
	RMobileCall::TMobileCallInfoV1Pckg infoPckg(info); 
	TInt res = conferenceCall.GetMobileCallInfo(0, infoPckg);
	ASSERT_EQUALS(KErrNotReady, res);
	
	//-------------------------------------------------------------------------
	// TEST C1: Successful completion request of
	// RMobileConferenceCall::GetMobileCallInfo
	// when conference is active
 	//-------------------------------------------------------------------------

	// create conference
	RLine line;
	OpenLineLC(line);

	// open first call
	RCall call;
	TName callName1;
	OpenNewCallLC(call, line, &callName1);
	// dial first call (status hold)
	_LIT(KPhoneNumber1, "1357924680");   	
	DialL(call, 1, RMobilePhone::EVoiceService, KPhoneNumber1);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);

	// create a hold call out of the conference (just for increase coverage)
	RCall call3;
	OpenNewCallLC(call3, line);
	_LIT(KPhoneNumber3, "3333333330");   	
	DialL(call3, 3, RMobilePhone::EVoiceService, KPhoneNumber3);
	ChangeCallStatusL(3, RMobileCall::EStatusHold);
	
	// open second call
	RCall call2;
	TName callName2;
	OpenNewCallLC(call2, line, &callName2);
	// dial second call (status connected)
	_LIT(KPhoneNumber2, "1234567890");   	
	DialL(call2, 2, RMobilePhone::EVoiceService, KPhoneNumber2);
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	conferenceCall.CreateConference(reqStatus);
	
	// connect first call
	ChangeCallStatusL(1, RMobileCall::EStatusConnected);
	// update second call status
	ChangeCallStatusL(2, RMobileCall::EStatusConnected);

	// conference is created here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	
	sleep(2);
	
	// test (get info for 2nd call (index = 1))
	res = conferenceCall.GetMobileCallInfo(1, infoPckg);
	ASSERT_EQUALS(KErrNone, res);
    ASSERT_EQUALS(static_cast<TUint32>(RMobileCall::KCallStartTime | 
	                                   RMobileCall::KCallDuration | 
	                                   RMobileCall::KCallId | 
	                                   RMobileCall::KCallExitCode | 
	                                   RMobileCall::KCallEmergency |
	                                   RMobileCall::KCallRemoteParty |
	                                   RMobileCall::KCallDialledParty |
	                                   RMobileCall::KCallAlternating), info.iValid);
    ASSERT_EQUALS(callName2, info.iCallName);
    ASSERT_TRUE( 0 == KMmTsyVoice1LineName().Compare(info.iLineName) );
    ASSERT_EQUALS(RMobileCall::EStatusConnected, info.iStatus);
    const TDateTime KZeroTime(0,TMonth(0),0,0,0,0,0);
    ASSERT_EQUALS(0, memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
    ASSERT_TRUE( TTimeIntervalSeconds(0) == info.iDuration );
    ASSERT_EQUALS(2, info.iCallId);
    ASSERT_EQUALS(0, info.iExitCode);
    ASSERT_EQUALS(0, info.iEmergency);
    ASSERT_EQUALS(RMobilePhone::EAlternatingModeUnspecified, info.iAlternatingCall);
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityUnknown, info.iRemoteParty.iRemoteIdStatus);
    ASSERT_EQUALS(RMobileCall::EMobileOriginated, info.iRemoteParty.iDirection);
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteParty.iRemoteNumber.iNumberPlan);
    ASSERT_EQUALS(0, info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
    ASSERT_EQUALS(0, info.iRemoteParty.iCallingName.Size());
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iDialledParty.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iDialledParty.iNumberPlan);
    ASSERT_TRUE( 0 == KPhoneNumber2().Compare(info.iDialledParty.iTelNumber) );
    ASSERT_EQUALS(RMobilePhone::EVoiceService, info.iService);
	AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMobileConferenceCall::GetMobileCallInfo
	// when conference is hold
	//-------------------------------------------------------------------------

	// swap conference 
	iMockLTSY.ExpectL(EMobileConferenceCallSwap);
	conferenceCall.Swap(reqStatus);

	ChangeCallStatusL(3, RMobileCall::EStatusConnected);
	ChangeCallStatusL(1, RMobileCall::EStatusHold);
	ChangeCallStatusL(2, RMobileCall::EStatusHold);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// test (get info for 1st call (index = 0))
	res = conferenceCall.GetMobileCallInfo(0, infoPckg);
	ASSERT_EQUALS(KErrNone, res);
    ASSERT_EQUALS(static_cast<TUint32>(RMobileCall::KCallStartTime | 
	                                   RMobileCall::KCallDuration | 
	                                   RMobileCall::KCallId | 
	                                   RMobileCall::KCallExitCode | 
	                                   RMobileCall::KCallEmergency |
	                                   RMobileCall::KCallRemoteParty |
	                                   RMobileCall::KCallDialledParty |
	                                   RMobileCall::KCallAlternating), info.iValid);
    ASSERT_EQUALS(callName1, info.iCallName);
    ASSERT_TRUE( 0 == KMmTsyVoice1LineName().Compare(info.iLineName) );
    ASSERT_EQUALS(RMobileCall::EStatusHold, info.iStatus);
    ASSERT_EQUALS(0, memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
    ASSERT_TRUE( TTimeIntervalSeconds(0) == info.iDuration );
    ASSERT_EQUALS(1, info.iCallId);
    ASSERT_EQUALS(0, info.iExitCode);
    ASSERT_EQUALS(0, info.iEmergency);
    ASSERT_EQUALS(RMobilePhone::EAlternatingModeUnspecified, info.iAlternatingCall);
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityUnknown, info.iRemoteParty.iRemoteIdStatus);
    ASSERT_EQUALS(RMobileCall::EMobileOriginated, info.iRemoteParty.iDirection);
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteParty.iRemoteNumber.iNumberPlan);
    ASSERT_EQUALS(0, info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
    ASSERT_EQUALS(0, info.iRemoteParty.iCallingName.Size());
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iDialledParty.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iDialledParty.iNumberPlan);
    ASSERT_TRUE( 0 == KPhoneNumber1().Compare(info.iDialledParty.iTelNumber) );
    ASSERT_EQUALS(RMobilePhone::EVoiceService, info.iService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // call2, call3, call, line, conferenceCall, this
	}


/**
@SYMTestCaseID BA-CTSY-CONC-CGMCI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileConferenceCall::GetMobileCallInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileConferenceCall::GetMobileCallInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyConferenceCallControlFU::TestGetMobileCallInfo0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileConferenceCall conferenceCall;
	OpenConferenceLC(conferenceCall, iPhone);

	// create conference
	RLine line;
	RCall call;
	RCall call2;
	_LIT(KPhoneNumber1, "1357924680");
	_LIT(KPhoneNumber2, "1234567890");
	CreateConferenceLC(iPhone, conferenceCall, line, call, 1, KPhoneNumber1, call2, 2, KPhoneNumber2);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobileConferenceCall::GetMobileCallInfo
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallInfoV1 info; 
	RMobileCall::TMobileCallInfoV1Pckg infoPckg(info); 
	TInt res = conferenceCall.GetMobileCallInfo(2, infoPckg); //wrong index
	ASSERT_EQUALS(KErrNotFound, res);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileConferenceCall::GetMobileCallInfo
 	//-------------------------------------------------------------------------

	TBuf8<1> smallSizeBuf;
	res = conferenceCall.GetMobileCallInfo(0, smallSizeBuf);
    ASSERT_TRUE(KErrNone != res);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call2, call, line, conferenceCall, this

	}


void CCTsyConferenceCallControlFU::CreateConferenceLC(RPhone& aPhone,
		RMobileConferenceCall& aConferenceCall,
		RLine& aLine,
		RCall& aCall,
		TInt aCallId1,
		const TDesC &aTelNumber1,
		RCall& aCall2,
		TInt aCallId2,
		const TDesC &aTelNumber2)
	{

	TInt errorCode = aLine.Open(aPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode)
	CleanupClosePushL(aLine);

	// open first call
	OpenNewCallLC(aCall, aLine);

	// dial first call (status hold)
	DialL(aCall, aCallId1, RMobilePhone::EVoiceService, aTelNumber1);
	ChangeCallStatusL(aCallId1, RMobileCall::EStatusHold);
	
	// open second call
	OpenNewCallLC(aCall2, aLine);

	// dial second call (status connected)
	DialL(aCall2, aCallId2, RMobilePhone::EVoiceService, aTelNumber2);
	ChangeCallStatusL(aCallId2, RMobileCall::EStatusConnected);

	// create conference
	iMockLTSY.ExpectL(EMobileConferenceCallCreateConference);
	TRequestStatus reqStatus;
	aConferenceCall.CreateConference(reqStatus);
	
	// connect first call
	ChangeCallStatusL(aCallId1, RMobileCall::EStatusConnected);
	// update second call status
	ChangeCallStatusL(aCallId2, RMobileCall::EStatusConnected);

	// conference is created here
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

	}

void CCTsyConferenceCallControlFU::OpenConferenceLC(RMobileConferenceCall& aConferenceCall, RMobilePhone& aPhone)
	{
	TInt res = aConferenceCall.Open(aPhone);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(aConferenceCall);
	}

void CCTsyConferenceCallControlFU::OpenLineLC(RLine& aLine, const TDesC& aName)
	{
	CCtsyComponentTestBase::OpenLineLC(aLine, aName);
	}

void CCTsyConferenceCallControlFU::OpenNewCallLC(RCall& aCall, RLine& aLine, TDes* aCallName)
	{
	TInt res;
	if (aCallName)
		{
		res = aCall.OpenNewCall(aLine, *aCallName);
		}
	else
		{
		res = aCall.OpenNewCall(aLine);
		}
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(aCall);
	}

void CCTsyConferenceCallControlFU::ChangeConferenceCallStatusL(const RArray<TInt>& aCallsInConference, RMobileCall::TMobileCallStatus aCallStatus)
	{
	TInt count = aCallsInConference.Count();
	
	for(TInt i=0; i<count; ++i)
		{
		ChangeCallStatusL(aCallsInConference[i], aCallStatus);
		}
	}

void CCTsyConferenceCallControlFU::MakeCallAndAddToConferenceLC(
		RCall& aCall,
		RLine& aLine,
		RMobileConferenceCall& aConferenceCall,
		TInt aCallId,
		const TDesC &aTelNumber,
		const RArray<TInt>& aCallsInConference)
	{
	// First put the conference call on hold
	ChangeConferenceCallStatusL(aCallsInConference, RMobileCall::EStatusHold);

	TName callName;
	OpenNewCallLC(aCall, aLine, &callName);
	DialL(aCall, aCallId, RMobilePhone::EVoiceService, aTelNumber);
	// Apply all expected status changes so that capabilities can be assigned correctly
	ChangeCallStatusL(aCallId, RMobileCall::EStatusConnecting);
	ChangeCallStatusL(aCallId, RMobileCall::EStatusConnected);
	AddCallL(aConferenceCall, aCallId, callName, aCallsInConference);
	}

void CCTsyConferenceCallControlFU::ChangeCallStatusL(TInt aCallId,
		RMobileCall::TMobileCallStatus aCallStatus)
	{
	CCtsyComponentTestBase::ChangeCallStatusL(aCallId, RMobilePhone::EVoiceService, aCallStatus);
	}

void CCTsyConferenceCallControlFU::ChangeCallStatusInOrderL(TInt aStartCallId, TInt aEndCallId, RMobileCall::TMobileCallStatus aCallStatus)
	{
	for(TInt i=aStartCallId; i<=aEndCallId; ++i)
		{
		ChangeCallStatusL(i, aCallStatus);
		}
	}

void CCTsyConferenceCallControlFU::ConnectAndPutCallOnHoldInRightOrderL(TInt aCallId)
	{
	// a call cannot go from Idle state to Hold state directly
	// change the status of call in order below so that right capabilities can be assigned
	ChangeCallStatusL(aCallId, RMobileCall::EStatusConnecting);
	ChangeCallStatusL(aCallId, RMobileCall::EStatusConnected);
	ChangeCallStatusL(aCallId, RMobileCall::EStatusHold);
	}

void CCTsyConferenceCallControlFU::AddCallL(RMobileConferenceCall& aConferenceCall,
	                                        TInt aCallId,
	                                        const TName& aCallName,
	                                        const RArray<TInt>& aCallsInConference)
	{

	// prepare add call request
	RBuf8 data;
	data.CleanupClosePushL();

	TMockLtsyCallData0 emptyCallDataServiceUnspecified(aCallId, RMobilePhone::EServiceUnspecified);
	emptyCallDataServiceUnspecified.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileConferenceCallAddCall, data);

	CleanupStack::PopAndDestroy(&data);

	// send request
	TRequestStatus reqStatus;
	aConferenceCall.AddCall(reqStatus, aCallName);
	
	// First make the conference call active
	ChangeConferenceCallStatusL(aCallsInConference, RMobileCall::EStatusConnected);
	
	// request is now completed
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

	}
