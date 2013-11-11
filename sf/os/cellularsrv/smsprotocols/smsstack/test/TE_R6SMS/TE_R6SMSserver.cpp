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

#include "TE_R6SMSserver.h"
#include "TE_testR6SMS.h"


_LIT(KServerName,"TE_R6SMS");


CR6SMSTestServer* CR6SMSTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Same code for Secure and non-secure variants
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CR6SMSTestServer * server = new (ELeave) CR6SMSTestServer();
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
	CR6SMSTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CR6SMSTestServer::NewL());
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

CTestStep* CR6SMSTestServer::CreateTestStep(const TDesC& aStepName)
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
	if (aStepName == _L("CTestCSmsMessageAdditionalAttributes1"))
	    {
		testStep = new CTestCSmsMessageAdditionalAttributes1;
	    }
	else if (aStepName == _L("CTestCSmsMessageAdditionalAttributes2"))
	    {
		testStep = new CTestCSmsMessageAdditionalAttributes2;
	    }
	else if (aStepName == _L("CTestCSmsMessageAdditionalAttributes3"))
	    {
		testStep = new CTestCSmsMessageAdditionalAttributes3;
	    }
 	else if (aStepName == _L("CTestR6CSmsMessage"))
	    {
		testStep = new CTestR6CSmsMessage;
	    }
	else if (aStepName == _L("CTestAddingIEsViaNewAndExistingInterfaces"))
	    {
		testStep = new CTestAddingIEsViaNewAndExistingInterfaces;
	    }
	else if (aStepName == _L("CTestIECategoryDefinitions"))
	    {
		testStep = new CTestIECategoryDefinitions;
	    }
	else if (aStepName == _L("CTestR6DCS"))
	    {
		testStep = new CTestR6DCS;
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
