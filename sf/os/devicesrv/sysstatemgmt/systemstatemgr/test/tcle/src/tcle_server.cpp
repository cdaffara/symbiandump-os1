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
 

#include "tcle_server.h"
#include "tcle_step_rclesession.h"
#include "tcle_step_rclesessionfail.h"
#include "tcle_step_pause.h"
#include "tcle_step_dwfs_bad.h"
#include "tcle_step_dwfs_bad_norend.h"
#include "tcle_step_dwfs_good.h"
#include "tcle_step_fandf_bad.h"
#include "tcle_step_fandf_good.h"
#include "tcle_step_wfs_bad.h"
#include "tcle_step_wfs_good.h"


_LIT(KCleTestServerName, "CleTestServer");

CCleTestServer* CCleTestServer::NewLC( )
	{
	CCleTestServer * server = new (ELeave) CCleTestServer();
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

	CCleTestServer* server = CCleTestServer::NewLC( );
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
		_LIT(KMainErrorStatement, "CleTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KCleTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CCleTestServer::CreateTestStep(const TDesC& aStepName )
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTCleRCleSessionStep)
		{
		testStep = new CCleRCleSessionTest();
		}
	else if (aStepName == KTClePauseStep)
		{
		testStep = new CClePauseTest();
		}
	else if (aStepName == KTCleFandFGoodStep)
		{
		testStep = new CCleFandFGoodStep();
		}
	else if (aStepName == KTCleFandFBadStep)
		{
		testStep = new CCleFandFBadStep();
		}
	else if (aStepName == KTCleWfsGoodStep)
		{
		testStep = new CCleWfsGoodStep();
		}
	else if (aStepName == KTCleWfsBadStep)
		{
		testStep = new CCleWfsBadStep();
		}
	else if (aStepName == KTCleDwfsGoodStep)
		{
		testStep = new CCleDwfsGoodStep();
		}
	else if (aStepName == KTCleDwfsBadStep)
		{
		testStep = new CCleDwfsBadStep();
		}
	else if (aStepName == KTCleDwfsBadNoRendStep)
		{
		testStep = new CCleDwfsBadNoRendStep();
		}
	else if (aStepName == KTCleRCleSessionFailStep)
		{
		testStep = new CCleRCleSessionFailTest();
		}
	else
		{
		RDebug::Printf ("Unknown CleTest step %S", &aStepName );
		}

	return testStep;
	}
