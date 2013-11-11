// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
	@test
 */

#include <ecom/ecom.h>

#include "egltestrefserver.h"
#include "egltestrefstep.h"


_LIT(KEGLTestServerName,"egltestrefserver");


CTestStep* CTestEGLRefServer::CreateTestStep(const TDesC& aStepName)
	{
	return EglTestStepFactory::GetEglTestStep(aStepName);
	}

CTestEGLRefServer* CTestEGLRefServer::NewL()
	{
	CTestEGLRefServer* server = new(ELeave) CTestEGLRefServer();
	CleanupStack::PushL(server);
	// CServer base class call
	TParsePtrC serverName(RProcess().FileName());	
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

#ifdef __WINS__
	// Construct and destroy a process-wide state object in emulator builds.
	// This will cause initialisation of PLS for EGL and SgDriver
	// and allow checking for leaks in tests
	if(!eglReleaseThread())
		{
		// Call to eglReleaseThread failed
		User::Leave(KErrGeneral);
		}
#endif //__WINS__ 

	CTestEGLRefServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTestEGLRefServer::NewL());
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
  @return Standard Epoc error code on process exit
  Process entry point. Called by client using RProcess API
  */
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	
	if (err)
		{
		RDebug::Print(_L("CTestEGLRefServer::MainL - Error: %d"), err);
	   	User::Panic(KEGLTestServerName,err);
		}
		
	delete cleanup;
	REComSession::FinalClose();	
	__UHEAP_MARKEND;
	return KErrNone;
    }
