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
// Test step definitions for the SupplementaryServiceCaps functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsupplementaryservicecaps.h"
#include "cetelsessionmgr.h"


	
CCTSYIntegrationTestSupplementaryServiceCapsBase::CCTSYIntegrationTestSupplementaryServiceCapsBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSupplementaryServiceCapsBase::~CCTSYIntegrationTestSupplementaryServiceCapsBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestSupplementaryServiceCaps0001::CCTSYIntegrationTestSupplementaryServiceCaps0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSupplementaryServiceCapsBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSupplementaryServiceCaps0001::GetTestStepName());
	}

CCTSYIntegrationTestSupplementaryServiceCaps0001::~CCTSYIntegrationTestSupplementaryServiceCaps0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSupplementaryServiceCaps0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SUSC-0001
 * @SYMFssID BA/CTSY/SUSC-0001
 * @SYMTestCaseDesc Get the supplementary service caps.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCallServiceCaps
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
	//Get the supplementary service caps
	//Verify correct caps are returned.
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetCallServiceCaps returns caps in set of

	//   RMobilePhone::KCapsGetCFStatusNetwork 

	//         | RMobilePhone::KCapsSetCFStatus 

	//         | RMobilePhone::KCapsNotifyCFStatus 

	//         | RMobilePhone::KCapsGetClipStatus 

	//         | RMobilePhone::KCapsGetClirStatus 

	//         | RMobilePhone::KCapsGetColpStatus 

	//         | RMobilePhone::KCapsGetColrStatus 

	//         | RMobilePhone::KCapsGetCnapStatus 

	//         | RMobilePhone::KCapsGetCBStatusNetwork 

	//         | RMobilePhone::KCapsSetCBStatus 

	//         | RMobilePhone::KCapsNotifyCBStatus 

	//         | RMobilePhone::KCapsChangeCBPassword 

	//         | RMobilePhone::KCapsBarAllIncoming 

	//         | RMobilePhone::KCapsBarIncomingRoaming 

	//         | RMobilePhone::KCapsBarAllOutgoing 

	//         | RMobilePhone::KCapsBarOutgoingInternational 

	//         | RMobilePhone::KCapsBarOutgoingInternationalExHC 

	//         | RMobilePhone::KCapsBarAllCases 

	//         | RMobilePhone::KCapsGetCWStatusNetwork 

	//         | RMobilePhone::KCapsSetCWStatus 

	//         | RMobilePhone::KCapsNotifyCWStatus 

	//         | RMobilePhone::KCapsNetworkServiceRequest 

	TUint32 callServiceCaps;
	ASSERT_EQUALS(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, 
					_L("RMobilePhone::GetCallServiceCaps returned error"))
	TUint32 expectedCaps = 
							  RMobilePhone::KCapsGetCFStatusNetwork
					        | RMobilePhone::KCapsSetCFStatus
					        | RMobilePhone::KCapsNotifyCFStatus
					        | RMobilePhone::KCapsGetClipStatus
					        | RMobilePhone::KCapsGetClirStatus
					        | RMobilePhone::KCapsGetColpStatus
					        | RMobilePhone::KCapsGetColrStatus
					        | RMobilePhone::KCapsGetCnapStatus
					        | RMobilePhone::KCapsGetCBStatusNetwork
					        | RMobilePhone::KCapsSetCBStatus
					        | RMobilePhone::KCapsNotifyCBStatus
					        | RMobilePhone::KCapsChangeCBPassword
					        | RMobilePhone::KCapsBarAllIncoming
					        | RMobilePhone::KCapsBarIncomingRoaming
					        | RMobilePhone::KCapsBarAllOutgoing
					        | RMobilePhone::KCapsBarOutgoingInternational
					        | RMobilePhone::KCapsBarOutgoingInternationalExHC
					        | RMobilePhone::KCapsBarAllCases
					        | RMobilePhone::KCapsGetCWStatusNetwork
					        | RMobilePhone::KCapsSetCWStatus
					        | RMobilePhone::KCapsNotifyCWStatus
					        | RMobilePhone::KCapsNetworkServiceRequest;
	
	TUint32 unwantedCaps = 0;
	
	ASSERT_BITS_SET(callServiceCaps, expectedCaps, unwantedCaps, _L("RMobilePhone::GetCallServiceCaps returned incorrect caps"))
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSupplementaryServiceCaps0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSupplementaryServiceCaps0001");
	}



