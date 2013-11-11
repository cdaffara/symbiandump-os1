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
// Test step definitions for the CustomAPI functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcustomapi.h"

#include <ctsy/rmmcustomapi.h>


CCTSYIntegrationTestCustomAPIBase::CCTSYIntegrationTestCustomAPIBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iCustomApiTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCustomAPIBase::~CCTSYIntegrationTestCustomAPIBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestCustomAPI0001::CCTSYIntegrationTestCustomAPI0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0001::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0001::~CCTSYIntegrationTestCustomAPI0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0001
 * @SYMFssID BA/CTSY/CAPI-0001
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SetDriveMode
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SetDriveMode
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMmCustomAPI& customApi = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone, KMainCustomApi);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Test calling RMmCustomAPI::SetDriveMode 
    TExtEtelRequestStatus setDriveModeStatus(customApi, ECustomSetDriveModeIPC);
	CleanupStack::PushL(setDriveModeStatus);
    RMmCustomAPI::TSetDriveMode driveMode = RMmCustomAPI::EActivateDriveMode;
    customApi.SetDriveMode(setDriveModeStatus, driveMode);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(setDriveModeStatus, ETimeMedium), KErrNone, _L("RMmCustomAPI::SetDriveMode timed out"));
    ASSERT_EQUALS(setDriveModeStatus.Int(), KErrNone, _L("RMmCustomAPI::SetDriveMode returned an error"));

	
	//
	// TEST END
	//

    StartCleanup();

    // setDriveModeStatus
	CleanupStack::PopAndDestroy(1, &setDriveModeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0001");
	}



CCTSYIntegrationTestCustomAPI0002::CCTSYIntegrationTestCustomAPI0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0002::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0002::~CCTSYIntegrationTestCustomAPI0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0002
 * @SYMFssID BA/CTSY/CAPI-0002
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ActivateSimLock
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ActivateSimLock
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
	
	
	// Test calling RMmCustomAPI::ActivateSimLock 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0002");
	}



CCTSYIntegrationTestCustomAPI0003::CCTSYIntegrationTestCustomAPI0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0003::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0003::~CCTSYIntegrationTestCustomAPI0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0003
 * @SYMFssID BA/CTSY/CAPI-0003
 * @SYMTestCaseDesc Test calling RMmCustomAPI::DeActivateSimLock
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::DeActivateSimLock
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
	
	
	// Test calling RMmCustomAPI::DeActivateSimLock 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0003");
	}



CCTSYIntegrationTestCustomAPI0004::CCTSYIntegrationTestCustomAPI0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0004::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0004::~CCTSYIntegrationTestCustomAPI0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0004
 * @SYMFssID BA/CTSY/CAPI-0004
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyDtmfEvent
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyDtmfEvent
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
	
	
	// Test calling RMmCustomAPI::NotifyDtmfEvent 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0004");
	}



CCTSYIntegrationTestCustomAPI0005::CCTSYIntegrationTestCustomAPI0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0005::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0005::~CCTSYIntegrationTestCustomAPI0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0005
 * @SYMFssID BA/CTSY/CAPI-0005
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetDiagnosticOctets
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetDiagnosticOctets
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
	
	
	// Test calling RMmCustomAPI::GetDiagnosticOctets 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0005");
	}



CCTSYIntegrationTestCustomAPI0006::CCTSYIntegrationTestCustomAPI0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0006::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0006::~CCTSYIntegrationTestCustomAPI0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0006
 * @SYMFssID BA/CTSY/CAPI-0006
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SetAlsBlocked
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test calling RMmCustomAPI::SetAlsBlocked
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
	
	
	// Test calling RMmCustomAPI::SetAlsBlocked 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0006");
	}



CCTSYIntegrationTestCustomAPI0007::CCTSYIntegrationTestCustomAPI0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0007::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0007::~CCTSYIntegrationTestCustomAPI0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0007
 * @SYMFssID BA/CTSY/CAPI-0007
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetAlsBlocked
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test calling RMmCustomAPI::GetAlsBlocked
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
	
	
	// Test calling RMmCustomAPI::GetAlsBlocked 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0007");
	}



CCTSYIntegrationTestCustomAPI0008::CCTSYIntegrationTestCustomAPI0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0008::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0008::~CCTSYIntegrationTestCustomAPI0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0008
 * @SYMFssID BA/CTSY/CAPI-0008
 * @SYMTestCaseDesc Test calling RMmCustomAPI::CheckAlsPpSupport
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test calling RMmCustomAPI::CheckAlsPpSupport
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
	
	
	// Test calling RMmCustomAPI::CheckAlsPpSupport 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0008");
	}



CCTSYIntegrationTestCustomAPI0009::CCTSYIntegrationTestCustomAPI0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0009::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0009::~CCTSYIntegrationTestCustomAPI0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0009
 * @SYMFssID BA/CTSY/CAPI-0009
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetRemoteAlertingToneStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetRemoteAlertingToneStatus
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
	
	
	// Test calling RMmCustomAPI::GetRemoteAlertingToneStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0009");
	}



CCTSYIntegrationTestCustomAPI0010::CCTSYIntegrationTestCustomAPI0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0010::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0010::~CCTSYIntegrationTestCustomAPI0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0010
 * @SYMFssID BA/CTSY/CAPI-0010
 * @SYMTestCaseDesc Test calling RMmCustomAPI::CallOrigin
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::CallOrigin
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
	
	
	// Test calling RMmCustomAPI::CallOrigin 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0010");
	}



CCTSYIntegrationTestCustomAPI0011::CCTSYIntegrationTestCustomAPI0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0011::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0011::~CCTSYIntegrationTestCustomAPI0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0011
 * @SYMFssID BA/CTSY/CAPI-0011
 * @SYMTestCaseDesc Test calling RMmCustomAPI::TerminateCall
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::TerminateCall
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
	
	
	// Test calling RMmCustomAPI::TerminateCall 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0011");
	}



CCTSYIntegrationTestCustomAPI0012::CCTSYIntegrationTestCustomAPI0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0012::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0012::~CCTSYIntegrationTestCustomAPI0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0012
 * @SYMFssID BA/CTSY/CAPI-0012
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyAlsBlockedChanged
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyAlsBlockedChanged
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
	
	
	// Test calling RMmCustomAPI::NotifyAlsBlockedChanged 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0012");
	}



CCTSYIntegrationTestCustomAPI0013::CCTSYIntegrationTestCustomAPI0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0013::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0013::~CCTSYIntegrationTestCustomAPI0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0013
 * @SYMFssID BA/CTSY/CAPI-0013
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetCipheringInfo
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetCipheringInfo
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
	
	
	// Test calling RMmCustomAPI::GetCipheringInfo 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0013");
	}



CCTSYIntegrationTestCustomAPI0014::CCTSYIntegrationTestCustomAPI0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0014::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0014::~CCTSYIntegrationTestCustomAPI0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0014
 * @SYMFssID BA/CTSY/CAPI-0014
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyCipheringInfoChange
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyCipheringInfoChange
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
	
	
	// Test calling RMmCustomAPI::NotifyCipheringInfoChange 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0014");
	}



CCTSYIntegrationTestCustomAPI0015::CCTSYIntegrationTestCustomAPI0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0015::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0015::~CCTSYIntegrationTestCustomAPI0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0015
 * @SYMFssID BA/CTSY/CAPI-0015
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyNSPSStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyNSPSStatus
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
	
	
	// Test calling RMmCustomAPI::NotifyNSPSStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0015");
	}



CCTSYIntegrationTestCustomAPI0016::CCTSYIntegrationTestCustomAPI0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0016::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0016::~CCTSYIntegrationTestCustomAPI0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0016
 * @SYMFssID BA/CTSY/CAPI-0016
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NetWakeup
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::NetWakeup
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
	
	
	// Test calling RMmCustomAPI::NetWakeup 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0016");
	}



CCTSYIntegrationTestCustomAPI0017::CCTSYIntegrationTestCustomAPI0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0017::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0017::~CCTSYIntegrationTestCustomAPI0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0017
 * @SYMFssID BA/CTSY/CAPI-0017
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ReadViagHomeZoneParams
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::ReadViagHomeZoneParams
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
	
	
	// Test calling RMmCustomAPI::ReadViagHomeZoneParams 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0017");
	}



CCTSYIntegrationTestCustomAPI0018::CCTSYIntegrationTestCustomAPI0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0018::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0018::~CCTSYIntegrationTestCustomAPI0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0018
 * @SYMFssID BA/CTSY/CAPI-0018
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ReadViagHomeZoneCache
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::ReadViagHomeZoneCache
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
	
	
	// Test calling RMmCustomAPI::ReadViagHomeZoneCache 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0018");
	}



CCTSYIntegrationTestCustomAPI0019::CCTSYIntegrationTestCustomAPI0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0019::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0019::~CCTSYIntegrationTestCustomAPI0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0019
 * @SYMFssID BA/CTSY/CAPI-0019
 * @SYMTestCaseDesc Test calling RMmCustomAPI::WriteViagHomeZoneCache
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::WriteViagHomeZoneCache
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
	
	
	// Test calling RMmCustomAPI::WriteViagHomeZoneCache 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0019");
	}



CCTSYIntegrationTestCustomAPI0020::CCTSYIntegrationTestCustomAPI0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0020::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0020::~CCTSYIntegrationTestCustomAPI0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0020
 * @SYMFssID BA/CTSY/CAPI-0020
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ClearCallBlackList
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ClearCallBlackList
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
	
	
	// Test calling RMmCustomAPI::ClearCallBlackList 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0020");
	}



CCTSYIntegrationTestCustomAPI0021::CCTSYIntegrationTestCustomAPI0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0021::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0021::~CCTSYIntegrationTestCustomAPI0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0021
 * @SYMFssID BA/CTSY/CAPI-0021
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SsAdditionalInfoNotification
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::SsAdditionalInfoNotification
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
	
	
	// Test calling RMmCustomAPI::SsAdditionalInfoNotification 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0021");
	}



CCTSYIntegrationTestCustomAPI0022::CCTSYIntegrationTestCustomAPI0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0022::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0022::~CCTSYIntegrationTestCustomAPI0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0022
 * @SYMFssID BA/CTSY/CAPI-0022
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SsRequestCompleteNotification
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SsRequestCompleteNotification
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
	
	
	// Test calling RMmCustomAPI::SsRequestCompleteNotification 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0022");
	}



CCTSYIntegrationTestCustomAPI0023::CCTSYIntegrationTestCustomAPI0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0023::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0023::~CCTSYIntegrationTestCustomAPI0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0023
 * @SYMFssID BA/CTSY/CAPI-0023
 * @SYMTestCaseDesc Test calling RMmCustomAPI::IsBlocked
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::IsBlocked
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
	
	
	// Test calling RMmCustomAPI::IsBlocked 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0023");
	}



CCTSYIntegrationTestCustomAPI0024::CCTSYIntegrationTestCustomAPI0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0024::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0024::~CCTSYIntegrationTestCustomAPI0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0024
 * @SYMFssID BA/CTSY/CAPI-0024
 * @SYMTestCaseDesc Test calling RMmCustomAPI::CheckSecurityCode
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::CheckSecurityCode
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
	
	
	// Test calling RMmCustomAPI::CheckSecurityCode 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0024");
	}



CCTSYIntegrationTestCustomAPI0025::CCTSYIntegrationTestCustomAPI0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0025::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0025::~CCTSYIntegrationTestCustomAPI0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0025
 * @SYMFssID BA/CTSY/CAPI-0025
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetActivePin
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetActivePin
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
	
	
	// Test calling RMmCustomAPI::GetActivePin 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0025");
	}



CCTSYIntegrationTestCustomAPI0026::CCTSYIntegrationTestCustomAPI0026(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0026::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0026::~CCTSYIntegrationTestCustomAPI0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0026
 * @SYMFssID BA/CTSY/CAPI-0026
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetAirTimeDuration
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetAirTimeDuration
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
	
	
	// Test calling RMmCustomAPI::GetAirTimeDuration 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0026");
	}



CCTSYIntegrationTestCustomAPI0027::CCTSYIntegrationTestCustomAPI0027(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0027::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0027::~CCTSYIntegrationTestCustomAPI0027()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0027::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0027
 * @SYMFssID BA/CTSY/CAPI-0027
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetNetworkProviderName
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetNetworkProviderName
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
	
	
	// Test calling RMmCustomAPI::GetNetworkProviderName 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0027::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0027");
	}



CCTSYIntegrationTestCustomAPI0028::CCTSYIntegrationTestCustomAPI0028(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0028::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0028::~CCTSYIntegrationTestCustomAPI0028()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0028::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0028
 * @SYMFssID BA/CTSY/CAPI-0028
 * @SYMTestCaseDesc Test calling RMmCustomAPI::CheckEmergencyNumber
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::CheckEmergencyNumber
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
	
	
	// Test calling RMmCustomAPI::CheckEmergencyNumber 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0028::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0028");
	}



CCTSYIntegrationTestCustomAPI0029::CCTSYIntegrationTestCustomAPI0029(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0029::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0029::~CCTSYIntegrationTestCustomAPI0029()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0029::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0029
 * @SYMFssID BA/CTSY/CAPI-0029
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyPndCacheReady
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyPndCacheReady
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
	
	
	// Test calling RMmCustomAPI::NotifyPndCacheReady 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0029::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0029");
	}



CCTSYIntegrationTestCustomAPI0030::CCTSYIntegrationTestCustomAPI0030(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0030::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0030::~CCTSYIntegrationTestCustomAPI0030()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0030::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0030
 * @SYMFssID BA/CTSY/CAPI-0030
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetPndCacheStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetPndCacheStatus
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
	
	
	// Test calling RMmCustomAPI::GetPndCacheStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0030::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0030");
	}



CCTSYIntegrationTestCustomAPI0031::CCTSYIntegrationTestCustomAPI0031(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0031::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0031::~CCTSYIntegrationTestCustomAPI0031()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0031::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0031
 * @SYMFssID BA/CTSY/CAPI-0031
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetOperatorName
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetOperatorName
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
	
	
	// Test calling RMmCustomAPI::GetOperatorName 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0031::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0031");
	}



CCTSYIntegrationTestCustomAPI0032::CCTSYIntegrationTestCustomAPI0032(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0032::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0032::~CCTSYIntegrationTestCustomAPI0032()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0032::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0032
 * @SYMFssID BA/CTSY/CAPI-0032
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetProgrammableOperatorLogo
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetProgrammableOperatorLogo
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
	
	
	// Test calling RMmCustomAPI::GetProgrammableOperatorLogo 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0032::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0032");
	}



CCTSYIntegrationTestCustomAPI0033::CCTSYIntegrationTestCustomAPI0033(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0033::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0033::~CCTSYIntegrationTestCustomAPI0033()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0033::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0033
 * @SYMFssID BA/CTSY/CAPI-0033
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyProgrammableOperatorLogoChange
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyProgrammableOperatorLogoChange
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
	
	
	// Test calling RMmCustomAPI::NotifyProgrammableOperatorLogoChange 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0033::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0033");
	}



CCTSYIntegrationTestCustomAPI0034::CCTSYIntegrationTestCustomAPI0034(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0034::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0034::~CCTSYIntegrationTestCustomAPI0034()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0034::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0034
 * @SYMFssID BA/CTSY/CAPI-0034
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifySsNetworkEvent
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifySsNetworkEvent
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
	
	
	// Test calling RMmCustomAPI::NotifySsNetworkEvent 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0034::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0034");
	}



CCTSYIntegrationTestCustomAPI0035::CCTSYIntegrationTestCustomAPI0035(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0035::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0035::~CCTSYIntegrationTestCustomAPI0035()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0035::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0035
 * @SYMFssID BA/CTSY/CAPI-0035
 * @SYMTestCaseDesc Test calling RMmCustomAPI::CancelUssdSession
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::CancelUssdSession
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
	
	
	// Test calling RMmCustomAPI::CancelUssdSession 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0035::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0035");
	}



CCTSYIntegrationTestCustomAPI0036::CCTSYIntegrationTestCustomAPI0036(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0036::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0036::~CCTSYIntegrationTestCustomAPI0036()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0036::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0036
 * @SYMFssID BA/CTSY/CAPI-0036
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SatRefreshCompleteNotification - no implementation in CTSY
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SatRefreshCompleteNotification - no implementation in CTSY
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
	
	
	// Test calling RMmCustomAPI::SatRefreshCompleteNotification - no implementation in CTSY 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0036::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0036");
	}



CCTSYIntegrationTestCustomAPI0037::CCTSYIntegrationTestCustomAPI0037(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0037::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0037::~CCTSYIntegrationTestCustomAPI0037()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0037::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0037
 * @SYMFssID BA/CTSY/CAPI-0037
 * @SYMTestCaseDesc Test calling RMmCustomAPI::CheckTwoDigitDialSupport
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::CheckTwoDigitDialSupport
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
	
	
	// Test calling RMmCustomAPI::CheckTwoDigitDialSupport 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0037::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0037");
	}



CCTSYIntegrationTestCustomAPI0038::CCTSYIntegrationTestCustomAPI0038(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0038::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0038::~CCTSYIntegrationTestCustomAPI0038()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0038::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0038
 * @SYMFssID BA/CTSY/CAPI-0038
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ResetNetServer
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ResetNetServer
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
	
	
	// Test calling RMmCustomAPI::ResetNetServer 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0038::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0038");
	}



CCTSYIntegrationTestCustomAPI0039::CCTSYIntegrationTestCustomAPI0039(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0039::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0039::~CCTSYIntegrationTestCustomAPI0039()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0039::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0039
 * @SYMFssID BA/CTSY/CAPI-0039
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ReleaseFile
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ReleaseFile
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
	
	
	// Test calling RMmCustomAPI::ReleaseFile 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0039::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0039");
	}



CCTSYIntegrationTestCustomAPI0040::CCTSYIntegrationTestCustomAPI0040(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0040::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0040::~CCTSYIntegrationTestCustomAPI0040()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0040::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0040
 * @SYMFssID BA/CTSY/CAPI-0040
 * @SYMTestCaseDesc Test calling RMmCustomAPI::RestartFile
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::RestartFile
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
	
	
	// Test calling RMmCustomAPI::RestartFile 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0040::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0040");
	}



CCTSYIntegrationTestCustomAPI0041::CCTSYIntegrationTestCustomAPI0041(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0041::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0041::~CCTSYIntegrationTestCustomAPI0041()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0041::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0041
 * @SYMFssID BA/CTSY/CAPI-0041
 * @SYMTestCaseDesc Test calling RMmCustomAPI::StartSimCbTopicBrowsing
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::StartSimCbTopicBrowsing
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
	
	
	// Test calling RMmCustomAPI::StartSimCbTopicBrowsing 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0041::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0041");
	}



CCTSYIntegrationTestCustomAPI0042::CCTSYIntegrationTestCustomAPI0042(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0042::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0042::~CCTSYIntegrationTestCustomAPI0042()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0042::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0042
 * @SYMFssID BA/CTSY/CAPI-0042
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetNextSimCbTopic
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetNextSimCbTopic
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
	
	
	// Test calling RMmCustomAPI::GetNextSimCbTopic 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0042::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0042");
	}



CCTSYIntegrationTestCustomAPI0043::CCTSYIntegrationTestCustomAPI0043(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0043::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0043::~CCTSYIntegrationTestCustomAPI0043()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0043::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0043
 * @SYMFssID BA/CTSY/CAPI-0043
 * @SYMTestCaseDesc Test calling RMmCustomAPI::DeleteSimCbTopic
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::DeleteSimCbTopic
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
	
	
	// Test calling RMmCustomAPI::DeleteSimCbTopic 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0043::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0043");
	}



CCTSYIntegrationTestCustomAPI0044::CCTSYIntegrationTestCustomAPI0044(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0044::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0044::~CCTSYIntegrationTestCustomAPI0044()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0044::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0044
 * @SYMFssID BA/CTSY/CAPI-0044
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyNetworkConnectionFailure
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyNetworkConnectionFailure
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
	
	
	// Test calling RMmCustomAPI::NotifyNetworkConnectionFailure 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0044::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0044");
	}



CCTSYIntegrationTestCustomAPI0045::CCTSYIntegrationTestCustomAPI0045(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0045::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0045::~CCTSYIntegrationTestCustomAPI0045()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0045::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0045
 * @SYMFssID BA/CTSY/CAPI-0045
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SendAPDUReq
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test calling RMmCustomAPI::SendAPDUReq
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
	
	
	// Test calling RMmCustomAPI::SendAPDUReq 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0045::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0045");
	}



CCTSYIntegrationTestCustomAPI0046::CCTSYIntegrationTestCustomAPI0046(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0046::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0046::~CCTSYIntegrationTestCustomAPI0046()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0046::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0046
 * @SYMFssID BA/CTSY/CAPI-0046
 * @SYMTestCaseDesc Test calling RMmCustomAPI::DisablePhoneLock
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::DisablePhoneLock
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
	
	
	// Test calling RMmCustomAPI::DisablePhoneLock 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0046::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0046");
	}



CCTSYIntegrationTestCustomAPI0047::CCTSYIntegrationTestCustomAPI0047(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0047::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0047::~CCTSYIntegrationTestCustomAPI0047()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0047::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0047
 * @SYMFssID BA/CTSY/CAPI-0047
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyEGprsInfoChange
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyEGprsInfoChange
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
	
	
	// Test calling RMmCustomAPI::NotifyEGprsInfoChange 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0047::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0047");
	}



CCTSYIntegrationTestCustomAPI0048::CCTSYIntegrationTestCustomAPI0048(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0048::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0048::~CCTSYIntegrationTestCustomAPI0048()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0048::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0048
 * @SYMFssID BA/CTSY/CAPI-0048
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetEGprsInfo
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetEGprsInfo
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
	
	
	// Test calling RMmCustomAPI::GetEGprsInfo 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0048::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0048");
	}



CCTSYIntegrationTestCustomAPI0049::CCTSYIntegrationTestCustomAPI0049(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0049::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0049::~CCTSYIntegrationTestCustomAPI0049()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0049::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0049
 * @SYMFssID BA/CTSY/CAPI-0049
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ReadSimFile
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ReadSimFile
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
	
	
	// Test calling RMmCustomAPI::ReadSimFile 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0049::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0049");
	}



CCTSYIntegrationTestCustomAPI0050::CCTSYIntegrationTestCustomAPI0050(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0050::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0050::~CCTSYIntegrationTestCustomAPI0050()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0050::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0050
 * @SYMFssID BA/CTSY/CAPI-0050
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetLifeTime
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetLifeTime
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMmCustomAPI& customApi = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone, KMainCustomApi);	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Test calling RMmCustomAPI::GetLifeTime
    TExtEtelRequestStatus getLifeTimeStatus(customApi, ECustomGetLifeTimeIPC);    
	CleanupStack::PushL(getLifeTimeStatus);	
    RMmCustomAPI::TLifeTimeData ltData;
    TPckg<RMmCustomAPI::TLifeTimeData> ltPckg(ltData);
    customApi.GetLifeTime(getLifeTimeStatus, ltPckg);
    
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getLifeTimeStatus, ETimeLong), KErrNone, _L("RMmCustomAPI::GetLifeTime timed out"));
    ASSERT_EQUALS(getLifeTimeStatus.Int(), KErrNone, _L("RMmCustomAPI::GetLifeTime returned an error"));
    		
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1, &getLifeTimeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0050::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0050");
	}



CCTSYIntegrationTestCustomAPI0051::CCTSYIntegrationTestCustomAPI0051(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0051::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0051::~CCTSYIntegrationTestCustomAPI0051()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0051::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0051
 * @SYMFssID BA/CTSY/CAPI-0051
 * @SYMTestCaseDesc Test calling RMmCustomAPI::Get3GPBInfo - probably stands for Get 3G phone book info
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::Get3GPBInfo - probably stands for Get 3G phone book info
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
	
	
	// Test calling RMmCustomAPI::Get3GPBInfo - probably stands for Get 3G phone book info 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0051::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0051");
	}



CCTSYIntegrationTestCustomAPI0052::CCTSYIntegrationTestCustomAPI0052(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0052::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0052::~CCTSYIntegrationTestCustomAPI0052()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0052::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0052
 * @SYMFssID BA/CTSY/CAPI-0052
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetSystemNetworkModes
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetSystemNetworkModes
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
	
	
	// Test calling RMmCustomAPI::GetSystemNetworkModes 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0052::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0052");
	}



CCTSYIntegrationTestCustomAPI0053::CCTSYIntegrationTestCustomAPI0053(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0053::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0053::~CCTSYIntegrationTestCustomAPI0053()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0053::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0053
 * @SYMFssID BA/CTSY/CAPI-0053
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SetSystemNetworkMode
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SetSystemNetworkMode
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
	
	
	// Test calling RMmCustomAPI::SetSystemNetworkMode 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0053::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0053");
	}



CCTSYIntegrationTestCustomAPI0054::CCTSYIntegrationTestCustomAPI0054(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0054::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0054::~CCTSYIntegrationTestCustomAPI0054()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0054::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0054
 * @SYMFssID BA/CTSY/CAPI-0054
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetCurrentSystemNetworkModes
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetCurrentSystemNetworkModes
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
	
	
	// Test calling RMmCustomAPI::GetCurrentSystemNetworkModes 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0054::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0054");
	}



CCTSYIntegrationTestCustomAPI0055::CCTSYIntegrationTestCustomAPI0055(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0055::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0055::~CCTSYIntegrationTestCustomAPI0055()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0055::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0055
 * @SYMFssID BA/CTSY/CAPI-0055
 * @SYMTestCaseDesc Test calling RMmCustomAPI::PowerSimOn
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::PowerSimOn
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
	
	
	// Test calling RMmCustomAPI::PowerSimOn 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0055::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0055");
	}



CCTSYIntegrationTestCustomAPI0056::CCTSYIntegrationTestCustomAPI0056(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0056::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0056::~CCTSYIntegrationTestCustomAPI0056()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0056::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0056
 * @SYMFssID BA/CTSY/CAPI-0056
 * @SYMTestCaseDesc Test calling RMmCustomAPI::PowerSimOff
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::PowerSimOff
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
	
	
	// Test calling RMmCustomAPI::PowerSimOff 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0056::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0056");
	}



CCTSYIntegrationTestCustomAPI0057::CCTSYIntegrationTestCustomAPI0057(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0057::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0057::~CCTSYIntegrationTestCustomAPI0057()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0057::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0057
 * @SYMFssID BA/CTSY/CAPI-0057
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SimWarmReset
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SimWarmReset
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
	
	
	// Test calling RMmCustomAPI::SimWarmReset 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0057::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0057");
	}



CCTSYIntegrationTestCustomAPI0058::CCTSYIntegrationTestCustomAPI0058(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0058::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0058::~CCTSYIntegrationTestCustomAPI0058()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0058::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0058
 * @SYMFssID BA/CTSY/CAPI-0058
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetATR
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetATR
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
	
	
	// Test calling RMmCustomAPI::GetATR 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0058::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0058");
	}



CCTSYIntegrationTestCustomAPI0059::CCTSYIntegrationTestCustomAPI0059(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0059::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0059::~CCTSYIntegrationTestCustomAPI0059()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0059::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0059
 * @SYMFssID BA/CTSY/CAPI-0059
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetSimCardReaderStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetSimCardReaderStatus
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
	
	
	// Test calling RMmCustomAPI::GetSimCardReaderStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0059::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0059");
	}



CCTSYIntegrationTestCustomAPI0060::CCTSYIntegrationTestCustomAPI0060(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0060::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0060::~CCTSYIntegrationTestCustomAPI0060()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0060::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0060
 * @SYMFssID BA/CTSY/CAPI-0060
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifySimCardStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifySimCardStatus
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
	
	
	// Test calling RMmCustomAPI::NotifySimCardStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0060::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0060");
	}



CCTSYIntegrationTestCustomAPI0061::CCTSYIntegrationTestCustomAPI0061(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0061::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0061::~CCTSYIntegrationTestCustomAPI0061()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0061::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0061
 * @SYMFssID BA/CTSY/CAPI-0061
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetWlanSimAuthenticationData
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetWlanSimAuthenticationData
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
	
	
	// Test calling RMmCustomAPI::GetWlanSimAuthenticationData 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0061::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0061");
	}



CCTSYIntegrationTestCustomAPI0062::CCTSYIntegrationTestCustomAPI0062(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0062::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0062::~CCTSYIntegrationTestCustomAPI0062()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0062::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0062
 * @SYMFssID BA/CTSY/CAPI-0062
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SetSimMessageStatusRead
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SetSimMessageStatusRead
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
	
	
	// Test calling RMmCustomAPI::SetSimMessageStatusRead 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0062::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0062");
	}



CCTSYIntegrationTestCustomAPI0063::CCTSYIntegrationTestCustomAPI0063(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0063::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0063::~CCTSYIntegrationTestCustomAPI0063()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0063::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0063
 * @SYMFssID BA/CTSY/CAPI-0063
 * @SYMTestCaseDesc Test calling RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
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
	
	
	// Test calling RMmCustomAPI::WriteViagHomeZoneUHZIUESettings 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0063::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0063");
	}



CCTSYIntegrationTestCustomAPI0064::CCTSYIntegrationTestCustomAPI0064(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0064::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0064::~CCTSYIntegrationTestCustomAPI0064()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0064::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0064
 * @SYMFssID BA/CTSY/CAPI-0064
 * @SYMTestCaseDesc Test calling RMmCustomAPI::SetAlwaysOn
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::SetAlwaysOn
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
	
	
	// Test calling RMmCustomAPI::SetAlwaysOn 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0064::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0064");
	}



CCTSYIntegrationTestCustomAPI0065::CCTSYIntegrationTestCustomAPI0065(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0065::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0065::~CCTSYIntegrationTestCustomAPI0065()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0065::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0065
 * @SYMFssID BA/CTSY/CAPI-0065
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyRauEvent
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyRauEvent
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
	
	
	// Test calling RMmCustomAPI::NotifyRauEvent 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0065::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0065");
	}



CCTSYIntegrationTestCustomAPI0066::CCTSYIntegrationTestCustomAPI0066(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0066::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0066::~CCTSYIntegrationTestCustomAPI0066()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0066::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0066
 * @SYMFssID BA/CTSY/CAPI-0066
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ReadHSxPAStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ReadHSxPAStatus
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
	
	
	// Test calling RMmCustomAPI::ReadHSxPAStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0066::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0066");
	}



CCTSYIntegrationTestCustomAPI0067::CCTSYIntegrationTestCustomAPI0067(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0067::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0067::~CCTSYIntegrationTestCustomAPI0067()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0067::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0067
 * @SYMFssID BA/CTSY/CAPI-0067
 * @SYMTestCaseDesc Test calling RMmCustomAPI::WriteHSxPAStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::WriteHSxPAStatus
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
	
	
	// Test calling RMmCustomAPI::WriteHSxPAStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0067::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0067");
	}



CCTSYIntegrationTestCustomAPI0068::CCTSYIntegrationTestCustomAPI0068(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0068::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0068::~CCTSYIntegrationTestCustomAPI0068()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0068::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0068
 * @SYMFssID BA/CTSY/CAPI-0068
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyHSxPAStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyHSxPAStatus
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
	
	
	// Test calling RMmCustomAPI::NotifyHSxPAStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0068::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0068");
	}



CCTSYIntegrationTestCustomAPI0069::CCTSYIntegrationTestCustomAPI0069(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0069::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0069::~CCTSYIntegrationTestCustomAPI0069()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0069::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0069
 * @SYMFssID BA/CTSY/CAPI-0069
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetIccCallForwardingIndicatorStatus
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetIccCallForwardingIndicatorStatus
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
	
	
	// Test calling RMmCustomAPI::GetIccCallForwardingIndicatorStatus 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0069::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0069");
	}



CCTSYIntegrationTestCustomAPI0070::CCTSYIntegrationTestCustomAPI0070(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0070::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0070::~CCTSYIntegrationTestCustomAPI0070()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0070::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0070
 * @SYMFssID BA/CTSY/CAPI-0070
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyIccCallForwardingStatusChange
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyIccCallForwardingStatusChange
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
	
	
	// Test calling RMmCustomAPI::NotifyIccCallForwardingStatusChange 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0070::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0070");
	}



CCTSYIntegrationTestCustomAPI0071::CCTSYIntegrationTestCustomAPI0071(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0071::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0071::~CCTSYIntegrationTestCustomAPI0071()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0071::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0071
 * @SYMFssID BA/CTSY/CAPI-0071
 * @SYMTestCaseDesc Test calling RMmCustomAPI::GetCellInfo
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::GetCellInfo
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
	
	
	// Test calling RMmCustomAPI::GetCellInfo 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0071::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0071");
	}



CCTSYIntegrationTestCustomAPI0072::CCTSYIntegrationTestCustomAPI0072(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0072::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0072::~CCTSYIntegrationTestCustomAPI0072()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0072::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0072
 * @SYMFssID BA/CTSY/CAPI-0072
 * @SYMTestCaseDesc Test calling RMmCustomAPI::NotifyCellInfoChange
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Test calling RMmCustomAPI::NotifyCellInfoChange
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
	
	
	// Test calling RMmCustomAPI::NotifyCellInfoChange 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0072::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0072");
	}



CCTSYIntegrationTestCustomAPI0073::CCTSYIntegrationTestCustomAPI0073(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0073::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0073::~CCTSYIntegrationTestCustomAPI0073()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0073::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0073
 * @SYMFssID BA/CTSY/CAPI-0073
 * @SYMTestCaseDesc Test calling RMmCustomAPI::WriteAlphaString
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::WriteAlphaString
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
	
	
	// Test calling RMmCustomAPI::WriteAlphaString 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0073::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0073");
	}



CCTSYIntegrationTestCustomAPI0074::CCTSYIntegrationTestCustomAPI0074(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0074::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0074::~CCTSYIntegrationTestCustomAPI0074()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0074::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0074
 * @SYMFssID BA/CTSY/CAPI-0074
 * @SYMTestCaseDesc Test calling RMmCustomAPI::ReadAlphaString
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::ReadAlphaString
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
	
	
	// Test calling RMmCustomAPI::ReadAlphaString 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0074::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0074");
	}



CCTSYIntegrationTestCustomAPI0075::CCTSYIntegrationTestCustomAPI0075(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCustomAPIBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCustomAPI0075::GetTestStepName());
	}

CCTSYIntegrationTestCustomAPI0075::~CCTSYIntegrationTestCustomAPI0075()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCustomAPI0075::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CAPI-0075
 * @SYMFssID BA/CTSY/CAPI-0075
 * @SYMTestCaseDesc Test calling RMmCustomAPI::DeleteAlphaString
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Place holder - fill this in when writing test
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Test calling RMmCustomAPI::DeleteAlphaString
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
	
	
	// Test calling RMmCustomAPI::DeleteAlphaString 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCustomAPI0075::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCustomAPI0075");
	}



