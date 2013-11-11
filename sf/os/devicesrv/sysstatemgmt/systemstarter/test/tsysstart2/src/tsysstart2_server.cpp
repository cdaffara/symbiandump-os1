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


#include "tsysstart2_server.h"

#include "tsysstart2_teststep.h"
#include "tsysstart2_procmon.h"
#include "tsysstart2_procmondeferred.h"
#include "tsysstart2_procstart.h"

_LIT(KServerName, "tsysstart2");

CAppFwkSysStart2TestServer* CAppFwkSysStart2TestServer::NewL()
	{
	CAppFwkSysStart2TestServer * server = new (ELeave) CAppFwkSysStart2TestServer();
	CleanupStack::PushL(server);
	
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	// Create the CTestServer derived server, sync with the client and enter the active scheduler
	CAppFwkSysStart2TestServer* server = CAppFwkSysStart2TestServer::NewL();
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	if( cleanup == NULL )
		{
		return KErrNoMemory;
		}
		
	TRAPD( err, MainL() );
	
	if ( KErrNone != err)
		{
		RDebug::Print( _L("MainL - Error: %d"), err );
	   	User::Panic( KServerName, err );
		}
	
	
	delete cleanup;
	return KErrNone;
    }



CTestStep* CAppFwkSysStart2TestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if (aStepName == KCTestCaseProcMon)
		{
		testStep = new CAppFwkSysStart2TestStepProcMon();
		}
	else if (aStepName == KCTestCaseProcMonDeferred)
		{
		testStep = new CAppFwkSysStart2TestStepProcMonDeferred();
		}
	else if (aStepName == KCTestCaseProcStart)
		{
		testStep = new CAppFwkSysStart2TestStepProcStart();
		}

	return testStep;
	}
	
