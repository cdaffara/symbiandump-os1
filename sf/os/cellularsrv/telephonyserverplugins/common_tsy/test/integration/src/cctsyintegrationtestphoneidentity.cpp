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
// Test step definitions for the PhoneIdentity functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestphoneidentity.h"

#include "cctsyinidata.h"

	
CCTSYIntegrationTestPhoneIdentityBase::CCTSYIntegrationTestPhoneIdentityBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iCallControlHelper(*this), iSupplementalHelper(*this), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhoneIdentityBase::~CCTSYIntegrationTestPhoneIdentityBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPhoneIdentity0001::CCTSYIntegrationTestPhoneIdentity0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0001::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0001::~CCTSYIntegrationTestPhoneIdentity0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0001
 * @SYMFssID BA/CTSY/PID-0001
 * @SYMTestCaseDesc Deactivate Calling Line Identification Restriction (CLIR).
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus, RMobileCall::GetMobileCallInfo, RCall::Dial, RCall::HangUp
 * @SYMTestExpectedResults Pass - CLIR can be deactivated and remote party can get the calling number.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify service status notifications complete and called party can get the calling number.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetClirStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	TUint32 callServiceCaps;
	
	CHECK_TRUE_L(mobilePhone.GetCallServiceCaps(callServiceCaps)==KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsGetClirStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	

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
	
	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActiveDefaultAllowed 
	RMobilePhone::TMobilePhoneIdServiceStatus idServiceStatus;	
	TExtEtelRequestStatus getIdentityServiceStatus(mobilePhone, EMobilePhoneGetIdentityServiceStatus);
	CleanupStack::PushL(getIdentityServiceStatus);
	
	mobilePhone.GetIdentityServiceStatus(getIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, idServiceStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(getIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus getIdentityServiceStatus completed with incorrect error"));
   
	ASSERT_EQUALS(idServiceStatus, RMobilePhone::EIdServiceActiveDefaultAllowed, _L("RMobilePhone::GetIdentityServiceStatus idServiceStatus is not RMobilePhone::EIdServiceActiveDefaultAllowed"));
	
	/* $CTSYProblem
	 * The CTSY does not support setting of the identity service status with RMobilePhone::SetIdentityServiceStatus API.
	 * If support is added, the rest of this test case can be used.
	 */
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::SetIdentityServiceStatus not supported by CTSY.<font color=Black>"));

	// Set CLIR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerRestriction aSetting=EIdServicePresentationRestricted 
	/*TExtEtelRequestStatus setIdentityServiceStatus(mobilePhone, EMobilePhoneSetIdentityServiceStatus);
	CleanupStack::PushL(setIdentityServiceStatus);
	
	mobilePhone.SetIdentityServiceStatus(setIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, RMobilePhone::EIdServicePresentationRestricted);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(setIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus setIdentityServiceStatus completed with incorrect error"));
    
	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActiveDefaultRestricted 
	mobilePhone.GetIdentityServiceStatus(getIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, idServiceStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(getIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus getIdentityServiceStatus completed with incorrect error"));
   
	ASSERT_EQUALS(idServiceStatus, RMobilePhone::EIdServiceActiveDefaultRestricted, _L("RMobilePhone::GetIdentityServiceStatus idServiceStatus is not RMobilePhone::EIdServiceActiveDefaultRestricted"));
	
	
	// ===  Check that caller ID is not available when CLIR is active ===

	// Display a prompt to the user to telling them that they should have set their own phone number to voice number 1 in the config file
	DisplayUserInteractionPromptL(_L("You should have set your own phone number to voice number 1 in the config file?"), ETimeMedium);
	
	// Dial the user's phone number.
	RMobileCall::TMobileCallStatus mobileCallState;
	
	// Post notification for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatus(mobileCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatus);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallState);
	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallState,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallState);
	
	// User should check that they can not see the caller's number
	DisplayUserInteractionPromptL(_L("User should check that they have received a phone call and should check that can not see the caller's number."), ETimeMedium);
		
	// Hang up.
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus=RMobileCall::EStatusIdle;
	iCallControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallState,
																			   expectedMobileCallStatus,
																			   KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallState);
	
	// ===  Deactivate CLIR ===

	// Deactivate CLIR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerRestriction aSetting=EIdServicePresentationAllowed 	
	RMobilePhone::TMobilePhoneIdService idService = RMobilePhone::EIdServiceCallerRestriction;
	RMobilePhone::TMobilePhoneIdServiceStatus idServiceStateNotify;
	
	// Post notification for RMobileCall::NotifyIdentityServiceStatus
	TExtEtelRequestStatus notifyIdentityServiceStatus(mobilePhone, EMobilePhoneNotifyIdentityServiceStatus);
	CleanupStack::PushL(notifyIdentityServiceStatus);
	
	mobilePhone.NotifyIdentityServiceStatus(notifyIdentityServiceStatus, idService, idServiceStateNotify);
	
	mobilePhone.SetIdentityServiceStatus(setIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, RMobilePhone::EIdServicePresentationAllowed);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(setIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus setIdentityServiceStatus completed with incorrect error"));

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerRestriction completes with aStatus=EIdServiceActiveDefaultAllowed
	RMobilePhone::TMobilePhoneIdService expectedService=RMobilePhone::EIdServiceCallerRestriction;
	iSupplementalHelper.WaitForMobilePhoneNotifyIdentityServiceStatus(mobilePhone,
																	  notifyIdentityServiceStatus,
																	  idService,
																	  expectedService,
																	  idServiceStateNotify,
																	  KErrNone);

	mobilePhone.NotifyIdentityServiceStatus(notifyIdentityServiceStatus, idService, idServiceStateNotify);
	
	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActiveDefaultAllowed
	mobilePhone.GetIdentityServiceStatus(getIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, idServiceStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(getIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus getIdentityServiceStatus completed with incorrect error"));
   
	ASSERT_EQUALS(idServiceStatus, RMobilePhone::EIdServiceActiveDefaultAllowed, _L("RMobilePhone::GetIdentityServiceStatus idServiceStatus is not RMobilePhone::EIdServiceActiveDefaultAllowed"));
	
	// ===  Make a call and see if the remote number is present ===

	// Dial the user's phone number.
	DisplayUserInteractionPromptL(_L("You should have set your own phone number to voice number 1 in the config file?"), ETimeMedium);

	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																	 notifyMobileCallStatus,
																	 mobileCallState,
																	 expectedMobileCallStatus,
																	 KErrNone);

	// User should check that they can see the caller's number
	DisplayUserInteractionPromptL(_L("User should check that they have received a phone call and should check that can see the caller's number."), ETimeMedium);
		
	// Hang up.
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMobileCallStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(notifyMobileCallStatus.Int(), KErrNone, _L("RMobileCall::HangUp notifyMobileCallStatus completed with incorrect error"));
	
	ASSERT_EQUALS(mobileCallState, RMobileCall::EStatusIdle, _L("RMobileCall::HangUp mobileCallState is not EStatusIdle"));

	// ===  Reactivate CLIR ===

	// Activate CLIR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerRestriction aSetting=EIdServicePresentationRestricted
	mobilePhone.SetIdentityServiceStatus(setIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, RMobilePhone::EIdServicePresentationRestricted);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(setIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus setIdentityServiceStatus completed with incorrect error"));

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerRestriction completes with aStatus=EIdServiceActiveDefaultRestricted
	expectedService=RMobilePhone::EIdServiceCallerRestriction;
	iSupplementalHelper.WaitForMobilePhoneNotifyIdentityServiceStatus(mobilePhone,
																	  notifyIdentityServiceStatus,
																	  idService,
																	  expectedService,
																	  idServiceStateNotify,
																	  KErrNone);
	
	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActiveDefaultRestricted
	mobilePhone.GetIdentityServiceStatus(getIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, idServiceStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(getIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus getIdentityServiceStatus completed with incorrect error"));
  
	ASSERT_EQUALS(idServiceStatus, RMobilePhone::EIdServiceActiveDefaultRestricted, _L("RMobilePhone::GetIdentityServiceStatus idServiceStatus is not RMobilePhone::EIdServiceActiveDefaultRestricted"));
*/
	//
	// TEST END
	//

    StartCleanup();

	CleanupStack::PopAndDestroy(&getIdentityServiceStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0001");
	}



CCTSYIntegrationTestPhoneIdentity0002::CCTSYIntegrationTestPhoneIdentity0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0002::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0002::~CCTSYIntegrationTestPhoneIdentity0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0002
 * @SYMFssID BA/CTSY/PID-0002
 * @SYMTestCaseDesc Activate Calling Line Identification Restriction (CLIR).
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetIdentityServiceStatus, RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus, RMobileCall::GetMobileCallInfo, RCall::Dial, RCall::HangUp
 * @SYMTestExpectedResults Pass - CLIR can be activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify service status notifications complete and called party does not get the calling party's number.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetClirStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	TUint32 callServiceCaps;
	
	CHECK_TRUE_L(mobilePhone.GetCallServiceCaps(callServiceCaps)==KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedservicecaps = RMobilePhone::KCapsGetClirStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedservicecaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	

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
		
	
	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActiveDefaultAllowed 
	RMobilePhone::TMobilePhoneIdServiceStatus idServiceStatus;	
	TExtEtelRequestStatus getIdentityServiceStatus(mobilePhone, EMobilePhoneGetIdentityServiceStatus);
	CleanupStack::PushL(getIdentityServiceStatus);
	
	mobilePhone.GetIdentityServiceStatus(getIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, idServiceStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(getIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus getIdentityServiceStatus completed with incorrect error"));
	ASSERT_EQUALS(idServiceStatus, RMobilePhone::EIdServiceActiveDefaultAllowed, _L("RMobilePhone::GetIdentityServiceStatus idServiceStatus is not RMobilePhone::EIdServiceActiveDefaultAllowed"));
	
	// $CTSYProblem
	// The CTSY currently does not support RMobilePhone::SetIdentityServiceStatus
	// so the rest of this test case is not applicable. If support is added for this API
	// The rest of this test can be uncommented and used.
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhone::SetIdentityServiceStatus not supported by CTSY.<font color=Black>"));
	/*
	// ===  Activate CLIR ===

	// Activate CLIR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerRestriction aSetting=EIdServicePresentationRestricted 
	RMobilePhone::TMobilePhoneIdService idService = RMobilePhone::EIdServiceCallerRestriction;
	RMobilePhone::TMobilePhoneIdServiceStatus idServiceStateNotify;
	
	// Post notification for RMobileCall::NotifyIdentityServiceStatus
	TExtEtelRequestStatus notifyIdentityServiceStatus(mobilePhone, EMobilePhoneNotifyIdentityServiceStatus);
	CleanupStack::PushL(notifyIdentityServiceStatus);
	
	mobilePhone.NotifyIdentityServiceStatus(notifyIdentityServiceStatus, idService, idServiceStateNotify);
	
	mobilePhone.SetIdentityServiceStatus(setIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, RMobilePhone::EIdServicePresentationRestricted);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(setIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::SetIdentityServiceStatus setIdentityServiceStatus completed with incorrect error"));

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerRestriction completes with aStatus=EIdServiceActiveDefaultRestricted
	RMobilePhone::TMobilePhoneIdService expectedService=RMobilePhone::EIdServiceCallerRestriction;
	iSupplementalHelper.WaitForMobilePhoneNotifyIdentityServiceStatus(mobilePhone,
																	  notifyIdentityServiceStatus,
																	  idService,
																	  expectedService,
																	  idServiceStateNotify,
																	  KErrNone);
	
	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActiveDefaultRestricted
	mobilePhone.GetIdentityServiceStatus(getIdentityServiceStatus, RMobilePhone::EIdServiceCallerRestriction, idServiceStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIdentityServiceStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus timed-out"));
	ASSERT_EQUALS(getIdentityServiceStatus.Int(), KErrNone, _L("RMobilePhone::GetIdentityServiceStatus getIdentityServiceStatus completed with incorrect error"));
   
	ASSERT_EQUALS(idServiceStatus, RMobilePhone::EIdServiceActiveDefaultRestricted, _L("RMobilePhone::GetIdentityServiceStatus idServiceStatus is not RMobilePhone::EIdServiceActiveDefaultAllowed"));
	
	// ===  Check that caller ID is not available when CLIR is active ===

	// Display a prompt to the user to telling them that they should have set their own phone number to voice number 1 in the config file
	DisplayUserInteractionPromptL(_L("You should have set your own phone number to voice number 1 in the config file?"), ETimeMedium);
	
	// Dial the user's phone number.
	RMobileCall::TMobileCallStatus mobileCallState;
	
	TExtEtelRequestStatus notifyMobileCallStatus(mobileCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatus);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallState);
	
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &number);
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial problem dialling"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallState=RMobileCall::EStatusConnected;
	iCallControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																	 notifyMobileCallStatus,
																	 mobileCallState,
																	 expectedMobileCallState,
																	 KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallState);
	
	// User should check that they can not see the caller's number
	DisplayUserInteractionPromptL(_L("User should check that they have received a phone call and should check that can not see the caller's number."), ETimeMedium);

	// Hang up.
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallState=RMobileCall::EStatusIdle;
	iCallControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																	 notifyMobileCallStatus,
																	 mobileCallState,
																	 expectedMobileCallState,
																	 KErrNone);
	*/
	//
	// TEST END
	//

    StartCleanup();

	CleanupStack::PopAndDestroy(&getIdentityServiceStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0002");
	}



CCTSYIntegrationTestPhoneIdentity0003::CCTSYIntegrationTestPhoneIdentity0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0003::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0003::~CCTSYIntegrationTestPhoneIdentity0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0003
 * @SYMFssID BA/CTSY/PID-0003
 * @SYMTestCaseDesc Activate Calling Line Identity Presentation (CLIP)
 * @SYMTestPriority High
 * @SYMTestActions RCall::AnswerIncomingCall, RMobileCall::NotifyMobileCallStatusChange, RMobilePhone::GetIdentityServiceStatus, RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus, RMobileCall::GetMobileCallInfo, RCall::HangUp
 * @SYMTestExpectedResults Pass - Remote party's number retrieved correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify service status notifications complete and remote party information is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetClipStatus capability. 

	// Ensure RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerPresentation and aLocation=EInfoLocationCachePreferred does not return aStatus=EIdServiceActiveDefaultAllowed 

	// Get Voice Line 1 

	// Get Call 1 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Activate CLIP - Be able to see who is calling you - check notification completes ===

	// Activate CLIP with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerPresentation and aSetting=EIdServicePresentationAllowed 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerPresentation completes with aStatus=EIdServiceActivePermanent

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerPresentation and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActivePermanent

	// ===  Request incoming call and check that the remote number can be retrieved ===

	// Request an incoming call from RPS. 

	// Answer the call. 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.

	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iCallingName has length > 0
	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iDirection = EMobileTerminated
	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iRemoteIdStatus = ERemoteIdentityAvailable
	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iRemoteNumber is the number RPS is calling from (coded in BCD)
	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	// Check RMobileCall::GetMobileCallInfo returns data for attributes marked as valid in iValid.

	// Hang up call. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0003");
	}



CCTSYIntegrationTestPhoneIdentity0004::CCTSYIntegrationTestPhoneIdentity0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0004::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0004::~CCTSYIntegrationTestPhoneIdentity0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0004
 * @SYMFssID BA/CTSY/PID-0004
 * @SYMTestCaseDesc Deactivate Calling Line Identity Presentation (CLIP)
 * @SYMTestPriority High
 * @SYMTestActions RCall::AnswerIncomingCall, RMobileCall::NotifyMobileCallStatusChange, RMobilePhone::GetIdentityServiceStatus, RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus, RMobileCall::GetMobileCallInfo, RCall::HangUp
 * @SYMTestExpectedResults Pass - Remote party's number not retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify service status notifications complete and it is not possible to get the calling line identity.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetClipStatus capability. 

	// Ensure RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerPresentation and aLocation=EInfoLocationCachePreferred does not return aStatus=EIdServiceActiveDefaultRestricted 

	// Get Voice Line 1 

	// Get Call 1 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Deactivate (restrict) CLIP - Not be able to see who is calling you - check notification completes ===

	// Deactivate CLIP with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerPresentation and aSetting=EIdServicePresentationRestricted 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerPresentation completes with aStatus=EIdServiceActivePermanent

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerPresentation and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent

	// ===  Request incoming call and check that the remote number can be retrieved ===

	// Request an incoming call from RPS. 

	// Answer the call. 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.

	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iCallingName has length > 0
	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iDirection = EMobileTerminated
	// Check RMobileCall::GetMobileCallInfo returns iRemoteParty.iRemoteIdStatus = ERemoteIdentityAvailableNoCliUnavailable or = ERemoteIdentityUnavailableNoCliInteractionWithOtherService
	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	// Check RMobileCall::GetMobileCallInfo returns data for attributes marked as valid in iValid.

	// Hang up call. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0004");
	}



CCTSYIntegrationTestPhoneIdentity0005::CCTSYIntegrationTestPhoneIdentity0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0005::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0005::~CCTSYIntegrationTestPhoneIdentity0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0005
 * @SYMFssID BA/CTSY/PID-0005
 * @SYMTestCaseDesc Receive rejection of suppression of calling identity.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentitySuppressionRejected
 * @SYMTestExpectedResults Pass - Notification of identity suppression rejected completes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetClirStatus capability. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Attempt to restrict caller ID ===

	// Activate CLIR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerRestriction aSetting=EIdServicePresentationRestricted 

	// Simulator should reject CLIR activation 

	// Check RMobilePhone::NotifyIdentitySuppressionRejected completes with KErrNone

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerRestriction and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActiveDefaultRestricted

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0005");
	}



CCTSYIntegrationTestPhoneIdentity0006::CCTSYIntegrationTestPhoneIdentity0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0006::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0006::~CCTSYIntegrationTestPhoneIdentity0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0006
 * @SYMFssID BA/CTSY/PID-0006
 * @SYMTestCaseDesc Activate and deactivate CNAP (calling name presentation)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus
 * @SYMTestExpectedResults Pass - Identity service status notification completes and service status is correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify notification completes and identity status is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetCnapStatus capability. 

	// Ensure RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerName and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Activate CNAP ===

	// Activate CNAP with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerName and aSetting=EIdServicePresentationRestricted 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerName completes with aStatus=EIdServicePresentationRestricted

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerName and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActivePermanent

	// ===  Deactivate CNAP ===

	// Deactivate CNAP with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceCallerName and aSetting=EIdServicePresentationAllowed 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceCallerName completes with aStatus=EIdServicePresentationAllowed

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceCallerName and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0006");
	}



CCTSYIntegrationTestPhoneIdentity0007::CCTSYIntegrationTestPhoneIdentity0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0007::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0007::~CCTSYIntegrationTestPhoneIdentity0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0007
 * @SYMFssID BA/CTSY/PID-0007
 * @SYMTestCaseDesc Get subscriber ID (IMSI) when no SIM present.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::GetSubscriberId
 * @SYMTestExpectedResults Pass - Error returned on attempt to get IMSI with no SIM.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify IMSI retrieval fails.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Remove the SIM.
	TBuf<100> prompt(_L("Ensure SIM is not present"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobilePhone::GetSubscriberId completes with error != KErrNone
	TExtEtelRequestStatus getSubscriberIdStatus(mobilePhone, EMobilePhoneGetSubscriberId);
	RMobilePhone::TMobilePhoneSubscriberId subscriberId;
	CleanupStack::PushL(getSubscriberIdStatus);
	
	mobilePhone.GetSubscriberId(getSubscriberIdStatus,subscriberId);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSubscriberIdStatus, ETimeShort), KErrNone, _L("RMobilePhone::GetSubscriberId timed-out"));
	ASSERT_TRUE(getSubscriberIdStatus.Int() != KErrNone, _L("RMobilePhone::getSubscriberIdStatus returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&getSubscriberIdStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0007");
	}



CCTSYIntegrationTestPhoneIdentity0008::CCTSYIntegrationTestPhoneIdentity0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0008::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0008::~CCTSYIntegrationTestPhoneIdentity0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0008
 * @SYMFssID BA/CTSY/PID-0008
 * @SYMTestCaseDesc Activate and deactivate COLP (connected line identification presentation)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetIdentityServiceStatus, RMobilePhone::NotifyIdentityServiceStatus
 * @SYMTestExpectedResults Pass - Identity service status notification completes and service status is correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify notification completes and identity status is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetColpStatus 

	// Ensure RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceConnectedPresentation and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Activate COLP ===

	// Activate COLP with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceConnectedPresentation and aSetting=EIdServicePresentationRestricted 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceConnectedPresentation completes with aStatus=EIdServicePresentationRestricted

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceConnectedPresentation and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActivePermanent

	// ===  Deactivate COLP ===

	// Deactivate COLP with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceConnectedPresentation and aSetting=EIdServicePresentationAllowed 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceConnectedPresentation completes with aStatus=EIdServicePresentationAllowed

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceConnectedPresentation and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0008");
	}



CCTSYIntegrationTestPhoneIdentity0009::CCTSYIntegrationTestPhoneIdentity0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0009::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0009::~CCTSYIntegrationTestPhoneIdentity0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0009
 * @SYMFssID BA/CTSY/PID-0009
 * @SYMTestCaseDesc Activate and deactivate COLR (connected line identity restriction)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - COLR activated and deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify notification completes and identity status is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetColrStatus 

	// Ensure RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceConnectedRestriction and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Activate COLR ===

	// Activate COLR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceConnectedRestriction and aSetting=EIdServicePresentationRestricted 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceConnectedRestriction completes with aStatus=EIdServicePresentationRestricted

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceConnectedRestriction and aLocation=EInfoLocationCachePreferred returns aStatus=EIdServiceActivePermanent

	// ===  Deactivate COLR ===

	// Deactivate COLR with RMobilePhone::SetIdentityServiceStatus with aService=EIdServiceConnectedRestriction and aSetting=EIdServicePresentationAllowed 

	// Check RMobilePhone::NotifyIdentityServiceStatus with aService=EIdServiceConnectedRestriction completes with aStatus=EIdServicePresentationAllowed

	// Check RMobilePhone::GetIdentityServiceStatus with aService=EIdServiceConnectedRestriction and aLocation=EInfoLocationCachePreferred returns aStatus!=EIdServiceActivePermanent

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneIdentity0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0009");
	}



CCTSYIntegrationTestPhoneIdentity0010::CCTSYIntegrationTestPhoneIdentity0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneIdentityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneIdentity0010::GetTestStepName());
	}

CCTSYIntegrationTestPhoneIdentity0010::~CCTSYIntegrationTestPhoneIdentity0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneIdentity0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PID-0010
 * @SYMFssID BA/CTSY/PID-0010
 * @SYMTestCaseDesc Get the phone identity
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Phone manufacturer, model, revision and serial number returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify manufacturer, model, revision and serial number returned.
 *
 * @return - TVerdict code
 */
		{
		
	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// EnsureRMobilePhone::GetIdentityCaps returns caps in set of 
	// RMobilePhone::KCapsGetManufacturer | RMobilePhone::KCapsGetModel | RMobilePhone::KCapsGetRevision |
	//  RMobilePhone::KCapsGetSerialNumber 
	
	TUint32 caps; 
	TInt result = phone.GetIdentityCaps(caps);
	CHECK_TRUE_L(result == KErrNone, _L("RMobilePhone::GetIdentityCaps failed."))
	
	// Needs all caps
	TUint neededCaps = RMobilePhone::KCapsGetManufacturer | RMobilePhone::KCapsGetModel | 
							RMobilePhone::KCapsGetRevision | RMobilePhone::KCapsGetSerialNumber;	
	CHECK_BITS_SET_L( caps, neededCaps, KNoUnwantedBits, _L("RMobilePhone::GetIdentityCaps returned wrong mask") )
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get the phone identity ===
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;	
	TExtEtelRequestStatus requestStatus(phone, EMobilePhoneGetPhoneId);
	CleanupStack::PushL(requestStatus);	
	phone.GetPhoneId(requestStatus, phoneId);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestStatus, ETimeLong), 
			KErrNone, _L("RMobilePhone::GetPhoneId timed out."))
	ASSERT_EQUALS(requestStatus.Int(), KErrNone, _L("RMobilePhone::GetPhoneId completed with incorrect status")) 
	
	// Check RMobilePhone::GetPhoneId returns aId.iManufacturer with length > 0
	ASSERT_TRUE(phoneId.iManufacturer.Length() > 0, 
				_L("RMobilePhone::GetPhoneId returned invalid manufacturer"))	
	
	// Check RMobilePhone::GetPhoneId returns aId.iModel with length > 0
	ASSERT_TRUE(phoneId.iModel.Length() > 0, 
				_L("RMobilePhone::GetPhoneId returned invalid model"))
	
	// Check RMobilePhone::GetPhoneId returns aId.iRevision with length > 0
	ASSERT_TRUE(phoneId.iRevision.Length() > 0, 
				_L("RMobilePhone::GetPhoneId returned invalid revision"))	

	// Check RMobilePhone::GetPhoneId returns aId.iSerialNumber with length > 0
	ASSERT_TRUE(phoneId.iSerialNumber.Length() > 0, 
				_L("RMobilePhone::GetPhoneId returned invalid serial number"))

	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::Pop(); // requestStatus
	
	return TestStepResult();
	}
	

TPtrC CCTSYIntegrationTestPhoneIdentity0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneIdentity0010");
	}



