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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tstartupproperties_server.h"
#include "tstartupproperties_stepdefval.h"
#include "tstartupproperties_stepinternalexternal.h"

_LIT(KServerName, "tstartupproperties_server");

CStartupPropertiesTestServer* CStartupPropertiesTestServer::NewLC()
	{
	CStartupPropertiesTestServer * server = new (ELeave) CStartupPropertiesTestServer();
	CleanupStack::PushL(server);
	server->StartL(KServerName);
	return server;
	}
	

static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);	
	CActiveScheduler::Install(sched);
	
	// Create the CTestServer derived server
	CStartupPropertiesTestServer* server = CStartupPropertiesTestServer::NewLC();
	RProcess::Rendezvous(KErrNone);
	sched->Start();
	
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if(cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
	
	__UHEAP_MARKEND;
	return err;
    }//lint -e765 -e714 Suppress 'not referenced' and 'could be static'

/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CStartupPropertiesTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KCTestCaseDefVal)
		{
		testStep = new CStartupPropertiesTestStepDefVal();
		}
	else if (aStepName == KCTestCaseInternalExternal)
		{
		testStep = new CStartupPropertiesTestStepInternalExternal();
		}
	else 
		{
		RDebug::Printf("Unknown startupproperties teststep %S", &aStepName);	
		}
	
	return testStep;
	}
	
