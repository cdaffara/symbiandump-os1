// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ElementServer.h"
#include "ElementStep.h"
#include "StateMachineStep.h"

_LIT(KServerName,"Te_ElementServer");

CElementServer* CElementServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CElementServer * server = new (ELeave) CElementServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}



LOCAL_C void MainL()
/**
 * Secure variant
 * Uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CElementServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CElementServer::NewL());
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
 * @return - Standard Epoc error code on process exit
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


CTestStep* CElementServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	if(aStepName == KElementStep1)
		testStep = new CElementStep1();
	else if(aStepName == KElementStep2)
		testStep = new CElementStep2(*this);
	else if(aStepName == KElementStep3)
		testStep = new CElementStep3(*this);
	else if (aStepName == KElementStateMachineStep1_1)
	    testStep = new CElementStateMachineStep1_1();
	else if (aStepName == KElementStateMachineStep1_2)
	    testStep = new CElementStateMachineStep1_2();
	else if (aStepName == KElementStateMachineStep1_3)
	    testStep = new CElementStateMachineStep1_3();
    else if (aStepName == KElementStateMachineStep1_4)
        testStep = new CElementStateMachineStep1_4();
    else if (aStepName == KElementStateMachineStep1_5)
        testStep = new CElementStateMachineStep1_5();
    else if (aStepName == KElementStateMachineStep2_1)
        testStep = new CElementStateMachineStep2_1();
    else if (aStepName == KElementStateMachineStep2_2)
        testStep = new CElementStateMachineStep2_2();
    else if (aStepName == KElementStateMachineStep2_3)
        testStep = new CElementStateMachineStep2_3();
    else if (aStepName == KElementStateMachineStep2_4)
        testStep = new CElementStateMachineStep2_4();
    else if (aStepName == KElementStateMachineStep2_5)
        testStep = new CElementStateMachineStep2_5();
    else if (aStepName == KElementStateMachineStep2_6)
        testStep = new CElementStateMachineStep2_6();
    else if (aStepName == KElementStateMachineStep2_7)
        testStep = new CElementStateMachineStep2_7();
    else if (aStepName == KElementStateMachineStep2_8)
        testStep = new CElementStateMachineStep2_8();
	return testStep;
	}
