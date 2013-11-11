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
//



/**
 @file The TEFUnit test suite for CallControl in the Common TSY.
*/

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>

#include "config.h"
#include <centralrepository.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include "mockltsyindicatorids.h"
#include "cctsycallcontrolfunegative.h"

/**
Create incoming call on RLine
@param aLine RLine use to create call.
@param aCallId Call id for created call.
@param aLineName The name of the line. Deprecated, line should be opened in advance
@param aIncomingCallName On completion contains the name of the incoming call.
@param aMobileService Applicability of created call to a mobile service group.
@param aMobileCallStatus Call state of created call.
*/

void CCTsyCallControlFUNegative::CreateIncomingCallL(TInt aCallId, RMobilePhone::TMobileService aMode)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = aMode;
    mobileCallInfo.iCallId = aCallId;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(aCallId, aMode, mobileCallInfo);
    mockCallData1.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyIncomingCallIndId, KErrNone, data);

    data.Close();
    CleanupStack::PopAndDestroy(1, &data);
	}

void CCTsyCallControlFUNegative::WaitForIncomingCallNotificationL(
		RMobileCall& aMobileCall, RMobileLine& aMobileLine, TName& aCallName)
	{
    TRequestStatus notifyStatus;
    aMobileLine.NotifyIncomingCall(notifyStatus, aCallName);
    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());

    TInt err = aMobileCall.OpenExistingCall(aMobileLine, aCallName);
    CleanupClosePushL(aMobileCall);
    ASSERT_EQUALS(KErrNone, err);

    RMobileCall::TMobileCallStatus callStatus;
    aMobileCall.GetMobileCallStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusRinging, callStatus);
	}

CTestSuite* CCTsyCallControlFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0009L);

	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0017L);

	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0019L);

	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFUNegative, TestUnit0024L);

	END_SUITE;
	}

//
// 'Negative' unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallAnswerISV is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallISV()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));
	mobileCall.Close();
	CleanupStack::Pop(&mobileCall);

	TName incomingCallName;
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
	CreateIncomingCallL(callId, mobileService);

	DriverCompleteGetMobileCallInfoL(callId, mobileService);

	// KErrGsmBusyUserRequest hardcoded to be sent by CTSY to LTSY,
	// mapped to KErrGsmCCUserBusy
	TInt hangUpCause = KErrGsmCCUserBusy;
	TMockLtsyData2<TInt, TInt> mockData2(callId, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCCallRejected, EFalse);
	
	WaitForIncomingCallNotificationL(mobileCall, mobileLine, incomingCallName);
	
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, callStatus);
    data.Close();
    mockCallData3.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId , KErrNone, data);
    
    TRequestStatus requestStatus;
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> callParamsPckg(callParams);
	
    mobileCall.AnswerIncomingCallISV(requestStatus, callParamsPckg);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallHold is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes MobileCall::Hold()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);	
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus requestStatus;
	
	mobileCall.Hold(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);

	mobileCall.Close();
	mobileLine.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallDialEmergencyCall is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes MobileCall::DialEmergencyCall()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0003L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));

	TRequestStatus requestStatus;
	RMobileENStore::TEmergencyNumber emergencyNumber = _L("911");
	
	mobileCall.DialEmergencyCall(requestStatus, emergencyNumber);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStopDTMFTone is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::StopDTMFTone()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0004L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	RBuf8 data;
	CleanupClosePushL(data);

	TChar tone('1');
	TMockLtsyData2<TInt, TChar> toneLtsyData(callId, tone);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, KErrNone);
	
	TInt err = iPhone.StartDTMFTone(tone);
    ASSERT_EQUALS(err, KErrNone); 	
	
	err = iPhone.StopDTMFTone();
    ASSERT_EQUALS(err, KErrNotSupported);    
    
    TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSetALSLine is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetALSLine()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0005L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLineAuxiliary;
	
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(1, this); // data, centRep, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSendDTMFTonesCancel is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendDTMFTones() and then RMobilePhone::CancelAsyncRequest()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0006L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInfoName tones(_L("123456789"));
	TMockLtsyData2<TInt, TInfoName> toneLtsyData(callId, tones);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test
	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	ClientDialCallL(mobileLine, mobileCall, mobileService);


	// Test sending DTMF tones and then cancelling
	TRequestStatus requestStatus;
	
	iPhone.SendDTMFTones(requestStatus, tones);
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EEtelCallHangUp is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::HangUp()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0007L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);	
		
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	ClientDialCallL(mobileLine, mobileCall, mobileService);

    TRequestStatus requestStatus;
    mobileCall.HangUp(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallResume is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Resume()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

    DriverHoldCallL(callId, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId, hangUpCause);

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus notifyStatus;
	RMobileCall::TMobileCallStatus callStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	TRequestStatus holdStatus;
	mobileCall.Hold(holdStatus);
	User::WaitForRequest(holdStatus);
	ASSERT_EQUALS(KErrNone, holdStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	TRequestStatus requestStatus;
	
	mobileCall.Resume(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());

	mobileCall.Close();
	mobileLine.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallSetDynamicHscsdParams is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetDynamicHscsdParams()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId, hangUpCause);

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus requestStatus;
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
	
	mobileCall.SetDynamicHscsdParams(requestStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());

	mobileCall.Close();
	mobileLine.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallTransfer is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Transfer()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	DriverDialAndHoldFirstCallDialSecondCallL(mobileService, callId1, callId2);
	
	TInt hangUpCause = KErrGsmCCNormalCallClearing;

	DriverHangUpCallL(callId1, hangUpCause, EFalse);
	DriverHangUpCallL(callId2, hangUpCause, ETrue);

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

	TRequestStatus requestStatus;
	
	mobileCall.Transfer(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);
	ClientHangUpCallL(mobileCall2);

	mobileCall.Close();
	mobileLine.Close();

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // mobileCall2, mobileLine2, mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSendDTMFTones is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendDTMFTones()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0012L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus requestStatus;
	TInfoName fullTonesString = _L("12345w67890wABCD");
	
	iPhone.SendDTMFTones(requestStatus, fullTonesString);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);

	mobileCall.Close();
	mobileLine.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetIdentityServiceStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIdentityServiceStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0013L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdService expService(RMobilePhone::EIdServiceCallerPresentation);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> expLtsyData(expService);
	RMobilePhone::TMobilePhoneIdServiceStatus status;
	
	iPhone.GetIdentityServiceStatus(requestStatus, expService, status);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileCallSwap is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::Swap()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0014L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TInt callId1 = 1;
	TInt callId2 = 2;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	DriverDialAndHoldFirstCallDialSecondCallL(mobileService, callId1, callId2);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId1, hangUpCause, EFalse);
	DriverHangUpCallL(callId2, hangUpCause, ETrue);

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

	TRequestStatus requestStatus;
	
	mobileCall.Swap(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	ASSERT_EQUALS(KErrNone, mobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);
	ClientHangUpCallL(mobileCall2);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);
	
	mobileCall.Close();
	mobileLine.Close();
	mobileCall2.Close();
	mobileLine2.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // mobileCall2, mobileLine2, mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EEtelCallLoanDataPort is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::LoanDataPort()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0015L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);	
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus requestStatus;
    _LIT(KCom, "Com");    
	TFileName csy(KCom);
    TName portName(KCom);  
    RCall::TCommPort port;
    port.iCsy = csy;
    port.iPort = portName; 
	
    mobileCall.LoanDataPort(requestStatus, port);    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);

	mobileCall.Close();
	mobileLine.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EEtelCallRecoverDataPort is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::RecoverDataPort()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0016L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);
	
	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);	
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

    TInt err = mobileCall.RecoverDataPort();    
	ASSERT_EQUALS(err, KErrNotSupported);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);

	mobileCall.Close();
	mobileLine.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0017
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneStartDTMFTone is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::StartDTMFTone()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0017L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);

	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TChar tone('1');
	
    TInt err = iPhone.StartDTMFTone(tone);
    ASSERT_EQUALS(err, KErrNotSupported);    
    
    TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0019
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomCheckEmergencyNumberIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckEmergencyNumber()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0019L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus requestStatus;	
	TBool result;
	RMmCustomAPI::TEmerNumberCheckMode mode;
	_LIT(KEmerNum, "IfThisIsEmer?");
	mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckAdvanced;
	mode.iNumber.Copy(KEmerNum);

    customApi.CheckEmergencyNumber(requestStatus, mode, result);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0021
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetAlsBlockedIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetAlsBlocked()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0021L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus requestStatus;	
	RMmCustomAPI::TGetAlsBlockStatus status;
		
	TInt err = customApi.GetAlsBlocked(status);
 	ASSERT_EQUALS(err, KErrNotSupported);
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0022
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomSetAlsBlockedIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetAlsBlocked()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0022L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus requestStatus;	
	RMmCustomAPI::TSetAlsBlock setData = RMmCustomAPI::EActivateBlock;
		
	TInt err = customApi.SetAlsBlocked(setData);
 	ASSERT_EQUALS(err, KErrNotSupported);
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0023
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetLifeTimeIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetLifeTime()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0023L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus requestStatus;
	RMmCustomAPI::TLifeTimeData lifeTime;
	RMmCustomAPI::TLifeTimeDataPckg lifeTimePckg(lifeTime);
		
	TInt err = customApi.GetLifeTime(lifeTimePckg);
 	ASSERT_EQUALS(err, KErrNotSupported);
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-NEGATIVE-UN0024
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomTerminateCallIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::TerminateCall()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFUNegative::TestUnit0024L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId, EFalse);
	// or MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TInt callId = 1;
	DriverDialCallL(callId, RMobilePhone::EVoiceService);
	
    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService);

	// Getting the call information
	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallInfo(callInfoPckg));
	ASSERT_EQUALS(callId , callInfo.iCallId);
	
	TInt err = customApi.TerminateCall(callInfo.iCallName);
 	ASSERT_EQUALS(err, KErrNotSupported);

 	// Hang up the call
	DriverHangUpCallL(callId, KErrGsmCCNormalCallClearing);
    
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(), KErrNone);	
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, customApi, this
	}
