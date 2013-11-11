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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "trestartsys_server.h"
#include "trestartsys_abort.h"
#include "trestartsys_withmode.h"

_LIT(KServerName, "trestartsys_server");

CRestartSysTestServer* CRestartSysTestServer::NewLC()
	{
	CRestartSysTestServer* server = new (ELeave) CRestartSysTestServer();
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
	CRestartSysTestServer* server = CRestartSysTestServer::NewLC();
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

CTestStep* CRestartSysTestServer::CreateTestStep(const TDesC& aStepName)
	{
#ifdef __WINS__	//Restart is currently supported on HW only		
	RDebug::Printf("Test %S not executed. Restart is currently not supported in emulator.", &aStepName);	
	return NULL;
#endif

	CTestStep* testStep = NULL; //lint !e527 Unreachable
	if (aStepName == KRestartSysAbort) //lint !e527 Unreachable
		{
		testStep = new CRestartSysAbort(aStepName);
		}
	else if (aStepName == KRestartSysWithMode)
		{
		testStep = new CRestartSysWithMode(aStepName);
		}
	else
		{
		RDebug::Printf("Unknown restartsys teststep %S", &aStepName);
		}
		
	return testStep;
	}
	
