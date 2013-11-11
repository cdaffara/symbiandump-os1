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
// Telephony Watchers Integration Test server test code.
// 
//

/**
 @file
*/

#include "TE_TelWatchersIntServer.h"
#include "TE_TelWatchersIntTest.h"
#include "simtsy.h"


_LIT(KServerName,"TE_TelWatchersInt");


/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CTelWatchersIntTestServer* CTelWatchersIntTestServer::NewL()
	{
	CTelWatchersIntTestServer*  server = new(ELeave) CTelWatchersIntTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CTelWatchersIntTestIntServer::NewL


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
	CTelWatchersIntTestServer* server = NULL;

	TRAPD(err, server = CTelWatchersIntTestServer::NewL());
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
CTestStep* CTelWatchersIntTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*  testStep = NULL;

    //
	// Create the required test step...
	//		
	if (aStepName == _L("TestRetrieveSignalStrengthWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestRetrieveChargerStatusWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe();
		}

	if (aStepName == _L("TestNetworkStatusWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestNetworkStatusWithPublishSubscribe();
		}

	if (aStepName == _L("TestIncomingCallStatusWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestOutgoingCallStatusWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestIndicatorKErrNotSupportedScenarioWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe();
		}
		
	if (aStepName == _L("TestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe"))
		{
		testStep = new(ELeave) CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe();
		}

	return testStep;
	} // CTelWatchersIntTestServer::CreateTestStep
