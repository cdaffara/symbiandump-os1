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

#include "tamastarter_server.h"
#include "tamastarter_step_standalone.h"
#include "tamastarter_step_ssc.h"
#include "tamastarter_step_invalidsid.h"
#include "tamastarter_step_startl.h"
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include "tamastarter_step_startlasync.h"
#include "tamastarter_step_cancel.h"
#include "tamastarter_step_startpending.h"
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT

_LIT(KCAppfwkAmaStarterTestServerName,"tamastarter_server");

/**
  @return - Instance of the test server
  Called inside the MainL() function to create and start the
  CTestServer derived server.
 */
CAppfwkAmaStarterTestServer* CAppfwkAmaStarterTestServer::NewL()
	{
	CAppfwkAmaStarterTestServer * server = new (ELeave) CAppfwkAmaStarterTestServer();
	CleanupStack::PushL(server);
	TParsePtrC serverName(RProcess().FileName());	
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

void MainL()
	{	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CAppfwkAmaStarterTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CAppfwkAmaStarterTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
  @return - Standard Epoc error code on process exit
  Process entry point. Called by client using RProcess API
  */
TInt E32Main(void)
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
		_LIT(KMainErrorStatement, "CAppfwkAmaStarterTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KCAppfwkAmaStarterTestServerName,err);
		}		
	delete cleanup;
		
	__UHEAP_MARKEND;
	return KErrNone;
    }

/**
  @return - A CTestStep derived instance
  @param - aStepName the test case name to be executed
  Implementation of CTestServer pure virtual
 */
CTestStep* CAppfwkAmaStarterTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if (aStepName == KAppfwkAmaStarterStandalone)
		{
		testStep = new CAppfwkAmaStarterTestStepStandalone(aStepName);
		}
	else if (aStepName == KAppfwkAmaStarterSsc)
		{
		testStep = new CAppfwkAmaStarterTestStepSsc(aStepName);
		}
	else if (aStepName == KAppfwkAmaStarterInvalidSID)
		{
		testStep = new CAppfwkAmaStarterTestStepInvalidSid(aStepName);
		}
	else if (aStepName == KAppfwkAmaStarterStartL)
		{
		testStep = new CAppfwkAmaStarterTestStepStart(aStepName);
		}
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	else if (aStepName == KAppfwkAmaStarterStartLAsync)
		{
		testStep = new CAppfwkAmaStarterTestStepStartLAsync(aStepName);
		}
	else if (aStepName == KAppfwkAmaStarterCancel)
		{
		testStep = new CAppfwkAmaStarterTestStepCancel(aStepName);
		}
	else if (aStepName == KAppfwkAmaStarterStartPending)
		{
		testStep = new CAppfwkAmaStarterTestStepStartPending(aStepName);
		}
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT
	return testStep;
	}
