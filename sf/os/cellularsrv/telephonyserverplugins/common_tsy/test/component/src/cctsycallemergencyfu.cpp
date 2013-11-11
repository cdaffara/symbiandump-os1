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
// The TEFUnit test suite for CallEmergency in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallemergencyfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "e32property.h"

#include <ssm/ssmadaptationcli.h>

// for keys definition. These keys are  used to manage SSM_Stub behaviour
#include "ssmstub_properties.h"

#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT
  const TUid KThisUID = {0x101F777C};
#endif

CTestSuite* CCTsyCallEmergencyFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0001cL);
	
	// tests for SSM plugin (System State Manager)
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0001dL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0001eL);

	//test for dialling an emergency call when modem boot sequence has not run
	ADD_TEST_STEP_ISO_CPP(CCTsyCallEmergencyFU, TestDialEmergencyCall0001fL);
	END_SUITE;
	}


void CCTsyCallEmergencyFU::FillInEmergencyCallInfo(RMobileCall::TMobileCallInfoV1 &aInfo,
		const TInt aCallId,
		const TName& aCallName,
		const RMobilePhone::TMobileService aService)
	{
	aInfo.iService = aService;
	aInfo.iStatus = RMobileCall::EStatusIdle;
	aInfo.iCallId = aCallId;
	aInfo.iEmergency = ETrue;
	aInfo.iCallName = aCallName;
	switch ( aService )
		{
		case RMobilePhone::EAuxVoiceService:
			aInfo.iLineName = KMmTsyVoice2LineName;
			break;
		case RMobilePhone::EVoiceService:
		default:
			aInfo.iLineName = KMmTsyVoice1LineName;
			break;
		}
	aInfo.iValid = RMobileCall::KCallId | RMobileCall::KCallEmergency;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	TRequestStatus reqStatus;

	RMobileCall mobileCall;
	TName callName;
	err = mobileCall.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);

	const TInt KCallId = 1;
	RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
	RMobileCall::TMobileCallInfoV1 callInfo;
	FillInEmergencyCallInfo(callInfo, KCallId, callName, KMobileService);
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId, KMobileService, callInfo);
	
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(KCallId, KMobileService, mobileCallStatus);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNotSupported);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B1: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGeneral, data);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B2: failure on completion of pending request from LTSY->CTSY
	// when LTSY returns EStatusIdle call status
 	//-------------------------------------------------------------------------

	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotReady, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C1: Successful completion request of
	// RMobileCall::DialEmergencyCall.
 	//-------------------------------------------------------------------------
	
	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY 
	// when emergency call is allready dialed
 	//-------------------------------------------------------------------------

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotReady, reqStatus.Int());
	AssertMockLtsyStatusL();

	PrepareCallCloseL(KCallId, KMobileService);
	CleanupStack::PopAndDestroy(&mobileCall);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMobileCall::DialEmergencyCall in OOM condition.
 	//-------------------------------------------------------------------------
	
	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

	err = iTelServer.SetPriorityClientV2();
	ASSERT_EQUALS(KErrNone, err);

	err = mobileCall.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);
	
	// simulate a heap allocation failure
	User::__DbgSetAllocFail(EFalse, RAllocator::EDeterministic, 1);
	User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);

	// test RMobileCall::DialEmergencyCall in out-of-memory condition
	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);

	// turn back a heap allocation possibility
	User::__DbgSetAllocFail(EFalse, RAllocator::ENone, 1);
	User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);

   
	// check results
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialEmergencyCall
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	TMockLtsyCallData0 completeEmptyLtsyData(KCallId, KMobileService);
	data.Close();
	completeEmptyLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallDialEmergencyCall, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// increase coverage

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    data.Close();
    mobileCallStatus = RMobileCall::EStatusIdle;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::DialEmergencyCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::DialEmergencyCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	TRequestStatus reqStatus;
	TRequestStatus mockLtsyStatus;

	TName callName;
	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);

	const RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
	const TInt KCallId1 = 1;
	RMobileCall::TMobileCallInfoV1 callInfo;
	FillInEmergencyCallInfo(callInfo, KCallId1, callName, KMobileService);
	callInfo.iValid = RMobileCall::KCallId | RMobileCall::KCallEmergency;
	
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId1, KMobileService, callInfo);
	
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> expHangUpData(KCallId1, KMobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);

	TMockLtsyCallData0 completeEmptyLtsyData(KCallId1, KMobileService);

	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(KCallId1, KMobileService, mobileCallStatus);

	//-------------------------------------------------------------------------
    // Test cancelling of RMobileCall::DialEmergencyCall
    // before than LTSY sends any completions
    //-------------------------------------------------------------------------

    RMobileCall mobileCall1;
    err = mobileCall1.OpenNewCall(line, callName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileCall1);

    numberLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);
    
    mobileCall1.DialEmergencyCall(reqStatus, number);
    User::After(1000); // Delay needed to reach DialEmergencyCall request to LTSY
    mobileCall1.CancelAsyncRequest(EMobileCallDialEmergencyCall);
    
    data.Close();
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);      
    
    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling; 
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
        
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, hangUpCause, data);
      
    data.Close();
    expHangUpData.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data, KErrNone);
       
    User::WaitForRequest(mockLtsyStatus);
   
    data.Close();
    mobileCallStatus = RMobileCall::EStatusIdle;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, hangUpCause, data);
    User::WaitForRequest(reqStatus);
    
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    CleanupStack::PopAndDestroy(&mobileCall1);
    AssertMockLtsyStatusL();
	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::DialEmergencyCall
	// after callId is changed
 	//-------------------------------------------------------------------------

	RMobileCall mobileCall2;
	err = mobileCall2.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall2);

    data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data, 10);

    mobileCall2.DialEmergencyCall(reqStatus, number);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    data.Close();
	callInfo.iCallName = callName;
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	data.Close();
	expHangUpData.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data, KErrNone);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusIdle;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, hangUpCause, data, 10);

	mobileCall2.CancelAsyncRequest(EMobileCallDialEmergencyCall);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	CleanupStack::PopAndDestroy(&mobileCall2);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::DialEmergencyCall
	// after call status is changed to dialling
 	//-------------------------------------------------------------------------

	RMobileCall mobileCall3;
	err = mobileCall3.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall3);

    data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

	mobileCall3.DialEmergencyCall(reqStatus, number);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    data.Close();
	callInfo.iCallName = callName;
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	ChangeCallStatusL(KCallId1 , KMobileService, RMobileCall::EStatusDialling);

	data.Close();
	expHangUpData.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data, KErrNone);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusIdle;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, hangUpCause, data);

	mobileCall3.CancelAsyncRequest(EMobileCallDialEmergencyCall);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	CleanupStack::PopAndDestroy(&mobileCall3);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::DialEmergencyCall
	// after call status is changed to connecting
 	//-------------------------------------------------------------------------

	RMobileCall mobileCall4;
	err = mobileCall4.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall4);

    data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

	mobileCall4.DialEmergencyCall(reqStatus, number);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    data.Close();
	callInfo.iCallName = callName;
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	ChangeCallStatusL(KCallId1 , KMobileService, RMobileCall::EStatusDialling);
	ChangeCallStatusL(KCallId1 , KMobileService, RMobileCall::EStatusConnecting);

	data.Close();
	expHangUpData.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data, KErrNone);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, hangUpCause, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusIdle;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, hangUpCause, data);

	mobileCall4.CancelAsyncRequest(EMobileCallDialEmergencyCall);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	CleanupStack::PopAndDestroy(&mobileCall4);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	TRequestStatus reqStatus;

	RMobileCall mobileCall;
	err = mobileCall.OpenNewCall(line);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileCall::DialEmergencyCall
 	//-------------------------------------------------------------------------

	RMobileENStore::TEmergencyNumber number(KNullDesC);
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrArgument);

	TBuf<1> bufNumber(KNullDesC);
	mobileCall.DialEmergencyCall(reqStatus, bufNumber);
	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	TBuf<7> emergNumber = _L("9111120");
	mobileCall.DialEmergencyCall(reqStatus, emergNumber);
	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	
	CleanupStack::PopAndDestroy(4, this); // mobileCall, line, data, this
	}


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::DialEmergencyCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::DialEmergencyCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	TRequestStatus reqStatus;

	RMobileCall mobileCall;
	TName callName;
	err = mobileCall.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);
	RMobileENStore::TEmergencyNumber number2(_L("112"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData2(number2);

	const RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
	const TInt KCallId = 1;
	const TInt KCallId2 = 2;

	RMobileCall::TMobileCallInfoV1 completeDialCallInfo;
	completeDialCallInfo.iService = KMobileService;
	completeDialCallInfo.iStatus = RMobileCall::EStatusIdle;
	completeDialCallInfo.iCallId = KCallId;
	completeDialCallInfo.iCallName = callName;
	completeDialCallInfo.iLineName = KMmTsyVoice1LineName;
	completeDialCallInfo.iValid = RMobileCall::KCallId;
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeDialCallInfoData(KCallId, KMobileService, completeDialCallInfo);

	RMobileCall::TMobileCallInfoV1 callInfo;
	FillInEmergencyCallInfo(callInfo, KCallId, callName, KMobileService);
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId, KMobileService, callInfo);
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData2(KCallId2, KMobileService, callInfo);
	
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(KCallId, KMobileService, mobileCallStatus);
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData2(KCallId2, KMobileService, mobileCallStatus);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::DialEmergencyCall
 	//-------------------------------------------------------------------------

	// Open second client
	RTelServer telServer2;
	err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	RLine line2;
	err = line2.Open(phone2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line2);
	
	RMobileCall mobileCall2;
	TName callName2;
	err = mobileCall2.OpenNewCall(line2, callName2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall2);

	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

	mobileCall.DialEmergencyCall(reqStatus, number);

	data.Close();
	numberLtsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

	data.Close();
    callInfo.iCallName = callName;
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    data.Close();
    callInfo.iCallId = KCallId2;
    callInfo.iCallName = callName2;
    completeCallInfoData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    data.Close();
    completeCallStatusData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    data.Close();
    completeCallStatusData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    data.Close();
    completeCallStatusData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

	ASSERT_EQUALS(KRequestPending, reqStatus.Int());
	TRequestStatus reqStatus2;
	mobileCall2.DialEmergencyCall(reqStatus2, number2);

	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(8, this); // mobileCall2, line2, phone2, telServer2, mobileCall, line, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	RMobileCall mobileCall;
	TName callName;
	err = mobileCall.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileCall::DialEmergencyCall
 	//-------------------------------------------------------------------------

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

	TRequestStatus reqStatus;
	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test: Test timeout of RMobileCall::DialEmergencyCall
	// when callId is changed
 	//-------------------------------------------------------------------------

	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

	const RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
	const TInt KCallId = 1;
	RMobileCall::TMobileCallInfoV1 callInfo;
	FillInEmergencyCallInfo(callInfo, KCallId, callName, KMobileService);
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId, KMobileService, callInfo);
	data.Close();
	completeCallInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::DialEmergencyCall.
 	//-------------------------------------------------------------------------

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	RMobileENStore::TEmergencyNumber number(_L("911"));
	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0001cL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::DialEmergencyCall.
 	//-------------------------------------------------------------------------

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	RMobileENStore::TEmergencyNumber number(_L("911"));
	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(this);
	
	}

/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0001d
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall for voice calls. It tests differen answers from SSMPlugin
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0001dL()
	{
	// if there is no SSM it means that we use SSMStub -> we can use RProperty for emulation   
#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT    

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	TRequestStatus reqStatus;

	RMobileCall mobileCall;
	TName callName;
	err = mobileCall.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);

	const TInt KCallId = 1;
	const RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
	RMobileCall::TMobileCallInfoV1 callInfo;
	FillInEmergencyCallInfo(callInfo, KCallId, callName, KMobileService);
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId, KMobileService, callInfo);
	
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(KCallId, KMobileService, mobileCallStatus);

 	//-------------------------------------------------------------------------
	// TEST: Failure to connect to System State Manager
 	//------------------------------------------------------------------------- 	
 	
 	// Forse  SSMStub's Connect() to return KErrNotSupported  	
 	TInt ssmError(KErrNotSupported);
 	err = RProperty::Define(KThisProcessUID, EErrorForConnect, RProperty::EInt);
 	ASSERT_EQUALS(KErrNone, err); 	
 	RProperty::Set(KThisProcessUID, EErrorForConnect, ssmError);
 	
 	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// Forse  SSMStub's Connect() to return KErrNone 
	ssmError = KErrNone;
	RProperty::Set(KThisProcessUID, EErrorForConnect, ssmError);	
	
	//-------------------------------------------------------------------------
	// TEST: Failure to connect to Activate RF in System State Manager
 	//-------------------------------------------------------------------------
 
 	// Forse  SSMStub's ActivateRfForEmergencyCall() to return KErrGeneral 	
 	ssmError = KErrGeneral;
 	err = RProperty::Define(KThisProcessUID, EErrorForActivate, RProperty::EInt);
 	ASSERT_EQUALS(KErrNone, err); 	
 	RProperty::Set(KThisProcessUID, EErrorForActivate, ssmError);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// Forse  SSMStub's ActivateRfForEmergencyCall() to return KErrNone 
	ssmError = KErrNone;
	RProperty::Set(KThisProcessUID, EErrorForActivate, ssmError);
	
	
	//-------------------------------------------------------------------------
	// TEST: Failure to connect to Deactivate RF in System State Manager
 	//-------------------------------------------------------------------------
	
	// Forse  SSMStub's DeactivateRfForEmergencyCall() to return KErrNotFound 	
 	ssmError = KErrNotFound;
 	err = RProperty::Define(KThisProcessUID, EErrorForDeactivate, RProperty::EInt);
 	ASSERT_EQUALS(KErrNone, err); 	
 	RProperty::Set(KThisProcessUID, EErrorForDeactivate, ssmError);
 	 	
 	
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	// KErrNone (not KErrNotFound) because CTSY doesn't care about result of RF Deactivating
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	
	// Forse  SSMStub's DeactivateRfForEmergencyCall() to return KErrNone 
	ssmError = KErrNone;
	RProperty::Set(KThisProcessUID, EErrorForDeactivate, ssmError);
	
	// Delete defined Properties
	err = RProperty::Delete(KThisProcessUID, EErrorForConnect);
 	ASSERT_EQUALS(KErrNone, err); 
	err = RProperty::Delete(KThisProcessUID, EErrorForActivate);
 	ASSERT_EQUALS(KErrNone, err); 
	err = RProperty::Delete(KThisProcessUID, EErrorForDeactivate);
 	ASSERT_EQUALS(KErrNone, err);  	
 	
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	CleanupStack::PopAndDestroy(2, this); // data, this	
#endif	
	}
	
	
	
/**
@SYMTestCaseID BA-CTSY-CEMG-CDEC-0001e
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialEmergencyCall cancelling becase of SSM delay
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialEmergencyCall for voice calls. It tests cases when there is no answer from SSM
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallEmergencyFU::TestDialEmergencyCall0001eL()
	{
	// if there is no SSM it means that we use SSMStub -> we can use RProperty for emulation   
#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT 
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	TRequestStatus reqStatus;

	RMobileCall mobileCall;
	TName callName;
	err = mobileCall.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(mobileCall);

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);

	const TInt KCallId = 1;
	const RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
	RMobileCall::TMobileCallInfoV1 callInfo;
	FillInEmergencyCallInfo(callInfo, KCallId, callName, KMobileService);
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId, KMobileService, callInfo);
	
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(KCallId, KMobileService, mobileCallStatus);
	
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
	TMockLtsyCallData2<TInt, TBool> expHangUpData(KCallId, KMobileService, 
                                              hangUpCause, 
                                              autoStChangeDisable);
                                              
	TMockLtsyCallData0 completeEmptyLtsyData(KCallId, KMobileService);                                              

 	//-------------------------------------------------------------------------
	// TEST: No answer from Activate RF in System State Manager
	// if there is no answer from Activate RF, then this request is never completed
	// So let's cancel DialEmergencyCall
 	//------------------------------------------------------------------------- 	
 	
 	// Forse  SSMStub's Activate RF no to complete request 	
 	TInt complete(0); // EFalse
 	err = RProperty::Define(KThisProcessUID, ECompleteActivate, RProperty::EInt);
 	ASSERT_EQUALS(KErrNone, err); 	
 	RProperty::Set(KThisProcessUID, ECompleteActivate, complete);
  
	mobileCall.DialEmergencyCall(reqStatus, number);

	mobileCall.CancelAsyncRequest(EMobileCallDialEmergencyCall);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();

	// Allow   SSMStub's Activate RF to to complete request
	complete = 1;// ETrue
	RProperty::Set(KThisProcessUID, ECompleteActivate, complete);	
	
	
	//-------------------------------------------------------------------------
	// TEST: No answer from Deactivate RF in System State Manager
 	//------------------------------------------------------------------------- 
 	 	
 	// Forse  SSMStub's Deactivate  RF no to complete request 	
 	complete = 0; // EFalse
 	err = RProperty::Define(KThisProcessUID, ECompleteDeactivate, RProperty::EInt);
 	ASSERT_EQUALS(KErrNone, err); 	
 	RProperty::Set(KThisProcessUID, ECompleteDeactivate, complete);
 	
 	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);
	
    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

	mobileCall.DialEmergencyCall(reqStatus, number);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); // KErrNone cause CTSY doesn't care about result of Deactivating
	AssertMockLtsyStatusL();
		
	// Allow   SSMStub's Deactivete RF to to complete request
	complete = 1;// ETrue
	RProperty::Set(KThisProcessUID, ECompleteDeactivate, complete);	
	
	
	
	// Delete defined Properties
	err = RProperty::Delete(KThisProcessUID, ECompleteActivate);
 	ASSERT_EQUALS(KErrNone, err); 
	err = RProperty::Delete(KThisProcessUID, ECompleteDeactivate);
 	ASSERT_EQUALS(KErrNone, err); 
	
	CleanupStack::PopAndDestroy(4, this); // ...,  this	
#endif		
	}

void CCTsyCallEmergencyFU::TestDialEmergencyCall0001fL()
    {
    OpenEtelServerL();
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    TInt err = iPhone.Open(iTelServer, KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, err);

    err = iMockLTSY.Connect();
    ASSERT_EQUALS(KErrNone, err);
    RBuf8 data;
    CleanupClosePushL(data);

    RLine line;
    err = line.Open(iPhone, KMmTsyVoice1LineName);
    ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);

    TRequestStatus reqStatus;

    RMobileCall mobileCall;
    TName callName;
    err = mobileCall.OpenNewCall(line, callName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileCall);

    RMobileENStore::TEmergencyNumber number(_L("911"));
    TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);

    const TInt KCallId = 1;
    RMobilePhone::TMobileService KMobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 callInfo;
    FillInEmergencyCallInfo(callInfo, KCallId, callName, KMobileService);
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData(KCallId, KMobileService, callInfo);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(KCallId, KMobileService, mobileCallStatus);


    data.Close();
    numberLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    mobileCall.DialEmergencyCall(reqStatus, number);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(4); //mobileCall, line, data, this
    
    }
