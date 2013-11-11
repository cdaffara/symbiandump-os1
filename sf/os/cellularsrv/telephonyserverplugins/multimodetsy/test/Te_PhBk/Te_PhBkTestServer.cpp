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

#include "Te_PhBkTestServer.h"
#include "Te_PhBk.h"

_LIT(KServerName,"Te_PhBk");

CPhBkTestServer* CPhBkTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CPhBkTestServer * testServer = new (ELeave) CPhBkTestServer();
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
	CPhBkTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CPhBkTestServer::NewL());
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

CTestStep* CPhBkTestServer::CreateTestStep(const TDesC& aStepName)
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
	return testStep;
	}
