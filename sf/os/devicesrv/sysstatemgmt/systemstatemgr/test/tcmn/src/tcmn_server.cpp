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
 

#include "tcmn_server.h"
#include "tcmn_step_state.h"
#include "tcmn_step_swpvalue.h"
#include "tcmn_step_stateawaresession.h"
#include "tcmn_step_systemwideproperty.h"


_LIT(KCmnTestServerName, "CmnTestServer");

CCmnTestServer* CCmnTestServer::NewLC( )
	{
	CCmnTestServer * server = new (ELeave) CCmnTestServer();
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

	CCmnTestServer* server = CCmnTestServer::NewLC( );
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
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());	
	if (err)
		{
		_LIT(KMainErrorStatement, "CmnTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KCmnTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CCmnTestServer::CreateTestStep(const TDesC& aStepName )
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTCmnStateStep)
		{
		testStep = new CCmnStateTest();
		}
	else if (aStepName == KTCmnSwpValueStep)
		{
		testStep = new CCmnSwpValueTest();
		}
	else if (aStepName == KTCmnStateAwareSessionStep)
		{
		testStep = new CCmnStateAwareSessionTest();
		}
	else if (aStepName == KTCmnSystemWidePropertyStep)
		{
		testStep = new CCmnSystemWidePropertyTest();
		}
	else
		{
		RDebug::Printf ("Unknown CmnTest step %S", &aStepName );
		}

	return testStep;
	}
