// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file	Test step definitions for the USimApplication functional unit.
 @internalTechnology
*/
 
#include "cctsyintegrationtestusimapplication.h"

#include "cctsyinidata.h"

CCTSYIntegrationTestUSimApplicationBase::CCTSYIntegrationTestUSimApplicationBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)  
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestUSimApplicationBase::~CCTSYIntegrationTestUSimApplicationBase()
/*
 * Destructor
 */
	{
	}

CCTSYIntegrationTestUSimApplication0001::CCTSYIntegrationTestUSimApplication0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUSimApplicationBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUSimApplication0001::GetTestStepName());
	}

CCTSYIntegrationTestUSimApplication0001::~CCTSYIntegrationTestUSimApplication0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUSimApplication0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USIM-0001
 * @SYMFssID BA/CTSY/USIM-0001
 * @SYMTestCaseDesc Get the currrent active usim application id
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - current active usim application id returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify current active usim application id returned.
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
		
	// ===  Get the current active usim application id ===

	// Check RMobilePhone::GetCurrentActiveUSimApplication returns aid with length > 0
    RMobilePhone::TAID aid;	
	TExtEtelRequestStatus requestStatus(phone, EMobilePhoneGetCurrentActiveUSimApplication);
	CleanupStack::PushL(requestStatus);	
	phone.GetCurrentActiveUSimApplication(requestStatus, aid);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestStatus, ETimeLong), 
			KErrNone, _L("RMobilePhone::GetCurrentActiveUSimApplication timed out."))
	ASSERT_EQUALS(requestStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentActiveUSimApplication completed with incorrect status")) 
	ASSERT_TRUE(aid.Length() > 0, 
				_L("RMobilePhone::GetCurrentActiveUSimApplication returned invalid current active usim app id"))	
	
		
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
    // requestStatus
	CleanupStack::Pop();

	return TestStepResult();
	}
	

TPtrC CCTSYIntegrationTestUSimApplication0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUSimApplication0001");
	}
