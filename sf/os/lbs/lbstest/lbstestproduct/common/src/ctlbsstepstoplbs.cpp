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
// @file ctlbslocdatasrc.cpp
// This is the class implementation for the step to stop the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "ctlbsstepstoplbs.h"
#include <lbs/test/lbstestclient.h>

/**
 * Destructor
 */
CT_LbsStep_StopLbs::~CT_LbsStep_StopLbs()
	{
	}


/**
 * Constructor
 */
CT_LbsStep_StopLbs::CT_LbsStep_StopLbs(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_StopLbs);
	}


/**
Static Constructor
*/
CT_LbsStep_StopLbs* CT_LbsStep_StopLbs::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_StopLbs(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

 
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_StopLbs::doTestStepL()
	{
	// Test step used to stop the lbs system
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_StopLbs::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		CLbsTestServer* testServer = CLbsTestServer::NewL();
		CleanupStack::PushL(testServer);

		User::LeaveIfError(testServer->StopLbsSystem());

		CleanupStack::PopAndDestroy(testServer);
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_StopLbs::doTestStepL()"));

	return TestStepResult();
	}
