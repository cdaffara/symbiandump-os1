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

#include "te_configutilserver.h"
#include "te_configutilsteps.h"
#include <cfshared.h>
using namespace CommsFW;

_LIT(KServerName,"te_configutil");

CConfigUtilServer* CConfigUtilServer::NewL()
/**
@return - Instance of the test server
Called inside the MainL() function to create and start the \
CTestServer derived server.
 */
	{
	CConfigUtilServer * server = new (ELeave) CConfigUtilServer();
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

	CConfigUtilServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CConfigUtilServer::NewL());
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
	__ASSERT_ALWAYS(!err, User::Panic(_L("Panic From MainL"), err));
	delete cleanup;
	return KErrNone;
    }

CTestStep* CConfigUtilServer::CreateTestStep(const TDesC& aStepName)
/** 
Implementation of CTestServer pure virtual
@return A CTestStep derived instance
*/
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestStepLoadConfig)
		{
		testStep = new CTestStepLoadConfig();
		}
	else if(aStepName == KTestStepUnLoadConfig)
		{
		testStep = new CTestStepUnLoadConfig(EGraceful);
		}
	else if(aStepName == KTestStepUnLoadConfigForced)
		{
		testStep = new CTestStepUnLoadConfig(EImmediate);
		}

	return testStep;
	}

