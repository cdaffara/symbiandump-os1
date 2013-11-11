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
// This is the test server for all Lbs SUPL Push API unit tests.
// 
//

#include <lbs/lbsadmin.h>

#include "lbssystemcontroller.h"

#include "Te_LbsSuplPushApiServer.h"
#include "Te_LbsSuplErrorPattern.h"
#include "Te_LbsSuplPushApiPropsStep.h"
#include "Te_LbsSuplPushApiPropsNoMemoryStep.h"
#include "Te_LbsSuplPushApiPropsErrorStep.h"
#include "Te_LbsSuplPushApiReceiverStep.h"
#include "Te_LbsSuplPushApiRecChannelStep.h"
#include "Te_LbsSuplPushIntegStep.h"
#include "Te_LbsSuplPushStateStep.h"

/** The name of the server */
_LIT(KServerName,"Te_LbsSuplPushApiSuite");

/** The UID of the SUPL Push API unit test suite dll*/
const TUid  KLbsSuplPushSuiteUid = {0x10283754};


/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CTe_LbsSuplPushApiSuite* CTe_LbsSuplPushApiSuite::NewL()
	{
	CTe_LbsSuplPushApiSuite* server = new (ELeave) CTe_LbsSuplPushApiSuite();
	CleanupStack::PushL(server);

	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}
/**
Destructor.
*/
CTe_LbsSuplPushApiSuite::~CTe_LbsSuplPushApiSuite()
	{
	//Intentionally left blank
	}

/**
The 2nd phase constructor.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushApiSuite::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	User::LeaveIfError(adminApi->Set(KLbsSettingLogger, CLbsAdmin::ELoggerOff));
	CleanupStack::PopAndDestroy();
	
	//We start LBS system here to define the P&S properties  
	RLbsSystemController lbsSysController;
	lbsSysController.OpenL(KLbsSuplPushSuiteUid);
	CleanupClosePushL(lbsSysController);
	
	//Start the LBS sub-system if it is not running
	if(lbsSysController.RequestSystemStartup()!= ELbsStartupSuccess)
		{
		User::Leave(KErrNotReady);
		}
	//We do not need Lbs to be running - not to interfere with our tests
	User::LeaveIfError(lbsSysController.RequestCompleteCloseDown());
	
	CleanupStack::PopAndDestroy(&lbsSysController);
	
	}


/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CTe_LbsSuplPushApiSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsSuplPushApiSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	
	delete server;
	delete sched;
	}


/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
GLDEF_C TInt E32Main()
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

/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTe_LbsSuplPushApiSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* step = NULL;
	TRAPD(err, step = CreateTestStepL(aStepName));
	if(err==KErrNone)
		{
		return step;
		}	
	return NULL;
	}
/**
Creates the test step by its name.

@param aStepName [In] The name of the test step.

@return The test step created.

@leave If a error happens, it leaves with one of the system error codes.
*/
CTestStep* CTe_LbsSuplPushApiSuite::CreateTestStepL(const TDesC& aStepName)
	{
	if(aStepName==KLbsSuplPushApiPropsStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushApiPropsStep();
    	}
    else if(aStepName==KLbsSuplPushApiPropsNoMemoryStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushApiPropsNoMemoryStep();
    	}
    else if(aStepName==KLbsSuplPushApiPropsErrorStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushApiPropsErrorStep();
    	}
    else if(aStepName==KLbsSuplPushApiReceiverStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushApiReceiverStep();
    	}
    else if(aStepName==KLbsSuplPushApiRecChannelStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushApiRecChannelStep();
    	}
    else if(aStepName==KLbsSuplPushIntegStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushIntegStep();
    	}
    else if(aStepName==KLbsSuplPushStateStep)
    	{
    	return new(ELeave) CTe_LbsSuplPushStateStep();
    	}
	return NULL;
	}
