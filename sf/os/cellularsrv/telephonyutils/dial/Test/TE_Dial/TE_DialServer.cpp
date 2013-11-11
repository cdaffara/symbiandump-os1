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
// Dial API Test server test code.
// 
//

/**
 @file
*/

#include "TE_DialServer.h"
#include "TE_DialTest.h"


_LIT(KServerName,"TE_Dial");


/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CDialTestServer* CDialTestServer::NewL()
	{
	CDialTestServer*  server = new(ELeave) CDialTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CDialTestIntServer::NewL


/**
 *  Much simpler, uses the new Rendezvous() call to sync with the client.
 */
LOCAL_C void MainL()
	{
	//
	// Start an active scheduler...
	//
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	//
	// Create the CTestServer derived server...
	//
	CDialTestServer* server = NULL;

	TRAPD(err, server = CDialTestServer::NewL());
	if (err == KErrNone)
		{
		//
		// Sync with the client and enter the active scheduler
		//
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

	//
	// Clean up...
	//
	CleanupStack::Pop(scheduler);
	delete server;
	delete scheduler;
	} // MainL
/**
 *  @return Standard Epoc error code on exit.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup*  cleanup = CTrapCleanup::New();

	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;

	return err;
	} // E32Main


/**
 *  Implementation of CTestServer pure virtual.
 *
 *  @return A CTestStep derived instance.
 */
CTestStep* CDialTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*  testStep = NULL;

    //
	// Create the required test step...
	//
	if (aStepName == _L("Test1"))
		{
		testStep = new CDialTestTest1();
		}
	else if (aStepName == _L("Test2"))
		{
		testStep = new CDialTestTest2();
		}
	else if (aStepName == _L("Test3"))
		{
		testStep = new CDialTestTest3();
		}
	else if (aStepName == _L("Test4"))
		{
		testStep = new CDialTestTest4();
		}
	else if (aStepName == _L("Test5"))
		{
		testStep = new CDialTestTest5();
		}
	else if (aStepName == _L("Test6"))
		{
		testStep = new CDialTestTest6();
		}
	else if (aStepName == _L("TestDtmf2"))
		{
		testStep = new CDialTestTestDtmf2();
		}
	else if (aStepName == _L("Test7"))
		{
		testStep = new CDialTestTest7();
		}
	else if (aStepName == _L("Test8"))
		{
		testStep = new CDialTestTest8();
		}
	else if (aStepName == _L("IllegalTest"))
		{
		testStep = new CDialTestIllegalTest();
		}

	return testStep;
	} // CDialTestServer::CreateTestStep
