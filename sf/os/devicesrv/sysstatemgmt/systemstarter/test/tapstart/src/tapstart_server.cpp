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

#include "tapstart_server.h"
#include "tapstart_step.h"

_LIT(KServerName, "tapstart_server");
    
static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);	
	CActiveScheduler::Install(sched);
	
	CApStartTestServer* server = CApStartTestServer::NewLC();
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
    }

CApStartTestServer* CApStartTestServer::NewLC()
	{
	CApStartTestServer * server = new (ELeave) CApStartTestServer();
	CleanupStack::PushL(server);
	server->StartL(KServerName);
	return server;
	}

/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CApStartTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KCTestCaseApStart)
		{
		testStep = new CApStartTestStep();
		}
	else 
		{
		RDebug::Printf("Unknown apstart teststep %S", &aStepName);	
		}
	
	return testStep;
	}
