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

#include "Te_SimPhBkTestServer.h"
#include "Te_SimPhBk.h"

_LIT(KServerName,"Te_SimPhBk");

CSimPhBkTestServer* CSimPhBkTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSimPhBkTestServer * testServer = new (ELeave) CSimPhBkTestServer();
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
   	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	
	// Create the CTestServer derived server
	CSimPhBkTestServer* testServer = NULL;
	TRAPD(err,testServer = CSimPhBkTestServer::NewL());
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

CTestStep* CSimPhBkTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == _L("TestPhoneInfo"))
		{
		testStep = new CTestPhoneInfo();
		}
	else if(aStepName == _L("TestGetCapsAndInfo"))
		{
		testStep = new CTestGetCapsAndInfo();
		}
	else if(aStepName == _L("TestReadAndWrite"))
		{
		testStep = new CTestReadAndWrite();
		}
	else if(aStepName == _L("TestDelete"))
		{
		testStep = new CTestDelete();
		}
	else if(aStepName == _L("TestNotifications"))
		{
		testStep = new CTestNotifications();
		}
	else if(aStepName == _L("TestInvalidOps"))
		{
		testStep = new CTestInvalidOps();
		}
	else if(aStepName == _L("TestAdvancedPhBk"))
		{
		testStep = new CTestAdvancedPhBk();
		}
	else if(aStepName == _L("RetrievalOfWholePhoneBook"))
		{
		testStep = new CRetrievalOfWholePhoneBook();
		}
	else if(aStepName == _L("RetrievalOfBatchOfPhoneBookEntries"))
		{
		testStep = new CRetrievalOfBatchOfPhoneBookEntries();
		}
	else if(aStepName == _L("RetrievalOfHalfOfPhoneBookEntries"))
		{
		testStep = new CRetrievalOfHalfOfPhoneBookEntries();
		}
	else if(aStepName == _L("TestAllPhBooks"))
		{
		testStep = new CTestAllPhBooks();
		}

	return testStep;
	}




