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
// Test step definitions for the IMS functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestims.h"



	
CCTSYIntegrationTestIMSBase::CCTSYIntegrationTestIMSBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestIMSBase::~CCTSYIntegrationTestIMSBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestIMS0001::CCTSYIntegrationTestIMS0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestIMSBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestIMS0001::GetTestStepName());
	}

CCTSYIntegrationTestIMS0001::~CCTSYIntegrationTestIMS0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestIMS0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-IMS-0001
 * @SYMFssID BA/CTSY/IMS-0001
 * @SYMTestCaseDesc Authenticate ISIM
 * @SYMTestPriority High
 * @SYMTestActions CAsyncRetrieveAuthorizationInfo::Start
 * @SYMTestExpectedResults Pass - IMS authenticate completes with KErrNone. Authorization info retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify ISIM authenticated
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Insert 3G ISIM with smart card applications 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send IMS authenticate command with RMobilePhone::ImsAuthenticate with valid iAUTN,iAUTS,iRAND in RMobilePhone::TImsAuthenticateDataV5 

	// Check request completes with KErrNone.

	// ===  Check authenticate data populated ===

	// Check RMobilePhone::TImsAuthenticateDataV5::iCK has length > 0

	// Check RMobilePhone::TImsAuthenticateDataV5::iIK has length > 0

	// Check RMobilePhone::TImsAuthenticateDataV5::iRES has length > 0

	// Use the helper class for CAsyncRetrieveAuthorizationInfo to start retrieval of the authorization info list from CAsyncRetrieveAuthorizationInfo::Start 

	// ===  Check the RMobilePhone::CImsAuthorizationInfoV5 contains valid information ===

	// Check iIMPI has length > 0

	// Check iIMPUArray.Count() > 0

	// Check each item in iIMPUArray has length > 0

	// Check iHomeNetworkDomainName has length > 0

	// Check iAuthenticationDataSource has length > 0

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestIMS0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestIMS0001");
	}



