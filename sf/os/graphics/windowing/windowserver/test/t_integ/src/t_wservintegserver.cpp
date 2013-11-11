// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//	User Includes
#include "t_wservintegserver.h"
#include "t_wservintegstepbase.h"
#include "t_wservintegstepbasic.h"
#include "t_wservintegstepdsa.h"
#include "t_wservintegstepsurface.h"
#include "t_wservintegstepfps.h"

CT_WServIntegServer* CT_WServIntegServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CT_WServIntegServer*	server = new (ELeave) CT_WServIntegServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

void CT_WServIntegServer::ConstructL()
	{
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	CTestServer::ConstructL(serverName.Name());
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler*		sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CT_WServIntegServer*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CT_WServIntegServer::NewL());
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
#if (defined TRAP_IGNORE)
	TRAP_IGNORE(MainL());
#else
	TRAPD(err,MainL());
#endif
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CT_WServIntegServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep*	testStep = NULL;

	if ( aStepName==KT_WServIntegStep )
		{
		testStep=new CT_WServIntegStep();
		}
	else if ( aStepName==KT_WServIntegStepDsa )
		{
		testStep= new CT_WServIntegStepDsa();
		}
	else if ( aStepName==KT_WServIntegStepSurface )
		{
		testStep= new CT_WServIntegStepSurface();
		}
	else if ( aStepName==KT_WServIntegStepBasic )
		{
		testStep= new CT_WServIntegStepBasic();
		}
	else if ( aStepName==KT_WServIntegStepFps )
		{
		testStep= new CT_WServIntegStepFps();		
		}

	return testStep;
	}
