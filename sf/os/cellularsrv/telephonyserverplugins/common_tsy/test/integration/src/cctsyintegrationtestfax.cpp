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
// Test step definitions for the Fax functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestfax.h"

#include "cctsyinidata.h"



	
CCTSYIntegrationTestFaxBase::CCTSYIntegrationTestFaxBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestFaxBase::~CCTSYIntegrationTestFaxBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestFax0001::CCTSYIntegrationTestFax0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestFaxBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestFax0001::GetTestStepName());
	}

CCTSYIntegrationTestFax0001::~CCTSYIntegrationTestFax0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestFax0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-FAX-0001
 * @SYMFssID BA/CTSY/FAX-0001
 * @SYMTestCaseDesc MO fax call to fax device
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Fax is successfully sent
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify MO fax call succeeds.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	

	// Ensure phone supports KCapsFax.
	RMobilePhone::TCaps mobilePhoneCaps;
	
	CHECK_TRUE_L(mobilePhone.GetCaps(mobilePhoneCaps) == KErrNone, _L("RMobilePhone::GetCaps"));
	TUint32 expectedMobilePhoneCaps = RLine::KCapsFax;
	CHECK_BITS_SET_L(mobilePhoneCaps.iFlags, expectedMobilePhoneCaps, KNoWantedBits, _L("RMobilePhone::GetCaps returned wrong dynamic caps"));	
		
	// Get Fax Line
	RMobileLine& faxLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KFaxLine);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Try to get Fax.	 
	TRAPD (err, RFax& mainFax = iEtelSessionMgr.GetFaxL (KMainServer, KMainPhone, KFaxLine, KCall1, KMainFax ));
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10051);
	ASSERT_EQUALS(KErrNone, err, _L("GetFaxL() problem opening "));
	
	// Use te_fax test	
	
	//
	// TEST END
	//

	StartCleanup();	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestFax0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestFax0001");
	}



CCTSYIntegrationTestFax0002::CCTSYIntegrationTestFax0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestFaxBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestFax0002::GetTestStepName());
	}

CCTSYIntegrationTestFax0002::~CCTSYIntegrationTestFax0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestFax0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-FAX-0002
 * @SYMFssID BA/CTSY/FAX-0002
 * @SYMTestCaseDesc MT fax call from fax device
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Fax is successfully received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify MT fax call succeeds.
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	

	// Ensure phone supports KCapsFax.
	RMobilePhone::TCaps mobilePhoneCaps;
	
	CHECK_TRUE_L(mobilePhone.GetCaps(mobilePhoneCaps) == KErrNone, _L("RMobilePhone::GetCaps"));
	TUint32 expectedMobilePhoneCaps = RLine::KCapsFax;
	CHECK_BITS_SET_L(mobilePhoneCaps.iFlags, expectedMobilePhoneCaps, KNoWantedBits, _L("RMobilePhone::GetCaps returned wrong dynamic caps"));	
		
	// Get Fax Line
	RMobileLine& faxLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KFaxLine);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Try to get Fax.	 
	TRAPD (err, RFax& mainFax = iEtelSessionMgr.GetFaxL (KMainServer, KMainPhone, KFaxLine, KCall1, KMainFax ));
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10051);
	ASSERT_EQUALS(KErrNone, err, _L("GetFaxL() problem opening "));			

	// Use te_fax test
	
	//
	// TEST END
	//

	StartCleanup();	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestFax0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestFax0002");
	}



