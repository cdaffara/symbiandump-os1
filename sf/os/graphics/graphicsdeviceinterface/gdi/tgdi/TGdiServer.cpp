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

#include "TGdiServer.h"
#include "TRGB.H"
#include "TTYPES.H"
#include "TBiDi.h"
#include "TBiDiDefect.h"
#include "TLineBreak.h"
#include "TGlyphSel.h"
 
/* Path to the script
z:\GraphicsTest\gditest.script
*/

_LIT(KServerName,"TGdiServer");

CTGdiServer* CTGdiServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTGdiServer * server = new (ELeave) CTGdiServer();
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
	CTGdiServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTGdiServer::NewL());
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

CTestStep* CTGdiServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KTRgbStep)
		testStep = new CTRgbStep();
	else if(aStepName == KTTypesStep)
		testStep = new CTTypesStep();
	else if(aStepName == KTBiDiStep)
		testStep = new CTBiDiStep();
	else if(aStepName == KTBiDiDefectStep)
		testStep = new CTBiDiDefectStep();
	else if(aStepName == KTLineBreakStep)
		testStep = new CTLineBreakStep();
	else if(aStepName == KTGlyphSelectionStep)
		testStep = new CTGlyphSelectionStep();

	return testStep;
	}
