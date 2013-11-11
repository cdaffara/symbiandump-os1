// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "trasterizer.h"
#include "trasterizerinvalid.h"
#include "trasterizerserver.h"
#include <e32cmn.h> 
#include <fbs.h>
#include <graphics/fbsdefs.h>


_LIT(KServerName,"trastserver");


CTRasterizerServer* CTRasterizerServer::NewL()
	{
	CTRasterizerServer* server = new (ELeave) CTRasterizerServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{ 	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTRasterizerServer* server = NULL;
	
	// Create the CTestServer derived server
	server = CTRasterizerServer::NewL();
	// startup fbs server
	FbsStartup();
	// Sync with the client and enter the active scheduler
	RProcess::Rendezvous(KErrNone);
	sched->Start();

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
		RDebug::Print(_L("TRasterizerServer::MainL - Error: %d"), err);
	   	User::Panic(KServerName,err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


CTestStep* CTRasterizerServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if (aStepName == KTRasterizerStep)
		{
		testStep = new CTRasterizer();
		}
	else if (aStepName == KTRasterizerInvalidStep)
		{
		testStep = new CTRasterizerInvalid();
		}		
	
	return testStep;
	}

