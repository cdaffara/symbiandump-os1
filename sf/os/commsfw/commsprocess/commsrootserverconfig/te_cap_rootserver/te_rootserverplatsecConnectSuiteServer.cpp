// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file 
*/

#include "te_rootserverplatsecConnectSuiteServer.h"
#include "te_rootserverplatsecConnectStep.h"

_LIT(KServerName,"te_cap_RootServer_Connect_sc");


/**
 @return - Instance of the test server
 Same code for Secure and non-secure variants
 Called inside the MainL() function to create and start the
 CTestServer derived server.
*/
Cte_rootserverplatsecConnectSuite* Cte_rootserverplatsecConnectSuite::NewL()
	{
	Cte_rootserverplatsecConnectSuite* server = new (ELeave) Cte_rootserverplatsecConnectSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

/**
 Secure variant
 Secure variants much simpler For EKA2, just an E32Main and a MainL()
 Much simpler, uses the new Rendezvous() call to sync with the client
*/
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	Cte_rootserverplatsecConnectSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = Cte_rootserverplatsecConnectSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}




/**
 @return - Standard Epoc error code on process exit
 Secure variant only
 Process entry point. Called by client using RProcess API
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


/**
 @return - A CTestStep derived instance
 Secure and non-secure variants
 Implementation of CTestServer pure virtual
*/
CTestStep* Cte_rootserverplatsecConnectSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
    
    if(aStepName == KRootServerPlatSecConnectStep)
    	{
		testStep = new CRootServerPlatSecConnectStep();
    	}
    	
	return testStep;
	}

