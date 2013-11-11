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
// @file 
// This is the class implementation for the step to stop the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "stoplbsstep.h"
#include <lbs/test/lbstestclient.h>

/**
 * Destructor
 */
CT_StopLbsStep::~CT_StopLbsStep()
	{
	}


/**
 * Constructor
 */
CT_StopLbsStep::CT_StopLbsStep()
	{
	SetTestStepName(KStopLbsStep);
	}

 
TVerdict CT_StopLbsStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_StopLbsStep::doTestStepPreambleL()"));
	
	CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);
	
	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("&lt;&lt;CT_StopLbsStep::doTestStepPreambleL()"));

	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_StopLbsStep::doTestStepL()
	{
	// Test step used to stop the lbs system
	INFO_PRINTF1(_L("&gt;&gt;CT_StopLbsStep::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		CLbsTestServer* testServer = CLbsTestServer::NewL();
		CleanupStack::PushL(testServer);

		User::LeaveIfError(testServer->StopLbsSystem());

		CleanupStack::PopAndDestroy(testServer);
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_StopLbsStep::doTestStepL()"));

	return TestStepResult();
	}

TVerdict CT_StopLbsStep::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_StopLbsStep::doTestStepPostambleL()"));
	

	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("&lt;&lt;CT_StopLbsStep::doTestStepPostambleL()"));
	return TestStepResult();
	}

