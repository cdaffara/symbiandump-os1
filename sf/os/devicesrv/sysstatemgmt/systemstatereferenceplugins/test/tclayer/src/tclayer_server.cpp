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
 
#include "tclayer_server.h"
#include "tclayer_step_startersession.h"
#include "tclayer_step_startupadaptationadapter.h"
#include "tclayer_step_ssmpropertyobserver.h"
#include "tclayer_step_ssmsimstatusobserver.h"
#include "tclayer_step_ssmsecurityeventobserver.h"

#include "ssmdebug.h"

CCLayerTestServer* CCLayerTestServer::NewLC( )
	{
	CCLayerTestServer * server = new (ELeave) CCLayerTestServer();
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

	CCLayerTestServer* server = CCLayerTestServer::NewLC( );
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
		DEBUGPRINT2(_L("CCLayerTestServer::MainL - Error: %d"), err);
		}
	delete cleanup;
		
	__UHEAP_MARKEND;
	return err;
	} //lint -esym(765, E32Main) Suppress 'E32Main' could be made static
	  //lint -esym(714, E32Main) Suppress 'E32Main' not referenced

/**
 @return - A CTestStep derived instance
 */
CTestStep* CCLayerTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTCCLayerTestStarterSession)
		{
		testStep = new CCLayerTestStarterSession();
		}
	else if (aStepName == KTCCLayerTestStartupAdaptationAdapter)
		{
		testStep = new CCLayerTestStartupAdaptationAdapter();
		}
	else if (aStepName == KTCCLayerTestSsmSimStatusObserver)
		{
		testStep = new CCLayerTestSsmSimStatusObserver();
		}
	else if (aStepName == KTCCLayerTestSsmPropertyObserver)
		{
		testStep = new CCLayerTestSsmPropertyObserver();
		}
	else if (aStepName == KTCLayerTestSsmEventObserver)
		{
		testStep = new CLayerTestSsmEventObserver();
		}
	else
		{
		DEBUGPRINT2(_L("Unknown ClayerTest step %S"), &aStepName);
		}

	return testStep;
	}
