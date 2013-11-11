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

/**
 @file
 @test 
*/

#include "te_graphicsperformancesuiteserver_s60.h"
#include "ttextlistscroll.h"
#include "tfullscreentext.h"
#include "tuiandvideo.h"
#include "tcomplexfonts.h"
#include "tfontmultiplesizes.h"
#include "tpan.h"
#include "tscale.h"
#include "tflowwindowstest.h"
#include "trotateteststep.h"
#include "tflowwindowstestopenglvg.h"
#include "tsmallwindowstest.h"
#include "tsmallwindowstestopengl.h"
#include "tsmallwindowstestzoom.h"
#include "tzorder.h"
#include "tzordermultiplesurfaces.h"
#include "tsmallwindowstestopenvg.h"
#include "tsmallwindowstestraster.h"
#include "tcopyrect.h"


/**
Same code for Secure and non-secure variants
Called inside the MainL() function to create and start the
CTestServer derived server.

@return - Instance of the test server
*/
CTe_graphicsperformanceSuite_S60* CTe_graphicsperformanceSuite_S60::NewL()
	{
	CTe_graphicsperformanceSuite_S60 * server = new (ELeave) CTe_graphicsperformanceSuite_S60();
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

	CTe_graphicsperformanceSuite_S60* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CTe_graphicsperformanceSuite_S60::NewL());
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
CTestStep* CTe_graphicsperformanceSuite_S60::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	if (aStepName == KTTextListScroll)
	    {
		testStep = new CTTextListScroll();
		}
	else if (aStepName == KFullScreenText)
	    {
		testStep = new CTFullScreenText();
		}
	else if(aStepName == KTUiAndVideo)
	    {
		testStep = new CTUiAndVideo();
		}
	else if(aStepName == KTComplexFonts)
		{
		testStep = new CTComplexFonts();
		}
	else if(aStepName == KFontMultipleSizesTestStep)
	    {
		testStep = new CTFontMultipleSizesTestStep();
		}
	else if(aStepName == KTPan)
	    {
		testStep = new CTPan();
		}
	else if(aStepName == KTScale)
	    {
		testStep = new CTScale();
		}
	else if(aStepName == KTFlowWindows)
	    {
		testStep = new CTFlowWindowsTest();
		}
	else if(aStepName == KTRotateTestStep)
	    {
		testStep = new CTRotateTestStep();
		}
	else if(aStepName == KTFlowWindowsOpenGLVG)
	    {
		testStep = new CTFlowWindowsTestOpenGLVG();
		}
	else if(aStepName == KTSmallWindows)
	    {
		testStep = new CTSmallWindowsTest();
		}
	else if(aStepName == KTSmallWindowsOpenGL)
	    {
		testStep = new CTSmallWindowsTestOpenGL();
		}
	else if(aStepName == KTSmallWindowsZoom)
	    {
		testStep = new CTSmallWindowsTestZoom();
		}
	else if(aStepName == KTZOrder)
	    {
		testStep = new CTZOrder();
		}
	else if(aStepName == KTZOrderMultipleSurfaces)
	    {
		testStep = new CTZOrderMultipleSurfaces();
		}
	else if(aStepName == KTSmallWindowsOpenVG)
	    {
		testStep = new CTSmallWindowsTestOpenVG();
		}
	else if(aStepName == KTSmallWindowsRaster)
	    {
		testStep = new CTSmallWindowsTestRaster();
		}
	else if(aStepName == KTCopyRect)
	    {
		testStep = new CTCopyRect();
		}

	return testStep;
	}
