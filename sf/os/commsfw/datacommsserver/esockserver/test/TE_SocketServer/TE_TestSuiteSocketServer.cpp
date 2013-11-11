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
//



// EPOC includes
#include <e32base.h>

// Test system includes
//#include "TestStepSocket.h"
#include "TE_TestSuiteSocketServer.h"
#include "TE_SocketServerTestSections.h"
#include "TE_TestSuiteSocketServer.h"


//Forward declarations
LOCAL_C void MainL();


GLDEF_C TInt E32Main()
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


LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestServerSocketServer * server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestServerSocketServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


CTestServerSocketServer* CTestServerSocketServer::NewL()
	{
	CTestServerSocketServer* server = new(ELeave) CTestServerSocketServer;
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	//	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestServerSocketServer::~CTestServerSocketServer()
	{
	}

CTestServerSocketServer::CTestServerSocketServer()
	{
	}


#define START_TEST_STEP_LIST

// No ELeave as harness will test ptr. This is more efficient
// than using TRAP_IGNORE
#define ADD_TEST_STEP(c) \
	if (aStepName == c::GetTestName()) \
	{ \
	CTestStepSocketServer* ret= new c; \
	if(ret) \
	{ \
	ret->SetTestStepName(c::GetTestName()); \
	} \
	return ret; \
	}\
else 

#define END_TEST_STEP_LIST \
	{ \
	return NULL; \
		}

CTestStep* CTestServerSocketServer::CreateTestStep(const TDesC& aStepName)
	{
	START_TEST_STEP_LIST
		ADD_TEST_STEP(CSocketServerTest1)
		ADD_TEST_STEP(CSocketServerTest2)
		ADD_TEST_STEP(CSocketServerTest3)
		ADD_TEST_STEP(CSocketServerTest5)
		ADD_TEST_STEP(CSocketServerTest7)
		ADD_TEST_STEP(CSocketServerTest9)
		ADD_TEST_STEP(CSocketServerTest16)
		ADD_TEST_STEP(CSocketServerTest17)
		ADD_TEST_STEP(CSocketServerTest18)
		ADD_TEST_STEP(CSocketServerTest20)
		ADD_TEST_STEP(CSocketServerTest21)
		ADD_TEST_STEP(CSocketServerTest22)
		ADD_TEST_STEP(CSocketServerTest23)
		ADD_TEST_STEP(CSocketServerTest24)
		ADD_TEST_STEP(CSocketServerTest25)
		ADD_TEST_STEP(CSocketServerTest26)
		ADD_TEST_STEP(CSocketServerTest27)
		ADD_TEST_STEP(CSocketServerTest28)
		ADD_TEST_STEP(CSocketServerTest29)
	END_TEST_STEP_LIST
	}

