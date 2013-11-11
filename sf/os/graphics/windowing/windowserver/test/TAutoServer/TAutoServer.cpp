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
// Internal Symbian test code
// 
//

/**
 @file
 @internalComponent
*/

#include "TAutoServer.h"

// Convereted tests
#include "TEVENT.H"
#include "TSPRITE.H"
#include "TBLANK.H"
#include "TMDISPLAY.H"
#include "TPROCPRI.H"
#include "TWINDOW.H"
#include "TRECTLIST.H"
#include "TDERIVED.H"
#include "TSCRDEV.H"
#include "TSCRDEVRESSWITCH.H"
#include "TMESSAGE.H"
#include "TOOMDRAW.H"
#include "TREDRSTR.H"
#include "TPNTKEY.H"
#include "TScreenModeScaling.h"
#include "TSCREENMODEPOSITIONING.H"
#include "TKRepeat.H"
#include "TBITMAP.H"
#include "TBACKUPW.H"
#include "TFADE.H"
#include "TGETPIXEL.H"
#include "TORDINAL.H"
#include "TGWHANDLE.H"
#include "TTransptAnim.h"
#include "TKEY.H"
#include "TMODCHG.H"
#include "TTEXTCURS.H"
#include "TSCROLL.H"
#include "TAlphaChannel.H"
#include "TPANIC.H"
#include "TGDI.H"
#include "TRegion.H"
#include "TREDRAW.H"
#include "TMulTran.H"
#include "TALPHAWIN.H"
#include "TMULTCON.H"
#include "TPNTCAP.H"
#include "TSCRMODE.H"
#include "TAUTODLL.H"
#include "TWSGRAPHS.H"
#include "TCAPABILITY.H"
#include "TPointer.H"
#include "tdirecta.h"
#include "tdirecta2.h"
#include "TOOM.H"
#include "TCapKey.H"
#include "TMULSCREENS.h"
#include "TBUFFERSECURITY.H"
#include "TFLICKERFREE.H"
#include "tdevicerotation.h"
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#include "TFADINGBITMAP.H"
#endif
#include "tcrpanim.h"
#include "TMemLeakMode.h"
#include "tgc.h"
#include "THeartBeat.h"
#include "TGRAPHICSDRAWER.H"
#include "tw32cmdbuf.h"
#include "tadvancedpointerevent.h"
#include "tmultiptrevent.h"
#include "tdrawresource.h"
#include "twindowsizecache.h"


/* Path to the script
z:\GraphicsTest\gditest.script
*/

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
_LIT(KServerName,"tautoserver_nonnga");
#endif

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
_LIT(KServerName,"tautoserver_nga");
#endif

//CTClient *client;

CTAutoServer* CTAutoServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{

	CTAutoServer * server = new (ELeave) CTAutoServer();
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
	CTAutoServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTAutoServer::NewL());
	if(!err)
		{
//		client = CreateClientL();
//		client->SetScreenNumber(1);
//		client->ConstructL();

		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();

		}
	delete server;
	delete sched;
//
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

	TWinCommand commandLine;
	User::CommandLine(commandLine);
	TInt ret=KErrNone;
	if (commandLine.Length()>0)
		{
		ret=CProcess::Start(commandLine);
		// pass test result to parent process (in multi display test)
		RProcess().Terminate(ret);
		return(ret);
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

//	if (client != NULL)
//	delete client;
	return KErrNone;
    }

CTestStep* CTAutoServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	// Debug statements to indicate the progress of the wserv test suite
	RDebug::Print(_L("**********************************************"));
	RDebug::Print(_L("* WServ Test Step: %S"), &aStepName);
	RDebug::Print(_L("**********************************************"));
	
	++CTestDriver::iTestNum;
	CTestStep* testStep = NULL;
	if(aStepName == KTEventTestStep)
		testStep = new CTEventTestStep();
	else if(aStepName == KTTSpriteStep)
		testStep = new CTTSpriteStep();
	else if(aStepName == KTBlankStep)
		testStep = new CTBlankStep();
	else if(aStepName == KTRectListStep)
		testStep = new CTRectListStep();
	else if(aStepName == KTMultiDisplayStep)
		testStep = new CTMultiDisplayStep();
	else if(aStepName == KTWindowTestStep)
		testStep = new CTWindowTestStep();
	else if(aStepName == KTDerivedStep)
		testStep = new CTDerivedStep();
	else if(aStepName == KTScreenDeviceStep)
		testStep = new CTScreenDeviceStep();
	else if(aStepName == KTScreenDeviceResSwitchStep)
	    testStep = new CTScreenDeviceResSwitchStep();
	else if(aStepName == KTMessageStep)
		testStep = new CTMessageStep();
	else if(aStepName == KToomDrawStep)
		testStep = new CToomDrawStep();
	else if(aStepName == KTRedrawStoringStep)
		testStep = new CTRedrawStoringStep();
	else if(aStepName == KTPntKeyStep)
		testStep = new CTPntKeyStep();
	else if(aStepName == KTKRepeatStep)
		testStep = new CTKRepeatStep();
	else if(aStepName == KTBitmapStep)
		testStep = new CTBitmapStep();
	else if(aStepName == KTScreenModeScalingStep)
		testStep = new CTScreenModeScalingStep();
	else if(aStepName == KTScreenModePositioningStep)
		testStep = new CTScreenModePositioningStep();
	else if(aStepName == KTBackedUpWindowStep)
		testStep = new CTBackedUpWindowStep();
	else if(aStepName == KTFadeStep)
		testStep = new CTFadeStep();
	else if(aStepName == KTGetPixelStep)
		testStep = new CTGetPixelStep();
	else if(aStepName == KTOrdinalStep)
		testStep = new CTOrdinalStep();
	else if(aStepName == KTGwHandleStep)
		testStep = new CTGwHandleStep();
	else if (aStepName == KTTransparentAnimStep)
		testStep = new CTTransparentAnimStep();	
	else if(aStepName == KTProcPriStep)
		testStep = new CTProcPriStep();
	else if(aStepName == KTKeyStep)
		testStep = new CTKeyStep();
 	else if(aStepName == KTModifiersChangedStep)
  		testStep = new CTModifiersChangedStep();
 	else if(aStepName == KTCursorTestStep)
  		testStep = new CTCursorTestStep();
 	else if(aStepName == KTScrollStep)
  		testStep = new CTScrollStep();
  	else if(aStepName == KTAlphaChannelStep)
  		testStep = new CTAlphaChannelStep();
 	else if(aStepName == KTPanicStep)
  		testStep = new CTPanicStep();
 	else if(aStepName == KTGdiStep)
  		testStep = new CTGdiStep();
 	else if(aStepName == KTRegionStep)
  		testStep = new CTRegionStep();
 	else if(aStepName == KTRedrawTestStep)
  		testStep = new CTRedrawTestStep();
 	else if(aStepName == KTMultipleTransStep)
  		testStep = new CTMultipleTransStep();
 	else if(aStepName == KTAlphaWinStep)
  		testStep = new CTAlphaWinStep();
 	else if(aStepName == KTMultiConStep)
  		testStep = new CTMultiConStep();
 	else if(aStepName == KTPointerCaptureStep)
  		testStep = new CTPointerCaptureStep();
 	else if(aStepName == KTScrModeStep)
  		testStep = new CTScrModeStep();
 	else if(aStepName == KTAnimDllStep)
  		testStep = new CTAnimDllStep();
 	else if(aStepName == KTWsGraphsStep)
  		testStep = new CTWsGraphsStep();
 	else if(aStepName == KTCapabilityStep)
  		testStep = new CTCapabilityStep();
 	else if(aStepName == KTPointerStep)
  		testStep = new CTPointerStep();
 	else if(aStepName == KTDirectStep)
  		testStep = new CTDirectStep();
 	else if(aStepName == KTDirect2Step)
  		testStep = new CTDirect2Step();
 	else if(aStepName == KTOomStep)
  		testStep = new CTOomStep();
 	else if(aStepName == KTCaptureKeyStep)
  		testStep = new CTCaptureKeyStep();
  	else if(aStepName == KTMulScreensStep)
  		testStep = new CTMulScreensStep();
 	else if(aStepName == KTBufferSecurityStep)
  		testStep = new CTBufferSecurityStep();
   	else if(aStepName == KTFlickerFreeStep)
  		testStep = new CTFlickerFreeStep();
    else if(aStepName == KTDeviceRotationStep)
     testStep = new CTDeviceRotationStep();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	else if(aStepName == KTFadingBitmapStep)
  		testStep = new CTFadingBitmapStep();
#endif
	else if(aStepName == KTCrpAnimStep)
		testStep = new CTCrpAnimStep();
	else if(aStepName == KTMemLeakCheckEnableStep)
		testStep = new CTMemLeakCheckEnableStep();
	else if(aStepName == KTMemLeakCheckOneShotStep)
		testStep = new CTMemLeakCheckOneShotStep();
	else if(aStepName == KTGcStep)
		testStep = new CTGcStep(); 
 	else if(aStepName == KTHeartBeatTestStep)
  		testStep = new CTHeartBeatTestStep();
 	else if(aStepName == KTGraphicsDrawerStep)
 		testStep = new CTGraphicsDrawerStep();
 	else if(aStepName == KTW32CmdBufStep) 
 		testStep = new CTW32CmdBufStep();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
 	else if(aStepName == KTAdvancedPointerEventStep)
 		testStep = new CTAdvancedPointerEventStep();
 	else if(aStepName == KTMultiPtrEventTestStep) 
 	 	testStep = new CTMultiPtrEventTestStep();
	else if(aStepName == KTDrawResourceStep)
		testStep = new CTDrawResourceStep();
    else if(aStepName == KTWindowSizeCacheTestStep)
        testStep = new CTWindowSizeCacheTestStep();
#endif
	return testStep;
	}
