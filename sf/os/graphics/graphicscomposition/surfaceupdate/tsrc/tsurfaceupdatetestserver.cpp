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

#include "tsurfaceupdatetestserver.h"
#include "tsurfaceupdate.h"
 
_LIT(KServerName,"TSurfaceUpdateTestServer");

CTSurfaceUpdateServer* CTSurfaceUpdateServer::NewL()
	{
	CTSurfaceUpdateServer * server = new (ELeave) CTSurfaceUpdateServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTSurfaceUpdateServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTSurfaceUpdateServer::NewL());
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
		RDebug::Print(_L("TSurfaceUpdateServer::MainL - Error: %d"), err);
	   	User::Panic(KServerName,err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTSurfaceUpdateServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTSurfaceUpdateStep)
		{
		testStep = new CTSurfaceUpdateStep();
		}
		
	return testStep;
	}
