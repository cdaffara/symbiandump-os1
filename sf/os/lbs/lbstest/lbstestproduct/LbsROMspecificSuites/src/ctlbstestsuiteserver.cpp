/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/


/**
 @file 
 @internalTechnology
 @test
*/


// System includes
#include <e32std.h>
#include <rsshared.h>

//#include "serverteststep.h"
#include "ctlbstestsuiteserver.h"
#include "ctpositionerteststep.h"


_LIT(KServerName,"lbsromsuitetestserver");

CTe_TestSuite* CTe_TestSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_TestSuite * server = new (ELeave) CTe_TestSuite();
	CleanupStack::PushL(server);

	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}


CTe_TestSuite::CTe_TestSuite()
	{
	
	}


/**
2nd phase constructor. Calls the base class method passing the name of the suite.
*/	
void CTe_TestSuite::ConstructL()
	{
	_LIT(KTestName, "CTe_TestSuite::ConstructL()");
	INFO_PRINTF1(KTestName);

	//ConstructL of the base class
	CT_LbsServer::ConstructL(KServerName);
	
	}

CTe_TestSuite::~CTe_TestSuite()
	{

	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */

	{
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	__UHEAP_MARK;
	// this registers the server with the active scheduler and calls SetActive
	CTe_TestSuite* server = CTe_TestSuite::NewL();

	// signal to the client that we are ready by
	// rendevousing process
	RProcess::Rendezvous(KErrNone);

	// run the active scheduler
	sched->Start();

	// clean up
	delete server;
	__UHEAP_MARKEND;
	CleanupStack::PopAndDestroy(sched);
	
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


CTestStep* CTe_TestSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	_LIT(KTestName, "CTe_TestSuite::CreateTestStep()");
	INFO_PRINTF1(KTestName);

	CTestStep* testStep = NULL;
	if(aStepName == KPositionerTestStep)
		{
		testStep = new CPositionerTestStep();
		return testStep;
		}

	return  CT_LbsServer::CreateTestStep(aStepName);
	}
