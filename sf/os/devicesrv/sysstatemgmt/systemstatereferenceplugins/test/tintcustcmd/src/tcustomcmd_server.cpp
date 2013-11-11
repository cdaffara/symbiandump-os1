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

#include "tcustomcmd_server.h"
#include "tcustcmd_step_simsecuritypincheck.h"
#include "tcustcmd_step_activateemergencycall.h"
#include "tcustcmd_step_deactivateemergencycall.h"
#include "tcustcmd_step_devicesecuritypincheck.h"
#include "tcustcmd_step_int_execute.h"
#include "ssmdebug.h"

_LIT(KCustomCmdTestServerName, "tcustomcmd_server");

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
		DEBUGPRINT2(_L("CustCmdTestServer::MainL - Error: %d"), err);
	   	SSMLOGLEAVE(err);
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

	if (aStepName == KTCCustomCmdTestSecurityPinCheck)
		{
		testStep = new CCustomCmdTestSecurityPinCheck();
		}
	else if (aStepName == KTCCustomCmdTestDeviceSecurityPinCheck)
		{
		testStep = new CCustomCmdTestDeviceSecurityPinCheck();
		}	
	else if (aStepName == KTCCustomCmdTestActivateEmergencyCall)
		{
		testStep = new CCustomCmdTestActivateEmergencyCall();
		}
	else if (aStepName == KTCCustomCmdTestDeActivateEmergencyCall)
		{
		testStep = new CCustomCmdTestDeActivateEmergencyCall();
		}
	else if (aStepName == KTestExecuteCustCmd)
		{
		testStep = new CTestExecuteCustCmd();
		}		
		
	else 
		{
		DEBUGPRINT2(_L("Unknown CmdTest step %S"), &aStepName);
	   	User::Panic(KCustomCmdTestServerName, KErrArgument);
		}

	return testStep;
	}

