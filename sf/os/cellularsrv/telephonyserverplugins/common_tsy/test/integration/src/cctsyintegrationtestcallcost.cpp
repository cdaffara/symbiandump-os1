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
// Test step definitions for the CallCost functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallcost.h"
#include "cetelsessionmgr.h"


	
CCTSYIntegrationTestCallCostBase::CCTSYIntegrationTestCallCostBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCallCostBase::~CCTSYIntegrationTestCallCostBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestCallCost0001::CCTSYIntegrationTestCallCost0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0001::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0001::~CCTSYIntegrationTestCallCost0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0001
 * @SYMFssID BA/CTSY/CCOS-0001
 * @SYMTestCaseDesc Ensure that AoCC (advice of charge) information is correctly updated during an MO call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial, RCall:HangUp, RMobilePhone::ClearCostMeter
 * @SYMTestExpectedResults Pass - Call cost information can be retrieved and meter can be cleared.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has KCapsClearCost, KCapsCostCharging caps. AoCC configured. 

	// Get voice line 1. Get call 1. 

	// Clear the cost meter. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial a number that is chargable. 

	// Get the cost information during the call. 

	// Hang up the call. 

	// Get the cost information. 

	// Clear the cost meter. 

	// Dial a number that is free of charge. 

	// Get the cost information during the call. 

	// Hang up the call. 

	// Get the cost information. 

	// Repeat the whole test with data and fax calls. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0001");
	}



CCTSYIntegrationTestCallCost0002::CCTSYIntegrationTestCallCost0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0002::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0002::~CCTSYIntegrationTestCallCost0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0002
 * @SYMFssID BA/CTSY/CCOS-0002
 * @SYMTestCaseDesc Ensure that AoCC (advice of charge) information is correctly updated during an MT call.
 * @SYMTestPriority High
 * @SYMTestActions RCall:HangUp, RMobilePhone::ClearCostMeter, RCall:AnswerIncomingCall, RMobilePhone::NotifyCostInfoChange
 * @SYMTestExpectedResults Pass - Call cost information can be retrieved and meter can be cleared.
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

	
	// Ensure phone has KCapsClearCost and KCapsCostCharging caps. AoCC configured. 

	// Get voice line 1. Get call 1. 

	// Clear the cost meter. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Arrange for RPS to invoke an incoming call that should be charged. 

	// Answer the incoming call (call 1). 

	// Get the cost information. 

	// Verify notification completes and ACM and CCM info changed. 

	// Hang up the call (call 1). 

	// Get the cost information. 

	// Clear cost meter. 

	// Verify notification completes and CCM is 0. 

	// Arrange for RPS to simulate an incoming call that is free. 

	// Answer the incoming call (call 1).  

	// Get the cost information. Verify no charge. 

	// Hang up the call. 

	// Get the cost information. Verify no charge. 

	// Repeat whole test for data and fax calls. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0002");
	}



CCTSYIntegrationTestCallCost0003::CCTSYIntegrationTestCallCost0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0003::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0003::~CCTSYIntegrationTestCallCost0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0003
 * @SYMFssID BA/CTSY/CCOS-0003
 * @SYMTestCaseDesc Ensure that AoCI information is correctly updated during an MO call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial, RCall:HangUp, RMobilePhone::ClearCostMeter, RMobilePhone::NotifyCostInfoChange
 * @SYMTestExpectedResults Pass - Call cost information can be retrieved and meter can be cleared.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has KCapsClearCost and KCapsCostInformation caps. AoCI configured. 

	// Get voice line 1. Get call 1. 

	// Clear the cost meter. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Dial a number that is chargable. 

	// Get the cost information during the call. 

	// Hang up the call. 

	// Get the cost information. 

	// Clear the cost meter. 

	// Dial a number that is free of charge. 

	// Get the cost information during the call. 

	// Hang up the call. 

	// Get the cost information. 

	// Repeat the whole test with data and fax calls. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0003");
	}



CCTSYIntegrationTestCallCost0004::CCTSYIntegrationTestCallCost0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0004::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0004::~CCTSYIntegrationTestCallCost0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0004
 * @SYMFssID BA/CTSY/CCOS-0004
 * @SYMTestCaseDesc Ensure that AoCI information is correctly updated during an MT call.
 * @SYMTestPriority High
 * @SYMTestActions RCall:HangUp, RMobilePhone::ClearCostMeter, RCall:AnswerIncomingCall
 * @SYMTestExpectedResults Pass - Call cost information can be retrieved and meter can be cleared.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone has KCapsClearCost and KCapsCostInformation caps. AoCI configured. 

	// Get voice line 1. Get call 1. 

	// Clear the cost meter. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Arrange for RPS to simulate an incoming call that should be charged. 

	// Answer the incoming call (call 1). 

	// Get the cost information. 

	// Hang up the call (call 1). 

	// Get the cost information. 

	// Clear cost meter. 

	// Arrange for RPS to simulate an incoming call that is free. 

	// Answer the incoming call (call 1).  

	// Get the cost information. Verify no charge. 

	// Hang up the call. 

	// Get the cost information. Verify no charge. 

	// Repeat whole test for data and fax calls. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0004");
	}



CCTSYIntegrationTestCallCost0005::CCTSYIntegrationTestCallCost0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0005::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0005::~CCTSYIntegrationTestCallCost0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0005
 * @SYMFssID BA/CTSY/CCOS-0005
 * @SYMTestCaseDesc iTest.Printf(_L("3:\tCost Meter, clear when no security event outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0005");
	}



CCTSYIntegrationTestCallCost0006::CCTSYIntegrationTestCallCost0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0006::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0006::~CCTSYIntegrationTestCallCost0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0006
 * @SYMFssID BA/CTSY/CCOS-0006
 * @SYMTestCaseDesc iTest.Printf(_L("4:\tCost Meter, clear when a security event is outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0006");
	}



CCTSYIntegrationTestCallCost0007::CCTSYIntegrationTestCallCost0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0007::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0007::~CCTSYIntegrationTestCallCost0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0007
 * @SYMFssID BA/CTSY/CCOS-0007
 * @SYMTestCaseDesc iTest.Printf(_L("5:\tCost Meter, cancel clear request when security event is notified\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0007");
	}



CCTSYIntegrationTestCallCost0008::CCTSYIntegrationTestCallCost0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0008::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0008::~CCTSYIntegrationTestCallCost0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0008
 * @SYMFssID BA/CTSY/CCOS-0008
 * @SYMTestCaseDesc iTest.Printf(_L("6:\tCost Meter, cancel clear request when security event is verified\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0008");
	}



CCTSYIntegrationTestCallCost0009::CCTSYIntegrationTestCallCost0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0009::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0009::~CCTSYIntegrationTestCallCost0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0009
 * @SYMFssID BA/CTSY/CCOS-0009
 * @SYMTestCaseDesc iTest.Printf(_L("7:\tCost Meter, set max when no security event outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0009");
	}



CCTSYIntegrationTestCallCost0010::CCTSYIntegrationTestCallCost0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0010::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0010::~CCTSYIntegrationTestCallCost0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0010
 * @SYMFssID BA/CTSY/CCOS-0010
 * @SYMTestCaseDesc iTest.Printf(_L("8:\tCost Meter, set max when a security event is outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0010");
	}



CCTSYIntegrationTestCallCost0011::CCTSYIntegrationTestCallCost0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0011::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0011::~CCTSYIntegrationTestCallCost0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0011
 * @SYMFssID BA/CTSY/CCOS-0011
 * @SYMTestCaseDesc iTest.Printf(_L("9:\tCost Meter, cancel set max request when security event is notified\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0011");
	}



CCTSYIntegrationTestCallCost0012::CCTSYIntegrationTestCallCost0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0012::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0012::~CCTSYIntegrationTestCallCost0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0012
 * @SYMFssID BA/CTSY/CCOS-0012
 * @SYMTestCaseDesc iTest.Printf(_L("10:\tCost Meter, cancel set max request when security event is verified\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0012");
	}



CCTSYIntegrationTestCallCost0013::CCTSYIntegrationTestCallCost0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0013::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0013::~CCTSYIntegrationTestCallCost0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0013
 * @SYMFssID BA/CTSY/CCOS-0013
 * @SYMTestCaseDesc iTest.Printf(_L("12:\tCost Meter, set puct when no security event outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0013");
	}



CCTSYIntegrationTestCallCost0014::CCTSYIntegrationTestCallCost0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0014::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0014::~CCTSYIntegrationTestCallCost0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0014
 * @SYMFssID BA/CTSY/CCOS-0014
 * @SYMTestCaseDesc iTest.Printf(_L("13:\tCost Meter, set puct when a security event is outstanding\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0014");
	}



CCTSYIntegrationTestCallCost0015::CCTSYIntegrationTestCallCost0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0015::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0015::~CCTSYIntegrationTestCallCost0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0015
 * @SYMFssID BA/CTSY/CCOS-0015
 * @SYMTestCaseDesc iTest.Printf(_L("14:\tCost Meter, cancel set puct request when security event is notified\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0015");
	}



CCTSYIntegrationTestCallCost0016::CCTSYIntegrationTestCallCost0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0016::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0016::~CCTSYIntegrationTestCallCost0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0016
 * @SYMFssID BA/CTSY/CCOS-0016
 * @SYMTestCaseDesc iTest.Printf(_L("15:\tCost Meter, cancel set puct request when security event is verified\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0016");
	}



CCTSYIntegrationTestCallCost0017::CCTSYIntegrationTestCallCost0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0017::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0017::~CCTSYIntegrationTestCallCost0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0017
 * @SYMFssID BA/CTSY/CCOS-0017
 * @SYMTestCaseDesc iTest.Printf(_L("16:\tCost Meter, set puct to 0\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0017");
	}



CCTSYIntegrationTestCallCost0018::CCTSYIntegrationTestCallCost0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0018::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0018::~CCTSYIntegrationTestCallCost0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0018
 * @SYMFssID BA/CTSY/CCOS-0018
 * @SYMTestCaseDesc iTest.Printf(_L("18:\tCost Info, emergency call when max cost reached\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0018");
	}



CCTSYIntegrationTestCallCost0019::CCTSYIntegrationTestCallCost0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0019::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0019::~CCTSYIntegrationTestCallCost0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0019
 * @SYMFssID BA/CTSY/CCOS-0019
 * @SYMTestCaseDesc iTest.Printf(_L("101:\tCost Meter, clear when cost info not supported\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0019");
	}



CCTSYIntegrationTestCallCost0020::CCTSYIntegrationTestCallCost0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0020::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0020::~CCTSYIntegrationTestCallCost0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0020
 * @SYMFssID BA/CTSY/CCOS-0020
 * @SYMTestCaseDesc iTest.Printf(_L("102:\tCost Meter, set max when cost info not supported\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0020");
	}



CCTSYIntegrationTestCallCost0021::CCTSYIntegrationTestCallCost0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0021::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0021::~CCTSYIntegrationTestCallCost0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0021
 * @SYMFssID BA/CTSY/CCOS-0021
 * @SYMTestCaseDesc iTest.Printf(_L("103:\tCost Meter, set puct when cost info not supported\n"));
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
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
	
	
	// See TRP test 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0021");
	}



CCTSYIntegrationTestCallCost0022::CCTSYIntegrationTestCallCost0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0022::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0022::~CCTSYIntegrationTestCallCost0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0022
 * @SYMFssID BA/CTSY/CCOS-0022
 * @SYMTestCaseDesc Get call cost capabilities.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNotSupported returned
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
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetCostCaps returns KErrNotSupported
	TUint32 costCaps=5;
	TInt err=phone.GetCostCaps(costCaps);
	//We only check the return code as the caps could be slightly different for each TSY.
	ASSERT_EQUALS(err,KErrNotSupported, _L("RMobilePhone::GetCostCaps completed with error."))
		
	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0022");
	}



CCTSYIntegrationTestCallCost0023::CCTSYIntegrationTestCallCost0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallCostBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallCost0023::GetTestStepName());
	}

CCTSYIntegrationTestCallCost0023::~CCTSYIntegrationTestCallCost0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallCost0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CCOS-0023
 * @SYMFssID BA/CTSY/CCOS-0023
 * @SYMTestCaseDesc Get cost information.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCostInfo
 * @SYMTestExpectedResults Pass - KErrNotSupported returned
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
	//Get cost information.
	//Verify KErrNotSupported is returned.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetCostInfo returns KErrNotSupported

	TExtEtelRequestStatus requestStatus(mobilePhone,EMobilePhoneGetCostInfo);	
	RMobilePhone::TMobilePhoneCostInfoV1  costInfo;
	RMobilePhone::TMobilePhoneCostInfoV1Pckg costInfoPk(costInfo);

	mobilePhone.GetCostInfo(requestStatus, costInfoPk);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestStatus, ETimeShort), 
			KErrNone, _L("RMobilePhone::GetCostInfo did not complete."))
    ASSERT_EQUALS(requestStatus.Int(),KErrNotSupported, 
    		_L("RMobilePhone::GetCostInfo completed with incorrect error"))
	//
	// TEST END
	//

    StartCleanup();
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallCost0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallCost0023");
	}



