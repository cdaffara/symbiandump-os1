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
// Test step definitions for the Privacy functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestprivacy.h"
#include "cctsyinidata.h"


	
CCTSYIntegrationTestPrivacyBase::CCTSYIntegrationTestPrivacyBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this), iCallControlTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPrivacyBase::~CCTSYIntegrationTestPrivacyBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPrivacy0001::CCTSYIntegrationTestPrivacy0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPrivacyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPrivacy0001::GetTestStepName());
	}

CCTSYIntegrationTestPrivacy0001::~CCTSYIntegrationTestPrivacy0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPrivacy0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PRI-0001
 * @SYMFssID BA/CTSY/PRI-0001
 * @SYMTestCaseDesc Dial a voice call and get privacy confirmation.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::NotifyPrivacyConfirmation, RCall::HangUp
 * @SYMTestExpectedResults Pass - Correct privacy notification received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify privacy confirmation notification completes
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10090);
	User::Leave(KErrNone);


	// Check if we are on a simulated network.
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Make a voice call where the network confirms that privacy is on. ===
	
	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call1
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyCallStatusChange(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatusChange);
	call1.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	
	// Dial a number that answers. 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Post notifier for RMobileCall::NotifyPrivacyConfirmation
	RMobilePhone::TMobilePhonePrivacy phonePrivacy;
	TExtEtelRequestStatus notifyPrivacy(call1, EMobileCallNotifyPrivacyConfirmation);
	CleanupStack::PushL(notifyPrivacy);
	call1.NotifyPrivacyConfirmation(notifyPrivacy, phonePrivacy);

	// Simulated network should send confirmation that privacy is on.
	DisplayUserInteractionPromptL(_L("Simulated network privacy should now be set to on"), ETimeMedium);
 
	// Check RMobileCall::NotifyPrivacyConfirmation completes with EPrivacyOn
	RMobilePhone::TMobilePhonePrivacy expectedphonePrivacy = RMobilePhone::EPrivacyOn;
	iCallControlTestHelper.WaitForMobileCallNotifyPrivacyConfirmation(call1,
																		notifyPrivacy,
																		phonePrivacy,
																		expectedphonePrivacy,
																		KErrNone);
	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	
	// Hang up call. 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (call1, &RMobileCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::HangUp returned with error"));	
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);	
																		
	// ===  Make a voice call where the network confirms that privacy is off. ===

	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	
	// Dial a number that answers. 
	call1.Dial(dialStatus, number);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	// Repost notifier
	call1.NotifyPrivacyConfirmation(notifyPrivacy, phonePrivacy);																	
	
	// Simulated network should send confirmation that privacy is off.
	DisplayUserInteractionPromptL(_L("Simulated network privacy should now be set to off"), ETimeMedium);
 
	// Check RMobileCall::NotifyPrivacyConfirmation completes with EPrivacyOff
	expectedphonePrivacy = RMobilePhone::EPrivacyOff;
	iCallControlTestHelper.WaitForMobileCallNotifyPrivacyConfirmation(call1,
																		notifyPrivacy,
																		phonePrivacy,
																		expectedphonePrivacy,
																		KErrNone);
	
	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyCallStatusChange, mobileCallStatus);
	
	// Hang up call. 
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::HangUp returned with error"));	
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyCallStatusChange,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyCallStatusChange
	// dialStatus
	// notifyPrivacy
	// hangUpStatus
	CleanupStack::PopAndDestroy(4, &notifyCallStatusChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPrivacy0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPrivacy0001");
	}



CCTSYIntegrationTestPrivacy0002::CCTSYIntegrationTestPrivacy0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPrivacyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPrivacy0002::GetTestStepName());
	}

CCTSYIntegrationTestPrivacy0002::~CCTSYIntegrationTestPrivacy0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPrivacy0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PRI-0002
 * @SYMFssID BA/CTSY/PRI-0002
 * @SYMTestCaseDesc Dial a data call and get privacy confirmation.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::NotifyPrivacyConfirmation, RCall::HangUp
 * @SYMTestExpectedResults Pass - Correct privacy notification received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify privacy confirmation notification completes
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10091);
	User::Leave(KErrNone);	

    // Check if we are on a simulated network.
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get data line 1. 
	RMobileLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 1.
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KDataLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Make a data call where the network confirms that privacy is on. ===
	
	// Set data call parameters
	RMobileCall::TMobileDataCallParamsV1 mobileDataCallParams;
	mobileDataCallParams.iService = RMobileCall::EServiceDataCircuitAsync;
	mobileDataCallParams.iSpeed = RMobileCall::ESpeed9600;
	mobileDataCallParams.iProtocol = RMobileCall::EProtocolV32;
	mobileDataCallParams.iQoS = RMobileCall::EQoSNonTransparent;
	RMobileCall::TMobileDataCallParamsV1Pckg mobileDataCallParamsPkg(mobileDataCallParams);

	// Post notifier for RMobileCall::NotifyMobileCallStatusChange for call1
	RMobileCall::TMobileCallStatus mobileCallStatusDataCall;
	TExtEtelRequestStatus notifyDataCallStatusChange(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyDataCallStatusChange);
	call1.NotifyMobileCallStatusChange(notifyDataCallStatusChange, mobileCallStatusDataCall);
	
	// Dial a data call to a number that answers. 
	TPtrC dataNumber;
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatusDataCall(call1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatusDataCall);
	call1.Dial(dialStatusDataCall, mobileDataCallParamsPkg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusDataCall, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatusDataCall.Int(), KErrNone,  _L("RMobileCall::Dial returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected
	RMobileCall::TMobileCallStatus expectedDataCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyDataCallStatusChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);
	
	// Post notifier for RMobileCall::NotifyPrivacyConfirmation
	RMobilePhone::TMobilePhonePrivacy phonePrivacy;
	TExtEtelRequestStatus notifyPrivacy(call1, EMobileCallNotifyPrivacyConfirmation);
	CleanupStack::PushL(notifyPrivacy);
	call1.NotifyPrivacyConfirmation(notifyPrivacy, phonePrivacy);

	// Simulated network should send confirmation that privacy is on.
	DisplayUserInteractionPromptL(_L("Simulated network privacy should now be set to on"), ETimeMedium);
 
	// Check RMobileCall::NotifyPrivacyConfirmation completes with EPrivacyOn
	RMobilePhone::TMobilePhonePrivacy expectedphonePrivacy = RMobilePhone::EPrivacyOn;
	iCallControlTestHelper.WaitForMobileCallNotifyPrivacyConfirmation(call1,
																		notifyPrivacy,
																		phonePrivacy,
																		expectedphonePrivacy,
																		KErrNone);

	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyDataCallStatusChange, mobileCallStatusDataCall);
	
	// Hang up call. 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (call1, &RMobileCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::HangUp returned with error"));	
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle
	expectedDataCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyDataCallStatusChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);

	// ===  Make a data call where the network confirms that privacy is off. ===
	
	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyDataCallStatusChange, mobileCallStatusDataCall);
	
	// Dial a data call to a number that answers. 
	call1.Dial(dialStatusDataCall, mobileDataCallParamsPkg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatusDataCall, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatusDataCall.Int(), KErrNone,  _L("RMobileCall::Dial returned with an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected
	expectedDataCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyDataCallStatusChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);
	
	// Repost notifier
	call1.NotifyPrivacyConfirmation(notifyPrivacy, phonePrivacy);

	// Simulated network should send confirmation that privacy is off.
	DisplayUserInteractionPromptL(_L("Simulated network privacy should now be set to off"), ETimeMedium);
 
	// Check RMobileCall::NotifyPrivacyConfirmation completes with EPrivacyOff
	expectedphonePrivacy = RMobilePhone::EPrivacyOff;
	iCallControlTestHelper.WaitForMobileCallNotifyPrivacyConfirmation(call1,
																		notifyPrivacy,
																		phonePrivacy,
																		expectedphonePrivacy,
																		KErrNone);

	// Repost notifier
	call1.NotifyMobileCallStatusChange(notifyDataCallStatusChange, mobileCallStatusDataCall);
	
	// Hang up call. 
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::HangUp returned with error"));	
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle
	expectedDataCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyDataCallStatusChange,
																		mobileCallStatusDataCall,
																		expectedDataCallStatus,
																		KErrNone);

	
	//
	// TEST END
	//

    StartCleanup();
    
    // notifyDataCallStatusChange
    // dialStatusDataCall
    // notifyPrivacy
    // hangUpStatus
	CleanupStack::PopAndDestroy(4, &notifyDataCallStatusChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPrivacy0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPrivacy0002");
	}



CCTSYIntegrationTestPrivacy0003::CCTSYIntegrationTestPrivacy0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPrivacyBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPrivacy0003::GetTestStepName());
	}

CCTSYIntegrationTestPrivacy0003::~CCTSYIntegrationTestPrivacy0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPrivacy0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PRI-0003
 * @SYMFssID BA/CTSY/PRI-0003
 * @SYMTestCaseDesc Dial a fax call and get privacy confirmation.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileCall::NotifyPrivacyConfirmation, RCall::HangUp
 * @SYMTestExpectedResults Pass - Correct privacy notification received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify privacy confirmation notification completes
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get fax line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Make a fax call where the network confirms that privacy is on. ===

	// Dial a number that answers supplying any valid RMobileCall::TMobileCallParamsV1 as call params. 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected

	// Simulated network should send confirmation that privacy is on. 

	// Check RMobileCall::NotifyPrivacyConfirmation completes with EPrivacyOn

	// Hang up call. 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle

	// ===  Make a fax call where the network confirms that privacy is off. ===

	// Dial a number that answers supplying any valid RMobileCall::TMobileCallParamsV1 as call params. 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected

	// Simulated network should send confirmation that privacy is on. 

	// Check RMobileCall::NotifyPrivacyConfirmation completes with EPrivacyOff

	// Hang up call. 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPrivacy0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPrivacy0003");
	}



