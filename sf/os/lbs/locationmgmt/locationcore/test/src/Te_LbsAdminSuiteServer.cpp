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

/**
 @file Te_LbsAdminSuiteServer.cpp
*/
#include "te_lbsadminsuitedefs.h"
#include "te_lbsadminsuiteserver.h"
#include "lbsadminmodifierstep.h"
#include "lbsadminobserverstep.h"
#include "lbsadminerrorcheckstep.h"
#include "backwardcompatibilityteststep.h"
#include "outofrangestep.h"
#include "unsupportedsetcalstep.h"
#include "multipleprotocolmodules.h"

_LIT(KServerName,"Te_LbsAdminSuite");
CTe_LbsAdminSuite* CTe_LbsAdminSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsAdminSuite * server = new (ELeave) CTe_LbsAdminSuite();
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
	CTe_LbsAdminSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsAdminSuite::NewL());
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


CTestStep* CTe_LbsAdminSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	TRAPD(err,
	if(aStepName == KLbsAdminModifierStep)
		testStep = new(ELeave) CLbsAdminModifierStep();
	else if(aStepName == KLbsAdminObserverStep)
		testStep = new(ELeave) CLbsAdminObserverStep();
	else if(aStepName == KLbsAdminErrorCheckStep)
		testStep = new(ELeave) CLbsAdminErrorCheckStep();
	else if(aStepName == KBackwardCompatibilityTestStep)
		testStep = new(ELeave) CBackwardCompatibilityTestStep();
	else if(aStepName == KOutOfRangeStep)
		testStep = new(ELeave) COutOfRangeStep();
	else if(aStepName == KUnsupportedSetCalStep)
		testStep = new(ELeave) CUnsupportedSetCalStep();
	else if(aStepName == KMultipleProtocolModules)
		testStep = new(ELeave) CMultipleProtocolModules();
	);
	if(KErrNone != err)
		{
		INFO_PRINTF2(KErrorCreateTELBSAdminTestStep, err);
		}
	
	return testStep;
	}
