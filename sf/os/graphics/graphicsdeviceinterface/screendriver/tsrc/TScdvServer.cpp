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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "TScdvServer.h"
#include "Tlld.h"
#include "TScdvScaling.h"
#include "TScdvTest.h"
#include "TRWindows.h"

#if defined(SYMBIAN_GRAPHICS_GCE)
#include "TDirectScreenBitmap.h"
#include "tscdvdevorientation.h"
#endif //SYMBIAN_GRAPHICS_GCE

/*
z:\GraphicsTest\scdvtest.script
*/

_LIT(KServerName,"TScdvServer");

CTScdvServer* CTScdvServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTScdvServer * server = new (ELeave) CTScdvServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//
	{
#if (defined __DATA_CAGING__)
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTScdvServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTScdvServer::NewL());
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

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
*/
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	// This if statement is here just to shut up RVCT, which would otherwise warn
	// that err was set but never used
	if (err)
	    {
	    err = KErrNone;
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTScdvServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KTLowLevelStep)
		testStep = new CTLowLevelStep();
	else if(aStepName == KTLowLevel1Step)
		testStep = new CTLowLevel1Step();
	else if(aStepName == KTRWindowsStep)
		testStep = new CTRWindowsStep();
	else if(aStepName == KTScalingStep)
		testStep = new CTScalingStep();
	else if(aStepName == KTScdvStep)
		testStep = new CTScdvStep();
#if defined(SYMBIAN_GRAPHICS_GCE)
	else if (aStepName == KTDevOrientationStep)
	 	testStep = new CTDevOrientationStep();
	else if(aStepName == KTDirectScreenBitmapStep)
		testStep = new CTDirectScreenBitmapStep();
#endif //SYMBIAN_GRAPHICS_GCE

	return testStep;
	}
