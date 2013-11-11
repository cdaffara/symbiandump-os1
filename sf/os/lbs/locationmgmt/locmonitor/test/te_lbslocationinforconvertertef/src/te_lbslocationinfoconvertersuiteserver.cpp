/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "te_lbslocationinfoconvertersuiteserver.h"
#include "step.h"

#include "te_lbslocationinfoconvertersuitedefs.h"
_LIT(KServerName,"te_lbslocationinfoconvertersuite");
CTe_LbsLocationMonitorSuite* CTe_LbsLocationMonitorSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsLocationMonitorSuite * server = new (ELeave) CTe_LbsLocationMonitorSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CTe_LbsLocationMonitorSuite::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);

	}

CTe_LbsLocationMonitorSuite::~CTe_LbsLocationMonitorSuite()
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
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_LbsLocationMonitorSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsLocationMonitorSuite::NewL());
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


CTestStep* CTe_LbsLocationMonitorSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	TRAPD(err,
	if(aStepName == KStep)
		testStep = new(ELeave) CStep();
	);
	if(KErrNone != err)
		{
		INFO_PRINTF2(KErrorCreateTELBSLocationMonitorTestStep, err);
		}
	
	return testStep;
	}
