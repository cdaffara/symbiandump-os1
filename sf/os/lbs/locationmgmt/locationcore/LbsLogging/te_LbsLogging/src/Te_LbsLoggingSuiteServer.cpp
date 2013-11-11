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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_LbsLoggingSuiteServer.cpp
*/

#include "Te_LbsLoggingSuiteServer.h"

//test step files
#include "testSelfLocateLogStep.h"
#include "testNetworkLocateLogStep.h"
#include "testExternalLocateLogStep.h"
#include "testX3PLocateLogStep.h"
#include "testAssistanceDataLogStep.h"
#include "testLoggingPerformanceStep.h"

_LIT(KServerName,"Te_LbsLoggingSuiteServer");
CTe_LbsLoggingSuiteServer* CTe_LbsLoggingSuiteServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsLoggingSuiteServer * server = new (ELeave) CTe_LbsLoggingSuiteServer();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_LbsLoggingSuiteServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsLoggingSuiteServer::NewL());
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


CTestStep* CTe_LbsLoggingSuiteServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	//logging test steps
	    	if(aStepName == KTestAssistanceDataLogStep)
	       	testStep = new CTestAssistanceDataLogStep();
	        else if(aStepName == KTestExternalLocateLogStep)
	       	testStep = new CTestExternalLocateLogStep();
			else if(aStepName == KTestSelfLocateLogStep)
	       	testStep = new CTestSelfLocateLogStep();
	        else if(aStepName == KTestLoggingPerformanceStep)
	       	testStep = new CTestLoggingPerformanceStep();
		    else if(aStepName == KTestNetworkLocateLogStep)
	       	testStep = new CTestNetworkLocateLogStep();
	       	else if(aStepName == KTestX3PLocateLogStep)
	       	testStep = new CTestX3PLocateLogStep();
       
	return testStep;
	}
