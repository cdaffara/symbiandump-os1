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
 @file Te_locsrvSuiteServer.cpp
 @internalTechnology
*/

#include "te_locsrvsuiteserver.h"
#include "getfixteststep.h"
#include "openteststep.h"
#include "positionerteststep.h"
#include "serverteststep.h"

_LIT(KServerName,"te_locsrvsuite");

CTe_locsrvSuite* CTe_locsrvSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_locsrvSuite * server = new (ELeave) CTe_locsrvSuite();
	CleanupStack::PushL(server);

	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

/**
2nd phase constructor. Calls the base class method passing the name of the suite.
*/	
void CTe_locsrvSuite::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
	// Start lbsroot (so properties are defined), 
	iStarter.RestartLbs_RootOnlyL();	
	}

CTe_locsrvSuite::~CTe_locsrvSuite()
	{
	TRAP_IGNORE(iStarter.RestartLbs_NormalL());
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
	CTe_locsrvSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_locsrvSuite::NewL());
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


CTestStep* CTe_locsrvSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
              if(aStepName == KGetFixTestStep)
                            testStep = new CGetFixTestStep();
              else if(aStepName == KOpenTestStep)
                            testStep = new COpenTestStep();
              else if(aStepName == KPositionerTestStep)
                            testStep = new CPositionerTestStep();
              else if(aStepName == KServerTestStep)
                            testStep = new CServerTestStep();

	return testStep;
	}
