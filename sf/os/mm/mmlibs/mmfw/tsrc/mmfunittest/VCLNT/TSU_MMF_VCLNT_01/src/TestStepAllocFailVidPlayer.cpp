// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32hal.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <hal.h>

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
#include "TestStepAllocFailVidPlayer.h"

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepOOMVidPlayerNew.
 *
 *
 * @return	"CTestStepOOMVidPlayerNew*"
 *			The constructed CTestStepOOMVidPlayerNew

 */
CTestStepOOMVidPlayerNew* CTestStepOOMVidPlayerNew::NewL(TBool aUseSurface)
	{
	CTestStepOOMVidPlayerNew* self = new(ELeave) CTestStepOOMVidPlayerNew(aUseSurface);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.

 */
CTestStepOOMVidPlayerNew::CTestStepOOMVidPlayerNew(TBool aUseSurface) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = aUseSurface;
	if (aUseSurface)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0900-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0100-CP");
		}
	}

/**
 *
 * Test step destructor.

 */
CTestStepOOMVidPlayerNew::~CTestStepOOMVidPlayerNew()
	{
	}

/**
 * Implementation of the MVideoPlayererUtilityObserver interface functions
 **/
void CTestStepOOMVidPlayerNew::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepOOMVidPlayerNew::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepOOMVidPlayerNew::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepOOMVidPlayerNew::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepOOMVidPlayerNew::MvpuoEvent(const TMMFEvent& /*aEvent*/)
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
TVerdict CTestStepOOMVidPlayerNew::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"),&iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility object with NewL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility2 object with NewL()"));
		}
		
	TInt err = KErrNone;
	iError = KErrNone; //KErrTimedOut;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging

	// N.B. we have to call HAL::Get in order to prevent a memory leak being detected
	// between the __UHEAP markers below. This is because HAL::Get allocates some memory on the 
	// first call and is called in CVideoPlayerUtility::NewL (setting up direct screen access).
	// There is no method to deallocate this memory so we allocate it outside of the __UHEAP markers.
	if (!iVpu2)
		{
		TInt address = 0;
		HAL::Get(HALData::EDisplayMemoryAddress, address);
		}

	for(;;)	
		{
		InitWservL();

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		if (!iVpu2)
			{
			TRect rect, clipRect;
			TRAP( err, (iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
														EMdaPriorityPreferenceTimeAndQuality,
														iWs, *iScreen, *iWindow, rect, clipRect)));
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			TRAP( err, (iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal,
														EMdaPriorityPreferenceTimeAndQuality)));
			}
#endif			
		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				{
				User::Free(testAlloc);
				}
				
			// see if valid result and break if wrong - might be premature result
				
			if ( !PlayerStateStopped() )
				{
				badResult = ETrue;
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
			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			}
			
		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		DeInitWserv();
		User::After(2000000);

		if (completed)
			{
			break; // exit loop
			}
			
		failCount++;
		}

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone || badResult)
		{
		if (badResult)
			{	
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
			}
		else
			{	
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
			}
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}	


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepOOMVidPlayerOpenFile.
 *
 *
 * @return	"CTestStepOOMVidPlayerOpenFile*"
 *			The constructed CTestStepOOMVidPlayerOpenFile
 * 
 */
CTestStepOOMVidPlayerOpenFile* CTestStepOOMVidPlayerOpenFile::NewL(TBool aUseFileName, TInt aSurfaceMode)
	{
	CTestStepOOMVidPlayerOpenFile* self = new(ELeave) CTestStepOOMVidPlayerOpenFile(aUseFileName,aSurfaceMode);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepOOMVidPlayerOpenFile::CTestStepOOMVidPlayerOpenFile(TBool aUseFileHandle, TInt aSurfaceMode) 
	{
	iTestStepResult = EPass;
	iUseFileHandle = aUseFileHandle;
	if (aSurfaceMode == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iVpu2 = EFalse;
		if (iUseFileHandle)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0111-CP");
			}
		else
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0101-CP");
			}
		}
	else
		{
		iVpu2 = ETrue;
		if (iUseFileHandle)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0904-CP");
			}
		else
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0901-CP");
			}
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepOOMVidPlayerOpenFile::~CTestStepOOMVidPlayerOpenFile()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepOOMVidPlayerOpenFile::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepOOMVidPlayerOpenFile::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepOOMVidPlayerOpenFile::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepOOMVidPlayerOpenFile::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepOOMVidPlayerOpenFile::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepOOMVidPlayerOpenFile::DoTestStepPreambleL(void)
	{
	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"),&iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility::OpenFileL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility2::OpenFileL()"));
		}
	_LIT(KFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");
	iFileName.Set(KFileName);

	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	if (verdict==EPass)
		{
		InitWservL();

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
		if ( !PlayerStateStopped() )
			{
			return EInconclusive;
			}
		}

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepOOMVidPlayerOpenFile::DoTestStepPostambleL(void)
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

void CTestStepOOMVidPlayerOpenFile::OpenFileAndStartSchedulerL()
	{
	if (iUseFileHandle && !iVpu2)
		{
		iPlayer->OpenFileL(iFile);
		}
#ifdef SYMBIAN_BUILD_GCE
	else if (iUseFileHandle && iVpu2)
		{
		iPlayer2->OpenFileL(iFile);
		}
	else if (!iUseFileHandle && iVpu2)
		{
		iPlayer2->OpenFileL(iFileName);
		}
#endif
	else 
		{
		iPlayer->OpenFileL(iFileName);
		}
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
TVerdict CTestStepOOMVidPlayerOpenFile::DoTestStepL()
	{
	TInt err = KErrNone;
	iError = KErrNone; //KErrTimedOut;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging

	// NB We can't call __MM_HEAP_MARKEND at the end of each step because 
	// something (CDirectScreenAccess ?) allocates memory during opening
	// but at least we can check it gets cleaned up when the player is deleted
	__MM_HEAP_MARK;
	
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));

	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		TRAP( err, OpenFileAndStartSchedulerL() );
		
		completed = EFalse;
		if ((err == KErrNone) && (iError == KErrNone))
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				{
				User::Free(testAlloc);
				}
				
			// see if valid result and break if wrong - might be premature result
			//check possibly in another way if source is open

			if (!PlayerStateStopped() )
				{
				badResult = ETrue;
				}
			
			if(iPlayer)
				{
				iPlayer->Close();
				}
#ifdef SYMBIAN_BUILD_GCE
			if(iPlayer2)
				{
				iPlayer2->Close();
				}
#endif
			completed = reachedEnd || badResult;
			}
		else if (((err != KErrNone) && (err != KErrNoMemory)) || 
				((iError != KErrNone) && (iError != KErrNoMemory))) // bad error code
			{
			completed = ETrue;
			}

		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			{
			break; // exit loop
			}

		failCount++;
		}


	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.
	delete iPlayer;
	iPlayer = NULL;
	
	iFile.Close();
	iFs.Close();

	__MM_HEAP_MARKEND;
	if (err != KErrNone || badResult)
		{
		if (badResult)
			{
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
			}
		else
			{
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
			}
		iTestStepResult = EFail;
		}
			
	 
	if (err == KErrNone && !badResult)
		{
		INFO_PRINTF2(_L("Test Completed OK with %d memory allocations tested"), failCount);		
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepOOMVidPlayerOpenDesc.
 *
 *
 * @return	"CTestStepOOMVidPlayerOpenDesc*"
 *			The constructed CTestStepOOMVidPlayerOpenDesc
 */
CTestStepOOMVidPlayerOpenDesc* CTestStepOOMVidPlayerOpenDesc::NewL(TInt aSurfaceMode)
	{
	CTestStepOOMVidPlayerOpenDesc* self = new(ELeave) CTestStepOOMVidPlayerOpenDesc(aSurfaceMode);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepOOMVidPlayerOpenDesc::CTestStepOOMVidPlayerOpenDesc(TInt aSurfaceMode) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	if (aSurfaceMode == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iVpu2 = EFalse;
		iTestStepName = _L("MM-MMF-VCLNT-U-0102-CP");
		}
	else 
		{
		iVpu2 = ETrue;
		iTestStepName = _L("MM-MMF-VCLNT-U-0902-CP");
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepOOMVidPlayerOpenDesc::~CTestStepOOMVidPlayerOpenDesc()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepOOMVidPlayerOpenDesc::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepOOMVidPlayerOpenDesc::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepOOMVidPlayerOpenDesc::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepOOMVidPlayerOpenDesc::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepOOMVidPlayerOpenDesc::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepOOMVidPlayerOpenDesc::DoTestStepPreambleL(void)
	{
	_LIT(KFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\smallv.wav");

	iFileName.Set(KFileName);

	TInt fSize;
	iFs.Connect();
	if ( iFile.Open(iFs, iFileName, EFileRead) != KErrNone )
		{
		return EInconclusive;
		}
	iFile.Size(fSize);
	// load the sound from the file to a Descriptor
	iBuf = HBufC8::NewL( fSize );
	TPtr8 des = iBuf->Des();
	iFile.Read(des);

	iFile.Close();
	iFs.Close();
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepOOMVidPlayerOpenDesc::DoTestStepPostambleL(void)
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

void CTestStepOOMVidPlayerOpenDesc::OpenDescAndStartSchedulerL()
	{
	if (!iVpu2)
		{
		iPlayer->OpenDesL(iBuf->Des());
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->OpenDesL(iBuf->Des());
		}
#endif
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
TVerdict CTestStepOOMVidPlayerOpenDesc::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"),&iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility::OpenDesL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility2::OpenDesL()"));
		}
	
	iError = KErrNone; //KErrTimedOut;

	InitWservL();

	// N.B. we have to call HAL::Get in order to prevent a memory leak being detected
	// between the __UHEAP markers below. This is because HAL::Get allocates some memory on the 
	// first call and is called in CVideoPlayerUtility::NewL (setting up direct screen access).
	// There is no method to deallocate this memory so we allocate it outside of the __UHEAP markers.
	TInt address = 0;
	HAL::Get(HALData::EDisplayMemoryAddress, address);

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
	if (!PlayerStateStopped() )
		{
		return EInconclusive;
		}
	
	TRAP(err, OpenDescAndStartSchedulerL() );
	if (err == KErrNotFound)
		{
		iTestStepResult = EInconclusive;
		}
	else if (err == KErrNotSupported)
		{
		iTestStepResult = EPass;
		}
	else if (err != KErrNone) 
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
	__MM_HEAP_MARKEND;

	DeInitWserv();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepOOMVidPlayerOpenUrl.
 *
 *
 * @return	"CTestStepOOMVidPlayerOpenUrl*"
 *			The constructed CTestStepOOMVidPlayerOpenUrl
 */
CTestStepOOMVidPlayerOpenUrl* CTestStepOOMVidPlayerOpenUrl::NewL(TInt aSurfaceMode)
	{
	CTestStepOOMVidPlayerOpenUrl* self = new(ELeave) CTestStepOOMVidPlayerOpenUrl(aSurfaceMode);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepOOMVidPlayerOpenUrl::CTestStepOOMVidPlayerOpenUrl(TInt aSurfaceMode) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	if (aSurfaceMode == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iVpu2 = EFalse;
		iTestStepName = _L("MM-MMF-VCLNT-U-0103-CP");
		}
	else 
		{
		iVpu2 = ETrue;
		iTestStepName = _L("MM-MMF-VCLNT-U-0903-CP");
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepOOMVidPlayerOpenUrl::~CTestStepOOMVidPlayerOpenUrl()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepOOMVidPlayerOpenUrl::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepOOMVidPlayerOpenUrl::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepOOMVidPlayerOpenUrl::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepOOMVidPlayerOpenUrl::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

	
void CTestStepOOMVidPlayerOpenUrl::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepOOMVidPlayerOpenUrl::DoTestStepPreambleL(void)
	{
	_LIT(KFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\small2.wav");

	iFileName.Set(KFileName);
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepOOMVidPlayerOpenUrl::DoTestStepPostambleL(void)
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

void CTestStepOOMVidPlayerOpenUrl::OpenUrlAndStartSchedulerL()
	{
	if (!iVpu2)
		{
		iPlayer->OpenUrlL(iFileName, KUseDefaultIap, _L8("audio/wav"));
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->OpenUrlL(iFileName, KUseDefaultIap, _L8("audio/wav"));
		}
#endif
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
TVerdict CTestStepOOMVidPlayerOpenUrl::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	// N.B. we have to call HAL::Get in order to prevent a memory leak being detected
	// between the __UHEAP markers below. This is because HAL::Get allocates some memory on the 
	// first call and is called in CVideoPlayerUtility::NewL (setting up direct screen access).
	// There is no method to deallocate this memory so we allocate it outside of the __UHEAP markers.
	TInt address = 0;
	HAL::Get(HALData::EDisplayMemoryAddress, address);

	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"),&iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility::OpenUrlL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is an Out of Memory test of CVideoPlayerUtility2::OpenUrlL()"));
		}

	iError = KErrNone; //KErrTimedOut;

	InitWservL();

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
	if ( !PlayerStateStopped() )
		{
		return EInconclusive;
		}

	TRAP(err, OpenUrlAndStartSchedulerL() );
	if (err == KErrNotFound)
		{
		iTestStepResult = EInconclusive;
		}
	else if (err == KErrNotSupported)
		{
		iTestStepResult = EPass;
		}
	else if (err != KErrNone) 
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

