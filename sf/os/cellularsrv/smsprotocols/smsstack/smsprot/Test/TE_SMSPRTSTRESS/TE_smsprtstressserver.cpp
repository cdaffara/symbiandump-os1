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
//

/**
 @file
*/

#include "TE_smsprtstressserver.h"

#include "TE_smsprtstress.h"

_LIT(KServerName,"TE_SMSPRTSTRESS");

CSmsPrtStressTestServer* CSmsPrtStressTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CSmsPrtStressTestServer * server = new (ELeave) CSmsPrtStressTestServer();
	CleanupStack::PushL(server);
	// CSmsStackTestServer intermediate base class call
//	server->InitializeTsyAndPhonesL();
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
	CSmsPrtStressTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmsPrtStressTestServer::NewL());
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

CTestStep* CSmsPrtStressTestServer::CreateTestStep(const TDesC& aStepName)
/**
 *  @return - A CTestStep derived instance
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
	if (aStepName == _L("TestRxAndRead"))
		{
		testStep = new CTestRxAndRead ;
		}
	else if (aStepName == _L("TestMultipleSimultaneousRequests"))
		{
		testStep = new CTestMultipleSimultaneousRequests ;
		}
	else if (aStepName == _L("TestSmsPrtBackup"))
		{
		testStep = new CTestSmsPrtBackup ;
		}
	else if (aStepName == _L("TestSmsPrtBackupWhenPhoneIsOff"))
		{
		testStep = new CTestSmsPrtBackupWhenPhoneIsOff();
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
