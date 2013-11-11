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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// code at the __EDIT_ME__ tags
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
//#include "CapTestStep.h"
#include "DevSoundServerTestStep.h"



_LIT(KServerName,"DSCapTestServer");
// __EDIT_ME__ - Use your own server class name
CCapTestServer* CCapTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
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
	// __EDIT_ME__ Your server name
	CCapTestServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
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


//Add TestStep
CSimulProcTestStep* CCapTestServer::CreateTestStep(const TDesC& aStepName) const
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CSimulProcTestStep* testStep = NULL; 
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	
	//
	//1) SDevSound IntTest: Access DevSoundServer test mm-mmf-sdevsound-i-0016
	if(aStepName == _L("CSDevSoundTestStep1"))
		{
		testStep = CSDevSoundTestStep1::NewL();
		}
	

	return testStep;
	}

