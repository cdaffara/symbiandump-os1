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
// Test step definitions for the CallCompletion functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallcompletion.h"



	
CCTSYIntegrationTestCallCompletionBase::CCTSYIntegrationTestCallCompletionBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCallCompletionBase::~CCTSYIntegrationTestCallCompletionBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestCallCompletion0001::CCTSYIntegrationTestCallCompletion0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0001::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0001::~CCTSYIntegrationTestCallCompletion0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0001
 * @SYMFssID BA/CTSY/CCOM-0001
 * @SYMTestCaseDesc CCBS Recall
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial, RMobileCall::ActivateCCBS, RMobilePhone::NotifyCCBSStatusChange, RMobilePhone::AcceptCCBSRecall, RMobilePhone::DeactivateCCBS, RMobilePhone::GetCCBSStatus, RCall::GetStatus
 * @SYMTestExpectedResults Pass - CCBS status is correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify CCBS status is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial RPS. RPS should return busy signal. 

	// Verify call caps notification completes and call has KCapsActivateCCBS capability. 

	// Activate CCBS. 

	// Get the CCBS status and verify it is correct. 

	// Request for CCBS status change notification and wait for RPS to be free. 

	// Accept CCBS recall. 

	// Get call status and verify it is connected. 

	// Deactivate CCBS. 

	// Attempt to deactivate with invalid index, verify error returned. 

	// Get the CCBS status and verify it is correct. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0001");
	}



CCTSYIntegrationTestCallCompletion0002::CCTSYIntegrationTestCallCompletion0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0002::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0002::~CCTSYIntegrationTestCallCompletion0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0002
 * @SYMFssID BA/CTSY/CCOM-0002
 * @SYMTestCaseDesc iTest.Printf(_L("1\tA maximum of 5 CCBS entries can be retrieved\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0002");
	}



CCTSYIntegrationTestCallCompletion0003::CCTSYIntegrationTestCallCompletion0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0003::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0003::~CCTSYIntegrationTestCallCompletion0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0003
 * @SYMFssID BA/CTSY/CCOM-0003
 * @SYMTestCaseDesc iTest.Printf(_L("5\tAccept a CCBS recall\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0003");
	}



CCTSYIntegrationTestCallCompletion0004::CCTSYIntegrationTestCallCompletion0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0004::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0004::~CCTSYIntegrationTestCallCompletion0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0004
 * @SYMFssID BA/CTSY/CCOM-0004
 * @SYMTestCaseDesc iTest.Printf(_L("6\tRefuse a CCBS recall\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0004");
	}



CCTSYIntegrationTestCallCompletion0005::CCTSYIntegrationTestCallCompletion0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0005::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0005::~CCTSYIntegrationTestCallCompletion0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0005
 * @SYMFssID BA/CTSY/CCOM-0005
 * @SYMTestCaseDesc iTest.Printf(_L("7\tAccept a CCBS recall using invalid index\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0005");
	}



CCTSYIntegrationTestCallCompletion0006::CCTSYIntegrationTestCallCompletion0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0006::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0006::~CCTSYIntegrationTestCallCompletion0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0006
 * @SYMFssID BA/CTSY/CCOM-0006
 * @SYMTestCaseDesc iTest.Printf(_L("8\tRefuse a CCBS recall using invalid index\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0006");
	}



CCTSYIntegrationTestCallCompletion0007::CCTSYIntegrationTestCallCompletion0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0007::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0007::~CCTSYIntegrationTestCallCompletion0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0007
 * @SYMFssID BA/CTSY/CCOM-0007
 * @SYMTestCaseDesc iTest.Printf(_L("9\tAccept a CCBS recall when more than one outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0007");
	}



CCTSYIntegrationTestCallCompletion0008::CCTSYIntegrationTestCallCompletion0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0008::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0008::~CCTSYIntegrationTestCallCompletion0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0008
 * @SYMFssID BA/CTSY/CCOM-0008
 * @SYMTestCaseDesc iTest.Printf(_L("10\tRefuse a CCBS recall when more than one outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0008");
	}



CCTSYIntegrationTestCallCompletion0009::CCTSYIntegrationTestCallCompletion0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0009::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0009::~CCTSYIntegrationTestCallCompletion0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0009
 * @SYMFssID BA/CTSY/CCOM-0009
 * @SYMTestCaseDesc iTest.Printf(_L("3\tReject a CCBS recall\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0009");
	}



CCTSYIntegrationTestCallCompletion0010::CCTSYIntegrationTestCallCompletion0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0010::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0010::~CCTSYIntegrationTestCallCompletion0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0010
 * @SYMFssID BA/CTSY/CCOM-0010
 * @SYMTestCaseDesc iTest.Printf(_L("4\tActivate a CCBS request when not available\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0010");
	}



CCTSYIntegrationTestCallCompletion0011::CCTSYIntegrationTestCallCompletion0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCompletionBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCompletion0011::GetTestStepName());
	}

CCTSYIntegrationTestCallCompletion0011::~CCTSYIntegrationTestCallCompletion0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCompletion0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOM-0011
 * @SYMFssID BA/CTSY/CCOM-0011
 * @SYMTestCaseDesc iTest.Printf(_L("5\tReject a CCBS request when not available\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *

 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Same scenario as TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCompletion0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCompletion0011");
	}



