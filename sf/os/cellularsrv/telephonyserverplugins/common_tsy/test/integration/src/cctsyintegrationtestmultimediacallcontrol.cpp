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
// Test step definitions for the MultimediaCallControl functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestmultimediacallcontrol.h"
#include "cctsyinidata.h"

/*
_LIT8(KWriteTestData, "Some data");
*/
	
CCTSYIntegrationTestMultimediaCallControlBase::CCTSYIntegrationTestMultimediaCallControlBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),
			iNetworkTestHelper(*this),
			iCallControlTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestMultimediaCallControlBase::~CCTSYIntegrationTestMultimediaCallControlBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestMultimediaCallControl0001::CCTSYIntegrationTestMultimediaCallControl0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0001::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0001::~CCTSYIntegrationTestMultimediaCallControl0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0001
 * @SYMFssID BA/CTSY/MMCC-0001
 * @SYMTestCaseDesc Ensure a video call can be made.
 * @SYMTestPriority High
 * @SYMTestActions RCall::LoanDataPort, RCall::RecoverDataPort, RCall:HangUp, RCall::DialCancel, RCall::GetCallParams
 * @SYMTestExpectedResults Pass - Dialling multimedia call is successful when parameters are correct. Cancelling dial is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 1. 
	RMobileCall& call1= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

    // Check current networkmode returns ENetworkModeWcdma
	RMobilePhone::TMobilePhoneNetworkMode mode;
	CHECK_EQUALS_L(phone.GetCurrentMode(mode), KErrNone, _L("RMobilePhone::GetCurrentMode returned with an error"));
	CHECK_EQUALS_L(mode, RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentMode did not return ENetworkModeWcdma as expected"));
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	
	// 1. Set up multimedia mobile data call parameters. 
	RMobileCall::TMobileHscsdCallParamsV1 multimediaParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg checkParamsPkg(multimediaParams);
	
	// valid parameters 
	multimediaParams.iIdRestrict = RMobileCall::ESendMyId;
	multimediaParams.iAckTimer = 20;
	multimediaParams.iMSToModemWindowSize = 2;
	multimediaParams.iModemToMSWindowSize = 2;	
	multimediaParams.iProtocol = RMobileCall::EProtocolPstnMultimedia;
	multimediaParams.iQoS = RMobileCall::EQoSNonTransparent;
	multimediaParams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	multimediaParams.iResequencingPeriod = 1;
	multimediaParams.iRetransmissionAttempts = 2;
	multimediaParams.iService = RMobileCall::EServiceDataCircuitAsync;
	multimediaParams.iSpeed = RMobileCall::ESpeed64000;
	multimediaParams.iUseEdge = EFalse;
	multimediaParams.iV42bisCodewordsNum = 0;
    multimediaParams.iV42bisMaxStringLength = 0;
    multimediaParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	multimediaParams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	multimediaParams.iCodings = RMobileCall::KCapsAiurCoding144;
	multimediaParams.iUserInitUpgrade = EFalse;
	multimediaParams.iMaxTimeSlots = 15;
	multimediaParams.iWantedRxTimeSlots = 15;
	multimediaParams.iWantedAiur = RMobileCall::EAiurBpsUnspecified;

	// post a notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyStatusChangeStatus (mobileLine,&RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus lineStatus;
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);
	
	// Dial a number that answers. 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);

    // Failing CSD calls (see $CTSYProblem described below).
    // code can be added in after multimedia calls are working.

	// $CTSYProblem: dial returns -18 or -6 depending of selected parametes (protocol)
    // EProtocolPstnMultimedia = -6, EProtocolV34 = -18
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue
    /*
	call1.Dial(dialStatus, checkParamsPkg, number); 
	TEST_CHECK_POINT_L(_L("Couldn't dial a call, no point continuing."));
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));
	
	// Verify line status goes from ringing to connected. 
	RCall::TStatus expectedLineStatus = RCall::EStatusRinging;
	const TInt expectedStatus = KErrNone;
	
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);
	
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);													 													 
	expectedLineStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);
	// Loan comm port.
	RCall::TCommPort commPort;
	ASSERT_EQUALS(call1.LoanDataPort(commPort), KErrNone, _L("RCall::LoanCommPort returned an error"));
	
	// Get call parameters with RMobileCall::TMobileDataCallParamsVx and verify it is as expected. 
	ASSERT_EQUALS(call1.GetCallParams(checkParamsPkg), KErrNone, _L("RCall::GetCallParams returned an error"));
	ASSERT_TRUE(multimediaParams.iSpeed = RMobileCall::ESpeed64000, _L("RCall::GetCallParams returned wrongly RMobileCall::ESpeedUnspecified"));
	ASSERT_TRUE(multimediaParams.iProtocol = RMobileCall::EProtocolPstnMultimedia, _L("TMobileDataCallParamsV1 returns aParams.iProtocol"));
	ASSERT_EQUALS(multimediaParams.iService,  RMobileCall::EServiceDataCircuitAsync, _L("RCall::GetCallParams service was not the same that used in dial"));
	 	
	// Send some data. 
	RCommServ cs;
	cs.Connect();
	RComm port;
	port.Open(cs, commPort.iPort, ECommShared);
	TRequestStatus writeStatus;
	port.Write(writeStatus, KWriteTestData); 
	port.Close();
	cs.Close();
	
	// Recover comm port. 
	ASSERT_EQUALS(call1.RecoverDataPort(), KErrNone, _L("RMobileCall::RecoverDataPort() returned an error"));

	// Hang up. 
	*/
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
    
    /*
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	*/
	
	// 2. Set up multimedia mobile data call parameters. 
	RMobileCall::TMobileHscsdCallParamsV1 multimediaParams2;
	RMobileCall::TMobileHscsdCallParamsV1Pckg checkParamsPkg2(multimediaParams2);
	
	// valid parameters 
	multimediaParams2.iIdRestrict = RMobileCall::ESendMyId;
	multimediaParams2.iAckTimer = 20;
	multimediaParams2.iMSToModemWindowSize = 2;
	multimediaParams2.iModemToMSWindowSize = 2;	
	multimediaParams2.iProtocol = RMobileCall::EProtocolPstnMultimedia;
	multimediaParams2.iQoS = RMobileCall::EQoSNonTransparent;
	multimediaParams2.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	multimediaParams2.iResequencingPeriod = 1;
	multimediaParams2.iRetransmissionAttempts = 2;
	multimediaParams2.iService = RMobileCall::EServiceDataCircuitAsync;
	multimediaParams2.iSpeed = RMobileCall::ESpeed64000;
	multimediaParams2.iUseEdge = EFalse;
	multimediaParams2.iV42bisCodewordsNum = 0;
    multimediaParams2.iV42bisMaxStringLength = 0;
    multimediaParams2.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
    multimediaParams.iMaxTimeSlots = 15;
	multimediaParams.iWantedRxTimeSlots = 15;
	multimediaParams.iWantedAiur = RMobileCall::EAiurBpsUnspecified;
	
	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyCallStatusChangeStatus (call1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyCallStatusChangeStatus);
	call1.NotifyStatusChange(notifyCallStatusChangeStatus,callStatus);

	/* 
	// post a notifier for RLine::NotifyStatusChange
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);
	
	// Dial a number which does not answer.  
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber2, number) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	// $CTSYProblem: dial returns -18 or -6 depending of selected parametes (protocol)
    // EProtocolPstnMultimedia = -6, EProtocolV34 = -18
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue

	call1.Dial(dialStatus, checkParamsPkg2, number); 
	TEST_CHECK_POINT_L(_L("Couldn't dial a call, no point continuing."));
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));
	
	// Verify line status goes to ringing. 
	expectedLineStatus = RCall::EStatusRinging;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);
														 
	// Cancel dialling. 
	call1.DialCancel();
	
	
	// Verify call and line status is idle. 
	RCall::TStatus expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);
														 
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);													
	expectedLineStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);
	*/
														 
	// 3. Set up multimedia mobile data call with bad parameters in TMobileDataCallParamsVx. 
	RMobileCall::TMobileHscsdCallParamsV1 multimediaParams3;
	RMobileCall::TMobileHscsdCallParamsV1Pckg checkParamsPkg3(multimediaParams3);
	
	// valid parameters with invalid iProtocol parameter 
	multimediaParams3.iIdRestrict = RMobileCall::ESendMyId;
	multimediaParams3.iAckTimer = 20;
	multimediaParams3.iMSToModemWindowSize = 2;
	multimediaParams3.iModemToMSWindowSize = 2;	
	multimediaParams3.iProtocol = RMobileCall::EProtocolPstnMultimedia;
	multimediaParams3.iQoS = RMobileCall::EQoSNonTransparent;
	multimediaParams3.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	multimediaParams3.iResequencingPeriod = 1;
	multimediaParams3.iRetransmissionAttempts = 2;
	multimediaParams3.iService = RMobileCall::EServiceDataCircuitSync;
	multimediaParams3.iSpeed = RMobileCall::ESpeed64000;
	multimediaParams3.iUseEdge = EFalse;
	multimediaParams3.iV42bisCodewordsNum = 0;
    multimediaParams3.iV42bisMaxStringLength = 0;
    multimediaParams3.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
    multimediaParams.iMaxTimeSlots = 15;
	multimediaParams.iWantedRxTimeSlots = 15;
	multimediaParams.iWantedAiur = RMobileCall::EAiurBpsUnspecified;
	
	// Dial a number that answers. 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	// $CTSYProblem: dial returns -18 or -6 depending of selected parametes (protocol)
    // EProtocolPstnMultimedia = -6, EProtocolV34 = -18
    // All CSD call are currently failing, this is not a CTSY problem because CTSY
    // has no control over this issue
	call1.Dial(dialStatus, checkParamsPkg2, number); 

	// Verify dialling fails. 
	ASSERT_TRUE((dialStatus.Int()!= KErrNone), _L("RCall::Dial did not returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	
	// Pop
	// notifyStatusChangeStatus
	// dialStatus
	// hangUpStatus
	// notifyCallStatusChangeStatus
	CleanupStack::PopAndDestroy(4, &notifyStatusChangeStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0001");
	}



CCTSYIntegrationTestMultimediaCallControl0002::CCTSYIntegrationTestMultimediaCallControl0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0002::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0002::~CCTSYIntegrationTestMultimediaCallControl0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0002
 * @SYMFssID BA/CTSY/MMCC-0002
 * @SYMTestCaseDesc Ensure a video call can be hung up by originator.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::GetMobileCallInfo, RLine::NotifyStatusChange, RCall::HangUp, RCall::GetCallParams
 * @SYMTestExpectedResults Pass - Video call is hung up.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 1. 
	RMobileCall& call1= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);
	
    // Check current networkmode returns ENetworkModeWcdma
	RMobilePhone::TMobilePhoneNetworkMode mode;
	CHECK_EQUALS_L(phone.GetCurrentMode(mode), KErrNone, _L("RMobilePhone::GetCurrentMode returned with an error"));
	CHECK_EQUALS_L(mode, RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentMode did not return ENetworkModeWcdma as expected"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 
	RMobileCall::TMobileHscsdCallParamsV1 multimediaParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg checkParamsPkg(multimediaParams);
	
	// valid parameters 
	multimediaParams.iIdRestrict = RMobileCall::ESendMyId;
	multimediaParams.iAckTimer = 20;
	multimediaParams.iMSToModemWindowSize = 2;
	multimediaParams.iModemToMSWindowSize = 2;	
	multimediaParams.iProtocol = RMobileCall::EProtocolV34;
	multimediaParams.iQoS = RMobileCall::EQoSNonTransparent;
	multimediaParams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	multimediaParams.iResequencingPeriod = 1;
	multimediaParams.iRetransmissionAttempts = 2;
	multimediaParams.iService = RMobileCall::EServiceDataCircuitAsync;
	multimediaParams.iSpeed = RMobileCall::ESpeed64000;
	multimediaParams.iUseEdge = EFalse;
	multimediaParams.iV42bisCodewordsNum = 0;
    multimediaParams.iV42bisMaxStringLength = 0;
    multimediaParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	multimediaParams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	multimediaParams.iCodings = RMobileCall::KCapsAiurCoding144;
	multimediaParams.iUserInitUpgrade = EFalse;
	multimediaParams.iMaxTimeSlots = 15;
	multimediaParams.iWantedRxTimeSlots = 15;
	multimediaParams.iWantedAiur = RMobileCall::EAiurBpsUnspecified;

    // post a notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyStatusChangeStatus(mobileLine,&RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus lineStatus;
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);
    
	// Dial a number that answers.
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);

    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

	// $CTSYProblem: dial returns -18 or -6 depending of selected parametes (protocol)
    // EProtocolPstnMultimedia = -6, EProtocolV34 = -18
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue
    /*
	call1.Dial(dialStatus, checkParamsPkg, number); 
	TEST_CHECK_POINT_L(_L("Couldn't dial a call, no point continuing."));
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));
	
	// Verify line status goes from ringing to connected 
	RCall::TStatus expectedLineStatus = RCall::EStatusRinging;
	const TInt expectedStatus = KErrNone;
	
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);			

	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);
	expectedLineStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);

	// Get call information and verify it is as expected. 
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(call1.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iCallName.Length() >0, _L("RMobileCall::GetMobileCallInfo returned an invalid callName"));
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() >0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));

	// Get call params and verify it is as expected.
	ASSERT_EQUALS(call1.GetCallParams(checkParamsPkg), KErrNone, _L("RCall::GetCallParams returned an error"));
	ASSERT_TRUE(multimediaParams.iSpeed = RMobileCall::ESpeed64000, _L("RCall::GetCallParams returned wrongly RMobileCall::ESpeedUnspecified"));
	ASSERT_TRUE(multimediaParams.iProtocol = RMobileCall::EProtocolPstnMultimedia, _L("TMobileDataCallParamsV1 returns aParams.iProtocol"));
	ASSERT_EQUALS(multimediaParams.iService,  RMobileCall::EServiceDataCircuitAsync, _L("RCall::GetCallParams service was not the same that used in dial"));
	*/
	// Hang up.
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	/*
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	*/
	
	//
	// TEST END
	//

    StartCleanup();
	
	//notifyStatusChangeStatus
	//dialStatus
	//hangUpStatus
	CleanupStack::PopAndDestroy(3, &notifyStatusChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0002");
	}



CCTSYIntegrationTestMultimediaCallControl0003::CCTSYIntegrationTestMultimediaCallControl0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0003::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0003::~CCTSYIntegrationTestMultimediaCallControl0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0003
 * @SYMFssID BA/CTSY/MMCC-0003
 * @SYMTestCaseDesc Ensure a video call can be hung up by remote party.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::GetMobileCallInfo, RLine::NotifyIncomingCall, RCall::HangUp, RCall::AnswerIncomingCall, RCall::GetCallParams
 * @SYMTestExpectedResults Pass - Video call is hung up.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Make an incoming call to the board. 
	
	// post notifier for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (mobileLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName name;
	mobileLine.NotifyIncomingCall(notifyIncomingCallStatus,name);	

    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

	// $CTSYProblem: The line is not notifying tester to answer the incoming call ( notifier never completes). 
	// CTSY does not receive anykind of signal from line about incoming call thus it cannot be answered.
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue

	/*
	// Remote party invokes an incoming video call. 
	DisplayUserInteractionPromptL(_L("You should now dial a multimedia call to this board from some other mobilephone"), ETimeLong);

	// Check RLine::NotifyIncomingCall completes with valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name.Length() > 0, _L("RLine::NotifyIncomingCall did not set the name"));

	// Get call 1.
	TCallId incomingCallId;
	RMobileCall& dataCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// post notifier for RCall::NotifyStatusChange for incoming call
	TCoreEtelRequestStatus<RCall> notifyCallStatusChangeStatus (dataCall, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyCallStatusChangeStatus);
	RCall::TStatus callStatus;
	dataCall.NotifyStatusChange(notifyCallStatusChangeStatus, callStatus);
	
	// Answer incoming call. 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(dataCall, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	
	dataCall.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));
	
	// Verify call connected. 
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);

	// Get call information and verify it is as expected. 
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(dataCall.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iCallName.Length() >0, _L("RMobileCall::GetMobileCallInfo returned an invalid callName"));
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() >0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));

	// Get call params and verify it is as expected. 
	RMobileCall::TMobileDataCallParamsV1 multimediaParams;
	RMobileCall::TMobileCallParamsV1Pckg checkParamsPkg(multimediaParams);
	ASSERT_EQUALS(dataCall.GetCallParams(checkParamsPkg), KErrNone, _L("RCall::GetCallParams returned an error"));
	ASSERT_TRUE(multimediaParams.iSpeed = RMobileCall::ESpeed64000, _L("RCall::GetCallParams returned wrongly RMobileCall::ESpeedUnspecified"));
	ASSERT_TRUE(multimediaParams.iProtocol = RMobileCall::EProtocolPstnMultimedia, _L("TMobileDataCallParamsV1 returns aParams.iProtocol"));
	ASSERT_EQUALS(multimediaParams.iService,  RMobileCall::EServiceDataCircuitAsync, _L("RCall::GetCallParams service was not the same that used in dial"));
	
	// post a notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyStatusChangeStatus (mobileLine,&RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus lineStatus;
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus,lineStatus);

	// post notify for RCall::NotifyStatusChange
	dataCall.NotifyStatusChange(notifyCallStatusChangeStatus, callStatus);

	// Hang up. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (dataCall, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	dataCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	

	// Verify call and line are idle. 
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);
														 
	RCall::TStatus expectedLineStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);
	*/
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyIncomingCallStatus
	// notifyCallStatusChangeStatus
	// answerIncomingStatus
	// notifyStatusChangeStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(1, &notifyIncomingCallStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0003");
	}



CCTSYIntegrationTestMultimediaCallControl0004::CCTSYIntegrationTestMultimediaCallControl0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0004::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0004::~CCTSYIntegrationTestMultimediaCallControl0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0004
 * @SYMFssID BA/CTSY/MMCC-0004
 * @SYMTestCaseDesc Test hanging up a video call before the line starts ringing.
 * @SYMTestPriority High
 * @SYMTestActions RCall::DialCancel, RCall::GetStatus, RLine::GetStatus
 * @SYMTestExpectedResults Pass - Video call dialling is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial. 

	// Cancel dial. 

	// Verify call and line are idle. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0004");
	}



CCTSYIntegrationTestMultimediaCallControl0005::CCTSYIntegrationTestMultimediaCallControl0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0005::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0005::~CCTSYIntegrationTestMultimediaCallControl0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0005
 * @SYMFssID BA/CTSY/MMCC-0005
 * @SYMTestCaseDesc Test hanging up a video call while the line is ringing.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RCall::HangUp, RCall::GetStatus, RLine::GetStatus
 * @SYMTestExpectedResults Pass - Video call is hung up.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a number that won't answer. 

	// Verify line goes to ringing. 

	// Hang up. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0005");
	}



CCTSYIntegrationTestMultimediaCallControl0006::CCTSYIntegrationTestMultimediaCallControl0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0006::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0006::~CCTSYIntegrationTestMultimediaCallControl0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0006
 * @SYMFssID BA/CTSY/MMCC-0006
 * @SYMTestCaseDesc Test handling of time out when remote party does not answer the video call.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange
 * @SYMTestExpectedResults Pass - Call does not connect.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a number that won't answer. 

	// Cancel dial. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0006");
	}



CCTSYIntegrationTestMultimediaCallControl0007::CCTSYIntegrationTestMultimediaCallControl0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0007::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0007::~CCTSYIntegrationTestMultimediaCallControl0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0007
 * @SYMFssID BA/CTSY/MMCC-0007
 * @SYMTestCaseDesc Test handling of video call when called party is busy.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Error returned when dialling busy number.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get data line 1. 
	RMobileLine& line = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get call 1.
	RMobileCall& call = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1); 

	// Ensure call waiting is deactivated. 
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(phone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	phone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallWaitingStatus timed-out"))
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, _L("RMobilePhone::SetCallWaitingStatus returned with an error"))		


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 
	RMobileCall::TMobileHscsdCallParamsV1 mobileHscsdCallParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg mobileHscsdCallParamsPckg(mobileHscsdCallParams);
	mobileHscsdCallParams.iIdRestrict = RMobileCall::ESendMyId;
	mobileHscsdCallParams.iAckTimer = 20;
	mobileHscsdCallParams.iMSToModemWindowSize = 2;
	mobileHscsdCallParams.iModemToMSWindowSize = 2;	
	mobileHscsdCallParams.iProtocol = RMobileCall::EProtocolPstnMultimedia;
	mobileHscsdCallParams.iQoS = RMobileCall::EQoSNonTransparent;
	mobileHscsdCallParams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	mobileHscsdCallParams.iResequencingPeriod = 1;
	mobileHscsdCallParams.iRetransmissionAttempts = 2;
	mobileHscsdCallParams.iService = RMobileCall::EServiceDataCircuitAsync;
	mobileHscsdCallParams.iSpeed = RMobileCall::ESpeed64000;
	mobileHscsdCallParams.iUseEdge = EFalse;
	mobileHscsdCallParams.iV42bisCodewordsNum = 0;
    mobileHscsdCallParams.iV42bisMaxStringLength = 0;
    mobileHscsdCallParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	mobileHscsdCallParams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	mobileHscsdCallParams.iCodings = RMobileCall::KCapsAiurCoding144;
	mobileHscsdCallParams.iUserInitUpgrade = EFalse;
	mobileHscsdCallParams.iMaxTimeSlots = 15;
	mobileHscsdCallParams.iWantedRxTimeSlots = 15;
	mobileHscsdCallParams.iWantedAiur = RMobileCall::EAiurBpsUnspecified;
	
	// Dial a number which is engaged. 
	DisplayUserInteractionPromptL(_L("Please be sure that DataNumber1 is currently engaged"), ETimeMedium);
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board is now dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);

    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

	// $CTSYProblem: dial returns -18 or -6 depending of selected parametes (protocol)
    // EProtocolPstnMultimedia = -6, EProtocolV34 = -18
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue
    /*
	call.Dial(dialStatus, mobileHscsdCallParamsPckg, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));

	// Verify dial fails. 
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::Dial did not return error as expected"));
	*/
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
    // mobilePhoneSetCallWaitingStatus
    // dialStatus
    CleanupStack::PopAndDestroy(2, &mobilePhoneSetCallWaitingStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0007");
	}



CCTSYIntegrationTestMultimediaCallControl0008::CCTSYIntegrationTestMultimediaCallControl0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0008::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0008::~CCTSYIntegrationTestMultimediaCallControl0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0008
 * @SYMFssID BA/CTSY/MMCC-0008
 * @SYMTestCaseDesc Test making a video call after temporary deactivation of CLIR.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus
 * @SYMTestExpectedResults Pass - Caller ID retrieved when CLIR deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Deactivate CLIR. 

	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a number from the board.  

	// Verify that caller ID is available for incoming call. 

	// Answer that call. 

	// Verify call is answered and line status notifications produced. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0008");
	}



CCTSYIntegrationTestMultimediaCallControl0009::CCTSYIntegrationTestMultimediaCallControl0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0009::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0009::~CCTSYIntegrationTestMultimediaCallControl0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0009
 * @SYMFssID BA/CTSY/MMCC-0009
 * @SYMTestCaseDesc Test making a video call with CLIR activated.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus
 * @SYMTestExpectedResults Pass - Caller ID not available when CLIR active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate CLIR. 

	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a number which answers.  

	// Verifies that caller ID is not available for incoming call. 

	// Answer the call. 

	// Verify call is answered and line status notifications produced. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0009");
	}



CCTSYIntegrationTestMultimediaCallControl0010::CCTSYIntegrationTestMultimediaCallControl0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0010::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0010::~CCTSYIntegrationTestMultimediaCallControl0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0010
 * @SYMFssID BA/CTSY/MMCC-0010
 * @SYMTestCaseDesc Test attempt to make a video call to a GSM mobile.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Dialling video call to GSM phone fails.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial GSM phone. 

	// Verify dial fails with error that called party is busy.  

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0010");
	}



CCTSYIntegrationTestMultimediaCallControl0011::CCTSYIntegrationTestMultimediaCallControl0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0011::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0011::~CCTSYIntegrationTestMultimediaCallControl0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0011
 * @SYMFssID BA/CTSY/MMCC-0011
 * @SYMTestCaseDesc Test attempt to make a video call to a PSTN number.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Dialling video call to PSTN number fails.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial PSTN number. 

	// Verify dial fails with an error indicating video calls not supported. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0011");
	}



CCTSYIntegrationTestMultimediaCallControl0012::CCTSYIntegrationTestMultimediaCallControl0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0012::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0012::~CCTSYIntegrationTestMultimediaCallControl0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0012
 * @SYMFssID BA/CTSY/MMCC-0012
 * @SYMTestCaseDesc Ensure that a video call can be made whilst roaming.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RCall::NotifyStatusChange, RCall::GetCallParams
 * @SYMTestExpectedResults Pass - Video call established when roaming.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Insert roaming SIM. 

	// Ensure phone is camped on roaming network. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a video call to mobile in the same country which answers. 

	// Verify call is answers and call is connected. 

	// Repeat test dialling a video call to mobile in the same country using + international format instead. 

	// Repeat test dialling a video call to mobile in a different country using + international format instead. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0012");
	}



CCTSYIntegrationTestMultimediaCallControl0013::CCTSYIntegrationTestMultimediaCallControl0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0013::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0013::~CCTSYIntegrationTestMultimediaCallControl0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0013
 * @SYMFssID BA/CTSY/MMCC-0013
 * @SYMTestCaseDesc Test handling of incoming video call when there is already an active multimedia call.


 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange
 * @SYMTestExpectedResults Pass - 2nd video call answered. 1st video call dropped.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 1. 
	RMobileCall& dataCall1= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	// Ensure call waiting is activated. 
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(phone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	phone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallWaitingStatus timed-out"));
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, _L("RMobilePhone::SetCallWaitingStatus returned with an error"));	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Set up multimedia mobile data call parameters for a multimedia call. 
	RMobileCall::TMobileHscsdCallParamsV1 multimediaParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg checkParamsPkg(multimediaParams);
	
	// valid parameters 
	multimediaParams.iIdRestrict = RMobileCall::ESendMyId;
	multimediaParams.iAckTimer = 20;
	multimediaParams.iMSToModemWindowSize = 2;
	multimediaParams.iModemToMSWindowSize = 2;	
	multimediaParams.iProtocol = RMobileCall::EProtocolV34;
	multimediaParams.iQoS = RMobileCall::EQoSNonTransparent;
	multimediaParams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	multimediaParams.iResequencingPeriod = 1;
	multimediaParams.iRetransmissionAttempts = 2;
	multimediaParams.iService = RMobileCall::EServiceDataCircuitAsync;
	multimediaParams.iSpeed = RMobileCall::ESpeed64000;
	multimediaParams.iUseEdge = EFalse;
	multimediaParams.iV42bisCodewordsNum = 0;
    multimediaParams.iV42bisMaxStringLength = 0;
    multimediaParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	multimediaParams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	multimediaParams.iCodings = RMobileCall::KCapsAiurCoding144;
	multimediaParams.iUserInitUpgrade = EFalse;
	multimediaParams.iMaxTimeSlots = 15;
	multimediaParams.iWantedRxTimeSlots = 15;
	multimediaParams.iWantedAiur = RMobileCall::EAiurBpsUnspecified;

	// post notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyCallStatusChangeStatus (dataCall1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyCallStatusChangeStatus);
	RCall::TStatus callStatus;
	dataCall1.NotifyStatusChange(notifyCallStatusChangeStatus, callStatus);

	// Dial a video call to remote party 1. 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(dataCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);

    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

    // $CTSYProblem: dial returns -18 or -6 depending of selected parametes (protocol)
    // EProtocolPstnMultimedia = -6, EProtocolV34 = -18
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue
	/*
	dataCall1.Dial(dialStatus, checkParamsPkg, number); 
	TEST_CHECK_POINT_L(_L("Couldn't dial a call, no point continuing."));
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));

	// Verify that remote party 1 can see incoming caller ID. 
	DisplayUserInteractionPromptL(_L("Verify that remote party 1 can see incoming caller ID"), ETimeLong);

	// Remote party 1 can answer call and call is connected. 
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	const TInt expectedStatus = KErrNone;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);

	// Get data line 2. 
	RMobileLine& mobileLine2 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// post notifier for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (mobileLine2,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName name;
	mobileLine2.NotifyIncomingCall(notifyIncomingCallStatus,name);	

	// $CTSYProblem: The line is not notifying tester to answer the incoming call ( notifier never completes). 
	// CTSY does not receive anykind of signal from line about incoming call thus it cannot be answered.
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue

	// Remote party 2 invokes an incoming video call. 
	DisplayUserInteractionPromptL(_L("You should now dial a multimedia call to this board from some other mobilephone"), ETimeLong);

	// Check RLine::NotifyIncomingCall completes with valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name.Length() > 0, _L("RLine::NotifyIncomingCall did not set the name"));

	// Get call 2
	TCallId incomingCallId;
	RMobileCall& dataCall2 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);

	// post notifier for RCall::NotifyStatusChange for dataCall2
	TCoreEtelRequestStatus<RCall> notifyCallStatusChangeStatus2 (dataCall2, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyCallStatusChangeStatus2);
	RCall::TStatus callStatus2;
	dataCall2.NotifyStatusChange(notifyCallStatusChangeStatus2, callStatus2);
	
	// Answer video call 2 and verify video call 2 is connected and call 1 is dropped. 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(dataCall2, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	dataCall2.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Check RCall::NotifyStatusChange completes with EStatusIdle (call 1)
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);
														 
	// Check RCall::NotifyStatusChange completes with EStatusConnected (call 2)
	RCall::TStatus expectedCallStatus2 = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall2,
														 notifyCallStatusChangeStatus2,
														 callStatus2,
														 expectedCallStatus2,
														 KErrNone);
	*/
	
	//
	// TEST END
	//
	
	// Deactivate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EAllServices  and aAction=EServiceActionDeactivate 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	phone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallWaitingStatus timed-out"));
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, _L("RMobilePhone::SetCallWaitingStatus returned with an error"));

	/*
	dataCall2.HangUp();
	*/
    StartCleanup();

	// Pop
	// mobilePhoneSetCallWaitingStatus
	// notifyCallStatusChangeStatus
	// dialStatus
	// notifyIncomingCallStatus
	// notifyCallStatusChangeStatus2
	// answerIncomingStatus
	CleanupStack::PopAndDestroy(3, &mobilePhoneSetCallWaitingStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0013");
	}



CCTSYIntegrationTestMultimediaCallControl0014::CCTSYIntegrationTestMultimediaCallControl0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0014::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0014::~CCTSYIntegrationTestMultimediaCallControl0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0014
 * @SYMFssID BA/CTSY/MMCC-0014
 * @SYMTestCaseDesc Test handling of incoming voice/video call when there is already an active video call.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RCall::AnswerIncomingCall, RCall::HangUp, RMobileCall::Hold, RMobileCall::Resume
 * @SYMTestExpectedResults Pass - Incoming video call can be answered when active voice call is held.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get voice line 1. 
	RMobileLine& voiceLine1= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get data line 2. 
	RMobileLine& dataLine2= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Ensure call waiting is activated. 
	TExtEtelRequestStatus mobilePhoneSetCallWaitingStatus(phone, EMobilePhoneSetCallWaitingStatus);
	CleanupStack::PushL(mobilePhoneSetCallWaitingStatus);
	RMobilePhone::TMobileService mobileService = RMobilePhone::EAllServices;
	RMobilePhone::TMobilePhoneServiceAction mobilePhoneServiceAction = RMobilePhone::EServiceActionActivate;
	phone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallWaitingStatus timed-out"));
	CHECK_EQUALS_L(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, _L("RMobilePhone::SetCallWaitingStatus returned with an error"));	


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post notifier for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (voiceLine1,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName name;
	voiceLine1.NotifyIncomingCall(notifyIncomingCallStatus,name);	

	// Dial the board (voice call, call 1) 
	DisplayUserInteractionPromptL(_L("You should now dial a voice call to this board from some other mobilephone"), ETimeLong);

	// Check RLine::NotifyIncomingCall completes with valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name.Length() > 0, _L("RLine::NotifyIncomingCall did not set the name"));

	// Get call 1
	TCallId incomingCallId;
	RMobileCall& voiceCall1= iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, name, incomingCallId);

	// post a notifier for RMobileLine::NotifyMobileLineStatusChange
	RMobileCall::TMobileCallStatus mobileLineStatus;
	TExtEtelRequestStatus notifyMobileLineChangeStatus(voiceLine1, EMobileLineNotifyMobileLineStatusChange);
	CleanupStack::PushL(notifyMobileLineChangeStatus);
	voiceLine1.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);

	// Answer voice call 1 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(voiceCall1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	voiceCall1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Verify line status goes to connected.
	RMobileCall::TMobileCallStatus expectedLineStatus = RMobileCall::EStatusConnected;
	voiceLine1.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine1,
														 notifyMobileLineChangeStatus,
														 mobileLineStatus,
														 expectedLineStatus,
														 KErrNone);

    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

	// $CTSYProblem: The line is not notifying tester to answer the incoming call ( notifier never completes). 
	// CTSY does not receive anykind of signal from line about incoming call thus it cannot be answered.
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue

	/*
	// post notifier for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus2 (dataLine2,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus2);
	TName name2;
	dataLine2.NotifyIncomingCall(notifyIncomingCallStatus2,name2);	

	// Dial the board (video call, call 2) 
	DisplayUserInteractionPromptL(_L("You should now dial a multimedia call to this board from some other mobilephone"), ETimeLong);

	// Verify incoming call notification completes. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus2,ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingCallStatus2.Int(), KErrNone, _L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name2.Length() > 0, _L("RLine::NotifyIncomingCall did not set the name"));
	*/
	
	// Hold call 1. 
	TExtEtelRequestStatus holdStatus(voiceCall1, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	voiceCall1.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed out"));
	ASSERT_EQUALS(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold returned with an error"));		

	/*
	// Get call 2
	TCallId incomingCallId2;
	RMobileCall& dataCall2 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name2, incomingCallId2);

	// post notifier for RMobileCall::NotifyMobileCallStatusChange for dataCall2
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus2(dataCall2, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus2);
	RMobileCall::TMobileCallStatus mobileCallStatus2;
	dataCall2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);
	
	// Answer incoming video call on call 2. 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus2(dataCall2, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus2);	
	dataCall2.AnswerIncomingCall(answerIncomingStatus2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus2, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus2.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Verify call 2 connected. 
	RMobileCall::TMobileCallStatus expectedCallStatus2 = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall2,
														 notifyMobileCallStatusChangeStatus2,
														 mobileCallStatus2,
														 expectedCallStatus2,
														 KErrNone);

	// post a notifier for RMobileLine::NotifyMobileLineStatusChange (call 2)
	RMobileCall::TMobileCallStatus mobileLineStatus2;
	TExtEtelRequestStatus notifyMobileLineChangeStatus2(dataLine2, EMobileLineNotifyMobileLineStatusChange);
	CleanupStack::PushL(notifyMobileLineChangeStatus2);
	dataLine2.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus2, mobileLineStatus2);

	// Hang up call 2. 
	TCoreEtelRequestStatus<RCall> hangUpStatus2 (dataCall2, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus2);
	dataCall2.HangUp(hangUpStatus2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus2, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus2.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	*/
	//remove this delay when Code is added in again.
	// delay is needed for the moment because call1 doesn't have time to change it's status to RMobileCall::EStatusHold
	// This is because Code.
	User::After(ETimeShort);
	// Verify call 1 still on hold. 
	RMobileCall::TMobileCallStatus getCallStatus;
	ASSERT_EQUALS(voiceCall1.GetMobileCallStatus(getCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned an Error"));
	ASSERT_EQUALS(getCallStatus, RMobileCall::EStatusHold, _L("RMobileCall::GetMobileCallStatus did not return EStatusHold"));

	// post notifier for RMobileCall::NotifyMobileCallStatusChange for voice call1
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(voiceCall1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	voiceCall1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Resume call 1. 
	TExtEtelRequestStatus resumeStatus(voiceCall1, EMobileCallResume);
	CleanupStack::PushL(resumeStatus);
	voiceCall1.Resume(resumeStatus);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeStatus, ETimeMedium), KErrNone, _L("RMobileCall::Resume timed-out"));
	ASSERT_EQUALS(resumeStatus.Int(), KErrNone, _L("RMobileCall::Resume resumeStatus completed with incorrect error"));

	// Verify call 1 connected. 
	RMobileCall::TMobileCallStatus expectedCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(voiceCall1,
														 notifyMobileCallStatusChangeStatus,
														 mobileCallStatus,
														 expectedCallStatus,
														 KErrNone);

	// post notifier for RMobileLine::NotifyMobileLineStatusChange (call 1)
	voiceLine1.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);

	// Hang up call 1. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (voiceCall1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	voiceCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Verify both lines are idle. 
	expectedLineStatus = RMobileCall::EStatusIdle;
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50001);
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine1,
														 notifyMobileLineChangeStatus,
														 mobileLineStatus,
														 expectedLineStatus,
														 KErrNone);
	/*
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(dataLine2,
														 notifyMobileLineChangeStatus2,
														 mobileLineStatus2,
														 expectedLineStatus,
														 KErrNone);
	*/
	
	//
	// TEST END
	//

	// Deactivate call waiting with RMobilePhone::SetCallWaitingStatus with aServiceGroup=EAllServices  and aAction=EServiceActionDeactivate 
	mobilePhoneServiceAction = RMobilePhone::EServiceActionDeactivate;
	phone.SetCallWaitingStatus(mobilePhoneSetCallWaitingStatus, mobileService, mobilePhoneServiceAction);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(mobilePhoneSetCallWaitingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallWaitingStatus timed-out"));
	ASSERT_EQUALS(mobilePhoneSetCallWaitingStatus.Int(), KErrNone, _L("RMobilePhone::SetCallWaitingStatus returned with an error"));

    StartCleanup();
	
	// Pop
	// mobilePhoneSetCallWaitingStatus
	// notifyIncomingCallStatus
	// notifyMobileLineChangeStatus
	// answerIncomingStatus
	// notifyIncomingCallStatus2
	// holdStatus
	// notifyMobileCallStatusChangeStatus2
	// answerIncomingStatus2
	// notifyMobileLineChangeStatus2
	// hangUpStatus2
	// notifyMobileCallStatusChangeStatus
	// resumeStatus
	// hangUpStatus

	CleanupStack::PopAndDestroy(8, &mobilePhoneSetCallWaitingStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0014");
	}



CCTSYIntegrationTestMultimediaCallControl0015::CCTSYIntegrationTestMultimediaCallControl0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0015::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0015::~CCTSYIntegrationTestMultimediaCallControl0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0015
 * @SYMFssID BA/CTSY/MMCC-0015
 * @SYMTestCaseDesc Test getting CLIP information for incoming and outgoing video calls.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyIdentityServiceStatus, RMobilePhone::GetIdentityServiceStatus, RCall::Dial, RCall::GetStatus
 * @SYMTestExpectedResults Pass - CLIP information presented when CLIP active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// 1. Outgoing calls 

	// Deactivate CLIR and verify it is deactivated. 

	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a video call to a number you can answer. 

	// Check that calling number can be retrieved from incoming call.

	// Answer the call. 

	// Verify call is connected. 

	// 2. Incoming calls 

	// Activate CLIP and verify it is active. 

	// Dial the board with a video call. 

	// Verify CLIP information can be retrieved from incoming call. 

	// Verify call can be answered and call is connected. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0015");
	}



CCTSYIntegrationTestMultimediaCallControl0016::CCTSYIntegrationTestMultimediaCallControl0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0016::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0016::~CCTSYIntegrationTestMultimediaCallControl0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0016
 * @SYMFssID BA/CTSY/MMCC-0016
 * @SYMTestCaseDesc Test incoming call with CLIP deactivated.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyIdentityServiceStatus, RMobilePhone::GetIdentityServiceStatus, RCall::Dial, RLine::NotifyIncomingCall, RCall::AnswerIncomingCall, RMobileCall::GetMobileCallInfo, RCall::GetStatus
 * @SYMTestExpectedResults Pass - CLIP information not presented.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Deactivate CLIP and verify CLIP is deactivated. 

	// Dial a board with a video call. 

	// Verify caller ID (CLIP information) not present for incoming call. 

	// Verify call can be answered and call is connected. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0016");
	}



CCTSYIntegrationTestMultimediaCallControl0017::CCTSYIntegrationTestMultimediaCallControl0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0017::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0017::~CCTSYIntegrationTestMultimediaCallControl0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0017
 * @SYMFssID BA/CTSY/MMCC-0017
 * @SYMTestCaseDesc Test activation of call barring with video calls.
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial
 * @SYMTestExpectedResults Pass - Barred number cannot be dialled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has the following capabilities: KCapsNotifyCBStatus, KCapsSetCBStatus, KCapsBarAllOutgoing, KCapsBarOutgoingInternational, KCapsBarOutgoingInternationalExHC, and KCapsBarAllCases 

	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// For each of the outgoing call barring conditions and 'call barring all cases' in TMobilePhoneCBCondition: 

	// Activate the call barring condition for video calls. 

	// Make an outgoing video call to a barred number. 

	// Verify dial fails due to number being barred. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0017");
	}



CCTSYIntegrationTestMultimediaCallControl0018::CCTSYIntegrationTestMultimediaCallControl0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0018::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0018::~CCTSYIntegrationTestMultimediaCallControl0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0018
 * @SYMFssID BA/CTSY/MMCC-0018
 * @SYMTestCaseDesc Test call forwarding of video calls if there is no reply.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, RMobilePhone::NotifyCallForwardingActive
 * @SYMTestExpectedResults Pass - Incoming call is forwarded.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has the following capabilities: KCapsSetCFStatus, KCapsNotifyCFStatus. 

	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate call forwarding if no reply (ECallForwardingNoReply) for video calls and verify call forwarding status change notification completes. 

	// Dial the board with a video call.  

	// Board does not answer call. 

	// Verify call forwarding active notification completes and call is forwarded to number specified when activating call forwarding. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0018");
	}



CCTSYIntegrationTestMultimediaCallControl0019::CCTSYIntegrationTestMultimediaCallControl0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0019::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0019::~CCTSYIntegrationTestMultimediaCallControl0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0019
 * @SYMFssID BA/CTSY/MMCC-0019
 * @SYMTestCaseDesc Test unconditional call forwarding with video calls.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, RMobilePhone::NotifyCallForwardingActive
 * @SYMTestExpectedResults Pass - Incoming call is forwarded.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has the following capabilities: KCapsSetCFStatus, KCapsNotifyCFStatus. 

	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate unconditional call forwarding for video calls and verify call forwarding status change notification completes.  

	// Dial the board. 

	// Verify call forwarding active notification completes and call is forwarded to number specified when activating call forwarding. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0019");
	}



CCTSYIntegrationTestMultimediaCallControl0020::CCTSYIntegrationTestMultimediaCallControl0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0020::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0020::~CCTSYIntegrationTestMultimediaCallControl0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0020
 * @SYMFssID BA/CTSY/MMCC-0020
 * @SYMTestCaseDesc Test call forwarding when busy with video calls.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, RMobilePhone::NotifyCallForwardingActive, RCall::Dial
 * @SYMTestExpectedResults Pass - Call forwarded when phone is busy.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has the following capabilities: KCapsSetCFStatus, KCapsNotifyCFStatus. 

	// Ensure phone is camped on a valid cell. 

	// Get voice line 1. 

	// Get call 1. 

	// Get data line 2. 

	// Get call 2. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate call forwarding if busy for video calls and verify call forwarding status change notification completes. 

	// Dial a number that answers on voice line 1. 

	// Verify call is active. 

	// Request notification of an incoming call on data line 2. 

	// Dial the board with a data call on data line 2. 

	// Verify call forwarding active notification completes and call is forwarded to number specified when activating call forwarding. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0020");
	}



CCTSYIntegrationTestMultimediaCallControl0021::CCTSYIntegrationTestMultimediaCallControl0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0021::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0021::~CCTSYIntegrationTestMultimediaCallControl0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0021
 * @SYMFssID BA/CTSY/MMCC-0021
 * @SYMTestCaseDesc Test call forwarding when not reachable with video calls.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has the following capabilities: KCapsSetCFStatus, KCapsNotifyCFStatus. 

	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Activate call forwarding when not reachable for video calls and verify call forwarding status change notification completes. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0021");
	}



CCTSYIntegrationTestMultimediaCallControl0022::CCTSYIntegrationTestMultimediaCallControl0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0022::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0022::~CCTSYIntegrationTestMultimediaCallControl0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0022
 * @SYMFssID BA/CTSY/MMCC-0022
 * @SYMTestCaseDesc Test receiving an SMS whilst engaged in a video call.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RCall::GetStatus, RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS reception does not affect active video call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a video call. Answer that call. 

	// Verify call is connected. 

	// Gets call information and verifies it is correct. 

	// Send an SMS to the baord. 

	// Verify SMS received. 

	// Verify video call is still connected. 

	// Hang up video call. 

	// Verify SMS message content is correct. 

	// Repeat with a message longer than with the maximum number of concatenated messages supported. 

	// Repeat with a message which is longer than the maximum number of concatenations supported. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0022");
	}



CCTSYIntegrationTestMultimediaCallControl0023::CCTSYIntegrationTestMultimediaCallControl0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0023::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0023::~CCTSYIntegrationTestMultimediaCallControl0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0023
 * @SYMFssID BA/CTSY/MMCC-0023
 * @SYMTestCaseDesc Test receiving an MMS whilst engaged in a video call.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RCall::GetStatus, RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - MMS reception does not affect active video call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set up multimedia mobile data call parameters for a multimedia call. 

	// Dial a video call. Answer it. 

	// Verify call is connected. 

	// Get the call information and verify it is correct. 

	// Send an MMS to the phone. 

	// Verify MMS received. 

	// Verify video call is still connected. 

	// Hang up video call. 

	// Verify MMS message content is correct. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0023");
	}



CCTSYIntegrationTestMultimediaCallControl0024::CCTSYIntegrationTestMultimediaCallControl0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0024::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0024::~CCTSYIntegrationTestMultimediaCallControl0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0024
 * @SYMFssID BA/CTSY/MMCC-0024
 * @SYMTestCaseDesc iTest.Printf(_L("1\tReceive a MT multimedia call and transfer data\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - See TRP test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0024");
	}



CCTSYIntegrationTestMultimediaCallControl0025::CCTSYIntegrationTestMultimediaCallControl0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0025::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0025::~CCTSYIntegrationTestMultimediaCallControl0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0025
 * @SYMFssID BA/CTSY/MMCC-0025
 * @SYMTestCaseDesc iTest.Printf(_L("2\tReject the incoming multimedia call\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - See TRP test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	 // Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
     // Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial the board (video call, call 1) 
	// post notifier for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (mobileLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName name;
	mobileLine.NotifyIncomingCall(notifyIncomingCallStatus,name);	

    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

	// $CTSYProblem: The line is not notifying tester to answer the incoming call ( notifier never completes). 
	// CTSY does not receive anykind of signal from line about incoming call thus it cannot be answered.
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue

	/*
	// Remote party invokes an incoming video call. 
	DisplayUserInteractionPromptL(_L("You should now dial a multimedia call to this board from some other mobilephone"), ETimeLong);

    // Verify incoming call notification completes. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name.Length() > 0, _L("RLine::NotifyIncomingCall did not set the name"));

	TCallId incomingCallId;
	RMobileCall& incomingCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, name, incomingCallId);
	
	// post a notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallChangeStatus(incomingCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	incomingCall.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);
	
    // Reject incoming video call on call 1. 
	// HangupCall
	TCoreEtelRequestStatus<RCall> hangUpStatus (incomingCall, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	incomingCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
     

    // Verify call1 goes -> disconnecting -> idle.
    // RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected. 
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	TInt expectedStatus = KErrNone;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
														 					notifyMobileCallChangeStatus,
														 					mobileCallStatus,
														 					expectedMobileCallStatus,
														 					expectedStatus);
														 
	incomingCall.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);												
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(incomingCall,
														 					notifyMobileCallChangeStatus,
														 					mobileCallStatus,
														 					expectedMobileCallStatus,
														 					expectedStatus);
	*/

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyIncomingCallStatus
	// notifyMobileCallChangeStatus
	CleanupStack::PopAndDestroy(1, &notifyIncomingCallStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0025");
	}



CCTSYIntegrationTestMultimediaCallControl0026::CCTSYIntegrationTestMultimediaCallControl0026(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0026::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0026::~CCTSYIntegrationTestMultimediaCallControl0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0026
 * @SYMFssID BA/CTSY/MMCC-0026
 * @SYMTestCaseDesc iTest.Printf(_L("3\tAnswer the incoming multimedia call as voice\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - See TRP test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

    //
    // SET UP
    //

     
    // Ensure phone is camped on a valid cell.
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get data line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);	

    //
    // SET UP END
    //
     
    StartTest();
     
    //
    // TEST START
    //
    
    // Dial the board (video call, call 1)
    
    // post notifier for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (mobileLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName name;
	mobileLine.NotifyIncomingCall(notifyIncomingCallStatus,name);
    
    // Failing CSD calls (see $CTSYProblem described below).
    // Code can be added in after multimedia calls are working.

	// $CTSYProblem: The line is not notifying tester to answer the incoming call ( notifier never completes). 
	// CTSY does not receive anykind of signal from line about incoming call thus it cannot be answered.
    // All CSD call are currently failing, this is not a CTSY problem because CTSY has no control over this issue

	/*
	// Remote party invokes an incoming video call. 
	DisplayUserInteractionPromptL(_L("You should now dial a multimedia call to this board from some other mobilephone"), ETimeLong);
	
    // Verify incoming call notification completes. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name.Length() > 0, _L("RLine::NotifyIncomingCall did not set the name"));

	// Get call 1.
	TCallId incomingCallId;
	RMobileCall& call1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
    
	// post notifier for RCall::NotifyStatusChange for incoming call
	TCoreEtelRequestStatus<RCall> notifyCallStatusChangeStatus (call1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyCallStatusChangeStatus);
	RCall::TStatus callStatus;
	call1.NotifyStatusChange(notifyCallStatusChangeStatus, callStatus);
    
    // Answer incoming call as a voice call.
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(call1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);
	RMobileCall::TMobileDataCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
	TName voiceName;
	call1.AnswerMultimediaCallAsVoice(answerIncomingStatus, callParamsPckg, voiceName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));
	ASSERT_EQUALS(call1.OpenExistingCall(mobileLine, voiceName), KErrNone, _L("RMobileCall::OpenExistingCall returned with an error"));

    // Check that call is connected and that status goes to EStatusConnected
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);
    
    // Check RMobilePhone::GetMobileCallInfo completes with TMobileService::EVoiceService
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(call1.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_EQUALS(mobileCallInfo.iService, RMobilePhone::EVoiceService, _L("RMobileCall::GetMobileCallInfo did not return RMobilePhone::EVoiceService as expected"));
    
	// HangupCall
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
    */
    
	//
	// TEST END
	//

    StartCleanup();
	
	//notifyIncomingCallStatus
	//notifyCallStatusChangeStatus
	//answerIncomingStatus
	//hangUpStatus
	CleanupStack::PopAndDestroy(1, &notifyIncomingCallStatus);
    
	return TestStepResult();
	
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0026");
	}



CCTSYIntegrationTestMultimediaCallControl0027::CCTSYIntegrationTestMultimediaCallControl0027(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0027::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0027::~CCTSYIntegrationTestMultimediaCallControl0027()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0027::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0027
 * @SYMFssID BA/CTSY/MMCC-0027
 * @SYMTestCaseDesc iTest.Printf(_L("4\tRetrieve the params negotiated for the MT call\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - See TRP test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0027::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0027");
	}



CCTSYIntegrationTestMultimediaCallControl0028::CCTSYIntegrationTestMultimediaCallControl0028(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultimediaCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultimediaCallControl0028::GetTestStepName());
	}

CCTSYIntegrationTestMultimediaCallControl0028::~CCTSYIntegrationTestMultimediaCallControl0028()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultimediaCallControl0028::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MMCC-0028
 * @SYMFssID BA/CTSY/MMCC-0028
 * @SYMTestCaseDesc iTest.Printf(_L("1\tNotify MO multimedia call voice fallback\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - See TRP test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultimediaCallControl0028::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultimediaCallControl0028");
	}



