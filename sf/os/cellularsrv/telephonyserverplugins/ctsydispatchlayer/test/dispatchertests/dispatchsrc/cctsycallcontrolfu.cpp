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
// The TEFUnit test suite for CallControlControl in the Common TSY.
//



#include "cctsycallcontrolfu.h"
#include "isvao.h"

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include "testconstants.h"

#include "mockltsyindicatorids.h"
#include <sacls.h> // For phone power P&S key needed by Etel3rdParty testing
#include <e32property.h>
#include <ctsy/rmmcustomapi.h>
#include <centralrepository.h>

CTestSuite* CCTsyCallControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	// Add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUseCase0025L);

	// Add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallControlFU, TestUnit0021L);


	END_SUITE;
	}


//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for mobile originated voice call accepted and hang up
@SYMTestPriority High
@SYMTestActions Make a voice call, receive status change notifications, hang up
@SYMTestExpectedResults Pass - Call reaches connected state and call is hung up
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	mobileLine.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //mobileCall, mobileLine, this
	} // CCTsyCallControlFU::TestUseCase0001L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for failed dialling of a mobile originated call
@SYMTestPriority High
@SYMTestActions Dials a voice call, receives error from LTSY
@SYMTestExpectedResults Pass - Dial request fails
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverExpectDialCallRequestL(callId, mobileService, KErrGeneral);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));

	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
	callParams.iAutoRedial = ETrue;
	callParams.iIdRestrict = RMobileCall::ESendMyId;
	TRequestStatus dialStatus;
	mobileCall.Dial(dialStatus, callParamsPckg, KPhoneNumber);
	User::WaitForRequest(dialStatus);
	ASSERT_EQUALS(KErrGeneral, dialStatus.Int());

	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0002L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for remote party rejecting a dialled call
@SYMTestPriority High
@SYMTestActions Dials a call, remote party rejects it, check call status and notifications are correct
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
		
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverExpectDialCallRequestL(callId, mobileService, KErrNone);

	// Complete KLtsyDispatchCallControlNotifyCallStatusChangeApiId
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusDialling;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
			callId, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

    // Complete events expected when remote party rejects call

    // Complete KLtsyDispatchCallControlNotifyCallStatusChangeApiId
    DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCCallRejected, EFalse);
	
	// Client Side Test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));

	TRequestStatus notifyCallStatus;
	callStatus = RMobileCall::EStatusUnknown;
	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
	callParams.iAutoRedial = ETrue;
	callParams.iIdRestrict = RMobileCall::ESendMyId;
	TRequestStatus dialStatus;
	mobileCall.Dial(dialStatus, callParamsPckg, KPhoneNumber);
	User::WaitForRequest(dialStatus);
	ASSERT_EQUALS(KErrGsmCCCallRejected, dialStatus.Int());

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDialling);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

	callStatus = RMobileCall::EStatusUnknown;
	mobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);

	expectedCallStatuses.Close();
	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectedCallStatuses, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0003L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for remote party hang up without inband info
@SYMTestPriority High
@SYMTestActions Invokes dialling voice call, receive remote party hang up
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	RMobileCall::TMobileCallEvent callEvent = RMobileCall::ERemoteTerminated;
	TMockLtsyCallData1<RMobileCall::TMobileCallEvent> callEventData(callId, mobileService, callEvent);
	callEventData.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallEventIndId, KErrNone, data);

    DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCNormalCallClearing);

    TRequestStatus notifyCallEventStatus;
    RMobileCall::TMobileCallEvent receivedCallEvent;
    mobileCall.NotifyCallEvent(notifyCallEventStatus, receivedCallEvent);

	TRequestStatus notifyCallStatus;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

    User::WaitForRequest(notifyCallEventStatus);
    ASSERT_EQUALS(KErrNone, notifyCallEventStatus.Int());
    ASSERT_EQUALS(callEvent, receivedCallEvent);

    expectedCallStatuses.Close();
	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectedCallStatuses, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0004L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for remote party hang up with inband info
@SYMTestPriority High
@SYMTestActions Dial a voice call, remote party hangs up with inband info
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUseCase0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	DriverCompleteSuccessfulHangUpNotificationsL(callId, ETrue, KErrGsmCCNormalCallClearing);

	TRequestStatus notifyCallStatus;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnectingWithInband);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

    expectedCallStatuses.Close();
	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectedCallStatuses, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0005L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for mobile originated data call
@SYMTestPriority High
@SYMTestActions Invokes Dials a data call, receives call status notifications, loan and recover the data port, hang up call.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUseCase0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	DriverDialCallL(callId, mobileService);

	// KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId
	RMobileCall::TMobileCallDataCapsV1 completeCallCaps;
	completeCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed48000;
	completeCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV34;
	completeCallCaps.iServiceCaps = RMobileCall::KCapsPacketAccessSyncUDI;
	completeCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
	completeCallCaps.iHscsdSupport = ETrue;
	completeCallCaps.iMClass = 2;
	completeCallCaps.iMaxRxTimeSlots = 4;
	completeCallCaps.iMaxTxTimeSlots = 8;
	completeCallCaps.iTotalRxTxTimeSlots = 10;
	completeCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding96;
	completeCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryDownlink;
	completeCallCaps.iUserInitUpgrade = ETrue;
	completeCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
	completeCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisRxDirection;
	data.Close();
	TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1>
		completeDataCapsData(callId, RMobilePhone::EServiceUnspecified, completeCallCaps);
	completeDataCapsData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId, KErrNone, data);


    _LIT(KDataPort, "DATAPORT::0");
    RCall::TCommPort commPort;
    commPort.iPort = KDataPort;
    data.Close();
    TMockLtsyData2<TInt,RCall::TCommPort> loanDataPortExpData(callId,commPort);
	loanDataPortExpData.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId, data);
    iMockLTSY.ExpectL(MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId, data);

    		
	// MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
	data.Close();
	TMockLtsyData2<TInt, RMobileCall::TMobileHscsdCallParamsV8> paramData(callId, hscsdParams);
	paramData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId, data);

    TMockLtsyData1<TInt> mockParamData(callId);
    data.Close();
    mockParamData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId, KErrNone, data);

	// KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId
    RMobileCall::TMobileCallHscsdInfoV8 hscsdInfo;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;

    RMobileCall::TMobileCallDataCapsV1 completeCallCaps2 = completeCallCaps;
    completeCallCaps2.iHscsdSupport = ETrue;
    completeCallCaps2.iSpeedCaps = RMobileCall::KCapsSpeed57600;
    completeCallCaps2.iMaxRxTimeSlots = 3;
    completeCallCaps2.iMaxTxTimeSlots = 5;

    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV8, RMobileCall::TMobileCallDataCapsV1>
    		hscsdData(1, RMobilePhone::ECircuitDataService, hscsdInfo, completeCallCaps2);
    data.Close();
    hscsdData.SerialiseL(data);

	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId, KErrNone, data);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	ClientDialCallL(mobileLine, mobileCall, mobileService);

	RCall::TCommPort dataPort;
	TRequestStatus reqStatus;
	mobileCall.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(0, dataPort.iPort.Compare(KDataPort()));

	TInt err = mobileCall.RecoverDataPort();
	ASSERT_EQUALS(KErrNone, err);

	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	mobileCall.GetMobileDataCallCaps(callCapsPckg);
	ASSERT_TRUE(TComparator<RMobileCall::TMobileCallDataCapsV1>::IsEqual(completeCallCaps, callCaps));

	TRequestStatus hscsdInfoStatus;
	RMobileCall::TMobileCallHscsdInfoV8 hscsdInfo2;
	RMobileCall::TMobileCallHscsdInfoV8Pckg hscsdInfoPckg(hscsdInfo2);
	mobileCall.NotifyHscsdInfoChange(hscsdInfoStatus, hscsdInfoPckg);

	TRequestStatus dataCallCapsStatus;
	RMobileCall::TMobileCallDataCapsV1 dataCallCaps2;
	RMobileCall::TMobileCallDataCapsV1Pckg dataCallCapsPckg(dataCallCaps2);
	mobileCall.NotifyMobileDataCallCapsChange(dataCallCapsStatus, dataCallCapsPckg);

	TRequestStatus hscsdParamStatus;
	mobileCall.SetDynamicHscsdParams(hscsdParamStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(hscsdParamStatus);

	User::WaitForRequest(hscsdInfoStatus);
	ASSERT_EQUALS(KErrNone, hscsdInfoStatus.Int());
	ASSERT_EQUALS(hscsdInfo2.iAiur, hscsdInfo.iAiur);
	ASSERT_EQUALS(hscsdInfo2.iRxTimeSlots, hscsdInfo.iRxTimeSlots);
	ASSERT_EQUALS(hscsdInfo2.iTxTimeSlots, hscsdInfo.iTxTimeSlots);
	ASSERT_EQUALS(hscsdInfo2.iCodings, hscsdInfo.iCodings);

	User::WaitForRequest(dataCallCapsStatus);
	ASSERT_EQUALS(KErrNone, dataCallCapsStatus.Int());
	ASSERT_EQUALS(completeCallCaps2.iSpeedCaps, dataCallCaps2.iSpeedCaps);
	ASSERT_EQUALS(completeCallCaps2.iHscsdSupport, dataCallCaps2.iHscsdSupport);
	ASSERT_EQUALS(completeCallCaps2.iMaxRxTimeSlots, dataCallCaps2.iMaxRxTimeSlots);
	ASSERT_EQUALS(completeCallCaps2.iMaxTxTimeSlots, dataCallCaps2.iMaxTxTimeSlots);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 	
	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0006L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0007
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for incoming voice call accepted
 @SYMTestPriority High
 @SYMTestActions Requests notification of an incoming call, incoming call arrives, answer the call, hang up
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TName incomingCallName;

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));
	mobileCall.Close();
	CleanupStack::Pop(&mobileCall);

	// Create an incoming call and answer it

	DriverCreateIncomingCallL(callId, mobileService);

	DriverCompleteGetMobileCallInfoL(callId, mobileService);

	DriverAnswerIncomingCallL(callId, mobileService);

    TInt hangUpCause = KErrGsmCCNormalCallClearing;
    DriverHangUpCallL(callId, hangUpCause);

    // Create another incoming call but fail to answer this one

	DriverCreateIncomingCallL(callId, mobileService);

	DriverCompleteGetMobileCallInfoL(callId, mobileService);

	DriverAnswerIncomingCallL(callId, mobileService, EFalse, KErrGeneral);

    // Client side test

    ClientWaitForIncomingCallNotificationLC(mobileCall, mobileLine, incomingCallName);

    ClientAnswerIncomingCallL(mobileCall);

	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	CleanupStack::Pop(&mobileCall);

	ClientWaitForIncomingCallNotificationLC(mobileCall, mobileLine, incomingCallName);

	ClientAnswerIncomingCallL(mobileCall, KErrGeneral);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0007L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0008
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for incoming voice call rejected
 @SYMTestPriority High
 @SYMTestActions Invokes actions
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TName incomingCallName;

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));
	mobileCall.Close();
	CleanupStack::Pop(&mobileCall);

	DriverCreateIncomingCallL(callId, mobileService);

	DriverCompleteGetMobileCallInfoL(callId, mobileService);

	// KErrGsmBusyUserRequest hardcoded to be sent by CTSY to LTSY,
	// mapped to KErrGsmCCUserBusy
	TInt hangUpCause = KErrGsmCCUserBusy;
	TMockLtsyData2<TInt, TInt> mockData2(callId, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);
	DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCCallRejected, EFalse);

	// Client side test

	ClientWaitForIncomingCallNotificationLC(mobileCall, mobileLine, incomingCallName);

	ClientHangUpCallL(mobileCall);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0008L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0009
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for cancel dialling a call when it is still in dialling state.
 @SYMTestPriority High
 @SYMTestActions Invokes dialling a call and then cancelling the dial during dialling process.
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	DriverExpectDialCallRequestL(callId, mobileService, KErrNone);

	// Complete KLtsyDispatchCallControlNotifyCallStatusChangeApiId with dialling
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusDialling;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
			callId, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // This cause always passed by CTSY in this situation
	TMockLtsyData2<TInt, TInt> mockData2(callId, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCNormalCallClearing, EFalse);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));

	TRequestStatus notifyCallStatus;
	callStatus = RMobileCall::EStatusUnknown;
	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
	callParams.iAutoRedial = ETrue;
	callParams.iIdRestrict = RMobileCall::ESendMyId;
	TRequestStatus dialStatus;
	mobileCall.Dial(dialStatus, callParamsPckg, KPhoneNumber);

	User::WaitForRequest(notifyCallStatus);
	ASSERT_EQUALS(KErrNone, notifyCallStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);

	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	mobileCall.DialCancel();

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);
	
	User::WaitForRequest(dialStatus);
	ASSERT_EQUALS(KErrCancel, dialStatus.Int());

	expectedCallStatuses.Close();
	mobileLine.Close();
	mobileCall.Close();
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectedCallStatuses, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0009L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0010
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for cancelling dial request when call is connecting
 @SYMTestPriority High
 @SYMTestActions Invokes dialling a call and cancelling the dial request when call reaches connecting state
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	DriverExpectDialCallRequestL(callId, mobileService, KErrNone);

	// Complete KLtsyDispatchCallControlNotifyCallStatusChangeApiId with dialling
	RArray<RMobileCall::TMobileCallStatus> arrayCallStatus;
	CleanupClosePushL(arrayCallStatus);
	arrayCallStatus.AppendL(RMobileCall::EStatusDialling);
	arrayCallStatus.AppendL(RMobileCall::EStatusConnecting);
	DriverCompleteCallStatusChangeNotificationsL(arrayCallStatus, callId, KErrNone);
	arrayCallStatus.Close();

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // This cause always passed by CTSY in this situation
	TMockLtsyData2<TInt, TInt> mockData2(callId, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCNormalCallClearing, EFalse);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));

	TRequestStatus notifyCallStatus;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
	callParams.iAutoRedial = ETrue;
	callParams.iIdRestrict = RMobileCall::ESendMyId;
	TRequestStatus dialStatus;
	mobileCall.Dial(dialStatus, callParamsPckg, KPhoneNumber);

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDialling);
    expectedCallStatuses.AppendL(RMobileCall::EStatusConnecting);
    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	mobileCall.DialCancel();

	expectedCallStatuses.Close();
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

	User::WaitForRequest(dialStatus);
	ASSERT_EQUALS(KErrCancel, dialStatus.Int());

	expectedCallStatuses.Close();
	arrayCallStatus.Close();
	mobileLine.Close();
	mobileCall.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // expectedCallStatuses, mobileCall, mobileLine, arrayCallStatus, data, this
	} // CCTsyCallControlFU::TestUseCase0010L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0011
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for missed incoming call
 @SYMTestPriority High
 @SYMTestActions Invokes requesting an incoming call, line rings, call is not answered, call disconnects
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	TName incomingCallName;

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ASSERT_EQUALS(KErrNone, OpenLineAndCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService));
	mobileCall.Close();
	CleanupStack::Pop(&mobileCall);

    DriverCreateIncomingCallL(callId, mobileService);

	DriverCompleteGetMobileCallInfoL(callId, mobileService);

	ClientWaitForIncomingCallNotificationLC(mobileCall, mobileLine, incomingCallName);

	DriverCompleteSuccessfulHangUpNotificationsL(callId, EFalse, KErrGsmCCNormalCallClearing, EFalse);

    TRequestStatus notifyCallStatus;
    RMobileCall::TMobileCallStatus callStatus;
    mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

    RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);

    ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

	expectedCallStatuses.Close();
	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectedCallStatuses, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0011L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0012
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for holding, resuming a call
 @SYMTestPriority High
 @SYMTestActions Invokes dialling a call, holding it, then resuming it
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	// Hold call
    DriverHoldCallL(callId, KErrNone);

	// Resume call
	DriverResumeCallL(callId, KErrNone);

	// Swap single call.
	DriverSwapCallL(callId, KErrNone, RMobileCall::EStatusHold);
	
	// Swap single call again.
	DriverSwapCallL(callId, KErrNone, RMobileCall::EStatusConnected);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

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

	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	TRequestStatus resumeStatus;
	mobileCall.Resume(resumeStatus);
	User::WaitForRequest(resumeStatus);
	ASSERT_EQUALS(KErrNone, resumeStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);
	
	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);
	
	// Attempt to swap it when it is the sole call.
	TRequestStatus swapStatus;
	mobileCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrNone, swapStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);
	
	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	// Swapping single call again, back to connected.
	mobileCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrNone, swapStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0012L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0013
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to hold a connected call
 @SYMTestPriority High
 @SYMTestActions Invokes dialling a call, attempting to hold it, receiving an error
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	DriverHoldCallL(callId, KErrGeneral);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId, hangUpCause);

	// Client side test
    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus holdStatus;
	mobileCall.Hold(holdStatus);
	User::WaitForRequest(holdStatus);
	ASSERT_EQUALS(KErrGeneral, holdStatus.Int());

	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0013L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0014
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to resume
 @SYMTestPriority High
 @SYMTestActions Invokes holding a call, attempting to resume it and receiving a failure.
 @SYMTestExpectedResults Pass - Call still on hold
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	DriverHoldCallL(callId, KErrNone);

	DriverResumeCallL(callId, KErrGeneral);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	TMockLtsyData2<TInt, TInt> mockData2(callId, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus holdStatus;
	mobileCall.Hold(holdStatus);
	User::WaitForRequest(holdStatus);
	ASSERT_EQUALS(KErrNone, holdStatus.Int());

	TRequestStatus resumeStatus;
	mobileCall.Resume(resumeStatus);
	User::WaitForRequest(resumeStatus);
	ASSERT_EQUALS(KErrGeneral, resumeStatus.Int());

	RMobileCall::TMobileCallStatus callStatus;
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0014L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0015
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to swap a call
 @SYMTestPriority High
 @SYMTestActions Invokes swapping a call and receiving error
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0015L()
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

	DriverSwapCallL(callId1, callId2, KErrGeneral, RMobileCall::EStatusConnected, RMobileCall::EStatusHold);

	DriverSwapCallL(callId1, callId2, KErrGeneral, RMobileCall::EStatusConnected, RMobileCall::EStatusHold);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId1, hangUpCause, EFalse);

	DriverHangUpCallL(callId2, hangUpCause, ETrue);

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

	TRequestStatus swapStatus;
	mobileCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrGeneral, swapStatus.Int());

	mobileCall2.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrGeneral, swapStatus.Int());

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
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); //mobileCall2, mobileLine2, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0015L



/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0016
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for holding a call then dialling a second and swapping them
 @SYMTestPriority High
 @SYMTestActions Invokes dialling then holding a call and dialling a second call and swapping them
 @SYMTestExpectedResults Pass - Second call can be dialled
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0016L()
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

	DriverSwapCallL(callId1, callId2, KErrNone, RMobileCall::EStatusConnected, RMobileCall::EStatusHold);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId2, hangUpCause, EFalse);

	hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId1, hangUpCause, ETrue);

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

	TRequestStatus notifyStatus;
	RMobileCall::TMobileCallStatus callStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	TRequestStatus notifyStatus2;
	RMobileCall::TMobileCallStatus callStatus2;
	mobileCall2.NotifyMobileCallStatusChange(notifyStatus2, callStatus2);

	TRequestStatus swapStatus;
	mobileCall.Swap(swapStatus);
	User::WaitForRequest(swapStatus);
	ASSERT_EQUALS(KErrNone, swapStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus2);

	ClientHangUpCallL(mobileCall2);

	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // mobileCall2, mobileLine2, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0016L



/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0017
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for dialling and hanging up an ISV call
 @SYMTestPriority High
 @SYMTestActions Invokes dialling then hanging up a voice call using CTelephony
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0017L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC* phoneNumberPtr = const_cast<TDesC*>(&KPhoneNumber);
	TBool isEmergency = EFalse;
	TMockLtsyData2<TDesC*, TBool> queryIsEmergencyNumberExpData(phoneNumberPtr,isEmergency);
	queryIsEmergencyNumberExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId, data);
	data.Close();
	
	TInt callId = 1;
	RMobilePhone::TMobileService mode = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mode, ETrue);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	ClientSetIsvPubSubKeyL();

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	CIsvAO* isvAO = CIsvAO::NewL();
	CleanupStack::PushL(isvAO);

	isvAO->DoOperation(CIsvAO::EIsvDial);
	ASSERT_EQUALS(KErrNone, isvAO->Status().Int());

	isvAO->DoOperation(CIsvAO::EIsvHangUp);
	ASSERT_EQUALS(KErrNone, isvAO->Status().Int());

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //isvAO, data, customApi, this
	} // CCTsyCallControlFU::TestUseCase0017L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0018
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for getting notification of and answering an incoming call using CTelephony
 @SYMTestPriority High
 @SYMTestActions Invokes CTelephony APIs to get notification of an incoming call, answer it.
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0018L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	ClientSetIsvPubSubKeyL();

	CIsvAO* isvAO = CIsvAO::NewL();
	CleanupStack::PushL(isvAO);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverCreateIncomingCallL(callId, mobileService);

	isvAO->DoOperation(CIsvAO::EIsvNotifyIncomingCall);
	ASSERT_EQUALS(KErrNone, isvAO->Status().Int());

	DriverCompleteGetMobileCallInfoL(callId, mobileService);

	DriverAnswerIncomingCallL(callId, mobileService, ETrue);

    TInt hangUpCause = KErrGsmCCNormalCallClearing;
    DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
    
	isvAO->DoOperation(CIsvAO::EIsvAnswer);
	ASSERT_EQUALS(KErrNone, isvAO->Status().Int());

	isvAO->DoOperation(CIsvAO::EIsvHangUp);
	ASSERT_EQUALS(KErrNone, isvAO->Status().Int());

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // isvAO, this
	} // CCTsyCallControlFU::TestUseCase0018L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0019
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for dialling an emergency call
 @SYMTestPriority High
 @SYMTestActions Invokes dialling of an emergency call
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0019L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileENStore::TEmergencyNumber emergencyNumber = _L("911");
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> dialData(emergencyNumber);
	dialData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId, data);

	TInt callId = 1;
	TMockLtsyData1<TInt> dialData2(callId);
	data.Close();
	dialData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId, KErrNone, data);

	RMobilePhone::TMobileService mode = RMobilePhone::EVoiceService;
	DriverCompleteGetMobileCallInfoL(callId, mode);
	DriverCompleteSuccessfulDialNotificationsL(callId);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	ClientDialCallL(mobileLine, mobileCall, mode, ETrue);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileLine, mobileCall, data, this
	} // CCTsyCallControlFU::TestUseCase0019L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0020
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failed dialling an emergency call
 @SYMTestPriority High
 @SYMTestActions Invokes dialling an emergency call
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0020L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileENStore::TEmergencyNumber emergencyNumber = _L("911");
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> dialData(emergencyNumber);
	dialData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId, data);

	TInt callId = 1;
	RMobilePhone::TMobileService mode = RMobilePhone::EVoiceService;
	TMockLtsyCallData0 dialData2(callId, mode); // Mode not actually used
	data.Close();
	dialData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId, KErrGeneral, data);

	// Client side test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	OpenLineAndCallL(mobileLine, mobileCall, mode);

	TRequestStatus dialStatus;
	mobileCall.DialEmergencyCall(dialStatus, emergencyNumber);
	User::WaitForRequest(dialStatus);
	ASSERT_EQUALS(KErrGeneral, dialStatus.Int());

	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileLine, mobileCall, data, this
	} // CCTsyCallControlFU::TestUseCase0020L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0021
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for transferring a call
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileCall::Transfer
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0021L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	DriverHoldCallL(callId, KErrNone);

	TInt callId2 = 2;
	DriverDialCallL(callId2, mobileService);

	TMockLtsyData2<TInt, TInt> mockData(callId, callId2);
	data.Close();
	mockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId, data);

    iMockLTSY.CompleteL(MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId, KErrNone);

    RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
  	    				callId, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
      	    				callId2, RMobilePhone::EServiceUnspecified, callStatus);
    data.Close();
    mockCallData2.SerialiseL(data);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
    
    data.Close();
    
	TUint32 duration = 3; //this is the length of the call and its not possible to be certain of this due to
							//e.g. logging statements so this argument is not actually checked in the mockltsy
	TMockLtsyData1<TUint32> durationLtsyData(duration);
	durationLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, data, KErrNone);
	data.Close();

	TRequestStatus termiantedReqStatus;
	iMockLTSY.NotifyTerminated(termiantedReqStatus);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, KErrNone);
    
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

	TRequestStatus notifyStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	TRequestStatus notifyStatus2;
	RMobileCall::TMobileCallStatus callStatus2;
	mobileCall2.NotifyMobileCallStatusChange(notifyStatus2, callStatus2);

	TRequestStatus transferStatus;
	mobileCall.Transfer(transferStatus);
	User::WaitForRequest(transferStatus);
	ASSERT_EQUALS(KErrNone, transferStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus2);

	User::WaitForRequest(termiantedReqStatus);
	ASSERT_EQUALS(KErrNone, termiantedReqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); //mobileCall2, mobileLine2, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0021L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0022
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for failure to transfer a call
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileCall::Transfer
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0022L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	DriverHoldCallL(callId, KErrNone);

	TInt callId2 = 2;
	DriverDialCallL(callId2, mobileService);

	TMockLtsyData2<TInt, TInt> mockData(callId, callId2);
	data.Close();
	mockData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId, data);

    iMockLTSY.CompleteL(MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId, KErrGeneral);

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

	// Transfer call, which fails, check first call is still on hold and second
	// is still connected
	TRequestStatus notifyStatus;
	RMobileCall::TMobileCallStatus callStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	TRequestStatus notifyStatus2;
	RMobileCall::TMobileCallStatus callStatus2;
	mobileCall2.NotifyMobileCallStatusChange(notifyStatus2, callStatus2);

	TRequestStatus transferStatus;
	mobileCall.Transfer(transferStatus);
	User::WaitForRequest(transferStatus);
	ASSERT_EQUALS(KErrGeneral, transferStatus.Int());

	ASSERT_EQUALS(KRequestPending, notifyStatus.Int());
	mobileCall.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCancel, notifyStatus.Int());

	mobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	ASSERT_EQUALS(KRequestPending, notifyStatus2.Int());
	mobileCall2.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
	ASSERT_EQUALS(KErrCancel, notifyStatus2.Int());

	mobileCall2.GetMobileCallStatus(callStatus2);
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus2);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	
	DriverHangUpCallL(callId, hangUpCause, EFalse);
	ClientHangUpCallL(mobileCall);
	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	DriverHangUpCallL(callId2, hangUpCause, ETrue);
	ClientHangUpCallL(mobileCall2);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(6, this); //mobileCall2, mobileLine2, mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0022L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0023
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for sending a DTMF string containing a 'w' and continuing after the 'w'
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SendDTMFTones with a 'w' in the string then RMobilePhone::ContinueDTMFStringSending
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0023L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInfoName fullTonesString = _L("12345w67890wABCD");
	TBuf<5> tonesFirstPart;
	tonesFirstPart.Copy(fullTonesString.Left(5)); // 12345
	TMockLtsyData2<TInt, TBuf<5> > toneLtsyData(callId, tonesFirstPart);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	TBuf<5> tonesSecondPart;
	tonesSecondPart.Copy(fullTonesString.Mid(6, 5)); // 67890
	data.Close();
	TMockLtsyData2<TInt, TBuf<5> > toneLtsyData2(callId, tonesSecondPart);
	toneLtsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	TBuf<4> tonesThirdPart;
	tonesThirdPart.Copy(fullTonesString.Mid(12, 4)); // ABCD
	data.Close();
	TMockLtsyData2<TInt, TBuf<4> > toneLtsyData3(callId, tonesThirdPart);
	toneLtsyData3.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client side test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus notifyStopStatus;
	iPhone.NotifyStopInDTMFString(notifyStopStatus);

	TRequestStatus sendStatus;
	iPhone.SendDTMFTones(sendStatus, fullTonesString);

	User::WaitForRequest(notifyStopStatus);
	ASSERT_EQUALS(KErrNone, notifyStopStatus.Int());
	iPhone.NotifyStopInDTMFString(notifyStopStatus);

	ASSERT_EQUALS(KErrNone, iPhone.ContinueDTMFStringSending(ETrue));

	User::WaitForRequest(notifyStopStatus);
	ASSERT_EQUALS(KErrNone, notifyStopStatus.Int());

	ASSERT_EQUALS(KErrNone, iPhone.ContinueDTMFStringSending(ETrue));

	User::WaitForRequest(sendStatus);
	ASSERT_EQUALS(KErrNone, sendStatus.Int());

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 

	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0023L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0024
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendDTMFTones with 'w' and not continuing
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SendDTMFTones with 'w' and RMobilePhone::ContinueDTMFStringSending with EFalse
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0024L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TInfoName fullTonesString = _L("12345w67890");
	TBuf<5> tonesFirstPart;
	tonesFirstPart.Copy(fullTonesString.Left(5)); // 12345
	TMockLtsyData2<TInt, TBuf<5> > toneLtsyData(callId, tonesFirstPart);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client side test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus notifyStopStatus;
	iPhone.NotifyStopInDTMFString(notifyStopStatus);

	TRequestStatus sendStatus;
	iPhone.SendDTMFTones(sendStatus, fullTonesString);

	User::WaitForRequest(notifyStopStatus);
	ASSERT_EQUALS(KErrNone, notifyStopStatus.Int());

	ASSERT_EQUALS(KErrNone, iPhone.ContinueDTMFStringSending(EFalse));

	User::WaitForRequest(sendStatus);
	ASSERT_EQUALS(KErrAbort, sendStatus.Int());

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0024L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UC0025
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for dialling a call using the alternate line
 @SYMTestPriority High
 @SYMTestActions Invokes dialling a call using the alternate line
 @SYMTestExpectedResults Pass
 @SYMTestType CT
 */
void CCTsyCallControlFU::TestUseCase0025L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAuxVoiceService;
	DriverDialCallL(callId, mobileService);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);
	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
		
	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUseCase0025L


//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY Dispatcher for RMobilePhone::GetIdentityServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIdentityServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyCallControlFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneIdService expService(RMobilePhone::EIdServiceCallerPresentation);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> expLtsyData(expService);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId, data);

	RMobilePhone::TMobilePhoneIdServiceStatus completeStatus(RMobilePhone::EIdServiceActivePermanent);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdServiceStatus> completeLtsyData(completeStatus);
	data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId, KErrNone, data);

	// Client Side Test

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdServiceStatus status;
	iPhone.GetIdentityServiceStatus(reqStatus, expService, status);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeStatus, status);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	} // CCTsyCallControlFU::TestUnit0001L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0002
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomApi::CheckEmergencyNumber to check emergency number querying
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomApi::CheckEmergencyNumber to check emergency number querying
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC* phoneNumberPtr = const_cast<TDesC*>(&KEmergencyPhoneNumber);
	TBool isEmergency = ETrue;
	TMockLtsyData2<TDesC*, TBool> queryIsEmergencyNumberExpData(phoneNumberPtr, isEmergency);
	queryIsEmergencyNumberExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId, data);
	data.Close();
	
	TRequestStatus reqStatus;
	RMmCustomAPI::TEmerNumberCheckMode numCheckMode;
	numCheckMode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal;
	numCheckMode.iNumber = KEmergencyPhoneNumber;
	TBool emergencyNumCheckResult = EFalse;
	customApi.CheckEmergencyNumber(reqStatus, numCheckMode, emergencyNumCheckResult);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(emergencyNumCheckResult, isEmergency);

	customApi.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customApi, data, this

	} // CCTsyCallControlFU::TestUnit0002L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN003
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMobilePhone::SetALSLine
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SetALSLine
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLineAuxiliary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> expLtsyData(alsLine);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId, KErrNone);

	// Client Side Test

	const TUid KCRUidCtsyMEAlsLine = { 0x102029A2 };
	const TUint KCtsyMEAlsSupported = { 0x00000002 };

	// Create centRep to change repository
	CRepository* centRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
	CleanupStack::PushL(centRep);

	TInt ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);

	TRequestStatus notifyStatus;
	RMobilePhone::TMobilePhoneALSLine newAlsLine;
	iPhone.NotifyALSLineChange(notifyStatus, newAlsLine);

	TRequestStatus requestStatus;
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(alsLine, newAlsLine);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // centRep, data, this

	} // CCTsyCallControlFU::TestUnit0003L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0004
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMobilePhone::StartDTMFTone and RMobilePhone::StopDTMFTone
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::StartDTMFTone and RMobilePhone::StopDTMFTone
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TChar tone('1');
	TMockLtsyData2<TInt, TChar> toneLtsyData(callId, tone);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNone);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, KErrNone);

	TMockLtsyData1<TInt> toneLtsyData2(callId);
	toneLtsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, data, KErrNone);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	ASSERT_EQUALS(KErrNone, iPhone.StartDTMFTone(tone));

	ASSERT_EQUALS(KErrNone, iPhone.StopDTMFTone());

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUnit0004L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0005
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for cancelling sending of a DTMF string
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SendDTMFTones and cancels it
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0005L()
	{
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

	TMockLtsyData1<TInt> cancelMockData(callId);
	cancelMockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId, data, KErrNone);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	// First test sending cancel when there is no active call in progress
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	ClientDialCallL(mobileLine, mobileCall, mobileService);

	// Test sending cancel when there is no SendDTMFTones in progress
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	// Test sending DTMF tones and then cancelling

	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Test sending cancel when there is no SendDTMFTones in progress
	// after a cancel
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	// Test sending cancel when there is no SendDTMFTones in progress
	// and no call in progress
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());

	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this

	} // CCTsyCallControlFU::TestUnit0005L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0006
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMobilePhone::StartDTMFTone
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::StartDTMFTone
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Set up so that there is an active call for testing sending of DTMF
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TChar tone('1');
	TMockLtsyData2<TInt, TChar> toneLtsyData(callId, tone);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNotSupported);

	ASSERT_EQUALS(KErrNotSupported, iPhone.StartDTMFTone(tone));

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, KErrNotSupported);

	ASSERT_EQUALS(KErrNotSupported, iPhone.StartDTMFTone(tone));

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::StartDTMFTone
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, KErrNone);

	ASSERT_EQUALS(KErrNone, iPhone.StartDTMFTone(tone));

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::StartDTMFTone
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus terminatedStatus;
	iMockLTSY.NotifyTerminated(terminatedStatus);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, KErrNone);

	User::WaitForRequest(terminatedStatus);

	//-------------------------------------------------------------------------
	// TEST F: Extra coverage tests, extra scenarios
 	//-------------------------------------------------------------------------

	// Test StartDTMFTone when there is no active call
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	ASSERT_EQUALS(KErrEtelCallNotActive, iPhone.StartDTMFTone(tone));

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUnit0006L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0007
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMobilePhone::StopDTMFTone
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::StopDTMFTone
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Set up so that there is an active call for testing stopping of DTMF
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TMockLtsyData1<TInt> toneLtsyData(callId);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, data, KErrNotSupported);

	ASSERT_EQUALS(KErrNotSupported, iPhone.StopDTMFTone());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, KErrNotSupported);

	ASSERT_EQUALS(KErrNotSupported, iPhone.StopDTMFTone());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::StopDTMFTone
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, KErrNone);

	ASSERT_EQUALS(KErrNone, iPhone.StopDTMFTone());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::StopDTMFTone
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus terminatedStatus;
	iMockLTSY.NotifyTerminated(terminatedStatus);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, KErrNone);

	User::WaitForRequest(terminatedStatus);

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RMobilePhone::StopDTMFTone
 	//-------------------------------------------------------------------------

	// Test StopDTMFTone when there is no active call
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	ASSERT_EQUALS(KErrEtelCallNotActive, iPhone.StopDTMFTone());

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this

	} // CCTsyCallControlFU::TestUnit0007L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0008
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMobilePhone::SendDTMFTones
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SendDTMFTones
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Set up so that there is an active call for testing sending of DTMF
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TInfoName tones = _L("12345");
	TMockLtsyData2<TInt, TInfoName> toneLtsyData(callId, tones);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNotSupported);

	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNotSupported);

	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SendDTMFTones
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SendDTMFTones
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus terminatedStatus;
	iMockLTSY.NotifyTerminated(terminatedStatus);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	User::WaitForRequest(terminatedStatus);

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RMobilePhone::SendDTMFTones
 	//-------------------------------------------------------------------------

	// Test second clients requesting to send DTMF tones when one request is still
	// pending returns KErrServerBusy

	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, KErrNone);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	iPhone.SendDTMFTones(reqStatus, tones);
	TRequestStatus reqStatus2;
	phone2.SendDTMFTones(reqStatus2, tones);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// Test SendDtmfTone when there is no active call

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrEtelCallNotActive, reqStatus.Int());

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this);//phone2, telserver2, mobileCall, mobileLine, data, this

	} // CCTsyCallControlFU::TestUnit0008L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0009
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMobileCall::ReceiveUUI
 @SYMTestPriority High
 @SYMTestActions Invokes RMobileCall::ReceiveUUI
 @SYMTestExpectedResults Pass - client receives data from LTSY
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);
	
	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	_LIT(KUUI, "important");
	RMobileCall::TMobileCallUUI receiveUui;
	receiveUui.Append(KUUI);
	data.Close();
	TMockLtsyCallData1<RMobileCall::TMobileCallUUI> mockData(callId, mobileService, receiveUui);
	mockData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlReceiveUuiIndId, KErrNone, data);

	TRequestStatus uuiStatus;
	RMobileCall::TMobileCallUUI receivedUui;
	mobileCall.ReceiveUUI(uuiStatus, receivedUui);
	User::WaitForRequest(uuiStatus);
	ASSERT_EQUALS(KErrNone, uuiStatus.Int());
	ASSERT_EQUALS(0, receivedUui.Compare(receiveUui));
	
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	
	DriverHangUpCallL(callId, hangUpCause);
	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 

	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUnit0009L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0010
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendDTMFTones and waits for it to timeout
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::SendDTMFTones and waits for it to timeout
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);
	
	
	TBuf<5> tones = _L("12345");
	TMockLtsyData2<TInt, TBuf<5> > toneLtsyData(callId, tones);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId, data, KErrNone);
	data.Close();
	
	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);

	//KMmPhoneSendDTMFTonesTimeOut = 125, 
	//Life timers come every 10s thus 125s / 10s = 12 expects to queue.
	TUint32 duration = 10; //this is the length of the call and its not possible to be certain of this due to
							//e.g. logging statements so this argument is not actually checked in the mockltsy
	TMockLtsyData1<TUint32> durationLtsyData(duration);
	durationLtsyData.SerialiseL(data);

	for(TInt i = 0; i < 12; ++i)
		{
		iMockLTSY.ExpectL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, data, KErrNone);
		iMockLTSY.CompleteL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, KErrNone);
		}
	data.Close();	
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated);
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(reqStatusTerminated.Int(),KErrNone);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUnit0010L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0011
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for RMobilePhone::StartDTMFTone and RMobilePhone::StopDTMFTone timeout
 @SYMTestPriority High
 @SYMTestActions Invokes RMobilePhone::StartDTMFTone and RMobilePhone::StopDTMFTone waits for it to timeout
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);

	TChar tone('1');
	TMockLtsyData2<TInt, TChar> toneLtsyData(callId, tone);
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNone);

	iMockLTSY.ExpectL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, data, KErrNone);

	iMockLTSY.CompleteL(MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId, KErrNone);

	TMockLtsyData1<TInt> toneLtsyData2(callId);
	toneLtsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId, data, KErrNone);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	ASSERT_EQUALS(KErrTimedOut, iPhone.StartDTMFTone(tone));

	// Now test timeout of stop
	ASSERT_EQUALS(KErrNone, iPhone.StartDTMFTone(tone));
	ASSERT_EQUALS(KErrTimedOut, iPhone.StopDTMFTone());

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 

	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUnit0011L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for ICC Call Forwarding Status Change Indicator
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyIccCallForwardingStatusChange()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus reqStatus;
	
	TInt error = KErrNone;
	
	RBuf8 data;
	CleanupClosePushL(data);
	RMmCustomAPI::TCFIndicators cfIndicators;
	TPckg<RMmCustomAPI::TCFIndicators> cfIndicatorsPckg(cfIndicators);
	
	
	RMobilePhone::TMobileAddress phoneNumber;
	phoneNumber.iTelNumber = _L("012345");
	RMobilePhone::TCFUIndicatorStatusFlags indicators = RMobilePhone::KCFUIndicatorFax | RMobilePhone::KCFUIndicatorData;
	RMmCustomAPI::TMultipleSubscriberProfileID subscriberId (RMmCustomAPI::KProfileIdentityThree);
	
	TMockLtsyData3<RMobilePhone::TMobileAddress,
    	RMobilePhone::TCFUIndicatorStatusFlags,
    	RMmCustomAPI::TMultipleSubscriberProfileID> mockLtsyData(phoneNumber, indicators, subscriberId);

	mockLtsyData.SerialiseL(data);
	
	customApi.NotifyIccCallForwardingStatusChange(reqStatus,cfIndicatorsPckg);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyIccCallForwardingStatusChangeIndId,error,data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_EQUALS(cfIndicators.iIndicator,indicators);
	ASSERT_EQUALS(cfIndicators.iMultipleSubscriberProfileID, subscriberId);
	ASSERT_TRUE(cfIndicators.iCFNumber.iTelNumber.Compare(phoneNumber.iTelNumber) == 0);
	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0013
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomAPI::GetAlsBlocked and RMmCustomAPI::SetAlsBlocked
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomAPI::GetAlsBlocked and RMmCustomAPI::SetAlsBlocked
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    // A. Get Status
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId, KErrNone);
	RMmCustomAPI::TGetAlsBlockStatus compData = RMmCustomAPI::EBlockStatusInactive;
	TMockLtsyData1<RMmCustomAPI::TGetAlsBlockStatus> alsLtsyData1(compData);
	alsLtsyData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId, KErrNone, data);

	RMmCustomAPI::TGetAlsBlockStatus status;
	ASSERT_EQUALS(KErrNone, customApi.GetAlsBlocked(status));
	ASSERT_EQUALS(compData, status);

    // B. Set blocked
    data.Close();
    RMmCustomAPI::TSetAlsBlock setData = RMmCustomAPI::EActivateBlock;
	TMockLtsyData1<RMmCustomAPI::TSetAlsBlock> alsLtsyData2(setData);
	alsLtsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId, KErrNone);

	ASSERT_EQUALS(KErrNone, customApi.SetAlsBlocked(setData));

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customApi, data, this
	} // CCTsyCallControlFU::TestUnit0013L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0014
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomAPI::GetLifeTime
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomAPI::GetLifeTime 
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    // A. Test when there is no life time data
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone);
    
	RMmCustomAPI::TLifeTimeData compData;
    compData.iCaps = 0;
	TMockLtsyData1<RMmCustomAPI::TLifeTimeData> ltLtsyData1(compData);
	ltLtsyData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone, data);

	RMmCustomAPI::TLifeTimeData lifeTime;
	RMmCustomAPI::TLifeTimeDataPckg lifeTimePckg(lifeTime);
	ASSERT_EQUALS(KErrNone, customApi.GetLifeTime(lifeTimePckg));
	ASSERT_EQUALS(compData.iCaps, lifeTime.iCaps);


    // B. Test when there is only life time data
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone);
    
    compData.iCaps = RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime;
    compData.iHours = 12234;
    compData.iMinutes = 14;
    data.Close();
	TMockLtsyData1<RMmCustomAPI::TLifeTimeData> ltLtsyData2(compData);
	ltLtsyData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone, data);

	ASSERT_EQUALS(KErrNone, customApi.GetLifeTime(lifeTimePckg));
	ASSERT_EQUALS(compData.iHours, lifeTime.iHours);
	ASSERT_EQUALS(compData.iMinutes, lifeTime.iMinutes);

    // C. Test when there is only manufacturing date data
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone);
    
    compData.iCaps = RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsManufacturerDate;
    compData.iManufacturingDate.SetYear(1995);
    compData.iManufacturingDate.SetMonth(EAugust);
    compData.iManufacturingDate.SetDay(21);
    data.Close();
	TMockLtsyData1<RMmCustomAPI::TLifeTimeData> ltLtsyData3(compData);
	ltLtsyData3.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone, data);

	ASSERT_EQUALS(KErrNone, customApi.GetLifeTime(lifeTimePckg));
	ASSERT_EQUALS(compData.iManufacturingDate.Year(), lifeTime.iManufacturingDate.Year());
	ASSERT_EQUALS(compData.iManufacturingDate.Month(), lifeTime.iManufacturingDate.Month());
	ASSERT_EQUALS(compData.iManufacturingDate.Day(), lifeTime.iManufacturingDate.Day());

    // D. Test when there is manufacturing date data and lifetime data
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone);
    
    compData.iCaps = RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsManufacturerDate | RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime;
    compData.iManufacturingDate.SetYear(2000);
    compData.iManufacturingDate.SetMonth(EJune);
    compData.iManufacturingDate.SetDay(12);
    compData.iHours = 31234;
    compData.iMinutes = 43;
    data.Close();
	TMockLtsyData1<RMmCustomAPI::TLifeTimeData> ltLtsyData4(compData);
	ltLtsyData3.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId, KErrNone, data);

	ASSERT_EQUALS(KErrNone, customApi.GetLifeTime(lifeTimePckg));
	ASSERT_EQUALS(compData.iHours, lifeTime.iHours);
	ASSERT_EQUALS(compData.iMinutes, lifeTime.iMinutes);
	ASSERT_EQUALS(compData.iManufacturingDate.Year(), lifeTime.iManufacturingDate.Year());
	ASSERT_EQUALS(compData.iManufacturingDate.Month(), lifeTime.iManufacturingDate.Month());
	ASSERT_EQUALS(compData.iManufacturingDate.Day(), lifeTime.iManufacturingDate.Day());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //customApi, data, this
	} // CCTsyCallControlFU::TestUnit0014L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0015
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomAPI::TerminateCall
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomAPI::TerminateCall 
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0015L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    // A. Terminate a specific call
    // Create an active call
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

    // Getting mock ltsy ready
    data.Close();
	TMockLtsyData1<TInt> termLtsyData1(callId);
	termLtsyData1.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId, KErrNone, data);

    // Activate the API
	ASSERT_EQUALS(KErrNone, customApi.TerminateCall(callInfo.iCallName));

    // Hang up the call
	DriverHangUpCallL(callId, KErrGsmCCNormalCallClearing);
    ClientHangUpCallL(mobileCall);

    // B. All calls
    // Create an active call
    callId = 71;
	DriverDialCallL(callId, RMobilePhone::EVoiceService);
    RMobileLine mobileLine2;
	CleanupClosePushL(mobileLine2);
	RMobileCall mobileCall2;
	CleanupClosePushL(mobileCall2);
	ClientDialCallL(mobileLine2, mobileCall2, RMobilePhone::EVoiceService);

    // Getting mock ltsy ready
	iMockLTSY.ExpectL(MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId, KErrNone);

    // Activate the API
	TName terminateAll(KTerminateAllCalls);
	ASSERT_EQUALS(KErrNone, customApi.TerminateCall(terminateAll));

    // Hang up the call
	DriverHangUpCallL(callId, KErrGsmCCNormalCallClearing);
    
	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall2);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());		
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); //mobileCall2, mobileLine2, mobileCall, mobileLine, customApi, data, this
	} // CCTsyCallControlFU::TestUnit0015L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0016
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomAPI::GetRemoteAlertingToneStatus and RMmCustomAPI::NotifyRemoteAlertingToneStatusChange
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomAPI::GetRemoteAlertingToneStatus and RMmCustomAPI::NotifyRemoteAlertingToneStatusChange
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0016L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    // A. Test without notification from the LTSY
    RMmCustomAPI::TRemoteAlertingToneStatus status;
	ASSERT_EQUALS(KErrNone, customApi.GetRemoteAlertingToneStatus(status));
    ASSERT_EQUALS(RMmCustomAPI::EUiNoTone, status);

    // B. LTSY notify that the status was set
	TRequestStatus terminatedStatus;
	iMockLTSY.NotifyTerminated(terminatedStatus);
    RMmCustomAPI::TRemoteAlertingToneStatus compStatus = RMmCustomAPI::EUiRbtTone;
	TMockLtsyData1<RMmCustomAPI::TRemoteAlertingToneStatus> compData1(compStatus);
    data.Close();
    compData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlRemoteAlertingToneChangeIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

    // Activate the API
	ASSERT_EQUALS(KErrNone, customApi.GetRemoteAlertingToneStatus(status));
    ASSERT_EQUALS(compStatus, status);


    // C. Check the change notification
    TRequestStatus notifyStatus;
    customApi.NotifyRemoteAlertingToneStatusChange(notifyStatus, status);
    compStatus =  RMmCustomAPI::EUiStopTone;
	TMockLtsyData1<RMmCustomAPI::TRemoteAlertingToneStatus> compData2(compStatus);
    data.Close();
    compData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlRemoteAlertingToneChangeIndId, KErrNone, data);
	User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(compStatus, status);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //customApi, data, this
	} // CCTsyCallControlFU::TestUnit0016L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0017
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomAPI::GetIccCallForwardingIndicatorStatus
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomAPI::GetIccCallForwardingIndicatorStatus
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0017L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    // Getting mock ltsy ready
    data.Close();
    iMockLTSY.ExpectL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId);
	RMobilePhone::TMobilePhoneCFUIndicatorV6 compIndicator;
	compIndicator.iMultipleSubscriberProfileID = RMobilePhone::KProfileIdentityThree;
	compIndicator.iIndicator = RMobilePhone::KCFUIndicatorVoice | RMobilePhone::KCFUIndicatorData;
	compIndicator.iCFNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	compIndicator.iCFNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
	_LIT(KTelNumber, "020102010201");
	compIndicator.iCFNumber.iTelNumber = KTelNumber; 
	TMockLtsyData1<RMobilePhone::TMobilePhoneCFUIndicatorV6> indicatorData(compIndicator);
	data.Close();
	indicatorData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId,KErrNone,data,0);

    // Activate the API
    RMmCustomAPI::TCFIndicators indicator;
    RMmCustomAPI::TCFIndicatorsPckg indicatorPckg(indicator);
	TRequestStatus apiStatus;
	customApi.GetIccCallForwardingIndicatorStatus(apiStatus, indicatorPckg);
	User::WaitForRequest(apiStatus);
	ASSERT_EQUALS(KErrNone, apiStatus.Int());
	ASSERT_EQUALS(compIndicator.iCFNumber.iTelNumber, indicator.iCFNumber.iTelNumber);
	ASSERT_EQUALS(compIndicator.iCFNumber.iTypeOfNumber, indicator.iCFNumber.iTypeOfNumber);
	ASSERT_EQUALS(compIndicator.iCFNumber.iNumberPlan, indicator.iCFNumber.iNumberPlan);
	ASSERT_EQUALS(compIndicator.iIndicator, indicator.iIndicator);
	// CTSY sets the value of iMultipleSubscriberProfileID into a private repository, but not to the API struct.
    // ASSERT_EQUALS(RMmCustomAPI::KProfileIdentityThree, indicator.iMultipleSubscriberProfileID);
    

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //customApi, data, this
	} // CCTsyCallControlFU::TestUnit0017L


/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0018
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY Dispatcher for RMmCustomAPI::CheckAlsPpSupport 
 @SYMTestPriority High
 @SYMTestActions Invokes RMmCustomAPI::CheckAlsPpSupport 
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0018L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId, KErrNone);


	RMmCustomAPI::TAlsSupport compData = RMmCustomAPI::EAlsSupportOff;
	TMockLtsyData1<RMmCustomAPI::TAlsSupport> alsLtsyData1(compData);
	alsLtsyData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId, KErrNone, data);

	RMmCustomAPI::TAlsSupport support;
	ASSERT_EQUALS(KErrNone, customApi.CheckAlsPpSupport(support));
	ASSERT_EQUALS(compData, support);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //customApi, data, this
	} // CCTsyCallControlFU::TestUnit0018L

/**
 @SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0019
 @SYMComponent telephony_ctsy
 @SYMTestCaseDesc Test support in CTSY for ECtsyUpdateLifeTimeReq/ECtsyUpdateLifeTimeComp IPCs by keeping a call open for longer than 10s
 @SYMTestPriority High
 @SYMTestActions Invokes ECtsyUpdateLifeTimeReq/ECtsyUpdateLifeTimeComp
 @SYMTestExpectedResults Pass
 @SYMTestType UT
 */
void CCTsyCallControlFU::TestUnit0019L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);




	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);


	AssertMockLtsyStatusL();
	
	//expect the lifetimer call to come through
	
	TUint32 duration = 10;
	TMockLtsyData1<TUint32> durationLtsyData(duration);
	durationLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, data, KErrNone);
	data.Close();
	iMockLTSY.CompleteL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, KErrNone);
	
	User::After(12500000);
	
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);
	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	
	ClientHangUpCallL(mobileCall);

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, &data); //mobileCall, mobileLine, data
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	} // CCTsyCallControlFU::TestUnit0019L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UN0020
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for mobile originated voice call with no FDN check
@SYMTestPriority High
@SYMTestActions RMobileCall::DialNoFdnCheck() for a voice call
@SYMTestExpectedResults Pass - Call reaches connected state and call is hung up
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUnit0020L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	
	//Test voice call
	
	iMockLTSY.NotifyTerminated(reqStatus); 
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService, EFalse, EFalse, EFalse);

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService, EFalse, EFalse);

	ClientHangUpCallL(mobileCall);
	

	mobileCall.Close();
	mobileLine.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //mobileCall, mobileLine, this
	} // CCTsyCallControlFU::TestUnit0020L


/**
@SYMTestCaseID BA-CTSYD-DIS-CALLCONTROL-UT0021
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for mobile originated data call with no FDN check
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck() for a data call
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallControlFU::TestUnit0021L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	DriverDialCallL(callId, mobileService, EFalse, EFalse, EFalse);

	// KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId
	RMobileCall::TMobileCallDataCapsV1 completeCallCaps;
	completeCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed48000;
	completeCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV34;
	completeCallCaps.iServiceCaps = RMobileCall::KCapsPacketAccessSyncUDI;
	completeCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
	completeCallCaps.iHscsdSupport = ETrue;
	completeCallCaps.iMClass = 2;
	completeCallCaps.iMaxRxTimeSlots = 4;
	completeCallCaps.iMaxTxTimeSlots = 8;
	completeCallCaps.iTotalRxTxTimeSlots = 10;
	completeCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding96;
	completeCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryDownlink;
	completeCallCaps.iUserInitUpgrade = ETrue;
	completeCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
	completeCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisRxDirection;
	data.Close();
	TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1>
		completeDataCapsData(callId, RMobilePhone::EServiceUnspecified, completeCallCaps);
	completeDataCapsData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId, KErrNone, data);

    _LIT(KDataPort, "DATAPORT::0");
    RCall::TCommPort commPort;
    commPort.iPort = KDataPort;
    data.Close();
    TMockLtsyData2<TInt,RCall::TCommPort> loanDataPortExpData(callId,commPort);
	loanDataPortExpData.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId, data);
    iMockLTSY.ExpectL(MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId, data);

    
	// MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
	data.Close();
	TMockLtsyData2<TInt, RMobileCall::TMobileHscsdCallParamsV8> paramData(callId, hscsdParams);
	paramData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId, data);
	
    TMockLtsyData1<TInt> mockParamData(callId);
    data.Close();
    mockParamData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId, KErrNone, data);

	// KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId
    RMobileCall::TMobileCallHscsdInfoV8 hscsdInfo;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;

    RMobileCall::TMobileCallDataCapsV1 completeCallCaps2 = completeCallCaps;
    completeCallCaps2.iHscsdSupport = ETrue;
    completeCallCaps2.iSpeedCaps = RMobileCall::KCapsSpeed57600;
    completeCallCaps2.iMaxRxTimeSlots = 3;
    completeCallCaps2.iMaxTxTimeSlots = 5;

    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV8, RMobileCall::TMobileCallDataCapsV1>
    		hscsdData(1, RMobilePhone::ECircuitDataService, hscsdInfo, completeCallCaps2);
    data.Close();
    hscsdData.SerialiseL(data);

	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId, KErrNone, data);

	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	DriverHangUpCallL(callId, hangUpCause);

	// Client Side Test

    RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);

	ClientDialCallL(mobileLine, mobileCall, mobileService, EFalse, EFalse);

	RCall::TCommPort dataPort;
	TRequestStatus reqStatus;
	mobileCall.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(0, dataPort.iPort.Compare(KDataPort()));

	TInt err = mobileCall.RecoverDataPort();
	ASSERT_EQUALS(KErrNone, err);

	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	mobileCall.GetMobileDataCallCaps(callCapsPckg);
	ASSERT_TRUE(TComparator<RMobileCall::TMobileCallDataCapsV1>::IsEqual(completeCallCaps, callCaps));

	TRequestStatus hscsdInfoStatus;
	RMobileCall::TMobileCallHscsdInfoV8 hscsdInfo2;
	RMobileCall::TMobileCallHscsdInfoV8Pckg hscsdInfoPckg(hscsdInfo2);
	mobileCall.NotifyHscsdInfoChange(hscsdInfoStatus, hscsdInfoPckg);

	TRequestStatus dataCallCapsStatus;
	RMobileCall::TMobileCallDataCapsV1 dataCallCaps2;
	RMobileCall::TMobileCallDataCapsV1Pckg dataCallCapsPckg(dataCallCaps2);
	mobileCall.NotifyMobileDataCallCapsChange(dataCallCapsStatus, dataCallCapsPckg);

	TRequestStatus hscsdParamStatus;
	mobileCall.SetDynamicHscsdParams(hscsdParamStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(hscsdParamStatus);

	User::WaitForRequest(hscsdInfoStatus);
	ASSERT_EQUALS(KErrNone, hscsdInfoStatus.Int());
	ASSERT_EQUALS(hscsdInfo2.iAiur, hscsdInfo.iAiur);
	ASSERT_EQUALS(hscsdInfo2.iRxTimeSlots, hscsdInfo.iRxTimeSlots);
	ASSERT_EQUALS(hscsdInfo2.iTxTimeSlots, hscsdInfo.iTxTimeSlots);
	ASSERT_EQUALS(hscsdInfo2.iCodings, hscsdInfo.iCodings);

	User::WaitForRequest(dataCallCapsStatus);
	ASSERT_EQUALS(KErrNone, dataCallCapsStatus.Int());
	ASSERT_EQUALS(completeCallCaps2.iSpeedCaps, dataCallCaps2.iSpeedCaps);
	ASSERT_EQUALS(completeCallCaps2.iHscsdSupport, dataCallCaps2.iHscsdSupport);
	ASSERT_EQUALS(completeCallCaps2.iMaxRxTimeSlots, dataCallCaps2.iMaxRxTimeSlots);
	ASSERT_EQUALS(completeCallCaps2.iMaxTxTimeSlots, dataCallCaps2.iMaxTxTimeSlots);

	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 	
	ClientHangUpCallL(mobileCall);

	mobileCall.Close();
	mobileLine.Close();
	data.Close();

	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // mobileCall, mobileLine, data, this
	} // CCTsyCallControlFU::TestUnit0021L


//
// Other helper functions
//


void CCTsyCallControlFU::ClientSetIsvPubSubKeyL()
	{
	// CTelephony requires property to be set otherwise it thinks flight mode
	// is on.
	TInt err = RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn);
	User::LeaveIfError(err);
	} // CCTsyCallControlFU::ClientSetIsvPubSubKeyL



void CCTsyCallControlFU::DriverCreateIncomingCallL(TInt aCallId, RMobilePhone::TMobileService aMode)
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
	} // CCTsyCallControlFU::DriverCreateIncomingCallL

void CCTsyCallControlFU::DriverAnswerIncomingCallL(TInt aCallId, RMobilePhone::TMobileService aMode, TBool aIsIsvCall,TInt aAnswerError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

    TMockLtsyData2<TInt, TInt> data1(aCallId, aIsIsvCall);
    data.Close();
    data1.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId, data);

    TMockLtsyData1<TInt> data2(aCallId);
    data.Close();
    data2.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId, aAnswerError, data);

    if (aAnswerError == KErrNone)
    	{
	    RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusAnswering;
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(aCallId, aMode, callStatus);
	    data.Close();
	    mockCallData3.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	    callStatus = RMobileCall::EStatusConnected;
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(aCallId, aMode, callStatus);
	    data.Close();
	    mockCallData4.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
    	}
    else
    	{
    	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusIdle;
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(aCallId, aMode, callStatus);
	    data.Close();
	    mockCallData3.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId , KErrNone, data);
    	}

    data.Close();
    CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlFU::DriverAnswerIncomingCallL


void CCTsyCallControlFU::DriverResumeCallL(TInt aCallId, TInt aResumeError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData1<TInt> mockData2(aCallId);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId, data);

	TMockLtsyData1<TInt> resumeData(aCallId);
	data.Close();
	resumeData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId, aResumeError, data);

    if (aResumeError == KErrNone)
    	{
		RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
	    		aCallId, RMobilePhone::EServiceUnspecified, callStatus);
	    data.Close();
	    mockCallData2.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
    	}

    data.Close();
    CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlFU::DriverResumeCallL

/**
 * Swap the call.
 *
 * @param aCallId Call ID of held call to swap.
 * @param aSwapError Error returned by LTSY in response to the swap request.
 * @param aNewCallStatus New status of the call 1 if aSwapError = KErrNone
 *
 * In the case where aSwapError passed is not KErrNone, the new call statuses
 * are irrelevant as the calls don't change state.
 */
void CCTsyCallControlFU::DriverSwapCallL(TInt aCallId, TInt aSwapError, 
								RMobileCall::TMobileCallStatus aNewCallStatus)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData1<TInt> mockData2(aCallId);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId, data);

    iMockLTSY.CompleteL(MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId, aSwapError);

    if (aSwapError == KErrNone)
    	{
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
	    		aCallId, RMobilePhone::EServiceUnspecified, aNewCallStatus);
	    data.Close();
	    mockCallData2.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
    	}

    data.Close();
    CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlFU::DriverSwapCallL


/**
 * Swap the call.
 *
 * @param aCallId1 Call ID of held call to swap.
 * @param aCallId2 Call ID of connected call to swap.
 * @param aSwapError Error returned by LTSY in response to the swap request.
 * @param aNewCall1Status New status of the call 1 if aSwapError = KErrNone
 * @param aNewCall2Status New status of the call 2 if aSwapError = KErrNone
 *
 * In the case where aSwapError passed is not KErrNone, the new call statuses
 * are irrelevant as the calls don't change state.
 */
void CCTsyCallControlFU::DriverSwapCallL(TInt aCallId1, TInt aCallId2,
	TInt aSwapError, RMobileCall::TMobileCallStatus aNewCall1Status,
	RMobileCall::TMobileCallStatus aNewCall2Status)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData2<TInt, TInt> mockData2(aCallId1, aCallId2);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId, data);

    iMockLTSY.CompleteL(MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId, aSwapError);

    if (aSwapError == KErrNone)
    	{
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(
	    		aCallId1, RMobilePhone::EServiceUnspecified, aNewCall1Status);
	    data.Close();
	    mockCallData2.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);

	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(
	    		aCallId2, RMobilePhone::EServiceUnspecified, aNewCall2Status);
	    data.Close();
	    mockCallData3.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
    	}

    data.Close();
    CleanupStack::PopAndDestroy(1, &data);
	} // CCTsyCallControlFU::DriverSwapCallL


/**
 * Waits for an incoming call notification and opens the call.
 *
 * @param aMobileCall RMobileCall subsession, should not be open before calling this function.
 * Pushed on CleanupStack in this function.
 *
 * @param aMobileLine Line that will ring.
 * @param aCallName Populated with the call name when incoming call arrives.
 */
void CCTsyCallControlFU::ClientWaitForIncomingCallNotificationLC(
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
	} // CCTsyCallControlFU::ClientWaitForIncomingCallNotificationLC

void CCTsyCallControlFU::ClientAnswerIncomingCallL(RMobileCall& aMobileCall, TInt aExpectedAnswerError)
	{
	TRequestStatus notifyStatus;
	RMobileCall::TMobileCallStatus callStatus;
	aMobileCall.NotifyMobileCallStatusChange(notifyStatus, callStatus);

    TRequestStatus ansStatus;
    aMobileCall.AnswerIncomingCall(ansStatus);

    RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);

    if (aExpectedAnswerError == KErrNone)
    	{
	    expectedCallStatuses.AppendL(RMobileCall::EStatusAnswering);
	    expectedCallStatuses.AppendL(RMobileCall::EStatusConnected);
    	}
    else
    	{
    	expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    	}

    ClientWaitForCallStatusNotifications(aMobileCall, notifyStatus, callStatus, expectedCallStatuses, KErrNone);

    User::WaitForRequest(ansStatus);
	ASSERT_EQUALS(aExpectedAnswerError, ansStatus.Int());

	aMobileCall.GetMobileCallStatus(callStatus);

	if (aExpectedAnswerError == KErrNone)
		{
		ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);
		}
	else
		{
		ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);
		}

	expectedCallStatuses.Close();
	CleanupStack::PopAndDestroy(1, &expectedCallStatuses);
	} // CCTsyCallControlFU::ClientAnswerIncomingCallL

