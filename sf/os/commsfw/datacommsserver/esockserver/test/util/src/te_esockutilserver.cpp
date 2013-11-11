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
//

#include "te_esockutilserver.h"
#include "te_esockutilsteps.h"
#include <cfshared.h>
using namespace CommsFW;

_LIT(KServerName,"te_esockutil");

CESockUtilServer* CESockUtilServer::NewL()
/**
@return - Instance of the test server
Called inside the MainL() function to create and start the \
CTestServer derived server.
 */
	{
	CESockUtilServer * server = new (ELeave) CESockUtilServer();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
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
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CESockUtilServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CESockUtilServer::NewL());
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
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CESockUtilServer::CreateTestStep(const TDesC& aStepName)
/** 
Implementation of CTestServer pure virtual
@return A CTestStep derived instance
*/
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestStepLoadESock)
		{
		testStep = new CTestStepLoadESock();
		}
	else if(aStepName == KTestStepUnloadESock)
		{
		testStep = new CTestStepUnloadESock(EGraceful, CTestStepUnloadESock::ENoLeak, EFalse);
   		}
   	else if(aStepName == KTestStepUnloadESockForced)
   		{
		testStep = new CTestStepUnloadESock(EImmediate, CTestStepUnloadESock::ENoLeak, EFalse);
		}
	else if(aStepName == KTestStepUnloadESockForcedTimeout)
		{
		testStep = new CTestStepUnloadESock(EImmediate, CTestStepUnloadESock::ENoLeak, ETrue);
		}
	else if(aStepName == KTestStepUnloadESock_MayLeak)
		{
		testStep = new CTestStepUnloadESock(EImmediate, CTestStepUnloadESock::EMayLeak, EFalse);
		}
	else if(aStepName == KTestStepUnloadESock_Leaks)
		{
		testStep = new CTestStepUnloadESock(EImmediate, CTestStepUnloadESock::ELeaks, EFalse);
		}
	else if(aStepName == KTestStepStopAllInterfaces)
		{
		testStep = new CTestStepStopAllInterfaces();
		}
	else if(aStepName == KTestStepDefaultToConfig1)
		{
		testStep = new CTestStepDefaultToConfig1();
		}
	else if(aStepName == KTestStepIsNetworkNotActive)
		{
		testStep = new CTestStepIsNetworkNotActive();
		}
	else if(aStepName == KTestStepDelay)
		{
		testStep = new CTestStepDelay();
		}
	else if(aStepName == KTestStepAlwaysFail)
		{
		testStep = new CTestStepAlwaysFail();
		}
	else if(aStepName == KTestStepResetAllocFails)
		{
		testStep = new CTestStepResetAllocFails();
		}
	else if(aStepName == KTestStepLoadConfig)
		{
		testStep = new CTestStepLoadConfig();
		}
	else if(aStepName == KTestStepCopyFiles)
		{
		testStep = new CTestStepCopyFiles();
		}

	return testStep;
	}

