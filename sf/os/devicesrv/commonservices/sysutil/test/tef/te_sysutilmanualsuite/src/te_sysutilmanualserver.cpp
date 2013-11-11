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
// Test Server for the System Utils Tests
// For (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilmanualserver.h"

#include "te_syutilmmcdrivelocation.h"
#include "te_syutilfindcriticalthresholdvalues.h"

_LIT(KServerName,"te_sysutilmanualsuite");

/**
Same code for Secure and non-secure variants
Called inside the MainL() function to create and start the
CTestServer derived server.

@return - Instance of the test server
 */
CTeSysUtilsSuite* CTeSysUtilsSuite::NewL()
	{
	CTeSysUtilsSuite * server = new (ELeave) CTeSysUtilsSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}



/**
Secure variant
Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTeSysUtilsSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTeSysUtilsSuite::NewL());
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
Secure variant only
Process entry point. Called by client using RProcess API

@return - Standard Epoc error code on process exit
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
Secure and non-secure variants
Implementation of CTestServer pure virtual

@return - A CTestStep derived instance
 */
CTestStep* CTeSysUtilsSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
    if(aStepName ==KSysUtilsMMCDriveLocationStep)
       	testStep = new CSysUtilsMMCDriveLocationStep();
    else if(aStepName ==KSysUtilsFindCriticalThresholdValuesStep)
       	testStep = new CSysUtilsFindCriticalThresholdValuesStep();


	return testStep;
	}
