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
// Test step definitions for the Indicator functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestindicator.h"

#include "cctsyinidata.h"

#include "listretrievalhelpers.h"



	
CCTSYIntegrationTestIndicatorBase::CCTSYIntegrationTestIndicatorBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this), iCallForwardingControlHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestIndicatorBase::~CCTSYIntegrationTestIndicatorBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestIndicator0001::CCTSYIntegrationTestIndicator0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestIndicatorBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestIndicator0001::GetTestStepName());
	}

CCTSYIntegrationTestIndicator0001::~CCTSYIntegrationTestIndicator0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestIndicator0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-IND-0001
 * @SYMFssID BA/CTSY/IND-0001
 * @SYMTestCaseDesc Get the indicator caps.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Correct caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify caps returned are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobilePhone::GetIndicatorCaps returns aActionCaps in set
	// of RMobilePhone::KCapsGetIndicator | RMobilePhone::KCapsNotifyIndicatorChange
	// and indicator caps in set of RMobilePhone::KIndChargerConnected | RMobilePhone::KIndNetworkAvailable |
    // RMobilePhone::KIndCallInProgress

	TUint32 actionCaps=0;
	TUint32 indCaps=0;
	ASSERT_EQUALS(mobilePhone.GetIndicatorCaps(actionCaps, indCaps), KErrNone, _L("RMobilePhone::GetIndicatorCaps returned an error"));
	
	TUint32 expectedServiceCaps = RMobilePhone::KCapsGetIndicator | RMobilePhone::KCapsNotifyIndicatorChange;
	ASSERT_BITS_SET(actionCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicatorCaps returned wrong caps"));

	expectedServiceCaps = RMobilePhone::KIndChargerConnected | RMobilePhone::KIndNetworkAvailable | RMobilePhone::KIndCallInProgress;
	ASSERT_BITS_SET(indCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicatorCaps returned wrong caps"));

	//
	// TEST END
	//

    StartCleanup();

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestIndicator0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestIndicator0001");
	}



CCTSYIntegrationTestIndicator0002::CCTSYIntegrationTestIndicator0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestIndicatorBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestIndicator0002::GetTestStepName());
	}

CCTSYIntegrationTestIndicator0002::~CCTSYIntegrationTestIndicator0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestIndicator0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-IND-0002
 * @SYMFssID BA/CTSY/IND-0002
 * @SYMTestCaseDesc Get indicator when network is available and call is in progress
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RMobilePhone::GetIndicator, RMobilePhone::NotifyIndicatorChange, RCall::HangUp, RCall::Dial
 * @SYMTestExpectedResults Pass - Call indicator notification completes when call is made
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify correct indicator is returned and indicator notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RMobilePhone::GetIndicatorCaps returns aActionCaps in set of RMobilePhone::KCapsGetIndicator  | RMobilePhone::KCapsNotifyIndicatorChange
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	TUint32 actionCaps = 0;
	TUint32 indCaps = 0;
	CHECK_EQUALS_L(mobilePhone.GetIndicatorCaps(actionCaps, indCaps), KErrNone, _L("RMobilePhone::GetIndicatorCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsGetIndicator  | RMobilePhone::KCapsNotifyIndicatorChange;
	CHECK_BITS_SET_L(actionCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicatorCaps returned wrong caps"));

	// Ensure RMobilePhone::GetIndicatorCaps returns aIndCaps in set of RMobilePhone::KIndCallInProgress | RMobilePhone::KIndNetworkAvailable
	expectedServiceCaps = RMobilePhone::KIndCallInProgress | RMobilePhone::KIndNetworkAvailable;
	CHECK_BITS_SET_L(indCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicatorCaps returned wrong caps"));

	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegisteredOnHomeNetwork 
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	TExtEtelRequestStatus getNetworkRegistraionStatusStatus(mobilePhone,EMobilePhoneGetNetworkRegistrationStatus);
	CleanupStack::PushL(getNetworkRegistraionStatusStatus);
	mobilePhone.GetNetworkRegistrationStatus(getNetworkRegistraionStatusStatus, registrationStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getNetworkRegistraionStatusStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus timed out"));
	CHECK_EQUALS_L(getNetworkRegistraionStatusStatus.Int(), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned an error"));
	CHECK_EQUALS_L(registrationStatus, RMobilePhone::ERegisteredOnHomeNetwork,_L("RMobilePhone::GetNetworkRegistrationStatus did not set the registration status to ERegisteredOnHomeNetwork"));
	
	// Get voice line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Check RMobilePhone::GetIndicator returns aIndicator with flags in set of KIndNetworkAvailable
	// Check RMobilePhone::GetIndicator returns aIndicator with flags NOT in set of KIndCallInProgress
	TUint32 indicator=0;
	TExtEtelRequestStatus getIndicatorStatus(mobilePhone,EMobilePhoneGetIndicator);
	CleanupStack::PushL(getIndicatorStatus);

	mobilePhone.GetIndicator(getIndicatorStatus, indicator);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIndicatorStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIndicator timed out"));
	ASSERT_EQUALS(getIndicatorStatus.Int(), KErrNone, _L("RMobilePhone::GetIndicator returned an error"));

	expectedServiceCaps = RMobilePhone::KIndNetworkAvailable;
	TUint32 unexpectServiceCaps = RMobilePhone::KIndCallInProgress;
	ASSERT_BITS_SET(indicator, expectedServiceCaps, unexpectServiceCaps, _L("RMobilePhone::GetIndicator returned wrong caps"));
	
	// Post notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatus(mobileCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// Post notifier for RMobileCall::NotifyIndicatorChange
	TExtEtelRequestStatus notifyIndicatorChangeStatus(mobilePhone, EMobilePhoneNotifyIndicatorChange);
	CleanupStack::PushL(notifyIndicatorChangeStatus);
	TUint32 notifyIndicatorChangeIndicator=0;
	mobilePhone.NotifyIndicatorChange(notifyIndicatorChangeStatus, notifyIndicatorChangeIndicator);
	
	// ===  Dial a call ===

	// Dial a number that answers using RCall::Dial
	TPtrC outgoingnumber; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, outgoingnumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &outgoingnumber);
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, outgoingnumber);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned an error"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus=RMobileCall::EStatusDialling;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusConnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
		
	expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	// Check RMobilePhone::NotifyIndicatorChange completes with aIndicator with flags in set of KIndCallInProgress | KIndNetworkAvailable
	TUint32 expectedIndicator = RMobilePhone::KIndCallInProgress | RMobilePhone::KIndNetworkAvailable;
	iCallForwardingControlHelper.WaitForMobilePhoneNotifyIndicatorChange(mobilePhone,
																		 notifyIndicatorChangeStatus,
																		 notifyIndicatorChangeIndicator, 
																		 expectedIndicator,
																		 KErrNone);
	
	mobilePhone.NotifyIndicatorChange(notifyIndicatorChangeStatus, notifyIndicatorChangeIndicator);
	
	// Check RMobilePhone::GetIndicator returns aIndicator with flags in set of KIndCallInProgress | KIndNetworkAvailable
	mobilePhone.GetIndicator(getIndicatorStatus, indicator);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIndicatorStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIndicator timed out"));
	ASSERT_EQUALS(getIndicatorStatus.Int(), KErrNone, _L("RMobilePhone::GetIndicator returned an error"));

	expectedServiceCaps = RMobilePhone::KIndCallInProgress | RMobilePhone::KIndNetworkAvailable;
	CHECK_BITS_SET_L(indicator, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicator returned wrong caps"));
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// Hang up call with RCall::HangUp
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	// Check RCall::NotifyStatusChange completes with EStatusDisconnecting -> EStatusIdle
	expectedMobileCallStatus=RMobileCall::EStatusDisconnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusIdle;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	// Check RMobilePhone::NotifyIndicatorChange completes with aIndicator with flags in set of KIndNetworkAvailable
	expectedIndicator = RMobilePhone::KIndNetworkAvailable;
	iCallForwardingControlHelper.WaitForMobilePhoneNotifyIndicatorChange(mobilePhone,
																		 notifyIndicatorChangeStatus,
																		 notifyIndicatorChangeIndicator, 
																		 expectedIndicator,
																		 KErrNone);
	
	// Check RMobilePhone::GetIndicator returns aIndicator with flags NOT in set of KIndCallInProgress
	mobilePhone.GetIndicator(getIndicatorStatus, indicator);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIndicatorStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIndicator timed out"));
	ASSERT_EQUALS(getIndicatorStatus.Int(), KErrNone, _L("RMobilePhone::GetIndicator returned an error"));

	unexpectServiceCaps = RMobilePhone::KIndCallInProgress;
	CHECK_BITS_SET_L(indicator, KNoUnwantedBits, unexpectServiceCaps, _L("RMobilePhone::GetIndicator returned wrong caps"));

	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&dialStatus);
	CleanupStack::PopAndDestroy(&notifyIndicatorChangeStatus);
	CleanupStack::PopAndDestroy(&notifyMobileCallStatus);
	CleanupStack::PopAndDestroy(&getIndicatorStatus);
	CleanupStack::PopAndDestroy(&getNetworkRegistraionStatusStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestIndicator0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestIndicator0002");
	}



CCTSYIntegrationTestIndicator0003::CCTSYIntegrationTestIndicator0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestIndicatorBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestIndicator0003::GetTestStepName());
	}

CCTSYIntegrationTestIndicator0003::~CCTSYIntegrationTestIndicator0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestIndicator0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-IND-0003
 * @SYMFssID BA/CTSY/IND-0003
 * @SYMTestCaseDesc Connect the phone charger
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyIndicatorChange
 * @SYMTestExpectedResults Pass - Indicator notification completes when charger is connected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify indicator notification completes
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Check RMobilePhone::GetIndicatorCaps returns aActionCaps in set of RMobilePhone::KCapsGetIndicator | RMobilePhone::KCapsNotifyIndicatorChange
	TUint32 actionCaps=0;
	TUint32 indCaps=0;
	ASSERT_EQUALS(mobilePhone.GetIndicatorCaps(actionCaps, indCaps), KErrNone, _L("RMobilePhone::GetIndicatorCaps returned an error"));
	
	TUint32 expectedServiceCaps = RMobilePhone::KCapsGetIndicator | RMobilePhone::KCapsNotifyIndicatorChange;
	CHECK_BITS_SET_L(actionCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicatorCaps returned wrong caps"));

	// Check RMobilePhone::GetIndicatorCaps returns aIndCaps in set of RMobilePhone::KIndChargerConnected
	expectedServiceCaps = RMobilePhone::KIndChargerConnected;
	CHECK_BITS_SET_L(indCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicatorCaps returned wrong caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobilePhone::GetIndicator returns aIndicator with flags NOT in  set of RMobilePhone::KIndChargerConnected
	TUint32 indicator=0;
	TExtEtelRequestStatus getIndicatorStatus(mobilePhone,EMobilePhoneGetIndicator);
	CleanupStack::PushL(getIndicatorStatus);
	
	mobilePhone.GetIndicator(getIndicatorStatus, indicator);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIndicatorStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIndicator timed out"));
	ASSERT_EQUALS(getIndicatorStatus.Int(), KErrNone, _L("RMobilePhone::GetIndicator returned an error"));

	TUint32 unexpectedIndicatorCaps = RMobilePhone::KIndChargerConnected;
	ASSERT_BITS_SET(indicator, KNoUnwantedBits, unexpectedIndicatorCaps, _L("RMobilePhone::GetIndicator returned wrong caps"));
	
	// Post notifier for RMobileCall::NotifyIndicatorChange
	TExtEtelRequestStatus notifyIndicatorChangeStatus(mobilePhone, EMobilePhoneNotifyIndicatorChange);
	CleanupStack::PushL(notifyIndicatorChangeStatus);
	TUint32 notifyIndicatorChangeIndicator=0;
	mobilePhone.NotifyIndicatorChange(notifyIndicatorChangeStatus, notifyIndicatorChangeIndicator);
	
	// Connect the charger to the phone 
	TBuf<50> prompt(_L("Connect the charger to the phone"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobilePhone::NotifyIndicatorChange completes with aIndicator with flags in set of RMobilePhone::KIndChargerConnected and RMobilePhone::KIndNetworkAvailable
	TUint32 expectedIndicator = RMobilePhone::KIndChargerConnected | RMobilePhone::KIndNetworkAvailable;
	iCallForwardingControlHelper.WaitForMobilePhoneNotifyIndicatorChange(mobilePhone,
																		 notifyIndicatorChangeStatus,
																		 notifyIndicatorChangeIndicator, 
																		 expectedIndicator,
																		 KErrNone);
	
	// Check RMobilePhone::GetIndicator returns aIndicator with flags in set of RMobilePhone::KIndChargerConnected
	mobilePhone.GetIndicator(getIndicatorStatus, indicator);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getIndicatorStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetIndicator timed out"));
	ASSERT_EQUALS(getIndicatorStatus.Int(), KErrNone, _L("RMobilePhone::GetIndicator returned an error"));
	
	TUint32 expectedIndicatorCaps = RMobilePhone::KIndChargerConnected;
	ASSERT_BITS_SET(indicator, expectedIndicatorCaps, KNoUnwantedBits, _L("RMobilePhone::GetIndicator returned wrong caps"));

	//
	// TEST END
	//

    StartCleanup();
    
	CleanupStack::PopAndDestroy(&notifyIndicatorChangeStatus);
	CleanupStack::PopAndDestroy(&getIndicatorStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestIndicator0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestIndicator0003");
	}

