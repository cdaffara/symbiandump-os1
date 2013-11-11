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
 

#include "tsus_server.h"
#include "tsus_step_supinfo.h"
#include "tsus_step_suploader.h"
#include "tsus_step_utilserver.h"
#include "tsus_step_platsec.h"
#include "tsus_step_platsec2.h"
#include "tsus_step_loadplugin.h"
#include "tsus_step_adaptionserver.h"
#include "tsus_step_susadaptationpluginloader.h"
#include "tsus_step_loadadaptationserver.h"


_LIT(KSusTestServerName, "SusTestServer");

CSusTestServer* CSusTestServer::NewLC( )
	{
	CSusTestServer* server = new (ELeave) CSusTestServer();
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

	CSusTestServer* server = CSusTestServer::NewLC( );
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
		_LIT(KMainErrorStatement, "SusTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KSusTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CSusTestServer::CreateTestStep(const TDesC& aStepName )
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTSusUtilServerStep)
		{
		testStep = new CSusUtilServerTest();
		}
	else if (aStepName == KTSusAdaptionServerStep)
		{
		testStep = new CSusAdaptionServerTest();
		}
	else if (aStepName == KTSsmSupInfoStep)
		{
		testStep = new CSsmSupInfoTest();
		}
	else if (aStepName == KTSsmSupLoaderStep)
		{
		testStep = new CSsmSupLoaderTest();
		}
	else if (aStepName == KTSusPlatsecStep)
		{
		testStep = new CSusPlatsecTest();
		}
	else if (aStepName == KTSusPlatsecStepProtSrvBegin)
		{
		testStep = new CSusPlatsecTestProtSrvBegin();
		}
	else if (aStepName == KTSusPlatsecStepProtSrv)
		{
		testStep = new CSusPlatsecTestProtSrv();
		}
	else if (aStepName == KTSusPlatsecStepProtSrvEnd)
		{
		testStep = new CSusPlatsecTestProtSrvEnd();
		}
	else if (aStepName == KTSusLoadPluginStep)
		{
		testStep = new CSusLoadPluginTest();
		}
	else if (aStepName == KTSsmSupAdaptationPluginLoaderStep)
		{
		testStep = new CSsmSupAdaptationPluginLoaderTest();
		}
	else if (aStepName == KTSusLoadAdaptationServerStep)
		{
		testStep = new CTestSusLoadAdaptationServer();
		}
	else
		{
		RDebug::Printf ("Unknown SusTest step %S", &aStepName );
		}

	return testStep;
	}
