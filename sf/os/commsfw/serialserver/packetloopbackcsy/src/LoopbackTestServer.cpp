// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TelephonyTestServer.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to
// be started in the process of the client. The client initialises the server
// by calling the one and only ordinal.
// 
//

/**
 @file
*/

#include "LoopbackTestServer.h"

#include "LoopbackTestStep.h"

_LIT(KServerName,"TE_PacketLoopback");

CLoopbackTestServer* CLoopbackTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// Instantiate server class here
	CLoopbackTestServer * server = new (ELeave) CLoopbackTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CLoopbackTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CLoopbackTestServer::NewL());
	if(!err)
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
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	return err;
	}

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CLoopbackTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	TRAPD
		(err,

		if(aStepName == KLoopbackTestStep1)
			testStep = new(ELeave) CLoopbackTestStep1();
		if(aStepName == KLoopbackTestStep2)
			testStep = new(ELeave) CLoopbackTestStep2();
		if(aStepName == KLoopbackTestStep3)
			testStep = new(ELeave) CLoopbackTestStep3();
		if(aStepName == KLoopbackTestStep4)
			testStep = new(ELeave) CLoopbackTestStep4();
		if(aStepName == KLoopbackTestStep5)
			testStep = new(ELeave) CLoopbackTestStep5();
		if(aStepName == KLoopbackTestStep6)
			testStep = new(ELeave) CLoopbackTestStep6();
		if(aStepName == KLoopbackTestStep7)
			testStep = new(ELeave) CLoopbackTestStep7();
		if(aStepName == KLoopbackTestStep8)
			testStep = new(ELeave) CLoopbackTestStep8();
		if(aStepName == KLoopbackTestStep9)
			testStep = new(ELeave) CLoopbackTestStep9();
		if(aStepName == KLoopbackTestStep10)
			testStep = new(ELeave) CLoopbackTestStep10();
		if(aStepName == KLoopbackTestStep11)
			testStep = new(ELeave) CLoopbackTestStep11();
		if(aStepName == KLoopbackTestStep12)
			testStep = new(ELeave) CLoopbackTestStep12();
		if(aStepName == KLoopbackTestStep13)
			testStep = new(ELeave) CLoopbackTestStep13();
		
		);
	err = err; 		// fix compiler warning about unused variable
	return testStep;
	}


