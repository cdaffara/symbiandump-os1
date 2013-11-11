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

#include <ssm/ssmswp.h>
 
#include "tcmd_server.h"
#include "tcmd_step_resourcereader.h"
#include "tcmd_step_commandlist.h"
#include "tcmd_step_reqswpchange.h"
#include "tcmd_step_publishsystemstate.h"
#include "tcmd_step_publishswp.h"
#include "tcmd_step_poweroff.h"
#include "tcmd_step_persisthalattributes.h"
#include "tcmd_step_loadsup.h"
#include "tcmd_step_customcommand.h"
#include "tcmd_step_finalisedrives.h"
#include "tcmd_step_amastarter.h"
#include "tcmd_step_multiplewait.h"
#include "tcmd_step_setpandskey.h"
#include "tcmd_step_startapp.h"
#include "tcmd_step_startprocess.h"
#include "tcmd_step_waitforapparcinit.h"
#include "tcmd_step_createswp.h"

#include "ssmdebug.h"

_LIT(KCmdTestServerName, "CmdTestServer");

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
		_LIT(KMainErrorStatement, "CmdTestServer::MainL - Error: %d");
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
CTestStep* CCmdTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTResourceReader)
		{
		testStep = new CResourceReaderTest();
		}
	else if (aStepName == KTCommandList)
		{
		testStep = new CCommandListTest();
		}
	else if (aStepName == KTCCmdTestCustomCommand)
		{
		testStep = new CCmdTestCustomCommand();
		}
	else if (aStepName == KTCCmdTestFinaliseDrives)
		{
		testStep = new CCmdTestFinaliseDrives();
		}
	else if (aStepName == KTCCmdTestLoadSup)
		{
		testStep = new CCmdTestLoadSup();
		}
	else if (aStepName == KTCCmdTestPersistHalAttributes)
		{
		testStep = new CCmdTestPersistHalAttributes();
		}
	else if (aStepName == KTCCmdTestPowerOff)
		{
		testStep = new CCmdTestPowerOff();
		}
	else if (aStepName == KTCCmdTestPublishSwp)
		{
		testStep = new CCmdTestPublishSwp();
		}
	else if (aStepName == KTCCmdTestPublishSystemState)
		{
		testStep = new CCmdTestPublishSystemState();
		}
	else if (aStepName == KTCCmdTestReqSwpChange)
		{
		testStep = new CCmdTestReqSwpChange();
		}
	else if (aStepName == KTCCmdTestAMAStarter)
		{
		testStep = new CCmdTestAMAStarter();
		}
	else if (aStepName == KTCCmdTestMultipleWait)
		{
		testStep = new CCmdTestMultipleWait();
		}
	else if (aStepName == KTCCmdTestSetPAndSKey)
		{
		testStep = new CCmdTestSetPAndSKey();
		}
	else if (aStepName == KTCCmdTestStartApp)
		{
		testStep = new CCmdTestStartApp();
		}
	else if (aStepName == KTCCmdTestStartProcess)
		{
		testStep = new CCmdTestStartProcess();
		}
	else if (aStepName == KTCCmdTestWaitForApparcInit)
		{
		testStep = new CCmdTestWaitForApparcInit();
		}
	else if (aStepName == KTCCmdTestCreateSwp)
		{
		testStep = new CCmdTestCreateSwp();
		}
	else
		{
		RDebug::Printf ("Unknown CmmTest step %S", &aStepName );
		}

	return testStep;
	}
