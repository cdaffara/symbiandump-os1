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
// Test step definitions for the CallOwnership functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallownership.h"



	
CCTSYIntegrationTestCallOwnershipBase::CCTSYIntegrationTestCallOwnershipBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCallOwnershipBase::~CCTSYIntegrationTestCallOwnershipBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestCallOwnership0001::CCTSYIntegrationTestCallOwnership0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallOwnershipBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallOwnership0001::GetTestStepName());
	}

CCTSYIntegrationTestCallOwnership0001::~CCTSYIntegrationTestCallOwnership0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallOwnership0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-COWN-0001
 * @SYMFssID BA/CTSY/COWN-0001
 * @SYMTestCaseDesc Check that call ownership is not supported for voice calls.
 * @SYMTestPriority High
 * @SYMTestActions RCall::TransferOwnership, RCall::GetOwnershipStatus
 * @SYMTestExpectedResults Pass - KErrNotSupported returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported is returned.
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
	
	// Check RCall::GetOwnershipStatus for call 1 returns EOwnershipUnowned
	RCall::TOwnershipStatus ownershipStatus;
		
	ASSERT_EQUALS(mobileCall.GetOwnershipStatus(ownershipStatus), KErrNone, _L("RCall::GetOwnershipStatus returned incorrect value"));
	ASSERT_EQUALS(ownershipStatus, RCall::EOwnershipUnowned, _L("RCall::GetOwnershipStatus returned incorrect value"));
		
	// Check RCall::TransferOwnership for call 1 returns KErrNotSupported
	ASSERT_EQUALS(mobileCall.TransferOwnership(), KErrNotSupported, _L("RCall::TransferOwnership returned incorrect value"));

	// Check RCall::AcquireOwnership for call 1 returns KErrNotSupported
	TCoreEtelRequestStatus<RCall> acquireOwnershipStatus(mobileCall, &RCall::AcquireOwnershipCancel);
	CleanupStack::PushL(acquireOwnershipStatus);
	
	mobileCall.AcquireOwnership(acquireOwnershipStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(acquireOwnershipStatus, ETimeShort), KErrNone, _L("RCall::AcquireOwnership timed-out"));
	ASSERT_EQUALS(acquireOwnershipStatus.Int(), KErrNotSupported, _L("RCall::AcquireOwnership returned with an incorrect error"));

	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&acquireOwnershipStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallOwnership0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallOwnership0001");
	}



CCTSYIntegrationTestCallOwnership0002::CCTSYIntegrationTestCallOwnership0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallOwnershipBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallOwnership0002::GetTestStepName());
	}

CCTSYIntegrationTestCallOwnership0002::~CCTSYIntegrationTestCallOwnership0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallOwnership0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-COWN-0002
 * @SYMFssID BA/CTSY/COWN-0002
 * @SYMTestCaseDesc Acquire ownership of a data and fax call when call owner closes subsession.
 * @SYMTestPriority High
 * @SYMTestActions RCall::TransferOwnership, RCall::Dial, RCall::HangUp, RCall::GetOwnershipStatus
 * @SYMTestExpectedResults Pass - Call 2 acquires ownership of the call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call ownership acquired.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get data line 1. 

	// Get call 1. 

	// Get data line 2 on alternate server. 

	// Get call 2 on the alternate server. Call 2 should open existing call with call 1's name. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Call 1 dials, then call 2 requests ownership ===

	// Dial a number that answers using call 1 

	// Check RCall::GetOwnershipStatus on call 1 returns EOwnershipOwnedByThisClient

	// Call RCall::AcquireOwnership on call 2. 

	// ===  Close call 1 without hanging-up ===

	// Release call 1 subsession 

	// ===  Check ownership transfer is successfull ===

	// Check RCall::AcquireOwnership on call 2 completes with KErrNone

	// Check RCall::GetOwnershipStatus on call 2 returns EOwnershipOwnedByThisClient

	// Hang up on call 2. 

	// Repeat test with a fax call. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallOwnership0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallOwnership0002");
	}



CCTSYIntegrationTestCallOwnership0003::CCTSYIntegrationTestCallOwnership0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallOwnershipBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallOwnership0003::GetTestStepName());
	}

CCTSYIntegrationTestCallOwnership0003::~CCTSYIntegrationTestCallOwnership0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallOwnership0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-COWN-0003
 * @SYMFssID BA/CTSY/COWN-0003
 * @SYMTestCaseDesc Transfer ownership of a data and fax call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::TransferOwnership, RCall::Dial, RCall::HangUp, RCall::GetOwnershipStatus
 * @SYMTestExpectedResults Pass - Call ownership transferred by call owner when another client is interested.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify ownership can only be transferred by call owner when another client is interested.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get data line 1. 

	// Get call 1. 

	// Get data line 2 on alternate server. 

	// Get call 2 on the alternate server. Call 2 should open existing call with call 1's name. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  call 1 dials, then call 2 requests ownership ===

	// Dial a number that answers using call 1 

	// Check RCall::GetOwnershipStatus on call 1 returns EOwnershipOwnedByThisClient

	// ===  Call 1 transfers ownership when client is not interested ===

	// Check RCall::TransferOwnership() on call 1 returns KErrEtelNoClientInterestedInThisCall

	// ===  Call 2 attempts to transfer ownership when it is not call owner ===

	// Check RCall::TransferOwnership on call 2 returns KErrEtelNotCallOwner

	// Call RCall::AcquireOwnership on call 2 

	// ===  Call 1 transfers ownership when client is interested ===

	// Check RCall::TransferOwnership() on call 1 returns KErrNone

	// ===  Check ownership transfer is successfull ===

	// Check RCall::AcquireOwnership on call 2 completes with KErrNone

	// Check RCall::GetOwnershipStatus on call 2 returns EOwnershipOwnedByThisClient
	// Check RCall::GetOwnershipStatus on call 1 returns EOwnershipOwnedByAnotherClient

	// ===  call 2 has ownership. Hang up call ===

	// Hang up call 2. 

	// Repeat test with a fax call. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallOwnership0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallOwnership0003");
	}



CCTSYIntegrationTestCallOwnership0004::CCTSYIntegrationTestCallOwnership0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallOwnershipBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallOwnership0004::GetTestStepName());
	}

CCTSYIntegrationTestCallOwnership0004::~CCTSYIntegrationTestCallOwnership0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallOwnership0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-COWN-0004
 * @SYMFssID BA/CTSY/COWN-0004
 * @SYMTestCaseDesc Cancel acquire ownership of a data and fax call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::AcquireOwnership, RCall::AcquireOwnershipCancel, RCall::GetOwnershipStatus
 * @SYMTestExpectedResults Pass - Acquire ownership request cancelled.
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

	
	// Get data line 1. 

	// Get call 1. 

	// Get data line 2 on alternate server. 

	// Get call 2 on the alternate server. Call 2 should open existing call with call 1's name. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial a number that answers using lcall1 and dialReqStatus 

	// Call RCall::AcquireOwnership on call 2 

	// Check RCall::AcquireOwnershipCancel on call 2 completes with KErrCancel

	// Check RCall::GetOwnershipStatus on call 1 returns EOwnershipOwnedByThisClient
	// Check RCall::GetOwnershipStatus on call 2 returns EOwnershipOwnedByAnotherClient

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallOwnership0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallOwnership0004");
	}



CCTSYIntegrationTestCallOwnership0005::CCTSYIntegrationTestCallOwnership0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallOwnershipBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallOwnership0005::GetTestStepName());
	}

CCTSYIntegrationTestCallOwnership0005::~CCTSYIntegrationTestCallOwnership0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallOwnership0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-COWN-0005
 * @SYMFssID BA/CTSY/COWN-0005
 * @SYMTestCaseDesc Cancel transfer ownership of a data and fax call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::AcquireOwnership, RCall::TransferOwnership, RCall::TransferOwnershipCancel
 * @SYMTestExpectedResults Pass - Transfer ownership cancelled.
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

	
	// Get data line 1. 

	// Get call 1. 

	// Get data line 2 on alternate server. 

	// Get call 2 on the alternate server. Call 2 should open existing call with call 1's name. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial a number that answers using lcall1 and dialReqStatus 

	// Call RCall::AcquireOwnership on call 2 

	// Call RCall::TransferOwnership on call 1. 

	// Call RCall::TransferOwnershipCancel on call 1 completes with KErrCancel | KErrNone  

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallOwnership0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallOwnership0005");
	}



