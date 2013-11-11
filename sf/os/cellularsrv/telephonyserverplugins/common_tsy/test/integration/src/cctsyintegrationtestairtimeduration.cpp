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
// Test step definitions for the AirTimeDuration functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestairtimeduration.h"
#include "cctsyinidata.h"



	
CCTSYIntegrationTestAirTimeDurationBase::CCTSYIntegrationTestAirTimeDurationBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestAirTimeDurationBase::~CCTSYIntegrationTestAirTimeDurationBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestAirTimeDuration0001::CCTSYIntegrationTestAirTimeDuration0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAirTimeDurationBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAirTimeDuration0001::GetTestStepName());
	}

CCTSYIntegrationTestAirTimeDuration0001::~CCTSYIntegrationTestAirTimeDuration0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAirTimeDuration0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ATD-0001
 * @SYMFssID BA/CTSY/ATD-0001
 * @SYMTestCaseDesc Make a call and get the air time duration.
 * @SYMTestPriority High
 * @SYMTestActions RMmCustomAPI::GetAirTimeDuration, RCall::Dial, RCall::HangUp
 * @SYMTestExpectedResults Pass - Air time duration received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify air time duration advances as call is connected.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get voice line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);
	
	// Get CustomApi 
	// Note: RMobilePhone::GetAirTimeDuration is not supported by the CTSY
	RMmCustomAPI& customApi= iEtelSessionMgr.GetCustomApiL(KMainServer,KMainPhone,KMainCustomApi);
	
	// Registered with the network
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, 
			_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Dial a number that answers. 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF1(_L("Board Now Dialling: "));
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"))
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"))

	// Note that the air time reported before dialing a call is the last call airtime
	TTimeIntervalSeconds timeOne;
	TTimeIntervalSeconds timeTwo;
	ASSERT_EQUALS(customApi.GetAirTimeDuration(timeOne), KErrNone, _L("RMmCustomAPI::GetAirTimeDuration returned with an error"))

	// Wait for two second and check RMobilePhone::GetAirTimeDuration returns duration > previous duration. Repeat for 20 seconds. 
	TInt count=1;
	do
		{
		User::After(KOneSecond*2);
		ASSERT_EQUALS(customApi.GetAirTimeDuration(timeTwo), KErrNone, _L("RMmCustomAPI::GetAirTimeDuration returned with an error"))
		ASSERT_TRUE(timeTwo.Int()>timeOne.Int(),_L("RMmCustomAPI::GetAirTimeDuration did not return bigger time duration then the previous one"))
		timeOne=timeTwo;
		count++;
		}while(count!=10);
	
	// Hang up the active call. 
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone,_L("RMobileCall::HangUp returned with an error"))

	//
	// TEST END
	//

    StartCleanup();
    // PopAndDestroy
    // dialStatus
   	CleanupStack::PopAndDestroy(&dialStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAirTimeDuration0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAirTimeDuration0001");
	}



CCTSYIntegrationTestAirTimeDuration0002::CCTSYIntegrationTestAirTimeDuration0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAirTimeDurationBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAirTimeDuration0002::GetTestStepName());
	}

CCTSYIntegrationTestAirTimeDuration0002::~CCTSYIntegrationTestAirTimeDuration0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAirTimeDuration0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ATD-0002
 * @SYMFssID BA/CTSY/ATD-0002
 * @SYMTestCaseDesc Answer a call and get the air time duration.
 * @SYMTestPriority High
 * @SYMTestActions RMmCustomAPI::GetAirTimeDuration, RLine::NotifyIncomingCall, RCall::AnswerIncomingCall, RCall::HangUp
 * @SYMTestExpectedResults Pass - Air time duration received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/rps
 *
 * Reason for test: Verify air time duration advances as call is connected.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get voice line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get CustomApi 
	// Note: RMobilePhone::GetAirTimeDuration is not supported by the CTSY
	RMmCustomAPI& customApi= iEtelSessionMgr.GetCustomApiL(KMainServer,KMainPhone,KMainCustomApi);

	// Get call 1. 
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMmCustomAPI::GetAirTimeDuration returns duration == 0.
	TTimeIntervalSeconds timeBeforeDial;
	TTimeIntervalSeconds timeAfterDial;
	ASSERT_EQUALS(customApi.GetAirTimeDuration(timeBeforeDial), KErrNone, _L("RMmCustomAPI::GetAirTimeDuration returned with an error"))
	ASSERT_EQUALS(timeBeforeDial.Int(),0,_L("RMmCustomAPI::GetAirTimeDuration did not return zero time duration as expected"))
	
	// Post notification for incoming call from RPS
	TName name;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (mobileLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	mobileLine.NotifyIncomingCall(notifyIncomingCallStatus,name);
		
	// Request incoming call from RPS. 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EVoiceLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));
	
	// Check RLine::NotifyIncomingCall completes with call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"))
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"))
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"))
	
	// Get Incoming Call. 
	TCallId incomingCallId;
	RMobileCall& incomingCall =iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, name, incomingCallId);

	// Answer the call. 
	ASSERT_EQUALS(incomingCall.AnswerIncomingCall(), KErrNone,
			_L("RCall::AnswerIncomingCall returned an error"));

	// Wait for two second and check RMmCustomAPI::GetAirTimeDuration returns duration > previous duration. Repeat for 10 seconds. 
	TInt count=1;
	do
		{
		User::After(KOneSecond*2);
		ASSERT_EQUALS(customApi.GetAirTimeDuration(timeAfterDial), KErrNone, _L("RMmCustomAPI::GetAirTimeDuration returned with an error"))
		ASSERT_TRUE(timeAfterDial.Int()>timeBeforeDial.Int(),_L("RMmCustomAPI::GetAirTimeDuration did not return bigger time duration then the previous one"))
		timeBeforeDial=timeAfterDial;
		count++;
		}while(count!=10);
	
	// Hang up. 
	ASSERT_TRUE(incomingCall.HangUp()==KErrNone,_L("RMobileCall::HangUp returned with an error"))
	
	//
	// TEST END
	//

    StartCleanup();
    
	// Pop
	// notifyIncomingCallStatus
			
	CleanupStack::PopAndDestroy(&notifyIncomingCallStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAirTimeDuration0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAirTimeDuration0002");
	}



CCTSYIntegrationTestAirTimeDuration0003::CCTSYIntegrationTestAirTimeDuration0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAirTimeDurationBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAirTimeDuration0003::GetTestStepName());
	}

CCTSYIntegrationTestAirTimeDuration0003::~CCTSYIntegrationTestAirTimeDuration0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAirTimeDuration0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ATD-0003
 * @SYMFssID BA/CTSY/ATD-0003
 * @SYMTestCaseDesc Make a video call and get the air time duration.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetAirTimeDuration, RCall::Dial, RCall::HangUp
 * @SYMTestExpectedResults Pass - Air time duration received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
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

	// Dial video call to a number that answers. 

	// Wait for one second and check RMobilePhone::GetAirTimeDuration returns duration > previous duration. Repeat for 10 seconds. 

	// Hang up. 

	// Check RMobilePhone::NotifyAirTimeDurationChange completes with duration > 0.

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAirTimeDuration0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAirTimeDuration0003");
	}



CCTSYIntegrationTestAirTimeDuration0004::CCTSYIntegrationTestAirTimeDuration0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAirTimeDurationBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAirTimeDuration0004::GetTestStepName());
	}

CCTSYIntegrationTestAirTimeDuration0004::~CCTSYIntegrationTestAirTimeDuration0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAirTimeDuration0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ATD-0004
 * @SYMFssID BA/CTSY/ATD-0004
 * @SYMTestCaseDesc Receive a video call and get the air time duration.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetAirTimeDuration, RLine::NotifyIncomingCall, RCall::AnswerIncomingCall, RCall::HangUp
 * @SYMTestExpectedResults Pass - Air time duration received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get data line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get air time duration and verify result is as expected. 

	// Receive video call from RPS. 

	// Answer call. 

	// Wait for one second and check RMobilePhone::GetAirTimeDuration returns duration > previous duration. Repeat for 10 seconds. 

	// Hang up. 

	// Check RMobilePhone::NotifyAirTimeDurationChange completes with duration > 0.

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAirTimeDuration0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAirTimeDuration0004");
	}



