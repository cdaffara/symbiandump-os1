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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tgraphicsresourceinternalserver.h"
#include "tgraphicsresourceinternal.h"

/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
CTGraphicsResourceInternalServer* CTGraphicsResourceInternalServer::NewL()
	{
	CTGraphicsResourceInternalServer * server = new (ELeave) CTGraphicsResourceInternalServer();
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

  	CTGraphicsResourceInternalServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTGraphicsResourceInternalServer::NewL());
	if(err == KErrNone)
		{
		// Sync with the client
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
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
@return - A CTestStep derived instance
 */
CTestStep* CTGraphicsResourceInternalServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTGraphicsResourceInternalStep)
		{
		testStep = new CTGraphicsResourceInternal();
		}

	return testStep;
	}
