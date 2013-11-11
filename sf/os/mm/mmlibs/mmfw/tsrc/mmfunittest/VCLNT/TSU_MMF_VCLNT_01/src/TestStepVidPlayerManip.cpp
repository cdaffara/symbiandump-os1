// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <badesca.h>
// #include <techview/eikon.rsg>
// #include <techview/eikcfdlg.h>
// #include <mw/apgcli.h>
// #include <mw/apacmdln.h>

// Test system includes
#include <testframework.h>

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mmf/common/mmfcontroller.h>
#include <videoplayer.h>
#include <videorecorder.h>
#include <caf/caf.h>

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#endif

#include "TS_Codes.h"


_LIT(KVclntAppTestResultName, "C:\\VCLNT_APP_TEST_RESULT.txt");
//_LIT(KVclntAppSemName, "VclntAppSem");
_LIT8(KLitPass, "EPass");
_LIT8(KLitFail, "EFail");
_LIT8(KLitInconclusive, "EInconclusive");
_LIT8(KLitTestSuiteError, "ETestSuiteError");



using namespace ContentAccess;

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidPlayer.h"

const TInt KFrameWidth = 100;
const TInt KFrameHeight = 50;


const TReal32 KScaleWidth1 = 1.5f;
const TReal32 KScaleWidth2 = .75f;
const TReal32 KScaleHeight1 = 1.65f;
const TReal32 KScaleHeight2 = .3f;

const TInt KCropRegionRectTop = 1;
const TInt KCropRegionRectLeft =3;
const TInt KCropRegionRectRight = 5;
const TInt KCropRegionRectBottom = 7;


_LIT(KClockFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");
_LIT(KVideoTestControllerName, "CMMFVideoTestController");
_LIT(KVidOnlyFile, "c:\\mm\\mmf\\testfiles\\vclnt\\xvidonly.avi");

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerPlay.
 *
 *
 * @return	"CTestStepVidPlayerPlay*"
 *			The constructed CTestStepVidPlayerPlay
 */
CTestStepVidPlayerPlay* CTestStepVidPlayerPlay::NewL()
	{
	CTestStepVidPlayerPlay* self = new(ELeave) CTestStepVidPlayerPlay;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerPlay::CTestStepVidPlayerPlay() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0020-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerPlay::~CTestStepVidPlayerPlay()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerPlay::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerPlay::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerPlay::MvpuoPlayComplete(TInt aError)
	{
	iHasPlayed = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerPlay::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerPlay::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerPlay::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing a video with a CVideoPlayerUtility::PlayL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerPlay::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerPlay::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !PlayerStateStopped() )
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;

	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, PlayAndStartSchedulerL() );
	if ( err == KErrNone && iHasPlayed )
		return EPass;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerStop.
 *
 *
 * @return	"CTestStepVidPlayerStop*"
 *			The constructed CTestStepVidPlayerStop
 */
CTestStepVidPlayerStop* CTestStepVidPlayerStop::NewL()
	{
	CTestStepVidPlayerStop* self = new(ELeave) CTestStepVidPlayerStop;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerStop::CTestStepVidPlayerStop() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0021-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerStop::~CTestStepVidPlayerStop()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerStop::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerStop::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerStop::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerStop::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerStop::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerStop::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test stops playing a video with CVideoPlayerUtility::StopL()"));
	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerStop::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerStop::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EPass;

	iError = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !iPlayer || !PlayerStateStopped() )
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EFail;
		
	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, PlayAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone )
		return EInconclusive;


	err = iPlayer->Stop();

	if (err != KErrNone)
		return EFail;

	if ( !iPlayer || !PlayerStateStopped() )
		return EFail;

	delete iPlayer;
	iPlayer = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerPause.
 *
 *
 * @return	"CTestStepVidPlayerPause*"
 *			The constructed CTestStepVidPlayerPause
 */
CTestStepVidPlayerPause* CTestStepVidPlayerPause::NewL()
	{
	CTestStepVidPlayerPause* self = new(ELeave) CTestStepVidPlayerPause;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerPause::CTestStepVidPlayerPause() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0022-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerPause::~CTestStepVidPlayerPause()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerPause::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerPause::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerPause::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerPause::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerPause::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerPause::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test pauses a playing video with a CVideoPlayerUtility::PauseL()"));

	iError = KErrNone; //KErrTimedOut;
	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerPause::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerPause::OpenAndStartSchedulerL()
	{
	iPlayer->OpenFileL(iFileName);
	CActiveScheduler::Start();
	}
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerPause::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EPass;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !iPlayer || !PlayerStateStopped() )
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EFail;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if (err != KErrNone) 
		return EFail;

	TRAP(err, PlayAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone )
		return EInconclusive;

	iPlayer->PauseL();

	if ( !iPlayer || !PlayerStatePaused() )
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerPriority.
 *
 *
 * @return	"CTestStepVidPlayerPriority*"
 *			The constructed CTestStepVidPlayerPriority
 */
CTestStepVidPlayerPriority* CTestStepVidPlayerPriority::NewL()
	{
	CTestStepVidPlayerPriority* self = new(ELeave) CTestStepVidPlayerPriority;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerPriority::CTestStepVidPlayerPriority() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0023-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerPriority::~CTestStepVidPlayerPriority()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerPriority::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerPriority::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerPriority::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerPriority::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerPriority::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerPriority::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test retrieves the priority setting with CVideoPlayerUtility::PriorityL()"));


	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerPriority::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerPriority::DoTestStepL()
	{
	iTestStepResult = EFail;
	TInt err;
	err = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !PlayerStateStopped())
		{
		INFO_PRINTF1(_L("Player not in stopped state"));
		return EFail;
		}

	TInt priority;
	TMdaPriorityPreference preference;

	// Set/GetPriority test 0
	// Testing before the controller is ready
	iPlayer->SetPriorityL(EMdaPriorityMin, EMdaPriorityPreferenceNone);
	iPlayer->PriorityL(priority, preference);
	if ((priority != EMdaPriorityMin) && (preference != EMdaPriorityPreferenceNone))
		{
		INFO_PRINTF1(_L("Priority test failed"));
		return EFail;
		}

	TRAP(err, OpenAndStartSchedulerL() );
	if ( err != KErrNone || iError != KErrNone) 
		{
		INFO_PRINTF2(_L("Error calling OpenAndStartSchedulerL %d"), iError);
		return EInconclusive;
		}

	// check that the values are still correct
	iPlayer->PriorityL(priority, preference);
	if ((priority != EMdaPriorityMin) && (preference != EMdaPriorityPreferenceNone))
		{
		INFO_PRINTF1(_L("Priority test failed"));
		return EFail;
		}

	// Set/GetPriority test 1
	iPlayer->SetPriorityL(EMdaPriorityMin, EMdaPriorityPreferenceNone);
	iPlayer->PriorityL(priority, preference);
	if ((priority != EMdaPriorityMin) && (preference != EMdaPriorityPreferenceNone))
		{
		INFO_PRINTF1(_L("Priority test failed"));
		return EFail;
		}

	// Set/GetPriority test 2
	iPlayer->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTime);
	iPlayer->PriorityL(priority, preference);
	if ((priority != EMdaPriorityNormal) && (preference != EMdaPriorityPreferenceTime))
		{
		INFO_PRINTF1(_L("Priority test failed"));
		return EFail;
		}

	// Set/GetPriority test 3
	iPlayer->SetPriorityL(EMdaPriorityMax, EMdaPriorityPreferenceQuality);
	iPlayer->PriorityL(priority, preference);
	if ((priority != EMdaPriorityMax) && (preference != EMdaPriorityPreferenceQuality))
		{
		INFO_PRINTF1(_L("Priority test failed"));
		return EFail;
		}

	// Set/GetPriority test 4
	iPlayer->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	iPlayer->PriorityL(priority, preference);
	if ((priority != EMdaPriorityNormal) && (preference != EMdaPriorityPreferenceTimeAndQuality))
		{
		INFO_PRINTF1(_L("Priority test failed"));
		return EFail;
		}

	iTestStepResult = EPass;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerSetDisplayWindow.
 *
 *
 * @return	"CTestStepVidPlayerSetDisplayWindow*"
 *			The constructed CTestStepVidPlayerSetDisplayWindow
 */
CTestStepVidPlayerSetDisplayWindow* CTestStepVidPlayerSetDisplayWindow::NewL(TBool aUseSurface,TInt aStatus)
	{
	CTestStepVidPlayerSetDisplayWindow* self = new(ELeave) CTestStepVidPlayerSetDisplayWindow(aUseSurface, aStatus);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerSetDisplayWindow::CTestStepVidPlayerSetDisplayWindow(TBool aUseSurface, TInt aStatus) 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = aUseSurface;
	if (aUseSurface && aStatus == CTestStepUnitMMFVidClient::ESurfaceMode)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0854-CP");
		}
	else if (!aUseSurface && aStatus == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0025-HP");
		}
	else
		{
		iTestStepName = _L("unknown");
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerSetDisplayWindow::~CTestStepVidPlayerSetDisplayWindow()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSetDisplayWindow::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerSetDisplayWindow::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerSetDisplayWindow::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerSetDisplayWindow::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerSetDisplayWindow::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerSetDisplayWindow::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this test sets the display window with CVideoPlayerUtility::SetDisplayWindowL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this test sets the display window with CVideoPlayerUtility2::SetDisplayWindowL()"));
		}

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerSetDisplayWindow::DoTestStepPostambleL(void)
	{
	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else 
		{
		delete iPlayer2;
		iPlayer2 = NULL;	
		}
#endif
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerSetDisplayWindow::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	TRect clipRect(0, 0, 80, 60);
	TPoint screenPos(10, 10);
	TRect screenRect(screenPos, clipRect.Size());
	TRect refScreenRect, refClipRect;

	InitWservL();

	TRect rect;
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, rect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else 
		{
			iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		}
#endif	
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("Failed NewL() or the state is not stopped."));
		return EInconclusive;
		}

	TRAP(err, OpenAndStartSchedulerL());

	if (err != KErrNone || iError != KErrNone) 
		{
		return  EInconclusive;
		}
		
	if (!iVpu2)
		{
		TRAP(err, iPlayer->SetDisplayWindowL(iWs, *iScreen, *iWindow, screenRect, clipRect));	
		}
#ifdef SYMBIAN_BUILD_GCE
	else 
		{
		TRAP(err, iPlayer2->SetDisplayWindowL(iWs, *iScreen, *iWindow, screenRect, clipRect));
		}

	if (iVpu2 && err != KErrNotSupported)
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		return EFail;
		}
#endif
	if (!iVpu2)
		{
		if (err != KErrNone || 
			GetPlayerWindowRect(refScreenRect) || 
			GetPlayerClipRect(refClipRect) )
			{
			return EFail;
			}

		if (refClipRect != clipRect || refScreenRect != screenRect)
			{
			return EFail;
			}
		
#ifdef SYMBIAN_BUILD_GCE
		SurfaceSupported();
#endif	
		if (!iSurfaceSupported)
			{
			iTestStepResult = ControllerCalled(_L8("SetDisplayWindowL called"));
			}

		delete iPlayer;
		iPlayer = NULL;
		}

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//
CTestStepVidPlayerRestart* CTestStepVidPlayerRestart::NewL()
	{
	CTestStepVidPlayerRestart* self = new(ELeave) CTestStepVidPlayerRestart();
	return self;
	}

CTestStepVidPlayerRestart::CTestStepVidPlayerRestart()
	{
	iTestStepResult = EPass;
	iTestStepName = _L("MM-MMF-VCLNT-U-0050-LP");
	}

CTestStepVidPlayerRestart::~CTestStepVidPlayerRestart()
	{
	}

TVerdict CTestStepVidPlayerRestart::DoTestStepPreambleL()
	{
	TVerdict verdict = EFail;

	InitWservL();

	TRAPD(err, InitEikonL());
	if (err == KErrNone)
		{
		iFileName.Set(KClockFileName);
		INFO_PRINTF1(iTestStepName);
		INFO_PRINTF1(_L("Testing CVideoPlayerUtility::CBody::Restart"));
		verdict = EPass;
		}

	return verdict;
	}

TVerdict CTestStepVidPlayerRestart::DoTestStepPostambleL()
	{
	delete iPlayer;
	iPlayer = NULL;

	DeInitWserv();

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerRestart::Close()
	{
	CTestStepUnitMMFVidClient::Close();
	// DeInitEikon has to be called outside of any TRAP handler
	DeInitEikon();
	}

TVerdict CTestStepVidPlayerRestart::DoTestStepL()
	{
	iTestStepResult = EFail;

	TInt err = KErrNone;

	RFs fs;
	err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting to fs session %d"), err);
		return EInconclusive;
		}

	// Make sure the results file does not exist
	err = fs.Delete(KVclntAppTestResultName);
	if (err != KErrNone && err != KErrNotFound)
		{
		INFO_PRINTF2(_L("Error deleting results file %d"), err);
		return EInconclusive;
		}

	// Prepare to run the vclnt test app
	TBuf8<KMaxLenTestStepName> testStepName;
	testStepName.Copy(iTestStepName);
/*	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL(KTestAppFullPath);
	cmdLine->SetTailEndL(testStepName);
	RApaLsSession session;
	err = session.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Unable to connect to RApaLsSession"));
		return EInconclusive;
		}
	// Start the app
	INFO_PRINTF1(_L("Starting VCLNT test app..."));
	err = session.StartApp(*cmdLine);
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Unable to start vclnt test app"));
		return EInconclusive;
		}

	// Wait for the app to finish
	RSemaphore sem;
	err = sem.CreateGlobal(KVclntAppSemName, 0);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error creating semaphore %d"), err);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Waiting for VCLNT test app to finish..."));
	sem.Wait();
	sem.Close();
	CleanupStack::PopAndDestroy(cmdLine);

	INFO_PRINTF1(_L("App finished"));
*/
	// Get the test result from file created by the vclnt test app
	RFile file;
	err = file.Open(fs, KVclntAppTestResultName, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening results file %d"), err);
		return EInconclusive;
		}

	TBuf8<32> resultsBuf;
	err = file.Read(resultsBuf);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading results file %d"), err);
		return EInconclusive;
		}

	// Convert the buffer to a valid result value
	TBool recognizedResult = ETrue;
	if (resultsBuf.Compare(KLitPass) == 0)
		{
		iTestStepResult = EPass;
		}
	else if (resultsBuf.Compare(KLitFail) == 0)
		{
		iTestStepResult = EFail;
		}
	else if (resultsBuf.Compare(KLitInconclusive) == 0)
		{
		iTestStepResult = EInconclusive;
		}
	else if (resultsBuf.Compare(KLitTestSuiteError) == 0)
		{
		iTestStepResult = ETestSuiteError;
		}
	else
		{
		// Result was not recognized
		recognizedResult = EFalse;
		iTestStepResult = EInconclusive;
		}
	file.Close();

	if (recognizedResult)	// if it's an unrecognized result, don't delete the result file
		{
		// Delete the test result file
		err = fs.Delete(KVclntAppTestResultName);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Warning: Error deleting results file %d"), err);
			}
		}
	fs.Close();

	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
	}

// from MVideoRecorderUtilityObserver
void CTestStepVidPlayerRestart::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepVidPlayerRestart::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerRestart::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
	
void CTestStepVidPlayerRestart::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

	
void CTestStepVidPlayerRestart::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

//
CTestStepVidPlayerStopStartDSA* CTestStepVidPlayerStopStartDSA::NewL(TInt aSurfaceMode)
	{
	CTestStepVidPlayerStopStartDSA* self = new(ELeave) CTestStepVidPlayerStopStartDSA(aSurfaceMode);
	return self;
	}

CTestStepVidPlayerStopStartDSA::CTestStepVidPlayerStopStartDSA(TInt aSurfaceMode)
	{
	iTestStepResult = EPass;
	if (aSurfaceMode == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iVpu2 = EFalse;
		iTestStepName = _L("MM-MMF-VCLNT-U-0051-LP");
		}
	else 
		{
		iVpu2 = ETrue;
		iTestStepName = _L("MM-MMF-VCLNT-U-0864-CP");
		}
	}

CTestStepVidPlayerStopStartDSA::~CTestStepVidPlayerStopStartDSA()
	{
	}

TVerdict CTestStepVidPlayerStopStartDSA::DoTestStepPreambleL()
	{
	TVerdict verdict = EFail;

	InitWservL();

	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("This test is stopping and restarting DirectScreenAccess of CVideoPlayerUtility"));
		}
	else
		{
		INFO_PRINTF1(_L("This is negative test (KErrNotSupported) for stopping and restarting DirectScreenAccess when using CVideoPlayerUtility2"));
		}
	
	return verdict;
	}

TVerdict CTestStepVidPlayerStopStartDSA::DoTestStepPostambleL()
	{
		if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif

	DeInitWserv();

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerStopStartDSA::Close()
	{
	CTestStepUnitMMFVidClient::Close();
	}
	
TVerdict CTestStepVidPlayerStopStartDSA::DoTestStepL()
	{
	TInt err = KErrNone;
	TRect rect, clipRect;
	
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, 
											EMdaPriorityPreferenceTimeAndQuality,
											iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal,
											EMdaPriorityPreferenceTimeAndQuality);
		}
#endif		

	if ( !PlayerStateStopped() )
		{
		return EInconclusive;
		}
	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		{
		return EInconclusive;
		}
	if (!iVpu2)
		{
		TRAP(err, iPlayer->StopDirectScreenAccessL());
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		TRAP(err, iPlayer2->StopDirectScreenAccessL());
		}
#endif			
	if (!iVpu2 && err != KErrNone)
		{
		INFO_PRINTF2(_L("Failed to stop DirectScreenAccess, %d"), err);
		return EFail;
		}
	if (iVpu2 && err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Expected KErrNotSuported. Got %d"), err);
		return EFail;
		}
	
	if (!iVpu2)
		{
		TRAP(err, iPlayer->StartDirectScreenAccessL());
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		TRAP(err, iPlayer2->StartDirectScreenAccessL());
		}
#endif		
	if (!iVpu2 && err == KErrNone)
		{
		INFO_PRINTF1(_L("Restarted DirectScreenAccess"));		
		return EPass;
		}
	if (iVpu2 && err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Expected KErrNotSuported. Got %d"), err);
		return EFail;
		}
	if (!iVpu2 && err != KErrNone)
		{
		INFO_PRINTF2(_L("Failed to restart DirectScreenAccess, %d"), err);
		iTestStepResult = EFail;
		}

	return iTestStepResult;
	}

// from MVideoPlayerUtilityObserver
void CTestStepVidPlayerStopStartDSA::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepVidPlayerStopStartDSA::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerStopStartDSA::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
	
void CTestStepVidPlayerStopStartDSA::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

	
void CTestStepVidPlayerStopStartDSA::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGetFrame.
 *
 *
 * @return	"CTestStepVidPlayerGetFrame*"
 *			The constructed CTestStepVidPlayerGetFrame
 */
CTestStepVidPlayerGetFrame* CTestStepVidPlayerGetFrame::NewL(TBool aUseIntentAPI)
	{
	CTestStepVidPlayerGetFrame* self = new(ELeave) CTestStepVidPlayerGetFrame(aUseIntentAPI);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGetFrame::CTestStepVidPlayerGetFrame(TBool aUseIntentAPI) :
	iUseIntentAPI(aUseIntentAPI)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	if (!iUseIntentAPI)
		iTestStepName = _L("MM-MMF-VCLNT-U-0027-CP");
	else
		iTestStepName = _L("MM-MMF-VCLNT-U-0071-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGetFrame::~CTestStepVidPlayerGetFrame()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGetFrame::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerGetFrame::MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError)
	{
	if (aError!=KErrNone)
		iError2 = aError;
	else
		{
		if (aFrame.Handle() == NULL || aFrame.SizeInPixels() != TSize(KFrameWidth,KFrameHeight) )
			iError2 = KErrArgument;
		else
			iError2 = KErrNone;
		}
	CActiveScheduler::Stop();
	}

void CTestStepVidPlayerGetFrame::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerGetFrame::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	

void CTestStepVidPlayerGetFrame::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGetFrame::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets a frame with CVideoPlayerUtility::GetFrameL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGetFrame::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGetFrame::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;
	iError = KErrTimedOut;		// Set error to something other than KErrNone
	iError2 = KErrTimedOut;		// Set error to something other than KErrNone

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer || !PlayerStateStopped())
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if ( err != KErrNone || iError != KErrNone) 
		{
		INFO_PRINTF2(_L("Error calling OpenAndStartSchedulerL %d"), iError);
		return EInconclusive;
		}
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if ( err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	// Call GetFrameL
	if (iUseIntentAPI)
		{
		TRAP(err, iPlayer->GetFrameL(EColor16M, EPeek) );
		}
	else 
		{
		TRAP(err, iPlayer->GetFrameL(EColor16M) );
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error calling GetFrameL %d"), err);
		return EFail;
		}

	// Set error to something other than KErrNone
	iError2 = KErrTimedOut;
	CActiveScheduler::Start();

	if (iError2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error in MvpuoFrameReady callback %d, or MvpuoFrameReady not called"), iError2);
		return EFail;
		}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGetFrameRate.
 *
 *
 * @return	"CTestStepVidPlayerGetFrameRate*"
 *			The constructed CTestStepVidPlayerGetFrameRate
 */
CTestStepVidPlayerGetFrameRate* CTestStepVidPlayerGetFrameRate::NewL()
	{
	CTestStepVidPlayerGetFrameRate* self = new(ELeave) CTestStepVidPlayerGetFrameRate;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGetFrameRate::CTestStepVidPlayerGetFrameRate() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0028-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGetFrameRate::~CTestStepVidPlayerGetFrameRate()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGetFrameRate::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerGetFrameRate::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerGetFrameRate::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerGetFrameRate::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerGetFrameRate::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGetFrameRate::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test get the frame rate with CVideoPlayerUtility::VideoFrameRateL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGetFrameRate::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGetFrameRate::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;
	TReal32 frameRate;
	TReal32 receivedFrameRate=0;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if (err != KErrNone) 
		return EFail;
		
	
	frameRate = 32;
	
	TRAP(err, iPlayer->SetVideoFrameRateL(frameRate) );
	if (err!=KErrNone)
		return EFail;
		
	TRAP(err, receivedFrameRate = iPlayer->VideoFrameRateL() );

	if (err != KErrNone ||
		frameRate != receivedFrameRate )  // or check with a known frame rate clip when there is a video controller and decoder available
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGetVFrameSize.
 *
 *
 * @return	"CTestStepVidPlayerGetVFrameSize*"
 *			The constructed CTestStepVidPlayerGetVFrameSize
 */
CTestStepVidPlayerGetVFrameSize* CTestStepVidPlayerGetVFrameSize::NewL()
	{
	CTestStepVidPlayerGetVFrameSize* self = new(ELeave) CTestStepVidPlayerGetVFrameSize;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGetVFrameSize::CTestStepVidPlayerGetVFrameSize() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0029-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGetVFrameSize::~CTestStepVidPlayerGetVFrameSize()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGetVFrameSize::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerGetVFrameSize::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerGetVFrameSize::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerGetVFrameSize::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerGetVFrameSize::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGetVFrameSize::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets the video frame size with CVideoPlayerUtility::VideoFrameSizeL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGetVFrameSize::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGetVFrameSize::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;
	TSize frameSize;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;


	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, iPlayer->VideoFrameSizeL(frameSize) );

	if (err != KErrNone ||
		frameSize.iWidth < 0 ||
		frameSize.iHeight < 0 )  // check with a known frame size clip when there is a video controller and decoder available
		return EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGetVBitRate.
 *
 *
 * @return	"CTestStepVidPlayerGetVBitRate*"
 *			The constructed CTestStepVidPlayerGetVBitRate
 */
CTestStepVidPlayerGetVBitRate* CTestStepVidPlayerGetVBitRate::NewL()
	{
	CTestStepVidPlayerGetVBitRate* self = new(ELeave) CTestStepVidPlayerGetVBitRate;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGetVBitRate::CTestStepVidPlayerGetVBitRate() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0030-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGetVBitRate::~CTestStepVidPlayerGetVBitRate()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGetVBitRate::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerGetVBitRate::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerGetVBitRate::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerGetVBitRate::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerGetVBitRate::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGetVBitRate::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets the video bit rate with CVideoPlayerUtility::VideoBitRateL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGetVBitRate::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGetVBitRate::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt bitRate = 0; 

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;


	TRAP(err, OpenAndStartSchedulerL() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, bitRate = iPlayer->VideoBitRateL() );

	if (err != KErrNone ||
		bitRate < 0 )  // check with a known frame size clip when there is a video controller and decoder available
		return EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGetABitRate.
 *
 *
 * @return	"CTestStepVidPlayerGetABitRate*"
 *			The constructed CTestStepVidPlayerGetABitRate
 */
CTestStepVidPlayerGetABitRate* CTestStepVidPlayerGetABitRate::NewL()
	{
	CTestStepVidPlayerGetABitRate* self = new(ELeave) CTestStepVidPlayerGetABitRate;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGetABitRate::CTestStepVidPlayerGetABitRate() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0031-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGetABitRate::~CTestStepVidPlayerGetABitRate()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGetABitRate::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerGetABitRate::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerGetABitRate::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerGetABitRate::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerGetABitRate::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGetABitRate::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets the audio bit rate with CVideoPlayerUtility::AudioBitRateL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGetABitRate::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGetABitRate::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt bitRate = 0; 

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EInconclusive;

	if (!PlayerStateStopped())
		 return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, bitRate = iPlayer->AudioBitRateL() );

	if (err != KErrNone ||
		bitRate < 0 )  // check with a known frame size clip when there is a video controller and decoder available
		return EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerAudEnabled.
 *
 *
 * @return	"CTestStepVidPlayerAudEnabled*"
 *			The constructed CTestStepVidPlayerAudEnabled
 */
CTestStepVidPlayerAudEnabled* CTestStepVidPlayerAudEnabled::NewL()
	{
	CTestStepVidPlayerAudEnabled* self = new(ELeave) CTestStepVidPlayerAudEnabled;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerAudEnabled::CTestStepVidPlayerAudEnabled() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0032-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerAudEnabled::~CTestStepVidPlayerAudEnabled()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerAudEnabled::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerAudEnabled::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerAudEnabled::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerAudEnabled::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}


void CTestStepVidPlayerAudEnabled::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerAudEnabled::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test checks if the audio is enabled with CVideoPlayerUtility::AudioEnabledL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerAudEnabled::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerAudEnabled::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
//	TBool audEnabled;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


//	TRAP(err, audEnabled = iPlayer->AudioEnabledL() );
	TRAP(err, iPlayer->AudioEnabledL() );	// EABI warning removal

	if (err != KErrNone )
		return EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerClipPosition.
 *
 *
 * @return	"CTestStepVidPlayerClipPosition*"
 *			The constructed CTestStepVidPlayerClipPosition
 */
CTestStepVidPlayerClipPosition* CTestStepVidPlayerClipPosition::NewL()
	{
	CTestStepVidPlayerClipPosition* self = new(ELeave) CTestStepVidPlayerClipPosition;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerClipPosition::CTestStepVidPlayerClipPosition() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0033-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerClipPosition::~CTestStepVidPlayerClipPosition()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerClipPosition::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerClipPosition::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerClipPosition::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerClipPosition::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerClipPosition::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerClipPosition::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test sets the position in a video clip with CVideoPlayerUtility::SetPositionL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerClipPosition::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerClipPosition::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds pos = TTimeIntervalMicroSeconds(500000);
	TTimeIntervalMicroSeconds refPos;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, iPlayer->SetPositionL(pos) );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EInconclusive;
	else 
		return EPass;
#endif

	if (err != KErrNone )
		return EFail;

	TRAP(err, refPos = iPlayer->PositionL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported || refPos != pos)
		return EFail;
	else
		return EPass;
#endif
	
	if (err != KErrNone || pos != refPos)
		return EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerDuration.
 *
 *
 * @return	"CTestStepVidPlayerDuration*"
 *			The constructed CTestStepVidPlayerDuration
 */
CTestStepVidPlayerDuration* CTestStepVidPlayerDuration::NewL()
	{
	CTestStepVidPlayerDuration* self = new(ELeave) CTestStepVidPlayerDuration;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerDuration::CTestStepVidPlayerDuration() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0035-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerDuration::~CTestStepVidPlayerDuration()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerDuration::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerDuration::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerDuration::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerDuration::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerDuration::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerDuration::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets the duration of a video clip with CVideoPlayerUtility::DurationL()"));
	
	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerDuration::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerDuration::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds dur;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EInconclusive;

	if (!PlayerStateStopped())
		 return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


	TRAP(err, dur = iPlayer->DurationL() );

	if (err != KErrNone)
		return EFail;

	// check a known duration when codecs are available

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerVideoType.
 *
 *
 * @return	"CTestStepVidPlayerVideoType*"
 *			The constructed CTestStepVidPlayerVideoType
 */
CTestStepVidPlayerVideoType* CTestStepVidPlayerVideoType::NewL()
	{
	CTestStepVidPlayerVideoType* self = new(ELeave) CTestStepVidPlayerVideoType;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerVideoType::CTestStepVidPlayerVideoType() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0036-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerVideoType::~CTestStepVidPlayerVideoType()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerVideoType::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerVideoType::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerVideoType::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerVideoType::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerVideoType::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerVideoType::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets the video type of a video clip with CVideoPlayerUtility::VideoFormatMimeType()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerVideoType::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerVideoType::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TBuf8<256> type;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EInconclusive;

	if (!PlayerStateStopped())
		 return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


	TRAP(err, type = iPlayer->VideoFormatMimeType() );

	if (err != KErrNone)
		return EFail;

	// check a known type when codecs are available

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerAudioType.
 *
 *
 * @return	"CTestStepVidPlayerAudioType*"
 *			The constructed CTestStepVidPlayerAudioType
 */
CTestStepVidPlayerAudioType* CTestStepVidPlayerAudioType::NewL()
	{
	CTestStepVidPlayerAudioType* self = new(ELeave) CTestStepVidPlayerAudioType;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerAudioType::CTestStepVidPlayerAudioType() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0037-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerAudioType::~CTestStepVidPlayerAudioType()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerAudioType::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerAudioType::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerAudioType::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerAudioType::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerAudioType::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerAudioType::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test gets the audio type of a video clip with CVideoPlayerUtility::AudioTypeL()"));
	
	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerAudioType::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerAudioType::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TFourCC type;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, type = iPlayer->AudioTypeL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
	else 
		return EPass;
#else

	if (err != KErrNone)
		return EFail;
#endif

	// check a known audio type when codecs are available

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerVol.
 *
 *
 * @return	"CTestStepVidPlayerVol*"
 *			The constructed CTestStepVidPlayerVol
 */
CTestStepVidPlayerVol* CTestStepVidPlayerVol::NewL()
	{
	CTestStepVidPlayerVol* self = new(ELeave) CTestStepVidPlayerVol;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerVol::CTestStepVidPlayerVol() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0038-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerVol::~CTestStepVidPlayerVol()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerVol::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerVol::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerVol::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerVol::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerVol::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerVol::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this tests the volue functions with CVideoPlayerUtility::[Max|Set|]VolumeL()"));


	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerVol::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerVol::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TInt vol = 0;
	TInt refVol = 0;

	TRAP(err, vol = iPlayer->MaxVolume() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#else
	if (err != KErrNone ||
		vol < 0)
		return EFail;
#endif

	TRAP(err, iPlayer->SetVolumeL(vol/2) );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#else
	if (err != KErrNone)
		return EFail;
#endif

	TRAP(err, refVol = iPlayer->Volume() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported || refVol != vol/2)
		return EFail;
	else 
		return EPass;
#else
	if (err != KErrNone ||
		refVol != vol/2 )
		return EFail;
#endif



	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerBalance.
 *
 *
 * @return	"CTestStepVidPlayerBalance*"
 *			The constructed CTestStepVidPlayerBalance
 */
CTestStepVidPlayerBalance* CTestStepVidPlayerBalance::NewL()
	{
	CTestStepVidPlayerBalance* self = new(ELeave) CTestStepVidPlayerBalance;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerBalance::CTestStepVidPlayerBalance() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0041-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerBalance::~CTestStepVidPlayerBalance()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerBalance::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerBalance::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerBalance::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerBalance::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerBalance::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerBalance::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this tests the balance functions with CVideoPlayerUtility::[Get|Set]Balance()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerBalance::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerBalance::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt refBal = 0; //KMMFBalanceCentre

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EInconclusive;

	if (!PlayerStateStopped())
		 return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


	// test 1
	TRAP(err, iPlayer->SetBalanceL(refBal) ); 

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone)
		return EFail;
#endif

	TRAP(err, refBal = iPlayer->Balance() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported || refBal != 0)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone )
		// refBal !=  0)
		return EFail;
#endif

	// test 2
	TRAP(err, iPlayer->SetBalanceL(-100) ); 

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone)
		return EFail;
#endif

	TRAP(err, refBal = iPlayer->Balance() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported || refBal != -100)
		return EFail;
	else 
		return EPass;
#endif

	if (err != KErrNone )
		// refVBal != -100 )
		return EFail;


	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerMeta.
 *
 *
 * @return	"CTestStepVidPlayerMeta*"
 *			The constructed CTestStepVidPlayerMeta
 */
CTestStepVidPlayerMeta* CTestStepVidPlayerMeta::NewL()
	{
	CTestStepVidPlayerMeta* self = new(ELeave) CTestStepVidPlayerMeta;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerMeta::CTestStepVidPlayerMeta() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0043-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerMeta::~CTestStepVidPlayerMeta()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerMeta::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerMeta::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerMeta::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerMeta::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerMeta::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerMeta::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this tests the metadata functions with CVideoPlayerUtility::[NumberOf]MetaDataEntr[y|ies]L()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerMeta::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerMeta::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
//	TInt numOfMeta;
//	CMMFMetaDataEntry* meta;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


//	TRAP(err, numOfMeta = iPlayer->NumberOfMetaDataEntriesL() );
	TRAP(err, iPlayer->NumberOfMetaDataEntriesL() );	// EABI warning removal

	if (err != KErrNone)
		return EFail;

	// call that goes to the dummy controller
	TRAP(err, iPlayer->MetaDataEntryL(1) )

	if (err != KErrNone)
		return EFail;

	// awaits video codec implementation
	//for (TInt i=0; i<numOfMeta; i++)
	//	{
	//	TRAP(err, meta = iPlayer->MetaDataEntryL(i) );
	//	if (err != KErrNone ||
	//		meta != NULL)
	//		return EFail;
	//	}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerPlayWindow.
 *
 *
 * @return	"CTestStepVidPlayerPlayWindow*"
 *			The constructed CTestStepVidPlayerPlayWindow
 */
CTestStepVidPlayerPlayWindow* CTestStepVidPlayerPlayWindow::NewL()
	{
	CTestStepVidPlayerPlayWindow* self = new(ELeave) CTestStepVidPlayerPlayWindow;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerPlayWindow::CTestStepVidPlayerPlayWindow() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0060-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerPlayWindow::~CTestStepVidPlayerPlayWindow()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerPlayWindow::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerPlayWindow::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerPlayWindow::MvpuoPlayComplete(TInt aError)
	{
	iHasPlayed = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerPlayWindow::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerPlayWindow::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerPlayWindow::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing a video with a CVideoPlayerUtility::Play(StartTime,EndTime)"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerPlayWindow::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerPlayWindow::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !PlayerStateStopped() )
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
	
		
	iPlayer->Play(TTimeIntervalMicroSeconds(KPlayStartTime),
				  TTimeIntervalMicroSeconds(KPlayEndTime));
	CActiveScheduler::Start();
	if (iError == KErrNone && iHasPlayed )
		return EPass;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGetImplementationInfo.
 *
 *
 * @return	"CTestStepVidPlayerGetImplementationInfo*"
 *			The constructed CTestStepVidPlayerGetImplementationInfo
 */
CTestStepVidPlayerGetImplementationInfo* CTestStepVidPlayerGetImplementationInfo::NewL()
	{
	CTestStepVidPlayerGetImplementationInfo* self = new(ELeave) CTestStepVidPlayerGetImplementationInfo;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGetImplementationInfo::CTestStepVidPlayerGetImplementationInfo() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0061-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGetImplementationInfo::~CTestStepVidPlayerGetImplementationInfo()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGetImplementationInfo::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerGetImplementationInfo::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerGetImplementationInfo::MvpuoPlayComplete(TInt aError)
	{
	iHasPlayed = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerGetImplementationInfo::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerGetImplementationInfo::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGetImplementationInfo::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing a video with a CVideoPlayerUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGetImplementationInfo::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGetImplementationInfo::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !PlayerStateStopped() )
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iPlayer->ControllerImplementationInformationL());
	if (err != KErrNone)
		return EFail;

	if (info->DisplayName().Compare(KVideoTestControllerName)==0)
		{
		const RMMFFormatImplInfoArray& playFormats = info->PlayFormats();
		INFO_PRINTF1(_L("Play Format Support"));
		
		TBuf<256> ext;
		TBuf<256> mime;
		TInt i,j;
		if (playFormats.Count() >0 )
			{
			for (i = 0;i<playFormats.Count();i++)
				{
				INFO_PRINTF2(_L("supported format %d: "),i);
				const CDesC8Array& extensions = playFormats[i]->SupportedFileExtensions();
				for (j = 0;j < extensions.Count(); j++)
					{
					
					ext.Copy(extensions[j]);
					INFO_PRINTF2(_L("extension: %S"), &ext);
					}
				const CDesC8Array& mimeTypes = playFormats[i]->SupportedMimeTypes();
				for (j = 0;j < mimeTypes.Count(); j++)
					{
					mime.Copy(mimeTypes[j]);
					INFO_PRINTF2(_L("mime type: %S"), &mime);
					}
				
				}
				
						
			
			}
		else
			return EFail;
			
			
		const RMMFFormatImplInfoArray& recordFormats = info->RecordFormats();
		INFO_PRINTF1(_L("Record Format Support"));
		
		if (recordFormats.Count() >0 )
			{
			TInt i,j;
			for (i = 0;i<recordFormats.Count();i++)
				{
				INFO_PRINTF2(_L("supported format %d: "),i);
				const CDesC8Array& extensions = recordFormats[i]->SupportedFileExtensions();
				for (j = 0;j < extensions.Count(); j++)
					{
					ext.Copy(extensions[j]);
					INFO_PRINTF2(_L("extension: %S"), &ext);
					}
				const CDesC8Array& mimeTypes = recordFormats[i]->SupportedMimeTypes();
				for (j = 0;j < mimeTypes.Count(); j++)
					{
					mime.Copy(mimeTypes[j]);
					INFO_PRINTF2(_L("mime type: %S"), &mime);
					}
				
				}
			}
		else
			return EFail;

		
		iTestStepResult = EPass;
		
		}
	else
		iTestStepResult = EFail;
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerGeneralEvent.
 *
 *
 * @return	"CTestStepVidPlayerGeneralEvent*"
 *			The constructed CTestStepVidPlayerGeneralEvent
 */
CTestStepVidPlayerGeneralEvent* CTestStepVidPlayerGeneralEvent::NewL()
	{
	CTestStepVidPlayerGeneralEvent* self = new(ELeave) CTestStepVidPlayerGeneralEvent;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerGeneralEvent::CTestStepVidPlayerGeneralEvent() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0064-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerGeneralEvent::~CTestStepVidPlayerGeneralEvent()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerGeneralEvent::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerGeneralEvent::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerGeneralEvent::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerGeneralEvent::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerGeneralEvent::MvpuoEvent(const TMMFEvent& aEvent)
	{
	iEventReceived = ETrue;
	if (aEvent.iEventType == KMMFEventCategoryVideoPlayerGeneralError &&
		aEvent.iErrorCode == KTestGeneralError)
		iCorrectEventReceived = ETrue;
	else
		iCorrectEventReceived = EFalse;
	CActiveScheduler::Stop(); 
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerGeneralEvent::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is of receiving a general event from a controller"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerGeneralEvent::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerGeneralEvent::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	TRAP(err, OpenAndStartSchedulerL() );

	if (err!=KErrNone)
		return EInconclusive;
		
	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandRequestGeneralEvent,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	iTestStepResult = EFail;
	
	if (iEventReceived)
		{
		if (iCorrectEventReceived)
			iTestStepResult = EPass;
		else
			INFO_PRINTF1(_L("incorrect event received"));
		}
	else
		INFO_PRINTF1(_L("incorrect callback received"));
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerSendCustomCommandAsync.
 *
 *
 * @return	"CTestStepVidPlayerSendCustomCommandAsync*"
 *			The constructed CTestStepVidPlayerSendCustomCommandAsync
 */
CTestStepVidPlayerSendCustomCommandAsync* CTestStepVidPlayerSendCustomCommandAsync::NewL()
	{
	CTestStepVidPlayerSendCustomCommandAsync* self = new(ELeave) CTestStepVidPlayerSendCustomCommandAsync;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerSendCustomCommandAsync::CTestStepVidPlayerSendCustomCommandAsync() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0063-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerSendCustomCommandAsync::~CTestStepVidPlayerSendCustomCommandAsync()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSendCustomCommandAsync::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerSendCustomCommandAsync::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerSendCustomCommandAsync::MvpuoPlayComplete(TInt aError)
	{
	iHasPlayed = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerSendCustomCommandAsync::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerSendCustomCommandAsync::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerSendCustomCommandAsync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;
	
	iFileName.Set(KClockFileName);
		
	iAsyncObserver = new (ELeave) CAsyncObserver;

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CVideoPlayerUtility::CustomCommandAsync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerSendCustomCommandAsync::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	delete iAsyncObserver;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerSendCustomCommandAsync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);


	TRAP(err, OpenAndStartSchedulerL() );
	
	if (err!=KErrNone)
		return EInconclusive;

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	iDestinationPckg = TMMFMessageDestinationPckg(uid);
	
	// test without a return value
	iPlayer->CustomCommandAsync(iDestinationPckg,
								EMMFTestCustomCommandGeneralAsynchronous,
								KFirstParameter,
								KSecondParameter,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
								
	TBuf8<256> buf;
		
	iPlayer->CustomCommandAsync(iDestinationPckg,
								EMMFTestCustomCommandGeneralAsynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerSendCustomCommandSync.
 *
 *
 * @return	"CTestStepVidPlayerSendCustomCommandSync*"
 *			The constructed CTestStepVidPlayerSendCustomCommandSync
 */
CTestStepVidPlayerSendCustomCommandSync* CTestStepVidPlayerSendCustomCommandSync::NewL()
	{
	CTestStepVidPlayerSendCustomCommandSync* self = new(ELeave) CTestStepVidPlayerSendCustomCommandSync;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerSendCustomCommandSync::CTestStepVidPlayerSendCustomCommandSync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0062-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerSendCustomCommandSync::~CTestStepVidPlayerSendCustomCommandSync()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSendCustomCommandSync::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerSendCustomCommandSync::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}
	
void CTestStepVidPlayerSendCustomCommandSync::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerSendCustomCommandSync::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerSendCustomCommandSync::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerSendCustomCommandSync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CVideoPlayerUtility::CustomCommandSync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerSendCustomCommandSync::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerSendCustomCommandSync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	TRAP(err, OpenAndStartSchedulerL() );

	if (err!=KErrNone)
		return EInconclusive;

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronous,
								KFirstParameter,
								KSecondParameter);
								
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	TBuf8<256> buf;
		
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf);
	
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerReloadingTest.
 *
 *
 * @return	"CTestStepVidPlayerReloadingTest*"
 *			The constructed CTestStepVidPlayerReloadingTest
 */
CTestStepVidPlayerReloadingTest* CTestStepVidPlayerReloadingTest::NewL()
	{
	CTestStepVidPlayerReloadingTest* self = new(ELeave) CTestStepVidPlayerReloadingTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerReloadingTest::CTestStepVidPlayerReloadingTest()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0065-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerReloadingTest::~CTestStepVidPlayerReloadingTest()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerReloadingTest::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerReloadingTest::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}
	
void CTestStepVidPlayerReloadingTest::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}


void CTestStepVidPlayerReloadingTest::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerReloadingTest::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}
	
	
void CTestStepVidPlayerReloadingTest::MvloLoadingStarted()
	{
	iLoadingStarted = ETrue;
	CActiveScheduler::Stop();
	}
	
void CTestStepVidPlayerReloadingTest::MvloLoadingComplete()
	{
	iLoadingComplete = ETrue;
	CActiveScheduler::Stop();
	}




/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerReloadingTest::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test simulates a video clip being reloaded/rebuffered"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerReloadingTest::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerReloadingTest::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	TRAP(err, OpenAndStartSchedulerL() );

	if (err!=KErrNone)
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

		
	iPlayer->RegisterForVideoLoadingNotification(*this);

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateReloading,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	
	if (!iLoadingStarted)
		return EFail;
		
	TInt percentageComplete = 0;
	TInt oldPercentageComplete =0;
	while (percentageComplete!=100 && percentageComplete>=oldPercentageComplete)
		{
		oldPercentageComplete = percentageComplete;
		iPlayer->GetVideoLoadingProgressL(percentageComplete);
		INFO_PRINTF2(_L("Loading progress: %d"),percentageComplete);
		User::After(100);
		}


	CActiveScheduler::Start();
		
	if (iLoadingComplete && percentageComplete == 100)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerRefreshFrame.
 *
 *
 * @return	"CTestStepVidPlayerRefreshFrame*"
 *			The constructed CTestStepVidPlayerRefreshFrame
 */
CTestStepVidPlayerRefreshFrame* CTestStepVidPlayerRefreshFrame::NewL()
	{
	CTestStepVidPlayerRefreshFrame* self = new(ELeave) CTestStepVidPlayerRefreshFrame;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerRefreshFrame::CTestStepVidPlayerRefreshFrame() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0066-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerRefreshFrame::~CTestStepVidPlayerRefreshFrame()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerRefreshFrame::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerRefreshFrame::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerRefreshFrame::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerRefreshFrame::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerRefreshFrame::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerRefreshFrame::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this requests the controller to redraw the current frame with CVideoPlayerUtility::RefreshFrameL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerRefreshFrame::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerRefreshFrame::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
	
	TRAP(err,  iPlayer->RefreshFrameL() );

	if (err != KErrNone)   // or check with a known frame rate clip when there is a video controller and decoder available
		return EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 * Static constructor for CTestStepVidPlayerRotation.
 *
 * @return	"CTestStepVidPlayerRotation*"
 *			The constructed CTestStepVidPlayerRotation
 */
CTestStepVidPlayerRotation* CTestStepVidPlayerRotation::NewL()
	{
	CTestStepVidPlayerRotation* self = new(ELeave) CTestStepVidPlayerRotation;
	return self;
	}

/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerRotation::CTestStepVidPlayerRotation() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	
	iTestStepName = _L("MM-MMF-VCLNT-U-0067-HP");
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerRotation::~CTestStepVidPlayerRotation()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerRotation::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerRotation::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerRotation::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerRotation::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerRotation::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerRotation::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this tests CVideoPlayerUtility::SetRotationL() and CVideoPlayerUtility::RotationL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerRotation::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerRotation::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TInt rotation = EVideoRotationNone;

#ifdef SYMBIAN_BUILD_GCE
	SurfaceSupported();
#endif
	if (iSurfaceSupported)
		{
		INFO_PRINTF1(_L("The controller supports the graphics surfaces"));
		}

    // EVideoRotationNone		
	TRAP(err, iPlayer->SetRotationL(EVideoRotationNone));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, rotation = iPlayer->RotationL());
	
	if (err!=KErrNone || rotation != EVideoRotationNone)
		return EFail;
		
    // EVideoRotationClockwise90		
	TRAP(err, iPlayer->SetRotationL(EVideoRotationClockwise90));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, rotation = iPlayer->RotationL());
	
	if (err!=KErrNone || rotation != EVideoRotationClockwise90)
		return EFail;

    // EVideoRotationClockwise180		
	TRAP(err, iPlayer->SetRotationL(EVideoRotationClockwise180));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, rotation = iPlayer->RotationL());
	
	if (err!=KErrNone || rotation != EVideoRotationClockwise180)
		return EFail;

    // EVideoRotationClockwise270		
	TRAP(err, iPlayer->SetRotationL(EVideoRotationClockwise270));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, rotation = iPlayer->RotationL());
	
	if (err!=KErrNone || rotation != EVideoRotationClockwise270)
		return EFail;
	
#ifdef SYMBIAN_BUILD_GCE
	SurfaceSupported();
#endif	
	if (!iSurfaceSupported)
		{
		iTestStepResult = ControllerCalled(_L8("MvpcGetRotationL Called"));
		}
	else 
		{
		iTestStepResult = ControllerCalled(_L8("MvpcGetRotationL Called"), EFalse);
		}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerScale.
 *
 *
 * @return	"CTestStepVidPlayerScale*"
 *			The constructed CTestStepVidPlayerScale
 */
CTestStepVidPlayerScale* CTestStepVidPlayerScale::NewL()
	{
	CTestStepVidPlayerScale* self = new(ELeave) CTestStepVidPlayerScale;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerScale::CTestStepVidPlayerScale() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0068-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerScale::~CTestStepVidPlayerScale()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerScale::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerScale::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerScale::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerScale::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerScale::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerScale::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this tests CVideoPlayerUtility::SetScaleFactorL() and CVideoPlayerUtility::GetScaleFactor"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerScale::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerScale::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
#ifdef SYMBIAN_BUILD_GCE
	SurfaceSupported();
#endif
		
	if (iSurfaceSupported)
		{
		INFO_PRINTF1(_L("The controller supports the graphics surfaces"));
		}
	
	TReal32 widthScaleFactor = 0.0f;
	TReal32 heightScaleFactor = 0.0f;
	TBool antiAliasFilter = EFalse;


	TRAP(err, iPlayer->SetScaleFactorL(KScaleWidth1, KScaleHeight1, ETrue));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, iPlayer->GetScaleFactorL(widthScaleFactor,heightScaleFactor,antiAliasFilter));
	
	if (err!=KErrNone || 
		widthScaleFactor != KScaleWidth1 ||
		heightScaleFactor != KScaleHeight1 ||
		!antiAliasFilter)
		return EFail;

	TRAP(err, iPlayer->SetScaleFactorL(KScaleWidth2, KScaleHeight2, EFalse));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, iPlayer->GetScaleFactorL(widthScaleFactor,heightScaleFactor,antiAliasFilter));
	
	if (err!=KErrNone || 
		widthScaleFactor != KScaleWidth2 ||
		heightScaleFactor != KScaleHeight2 ||
		antiAliasFilter)
		return EFail;

#ifdef SYMBIAN_BUILD_GCE
	SurfaceSupported();
#endif		
	if (!iSurfaceSupported)
		{
		iTestStepResult = ControllerCalled(_L8("MvpcGetScaleFactorL Called"));
		}
	else 
		{
		iTestStepResult = ControllerCalled(_L8("MvpcGetScaleFactorL Called"), EFalse);
		}
		
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerCropRegion.
 *
 *
 * @return	"CTestStepVidPlayerCropRegion*"
 *			The constructed CTestStepVidPlayerCropRegion
 */
CTestStepVidPlayerCropRegion* CTestStepVidPlayerCropRegion::NewL()
	{
	CTestStepVidPlayerCropRegion* self = new(ELeave) CTestStepVidPlayerCropRegion;
	return self;
	}

/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerCropRegion::CTestStepVidPlayerCropRegion() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0069-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerCropRegion::~CTestStepVidPlayerCropRegion()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerCropRegion::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerCropRegion::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerCropRegion::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerCropRegion::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerCropRegion::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerCropRegion::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this tests CVideoPlayerUtility::SetCropRegionL() and CVideoPlayerUtility::GetCropRegionL()"));
	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerCropRegion::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerCropRegion::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;
	err = KErrNone;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer)
		return EFail;

	if (!PlayerStateStopped())
		 return EFail;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
	
	TRect cropRegion(KCropRegionRectLeft, KCropRegionRectTop, KCropRegionRectRight, KCropRegionRectBottom);
	TRect region;

#ifdef SYMBIAN_BUILD_GCE
	SurfaceSupported();
#endif		
	if (iSurfaceSupported)
		{
		INFO_PRINTF1(_L("The controller supports the graphics surfaces"));
		}

    // Crop Region
	TRAP(err, iPlayer->SetCropRegionL(cropRegion));
	
	if (err!=KErrNone)
		return EFail;
		
				
	TRAP(err, iPlayer->GetCropRegionL(region));
	
	if (err!=KErrNone || cropRegion != region)
		return EFail;
	
#ifdef SYMBIAN_BUILD_GCE
	SurfaceSupported();
#endif
	
	if (!iSurfaceSupported)
		{
		iTestStepResult = ControllerCalled(_L8("MvpcGetCropRegionL Called"));
		}
	else 
		{
		iTestStepResult = ControllerCalled(_L8("MvpcGetCropRegionL Called"), EFalse);
		}
			
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	

//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerPlayPause.
 *
 *
 * @return	"CTestStepVidPlayerPlayPause*"
 *			The constructed CTestStepVidPlayerPlayPause
 */
CTestStepVidPlayerPlayPause* CTestStepVidPlayerPlayPause::NewL()
	{
	CTestStepVidPlayerPlayPause* self = new(ELeave) CTestStepVidPlayerPlayPause;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerPlayPause::CTestStepVidPlayerPlayPause() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0070-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerPlayPause::~CTestStepVidPlayerPlayPause()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerPlayPause::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerPlayPause::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerPlayPause::MvpuoPlayComplete(TInt aError)
	{
	iHasPlayed = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerPlayPause::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerPlayPause::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerPlayPause::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing and then pausing a video & waiting for a MvpuoPlayComplete() callback"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerPlayPause::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerPlayPause::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !PlayerStateStopped() )
		return EInconclusive;

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
	
		
	// play & then pause the controller...
	// because the test controller effectively ignores pause
	// we should get a play complete callback anyway...
	iPlayer->Play(TTimeIntervalMicroSeconds(KPlayStartTime),
				  TTimeIntervalMicroSeconds(KPlayEndTime));
	iPlayer->PauseL();

	CActiveScheduler::Start();
	if (iError == KErrNone && iHasPlayed )
		return EPass;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerCheckImplementationInfo.
 *
 *
 * @return	"CTestStepVidPlayerCheckImplementationInfo*"
 *			The constructed CTestStepVidPlayerCheckImplementationInfo
 */
CTestStepVidPlayerCheckImplementationInfo* CTestStepVidPlayerCheckImplementationInfo::NewL()
	{
	CTestStepVidPlayerCheckImplementationInfo* self = new(ELeave) CTestStepVidPlayerCheckImplementationInfo;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerCheckImplementationInfo::CTestStepVidPlayerCheckImplementationInfo() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0254-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerCheckImplementationInfo::~CTestStepVidPlayerCheckImplementationInfo()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerCheckImplementationInfo::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidPlayerCheckImplementationInfo::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerCheckImplementationInfo::MvpuoPlayComplete(TInt aError)
	{
	iHasPlayed = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerCheckImplementationInfo::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerCheckImplementationInfo::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
TVerdict CTestStepVidPlayerCheckImplementationInfo::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	
	if (verdict != EPass)
		{
		return verdict;	
		}

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing a video with a CVideoPlayerUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
TVerdict CTestStepVidPlayerCheckImplementationInfo::DoTestStepPostambleL(void)
	{
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerCheckImplementationInfo::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EFail;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if ( !PlayerStateStopped() )
		{
		return EInconclusive;	
		}

	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone) 
		{
		return EInconclusive;	
		}
			
	TRAP(err, PlayerPrepareAndStartScheduler() );
	
	if (err != KErrNone || iError != KErrNone) 
		{
		return EInconclusive;	
		}

	TRAP(err,  &iPlayer->ControllerImplementationInformationL());
	if (err != KErrNone)
		{
		return EFail;	
		}
	
	iPlayer->Close();
	
	TRAP(err,  &iPlayer->ControllerImplementationInformationL());
	
	// Checking for KErrNotReady because we dont have one more videocontroller 
	// to compare
	if (err == KErrNotReady  )
		{
		iTestStepResult = EPass;			
		}
	else
		{
		iTestStepResult = EFail;	
		}
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//While video is playing, interrupt audio component. 
//Video should stop completely
CTestStepVideoInterruptionByAudio* CTestStepVideoInterruptionByAudio::NewL()
	{
	CTestStepVideoInterruptionByAudio* self = new(ELeave) CTestStepVideoInterruptionByAudio();
	return self;
	}

CTestStepVideoInterruptionByAudio::CTestStepVideoInterruptionByAudio()
	{
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0508-HP");
	}
	
TVerdict CTestStepVideoInterruptionByAudio::DoTestStepPreambleL()
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}
	_LIT(KVideoFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\AudVid.avi");
	iFileName.Set(KVideoFileName);
	
	//create audio stream A - high priority
	iAudStreamA = CMdaAudioOutputStream::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceNone);
	InitWservL();
	//create video - normal priority
	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceNone,
										iWs, *iScreen, *iWindow, rect, clipRect);
	
	const TInt KAudioBufferSize = 1024 * 10; //10 KB
	iBuffer.CreateMaxL(KAudioBufferSize);
	
	iTestTimer = CTestTimer::NewL(*this);	
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("This step interrupts audio component of video,while playing.Video should be stopped"));
	iTestStepResult = EFail;
	return verdict;
	}

TVerdict CTestStepVideoInterruptionByAudio::DoTestStepPostambleL()
	{
	//cleanup audio stream
	delete iAudStreamA; 
	iAudStreamA = NULL;
	//video
	delete iPlayer;
	iPlayer = NULL;
	
	delete iTestTimer;
	iTestTimer = NULL;
	
	//window cleanup
	DeInitWserv();
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();	
	}

TVerdict CTestStepVideoInterruptionByAudio::DoTestStepL()
	{
	iAudStreamA->Open(NULL);
	CActiveScheduler::Start();
	return iTestStepResult;
	}

void CTestStepVideoInterruptionByAudio::Tester(TEvents aEvent, TInt aError)
	{
	TInt err = KErrNone;
	switch(aEvent)
		{
	case EAudioOpenComplete:
		if(aError != KErrNone)
			{
			INFO_PRINTF2(_L("Error in opening audio stream:%d"),aError);
			FailTest();
			break;
			}
		TRAP(err,iPlayer->OpenFileL(iFileName));
		if(err != KErrNone)	
			{
			INFO_PRINTF2(_L("Error: leave in iPlayer->OpenFileL(iFileName):%d"),aError);
			FailTest();
			break;
			}
		break;
		
	case EVideoOpenComplete:
	#ifndef SYMBIAN_MMF_MODEL_CONTROLLERS
	if(aError == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Feature not supported"));	
		iTestStepResult = EPass;
		CActiveScheduler::Stop();
		return;	
		}
	#endif
		if(aError != KErrNone)
			{
			INFO_PRINTF2(_L("Error: in opening video:%d"),aError);
			FailTest();
			break;
			}
		iPlayer->Prepare();
		break;
	
	case EVideoPrepareComplete:
		TRAP(err, iPlayTime = iPlayer->DurationL());
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:leave in -iPlayer->DurationL():%d "),err);
			FailTest();	
			break;
			}
		iPlayer->Play(); //play video
		iTestTimer->After(iPlayTime.Int64() + 5000000); //give double play time to end test
		TRAP(err,iAudStreamA->WriteL(iBuffer));//start high priority audio	
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:leave in - iAudStreamA->WriteL(iBuffer):%d"),err);
			FailTest();
			break;
			}
		break;
	
	case EVideoPlayComplete:
		INFO_PRINTF2(_L("Video play complete: %d"), aError);
		if(aError == KErrNone)
			{
			INFO_PRINTF1(_L("Test failure:Video played till end without interruption"));
			FailTest();	
			}
		else if(aError == KErrInUse)
			{
			iVidGotKErrInUse = ETrue;	
			}
		break;
		}
	}

void CTestStepVideoInterruptionByAudio::MaoscOpenComplete(TInt aError)
	{
	Tester(EAudioOpenComplete, aError);
	}
	
void CTestStepVideoInterruptionByAudio::MaoscBufferCopied(TInt /* aError */, const TDesC8& /* aBuffer */)
	{	
	}
	
void CTestStepVideoInterruptionByAudio::MaoscPlayComplete(TInt aError)
	{
	Tester(EAudioPlayComplete, aError);	
	}

void CTestStepVideoInterruptionByAudio::MvpuoOpenComplete(TInt aError)
	{
	Tester(EVideoOpenComplete, aError);
	}
	
void CTestStepVideoInterruptionByAudio::MvpuoPrepareComplete(TInt aError)
	{
	Tester(EVideoPrepareComplete, aError);
	}
	
void CTestStepVideoInterruptionByAudio::MvpuoFrameReady(CFbsBitmap& /* aFrame */, TInt /* aError */)
	{		
	}
	
void CTestStepVideoInterruptionByAudio::MvpuoPlayComplete(TInt aError)
	{
	Tester(EVideoPlayComplete, aError);
	}
	
void CTestStepVideoInterruptionByAudio::MvpuoEvent(const TMMFEvent& /* aEvent */)
	{	
	}

void CTestStepVideoInterruptionByAudio::OnTimer()
	{
	iPlayer->Close();
	if(iVidGotKErrInUse == EFalse)
		{
		INFO_PRINTF1(_L("Audio component of video not interrupted by Audio stream"));
		FailTest();
		}
	else
		{
		iTestStepResult = EPass;
		CActiveScheduler::Stop();		
		}
	}
	
void CTestStepVideoInterruptionByAudio::FailTest()
	{
	iTestStepResult = EFail;
	CActiveScheduler::Stop();	
	}
	
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidOnlyPlayerNoAudioType.
 *
 *
 * @return	"CTestStepVidOnlyPlayerNoAudioType*"
 *			The constructed CTestStepVidOnlyPlayerNoAudioType
 */
CTestStepVidOnlyPlayerNoAudioType* CTestStepVidOnlyPlayerNoAudioType::NewL()
	{
	CTestStepVidOnlyPlayerNoAudioType* self = new(ELeave) CTestStepVidOnlyPlayerNoAudioType;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidOnlyPlayerNoAudioType::CTestStepVidOnlyPlayerNoAudioType() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0257-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidOnlyPlayerNoAudioType::~CTestStepVidOnlyPlayerNoAudioType()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidOnlyPlayerNoAudioType::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidOnlyPlayerNoAudioType::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidOnlyPlayerNoAudioType::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidOnlyPlayerNoAudioType::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidOnlyPlayerNoAudioType::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidOnlyPlayerNoAudioType::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;		
		}

	iFileName.Set(KVidOnlyFile);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test tries to get the audio type of a video only clip with CVideoPlayerUtility::AudioTypeL(), so expecting KErrNotSupported"));
	
	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidOnlyPlayerNoAudioType::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidOnlyPlayerNoAudioType::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TFourCC type;

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
	if (!iPlayer)
		{
		INFO_PRINTF1(_L("Left while creating video player utility object"));
		return EFail;
		}

	if (!PlayerStateStopped())
		{
		INFO_PRINTF1(_L("Left when state is not expected one"));
		return EFail;
		}

	TRAP(err, OpenAndStartSchedulerL() );
	if (err != KErrNone || iError != KErrNone) 
		{
		INFO_PRINTF3(_L("ERROR: Left when the file is not found. err:%d; iError: %d"), err, iError );
		return EInconclusive;
		}
		
	TRAP(err, PlayerPrepareAndStartScheduler() );
	if (err != KErrNone || iError != KErrNone) 
		{
		INFO_PRINTF2(_L("Error:left when the prepare is not success:%d"),err);
		return EInconclusive;		
		}

	TRAP(err, type = iPlayer->AudioTypeL() );
	//Typhoon fix, Audio is not supported
	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Error:left when the error returned is not the expected one:%d"),err);
		return EFail;
		}
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// INC122181
//

CTestStepINC122181* CTestStepINC122181::NewL()
	{
	CTestStepINC122181* self = new(ELeave) CTestStepINC122181();
	return self;
	}

CTestStepINC122181::CTestStepINC122181()
	{
	iTestStepName = _L("MM-MMF-VCLNT-U-INC122181-CP");
	}

CTestStepINC122181::~CTestStepINC122181()
	{
	}

void CTestStepINC122181::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepINC122181::MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError)
	{
	if (aError!=KErrNone)
		iError2 = aError;
	else
		{
		if (aFrame.Handle() == NULL || aFrame.SizeInPixels() != TSize(KFrameWidth,KFrameHeight) )
			{
			iError2 = KErrArgument;
			}
		else
			{
			iTestStepResult = EPass;
			iError2 = KErrNone;
			}
		}
	CActiveScheduler::Stop();
	}

void CTestStepINC122181::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	}
	
void CTestStepINC122181::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	

void CTestStepINC122181::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

TVerdict CTestStepINC122181::DoTestStepPreambleL(void)
	{
	TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file

	return verdict;
	}

TVerdict CTestStepINC122181::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

TVerdict CTestStepINC122181::DoTestStepL()
	{
	iError = KErrTimedOut;		// Set error to something other than KErrNone
	iError2 = KErrTimedOut;		// Set error to something other than KErrNone

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer || !PlayerStateStopped())
		return EInconclusive;

	TRAPD(err, OpenAndStartSchedulerL() );

	if ( err != KErrNone || iError != KErrNone) 
		{
		INFO_PRINTF2(_L("Error calling OpenAndStartSchedulerL %d"), iError);
		return EInconclusive;
		}
		
	TRAP(err, PlayerPrepareAndStartScheduler() );

	if ( err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, iPlayer->GetFrameL(EColor16M) );

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error calling GetFrameL %d"), err);
		return EFail;
		}

	iPlayer->Play();

	// Set error to something other than KErrNone
	iError2 = KErrTimedOut;
	CActiveScheduler::Start();

	if (iError2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error in MvpuoFrameReady callback %d, or MvpuoFrameReady not called"), iError2);
		return EFail;
		}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


#ifdef SYMBIAN_BUILD_GCE
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerAddDisplayWindow.
 *
 *
 * @return	"CTestStepVidPlayerAddDisplayWindow*"
 *			The constructed CTestStepVidPlayerAddDisplayWindow
 * 
 */
CTestStepVidPlayerAddDisplayWindow* CTestStepVidPlayerAddDisplayWindow::NewL(TBool aSimplifiedVer, TInt aError, TBool aSameWindow)
	{
	CTestStepVidPlayerAddDisplayWindow* self = new(ELeave) CTestStepVidPlayerAddDisplayWindow(aSimplifiedVer, aError, aSameWindow);
	return self;
	}

/**
 * Test timer constructor.
 */
CTestStepVidPlayerAddDisplayWindow::CTestTimer* CTestStepVidPlayerAddDisplayWindow::CTestTimer::NewL(CTestStepVidPlayerAddDisplayWindow& aTestStepVidPlayer) 
	{
	CTestTimer* self = new(ELeave)CTestTimer(aTestStepVidPlayer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerAddDisplayWindow::CTestStepVidPlayerAddDisplayWindow(TBool aSimplifiedVer, TInt aError, TBool aSameWindow) 
	{
	iSimplifiedVer = aSimplifiedVer;
	iExpectedError = aError;
	iSameWindow = aSameWindow;
	iVpu2 = ETrue;
	// store the name of this test case
	// this is the name that is used by the script file	
	if (!aSimplifiedVer)
		{
		if (aError == KErrNone)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0815-CP");
			}
		else if (aError == KErrInUse && aSameWindow)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0856-CP");
			}
		else if (aError == KErrInUse && !aSameWindow)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0865-CP");
			}
		else if (aError == KErrNotReady)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0857-CP");
			}
		else if (aError == KErrArgument)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0860-CP");
			}
		else
			{
			iTestStepName = _L("unknown");
			}
		}
	else
		{
		if (aError == KErrNone)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0816-CP");
			}
		else if (aError == KErrInUse)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0858-CP");
			}
		else if (aError == KErrNotReady)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0859-CP");
			}
		else
			{
			iTestStepName = _L("unknown");
			}
		}	
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerAddDisplayWindow::~CTestStepVidPlayerAddDisplayWindow()
	{
	}


/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerAddDisplayWindow::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerAddDisplayWindow::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerAddDisplayWindow::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerAddDisplayWindow::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerAddDisplayWindow::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	INFO_PRINTF1(_L("MvpuoEvent received"));
	}

void CTestStepVidPlayerAddDisplayWindow::OnTimer()
	{
	CActiveScheduler::Stop();
	}

/**
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerAddDisplayWindow::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iSimplifiedVer)
		{
		if ( iExpectedError == KErrNone)
			{
			INFO_PRINTF1(_L("This test adds the display window with CVideoPlayerUtility2::AddDisplayWindowL()"));
			}
		if (iExpectedError == KErrInUse)
			{
			INFO_PRINTF1(_L("This is failure test for CVideoPlayerUtility2::AddDisplayWindowL() with KErrInUse"));
			}
		if (iExpectedError == KErrNotReady)
			{
			INFO_PRINTF1(_L("This is failure test for CVideoPlayerUtility2::AddDisplayWindowL() with KErrNotReady"));
			}
		if (iExpectedError == KErrArgument)
			{
			INFO_PRINTF1(_L("This is failure test for CVideoPlayerUtility2::AddDisplayWindowL() with KErrArgument"));
			}
		}
	else 
		{
		if (iExpectedError == KErrNone)
			{
			INFO_PRINTF1(_L("this test adds the display window with CVideoPlayerUtility2::AddDisplayWindowL() simplified version"));
			}
		if (iExpectedError == KErrInUse)
			{
			INFO_PRINTF1(_L("This is failure test for CVideoPlayerUtility2::AddDisplayWindowL() simplified version with KErrInUse"));
			}
		if (iExpectedError == KErrNotReady)
			{
			INFO_PRINTF1(_L("This is failure test for CVideoPlayerUtility2::AddDisplayWindowL() simplified version with KErrNotReady"));
			}
		}
	return verdict;
	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerAddDisplayWindow::DoTestStepPostambleL(void)
	{
	delete iPlayer2;
	iPlayer2 = NULL;
	delete iTestTimer;
	iTestTimer = NULL;	

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 * Test set video extent after graphics surface created.
 */
TBool CTestStepVidPlayerAddDisplayWindow::TestSetVideoExtentL()
	{
	TInt err = KErrNone;
	TBool result = ETrue;
	INFO_PRINTF1(_L("Set video extent after surface created."));
	TRect videoExtent(0,0,50,70);
	TRect getVideoExtent;
	
	TRAP(err, iPlayer2->SetVideoExtentL(*iWindow, videoExtent));
	if (err != KErrNone || GetPlayerWindowRect(getVideoExtent) != KErrNone
		|| getVideoExtent != videoExtent)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync()"));
		result = EFalse;
		}
	return result;			
	}
	
/**
 * Test set window clipping region after graphics surface created.
 */
TBool CTestStepVidPlayerAddDisplayWindow::TestSetGetWindowClipRectL()
	{
	TInt err = KErrNone;
	TBool result = ETrue;
	INFO_PRINTF1(_L("Set window clip rectangle after surface created."));
	TRect clipRect(0,0,50,70);
	TRect getClipRect;

	TRAP(err, iPlayer2->SetWindowClipRectL(*iWindow, clipRect));
	if (err != KErrNone || GetPlayerClipRect(getClipRect) != KErrNone
		|| getClipRect != clipRect)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync()"));
		result = EFalse;
		}
	return result;			
	}

/**
 * Test set and get rotation after graphics surface created.
 */
TBool CTestStepVidPlayerAddDisplayWindow::TestSetGetRotationL()
	{
	TInt err = KErrNone;
	TBool result = ETrue;
	INFO_PRINTF1(_L("Set Rotation after surface created."));
	TRAP(err, static_cast<CVideoPlayerUtility*>(iPlayer2)->SetRotationL(EVideoRotationClockwise90));
	TRAPD(err1, iPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise180));
	
	if (err!=KErrNone || err1 != KErrNone)
		{
		ERR_PRINTF1(_L("Failed to set rotation."));
		result = EFalse;
		}
			
	TInt rotation0 = EVideoRotationNone;
	TInt rotation1 = EVideoRotationNone;
	TRAP(err, rotation0 = iPlayer2->RotationL(*iWindow));
	TRAP(err1, rotation1 = static_cast<CVideoPlayerUtility*>(iPlayer2)->RotationL());	
		
	if (err!=KErrNone || err1 != KErrNone ||
		rotation0 != EVideoRotationClockwise180 || rotation1 != EVideoRotationClockwise90)
		{
		ERR_PRINTF1(_L("Failed to get rotation"));
		result = EFalse;
		}
	return result;			
	}
	
/**
 * Test set and get scale factor after graphics surface created.
 */
TBool CTestStepVidPlayerAddDisplayWindow::TestSetGetScaleFactorL()
	{
	TInt err = KErrNone;
	TBool result = ETrue;
	INFO_PRINTF1(_L("Set Scale Factor after surface created."));
	TReal32 widthScaleFactor1 = 0.0f;
	TReal32 heightScaleFactor1 = 0.0f;
	TReal32 widthScaleFactor2 = 0.0f;
	TReal32 heightScaleFactor2 = 0.0f;
	TBool antiAliasFilter = EFalse;

	TRAP(err, static_cast<CVideoPlayerUtility*>(iPlayer2)->SetScaleFactorL(KScaleWidth1, KScaleHeight1, ETrue));
	TRAPD(err1, iPlayer2->SetScaleFactorL(*iWindow, KScaleWidth2, KScaleHeight2));
	
	if (err!=KErrNone || err1 != KErrNone)
		{
		ERR_PRINTF1(_L("Failed to set scale factor"));
		result = EFalse;
		}
	
	TRAP(err, static_cast<CVideoPlayerUtility*>(iPlayer2)->GetScaleFactorL(widthScaleFactor1,heightScaleFactor1,antiAliasFilter));		
	TRAP(err1, iPlayer2->GetScaleFactorL(*iWindow, widthScaleFactor2,heightScaleFactor2));
	
	if (err!=KErrNone || err1 != KErrNone || !antiAliasFilter ||
		widthScaleFactor1 != KScaleWidth1 || heightScaleFactor1 != KScaleHeight1 ||
		widthScaleFactor2 != KScaleWidth2 || heightScaleFactor2 != KScaleHeight2)
		{
		ERR_PRINTF1(_L("Failed to get scale factor"));
		result = EFalse;
		}
	return result;			
	}
	
/**
 * Test set and get scale factor after graphics surface created.
 */
TBool CTestStepVidPlayerAddDisplayWindow::TestSetGetCropRegionL()
	{
	TInt err = KErrNone;
	TBool result = ETrue;
	INFO_PRINTF1(_L("Set crop region after surface created."));
	TRect cropRegion0(KCropRegionRectLeft, KCropRegionRectTop, KCropRegionRectRight, KCropRegionRectBottom);
	TRect region0;
	TRAP(err, static_cast<CVideoPlayerUtility*>(iPlayer2)->SetCropRegionL(cropRegion0));
	
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed to set crop region"));
		result = EFalse;
		}
				
	TRAP(err, static_cast<CVideoPlayerUtility*>(iPlayer2)->GetCropRegionL(region0));
		
	if (err!=KErrNone || region0 != cropRegion0)
		{
		ERR_PRINTF1(_L("Failed to get crop region"));
		result = EFalse;
		}
	return result;			
	}

/**
 * Send custom command to the client
 */
TInt CTestStepVidPlayerAddDisplayWindow::SendCustomCommandSync(TMMFMessageDestinationPckg aDestPckg, TInt aFunction)
	{
	TInt err = KErrNone;
	err = iPlayer2->CustomCommandSync(aDestPckg,
					aFunction,
					KNullDesC8,
					KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync()"));
		return err;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();
	return err;			
	}	
		
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 */
TVerdict CTestStepVidPlayerAddDisplayWindow::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	
	TRect clipRect(0,0,60,80);
	TRect clipRect2(0,0,600,800);
	TPoint screenPos(10, 10);
	TRect screenRect(screenPos, clipRect.Size());
	TRect refScreenRect, refClipRect;

	InitWservL();
	
	iTestTimer = CTestTimer::NewL(*this);

	TRect rect;
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("The state is not stopped"));
		return  EInconclusive;
		}
	
	if (!iSimplifiedVer && (iExpectedError == KErrNone || iExpectedError == KErrArgument))
		{
		TRAP(err, OpenAndStartSchedulerL());

		if (err != KErrNone || iError != KErrNone) 
			{
			ERR_PRINTF1(_L("Failed to open file"));
			return  EInconclusive;
			}
		
		if (iExpectedError == KErrNone )
			{
			TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, screenRect, clipRect));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
				return  EFail;
				}
			}
		else 
			{
			TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, screenRect, clipRect2));
			if (err != KErrArgument)
				{
				ERR_PRINTF2(_L("Nagetive AddDisplayWindow test failed: %d. Expected: KErrArgument."), err);
				return  EFail;
				}
			}
		}
		
	if (!iSimplifiedVer && iExpectedError == KErrInUse)
		{
		// This is kind of sketchy since the error we expect at this point is actually KErrNone 
		// but due to the poorly constructed test case if we change it to that we'll actually run another test case!
		TRAP(err, OpenAndStartSchedulerL() );

		if (err != KErrNone || iError != KErrNone) 
			{
			ERR_PRINTF1(_L("Failed to open file"));
			return  EInconclusive;
			}
		
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, screenRect, clipRect));
		if (err != KErrNone)
			{
			return  EInconclusive;
			}
		
		CWsScreenDevice* screen2 = NULL;
		
		if (!iSameWindow)
			{			
			TInt wsNumber = iWs.NumberOfScreens();
			if (wsNumber > 1)
				{
				screen2 = new (ELeave) CWsScreenDevice(iWs);
				CleanupStack::PushL(screen2);
				TInt newScreen = iScreen->GetScreenNumber() + 1; // a different screen number;
				User::LeaveIfError(screen2->Construct(newScreen));
				}
			else
				{
				ERR_PRINTF1(_L("Failed when the winscw environment does not support multiple windows"));
				return  EInconclusive;
				}
			}
		
		// Add display window using the secondary screen device (if it exists).  Otherwise use the original screen.
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *(screen2 ? screen2 : iScreen), *iWindow, screenRect, clipRect));
		
		if (screen2)
			{
			CleanupStack::PopAndDestroy(screen2);
			}
		
		if (err != KErrInUse)
			{
			return EFail;
			}
		}
		
	if (!iSimplifiedVer && iExpectedError == KErrNotReady)
		{
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, screenRect, clipRect));
		if (err != KErrNotReady)
			{
			return EFail;
			}
		}
	
	if (iSimplifiedVer && iExpectedError == KErrNone)
		{
		TRAP(err, OpenAndStartSchedulerL() );

		if (err != KErrNone || iError != KErrNone) 
			{
			ERR_PRINTF1(_L("Failed to open file"));
			return  EInconclusive;
			}
		
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
		if (err != KErrNone)
			{
			return  EFail;
			}
		}
	if (iSimplifiedVer && iExpectedError == KErrInUse)
		{
		TRAP(err, OpenAndStartSchedulerL() );

		if (err != KErrNone || iError != KErrNone) 
			{
			ERR_PRINTF1(_L("Failed to open file"));
			return  EInconclusive;
			}
		
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
		if (err != KErrNone)
			{
			return  EInconclusive;
			}
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
		if (err != KErrInUse)
			{
			return EFail;
			}
		}
	if (iSimplifiedVer && iExpectedError == KErrNotReady)
		{
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
		if (err != KErrNotReady)
			{
			return EFail;
			}
		}	

	if (err == KErrNone && (GetPlayerWindowRect(refScreenRect) || 
			GetPlayerClipRect(refClipRect)))
		{
		ERR_PRINTF1(_L("Failed at get"));
		return EFail;
		}

	if ((!iSimplifiedVer && iExpectedError == KErrNone) && 
		(refClipRect != clipRect || refScreenRect != screenRect))
		{
		ERR_PRINTF1(_L("Failed when comparing the values"));		
		return EFail;
		}
	
	if ((iSimplifiedVer && iExpectedError == KErrNone) && 
		(refClipRect != TRect(iWindow->Size()) || refScreenRect != TRect(iWindow->Size())))
		{	
		ERR_PRINTF1(_L("Failed when comparing the values"));		
		return EFail;
		}
	
	// Now the surface has been created. Trigger the controller to send video surface created event to client api.
	if (iExpectedError == KErrNone)
		{
		TUid uid;
		uid = TUid::Uid(0x101f72B4);
		TMMFMessageDestinationPckg destPckg(uid);
		
		INFO_PRINTF1(_L("AddDisplayWindowL() passed, then trigger event KMMFEventCategoryVideoSurfaceCreated video client."));
	
		err = SendCustomCommandSync(destPckg, EMMFTestCustomCommandSimulateSurfaceCreated);
								
		if (err!=KErrNone)
			{
			return EInconclusive;
			}
		
		// Controller should receive GetSurfaceParameters call.
		iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
		if (iTestStepResult == EFail)
			{
			return iTestStepResult;
			}
	
		if (SurfaceCreated())
			{
			if (!TestSetVideoExtentL() || !TestSetGetWindowClipRectL() ||
				!TestSetGetRotationL() || !TestSetGetScaleFactorL() ||
				!TestSetGetCropRegionL())
				{
				return EFail;
				}
			}
		}

	delete iPlayer2;
	iPlayer2 = NULL;

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//

/**
 *
 * Static constructor for CTestStepVidPlayerSetInitScreenNumber.
 *
 * @return	"CTestStepVidPlayerSetInitScreenNumber*"
 *			The constructed CTestStepVidPlayerSetInitScreenNumber
 */
CTestStepVidPlayerSetInitScreenNumber* CTestStepVidPlayerSetInitScreenNumber::NewL(TBool aUseSurface)
	{
	CTestStepVidPlayerSetInitScreenNumber* self = new(ELeave) CTestStepVidPlayerSetInitScreenNumber(aUseSurface);
	return self;
	}

/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerSetInitScreenNumber::CTestStepVidPlayerSetInitScreenNumber(TBool aUseSurface) 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = aUseSurface;
	if (aUseSurface)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0855-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-00xx-HP");
		}
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerSetInitScreenNumber::~CTestStepVidPlayerSetInitScreenNumber()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSetInitScreenNumber::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerSetInitScreenNumber::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerSetInitScreenNumber::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerSetInitScreenNumber::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerSetInitScreenNumber::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Test step Preamble. 
 */
enum TVerdict CTestStepVidPlayerSetInitScreenNumber::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this test sets init screen number with CVideoPlayerUtility::SetInitScreenNumber()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is negative test with CVideoPlayerUtility2::SetInitScreenNumber()"));		
		}
	return verdict;

	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerSetInitScreenNumber::DoTestStepPostambleL(void)
	{
	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
	else 
		{
		delete iPlayer2;
		iPlayer2 = NULL;	
		}

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 * 
 */
TVerdict CTestStepVidPlayerSetInitScreenNumber::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	
	InitWservL();

	TRect rect;
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, rect);
		}
	else 
		{
			iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		}
		
	if ( !PlayerStateStopped())
		{
		return EInconclusive;
		}
		
	if (!iVpu2)
		{
		TRAP(err, iPlayer->SetInitScreenNumber(iScreen->GetScreenNumber()));	
		}
	else 
		{
		err = iPlayer2->SetInitScreenNumber(iScreen->GetScreenNumber());
		}

	if (iVpu2 && err != KErrNotSupported)
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		return EFail;
		}
	if (!iVpu2)
		{
		if (err != KErrNone || GetInitScreenNumber() )
			{
			return EFail;
			}

		delete iPlayer;
		iPlayer = NULL;
		}

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//

/**
 *
 * Static constructor for CTestStepVidPlayerRemoveDisplayWindow.
 *
 * @return	"CTestStepVidPlayerRemoveDisplayWindow*"
 *			The constructed CTestStepVidPlayerRemoveDisplayWindow
 */
CTestStepVidPlayerRemoveDisplayWindow* CTestStepVidPlayerRemoveDisplayWindow::NewL(TBool aPositve)
	{
	CTestStepVidPlayerRemoveDisplayWindow* self = new(ELeave) CTestStepVidPlayerRemoveDisplayWindow(aPositve);
	return self;
	}

/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerRemoveDisplayWindow::CTestStepVidPlayerRemoveDisplayWindow(TBool aPositve) 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iPositive = aPositve;
	iVpu2 = ETrue;
	if (aPositve)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0817-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0818-CP");
		}
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerRemoveDisplayWindow::~CTestStepVidPlayerRemoveDisplayWindow()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerRemoveDisplayWindow::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerRemoveDisplayWindow::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerRemoveDisplayWindow::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerRemoveDisplayWindow::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerRemoveDisplayWindow::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Test step Preamble. 
 */
enum TVerdict CTestStepVidPlayerRemoveDisplayWindow::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (iPositive)
		{
		INFO_PRINTF1(_L("this tests removes display window with CVideoPlayerUtility2"));
		}
	else
		{
		INFO_PRINTF1(_L("this tests removes display window with CVideoPlayerUtility2::RemoveDisplayWindow() when the display is not added"));		
		}
	return verdict;

	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerRemoveDisplayWindow::DoTestStepPostambleL(void)
	{
	delete iPlayer2;
	iPlayer2 = NULL;	
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 * 
 */
TVerdict CTestStepVidPlayerRemoveDisplayWindow::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	
	InitWservL();

	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		
	if ( !PlayerStateStopped())
		{
		return EInconclusive;
		}
	
	TRAP(err, OpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		{
		return  EInconclusive;
		}
	
	if (iPositive)
		{	
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));

		if (err != KErrNone || iError != KErrNone) 
			{
			return  EInconclusive;
			}
		}
		
	TRAP(err, iPlayer2->RemoveDisplayWindow(*iWindow));
	
	if (err != KErrNone)
		{
		return EFail;
		}

	delete iPlayer2;
	iPlayer2 = NULL;

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//

/**
 *
 * Static constructor for CTestStepVidPlayerSetVideoExtent.
 *
 * @return	"CTestStepVidPlayerSetVideoExtent*"
 *			The constructed CTestStepVidPlayerSetVideoExtent
 */
CTestStepVidPlayerSetVideoExtent* CTestStepVidPlayerSetVideoExtent::NewL(TInt aExpected)
	{
	CTestStepVidPlayerSetVideoExtent* self = new(ELeave) CTestStepVidPlayerSetVideoExtent(aExpected);
	return self;
	}

/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerSetVideoExtent::CTestStepVidPlayerSetVideoExtent(TInt aExpected) 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iExpected = aExpected;
	iVpu2 = ETrue;
	if (aExpected == KErrNone)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0819-CP");
		}
	else if (aExpected == KErrNotReady)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0861-CP");
		}
	else
		{
		ERR_PRINTF1(_L("Unknown test case."));
		}
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerSetVideoExtent::~CTestStepVidPlayerSetVideoExtent()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSetVideoExtent::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerSetVideoExtent::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerSetVideoExtent::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerSetVideoExtent::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerSetVideoExtent::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Test step Preamble. 
 */
enum TVerdict CTestStepVidPlayerSetVideoExtent::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (iExpected == KErrNone)
		{
		INFO_PRINTF1(_L("this tests set video extent with CVideoPlayerUtility2"));
		}
	else
		{
		INFO_PRINTF1(_L("this is negative test with CVideoPlayerUtility2::SetVideoExtentL()"));		
		}
	return verdict;

	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerSetVideoExtent::DoTestStepPostambleL(void)
	{
	delete iPlayer2;
	iPlayer2 = NULL;	
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 * 
 */
TVerdict CTestStepVidPlayerSetVideoExtent::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	
	InitWservL();

	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		
	if ( !PlayerStateStopped())
		{
		return EInconclusive;
		}
	
	if (iExpected == KErrNone)
		{	
		TRAP(err, OpenAndStartSchedulerL() );

		if (err != KErrNone || iError != KErrNone) 
			{
			return  EInconclusive;
			}
		}
	
	TSize size((iWindow->Size().iWidth + 10), (iWindow->Size().iHeight + 10));
	TRect rect(size);

	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	
	if (iExpected == KErrNotReady && err != KErrNotReady)
		{
		ERR_PRINTF2(_L("AddDisplayWindowL failed with %d."), err);
		return EFail;
		}

	TRAP(err, iPlayer2->SetVideoExtentL(*iWindow, rect));
	
	if (iExpected == KErrNotReady && err != KErrNotReady)
		{
		return EFail;
		}

	delete iPlayer2;
	iPlayer2 = NULL;

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//

/**
 *
 * Static constructor for CTestStepVidPlayerSetWindowClipRect.
 *
 * @return	"CTestStepVidPlayerSetWindowClipRect*"
 *			The constructed CTestStepVidPlayerSetWindowClipRect
 */
CTestStepVidPlayerSetWindowClipRect* CTestStepVidPlayerSetWindowClipRect::NewL(TInt aExpected)
	{
	CTestStepVidPlayerSetWindowClipRect* self = new(ELeave) CTestStepVidPlayerSetWindowClipRect(aExpected);
	return self;
	}

/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerSetWindowClipRect::CTestStepVidPlayerSetWindowClipRect(TInt aExpected) 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iExpected = aExpected;
	iVpu2 = ETrue;
	if (aExpected == KErrNone)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0820-CP");
		}
	else if (aExpected == KErrNotReady)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0862-CP");
		}
	else if (aExpected == KErrArgument)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0863-CP");
		}
	else 
		{		
		ERR_PRINTF1(_L("Unknown test case."));
		}
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerSetWindowClipRect::~CTestStepVidPlayerSetWindowClipRect()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSetWindowClipRect::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerSetWindowClipRect::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerSetWindowClipRect::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerSetWindowClipRect::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerSetWindowClipRect::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Test step Preamble. 
 */
enum TVerdict CTestStepVidPlayerSetWindowClipRect::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (iExpected == KErrNone)
		{
		INFO_PRINTF1(_L("this tests set window clipping rectangle with CVideoPlayerUtility2"));
		}
	else if (iExpected == KErrNotReady)
		{
		INFO_PRINTF1(_L("this is negative test KErrNotReady with CVideoPlayerUtility2::SetWindowClipRectL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is negative test KErrArgument with CVideoPlayerUtility2::SetWindowClipRectL()"));		
		}
	return verdict;

	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerSetWindowClipRect::DoTestStepPostambleL(void)
	{
	delete iPlayer2;
	iPlayer2 = NULL;	
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 * 
 */
TVerdict CTestStepVidPlayerSetWindowClipRect::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	
	InitWservL();

	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		
	if ( !PlayerStateStopped())
		{
		return EInconclusive;
		}
	
	if (iExpected == KErrNone || iExpected == KErrArgument)
		{	
		TRAP(err, OpenAndStartSchedulerL() );

		if (err != KErrNone || iError != KErrNone) 
			{
			return  EInconclusive;
			}
		}
	
	TRect rect1(iWindow->Size());
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (iExpected == KErrNotReady && err != KErrNotReady)
		{
		ERR_PRINTF2(_L("AddDisplayWindowL failed with %d."), err);
		return EFail;
		}
	
	TRAP(err, iPlayer2->SetWindowClipRectL(*iWindow, rect1));
	
	if (iExpected == KErrNotReady && err != KErrNotReady)
		{
		return EFail;
		}

	TSize size((iWindow->Size().iWidth + 10), (iWindow->Size().iHeight + 10));
	TRect rect2(size);
	TRAP(err, iPlayer2->SetWindowClipRectL(*iWindow, rect2));
	
	if (iExpected == KErrArgument && err != KErrArgument)
		{
		return EFail;
		}
		
	delete iPlayer2;
	iPlayer2 = NULL;

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerReplaceSurfaceSequence1.
 *
 *
 * @return	"CTestStepVidPlayerReplaceSurfaceSequence1*"
 *			The constructed CTestStepVidPlayerReplaceSurfaceSequence1
 * 
 */
CTestStepVidPlayerReplaceSurfaceSequence1* CTestStepVidPlayerReplaceSurfaceSequence1::NewL()
	{
	CTestStepVidPlayerReplaceSurfaceSequence1* self = new(ELeave) CTestStepVidPlayerReplaceSurfaceSequence1();
	return self;
	}

/**
 * Test timer constructor.
 */
CTestStepVidPlayerReplaceSurfaceSequence1::CTestTimer* 
CTestStepVidPlayerReplaceSurfaceSequence1::CTestTimer::NewL(CTestStepVidPlayerReplaceSurfaceSequence1& aTestStepVidPlayer) 
	{
	CTestTimer* self = new(ELeave)CTestTimer(aTestStepVidPlayer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerReplaceSurfaceSequence1::CTestStepVidPlayerReplaceSurfaceSequence1() 
	{
	iVpu2 = ETrue;
	// store the name of this test case
	// this is the name that is used by the script file	
	iTestStepName = _L("MM-MMF-VCLNT-U-0821-CP");	
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerReplaceSurfaceSequence1::~CTestStepVidPlayerReplaceSurfaceSequence1()
	{
	}


/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerReplaceSurfaceSequence1::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerReplaceSurfaceSequence1::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerReplaceSurfaceSequence1::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerReplaceSurfaceSequence1::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerReplaceSurfaceSequence1::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	INFO_PRINTF1(_L("MvpuoEvent received"));
	}

void CTestStepVidPlayerReplaceSurfaceSequence1::OnTimer()
	{
	CActiveScheduler::Stop();
	}

/**
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerReplaceSurfaceSequence1::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
		
	return verdict;
	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerReplaceSurfaceSequence1::DoTestStepPostambleL(void)
	{
	delete iPlayer2;
	iPlayer2 = NULL;
	delete iTestTimer;
	iTestTimer = NULL;	

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 */
TVerdict CTestStepVidPlayerReplaceSurfaceSequence1::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	InitWservL();
	
	iTestTimer = CTestTimer::NewL(*this);

	TRect rect;
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	
	if ( !PlayerStateStopped())
		{
		ERR_PRINTF1(_L("Failed with NewL() or the state is not stopped"));
		return  EInconclusive;
		}
	
	TRAP(err, OpenAndStartSchedulerL());

	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF1(_L("Failed to open file"));
		return  EInconclusive;
		}
		
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EFail;
		}

	// Send event to the controller

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);

	
	// Create --------------------------
	INFO_PRINTF1(_L("AddDisplayWindowL() passed, then request controller to send event KMMFEventCategoryVideoSurfaceCreated video client."));
	
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateSurfaceCreated,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateSurfaceCreated"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();

	// Controller should receive GetSurfaceParameters call.
	iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
		
	if (!SurfaceCreated())
		{
		return EFail;
		}

	// parameters changed --------------------------
	INFO_PRINTF1(_L("Surface successfully created now request controller to send KMMFEventCategoryVideoSurfaceParametersChanged to video client."));
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateParaChanged,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateParaChanged"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();
	
	// Controller should receive GetSurfaceParameters call.
	iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
	
	// Remove --------------------------
	INFO_PRINTF1(_L("Request controller to send event KMMFEventCategoryVideoRemoveSurface to video client."));
	
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateRemoveSurface,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateRemoveSurfaces"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();

	// Controller should receive SurfaceRemoved call.
	iTestStepResult = ControllerCalled(_L8("MvpssSurfaceRemovedL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
	
	if (SurfaceCreated())
		{
		return EFail;
		}
	
	// create --------------------------
	INFO_PRINTF1(_L("Remove passed, now request controller to send event KMMFEventCategoryVideoSurfaceCreated to video client."));
	
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateSurfaceCreated,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateSurfaceCreated"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();

	// Controller should receive GetSurfaceParameters call.
	iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
		
	if (!SurfaceCreated())
		{
		return EFail;
		}
	
	// done - tidy up
	delete iPlayer2;
	iPlayer2 = NULL;

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerReplaceSurfaceSequence2.
 *
 *
 * @return	"CTestStepVidPlayerReplaceSurfaceSequence2*"
 *			The constructed CTestStepVidPlayerReplaceSurfaceSequence2
 * 
 */
CTestStepVidPlayerReplaceSurfaceSequence2* CTestStepVidPlayerReplaceSurfaceSequence2::NewL()
	{
	CTestStepVidPlayerReplaceSurfaceSequence2* self = new(ELeave) CTestStepVidPlayerReplaceSurfaceSequence2();
	return self;
	}

/**
 * Test timer constructor.
 */
CTestStepVidPlayerReplaceSurfaceSequence2::CTestTimer* 
CTestStepVidPlayerReplaceSurfaceSequence2::CTestTimer::NewL(CTestStepVidPlayerReplaceSurfaceSequence2& aTestStepVidPlayer) 
	{
	CTestTimer* self = new(ELeave)CTestTimer(aTestStepVidPlayer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
/**
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerReplaceSurfaceSequence2::CTestStepVidPlayerReplaceSurfaceSequence2() 
	{
	iVpu2 = ETrue;
	// store the name of this test case
	// this is the name that is used by the script file	
	iTestStepName = _L("MM-MMF-VCLNT-U-0822-CP");	
	}

/**
 * Test step destructor.
 */
CTestStepVidPlayerReplaceSurfaceSequence2::~CTestStepVidPlayerReplaceSurfaceSequence2()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerReplaceSurfaceSequence2::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerReplaceSurfaceSequence2::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerReplaceSurfaceSequence2::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerReplaceSurfaceSequence2::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerReplaceSurfaceSequence2::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	INFO_PRINTF1(_L("MvpuoEvent received"));
	}

void CTestStepVidPlayerReplaceSurfaceSequence2::OnTimer()
	{
	CActiveScheduler::Stop();
	}

/**
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerReplaceSurfaceSequence2::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
		
	return verdict;
	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerReplaceSurfaceSequence2::DoTestStepPostambleL(void)
	{
	delete iPlayer2;
	iPlayer2 = NULL;
	delete iTestTimer;
	iTestTimer = NULL;	

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 */
TVerdict CTestStepVidPlayerReplaceSurfaceSequence2::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	InitWservL();
	
	iTestTimer = CTestTimer::NewL(*this);

	TRect rect;
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	
	if ( !PlayerStateStopped())
		{
		ERR_PRINTF1(_L("Failed with NewL() or the state is not stopped"));
		return  EInconclusive;
		}
	
	TRAP(err, OpenAndStartSchedulerL());

	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF1(_L("Failed to open file"));
		return  EInconclusive;
		}
		
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EFail;
		}

	// Send event to the controller

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);

	
	// Create --------------------------
	INFO_PRINTF1(_L("AddDisplayWindowL() passed, then request controller to send event KMMFEventCategoryVideoSurfaceCreated to video client."));
	
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateSurfaceCreated,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateSurfaceCreated"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();

	// Controller should receive GetSurfaceParameters call.
	iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
		
	if (!SurfaceCreated())
		{
		return EFail;
		}

	// parameters changed --------------------------
	INFO_PRINTF1(_L("Surface successfully created now request controller to send KMMFEventCategoryVideoSurfaceParametersChanged to video client."));
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateParaChanged,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateParaChanged"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();
	
	// Controller should receive GetSurfaceParameters call.
	iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
	
	// Create --------------------------
	INFO_PRINTF1(_L("Now Replace surface by requesting controller to send event KMMFEventCategoryVideoSurfaceCreated to video client."));
	
	err = iPlayer2->CustomCommandSync(destPckg,
			EMMFTestCustomCommandSimulateSurfaceCreated,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateSurfaceCreated"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();

	// Controller should receive SurfaceRemoved call.
	iTestStepResult = ControllerCalled(_L8("MvpssSurfaceRemovedL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}

	// Controller should receive GetSurfaceParameters call.
	iTestStepResult = ControllerCalled2(_L8("MvpssGetSurfaceParametersL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
	
	if (!SurfaceCreated())
		{
		return EFail;
		}
	
	// Remove --------------------------
	INFO_PRINTF1(_L("Request controller to send event KMMFEventCategoryVideoRemoveSurface to video client."));
	
	err = iPlayer2->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateRemoveSurface,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Failed when calling CustomCommandSync() with EMMFTestCustomCommandSimulateRemoveSurfaces"));
		return EInconclusive;
		}
	iTestTimer->After(5000000);	
	CActiveScheduler::Start();

	// Controller should receive SurfaceRemoved call.
	iTestStepResult = ControllerCalled(_L8("MvpssSurfaceRemovedL Called"));
	if (iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
	
	if (SurfaceCreated())
		{
		return EFail;
		}	
	
	// done - tidy up
	delete iPlayer2;
	iPlayer2 = NULL;

	iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

//
// Auto Scale test
//
CTestStepVidPlayerAutoScale* CTestStepVidPlayerAutoScale::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerAutoScale* self = new(ELeave) CTestStepVidPlayerAutoScale(aTestName);
	return self;
	}

CTestStepVidPlayerAutoScale::CTestStepVidPlayerAutoScale(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	iFileName.Set(KClockFileName);
	iVpu2 = ETrue;
	}
	
TVerdict CTestStepVidPlayerAutoScale::DoTestStepL()
	{
	TInt err = KErrNone;
	
	InitWservL();

	TInt expectedResult = KErrNone;
	TBool useSurfaces = ETrue;
	
	GetIntFromConfig(iTestStepName, _L("ExpectedResult"), expectedResult);
	GetBoolFromConfig(iTestStepName, _L("UseSurfaces"), useSurfaces);

	if (!useSurfaces)
		{
		iVpu2 = EFalse;
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, iWs, 
											*iScreen, *iWindow, TRect(0, 0, 100, 100), TRect(0, 0, 100, 100));
		
		TRAP(err, OpenAndStartSchedulerL() );
		
		if (err != KErrNone)
			{
			iTestStepResult = EInconclusive;
			delete iPlayer;
			iPlayer = NULL;
			return iTestStepResult;
			}
		
		TRAP(err, iPlayer->SetAutoScaleL(EAutoScaleNone));
		
		if (err == expectedResult)
			{
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			}
		
		delete iPlayer;
		iPlayer = NULL;
		return iTestStepResult;
		}
	

	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;
		}
	
	TRAP(err, OpenAndStartSchedulerL() );
	
	iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);
	
	TAutoScaleType autoScaleType;
	TInt autoScaleHoriz;
	TInt autoScaleVert;
	TPtrC result;
	
	GetStringFromConfig(iTestStepName, _L("AutoScaleType"), result);
	if (result == _L("EAutoScaleNone"))
		{
		autoScaleType = EAutoScaleNone;
		}
	else if (result == _L("EAutoScaleBestFit"))
		{
		autoScaleType = EAutoScaleBestFit;
		}
	else if (result == _L("EAutoScaleClip"))
		{
		autoScaleType = EAutoScaleClip;
		}
	else if (result == _L("EAutoScaleStretch"))
		{
		autoScaleType = EAutoScaleStretch;
		}
	else
		{
		ERR_PRINTF1(_L("Unknown Auto Scale type"));
		iTestStepResult = EInconclusive;
		delete iPlayer2;
		iPlayer2 = NULL;
		return iTestStepResult;
		}
	
	GetStringFromConfig(iTestStepName, _L("AutoScaleHoriz"), result);
	if (result == _L("EHorizontalAlignCenter"))
		{
		autoScaleHoriz = EHorizontalAlignCenter;
		}
	else if (result == _L("EHorizontalAlignRight"))
		{
		autoScaleHoriz = EHorizontalAlignRight;
		}
	else if (result == _L("EHorizontalAlignLeft"))
		{
		autoScaleHoriz = EHorizontalAlignLeft;
		}
	else 
		{
		GetIntFromConfig(iTestStepName, _L("AutoScaleHoriz"), autoScaleHoriz);
		}
	
	GetStringFromConfig(iTestStepName, _L("AutoScaleVert"), result);
	if (result == _L("EVerticalAlignCenter"))
		{
		autoScaleVert = EVerticalAlignCenter;
		}
	else if (result == _L("EVerticalAlignTop"))
		{
		autoScaleVert = EVerticalAlignTop;
		}
	else if (result == _L("EVerticalAlignBottom"))
		{
		autoScaleVert = EVerticalAlignBottom;
		}
	else 
		{
		GetIntFromConfig(iTestStepName, _L("AutoScaleVert"), autoScaleVert);
		}
	
	TRAP(err, static_cast<CVideoPlayerUtility*>(iPlayer2)->SetAutoScaleL(autoScaleType, autoScaleHoriz, autoScaleVert));
	TRAPD(err1, iPlayer2->SetAutoScaleL(*iWindow, autoScaleType, autoScaleHoriz, autoScaleVert));
	
	if (err1 != expectedResult ||
		err != expectedResult)
		{
		ERR_PRINTF4(_L("SetAutoScaleL() left with %d, %d.  Expected %d."), err, err1, expectedResult);
		delete iPlayer2;
		iPlayer2 = NULL;
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	
	TRAP(err, PlayerPrepareAndStartScheduler());
	
	if (err != KErrNone || iError != KErrNone) 
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		iTestStepResult = EInconclusive;
		return iTestStepResult;
		}

	TRAP(err, PlayAndStartSchedulerL());
	if (err == KErrNone && iError == KErrNone)
		{
		iTestStepResult = EPass;
		}
	else
		{
		iTestStepResult = EFail;
		ERR_PRINTF1(_L("Failed to play file"));
		}
	
	delete iPlayer2;
	iPlayer2 = NULL;
	return iTestStepResult;	
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerAutoScale::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerAutoScale::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerAutoScale::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerAutoScale::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerAutoScale::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

#endif // SYMBIAN_BUILD_GCE
