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

#include "TE_smspdudbserver.h"
#include "TE_testsmspdudb.h"


_LIT(KServerName,"TE_SMSPDUDB");


CSmsPduDbTestServer* CSmsPduDbTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Same code for Secure and non-secure variants
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CSmsPduDbTestServer * server = new (ELeave) CSmsPduDbTestServer();
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
	CSmsPduDbTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmsPduDbTestServer::NewL());
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

CTestStep* CSmsPduDbTestServer::CreateTestStep(const TDesC& aStepName)
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
	if(aStepName == _L("TestSmsPduDbMessage"))
		{
		testStep = new CTestSmsPduDbMessage ;
		}
	else if (aStepName == _L("TestSmsPduDbPdu"))
		{
		testStep = new CTestSmsPduDbPdu ;
		}
	else if (aStepName == _L("TestSmsPduDbConcatSegment"))
		{
		testStep = new CTestSmsPduDbConcatSegment ;
		}
	else if (aStepName == _L("TestSmsPduDbConcat"))
		{
		testStep = new CTestSmsPduDbConcat ;
		}
	else if (aStepName == _L("TestSmsPduDbParse"))
		{
		testStep = new CTestSmsPduDbParse ;
		}
	else if (aStepName == _L("TestSmsPduDbCompile"))
		{
		testStep = new CTestSmsPduDbCompile ;
		}
	else if (aStepName == _L("TestSmsPduDbTools"))
		{
		testStep = new CTestSmsPduDbTools ;
		}
	else if (aStepName == _L("TestSmsServiceCenterTimeStamp"))
		{
		testStep = new CTestSmsServiceCenterTimeStamp;
		}
	else if(aStepName == _L("TestSmsValidityPeriod"))
		{
		testStep = new CTestSmsValidityPeriod;
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
