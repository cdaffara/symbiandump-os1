// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test cases for the socket class
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file 	@file TestSuiteProtocol.cpp
*/

#include "TestSuiteProtocol.h"
#include "ProtocolTestSection.h"

_LIT(KServerName,"TE_Protocol");

CTestServerProtocol* CTestServerProtocol::NewL()
/**
@return - Instance of the test server
Called inside the MainL() function to create and start the
CTestServer derived server.
*/
	{
	CTestServerProtocol * server = new (ELeave) CTestServerProtocol();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	
	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
Much simpler, uses the new Rendezvous() call to sync with the client
*/
	{
	// Leave the hooks in for platform security

	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CTestServerProtocol* server = NULL;
	// Create the CTestServer derived server
	
	TRAPD(err,server = CTestServerProtocol::NewL());
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
@return - Standard Epoc error code on exit
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

CTestServerProtocol::CTestServerProtocol()
	{
	}

CTestServerProtocol::~CTestServerProtocol()
	{
	}


CTestStep* CTestServerProtocol::CreateTestStep(const TDesC& aStepName)
/**
@return - A CTestStep derived instance
Implementation of CTestServer pure virtual
*/
	{
	START_TEST_STEP_LIST
		ADD_TEST_STEP(CProtocolTest1)
		ADD_TEST_STEP(CProtocolTest2)
		ADD_TEST_STEP(CProtocolTest3)
		ADD_TEST_STEP(CProtocolTest4)
		ADD_TEST_STEP(CProtocolTest5)
		ADD_TEST_STEP(CProtocolTest6)
		ADD_TEST_STEP(CProtocolTest7)
		ADD_TEST_STEP(CProtocolTest8)
		ADD_TEST_STEP(CProtocolTest9)
		ADD_TEST_STEP(CProtocolTest10)
		ADD_TEST_STEP(CProtocolTest11)
		ADD_TEST_STEP(CProtocolTest12)
		ADD_TEST_STEP(CProtocolTest13)	
	END_TEST_STEP_LIST
	}



