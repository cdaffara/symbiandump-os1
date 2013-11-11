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
//

#include "Te_DataTestServer.h"
#include "Te_Data.h"

_LIT(KServerName,"Te_Data");

CDataTestServer* CDataTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CDataTestServer * testServer = new (ELeave) CDataTestServer();
	CleanupStack::PushL(testServer);

	testServer->ConstructL(KServerName);
	CleanupStack::Pop(testServer);
	return testServer;
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CDataTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CDataTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

CTestStep* CDataTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    
	if(aStepName == _L("TestLineStatusIncommingCall"))
		{
		testStep = new CTestLineStatusIncommingCall();
		}
	else if(aStepName == _L("TestLineStatusOutgoingCall"))
		{
		testStep = new CTestLineStatusOutgoingCall();
		}
	else if(aStepName == _L("TestLineInfo"))
		{
		testStep = new CTestLineInfo();
		}
	else if(aStepName == _L("TestLineNotificationsIncommingCall"))
		{
		testStep = new CTestLineNotificationsIncommingCall();
		}
	else if(aStepName == _L("TestLineNotificationsOutgoingCall"))
		{
		testStep = new CTestLineNotificationsOutgoingCall();
		}
	else if(aStepName == _L("TestLineCancels"))
		{
		testStep = new CTestLineCancels();
		}
	else if(aStepName == _L("TestCallInfo"))
		{
		testStep = new CTestCallInfo();
		}
	else if(aStepName == _L("TestCallNotifications"))
		{
		testStep = new CTestCallNotifications();
		}
	else if(aStepName == _L("TestCallStatusIncommingCall"))
		{
		testStep = new CTestCallStatusIncommingCall();
		}
	else if(aStepName == _L("TestCallStatusOutgoingCall"))
		{
		testStep = new CTestCallStatusOutgoingCall();
		}
	else if(aStepName == _L("TestCallCancels"))
		{
		testStep = new CTestCallCancels();
		}
	else if(aStepName == _L("TestHandShake"))
		{
		testStep = new CTestHandShake();
		}
	else if(aStepName == _L("TestGetMobileDataCallCapsL"))
		{
		testStep = new CTestGetMobileDataCallCaps();
		}
	return testStep;
	}
