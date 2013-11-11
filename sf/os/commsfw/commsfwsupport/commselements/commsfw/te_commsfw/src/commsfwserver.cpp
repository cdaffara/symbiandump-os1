// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file CommsFWServer.cpp
*/
#include "CommsFWServer.h"
#include "CommsFWStep.h"

_LIT(KServerName,"TE_CommsFW");
CCommsFWServer* CCommsFWServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CCommsFWServer * server = new (ELeave) CCommsFWServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CCommsFWServer::ConstructL(const TDesC&)
	{
	__CFLOG_CREATEL;
	}

CCommsFWServer::~CCommsFWServer()
	{
	__CFLOG_DELETE;
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
	CCommsFWServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCommsFWServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

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

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CCommsFWServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	// Need to set CFLog up for this new thread
   	__CFLOG_STMT( __cflog__->SetContext(); )
	__CFLOG_OPEN;

	CTestStep* testStep = NULL;
	if(aStepName.Compare(_L("TestUDSP")) == 0)
		testStep = new CTestUDSP;
	else if(aStepName.Compare(_L("TestUDTP")) == 0)
		testStep = new CTestUDTP;
	else if(aStepName.Compare(_L("TestBDSP")) == 0)
		testStep = new CTestBDSP;
	else if(aStepName.Compare(_L("TestPanic")) == 0)
		testStep = new CTestPanic;
	else if(aStepName.Compare(_L("TestRCFThread")) == 0)
		testStep = new CTestRCFThread;
	else if(aStepName.Compare(_L("TestTCFSubModuleAddress")) == 0)
		testStep = new CTestTCFSubModuleAddress;
	
	return testStep;
	}


