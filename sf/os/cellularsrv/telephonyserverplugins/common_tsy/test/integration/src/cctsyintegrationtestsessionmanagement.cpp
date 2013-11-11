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
// Test step definitions for the SessionManagement functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsessionmanagement.h"



	
CCTSYIntegrationTestSessionManagementBase::CCTSYIntegrationTestSessionManagementBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSessionManagementBase::~CCTSYIntegrationTestSessionManagementBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestSessionManagement0001::CCTSYIntegrationTestSessionManagement0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSessionManagementBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSessionManagement0001::GetTestStepName());
	}

CCTSYIntegrationTestSessionManagement0001::~CCTSYIntegrationTestSessionManagement0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSessionManagement0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SESMG-0001
 * @SYMFssID BA/CTSY/SESMG-0001
 * @SYMTestCaseDesc Automatic network selection at start
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::GetCurrentNetwork
 * @SYMTestExpectedResults Pass - Registration status notificatins are received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify current network change and network registration status notifications complete.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetRegistrationStatus 

	// Close down TSY, so registration will re-start when TSY re-opens 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Open TSY to force registration ===

	// Load the TSY with RTelServer::LoadPhoneModule 

	// Open the phone subsession with RMobilePhone::Open 

	// ===  Ensure network registration status's go from unknown->searching->registered home network ===

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with ERegistrationUnknown->ENotRegisteredSearching->ERegisteredOnHomeNetwork

	// Check RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork

	// ===  Check receive notifciations for current network change ===

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iCountryCode with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iDisplayTag with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iLongName with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iMode=ENetworkModeGsm
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iNetworkId with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iShortName with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iStatus=ENetworkStatusCurrent
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aArea.iCellId > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aArea.iLocationAreaCode > 0

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSessionManagement0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSessionManagement0001");
	}



