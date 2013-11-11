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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file CCommsdatSuiteServer.cpp
*/

#include "CCommsdatSuiteServer.h"
#include "step_001_01.h"
#include "step_002_01.h"
#include "step_003_01.h"
#include "step_004_01.h"


_LIT(KServerName,"CCommsdatSuite");
CCommsdatSuite* CCommsdatSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CCommsdatSuite * server = new (ELeave) CCommsdatSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	//server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CCommsdatSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCommsdatSuite::NewL());
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
	return KErrNone;
    }


CTestStep* CCommsdatSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
              if(aStepName == _L("step_001_01"))
                            testStep = new CCommsDatTest001_01();
              else if(aStepName == _L("step_001_02"))
                            testStep = new CCommsDatTest001_02();
              else if(aStepName == _L("step_001_03"))
                            testStep = new CCommsDatTest001_03();
              else if(aStepName == _L("step_001_04"))
                            testStep = new CCommsDatTest001_04();
              else if(aStepName == _L("step_001_05"))
                            testStep = new CCommsDatTest001_05();
              else if(aStepName == _L("step_002_01"))
                            testStep = new CCommsDatTest002_01();
              else if(aStepName == _L("step_002_02"))
                            testStep = new CCommsDatTest002_02();
              else if(aStepName == _L("step_002_03"))
                            testStep = new CCommsDatTest002_03();
              else if(aStepName == _L("step_002_04"))
                            testStep = new CCommsDatTest002_04();
              else if(aStepName == _L("step_002_05"))
                            testStep = new CCommsDatTest002_05();
              else if(aStepName == _L("step_002_06"))
                            testStep = new CCommsDatTest002_06();
              else if(aStepName == _L("step_002_07"))
                            testStep = new CCommsDatTest002_07();
              else if(aStepName == _L("step_002_08"))
                            testStep = new CCommsDatTest002_08();
              else if(aStepName == _L("step_002_09"))
                            testStep = new CCommsDatTest002_09();
              else if(aStepName == _L("step_002_10"))
                            testStep = new CCommsDatTest002_10();
              else if(aStepName == _L("step_002_11"))
                            testStep = new CCommsDatTest002_11();
              else if(aStepName == _L("step_002_12"))
                            testStep = new CCommsDatTest002_12();
              else if(aStepName == _L("step_002_13"))
                            testStep = new CCommsDatTest002_13();
              else if(aStepName == _L("step_003_01"))
                            testStep = new CCommsDatTest003_01();
              else if(aStepName == _L("step_003_02"))
                            testStep = new CCommsDatTest003_02();
              else if(aStepName == _L("step_003_03"))
                            testStep = new CCommsDatTest003_03();
              else if(aStepName == _L("step_003_04"))
                            testStep = new CCommsDatTest003_04();
              else if(aStepName == _L("step_003_05"))
                            testStep = new CCommsDatTest003_05();
              else if(aStepName == _L("step_003_06"))
                            testStep = new CCommsDatTest003_06();
              else if(aStepName == _L("step_004_01"))
                            testStep = new CCommsDatTest004_01();
              
	return testStep;
	}
