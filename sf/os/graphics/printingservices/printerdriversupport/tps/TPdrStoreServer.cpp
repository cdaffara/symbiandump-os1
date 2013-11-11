// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TPdrStoreServer.h"
#include "T_PDRLST.H"
#include "T_PDRMEM.H"
#include "T_PDR.H"


_LIT(KServerName,"TPdrStoreServer");

CTPdrStoreServer* CTPdrStoreServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTPdrStoreServer * server = new (ELeave) CTPdrStoreServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
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
	CTPdrStoreServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTPdrStoreServer::NewL());
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

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
*/
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
		RDebug::Print(_L("CTPdrStoreServer::MainL - Error: %d"), err);
	   	User::Panic(KServerName,err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }



CTestStep* CTPdrStoreServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTPdrLstStep)
		{
		testStep = new CTPdrLstStep();
		}
	
	else if(aStepName == KTPdrMemStep)
		{
		testStep = new CTPdrMemStep();
		}
		
	else if(aStepName == KTPdrStep)
		{
		testStep = new CTPdrStep();
		}

	return testStep;
	}
