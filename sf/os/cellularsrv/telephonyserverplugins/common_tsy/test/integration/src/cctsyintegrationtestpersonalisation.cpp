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
// Test step definitions for the Personalisation functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestpersonalisation.h"



	
CCTSYIntegrationTestPersonalisationBase::CCTSYIntegrationTestPersonalisationBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPersonalisationBase::~CCTSYIntegrationTestPersonalisationBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPersonalisation0001::CCTSYIntegrationTestPersonalisation0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPersonalisationBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPersonalisation0001::GetTestStepName());
	}

CCTSYIntegrationTestPersonalisation0001::~CCTSYIntegrationTestPersonalisation0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPersonalisation0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PERS-0001
 * @SYMFssID BA/CTSY/PERS-0001
 * @SYMTestCaseDesc Get personalisation caps
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Caps returned correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify caps returned are correct.
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
	
	
	// Check RMobilePhone::GetPersonalisationCaps returns caps in set of RMobilePhone::KCapsPersonalisedNetwork |

	//       RMobilePhone::KCapsPersonalisedNetworkSub | 

	//       RMobilePhone::KCapsPersonalisedServiceProvider | 

	//       RMobilePhone::KCapsPersonalisedCorporate | 

	//       RMobilePhone::KCapsPersonalisedUSim 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPersonalisation0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPersonalisation0001");
	}



