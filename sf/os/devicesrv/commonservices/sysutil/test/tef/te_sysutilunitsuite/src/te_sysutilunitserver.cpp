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

#include "te_sysutilunitserver.h"

#include "te_sysutilsgetversionmissing.h"

#include "te_sysutilsgetversionunreadable.h"
#include "te_sysutilsgetversiontoomuchtext.h"

#include "te_sysutilsgetversionnotext.h"
#include "te_sysutilsgetversionnonewlines.h"

_LIT(KServerName,"te_sysutilunitsuite");

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
	
    if(aStepName == KSysUtilsGetSWVersionMissingStep)
		testStep = new CSysUtilsGetSWVersionMissingStep(); 
    else if(aStepName == KSysUtilsGetLangVersionMissingStep)
		testStep = new CSysUtilsGetLangVersionMissingStep();
    else if(aStepName == KSysUtilsGetLangSWVersionMissingStep)
		testStep = new CSysUtilsGetLangSWVersionMissingStep();
	else if(aStepName == KSysUtilsGetSWVersionNoNewLinesStep)
		testStep = new CSysUtilsGetSWVersionNoNewLinesStep();
	else if(aStepName == KSysUtilsGetLangSWVersionNoNewLinesStep)
		testStep = new CSysUtilsGetLangSWVersionNoNewLinesStep();
	else if(aStepName == KSysUtilsGetSWVersionUnreadableStep)
		testStep = new CSysUtilsGetSWVersionUnreadableStep();
	else if(aStepName == KSysUtilsGetLangVersionUnreadableStep)
		testStep = new CSysUtilsGetLangVersionUnreadableStep();
	else if(aStepName == KSysUtilsGetLangSWVersionUnreadableStep)
		testStep = new CSysUtilsGetLangSWVersionUnreadableStep();
	else if(aStepName == KSysUtilsGetVersionTooMuchTextStep)
		testStep = new CSysUtilsGetVersionTooMuchTextStep(); 
	else if(aStepName == KSysUtilsGetSWVersionNoTextStep)
       	testStep = new CSysUtilsGetSWVersionNoTextStep();
	else if(aStepName == KSysUtilsGetLangSWVersionNoTextStep)
       	testStep = new CSysUtilsGetLangSWVersionNoTextStep();
	else if(aStepName == KSysUtilsGetLangVersionNoTextStep)
       	testStep = new CSysUtilsGetLangVersionNoTextStep();
	else if(aStepName == KSysUtilsGetPRInformationMissingStep)
            testStep = new CSysUtilsGetPRInformationMissingStep(); 
    else if(aStepName == KSysUtilsGetPRInformationnNoNewLinesStep)
            testStep = new CSysUtilsGetPRInformationNoNewLinesStep();
    else if(aStepName == KSysUtilsGetPRInformationNoTextStep)
            testStep = new CSysUtilsGetPRInformationNoTextStep();
    else if(aStepName == KSysUtilsGetPRInformationUnreadableStep)
            testStep = new CSysUtilsGetPRInformationUnreadableStep();             

	return testStep;
	}
