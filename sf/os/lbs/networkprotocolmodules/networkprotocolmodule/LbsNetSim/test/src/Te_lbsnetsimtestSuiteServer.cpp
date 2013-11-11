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
 @file Te_lbsnetsimtestSuiteServer.cpp
*/

#include "Te_lbsnetsimtestSuiteServer.h"
#include "LbsConnectDisconnectNotificationTest.h"
#include "lbsmulticonnectdisconnectnotificationtest.h"
#include "lbsregisterlcsmolrtest.h"
#include "lbsregisterlcsmolrwitherrortest.h"
#include "lbsregisterlcsmolrtestnotestapi.h"
#include "lbsx3ptest.h"
#include "lbscancelmolrtest.h"
#include "lbsregisterlcsmolrtestmoreassistancedata.h"
#include "lbsnetsimsettertests.h"
#include "lbsmtlrtest.h"
#include "lbsx3pattemptmtlrtest.h"
#include "lbsmolrcancelformtlr.h"
#include "lbsx3ptestinvaliddestination.h"


_LIT(KServerName,"Te_lbsnetsimtestSuite");
CTe_lbsnetsimtestSuite* CTe_lbsnetsimtestSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_lbsnetsimtestSuite * server = new (ELeave) CTe_lbsnetsimtestSuite();
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
	CTe_lbsnetsimtestSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_lbsnetsimtestSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess().SetPriority(EPriorityLow);
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


CTestStep* CTe_lbsnetsimtestSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	//
	if(aStepName == KConnectDisconnectNotificationTest)
		testStep = new CLbsConnectDisconnectNotificationTest();
	//
	else if(aStepName == KMultiConnectDisconnectNotificationTest)
		testStep = new CLbsMultiConnectDisconnectNotificationTest();
	//
	else if(aStepName == KRegisterLcsMoLrTest)
		testStep = new CLbsRegisterLcsMoLrTest();
	//	
	else if (aStepName == KX3PTest)
		testStep = new CLbsX3PTest();
	//
	else if(aStepName == KCancelMoLrTest)
		testStep = new CLbsCancelMoLrTest();
	//
	else if(aStepName == KRegisterLcsMoLrTestMoreAssistanceData)
		testStep = new CLbsRegisterLcsMoLrTestMoreAssistanceData();
	//
	else if (aStepName == KRegisterLcsMoLrNoTestApiTest)
		testStep = new CLbsRegisterLcsMoLrTestNoTestAPI();
	//
	else if (aStepName == KSetterTests)
		testStep = new CLbsSetterTests();
	//
	else if (aStepName == KRegisterLcsMoLrWithErrorTest)
		testStep = new CLbsRegisterLcsMoLrWithErrorTest();
	//
	else if (aStepName == KMtLrTest)
		testStep = new CLbsMtLrTest();
	//
	else if (aStepName == KX3PTestAttemptMtLrTest)
		testStep = new CLbsX3PAttemptMrLrTest();
	//
	else if (aStepName == KMoLrCancelForMtLr)
		testStep = new CLbsMolrCancelForMtLr();
	//
	else if (aStepName == KX3PTestInvalidDestination)
		testStep = new CLbsX3PTestInvalidDestination();
	
	return testStep;
	}
