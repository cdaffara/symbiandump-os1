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
// Test step definitions for the MessageWaiting functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestmessagewaiting.h"



	
CCTSYIntegrationTestMessageWaitingBase::CCTSYIntegrationTestMessageWaitingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestMessageWaitingBase::~CCTSYIntegrationTestMessageWaitingBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestMessageWaiting0001::CCTSYIntegrationTestMessageWaiting0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMessageWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMessageWaiting0001::GetTestStepName());
	}

CCTSYIntegrationTestMessageWaiting0001::~CCTSYIntegrationTestMessageWaiting0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMessageWaiting0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MSGW-0001
 * @SYMFssID BA/CTSY/MSGW-0001
 * @SYMTestCaseDesc Activate and deactivate the message waiting indicators.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetIccMessageWaitingIndicators, RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
 * @SYMTestExpectedResults Pass - Message waiting indicator is set.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify message waiting indicator notification completes and status is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Ensure that RMobilePhone::GetIccAccessCaps returns caps in set of RMobilePhone::KCapsUSimAccessSupported
	
	// This step is needed before we access the sim, and was taken from cctsyintegrationtestsubscriberinfo.cpp ....
	// In the CTSY, the security event triggers a call to get the ICC type from the SIM
	TExtEtelRequestStatus notifySecurityEventStatus(phone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(notifySecurityEventStatus);
	RMobilePhone::TMobilePhoneSecurityEvent event;
	phone.NotifySecurityEvent(notifySecurityEventStatus, event); 
	User::After(KOneSecond*5); 

	// Now can call RMobilePhone::GetIccAccessCaps 
	TUint32 iccCaps;
	ASSERT_EQUALS(phone.GetIccAccessCaps(iccCaps), KErrNone,  
			_L("RMobilePhone::GetIccAccessCaps returned an error"));
	ASSERT_BITS_SET(iccCaps, RMobilePhone::KCapsUSimAccessSupported, KNoUnwantedBits, 
			_L("RMobilePhone::GetIccAccessCaps returned wrong caps"));

	// Turn off all message waiting indicators using RMobilePhone::SetIccMessageWaitingIndicators with TMobilePhoneMessageWaitingV1::iDisplayStatus=0 
	TExtEtelRequestStatus setIccMsgWaitIndStatus(phone, EMobilePhoneSetIccMessageWaitingIndicators);
	CleanupStack::PushL(setIccMsgWaitIndStatus);
	RMobilePhone::TMobilePhoneMessageWaitingV1 msgWaiting;
 	msgWaiting.iDisplayStatus = 0;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgWaitingPckg(msgWaiting);
	phone.SetIccMessageWaitingIndicators(setIccMsgWaitIndStatus, msgWaitingPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setIccMsgWaitIndStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetIccMessageWaitingIndicators timed-out."));
	ASSERT_EQUALS(setIccMsgWaitIndStatus.Int(), KErrNone,  
			_L("RMobilePhone::SetIccMessageWaitingIndicators returned error status.")); 
	
	// Ensure RMobilePhone::GetIccMessageWaitingIndicators returns TMobilePhoneMessageWaitingV1::iDisplayStatus=0 
	TExtEtelRequestStatus getIccMsgWaitIndStatus(phone, EMobilePhoneGetIccMessageWaitingIndicators);
	CleanupStack::PushL(getIccMsgWaitIndStatus);
	msgWaiting.iDisplayStatus = 111;
	phone.GetIccMessageWaitingIndicators(getIccMsgWaitIndStatus, msgWaitingPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIccMsgWaitIndStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::GetIccMessageWaitingIndicators timed-out."));
	ASSERT_EQUALS(getIccMsgWaitIndStatus.Int(), KErrNone,  
			_L("RMobilePhone::GetIccMessageWaitingIndicators returned error status."));
	ASSERT_BITS_SET(msgWaiting.iDisplayStatus, 0, KNoUnwantedBits,
			_L("RMobilePhone::GetIccMessageWaitingIndicators returned incorrect value for iDisplayStatus.")); 

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	/*
	 * Note:
	 * KDisplayVoicemailActive, KDisplayFaxActive, KDisplayAuxVoicemailActivev, KDisplayDataActive WORK.
	 * KDisplayEmailActive, KDisplayOtherActive DO NOT WORK
	 */
	const TInt KMaxDisplayFlags = 4;
	const TUint KIndDisplayArray[KMaxDisplayFlags] =
			{ RMobilePhone::KDisplayVoicemailActive,
			  RMobilePhone::KDisplayFaxActive,
			  //RMobilePhone::KDisplayEmailActive,
			  //RMobilePhone::KDisplayOtherActive,
			  RMobilePhone::KDisplayAuxVoicemailActive,
			  RMobilePhone::KDisplayDataActive };


	// ===  Set message waiting indicator for voice and check notification and get message waiting indicator ===

	// Set up for notification RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
	TExtEtelRequestStatus notifyIccMsgWaitIndStatus(phone, EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);
	CleanupStack::PushL(notifyIccMsgWaitIndStatus);
	RMobilePhone::TMobilePhoneMessageWaitingV1 mgWaitingNotif;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mgWaitingNotifPckg(mgWaitingNotif);
	
	TInt i = 0;
	do{
		RMobilePhone::TMobilePhoneIndicatorDisplay displayStatusSetting = (RMobilePhone::TMobilePhoneIndicatorDisplay)KIndDisplayArray[i];
	
		// Post notification RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
		phone.NotifyIccMessageWaitingIndicatorsChange(notifyIccMsgWaitIndStatus, mgWaitingNotifPckg);
		
		// Set the message waiting indicators with RMobilePhone::SetIccMessageWaitingIndicators with TMobilePhoneMessageWaitingV1::iDisplayStatus=KDisplayVoicemailActive 
		msgWaiting.iDisplayStatus = displayStatusSetting;
		phone.SetIccMessageWaitingIndicators(setIccMsgWaitIndStatus, msgWaitingPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(setIccMsgWaitIndStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::SetIccMessageWaitingIndicators timed-out."));
		ASSERT_EQUALS(setIccMsgWaitIndStatus.Int(), KErrNone,
				_L("RMobilePhone::SetIccMessageWaitingIndicators returned error status.")); 
		
		// Check RMobilePhone::NotifyIccMessageWaitingIndicatorsChange completes with TMobilePhoneMessageWaitingV1::iDisplayStatus in set of KDisplayVoicemailActive
		ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIccMsgWaitIndStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange timed-out."));
		ASSERT_EQUALS(notifyIccMsgWaitIndStatus.Int(), KErrNone,
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange returned with an error status."));
		ASSERT_BITS_SET(mgWaitingNotif.iDisplayStatus, displayStatusSetting, KNoUnwantedBits,
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange did not return expected value for iDisplayStatus."));

		// Check RMobilePhone::GetIccMessageWaitingIndicators returns TMobilePhoneMessageWaitingV1::iDisplayStatus in set of KDisplayVoicemailActive
		phone.GetIccMessageWaitingIndicators(getIccMsgWaitIndStatus, msgWaitingPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getIccMsgWaitIndStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::GetIccMessageWaitingIndicators timed-out."));
		ASSERT_EQUALS(getIccMsgWaitIndStatus.Int(), KErrNone,
				_L("RMobilePhone::GetIccMessageWaitingIndicators returned error status."));
		ASSERT_BITS_SET(msgWaiting.iDisplayStatus, displayStatusSetting, KNoUnwantedBits,
				_L("RMobilePhone::GetIccMessageWaitingIndicators returned incorrect value for iDisplayStatus."));
		
		// ===  Unset message waiting indicator for voice and check notification and get message waiting indicator ===
	
		// Post notification RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
		phone.NotifyIccMessageWaitingIndicatorsChange(notifyIccMsgWaitIndStatus, mgWaitingNotifPckg);
		
		// Set the message waiting indicators with RMobilePhone::SetIccMessageWaitingIndicators with TMobilePhoneMessageWaitingV1::iDisplayStatus=0 
		TUint8 invMask = TUint8(displayStatusSetting);
		invMask = ~invMask;
		TUint8 invDisplayStatusSetting = displayStatusSetting & invMask;
		msgWaiting.iDisplayStatus = invDisplayStatusSetting;
		phone.SetIccMessageWaitingIndicators(setIccMsgWaitIndStatus, msgWaitingPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(setIccMsgWaitIndStatus, ETimeMedium), KErrNone, 
				_L("RMobilePhone::SetIccMessageWaitingIndicators timed-out."));
		ASSERT_EQUALS(setIccMsgWaitIndStatus.Int(), KErrNone,  
				_L("RMobilePhone::SetIccMessageWaitingIndicators returned error status."));

		// Check RMobilePhone::NotifyIccMessageWaitingIndicatorsChange completes with TMobilePhoneMessageWaitingV1::iDisplayStatus not in set of KDisplayVoicemailActive
		ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIccMsgWaitIndStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange timed-out."));
		ASSERT_EQUALS(notifyIccMsgWaitIndStatus.Int(), KErrNone,
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange returned with an error status."));
		ASSERT_BITS_SET(mgWaitingNotif.iDisplayStatus, invDisplayStatusSetting, KNoUnwantedBits,
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange did not return expected value for iDisplayStatus."));

		// Check RMobilePhone::GetIccMessageWaitingIndicators returns TMobilePhoneMessageWaitingV1::iDisplayStatus not in set of KDisplayVoicemailActive
		phone.GetIccMessageWaitingIndicators(getIccMsgWaitIndStatus, msgWaitingPckg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getIccMsgWaitIndStatus, ETimeMedium), KErrNone,
				_L("RMobilePhone::GetIccMessageWaitingIndicators timed-out."));
		ASSERT_EQUALS(getIccMsgWaitIndStatus.Int(), KErrNone,
				_L("RMobilePhone::GetIccMessageWaitingIndicators returned error status."));
		ASSERT_BITS_SET(msgWaiting.iDisplayStatus, invDisplayStatusSetting, KNoUnwantedBits,
				_L("RMobilePhone::GetIccMessageWaitingIndicators returned incorrect value for iDisplayStatus."));

		++i;	
	} while(i < KMaxDisplayFlags);

	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(4, &notifySecurityEventStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMessageWaiting0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMessageWaiting0001");
	}



CCTSYIntegrationTestMessageWaiting0002::CCTSYIntegrationTestMessageWaiting0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMessageWaitingBase(aEtelSessionMgr)
/**
 * Constructor.
 */ 
	{
	SetTestStepName(CCTSYIntegrationTestMessageWaiting0002::GetTestStepName());
	}

CCTSYIntegrationTestMessageWaiting0002::~CCTSYIntegrationTestMessageWaiting0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMessageWaiting0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MSGW-0002
 * @SYMFssID BA/CTSY/MSGW-0002
 * @SYMTestCaseDesc Receive message waiting indicator from network
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Message waiting indicator received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message waiting indicator notification completes and status is correct.
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// This step is needed before we access the sim, and was taken from cctsyintegrationtestsubscriberinfo.cpp ....
	// In the CTSY, the security event triggers a call to get the ICC type from the SIM
	TExtEtelRequestStatus notifySecurityEventStatus(phone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(notifySecurityEventStatus);
	RMobilePhone::TMobilePhoneSecurityEvent event;
	phone.NotifySecurityEvent(notifySecurityEventStatus, event);
	User::After(KOneSecond*5); 
	
	// Ensure that RMobilePhone::GetIccAccessCaps returns caps in set of RMobilePhone::KCapsUSimAccessSupported
	TUint32 iccAccessCaps;
	ASSERT_EQUALS(phone.GetIccAccessCaps(iccAccessCaps), KErrNone,
			_L("RMobilePhone::GetIccAccessCaps returned error."));
	ASSERT_BITS_SET(iccAccessCaps, RMobilePhone::KCapsUSimAccessSupported, KNoUnwantedBits,
			_L("RMobilePhone::GetIccAccessCaps returned incorrect caps."));
	
	// Turn off all message waiting indicators using RMobilePhone::SetIccMessageWaitingIndicators with TMobilePhoneMessageWaitingV1::iDisplayStatus=KDisplayVoicemailActive 
	TExtEtelRequestStatus setIccMsgWaitIndStatus(phone, EMobilePhoneSetIccMessageWaitingIndicators);
	CleanupStack::PushL(setIccMsgWaitIndStatus);
	RMobilePhone::TMobilePhoneMessageWaitingV1 msgWaiting;
	msgWaiting.iDisplayStatus = RMobilePhone::KDisplayVoicemailActive;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgWaitingPckg(msgWaiting);
	phone.SetIccMessageWaitingIndicators(setIccMsgWaitIndStatus, msgWaitingPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setIccMsgWaitIndStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetIccMessageWaitingIndicators timed-out."));
	ASSERT_EQUALS(setIccMsgWaitIndStatus.Int(), KErrNone,  
			_L("RMobilePhone::SetIccMessageWaitingIndicators returned error status."));
	
	// Ensure RMobilePhone::GetIccMessageWaitingIndicators returns TMobilePhoneMessageWaitingV1::iDisplayStatus in set of KDisplayVoicemailActive 
	TExtEtelRequestStatus getIccMsgWaitIndStatus(phone, EMobilePhoneGetIccMessageWaitingIndicators);
	CleanupStack::PushL(getIccMsgWaitIndStatus);
	phone.GetIccMessageWaitingIndicators(getIccMsgWaitIndStatus, msgWaitingPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIccMsgWaitIndStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::GetIccMessageWaitingIndicators timed-out."));
	ASSERT_EQUALS(getIccMsgWaitIndStatus.Int(), KErrNone,  
			_L("RMobilePhone::GetIccMessageWaitingIndicators returned error status."));
	ASSERT_BITS_SET(msgWaiting.iDisplayStatus, RMobilePhone::KDisplayVoicemailActive, KNoUnwantedBits,
			_L("RMobilePhone::GetIccMessageWaitingIndicators returned incorrect value for iDisplayStatus."));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post RMobilePhone::NotifyMessageWaiting
	// Set up for notification RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
	TExtEtelRequestStatus notifyIccMsgWaitIndStatus(phone, EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);
	CleanupStack::PushL(notifyIccMsgWaitIndStatus);
	RMobilePhone::TMobilePhoneMessageWaitingV1 mgWaitingNotif;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mgWaitingNotifPckg(mgWaitingNotif);
	phone.NotifyIccMessageWaitingIndicatorsChange(notifyIccMsgWaitIndStatus, mgWaitingNotifPckg);

	// Simulated network sends a message waiting indication to the phone. 

	/*
	 * Here it seems you may have several choices on how to get a voicemail on to the device.
	 * 1. Use a sim network as suggested in the comment above - don't know too much about this
	 * 2. Use code to divert an incoming call to the voicemail number
	 * 3. Use a real device to set the sim to divert all incoming calls to voicemail, then wait for a call
	 * 
	 * Option 3 was used at this point.
	 */
	
	DisplayUserInteractionPromptL(_L("Dial this SIM now."), ETimeLong);
	
	User::After(10000000);
	DisplayUserInteractionPromptL(_L("Hangup call now."), ETimeLong);
	User::After(10000000);		// Wait for MWIS to change

	// Check RMobilePhone::NotifyIccMessageWaitingIndicatorsChange completes with TMobilePhoneMessageWaitingV1::iDisplayStatus in set of KDisplayVoicemailActive
	ERR_PRINTF2(_L("<font color=Orange>@CTSYKnownFailure: defect id = %d</font>"), 10057);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIccMsgWaitIndStatus, ETimeLong), KErrNone,
			_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange timed-out."));
	ASSERT_EQUALS(notifyIccMsgWaitIndStatus.Int(), KErrNone,
			_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange returned with an error status."));
	ASSERT_TRUE(mgWaitingNotif.iVoiceMsgs > 0,
			_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange did not return expected value fot voicemail messages waiting."));
	
	// Check RMobilePhone::GetIccMessageWaitingIndicators returns TMobilePhoneMessageWaitingV1::iDisplayStatus in set of KDisplayVoicemailActive
	phone.GetIccMessageWaitingIndicators(getIccMsgWaitIndStatus, msgWaitingPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIccMsgWaitIndStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::GetIccMessageWaitingIndicators timed-out."));
	ASSERT_EQUALS(getIccMsgWaitIndStatus.Int(), KErrNone,  
			_L("RMobilePhone::GetIccMessageWaitingIndicators returned error status."));
	ASSERT_BITS_SET(msgWaiting.iDisplayStatus, RMobilePhone::KDisplayVoicemailActive, KNoUnwantedBits,
			_L("RMobilePhone::GetIccMessageWaitingIndicators returned incorrect value for iDisplayStatus."));
	
	// Check RMobilePhone::GetIccMessageWaitingIndicators returnsTMobilePhoneMessageWaitingV1::iVoiceMsgs > 0
	ERR_PRINTF2(_L("<font color=Orange>@CTSYKnownFailure: defect id = %d</font>"), 10056);
	ASSERT_TRUE(msgWaiting.iVoiceMsgs > 0,
			_L("RMobilePhone::GetIccMessageWaitingIndicators did not return expected value fot voicemail messages waiting."));

	//
	// TEST END
	//

    StartCleanup();
	
    CleanupStack::PopAndDestroy(4, &notifySecurityEventStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMessageWaiting0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMessageWaiting0002");
	}



