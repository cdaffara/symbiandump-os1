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

#include <c32comm.h>
#include "te_c32utilserver.h"
#include "te_c32utilsteps.h"
#include <cfshared.h>
using namespace CommsFW;

_LIT(KServerName,"te_c32util");

CC32UtilServer* CC32UtilServer::NewL()
/**
@return - Instance of the test server
Called inside the MainL() function to create and start the \
CTestServer derived server.
 */
	{
	CC32UtilServer * server = new (ELeave) CC32UtilServer();
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

	CC32UtilServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CC32UtilServer::NewL());
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

CTestStep* CC32UtilServer::CreateTestStep(const TDesC& aStepName)
/** 
Implementation of CTestServer pure virtual
@return A CTestStep derived instance
*/
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestStepLoadC32)
		{
		testStep = new CTestStepLoadC32();
		}
	else if(aStepName == KTestStepUnloadC32)
		{
		testStep = new CTestStepUnloadC32(EGraceful);
		}
	else if(aStepName == KTestStepUnloadC32Forced)
		{
		testStep = new CTestStepUnloadC32(EImmediate);
		}
	else if(aStepName == KTestStepUnloadC32_Leaks)
		{
		testStep = new CTestStepUnloadC32(EImmediate, CTestStepUnloadC32::ELeaks);
		}
	else if(aStepName == KTestStepInjectMemLeakC32)
		{
		testStep = new CTestStepInjectMemLeakC32();
		}
		

	return testStep;
	}

