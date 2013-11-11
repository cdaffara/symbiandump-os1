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
// @file
// 
//


#include "TE_wapsmsserver.h"
#include "TE_testwapsms.h"


_LIT(KServerName,"TE_WAPSMS");


CWapSmsTestServer* CWapSmsTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Same code for Secure and non-secure variants
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
{
	CWapSmsTestServer * server = new (ELeave) CWapSmsTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
}

LOCAL_C void MainL()
/**
 *  Much simpler, uses the new Rendezvous() call to sync with the client
 */
{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CWapSmsTestServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CWapSmsTestServer::NewL());
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
 *  @return - Standard Epoc error code on exit
 */
{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
	{
		return KErrNoMemory;
	}
	TRAPD(err,MainL());
	delete cleanup;
	return err;
}

CTestStep* CWapSmsTestServer::CreateTestStep(const TDesC& aStepName)
/**
 *  @return - A CTestStep derived instance
 *  Secure and non-secure variants
 *  Implementation of CTestServer pure virtual
 */
	{
	CTestStep*  testStep = NULL;
	
	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//
	if (aStepName == _L("TestTxRx7DCS"))
		{
		testStep = new CTestTxRx7DCS ;
		}
	else if (aStepName == _L("TestTxRx8DCS"))
		{
		testStep = new CTestTxRx8DCS ;
		}
	else if (aStepName == _L("TestTxRxRingTone"))
		{
		testStep = new CTestTxRxRingTone ;
		}
	else if (aStepName == _L("TestTxRxWap"))
		{
		testStep = new CTestTxRxWap;
		}
	else if (aStepName == _L("TestWapAndApplicationPortBinding"))
		{
		testStep = new CTestWapAndApplicationPortBinding ;
		}
	else if (aStepName == _L("TestSendAndRecvViaAppllicationPortAndWap"))
		{
		testStep = new CTestSendAndRecvViaAppllicationPortAndWap;
		}
	else if (aStepName == _L("TestUnSupportedAck"))
		{
		testStep = new CTestUnSupportedAck ;
		}
	else if (aStepName == _L("TestConsecutiveTx"))
		{
		testStep = new CTestConsecutiveTx;
		}
	else if (aStepName == _L("TestSinglePDUWapMessageWithConCatRef"))
		{
		testStep = new CTestSinglePDUWapMessageWithConCatRef;
		}

	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}
	
	return testStep;
	}
