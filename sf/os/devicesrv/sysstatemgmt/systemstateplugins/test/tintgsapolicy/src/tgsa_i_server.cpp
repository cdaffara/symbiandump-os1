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
 

#include "tgsa_i_server.h"
#include "gsatestapps.h"
#include "tgsa_step_statetrantodiffstates.h"
#include "tgsa_step_duplicatesubstate.h"
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
#include "tgsa_step_fleximergediffsubstate.h"
#include "tgsa_step_fleximergesamesubstate.h"
#include "tgsa_step_fleximergesortsamefile.h"
#include "tgsa_step_fleximergesortdifffile.h"
#include "tgsa_step_fleximergemanyfiles.h"
#endif

_LIT(KGsaIntTestServerName, "GsaIntTestServer");

CGsaIntTestServer* CGsaIntTestServer::NewLC( )
	{
	CGsaIntTestServer * server = new (ELeave) CGsaIntTestServer();
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

	CGsaIntTestServer* server = CGsaIntTestServer::NewLC( );
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
		_LIT(KMainErrorStatement, "GsaIntTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KGsaIntTestServerName, err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 @return - A CTestStep derived instance
 */
CTestStep* CGsaIntTestServer::CreateTestStep(const TDesC& aStepName )
	{
	//Following tests can be run only on techview emulator as it requires restarting of the emulator
//#ifdef __WINS__
	CTestStep* testStep = NULL;
	if (aStepName == KTGsaStateTranFromNormalStep)
		{
		testStep = new CGsaStateTranToDiffStatesTest(KTestProcTranFromNormal);
		}
	else if (aStepName == KTGsaStateTranFromStartupStep)
		{
		testStep = new CGsaStateTranToDiffStatesTest(KTestProcTranFromStartup);
		}
	else if (aStepName == KTGsaDuplicateSubstateStep)
		{
		testStep = new CGsaDuplicateSubstateTest();
		}
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	else if (aStepName == KTGsaGsaFlexiMergeDiffSubstateStep)
		{
		testStep = new CGsaFlexiMergeDiffSubstateTest();
		}
	else if (aStepName == KTGsaGsaFlexiMergeSameSubstateStep)
		{
		testStep = new CGsaFlexiMergeSameSubstateTest();
		}	
	else if (aStepName == KTGsaFlexiMergeSortSameFileStep)
		{
		testStep = new CGsaFlexiMergeSortSameFileTest();
		}
	else if (aStepName == KTGsaFlexiMergeSortDiffFileStep)
		{
		testStep = new CGsaFlexiMergeSortDiffFileTest();
		}
	else if (aStepName == KTGsaFlexiMergeManyFilesStep)
		{
		testStep = new CGsaFlexiMergeManyFilesTest();
		}
#endif
	else
		{
		RDebug::Printf ("Unknown GsaIntTest step %S", &aStepName );
		}
	return testStep;
/*#else
	return NULL;
#endif*/
	}
