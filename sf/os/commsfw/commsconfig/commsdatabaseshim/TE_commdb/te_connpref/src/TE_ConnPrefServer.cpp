//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

// TE_ConnPrefServer.cpp
//
//
// This main DLL entry point for the TE_ConServer.dll


#include "TE_ConnPrefStep.h"
#include "TE_ConnPrefHelperSteps.h"
#include "TE_ConnPrefSteps.h"


EXPORT_C CTE_ConnPrefServer* NewConnPrefServer( void ) 
//
// NewTestSuiteConnPrefL is exported at ordinal 1
// this provides the interface to allow schedule test
// to create instances of this test suite
//
	{

	return new(ELeave) CTE_ConnPrefServer();
    }

	
	
// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"te_connpref");
// __EDIT_ME__ - Use your own server class name
CTE_ConnPrefServer* CTE_ConnPrefServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
	CTE_ConnPrefServer * server = new (ELeave) CTE_ConnPrefServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CTE_ConnPrefServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTE_ConnPrefServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

// __EDIT_ME__ - Use your own server class name
CTestStep* CTE_ConnPrefServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	
	
	if(aStepName.Compare(_L("ForceCCoverWrite")) == 0 )
		testStep = new(ELeave) CTE_ConnPrefForceCCoverWrite(_L("ForceCCoverWrite"));
	else if(aStepName.Compare(_L("ConstructAndVersion")) == 0 )
		testStep = new(ELeave) CTE_ConstructAndVersion();
	else if(aStepName.Compare(_L("SetAndGetConnPrefs")) == 0 )
		testStep = new(ELeave) CTE_SetAndGetConnPrefs();
	else if(aStepName.Compare(_L("AssignAndCopyConstruct")) == 0 )
		testStep = new(ELeave) CTE_AssignAndCopyConstruct();
	else if(aStepName.Compare(_L("ComparePrefs")) == 0 )
		testStep = new(ELeave) CTE_ComparePrefs();
	else if(aStepName.Compare(_L("ExtensionIdAndCast")) == 0 )
		testStep = new(ELeave) CTE_ExtensionIdAndCast();
	else if(aStepName.Compare(_L("MultiConnPref")) == 0 )
		testStep = new(ELeave) CTE_MultiConnPref();
	else if(aStepName.Compare(_L("MultiConnPrefVersion")) == 0 )
		testStep = new(ELeave) CTE_MultiConnPrefVersion();
	else if(aStepName.Compare(_L("MultiConnectionAttempts")) == 0 )
		testStep = new(ELeave) CTE_MultiConnectionAttempts();

	
	return testStep;
	}



// make a version string available for test system 
TPtrC CTE_ConnPrefServer::GetVersion( void )
	{

#ifdef _DEBUG
_LIT(KTxtVersion,"1.0 (udeb)");
#else
_LIT(KTxtVersion,"1.0");
#endif

	return KTxtVersion();
	}


