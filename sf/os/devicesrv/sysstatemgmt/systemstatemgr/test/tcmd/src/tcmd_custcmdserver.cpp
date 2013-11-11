// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcmd_custcmdserver.h"
#include "tcle_cleclient.h"
#include "tcmd_step_custcmdneverunload.h"

#include "ssmdebug.h"

_LIT(KCmdTestServerName, "CustCmdTestServer");

CCustCmdTestServer* CCustCmdTestServer::NewLC( )
	{
	CCustCmdTestServer * server = new (ELeave) CCustCmdTestServer();
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

	CCustCmdTestServer* server = CCustCmdTestServer::NewLC( );
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
		_LIT(KMainErrorStatement, "CustCmdTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KCmdTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CCustCmdTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTCmdTestCustCmd)
		{
		testStep = new CTestCustCmd();
		}
	else
		{
		RDebug::Printf ("Unknown CustCmdTest step %S", &aStepName );
		}

	return testStep;
	}
