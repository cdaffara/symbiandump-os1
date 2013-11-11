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
// This is the class implementation for the step to start the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "startlbsstep.h"
#include <lbs/test/lbstestclient.h>

/**
 * Destructor
 */
CT_StartLbsStep::~CT_StartLbsStep()
	{
	}


/**
 * Constructor
 */
CT_StartLbsStep::CT_StartLbsStep()
	{
	SetTestStepName(KStartLbsStep);
	}

TVerdict CT_StartLbsStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_StartLbsStep::doTestStepPreambleL()"));
	
	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("&lt;&lt;CT_StartLbsStep::doTestStepPreambleL()"));

	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_StartLbsStep::doTestStepL()
	{
	// Test step used to start the location manager.
	INFO_PRINTF1(_L("&gt;&gt;CT_StartLbsStep::doTestStepL()"));

	
	if (TestStepResult() == EPass)
		{	
		// Now start the LBS system
		CLbsTestServer* testServer = CLbsTestServer::NewL();
		CleanupStack::PushL(testServer);
		
		User::LeaveIfError(testServer->StartLbsSystem());
		CleanupStack::PopAndDestroy(testServer);
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_StartLbsStep::doTestStepL()"));

	return TestStepResult();
	}

TVerdict CT_StartLbsStep::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_StartLbsStep::doTestStepPostambleL()"));
	

	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("&lt;&lt;CT_StartLbsStep::doTestStepPostambleL()"));
	return TestStepResult();
	}


