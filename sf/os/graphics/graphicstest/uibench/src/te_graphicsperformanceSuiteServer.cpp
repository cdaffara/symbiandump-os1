// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_graphicsperformanceSuiteServer.h"
#include "tbitbltperf.h"
#include "tfbsfonthandleperf.h"
#include "tfbsbitmaphandleperf.h"
#include "talphadrawing.h"
#include "tscrollingperf.h"
#include "toptimisedbmp.h"
#include "tscaleddrawbitmap.h"
#include "tsolidcolour.h"
#include "talphablend.h"
#include "tsimpledrawing.h"
#include "tshadowperf.h"
#include "tredrawing.h"
#include "tnonredrawdrawing.h"
#include "tgdifontrenderingperf.h"
#include "toutlineshadowdrawing.h"
#include "tfadeperf.h"
#include "tredrawstoreperf.h"
#include "ttranslucent.h"
#include "tspriteperf.h"
#include "textendedbitmap.h"
#include "tfbsglyphdata.h"

/**
Same code for Secure and non-secure variants
Called inside the MainL() function to create and start the
CTestServer derived server.

@return - Instance of the test server
*/
CTe_graphicsperformanceSuite* CTe_graphicsperformanceSuite::NewL()
	{
	CTe_graphicsperformanceSuite * server = new (ELeave) CTe_graphicsperformanceSuite();
	CleanupStack::PushL(server);

	// Get server name from process so we can use SetCap to change the capabilites and use this server with a different filename
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());

	server->ConstructL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}


/**
MainL - uses the new Rendezvous() call to sync with the client
*/
LOCAL_C void MainL()
	{
	// Active scheduler only for this thread. Test need to create own active scheduler	
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	CTe_graphicsperformanceSuite* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CTe_graphicsperformanceSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}
	delete server;
	delete scheduler;
	}


/**
Secure variant only
Process entry point. Called by client using RProcess API

@return - Standard Epoc error code on process exit
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

/**
Secure and non-secure variants
Implementation of CTestServer pure virtual

@return - A CTestStep derived instance
*/
CTestStep* CTe_graphicsperformanceSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	if(aStepName == KTBitBltPerfName)
		testStep = new CTBitBltPerf();
	else if(aStepName == KTFbsFontHandlePerfName)
		testStep = new CTFbsFontHandlePerf();
	else if(aStepName == KTFbsBitmapHandlePerfName)
		testStep = new CTFbsBitmapHandlePerf();
	else if(aStepName == KTAlphaDrawing)
		testStep = new CTAlphaDrawing();
	else if(aStepName == KScrollingPerfName)
		testStep = new CScrollingPerf();
	else if(aStepName == KTOptimisedBmpStep)
		testStep = new CTOptimisedBmp();
	else if(aStepName == KTScaledDrawBitmap)
		testStep = new CTScaledDrawBitmap();
	else if (aStepName == KTSolidColourName)
		testStep = new CTSolidColour();
	else if(aStepName == KAlphaBlendTest)
		testStep = new CAlphaBlendTestNormal();
	else if(aStepName == KAlphaBlendTestIType)
		testStep = new CAlphaBlendTestIType();
	else if (aStepName == KTSimpleDrawingPerfName)
		testStep = new CTSimpleDrawing();
	else if (aStepName == KTShadowPerfName)
		testStep = new CTShadowPerf();
	else if (aStepName == KTFadePerfName)
		testStep = new CTFadePerf();
	else if (aStepName == KRedrawingTest)
		testStep = new CRedrawingTest();
	else if (aStepName == KTNonRedrawDrawing)
		testStep = new CTNonRedrawDrawing();
 	else if (aStepName == KTGDIFontRenderingPerfName)
		testStep = new CTGDIFontRenderingPerf();
	else if(aStepName == KTOutlineShadowDrawing)
		testStep = new CTOutlineShadowDrawing();
	else if(aStepName == KTRedrawstorePerf)
		testStep = new CTRedrawstorePerf();	
	else if(aStepName == KTTranslucent)
		testStep = new CTTranslucent();	
	else if (aStepName == KTSpritePerfName)
		testStep = new CTSpritePerf();
	else if(aStepName == KTExtendedBitmap)
		testStep = new CTExtendedBitmap();
	else if(aStepName == KTFbsGlyphData)
		testStep = new CTFbsGlyphData();
	return testStep;
	}
