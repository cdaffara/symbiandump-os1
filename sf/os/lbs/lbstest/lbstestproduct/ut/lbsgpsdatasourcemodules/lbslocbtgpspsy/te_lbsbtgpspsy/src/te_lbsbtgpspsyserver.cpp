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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
//



/**
 @file Te_LbsBtGpsPsySuiteServer.cpp
 @internalTechnology
*/

#include "Te_LbsBtGpsPsyServer.h"
#include "TDBT1Step.h"
#include "TDBT2Step.h"
#include "TDBT3Step.h"
#include "TDBT4Step.h"
#include "TDBT5Step.h"
#include "TDBT6Step.h"
#include "sethwaddrstep.h"
#include "tdbt_connectionstep.h"
#include "tdbt_lastdevicestep.h"
#include "removehwaddrstep.h"
#include "deletelastconnectedstep.h"
#include "setsecondaryhwaddrstep.h" 
#include "banhwaddress.h"
#include "unbanhwaddress.h"
#include "te_systemstarter.h"

_LIT(KServerName,"te_lbsbtgpspsysuite");
CTe_LbsBtGpsPsy* CTe_LbsBtGpsPsy::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsBtGpsPsy * server = new (ELeave) CTe_LbsBtGpsPsy();
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
	CTe_LbsBtGpsPsy* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsBtGpsPsy::NewL());
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


CTestStep* CTe_LbsBtGpsPsy::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KTDBT1Step)
	                testStep = new CTDBT1Step();
	else if(aStepName == KTDBT2Step)
	                testStep = new CTDBT2Step();
	else if(aStepName == KTDBT3Step)
	                testStep = new CTDBT3Step();
	else if(aStepName == KTDBT4Step)
	                testStep = new CTDBT4Step();
	else if(aStepName == KTDBT5Step)
	                testStep = new CTDBT5Step();
	else if(aStepName == KTDBT6Step)
		  			testStep = new CTDBT6Step();
	else if(aStepName == KSetHwAddrStep)
		  			testStep = new CSetHwAddrStep();
	else if(aStepName == KTDBT_ConnectStep)
		  			testStep = new CTDBT_ConnectStep();
	else if(aStepName == KTDBT_LastDeviceStep)
		  			testStep = new CTDBT_LastDeviceStep();
	else if(aStepName == KRemoveHwAddrStep)
		  			testStep = new CRemoveHwAddrStep();
	else if(aStepName == KDeleteLastConnectedDevice)
		  			testStep = new CDeleteLastConnectedDevice();
	else if(aStepName == KSetSecondaryHwAddrStep)
					testStep = new CSetSecondaryHwAddrStep();
	else if(aStepName == KBanHwAddrStep)
					testStep = new CBanHwAddrStep();
	else if(aStepName == KUnBanHwAddrStep)
					testStep = new CUnBanHwAddrStep();
	return testStep;
	}


void CTe_LbsBtGpsPsy::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
	// Start lbsroot (so properties are defined), 
	CTe_SystemStarter starter;
	starter.RestartLbs_RootOnlyL();
	}
