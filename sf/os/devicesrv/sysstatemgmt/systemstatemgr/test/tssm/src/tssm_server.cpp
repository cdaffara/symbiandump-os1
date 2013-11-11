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
 

#include "tssm_server.h"
#include "tssm_step_server.h"
#include "tssm_step_statepolicyresolver.h"
#include "tssm_step_statetransitionengine.h"
#include "tssm_step_swppolicymap.h"
#include "tssm_step_swppolicyresolver.h"
#include "tssm_step_swprequesthandler.h"
#include "tssm_step_swptransitionengine.h"
#include "tssm_step_swptransitionscheduler.h"
#include "tssm_step_validstatelist.h"
#include "tssm_step_validswplist.h"
#include "tssm_step_platsec.h"
#include "tssm_step_swpcommonplugin.h"
#include "tssm_step_paniccle.h"
#include "tssm_step_swpdependencies.h"
#include "tssm_step_swphandlecleret.h"
#include "tssm_step_statefurthertransition.h"
#include "tssm_step_conditionalcommands.h"
#include "tssm_step_ssmcledeadlock.h"
#include "tssm_step_repeatedpublishstate.h"
#include "tssm_step_notifypropertychange.h"
#include "tssmswppolicy_step_rswppolicysess.h"
#include "tssm_step_cmdexecutionoom.h"
#include "ssmserverpanic.h"

_LIT(KSsmTestServerName, "SsmTestServer");

CSsmTestServer* CSsmTestServer::NewLC( )
	{
	CSsmTestServer * server = new (ELeave) CSsmTestServer();
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

	CSsmTestServer* server = CSsmTestServer::NewLC( );
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
		_LIT(KMainErrorStatement, "SsmTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	PanicNow(KSsmTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CSsmTestServer::CreateTestStep(const TDesC& aStepName )
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTSsmServerStep)
		{
		testStep = new CSsmServerTest();
		}
	else if (aStepName == KTSsmStatePolicyResolverStep)
		{
		testStep = new CSsmStatePolicyResolverTest();
		}
	else if (aStepName == KTSsmStateTransitionEngineStep)
		{
		testStep = new CSsmStateTransitionEngineTest();
		}
	else if (aStepName == KTSsmSwpPolicyMapStep)
		{
		testStep = new CSsmSwpPolicyMapTest();
		}
	else if (aStepName == KTSsmSwpPolicyResolverStep)
		{
		testStep = new CSsmSwpPolicyResolverTest();
		}
	else if (aStepName == KTSsmSwpTransitionEngineStep)
		{
		testStep = new CSsmSwpTransitionEngineTest();
		}
	else if (aStepName == KTSsmSwpRequestHandlerStep)
		{
		testStep = new CSsmSwpRequestHandlerTest();
		}
	else if (aStepName == KTSsmSwpTransitionSchedulerStep)
		{
		testStep = new CSsmSwpTransitionSchedulerTest();
		}
	else if (aStepName == KTSsmValidStateListStep)
		{
		testStep = new CSsmValidStateListTest();
		}
	else if (aStepName == KTSsmValidSwpListStep)
		{
		testStep = new CSsmValidSwpListTest();
		}
	else if (aStepName == KTSsmPlatsecStep)
		{
		testStep = new CSsmPlatsecTest();
		}
	else if (aStepName == KTSsmSwpCommonPluginStep)
		{
		testStep = new CSsmSwpCommonPluginTest();
		}
	else if (aStepName == KTSsmPanicCleStep)
		{
		testStep = new CSsmPanicCleTest();
		}
	else if (aStepName == KTSsmSwpDependenciesStep)
		{
		testStep = new CSsmSwpDependenciesTest();
		}
	else if (aStepName == KTSsmStateFurtherTransitionStep)
		{
		testStep = new CSsmStateFurtherTransitionTest();
		}
	else if (aStepName == KTSsmSwpHandleCleRetStep)
		{
		testStep = new CSsmSwpHandleCleRetTest();
		}
	else if (aStepName == KTSsmConditionalCommandsStep)
		{
		testStep = new CSsmConditionalCommandsTest();
		}
	else if (aStepName == KTSsmSsmCleDeadlockStep)
		{
		testStep = new CSsmSsmCleDeadlockTest();
		}
	else if (aStepName == KTSsmRepeatedPublishStateStep)
		{
		testStep = new CSsmRepeatedPublishStateTest();
		}
	else if (aStepName == KTSsmNotifyPropertyChangeStep)
		{
		testStep = new CSsmNotifyPropertyChangeTest();
		}
	else if (aStepName == KTSsmSwpRSwpPolicySessionStep)
		{
		testStep = new CSsmSwpRSwpPolicySessionTest();
		}
	else if (aStepName == KTSsmCmdExecutionOomStep)
		{
		testStep = new CSsmCmdExecutionOomTest();
		}
	
	else
		{
		RDebug::Printf ("Unknown SsmTest step %S", &aStepName );
		}

	return testStep;
	}
