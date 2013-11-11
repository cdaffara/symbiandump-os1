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
 

#include "tadaptation_plugin.h"
#include "tstate_adaptationplugin_step.h"
#include "tsim_adaptationplugin_step.h"
#include "trtc_adaptationplugin_step.h"
#include "tmisc_adaptationplugin_step.h"
#include "temergencycallrf_adaptationplugin_step.h"


CAdaptationPluginTestServer* CAdaptationPluginTestServer::NewLC( )
	{
	CAdaptationPluginTestServer * server = new (ELeave) CAdaptationPluginTestServer();
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

	CAdaptationPluginTestServer* server = CAdaptationPluginTestServer::NewLC( );
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
#ifdef _DEBUG
		_LIT(KMainErrorStatement, "CmdTestServer::MainL - Error: %d");
		//INFO_PRINTF1(_L(KMainErrorStatement));//, err);
	   	User::Panic(KMainErrorStatement, err);
#endif
	   	SSMLOGLEAVE(err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CAdaptationPluginTestServer::CreateTestStep(const TDesC& aStepName )
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTCTestStateAdaptationPlugin)
		{
		testStep = new CTestStateAdaptationPlugin();
		}
	else if (aStepName == KTCTestSimAdaptationPlugin)
		{
		testStep = new CTestSimAdaptationPlugin();
		}
	else if (aStepName == KTCTestRtcAdaptationPlugin)
		{
		testStep = new CTestRtcAdaptationPlugin();
		}
	else if (aStepName == KTCTestMiscAdaptationPlugin)
		{
		testStep = new CTestMiscAdaptationPlugin();
		}
	else if (aStepName == KTCTestEmergencyCallRfAdaptationPlugin)
		{
		testStep = new CTestEmergencyCallRfAdaptationPlugin();
		}	
	else
		{
		DEBUGPRINT2(_L("Unknown Adaptation step %S"), &aStepName);
	   	User::Panic(KTCTestStateAdaptationPlugin, KErrArgument);
		}

	return testStep;
	}
