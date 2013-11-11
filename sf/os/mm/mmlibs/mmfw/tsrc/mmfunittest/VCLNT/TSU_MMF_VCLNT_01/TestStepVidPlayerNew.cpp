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

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#endif
#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidPlayer.h"



// --------------------------------------------

/**
 *
 * Static constructor for CTestStepVidPlayerNew.
 *
 *
 * @return	"CTestStepVidPlayerNew*"
 *			The constructed CTestStepVidPlayerNew
 */
CTestStepVidPlayerNew* CTestStepVidPlayerNew::NewL(TBool aUseSurface)
	{
	CTestStepVidPlayerNew* self = new(ELeave) CTestStepVidPlayerNew(aUseSurface);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerNew::CTestStepVidPlayerNew(TBool aUseSurface) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = aUseSurface;
	if (aUseSurface)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0801-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0001-CP");
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerNew::~CTestStepVidPlayerNew()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerNew::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerNew::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerNew::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidPlayerNew::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepVidPlayerNew::MvpuoEvent(const TMMFEvent& /*aEvent*/)
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
TVerdict CTestStepVidPlayerNew::DoTestStepL()
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
	INFO_PRINTF2(_L("%S"),&iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this test is initialising a CVideoPlayerUtility object with NewL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this test is initialising a CVideoPlayerUtility2 object with NewL()"));
		}
	
	iError = KErrNone; 

	InitWservL();

	__MM_HEAP_MARK;
	
	if (!iVpu2)
		{
		TRect rect, clipRect;
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		}
#endif	
	if (!iVpu2)
		{	
		if ( !PlayerStateStopped() )
			{
			INFO_PRINTF1(_L("Player not in stopped state or iPlayer NULL"));
			return EFail;
			}

		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		if ( !PlayerStateStopped() )
			{
			INFO_PRINTF1(_L("Player not in stopped state or iPlayer2 NULL"));
			return EFail;
			}

		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif // SYMBIAN_BUILD_GCE
		
	__MM_HEAP_MARKEND;

	DeInitWserv();

	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

