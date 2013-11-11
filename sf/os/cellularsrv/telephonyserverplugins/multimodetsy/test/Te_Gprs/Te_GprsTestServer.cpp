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

#include "Te_GprsTestServer.h"
#include "Te_Gprs.h"

_LIT(KServerName,"Te_Gprs");

CGprsTestServer* CGprsTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CGprsTestServer * testServer = new (ELeave) CGprsTestServer();
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
	CGprsTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CGprsTestServer::NewL());
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

CTestStep* CGprsTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    
	if(aStepName == _L("InitGlobals"))
		{
		testStep = new CInitGlobals;
		}
	else if(aStepName == _L("TestGprsDefaultContextParams"))
		{
		testStep = new CTestGprsDefaultContextParams;
		}
	else if(aStepName == _L("TestGprsUnSupported"))
		{
		testStep = new CTestGprsUnSupported;
		}
	else if(aStepName == _L("TestGprsContextAttach"))
		{
		testStep = new CTestGprsContextAttach;
		}
	else if(aStepName == _L("TestGprsContextDelete"))
		{
		testStep = new CTestGprsContextDelete;
		}
	else if(aStepName == _L("TestGprsContextLoanComport"))
		{
		testStep = new CTestGprsContextLoanComport;
		}
	else if(aStepName == _L("TestGprsContextUnSupported"))
		{
		testStep = new CTestGprsContextUnSupported;
		}
	else if(aStepName == _L("TestGprsAttach"))
		{
		testStep = new CTestGprsAttach;
		}
	else if(aStepName == _L("TestGprsNotification"))
		{
		testStep = new CTestGprsNotification;
		}
	else if(aStepName == _L("TestGprsMsClass"))
		{
		testStep = new CTestGprsMsClass;
		}
	else if(aStepName == _L("TestGprsContextConfig"))
		{
		testStep = new CTestGprsContextConfig;
		}
	else if(aStepName == _L("TestGprsContextConfigNotifications"))
		{
		testStep = new CTestGprsContextConfigNotifications;
		}
	else if(aStepName == _L("TestOpenGprsMaxContexts"))
		{
		testStep = new CTestOpenGprsMaxContexts;
		}
	else if(aStepName == _L("TestGprsCancel"))
		{
		testStep = new CTestGprsCancel;
		}
	else if(aStepName == _L("TestGprsfaxCancel"))
		{
		testStep = new CTestGprsfaxCancel;
		}
	else if(aStepName == _L("TestOpenContextByName"))
		{
		testStep = new CTestOpenContextByName;
		}
	else if(aStepName == _L("TestNetworkRegStatus"))
		{
		testStep = new CTestNetworkRegStatus;
		}
	else if(aStepName == _L("TestGetStaticCaps"))
		{
		testStep = new CTestGetStaticCaps;
		}
	else if(aStepName == _L("TestOpenExistingContext"))
		{
		testStep = new CTestOpenExistingContext; 
		}
	else if(aStepName == _L("TestGprsDownloadWebbPage"))
		{
		testStep = new CTestGprsDownloadWebPage;
		}
	else if(aStepName == _L("Test2GprsConnection"))
		{
		testStep = new CTest2GprsConnection;
		}

	return testStep;
	}
