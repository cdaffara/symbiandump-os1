// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ctlbsbtgpspsysuiteserver.cpp
// Testexecute test server for the BT GPS PSY Integration Tests
//

#include "ctlbsbtgpspsyserver.h"
#include "ctsetupbtgpsdevicelist.h"
#include "ctcheckbtgpsdevicelist.h"
#include "ctdeletebtgpsdevicelist.h"
#include "ctbtgpsdevicelisthandlerbase.h"
#include "ctlbsbtgpspsylocationrequeststep.h"
#include "ctlbsbtgpspsypnokstep.h"
#include "ctlbsbtgpspsyreconnectionstep.h"
#include "ctlbsbtgpspsylistupdatesstep.h"
#include "te_systemstarter.h"


_LIT(KServerName,"ctlbsbtgpspsytestserver");
CT_LbsBtGpsPsyTestServer* CT_LbsBtGpsPsyTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CT_LbsBtGpsPsyTestServer * server = new (ELeave) CT_LbsBtGpsPsyTestServer();
	CleanupStack::PushL(server);
	server->ConstructL();
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
	CT_LbsBtGpsPsyTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CT_LbsBtGpsPsyTestServer::NewL());
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


CTestStep* CT_LbsBtGpsPsyTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	/*Common Base Test Steps used for setting up the device list*/
	if(aStepName == KSetupDeviceConfigList)
		testStep = new CT_SetupBTGPSDeviceList();
	else if(aStepName == KCheckDeviceConfigList)
		testStep = new CT_CheckBTGPSDeviceList();
	else if(aStepName == KDeleteDeviceConfigList)
			testStep = new CT_DeleteBTGPSDeviceList();
	else if(aStepName == KBTGPSPsyLocationRequestStep)
		testStep = new CT_LbsBtGpsPsyLocationRequestStep();
	else if(aStepName == KBTGPSPsyPnokStep)
		testStep = new CT_LbsBtGpsPsyPnokStep();
	else if(aStepName == KBTGPSPsyReconnectionStep)
			testStep = new CT_LbsBtGpsPsyReconnectionStep();
	else if(aStepName == KBTGPSPsyListUpdatesStep)
			testStep = new CT_LbsBtGpsPsyListUpdatesStep();

	return testStep;
	}


void CT_LbsBtGpsPsyTestServer::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
	// Start lbsroot (so properties are defined), 
	CTe_SystemStarter starter;
	starter.RestartLbs_RootOnlyL();
	}
