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
// Test step definitions for the DTMF functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestdtmf.h"

#include "cctsyinidata.h"



	
CCTSYIntegrationTestDTMFBase::CCTSYIntegrationTestDTMFBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iCallControlTestHelper(*this), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestDTMFBase::~CCTSYIntegrationTestDTMFBase()
/*
 * Destructor
 */
	{
	}
 
CCTSYIntegrationTestDTMF0001::CCTSYIntegrationTestDTMF0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0001::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0001::~CCTSYIntegrationTestDTMF0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0001
 * @SYMFssID BA/CTSY/DTMF-0001
 * @SYMTestCaseDesc Get DTMF caps.
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial, RCall::HangUp, RMobilePhone::NotifyDTMFCapsChange, RMobilePhone::SetAlternatingCallMode
 * @SYMTestExpectedResults Pass - DTMF caps set when call is active. DTMF caps are 0 when call is not active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify dynamic  DTMF caps set correctly.
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
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification RMobilePhone::NotifyDTMFCapsChange
	TUint32 caps = 0;
	TExtEtelRequestStatus notifyDTMFCapsChangeStatus(mobilePhone, EMobilePhoneNotifyDTMFCapsChange);	
	CleanupStack::PushL(notifyDTMFCapsChangeStatus);
	mobilePhone.NotifyDTMFCapsChange(notifyDTMFCapsChangeStatus,caps);
	
	// Dial a Phone Number
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF1(_L("Board Now Dialling: "));
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));
	
	// Check RMobilePhone::NotifyDTMFCapsChange completes with caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 wantedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	iCallControlTestHelper.WaitForMobilePhoneNotifyDTMFCapsChange(mobilePhone,
																  notifyDTMFCapsChangeStatus,
																  caps, 
																  wantedCaps,
																  KNoUnwantedBits,
																  KErrNone);
	

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	ASSERT_EQUALS(mobilePhone.GetDTMFCaps(caps), KErrNone,_L("RMobilePhone::GetDTMFCaps returned with an error"))
	ASSERT_BITS_SET(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"))
	
	// Post notifer
	mobilePhone.NotifyDTMFCapsChange(notifyDTMFCapsChangeStatus,caps);	
	
	// Hang up call.
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone,_L("RMobileCall::HangUp returned with an error"))
	
	// Check RMobilePhone::NotifyDTMFCapsChange completes with caps of 0
	wantedCaps=0;
	iCallControlTestHelper.WaitForMobilePhoneNotifyDTMFCapsChange(mobilePhone,
																  notifyDTMFCapsChangeStatus,
																  caps, 
																  wantedCaps,
																  KNoUnwantedBits,
																  KErrNone);

	// Check RMobilePhone::GetDTMFCaps returns caps of 0
	wantedCaps = 0;
	ASSERT_EQUALS(mobilePhone.GetDTMFCaps(caps), KErrNone,_L("RMobilePhone::GetDTMFCaps returned with an error"))
	ASSERT_BITS_SET(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"))

	//
	// TEST END
	//
	
     StartCleanup();
    
    // Pop
    // notifyDTMFCapsChangeStatus
    // dialStatus
    CleanupStack::PopAndDestroy(2,&notifyDTMFCapsChangeStatus);
    	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0001");
	}



CCTSYIntegrationTestDTMF0002::CCTSYIntegrationTestDTMF0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0002::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0002::~CCTSYIntegrationTestDTMF0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0002
 * @SYMFssID BA/CTSY/DTMF-0002
 * @SYMTestCaseDesc Send valid DTMF digits and strings.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::StopDTMFTone, RMobilePhone::SendDTMFTones, RMobilePhone::ContinueDTMFStringSending
 * @SYMTestExpectedResults Pass - DTMF tones can be sent individually and as a string.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify single tones and strings can be sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_TRUE_L(mobilePhone.GetDTMFCaps(dtmfCaps)==KErrNone, _L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	CHECK_BITS_SET_L(dtmfCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Send DTMF tones individually. Do the following with aTone = 0-9, A,B,C,D, #,*, p, w in turn. ===	
	TChar tone[18] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9','A' ,'B' ,'C' ,'D' ,'#' ,'*', 'p', 'w'};
	
	for(TInt i = 0 ; i<16; i++)
		{
		DEBUG_PRINTF2(_L("Send DTMF tones: %c"), (char)tone[i]);
		
		// Use RMobilePhone::StartDTMFTone to start sending a DTMF tone.
		ASSERT_EQUALS(mobilePhone.StartDTMFTone(tone[i]), KErrNone, _L("RMobilePhone::StartDTMFTone completed with incorrect error"));
		
		// Wait for one second
		User::After(KOneSecond);
		
		// Use RMobilePhone::StopDTMFTone to stop sending.
		ASSERT_EQUALS(mobilePhone.StopDTMFTone(), KErrNone, _L("RMobilePhone::StopDTMFTone completed with incorrect error"));
		
		// Repeat the above with other valid digits.
		}
		
	// Wait for 5 seconds
	User::After(5*KOneSecond);
	
	// ===  Send DTMF string with no 'w' character ===
	
	// Send DTMF tones
	TExtEtelRequestStatus sendDtmfTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDtmfTonesStatus);
		
	TBufC<50> toneSet1 = _L("0123456p789ABCD");
	
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet1);	
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet1);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones timed-out"));
    
    //Check request completes with KErrNone each time.
    ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones completed with incorrect error"));
	
	// ===  Send DTMF string with 'w' character and continue sending (then repeat and not continue sending after the stop) ===
	
	// Post notification for RMobilePhone::NotifyStopInDTMFString
	TExtEtelRequestStatus notifyStopStatus(mobilePhone, EMobilePhoneNotifyStopInDTMFString);
	CleanupStack::PushL(notifyStopStatus);
	mobilePhone.NotifyStopInDTMFString(notifyStopStatus);
		
	// Send a DTMF string "1234w567"	
	TBufC<50> toneSet2 = _L("1234w567");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet2);	
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet2);
	
	// Check RMobilePhone::NotifyStopInDTMFString completes
	iCallControlTestHelper.WaitForMobilePhoneNotifyStopInDTMFString(notifyStopStatus, KErrNone);
											
	// Continue sending the string with RMobilePhone::ContinueDTMFStringSending with aContinue = ETrue 
	ASSERT_EQUALS(mobilePhone.ContinueDTMFStringSending(ETrue), KErrNone, _L("RMobilePhone::ContinueDTMFStringSending has not returned KErrNone"));
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones sendDtmfTonesStatus is not KErrNone."));
	
	// Check request completes with KErrNone
    ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones sendDtmfTonesStatus completed with incorrect error"));
		
		
	// Hang up the call
	DEBUG_PRINTF1(_L("Hangup call"));	
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&notifyStopStatus);
	CleanupStack::PopAndDestroy(&sendDtmfTonesStatus);
	CleanupStack::PopAndDestroy(&dialStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0002");
	}

CCTSYIntegrationTestDTMF0003::CCTSYIntegrationTestDTMF0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0003::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0003::~CCTSYIntegrationTestDTMF0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0003
 * @SYMFssID BA/CTSY/DTMF-0003
 * @SYMTestCaseDesc Send DTMF strings containing 'p' and 'w'
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::ContinueDTMFStringSending, RMobilePhone::NotifyStopInDTMFString
 * @SYMTestExpectedResults Pass - Notification of a pause in DTMF received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify DTMF stop notifications are received.
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,
				 _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_EQUALS_L(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,
				   _L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	TUint32 unwantedCaps = KNoWantedBits;
	CHECK_BITS_SET_L(dtmfCaps, expectedCaps, unwantedCaps,
					_L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Request for notification of RMobilePhone::NotifyStopInDTMFString 
	TExtEtelRequestStatus notifyStopInDTMFStringStatus(mobilePhone, EMobilePhoneNotifyStopInDTMFString);
	CleanupStack::PushL(notifyStopInDTMFStringStatus);
	mobilePhone.NotifyStopInDTMFString(notifyStopInDTMFStringStatus);

	// ===  Send DTMF string that contains digits only and check that ===
	
	// Send DTMF tones with RMobilePhone::SendDTMFTones with aTones = "1234567890" 
	TBufC<50> toneDigitSet = _L("1234567890");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneDigitSet);
	TExtEtelRequestStatus sendDTMFTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDTMFTonesStatus);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneDigitSet);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones status is not KErrNone"));
	
	// ===  RMobilePhone::NotifyStopInDTMFString is still pending ===
	ASSERT_EQUALS(notifyStopInDTMFStringStatus.Int(), KRequestPending, 
				  _L("RMobilePhone::NotifyStopInDTMFString status is not KRequestPending"));
	
	// *** Note: expecting NotifyStopInDTMFString to still be active from previous post

	// ===  Send DTMF string with a pause and check that ===
	
	// Send DTMF tones with RMobilePhone::SendDTMFTones with aTones = "12345p67890" 
	TBufC<50> toneDigitSet2 = _L("12345p67890");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneDigitSet2);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneDigitSet2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones status is not KErrNone"));

	// Check RMobilePhone::NotifyStopInDTMFString is still KRequestPending
	ASSERT_EQUALS(notifyStopInDTMFStringStatus.Int(), KRequestPending, 
				  _L("RMobilePhone::NotifyStopInDTMFString status is not KRequestPending"));

	// Send DTMF tones with RMobilePhone::SendDTMFTones with aTones = "12345w67890" 
	
	// ===  Send DTMF string with a stop and check that ===
	TBufC<50> toneDigitSet3 = _L("12345w67890");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneDigitSet3);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneDigitSet3);
	
	
	// Check RMobilePhone::NotifyStopInDTMFString completes
	iCallControlTestHelper.WaitForMobilePhoneNotifyStopInDTMFString(notifyStopInDTMFStringStatus, KErrNone);

	// Complete the send
	ASSERT_EQUALS(mobilePhone.ContinueDTMFStringSending(ETrue), KErrNone,
				  _L("RMobilePhone::ContinueDTMFStringSending has not returned KErrNone"));
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones status is not KErrNone"));
	
	// Hang-up call
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));

	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(3, &dialStatus);
						// sendDTMFTonesStatus
						// notifyStopInDTMFStringStatus
						// dialStatus
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0003");
	}



CCTSYIntegrationTestDTMF0004::CCTSYIntegrationTestDTMF0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0004::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0004::~CCTSYIntegrationTestDTMF0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0004
 * @SYMFssID BA/CTSY/DTMF-0004
 * @SYMTestCaseDesc Send an invalid DTMF tone.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::StartDTMFTone
 * @SYMTestExpectedResults Pass - Error from baseband / dispatch layer when sending an invalid tone.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to send invalid tone.
 *
 * @return - TVerdict code
 * 
 * $CTSYProblem - The invalid tone is sent to CTSY.  CTSY simply passes it down to the LTSY.
 * The LTSY is returning KErrNone, and CTSY is propogating the error back up to Etel.
 */
	{
	//
	// SET UP
	//
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,
				 _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_EQUALS_L(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,
				   _L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	TUint32 unwantedCaps = KNoWantedBits;
	ASSERT_BITS_SET(dtmfCaps, expectedCaps, unwantedCaps,
					_L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Attempt to send a single but invalid tone ===

	// Use RMobilePhone::StartDTMFTone to send an invalid tone character.
	
	TChar invalidchar = 'Z';
	
	DEBUG_PRINTF2(_L("Send invalid DTMF tones: %c"), (char)invalidchar);
	
	ASSERT_EQUALS(mobilePhone.StartDTMFTone(invalidchar), KErrNone,
				  _L("RMobilePhone::StartDTMFTone returned with errror - not KErrNone"));

	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&dialStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0004");
	}



CCTSYIntegrationTestDTMF0005::CCTSYIntegrationTestDTMF0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0005::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0005::~CCTSYIntegrationTestDTMF0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0005
 * @SYMFssID BA/CTSY/DTMF-0005
 * @SYMTestCaseDesc Send a string of DTMF tones contains an invalid tone.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SendDTMFTones
 * @SYMTestExpectedResults Pass - Error from baseband / dispatch layer.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to send DTMF string containing an invalid tone.
 *
 * @return - TVerdict code
 * 
 * $CTSYProblem - LTSY is returning error code KErrGsmCCSemanticallyIncorrectMessage, and not KErrArgument
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,
				 _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_EQUALS_L(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,
				   _L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	TUint32 unwantedCaps = KNoWantedBits;
	ASSERT_BITS_SET(dtmfCaps, expectedCaps, unwantedCaps,
					_L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Send some tones but containing an invalid tone character ===
	
	// Use RMobilePhone::SendDTMFTones to send a string containing valid tones and an invalid tone 

	TBufC<50> toneInvalidSet = _L("0123456Q789ABCD");
	DEBUG_PRINTF2(_L("Send invalid DTMF tones: %S"), &toneInvalidSet);
	
	TExtEtelRequestStatus sendDTMFTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDTMFTonesStatus);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneInvalidSet);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	// Check KErrGsmCCSemanticallyIncorrectMessage is returned.
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrGsmCCSemanticallyIncorrectMessage,
				  _L("RMobilePhone::SendDTMFTones status is not KErrGsmCCSemanticallyIncorrectMessage"));
	
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(2, &dialStatus);
						// sendDTMFTonesStatus
						// dialStatus
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0005");
	}



CCTSYIntegrationTestDTMF0006::CCTSYIntegrationTestDTMF0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0006::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0006::~CCTSYIntegrationTestDTMF0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0006
 * @SYMFssID BA/CTSY/DTMF-0006
 * @SYMTestCaseDesc Test sending DTMF digits during a video call.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::StartDTMFTone, RMobilePhone::StopDTMFTone, RMobilePhone::SendDTMFTones, RMobilePhone::NotifyStopInDTMFString, RMobilePhone::ContinueDTMFStringSending, RCall::HangUp, RMobilePhone::GetDTMFCaps
 * @SYMTestExpectedResults Pass - DTMF digits and strings sent.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify DTMF digits can be sent during a video call.
 *
 * @return - TVerdict code
 */
	{  /*** NOTE: CODED BUT NOT TESTED ***/
	
	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Ensure phone supports KCapsData caps. 
	
	RMobilePhone::TCaps mobilephonecaps;
	
	CHECK_EQUALS_L(mobilePhone.GetCaps(mobilephonecaps), KErrNone, _L("RMobilePhone::GetCaps"));
	TUint32 expectedmobilephoneCaps = RMobilePhone::KCapsData | RMobilePhone::KCapsVoice;
	TUint32 unwantedmobilephoneCaps = KNoUnwantedBits;
	ASSERT_BITS_SET(mobilephonecaps.iFlags, expectedmobilephoneCaps, unwantedmobilephoneCaps,
					_L("RMobilePhone::GetCaps returned wrong dynamic caps"));
	
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);		//KDataLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);	//KDataLine, KCall1);

	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse,
				_L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	
	RMobileCall::TMobileDataCallParamsV1 dataParams, retrieveDataParams;
    RMobileCall::TMobileDataCallParamsV1Pckg dataParamsPckg(dataParams);
    RMobileCall::TMobileDataCallParamsV1Pckg retrieveDataParamsPckg(retrieveDataParams);
 
    dataParams.iService = RMobileCall::EServiceDataCircuitAsyncRdi;
    dataParams.iSpeed = RMobileCall::ESpeed9600;
    dataParams.iProtocol = RMobileCall::EProtocolV110;
    dataParams.iQoS = RMobileCall::EQoSNonTransparent;
    dataParams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion1;
    dataParams.iV42bisReq = RMobileCall::EV42bisBothDirections;
    dataParams.iUseEdge = ETrue;
	
	// Post RMobilePhone::NotifyDTMFCapsChange (most likely to change when the phone starts and ends voice calls,
	// so does this also apply to a data call ?)
	
	TUint32 completiondtmfcaps;
	TExtEtelRequestStatus notifyDTMFCapsChangeStatus(mobilePhone, EMobilePhoneNotifyDTMFCapsChange);
	CleanupStack::PushL(notifyDTMFCapsChangeStatus);
	mobilePhone.NotifyDTMFCapsChange(notifyDTMFCapsChangeStatus, completiondtmfcaps);
	
	//IMPORT_C void Dial(TRequestStatus &aStatus, const TDesC8 &aCallParams, const TTelNumberC &aTelNumber);
	mobileCall.Dial(dialStatus, dataParamsPckg, number);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::NotifyDTMFCapsChange completes with caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	TUint32 unwantedCaps = KNoUnwantedBits;
	TInt wantedStatus = KErrNone;
	iCallControlTestHelper.WaitForMobilePhoneNotifyDTMFCapsChange( 
				mobilePhone,
				notifyDTMFCapsChangeStatus,
				completiondtmfcaps, 
				expectedCaps,
				unwantedCaps,
				wantedStatus);

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	ASSERT_EQUALS(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,_L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	ASSERT_BITS_SET(dtmfCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Send a valid individual DTMF digits.
	
	TChar tone = 'A';
	DEBUG_PRINTF2(_L("Send DTMF tones: %c"), (char)tone);
	ASSERT_EQUALS(mobilePhone.StartDTMFTone(tone), KErrNone,
				  _L("RMobilePhone::StartDTMFTone return value is not KErrNone"));
	User::After(KOneSecond);
	ASSERT_EQUALS(mobilePhone.StopDTMFTone(), KErrNone,
				  _L("RMobilePhone::StopDTMFTone return value is not KErrNone"));
	
	// Send a valid DTMF string without a stop or pause character. 
	
	TBufC<50> toneSet1 = _L("0123456789ABCD");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet1);
	TExtEtelRequestStatus sendDtmfTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDtmfTonesStatus);
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	// Check request completes with KErrNone each time.
    ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrNone,
    			  _L("RMobilePhone::SendDTMFTones completed with incorrect error"));
	if(sendDtmfTonesStatus.Int() != KErrNone)
		{
		sendDtmfTonesStatus.Cancel();
		}
		
	// Send a valid DTMF string "12345p67890" 
	
	TBufC<50> toneSet2 = _L("12345p67890");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet2);
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));	
	// Check request completes with KErrNone each time.
    ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrNone,
    			  _L("RMobilePhone::SendDTMFTones completed with incorrect error"));
	if(sendDtmfTonesStatus.Int() != KErrNone)
		{
		sendDtmfTonesStatus.Cancel();
		}

	// Send a valid DTMF string "12345w67890" 
	
	TBufC<50> toneSet3 = _L("12345w67890");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet3);
	
	// Post notification for RMobilePhone::NotifyStopInDTMFString
	TExtEtelRequestStatus notifyStopStatus(mobilePhone, EMobilePhoneNotifyStopInDTMFString);
	CleanupStack::PushL(notifyStopStatus);
	mobilePhone.NotifyStopInDTMFString(notifyStopStatus);
	
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet3);
	
	// Check RMobilePhone::NotifyStopInDTMFString completes
	iCallControlTestHelper.WaitForMobilePhoneNotifyStopInDTMFString(notifyStopStatus, KErrNone);
	
	// Continue sending the string with RMobilePhone::ContinueDTMFStringSending with aContinue = ETrue 
	ASSERT_EQUALS(mobilePhone.ContinueDTMFStringSending(ETrue), KErrNone,
				  _L("RMobilePhone::ContinueDTMFStringSending has not returned KErrNone"));
	// Check request completes with KErrNone each time.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones status is not KErrNone"));
	if(sendDtmfTonesStatus.Int() != KErrNone)
		{
		sendDtmfTonesStatus.Cancel();
		}
			
	// Send a valid DTMF string "12345w67890" again
	DEBUG_PRINTF2(_L("Send DTMF tones (again): %S"), &toneSet3);
	
	// Post notification for RMobilePhone::NotifyStopInDTMFString
	mobilePhone.NotifyStopInDTMFString(notifyStopStatus);
	
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet3);
	
	// Check RMobilePhone::NotifyStopInDTMFString completes
	iCallControlTestHelper.WaitForMobilePhoneNotifyStopInDTMFString(notifyStopStatus, KErrNone);
	
	// Continue sending the string with RMobilePhone::ContinueDTMFStringSending with aContinue = EFalse 
	ASSERT_EQUALS(mobilePhone.ContinueDTMFStringSending(EFalse), KErrNone,
				  _L("RMobilePhone::ContinueDTMFStringSending has not returned KErrNone"));

	// Check request completes with KErrNone each time.
	// *** Does the send complete even if we requested EFalse on the ContinueDTMFStringSending ?
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone,
				  _L("RMobilePhone::SendDTMFTones timed-out"));
	ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones status is not KErrNone"));
	if(sendDtmfTonesStatus.Int() != KErrNone)
		{
		sendDtmfTonesStatus.Cancel();
		}

	// Post NotifyDTMFCapsChange
	mobilePhone.NotifyDTMFCapsChange(notifyDTMFCapsChangeStatus, completiondtmfcaps);

	// Hang up.
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RMobileCall::HangUp did not return with KErrNone"));
	
	// Check RMobilePhone::NotifyDTMFCapsChange completes with caps of 0
	expectedCaps = KNoWantedBits;
	unwantedCaps = KNoUnwantedBits;
	wantedStatus = KErrNone;
	iCallControlTestHelper.WaitForMobilePhoneNotifyDTMFCapsChange( 
				mobilePhone,
				notifyDTMFCapsChangeStatus,
				completiondtmfcaps, 
				expectedCaps,
				unwantedCaps,
				wantedStatus);
				
	// Check RMobilePhone::GetDTMFCaps returns caps of 0
	ASSERT_EQUALS(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,_L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	ASSERT_BITS_SET(dtmfCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(4, &dialStatus);
					// notifyStopStatus
					// sendDtmfTonesStatus
					// notifyDTMFCapsChangeStatus
					// dialStatus
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0006");
	}



CCTSYIntegrationTestDTMF0007::CCTSYIntegrationTestDTMF0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0007::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0007::~CCTSYIntegrationTestDTMF0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0007
 * @SYMFssID BA/CTSY/DTMF-0007
 * @SYMTestCaseDesc Cancel the sending of DTMF Tones
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SendDTMFTones
 * @SYMTestExpectedResults Pass - KErrCancel returned on cancellation.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request to send DTMF tones can be cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	CHECK_TRUE_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium)==KErrNone, _L("RCall::Dial timed-out"));
	CHECK_TRUE_L(dialStatus.Int()==KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_TRUE_L(mobilePhone.GetDTMFCaps(dtmfCaps)==KErrNone, _L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	CHECK_BITS_SET_L(dtmfCaps, expectedCaps, KNoWantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Use RMobilePhone::SendDTMFTones to send a valid, long DTMF string (e.g. 20 characters) 
	TBufC<50> toneSet1 = _L("0123456789ABCD0123456789");
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet1);
	
	TExtEtelRequestStatus sendDtmfTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDtmfTonesStatus);
	mobilePhone.SendDTMFTones(sendDtmfTonesStatus, toneSet1);
	User::After(KOneSecond); // wait for 1 second

	// Cancel sending DTMF tones here 
	mobilePhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);
	
	// Check request completes with KErrCancel	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDtmfTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones timed-out"));	
	ASSERT_EQUALS(sendDtmfTonesStatus.Int(), KErrCancel, _L("RMobilePhone::SendDTMFTones sendDtmfTonesStatus completed with incorrect error"));
    
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&sendDtmfTonesStatus);
	CleanupStack::PopAndDestroy(&dialStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0007");
	}



CCTSYIntegrationTestDTMF0008::CCTSYIntegrationTestDTMF0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0008::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0008::~CCTSYIntegrationTestDTMF0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0008
 * @SYMFssID BA/CTSY/DTMF-0008
 * @SYMTestCaseDesc Remote party terminates call during DTMF sequence
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SendDTMFTones, RCall::NotifyStatusChange, RMobileCall::NotifyCallEvent
 * @SYMTestExpectedResults Pass - Call terminates.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify client is notified of call termination.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine); 

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	
	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));

	DisplayUserInteractionPromptL(_L("To run this test correctly, you should have set your own phone number to voice number 1 in the ini file."), ETimeLong);
	
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_EQUALS_L(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,_L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	CHECK_BITS_SET_L(dtmfCaps, expectedCaps, KNoWantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Use RMobilePhone::SendDTMFTones to send a long string of tones (20 characters). 
	// Do not wait for the request to complete before continuing. 	
	TExtEtelRequestStatus callEventStatus(mobileCall, EMobileCallNotifyCallEvent);	
	CleanupStack::PushL(callEventStatus);
	RMobileCall::TMobileCallEvent mobileCallEvent;
	
	mobileCall.NotifyCallEvent(callEventStatus, mobileCallEvent);
	
	TCoreEtelRequestStatus<RMobileCall> mobileStatus(mobileCall, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(mobileStatus);
	RMobileCall::TStatus changedStatus;
	
	mobileCall.NotifyStatusChange(mobileStatus, changedStatus);
	
	// Use RMobilePhone::SendDTMFTones to send a long string of tones (20 characters). 
	TBufC<50> toneSet1 = _L("0123456789ABCD01234567890123456789ABCD0123456789");
	TExtEtelRequestStatus sendDTMFTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDTMFTonesStatus);

	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet1);	
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneSet1);
	
	User::After(KOneSecond); // wait for 1 second
	
	DisplayUserInteractionPromptL(_L("Hang up the call."), ETimeLong);
		
	RMobileCall::TMobileCallEvent expectedEvent = RMobileCall::ERemoteTerminated;
	iCallControlTestHelper.WaitForMobileCallNotifyCallEvent(mobileCall,
															callEventStatus,
															mobileCallEvent,
															expectedEvent,
															KErrNone);
	
	// Check RCall::NotifyStatusChange completes with EStatusHangingUp.
	RMobileCall::TStatus expectedCallStatus = RMobileCall::EStatusHangingUp;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(mobileCall,
														 mobileStatus,
														 changedStatus,
														 expectedCallStatus,
														 KErrNone);
		
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));
		
	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // dialStatus
    // callEventStatus
    // mobileStatus
	// sendDTMFTonesStatus
	CleanupStack::PopAndDestroy(4, &dialStatus);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0008");
	}



CCTSYIntegrationTestDTMF0009::CCTSYIntegrationTestDTMF0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0009::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0009::~CCTSYIntegrationTestDTMF0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0009
 * @SYMFssID BA/CTSY/DTMF-0009
 * @SYMTestCaseDesc Check that outgoing DTMF tones are added to dialled party details
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::GetMobileCallInfo, RMobilePhone::SendDTMFTones
 * @SYMTestExpectedResults Pass - Dialled party details contains DTMF tones, remote number does not.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify remote party info contains the dialled number and the sent DTMF digits.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Dial a number that answers. 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));

	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	CHECK_EQUALS_L(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone, _L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	CHECK_BITS_SET_L(dtmfCaps, expectedCaps, KNoWantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobileCall::GetMobileCallInfo returns correct number in iRemoteParty.iRemoteNumber
	RMobileCall::TMobileCallInfoV1 syncCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg syncCallInfoPckg(syncCallInfo);
	
	User::After(KOneSecond*10);  // Give timers enough time to increment
	ASSERT_EQUALS(mobileCall.GetMobileCallInfo(syncCallInfoPckg), KErrNone, _L("RMobilePhone::GetMobileCallInfo has not returned KErrNone"));
	
	DEBUG_PRINTF2(_L("syncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber: \"%S\""), &syncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber);
	DEBUG_PRINTF2(_L("number: \"%S\""), &number);

	ASSERT_EQUALS(syncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Compare(number), 0, _L("aSyncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber is not correct"));
	
	// Check RMobileCall::GetMobileCallInfo returns correct number in iDialledParty
	ASSERT_EQUALS(syncCallInfo.iDialledParty.iTelNumber.Compare(number), 0, _L("syncCallInfo.iDialledParty.iTelNumber is not correct"));
	
	// Send some valid DTMF tones. 
	TExtEtelRequestStatus sendDTMFTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDTMFTonesStatus);
	
	TBufC<50> toneSet1 = _L("0123456789ABCD");
	
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet1);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneSet1);
	
	// Check request completes with KErrNone each time.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones timed-out"));	
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrNone, _L("RMobilePhone::SendDTMFTones completed with incorrect error"));
	
	// ===  Check that the DTMF has been added to the dialled number details, ===
	ASSERT_EQUALS(mobileCall.GetMobileCallInfo(syncCallInfoPckg), KErrNone, _L("RMobilePhone::GetMobileCallInfo has not returned KErrNone"));

	// ===  but not the remote party details. ===

	DEBUG_PRINTF2(_L("syncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber: %S"), &syncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber);
	DEBUG_PRINTF2(_L("number: %S"), &number);

	// Check RMobileCall::GetMobileCallInfo returns correct destination number without DTMF in iRemoteParty.iRemoteNumber
	ASSERT_EQUALS(syncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Compare(number), 0, _L("aSyncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber is not correct"));
	
	// Check RMobileCall::GetMobileCallInfo returns dialled number and DTMF tones sent in iDialledParty
	TBuf<50> expectedval;
	
	expectedval.Append(number);
	expectedval.Append(toneSet1);
	
	DEBUG_PRINTF2(_L("syncCallInfo.iDialledParty.iTelNumber: %S"), &syncCallInfo.iDialledParty.iTelNumber);
	DEBUG_PRINTF2(_L("expectedval: %S"), &expectedval);
	
	ASSERT_EQUALS(syncCallInfo.iDialledParty.iTelNumber.Compare(expectedval), 0, _L("syncCallInfo.iDialledParty.iTelNumber is not correct"));
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));

	//
	// TEST END
	//

    StartCleanup();

	CleanupStack::Pop(&dialStatus);
	CleanupStack::Pop(&sendDTMFTonesStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0009");
	}

CCTSYIntegrationTestDTMF0010::CCTSYIntegrationTestDTMF0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0010::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0010::~CCTSYIntegrationTestDTMF0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0010
 * @SYMFssID BA/CTSY/DTMF-0010
 * @SYMTestCaseDesc Receive DTMF tones.
 * @SYMTestPriority High
 * @SYMTestActions RCall::NotifyStatusChange, RMobilePhone::ReadDTMFTones, RCall::HangUp
 * @SYMTestExpectedResults Pass - DTMF tones read correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify DTMF tones received correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	

	// Ensure phone supports KCapsVoice.
	RMobilePhone::TCaps mobilePhoneCaps;
	
	CHECK_TRUE_L(mobilePhone.GetCaps(mobilePhoneCaps) == KErrNone, _L("RMobilePhone::GetCaps"));
	TUint32 expectedMobilePhoneCaps = RMobilePhone::KCapsVoice;
	CHECK_BITS_SET_L(mobilePhoneCaps.iFlags, expectedMobilePhoneCaps, KNoWantedBits, _L("RMobilePhone::GetCaps returned wrong dynamic caps"));	
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Set up an incoming call.	
	TName name;
	TCoreEtelRequestStatus<RLine> notifyIncomingStatus(mobileLine, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingStatus);
	mobileLine.NotifyIncomingCall(notifyIncomingStatus, name);
	
	RCall::TStatus toneSet;
	TCoreEtelRequestStatus<RMobileLine> notifyStatusChangeStatus(mobileLine, &RMobileLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, toneSet);
	
	DisplayUserInteractionPromptL(_L("Please call me "), ETimeMedium);										  
	
	// Check RLine::NotifyIncomingCall completes with valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingStatus,ETimeMedium), KErrNone, 
			_L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingStatus.Int(), KErrNone,  
			_L("RLine::NotifyIncomingCall set requestStatus to an error"));
	ASSERT_TRUE(name.Length() > 0,  
			_L("RLine::NotifyIncomingCall did not set the name"));

	TCallId incomingCallId;
	RMobileCall& incomingCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, name, incomingCallId);
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(incomingCall, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);

															
	RCall::TStatus expectedStatusChange=RCall::EStatusRinging;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
											  notifyStatusChangeStatus,
											  toneSet, 
											  expectedStatusChange,
											  KErrNone);

	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, toneSet);

	// Answer the incoming call. 		
	incomingCall.AnswerIncomingCall(answerIncomingStatus);	
	
	// Check RCall::NotifyStatusChange completes with EStatusAnswering -> EStatusConnected.	
	expectedStatusChange=RCall::EStatusAnswering;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
											  notifyStatusChangeStatus,
											  toneSet, 
											  expectedStatusChange,
											  KErrNone);
	
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, toneSet);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RCall::AnswerIncomingCall timed-out"));
	
	expectedStatusChange=RCall::EStatusConnected;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
											  notifyStatusChangeStatus,
											  toneSet, 
											  expectedStatusChange,
											  KErrNone);
											  
	DisplayUserInteractionPromptL(_L("Please tone 1234567890"), ETimeMedium);										  

	// Check RMobilePhone::ReadDTMFTones completes with correct tones	
	TBuf<50> tonesrecieved;
	tonesrecieved.Zero();
	
	TExtEtelRequestStatus readDtmfStatus(mobilePhone, EMobilePhoneReadDTMFTones);
	CleanupStack::PushL(readDtmfStatus);
	mobilePhone.ReadDTMFTones(readDtmfStatus, tonesrecieved);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readDtmfStatus, ETimeMedium), KErrNone, _L("RMobilePhone::ReadDTMFTones timed-out"));		
	
	ERR_PRINTF1(_L("<font color=Orange>$CTSYProblem: case EMobilePhoneReadDTMFTones: Not supported in CTSY </font>"));
	ASSERT_EQUALS(KErrNotSupported, readDtmfStatus.Int(), _L("RMobilePhone::ReadDTMFTones returned incorrect status."));	
	ASSERT_TRUE(tonesrecieved.Compare(_L(""))==0, _L("RMobilePhone::ReadDTMFTones wrong tone was recieved."));

	// Hang up call. 
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    // notifyIncomingStatus
    // notifyStatusChangeStatus
    // answerIncomingStatus
    // readDtmfStatus
    CleanupStack::PopAndDestroy(4,&notifyIncomingStatus);
	
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0010");
	}

CCTSYIntegrationTestDTMF0011::CCTSYIntegrationTestDTMF0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDTMFBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDTMF0011::GetTestStepName());
	}

CCTSYIntegrationTestDTMF0011::~CCTSYIntegrationTestDTMF0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDTMF0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DTMF-0011
 * @SYMFssID BA/CTSY/DTMF-0011
 * @SYMTestCaseDesc Ensure that DTMF tones can be sent to all conference call participants
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::Hold, RCall::HangUp, RMobileConferenceCall::CreateConference, RMobilePhone::StartDTMFTone, RMobilePhone::StopDTMFTone, RMobilePhone::SendDTMFTones, RMobilePhone::NotifyStopInDTMFString, RMobilePhone::ContinueDTMFStringSending, RMobilePhone::GetDTMFCaps, RMobilePhone::NotifyDTMFCapsChange
 * @SYMTestExpectedResults Pass - DTMF tones sent to all conference call participants.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify sending of DTMF tones and strings can be sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Ensure phone supports KCapsVoice.
	RMobilePhone::TCaps mobilephonecaps;
	
	CHECK_EQUALS_L(mobilePhone.GetCaps(mobilephonecaps), KErrNone, _L("RMobilePhone::GetCaps"));
	TUint32 expectedmobilephoneCaps = RMobilePhone::KCapsVoice;
	CHECK_BITS_SET_L(mobilephonecaps.iFlags, expectedmobilephoneCaps, KNoWantedBits, _L("RMobilePhone::GetCaps returned wrong dynamic caps"));
	
	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Set up a five party conference call.
	TExtEtelRequestStatus holdStatus(mobilePhone, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number), _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));
	
	mobileCall.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out"));
	ASSERT_EQUALS(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold status incorrect"));
	
	RMobileConferenceCall& cc = iEtelSessionMgr.GetConferenceCallL(KMainServer, KMainPhone, KMainConferenceCall);
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);
	
	TPtrC number2;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, number2), _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number2);
	TCoreEtelRequestStatus<RMobileCall> dialStatus2(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus2);
	call2.Dial(dialStatus2, number2); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus2, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus2.Int(), KErrNone, _L("RCall::Dial problem dialling"));
	
	TExtEtelRequestStatus createConferenceStatus(cc, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConferenceStatus);
	
	cc.CreateConference(createConferenceStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createConferenceStatus, ETimeMedium), KErrNone, _L("RMobileConferenceCall::CreateConference timed-out"));
	ASSERT_EQUALS(createConferenceStatus.Int(), KErrNone, _L("RMobileConferenceCall::CreateConference status incorrect"));
	
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
	
	TPtrC num3, num4, num5;
	ASSERT_TRUE(GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber6, num3), _L("Failed to obtain 3rd number for test"));	
	ASSERT_TRUE(GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), _L("Failed to obtain 4th number for test"));
	ASSERT_TRUE(GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), _L("Failed to obtain 5th number for test"));
	
	
	AddCallToConferenceL(cc, call3, num3, 3);
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));
	AddCallToConferenceL(cc, call4, num4, 4);
 	TEST_CHECK_POINT_L(_L("Adding call4 failed"));
 	AddCallToConferenceL(cc, call5, num5, 5);
 	TEST_CHECK_POINT_L(_L("Adding call5 failed"));
 		
			
	// Check RMobilePhone::GetDTMFCaps returns caps of KCapsSendDTMFString | KCapsSendDTMFSingleTone
	TUint32 dtmfCaps=0;
	ASSERT_EQUALS(mobilePhone.GetDTMFCaps(dtmfCaps), KErrNone,_L("RMobilePhone::GetDTMFCaps return value not KErrNone"));
	TUint32 expectedCaps = RMobilePhone::KCapsSendDTMFString | RMobilePhone::KCapsSendDTMFSingleTone;
	ASSERT_BITS_SET(dtmfCaps, expectedCaps, KNoWantedBits, _L("RMobilePhone::GetDTMFCaps returned wrong dynamic caps"));

	// Send a single DTMF tone (repeat with each individual character). 
	TChar tone[18] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9','A' ,'B' ,'C' ,'D' ,'#' ,'*', 'p', 'w'};
	
	for(TInt i = 0 ; i<18; i++)
		{
		DEBUG_PRINTF2(_L("Send DTMF tones: %c"), (char)tone[i]);
		ASSERT_EQUALS(mobilePhone.StartDTMFTone(tone[i]), KErrNone, _L("RMobilePhone::StartDTMFTone completed with incorrect error"));
		User::After(KOneSecond);
		ASSERT_EQUALS(mobilePhone.StopDTMFTone(), KErrNone, _L("RMobilePhone::StopDTMFTone completed with incorrect error"));
		}
	
	// Wait for 5 seconds
	User::After(5*KOneSecond);
	
	// Send a DTMF string without 'w' or 'p' 	
	TExtEtelRequestStatus sendDTMFTonesStatus(mobilePhone, EMobilePhoneSendDTMFTones);
	CleanupStack::PushL(sendDTMFTonesStatus);
	
	TBufC<50> toneSet1 = _L("0123456789");
	
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet1);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneSet1);
	
	// Check request completes with KErrNone each time.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones timed-out"));	
	ERR_PRINTF1(_L("<font color=Orange>$CTSYProblem: In case of conference call it gets \n KErrGsmCCSemanticallyIncorrectMessage from LTSY as an answer for SendDTMFTones() </font>"));
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrGsmCCSemanticallyIncorrectMessage, _L("RMobilePhone::SendDTMFTones completed with incorrect error"));

	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
 	ASSERT_EQUALS(cc.GetConferenceStatus(confCallConfStatus), KErrNone, 
 				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
 	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
 				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive")); 
	
	// Send a DTMF string with 'p'
	TBufC<50> toneSet2 = _L("0123456p789ABCD");
	
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet2);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneSet2);
	
	// Check request completes with KErrNone each time.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones timed-out"));	
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrGsmCCSemanticallyIncorrectMessage, _L("RMobilePhone::SendDTMFTones completed with incorrect error"));

	TExtEtelRequestStatus notifyStopStatus(mobilePhone, EMobilePhoneNotifyStopInDTMFString);
	CleanupStack::PushL(notifyStopStatus);
	mobilePhone.NotifyStopInDTMFString(notifyStopStatus);
	
	// Send a DTMF string "1234w567"
	TBufC<50> toneSet3 = _L("1234w567");
	
	DEBUG_PRINTF2(_L("Send DTMF tones: %S"), &toneSet3);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneSet3);
	
	// Check RMobilePhone::NotifyStopInDTMFString completes
	iCallControlTestHelper.WaitForMobilePhoneNotifyStopInDTMFString(notifyStopStatus, KErrNone);
	DEBUG_PRINTF1(_L("RMobilePhone::NotifyStopInDTMFString completes"));
	
	// Continue sending the string with RMobilePhone::ContinueDTMFStringSending with aContinue = ETrue 
	ASSERT_EQUALS(mobilePhone.ContinueDTMFStringSending(ETrue), KErrNone, _L("RMobilePhone::ContinueDTMFStringSending has not returned KErrNone"));
	
	// Check request completes with KErrNone each time.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendDTMFTonesStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SendDTMFTones timed-out"));	
	ASSERT_EQUALS(sendDTMFTonesStatus.Int(), KErrGsmCCSemanticallyIncorrectMessage, _L("RMobilePhone::SendDTMFTones completed with incorrect error"));
	
	// Send a valid DTMF string "1234w567" again
	mobilePhone.NotifyStopInDTMFString(notifyStopStatus);
	
	DEBUG_PRINTF2(_L("Send DTMF tones (again): %S"), &toneSet3);
	mobilePhone.SendDTMFTones(sendDTMFTonesStatus, toneSet3);
	
	// Check RMobilePhone::NotifyStopInDTMFString completes
	iCallControlTestHelper.WaitForMobilePhoneNotifyStopInDTMFString(notifyStopStatus, KErrNone);
	DEBUG_PRINTF1(_L("RMobilePhone::NotifyStopInDTMFString completes"));

	// Continue sending the string with RMobilePhone::ContinueDTMFStringSending with aContinue = EFalse 
	ASSERT_EQUALS(mobilePhone.ContinueDTMFStringSending(EFalse), KErrNone, _L("RMobilePhone::ContinueDTMFStringSending has not returned KErrNone"));
	
	// Post NotifyDTMFCapsChange
	TUint32 completiondtmfcaps;
	TExtEtelRequestStatus notifyDTMFCapsChangeStatus(mobilePhone, EMobilePhoneNotifyDTMFCapsChange);
	CleanupStack::PushL(notifyDTMFCapsChangeStatus);
	mobilePhone.NotifyDTMFCapsChange(notifyDTMFCapsChangeStatus, completiondtmfcaps);
	
	// Terminate the conference call.
    TExtEtelRequestStatus reqHangupStatus(cc, EMobileConferenceCallHangUp);
    CleanupStack::PushL(reqHangupStatus);
	cc.HangUp(reqHangupStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqHangupStatus, ETimeMedium), KErrNone, _L("RMobileConferenceCall::HangUp timed-out"));	
	ASSERT_EQUALS(reqHangupStatus.Int(), KErrNone, _L("RMobileConferenceCall::HangUp completed with incorrect error"));

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10053);
	// Check RMobilePhone::NotifyDTMFCapsChange completes with caps of 0
	TUint32 wantedCaps=0;
	iCallControlTestHelper.WaitForMobilePhoneNotifyDTMFCapsChange( 
				mobilePhone,
				notifyDTMFCapsChangeStatus,
				completiondtmfcaps, 
				wantedCaps,
				KNoUnwantedBits,
				KErrNone);
	
	// Check RMobilePhone::GetDTMFCaps returns caps of 0
	TUint32 getCaps;
	ASSERT_EQUALS(mobilePhone.GetDTMFCaps(getCaps), KErrNone, _L("RMobilePhone::GetDTMFCaps returned incorrect value"));	
	
	//
	// TEST END
	//

    StartCleanup();
	
    //holdStatus
    //dialStatus
    //dialStatus2
    //createConferenceStatus
    //notifyStopStatus
    //sendDTMFTonesStatus
    //notifyDTMFCapsChangeStatus
    //reqHangupStatus
 	CleanupStack::PopAndDestroy(8,&holdStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDTMF0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDTMF0011");
	}

/**
 * Add a call to a conference. Leaves on failure.
 * @param aConfCall Conference call object
 * @param aCall Call object to use
 * @param aNumber Number to dial
 *
 */
 void CCTSYIntegrationTestDTMF0011::AddCallToConferenceL(RMobileConferenceCall& aConfCall, 
 														 RMobileCall& aCall,
 														 TPtrC& aNumber,
 														 TInt aIndex)
 	{
 	
 	DEBUG_PRINTF3(_L("Adding call %d to conference : %S"), aIndex, &aNumber );
 	// Swap the conference call. 
 	TExtEtelRequestStatus reqSwapConfStatus(aConfCall,EMobileConferenceCallSwap);
 	CleanupStack::PushL(reqSwapConfStatus);
 	
 	aConfCall.Swap(reqSwapConfStatus);
 	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
 							_L("RMobileConferenceCall::Swap timed out"));
 	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
 							_L("RMobileConferenceCall::Swap returned error"));
 	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
 	
 	// Dial a number that answers 
 	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	TCoreEtelRequestStatus<RMobileCall> dialStatus(aCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	aCall.Dial( dialStatus, aNumber ); 
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned an error") );
 	TEST_CHECK_POINT_L(_L("Failed to connect call"));
 			
 	// Set up notification
 	TExtEtelRequestStatus notifyConfEventStatus(aConfCall,EMobileConferenceCallNotifyConferenceEvent);
 	CleanupStack::PushL(notifyConfEventStatus);
 	RMobileConferenceCall::TMobileConferenceEvent confEvent;
 	TName confEventCallName;
 	aConfCall.NotifyConferenceEvent( notifyConfEventStatus, confEvent, confEventCallName );

 	// Add call to the conference 
 	TExtEtelRequestStatus reqAddCallStatus(aConfCall,EMobileConferenceCallAddCall);
 	CleanupStack::PushL(reqAddCallStatus);

 	// Get the call info (need the name)
 	RCall::TCallInfo callInfo;
 	TInt err = aCall.GetInfo(callInfo);
 	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));	

 	aConfCall.AddCall(reqAddCallStatus,callInfo.iCallName);

 	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqAddCallStatus,ETimeShort),KErrNone, 
 							_L("RMobileConferenceCall::AddCall timed out"));
 	ASSERT_EQUALS( reqAddCallStatus.Int(),KErrNone, 
 							_L("RMobileConferenceCall::AddCall returned error"));
 	TEST_CHECK_POINT_L(_L("AddCallToConferenceL failed"));

 	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallAdded
 	iCallControlTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( aConfCall, 
 								notifyConfEventStatus,
 								confEvent,RMobileConferenceCall::EConferenceCallAdded,
 								confEventCallName,
 								KErrNone);
 	
 	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
 	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
 	ASSERT_EQUALS(aConfCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
 				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
 	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
 				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));
 	
 	TEST_CHECK_POINT_L(_L("AddCallToConferenceL failed"));
 									
 	// reqAddCallStatus
 	// dialStatus
 	// notifyConfEventStatus
 	// reqSwapConfStatus
 	CleanupStack::PopAndDestroy(4,&reqSwapConfStatus);				
 	}
