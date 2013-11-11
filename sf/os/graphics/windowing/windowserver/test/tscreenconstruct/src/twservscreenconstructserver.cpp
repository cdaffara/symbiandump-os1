// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

//	User includes
#include "twservscreenconstructserver.h"
#include "twservscreenconstructstepload.h"
#include "twservscreenconstructconsts.h"

/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CTWServScreenConstructServer* CTWServScreenConstructServer::NewL()
	{
	CTWServScreenConstructServer* server = new(ELeave) CTWServScreenConstructServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

void CTWServScreenConstructServer::ConstructL()
	{
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	CTestServer::ConstructL(serverName.Name());
	}

/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler*	sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTWServScreenConstructServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTWServScreenConstructServer::NewL());
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
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess Integ
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
#if (defined TRAP_IGNORE)
	TRAP_IGNORE(MainL());
#else
	TRAPD(err,MainL());
#endif
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTWServScreenConstructServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*	testStep = NULL;

	if ( aStepName==KTWServScreenConstructStepLoad )
		{
		testStep=new CTWServScreenConstructStepLoad();
		}
	return testStep;
	}
