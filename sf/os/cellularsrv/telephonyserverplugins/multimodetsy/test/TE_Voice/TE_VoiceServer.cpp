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
// Voice API Test server test code.
// 
//

/**
 @file
*/

#include "TE_VoiceServer.h"
#include "TE_Voice.h"

/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CVoiceTestServer* CVoiceTestServer::NewL()
	{
	CVoiceTestServer*  server = new(ELeave) CVoiceTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CVoiceTestIntServer::NewL

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
	CVoiceTestServer* server = NULL;

	TRAPD(err, server = CVoiceTestServer::NewL());
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
	
	__UHEAP_MARK;
	
	CTrapCleanup*  cleanup = CTrapCleanup::New();

	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;

	__UHEAP_MARKEND;

	return err;
	} // E32Main


/**
 *  Implementation of CTestServer pure virtual.
 *
 *  @return A CTestStep derived instance.
 */
CTestStep* CVoiceTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*  testStep = NULL;

    //
	// Create the required test step...
	//
	if (aStepName == _L("TestLineInfo"))
		{
		testStep = new CTestLineInfo();
		}
	else if (aStepName == _L("TestPhoneInfo"))
		{
		testStep = new CTestPhoneInfo();
		}
	else if (aStepName == _L("TestLineStatusIncomingCall"))
		{
		testStep = new CTestLineStatusIncomingCall();
		}
	else if (aStepName == _L("TestLineStatusOutgoingCall"))
		{
		testStep = new CTestLineStatusOutgoingCall();
		}
	else if (aStepName == _L("TestLineCapabilities"))
		{
		testStep = new CTestLineCapabilities();
		}
	else if (aStepName == _L("TestLineNotificationsIncomingCall"))
		{
		testStep = new CTestLineNotificationsIncomingCall();
		}
	else if (aStepName == _L("TestLineNotificationsOutgoingCall"))
		{
		testStep = new CTestLineNotificationsOutgoingCall();
		}
	else if (aStepName == _L("TestLineCancels"))
		{
		testStep = new CTestLineCancels();
		}
	else if (aStepName == _L("TestCallInfo"))
		{
		testStep = new CTestCallInfo();
		}
	else if (aStepName == _L("TestCallNotificationsIncomingCall"))
		{
		testStep = new CTestCallNotificationsIncomingCall();
		}
	else if (aStepName == _L("TestCallNotificationsOutgoingCall"))
		{
		testStep = new CTestCallNotificationsOutgoingCall();
		}
	else if (aStepName == _L("TestCallStatusIncomingCall"))
		{
		testStep = new CTestCallStatusIncomingCall();
		}
	else if (aStepName == _L("TestCallStatusOutgoingCall"))
		{
		testStep = new CTestCallStatusOutgoingCall();
		}
	else if (aStepName == _L("TestCallCancels"))
		{
		testStep = new CTestCallCancels();
		}
	else if (aStepName == _L("TestMoreCallInfo"))
		{
		testStep = new CTestMoreCallInfo();
		}
	else if (aStepName == _L("TestCloseCallWhenActive"))
		{
		testStep = new CTestCloseCallWhenActive();
		}
	else if (aStepName == _L("TestCancelDialRequest"))
		{
		testStep = new CTestCancelDialRequest();
		}

		
	return testStep;
	} // CVoiceTestServer::CreateTestStep
