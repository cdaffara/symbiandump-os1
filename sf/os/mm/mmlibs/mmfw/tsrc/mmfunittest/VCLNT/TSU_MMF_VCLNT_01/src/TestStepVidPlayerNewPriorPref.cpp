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
#include <hal.h>
#include <testframework.h>

// Test system includes
#include <testframework.h>

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mmf/common/mmfcontroller.h>
#include <videoplayer.h>
#include <videorecorder.h>

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#endif

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidPlayer.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepVidPlayerNewPriorPref.
 *
 *
 * @return	"CTestStepVidPlayerNewPriorPref*"
 *			The constructed CTestStepVidPlayerNewPriorPref
 */
CTestStepVidPlayerNewPriorPref* CTestStepVidPlayerNewPriorPref::NewL(TBool aUseSurface)
	{
	CTestStepVidPlayerNewPriorPref* self = new(ELeave) CTestStepVidPlayerNewPriorPref(aUseSurface);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerNewPriorPref::CTestStepVidPlayerNewPriorPref(TBool aUseSurface) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = aUseSurface;
	if (aUseSurface)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0802-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0002-CP");
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerNewPriorPref::~CTestStepVidPlayerNewPriorPref()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerNewPriorPref::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerNewPriorPref::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerNewPriorPref::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerNewPriorPref::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

	
void CTestStepVidPlayerNewPriorPref::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerNewPriorPref::DoTestStepL()
	{
	// N.B. we have to call HAL::Get in order to prevent a memory leak being detected
	// between the __UHEAP markers below. This is because HAL::Get allocates some memory on the 
	// first call and is called in CVideoPlayerUtility::NewL (setting up direct screen access).
	// There is no method to deallocate this memory so we allocate it outside of the __UHEAP markers.
	if (!iVpu2)
		{
		TInt address = 0;
		HAL::Get(HALData::EDisplayMemoryAddress, address);
		}

	__MM_HEAP_MARK;
	// Printing to the console and log file
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this test is iniatilising a CVideoPlayerUtility object with NewL() and specific Priority/Preference values"));
		}
	else
		{
		INFO_PRINTF1(_L("this test is iniatilising a CVideoPlayerUtility2 object with NewL() and specific Priority/Preference values"));
		}
	iTestStepResult = EPass;

	// test 1

	InitWservL();

	INFO_PRINTF1(_L("test 1 Priority: EMdaPriorityMax /Preference: EMdaPriorityPreferenceTimeAndQuality"));
	TRect rect, clipRect;
	if (!iVpu2)
		{	
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality);
		}
	if (iVpu2 && !iPlayer2) 
		{
		return EFail;
		}
#endif
	if (!PlayerStateStopped() )
		{
		return EFail;
		}

	if ((!iVpu2 && !iPlayer) || GetPlayerPriority(iPrioritySettings) != KErrNone)
		{
		return EFail;
		}

	if ( iPrioritySettings.iPriority != EMdaPriorityMax )
		{
		return EFail;
		}


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
	// test 2
	INFO_PRINTF1(_L("test 2 Priority: EMdaPriorityMin /Preference: EMdaPriorityPreferenceTimeAndQuality"));
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
		}
	
	if 	(iVpu2 && !iPlayer2)
		{
		return EFail;
		}
#endif
	if ((!iVpu2 && !iPlayer) || GetPlayerPriority(iPrioritySettings) != KErrNone)
		{
		return EFail;
		}
		
	if ( !PlayerStateStopped() || iPrioritySettings.iPriority != EMdaPriorityMin ||
		 iPrioritySettings.iPref != EMdaPriorityPreferenceTimeAndQuality)
		 {
		 return EFail;
		 }

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
	// test 3
	INFO_PRINTF1(_L("test 3 Priority: EMdaPriorityMax /Preference: EMdaPriorityPreferenceQuality"));
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceQuality);
		}
	
	if 	(iVpu2 && !iPlayer2)
		{
		return EFail;
		}
#endif
	if ( (!iVpu2 && !iPlayer) || GetPlayerPriority(iPrioritySettings)!=KErrNone)
		{
		return EFail;
		}
		
	if ( !PlayerStateStopped() ||
		 iPrioritySettings.iPriority != EMdaPriorityMax ||
		 iPrioritySettings.iPref != EMdaPriorityPreferenceQuality )
		 {
		 return EFail;
		 }

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
	// test 4
	INFO_PRINTF1(_L("test 4 Priority: EMdaPriorityMax /Preference: EMdaPriorityPreferenceTime"));
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceTime,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityMax, EMdaPriorityPreferenceTime);
		}
	
	if 	(iVpu2 && !iPlayer2)
		{
		return EFail;
		}
#endif
	if ((!iVpu2 && !iPlayer) || GetPlayerPriority(iPrioritySettings)!=KErrNone)
		{
		return EFail;
		}

	if ( !PlayerStateStopped() ||
		 iPrioritySettings.iPriority != EMdaPriorityMax ||
		 iPrioritySettings.iPref != EMdaPriorityPreferenceTime )
		 {
		 return EFail;
		 }

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
	// test 5
	INFO_PRINTF1(_L("test 5 Priority: EMdaPriorityMin /Preference: EMdaPriorityPreferenceQuality"));
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceQuality);
		}
	
	if 	(iVpu2 && !iPlayer2)
		{
		return EFail;
		}
#endif
	if ((!iVpu2 && !iPlayer) || GetPlayerPriority(iPrioritySettings)!=KErrNone)
		{
		return EFail;
		}

	if ( !PlayerStateStopped() ||
		 iPrioritySettings.iPriority != EMdaPriorityMin ||
		 iPrioritySettings.iPref != EMdaPriorityPreferenceQuality )
		 {
		 return EFail;
		 }

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
	// test 6
	INFO_PRINTF1(_L("test 6 Priority: EMdaPriorityMin /Preference: EMdaPriorityPreferenceTime"));
	if (!iVpu2)
		{
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceTime,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceTime);
		}
	
	if 	(iVpu2 && !iPlayer2)
		{
		return EFail;
		}
#endif
	if ((!iVpu2 && !iPlayer) || GetPlayerPriority(iPrioritySettings)!=KErrNone)
		{	
		return EFail;
		}

	if ( !PlayerStateStopped() ||
		 iPrioritySettings.iPriority != EMdaPriorityMin ||
		 iPrioritySettings.iPref != EMdaPriorityPreferenceTime )
		 {
		 return EFail;
		 }

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

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

