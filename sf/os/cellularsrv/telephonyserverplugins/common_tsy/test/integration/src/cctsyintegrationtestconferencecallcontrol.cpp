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
// Test step definitions for the ConferenceCallControl functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestconferencecallcontrol.h"
#include "cctsyinidata.h"
#include "etelmmerr.h"
#include "cctsytestlogging.h"
#include <stdlib.h>


	
CCTSYIntegrationTestConferenceCallControlBase::CCTSYIntegrationTestConferenceCallControlBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), 
		iCallControlTsyTestHelper(*this),
		iNetworkTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestConferenceCallControlBase::~CCTSYIntegrationTestConferenceCallControlBase()
/*
 * Destructor
 */
	{
	}
	
	
/**
* Make a call and verify the new status
* @param aCall Call object
* @param aNumber Number is be used
* @return Status of conference call
*/
RMobileCall::TMobileCallStatus CCTSYIntegrationTestConferenceCallControlBase::MakeCallL(RMobileCall& aCall, TPtrC& aNumber)
	{
	
	// set up notification for call status changes
	TExtEtelRequestStatus notifyCallStatus(aCall,EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatus);
	RMobileCall::TMobileCallStatus callStatus;	
	aCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);
	
	// Dial the number
	TCoreEtelRequestStatus<RMobileCall> dialStatus(aCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	aCall.Dial(dialStatus, aNumber); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned an error") );

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall, notifyCallStatus, callStatus, RMobileCall::EStatusDialling, KErrNone);
	aCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall, notifyCallStatus, callStatus, RMobileCall::EStatusConnecting, KErrNone);
	aCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall, notifyCallStatus, callStatus, RMobileCall::EStatusConnected, KErrNone);
	
	// Pop
	// notifyCallStatus
	// dialStatus
	CleanupStack::PopAndDestroy(2, &notifyCallStatus);
	
	return callStatus;
	}
	

/**
* Place a call on hold and verify the new status
* @param aCall Call object
* @return Status of conference call
*/
RMobileCall::TMobileCallStatus CCTSYIntegrationTestConferenceCallControlBase::HoldCallL( RMobileCall& aCall)
	{

    // Post notification for call status change
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(aCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;	
	aCall.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);

    // Hold call
	TExtEtelRequestStatus reqHoldStatus(aCall, EMobileCallHold);
	CleanupStack::PushL(reqHoldStatus);
	aCall.Hold(reqHoldStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqHoldStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out"));
	ASSERT_EQUALS(reqHoldStatus.Int(), KErrNone, _L("RMobileCall::Hold returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusHold.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(aCall,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															RMobileCall::EStatusHold,
															KErrNone);
    // Pop
	// notifyMobileCallStatusChangeStatus
	// reqHoldStatus
	CleanupStack::PopAndDestroy(2, &notifyMobileCallStatusChangeStatus);
	
	return mobileCallStatus;
	}


/**
* Add a call to a conference. Leaves on failure.
* @param aConfCall Conference call object
* @param aCall Call object to use
* @param aNumber Number to dial
*
*/
void CCTSYIntegrationTestConferenceCallControlBase::AddCallToConferenceL( RMobileConferenceCall& aConfCall, 
																				RMobileCall& aCall, TPtrC& aNumber,
																				TInt aIndex )
	{
	
	DEBUG_PRINTF3(_L("Adding call %d to conference : %S"), aIndex, &aNumber );
	// Swap the conference call. 
	TExtEtelRequestStatus reqSwapConfStatus(aConfCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);
	aConfCall.Swap(reqSwapConfStatus);
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, _L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, _L("RMobileConferenceCall::Swap returned error"));
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	// Dial a number that answers 
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(aCall, aNumber);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call"));
	TEST_CHECK_POINT_L(_L("Failed to connect call"));
			
	// Set up notification
	TExtEtelRequestStatus notifyConfEventStatus(aConfCall,EMobileConferenceCallNotifyConferenceEvent);
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);

	// Add call to the conference 
	TExtEtelRequestStatus reqAddCallStatus(aConfCall,EMobileConferenceCallAddCall);
	CleanupStack::PushL(reqAddCallStatus);

	// Get the call info (need the name)
	RCall::TCallInfo callInfo;
	TInt err = aCall.GetInfo(callInfo);
	ASSERT_EQUALS(err, KErrNone, _L("RCall::GetInfo returned error"));	
	aConfCall.AddCall(reqAddCallStatus,callInfo.iCallName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqAddCallStatus,ETimeShort),KErrNone, _L("RMobileConferenceCall::AddCall timed out"));
	ASSERT_EQUALS(reqAddCallStatus.Int(),KErrNone, _L("RMobileConferenceCall::AddCall returned error"));
	TEST_CHECK_POINT_L(_L("AddCallToConferenceL failed"));

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallAdded
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( aConfCall, 
								notifyConfEventStatus,
								confEvent,
								RMobileConferenceCall::EConferenceCallAdded,
								confEventCallName,
								KErrNone);
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(aConfCall.GetConferenceStatus(confCallConfStatus), KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS(confCallConfStatus, RMobileConferenceCall::EConferenceActive, _L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));
	TEST_CHECK_POINT_L(_L("AddCallToConferenceL failed"));
	
	// Pop								
	// reqAddCallStatus
	// reqSwapConfStatus
	CleanupStack::PopAndDestroy(2,&reqSwapConfStatus);				
	}

/**
* Check the conference call information returned 
* @param aConfCall Conference call object
* @param aNumber Dialled number for comparison
* @param aCallCount 1-based call number 
*/
void CCTSYIntegrationTestConferenceCallControlBase::CheckConfCallInfo(RMobileConferenceCall& aConfCall, 
																	TPtrC& aNumber,
																	TInt aCallCount)
	{	
	DEBUG_PRINTF2(_L("Checking call info from conf call for call %d"), aCallCount);

	// Check RMobileConferenceCall::GetMobileCallInfo returns
	RMobileCall::TMobileCallInfoV3 mobileCallInfo;
	RMobileCall::TMobileCallInfoV3Pckg mobileCallInfoPkg(mobileCallInfo);
	TInt err = aConfCall.GetMobileCallInfo(aCallCount-1, mobileCallInfoPkg);
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));

	// Valid iCallName 
	ASSERT_TRUE(mobileCallInfo.iCallName.Length() > 0, 
						_L("RMobileConferenceCall::GetMobileCallInfo returned empty iCallName"));

	// Correct number in iDialledParty 
	TPtrC dialledPartyNumber(mobileCallInfo.iDialledParty.iTelNumber);
	ASSERT_EQUALS_DES16( dialledPartyNumber, aNumber, 
					_L("RMobileConferenceCall::GetMobileCallInfo iDialledParty number does not match called number"));

	// Valid iLineName 
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0, _L("RMobileConferenceCall::GetMobileCallInfo returned empty iLineName"));
	
	// iStatus of EStatusConnected 
	ASSERT_EQUALS(mobileCallInfo.iStatus, RMobileCall::EStatusConnected, _L("RMobileConferenceCall::GetMobileCallInfo iDialledParty number does not match called number"));

	//  Relevant bits set in iValid based on the above parameters 
	// $CTSYProblem CTSY does not return the RMobileCall::KCallRemoteParty bit 
	ASSERT_BITS_SET(mobileCallInfo.iValid, RMobileCall::KCallDialledParty, // | RMobileCall::KCallRemoteParty,
						KNoUnwantedBits, _L("RMobileConferenceCall::GetMobileCallInfo returned invalid iValid"));

	}



CCTSYIntegrationTestConferenceCallControl0001::CCTSYIntegrationTestConferenceCallControl0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0001::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0001::~CCTSYIntegrationTestConferenceCallControl0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0001
 * @SYMFssID BA/CTSY/CONC-0001
 * @SYMTestCaseDesc Create a conference call with no calls.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetCaps, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::CreateConference, RMobileConferenceCall::NotifyCapsChange
 * @SYMTestExpectedResults Pass - Conference creation fails with KErrNotReady.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify creating a conference fails.
 * Verify conference call related notifications do not complete.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get conference call subsession. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Check conference status and information before creating a conference ===

	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate | KCapsHangUp | KCapsSwap
	TUint32 caps; 
	TInt result = confCall.GetCaps(caps);
	ASSERT_EQUALS(result, KErrNone, _L("RMobileConferenceCall::GetCaps failed."));
	
	TUint unwantedCaps = 	RMobileConferenceCall::KCapsCreate |
						RMobileConferenceCall::KCapsHangUp |
						RMobileConferenceCall::KCapsSwap;
	
	ASSERT_BITS_SET( caps,  KNoUnwantedBits, unwantedCaps, _L("RMobileConferenceCall::GetCaps returned wrong mask") );

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 0
	TInt callCount;
	ASSERT_EQUALS( confCall.EnumerateCalls(callCount), KErrNone, 
						_L("RMobileConferenceCall::EnumerateCalls failed."));
	ASSERT_EQUALS(callCount,0, _L("RMobileConferenceCall::EnumerateCalls returned non-zero"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus failed"));
	ASSERT_EQUALS( RMobileConferenceCall::EConferenceIdle,confCallConfStatus,
				_L("RMobileConferenceCall::GetConferenceStatus returned bad state"));
	
	// Request notification for RMobileConferenceCall::NotifyCapsChange 
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	TUint32 confCallCaps;
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);	

	// Request notification for RMobileConferenceCall::NotifyConferenceEvent 
	TExtEtelRequestStatus notifyConfEventStatus(confCall, EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventName);

	// Request notification for RMobileConferenceCall::NotifyConferenceStatusChange 
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );

	// Create a conference with RMobileConferenceCall::CreateConference 
	TExtEtelRequestStatus createConfStatus(confCall, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConfStatus);
	confCall.CreateConference(createConfStatus);
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createConfStatus, ETimeMedium), KErrNone, 
				_L("RMobileConferenceCall::CreateConference timed out."));
	
	// Check conference creation completes with KErrNotReady	
	ASSERT_EQUALS( createConfStatus.Int(), KErrNotReady,
				_L("RMobileConferenceCall::CreateConference did not return KErrNotReady") );


	// ===  Check conference notifications did not complete ===

	// Check RMobileConferenceCall::NotifyCapsChange is still KRequestPending
	ASSERT_EQUALS( notifyCapsChangeStatus.Int(), KRequestPending, 
					_L("NotifyCapsChange completed unexpectedly"));

	// Check RMobileConferenceCall::NotifyConferenceEvent is still KRequestPending
	ASSERT_EQUALS( notifyConfEventStatus.Int(), KRequestPending,
					_L("NotifyConferenceEvent completed unexpectedly"));

	// Check RMobileConferenceCall::NotifyConferenceStatusChange is still KRequestPending
	ASSERT_EQUALS( notifyConfStatusChange.Int(), KRequestPending,
					_L("NotifyConferenceStatusChange completed unexpectedly"));
	
	//
	// TEST END
	//

    StartCleanup();

	CleanupStack::PopAndDestroy(4, &notifyCapsChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0001");
	}



CCTSYIntegrationTestConferenceCallControl0002::CCTSYIntegrationTestConferenceCallControl0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0002::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0002::~CCTSYIntegrationTestConferenceCallControl0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0002
 * @SYMFssID BA/CTSY/CONC-0002
 * @SYMTestCaseDesc Create a conference call with only one call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetCaps, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::CreateConference, RMobileConferenceCall::NotifyCapsChange, RCall::GetStatus, RCall::HangUp, RCall::Dial, RCall::NotifyStatusChange
 * @SYMTestExpectedResults Pass - Conference creation fails with KErrMMEtelCallForbidden
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify creating a conference fails.
Verify conference call related notifications can be cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	// Get conference call subsession. 
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
																			   
	// Get voice line 1. 
	RMobileLine& voiceLine1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post a notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus callStatus;
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	
	// Dial a number that answers.
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, 
				_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
				_L("RCall::Dial timed-out"))
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned an error"));
	
	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RCall::TStatus expectedCallStatus = RCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);	
														 								 
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	expectedCallStatus = RCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
													 	notifyStatusChangeStatus,
													 	callStatus,
													 	expectedCallStatus,
													 	KErrNone);
													 
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
													 	notifyStatusChangeStatus,
													 	callStatus,
													 	expectedCallStatus,
													 	KErrNone);														 
													 
	// Check RCall::GetStatus returns EStatusConnected.
	ASSERT_EQUALS(call1.GetStatus(callStatus),KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, 
				_L("RCall::GetStatus returned an unexpected call status"));

	// ===  Check conference caps, status and info are correct. ===

	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate | KCapsHangUp | KCapsSwap
	TUint32 caps;
	ASSERT_EQUALS(confCall.GetCaps(caps), KErrNone,
				_L("RMobileConferenceCall::GetCaps returned an error"));
	TUint32 wantedCaps = KNoWantedBits;
	TUint32 unWantedCaps = RMobileConferenceCall::KCapsCreate | RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap;
	ASSERT_BITS_SET(caps, wantedCaps, unWantedCaps,
				_L("RMobileConferenceCall::GetGaps returned unexpected caps"));

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 0
	TInt count;
	ASSERT_EQUALS(confCall.EnumerateCalls(count), KErrNone, 
				_L("RMobileConferenceCall::EnumerateCalls returned an error"));
	ASSERT_EQUALS(count,0, _L("RMobileConferenceCall::EnumerateCalls did not return aCount = 0"));				

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	ASSERT_EQUALS(confCall.GetConferenceStatus(confStatus), KErrNone,
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS(confStatus, RMobileConferenceCall::EConferenceIdle,
				_L("RMobileConferenceCall::GetConferenceStatus did not return status of EConferenceIdle"));					

	// Request notification for RMobileConferenceCall::NotifyCapsChange
	TExtEtelRequestStatus notifyCapsChangeStatus (confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);	
	confCall.NotifyCapsChange(notifyCapsChangeStatus, caps);
	
	// Request notification for RMobileConferenceCall::NotifyConferenceEvent 
	TExtEtelRequestStatus notifyConfEventStatus(confCall, EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confName);	

	// Request notification for RMobileConferenceCall::NotifyConferenceStatusChange 
	TExtEtelRequestStatus notifyMobileConfStatusChangeStatus (confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyMobileConfStatusChangeStatus);
	RMobileConferenceCall::TMobileConferenceStatus mobileConfStatus;
	confCall.NotifyConferenceStatusChange(notifyMobileConfStatusChangeStatus,mobileConfStatus);							
	
	// Create a conference with RMobileConferenceCall::CreateConference 
	TExtEtelRequestStatus createConfStatus (confCall, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConfStatus);
	confCall.CreateConference(createConfStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createConfStatus, ETimeMedium), KErrNone, _L("RMobileConferenceCall::CreateConference timed-out"));
	
	// Check conference creation completes with KErrMMEtelCallForbidden
	
	ASSERT_EQUALS(createConfStatus.Int(), KErrMMEtelCallForbidden, _L("RMobileConferenceCall::CreateConference returned an error"));

	// ===  Check conference notifications did not complete ===

	// Check RMobileConferenceCall::NotifyCapsChange is still KRequestPending
	ASSERT_EQUALS(notifyCapsChangeStatus.Int(), KRequestPending, _L("RMobileConferenceCall::NotifyCapsChange is not KRequestPending"));
	
	// Check RMobileConferenceCall::NotifyConferenceEvent is still KRequestPending
	ASSERT_EQUALS(notifyConfEventStatus.Int(), KRequestPending, _L("RMobileConferenceCall::NotifyConferenceEvent is not KRequestPending"));

	// Check RMobileConferenceCall::NotifyConferenceStatusChange is still KRequestPending
	ASSERT_EQUALS(notifyMobileConfStatusChangeStatus.Int(), KRequestPending, _L("RMobileConferenceCall::NotifyConferenceStatusChange is not KRequestPending"));

	// Hang up call.
	ASSERT_EQUALS(call1.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    //createConfStatus
	//notifyMobileConfStatusChangeStatus
	//notifyConfEventStatus
	//notifyCapsChangeStatus
	//dialStatus
	CleanupStack::PopAndDestroy(6,&notifyStatusChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0002");
	}



CCTSYIntegrationTestConferenceCallControl0003::CCTSYIntegrationTestConferenceCallControl0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0003::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0003::~CCTSYIntegrationTestConferenceCallControl0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0003
 * @SYMFssID BA/CTSY/CONC-0003
 * @SYMTestCaseDesc Ensure that a data call cannot be part of a creation set
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::CreateConference, RMobileConferenceCall::GetConferenceStatus
 * @SYMTestExpectedResults Pass - Create conference fails.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify creating a conference fails.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get the test numbers
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	TPtrC dataNumber; 
	CHECK_TRUE_L(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber) != EFalse, _L("CTestStep::GetStringFromConfig did not complete as expected"));
	
	// Get the phone session
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Make sure we are on the network
	CHECK_EQUALS_L(iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("Failed to register on home network"));

	// Get conference call
    RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	
	// Get voice line 1. 
	RMobileLine& voiceLine1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	
	// Get data line 1. 
	RMobileLine& dataLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get call 2 with data line 1.
	RMobileCall& dataCall = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall2);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


	// Dial a number that answers on call 1.
	RMobileCall::TMobileCallStatus call1Status = MakeCallL(call1, number);
	ASSERT_EQUALS(call1Status, RMobileCall::EStatusConnected, _L("Failed to connect call"));

	// Hold call 1.
	call1Status = HoldCallL(call1);
	ASSERT_EQUALS(call1Status, RMobileCall::EStatusHold, _L("Failed to hold the call"));

	// Post notification for data call status changes
	TExtEtelRequestStatus notifyDataCallStatus(dataCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyDataCallStatus);
	RMobileCall::TMobileCallStatus dataCallStatus;	
	dataCall.NotifyMobileCallStatusChange(notifyDataCallStatus, dataCallStatus);

	// Dial a number that answers on call 2. 
	TCoreEtelRequestStatus<RMobileCall> dataDialStatus(dataCall, &RCall::DialCancel);
	CleanupStack::PushL(dataDialStatus);
	RMobileCall::TMobileDataCallParamsV1 dataCallParams;
	RMobileCall::TMobileDataCallParamsV1Pckg	dataCallParamsPkg(dataCallParams);
	dataCallParams.iService = RMobileCall::EServiceDataCircuitAsyncRdi;
	dataCallParams.iSpeed = RMobileCall::ESpeed9600;
	dataCallParams.iProtocol = RMobileCall::EProtocolV32;
	dataCallParams.iQoS = RMobileCall::EQoSNonTransparent;
	dataCallParams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion1;
	dataCallParams.iV42bisReq = RMobileCall::EV42bisBothDirections;

	// $CTSYKnownFailure defect id=50008. Data call cannot be made if an active or connecting call exists
	// CMmDataCallTsy::Dial returns -2020 (KErrEtelCallAlreadyActive)
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d </font>"), 50008);
	
	dataCall.Dial(dataDialStatus, dataCallParamsPkg, dataNumber); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dataDialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dataDialStatus.Int(), KErrNone, _L("RCall::Dial returned an error"));
	
	// === Ensure both calls are connected for the test to start. ===

	// Data call
	RMobileCall::TMobileCallStatus expectedDataCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall,
                                                             notifyDataCallStatus,
                                                             dataCallStatus,
                                                             expectedDataCallStatus,
                                                             KErrNone);
	
	// Voice call
	RCall::TStatus callStatus;
	ASSERT_EQUALS(call1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return EStatusConnected as expected"));
	
	
	// ===  Check conference caps, status and info are correct. ===

	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate | KCapsHangUp | KCapsSwap
	TUint32 caps;
	TUint unwantedCaps = RMobileConferenceCall::KCapsCreate | RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap;
	ASSERT_EQUALS(confCall.GetCaps(caps), KErrNone, _L("RMobileConferenceCall::GetCaps failed."));
	ASSERT_BITS_SET(caps, KNoWantedBits, unwantedCaps, _L("RMobileConferenceCall::GetCaps returned wrong mask"));

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 0
	TInt callCount;
	ASSERT_EQUALS(confCall.EnumerateCalls(callCount), KErrNone, _L("RMobileConferenceCall::EnumerateCalls failed."));
	ASSERT_EQUALS(callCount, 0, _L("RMobileConferenceCall::EnumerateCalls returned non-zero"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	RMobileConferenceCall::TMobileConferenceStatus wantedConfCallConfStatus = RMobileConferenceCall::EConferenceIdle;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, _L("RMobileConferenceCall::GetConferenceStatus failed"));
	ASSERT_EQUALS(confCallConfStatus, wantedConfCallConfStatus, _L("RMobileConferenceCall::GetConferenceStatus returned wrong status"));

	// Create the conference. 
	TExtEtelRequestStatus createConfStatus(confCall, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConfStatus);
	confCall.CreateConference(createConfStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createConfStatus, ETimeMedium), KErrNone, _L("RMobileConferenceCall::CreateConference timed out."));
	
	// Check this fails (error != KErrNone).
	ASSERT_TRUE(createConfStatus.Int() != KErrNone, _L("RMobileConferenceCall::CreateConference did not return error as expected"));
	
	// Hangup calls
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	

	dataCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	

	//
	// TEST END
	//

    StartCleanup();
    
    // Pop
    // notifyDataCallStatus
    // dataDialStatus
	// createConfStatus
 	// hangUpStatus
    CleanupStack::PopAndDestroy(4, &notifyDataCallStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0003");
	}



CCTSYIntegrationTestConferenceCallControl0004::CCTSYIntegrationTestConferenceCallControl0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0004::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0004::~CCTSYIntegrationTestConferenceCallControl0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0004
 * @SYMFssID BA/CTSY/CONC-0004
 * @SYMTestCaseDesc Create a conference call with one active call and one held call then hang up conference call.
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetMobileCallInfo, RMobileLine::NotifyMobileLineStatusChange, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::NotifyCapsChange, RMobileCall::NotifyMobileCallStatusChange, RCall::NotifyStatusChange, RMobileLine::GetMobileLineStatus, RLine::GetStatus, RMobileConferenceCall::GetCaps, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::HangUp, RCall::GetStatus
 * @SYMTestExpectedResults Pass - Conference call created.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 * Reason for test: Verify conference creation successful.
Verify conference call related notifications complete correctly and can be cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Get conference call subsession. 
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
		
	// Get voice line 1.
	RMobileLine& voiceLine1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1.
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Get call 2. 
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);

	//
	// SET UP END
	//	
	StartTest();


	//
	// TEST START
	//
	
	// ===  Check conference caps, status and info are correct. ===

	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate | KCapsHangUp | KCapsSwap

	TUint32 caps;
	TUint32 wantedCaps = KNoWantedBits;
	TUint32 unWantedCaps = RMobileConferenceCall::KCapsCreate |
					       RMobileConferenceCall::KCapsHangUp |
						   RMobileConferenceCall::KCapsSwap;
	ASSERT_EQUALS(confCall.GetCaps(caps), KErrNone, 
				_L("RMobileConferenceCall::GetCaps returned an error"));
	ASSERT_BITS_SET(caps, wantedCaps, unWantedCaps,
				_L("RMobileConferenceCall::GetCaps  returned the wrong caps NOT set"));

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 0
	TInt count;
	ASSERT_EQUALS(confCall.EnumerateCalls(count), KErrNone, 
				_L("RMobileConferenceCall::EnumerateCalls returned an error"));
	ASSERT_EQUALS(count,0, _L("RMobileConferenceCall::EnumerateCalls did not return aCount = 0"));				

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	ASSERT_EQUALS(confCall.GetConferenceStatus(confStatus), KErrNone,
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS(confStatus, RMobileConferenceCall::EConferenceIdle,
				_L("RMobileConferenceCall::GetConferenceStatus did not return status of EConferenceIdle"));					

	// post a notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus callStatus;
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
		
	// Dial a number that answers using call 1. 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, 
				_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
				_L("RCall::Dial timed-out"))
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned an error"))
	TEST_CHECK_POINT_L(_L("Failed to dial call1"));
	
	// ===  Check call status for call 1 and 2 ===
	// Check RCall::NotifyStatusChange for call 1 completes with EStatusDialling -> 
	RCall::TStatus expectedCallStatus = RCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);	
														 
	// Check RCall::NotifyStatusChange for call 1 completes with EStatusConnecting -> 														 
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	expectedCallStatus = RCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
													 	notifyStatusChangeStatus,
													 	callStatus,
													 	expectedCallStatus,
													 	KErrNone);
													 
	// Check RCall::NotifyStatusChange for call 1 completes with EStatusConnected.													 
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
													 	notifyStatusChangeStatus,
													 	callStatus,
													 	expectedCallStatus,
													 	KErrNone);														 
													 
	// Check RCall::GetStatus returns EStatusConnected.
	ASSERT_EQUALS(call1.GetStatus(callStatus),KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, 
				_L("RCall::GetStatus returned an unexpected call status"));

	// Hold call 1. 
	TExtEtelRequestStatus holdStatus (call1,EMobileCallHold);
	CleanupStack::PushL(holdStatus);	
	call1.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, 
				_L("RMobileCall::Hold timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,
				_L("RMobileCall::Hold returned an error"));

	// post a notifier for RCall::NotifyStatusChange on call2
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus2 (call2,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus2);
	RCall::TStatus callStatus2;
	call2.NotifyStatusChange(notifyStatusChangeStatus2,callStatus2);
	
	// post a notifier for RMobileConferenceCall::NotifyCapsChange
	TExtEtelRequestStatus notifyCapsChangeStatus (call1,EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);	
	confCall.NotifyCapsChange(notifyCapsChangeStatus, caps);
	
	// Dial a number that answers using call 2. 
	TPtrC number2; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, number2) != EFalse, 
				_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus2(call2, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus2);
	call2.Dial(dialStatus2, number2); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus2, ETimeMedium), KErrNone, 
				_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus2.Int(), KErrNone, _L("RCall::Dial returned an error"));
	TEST_CHECK_POINT_L(_L("Failed to dial call 2"));
	
	// Check RCall::NotifyStatusChange for call 2 completes with EStatusDialling ->  
	expectedCallStatus = RCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
														 notifyStatusChangeStatus2,
														 callStatus2,
														 expectedCallStatus,
														 KErrNone);	
														 
	// Check RCall::NotifyStatusChange for call 2 completes with EStatusConnecting ->
	call2.NotifyStatusChange(notifyStatusChangeStatus2,callStatus2);
	expectedCallStatus = RCall::EStatusConnecting;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
													 notifyStatusChangeStatus2,
													 callStatus2,
													 expectedCallStatus,
													 KErrNone);	
													 
	// Check RCall::NotifyStatusChange for call 2 completes with EStatusConnected.
	call2.NotifyStatusChange(notifyStatusChangeStatus2,callStatus2);
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
													 notifyStatusChangeStatus2,
													 callStatus2,
													 expectedCallStatus,
													 KErrNone);	
													 		
	// Check RCall::GetStatus returns EStatusConnected.
	ASSERT_EQUALS(call2.GetStatus(callStatus2),KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus2, RCall::EStatusConnected, 
				_L("RCall::GetStatus returned an unexpected call status"));


	// ===  Check conference caps indicate conference can now be created ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps in set of KCapsCreate
	wantedCaps = RMobileConferenceCall::KCapsCreate;
	unWantedCaps = KNoUnwantedBits;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   caps,
																	   wantedCaps,
																	   unWantedCaps,
																	   KErrNone);
																		   
	// Check RMobileConferenceCall::GetCaps returns caps in set of KCapsCreate
	ASSERT_EQUALS(confCall.GetCaps(caps), KErrNone,
				_L("RMobileConferenceCall::GetCaps returned an error"));
	wantedCaps = RMobileConferenceCall::KCapsCreate;
	unWantedCaps = KNoUnwantedBits;
	ASSERT_BITS_SET(caps, wantedCaps, unWantedCaps,
				_L("RMobileConferenceCall::GetGaps returned unexpected caps"));

	// Post a notifier for RMobileConferenceCall::NotifyCapsChange
	confCall.NotifyCapsChange(notifyCapsChangeStatus, caps);
	
	// Post a notifier for RMobileConferenceCall::NotifyConferenceEvent
	TExtEtelRequestStatus notifyConfEventStatus(confCall,
										EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);	
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confName);	
	
	// Post a notifier for RMobileConferenceCall::NotifyConferenceStatusChange
	TExtEtelRequestStatus notifyMobileConfStatusChangeStatus (confCall,
									EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyMobileConfStatusChangeStatus);
	RMobileConferenceCall::TMobileConferenceStatus mobileConfStatus;
	confCall.NotifyConferenceStatusChange(notifyMobileConfStatusChangeStatus,mobileConfStatus);							
													   		
	// Create a conference. 
	TExtEtelRequestStatus createConfStatus (confCall,EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConfStatus);	
	confCall.CreateConference(createConfStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createConfStatus, ETimeMedium), KErrNone, 
				_L("RMobileConferenceCall::CreateConference timed-out"));
	ASSERT_EQUALS(createConfStatus.Int(), KErrNone, 
				_L("RMobileConferenceCall::CreateConference returned an error"));	
	TEST_CHECK_POINT_L(_L("Failed to dial call"));

				
	// ===  Check conference caps indicate conference can now be created ===

	// Check RMobileConferenceCall::NotifyCapsChange completes with caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps NOT in set of KCapsCreate
	wantedCaps = RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap;
	unWantedCaps = RMobileConferenceCall::KCapsCreate;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   caps,
																	   wantedCaps,
																	   unWantedCaps,
																	   KErrNone);
																	   	
	// Check RMobileConferenceCall::GetCaps returns caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate
	wantedCaps = RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap;
	unWantedCaps = RMobileConferenceCall::KCapsCreate;	
	ASSERT_EQUALS(confCall.GetCaps(caps), KErrNone,
				_L("RMobileConferenceCall::GetCaps returned an error"));
	ASSERT_BITS_SET(caps, wantedCaps, unWantedCaps,
				_L("RMobileConferenceCall::GetGaps returned unexpected caps"));

					
	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceBuilt
	RMobileConferenceCall::TMobileConferenceEvent expectedEvent = RMobileConferenceCall::EConferenceBuilt;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall,
													 					notifyConfEventStatus,
													 					confEvent,
													 					expectedEvent,
													 					confName,
													 					KErrNone);
	
	ASSERT_EQUALS(confEvent, expectedEvent,	_L("RMobileConferenceCall::NotifyConferenceEvent returned unexpected event"));
													 						
	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus expectedStatus = RMobileConferenceCall::EConferenceActive;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall,
													 					notifyMobileConfStatusChangeStatus,
													 					mobileConfStatus,
													 					expectedStatus,
													 					KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	ASSERT_EQUALS(confCall.GetConferenceStatus(confStatus), KErrNone,
					_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS(confStatus, RMobileConferenceCall::EConferenceActive,
					_L("RMobileConferenceCall::GetConferenceStatus status was not EConferenceActive"));				


	// ===  Get call info belonging to individual call in the conference ===
	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 2
	ASSERT_EQUALS(confCall.EnumerateCalls(count), KErrNone,
					_L("RMobileConferenceCall::EnumerateCalls returned an error"));
	ASSERT_EQUALS(count, 2, _L("RMobileConferenceCall::EnumerateCalls did not return aCount = 2"));						
	
	// Check RMobileConferenceCall::GetMobileCallInfo with aIndex = 0 returns KErrNone
	TInt index = 0;
	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	ASSERT_EQUALS(confCall.GetMobileCallInfo(index,callInfoPckg), KErrNone,
					_L("RMobileConferenceCall::GetMobileCallInfo returned an error"));
					
	//   valid iCallName 
	ASSERT_TRUE(callInfo.iCallName.Length() > 0, 
					_L("RMobileConferenceCall::GetMobileCallInfo did not return valid iCallName"));
					
	//   correct number in iDialledParty
	ASSERT_EQUALS_DES16(callInfo.iDialledParty.iTelNumber, number,
			_L("RMobileConferenceCall::GetMobileCallInfo did not return correct number in iDialledParty"));
					
	//   valid iLineName 
	ASSERT_TRUE(callInfo.iLineName.Length() > 0, 
					_L("RMobileConferenceCall::GetMobileCallInfo did not return valid iLineName"));

	//   iStatus of EStatusConnected 
	ASSERT_EQUALS(callInfo.iStatus, RMobileCall::EStatusConnected,
					_L("RMobileConferenceCall::GetMobileCallInfo did not return iStatus of EStatusConnected"));	

	//   relevant bits set in iValid based on the above parameters 
	TUint32 wantedBits = RMobileCall::KCallDialledParty | RMobileCall::KCallId;
	TUint32 unWantedBits = KNoUnwantedBits;						
	ASSERT_BITS_SET(callInfo.iValid, wantedBits, unWantedBits,
					_L("RMobileConferenceCall::GetMobileCallInfo did not return relevant bits set in iValid"));	

	// Check RMobileConferenceCall::GetMobileCallInfo with aIndex = 1 returns
	index = 1;
	ASSERT_EQUALS(confCall.GetMobileCallInfo(index,callInfoPckg), KErrNone,
					_L("RMobileConferenceCall::GetMobileCallInfo with aIndex = 1 returned an error"));

	//   valid iCallName 
	ASSERT_TRUE(callInfo.iCallName.Length() > 0, 
					_L("RMobileConferenceCall::GetMobileCallInfo did not return valid iCallName"));

	//   correct number in iDialledParty 
	ASSERT_EQUALS_DES16(callInfo.iDialledParty.iTelNumber, number2,
					_L("RMobileConferenceCall::GetMobileCallInfo did not return correct number in iDialledParty"));

	//   valid iLineName 
	ASSERT_TRUE(callInfo.iLineName.Length() > 0, 
					_L("RMobileConferenceCall::GetMobileCallInfo did not return valid iLineName"));

	//   iStatus of EStatusConnected 
	ASSERT_EQUALS(callInfo.iStatus, RMobileCall::EStatusConnected,
					_L("RMobileConferenceCall::GetMobileCallInfo did not return iStatus of EStatusConnected"));	

	//   relevant bits set in iValid based on the above parameters 
	wantedBits = RMobileCall::KCallDialledParty | RMobileCall::KCallId;
	unWantedBits = KNoUnwantedBits;						
	ASSERT_BITS_SET(callInfo.iValid, wantedBits, unWantedBits,
					_L("RMobileConferenceCall::GetMobileCallInfo did not return relevant bits set in iValid"));	

	// Post Notifier for RMobileConferenceCall::NotifyCapsChange
	confCall.NotifyCapsChange(notifyCapsChangeStatus, caps);

	// Post Notifier for RMobileConferenceCall::NotifyConferenceEvent	
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confName);	

	// Post a notifier for RMobileConferenceCall::NotifyConferenceStatusChange
	confCall.NotifyConferenceStatusChange(notifyMobileConfStatusChangeStatus,mobileConfStatus);		

	// post a notifier for RCall::NotifyStatusChange for call 1
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange for call1
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus (call1,EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Post a notifier for RCall::NotifyStatusChange for call2
	call2.NotifyStatusChange(notifyStatusChangeStatus2,callStatus2);

	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange for call2
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus2 (call2,EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus2);
	RMobileCall::TMobileCallStatus mobileCallStatus2;	
	call2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);

	// Post a notifier for RLine::NotifyStatusChange on line1
	TCoreEtelRequestStatus<RLine> notifyLineStatusChange (voiceLine1,&RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyLineStatusChange);
	RCall::TStatus lineStatus;
	voiceLine1.NotifyStatusChange(notifyLineStatusChange,lineStatus);	
	
	// Post a notifier for RMobileLine::NotifyMobileLineStatusChange for line 1
	TExtEtelRequestStatus notifyMobileLineStatusChangeStatus (voiceLine1,EMobileLineNotifyMobileLineStatusChange);
	CleanupStack::PushL(notifyMobileLineStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileLineStatus;	
	voiceLine1.NotifyMobileLineStatusChange(notifyMobileLineStatusChangeStatus, mobileLineStatus);
				
	// Hang up conference call with RMobileConferenceCall::HangUp
	TExtEtelRequestStatus hangUpStatus (confCall,EMobileConferenceCallHangUp);
	CleanupStack::PushL(hangUpStatus);		
	confCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, 
				_L("RMobileConferenceCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, 
				_L("RMobileConferenceCall::HangUp returned an error"));
	
	// ===  Check conference caps ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	wantedCaps = 0;
	unWantedCaps = KNoUnwantedBits;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   caps,
																	   wantedCaps,
																	   unWantedCaps,
																	   KErrNone);
																	   
	// Check RMobileConferenceCall::GetCaps returns caps of 0
	ASSERT_EQUALS(confCall.GetCaps(caps), KErrNone,
				_L("RMobileConferenceCall::GetCaps returned an error"));
	wantedCaps = 0;
	unWantedCaps = KNoUnwantedBits;
	ASSERT_BITS_SET(caps, wantedCaps, unWantedCaps,
				_L("RMobileConferenceCall::GetGaps returned unexpected caps"));


	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceTerminated
	// $CTSYKnownFailure defect id=10018  this notification is not delivered (times out)	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d </font>"), 10018);
/*
	expectedEvent = RMobileConferenceCall::EConferenceTerminated;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall,
													 					notifyConfEventStatus,
													 					confEvent,
													 					expectedEvent,
													 					confName,
													 					KErrNone);

*/
	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle

	
	expectedStatus = RMobileConferenceCall::EConferenceIdle;
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall,
													 					notifyMobileConfStatusChangeStatus,
													 					mobileConfStatus,
													 					expectedStatus,
													 					KErrNone);
													 					
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	ASSERT_EQUALS(confCall.GetConferenceStatus(confStatus), KErrNone,
					_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS(confStatus, RMobileConferenceCall::EConferenceIdle,
					_L("RMobileConferenceCall::GetConferenceStatus status was not EConferenceActive"));				

	
	// ===  Check individual calls in conference terminated ===

	// ===  Check call status ===
	// Check RCall::NotifyStatusChange for call 1 completes with EStatusHangingUp -> 
	expectedCallStatus = RCall::EStatusHangingUp;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);	

	// Check RCall::NotifyStatusChange for call 1 completes with EStatusIdle.
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
													 	notifyStatusChangeStatus,
													 	callStatus,
													 	expectedCallStatus,
													 	KErrNone);
													 	
	// Check RCall::GetStatus for call 1 returns EStatusIdle.
	ASSERT_EQUALS(call1.GetStatus(callStatus),KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusIdle, 
				_L("RCall::GetStatus returned an unexpected call status"));
				
	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with EStatusDisconnecting -> 
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
													 	notifyMobileCallStatusChangeStatus,
													 	mobileCallStatus,
													 	expectedMobileCallStatus,
													 	KErrNone);

	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with  EStatusIdle.
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
													 	notifyMobileCallStatusChangeStatus,
													 	mobileCallStatus,
													 	expectedMobileCallStatus,
													 	KErrNone);	
														 	
	// Check RMobileCall::GetMobileCallStatus for call 1 returns status of EStatusIdle.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusIdle, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));				
	
	// Check RCall::NotifyStatusChange for call 2 completes with EStatusHangingUp ->
	expectedCallStatus = RCall::EStatusHangingUp;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
														 notifyStatusChangeStatus2,
														 callStatus2,
														 expectedCallStatus,
														 KErrNone);	

	// Check RCall::NotifyStatusChange for call 2 completes with  EStatusIdle.
	call2.NotifyStatusChange(notifyStatusChangeStatus2,callStatus2);
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
													 notifyStatusChangeStatus2,
													 callStatus2,
													 expectedCallStatus,
													 KErrNone);	
													 															 
										 
	// Check RMobileCall::NotifyMobileCallStatusChange for call 2 completes with EStatusDisconnecting -> 
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
													 	notifyMobileCallStatusChangeStatus2,
													 	mobileCallStatus2,
													 	expectedMobileCallStatus,
													 	KErrNone);

	// Check RMobileCall::NotifyMobileCallStatusChange for call 2 completes with  EStatusIdle.
	call2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
													 	notifyMobileCallStatusChangeStatus2,
													 	mobileCallStatus2,
													 	expectedMobileCallStatus,
													 	KErrNone);	
													 	
	// Check RMobileCall::GetMobileCallStatus for call 2 returns status of EStatusIdle.
	ASSERT_EQUALS(call2.GetMobileCallStatus(mobileCallStatus2), KErrNone,
				_L("RMobileCall::GetMobileCallStatus on call 2 returned an error"));
	ASSERT_EQUALS(mobileCallStatus2, RMobileCall::EStatusIdle, 
				_L("RMobileCall::GetMobileCallStatus on call 2 returned an unexpected call status"));	
				
	// ===  Check line status ===
	// Check RLine::NotifyStatusChange for line 1 completes with EStatusHangingUp -> 
	RCall::TStatus expectedLineStatus = RCall::EStatusHangingUp;
	iCallControlTsyTestHelper.WaitForLineNotifyStatusChange(voiceLine1,
														 notifyLineStatusChange,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);	
														 
	// Check RLine::NotifyStatusChange for line 1 completes with EStatusIdle
	voiceLine1.NotifyStatusChange(notifyLineStatusChange,lineStatus);
	expectedLineStatus = RCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForLineNotifyStatusChange(voiceLine1,
														 notifyLineStatusChange,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);	
														 																 
	// Check RLine::GetStatus for line 1 returns EStatusIdle
	ASSERT_EQUALS(voiceLine1.GetStatus(lineStatus),KErrNone, 
				_L("RLine::GetStatus for line 1 returned an error"));
	ASSERT_EQUALS(lineStatus, RCall::EStatusIdle, 
				_L("RLine::GetStatus returned an unexpected call status"));
				
	// Check RMobileLine::NotifyMobileLineStatusChange for line 1 completes with EStatusDisconnecting -> 
	RMobileCall::TMobileCallStatus expectedMobileLineStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTsyTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine1,
													 	notifyMobileLineStatusChangeStatus,
													 	mobileLineStatus,
													 	expectedMobileLineStatus,
													 	KErrNone);

	// Check RMobileLine::NotifyMobileLineStatusChange for line 1 completes with EStatusIdle 
	voiceLine1.NotifyMobileLineStatusChange(notifyMobileLineStatusChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine1,
													 	notifyMobileLineStatusChangeStatus,
													 	mobileLineStatus,
													 	expectedMobileLineStatus,
													 	KErrNone);
													 																 	
	// Check RMobileLine::GetMobileLineStatus for line 1 returns EStatusIdle
	ASSERT_EQUALS(voiceLine1.GetMobileLineStatus(mobileLineStatus), KErrNone,
				_L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileLineStatus, RMobileCall::EStatusIdle, 
				_L("RMobileLine::GetMobileLineStatus returned an unexpected call status"));	
				

	//
	// TEST END
	//

    StartCleanup();
    
    // Hangup calls
    call1.HangUp();
    call2.HangUp();
	
	/*	notifyStatusChangeStatus
	    dialStatus
	    holdStatus
	    notifyStatusChangeStatus2
	    notifyCapsChangeStatus
	    dialStatus2
	    notifyConfEventStatus
	    notifyMobileConfStatusChangeStatus
	    createConfStatus
	    notifyMobileCallStatusChangeStatus
	    notifyMobileCallStatusChangeStatus2
	    notifyLineStatusChange
	    notifyMobileLineStatusChangeStatus
	    hangUpStatus */
	CleanupStack::PopAndDestroy(14, &notifyStatusChangeStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0004");
	}



CCTSYIntegrationTestConferenceCallControl0005::CCTSYIntegrationTestConferenceCallControl0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0005::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0005::~CCTSYIntegrationTestConferenceCallControl0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0005
 * @SYMFssID BA/CTSY/CONC-0005
 * @SYMTestCaseDesc Swap a conference call with an active voice call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileConferenceCall::NotifyConferenceEvent, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::Swap, RMobileConferenceCall::GetConferenceStatus
 * @SYMTestExpectedResults Pass - Conference call swapped.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference creation successful.
Verify conference call notifications complete with correct values.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Create a three party conference call. (Two remote parties) 

	// Get voice line 3. 

	// Get call 3. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Swap the conference call with RMobileConferenceCall::Swap 

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped

	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold

	// Dial a number that answers on call 3. 

	// Check RMobileCall::NotifyMobileCallStatusChange for call 3 completes with EStatusConnected

	// Check RMobileCall::GetMobileCallStatus for call 3 returns status of EStatusConnected

	// Swap the conference call with RMobileConferenceCall::Swap 

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped

	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive

	// ===  Check call 3 on hold ===

	// Check RMobileCall::NotifyMobileCallStatusChange for call 3 completes with EStatusHold

	// Check RMobileCall::GetMobileCallStatus for call 3 returns status of EStatusHold

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0005");
	}



CCTSYIntegrationTestConferenceCallControl0006::CCTSYIntegrationTestConferenceCallControl0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0006::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0006::~CCTSYIntegrationTestConferenceCallControl0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0006
 * @SYMFssID BA/CTSY/CONC-0006
 * @SYMTestCaseDesc Swap a conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceEvent, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetCaps, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::Swap, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::NotifyCapsChange, RMobileConferenceCall::HangUp
 * @SYMTestExpectedResults Pass - Conference call swapped with an active call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call can be swapped from active to hold and vice versa.
Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	TPtrC num1, num2, num3;

	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call (two remote parties).
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post a notifier for RMobileConferenceCall::NotifyConferenceEvent 
	TExtEtelRequestStatus notifyConfEventStatus(confCall, EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL( notifyConfEventStatus );		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// Post a notifier for RMobileConferenceCall::NotifyConferenceStatusChange
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange for call1
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus (call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange for call2
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus2 (call2, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus2);
	RMobileCall::TMobileCallStatus mobileCallStatus2;	
	call2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);

	// Post a notifier for RMobileCall::NotifyCapsChange
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	TUint32 confCallCaps;
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	// Swap the conference call with RMobileConferenceCall::Swap 
	TExtEtelRequestStatus reqSwapConfStatus(confCall, EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);
	
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus, ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall, 
								notifyConfEventStatus,
								confEvent,
								RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);	

	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
							_L("RMobileConferenceCall::NotifyConferenceEvent returned unexpected event"));
							
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));

	// Repost NotifyConferenceEvent
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall, 
							notifyConfStatusChange,
							confStatus,
							RMobileConferenceCall::EConferenceHold,
							KErrNone);
	
	ASSERT_EQUALS( confStatus, RMobileConferenceCall::EConferenceHold, 
						_L("RMobileConferenceCall::NotifyConferenceStatusChange returned wrong status"));
	
	// repost NotifyConferenceStatusChange
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus);
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceHold,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceHold"));
	
	TEST_CHECK_POINT_L(_L("Conference status incorrect"));
	
	// ===  Check individual calls in conference go on hold ===

	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with EStatusHold
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
													 	notifyMobileCallStatusChangeStatus,
													 	mobileCallStatus,
													 	RMobileCall::EStatusHold,
													 	KErrNone);
	
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusHold, 
			_L("RMobileConferenceCall::NotifyMobileCallStatusChange returned unexpected event"));
	
	// repost call1 NotifyMobileCallStatusChange
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);

	// Check RMobileCall::GetMobileCallStatus for call 1 returns status of EStatusHold
	RMobileCall::TMobileCallStatus getMobileCallStatus;
	ASSERT_EQUALS(call1.GetMobileCallStatus(getMobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusHold, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));				

	// Check RMobileCall::NotifyMobileCallStatusChange for call 2 completes with EStatusHold
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
													 	notifyMobileCallStatusChangeStatus2,
													 	mobileCallStatus2,
													 	RMobileCall::EStatusHold,
													 	KErrNone);
	ASSERT_EQUALS( mobileCallStatus2, RMobileCall::EStatusHold, 
			_L("RMobileConferenceCall::NotifyMobileCallStatusChange returned unexpected event"));

	// repost call2 NotifyMobileCallStatusChange
	call2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);
	
	// Check RMobileCall::GetMobileCallStatus for call 2 returns status of EStatusHold
	ASSERT_EQUALS(call2.GetMobileCallStatus(getMobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusHold, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));				

	// Swap the conference call with RMobileConferenceCall::Swap
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus, ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall, 
								notifyConfEventStatus,
								confEvent,
								RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);	
	
	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
			_L("RMobileConferenceCall::NotifyConferenceEvent returned unexpected event"));
							
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	
	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall, 
										notifyConfStatusChange,
										confStatus,
										RMobileConferenceCall::EConferenceActive,
										KErrNone);

	ASSERT_EQUALS( confStatus, RMobileConferenceCall::EConferenceActive, 
			_L("RMobileConferenceCall::NotifyConferenceStatusChange returned unexpected event"));
	
	// repost NotifyConferenceStatusChange
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus);
		
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// ===  Check individual calls in conference go on ===

	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with EStatusConnected
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
													 	notifyMobileCallStatusChangeStatus,
													 	mobileCallStatus,
													 	RMobileCall::EStatusConnected,
													 	KErrNone);
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusConnected, 
			_L("RMobileConferenceCall::NotifyMobileCallStatusChange returned unexpected status"));
	
	// Check RMobileCall::GetMobileCallStatus for call 1 returns status of EStatusConnected
	ASSERT_EQUALS(call1.GetMobileCallStatus(getMobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));				

	// Check RMobileCall::NotifyMobileCallStatusChange for call 2 completes with EStatusConnected
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
													 	notifyMobileCallStatusChangeStatus2,
													 	mobileCallStatus2,
													 	RMobileCall::EStatusConnected,
													 	KErrNone);	
	ASSERT_EQUALS( mobileCallStatus2, RMobileCall::EStatusConnected, 
			_L("RMobileConferenceCall::NotifyMobileCallStatusChange returned unexpected status"));

	// Check RMobileCall::GetMobileCallStatus for call 2 returns status of EStatusConnected
	ASSERT_EQUALS(call2.GetMobileCallStatus(getMobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));				

	// repost
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	
	// Hang up conference call with RMobileConferenceCall::HangUp 
	TExtEtelRequestStatus hangUpStatus (confCall,EMobileConferenceCallHangUp);
	CleanupStack::PushL(hangUpStatus);
	confCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, 
				_L("RMobileConferenceCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, 
				_L("RMobileConferenceCall::HangUp returned an error"));
	
	// ===  Check conference caps ===

	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																notifyCapsChangeStatus,
																confCallCaps,
																KNoWantedBits,
																KNoWantedBits,
																KErrNone);
	
	// Check RMobileConferenceCall::GetCaps returns caps of 0
	TUint32 caps; 
	TInt result = confCall.GetCaps(caps);
	ASSERT_EQUALS(result, KErrNone, _L("RMobileConferenceCall::GetCaps failed."));
	ASSERT_BITS_SET(caps,  KNoWantedBits, KNoWantedBits, _L("RMobileConferenceCall::GetCaps returned wrong mask") );

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceTerminated
	// $CTSYKnownFailure defect id=10018  this notification is not delivered (times out)
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10018);	
/*	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall, 
								notifyConfEventStatus,
								confEvent,
								RMobileConferenceCall::EConferenceTerminated,
								confEventCallName,
								KErrNone);
	
	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceTerminated, 
			_L("RMobileConferenceCall::NotifyConferenceEvent returned unexpected event"));
*/
	
	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall, 
							notifyConfStatusChange,
							confStatus,
							RMobileConferenceCall::EConferenceIdle,
							KErrNone);

	ASSERT_EQUALS( confStatus, RMobileConferenceCall::EConferenceIdle, 
			_L("RMobileConferenceCall::NotifyConferenceStatusChange returned wrong status"));
	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceIdle,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceIdle"));
	
	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();
	
	CleanupStack::PopAndDestroy(&hangUpStatus);
	CleanupStack::PopAndDestroy(&reqSwapConfStatus);
	CleanupStack::PopAndDestroy(&notifyCapsChangeStatus);
	CleanupStack::PopAndDestroy(&notifyMobileCallStatusChangeStatus2);
	CleanupStack::PopAndDestroy(&notifyMobileCallStatusChangeStatus);
	CleanupStack::PopAndDestroy(&notifyConfStatusChange);
	CleanupStack::PopAndDestroy(&notifyConfEventStatus);
	CleanupStack::PopAndDestroy(&reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0006");
	}



CCTSYIntegrationTestConferenceCallControl0007::CCTSYIntegrationTestConferenceCallControl0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0007::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0007::~CCTSYIntegrationTestConferenceCallControl0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0007
 * @SYMFssID BA/CTSY/CONC-0007
 * @SYMTestCaseDesc Hang up a two party conference call after one remote party has already hung up.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceEvent, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetCaps, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::NotifyCapsChange
 * @SYMTestExpectedResults Pass - KErrNotReady on attempt to hang up conference call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify conference call related notifications complete correctly.
Verify conference call cannot be hung up when there is an active call in the conference
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Need to call a number which is answered and can be hung up
	// For this manual test the user must set up a number they can answer and hang up on number 2
	_LIT(KPromptCheckNum3,"Check that the INI file has a \"RPS\" number you can answer on VoiceNumber2");
	DisplayUserInteractionPromptL(KPromptCheckNum3);	

	TPtrC num1, num2;

	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call. (Two remote parties where RPS is one of the remote parties.) 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for RMobileCall::NotifyCapsChange
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	TUint32 confCallCaps;
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	// Post notification for RMobileCall::NotifyConferenceEvent
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent( notifyConfEventStatus, confEvent, confEventCallName );
	
	// Request notification for RMobileConferenceCall::NotifyConferenceStatusChange 
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// RPS hangs up.
	
	_LIT(KPromptForHangup,"Please hangup \"RPS\" call (call 2)");
	DisplayUserInteractionPromptL(KPromptForHangup);	
	
	// ===  Conference should have terminated since there are no longer three parties ===

	// ===  Check conference caps ===

	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   confCallCaps,
																	   0,
																	   KNoUnwantedBits,
																	   KErrNone);

	// Check RMobileConferenceCall::GetCaps returns caps of 0
	TUint32 caps; 
	TInt result = confCall.GetCaps(caps);
	ASSERT_EQUALS(result, KErrNone, _L("RMobileConferenceCall::GetCaps failed."));
	ASSERT_BITS_SET( caps,  KNoUnwantedBits, KNoUnwantedBits, _L("RMobileConferenceCall::GetCaps returned wrong mask") );

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceTerminated
	// $CTSYKnownFailure defect id=10018  this notification is not delivered (times out)
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10018);	
/*	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall, 
								notifyConfEventStatus,
								confEvent,
								RMobileConferenceCall::EConferenceTerminated,
								confEventCallName,
								KErrNone);
*/
	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange(confCall, 
								notifyConfStatusChange,
								confStatus,
								RMobileConferenceCall::EConferenceIdle,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceIdle,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceIdle"));

	// ===  Hang up conference when it has already been terminated ===

	// Hang up conference.  
	TExtEtelRequestStatus hangUpStatus (confCall,EMobileConferenceCallHangUp);
	CleanupStack::PushL(hangUpStatus);
	confCall.HangUp(hangUpStatus);
	
	// Check hang up returns KErrNotReady
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, 
				_L("RMobileConferenceCall::HangUp timed-out"));
	
	ASSERT_EQUALS(hangUpStatus.Int(), KErrAccessDenied, 
				_L("RMobileConferenceCall::HangUp returned an unexpected error"));
	
	//
	// TEST END
	//

    StartCleanup();

	// Hang up calls
	call1.HangUp();
	call2.HangUp();
	
	CleanupStack::PopAndDestroy(&hangUpStatus);
	CleanupStack::PopAndDestroy(&notifyConfStatusChange);
	CleanupStack::PopAndDestroy(&notifyCapsChangeStatus);
	CleanupStack::PopAndDestroy(&reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0007");
	}



CCTSYIntegrationTestConferenceCallControl0008::CCTSYIntegrationTestConferenceCallControl0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0008::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0008::~CCTSYIntegrationTestConferenceCallControl0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0008
 * @SYMFssID BA/CTSY/CONC-0008
 * @SYMTestCaseDesc Hang up a conference call after the calls in the conference are hung up.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RCall::NotifyStatusChange, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetCaps, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::GetConferenceStatus, RCall::GetStatus
 * @SYMTestExpectedResults Pass - Calls in conference terminate.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call hangs up. 
Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	TPtrC num1, num2;

	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call. (Two remote parties where RPS is one of the remote parties.) 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post a notifier for RCall::NotifyStatusChange for call 1
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus notifyStatusChangeCallStatus;
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyStatusChangeCallStatus);
	
	// Post a notifier for RCall::NotifyStatusChange for call 2
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus2 (call2, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus2);
	RCall::TStatus notifyStatusChangeCallStatus2;
	call2.NotifyStatusChange(notifyStatusChangeStatus2, notifyStatusChangeCallStatus2);
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange for call 1
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus (call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange for call 2
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus2 (call2, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus2);
	RMobileCall::TMobileCallStatus mobileCallStatus2;	
	call2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);
		
	// Hang up call 1 in the conference.
	ASSERT_EQUALS(call1.HangUp(), KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange for call 1 completes with EStatusHangingUp -> EStatusIdle.
	RCall::TStatus expectedCallStatus = RCall::EStatusHangingUp;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyStatusChangeCallStatus,
														 expectedCallStatus,
														 KErrNone);	
														 								 
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyStatusChangeCallStatus);
	
	expectedCallStatus = RCall::EStatusIdle;
	
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyStatusChangeCallStatus,
														 expectedCallStatus,
														 KErrNone);	
														 								 
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyStatusChangeCallStatus);

	// Check RCall::GetStatus for call 1 returns EStatusIdle.
	RCall::TStatus getStatus;
	ASSERT_EQUALS(call1.GetStatus(getStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(getStatus, RCall::EStatusIdle, 
				_L("RCall::GetStatus returned an unexpected call status"));

	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with EStatusDisconnecting -> EStatusIdle.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
													 	notifyMobileCallStatusChangeStatus,
													 	mobileCallStatus,
													 	expectedMobileCallStatus,
													 	KErrNone);

	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
													 	notifyMobileCallStatusChangeStatus,
													 	mobileCallStatus,
													 	expectedMobileCallStatus,
													 	KErrNone);

	// Check RMobileCall::GetMobileCallStatus for call 1 returns status of EStatusIdle.
	RMobileCall::TMobileCallStatus getMobileCallStatus;
	ASSERT_EQUALS(call1.GetMobileCallStatus(getMobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusIdle, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));				
	
	// Hang up call 2 in the conference.
	ASSERT_TRUE(call2.HangUp()==KErrNone, _L("RMobileCall::HangUp return value not KErrNone"));

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange for call 2 completes with EStatusHangingUp -> EStatusIdle.
	expectedCallStatus = RCall::EStatusHangingUp;
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
														 notifyStatusChangeStatus2,
														 notifyStatusChangeCallStatus2,
														 expectedCallStatus,
														 KErrNone);	
														 								 
	call2.NotifyStatusChange(notifyStatusChangeStatus2, notifyStatusChangeCallStatus2);
	
	expectedCallStatus = RCall::EStatusIdle;
	
	iCallControlTsyTestHelper.WaitForCallNotifyStatusChange(call2,
														 notifyStatusChangeStatus2,
														 notifyStatusChangeCallStatus2,
														 expectedCallStatus,
														 KErrNone);	
														 								 
	call2.NotifyStatusChange(notifyStatusChangeStatus2, notifyStatusChangeCallStatus2);

	// Check RCall::GetStatus for call 2 returns EStatusIdle.
	ASSERT_EQUALS(call2.GetStatus(getStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(getStatus, RCall::EStatusIdle, 
				_L("RCall::GetStatus returned an unexpected call status"));

	// Check RMobileCall::NotifyMobileCallStatusChange for call 2 completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
													 	notifyMobileCallStatusChangeStatus2,
													 	mobileCallStatus2,
													 	expectedMobileCallStatus,
													 	KErrNone);

	call2.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus2, mobileCallStatus2);
	
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
													 	notifyMobileCallStatusChangeStatus2,
													 	mobileCallStatus2,
													 	expectedMobileCallStatus,
													 	KErrNone);

	// Check RMobileCall::GetMobileCallStatus for call 2 returns status of EStatusIdle.
	ASSERT_EQUALS(call2.GetMobileCallStatus(getMobileCallStatus), KErrNone,
				_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusIdle, 
				_L("RMobileCall::GetMobileCallStatus returned an unexpected call status"));
	
	// ===  Conference should have terminated ===

	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate | KCapsHangUp | KCapsSwap
	TUint32 caps; 
	TInt result = confCall.GetCaps(caps);
	ASSERT_EQUALS(result, KErrNone, _L("RMobileConferenceCall::GetCaps failed."))
	TUint32 unwantedbits = RMobileConferenceCall::KCapsCreate | RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap;
	ASSERT_BITS_SET( caps,  KNoUnwantedBits, unwantedbits, _L("RMobileConferenceCall::GetCaps returned wrong mask") )
	
	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 0
	TInt callCount;
	ASSERT_EQUALS( confCall.EnumerateCalls(callCount), KErrNone, 
						_L("RMobileConferenceCall::EnumerateCalls failed."));
	ASSERT_EQUALS(callCount, 0, _L("RMobileConferenceCall::EnumerateCalls returned non-zero"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceIdle,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceIdle"));
	
	//
	// TEST END
	//

    StartCleanup();
        
    //notifyMobileCallStatusChangeStatus2
    //notifyMobileCallStatusChangeStatus
    //notifyStatusChangeStatus2
    //notifyStatusChangeStatus
    //reqCreConfStatus	
	CleanupStack::PopAndDestroy(5, &reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0008");
	}



CCTSYIntegrationTestConferenceCallControl0009::CCTSYIntegrationTestConferenceCallControl0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0009::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0009::~CCTSYIntegrationTestConferenceCallControl0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0009
 * @SYMFssID BA/CTSY/CONC-0009
 * @SYMTestCaseDesc Hold second call in conference in two call conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::Hold
 * @SYMTestExpectedResults Pass - KErrNotReady on attempt to hold.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned and existing conference remains active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	TPtrC num1, num2;

	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Hold call 2 in the conference call.
	TExtEtelRequestStatus reqHoldStatus(call2,EMobileCallHold);
	CleanupStack::PushL(reqHoldStatus);

	call2.Hold(reqHoldStatus);
		
	// Check KErrNotReady is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqHoldStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out"));
	ASSERT_EQUALS(reqHoldStatus.Int(), KErrNotReady, _L("RMobileCall::Hold returned an error") )
		
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceIdle"));
				
	//
	// TEST END
	//

    StartCleanup();
	
	TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);
    
    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    	
	// reqHangupStatus
	// reqHoldStatus
	CleanupStack::PopAndDestroy(3, &reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0009");
	}

CCTSYIntegrationTestConferenceCallControl0010::CCTSYIntegrationTestConferenceCallControl0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0010::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0010::~CCTSYIntegrationTestConferenceCallControl0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0010
 * @SYMFssID BA/CTSY/CONC-0010
 * @SYMTestCaseDesc Dial another call when there is an active conference call. Conference call on automatic hold.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RCall::NotifyStatusChange, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetCaps, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::NotifyCapsChange, RMobileConferenceCall::HangUp, RCall::Dial
 * @SYMTestExpectedResults Pass - Third call dials successfully. Conference call goes on automatic hold.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify third call can be made.
Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	TPtrC num1, num2, num3;

	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Get voice line 3.
	
	// Get call 3.
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// post a notifier for RMobileConferenceCall::NotifyConferenceStatusChange
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// post a notifier for RCall::NotifyMobileCallStatusChange for call 1
	TExtEtelRequestStatus notifyCallStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatus);
	RMobileCall::TMobileCallStatus notifyMobileCallStatusChangeStatus;
	call1.NotifyMobileCallStatusChange( notifyCallStatus, notifyMobileCallStatusChangeStatus );
	
	// post a notifier for RCall::NotifyMobileCallStatusChange for call 2
	TExtEtelRequestStatus notifyCallStatus2(call2, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatus2);
	RMobileCall::TMobileCallStatus notifyMobileCallStatusChangeStatus2;
	call2.NotifyMobileCallStatusChange( notifyCallStatus2, notifyMobileCallStatusChangeStatus2 );
		
	// Dial a number that answers on call 3.
	callStatus = MakeCallL(call3, num3);
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusConnected,_L("Failed to connect call3"));
	
	// ===  Check conference goes on automatic hold ===

	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,
								RMobileConferenceCall::EConferenceHold,
								KErrNone);
								
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceHold,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceHold"));
	
	// ===  Check individual calls in conference go on hold ===

	// Check RMobileCall::NotifyMobileCallStatusChange for call 1 completes with EStatusHold
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyCallStatus, 
														notifyMobileCallStatusChangeStatus,
														RMobileCall::EStatusHold, 
														KErrNone );

	// Check RMobileCall::GetMobileCallStatus for call 1 returns status of EStatusHold
	err = call1.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(err, KErrNone,_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusHold, _L("RMobileCall::GetMobileCallStatus returned wrong status"))

	// Check RMobileCall::NotifyMobileCallStatusChange for call 2 completes with EStatusHold
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call2, 
														notifyCallStatus2, 
														notifyMobileCallStatusChangeStatus2,
														RMobileCall::EStatusHold, 
														KErrNone );
	
	// Check RMobileCall::GetMobileCallStatus for call 2 returns status of EStatusHold
	err = call2.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(err, KErrNone,_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusHold, _L("RMobileCall::GetMobileCallStatus returned wrong status"))
	
	// Request notification for RMobileConferenceCall::NotifyCapsChange 
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	TUint32 confCallCaps;
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);	
	
	// Request notification for RMobileConferenceCall::NotifyConferenceEvent
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);

	// Hang up conference call.
	TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);

	// ===  Check conference caps ===

	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																   notifyCapsChangeStatus,
																   confCallCaps,
																   0,
																   KNoUnwantedBits,
																   KErrNone);

	// Check RMobileConferenceCall::GetCaps returns caps of 0
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS(confCallCaps, (TUint32)0, _L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceTerminated
	// $CTSYKnownFailure defect id=10018  this notification is not delivered (times out)
/*	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10018);	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceTerminated,
								confEventCallName,
								KErrNone);
*/
	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,
								RMobileConferenceCall::EConferenceIdle,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"))
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceIdle,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));
	
	//
	// TEST END
	//

    // Hangup for safety
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
		
    StartCleanup();
	
    // reqHangupStatus
    // notifyConfEventStatus
    // notifyCapsChangeStatus
    // notifyCallStatus2
    // notifyCallStatus
    // notifyConfStatusChange
    // reqCreConfStatus
    CleanupStack::PopAndDestroy( 7, &reqCreConfStatus );
    
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0010");
	}



CCTSYIntegrationTestConferenceCallControl0011::CCTSYIntegrationTestConferenceCallControl0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0011::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0011::~CCTSYIntegrationTestConferenceCallControl0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0011
 * @SYMFssID BA/CTSY/CONC-0011
 * @SYMTestCaseDesc Create a conference call when there is already one active.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::GetConferenceStatus
 * @SYMTestExpectedResults Pass - KErrNotReady on attempt to create second conference call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify failure to create conference call and existing conference remains active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	TPtrC num1, num2;

	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Attempt to create another conference.
	confCall.CreateConference(reqCreConfStatus);

	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));

	// Check KErrNotReady is returned.
	CHECK_EQUALS_L( reqCreConfStatus.Int(), KErrNotReady, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));
		
	//
	// TEST END
	//

    StartCleanup();

	TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);

    // hang up for safety
    call1.HangUp();
    call2.HangUp();
    
	CleanupStack::PopAndDestroy(&reqHangupStatus);		
	CleanupStack::PopAndDestroy(&reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0011");
	}



CCTSYIntegrationTestConferenceCallControl0012::CCTSYIntegrationTestConferenceCallControl0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0012::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0012::~CCTSYIntegrationTestConferenceCallControl0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0012
 * @SYMFssID BA/CTSY/CONC-0012
 * @SYMTestCaseDesc Set up conference call again after a two party one is closed when one party hangs up.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::CreateConference, RCall::Dial, RMobileConferenceCall::NotifyConferenceEvent, RMobileCall::NotifyMobileCallStatusChange, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetCaps, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::HangUp, RMobileConferenceCall::NotifyCapsChange
 * @SYMTestExpectedResults Pass - Conference call can be created again.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call created. Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create a three party conference call (two remote parties). 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
												
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Hang up call 1 in the conference call. 
	// Set up the notification for the call
	TExtEtelRequestStatus notifyCallStatus(call1,EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatus);
	call1.NotifyMobileCallStatusChange( notifyCallStatus, callStatus );
		
	// set up notification for the caps change 
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);

	TUint32 confCallCaps;	
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);

	// Setup the notification for the event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);

	// Setup notification for the status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
		
	// Do the hangup
	err = call1.HangUp();
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::HangUp returned an error") );
		
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyCallStatus, 
														callStatus, RMobileCall::EStatusDisconnecting, 
														KErrNone );
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusDisconnecting,
								_L("RMobileCall::NotifyMobileCallStatusChange returned incorrect status") );	
	
	call1.NotifyMobileCallStatusChange( notifyCallStatus, callStatus );
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
			notifyCallStatus, 
			callStatus, RMobileCall::EStatusIdle, 
			KErrNone );
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusIdle,
			_L("RMobileCall::NotifyMobileCallStatusChange returned incorrect status") );	
	
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusIdle.
	err = call1.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(err, KErrNone,_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusIdle, _L("RMobileCall::GetMobileCallStatus returned status not idle"));
	
	// ===  Conference should now terminate ===
	// ===  Check conference caps ===

	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   confCallCaps,
																	   0,
																	   KNoUnwantedBits,
																	   KErrNone);

	// Check RMobileConferenceCall::GetCaps returns caps of 0
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceTerminated
	// $CTSYKnownFailure defect id=10018  this notification is not delivered (times out)
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10018);	
/*
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceTerminated,
								confEventCallName,
								KErrNone);								
	
	ASSERT_EQUALS(confEvent,RMobileConferenceCall::EConferenceTerminated,
								_L("RMobileConferenceCall::NotifyConferenceEvent returned wrong event"));
*/
	
	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceIdle,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Dial a number that answers on call 1. 
	callStatus = MakeCallL(call1, num1);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Create a conference. 
	// Setup notifications for caps change
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	// Setup notification for conference event
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);

	// Setup notification for conference status
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Create the conference
	confCall.CreateConference(reqCreConfStatus);
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
					_L("RMobileConferenceCall::CreateConference timed out"));
	ASSERT_EQUALS( reqCreConfStatus.Int(),KErrNone, 
					_L("RMobileConferenceCall::CreateConference returned an error"));

	// ===  Check conference caps indicate conference can now be created ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps NOT in set of KCapsCreate
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
			   notifyCapsChangeStatus,
			   confCallCaps,
			   RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap,
			   RMobileConferenceCall::KCapsCreate,
			   KErrNone);

	// Check RMobileConferenceCall::GetCaps returns caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS(err, KErrNone,_L("RMobileConferenceCall::GetCaps returned error") );
	ASSERT_BITS_SET(confCallCaps,
			   RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap,
			   RMobileConferenceCall::KCapsCreate,
			   _L("RMobileConferenceCall::NotifyCapsChange returned incorrect caps"));

	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceBuilt
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
			notifyConfEventStatus,
			confEvent,RMobileConferenceCall::EConferenceBuilt,
			confEventCallName,
			KErrNone);								

	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
			notifyConfStatusChange,
			confStatus,RMobileConferenceCall::EConferenceActive,
			KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceActive, 
					_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Hang up conference call with RMobileConferenceCall::HangUp 
	TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
   
    confCall.HangUp(reqHangupStatus);
    WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);
	
	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();

    // reqHangupStatus
    // notifyConfStatusChange
    // notifyConfEventStatus
    // notifyCapsChangeStatus
    // notifyCallStatus
    // reqCreConfStatus    
    CleanupStack::PopAndDestroy(6, &reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0012");
	}



CCTSYIntegrationTestConferenceCallControl0013::CCTSYIntegrationTestConferenceCallControl0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0013::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0013::~CCTSYIntegrationTestConferenceCallControl0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0013
 * @SYMFssID BA/CTSY/CONC-0013
 * @SYMTestCaseDesc Create a five remote party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::GetMobileCallInfo, RMobileCall::GetMobileCallStatus, RMobileCall::GetMobileCallInfo, RMobileCall::NotifyMobileCallStatusChange, RCall::GetInfo, RMobileConferenceCall::AddCall, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::Swap
 * @SYMTestExpectedResults Pass - Five party conference call created.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify calls can be added to conference call. Verify conference call notifications complete correctly. Verify call info correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	
	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
		
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Get call 3. 
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	// Get call 4
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	// Get call 5. 
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Add call 3 to the conference using RMobileConferenceCall::AddCall
	// - and also perform checks specified in test spec
	AddCallTestStepL( confCall, call3, num3, 3 );
 	TEST_CHECK_POINT_L(_L("Adding call3 failed"));
	
	// Repeat dialling a call on call 4 and 5 and adding those 
	// to the conference (RMobileConferenceCall::EnumerateCalls should return aCount which is one higher than 
	// previous call each time). 
	// Test and checks done by function
	AddCallTestStepL( confCall, call4, num4, 4 );
 	TEST_CHECK_POINT_L(_L("Adding call4 failed"));
 	AddCallTestStepL( confCall, call5, num5, 5 );
 	TEST_CHECK_POINT_L(_L("Adding call5 failed"));
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));
						

	// For each call in the conference check RMobileConferenceCall::GetMobileCallInfo
	CheckConfCallInfo( confCall, num1, 1 );
	CheckConfCallInfo( confCall, num2, 2 );
	CheckConfCallInfo( confCall, num3, 3 );
	CheckConfCallInfo( confCall, num4, 4 );
	CheckConfCallInfo( confCall, num5, 5 );

	//
	// TEST END
	//

    StartCleanup();
    
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);
    
    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
	
	// reqHangupStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy(2,&reqCreConfStatus);
	
	return TestStepResult();
	}

	
/**
* Add a call to the conference call 
* @param aConfCall Conference call object
* @param aCall Call object to use
* @param aNumber Number to dial
* @param aCallCount 1-based call number 
*/
	
void CCTSYIntegrationTestConferenceCallControl0013::AddCallTestStepL( RMobileConferenceCall& aConfCall, 
													RMobileCall& aCall, TPtrC& aNumber,
													TInt aCallCount )
	{
	
	DEBUG_PRINTF2(_L("Performing test step adding call %d"), aCallCount );
	
	// Setup the notification for the swap
	TExtEtelRequestStatus notifyConfEventStatus(aConfCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL( notifyConfEventStatus );
		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;

	aConfCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// Swap the conference call. 
	TExtEtelRequestStatus reqSwapConfStatus(aConfCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);
	
	aConfCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
	
	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped
	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( aConfCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);	

	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
							_L("RMobileConferenceCall::Swap returned unexpected event"));
														
	// Dial a number that answers on call
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(aCall, aNumber);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call"));
	
	// Wait a bit so that the duration has a chance to be > 0
	User::After(2*KOneSecond);
		
	// ===  Check call info ===
	RCall::TCallInfo callInfo;
	TInt err = aCall.GetInfo(callInfo);
	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));
	
	// Check RCall::GetInfo for call returns valid call name.
	ASSERT_TRUE(callInfo.iCallName.Length() > 0, _L("RCall::GetInfo returned empty iCallName"));

	// Check RCall::GetInfo for call returns valid voice line name.
	ASSERT_TRUE(callInfo.iLineName.Length() > 0, _L("RCall::GetInfo returned empty iLineName"));

	// Check RCall::GetInfo for call returns hook status of EHookStatusOff.
	// $CTSYProblem Does not return EHookStatusOff, returns EHookStatusUnknown
		
	ASSERT_EQUALS(callInfo.iHookStatus, RCall::EHookStatusUnknown, 
							_L("RCall::GetInfo hook status not EHookStatusUnknown"));

	// Check RCall::GetInfo for call returns call status of EStatusConnected.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusConnected, 
							_L("RCall::GetInfo returns call status not EStatusConnected"));

	// Check RCall::GetInfo for call returns call duration of > 0.
	ASSERT_TRUE(callInfo.iDuration.Int() > 0, _L("RCall::GetInfo returned 0 duration"));

	// Check RMobileCall::GetMobileCallInfo for call returns valid line name.
	RMobileCall::TMobileCallInfoV3 mobileCallInfo;
	RMobileCall::TMobileCallInfoV3Pckg mobileCallInfoPkg(mobileCallInfo);
	err = aCall.GetMobileCallInfo(mobileCallInfoPkg);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
		
	// Check RMobileCall::GetMobileCallInfo for call returns iValid > 0.
	ASSERT_TRUE(mobileCallInfo.iValid > 0, _L("RMobileCall::GetMobileCallInfo returned zero iValid"));
	
	// Check RMobileCall::GetMobileCallInfo for call returns data for attributes marked as valid in iValid.
	if( mobileCallInfo.iValid & RMobileCall::KCallStartTime )
		{
		ASSERT_TRUE(mobileCallInfo.iStartTime.Year() > 0, 
							_L("RMobileCall::GetMobileCallInfo returned invalid iStartTime"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallDuration )
		{
		ASSERT_TRUE(mobileCallInfo.iDuration.Int() > 0, 
							_L("RMobileCall::GetMobileCallInfo returned invalid iDuration"));
		}


	if( mobileCallInfo.iValid & RMobileCall::KCallId )
		{
		ASSERT_TRUE(mobileCallInfo.iCallId != -1, 
							_L("RMobileCall::GetMobileCallInfo returned invalid iCallId"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallRemoteParty )
		{
		ASSERT_TRUE(mobileCallInfo.iRemoteParty.iDirection > 0, 
							_L("RMobileCall::GetMobileCallInfo returned invalid iRemoteParty"));
		}

	if( mobileCallInfo.iValid & RMobileCall::KCallDialledParty )
		{
		ASSERT_TRUE(mobileCallInfo.iDialledParty.iTelNumber.Length() > 0, 
							_L("RMobileCall::GetMobileCallInfo returned invalid iDialledParty"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallExitCode )
		{
		// $CTSYProblem CTSY returns KErrNone not KErrNotFound as suggested by the documentation
		ASSERT_EQUALS(mobileCallInfo.iExitCode, KErrNone,
							_L("RMobileCall::GetMobileCallInfo returned invalid iExitCode"));
		}
	
	if( mobileCallInfo.iValid & RMobileCall::KCallEmergency )
		{
		ASSERT_TRUE( !mobileCallInfo.iEmergency,
							_L("RMobileCall::GetMobileCallInfo returned invalid iEmergency"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallForwarded )
		{
		ASSERT_TRUE( !mobileCallInfo.iForwarded,
							_L("RMobileCall::GetMobileCallInfo returned invalid iForwarded"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallPrivacy )
		{
		ASSERT_TRUE( (mobileCallInfo.iPrivacy == RMobilePhone::EPrivacyUnspecified) ||
					 (mobileCallInfo.iPrivacy == RMobilePhone::EPrivacyOff) ||
					 (mobileCallInfo.iPrivacy == RMobilePhone::EPrivacyOn),
							_L("RMobileCall::GetMobileCallInfo returned invalid iPrivacy"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallTch )
		{
		ASSERT_TRUE( (mobileCallInfo.iTch == RMobileCall::ETchUnknown) ||
					 (mobileCallInfo.iTch == RMobileCall::ETchDigital) ||
					 (mobileCallInfo.iTch == RMobileCall::ETchAnalog),
							_L("RMobileCall::GetMobileCallInfo returned invalid iTch"));
		}
		
	if( mobileCallInfo.iValid & RMobileCall::KCallAlternating )
		{
		ASSERT_TRUE( (mobileCallInfo.iAlternatingCall == RMobilePhone::EAlternatingModeUnspecified) ||
					 (mobileCallInfo.iAlternatingCall == RMobilePhone::EAlternatingModeSingle) ||
					 (mobileCallInfo.iAlternatingCall == RMobilePhone::EAlternatingModeVoiceData) ||
					 (mobileCallInfo.iAlternatingCall == RMobilePhone::EAlternatingModeVoiceThenData) ||
					 (mobileCallInfo.iAlternatingCall == RMobilePhone::EAlternatingModeVoiceFax),
							_L("RMobileCall::GetMobileCallInfo returned invalid iAlternatingCall"));
		}
	
	// V3
	if( mobileCallInfo.iValid & RMobileCall::KCallSecurity )
		{
		ASSERT_TRUE( (mobileCallInfo.iSecurity == RMobilePhone::ECipheringOff) ||
					 (mobileCallInfo.iSecurity == RMobilePhone::ECipheringGSM) ||
					 (mobileCallInfo.iSecurity == RMobilePhone::ECipheringWCDMA) ||
					 (mobileCallInfo.iSecurity == RMobilePhone::ECipheringCDMA),
							_L("RMobileCall::GetMobileCallInfo returned invalid iSecurity"));
		}
		
	// Check RMobileCall::GetMobileCallInfo for call returns valid line name.
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned zero length line name"));		
		
	// Set up notification
	aConfCall.NotifyConferenceEvent( notifyConfEventStatus, confEvent, confEventCallName );

	// Add call to the conference using RMobileConferenceCall::AddCall 
	TExtEtelRequestStatus reqAddCallStatus(aConfCall,EMobileConferenceCallAddCall);
	CleanupStack::PushL(reqAddCallStatus);
	aConfCall.AddCall(reqAddCallStatus,callInfo.iCallName);

	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqAddCallStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::AddCall timed out"));
	ASSERT_EQUALS( reqAddCallStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::AddCall returned error"));

	// ===  Check conference event and status ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallAdded
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( aConfCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceCallAdded,
								confEventCallName,
								KErrNone);
	
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with aCallName = callname of call
	ASSERT_EQUALS_DES16(confEventCallName,callInfo.iCallName, 
							_L("RMobileConferenceCall::NotifyConferenceEvent did not return matching call name"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(aConfCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));
				
	// Check RMobileConferenceCall::EnumerateCalls returns aCount
	TInt confCallCount;
	ASSERT_EQUALS( aConfCall.EnumerateCalls(confCallCount), KErrNone, 
				_L("RMobileConferenceCall::EnumerateCalls returned an error") );
				
	ASSERT_EQUALS( confCallCount, aCallCount, 
				_L("RMobileConferenceCall::EnumerateCalls returned bad count") );
				
	// reqAddCallStatus
	// reqSwapConfStatus
	// notifyConfEventStatus
	CleanupStack::PopAndDestroy(3,&notifyConfEventStatus);
	
	}
	
	
TPtrC CCTSYIntegrationTestConferenceCallControl0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0013");
	}



CCTSYIntegrationTestConferenceCallControl0014::CCTSYIntegrationTestConferenceCallControl0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0014::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0014::~CCTSYIntegrationTestConferenceCallControl0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0014
 * @SYMFssID BA/CTSY/CONC-0014
 * @SYMTestCaseDesc Swap five party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::Swap, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::HangUp
 * @SYMTestExpectedResults Pass - Conference call swapped.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call swapped. Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create basic 5 party conference call. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	AddCallToConferenceL( confCall, call4, num4, 4 );
	AddCallToConferenceL( confCall, call5, num5, 5 );
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Swap the conference call with RMobileConferenceCall::Swap 	
	// Setup the notification for the swap
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL( notifyConfEventStatus );		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;

	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// set up the notification for the status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	TExtEtelRequestStatus reqSwapConfStatus(confCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);
	
	// Do the swap
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
 	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);								
	// Logging
	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
							_L("RMobileConferenceCall::Swap returned unexpected event"));

	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceHold,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));	
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Hang up conference call with RMobileConferenceCall::HangUp 

    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
	
	// reqHangupStatus
	// reqSwapConfStatus
	// notifyConfStatusChange
	// notifyConfEventStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy(5,&reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0014");
	}



CCTSYIntegrationTestConferenceCallControl0015::CCTSYIntegrationTestConferenceCallControl0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0015::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0015::~CCTSYIntegrationTestConferenceCallControl0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0015
 * @SYMFssID BA/CTSY/CONC-0015
 * @SYMTestCaseDesc Swap five party conference call, then swap it again.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::Swap, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::HangUp
 * @SYMTestExpectedResults Pass - Conference call swapped.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call swapped. Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	AddCallToConferenceL( confCall, call4, num4, 4 );
	AddCallToConferenceL( confCall, call5, num5, 5 );
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Swap the conference call with RMobileConferenceCall::Swap 	
	// Setup the notification for the swap
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL( notifyConfEventStatus );		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;

	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// set up the notification for the status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	TExtEtelRequestStatus reqSwapConfStatus(confCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);
	
	// Do the swap
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);								
	// Logging
	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
							_L("RMobileConferenceCall::Swap returned unexpected event"));

	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceHold,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned error"));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
		

	// Swap the conference call with RMobileConferenceCall::Swap 
	// repost notifications
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );	

	// do the swap
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
	TEST_CHECK_POINT_L(_L("RMobileConferenceCall::Swap failed"));
	
	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);								
	// Logging
	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
							_L("RMobileConferenceCall::Swap returned unexpected event"));

	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceActive,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceActive, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    ASSERT_EQUALS( WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium), KErrNone, 
    					_L("RMobileConferenceCall::HangUp returned error") );
    ASSERT_EQUALS( reqHangupStatus.Int(), KErrNone, _L("RMobileConferenceCall::HangUp returned error") );

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS(err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned error"));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
	
	// reqHangupStatus
	// reqSwapConfStatus
	// notifyConfStatusChange
	// notifyConfEventStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy(5,&reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0015");
	}



CCTSYIntegrationTestConferenceCallControl0016::CCTSYIntegrationTestConferenceCallControl0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0016::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0016::~CCTSYIntegrationTestConferenceCallControl0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0016
 * @SYMFssID BA/CTSY/CONC-0016
 * @SYMTestCaseDesc Dial 6th call when there is an active 5 party conference call. Conference call auto holds.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::Hangup
 * @SYMTestExpectedResults Pass - Conference call holds automatically when 6th call dialled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call auto holds. Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Get call 6. 
	// Get its number, too
	TPtrC num6;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber6, num6), 
												_L("Failed to obtain 6th number for test") );
	RMobileCall& call6 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall6 );


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


	// set up the notification for the status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Dial a number that answers on call 6.
	callStatus = MakeCallL(call6, num6);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call6"));	 

	// ===  Check conference call auto-holds. ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceHold,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS(err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned error"));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	// Hang up conference call. 
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    ASSERT_EQUALS( WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium), KErrNone, 
    						_L("RMobileConferenceCall::HangUp returned error") );
    ASSERT_EQUALS( reqHangupStatus.Int(), KErrNone, _L("RMobileConferenceCall::HangUp returned error") );
	
	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
    call6.HangUp();	
    
	// reqHangupStatus
	// notifyConfStatusChange
	// reqCreConfStatus
	CleanupStack::PopAndDestroy(3, &reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0016");
	}



CCTSYIntegrationTestConferenceCallControl0017::CCTSYIntegrationTestConferenceCallControl0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0017::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0017::~CCTSYIntegrationTestConferenceCallControl0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0017
 * @SYMFssID BA/CTSY/CONC-0017
 * @SYMTestCaseDesc Swap a five party conference call then dial a 6th call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobileConferenceCall::NotifyConferenceEvent, RMobileConferenceCall::NotifyConferenceStatusChange, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::Hangup
 * @SYMTestExpectedResults Pass - Conference call swapped. 6th call dialled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call held and 6th call dialled.  Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
		
	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Get call 6. 
	// Get its number, too
	TPtrC num6;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber6, num6), 
												_L("Failed to obtain 6th number for test") );
	RMobileCall& call6 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall6 );


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


	// Swap conference call. 
	// Setup the notification for the event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL( notifyConfEventStatus );		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;

	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// set up the notification for the status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Do the swap
	TExtEtelRequestStatus reqSwapConfStatus(confCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);	
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));
	
	// Logging
	ASSERT_EQUALS( confEvent, RMobileConferenceCall::EConferenceSwapped, 
							_L("RMobileConferenceCall::Swap returned unexpected event"));

	// ===  Check conference call status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceHold,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error"))
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);								

	// Dial a number that answers on call 6. 	
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	callStatus = MakeCallL(call6, num6);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call6"));	 
									
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Hang up conference call. 
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    ASSERT_EQUALS( WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium), KErrNone, 
    				_L("RMobileConferenceCall::HangUp returned error") );
    ASSERT_EQUALS( reqHangupStatus.Int(), KErrNone, _L("RMobileConferenceCall::HangUp returned error") );
	
	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
    call6.HangUp();

	// reqHangupStatus
	// reqSwapConfStatus
	// notifyConfStatusChange
	// notifyConfEventStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy(5, &reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0017");
	}



CCTSYIntegrationTestConferenceCallControl0018::CCTSYIntegrationTestConferenceCallControl0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0018::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0018::~CCTSYIntegrationTestConferenceCallControl0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0018
 * @SYMFssID BA/CTSY/CONC-0018
 * @SYMTestCaseDesc Swap 6th active call with five party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceEvent, RMobileConferenceCall::NotifyConferenceStatusChange, RCall::Dial, RMobileCall::Hold, RCall::GetInfo, RMobileConferenceCall::CreateConference, RMobileConferenceCall::Swap, RMobileConferenceCall::AddCall, RLine::NotifyStatusChange, RMobileCall::NotifyMobileCallStatusChange, RMobileConferenceCall::GetCaps, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::GetMobileCallInfo
 * @SYMTestExpectedResults Pass - Conference call swapped with 6th call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify conference call and 6th voice call swapped.  Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);

	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Get call 6. 
	// Get its number, too
	TPtrC num6;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber6, num6), 
												_L("Failed to obtain 6th number for test") );
	RMobileCall& call6 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall6 );

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Swap conference call. 

	// Setup the notification for the event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;

	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);
	
	// set up the notification for the status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Do the swap
	TExtEtelRequestStatus reqSwapConfStatus(confCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);

	confCall.Swap(reqSwapConfStatus);
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));

	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);								

	// ===  Check conference call status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceHold,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Dial 6th call. 
	callStatus = MakeCallL(call6, num6);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call6"));	 

	// Swap conference call (active). 
		
	// set up the notification for the event
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	
	// set up the notification for the status change
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	// Do the swap
	confCall.Swap(reqSwapConfStatus);
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));

	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceSwapped	
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceSwapped,
								confEventCallName,
								KErrNone);								

	// ===  Check conference call status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceActive,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceActive, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	
	// Hang up conference call. 
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    
    confCall.HangUp(reqHangupStatus);
    ASSERT_EQUALS( WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium), KErrNone, 
    					_L("RMobileConferenceCall::HangUp returned error") );
    ASSERT_EQUALS( reqHangupStatus.Int(), KErrNone, _L("RMobileConferenceCall::HangUp returned error") );

	// Hang up 6th call. 
	call6.HangUp();

	// Dial a number that answers on call 6. 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	callStatus = MakeCallL(call6, num6);
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call6"));	 


	//
	// TEST END
	//

    StartCleanup();

    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
    call6.HangUp();	
    
	// notifyConfStatusChange
	// notifyConfEventStatus
	// reqSwapConfStatus
	// reqHangupStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy(5, &reqCreConfStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0018");
	}



CCTSYIntegrationTestConferenceCallControl0019::CCTSYIntegrationTestConferenceCallControl0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0019::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0019::~CCTSYIntegrationTestConferenceCallControl0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0019
 * @SYMFssID BA/CTSY/CONC-0019
 * @SYMTestCaseDesc Create another conference call when there is already a five party conference call active.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNotReady on attempt to create second conference call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify creation of second conference fails.  Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Call RMobileConferenceCall::CreateConference again. 
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));

	// Check KErrNotReady returned.
	CHECK_EQUALS_L( reqCreConfStatus.Int(), KErrNotReady, 
							_L("RMobileConferenceCall::CreateConference did not return KErrNotReady"));


	
	//
	// TEST END
	//

    StartCleanup();
    
    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
	
	// reqCreConfStatus
	CleanupStack::PopAndDestroy( 1, &reqCreConfStatus );
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0019");
	}



CCTSYIntegrationTestConferenceCallControl0020::CCTSYIntegrationTestConferenceCallControl0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0020::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0020::~CCTSYIntegrationTestConferenceCallControl0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0020
 * @SYMFssID BA/CTSY/CONC-0020
 * @SYMTestCaseDesc Hang up 3rd call in five party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetMobileCallInfo, RCall::HangUp, RMobileConferenceCall::NotifyCapsChange, RMobileConferenceCall::GetCaps, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetConferenceStatus
 * @SYMTestExpectedResults Pass - Third call hung up. Conference is still active. Conference info correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call hung up. Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber6, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Check conference caps, status and info are correct. ===
	TUint32 confCallCaps;
	err = confCall.GetCaps(confCallCaps);
	
	// Check RMobileConferenceCall::GetCaps returns caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate
	ASSERT_BITS_SET(confCallCaps, 	RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap,
									RMobileConferenceCall::KCapsCreate,
									_L("RMobileConferenceCall::GetCaps returned wrong bits") );

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 5
	TInt callCount;
	err = confCall.EnumerateCalls(callCount);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned error"));
	ASSERT_EQUALS(callCount,5, _L("RMobileConferenceCall::EnumerateCalls returned wrong count"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	err = confCall.GetConferenceStatus(confCallConfStatus);
	ASSERT_EQUALS(err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
						_L("RMobileConferenceCall::GetConferenceStatus returned bad state"));
	
	// Hang up call 3 in the conference with RCall::HangUp
	
	// Setup the notification for the event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	
	
	// Do the hangup
    call3.HangUp();

	// ===  Check conference event and status ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallRemoved
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								confEventCallName,
								KErrNone);								

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with aCallName = callname of call 3
	RMobileCall::TCallInfo callInfo;
	err = call3.GetInfo(callInfo);
	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));	
	ASSERT_EQUALS_DES16(confEventCallName, callInfo.iCallName, 
						_L("RMobileConferenceCall::NotifyConferenceEvent returned wrong call name") );

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 4
	err = confCall.EnumerateCalls(callCount);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned error"));
	ASSERT_EQUALS(callCount,4, _L("RMobileConferenceCall::EnumerateCalls returned wrong count"));

	// ===  Get call info for all the calls in the conference ===
	// Check RMobileConferenceCall::GetMobileCallInfo returns
	//   valid iCallName 
	//   correct number in iDialledParty 
	//   valid iLineName 
	//   valid iRemoteParty 
	//   iStatus of EStatusConnected 
	//   relevant bits set in iValid based on the above parameters 
	CheckConfCallInfo( confCall, num1, 1 );
	CheckConfCallInfo( confCall, num2, 2 );	// NB skipped 3!
	CheckConfCallInfo( confCall, num4, 3 );
	CheckConfCallInfo( confCall, num5, 4 );

	// Hang up conference call. 
	// set up notifications
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	
	
	// do the hangup
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    
    confCall.HangUp(reqHangupStatus);
    err = WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);
    ASSERT_EQUALS(err,KErrNone,_L("RMobileConferenceCall::HangUp timed out"));
    ASSERT_EQUALS(reqHangupStatus.Int(),KErrNone,_L("RMobileConferenceCall::HangUp returned an error"));
    
	// ===  Check conference caps ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   confCallCaps,
																	   0,
																	   KNoUnwantedBits,
																	   KErrNone);
	// Just for the log
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));
																	   
	// Check RMobileConferenceCall::GetCaps returns caps of 0
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallRemoved
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								confEventCallName,
								KErrNone);								
	// Just for the log
	// $CTSYProblem completes with EConferenceCallRemoved 
	ASSERT_EQUALS(confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								_L("RMobileConferenceCall::NotifyConferenceEvent returned wrong event"));

	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceIdle,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	//
	// TEST END
	//

    StartCleanup();    
    
    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call4.HangUp();
    call5.HangUp();
    
	// reqHangupStatus
	// notifyConfStatusChange
	// notifyCapsChangeStatus
	// notifyConfEventStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy( 5, &reqCreConfStatus );
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0020");
	}



CCTSYIntegrationTestConferenceCallControl0021::CCTSYIntegrationTestConferenceCallControl0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0021::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0021::~CCTSYIntegrationTestConferenceCallControl0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0021
 * @SYMFssID BA/CTSY/CONC-0021
 * @SYMTestCaseDesc Remote party terminates call in a five party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetMobileCallInfo, RMobileCall::NotifyCallEvent, RMobileConferenceCall::NotifyCapsChange, RMobileConferenceCall::GetCaps, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetConferenceStatus
 * @SYMTestExpectedResults Pass - Conference is  still active and conference notifications complete correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify call hung up. Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create basic 5 party conference call (one call should be to RPS). 
	
	// For this manual test the user must set up a number they can answer and hang up on number 3
	_LIT(KPromptCheckNum3,"Check that the INI file has a \"RPS\" number you can answer on VoiceNumber3");
	DisplayUserInteractionPromptL(KPromptCheckNum3);	
	
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3),
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Check conference caps, status and info are correct. ===

	TUint32 confCallCaps;
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetCaps returned error"));
	
	// Check RMobileConferenceCall::GetCaps returns caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate
	ASSERT_BITS_SET(confCallCaps, 	RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap,
									RMobileConferenceCall::KCapsCreate,
									_L("RMobileConferenceCall::GetCaps returned wrong bits") );

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 5
	TInt callCount;
	err = confCall.EnumerateCalls(callCount);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned error"));
	ASSERT_EQUALS(callCount,5, _L("RMobileConferenceCall::EnumerateCalls returned wrong count"));

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// RPS hangs up. 
	
	// Setup the notification for change in call status
	TExtEtelRequestStatus notifyCallEventStatus( call3, EMobileCallNotifyCallEvent );
	CleanupStack::PushL( notifyCallEventStatus );
	RMobileCall::TMobileCallEvent callEvent;
	call3.NotifyCallEvent(notifyCallEventStatus, callEvent);
	
	// Setup the notification for the conference event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	
	
	// Hangup the RPS call		
	_LIT(KPromptForHangup,"Please hangup \"RPS\" call (call 3)");
	DisplayUserInteractionPromptL(KPromptForHangup);	
	
	// ===  Check for notification of remote termination ===
	// Check RMobileCall::NotifyCallEvent completes with ERemoteTerminated.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyCallEvent(call3,
																notifyCallEventStatus,
																callEvent,
																RMobileCall::ERemoteTerminated,
																KErrNone );

	// ===  Check conference event and status ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallRemoved
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								confEventCallName,
								KErrNone);								

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with aCallName = callname of call 3
	RMobileCall::TCallInfo callInfo;
	err = call3.GetInfo(callInfo);
	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));			
	ASSERT_EQUALS_DES16( confEventCallName, 
						callInfo.iCallName, 
						_L("RMobileConferenceCall::NotifyConferenceEvent returned wrong call name") );
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 4
	err = confCall.EnumerateCalls(callCount);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned error"));
	ASSERT_EQUALS(callCount,4, _L("RMobileConferenceCall::EnumerateCalls returned wrong count"));

	// ===  Get call info for all the calls in the conference ===
	// Check RMobileConferenceCall::GetMobileCallInfo returns
	//   valid iCallName 
	//   correct number in iDialledParty 
	//   valid iLineName 
	//   valid iRemoteParty 
	//   iStatus of EStatusConnected 
	//   relevant bits set in iValid based on the above parameters 	
	CheckConfCallInfo( confCall, num1, 1 );
	CheckConfCallInfo( confCall, num2, 2 );	// NB skipped 3!
	CheckConfCallInfo( confCall, num4, 3 );
	CheckConfCallInfo( confCall, num5, 4 );

	// Hang up conference call. 
	// set up notifications
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	
	
	// do the hangup
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    
    confCall.HangUp(reqHangupStatus);
    err = WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);
    ASSERT_EQUALS(err,KErrNone,_L("RMobileConferenceCall::HangUp timed out"));
    ASSERT_EQUALS(reqHangupStatus.Int(),KErrNone,_L("RMobileConferenceCall::HangUp returned an error"));
    
	// ===  Check conference caps ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   confCallCaps,
																	   0,
																	   KNoUnwantedBits,
																	   KErrNone);
	// Just for the log
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));
																	   
	// Check RMobileConferenceCall::GetCaps returns caps of 0
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallRemoved
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								confEventCallName,
								KErrNone);								
	// Just for the log
	ASSERT_EQUALS(confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								_L("RMobileConferenceCall::NotifyConferenceEvent returned wrong event"));

	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceIdle,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    // hang up calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
	
	// reqHangupStatus
	// notifyConfStatusChange
	// notifyCapsChangeStatus
	// notifyConfEventStatus
	// notifyCallEventStatus
	// reqCreConfStatus
	CleanupStack::PopAndDestroy( 6, &reqCreConfStatus );


	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0021");
	}



CCTSYIntegrationTestConferenceCallControl0022::CCTSYIntegrationTestConferenceCallControl0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0022::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0022::~CCTSYIntegrationTestConferenceCallControl0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0022
 * @SYMFssID BA/CTSY/CONC-0022
 * @SYMTestCaseDesc Attempt to add a sixth party to a five party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::GetCaps, RMobileConferenceCall::AddCall, RMobileCall::GetMobileCallStatus, RMobileConferenceCall::Swap, RMobileConferenceCall::GetConferenceStatus, RCall::Dial
 * @SYMTestExpectedResults Pass - Error on attempt to add a 6th call to the conference.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error returned and conference call is still active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	TEST_CHECK_POINT_L(_L("Adding call3 failed"));	
	AddCallToConferenceL( confCall, call4, num4, 4 );
	TEST_CHECK_POINT_L(_L("Adding call4 failed"));	
	AddCallToConferenceL( confCall, call5, num5, 5 );
	TEST_CHECK_POINT_L(_L("Adding call5 failed"));	
	
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Get call 6. 
	// Get its number, too
	TPtrC num6;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber6, num6), 
												_L("Failed to obtain 6th number for test") );
	RMobileCall& call6 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall6 );

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Swap the conference call with RMobileConferenceCall::Swap 
	
	// Setup the status change notification
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Do the swap
	TExtEtelRequestStatus reqSwapConfStatus(confCall,EMobileConferenceCallSwap);
	CleanupStack::PushL(reqSwapConfStatus);	
	confCall.Swap(reqSwapConfStatus);	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSwapConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS( reqSwapConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::Swap returned error"));

	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceHold,
								KErrNone );

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));	
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Dial a number that answers on call 6. 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	callStatus = MakeCallL(call6, num6);
	ASSERT_EQUALS(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call6"));
		
	// Attempt to add call 6 to the conference with RMobileConferenceCall::AddCall 
	TExtEtelRequestStatus reqAddCallStatus(confCall,EMobileConferenceCallAddCall);
	CleanupStack::PushL(reqAddCallStatus);

	// Get the call info (need the name)
	RCall::TCallInfo callInfo;
	err = call6.GetInfo(callInfo);
	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));	

	
	confCall.AddCall(reqAddCallStatus,callInfo.iCallName);
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqAddCallStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::AddCall timed out"));

	// Check error is returned.
	ASSERT_TRUE( reqAddCallStatus.Int() != KErrNone, _L("RMobileConferenceCall::AddCall returned KErrNone"));
	
	// ===  Check conference caps, status and info are correct. ===
	TUint32 confCallCaps;
	err = confCall.GetCaps(confCallCaps);
	
	// Check RMobileConferenceCall::GetCaps returns caps in set of KCapsHangUp | KCapsSwap
	// Check RMobileConferenceCall::GetCaps returns caps NOT in set of KCapsCreate
	ASSERT_BITS_SET(confCallCaps, 	RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap,
									RMobileConferenceCall::KCapsCreate,
									_L("RMobileConferenceCall::GetCaps returned wrong bits") );
	
	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 5
	TInt callCount;
	err = confCall.EnumerateCalls(callCount);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned error"));
	ASSERT_EQUALS(callCount,5, _L("RMobileConferenceCall::EnumerateCalls returned wrong count"));
			
	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
							_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceHold,
							_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceHold"));


	//
	// TEST END
	//

    StartCleanup();
    
    // Hangup calls
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();
    call4.HangUp();
    call5.HangUp();
    call6.HangUp();
	
	// reqAddCallStatus
	// reqSwapConfStatus
	// notifyConfStatusChange
	// reqCreConfStatus
	CleanupStack::PopAndDestroy( 4, &reqCreConfStatus );
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0022");
	}



CCTSYIntegrationTestConferenceCallControl0023::CCTSYIntegrationTestConferenceCallControl0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0023::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0023::~CCTSYIntegrationTestConferenceCallControl0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0023
 * @SYMFssID BA/CTSY/CONC-0023
 * @SYMTestCaseDesc Go one to one with call in a two party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RMobileConferenceCall::GetCaps, RMobileConferenceCall::GetConferenceStatus, RMobileCall::GoOneToOne, RMobileConferenceCall::NotifyCapsChange, RMobileCall::GetMobileCallCaps, RCall::HangUp
 * @SYMTestExpectedResults Pass - Go one to one succeeds.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify go one to one succeeds.  Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Create basic 2 party conference call. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Make sure we are on the network
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
												
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//	
		
	// Check RMobileCall::GetMobileCallCaps for second call includes iCallControlCaps in set of KCapsOneToOne
	// Check RMobileCall::GetMobileCallCaps for second call includes iCallControlCaps NOT in set of KCapsJoin
	RMobileCall::TMobileCallCapsV1 callCaps;
	RMobileCall::TMobileCallCapsV1Pckg callCapsPkg(callCaps);
	
	err = call2.GetMobileCallCaps(callCapsPkg);
	ASSERT_EQUALS(err,KErrNone,_L("RMobileCall::GetMobileCallCaps returned an error"));
	ASSERT_BITS_SET( callCaps.iCallControlCaps, RMobileCall::KCapsOneToOne, RMobileCall::KCapsJoin,
							_L("RMobileCall::GetMobileCallCaps returned wrong bits in iCallControlCaps"));
	
	// Go one to one with second call with RMobileCall::GoOneToOne 

	// set up the notification for the conference status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );

	// Setup the notification for the conference event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);

	// set up conf call caps change notification
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);

	TUint32 confCallCaps;	
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	// Get the name of call1 for comparison
	RCall::TCallInfo call1Info;
	err = call1.GetInfo(call1Info);
	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));
	
	// Check RCall::GetInfo for call returns valid call name.
	ASSERT_TRUE(call1Info.iCallName.Length() > 0, _L("RCall::GetInfo returned empty iCallName"));	
	
	// make the GoOneToOne call
	TExtEtelRequestStatus reqGoOneToOneStatus(call2,EMobileCallGoOneToOne);
	CleanupStack::PushL(reqGoOneToOneStatus);
		
	call2.GoOneToOne(reqGoOneToOneStatus);
	err = WaitForRequestWithTimeOut(reqGoOneToOneStatus, ETimeShort );
	ASSERT_EQUALS( err, KErrNone, _L("RMobileCall::GoOneToOne timed out"));
	ASSERT_EQUALS( reqGoOneToOneStatus.Int(), KErrNone, _L("RMobileCall::GoOneToOne returned error"));

	// === Check the conference goes to idle ===
	
	// ===  Check conference event ===
	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallRemoved
	// (call 1 has been removed)
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
								notifyConfEventStatus,
								confEvent,RMobileConferenceCall::EConferenceCallRemoved,
								confEventCallName,
								KErrNone);	
	
	ASSERT_EQUALS_DES16( confEventCallName, call1Info.iCallName,
				_L("RMobileConferenceCall::NotifyConferenceEvent returned event for wrong call") )

	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
								notifyConfStatusChange,
								confStatus,RMobileConferenceCall::EConferenceIdle,
								KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	// ===  Check conference caps ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of KCapsCreate
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   confCallCaps,
																	   RMobileConferenceCall::KCapsCreate,
																	   KNoUnwantedBits,
																	   KErrNone);
	ASSERT_EQUALS(confCallCaps, (TUint32)RMobileConferenceCall::KCapsCreate,
									_L("RMobileConferenceCall::NotifyCapsChange returned incorrect caps"));
	
	// Check RMobileConferenceCall::GetCaps returns caps of KCapsCreate
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetCaps returned an error "));	
	ASSERT_EQUALS(confCallCaps, (TUint32)RMobileConferenceCall::KCapsCreate,
								_L("RMobileConferenceCall::GetCaps returned incorrect caps"));

	// === Hangup call 1 == 
	
	// set up notifier
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);
	
	// Do the hangup
	err = call1.HangUp();
	ASSERT_EQUALS( err, KErrNone, _L("RCall::HangUp returned an error "));	
	
	// ===  Check conference caps ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
																	   notifyCapsChangeStatus,
																	   confCallCaps,
																	   0,
																	   KNoUnwantedBits,
																	   KErrNone);
	// Logging
	ASSERT_EQUALS( confCallCaps, (TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned incorrect caps"));

	// Check RMobileConferenceCall::GetCaps returns caps of 0
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetCaps returned an error "));	
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// Hang up second call. 
	err = call2.HangUp();
	ASSERT_EQUALS( err, KErrNone, _L("RCall::HangUp returned an error "));	

	
	//
	// TEST END
	//

    StartCleanup();
	
    // reqGoOneToOneStatus
    // notifyCapsChangeStatus
    // notifyConfEventStatus
    // notifyConfStatusChange
    // reqCreConfStatus
    CleanupStack::PopAndDestroy( 5, &reqCreConfStatus );
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0023");
	}



CCTSYIntegrationTestConferenceCallControl0024::CCTSYIntegrationTestConferenceCallControl0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0024::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0024::~CCTSYIntegrationTestConferenceCallControl0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0024
 * @SYMFssID BA/CTSY/CONC-0024
 * @SYMTestCaseDesc Check the last client closing an active conference call hangs it up cleanly
 * @SYMTestPriority High
 * @SYMTestActions RTelServer::Close, RMobilePhone::GetMobileCallStatus, RMobilePhone::Close, RMobileConferenceCall::Close, RMobilePhone::Open, User::After, RTelServer::Connect, RTelServer::SetExtendedErrorGranularity, RMobileConferenceCall::EnumerateCalls, RMobileConferenceCall::Open
 * @SYMTestExpectedResults Pass - Conference call hung up successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call in conference becomes idle when last client closes session.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create basic 2 party conference call. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get the test numbers	
	TPtrC num1, num2;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
												
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));
	
	// Ensure conference call is active.
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	CHECK_EQUALS_L(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
 				_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	CHECK_EQUALS_L( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
 				_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Open second phone client ===

	// Declare a second RTelServer subsession (RTelServer 2)
	// Connect to it with RTelServer::Connect 
	RTelServer telServer = iEtelSessionMgr.GetServerL(KAltServer);
	ASSERT_EQUALS(telServer.Connect(), KErrNone, _L("RTelServer::Connect returned with error"));

	// Set extended error granularity RTelServer::SetExtendedErrorGranularity(RTelServer::EErrorExtended) 
	ASSERT_EQUALS(telServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended), KErrNone, _L("RTelServer::SetExtendedErrorGranularity returned with error"));

	// Load the TSY with RTelServer::LoadPhoneModule
	TBufC<10> fileName = _L("PHONETSY");
	ASSERT_EQUALS(telServer.LoadPhoneModule(fileName), KErrNone, _L("RTelServer::SetExtendedErrorGranularity returned with error"));
	
	// Declare a new RMobilePhone (phone 2)
	// Open the phone with RMobilePhone::Open with aSession=alternate RTelServer 
	RMobilePhone& phone2 = iEtelSessionMgr.GetPhoneL(KAltServer, KMainPhone);
	
	// ===  Open a second RMobileConference call subsession with second client ===

	// ===  and enumerate calls ===

	// Declare a RMobileConferenceCall subsession (conference call 2) 
	// Open the RMobileConferenceCall with RMobileConferenceCall::Open  
	RMobileConferenceCall& confCall2 = iEtelSessionMgr.GetConferenceCallL(KAltServer,KMainPhone,KMainConferenceCall);
	
	// Check RMobileConferenceCall::EnumerateCalls returns aCount=2
	TInt enumerateCallsCount;
	TInt requireEnumerateCallsCount = 2;
	
	ASSERT_EQUALS(confCall2.EnumerateCalls(enumerateCallsCount), KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned with error"));
	ASSERT_EQUALS(enumerateCallsCount, requireEnumerateCallsCount, _L("RMobileConferenceCall::EnumerateCalls did not return 2"));

	// Close the conference call 2 subsession with RMobileConferenceCall::Close 
	iEtelSessionMgr.ReleaseConferenceCall(KAltServer,KMainPhone,KMainConferenceCall);
	
	// Wait 10 seconds to allow calls to be cleaned up with User::After(); 
	User::After(10 * KOneSecond);

	// Check RMobileConferenceCall::EnumerateCalls with (conference call 1 subsession) returns aCount=2
	ASSERT_EQUALS(confCall.EnumerateCalls(enumerateCallsCount), KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned with error"));
	ASSERT_EQUALS(enumerateCallsCount, requireEnumerateCallsCount, _L("RMobileConferenceCall::EnumerateCalls did not return 2"));
	
	// Close the phone 2 subsession with RMobilePhone::Close 
	iEtelSessionMgr.ReleasePhone(KAltServer, KMainPhone);
	
	// Close the RTelServer 2 subsession with RTelServer::Close 
	iEtelSessionMgr.ReleaseServer(KAltServer);
	
	// ===  Check one of the calls is still connected ===

	// Check RMobilePhone::GetMobileCallStatus on one of the calls in the conference returns EStatusConnected
	RMobileCall::TMobileCallStatus getMobileCallStatus;
	ASSERT_EQUALS(call1.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned with error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus did not return EStatusConnected"));

	// ===  Close the conference call subsession and check call is idle ===

	// Close the first conference call subsession with RMobileConferenceCall::Close 
	iEtelSessionMgr.ReleaseConferenceCall(KMainServer,KMainPhone,KMainConferenceCall);
	
	User::After(5 * KOneSecond);
	
	// Check RMobilePhone::GetMobileCallStatus on one of the calls in the conference returns EStatusIdle
	ASSERT_EQUALS(call1.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned with error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusIdle, _L("RMobileCall::GetMobileCallStatus did not return EStatusIdle"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    CleanupStack::PopAndDestroy(&reqCreConfStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0024");
	}



CCTSYIntegrationTestConferenceCallControl0025::CCTSYIntegrationTestConferenceCallControl0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0025::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0025::~CCTSYIntegrationTestConferenceCallControl0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0025
 * @SYMFssID BA/CTSY/CONC-0025
 * @SYMTestCaseDesc Go one to one with call in a five party conference call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileConferenceCall::NotifyConferenceStatusChange, RCall::HangUp, RMobileCall::GoOneToOne, RMobileConferenceCall::NotifyCapsChange, RMobileConferenceCall::GetCaps, RMobileConferenceCall::GetConferenceStatus, RMobileConferenceCall::HangUp, RMobileCall::GetMobileCallCaps
 * @SYMTestExpectedResults Pass - Go one to one succeeds.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify go one to one succeeds.  Verify conference call notifications complete correctly.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Create basic 5 party conference call. 
	// Make sure we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TInt err = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone );
	CHECK_EQUALS_L( err, KErrNone, _L("Failed to register on home network"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3, num4, num5;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber4, num4), 
												_L("Failed to obtain 4th number for test") );
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber5, num5), 
												_L("Failed to obtain 5th number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );
	RMobileCall& call4 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall4 );
	RMobileCall& call5 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall5 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Add the other calls
	AddCallToConferenceL( confCall, call3, num3, 3 );
	AddCallToConferenceL( confCall, call4, num4, 4 );
	AddCallToConferenceL( confCall, call5, num5, 5 );

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobileCall::GetMobileCallCaps for third call includes iCallEventCaps in set of KCapsOneToOne
	// Check RMobileCall::GetMobileCallCaps for third call includes iCallEventCaps NOT in set of KCapsJoin
	RMobileCall::TMobileCallCapsV1 callCaps;
	RMobileCall::TMobileCallCapsV1Pckg callCapsPkg(callCaps);
	
	err = call3.GetMobileCallCaps(callCapsPkg);
	ASSERT_EQUALS(err,KErrNone,_L("RMobileCall::GetMobileCallCaps returned an error"));
	ASSERT_BITS_SET( callCaps.iCallControlCaps, RMobileCall::KCapsOneToOne, RMobileCall::KCapsJoin,
							_L("RMobileCall::GetMobileCallCaps returned wrong bits in iCallControlCaps"));

	// Go one to one with third call with RMobileCall::GoOneToOne 

	// set up the notification for the conference status change
	TExtEtelRequestStatus notifyConfStatusChange(confCall, EMobileConferenceCallNotifyConferenceStatusChange);
	CleanupStack::PushL(notifyConfStatusChange);
	RMobileConferenceCall::TMobileConferenceStatus confStatus;
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );

	// make the GoOneToOne call
	TExtEtelRequestStatus reqGoOneToOneStatus(call3,EMobileCallGoOneToOne);
	CleanupStack::PushL(reqGoOneToOneStatus);
	call3.GoOneToOne(reqGoOneToOneStatus);
	err = WaitForRequestWithTimeOut(reqGoOneToOneStatus, ETimeShort );
	ASSERT_EQUALS( err, KErrNone, _L("RMobileCall::GoOneToOne timed out"));
	ASSERT_EQUALS( reqGoOneToOneStatus.Int(), KErrNone, _L("RMobileCall::GoOneToOne returned error"));
	
	// ===  Check conference call goes on automatic hold ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceHold
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
			notifyConfStatusChange,
			confStatus,RMobileConferenceCall::EConferenceHold,
			KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceHold
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceHold, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));

	// Hang up third call with RCall::HangUp 
	
	// Setup the notification for the conference event
	TExtEtelRequestStatus notifyConfEventStatus(confCall,EMobileConferenceCallNotifyConferenceEvent);
	CleanupStack::PushL(notifyConfEventStatus);		
	RMobileConferenceCall::TMobileConferenceEvent confEvent;
	TName confEventCallName;
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	
	
	// setup the notification for conference status change
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );	
	
	// do the hangup
	err = call3.HangUp();
	ASSERT_EQUALS( err, KErrNone, _L("RCall::HangUp returned an error "));	

	// ===  Check number of calls in conference ===

	// Check RMobileConferenceCall::EnumerateCalls returns aCount = 4
	TUint32 confCallCaps;
	TInt callCount;	
	err = confCall.EnumerateCalls(callCount);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::EnumerateCalls returned error"));
	ASSERT_EQUALS( callCount, 4,_L("RMobileConferenceCall::EnumerateCalls returned wrong count") );

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceCallRemoved
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent( confCall, 
			notifyConfEventStatus,
			confEvent,RMobileConferenceCall::EConferenceCallRemoved,
			confEventCallName,
			KErrNone);								

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with aCallName = callname of call 3
	RMobileCall::TCallInfo callInfo;
	err = call3.GetInfo(callInfo);
	ASSERT_EQUALS( err, KErrNone, _L("RCall::GetInfo returned error"));	
	ASSERT_EQUALS_DES16(confEventCallName, callInfo.iCallName, 
	_L("RMobileConferenceCall::NotifyConferenceEvent returned wrong call name") );

	// ===  Check conference status ===

	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceActive
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
			notifyConfStatusChange,
			confStatus,RMobileConferenceCall::EConferenceActive,
			KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceActive
	RMobileConferenceCall::TMobileConferenceStatus confCallConfStatus;	
	ASSERT_EQUALS(confCall.GetConferenceStatus(confCallConfStatus), KErrNone, 
			_L("RMobileConferenceCall::GetConferenceStatus returned an error"));
	ASSERT_EQUALS( confCallConfStatus, RMobileConferenceCall::EConferenceActive,
			_L("RMobileConferenceCall::GetConferenceStatus did not return EConferenceActive"));

	// Hang up conference call with RMobileConferenceCall::HangUp 

	// set up caps change notify
	TExtEtelRequestStatus notifyCapsChangeStatus(confCall, EMobileConferenceCallNotifyCapsChange);
	CleanupStack::PushL(notifyCapsChangeStatus);
	confCall.NotifyCapsChange(notifyCapsChangeStatus, confCallCaps);

	// Set up status change notify
	confCall.NotifyConferenceStatusChange(notifyConfStatusChange, confStatus );
	
	// Set up event notify
	confCall.NotifyConferenceEvent(notifyConfEventStatus, confEvent, confEventCallName);	

	// do the hangup
    TExtEtelRequestStatus reqHangupStatus(confCall, EMobileConferenceCallHangUp );
    CleanupStack::PushL(reqHangupStatus);
    confCall.HangUp(reqHangupStatus);
    err = WaitForRequestWithTimeOut(reqHangupStatus,ETimeMedium);
    ASSERT_EQUALS(err,KErrNone,_L("RMobileConferenceCall::HangUp timed out"));
    ASSERT_EQUALS(reqHangupStatus.Int(),KErrNone,_L("RMobileConferenceCall::HangUp returned an error"));
	
	// ===  Check conference caps ===
	// Check RMobileConferenceCall::NotifyCapsChange completes with caps of 0
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyCapsChange(confCall,
			   notifyCapsChangeStatus,
			   confCallCaps,
			   0,
			   KNoUnwantedBits,
			   KErrNone);
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// Check RMobileConferenceCall::GetCaps returns caps of 0
	err = confCall.GetCaps(confCallCaps);
	ASSERT_EQUALS(confCallCaps,(TUint32)0,_L("RMobileConferenceCall::NotifyCapsChange returned non-zero caps"));

	// ===  Check conference event ===

	// Check RMobileConferenceCall::NotifyConferenceEvent completes with EConferenceTerminated
	// $CTSYKnownFailure defect id=10018  this notification is not delivered (times out)	
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10018</font>"));
/******
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceEvent(confCall,
													 					notifyConfEventStatus,
													 					confEvent,
													 					RMobileConferenceCall::EConferenceTerminated,
													 					confEventCallName,
													 					KErrNone);
******/
	// ===  Check conference status ===
	// Check RMobileConferenceCall::NotifyConferenceStatusChange completes with EConferenceIdle
	iCallControlTsyTestHelper.WaitForMobileConferenceCallNotifyConferenceStatusChange( confCall, 
			notifyConfStatusChange,
			confStatus,RMobileConferenceCall::EConferenceIdle,
			KErrNone);

	// Check RMobileConferenceCall::GetConferenceStatus returns EConferenceIdle
	err = confCall.GetConferenceStatus(confStatus);
	ASSERT_EQUALS( err, KErrNone, _L("RMobileConferenceCall::GetConferenceStatus returned an error "));
	ASSERT_EQUALS( confStatus,RMobileConferenceCall::EConferenceIdle, 
							_L("RMobileConferenceCall::GetConferenceStatus returned unexpected result"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    // reqHangupStatus
    // notifyCapsChangeStatus
    // notifyConfEventStatus
    // reqGoOneToOneStatus
    // notifyConfStatusChange
    // reqCreConfStatus
    CleanupStack::PopAndDestroy( 6, &reqCreConfStatus );	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0025");
	}



CCTSYIntegrationTestConferenceCallControl0026::CCTSYIntegrationTestConferenceCallControl0026(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestConferenceCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestConferenceCallControl0026::GetTestStepName());
	}

CCTSYIntegrationTestConferenceCallControl0026::~CCTSYIntegrationTestConferenceCallControl0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestConferenceCallControl0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CONC-0026
 * @SYMFssID BA/CTSY/CONC-0026
 * @SYMTestCaseDesc Go one to one with a call that is not part of a conference.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::GetMobileCallStatus, RMobileCall::GoOneToOne, RMobileCall::GetMobileCallCaps, RCall::Dial
 * @SYMTestExpectedResults Pass - KErrNotSupported returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported is returned and call caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Create a basic 2 party conference call. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get the test numbers	
	TPtrC num1, num2, num3;
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber1, num1), 
												_L("Failed to obtain 1st number for test") );	
												
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber2, num2), 
												_L("Failed to obtain 2nd number for test") );
	
	CHECK_TRUE_L( GetStringFromConfig( KIniVoiceNumSection, KIniVoiceNumber3, num3), 
												_L("Failed to obtain 3rd number for test") );
	
	// Get conference call subsession.
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	
	// and the call objects
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1 );
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2 );
		
	// Create a three party conference call (two remote parties). 
	RMobileCall::TMobileCallStatus callStatus = MakeCallL(call1, num1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call1"));
	
	// Hold 1st call
	callStatus = HoldCallL(call1);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusHold,_L("Failed to hold call1"));
	
	// Make second call
	callStatus = MakeCallL(call2, num2);
	CHECK_EQUALS_L(callStatus,RMobileCall::EStatusConnected,_L("Failed to connect call2"));
	
	// Create the conference	
	TExtEtelRequestStatus reqCreConfStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(reqCreConfStatus);
	
	confCall.CreateConference(reqCreConfStatus);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqCreConfStatus,ETimeShort),KErrNone, 
							_L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L( reqCreConfStatus.Int(),KErrNone, 
							_L("RMobileConferenceCall::CreateConference returned an error"));

	// Get call 3.
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3 );

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Dial a number that answers on call 3.
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	callStatus = MakeCallL(call3, num3);
	ASSERT_EQUALS(callStatus, RMobileCall::EStatusConnected,_L("Failed to connect call3"));

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	RMobileCall::TMobileCallStatus getMobileCallStatus;
	ASSERT_EQUALS(call3.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned error")); ;
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected,_L("RMobileCall::GetMobileCallStatus did not return EStatusConnected"));
	
	// Check RMobileCall::GetMobileCallCaps includes iCallEventCaps in set of KCapsJoin
	// Check RMobileCall::GetMobileCallCaps includes iCallEventCaps NOT in set of KCapsOneToOne
	RMobileCall::TMobileCallCapsV1 callCaps;
	RMobileCall::TMobileCallCapsV1Pckg callCapsPkg(callCaps);
	
	ASSERT_EQUALS(call3.GetMobileCallCaps(callCapsPkg), KErrNone, _L("RMobileCall::GetMobileCallCaps returned error")); ;
	ASSERT_BITS_SET(callCaps.iCallControlCaps, RMobileCall::KCapsJoin, RMobileCall::KCapsOneToOne,
			_L("RMobileCall::GetMobileCallCaps returned wrong bits in iCallControlCaps"));
	
	// Try to go one to one with call 3 using RMobileCall::GoOneToOne
	TExtEtelRequestStatus goOneToOneStatus(call3, EMobileCallGoOneToOne);
	CleanupStack::PushL(goOneToOneStatus);
	
	call3.GoOneToOne(goOneToOneStatus);
	
	// Check KErrNotSupported is returned.
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10046);
	TInt err = WaitForRequestWithTimeOut(goOneToOneStatus, ETimeShort);
	ASSERT_EQUALS(err, KErrNotSupported, _L("RMobileCall::GoOneToOne timed out"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
    CleanupStack::PopAndDestroy(&reqCreConfStatus);
    CleanupStack::PopAndDestroy(&goOneToOneStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestConferenceCallControl0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestConferenceCallControl0026");
	}



