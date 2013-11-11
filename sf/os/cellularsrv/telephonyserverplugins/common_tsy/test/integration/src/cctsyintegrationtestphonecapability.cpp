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
// Test step definitions for the PhoneCapability functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestphonecapability.h"



	
CCTSYIntegrationTestPhoneCapabilityBase::CCTSYIntegrationTestPhoneCapabilityBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhoneCapabilityBase::~CCTSYIntegrationTestPhoneCapabilityBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPhoneCapability0001::CCTSYIntegrationTestPhoneCapability0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneCapabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneCapability0001::GetTestStepName());
	}

CCTSYIntegrationTestPhoneCapability0001::~CCTSYIntegrationTestPhoneCapability0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneCapability0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PCAP-0001
 * @SYMFssID BA/CTSY/PCAP-0001
 * @SYMTestCaseDesc Get multimode capabilities.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Correct caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify correct caps are returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetMultimodeCaps returns caps in set of RMobilePhone::KCapsGsmSupported RMobilePhone::KCapsGprsSupported
	
	TUint32 caps;
	TUint32 expectedCaps = RMobilePhone::KCapsGsmSupported | RMobilePhone::KCapsGprsSupported;
	TUint32 unwantedCaps = 0;
	
	ASSERT_EQUALS(phone.GetMultimodeCaps(caps), KErrNone, 
			_L("RMobilePhone::GetMultimodeCaps returned with an error."))
	
	ASSERT_BITS_SET(caps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetMultimodeCaps returned wrong multimode caps"))
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneCapability0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneCapability0001");
	}



CCTSYIntegrationTestPhoneCapability0002::CCTSYIntegrationTestPhoneCapability0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneCapabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneCapability0002::GetTestStepName());
	}

CCTSYIntegrationTestPhoneCapability0002::~CCTSYIntegrationTestPhoneCapability0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneCapability0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PCAP-0002
 * @SYMFssID BA/CTSY/PCAP-0002
 * @SYMTestCaseDesc Cancel request for phone capability change notification.
 * @SYMTestPriority High
 * @SYMTestActions RPhone::NotifyCapsChangeCancel
 * @SYMTestExpectedResults Pass - KErrCancel returned on cancellation.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	TRequestStatus status;
	
	// Call RPhone::NotifyCapsChange.
	
	RPhone::TCaps caps;
	
	phone.NotifyCapsChange(status, caps);
	
	User::After(1000000);
	
	// Check RPhone::NotifyCapsChangeCancel completes with KErrCancel.

	phone.NotifyCapsChangeCancel();
	
	User::WaitForRequest(status);
	
	ASSERT_TRUE(status==KErrCancel, _L("RMobilePhone::NotifyCapsChangeCancel status is not KErrCancel."))
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneCapability0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneCapability0002");
	}



