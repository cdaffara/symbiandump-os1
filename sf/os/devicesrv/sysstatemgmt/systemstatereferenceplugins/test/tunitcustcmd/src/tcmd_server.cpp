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

#include "tcmd_server.h"
#include "tcmd_step_simsecuritycheck.h"
#include "tcmd_step_devicesecuritycheck.h"
#include "tcmd_step_coopsysselftest.h"
#include "tcmd_step_coopsysperformrestartactions.h"
#include "tcmd_step_coopsysperformshutdownactions.h"
#include "tcmd_step_coopsysperformrfsactions.h"
#include "tcmd_step_activaterfforemergencycall.h"
#include "tcmd_step_deactivaterfforemergencycall.h"
#include "tcmd_step_coopsysstatechange.h"
#include "tcmd_step_publishstartupmode.h"
#include "tcmd_step_publishsimownedandchanged.h"
#include "tcmd_step_execute.h"

_LIT(KCustomCmdTestServerName, "tunitcustcmd_server");

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
		RDebug::Print(_L("CmdTestServer::MainL - Error: %d"), err);
		User::Panic(KCustomCmdTestServerName, err);
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
	RDebug::Print(_L("CCmdTestServer::CreateTestStep"));
	CTestStep* testStep = NULL;
	if (aStepName == KTCCustomCmdTestSimSecurityCheck)
		{
		testStep = new CCustomCmdTestSimSecurityCheck();
		}
	else if (aStepName == KTCCustomCmdTestDeviceSecurityCheck)
		{
		testStep = new CCustomCmdTestDeviceSecurityCheck();
		}
	else if (aStepName == KTCCustomCmdTestCoopSysSelfTest)
		{
		testStep = new CCustomCmdTestCoopSysSelfTest();
		}
	else if (aStepName == KTCCustomCmdTestCoopSysPerformRestartActions)
		{
		testStep = new CCustomCmdTestCoopSysPerformRestartActions();
		}
	else if (aStepName == KTCCustomCmdTestCoopSysPerformShutdownActions)
		{
		testStep = new CCustomCmdTestCoopSysPerformShutdownActions();
		}
	else if (aStepName == KTCCustomCmdTestCoopSysPerformRfsActions)
		{
		testStep = new CCustomCmdTestCoopSysPerformRfsActions();
		}
	else if (aStepName == KTCCustomCmdTestActivateRfForEmergencyCall)
		{
		testStep = new CCustomCmdTestActivateRfForEmergencyCall();
		}
	else if (aStepName == KTCCustomCmdTestDeactivateRfForEmergencyCall)
		{
		testStep = new CCustomCmdTestDeactivateRfForEmergencyCall();
		}
	else if (aStepName == KTCCustomCmdTestCoopSysStateChange)
		{
		testStep = new CCustomCmdTestCoopSysStateChange();
		}
	else if (aStepName == KTestPublishStartupModeWithCap)
		{
		testStep = new CCustomCmdTestPublishStartupMode(KTestPublishStartupModeWithCap);
		}
	else if (aStepName == KTestPublishStartupModeWithoutCap)
        {
        testStep = new CCustomCmdTestPublishStartupMode(KTestPublishStartupModeWithoutCap);
        }
	else if (aStepName == KTCCustomCmdTestPublishSimOwnedAndChanged)
	    {
	    testStep = new CCustomCmdTestPublishSimOwnedAndChanged();
	    }
	else if (aStepName == KTestCustomCmdExecute)
		{
		testStep = new CTestCustomCmdExecute();
		}
	else
		{
		RDebug::Print(_L("Unknown CmdTest step %S"), &aStepName);
	   	User::Panic(KCustomCmdTestServerName, KErrArgument);
		}

	return testStep;
	}
