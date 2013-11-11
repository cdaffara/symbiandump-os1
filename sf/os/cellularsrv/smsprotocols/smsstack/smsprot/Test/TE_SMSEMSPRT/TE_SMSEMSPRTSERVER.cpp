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

#include "TE_SMSEMSPRTSERVER.h"
#include "TE_SMSEMSPRT.h"

_LIT(KServerName,"TE_SMSEMSPRT");
CSmsEmsPrtTestServer* CSmsEmsPrtTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CSmsEmsPrtTestServer* server = new (ELeave) CSmsEmsPrtTestServer();
	CleanupStack::PushL(server);
	// CSmsStackTestServer intermediate base class call
	server->InitializeTsyAndPhonesL();
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
	CSmsEmsPrtTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmsEmsPrtTestServer::NewL());
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

CTestStep* CSmsEmsPrtTestServer::CreateTestStep(const TDesC& aStepName)
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
	if (aStepName == _L("TestEmsFormatA"))
		{
		testStep = new CTestEmsFormatA ;
		}
	else if (aStepName == _L("TestEmsFormatB"))
		{
		testStep = new CTestEmsFormatB ;
		}
	else if (aStepName == _L("TestEmsFormatC"))
		{
		testStep = new CTestEmsFormatC ;
		}
	else if (aStepName == _L("TestEmsFormatD"))
		{
		testStep = new CTestEmsFormatD ;
		}
	else if (aStepName == _L("TestEmsFormatE"))
		{
		testStep = new CTestEmsFormatE ;
		}
	else if (aStepName == _L("TestEmsFormatF"))
		{
		testStep = new CTestEmsFormatF ;
		}
	else if (aStepName == _L("TestEmsSoundA"))
		{
		testStep = new CTestEmsSoundA ;
		}
	else if (aStepName == _L("TestEmsSoundB"))
		{
		testStep = new CTestEmsSoundB ;
		}
	else if (aStepName == _L("TestEmsSoundC"))
		{
		testStep = new CTestEmsSoundC ;
		}
	else if (aStepName == _L("TestEmsSoundD"))
		{
		testStep = new CTestEmsSoundD ;
		}
	else if (aStepName == _L("TestEmsPicture"))
		{
		testStep = new CTestEmsPicture ;
		}
	else if (aStepName == _L("TestEmsAnimation"))
		{
		testStep = new CTestEmsAnimation ;
		}
	else if (aStepName == _L("TestEmsUserPromptA"))
		{
		testStep = new CTestEmsUserPromptA ;
		}
	else if (aStepName == _L("TestEmsUserPromptB"))
		{
		testStep = new CTestEmsUserPromptB ;
		}
	else if (aStepName == _L("TestEmsLongMessageA"))
		{
		testStep = new CTestEmsLongMessageA ;
		}
	else if (aStepName == _L("TestEmsLongMessageB"))
		{
		testStep = new CTestEmsLongMessageB ;
		}
	else if (aStepName == _L("TestEmsLongMessageC"))
		{
		testStep = new CTestEmsLongMessageC ;
		}
	else if (aStepName == _L("TestEmsUnsupportedIE"))
		{
		testStep = new CTestEmsUnsupportedIE ;
		}
	else if (aStepName == _L("TestEmsUnsorted"))
		{
		testStep = new CTestEmsUnsorted ;
		}
	else if (aStepName == _L("TestEmsODIA"))
		{
		testStep = new CTestEmsODIA ;
		}
	else if (aStepName == _L("TestEmsODIB"))
		{
		testStep = new CTestEmsODIB ;
		}
	else if (aStepName == _L("TestEmsODIC"))
		{
		testStep = new CTestEmsODIC ;
		}
	else if (aStepName == _L("TestEmsODID"))
		{
		testStep = new CTestEmsODID ;
		}
	else if (aStepName == _L("TestEmsODIE"))
		{
		testStep = new CTestEmsODIE ;
		}
	else if (aStepName == _L("TestEmsODIF"))
		{
		testStep = new CTestEmsODIF;
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
