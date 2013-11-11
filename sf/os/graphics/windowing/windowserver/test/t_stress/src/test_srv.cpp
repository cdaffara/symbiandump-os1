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
 

#include "test_srv.h"
#include "test_step_comparison.h"


CWServTestSrv* CWServTestSrv::NewLC( )
	{
	CWServTestSrv* server = new (ELeave) CWServTestSrv();
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

	CWServTestSrv* server = CWServTestSrv::NewLC( );
	RProcess::Rendezvous (KErrNone );
	sched->Start( );

	CleanupStack::PopAndDestroy (server );
	CleanupStack::PopAndDestroy (sched );
	}

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */
TInt E32Main(void) //lint -e765 -e714 Suppress 'not referenced' and 'could be static'
	{
	
	//__asm { int 3 };
	//__DEBUGGER();
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());	
	if (err)
		{
		_LIT(KMainErrorStatement, "ProjectTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
		_LIT(KProjectTestServerName, "ProjectTestServer");
	   	User::Panic(KProjectTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CWServTestSrv::CreateTestStep(const TDesC& aStepName )
	{
	CTestStep* testStep = NULL;
	if (aStepName == KServStressTest)
		{
		testStep = new CWServTestStep();
		}
	else
		{
		RDebug::Printf ("Unknown test step %S", &aStepName );
		}

	return testStep;
	}
