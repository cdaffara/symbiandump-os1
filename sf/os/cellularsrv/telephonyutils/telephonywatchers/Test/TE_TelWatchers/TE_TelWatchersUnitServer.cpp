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
// Telephony Watchers Unit Test server test code.
// 
//

/**
 @file
*/

#include "TE_TelWatchersUnitServer.h"
#include "TE_TelWatchersUnitTest.h"


_LIT(KServerName,"TE_TelWatchersUnit");


/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CTelWatchersUnitTestServer* CTelWatchersUnitTestServer::NewL()
	{
	CTelWatchersUnitTestServer*  server = new(ELeave) CTelWatchersUnitTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CTelWatchersUnitTestServer::NewL


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
	CTelWatchersUnitTestServer* server = NULL;

	TRAPD(err, server = CTelWatchersUnitTestServer::NewL());
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

	TRAP_IGNORE(MainL());

	delete cleanup;

	return KErrNone;
	} // E32Main


/**
 *  Implementation of CTestServer pure virtual.
 *
 *  @return A CTestStep derived instance.
 */
CTestStep* CTelWatchersUnitTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*  testStep = NULL;

    //
	// Create the required test step...
	//		
	if (aStepName == _L("TestSimulteCommDBModemChangeWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestCommDBChangeWhenWatcherDisabledWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestCallStateChangesWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersUnitTestCallStateChangesWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestBatteryChargerChangesWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestNetworkAvailabilityChangesWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe();
		}

	return testStep;
	} // CTelWatchersUnitTestServer::CreateTestStep
