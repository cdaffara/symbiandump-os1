// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//  Test server
//

/**
 @file
 @internalComponent
*/


#include <commdbconnpref.h>
#include "te_mbufgobblerserver.h"
#include "te_mbufgobblertest.h"

_LIT(KServerName,"te_mbufgobbler");

CMbufGobblerTestServer* CMbufGobblerTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CMbufGobblerTestServer * server = new (ELeave) CMbufGobblerTestServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	//server->StartL(KServerName); 
	server->ConstructL(KServerName);	// from CTestBase
	
	CleanupStack::Pop(server);
	return server;
	}


/*virtual*/ CMbufGobblerTestServer::~CMbufGobblerTestServer()
	{
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
	CMbufGobblerTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CMbufGobblerTestServer::NewL());
	if (!err)
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
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CMbufGobblerTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if (aStepName == KTestMbufGobblerStartupTest)
		testStep = new CMbufGobblerStartupTest(this);
//	else if (aStepName ==   (other step name)
//		testStep = new  (other test step class)

	return testStep;
	}
