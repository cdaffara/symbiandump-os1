// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_simeaptestserver.h"
#include "te_simeap.h"

_LIT(KServerName, "te_simeap");

/**
Same code for Secure and non-secure variants
Called inside the MainL() function to create and start the
CTestServer derived server.

@return Instance of the test server
*/
CSimEapTestServer* CSimEapTestServer::NewL()
	{
	CSimEapTestServer * testServer = new(ELeave) CSimEapTestServer();
	CleanupStack::PushL(testServer);
	testServer->ConstructL(KServerName);
	CleanupStack::Pop(testServer);
	return testServer;
	}

/**
Secure variant.
Much simpler, uses the new Rendezvous() call to sync with the client.
*/
LOCAL_C void MainL()
	{
	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	// Create the CTestServer derived server
	CSimEapTestServer* testServer = NULL;
	TRAPD(err, testServer = CSimEapTestServer::NewL());
	if(err == KErrNone)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

/**
Secure variant only.
Process entry point.  Called by client using RProcess API.

@return System-wide error code on process exit.
*/
GLDEF_C TInt E32Main()
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

/**
Implementation of CTestServer pure virtual.

@return A CTestStep derived instance.
*/
CTestStep* CSimEapTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CSimEapTestStepBase* testStep = NULL;

	if(aStepName == _L("TestSmartCardEap"))
		{
		testStep = new CTestSmartCardEap();
 		}
	else if(aStepName == _L("TestSmartCardAppAuthenticate"))
		{
		testStep = new CTestSmartCardAppAuthenticate();
 		}
	else if(aStepName == _L("TestGetSmartCardAppEapList"))
		{
		testStep = new CTestGetSmartCardAppEapList();
 		}
 	else if(aStepName == _L("TestSmartCardBasedEapBadClient"))
 		{
 		testStep = new CTestInitEapWithoutClosing();
 		}
 	else if(aStepName == _L("TestSmartCardAppAuthenticate2"))
 		{
 		testStep = new CTestSmartCardAppAuthenticate2();
 		}

	return testStep;
	}
