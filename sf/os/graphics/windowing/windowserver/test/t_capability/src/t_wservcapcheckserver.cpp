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

#include "t_wservcapcheckserver.h"
#include "t_wservcapchecksteptsizemode.h"


CT_WServCapCheckServer* CT_WServCapCheckServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CT_WServCapCheckServer* server = new (ELeave) CT_WServCapCheckServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

void CT_WServCapCheckServer::ConstructL()
	{
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	CTestServer::ConstructL(serverName.Name());
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);

	CActiveScheduler*		sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CT_WServCapCheckServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CT_WServCapCheckServer::NewL());
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
 * Process entry point. Called by client using RProcess Integ
 */
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());
	delete cleanup;

	__UHEAP_MARKEND;

	return err;
    }

CTestStep* CT_WServCapCheckServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if ( aStepName == KT_WServCapCheckStepTSizeModePos )
		{
		testStep = new CT_WServCapCheckStepTSizeModePos();
		}
	if ( aStepName == KT_WServCapCheckStepTSizeModeNeg)
		{
		testStep = new CT_WServCapCheckStepTSizeModeNeg();
		}
	return testStep;
	}
