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
// Test step definitions for the AlternateLineService functional unit.
// 
//

/**
 @internalTechnology
*/

#include <ctsy/rmmcustomapi.h>  // ALS support is part of CustomApi

#include "cctsyintegrationtestalternatelineservice.h"
#include "cctsyinidata.h"


	
CCTSYIntegrationTestAlternateLineServiceBase::CCTSYIntegrationTestAlternateLineServiceBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this), iCallControlTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestAlternateLineServiceBase::~CCTSYIntegrationTestAlternateLineServiceBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestAlternateLineService0001::CCTSYIntegrationTestAlternateLineService0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAlternateLineServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAlternateLineService0001::GetTestStepName());
	}

CCTSYIntegrationTestAlternateLineService0001::~CCTSYIntegrationTestAlternateLineService0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAlternateLineService0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ALLS-0001
 * @SYMFssID BA/CTSY/ALLS-0001
 * @SYMTestCaseDesc Get and set ALS line when ALS is not supported by SIM
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetALSLine, RMobilePhone::GetALSLine
 * @SYMTestExpectedResults Pass - ALS line of unknown or not available returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify ALS returned is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMmCustomAPI& customApi = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone, KMainCustomApi);
	// Use CustomApi to request we check whether SIM supports ALS by calling RMmCustomAPI::CheckAlsPpSupport.
	// This also initialises CTSY's CenRep.
    TExtEtelRequestStatus checkAlsPpSupportStatus(customApi, ECustomSetDriveModeIPC);
	CleanupStack::PushL(checkAlsPpSupportStatus);
	RMmCustomAPI::TAlsSupport alsSupported;
	customApi.CheckAlsPpSupport(checkAlsPpSupportStatus, alsSupported);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(checkAlsPpSupportStatus, ETimeMedium), KErrNone,
    			  _L("RRMmCustomAPI::CheckAlsPpSupport timed out"));
    ASSERT_EQUALS(checkAlsPpSupportStatus.Int(), KErrNone,
    			  _L("RMmCustomAPI::CheckAlsPpSupport returned an error"));
    ASSERT_EQUALS(alsSupported, RMmCustomAPI::EAlsSupportOff,
			  _L("RRMmCustomAPI::CheckAlsPpSupport did not return correct status for SIM's ALS Support."));
    
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobilePhone::GetALSLine returns aALSLine = EAlternateLineUnknown or aALSLine = EAlternateLineNotAvailable
	RMobilePhone::TMobilePhoneALSLine alsLine;
	ASSERT_EQUALS(phone.GetALSLine(alsLine), KErrNone, _L("RMobilePhone::GetALSLine returned error."));
	
	ERR_PRINTF2(_L("<font color=Orange>@CTSYKnownFailure: defect id = %d</font>"), 10058);
	ASSERT_TRUE(alsLine == (RMobilePhone::EAlternateLineUnknown | RMobilePhone::EAlternateLineNotAvailable),
				_L("RMobilePhone::GetALSLine returned incorrect line status.") );

	// Post notifier for RMobilePhone::NotifyALSLineChange
	TExtEtelRequestStatus notifyALSLineChangeStatus(phone, EMobilePhoneNotifyALSLineChange);
	CleanupStack::PushL(notifyALSLineChangeStatus);
	RMobilePhone::TMobilePhoneALSLine alsLineNotif;
	phone.NotifyALSLineChange(notifyALSLineChangeStatus, alsLineNotif);

	// Check RMobilePhone::SetALSLine with aALSLine=EAlternateLineAuxiliary completes with KErrNotSupported
	RMobilePhone::TMobilePhoneALSLine alsLineSet = RMobilePhone::EAlternateLineAuxiliary; 
	TExtEtelRequestStatus notifyMobilePhoneSetALSLineStatus(phone, EMobilePhoneSetALSLine);
	CleanupStack::PushL(notifyMobilePhoneSetALSLineStatus);
	phone.SetALSLine(notifyMobilePhoneSetALSLineStatus, alsLineSet);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMobilePhoneSetALSLineStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetALSLine timed-out."));
	ASSERT_EQUALS(notifyMobilePhoneSetALSLineStatus.Int(), KErrNotSupported,  
			_L("RMobilePhone::SetALSLine did not return with status KErrNotSupported."));

	// Check RMobilePhone::NotifyALSLineChange completes with KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyALSLineChangeStatus, ETimeMedium), KErrNotSupported, 
			_L("RMobilePhone::NotifyALSLineChange timed-out"));
	ASSERT_EQUALS(notifyALSLineChangeStatus.Int(), KErrNotSupported,  
			_L("RMobilePhone::NotifyALSLineChange did not return KErrNotSupported."));

	//
	// TEST END
	//
	
    StartCleanup();
	
	CleanupStack::PopAndDestroy(3, &checkAlsPpSupportStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAlternateLineService0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAlternateLineService0001");
	}



CCTSYIntegrationTestAlternateLineService0002::CCTSYIntegrationTestAlternateLineService0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAlternateLineServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAlternateLineService0002::GetTestStepName());
	}

CCTSYIntegrationTestAlternateLineService0002::~CCTSYIntegrationTestAlternateLineService0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAlternateLineService0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ALLS-0002
 * @SYMFssID BA/CTSY/ALLS-0002
 * @SYMTestCaseDesc Set and get the ALS line.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyALSLineChange, RMobilePhone::SetALSLine
 * @SYMTestExpectedResults Pass - ALS  line notification completes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify ALS notification completes.
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

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Insert SIM which supports a primary voice line and an aux voice line. 
	DisplayUserInteractionPromptL(_L("Ensure you are using a SIM that has ALS enabled."), ETimeLong);
		
	// Post notifier for RMobilePhone::NotifyALSLineChange
	TExtEtelRequestStatus notifyALSLineChangeStatus(phone, EMobilePhoneNotifyALSLineChange);
	CleanupStack::PushL(notifyALSLineChangeStatus);
	RMobilePhone::TMobilePhoneALSLine alsLineNotif;
	phone.NotifyALSLineChange(notifyALSLineChangeStatus, alsLineNotif);
	
	// Set the ALS line with RMobilePhone::SetALSLine with aALSLine=EAlternateLineAuxiliary 
	RMobilePhone::TMobilePhoneALSLine alsLineSet = RMobilePhone::EAlternateLineAuxiliary;
	TExtEtelRequestStatus notifyMobilePhoneSetALSLineStatus(phone, EMobilePhoneSetALSLine);
	CleanupStack::PushL(notifyMobilePhoneSetALSLineStatus);
	phone.SetALSLine(notifyMobilePhoneSetALSLineStatus, alsLineSet);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMobilePhoneSetALSLineStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetALSLine timed-out."));
	ASSERT_EQUALS(notifyMobilePhoneSetALSLineStatus.Int(), KErrNone,  
			_L("RMobilePhone::SetALSLine returned error status.")); 
	
	// Check RMobilePhone::NotifyALSLineChange completes with aALSLine=EAlternateLineAuxiliary
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyALSLineChangeStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::NotifyALSLineChange timed-out."));
	ASSERT_EQUALS(notifyALSLineChangeStatus.Int(), KErrNone,  
			_L("RMobilePhone::NotifyALSLineChange returned with an error status."));
	ASSERT_EQUALS(alsLineNotif, alsLineSet, 
			_L("RMobilePhone::NotifyALSLineChange returned unexpected ALS line."));
	
	// Check RMobilePhone::GetALSLine returns aALSLine=EAlternateLineAuxiliary
	RMobilePhone::TMobilePhoneALSLine alsLineGet;
	ASSERT_EQUALS(phone.GetALSLine(alsLineGet), KErrNone, _L("RMobilePhone::GetALSLine returned error."));
	ASSERT_EQUALS(alsLineGet, alsLineSet,  
			_L("RMobilePhone::GetALSLine returned incorrect ALS line."));
	
	// Post notifier for RMobilePhone::NotifyALSLineChange
	phone.NotifyALSLineChange(notifyALSLineChangeStatus, alsLineNotif);
	
	// Set the ALS line with RMobilePhone::SetALSLine with aALSLine=EAlternateLinePrimary 
	alsLineSet = RMobilePhone::EAlternateLinePrimary;
	phone.SetALSLine(notifyMobilePhoneSetALSLineStatus, alsLineSet);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMobilePhoneSetALSLineStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetALSLine timed-out."));
	ASSERT_EQUALS(notifyMobilePhoneSetALSLineStatus.Int(), KErrNone,  
			_L("RMobilePhone::SetALSLine returned error status."));
	
	// Check RMobilePhone::NotifyALSLineChange completes with aALSLine=EAlternateLinePrimary
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyALSLineChangeStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::NotifyALSLineChange timed-out."));
	ASSERT_EQUALS(notifyALSLineChangeStatus.Int(), KErrNone,  
			_L("RMobilePhone::NotifyALSLineChange returned with an error status."));
	ASSERT_EQUALS(alsLineNotif, alsLineSet, 
			_L("RMobilePhone::NotifyALSLineChange returned unexpected ALS line."));
	
	// Check RMobilePhone::GetALSLine returns aALSLine=EAlternateLinePrimary
	ASSERT_EQUALS(phone.GetALSLine(alsLineGet), KErrNone, _L("RMobilePhone::GetALSLine returned error."));
	ASSERT_EQUALS(alsLineGet, alsLineSet,  
			_L("RMobilePhone::GetALSLine returned incorrect ALS line."));
	
	//
	// TEST END
	//

    StartCleanup();
	
    CleanupStack::PopAndDestroy(2, &notifyALSLineChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAlternateLineService0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAlternateLineService0002");
	}



CCTSYIntegrationTestAlternateLineService0003::CCTSYIntegrationTestAlternateLineService0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestAlternateLineServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestAlternateLineService0003::GetTestStepName());
	}

CCTSYIntegrationTestAlternateLineService0003::~CCTSYIntegrationTestAlternateLineService0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestAlternateLineService0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-ALLS-0003
 * @SYMFssID BA/CTSY/ALLS-0003
 * @SYMTestCaseDesc Make a call using alternate line.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RMobileLine::NotifyMobileLineStatusChange, RMobileCall::GetMobileCallStatus, RCall::Dial, RLine::GetCaps, RCall::NotifyStatusChange, RMobileCall::NotifyMobileCallStatusChange, RMobileLine::GetMobileLineStatus, RLine::GetStatus, RCall::GetStatus
 * @SYMTestExpectedResults Pass - Call is connected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify call and line status
 *
 * @return - TVerdict code
 */
	{
	/* *******
	 * LTSY Issue: LTSY is not returning acknowledgement that the Sim is ALS enanbled.
	 * Waiting for response from Teleca to see if we have to call another api first.
	 * 
	 * CTSY's SetALSLine behaviour is as follows:
	 * - it evaluates that SIM does not support ALS.... alsSupportedBySim: 0
	 * - so 'ME' based ALS is used (what is this?)
	 * - so when we call SetALSLine(), it sets its CenRep MEAlsLineInfo to what we pass it (EAlternateLineAuxiliary)
	 *   and CMmPhoneTsy::iAlsLine to EAlternateLineAuxiliary. Then completes the Notify & calls ReqCompleted().
	 * 
	 * CTSY's GetALSLine behaviour:
	 * - it evaluates that SIM does not support ALS.... alsSupportedBySim: 0
	 * - gets Default als info from Central Repository as EAlternateLineAuxiliary
	 * - sets client's variable as EAlternateLineAuxiliary, then calls ReqCompleted()
	 * 
	 * Because CTSY can not recognize the SIM to be ALS (& assumes its "ME" ALS line is Aux)
	 * the Dial fails to complete. CTelObject::ReqCompleted, IPC=80, TsyHandle=24, Error=-282591234
	 * *******/
	//
	// SET UP
	//

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	RMmCustomAPI& customApi = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone, KMainCustomApi);
	User::After(10000000);
	
	ERR_PRINTF2(_L("<font color=Orange>@CTSYKnownFailure: defect id = %d</font>"), 10059);
	
	//********
	// Recommended by Olavi.... Call GetCustomerServiceProfile() to check if ALS set
    TExtEtelRequestStatus getCustomerServiceProfileStatus(customApi, ECustomSetDriveModeIPC);
	CleanupStack::PushL(getCustomerServiceProfileStatus);
	RMobilePhone::TMobilePhoneCspFileV1 cspProfile;
	RMobilePhone::TMobilePhoneCspFileV1Pckg cspProfilePckg(cspProfile); 
	phone.GetCustomerServiceProfile(getCustomerServiceProfileStatus, cspProfilePckg);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getCustomerServiceProfileStatus, ETimeLong), KErrNone,
			  _L("RMobilePhone::GetCustomerServiceProfile timed out"));
	ASSERT_EQUALS(getCustomerServiceProfileStatus.Int(), KErrNone,
				  _L("RMobilePhone::GetCustomerServiceProfile returned an error"));
	
	//	check if cspProfile.iCphsTeleservices == RMobilePhone::KCspALS
	DEBUG_PRINTF2(_L("cspProfile.iCphsTeleservices: %d"), cspProfile.iCphsTeleservices);
//	ASSERT_BITS_SET(cspProfile.iCphsTeleservices, RMobilePhone::KCspALS,
//			  _L("RMobilePhone::GetCustomerServiceProfile did not return correct values."));

	CleanupStack::PopAndDestroy(); // getCustomerServiceProfileStatus
	// ... end of Olavi
	//********
	
	// Use CustomApi to request we check whether SIM supports ALS by calling RMmCustomAPI::CheckAlsPpSupport.
	// This also initialises CTSY's CenRep.
    TExtEtelRequestStatus checkAlsPpSupportStatus(customApi, ECustomSetDriveModeIPC);
	CleanupStack::PushL(checkAlsPpSupportStatus);
	RMmCustomAPI::TAlsSupport alsSupported;
	customApi.CheckAlsPpSupport(checkAlsPpSupportStatus, alsSupported);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(checkAlsPpSupportStatus, ETimeMedium), KErrNone,
    			  _L("RRMmCustomAPI::CheckAlsPpSupport timed out"));
    ASSERT_EQUALS(checkAlsPpSupportStatus.Int(), KErrNone,
    			  _L("RMmCustomAPI::CheckAlsPpSupport returned an error"));
    ASSERT_EQUALS(alsSupported, RMmCustomAPI::EAlsSupportOn,
			  _L("RRMmCustomAPI::CheckAlsPpSupport did not return correct status for SIM's ALS Support."));
    DEBUG_PRINTF2(_L("CheckAlsPpSupport returned alsSupported:%d"), alsSupported);
    
   // Check Als blocked status
    RMmCustomAPI::TGetAlsBlockStatus alsBlockStatus;
    customApi.GetAlsBlocked(alsBlockStatus);
   DEBUG_PRINTF2(_L("GetAlsBlocked returned alsBlockStatus:%d"), alsSupported);
		    
	// Insert SIM which supports a primary voice line and an aux voice line. 
	DisplayUserInteractionPromptL(_L("Ensure you are using a SIM that has ALS enabled."), ETimeLong);
	
	// Post notifier for RMobilePhone::NotifyALSLineChange
	TExtEtelRequestStatus notifyALSLineChangeStatus(phone, EMobilePhoneNotifyALSLineChange);
	CleanupStack::PushL(notifyALSLineChangeStatus);
	RMobilePhone::TMobilePhoneALSLine alsLineNotif;
	phone.NotifyALSLineChange(notifyALSLineChangeStatus, alsLineNotif);
	
	// Set the ALS line with RMobilePhone::SetALSLine with aALSLine=EAlternateLineAuxiliary 
	RMobilePhone::TMobilePhoneALSLine alsLineSet = RMobilePhone::EAlternateLineAuxiliary;
	TExtEtelRequestStatus notifyMobilePhoneSetALSLineStatus(phone, EMobilePhoneSetALSLine);
	CleanupStack::PushL(notifyMobilePhoneSetALSLineStatus);
	phone.SetALSLine(notifyMobilePhoneSetALSLineStatus, alsLineSet);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMobilePhoneSetALSLineStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetALSLine timed-out."));
	ASSERT_EQUALS(notifyMobilePhoneSetALSLineStatus.Int(), KErrNone,  
			_L("RMobilePhone::SetALSLine returned error status.")); 

	// Check RMobilePhone::NotifyALSLineChange completes with aALSLine=EAlternateLineAuxiliary
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyALSLineChangeStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::NotifyALSLineChange timed-out."));
	ASSERT_EQUALS(notifyALSLineChangeStatus.Int(), KErrNone,  
			_L("RMobilePhone::NotifyALSLineChange returned with an error status."));
	ASSERT_EQUALS(alsLineNotif, alsLineSet, 
			_L("RMobilePhone::NotifyALSLineChange returned unexpected ALS line."));

	// Check RMobilePhone::GetALSLine returns aALSLine=EAlternateLineAuxiliary
	RMobilePhone::TMobilePhoneALSLine alsLineGet;
	ASSERT_EQUALS(phone.GetALSLine(alsLineGet), KErrNone, _L("RMobilePhone::GetALSLine returned error."));
	ASSERT_EQUALS(alsLineGet, alsLineSet,  
			_L("RMobilePhone::GetALSLine returned incorrect ALS line."));
	
	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KAuxLine);

	// Get call 1 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KAuxLine, KCall1);

	// Check RLine::GetCaps on line 1 returns caps in set of KCapsAuxVoice
	RLine::TCaps lineCaps;
	ASSERT_EQUALS(voiceLine.GetCaps(lineCaps), KErrNone,  
			_L("RMobileLine::GetCaps returned an error"));
	ASSERT_BITS_SET(lineCaps.iFlags, RMobileLine::KCapsAuxVoice, KNoUnwantedBits, 
			_L("RLine::GetCaps returned wrong caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Check RLine::GetCaps supports caps in set of KCapsVoice.
	CHECK_BITS_SET_L(lineCaps.iFlags, RLine::KCapsVoice, KNoUnwantedBits,  
			_L("RLine::GetCaps returned wrong caps"));

	// Post Notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus(call1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus callStatus;
	call1.NotifyStatusChange(notifyStatusChangeStatus, callStatus);
	
	// Post Notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Post a notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyLineStatusChangeStatus (voiceLine,&RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyLineStatusChangeStatus);
	RCall::TStatus lineStatus;
	voiceLine.NotifyStatusChange(notifyLineStatusChangeStatus, lineStatus);
	
	// Post a notifier for RMobileLine::NotifyMobileLineStatusChange
	TExtEtelRequestStatus notifyMobileLineChangeStatus(voiceLine, EMobileLineNotifyMobileLineStatusChange);
	CleanupStack::PushL(notifyMobileLineChangeStatus);
	RMobileCall::TMobileCallStatus mobileLineStatus;
	voiceLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);

	// Dial a number that answers with RCall::Dial 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,
				_L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF1(_L("Board Now Dialling: "));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	TInt expectedStatus = KErrNone;
	RCall::TStatus expectedCallStatus = RCall::EStatusDialling;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);
														 
	call1.NotifyStatusChange(notifyStatusChangeStatus, callStatus);
	expectedCallStatus = RCall::EStatusConnecting;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);

	call1.NotifyStatusChange(notifyStatusChangeStatus, callStatus);
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);
	
	// Check RCall::GetStatus returns EStatusConnected.
	ASSERT_EQUALS(call1.GetStatus(callStatus), KErrNone,  
			  _L("RCall::GetStatus returned an error."));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, 
			  _L("RCall::GetStatus did not return EStatusConnected."));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	const TInt wantedStatus = KErrNone;
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);
															
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);
															
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;															
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);
															
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,  
			  _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected,  
			  _L("RMobileCall::GetMobileStatus did not return a status EStatusConnected"));
	
	// ===  Check line status ===

	// Check RLine::NotifyStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RCall::TStatus expectedLineStatus = RCall::EStatusDialling;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(voiceLine,
														 notifyLineStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);

	expectedLineStatus = RCall::EStatusConnecting;	
	voiceLine.NotifyStatusChange(notifyLineStatusChangeStatus, lineStatus);
	iCallControlTestHelper.WaitForLineNotifyStatusChange(voiceLine,
														 notifyLineStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);
	
	expectedLineStatus = RCall::EStatusConnected;	
	voiceLine.NotifyStatusChange(notifyLineStatusChangeStatus, lineStatus);	
	iCallControlTestHelper.WaitForLineNotifyStatusChange(voiceLine,
														 notifyLineStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);

	// Check RLine::GetStatus returns EStatusConnected
	ASSERT_EQUALS(voiceLine.GetStatus(lineStatus), KErrNone,  
			_L("RLine::GetStatus returned an error"));
	ASSERT_EQUALS(lineStatus, RCall::EStatusConnected,  
			_L("RLine::GetStatus did not set line status to EStatusConnected as expected"));
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileLineStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine,
																		notifyMobileLineChangeStatus,
																		mobileLineStatus,
																		expectedMobileLineStatus,
																		expectedStatus);

	voiceLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusConnecting;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine,
																		notifyMobileLineChangeStatus,
																		mobileLineStatus,
																		expectedMobileLineStatus,
																		expectedStatus);

	voiceLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine,
																		notifyMobileLineChangeStatus,
																		mobileLineStatus,
																		expectedMobileLineStatus,
																		expectedStatus);
	
	// Check RMobileLine::GetMobileLineStatus returns EStatusConnected
	ASSERT_EQUALS(voiceLine.GetMobileLineStatus(mobileLineStatus), KErrNone,  
			_L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileLineStatus, RMobileCall::EStatusConnected,  
			_L("RMobileLine::GetMobileLineStatus did not set status to EStatusConnected"));
	
	// Post Notifiers 
	call1.NotifyStatusChange(notifyStatusChangeStatus, callStatus);
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	voiceLine.NotifyStatusChange(notifyLineStatusChangeStatus, lineStatus);
	voiceLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	
	// Hang up with RCall::HangUp 
	ASSERT_EQUALS(call1.HangUp(), KErrNone, _L("RMobileCall::HangUp returned error."));

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusHangingUp -> EStatusIdle.
	expectedCallStatus = RCall::EStatusHangingUp;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);
														 
	call1.NotifyStatusChange(notifyStatusChangeStatus, callStatus);
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);
	
	// Check RCall::GetStatus returns EStatusIdle.
	ASSERT_EQUALS(call1.GetStatus(callStatus), KErrNone,  
			  _L("RCall::GetStatus returned an error."));
	ASSERT_EQUALS(callStatus, RCall::EStatusIdle, 
			  _L("RCall::GetStatus did not return EStatusConnected."));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);
															
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusIdle.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,  
			  _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusIdle,  
			  _L("RMobileCall::GetMobileStatus did not return a status EStatusConnected"));
	
	// ===  Check line status ===

	// Check RLine::NotifyStatusChange completes with EStatusHangingUp -> EStatusIdle
	expectedLineStatus = RCall::EStatusHangingUp;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(voiceLine,
														 notifyLineStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);

	expectedLineStatus = RCall::EStatusIdle;	
	voiceLine.NotifyStatusChange(notifyLineStatusChangeStatus, lineStatus);
	iCallControlTestHelper.WaitForLineNotifyStatusChange(voiceLine,
														 notifyLineStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 expectedStatus);
	
	// Check RLine::GetStatus returns EStatusIdle
	ASSERT_EQUALS(voiceLine.GetStatus(lineStatus), KErrNone,
			_L("RLine::GetStatus returned an error"));
	ASSERT_EQUALS(lineStatus, RCall::EStatusIdle,
			_L("RLine::GetStatus did not set line status to EStatusConnected as expected"));
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with EStatusDisconnecting -> EStatusIdle
	expectedMobileLineStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine,
																		notifyMobileLineChangeStatus,
																		mobileLineStatus,
																		expectedMobileLineStatus,
																		expectedStatus);

	voiceLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(voiceLine,
																		notifyMobileLineChangeStatus,
																		mobileLineStatus,
																		expectedMobileLineStatus,
																		expectedStatus);
	
	// Check RMobileLine::GetMobileLineStatus returns EStatusIdle
	ASSERT_EQUALS(voiceLine.GetMobileLineStatus(mobileLineStatus), KErrNone,  
			_L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileLineStatus, RMobileCall::EStatusIdle,  
			_L("RMobileLine::GetMobileLineStatus did not set status to EStatusConnected"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    CleanupStack::PopAndDestroy(8, &checkAlsPpSupportStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestAlternateLineService0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestAlternateLineService0003");
	}



