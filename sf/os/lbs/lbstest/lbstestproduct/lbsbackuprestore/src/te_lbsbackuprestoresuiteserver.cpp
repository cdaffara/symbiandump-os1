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
 @file Te_lbsbackuprestoreSuiteServer.cpp
 @internalTechnology
*/

#include "te_lbsbackuprestoresuiteserver.h"
#include "te_lbsbackuprestoresuitedefs.h"
#include "listpsyentriesstep.h"
#include "checkpsyexiststep.h"
#include "corruptpsystep.h"
#include "brandpsystep.h"

_LIT(KServerName,"te_lbsbackuprestoresuite");
CTe_lbsbackuprestoreSuite* CTe_lbsbackuprestoreSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_lbsbackuprestoreSuite * server = new (ELeave) CTe_lbsbackuprestoreSuite();
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
	CTe_lbsbackuprestoreSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_lbsbackuprestoreSuite::NewL());
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
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_lbsbackuprestoreSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	TRAPD(err, testStep = CreateTestStepL(aStepName););
	
	if(KErrNone != err)
		{
		ERR_PRINTF2(KImpossibleCreateTestStep, err);
		}
	
	return testStep;
	}

CTestStep* CTe_lbsbackuprestoreSuite::CreateTestStepL(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KListPsyEntriesStep)
	    testStep = CListPsyEntriesStep::NewL();
	else if(aStepName == KCheckPSYExistStep)
	    testStep = CCheckPSYExistStep::NewL();
	else if(aStepName == KBrandPSYStep)
	    testStep = CBrandPSYStep::NewL();
	else if(aStepName == KCorruptPSYStep)
	    testStep = CCorruptPSYStep::NewL();

	return testStep;
	}

