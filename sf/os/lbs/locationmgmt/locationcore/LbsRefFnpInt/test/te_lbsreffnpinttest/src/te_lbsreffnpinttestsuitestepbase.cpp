/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "te_lbsreffnpinttestsuitestepbase.h"
#include "te_lbsreffnpinttestsuitedefs.h"

// Device driver constants

TVerdict CTe_LbsReffnpintTestSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	RLbsNetworkLocationRequests::InitializeL();
	RLbsNetworkLocationResponses::InitializeL();
	RLbsNetworkPositionUpdates::InitializeL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsReffnpintTestSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	
	RLbsNetworkPositionUpdates::ShutDownL();
	RLbsNetworkLocationResponses::ShutDownL();
	RLbsNetworkLocationRequests::ShutDownL();
	
	delete iScheduler;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_LbsReffnpintTestSuiteStepBase::~CTe_LbsReffnpintTestSuiteStepBase()
	{
	}

CTe_LbsReffnpintTestSuiteStepBase::CTe_LbsReffnpintTestSuiteStepBase()
	{
	}
