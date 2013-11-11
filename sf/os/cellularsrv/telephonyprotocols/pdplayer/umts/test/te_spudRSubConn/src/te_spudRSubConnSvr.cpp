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
// te_spudRSubConSvr.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to
// be started in the process of the client. The client initialises the server
// by calling the one and only ordinal.
// 
//

/**
 @file
 @internalComponent
*/

#include "te_spudRSubConnSvr.h"
#include "te_spudRSubConnSteps.h"

_LIT(KServerName,"TE_SpudRSubConnSvr");




// EKA2 much simpler
// just an E32Main and a MainL()

/**
 * Much simpler, uses the new Rendezvous() call to sync with the client.
 *
 * @internalComponent
 *
 * @leave When new(ELeave) leaves 
 */
LOCAL_C void MainL()
	{
	// leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CSpudRSubConTestServer* server = NULL;
	// create the CTestServer derived server
	TRAPD(err,server = CSpudRSubConTestServer::NewL());
	if (!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// only a DLL on emulator for typhoon and earlier

/**
 * Main entry point.
 *
 * @internalComponent
 *
 * @return Standard Epoc error codes.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

/**
 * Called inside the MainL() function to create and start the CTestServer derived server.
 *
 * @internalComponent
 *
 * @return Instance of the test server.
 */
CSpudRSubConTestServer* CSpudRSubConTestServer::NewL()
	{
	CSpudRSubConTestServer * server = new (ELeave) CSpudRSubConTestServer();
	CleanupStack::PushL(server);
	
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	



/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CSpudRSubConTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* step = NULL;
	
	// removed ELeave as harness will test ptr. This is more efficient
	// than using TRAP_IGNORE
	if(aStepName == KCUdp1)
		{
		step = new CUdp1;			
		}
	else if(aStepName == KCUdp2)
		{
		step = new CUdp2;
		}
	else if(aStepName == KCLoopbackPpp1)
		{
		step = new CLoopbackPpp1;			
		}
	else if(aStepName == KCUpperFlowOnLowerNifDown)
		{
		step = new CUpperFlowOnLowerNifDown;			
		}
	else if(aStepName == KCSpudPppPrimaryStop)
		{
		step = new CSpudPppPrimaryStop;
		}
	return step;
	}











