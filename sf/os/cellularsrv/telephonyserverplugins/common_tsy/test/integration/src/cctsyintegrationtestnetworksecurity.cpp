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
// Test step definitions for the NetworkSecurity functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestnetworksecurity.h"



	
CCTSYIntegrationTestNetworkSecurityBase::CCTSYIntegrationTestNetworkSecurityBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestNetworkSecurityBase::~CCTSYIntegrationTestNetworkSecurityBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestNetworkSecurity0001::CCTSYIntegrationTestNetworkSecurity0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkSecurity0001::GetTestStepName());
	}

CCTSYIntegrationTestNetworkSecurity0001::~CCTSYIntegrationTestNetworkSecurity0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkSecurity0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWS-0001
 * @SYMFssID BA/CTSY/NTWS-0001
 * @SYMTestCaseDesc Get the ciphering indicator status
 * @SYMTestPriority High
 * @SYMTestActions RCall::GetCipheringIndicatorStatus
 * @SYMTestExpectedResults Pass - Correct ciphering indicator status.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify ciphering indicator status is correct
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
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Check RMobilePhone::GetCipheringIndicatorStatus completes with aIndicatorStatus=ECipheringDisplayOn
	TExtEtelRequestStatus getCipheringIndicatorStatusStatus(mobilePhone, EMobilePhoneGetCipheringIndicatorStatus);
	RMobilePhone::TMobileCallCipheringIndicator callCipheringIndicator;
	CleanupStack::PushL(getCipheringIndicatorStatusStatus);
	
	mobilePhone.GetCipheringIndicatorStatus(getCipheringIndicatorStatusStatus, callCipheringIndicator);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCipheringIndicatorStatusStatus, ETimeShort), KErrNone, _L("RMobilePhone::GetCipheringIndicatorStatus timed-out"));
	ASSERT_EQUALS(getCipheringIndicatorStatusStatus.Int(), KErrNone, _L("RMobilePhone::GetCipheringIndicatorStatus returned with an error"));
	ASSERT_EQUALS(callCipheringIndicator, RMobilePhone::ECipheringDisplayOn, _L("RMobilePhone::GetCipheringIndicatorStatus returned with unexpected value"));
			
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&getCipheringIndicatorStatusStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkSecurity0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkSecurity0001");
	}



CCTSYIntegrationTestNetworkSecurity0002::CCTSYIntegrationTestNetworkSecurity0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkSecurityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkSecurity0002::GetTestStepName());
	}

CCTSYIntegrationTestNetworkSecurity0002::~CCTSYIntegrationTestNetworkSecurity0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkSecurity0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWS-0002
 * @SYMFssID BA/CTSY/NTWS-0002
 * @SYMTestCaseDesc Get network security notifications when moving from an 3G to 2G only cell.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyNetworkSecurityLevelChange
 * @SYMTestExpectedResults Pass - Network security notification completes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify network security status notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Ensure phone is camped on simulated 3G cell supporting ciphering.
	DisplayUserInteractionPromptL(_L("Ensure phone is camped on simulated 3G cell supporting ciphering."), ETimeMedium);
		
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetNetworkSecurityLevel returns aSecurity=ECipheringWCDMA
	TExtEtelRequestStatus getNetworkSecurityLevelStatus(phone, EMobilePhoneGetNetworkSecurityLevel);
	CleanupStack::PushL(getNetworkSecurityLevelStatus);
	RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity;
	phone.GetNetworkSecurityLevel(getNetworkSecurityLevelStatus, networkSecurity);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkSecurityLevelStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetNetworkSecurityLevel timed out"));
	ASSERT_EQUALS(getNetworkSecurityLevelStatus.Int(), KErrNone, _L("RMobilePhone::GetNetworkSecurityLevel returned an error"));
	ASSERT_EQUALS(networkSecurity, RMobilePhone::ECipheringWCDMA, _L("RMobilePhone::GetNetworkSecurityLevel did not return ECipheringWCDMA as expected"));

    // post notify for networksecuritylevelchange
	TExtEtelRequestStatus reqNotifyNetworkSecurityLevelChange(phone, EMobilePhoneNotifyNetworkSecurityLevelChange);
    CleanupStack::PushL(reqNotifyNetworkSecurityLevelChange);
    phone.NotifyNetworkSecurityLevelChange(reqNotifyNetworkSecurityLevelChange, networkSecurity);

	// Simulate moving to a 2G only cell supporting ciphering 
	DisplayUserInteractionPromptL(_L("Simulate moving to a 2G only cell supporting ciphering."), ETimeMedium);
	
	// Check RMobilePhone::NotifyNetworkSecurityLevelChange completes with aSecurity=ECipheringGSM
	RMobilePhone::TMobilePhoneNetworkSecurity expectedNetworkSecurity = RMobilePhone::ECipheringGSM;
	iNetworkTestHelper.WaitForMobilePhoneNotifyNetworkSecurityLevelChange(phone,
														reqNotifyNetworkSecurityLevelChange,
														networkSecurity,
														expectedNetworkSecurity,
														KErrNone);
		
	// Check RMobilePhone::GetNetworkSecurityLevel returns aSecurity=ECipheringGSM
	phone.GetNetworkSecurityLevel(getNetworkSecurityLevelStatus, networkSecurity);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkSecurityLevelStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetNetworkSecurityLevel timed out"));
	ASSERT_EQUALS(getNetworkSecurityLevelStatus.Int(), KErrNone, _L("RMobilePhone::GetNetworkSecurityLevel returned an error"));
	ASSERT_EQUALS(networkSecurity, RMobilePhone::ECipheringGSM, _L("RMobilePhone::GetNetworkSecurityLevel did not return ECipheringGSM as expected"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getNetworkSecurityLevelStatus
	// reqNotifyNetworkSecurityLevelChange
	CleanupStack::PopAndDestroy(2, &getNetworkSecurityLevelStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkSecurity0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkSecurity0002");
	}



