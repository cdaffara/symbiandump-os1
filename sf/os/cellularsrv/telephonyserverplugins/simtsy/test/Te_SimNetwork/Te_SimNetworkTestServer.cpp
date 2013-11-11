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

#include "Te_SimNetworkTestServer.h"
#include "Te_SimNetwork.h"

_LIT(KServerName,"Te_SimNetwork");

CSimNetworkTestServer* CSimNetworkTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSimNetworkTestServer * testServer = new (ELeave) CSimNetworkTestServer();
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
	CSimNetworkTestServer* testServer = NULL;
	TRAPD(err,testServer = CSimNetworkTestServer::NewL());
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

CTestStep* CSimNetworkTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == _L("GetNetworkCapsTests"))
		{
		testStep = new CGetNetworkCapsTests();
		}
	else if(aStepName == _L("NetworkRegistrationStatusTests"))
		{
		testStep = new CNetworkRegistrationStatusTests();
		}
	else if(aStepName == _L("ModeTests"))
		{
		testStep = new CModeTests();
		}
	else if(aStepName == _L("CurrentNetworkTests"))
		{
		testStep = new CCurrentNetworkTests();
		}
	else if(aStepName == _L("HomeNeworkTests"))
		{
		testStep = new CHomeNeworkTests();
		}
	else if(aStepName == _L("NetworkSelectionSettingTests"))
		{
		testStep = new CNetworkSelectionSettingTests();
		}
	else if(aStepName == _L("NetworkTimeDateTests"))
		{
		testStep = new CNetworkTimeDateTests();
		}
	else if(aStepName == _L("NetworkCellInfoTests"))
		{
		testStep = new CNetworkCellInfoTests();
		}
	else if(aStepName == _L("DetectedNetworkTest"))
		{
		testStep = new CDetectedNetworkTest();
		}
	else if(aStepName == _L("ServerInfo"))
		{
		testStep = new CServerInfo();
		}

	return testStep;
	}




