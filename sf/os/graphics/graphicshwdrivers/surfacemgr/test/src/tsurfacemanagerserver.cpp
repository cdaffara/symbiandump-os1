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
 @internalComponent Internal Symbian test code
*/

#include "tsurfacemanagerserver.h"
#include "tsurfacemanager.h"
#include "tsurfacemanagermultithread.h" 
#include "tsurfacemanagermultiprocess.h"

CTSurfaceManagerServer* CTSurfaceManagerServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTSurfaceManagerServer * server = new (ELeave) CTSurfaceManagerServer();
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
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//
	{
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTSurfaceManagerServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTSurfaceManagerServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
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
	if (err)
	    {
		RDebug::Print(_L("CTSurfaceManagerServer::MainL - Error: %d"), err);
	   	User::Panic(_L("CTSurfaceManagerServer"),err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTSurfaceManagerServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	if(aStepName == KTSurfaceManagerStep)
		{
		return new CTSurfaceManagerStep();
		}
	else if(aStepName == KTSurfaceManagerMultiThreadStep)
		{
		return new CTSurfaceManagerMultiThreadStep();
		}
	else if(aStepName == KTSurfaceManagerMultiProcessStep)
		{
		return new CTSurfaceManagerMultiProcessStep();
		}
    return NULL;
	}

