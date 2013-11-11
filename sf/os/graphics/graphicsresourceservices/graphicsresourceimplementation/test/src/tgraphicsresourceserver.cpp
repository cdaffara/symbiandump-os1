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
 @file
 @test
 @internalComponent - Graphics Resource API Conformance Test Suite
*/

#include "tgraphicsresourceserver.h"
#include "tsgdrawablegeneric.h"
#include "tsgdriver.h"
#include "tsgimagegeneric.h"
#include "tsggenericmanual.h"

/**
   @return - Instance of the test server
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
CTSgServer* CTSgServer::NewL()
	{
	CTSgServer * server = new (ELeave) CTSgServer();
	CleanupStack::PushL(server);
	
	// Get server name from process so we can use SetCap to change the capabilites and use this server with a different filename
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	
	// CServer base class call
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
  	CActiveScheduler::Install(sched);

	CTSgServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTSgServer::NewL());
	if(err == KErrNone)
		{
		// Sync with the client
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;

	}

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
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
	if (err != KErrNone)
	    {
	    RProcess handle = RProcess();
		TParsePtrC serverName(handle.FileName());
	   	User::Panic(serverName.Name(),err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

/**
Secure and non-secure variants
Implementation of CTestServer pure virtual
@return - A CTestStep derived instance
 */
CTestStep* CTSgServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTSgDrawableGeneric)
		{
		testStep = new CTSgDrawableGeneric(ETrue);
		}
	else if(aStepName == KTSgDriver)
		{
		testStep = new CTSgDriver(ETrue);
		}
	else if(aStepName == KTSgImageGeneric)
		{
		testStep = new CTSgImageGeneric(ETrue);
		}
	else if(aStepName == KTSgGenericManual)
		{
		testStep = new CTSgGenericManual(ETrue);
		}
	return testStep;
	}
