// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_EtelSatTestServer.h"

#include "TE_EtelSatfns.h"
#include "TE_EtelSatenvelopecmds.h"
#include "TE_EtelSatcontrol.h"
#include "TE_EtelSatpcmds1a.h"
#include "TE_EtelSatpcmds1b.h"
#include "TE_EtelSatpcmds1c.h"
#include "TE_EtelSatpcmds1d.h"
#include "TE_EtelSatpcmds1e.h"
#include "TE_EtelSatpcmds1f.h"
#include "te_etelsatpcmds1g.h"

_LIT(KServerName,"Te_EtelSat");

CEtelSatTestServer* CEtelSatTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CEtelSatTestServer * testServer = new (ELeave) CEtelSatTestServer();
	CleanupStack::PushL(testServer);

	testServer->ConstructL(KServerName);
	CleanupStack::Pop(testServer);
	return testServer;
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
   	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CEtelSatTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CEtelSatTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
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

CTestStep* CEtelSatTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    
    if(aStepName == _L("TestSatFns"))
    	{
    	testStep = new CTestSatFns;
		}
    else if(aStepName == _L("TestSatEnvelopeCmds"))
    	{
    	testStep = new CTestSatEnvelopeCmds;
		}
    else if(aStepName == _L("TestSatControl"))
    	{
    	testStep = new CTestSatControl;
		}
    else if(aStepName == _L("TestSatPCmds1a"))
    	{
    	testStep = new CTestSatPCmds1a;
		}
    else if(aStepName == _L("TestSatPCmds1b"))
    	{
    	testStep = new CTestSatPCmds1b;
		}
    else if(aStepName == _L("TestSatPCmds1c"))
		{
    	testStep = new CTestSatPCmds1c;
		}
    else if(aStepName == _L("TestSatPCmds1d"))
		{
    	testStep = new CTestSatPCmds1d;
		}
    else if(aStepName == _L("TestSatPCmds1e"))
		{
    	testStep = new CTestSatPCmds1e;
		}
    else if(aStepName == _L("TestSatPCmds1f"))
		{
    	testStep = new CTestSatPCmds1f;
		}
    else if(aStepName == _L("TestSatPCmds1g"))
    		{
        	testStep = new CTestSatPCmds1g;
    		}
	return testStep;
	} // CEtelSatTestServer::CreateTestStep
