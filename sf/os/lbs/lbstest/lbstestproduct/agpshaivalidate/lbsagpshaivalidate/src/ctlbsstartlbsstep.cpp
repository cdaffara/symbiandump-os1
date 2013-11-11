// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsstartlbsstep.cpp
// This is the class implementation for the step to start the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "ctlbsstartlbsstep.h"
#include "lbstestloggermacros.h"
#include <lbs/test/lbstestclient.h>

/**
 * Destructor
 */
CT_LbsAGPSHAIValidateStep_StartLbs::~CT_LbsAGPSHAIValidateStep_StartLbs()
	{
	}


/**
 * Constructor
 */
CT_LbsAGPSHAIValidateStep_StartLbs::CT_LbsAGPSHAIValidateStep_StartLbs(CT_LbsAGPSHAIServerRoot& aParent) : iParent(aParent)
	{
	SetTestStepName(KLbsStartLbsStep);
	}


/**
Static Constructor
*/
CT_LbsAGPSHAIValidateStep_StartLbs* CT_LbsAGPSHAIValidateStep_StartLbs::New(CT_LbsAGPSHAIServerRoot& aParent)
	{
	return new CT_LbsAGPSHAIValidateStep_StartLbs(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

 
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAGPSHAIValidateStep_StartLbs::doTestStepL()
	{
	// Test step used to start the location manager.
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_StartLbs::doTestStepL()");

	if (TestStepResult() == EPass)
		{			
		CLbsTestServer* testServer = CLbsTestServer::NewL();
		CleanupStack::PushL(testServer);
		User::LeaveIfError(testServer->StartLbsSystem());
		CleanupStack::PopAndDestroy(testServer);
		}
		
	TESTLOG(ELogP2, "<< CT_LbsAGPSHAIValidateStep_StartLbs::doTestStepL()");

	return TestStepResult();
	}
