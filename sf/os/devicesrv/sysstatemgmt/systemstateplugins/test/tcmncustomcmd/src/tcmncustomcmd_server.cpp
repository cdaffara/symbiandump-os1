// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcmncustomcmd_server.h"
#include "tcmncustomcmd_step_rendezvouswithestart.h"
#include "tcmncustomcmd_step_cancelmonitoring.h"

_LIT(KCustomCmdTestServerName, "tcmncustomcmd_server");

CCmdTestServer* CCmdTestServer::NewLC( )
	{
	CCmdTestServer * server = new (ELeave) CCmdTestServer();
	CleanupStack::PushL(server);
	TParsePtrC serverName(RProcess().FileName());
	server->StartL(serverName.Name());
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched);
	CActiveScheduler::Install (sched);

	CCmdTestServer* server = CCmdTestServer::NewLC( );
	RProcess::Rendezvous (KErrNone );
	sched->Start( );

	CleanupStack::PopAndDestroy (server );
	CleanupStack::PopAndDestroy (sched );
	}

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */
TInt E32Main(void)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err,MainL());
	if (err)
		{
		RDebug::Print(_L("CmdTestServer::MainL - Error: %d"), err);
		User::Panic(KCustomCmdTestServerName, err);
		}
	delete cleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CCmdTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTCCustomCmdTestRendezvousWithEStart)
		{
		testStep = new CCustomCmdTestRendezvousWithEStart();
		}
	else if (aStepName == KTCCustomCmdTestCancelMonitoring)
        {
        testStep = new CCustomCmdTestCancelMonitoring();
        }   
	else
		{
		RDebug::Print(_L("Unknown CmdTest step %S"), &aStepName);
	   	User::Panic(KCustomCmdTestServerName, KErrArgument);
		}

	return testStep;
	}
