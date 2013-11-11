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
// The TEFUnit test suite for CallControl in the Common TSY.
// 
//

/**
 @file      
*/

#include "cctsycallcontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <etelmmerr.h>
#include "MmTsy_timeoutdefs.h"

CTestSuite* CCTsyCallControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCall00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006dL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006eL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006fL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006gL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006hL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0006iL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0009bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial0009cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDial00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00011bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00011cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHangUp00015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestHold0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSwap0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSwap0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSwap0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSwap0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSwap0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSwap00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestResume0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestResume0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestResume0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestResume0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestResume0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestResume00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDeflect0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDeflect0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDeflect0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTransfer00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateAllCalls0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateAllCalls0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateAllCalls0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateAllCalls0005L);	
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateActiveCalls0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateActiveCalls0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateActiveCalls0001cL);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateActiveCalls0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateActiveCalls0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestTerminateActiveCalls0005L);  
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestSetIncomingCallType0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCallISV0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCallISV0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCallISV0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCallISV0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCallISV0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestAnswerIncomingCallISV00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDialISV0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDialISV0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDialISV00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDialCallControl0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDialCallControl0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestDialCallControl00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestEnumerateCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestEnumerateCall0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestEnumerateCall00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestNotifyHookChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestNotifyHookChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestNotifyHookChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestCompleteNotifyStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUpdateLifeTime0001L);	

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AnswerIncomingCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AnswerIncomingCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
   
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData, KErrNotSupported);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrGeneral, expectData);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::AnswerIncomingCall when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::AnswerIncomingCall
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST : Call AnswerIncomingCall with null description
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    
    TPtr8 null(NULL,0);
    call.AnswerIncomingCall(requestStatus, null);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::AnswerIncomingCall coverage
	// covers "if ( 0 < answerCallHandle )" condition
 	//-------------------------------------------------------------------------

	RCall call2;
	CleanupClosePushL(call2);
    TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    call2.AnswerIncomingCall(requestStatus2, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call.Close();

	CloseIncomingCallL(call2, callId, mobileService);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::AnswerIncomingCall coverage
	// covers "else if( activeCall && 
	//                ( !( KETelExt3rdPartyV1 == activeCall->GetExtensionId() ) &&
	//                ( KETelExt3rdPartyV1 == extensionId ) ) )" condition
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusRinging;

	// create new incoming call1
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
	RMobileCall call1;
	CleanupClosePushL(call1);
    errorCode = call1.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------

 	TName incomingCallName2;
 	TInt callId2 = 2;
	mobileCallStatus = RMobileCall::EStatusConnected;
 	
	// create new incoming call2
	errorCode = CreateIncomingCallL(line, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	call2.AnswerIncomingCall(requestStatus2, mmParamsPckgV1);

	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> callParamsPckg(callParams);
	call1.AnswerIncomingCallISV(requestStatus, callParamsPckg);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGsmNotAllowed, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNotSupported, requestStatus2.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // expectData, completeData, call, line, call2
										  // this, call1 
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::AnswerIncomingCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::AnswerIncomingCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
 	
	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    expectData.Close();
    mockData0.SerialiseL(expectData);

    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrCancel, expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    expectData.Close();                                             
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

    call.CancelAsyncRequest(EEtelCallAnswer);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(4); // this, expectData, call, line
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::AnswerIncomingCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::AnswerIncomingCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RCall call1;
	CleanupClosePushL(call1);

	RCall call2;
	CleanupClosePushL(call2);

	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TName incomingCallName1;

	TInt callId2 = 2;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData01(callId1, mobileService);
  
    expectData.Close();
    mockData01.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    call1.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);

    TMockLtsyCallData0 mockData02(callId2, mobileService);
    expectData.Close();
    mockData02.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    call2.AnswerIncomingCall(requestStatus2, mmParamsPckgV1);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	// close incoming call1
    CloseIncomingCallL(call1, callId1, mobileService);
	AssertMockLtsyStatusL();

	// close incoming call2
    CloseIncomingCallL(call2, callId2, mobileService);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test for increase coverage
 	//-------------------------------------------------------------------------

	line1.NotifyIncomingCall(requestStatus1, incomingCallName1);   
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	
	mobileCallStatus = RMobileCall::EStatusConnected;
	
 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusHold;
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    call1.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGsmNotAllowed, requestStatus1.Int());

 	//-------------------------------------------------------------------------

	// Done !
	CleanupStack::PopAndDestroy(4); // line1, line2, call1, call2
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, expectData, this, completeData

	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AnswerIncomingCall with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AnswerIncomingCall and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call

	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AnswerIncomingCall for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AnswerIncomingCall for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData, KErrNotSupported);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrGeneral, expectData);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::AnswerIncomingCall when result is not cached.
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::AnswerIncomingCall
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmDataCallTsy::AnswerIncomingCall coverage
	// covers "if ( 0 < iAnswerCallHandle )" condition
 	//-------------------------------------------------------------------------

	RCall call2;
	CleanupClosePushL(call2);
    TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    RMobileCall::TMobileCallParamsV1 callParamsV2;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV2(callParamsV2);
	call2.AnswerIncomingCall(requestStatus2, mmParamsPckgV2);

	iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmDataCallTsy::AnswerIncomingCall coverage
	// covers "if ( iCallCaps.iFlags & RCall::KCapsAnswer || 
	//              RCall::EStatusIdle == iCallStatus )" condition
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
    // call CMmDataCallTsy::CompleteNotifyStatusChange for set 
    // iCallCaps.iFlags &= ~( RCall::KCapsAnswer );
    
    mobileCallStatus = RMobileCall::EStatusIdle;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-------------------------------------------------------------------------
    // call CMmVoiceLineTsy::CompleteNotifyIncomingCall for set iCallId = 1.
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // call2, line
	
	CloseIncomingCallL(call, callId, mobileService);
	CleanupStack::PopAndDestroy(3); // expectData, call, completeData

	CleanupStack::PopAndDestroy(1, this);
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::AnswerIncomingCall for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::AnswerIncomingCall for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
 	
	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TMockLtsyCallData0 mockData0(callId, mobileService);
  
    expectData.Close();
    mockData0.SerialiseL(expectData);

    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrCancel, expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

    call.CancelAsyncRequest(EEtelCallAnswer);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(4); // this, expectData, call, line
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AnswerIncomingCall with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AnswerIncomingCall with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RCall call;
	CleanupClosePushL(call);
	
	// open new line for KMmTsyVoice1LineName
	RLine line;
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::Dial
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
   
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    
    TPtr8 null(NULL,0);
    call.AnswerIncomingCall(requestStatus, null);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::AnswerIncomingCall for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::AnswerIncomingCall for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RCall call1;
	CleanupClosePushL(call1);

	RCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TName incomingCallName1;

	TInt callId2 = 2;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData01(callId1, mobileService);
  
    expectData.Close();
    mockData01.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call1.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);

    TMockLtsyCallData0 mockData02(callId2, mobileService);
    expectData.Close();
    mockData02.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV2;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV2(callParamsV2);
    call2.AnswerIncomingCall(requestStatus2, mmParamsPckgV2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(4); // line1, line2, call1, call2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AnswerIncomingCall with timeout for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AnswerIncomingCall and tests for timeout for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall00010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
 
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST : Call AnswerIncomingCall with null description
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, expectData);
    
    TPtr8 null(NULL,0);
    call.AnswerIncomingCall(requestStatus, null);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAIC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AnswerIncomingCall for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AnswerIncomingCall for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCall00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyFaxLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
   
    expectData.Close();
    mockData0.SerialiseL(expectData);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, call, line
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    TName name;
	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
			
	RMobileCall::TMobileCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);

	RCall::TCallParams callParamsX;
	TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
	callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParamsX.iInterval        = 100;
	callParamsX.iWaitForDialTone = RCall::EDialToneWait;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   
    expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	// Checking that we can return a specific error - KErrGsmCSConnectionBarred 
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGsmCSConnectionBarred, completeData);

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmCSConnectionBarred, requestStatus.Int());
	
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::Dial when result is not cached.
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::Dial
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to check that RCall::Dial works fine with RMobileCall::TMobileCallParamsV1
 	//-------------------------------------------------------------------------
	// open new call
	CleanupClosePushL(call);
	errorCode = call.OpenNewCall(line, name);
	ASSERT_EQUALS(KErrNone, errorCode);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	RMobileCall::TMobileCallParamsV1Pckg callParamsV1Pkg(callParamsV1);
	callParamsV1.iCug.iCugIndex = 0xFFFF;
	callParamsV1.iSpeakerControl  = RCall::EMonitorSpeakerControlOnExceptDuringDialling;
	callParamsV1.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeOff;
	callParamsV1.iInterval        = 100;
	callParamsV1.iWaitForDialTone = RCall::EDialToneNoWait;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockDataV1(0, mobileService, callParamsV1, callInfo);

	expectData.Close();
	mockDataV1.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
		
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
		
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	call.Dial(requestStatus, callParamsV1Pkg, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	RMobileCall::TMobileCallParamsV1 checkParams;
	RMobileCall::TMobileCallParamsV1Pckg checkParamsPkg(checkParams);
	call.GetCallParams(checkParamsPkg);
	
	TInt compareResult = ((checkParams.iSpeakerControl !=  callParamsV1.iSpeakerControl) ||
			(checkParams.iSpeakerVolume != callParamsV1.iSpeakerVolume) ||
			(checkParams.iInterval != callParamsV1.iInterval) ||
			(checkParams.iWaitForDialTone != callParamsV1.iWaitForDialTone) ||
			(checkParams.iCug.iCugIndex != callParamsV1.iCug.iCugIndex)); 
	
	if(KErrNone != compareResult)	
		{
		ERR_PRINTF1(_L("RCall::GetCallParams did not return the same params as that used for Dial"));
		ASSERT_EQUALS(KErrNone, compareResult);
		}
	
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST : to check that RCall::Dial works fine with RMobileCall::TMobileCallParamsV2
	//-------------------------------------------------------------------------
	// open new call
	CleanupClosePushL(call);
	errorCode = call.OpenNewCall(line, name);
	ASSERT_EQUALS(KErrNone, errorCode);

	RMobileCall::TMobileCallParamsV2 callParamsV2;
	RMobileCall::TMobileCallParamsV2Pckg callParamsV2Pkg(callParamsV2);
	callParamsV2.iBearerMode =  RMobileCall::EMulticallNewBearer;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV2, RMobileCall::TMobileCallInfoV8> 
			mockDataV2(0, mobileService, callParamsV2, callInfo);

	expectData.Close();
	mockDataV2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
			
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
			
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	call.Dial(requestStatus, callParamsV2Pkg, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	RMobileCall::TMobileCallParamsV2 checkParamsV2;
	RMobileCall::TMobileCallParamsV2Pckg checkParamsV2Pkg(checkParamsV2);
	call.GetCallParams(checkParamsV2Pkg);
		
	if(KErrNone != compareResult)	
		{
		ERR_PRINTF1(_L("RCall::GetCallParams did not return the same V2 params as that used for Dial"));
		ASSERT_EQUALS(KErrNone, compareResult);
		}
		
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "else if ( RMobileCall::EStatusIdle != iMobileCallStatus )" condition
 	//-------------------------------------------------------------------------

	TName incomingCallName;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

    CloseIncomingCallL(call, callId, mobileService);
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "else if ( ( activeCall ) && ( KETelExt3rdPartyV1 == extensionId ) &&
	//         ( KETelExt3rdPartyV1 != activeCall->GetExtensionId() ) )" condition
 	//-------------------------------------------------------------------------

	RMobileCall mobileCall;
    mobileCallStatus = RMobileCall::EStatusConnected;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(mobileCall);
   	errorCode = mobileCall.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParamsXX;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> pckgCallParamsXX(callParamsXX);
	mobileCall.DialISV(requestStatus, pckgCallParamsXX, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus.Int());

    call.Close();
	AssertMockLtsyStatusL();
    mobileCall.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "else" condition in case of 
	// impossible of conditions "if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )",
	// "else if ( RMobileCall::EStatusIdle != iMobileCallStatus )",
	// "else if ( ( 0 < dialHandle ) || ( ( KETelExt3rdPartyV1 == extensionId )
	// && ( 0 < dialHandleISV ) && !i3rdPartyEmergencyNumberCheckDone  ) )",
	// "else if ( ( activeCall ) && ( KETelExt3rdPartyV1 == extensionId )
	// && ( KETelExt3rdPartyV1 != activeCall->GetExtensionId() ) )" and
	// "else if( extensionId == KETelExt3rdPartyV1 && 
	// !i3rdPartyEmergencyNumberCheckDone )".
 	//-------------------------------------------------------------------------

	RCall call2;
    mobileCallStatus = RMobileCall::EStatusConnected;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);
	call2.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

    call.Close();
	AssertMockLtsyStatusL();
    call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )" condition
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TRfStateInfo rfInfo = ERfsStateInfoInactive;
	TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
	
    expectData.Close();
    mockData1.SerialiseL(expectData);
    
	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmOfflineOpNotAllowed, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for voice calls with minimal parameters (RCall::TCallParams)
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0001bL()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt errorCode = KErrNone;    

    //-- For Voice1 -------------------------

    TBuf<256> lineName(KMmTsyVoice1LineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);

    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   

    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");   

    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expectedCallId = 0;

    RMobileCall::TMobileCallParamsV7 callParams; 
    RMobileCall::TMobileCallParamsV7Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV8 callInfo;   
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval        = 100;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    

    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   

    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8 >
    mockCallData(expectedCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    TInt callId = 1;
    
    completeData.Close();
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    TMockLtsyCallData0 mockDataComplete(callId, mobileService);
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    RCall::TCallParams callParamsX;
    callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParamsX.iInterval        = 100;
    callParamsX.iWaitForDialTone = RCall::EDialToneWait;
    TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
        
    call.Dial(requestStatus, pckgCallParamsX, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    CleanupStack::PopAndDestroy(5,this);
    
    }


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for voice calls with changing parameters
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0001cL()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt errorCode = KErrNone;    

    //-- For Voice1 -------------------------

    TBuf<256> lineName(KMmTsyVoice1LineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);

    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   

    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");   

    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expectedCallId = 0;
    RMobileCall::TMobileCallInfoV8 callInfo;
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    
    // A. Call with V7 params.
    // Fill the params with random values
    RMobileCall::TMobileCallParamsV7 callParams;
    RMobileCall::TMobileCallParamsV7Pckg    pckgCallParams(callParams);
    callParams.iAlphaId.Copy(_L("Alpha Id"));
    callParams.iIconId.iQualifier = RMobileCall::ESelfExplanatory;
    callParams.iIconId.iIdentifier = 0x0A;
    callParams.iBCRepeatIndicator = RMobileCall::EBCServiceChangeAndFallbackMode;
    callParams.iBearerCap2.Copy(_L("Cap2"));
    callParams.iBearerCap1.Copy(_L("Cap1"));
    callParams.iSubAddress.Copy(_L("SubAddress"));
    callParams.iCallParamOrigin = RMobileCall::EOriginatorEtelClient;
    callParams.iBearerMode = RMobileCall::EMulticallShareBearer;
    callParams.iIdRestrict = RMobileCall::EDontSendMyId;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xAABB;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = ETrue;
    callParams.iAutoRedial = ETrue;
    callParams.iSpeakerControl = RCall::EMonitorSpeakerControlOnUntilCarrier; 
    callParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval = 0;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8 >
    mockCallData(expectedCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    TInt callId = 1;
    
    completeData.Close();
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    TMockLtsyCallData0 mockDataComplete(callId, mobileService);
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    call.Dial(requestStatus, pckgCallParams, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    // B. Call with V0 params. 
    // The CTSY should send the default V7 values to the LTSY, and not the previous call parameters which are irrelevant.  
    // Fill the params with other random values.
    callParams.iAlphaId.Zero();
    callParams.iIconId.iQualifier = RMobileCall::EIconQualifierNotSet;
    callParams.iIconId.iIdentifier = 0x00;
    callParams.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
    callParams.iBearerCap2.Zero();
    callParams.iBearerCap1.Zero();
    callParams.iSubAddress.Zero();
    callParams.iCallParamOrigin = RMobileCall::EOriginatorUnknown;
    callParams.iBearerMode = RMobileCall::EMulticallNotSupported;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;
    callParams.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOn; 
    callParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    callParams.iInterval = 12;
    callParams.iWaitForDialTone = RCall::EDialToneNoWait;
    

    RMobileCall::TCallParams callParams0;
    callParams0.iSpeakerControl = callParams.iSpeakerControl; 
    callParams0.iSpeakerVolume = callParams.iSpeakerVolume;
    callParams0.iInterval = callParams.iInterval;
    callParams0.iWaitForDialTone = callParams.iWaitForDialTone; 
    RCall::TCallParamsPckg   pckgCallParams0(callParams0);
    // We should expect the appropriate Alpha ID and Icon ID
    callInfo.iValid |= (RMobileCall::KCallAlphaId | RMobileCall::KCallIconId);    
    callInfo.iAlphaId.Copy(_L("Alpha Id"));
    callInfo.iIconId.iQualifier = RMobileCall::ESelfExplanatory;
    callInfo.iIconId.iIdentifier = 0x0A;

    expectData.Close();
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8 >
        mockCallData2(1, mobileService, callParams, callInfo);
    mockCallData2.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    
    completeData.Close();
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    call.Dial(requestStatus, pckgCallParams0, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    CleanupStack::PopAndDestroy(5,this);
    
    }


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::Dial for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::Dial for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	RMobileCall::TMobileCallParamsV7 callParams;
    callParams.iInterval = 100;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	callInfo.iStatus    = RMobileCall::EStatusUnknown;
	callInfo.iCallId    =-1;
	callInfo.iExitCode  =0; 
	callInfo.iEmergency =0;
	callInfo.iForwarded =0; 
	callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
	callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    

	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);

	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = ETrue;
	TMockLtsyCallData2<TInt, TBool> mockCallData2(callId, mobileService, 
												  hangUpCause, 
												  autoStChangeDisable);

	RCall::TCallParams callParamsX;
	TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
	callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParamsX.iInterval        = 100;
	callParamsX.iWaitForDialTone = RCall::EDialToneWait;

   //-------------------------------------------------------------------------
    // Test cancelling of RCall::Dial
    // before than LTSY sends any completions
    //-------------------------------------------------------------------------
    
    // open new call
    CleanupClosePushL(call);
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

       
    call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
    call.CancelAsyncRequest(EEtelCallDial);
    
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    
    RMobileCall::TMobileCallStatus  mobileCallStatus = RMobileCall::EStatusDialling; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallDataStatus.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);

    expectData.Close();
    mockCallData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
    User::WaitForRequest(mockLtsyStatus);

    // Cancelled Dial request. After this CTSY hangs up the call and it goes to idle state.
   
    RMobileCall::TMobileCallStatus  mobileCallStatus2 = RMobileCall::EStatusIdle; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus2(callId, mobileService, mobileCallStatus2);
    completeData.Close();
    mockCallDataStatus2.SerialiseL(completeData);
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, completeData);
        
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    call.Close();
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(1);
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::Dial
 	//-------------------------------------------------------------------------
 	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

		
	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	completeData.Close();
	mockCallDataStatus.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);

    expectData.Close();
    mockCallData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

    // Cancelled Dial request. After this CTSY hangs up the call and it goes to idle state.
    call.CancelAsyncRequest(EEtelCallDial);
	completeData.Close();
	mockCallDataStatus2.SerialiseL(completeData);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, completeData);
		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4); // expectData, completeData, this, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    TName name;
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	RMobileCall::TMobileCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RCall::Dial
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	RMobileCall::TEtel3rdPartyMobileCallParamsV1 paramsV1;
   	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> pckgParamsV1(paramsV1);

   	call.Dial(requestStatus, pckgParamsV1, KPhoneNumber);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	call.Close();
	CleanupStack::PopAndDestroy(&call);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::Dial
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

    _LIT8(KTempDesC8,"A");
	TBuf8<1> tempDes(KTempDesC8);
   	
   	call.Dial(requestStatus, tempDes, KPhoneNumber);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	call.Close();
	CleanupStack::PopAndDestroy(&call);

	//-------------------------------------------------------------------------
	// Test C: Test passing empty descriptor size to parameter in
	// RCall::Dial
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	TBuf8<1> nullDes(KNullDesC8);

    // If aCallParams is empty it is handled as there are no call parameters
    // and CTSY uses default values
   	call.Dial(requestStatus, nullDes, KPhoneNumber);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	call.Close();
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(&call);
	CleanupStack::PopAndDestroy(4); // completeData, expectData, line, this

	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::Dial for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::Dial for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RCall call1;
	RCall call2;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;

	// open new line1 and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName name;

	_LIT(KPhoneNumber1, "101632960000");   	
	_LIT(KPhoneNumber2, "654654864531");   	
   	
	TInt callId1 = 1;
	TInt callId2 = 2;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	RMobileCall::TMobileCallParamsV7 callParams1;
	callParams1.iInterval = 100;
	callParams1.iCug.iCugIndex = 0xFFFF;
	callParams1.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParams1.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParams1.iWaitForDialTone = RCall::EDialToneWait;
	callParams1.iIdRestrict = RMobileCall::EIdRestrictDefault;
	callParams1.iCug.iExplicitInvoke = EFalse;
	callParams1.iCug.iSuppressPrefCug = EFalse;
	callParams1.iCug.iSuppressOA = EFalse;
	callParams1.iAutoRedial = EFalse;
	
	RMobileCall::TMobileCallParamsV7 callParams2;
	callParams2.iInterval = 200;
	callParams2.iCug.iCugIndex = 0xFFFF;
	callParams2.iSpeakerControl  = RCall::EMonitorSpeakerControlAlwaysOn;
	callParams2.iSpeakerVolume   =RCall::EMonitorSpeakerVolumeMedium;
	callParams2.iWaitForDialTone = RCall::EDialToneWait;
	callParams2.iIdRestrict = RMobileCall::EIdRestrictDefault;
	callParams2.iCug.iExplicitInvoke = EFalse;
	callParams2.iCug.iSuppressPrefCug = EFalse;
	callParams2.iCug.iSuppressOA = EFalse;
	callParams2.iAutoRedial = EFalse;
    
    
    
	RMobileCall::TMobileCallInfoV8 callInfo1;
	callInfo1.iDialledParty.iTelNumber.Copy(KPhoneNumber1);
	callInfo1.iService = mobileService;
	callInfo1.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	callInfo1.iStatus    = RMobileCall::EStatusUnknown;
	callInfo1.iCallId    =-1;
	callInfo1.iExitCode  =0; 
	callInfo1.iEmergency =0;
	callInfo1.iForwarded =0; 
	callInfo1.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
	callInfo1.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    


	RMobileCall::TMobileCallInfoV8 callInfo2;
	callInfo2.iDialledParty.iTelNumber.Copy(KPhoneNumber2);
	callInfo2.iService = mobileService;
	callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	callInfo2.iStatus    = RMobileCall::EStatusUnknown;
	callInfo2.iCallId    =-1;
	callInfo2.iExitCode  =0; 
	callInfo2.iEmergency =0;
	callInfo2.iForwarded =0; 
	callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
	callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    

	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockData21(0, mobileService, callParams1, callInfo1);

	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockData22(0, mobileService, callParams2, callInfo2);

	TMockLtsyCallData0 mockData01(callId1, mobileService);
	TMockLtsyCallData0 mockData02(callId2, mobileService);

	RCall::TCallParams callParamsX1;
	TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);
	callParamsX1.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParamsX1.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParamsX1.iInterval        = 100;
	callParamsX1.iWaitForDialTone = RCall::EDialToneWait;

	RCall::TCallParams callParamsX2;
	TPckg<RCall::TCallParams> pckgCallParamsX2(callParamsX2);
	callParamsX2.iSpeakerControl  = RCall::EMonitorSpeakerControlAlwaysOn;
	callParamsX2.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeMedium;
	callParamsX2.iInterval        = 200;
	callParamsX2.iWaitForDialTone = RCall::EDialToneNoWait;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    call1.Dial(requestStatus1, pckgCallParamsX1, KPhoneNumber1);
    User::After(1000);
    
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgCallParamsX2, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, expectData, completeData, this
	                                      // line1, line2
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;

	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	
	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	RMobileCall::TMobileCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);

	RCall::TCallParams callParamsX;
	TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
	callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParamsX.iInterval        = 100;
	callParamsX.iWaitForDialTone = RCall::EDialToneWait;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::Dial
 	//-------------------------------------------------------------------------
 	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    
   	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4); // expectData, completeData, this, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006
@SYMDEF DEF127552
@SYMREQ	7745
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT

N.B. As the test case BA-CTSY-CCON-CD-0006 has a very long running time, it is split into sub-test cases.
The rationale for that is as following: When the call duration of a test case exceeds 10sec, CTYS sends
a life time update to LTSY. If a test case duration is at the border of ~10sec, the life time update
notification is completed depending on the speed of working environment. It varies machine to machine 
or board to board. Therefore, BA-CTSY-CCON-CD-0006 is divided into smaller parts where each part takes
far less than 10sec.
*/
void CCTsyCallControlFU::TestDial0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
			
	RMobileCall::TMobileDataCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);

	TPckg<RMobileCall::TMobileDataCallParamsV1> pckgMockParams1(callParams);
	
	RCall::TCallParams callParamsX;
	
	callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParamsX.iInterval        = 100;
	callParamsX.iWaitForDialTone = RCall::EDialToneWait;


 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::Dial when result is not cached.
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // we are dialing the call:
    call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
  	call.Close();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST D (Reloaded): Successful completion request of
	// RCall::Dial with TCallParams when result is not cached.
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileDataCallParamsV8 callParamsDefault;
    callParamsDefault.iInterval = 4;
    callParamsDefault.iCug.iCugIndex = 0xFFFF;
    callParamsDefault.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParamsDefault.iCug.iExplicitInvoke = EFalse;
    callParamsDefault.iCug.iSuppressPrefCug = EFalse;
    callParamsDefault.iCug.iSuppressOA = EFalse;
    callParamsDefault.iAutoRedial = EFalse;
       
    TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
    mockData3(0, mobileService, callParamsDefault, callInfo);
    
    expectData.Close();
	mockData3.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX); // passing TCallParams
	
    // we are dialing the call:
    call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
  	call.Close();
	CleanupStack::PopAndDestroy(1);
	
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006b
@SYMDEF DEF127552
@SYMREQ	7745
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls. It is in connection to BA-CTSY-CCON-CD-0006.
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
			
	RMobileCall::TMobileDataCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	TPckg<RMobileCall::TMobileDataCallParamsV1> pckgMockParams1(callParams);
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::Dial
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmDataCallTsy::Dial coverage
	// covers "else if ( RMobileCall::EStatusIdle != iMobileCallStatus )" condition
 	//-------------------------------------------------------------------------

	TName incomingCallName;
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	
	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// close incoming call
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
	RMobilePhone::TMobileService mobileServiceX = RMobilePhone::ECircuitDataService;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId, mobileServiceX, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    expectData.Close();
    mockData2x.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmDataCallTsy::Dial coverage
	// covers "else" condition in case of impossible of conditions
	// "if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )",
	// "else if ( RMobileCall::EStatusIdle != iMobileCallStatus )" and
	// "else if ( 0 < dialHandle )"
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusDisconnectingWithInband;
    TInt callId3 = 3;
    TName incomingCallName3;
    
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId3, incomingCallName3, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    TInt callId2 = 2;
    TName incomingCallName2;
    
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId2, incomingCallName2, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusRinging;
    //-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
   
  	// open new call
 	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrEtelCallAlreadyActive, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
      	
	//-------------------------------------------------------------------------
	// TEST : to increase CMmDataCallTsy::Dial coverage
	// covers "if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )" condition
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TRfStateInfo rfInfo = ERfsStateInfoInactive;
	TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
    expectData.Close();
    mockData1.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	rfInfo = (TRfStateInfo) 0;
	TMockLtsyData1<TRfStateInfo> mockDataRf(rfInfo);
	expectData.Close();
	mockDataRf.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());		 
			 		 
	// call CMmVoiceCallTsy::CompleteNotifyStatusChange for setting 
	// iMobileCallStatus = RMobileCall::EStatusIdle.
	RMobileCall::TMobileCallStatus mobileCallSt = RMobileCall::EStatusIdle; 
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus2(callId, mobileService, mobileCallSt);
	completeData.Close();
	mockCallDataStatus2.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//-------------------------------------------------------------------------
	// Tests for TMobileDataCallParamsV2
	
	RMobileCall::TMobileDataCallParamsV2 dataCallParams2;
	dataCallParams2.iInterval = 4;
	dataCallParams2.iCug.iCugIndex = 0xFFFF;
	dataCallParams2.iQoS = RMobileCall::EQosTransparentPreferred;
	dataCallParams2.iBearerMode = RMobileCall::EMulticallShareBearer;
	
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		mockDataCallParams2(0, mobileService, dataCallParams2, callInfo);

	TPckg<RMobileCall::TMobileDataCallParamsV2> pckgMockDataCallParams2(dataCallParams2);

 	//-------------------------------------------------------------------------
	// TEST A2: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------

	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
   // expectData.Close();
    mockDataCallParams2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockDataCallParams2, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B2: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
    mockDataCallParams2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockDataCallParams2, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
    mockDataCallParams2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	// we are dialing the call:
    call.Dial(requestStatus, pckgMockDataCallParams2, KPhoneNumber);
    
    CompleteDialStatusNotificationsL(call, callId, mobileService );
	
    // completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
     
    CompleteCallDisconnectNotificationsL(callId, mobileService);
  
	call.Close();
	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

// Returns sample call information variable
void GetCallInformation8(RMobileCall::TMobileCallInfoV8& aCallInformation, const TDesC& aPhoneNumber, RMobilePhone::TMobileService aMobileService)
	{
	aCallInformation.iDialledParty.iTelNumber.Copy(aPhoneNumber);
	aCallInformation.iService = aMobileService;
	aCallInformation.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	aCallInformation.iAlphaId.Copy(_L("Alpha Id"));
	aCallInformation.iValid |= RMobileCall::KCallAlphaId;
	aCallInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;;
	aCallInformation.iValid |= RMobileCall::KCallParamOrigin;
	}

// Returns sample call params variable
void GetCallParams8(RMobileCall::TMobileDataCallParamsV8& aDataCallParams)
	{
	aDataCallParams.iInterval = 4;
	aDataCallParams.iCug.iCugIndex = 0xFFFF;
	aDataCallParams.iQoS = RMobileCall::EQosTransparentPreferred;
	aDataCallParams.iBearerMode = RMobileCall::EMulticallShareBearer;
	aDataCallParams.iSubAddress.Copy(_L("An Unknown SubAddress"));
	aDataCallParams.iAlphaId.Copy(_L("Alpha Id"));
	aDataCallParams.iCallParamOrigin = RMobileCall::EOriginatorSIM;
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006c
@SYMDEF DEF127552
@SYMREQ	7745
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls. It is in connection to BA-CTSY-CCON-CD-0006.
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RCall call;
		
	TInt errorCode = KErrNone;
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
		
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "135468456456");   	
	   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    TMockLtsyCallData0 mockData0(callId, mobileService);
    
	//-------------------------------------------------------------------------
	// Tests for TMobileDataCallParamsV8
	
	RMobileCall::TMobileCallInfoV8 callInformation;
	GetCallInformation8(callInformation, KPhoneNumber(), mobileService);
	
	RMobileCall::TMobileDataCallParamsV8 dataCallParams8;
	GetCallParams8(dataCallParams8);
		
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		mockDataCallParams8(0, mobileService, dataCallParams8, callInformation);

	TPckg<RMobileCall::TMobileDataCallParamsV8> pckgMockDataCallParams8(dataCallParams8);

 	//-------------------------------------------------------------------------
	// TEST A3: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockDataCallParams8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockDataCallParams8, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B3: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
    mockDataCallParams8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockDataCallParams8, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C3: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
    mockDataCallParams8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	// we are dialing the call:
    call.Dial(requestStatus, pckgMockDataCallParams8, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
	call.Close();
	CleanupStack::PopAndDestroy(1); // call
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006d
@SYMDEF DEF127552
@SYMREQ	7745
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls. It is in connection to BA-CTSY-CCON-CD-0006.
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006dL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RCall call;
			
	TInt errorCode = KErrNone;
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
			
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;
	_LIT(KPhoneNumber, "135468456456"); 
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	RMobileCall::TMobileCallInfoV8 callInformation;
	GetCallInformation8(callInformation, KPhoneNumber(), mobileService);
	
	RMobileCall::TMobileDataCallParamsV8 dataCallParams8;
	GetCallParams8(dataCallParams8);
			
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
			mockDataCallParams8(0, mobileService, dataCallParams8, callInformation);

	TPckg<RMobileCall::TMobileDataCallParamsV8> pckgMockDataCallParams8(dataCallParams8);
	
	//-------------------------------------------------------------------------
	// Tests for default parameters
	
 	//-------------------------------------------------------------------------
	// TEST A4: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockDataCallParams8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B4: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
    mockDataCallParams8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C4: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
    mockDataCallParams8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);
	
	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	// we are dialing the call:
    call.Dial(requestStatus, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
	call.Close();
	CleanupStack::PopAndDestroy(1); // call
	
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006e
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls with HscsdCallParams. It is in connection to BA-CTSY-CCON-CD-0006
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006eL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
			
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	//-------------------------------------------------------------------------
	// Test: Test passing RMobileCall::TMobileHscsdCallParamsV1 to
	// RCall::Dial
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileHscsdCallParamsV1 hscsdCallParams;

	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockHscsdData(0, mobileService, hscsdCallParams, callInfo);

	TPckg<RMobileCall::TMobileHscsdCallParamsV1> pckgMockHscsdParams1(hscsdCallParams);

	//-------------------------------------------------------------------------
	// TEST A4: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockHscsdData.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockHscsdParams1, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B4: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockHscsdParams1, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C4: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------
	
	// open new call   	
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	// we are dialing the call:
    call.Dial(requestStatus, pckgMockHscsdParams1, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
	call.Close();
	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006f
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls with HscsdCallParams. It is in connection to BA-CTSY-CCON-CD-0006
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006fL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "135468456456");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    
    RMobileCall::TMobileCallInfoV8 callInfo;
    callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
    callInfo.iService = mobileService;
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
    	
	//-------------------------------------------------------------------------
    // Test: Test passing RMobileCall::TMobileHscsdCallParamsV2 to
	// RCall::Dial
	//-------------------------------------------------------------------------

	RMobileCall::TMobileHscsdCallParamsV2 hscsdCallParams2;

	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		mockHscsdData2(0, mobileService, hscsdCallParams2, callInfo);

	TPckg<RMobileCall::TMobileHscsdCallParamsV2> pckgMockHscsdParams2(hscsdCallParams2);

	//-------------------------------------------------------------------------
	// TEST A5: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockHscsdData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockHscsdParams2, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B5: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockHscsdParams2, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C5: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------

	// open new call   	
	CleanupClosePushL(call);
	errorCode = call.OpenNewCall(line, name);
	ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // we are dialing the call:
    call.Dial(requestStatus, pckgMockHscsdParams2, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
	call.Close();
	CleanupStack::PopAndDestroy(1);
	
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006g
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls with HscsdCallParams.It is in connection to BA-CTSY-CCON-CD-0006
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006gL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	//-------------------------------------------------------------------------
    // Test: Test passing RMobileCall::TMobileHscsdCallParamsV7 to
	// RCall::Dial
	//-------------------------------------------------------------------------

	RMobileCall::TMobileHscsdCallParamsV7 hscsdCallParams7;
	
	// TMobileHscsdCallParamsV7 params for Dial
    hscsdCallParams7.iInterval = 4;
    hscsdCallParams7.iCug.iCugIndex = 0xFFFF;
    hscsdCallParams7.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    hscsdCallParams7.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    hscsdCallParams7.iWaitForDialTone = RCall::EDialToneWait;
    hscsdCallParams7.iQoS = RMobileCall::EQosTransparentPreferred;
    hscsdCallParams7.iV42bisReq = RMobileCall::EV42bisTxDirection;
    hscsdCallParams7.iWantedRxTimeSlots = 2; 
    hscsdCallParams7.iCodings = 3;
    hscsdCallParams7.iBearerMode = RMobileCall::EMulticallShareBearer;
    hscsdCallParams7.iAlphaId.Copy(_L("Alpha Id"));
    hscsdCallParams7.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    
	RMobileCall::TMobileCallInfoV8 callInformation;
	GetCallInformation8(callInformation, KPhoneNumber(), mobileService);

	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockHscsdData7(0, mobileService, hscsdCallParams7, callInformation);

	TPckg<RMobileCall::TMobileHscsdCallParamsV7> pckgMockHscsdParams7(hscsdCallParams7);

	//-------------------------------------------------------------------------
	// TEST A6: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockHscsdData7.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockHscsdParams7, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B6: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData7.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockHscsdParams7, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C6: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------

	// open new call   	
	CleanupClosePushL(call);
	errorCode = call.OpenNewCall(line, name);
	ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData7.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // we are dialing the call:
    call.Dial(requestStatus, pckgMockHscsdParams7, KPhoneNumber);
	
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
	call.Close();
	CleanupStack::PopAndDestroy(1);
	
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006h
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls with HscsdCallParams.It is in connection to BA-CTSY-CCON-CD-0006
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006hL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;
	_LIT(KPhoneNumber, "135468456456");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	//-------------------------------------------------------------------------
    // Test: Test passing RMobileCall::TMobileHscsdCallParamsV8 to
	// RCall::Dial
	//-------------------------------------------------------------------------

	RMobileCall::TMobileHscsdCallParamsV8 hscsdCallParams8;
	
	// TMobileHscsdCallParamsV8 params for Dial
    hscsdCallParams8.iInterval = 2;
    hscsdCallParams8.iCug.iCugIndex = 0xFFFF;
    hscsdCallParams8.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    hscsdCallParams8.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    hscsdCallParams8.iWaitForDialTone = RCall::EDialToneWait;
    hscsdCallParams8.iQoS = RMobileCall::EQosTransparentPreferred;
    hscsdCallParams8.iV42bisReq = RMobileCall::EV42bisTxDirection;
    hscsdCallParams8.iWantedRxTimeSlots = 2; 
    hscsdCallParams8.iCodings = 3;
    hscsdCallParams8.iBearerMode = RMobileCall::EMulticallShareBearer;
    hscsdCallParams8.iAlphaId.Copy(_L("Alpha Id"));
    hscsdCallParams8.iSubAddress.Copy(_L("An Unknown SubAddress"));
    hscsdCallParams8.iCallParamOrigin = RMobileCall::EOriginatorSIM;

	RMobileCall::TMobileCallInfoV8 callInformation;
	GetCallInformation8(callInformation, KPhoneNumber(), mobileService);
	
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		mockHscsdData8(0, mobileService, hscsdCallParams8, callInformation);

	TPckg<RMobileCall::TMobileHscsdCallParamsV8> pckgMockHscsdParams8(hscsdCallParams8);

	//-------------------------------------------------------------------------
	// TEST A7: failure to dispatch request to LTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockHscsdData8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNotSupported);

	call.Dial(requestStatus, pckgMockHscsdParams8, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST B7: failure on completion of pending request from LTSY->CTSY 
 	//-------------------------------------------------------------------------
	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrGeneral, completeData);

	call.Dial(requestStatus, pckgMockHscsdParams8, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-------------------------------------------------------------------------
	// TEST C6: Successful completion request of
	// RCall::Dial when result is not cached
 	//-------------------------------------------------------------------------
	
	// open new call   	
	CleanupClosePushL(call);
	errorCode = call.OpenNewCall(line, name);
	ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdData8.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

	// we are dialing the call:
    call.Dial(requestStatus, pckgMockHscsdParams8, KPhoneNumber);
		
    CompleteDialStatusNotificationsL(call, callId, mobileService );
    
	// completion of the Dial() request:
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
      
    CompleteCallDisconnectNotificationsL(callId, mobileService);
	
	call.Close();
	CleanupStack::PopAndDestroy(1);

	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CD-0006i
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for data calls with minimal parameters (RCall::TCallParams)
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0006iL()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt errorCode = KErrNone;    

    //-- For Voice1 -------------------------

    TBuf<256> lineName(KMmTsyDataLineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);

    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   

    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");   

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;    
    TInt expectedCallId = 0;

    RMobileCall::TMobileDataCallParamsV8 callParams; 
    RMobileCall::TMobileDataCallParamsV8Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV8 callInfo;   
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval        = 100;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    

    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   

    TMockLtsyCallData2< RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8 >
    mockCallData(expectedCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    TInt callId = 1;
    
    completeData.Close();
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    TMockLtsyCallData0 mockDataComplete(callId, mobileService);
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    RCall::TCallParams callParamsX;
    callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParamsX.iInterval        = 100;
    callParamsX.iWaitForDialTone = RCall::EDialToneWait;
    TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
        
    call.Dial(requestStatus, pckgCallParamsX, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    CleanupStack::PopAndDestroy(5,this);
    
    }


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::Dial for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "135468456456");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	RMobileCall::TMobileDataCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	
    RMobileCall::TMobileCallStatus  mobileCallStatus2 = RMobileCall::EStatusIdle; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus2(callId, mobileService, mobileCallStatus2);
    completeData.Close();
    mockCallDataStatus2.SerialiseL(completeData);
		
	
    // Call Specific data
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	TPckg<RMobileCall::TMobileDataCallParamsV1> pckgMockParams1(callParams);
	
    expectData.Close();
    mockData2.SerialiseL(expectData);
	
    DialDataCallAndCancelL(line,pckgMockParams1,expectData,completeData);
	
	//------------------------------------------------------------------
	// TMobileDataCallParamsV2
	
	RMobileCall::TMobileDataCallParamsV2 dataCallParams2;
    dataCallParams2.iInterval = 4;
    dataCallParams2.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		mockDataCall2(0, mobileService, dataCallParams2, callInfo);

	TPckg<RMobileCall::TMobileDataCallParamsV2> pckgMockDataCallParams2(dataCallParams2);
	
    expectData.Close();
    mockDataCall2.SerialiseL(expectData);
    
    DialDataCallAndCancelL(line,pckgMockDataCallParams2,expectData,completeData);
	
	//------------------------------------------------------------------
	// TMobileDataCallParamsV8
	
	RMobileCall::TMobileDataCallParamsV8 dataCallParams8;
    dataCallParams8.iInterval = 4;
    dataCallParams8.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		mockDataCall8(0, mobileService, dataCallParams8, callInfo);

	TPckg<RMobileCall::TMobileDataCallParamsV8> pckgMockDataCallParams8(dataCallParams8);
	
    expectData.Close();
    mockDataCall8.SerialiseL(expectData);

    DialDataCallAndCancelL(line,pckgMockDataCallParams8,expectData,completeData);
    
	//------------------------------------------------------------------
	// TMobileHscsdCallParamsV1
		
	RMobileCall::TMobileHscsdCallParamsV1 hscsdCallParams;
	hscsdCallParams.iInterval = 4;
	hscsdCallParams.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall(0, mobileService, hscsdCallParams, callInfo);

	TPckg<RMobileCall::TMobileHscsdCallParamsV1> pckgMockHscsdCallParams(hscsdCallParams);
	
    expectData.Close();
    mockHscsdCall.SerialiseL(expectData);

    DialDataCallAndCancelL(line,pckgMockHscsdCallParams,expectData,completeData);
	
	//------------------------------------------------------------------
	// TMobileHscsdCallParamsV2
		
	RMobileCall::TMobileHscsdCallParamsV2 hscsdCallParams2;
	hscsdCallParams2.iInterval = 4;
	hscsdCallParams2.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall2(0, mobileService, hscsdCallParams2, callInfo);

	TPckg<RMobileCall::TMobileHscsdCallParamsV2> pckgMockHscsdCallParams2(hscsdCallParams2);
	
    expectData.Close();
    mockHscsdCall2.SerialiseL(expectData);

    DialDataCallAndCancelL(line,pckgMockHscsdCallParams2,expectData,completeData);
	
	//------------------------------------------------------------------
	// TMobileHscsdCallParamsV7
		
	RMobileCall::TMobileHscsdCallParamsV7 hscsdCallParams7;
	hscsdCallParams7.iInterval = 4;
	hscsdCallParams7.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall7(0, mobileService, hscsdCallParams7, callInfo);

	TPckg<RMobileCall::TMobileHscsdCallParamsV7> pckgMockHscsdCallParams7(hscsdCallParams7);
	
    expectData.Close();
    mockHscsdCall7.SerialiseL(expectData);
    
    DialDataCallAndCancelL(line,pckgMockHscsdCallParams7,expectData,completeData);
	
		
	//------------------------------------------------------------------
	// TMobileHscsdCallParamsV8
		
	RMobileCall::TMobileHscsdCallParamsV8 hscsdCallParams8;
	hscsdCallParams8.iInterval = 4;
	hscsdCallParams8.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall8(0, mobileService, hscsdCallParams8, callInfo);

	TPckg<RMobileCall::TMobileHscsdCallParamsV8> pckgMockHscsdCallParams8(hscsdCallParams8);
	
    expectData.Close();
    mockHscsdCall8.SerialiseL(expectData);

    DialDataCallAndCancelL(line,pckgMockHscsdCallParams8,expectData,completeData);
	CleanupStack::PopAndDestroy(4); // expectData, completeData, this, this
	}

void
CCTsyCallControlFU::DialDataCallAndCancelL(RLine& aLine,const TDesC8& aCallData,RBuf8& expectData,RBuf8& completeData)
	{
	// open new call
	
	TInt errorCode = KErrNone;
	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
	_LIT(KPhoneNumber, "135468456456");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	
	TName  name; 
	RCall call;
	CleanupClosePushL(call);
	errorCode = call.OpenNewCall(aLine, name);
	ASSERT_EQUALS(KErrNone, errorCode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);


	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

	call.Dial(requestStatus, aCallData, KPhoneNumber);

	User::WaitForRequest(mockLtsyStatus);

	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
	TMockLtsyCallData2<TInt, TBool> mockHangupData(callId, mobileService, 
                                       hangUpCause, 
                                       autoStChangeDisable);
	expectData.Close();
	mockHangupData.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	call.CancelAsyncRequest(EEtelCallDial);


	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, completeData);
      
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	}




/**
@SYMTestCaseID BA-CTSY-CCON-CD-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0008L()
	{

// This test should test sending bad parameter data for Dial
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	
	TInt8 callParams;    // bad param
	TPckg<TInt8> paramPckg(callParams);
	
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RCall::Dial
 	//-------------------------------------------------------------------------
	
	// open new call   	
	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
		
	call.Dial(requestStatus, paramPckg, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	call.Close();
	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::Dial for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0009L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RCall call1;
	RCall call2;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;

	// open new line1 and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber1, "101632960000");   	
	_LIT(KPhoneNumber2, "654654864531");   	
   	
	TInt callId1 = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	RMobileCall::TMobileDataCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV8 callInfo1;
	callInfo1.iDialledParty.iTelNumber.Copy(KPhoneNumber1);
	callInfo1.iService = mobileService;
	callInfo1.iValid = RMobileCall::KCallDialledParty |RMobileCall::KCallAlternating;

	RMobileCall::TMobileCallInfoV8 callInfo2;
	callInfo2.iDialledParty.iTelNumber.Copy(KPhoneNumber2);
	callInfo2.iService = mobileService;
	callInfo2.iValid = RMobileCall::KCallDialledParty |RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockData21(0, mobileService, callParams, callInfo1);

	TMockLtsyCallData0 mockData01(callId1, mobileService);
	
	TPckg<RMobileCall::TMobileDataCallParamsV1> pckgMockParams1(callParams);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	call1.Dial(requestStatus1, pckgMockParams1, KPhoneNumber1);

	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockParams1, KPhoneNumber2);
   	
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
   
    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
        
	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	//-----------------------------------------------------------------------
	// TMobileDataCallParamsV2
	RMobileCall::TMobileDataCallParamsV2 dataCallParams2;
    dataCallParams2.iInterval = 4;
    dataCallParams2.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		mockDataCall21(0, mobileService, dataCallParams2, callInfo1);

	TPckg<RMobileCall::TMobileDataCallParamsV2> pckgMockDataCallParams2(dataCallParams2);

	//-------------------------------------------------------------------------
	// Test A2: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockDataCall21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	call1.Dial(requestStatus1, pckgMockDataCallParams2, KPhoneNumber1);

	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockDataCallParams2, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-----------------------------------------------------------------------
	// TMobileDataCallParamsV8
	
	RMobileCall::TMobileDataCallParamsV8 dataCallParams8;
	dataCallParams8.iInterval = 2;
    dataCallParams8.iCug.iCugIndex = 0xFFFF;
    dataCallParams8.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    dataCallParams8.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    dataCallParams8.iWaitForDialTone = RCall::EDialToneWait;
    dataCallParams8.iQoS = RMobileCall::EQosTransparentPreferred;
    dataCallParams8.iV42bisReq = RMobileCall::EV42bisTxDirection;
    dataCallParams8.iBearerMode = RMobileCall::EMulticallShareBearer;
    dataCallParams8.iSubAddress.Copy(_L("An Unknown SubAddress"));
    dataCallParams8.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    dataCallParams8.iAlphaId.Copy(_L("Alpha Id"));
    
    RMobileCall::TMobileCallInfoV8 callInformation;
    callInformation.iDialledParty.iTelNumber.Copy(KPhoneNumber1);
    callInformation.iService = mobileService;
    callInformation.iValid |= RMobileCall::KCallDialledParty;
    callInformation.iValid |= RMobileCall::KCallAlternating;
    callInformation.iAlphaId.Copy(_L("Alpha Id"));
    callInformation.iValid |= RMobileCall::KCallAlphaId;
    callInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    callInformation.iValid |= RMobileCall::KCallParamOrigin;

	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		mockDataCall81(0, mobileService, dataCallParams8, callInformation);

	TPckg<RMobileCall::TMobileDataCallParamsV8> pckgMockDataCallParams8(dataCallParams8);

	//-------------------------------------------------------------------------
	// Test A3: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockDataCall81.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    
    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);

    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	call1.Dial(requestStatus1, pckgMockDataCallParams8, KPhoneNumber1);

	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockDataCallParams8, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
		
	// Done !
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, expectData, completeData, this
	                                      // line1, line2
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0009b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::Dial for data calls with HscsdCallParams. It is in connection to BA-CTSY-CCON-CD-0009 
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0009bL()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RCall call1;
	RCall call2;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;

	// open new line1 and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber1, "101632960000");   	
	_LIT(KPhoneNumber2, "654654864531");   	
   	
	TInt callId1 = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	RMobileCall::TMobileCallInfoV8 callInfo1;
	callInfo1.iDialledParty.iTelNumber.Copy(KPhoneNumber1);
	callInfo1.iService = mobileService;
	callInfo1.iValid = RMobileCall::KCallDialledParty |RMobileCall::KCallAlternating;

	RMobileCall::TMobileCallInfoV8 callInfo2;
	callInfo2.iDialledParty.iTelNumber.Copy(KPhoneNumber2);
	callInfo2.iService = mobileService;
	callInfo2.iValid = RMobileCall::KCallDialledParty |RMobileCall::KCallAlternating;
	
	//-----------------------------------------------------------------------
	// TMobileHscsdCallParamsV1
	RMobileCall::TMobileHscsdCallParamsV1 hscsdCallParams1;
	hscsdCallParams1.iInterval = 4;
	hscsdCallParams1.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall11(0, mobileService, hscsdCallParams1, callInfo1);

	TPckg<RMobileCall::TMobileHscsdCallParamsV1> pckgMockHscsdCallParams1(hscsdCallParams1);
	TMockLtsyCallData0 mockData01(callId1, mobileService);
	//-------------------------------------------------------------------------
	// Test A4: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdCall11.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	call1.Dial(requestStatus1, pckgMockHscsdCallParams1, KPhoneNumber1);

	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockHscsdCallParams1, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-----------------------------------------------------------------------
	// TMobileHscsdCallParamsV2
	RMobileCall::TMobileHscsdCallParamsV2 hscsdCallParams2;
	hscsdCallParams2.iInterval = 4;
	hscsdCallParams2.iCug.iCugIndex = 0xFFFF;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall21(0, mobileService, hscsdCallParams2, callInfo1);

	TPckg<RMobileCall::TMobileHscsdCallParamsV2> pckgMockHscsdCallParams2(hscsdCallParams2);

	//-------------------------------------------------------------------------
	// Test A5: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdCall21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	call1.Dial(requestStatus1, pckgMockHscsdCallParams2, KPhoneNumber1);

	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockHscsdCallParams2, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-----------------------------------------------------------------------
	// TMobileHscsdCallParamsV7
	RMobileCall::TMobileHscsdCallParamsV7 hscsdCallParams7;
	hscsdCallParams7.iInterval = 4;
	hscsdCallParams7.iCug.iCugIndex = 0xFFFF;
	hscsdCallParams7.iAlphaId.Copy(_L("Alpha Id"));
    hscsdCallParams7.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    
    RMobileCall::TMobileCallInfoV8 callInformation;
    callInformation.iDialledParty.iTelNumber.Copy(KPhoneNumber1);
    callInformation.iService = mobileService;
    callInformation.iValid |= RMobileCall::KCallDialledParty;
    callInformation.iValid |= RMobileCall::KCallAlternating;
    callInformation.iAlphaId.Copy(_L("Alpha Id"));
    callInformation.iValid |= RMobileCall::KCallAlphaId;
    callInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    callInformation.iValid |= RMobileCall::KCallParamOrigin;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall71(0, mobileService, hscsdCallParams7, callInformation);

	TPckg<RMobileCall::TMobileHscsdCallParamsV7> pckgMockHscsdCallParams7(hscsdCallParams7);

	//-------------------------------------------------------------------------
	// Test A6: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdCall71.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	call1.Dial(requestStatus1, pckgMockHscsdCallParams7, KPhoneNumber1);

	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockHscsdCallParams7, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	// Done !
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, expectData, completeData, this
	                                      // line1, line2
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-0009c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::Dial for data calls with HscsdCallParamsV8. It is in connection to Test support in CTSY for multiple client requests to RCall::Dial for data calls with HscsdCallParamsV8. It is in connection to BA-CTSY-CCON-CD-0009b
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::Dial for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial0009cL()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RCall call1;
	RCall call2;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;

	// open new line1 and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber1, "101632960000");   	
	_LIT(KPhoneNumber2, "654654864531");   	
   	
	TInt callId1 = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	RMobileCall::TMobileCallInfoV8 callInformation;
    callInformation.iDialledParty.iTelNumber.Copy(KPhoneNumber1);
    callInformation.iService = mobileService;
    callInformation.iValid |= RMobileCall::KCallDialledParty;
    callInformation.iValid |= RMobileCall::KCallAlternating;
    callInformation.iAlphaId.Copy(_L("Alpha Id"));
    callInformation.iValid |= RMobileCall::KCallAlphaId;
    callInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    callInformation.iValid |= RMobileCall::KCallParamOrigin;

		//-----------------------------------------------------------------------
	// TMobileHscsdCallParamsV8
	RMobileCall::TMobileHscsdCallParamsV8 hscsdCallParams8;
	hscsdCallParams8.iInterval = 4;
	hscsdCallParams8.iCug.iCugIndex = 0xFFFF;
	hscsdCallParams8.iAlphaId.Copy(_L("Alpha Id"));
	hscsdCallParams8.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    
	TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		mockHscsdCall81(0, mobileService, hscsdCallParams8, callInformation);

	TPckg<RMobileCall::TMobileHscsdCallParamsV8> pckgMockHscsdCallParams8(hscsdCallParams8);

	//-------------------------------------------------------------------------
	// Test A7: Test multiple clients requesting RCall::Dial
 	//-------------------------------------------------------------------------

	// call first clinet
	// open new call1
   	CleanupClosePushL(call1);
   	errorCode = call1.OpenNewCall(line1, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockHscsdCall81.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);

    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber1);
    call1.Dial(requestStatus1, pckgMockHscsdCallParams8, KPhoneNumber1);
    
    TMockLtsyCallData0 mockData01(callId1, mobileService);
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
   	
   	// call second clinet
	// open new call
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line2, name);
    ASSERT_EQUALS(KErrNone, errorCode);

   	call2.Dial(requestStatus2, pckgMockHscsdCallParams8, KPhoneNumber2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call1.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	
	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	// Done !
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, expectData, completeData, this
	                                      // line1, line2
	}



/**
@SYMTestCaseID BA-CTSY-CCON-CD-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial with timeout for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial and tests for timeout for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial00010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;

	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	RMobileCall::TMobileDataCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty;
	
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	TPckg<RMobileCall::TMobileDataCallParamsV1> pckgMockParams1(callParams);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::Dial
 	//-------------------------------------------------------------------------
 	
	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	expectData.Close();
	mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);

   	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	CleanupStack::PopAndDestroy(4); // expectData, completeData, this, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CD-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Dial for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Dial for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDial00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName name;

	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
			
	RMobileCall::TMobileCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	TPckg<RMobileCall::TMobileCallParamsV1> pckgMockParams1(callParams);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::Dial
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmFaxCallTsy::Dial coverage
	// covers "if( call->GetDialFlag() )" condition
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	// open new call
	RCall call2;
   	CleanupClosePushL(call2);
   	errorCode = call2.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.Dial(requestStatus, pckgMockParams1, KPhoneNumber);

	call2.Dial(requestStatus2, pckgMockParams1, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	call2.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmFaxCallTsy::Dial coverage
	// covers "if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() )" condition
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TRfStateInfo rfInfo = ERfsStateInfoInactive;
	TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
    expectData.Close();
    mockData1.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	RCall::TCallParams callParamsX;
	TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
	callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callParamsX.iInterval        = 100;
	callParamsX.iWaitForDialTone = RCall::EDialToneWait;
	
	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmOfflineOpNotAllowed, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmFaxCallTsy::Dial coverage
	// covers "else if ( RMobileCall::EStatusIdle != iMobileCallStatus )" condition
 	//-------------------------------------------------------------------------

	TName incomingCallName;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	rfInfo = ERfsStateInfoNormal;
	TMockLtsyData1<TRfStateInfo> mockData1X(rfInfo);
    expectData.Close();
    mockData1X.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	call.Dial(requestStatus, pckgCallParamsX, KPhoneNumber);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// close incoming call
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
	RMobilePhone::TMobileService mobileServiceX = RMobilePhone::EFaxService;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId, mobileServiceX, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    expectData.Close();
    mockData2x.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
	TInt callId2 = 2;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);
	TMockLtsyCallData0 mockData1(callId2, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNotSupported);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // call

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::HangUp when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	CleanupStack::PopAndDestroy(1); // call

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::HangUp
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    completeData.Close();
    mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "if ( 0 < hangUpHandle || iHangUpFlag )" condition
 	//-------------------------------------------------------------------------

	RCall call2;
	CleanupClosePushL(call2);
    TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrGeneral, completeData);
	call.HangUp(requestStatus);

	call2.HangUp(requestStatus2);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	// close incoming call
	call.Close();
	AssertMockLtsyStatusL();

	// close incoming call2
 	CloseIncomingCallL(call2, callId, mobileService);
	CleanupStack::PopAndDestroy(1); // call2
	
	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "if ( ( NULL != callList->GetMmCallByStatus(
	//                RMobileCall::EStatusConnected ) ) &&
	//              ( NULL != callList->GetMmCallByStatus(
	//                RMobileCall::EStatusHold ) ) &&
	//              ( NULL != callList->GetMmCallByStatus(
	//                RMobileCall::EStatusRinging ) ) )" condition
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusConnected;
    callId = 3;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusRinging;
    callId = 2;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusHold;
    callId = 1;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	callId = 2;
	hangUpCause = KErrGsmBusyUserRequest;
	autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2xx(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
    expectData.Close();
    mockData2xx.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData1.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
	call.HangUp(requestStatus);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// close incoming call for RMobileCall::EStatusRinging
    callId = 2;
 	CloseIncomingCallL(call, callId, mobileService);

	// open call for RMobileCall::EStatusHold
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);

	// close incoming call for RMobileCall::EStatusHold
    callId = 1;
 	CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	callId = 1;
	mobileCallStatus = RMobileCall::EStatusRinging;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	hangUpCause = KErrGsmBusyUserRequest;
	autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2xxx(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);

    expectData.Close();
    mockData2xxx.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrGeneral, completeData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // call

	//
	// Added for DEF139341 (Unexpected error note pops up after creating emergency call)
	// Tests that CTSY treats KErrGsmReleaseByUser as a normal return code, not as an error
	// (the client request should complete with KErrNone).
	//
	
	//-------------------------------------------------------------------------
    // create new incoming call
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
                                   mobileService, mobileCallStatus);    
    ASSERT_EQUALS(KErrNone, errorCode);
        
    CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------

    hangUpCause = KErrGsmBusyUserRequest;
    autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData3xxx(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);

    expectData.Close();
    mockData3xxx.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

    completeData.Close();
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrGsmReleaseByUser, completeData);

    call.HangUp(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    // close incoming call
    CloseIncomingCallL(call, callId, mobileService);
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(1); // call
	//
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::HangUp for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::HangUp for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::HangUp
 	//-------------------------------------------------------------------------
 	
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call.HangUp(requestStatus);
	
    call.CancelAsyncRequest(EEtelCallHangUp);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
 	
	User::WaitForRequest(mockLtsyStatus);
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::HangUp for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::HangUp for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RCall call1;
	RCall call2;
	RLine line1;
	RLine line2;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
	
	// open new line1
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
    
	TName incomingCallName1;
	TName incomingCallName2;
	   	
	TInt callId1 = 1;
	TInt callId2 = 2;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData21(callId1, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
    TMockLtsyCallData2<TInt, TBool> mockData22(callId2, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData01(callId1, mobileService);
	TMockLtsyCallData0 mockData02(callId2, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::HangUp
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call1);
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call2);
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	mockData01.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call1.HangUp(requestStatus1);

    expectData.Close();
    mockData22.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData02.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call2.HangUp(requestStatus2);
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	// close incoming call1
 	CloseIncomingCallL(call1, callId1, mobileService);
	AssertMockLtsyStatusL();

	// close incoming call2
 	CloseIncomingCallL(call2, callId2, mobileService);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // call1, call2
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, expectData, completeData, this
	                                      // line1, line2

	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::HangUp
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

    TTime t1,t2; //time stamps for measuring the Time Out
  
    
    t1.HomeTime(); // time stamp before request
	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	t2.HomeTime(); // timestamp after request
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	TTimeIntervalSeconds diff;
	TInt res = t2.SecondsFrom(t1,diff);
	ASSERT_EQUALS(KErrNone, res);
	if(diff.Int() < KMmCallHangUpTimeOut)
	{
        ERR_PRINTF3(_L("RCall::Hangup time out too short. Expected: %d Actual: %d "),KMmCallHangUpTimeOut, diff.Int());
        ASSERT(false);
	}
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this

	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNotSupported);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::HangUp when result is not cached.
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::HangUp
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    completeData.Close();
    mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "if ( 0 < hangUpHandle || iHangUpFlag )" condition
 	//-------------------------------------------------------------------------

	RCall call2;
	CleanupClosePushL(call2);
    TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrGeneral, completeData);
	call.HangUp(requestStatus);

	call2.HangUp(requestStatus2);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	// close incoming call2
	call2.Close();
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "else if ( RCall::EStatusIdle == iCallStatus &&
	//                   EMultimodeCallReqHandleUnknown == dialHandle )" condition
 	//-------------------------------------------------------------------------

	TInt  callId2 = 2;
	mobileCallStatus = RMobileCall::EStatusIdle;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId2, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	call2.HangUp(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

	// close incoming call2
	call2.Close();
 	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrGeneral, completeData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	
	
	//
    // Added for DEF139341 (Unexpected error note pops up after creating emergency call)
    // Tests that CTSY treats KErrGsmReleaseByUser as a normal return code, not as an error
    // (the client request should complete with KErrNone).
    //
    
    //-------------------------------------------------------------------------
    // create new incoming call
    TInt callId3 = 3;
    mobileCallStatus = RMobileCall::EStatusRinging;
    errorCode = CreateIncomingCallL(line, callId3, incomingCallName, 
                                   mobileService, mobileCallStatus);    
    ASSERT_EQUALS(KErrNone, errorCode);
    
    RCall call3;
    CleanupClosePushL(call3);
    errorCode = call3.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------

    TMockLtsyCallData2<TInt, TBool> mockData23(callId3, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
    
    TMockLtsyCallData0 mockData03(callId3, mobileService);
    
    expectData.Close();
    mockData23.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    completeData.Close();
    mockData03.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrGsmReleaseByUser, completeData);
    
    call3.HangUp(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	//
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // call3, call, line, completeData, expectData, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::HangUp for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::HangUp for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::HangUp
 	//-------------------------------------------------------------------------
 	
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	call.HangUp(requestStatus);
	
	// cancel hang-up
    // TSY has started a HangUp request and it is not possible to cancel this request
    call.CancelAsyncRequest(EEtelCallHangUp);
	
	// CTSY hangs up the call and it goes to idle state
    mobileCallStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmBusyUserRequest, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call, line, expectData, completeData, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CHU-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::HangUp for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::HangUp for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// Open third client
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);

	RMobilePhone phone3;
	ret = phone3.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	RCall call1;
	RCall call2;
	RCall call3;
	RLine line1;
	RLine line2;
	RLine line3;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus requestStatus3;
	
	// open new line1
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line3
	errorCode = OpenNewLineLC(phone3, line3, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName1;
	TName incomingCallName2;
	TName incomingCallName3;
	   	
	TInt callId1 = 1;
	TInt callId2 = 2;
	TInt callId3 = 3;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData21(callId1, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
    TMockLtsyCallData2<TInt, TBool> mockData22(callId2, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData01(callId1, mobileService);
	TMockLtsyCallData0 mockData02(callId2, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::HangUp
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call1);
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call2);
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call3
	
	mobileCallStatus = RMobileCall::EStatusConnected;
	errorCode = CreateIncomingCallL(line3, callId3, incomingCallName3, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call3);
    errorCode = call3.OpenExistingCall(line3, incomingCallName3);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	mockData01.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call1.HangUp(requestStatus1);

    expectData.Close();
    mockData22.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData02.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call2.HangUp(requestStatus2);

	hangUpCause = KErrGsmReleaseByUser;	                                              
    TMockLtsyCallData2<TInt, TBool> mockData23(callId3, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	TMockLtsyCallData0 mockData03(callId3, mobileService);

    expectData.Close();
    mockData23.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData03.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call3.HangUp(requestStatus3);
	
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	User::WaitForRequest(requestStatus3);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());
	
	CleanupStack::PopAndDestroy(3); // call1, call2, call3
	CleanupStack::PopAndDestroy(10, this); // phone2, telServer2, phone3, telServer3, 
	                                      // expectData, completeData, this
	                                      // line1, line2, line3
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp with timeout for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp and tests for timeout for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::HangUp
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 100303);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
	CleanupClosePushL(call);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNotSupported);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::HangUp when result is not cached.
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::HangUp
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    completeData.Close();
    mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "if ( 0 < hangUpHandle )" condition
 	//-------------------------------------------------------------------------

	RCall call2;
    TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
    errorCode = call2.OpenExistingCall(line, incomingCallName);
	CleanupClosePushL(call2);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrGeneral, completeData);
	call.HangUp(requestStatus);

	call2.HangUp(requestStatus2);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	// close incoming call2
	call2.Close();
 	AssertMockLtsyStatusL();	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrGeneral, completeData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

	
	//
    // Added for DEF139341 (Unexpected error note pops up after creating emergency call)
    // Tests that CTSY treats KErrGsmReleaseByUser as a normal return code, not as an error
    // (the client request should complete with KErrNone).
    //
    
    //-------------------------------------------------------------------------
    // create new incoming call
    TInt callId3 = 3;
    mobileCallStatus = RMobileCall::EStatusRinging;
    errorCode = CreateIncomingCallL(line, callId3, incomingCallName, 
                                   mobileService, mobileCallStatus);    
    ASSERT_EQUALS(KErrNone, errorCode);
    
    RCall call3;
    CleanupClosePushL(call3);
    errorCode = call3.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------

    TMockLtsyCallData2<TInt, TBool> mockData23(callId3, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
    
    TMockLtsyCallData0 mockData03(callId3, mobileService);
    
    expectData.Close();
    mockData23.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    completeData.Close();
    mockData03.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrGsmReleaseByUser, completeData);
    
    call3.HangUp(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	//
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // expectData, completeData, this, line, call, call2, call3
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00011b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00011bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	
	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "else if ( RCall::EStatusIdle == iCallStatus && 
	//                   EMultimodeCallReqHandleUnknown == dialHandle )" condition
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
	CleanupClosePushL(call);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	call.HangUp(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(5, this); // expectData, completeData, this, line, call
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00011c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00011cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyFaxLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusConnected;

	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	
	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::Dial coverage
	// covers "if ( RCall::EStatusRinging == iCallStatus )" condition
 	//-------------------------------------------------------------------------

	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
	CleanupClosePushL(call);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call.HangUp(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(5, this); // expectData, completeData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::HangUp for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::HangUp for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyFaxLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::HangUp
 	//-------------------------------------------------------------------------
 	
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	call.HangUp(requestStatus);
	
	// cancel hang-up
    // TSY has started a HangUp request and it is not possible to cancel this request
    call.CancelAsyncRequest(EEtelCallHangUp);
	
	// CTSY hangs up the call and it goes to idle state
    mobileCallStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmBusyUserRequest, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5, this); // call, line, expectData, completeData, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::HangUp for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::HangUp for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00014L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RCall call1;
	RCall call2;
	RLine line1;
	RLine line2;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
	
	// open new line1
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TName incomingCallName1;
	TName incomingCallName2;
	   	
	TInt callId1 = 1;
	TInt callId2 = 2;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData21(callId1, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
    TMockLtsyCallData2<TInt, TBool> mockData22(callId2, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData01(callId1, mobileService);
	TMockLtsyCallData0 mockData02(callId2, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::HangUp
 	//-------------------------------------------------------------------------

	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call1);
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
    
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call2);
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);

    expectData.Close();
    mockData21.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	mockData01.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call1.HangUp(requestStatus1);

    expectData.Close();
    mockData22.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	completeData.Close();
	mockData02.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);

	call2.HangUp(requestStatus2);
	
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::PopAndDestroy(2); // call1, call2
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, expectData, completeData, this
	                                      // line1, line2
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CHU-00015
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::HangUp with timeout for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::HangUp and tests for timeout for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHangUp00015L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
			
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
	
	TMockLtsyCallData0 mockData0(callId, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::HangUp
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call);
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

	call.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 100303);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CH-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Hold for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Hold for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
    TMockLtsyCallData0 mockData0(callId, mobileService);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData, KErrNotSupported);
    call.Hold(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData);
    iMockLTSY.CompleteL(EMobileCallHold, KErrGeneral, expectData);
    call.Hold(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::Hold when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData);
    iMockLTSY.CompleteL(EMobileCallHold, KErrNone, expectData);
    call.Hold(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::Hold
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallHold, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectData, completeData, call, line
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CH-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::Hold for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::Hold for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    TMockLtsyCallData0 mockData0(callId, mobileService);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
 	
	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData);
    iMockLTSY.CompleteL(EMobileCallHold, KErrNone, expectData);

    call.Hold(requestStatus);

    call.CancelAsyncRequest(EMobileCallHold);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    
    //TSY has started a request and it is not possible to then cancel this
    //request. The best thing for the TSY to do in this case is to proceed
    //as though the Cancel never happened. The server's call to the TSY
    //cancel function will return synchronously. The TSY then continues to
    //wait for the original acknowledgement and when it receives it,
    //the TSY will complete the original request.
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(4); // expectData, this, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CH-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::Hold for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::Hold for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RMobileCall call1;
	CleanupClosePushL(call1);

	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TName incomingCallName1;

	TInt callId2 = 2;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    TMockLtsyCallData0 mockData01(callId1, mobileService);
    TMockLtsyCallData0 mockData02(callId2, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData01.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData);
    iMockLTSY.CompleteL(EMobileCallHold, KErrNone, expectData);

    call1.Hold(requestStatus1);

    expectData.Close();
    mockData02.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData);
    iMockLTSY.CompleteL(EMobileCallHold, KErrNone, expectData);

    call2.Hold(requestStatus2);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(4); // line1, line2, call1, call2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CH-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Hold with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Hold and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    TMockLtsyCallData0 mockData0(callId, mobileService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallHold, expectData);
    call.Hold(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CH-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Hold for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Hold for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    call.Hold(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CH-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Hold for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Hold for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    call.Hold(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CH-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Hold and RMobileCall::AnswerIncomingCall fails because call waiting is disabled
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Hold for voice and data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestHold0001aL()
	{
	//Test voice call functionality
    TInt ret = TestAnsweringFailsBecauseHoldDoesNotWorkL( KMmTsyVoice1LineName );
    ASSERT_EQUALS(KErrNone, ret);
    
    //Test data call functionality
	ret = TestAnsweringFailsBecauseHoldDoesNotWorkL( KMmTsyDataLineName );
    ASSERT_EQUALS(KErrNone, ret);        
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Swap for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Swap for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSwap0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
    TMockLtsyCallData0 mockData0(callId, mobileService);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallSwap, expectData, KErrNotSupported);
    call.Swap(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallSwap, expectData);
    iMockLTSY.CompleteL(EMobileCallSwap, KErrGeneral, expectData);
    call.Swap(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::Swap when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallSwap, expectData);
    iMockLTSY.CompleteL(EMobileCallSwap, KErrNone, expectData);
    call.Swap(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::Swap
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallSwap, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectData, completeData, call, line
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::Swap for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::Swap for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSwap0002L()
	{

// This test should test cancellation of Swap
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
 	
	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);

    iMockLTSY.ExpectL(EMobileCallSwap, expectData);
    iMockLTSY.CompleteL(EMobileCallSwap, KErrNone, expectData);

    call.Swap(requestStatus);

    call.CancelAsyncRequest(EMobileCallSwap);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    // see CMmVoiceCallTsy::CancelService in line 508 in file CMmVoiceCallTsy.cpp
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(4); // expectData, this, call, line
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::Swap for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::Swap for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSwap0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RMobileCall call1;
	CleanupClosePushL(call1);

	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TName incomingCallName1;

	TInt callId2 = 2;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData01(callId1, mobileService);
    expectData.Close();
    mockData01.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallSwap, expectData);
    iMockLTSY.CompleteL(EMobileCallSwap, KErrNone, expectData);

    call1.Swap(requestStatus1);

    TMockLtsyCallData0 mockData02(callId2, mobileService);
    expectData.Close();
    mockData02.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallSwap, expectData);
    iMockLTSY.CompleteL(EMobileCallSwap, KErrNone, expectData);

    call2.Swap(requestStatus2);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(4); // line1, line2, call1, call2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Swap with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Swap and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSwap0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileCall::Swap
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallSwap, expectData);

    call.Swap(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Swap for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Swap for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSwap0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	// see line 252 in function CMmDataCallTsy::DoExtFuncL in file CMmDataCallTsy.cpp
    call.Swap(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CS-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Swap for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Swap for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSwap00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	// see line 265 in function CMmFaxCallTsy::DoExtFuncL in file CMmFaxCallTsy.cpp
    call.Swap(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Resume for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Resume for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestResume0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
    TMockLtsyCallData0 mockData0(callId, mobileService);
	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallResume, expectData, KErrNotSupported);
    call.Resume(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallResume, expectData);
    iMockLTSY.CompleteL(EMobileCallResume, KErrGeneral, expectData);
    call.Resume(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::Resume when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallResume, expectData);
    iMockLTSY.CompleteL(EMobileCallResume, KErrNone, expectData);
    call.Resume(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::Resume
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallResume, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectData, completeData, line, this, call
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::Resume for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::Resume for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestResume0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
 	
	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);

    iMockLTSY.ExpectL(EMobileCallResume, expectData);
    iMockLTSY.CompleteL(EMobileCallResume, KErrNone, expectData);

    call.Resume(requestStatus);

    call.CancelAsyncRequest(EMobileCallResume);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    // see CMmVoiceCallTsy::CancelService in line 507 in file CMmVoiceCallTsy.cpp
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(4); // expectData, this, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::Resume for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::Resume for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestResume0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RMobileCall call1;
	CleanupClosePushL(call1);

	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TName incomingCallName1;

	TInt callId2 = 2;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData01(callId1, mobileService);
    expectData.Close();
    mockData01.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallResume, expectData);
    iMockLTSY.CompleteL(EMobileCallResume, KErrNone, expectData);

    call1.Resume(requestStatus1);

    TMockLtsyCallData0 mockData02(callId2, mobileService);
    expectData.Close();
    mockData02.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallResume, expectData);
    iMockLTSY.CompleteL(EMobileCallResume, KErrNone, expectData);

    call2.Resume(requestStatus2);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(4); // line1, line2, call1, call2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Resume with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Resume and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestResume0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallResume, expectData);

    call.Resume(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CR-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Resume for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Resume for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestResume0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	// see EMobileCallResume case in function CMmDataCallTsy::DoExtFuncL in file CMmDataCallTsy.cpp
    call.Resume(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CR-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Resume for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Resume for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestResume00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	// see EMobileCallResume case in function CMmFaxCallTsy::DoExtFuncL in file CMmFaxCallTsy.cpp
    call.Resume(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CDF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Deflect for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Deflect for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDeflect0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// Test API is not supported
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallDeflect deflectType = RMobileCall::EDeflectUnspecified;
	RMobilePhone::TMobileAddress destination;
    call.Deflect(requestStatus, deflectType, destination);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CDF-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Deflect for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Deflect for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDeflect0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// Test API is not supported
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallDeflect deflectType = RMobileCall::EDeflectUnspecified;
	RMobilePhone::TMobileAddress destination;
    call.Deflect(requestStatus, deflectType, destination);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CDF-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Deflect for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Deflect for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDeflect0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for data
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;

    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// Test API is not supported
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallDeflect deflectType = RMobileCall::EDeflectUnspecified;
	RMobilePhone::TMobileAddress destination;
    call.Deflect(requestStatus, deflectType, destination);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // expectData, call, line
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Transfer for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Transfer for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;
	TInt callId(1);
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging; //RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusHold;
    callId = 2;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// create new incoming call
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallTransfer, expectData, KErrNotSupported);

    call.Transfer(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	// close incoming call
 	CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallTransfer, expectData);
    iMockLTSY.CompleteL(EMobileCallTransfer, KErrGeneral, expectData);
    call.Transfer(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::Transfer when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallTransfer, expectData);
    iMockLTSY.CompleteL(EMobileCallTransfer, KErrNone, expectData);
    call.Transfer(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::Transfer
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallTransfer, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // expectData, completeData, call, line, call2
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CT-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::Transfer for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::Transfer for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    // open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusHold;
    callId = 2;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::Transfer
 	//-------------------------------------------------------------------------
 	
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);

    iMockLTSY.ExpectL(EMobileCallTransfer, expectData);
    iMockLTSY.CompleteL(EMobileCallTransfer, KErrNone, expectData);

    call.Transfer(requestStatus);

    call.CancelAsyncRequest(EMobileCallTransfer);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    // see CMmVoiceCallTsy::CancelService in file CMmVoiceCallTsy.cpp
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(5); // expectData, this, call, line, call2
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CT-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::Transfer for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::Transfer for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line11;
	RLine line12;
	RLine line22;
	
	RMobileCall call11;
	CleanupClosePushL(call11);
	
	RMobileCall call12;
	CleanupClosePushL(call12);
	
	RMobileCall call22;
	CleanupClosePushL(call22);

	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TInt callId2 = 2;
	TName incomingCallName1;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	// open new line11 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line11, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line12 for KMmTsyVoice2LineName
	errorCode = OpenNewLineLC(iPhone, line12, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line22 for KMmTsyVoice2LineName
	errorCode = OpenNewLineLC(phone2, line22, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line11, callId1, incomingCallName1, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	errorCode = call11.OpenExistingCall(line11, incomingCallName1);
	ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusHold;
    mobileService = RMobilePhone::EAuxVoiceService;
 	//-------------------------------------------------------------------------
	// create new incoming call12 for iPhone
	errorCode = CreateIncomingCallL(line12, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();

	errorCode = call12.OpenExistingCall(line12, incomingCallName2);
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	RLine::TCallInfo callInfo22;
	errorCode = line22.GetCallInfo(0, callInfo22);
	ASSERT_EQUALS(KErrNone, errorCode);

	errorCode = call22.OpenExistingCall(line22, callInfo22.iCallName);
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::Transfer
 	//-------------------------------------------------------------------------

	TMockLtsyCallData0 mockData(callId2, mobileService);
    expectData.Close();
	mockData.SerialiseL(expectData);
	iMockLTSY.ExpectL(EMobileCallTransfer, expectData);
	iMockLTSY.CompleteL(EMobileCallTransfer, KErrNone, expectData, 10);

	call12.Transfer(requestStatus1);
    call22.Transfer(requestStatus2);

    User::WaitForRequest(requestStatus1);
    User::WaitForRequest(requestStatus2);

    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

    AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6); // line11, line12, line22, call11, call12, call22
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, expectData, this, completeData
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CT-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Transfer with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Transfer and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	
	RMobileCall call;
	CleanupClosePushL(call);
	
	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
	errorCode = call2.OpenExistingCall(line, incomingCallName);
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileCall::Transfer
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusHold;
	callId = 2;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallTransfer, expectData);

    call.Transfer(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(5, this); // expectData, this, line, call, call2
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CT-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Transfer for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Transfer for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);

	RMobileCall ringingCall;
	CleanupClosePushL(ringingCall);

	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
	
	// open new line and call for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = ringingCall.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    callId = 2;
    mobileCallStatus = RMobileCall::EStatusHold;
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    call.Transfer(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

	// close incoming call
 	CloseIncomingCallL(call, callId, RMobilePhone::ECircuitDataService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::Transfer
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallTransfer, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(6, this); // expectData, completeData, call, line, ringingCall
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CT-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::Transfer for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::Transfer for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer0009L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RMobileCall call1;
	CleanupClosePushL(call1);
	RMobileCall call2;
	CleanupClosePushL(call2);
	RMobileCall call3;
	CleanupClosePushL(call3);
	RMobileCall call4;
	CleanupClosePushL(call4);
	
	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TInt callId2 = 2;
	TInt callId3 = 3;
	TInt callId4 = 4;
	TName incomingCallName1;
	TName incomingCallName2;
	TName incomingCallName3;
	TName incomingCallName4;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyDataLineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatusHold = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

 	//-------------------------------------------------------------------------
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatusHold);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call2 for phone2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatusHold);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::Transfer
 	//-------------------------------------------------------------------------

	mobileCallStatusHold = RMobileCall::EStatusHold;

 	//-------------------------------------------------------------------------
	// create new incoming call1 for iPhone
	errorCode = CreateIncomingCallL(line1, callId3, incomingCallName3,
	                               mobileService, mobileCallStatusHold);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call3.OpenExistingCall(line1, incomingCallName3);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	errorCode = CreateIncomingCallL(line2, callId4, incomingCallName4,
	                               mobileService, mobileCallStatusHold);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call4.OpenExistingCall(line2, incomingCallName4);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    call3.Transfer(requestStatus1);
    call4.Transfer(requestStatus2);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotReady, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotReady, requestStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(6); // line1, line2, call1, call2, call3, call4
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, expectData, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CT-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::Transfer for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Transfer for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTransfer00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	RMobileCall ringingCall;
	CleanupClosePushL(ringingCall);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
	
	// open new line and call for KMmTsyFaxLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);

	errorCode = ringingCall.OpenExistingCall(line, incomingCallName);
	ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusHold;
	callId = 2;

	/*
	 * Can't create incoming call with call status equal RMobileCall::EStatusHold.
	 * Destructor of CMmCallTsy is called for inclomming call (callId = 1)
	 * with call status equal RMobileCall::EStatusRinging by call of RCall::Close().
	 */

//	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 100804);

	//ASSERT_TRUE(false);

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    call.Transfer(requestStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // expectData, completeData, call, line, ringingCall
	}


/**
@SYMTestCaseID BA-CTSY-CCON-MTAC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::TerminateAllCalls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateAllCalls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateAllCalls0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
    
	TRequestStatus requestStatus;
    
	//-----------------------------------------------------------------------
	// TEST 0: with no active calls 
	//----------------------------------------------------------------------
	
	iPhone.TerminateAllCalls(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// open new line and call for KMmTsyVoice1LineName
	RLine line;
	RCall call;
	TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
	ASSERT_EQUALS(KErrNone, errorCode);

	// create new incoming call
	TName incomingCallName;
	TInt callId = 1;
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
			RMobilePhone::EVoiceService, RMobileCall::EStatusConnecting);	
	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    TMockLtsyData1<TInt> mockData(callId);
	mockData.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECtsyPhoneTerminateAllCallsReq, expectData, KErrNotSupported);
	iPhone.TerminateAllCalls(requestStatus);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECtsyPhoneTerminateAllCallsReq, expectData);
	iMockLTSY.CompleteL(ECtsyPhoneTerminateAllCallsComp, KErrGeneral);

	iPhone.TerminateAllCalls(requestStatus);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::TerminateAllCalls.
 	//-------------------------------------------------------------------------
	
    iMockLTSY.ExpectL(ECtsyPhoneTerminateAllCallsReq, expectData);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateAllCallsComp, KErrNone);
    
	iPhone.TerminateAllCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();
    
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::TerminateAllCalls
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECtsyPhoneTerminateAllCallsComp, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-MTAC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling RMobilePhone::TerminateAllCalls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateAllCalls and cancels it
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateAllCalls0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
    
	//-------------------------------------------------------------------------
	
	// open new line and call for KMmTsyVoice1LineName
	RLine line;
	RCall call;
	TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
	ASSERT_EQUALS(KErrNone, errorCode);

	// create new incoming call
	TName incomingCallName;
	TInt callId = 1;
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
			RMobilePhone::EVoiceService, RMobileCall::EStatusConnecting);	
	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------
    
	//-------------------------------------------------------------------------
	// Test A: cancelling of RMobilePhone::TerminateAllCalls
 	//-------------------------------------------------------------------------

    TMockLtsyData1<TInt> mockData(callId);
	mockData.SerialiseL(expectData);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// Expect call down to LTSY for this IPC
	iMockLTSY.ExpectL(ECtsyPhoneTerminateAllCallsReq, expectData);

	// Queue response to be delayed to give time for cancel to come through
	iMockLTSY.CompleteL(ECtsyPhoneTerminateAllCallsComp, KErrNone, 10);

	TRequestStatus requestStatus;
	iPhone.TerminateAllCalls(requestStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneTerminateAllCalls);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-MTAC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests of RMobilePhone::TerminateAllCalls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateAllCalls on multiple clients
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateAllCalls0004L()
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

	//
	// set up line and call for first client
	
	RLine line;
	RCall call;
	TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
	ASSERT_EQUALS(KErrNone, errorCode);

	// create new incoming call
	TName incomingCallName;
	TInt callId = 1;
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
			RMobilePhone::EVoiceService, RMobileCall::EStatusConnecting);	
	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    //
    // set up call for second client
    
	RLine line2;
	RCall call2;
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName); // PUSH
	ASSERT_EQUALS(KErrNone, errorCode);

	// create new incoming call
	TName incomingCallName2;
	TInt callId2 = 2;
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2, 
			RMobilePhone::EVoiceService, RMobileCall::EStatusConnecting);	
	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call2); // PUSH
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	
    // let's start testing...
    
    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
    TMockLtsyData1<TInt> mockData(callId);
	mockData.SerialiseL(expectData);
	
    //RBuf8 expectData2;
    //CleanupClosePushL(expectData2); // PUSH
    //TMockLtsyData1<TInt> mockData2(callId2);
	//mockData2.SerialiseL(expectData2);

	iMockLTSY.ExpectL(ECtsyPhoneTerminateAllCallsReq, expectData);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateAllCallsComp, KErrNone, 20);
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    iPhone.TerminateAllCalls(requestStatus);
    phone2.TerminateAllCalls(requestStatus2);
    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(8, this); 	// expectData,
											// call2, line2, call, line,
											// phone2, telServer2, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-MTAC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::TerminateAllCalls with timeout.
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateAllCalls and tests for timeout.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateAllCalls0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
   
    
	// open new line and call for KMmTsyVoice1LineName
	RLine line;
	RCall call;
	TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
	ASSERT_EQUALS(KErrNone, errorCode);

	
	// create new incoming call
	TName incomingCallName;
	TInt callId = 1;
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
			RMobilePhone::EVoiceService, RMobileCall::EStatusConnecting);	
	
	ASSERT_EQUALS(KErrNone, errorCode);
		
	CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

    TMockLtsyData1<TInt> mockData(callId);
	mockData.SerialiseL(expectData);    
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::TerminateAllCalls
 	//-------------------------------------------------------------------------
    
	TRequestStatus requestStatus;
    iMockLTSY.ExpectL(ECtsyPhoneTerminateAllCallsReq, expectData);
	iPhone.TerminateAllCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
	}

/**
@SYMTestCaseID BA-CTSY-CCON-MTACTC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::TerminateActiveCalls  When the call status is EStatusHold
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateActiveCalls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateActiveCalls0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
    
    TRequestStatus requestStatus;
    
    //-----------------------------------------------------------------------
    // TEST 0: with no active calls 
    //----------------------------------------------------------------------
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq,KErrNone);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone);
    iPhone.TerminateActiveCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    // open new line and call for KMmTsyVoice1LineName
    RLine line;
    RCall call;
    TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    // create new incoming call
    TName incomingCallName;
    TInt callId = 1;
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
            RMobilePhone::EVoiceService, RMobileCall::EStatusHold);   
    
    ASSERT_EQUALS(KErrNone, errorCode);
        
    CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------

    TMockLtsyData1<TInt> mockData(callId);
    mockData.SerialiseL(expectData);
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq, expectData, KErrNotSupported);
    iPhone.TerminateActiveCalls(requestStatus);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    AssertMockLtsyStatusL();

    
    
    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq, expectData);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrGeneral);

    iPhone.TerminateActiveCalls(requestStatus);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobilePhone::TerminateActiveCalls.
    //-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq, expectData);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone);
    
    iPhone.TerminateActiveCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobilePhone::TerminateActiveCalls
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();
        
    CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
    }


/**
@SYMTestCaseID BA-CTSY-CCON-MTACTC-000B1
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::TerminateActiveCalls When the call status is EStatusRinging
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateActiveCalls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateActiveCalls0001bL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
        
    TRequestStatus requestStatus;
    // open new line and call for KMmTsyVoice1LineName
    RLine line;
    RCall call;
    TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    // create new incoming call
    TName incomingCallName;
    TInt callId = 1;
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
    RMobilePhone::EVoiceService, RMobileCall::EStatusRinging);   
        
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq,KErrNone);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone);
    iPhone.TerminateActiveCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();
        
    CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
    }
    
/**
@SYMTestCaseID BA-CTSY-CCON-MTACTC-000C1
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::TerminateActiveCalls When the call status is EStatusConnecting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateActiveCalls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateActiveCalls0001cL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
        
    TRequestStatus requestStatus;
    // open new line and call for KMmTsyVoice1LineName
    RLine line;    
    TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    // create new incoming call
    TName incomingCallName;
    TInt callId = 1;
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
                RMobilePhone::EVoiceService, RMobileCall::EStatusConnecting);   
        
    ASSERT_EQUALS(KErrNone, errorCode);
    RCall call;    
    CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq,KErrNone);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone);
    iPhone.TerminateActiveCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();
        
    CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
    }



/**
@SYMTestCaseID BA-CTSY-CCON-MTACTC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling RMobilePhone::TerminateActiveCalls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateActiveCalls and cancels it
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateActiveCalls0002L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
    
    //-------------------------------------------------------------------------
    
    // open new line and call for KMmTsyVoice1LineName
    RLine line;
    RCall call;
    TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    // create new incoming call
    TName incomingCallName;
    TInt callId = 1;
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
            RMobilePhone::EVoiceService, RMobileCall::EStatusHold);   
    
    ASSERT_EQUALS(KErrNone, errorCode);
        
    CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------
    
    //-------------------------------------------------------------------------
    // Test A: cancelling of RMobilePhone::TerminateActiveCalls
    //-------------------------------------------------------------------------

    TMockLtsyData1<TInt> mockData(callId);
    mockData.SerialiseL(expectData);
    
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
        
    // Expect call down to LTSY for this IPC
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq, expectData);

    // Queue response to be delayed to give time for cancel to come through
    // There is no interface for cancelling the simple TerminateActiveCalls request once passed to LTSY, 
    // however this test will still emulate the completion of the request which will finally return from LTSY regardless.
    
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone, 10);

    TRequestStatus requestStatus;
    iPhone.TerminateActiveCalls(requestStatus);
    iPhone.CancelAsyncRequest(EMobilePhoneTerminateActiveCalls);
    User::WaitForRequest(requestStatus);
    INFO_PRINTF2(_L("requestStatus.Int() %d"),(requestStatus.Int()));
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    AssertMockLtsyStatusL();
    
    User::WaitForRequest(mockLtsyStatus);
    INFO_PRINTF2(_L("mockLtsyStatus.Int() %d"),(mockLtsyStatus.Int()));
    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
    }

/**
@SYMTestCaseID BA-CTSY-CCON-MTACTC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests of RMobilePhone::TerminateActiveCalls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateActiveCalls on multiple clients
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateActiveCalls0004L()
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

    //
    // set up line and call for first client
    
    RLine line;
    RCall call;
    TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    // create new incoming call
    TName incomingCallName;
    TInt callId = 1;
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
            RMobilePhone::EVoiceService, RMobileCall::EStatusHold);   
    
    ASSERT_EQUALS(KErrNone, errorCode);
        
    CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    //
    // set up call for second client
    
    RLine line2;
    RCall call2;
    errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    // create new incoming call
    TName incomingCallName2;
    TInt callId2 = 2;
    errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2, 
            RMobilePhone::EVoiceService, RMobileCall::EStatusHold);   
    
    ASSERT_EQUALS(KErrNone, errorCode);
        
    CleanupClosePushL(call2); // PUSH
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    // let's start testing...
    
    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
    TMockLtsyData1<TInt> mockData(callId);
    mockData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq, expectData);
    iMockLTSY.CompleteL(ECtsyPhoneTerminateActiveCallsComp, KErrNone, 20);
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    iPhone.TerminateActiveCalls(requestStatus);
    phone2.TerminateActiveCalls(requestStatus2);
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(8, this);   // expectData,
                                            // call2, line2, call, line,
                                            // phone2, telServer2, this
    }

/**
@SYMTestCaseID BA-CTSY-CCON-MTACTC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::TerminateActiveCalls with timeout.
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateActiveCalls and tests for timeout.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestTerminateActiveCalls0005L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData); // PUSH
   
    
    // open new line and call for KMmTsyVoice1LineName
    RLine line;
    RCall call;
    TInt errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName); // PUSH
    ASSERT_EQUALS(KErrNone, errorCode);

    
    // create new incoming call
    TName incomingCallName;
    TInt callId = 1;
    errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
            RMobilePhone::EVoiceService, RMobileCall::EStatusHold);   
    
    ASSERT_EQUALS(KErrNone, errorCode);
        
    CleanupClosePushL(call); // PUSH
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------

    TMockLtsyData1<TInt> mockData(callId);
    mockData.SerialiseL(expectData);    
    //-------------------------------------------------------------------------
    // Test A: Test timeout of RMobilePhone::TerminateActiveCalls
    //-------------------------------------------------------------------------
    
    TRequestStatus requestStatus;
    iMockLTSY.ExpectL(ECtsyPhoneTerminateActiveCallsReq, expectData);
    iPhone.TerminateActiveCalls(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

    CleanupStack::PopAndDestroy(4, this); // call, line, expectData, this
    }

/**
@SYMTestCaseID BA-CTSY-CCON-MSICT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetIncomingCallType
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetIncomingCallType
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestSetIncomingCallType0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RMobilePhone::TMobilePhoneIncomingCallType callType = RMobilePhone::EIncomingTypeNotSpecified;
	
	RMobileCall::TMobileDataCallParamsV1 dataCallParamsV1;
	TPckg<RMobileCall::TMobileDataCallParamsV1> dataCallParamsV1Pckg(dataCallParamsV1);
	
 	//-------------------------------------------------------------------------
	// Test API is not supported
 	//-------------------------------------------------------------------------

	iPhone.SetIncomingCallType(requestStatus, callType, dataCallParamsV1Pckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAICI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallISV for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallISV for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCallISV0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    TMockLtsyCallData0 mockData0(callId, mobileService);
    
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> callParamsPckg(callParams);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData, KErrNotSupported);
    call.AnswerIncomingCallISV(requestStatus, callParamsPckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);
    iMockLTSY.CompleteL(EMobileCallAnswerISV, KErrGeneral, expectData);
    call.AnswerIncomingCallISV(requestStatus, callParamsPckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::AnswerIncomingCallISV when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);
    iMockLTSY.CompleteL(EMobileCallAnswerISV, KErrNone, expectData);
    call.AnswerIncomingCallISV(requestStatus, callParamsPckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// close incoming call
    CloseIncomingCallL(call, callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::AnswerIncomingCallISV
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallAnswerISV, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST to increase coverage 
 	//-------------------------------------------------------------------------

	RMobileCall call2;
	CleanupClosePushL(call2);
    TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
    errorCode = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);
    iMockLTSY.CompleteL(EMobileCallAnswerISV, KErrNone, expectData);
    call.AnswerIncomingCallISV(requestStatus, callParamsPckg);

    call2.AnswerIncomingCallISV(requestStatus2, callParamsPckg);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	call.Close();
	CloseIncomingCallL(call2, callId, mobileService);
	
 	//-------------------------------------------------------------------------
	// TEST : to increase CMmVoiceCallTsy::AnswerIncomingCall coverage
	// covers "else if( activeCall && 
	//                  ( !( KETelExt3rdPartyV1 == activeCall->GetExtensionId() ) &&
	//                  ( KETelExt3rdPartyV1 == extensionId ) ) )" condition
 	//-------------------------------------------------------------------------

	mobileCallStatus = RMobileCall::EStatusConnected;

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
    call.AnswerIncomingCallISV(requestStatus, callParamsPckg);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // expectData, completeData, call, line, call2
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAICI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::AnswerIncomingCallISV for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::AnswerIncomingCallISV for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCallISV0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    TMockLtsyCallData0 mockData0(callId, mobileService);
	
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> callParamsPckg(callParams);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AnswerIncomingCallISV
 	//-------------------------------------------------------------------------
 	
	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);

    call.AnswerIncomingCallISV(requestStatus, callParamsPckg);

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);

	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
	
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
	
    call.CancelAsyncRequest(EMobileCallAnswerISV);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(5); // expectData, completeData, this, call, line
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAICI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::AnswerIncomingCallISV for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::AnswerIncomingCallISV for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCallISV0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	RMobileCall call1;
	CleanupClosePushL(call1);

	RMobileCall call2;
	CleanupClosePushL(call2);
	
	TInt errorCode = KErrNone;

	TInt callId1 = 1;
	TName incomingCallName1;

	TInt callId2 = 2;
	TName incomingCallName2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	// open new line1 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line1, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	// open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AnswerIncomingCall
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call1
	errorCode = CreateIncomingCallL(line1, callId1, incomingCallName1,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call1.OpenExistingCall(line1, incomingCallName1);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
	// create new incoming call2
	errorCode = CreateIncomingCallL(line2, callId2, incomingCallName2,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData01(callId1, mobileService);
    expectData.Close();
    mockData01.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);
    iMockLTSY.CompleteL(EMobileCallAnswerISV, KErrNone, expectData);

	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call1.AnswerIncomingCallISV(requestStatus1, mmParamsPckgV1);

    TMockLtsyCallData0 mockData02(callId2, mobileService);
    expectData.Close();
    mockData02.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);
    iMockLTSY.CompleteL(EMobileCallAnswerISV, KErrNone, expectData);

    call2.AnswerIncomingCallISV(requestStatus2, mmParamsPckgV1);

    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(4); // line1, line2, call1, call2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAICI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallISV with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallISV and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCallISV0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileCall::AnswerIncomingCallISV
 	//-------------------------------------------------------------------------
		
 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName,
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------

    TMockLtsyCallData0 mockData0(callId, mobileService);
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallAnswerISV, expectData);

	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCallISV(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CAICI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallISV for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallISV for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCallISV0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyDataLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------

	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    call.AnswerIncomingCallISV(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, this, line, call
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CAICI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallISV for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallISV for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestAnswerIncomingCallISV00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RLine line;
	RMobileCall call;
	CleanupClosePushL(call);
	
	TInt errorCode = KErrNone;
	TInt callId = 1;
	TName incomingCallName;

    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	// open new line for KMmTsyFaxLineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
	
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
    TMockLtsyCallData0 mockData0(callId, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
 	//-------------------------------------------------------------------------
    expectData.Close();
    mockData0.SerialiseL(expectData);
    call.AnswerIncomingCallISV(requestStatus, mmParamsPckgV1);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, this, call, line
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CDI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialISV for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialISV for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDialISV0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RMobileLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line for KMmTsyVoice1LineName
    errorCode = line.Open(iPhone, KMmTsyVoice1LineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    _LIT(KDoubleColon, "::");
    
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KMmTsyVoice1LineName);
    name.Append(KDoubleColon);
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
			
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
	callParams.iIdRestrict = RMobileCall::ESendMyId;
	callParams.iAutoRedial = ETrue;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> pckgCallParams(callParams);
    
	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	RMmCustomAPI::TEmerNumberCheckMode emerNumberCheckMode;
	emerNumberCheckMode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal;
    emerNumberCheckMode.iNumber.Copy(KPhoneNumber);	
	
	RMmCustomAPI::TMobileTelNumber* telNumberPtr = NULL;
	
    TMockLtsyData1<RMmCustomAPI::TEmerNumberCheckMode> expectLtsyData1(emerNumberCheckMode);
    
	TMockLtsyCallData2<RMobileCall::TEtel3rdPartyMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		expectLtsyData2(0, mobileService, callParams, callInfo);  
			
	TMockLtsyCallData0 completeLtsyData1(callId, mobileService);	  
    
	TMockLtsyData1< RMmCustomAPI::TMobileTelNumber* > completeLtsyData2(telNumberPtr);	   
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RCall::Dial
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    completeLtsyData1.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallDialISV, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST A: Try to make RMobileCall::DialISV request in case where
	// CustomTSY is not opened. This should fail because in DialISV case TSY 	
	// checks dialling number type using CustomTSY.
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.DialISV(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::DialISV when result is not cached.
 	//-------------------------------------------------------------------------

 	//-------------------------------------------------------------------------
	// CustomTSY must be open before DialISV is requested
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
		
    RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;		
    currentlyRetrievedCache.iCacheId	= 1;
    currentlyRetrievedCache.iRecordId	= 0;
    TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyData2(currentlyRetrievedCache);
    expectData.Close();
    ltsyData2.SerialiseL(expectData);			
    iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, expectData);	
			
	RMmCustomAPI customAPI;
	customAPI.Open(iPhone);
	CleanupClosePushL(customAPI);
	AssertMockLtsyStatusL();
    //-------------------------------------------------------------------------	

    completeData.Close();
	completeLtsyData2.SerialiseL(completeData);    
    iMockLTSY.CompleteL(ECustomCheckEmergencyNumberIPC, KErrNone, completeData); 
    
    expectData.Close();
    expectLtsyData1.SerialiseL(expectData);
    iMockLTSY.ExpectL(ECustomCheckEmergencyNumberIPC, expectData);

	expectData.Close();
	expectLtsyData1.SerialiseL(expectData);
	iMockLTSY.ExpectL(EMobileCallDialISV, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);	
	
	completeData.Close();
	completeLtsyData1.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobileCallDialISV, KErrNone, completeData);
	
	call.DialISV(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	call.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // this, expectData, completeData, line, call, customAPI
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CDI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialISV for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialISV for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDialISV0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RMobileLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line for KMmTsyDataLineName
    errorCode = line.Open(iPhone, KMmTsyDataLineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    _LIT(KDoubleColon, "::");
    
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KMmTsyDataLineName);
    name.Append(KDoubleColon);
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
			
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
    
	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty;
	
	TMockLtsyCallData2<RMobileCall::TEtel3rdPartyMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> pckgCallParams(callParams);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.DialISV(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialISV
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallDialISV, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CDI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialISV for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialISV for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDialISV00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RMobileLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// open new line for KMmTsyDataLineName
    errorCode = line.Open(iPhone, KMmTsyFaxLineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    _LIT(KDoubleColon, "::");
    
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KMmTsyFaxLineName);
    name.Append(KDoubleColon);
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
			
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
    
	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty;
	
	TMockLtsyCallData2<RMobileCall::TEtel3rdPartyMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> pckgCallParams(callParams);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.DialISV(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialISV
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallDialISV, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line
	
	}


/**
@SYMTestCaseID BA-CTSY-CCON-CDCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialCallControl for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialCallControl for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDialCallControl0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RMobileLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// Open new line for KMmTsyVoice1LineName
    errorCode = line.Open(iPhone, KMmTsyVoice1LineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    _LIT(KDoubleColon, "::");
    
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KMmTsyVoice1LineName);
    name.Append(KDoubleColon);
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
			
	RMobileCall::TMobileCallParamsV7 callParams;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	TPckg<RMobileCall::TMobileCallParamsV7> pckgCallParams(callParams);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
        
    call.DialCallControl(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	
	// This is not a defect. DialCallControl fails because feature is not 
	// supported by CTSY.
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	
	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // call

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialCallControl
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallDialCallControl, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CDCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialCallControl for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialCallControl for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDialCallControl0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RMobileLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// Open new line for KMmTsyDataLineName
    errorCode = line.Open(iPhone, KMmTsyDataLineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    _LIT(KDoubleColon, "::");
    
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KMmTsyDataLineName);
    name.Append(KDoubleColon);
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
			
	RMobileCall::TMobileCallParamsV7 callParams;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	TPckg<RMobileCall::TMobileCallParamsV7> pckgCallParams(callParams);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

	call.DialCallControl(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	
	// This is not a defect. DialCallControl fails because feature is not 
	// supported by CTSY.	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // call

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialISV
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallDialCallControl, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, call
	
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CDCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialCallControl for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialCallControl for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestDialCallControl00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RMobileLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	// Open new line for KMmTsyFaxLineName
    errorCode = line.Open(iPhone, KMmTsyFaxLineName);
    CleanupClosePushL(line);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    _LIT(KDoubleColon, "::");
    
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KMmTsyFaxLineName);
    name.Append(KDoubleColon);
	   	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
			
	RMobileCall::TMobileCallParamsV7 callParams;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);
	
	TPckg<RMobileCall::TMobileCallParamsV7> pckgCallParams(callParams);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
	call.DialCallControl(requestStatus, pckgCallParams, KPhoneNumber);
	User::WaitForRequest(requestStatus);
	
	// This is not a defect. DialCallControl fails because feature is not 
	// supported by CTSY.	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //call

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialISV
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    expectData.Close();
    mockData0.SerialiseL(expectData);
	iMockLTSY.CompleteL(EMobileCallDialCallControl, KErrNone, expectData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // expectData, completeData, line, this
	}

/*
 * Test case where answering fails because automatic hold does not work.
 * 
 * @param aLineName Line name
 * @return KErrNone if successful, otherwise another of the system-wide error 
 */
TInt CCTsyCallControlFU::TestAnsweringFailsBecauseHoldDoesNotWorkL( const TDesC& aLineName )
	{ 
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));	    
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RMobilePhone phone2;
	TInt ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line1;
	RLine line2;
	
	//Open new line1 for aLineName
	TInt errorCode = OpenNewLineLC(iPhone, line1, aLineName);
	ASSERT_EQUALS(KErrNone, errorCode);

	//Open new line2 for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(phone2, line2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);	
	
	RMobileCall call1;
	CleanupClosePushL(call1);

	RMobileCall call2;
	CleanupClosePushL(call2);
	
	_LIT(KPhoneNumber, "101632960000"); 
	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService1;	
    RMobilePhone::TMobileService mobileService2 = RMobilePhone::EVoiceService;

	TInt callId1 = 1;
	TName callName1;

	TInt callId2 = 2;
	TName callName2;

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
 	// Create first call outgoing voice or data call
	
   	errorCode = call1.OpenNewCall(line1, callName1);
    ASSERT_EQUALS(KErrNone, errorCode);	
    
	if ( !aLineName.Compare( KMmTsyDataLineName ) )
        { 
        //Dial data call
        mobileService1 = RMobilePhone::ECircuitDataService;
                    
        CallGetMobileCallInfoL(callId1, mobileService1, KPhoneNumber);    

    	RMobileCall::TMobileDataCallParamsV1 callParams;
        callParams.iInterval = 4;
        callParams.iCug.iCugIndex = 0xFFFF;
        
    	RMobileCall::TMobileCallInfoV8 callInfo;
    	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
    	callInfo.iService = mobileService1;
    	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating ;
    	
    	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
    		mockData0(0, mobileService1, callParams, callInfo);
    
    	TPckg<RMobileCall::TMobileDataCallParamsV1> pckgCallParams(callParams);   
    
    	mockData0.SerialiseL(expectData);
      	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNone);

    	call1.Dial(requestStatus, pckgCallParams, KPhoneNumber);                                     	
        }
    else
        {
        //Dial voice call
        mobileService1 = RMobilePhone::EVoiceService;
            
        CallGetMobileCallInfoL(callId1, mobileService1, KPhoneNumber);      
            
    	RMobileCall::TMobileCallParamsV1 callParams;
        callParams.iInterval = 4;
        callParams.iCug.iCugIndex = 0xFFFF;
        TPckg<RMobileCall::TMobileCallParamsV1> pckgCallParams(callParams);
      
    	RMobileCall::TMobileCallInfoV8 callInfo;
    	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
    	callInfo.iService = mobileService1;
    	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
    	
    	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
    		mockData1(0, mobileService1, callParams, callInfo);
    
    	mockData1.SerialiseL(expectData);
    	iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNone);

    	call1.Dial(requestStatus, pckgCallParams, KPhoneNumber);  
        }
              	
    //Change call1 status to connected
    mobileCallStatus = RMobileCall::EStatusDialling;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData2(callId1, mobileService1, mobileCallStatus);
    completeData.Close();
    mockData2.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
 	User::WaitForRequest(mockLtsyStatus); 	
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
	    
    mobileCallStatus = RMobileCall::EStatusConnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData3(callId1, mobileService1, mobileCallStatus);
    completeData.Close();
    mockData3.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData); 	
 	User::WaitForRequest(mockLtsyStatus);
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  	 
       	   	    
    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData4(callId1, mobileService1, mobileCallStatus);
    completeData.Close();
    mockData4.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
 	User::WaitForRequest(mockLtsyStatus);
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int()); 

	if ( !aLineName.Compare( KMmTsyDataLineName ) )
        { 
        //Dial request for voice call is completed automatically by CTSY when call goes to connected state
        //Complete here data call Dial.
      	TMockLtsyCallData0 mockData5(callId1, mobileService1);
      	mockData5.SerialiseL(completeData);
      	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);         
        }
         
    User::WaitForRequest(requestStatus);   
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
  
    //Check that call1 is active
    call1.GetMobileCallStatus(mobileCallStatus);
    ASSERT_EQUALS(RMobileCall::EStatusConnected, mobileCallStatus);
	AssertMockLtsyStatusL(); 
	
	//-------------------------------------------------------------------------
	
	
 	//-------------------------------------------------------------------------
	// Create second incoming voice call.
	
	mobileCallStatus = RMobileCall::EStatusRinging;
	errorCode = CreateIncomingCallL(line2, callId2, callName2,
	                                mobileService2, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
		
    errorCode = call2.OpenExistingCall(line2, callName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------	  	
    	
 	//-------------------------------------------------------------------------
	// TEST A: AnswerIncomingCall for call2 is completed with error KErrMMEtelCallForbidden 
	// because automatic hold for call1 fails.
 	//-------------------------------------------------------------------------
	
    //There is now one active call and one ringing call. Cellular software sets
    //active call1 to hold automatically when ringing call is answered. Normally this operation
    //succeeds. In DoCoMo network hold operation fails in this situation if call waiting is
    //disabled. 
    
    //Background: Incoming call indication should not come from network if there is already an active call 
    //and call waiting is disabled. However in DoCoMo network incoming call indication comes 
    //through but answering to this call fails because hold operation for active call does not work.
    
    //Expected functionality: In this case CTSY should complete AnswerIncomingCall request with 
    //error and active call should stay in active state.	
	
    TMockLtsyCallData0 mockData6(callId2, mobileService2);
    expectData.Close();
    mockData6.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallAnswer, expectData);   
       
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);       
    call2.AnswerIncomingCall(requestStatus, mmParamsPckgV1);    
    
    //Incoming call goes to answering state.
    mobileCallStatus = RMobileCall::EStatusAnswering;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData7(callId2, mobileService2, mobileCallStatus);
    completeData.Close();
    mockData7.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);    
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);  
    User::WaitForRequest(mockLtsyStatus);    
    
    //Network rejects hold operation for active call. 
    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData8(callId1, mobileService1, mobileCallStatus);
    completeData.Close();
    mockData8.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrMMEtelCallForbidden, completeData); 
    
    //AnswerIncomingCall should be completed with error.
    User::WaitForRequest(requestStatus);    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrMMEtelCallForbidden, requestStatus.Int());	
    AssertMockLtsyStatusL();
    
    //Check that call1 is still in active state.
    call1.GetMobileCallStatus(mobileCallStatus);
    ASSERT_EQUALS(RMobileCall::EStatusConnected, mobileCallStatus);
	AssertMockLtsyStatusL();
	    
 	//-------------------------------------------------------------------------
	// Hangup calls
 	
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData9(callId2, mobileService2, hangUpCause, autoStChangeDisable);	
    expectData.Close();
    mockData9.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);	

	call2.HangUp(requestStatus);

    //Change call2 status to disconnecting
    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData10(callId2, mobileService2, mobileCallStatus);
    completeData.Close();
    mockData10.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
 	User::WaitForRequest(mockLtsyStatus); 	
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int()); 
 	
    //Change call2 status to idle
    mobileCallStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData11(callId2, mobileService2, mobileCallStatus);
    completeData.Close();
    mockData11.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
 	User::WaitForRequest(mockLtsyStatus); 	
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());   	
	
	//HangUp complete for call2    
    User::WaitForRequest(requestStatus);   
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
    
    // Only the voice line is needing this flag.
	if ( !aLineName.Compare( KMmTsyDataLineName ) )
		{
		autoStChangeDisable = EFalse;
		}
	else
		{
		autoStChangeDisable = ETrue;
		}
	
    TMockLtsyCallData2<TInt, TBool> mockData12(callId1, mobileService1, hangUpCause, autoStChangeDisable);	
    expectData.Close();
    mockData12.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);	

	call1.HangUp(requestStatus);

    //Change call1 status to disconnecting
    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData13(callId1, mobileService1, mobileCallStatus);
    completeData.Close();
    mockData13.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
 	User::WaitForRequest(mockLtsyStatus); 	
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int()); 
 	
    //Change call1 status to idle
    mobileCallStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData14(callId1, mobileService1, mobileCallStatus);
    completeData.Close();
    mockData14.SerialiseL(completeData);   
    iMockLTSY.NotifyTerminated(mockLtsyStatus);      
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    
    // when call becomes idle, remaining duration of the call is added to life time param in LTSY:		
	TUint32 duration2 = 4;	// this is a dummy value, which won't be checked by mocksy engine
	TMockLtsyData1<TUint32> ltsyData2( duration2 );
	expectData.Close();
	ltsyData2.SerialiseL(expectData);	
	iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, expectData);	            
    
 	User::WaitForRequest(mockLtsyStatus); 	
 	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());   	

	//HangUp complete for call1     
    User::WaitForRequest(requestStatus);   
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
    
    //-------------------------------------------------------------------------   	    
	
	// Done !     
	CleanupStack::PopAndDestroy(8, this); // call2, call1, line2, line1, phone2, 
	                                      // completeData, expectData, this	
	return KErrNone;
    }

/*
 * Opens a phone subsession by name.
 * 
 * @param aPhone The phone from which the line is to be opened.
 * @param On return, contains the new line. 
 * @param aName The name of the phone to be opened
 * @return KErrNone if successful, otherwise another of the system-wide error 
 */

TInt CCTsyCallControlFU::OpenNewLineLC(RMobilePhone& aPhone, RLine& aLine, const TDesC& aLineName)
    {
    TInt errorCode = KErrNone;

    // Open new line
    errorCode = aLine.Open(aPhone, aLineName);
    CleanupClosePushL(aLine);
    
    return errorCode;
    }

/*
 * This function creates an incomming call for line and returns a name of call. 
 * 
 * @param aLine The line from which the call is to be created.
 * @param aCallId ID of call.
 * @param aIncomingCallName On return, contains the new name of incomming call.
 * @param aMobileService Applicability of request to a mobile service group.
 * @param aMobileCallStatus Describes the possible call states.
 * @return KErrNone if successful, otherwise another of the system-wide error 
 */

TInt CCTsyCallControlFU::CreateIncomingCallL(RLine& aLine, 
                                            const TInt aCallId, 
                                            TName& aIncomingCallName,
                                            RMobilePhone::TMobileService aMobileService,
                                            RMobileCall::TMobileCallStatus aMobileCallStatus)
	{
    RBuf8 completeData;

    TRequestStatus requestNotify;
    TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// call CMmVoiceLineTsy::CompleteNotifyIncomingCall for set iCallId > 0.
	
	aLine.NotifyIncomingCall(requestNotify, aIncomingCallName);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(aCallId, aMobileService, mobileCallInfo);
    CleanupClosePushL(completeData);
	mockCallData1.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestNotify.Int());

 	//-------------------------------------------------------------------------
	
 	//-------------------------------------------------------------------------
    // call CMmVoiceCallTsy::CompleteNotifyStatusChange for set 
    // iMobileCallStatus = RMobileCall::EStatusRinging.
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(aCallId, aMobileService, aMobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // completeData 
	
	return KErrNone;
	}

/*
 * This function closes an incomming call by aCallId. 
 * 
 * @param aCall The incomming call which is to be closed.
 * @param aCallId ID of call.
 * @param aMobileService Applicability of request to a mobile service group.
 */

void CCTsyCallControlFU::CloseIncomingCallL(RCall& aCall, const TInt aCallId,
                                           RMobilePhone::TMobileService aMobileService)
	{
    RBuf8 expectData;
    CleanupClosePushL(expectData);

	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(aCallId, aMobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
	aCall.Close();
	AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(1); // expectData 
	}

//  ExpectL and CopmleteL must precede this function.
void CCTsyCallControlFU::CallGetMobileCallInfoL(TInt aCallId, 
                                               RMobilePhone::TMobileService aService, 
                                               const TDesC &aTelNumber)
	{
	_LIT(KNullDesC , "");
	
	RBuf8 data;
	CleanupClosePushL(data);

	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(aTelNumber);
	callInfo.iService = aService;
	callInfo.iEmergency = EFalse;
	callInfo.iRemoteParty.iCallingName = KNullDesC;
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iForwarded = EFalse;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;

	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(aCallId, aService, callInfo);
	callInfoData.SerialiseL(data);
		
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    
    CleanupStack::PopAndDestroy(1);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for RLine::EnumerateCall for voice
@SYMTestPriority High
@SYMTestActions Invokes RLine::EnumerateCall for voice
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestEnumerateCall0001L()
	{
	AuxEnumerateCallL(KMmTsyVoice1LineName);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CEC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for RLine::EnumerateCall for data
@SYMTestPriority High
@SYMTestActions Invokes RLine::EnumerateCall for data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestEnumerateCall0006L()
	{
	AuxEnumerateCallL(KMmTsyDataLineName);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CEC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for RLine::EnumerateCall for fax
@SYMTestPriority High
@SYMTestActions Invokes RLine::EnumerateCall for fax
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestEnumerateCall00011L()
	{
	AuxEnumerateCallL(KMmTsyFaxLineName);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CNHC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for RLine::NotifyHookChange for voice
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyHookChange for voice
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestNotifyHookChange0001L()
	{
	AuxNotifyHookChangeL(KMmTsyVoice1LineName);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CNHC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for RLine::NotifyHookChange for data
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyHookChange for data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestNotifyHookChange0006L()
	{
	AuxNotifyHookChangeL(KMmTsyDataLineName);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CNHC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for RLine::NotifyHookChange for fax
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyHookChange for fax
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestNotifyHookChange00011L()
	{
	AuxNotifyHookChangeL(KMmTsyFaxLineName);
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CCNSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test increase coverage for CMmLineTsy::CompleteNotifyStatusChange for voice
@SYMTestPriority High
@SYMTestActions Invokes CMmLineTsy::CompleteNotifyStatusChange for voice
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestCompleteNotifyStatusChange0001L()
	{
	AuxCompleteNotifyStatusChangeL(KMmTsyVoice1LineName);
	}

void CCTsyCallControlFU::AuxEnumerateCallL(const TDesC& aName)
	{

	if (aName.Compare(KMmTsyVoice1LineName) != 0 && 
		aName.Compare(KMmTsyDataLineName)   != 0 &&
		aName.Compare(KMmTsyFaxLineName)    != 0)
		return;

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLine line;

	// open new line for KMmTsyDataLineName
	TInt errorCode = OpenNewLineLC(iPhone, line, aName);
	ASSERT_EQUALS(KErrNone, errorCode);

	TInt index(0);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::EnumerateCall when result is not cached.
 	//-------------------------------------------------------------------------

	errorCode = line.EnumerateCall(index);
	ASSERT_EQUALS(KErrNone, errorCode);

	CleanupStack::PopAndDestroy(2, this); // line, this
	}

void CCTsyCallControlFU::AuxNotifyHookChangeL(const TDesC& aName)
	{

	if (aName.Compare(KMmTsyVoice1LineName) != 0 && 
		aName.Compare(KMmTsyDataLineName)   != 0 &&
		aName.Compare(KMmTsyFaxLineName)    != 0)
		return;

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLine line;

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RCall::THookStatus hookStatus;

	TInt errorCode = OpenNewLineLC(iPhone, line, aName);
	ASSERT_EQUALS(KErrNone, errorCode);

	line.NotifyHookChange(requestStatus, hookStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	line.NotifyHookChangeCancel();

	CleanupStack::PopAndDestroy(2, this); // line, this
	}

void CCTsyCallControlFU::AuxCompleteNotifyStatusChangeL(const TDesC& aName)
	{

	RMobilePhone::TMobileService mobileService = RMobilePhone::EServiceUnspecified;

	if (aName.Compare(KMmTsyVoice1LineName) == 0)
		{
		mobileService = RMobilePhone::EVoiceService;
		}
	else if (aName.Compare(KMmTsyDataLineName) == 0)
		{
		mobileService = RMobilePhone::ECircuitDataService;
		}
	else if (aName.Compare(KMmTsyFaxLineName) == 0)
		{
		mobileService = RMobilePhone::EFaxService;
		}
	else
		return;

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLine line;
	RCall call;
	RMobileCall::TMobileCallStatus mobileCallStatus(RMobileCall::EStatusUnknown);
	TName callName;
	TInt callId(1);
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestNotify;

	TInt errorCode = OpenNewLineLC(iPhone, line, aName);
	ASSERT_EQUALS(KErrNone, errorCode);

	//-------------------------------------------------------------------------

	line.NotifyIncomingCall(requestNotify, callName);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
	mockCallData1.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestNotify.Int());

	RArray<RMobileCall::TMobileCallStatus> arrayCallStatus;
	CleanupClosePushL(arrayCallStatus);
	
	arrayCallStatus.AppendL(RMobileCall::EStatusDialling);
	arrayCallStatus.AppendL(RMobileCall::EStatusAnswering);
	arrayCallStatus.AppendL(RMobileCall::EStatusConnecting);
	
	for (TInt i = 0; i < arrayCallStatus.Count(); i++)
		{
		
		mobileCallStatus = arrayCallStatus[i];
		
		TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
		completeData.Close();
		mockCallData2.SerialiseL(completeData);

		iMockLTSY.NotifyTerminated(mockLtsyStatus);
		iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
		User::WaitForRequest(mockLtsyStatus);
		AssertMockLtsyStatusL();
		ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
		}

	CleanupStack::PopAndDestroy(4, this); // line, this, completeName
										  // arrayCallStatus
	}

/**
@SYMTestCaseID BA-CTSY-CCON-CNUA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test: generating a call with duration to update life time param of total call time
@SYMTestPriority High
@SYMTestActions Invokes updating life time param with every specified interval of sec elapsed 
                and with the reminder of time when call status becomes idle (in case of the conference call
                it's the last call)
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUpdateLifeTime0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	TInt errorCode = KErrNone;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
	
	// open new line and call for KMmTsyVoice1LineName
	errorCode = OpenNewLineLC(iPhone, line, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode);
		
    TName name;
    
    // we are preparing to dial the call:
	
	_LIT(KPhoneNumber, "101632960000");   	
   	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
			
    RMobileCall::TMobileCallParamsV7 callParams; 
    RMobileCall::TMobileCallParamsV7Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV8 callInfo;   
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval        = 100;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;

    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;  
    callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);

	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		mockData2(0, mobileService, callParams, callInfo);

	TMockLtsyCallData0 mockData0(callId, mobileService);

	RCall::TCallParams callDialParams;
	TPckg<RCall::TCallParams> pckgCallDialParams(callDialParams);
	callDialParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
	callDialParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
	callDialParams.iInterval        = 100;
	callDialParams.iWaitForDialTone = RCall::EDialToneWait;

	// open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
	
	expectData.Close();
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(EEtelCallDial, expectData);

	CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
	
	completeData.Close();
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // with a notifications we make sure the state of the call will become: connected  
	TRequestStatus reqState1, reqState2, reqState3;
	RMobileCall::TMobileCallStatus status;  
    call.NotifyMobileCallStatusChange(reqState1, status);
    
    // we are dialing the call:
	call.Dial(requestStatus, pckgCallDialParams, KPhoneNumber);
		
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusDialling;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData4.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
				
    User::WaitForRequest(reqState1);            
    call.NotifyMobileCallStatusChange(reqState2, status);
    
    mobileCallStatus = RMobileCall::EStatusConnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData5(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData5.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
                 
    User::WaitForRequest(reqState2);
    call.NotifyMobileCallStatusChange(reqState3, status);
    
    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData6(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData6.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // status call becomes 'connected' here...   
            
    User::WaitForRequest(reqState3);    
	
	// completion of the Dial() request:
	User::WaitForRequest(requestStatus);

    // Here we will trigger call duration of 15sec. Timer interval causing update
    // of life time parameter is set to 10 sec: so we will have a one update caused
    // by firing timer. The remainding duration ~5sec will be triggered by
    // the call status becoming idle.
	TUint32 duration = 10;	
	TMockLtsyData1<TUint32> ltsyData( duration );
	expectData.Close();
	ltsyData.SerialiseL(expectData);
	// firing timer will cause this:	
	iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, expectData);
	
	// 15 sec call duration was chosen to trigger timer firing 1 update plus last update,
	// when call becomes idle...	
	User::After(15000000);

    // completion of the timer firing event:	
	TRequestStatus mockLtsyStatus2;
	iMockLTSY.NotifyTerminated(mockLtsyStatus2);
	iMockLTSY.CompleteL(EMmTsyUpdateLifeTimeIPC, KErrNone);	
	
	User::WaitForRequest(mockLtsyStatus2);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus2.Int());
    AssertMockLtsyStatusL();
			
	//
	
	// preparing call state to become 'idle' eventually:
	TRequestStatus notifyCallStatus;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	call.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

    // hangup call is going to LTSY:
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData3(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    expectData.Close();                                             
    mockData3.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
                                                     
	TRequestStatus hangupStatus;
	call.HangUp(hangupStatus);  //<--------- hang up triggers status changes
			
    callStatus = RMobileCall::EStatusDisconnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData7(callId, mobileService, callStatus);
    completeData.Close();
    mockCallData7.SerialiseL(completeData);
    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());        	
	
	User::WaitForRequest(notifyCallStatus);	
	ASSERT_EQUALS(KErrNone, notifyCallStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusDisconnecting, callStatus); // <------------disconnecting status
	
	call.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);
	
    callStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData8(callId, mobileService, callStatus);
    completeData.Close();
    mockCallData8.SerialiseL(completeData);
        
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);

    // when call becomes idle, remaining duration of the call is added to life time param in LTSY:		
	TUint32 duration2 = 4;	// this is a dummy value, which won't be checked by mocksy engine
	TMockLtsyData1<TUint32> ltsyData2( duration2 );
	expectData.Close();
	ltsyData2.SerialiseL(expectData);	
	iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, expectData);	    
					
	User::WaitForRequest(mockLtsyStatus);
	// we did set a sleep time for 15sec, but after stopping timer it may give us 14 sec or 15 sec
	// we can not determine the correct value for the mockLTSY;
	// we tell mocksy engine to ignore duration param value	
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());        	
    	
	User::WaitForRequest(notifyCallStatus);
	ASSERT_EQUALS(KErrNone, notifyCallStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus); // <-------- idle status
			
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int()); // <--- then hang up is finished
	
	call.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);
	
	//
				
	TRequestStatus mockLtsyStatus3;
	iMockLTSY.NotifyTerminated(mockLtsyStatus3);
	iMockLTSY.CompleteL(EMmTsyUpdateLifeTimeIPC, KErrNone);	
	
	User::WaitForRequest(mockLtsyStatus3);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus3.Int());
	
	call.Close();	
			
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4, this);
	}



