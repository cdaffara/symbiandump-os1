// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file TestSuiteESockSSA.cpp
*/
#include "TestSuiteESockSSA.h"
#include "TestStepESockSSA.h"
#include "ESockSSA_Step1.h"

_LIT(KServerName,"TE_ESockSSA");

CTestServerESockSSA* CTestServerESockSSA::NewL()
/**
@return - Instance of the test server
Called inside the MainL() function to create and start the
CTestServer derived server.
*/
	{
	CTestServerESockSSA * server = new (ELeave) CTestServerESockSSA();
	CleanupStack::PushL(server);
	
	// ConstructL will permit
	// Server Logging.
	
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
Much simpler, uses the new Rendezvous() call to sync with the client
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
	
	CTestServerESockSSA* server = NULL;
	// Create the CTestServer derived server
	
	TRAPD(err,server = CTestServerESockSSA::NewL());
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


CTestServerESockSSA::CTestServerESockSSA()
	{
	}

CTestServerESockSSA::~CTestServerESockSSA()
	{
	}


CTestStep* CTestServerESockSSA::CreateTestStep(const TDesC& aStepName)
/**
@return - A CTestStep derived instance
Implementation of CTestServer pure virtual
*/
	{
	START_TEST_STEP_LIST
	ADD_TEST_STEP(CESockSSATest1)
	ADD_TEST_STEP(CESockSSATest2)
	ADD_TEST_STEP(CESockSSATest3)
	ADD_TEST_STEP(CESockSSATest4)
	ADD_TEST_STEP(CESockSSATest5)
	END_TEST_STEP_LIST
	}



