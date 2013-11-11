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
// Test step definitions for the UserNetworkAccess functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestusernetworkaccess.h"



	
CCTSYIntegrationTestUserNetworkAccessBase::CCTSYIntegrationTestUserNetworkAccessBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestUserNetworkAccessBase::~CCTSYIntegrationTestUserNetworkAccessBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestUserNetworkAccess0001::CCTSYIntegrationTestUserNetworkAccess0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUserNetworkAccessBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUserNetworkAccess0001::GetTestStepName());
	}

CCTSYIntegrationTestUserNetworkAccess0001::~CCTSYIntegrationTestUserNetworkAccess0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUserNetworkAccess0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USNA-0001
 * @SYMFssID BA/CTSY/USNA-0001
 * @SYMTestCaseDesc Send a network service request.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNone returned on sending network service request.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request completes with KErrNone
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Send a network service request with RMobilePhone::SendNetworkServiceRequest and aServiceString="*#77#" (check status of COLR)
	TExtEtelRequestStatus sendNetworkServiceRequestStatus(phone, EMobilePhoneSendNetworkServiceRequest);
	CleanupStack::PushL(sendNetworkServiceRequestStatus);
	_LIT(KServiceStr, "*#77#");
	phone.SendNetworkServiceRequest(sendNetworkServiceRequestStatus, KServiceStr);

	// Check KErrNone is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendNetworkServiceRequestStatus, ETimeLong), KErrNone, 
			_L("RMobilePhone::SendNetworkServiceRequest timed-out"));	
	ASSERT_EQUALS(sendNetworkServiceRequestStatus.Int(), KErrNone,			 
			_L("RMobilePhone::SendNetworkServiceRequest returned error status."));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1, &sendNetworkServiceRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUserNetworkAccess0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUserNetworkAccess0001");
	}



