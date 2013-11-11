// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test code to develop a TestExecute Server
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file CapTestServer.cpp
*/

#include <simulprocclient.h>

#include "CapTestServer.h"
#include "CapTestStep0012.h"
#include "CapTestStep0014.h"


_LIT(KServerName,"CapTestServer");
CCapTestServer* CCapTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CCapTestServer * server = new (ELeave) CCapTestServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	server->StartL(KServerName); 

	//server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
CCapTestServer::CCapTestServer()
	{
	}
	
CCapTestServer::~CCapTestServer()
	{
	}
	

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CCapTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCapTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }


CSimulProcTestStep* CCapTestServer::CreateTestStep(const TDesC& aStepName) const
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CSimulProcTestStep* testStep = NULL; 
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created

	if(aStepName == _L("CapTestStep0012"))
		testStep = CAudPlayUtilTS0012::NewL();
	else if(aStepName == _L("CapTestStep0014"))
		testStep = CAudPlayUtilTS0014::NewL();

	return testStep;
	}

