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



// EPOC includes
#include <e32base.h>

// Test system includes
//#include "TestStepSocket.h"
#include "TE_EIntSockServer.h"
#include "TE_EIntSockTestStep.h"


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

	CTestServer* server = NULL;
	// Create the CTestServer derived server

	TRAPD(err,server = CTestEIntSockServer::NewL());
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

//---------------------------------------------------------------------------------------------------------------------------


CTestServer* CTestEIntSockServer::NewL()
	{
	CTestEIntSockServer* server = new(ELeave) CTestEIntSockServer;
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	//	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestEIntSockServer::~CTestEIntSockServer()
	{
	}

CTestEIntSockServer::CTestEIntSockServer()
	{
	}

CTestStep* CTestEIntSockServer::CreateTestStep(const TDesC& aStepName)
	{
	// removed ELeave as harness will test ptr. This is more efficient
	// than using TRAP_IGNORE
	CEIntSockTestStep* ret = new CEIntSockTestStep;
	if(ret)
	{
		ret->SetTestStepName(aStepName);
	}
	return ret;
	}

