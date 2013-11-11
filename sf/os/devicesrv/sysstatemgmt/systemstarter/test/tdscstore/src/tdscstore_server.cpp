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

#include "tdscstore_server.h"
#include "tdscstore_step_base.h"
#include "tdscstore_step_dsc_command.h"


_LIT(KCAppfwkDSCStoreTestServerName,"tdscstore_server");

/**
  @return - Instance of the test server
  Called inside the MainL() function to create and start the
  CTestServer derived server.
 */
CAppfwkDSCStoreTestServer* CAppfwkDSCStoreTestServer::NewL()
	{
	CAppfwkDSCStoreTestServer * server = new (ELeave) CAppfwkDSCStoreTestServer();
	CleanupStack::PushL(server);
	TParsePtrC serverName(RProcess().FileName());	
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	// Create the CTestServer derived server, sync with the client and enter the active scheduler
	CAppfwkDSCStoreTestServer* server = CAppfwkDSCStoreTestServer::NewL();
	RProcess::Rendezvous(KErrNone);
	sched->Start();
	
	delete server;
	CleanupStack::PopAndDestroy(sched);
	}

/**
  @return - Standard Epoc error code on process exit
  Process entry point. Called by client using RProcess API
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
		_LIT(KE32MainErrorStatement, "CAppfwkDSCStoreTestServer::MainL - Error: %d");
		RDebug::Print(KE32MainErrorStatement, err);
	   	User::Panic(KCAppfwkDSCStoreTestServerName,err);
		}
		
	delete cleanup;
	
	__UHEAP_MARKEND;
	return KErrNone;
    }

/**
  @return - A CTestStep derived instance
  @param - aStepName the test case name to be executed
  Implementation of CTestServer pure virtual
 */
CTestStep* CAppfwkDSCStoreTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	testStep = new CAppfwkDscStoreTestStepDscCommand(aStepName);
	
	return testStep;
	}
