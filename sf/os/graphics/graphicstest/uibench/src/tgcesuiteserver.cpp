// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tgcesuiteserver.h"
#include "tgraphicsresource.h"
#include "tbitbltperf_directgdi.h"
#include "tsimpledrawing_directgdi.h"
#include "tdirectgdiperf.h"
#include "teventhandlingperf.h"
#include "tflipframerate.h"
#include "trenderorientation.h"

/**
Same code for Secure and non-secure variants
Called inside the MainL() function to create and start the
CTestServer derived server.

@return - Instance of the test server
*/
CGceSuiteServer* CGceSuiteServer::NewL()
	{
	CGceSuiteServer * server = new (ELeave) CGceSuiteServer();
	CleanupStack::PushL(server);

	// Get server name from process so we can use SetCap to change the capabilites and use this server with a different filename
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());

	server->ConstructL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}


/**
MainL - uses the new Rendezvous() call to sync with the client
*/
LOCAL_C void MainL()
	{
	// Active scheduler only for this thread. Test need to create own active scheduler	
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	CGceSuiteServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CGceSuiteServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}
	delete server;
	delete scheduler;
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
CTestStep* CGceSuiteServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTGraphicsResource)
		testStep = new CTGraphicsResource();
	else if(aStepName == KTBitBltPerfDirectGdi)
		testStep = new CTBitBltPerfDirectGdi();
	else if(aStepName == KTDirectGdiPerfTest)
		testStep = new CTDirectGdiPerf();
	else if(aStepName == KTSimpleDrawingPerfDirectGdi)
		testStep = new CTSimpleDrawingDirectGdi();
	else if(aStepName == KEventHandlingPerfName)
		testStep = new CTEventHandlingPerf();
	else if(aStepName == KTFlipFramerate)
	    testStep = new CTFlipFramerate();
	else if(aStepName == KTRenderOrientation)
	    testStep = new CTRenderOrientation;
	
	return testStep;
	}
