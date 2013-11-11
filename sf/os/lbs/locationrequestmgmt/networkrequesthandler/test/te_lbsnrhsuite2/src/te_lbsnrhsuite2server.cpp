// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_lbsnrhsuite2server.cpp
*/

#include "lbsnrhngmsgs.h"
#include "te_lbsnrhsuite2server.h"
#include "lbsnrhstartnrhstep.h"
#include "lbsnrhstopnrhstep.h"
#include "lbsnrhprivctrlstep.h"
#include "lbsnrhstatetestbasestep.h"
#include "lbsnrhstatetestsinglestep.h"
#include "lbsnrhstatetestmultiplestep.h"


_LIT(KServerName,"Te_lbsnrhsuite2");
CTe_LbsNrhSuite* CTe_LbsNrhSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsNrhSuite * server = new (ELeave) CTe_LbsNrhSuite();
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
	CTe_LbsNrhSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsNrhSuite::NewL());
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


CTestStep* CTe_LbsNrhSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == KLbsNrhStartNrhStep)
		testStep = new CLbsNrhStartNrhStep();
	else if(aStepName == KLbsNrhStopNrhStep)
		testStep = new CLbsNrhStopNrhStep();
	else if (aStepName == KLbsNrhPrivCtrlStep)
		testStep = new CLbsNrhPrivCtrlStep();
	else if (aStepName == KLbsNrhStateTestSingleStep)
		testStep = new CLbsNrhStateTestSingleStep();
	else if (aStepName == KLbsNrhStateTestMultipleStep)
		testStep = new CLbsNrhStateTestMultipleStep();

	return testStep;
	}
