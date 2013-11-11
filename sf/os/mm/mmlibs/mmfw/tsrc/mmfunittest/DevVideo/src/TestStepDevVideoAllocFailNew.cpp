// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the various 
// ALLOC tests for DevVideo
// 
//

// Test system includes
#include <testframework.h>

#include "TestDevVideoPlugins/decoder.h"
#include "TestDevVideoPlugins/postproc.h"

#include "TestDevVideoPlugins/encoder.h"
#include "TestDevVideoPlugins/preproc.h"

#include "TestStepDevVideoAllocFailNew.h"
#include "TestDevVideoPlayTestData.h"


// --------------------------------------------

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 */
CTestStepDevVideoPlayAllocFail::CTestStepDevVideoPlayAllocFail(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 * 
 */
CTestStepDevVideoPlayAllocFail::~CTestStepDevVideoPlayAllocFail()
	{
	}

// MMMFDevVideoPlayObserver
void CTestStepDevVideoPlayAllocFail::MdvpoNewBuffers()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoNewBuffers()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoReturnPicture(TVideoPicture* /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoReturnPicture()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoSupplementalInformation(const TDesC8& /*aData*/, 
								  const TTimeIntervalMicroSeconds& /*aTimestamp*/, 
								  const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoSupplementalInformation()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoPictureLoss()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoPictureLoss()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoPictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoPictureLoss(TArray)"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoSliceLoss()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoReferencePictureSelection()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoTimedSnapshotComplete(TInt /*aError*/, 
								TPictureData* /*aPictureData*/, 
								const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
								const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoTimedSnapshotComplete()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoNewPictures()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoNewPictures()"));
	}

void CTestStepDevVideoPlayAllocFail::MdvpoFatalError(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestStepDevVideoPlayAllocFail::MdvpoFatalError()\nError = %d"), aError);
	}

void CTestStepDevVideoPlayAllocFail::MdvpoInitComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestStepDevVideoPlayAllocFail::MdvpoInitComplete():  Error = %d"), aError);
	}

void CTestStepDevVideoPlayAllocFail::MdvpoStreamEnd()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoPlayAllocFail::MdvpoStreamEnd()"));
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
TVerdict CTestStepDevVideoPlayAllocFail::DoTestStepL()
	{
	TVerdict ret = EFail;

	// do we want to do anything else in the base class?

	// call alloc test step
	ret = DoTestL();

	return	ret;
	}


//----------------------------------------------------------------------------

CTestStepDevVideoPlayAllocFailNew::CTestStepDevVideoPlayAllocFailNew(const TDesC& aTestName)
	:CTestStepDevVideoPlayAllocFail(aTestName)
	{
	}

CTestStepDevVideoPlayAllocFailNew* CTestStepDevVideoPlayAllocFailNew::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoPlayAllocFailNew* self = new(ELeave) CTestStepDevVideoPlayAllocFailNew(aTestName);
	return self;
	}

CTestStepDevVideoPlayAllocFailNew::~CTestStepDevVideoPlayAllocFailNew()
	{
	}

TVerdict CTestStepDevVideoPlayAllocFailNew::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of initialising CMMFDevVideoPlay with NewL()"));

	__UHEAP_MARK;
	TRAP( err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this) );
	delete iDevVideoPlay;
	iDevVideoPlay = NULL;
	__UHEAP_MARKEND;

	if (err != KErrNone)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this);
				   iDevVideoPlay->SelectDecoderL(KUidDevVideoTestDecodeHwDevice);
				   iDevVideoPlay->SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice); );

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		if (iDevVideoPlay)
			{
			delete iDevVideoPlay;
			iDevVideoPlay = NULL;
			}
		completed = reachedEnd;
		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	if (iDevVideoPlay)
		{
		delete iDevVideoPlay;
		iDevVideoPlay = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//----------------------------------------------------------------------------


CTestStepDevVideoPlayAllocFailFindPlugins::CTestStepDevVideoPlayAllocFailFindPlugins(const TDesC& aTestName)
	:CTestStepDevVideoPlayAllocFail(aTestName)
	{
	}

CTestStepDevVideoPlayAllocFailFindPlugins* CTestStepDevVideoPlayAllocFailFindPlugins::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoPlayAllocFailFindPlugins* self = new(ELeave) CTestStepDevVideoPlayAllocFailFindPlugins(aTestName);
	return self;
	}

CTestStepDevVideoPlayAllocFailFindPlugins::~CTestStepDevVideoPlayAllocFailFindPlugins()
	{
	}

TVerdict CTestStepDevVideoPlayAllocFailFindPlugins::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of finding decoder and post processor plugins"));


	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		err = KErrNone;

		if (!err)
			TRAP(err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
		if (!err)
			TRAP(err, GetDecodersL());
		if (!err)
			TRAP(err, GetPostProcessorsL());
		if (!err)
			TRAP(err, FindDecodersL());
		if (!err)
			TRAP(err, FindPostProcessorsL());

		REComSession::FinalClose();

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		delete iDevVideoPlay;
		iDevVideoPlay = NULL;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed || reachedEnd)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	delete iDevVideoPlay;
	iDevVideoPlay = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

void CTestStepDevVideoPlayAllocFailFindPlugins::CheckPluginsL(const RArray<TUid>& aFoundPlugins, const RArray<TUid>& aExpectedPlugins, const RArray<TUid>& aUnExpectedPlugins)
	{
	TBool foundUnExpected = EFalse;
	TInt numberToBeFoundInExpectedPlugins = aExpectedPlugins.Count();

	for (TInt i=0; i<aFoundPlugins.Count(); i++)
		{
		TUid uidToLookFor = aFoundPlugins[i];
		TInt j;
		for (j=0; j<aExpectedPlugins.Count(); j++)
			{
			if (aExpectedPlugins[j] == uidToLookFor)
				{
				numberToBeFoundInExpectedPlugins--;
				break;//j
				}
			}
		for (j=0; j<aUnExpectedPlugins.Count(); j++)
			{
			if (aUnExpectedPlugins[j] == uidToLookFor)
				{
				foundUnExpected = ETrue;
				break;//j
				}
			}
		}

	if ((foundUnExpected) || (numberToBeFoundInExpectedPlugins!=0))
		User::Leave(KErrGeneral);

	}


void CTestStepDevVideoPlayAllocFailFindPlugins::FindDecodersL()
	{
	RArray<TUid> foundDecoders;
	CleanupClosePushL(foundDecoders);
	RArray<TUid> expectedDecoders;
	CleanupClosePushL(expectedDecoders);
	RArray<TUid> unExpectedDecoders;
	CleanupClosePushL(unExpectedDecoders);

	
	// Supported mime type and post proc type (by exact match)
	_LIT8(KMimeType5, "video/zippyvideo");
	TUint32 requestedPostProcType = EPpMirror|EPpInputCrop;
	foundDecoders.Reset();
	expectedDecoders.Reset();
	unExpectedDecoders.Reset();
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
	User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
	User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
	User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
	User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

	iDevVideoPlay->FindDecodersL(KMimeType5(), requestedPostProcType, foundDecoders, ETrue);
	CheckPluginsL(foundDecoders, expectedDecoders, unExpectedDecoders);

	CleanupStack::PopAndDestroy(3);
	}

void CTestStepDevVideoPlayAllocFailFindPlugins::FindPostProcessorsL()
	{
	RArray<TUid> foundPostProx;
	CleanupClosePushL(foundPostProx);
	RArray<TUid> expectedPostProx;
	CleanupClosePushL(expectedPostProx);
	RArray<TUid> unExpectedPostProx;
	CleanupClosePushL(unExpectedPostProx);

	TUint32	requestedPostProcType = EPpMirror|EPpInputCrop;
	foundPostProx.Reset();
	expectedPostProx.Reset();
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
	User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
	User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
	User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));

	iDevVideoPlay->FindPostProcessorsL(requestedPostProcType, foundPostProx);
	CheckPluginsL(foundPostProx, expectedPostProx, unExpectedPostProx);

	CleanupStack::PopAndDestroy(3);
	}

void CTestStepDevVideoPlayAllocFailFindPlugins::GetDecodersL()
	{
	RArray<TUid> foundDecoders;
	CleanupClosePushL(foundDecoders);
	RArray<TUid> expectedDecoders;
	CleanupClosePushL(expectedDecoders);
	RArray<TUid> unExpectedDecoders;
	CleanupClosePushL(unExpectedDecoders);

	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

	iDevVideoPlay->GetDecoderListL(foundDecoders);

	CheckPluginsL(foundDecoders, expectedDecoders, unExpectedDecoders);

	CleanupStack::PopAndDestroy(3); //foundDecoders, expectedDecoders, unExpectedDecoders
	}

void CTestStepDevVideoPlayAllocFailFindPlugins::GetPostProcessorsL()
	{
	RArray<TUid> foundPostProx;
	CleanupClosePushL(foundPostProx);
	RArray<TUid> expectedPostProx;
	CleanupClosePushL(expectedPostProx);
	RArray<TUid> unExpectedPostProx;
	CleanupClosePushL(unExpectedPostProx);

	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));

	iDevVideoPlay->GetPostProcessorListL(foundPostProx);

	CheckPluginsL(foundPostProx, expectedPostProx, unExpectedPostProx);

	CleanupStack::PopAndDestroy(3);//foundPostProx, expectedPostProx, unExpectedPostProx
	}

//----------------------------------------------------------------------------

CTestStepDevVideoPlayAllocFailInfoDecoder::CTestStepDevVideoPlayAllocFailInfoDecoder(const TDesC& aTestName)
	:CTestStepDevVideoPlayAllocFail(aTestName)
	{
	}

CTestStepDevVideoPlayAllocFailInfoDecoder* CTestStepDevVideoPlayAllocFailInfoDecoder::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoPlayAllocFailInfoDecoder* self = new(ELeave) CTestStepDevVideoPlayAllocFailInfoDecoder(aTestName);
	return self;
	}

CTestStepDevVideoPlayAllocFailInfoDecoder::~CTestStepDevVideoPlayAllocFailInfoDecoder()
	{
	}

TVerdict CTestStepDevVideoPlayAllocFailInfoDecoder::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of VideoDecoderInfoLC()"));

	// moved creation and destruction of iDevVideoPlay inside the heap marks.
	// reason :- VideoDecoderInfoLC may allocate onto it
	
	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP (err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
		if (err == KErrNone)
			{

			CVideoDecoderInfo* vidInfo = NULL;

			TRAP( err, vidInfo = iDevVideoPlay->VideoDecoderInfoLC(KUidDevVideoTestDecodeHwDevice);
					   CleanupStack::Pop(vidInfo); );

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
					User::Free(testAlloc);
			
				}
			else if (err != KErrNoMemory) // bad error code
				completed = ETrue;

			// see if valid result and break if wrong - might be premature result
		
			if (vidInfo)
				{
				delete vidInfo;
				vidInfo = NULL;
				}
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;
			
		if (iDevVideoPlay)
			{
			delete iDevVideoPlay;
			iDevVideoPlay = NULL;
			}

		completed = reachedEnd;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//----------------------------------------------------------------------------

CTestStepDevVideoPlayAllocFailInfoPostProc::CTestStepDevVideoPlayAllocFailInfoPostProc(const TDesC& aTestName)
	:CTestStepDevVideoPlayAllocFail(aTestName)
	{
	}

CTestStepDevVideoPlayAllocFailInfoPostProc* CTestStepDevVideoPlayAllocFailInfoPostProc::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoPlayAllocFailInfoPostProc* self = new(ELeave) CTestStepDevVideoPlayAllocFailInfoPostProc(aTestName);
	return self;
	}

CTestStepDevVideoPlayAllocFailInfoPostProc::~CTestStepDevVideoPlayAllocFailInfoPostProc()
	{
	}

TVerdict CTestStepDevVideoPlayAllocFailInfoPostProc::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of PostProcessorInfoLC()"));

	// will leave on error
	TRAP (err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - Couldn't construct CMMFDevVideoPlay!"));
		return EInconclusive;
		}

	CPostProcessorInfo* postInfo = NULL;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, postInfo = iDevVideoPlay->PostProcessorInfoLC(KUidDevVideoTestPostProcHwDevice);
				   CleanupStack::Pop(postInfo); );

		REComSession::FinalClose();

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		
		if (postInfo)
			{
			delete postInfo;
			postInfo = NULL;
			}

		completed = reachedEnd;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	if (iDevVideoPlay)
		{
		delete iDevVideoPlay;
		iDevVideoPlay = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//----------------------------------------------------------------------------


CTestStepDevVideoPlayAllocFailGetPictures::CTestStepDevVideoPlayAllocFailGetPictures(const TDesC& aTestName)
	:CTestStepDevVideoPlayAllocFail(aTestName)
	{
	}

CTestStepDevVideoPlayAllocFailGetPictures* CTestStepDevVideoPlayAllocFailGetPictures::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoPlayAllocFailGetPictures* self = new(ELeave) CTestStepDevVideoPlayAllocFailGetPictures(aTestName);
	return self;
	}

CTestStepDevVideoPlayAllocFailGetPictures::~CTestStepDevVideoPlayAllocFailGetPictures()
	{
	}

TVerdict CTestStepDevVideoPlayAllocFailGetPictures::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of retrieving and returning pictures to the plugin"));


	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP(err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
		if (!err)
			TRAP(err, PerformTestL());

		REComSession::FinalClose();

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		delete iDevVideoPlay;
		iDevVideoPlay = NULL;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed || reachedEnd)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	delete iDevVideoPlay;
	iDevVideoPlay = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


void CTestStepDevVideoPlayAllocFailGetPictures::PerformTestL()
	{
	iTestError = KErrNone;

	// select decoder
	iDevVideoPlay->SelectDecoderL(KUidDevVideoTestDecodeHwDevice);

	// select post-processor
	iDevVideoPlay->SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice);

	//Tell plugin to start creating pictures.
	iDevVideoPlay->SetVideoDestScreenL(EFalse);

	// initialize CDevVideoPlay and wait for response
	iDevVideoPlay->Initialize();

	User::LeaveIfError(iTestError);

	iDevVideoPlay->Start();//Tell plugin to start creating pictures.
	// We should now get a series of NewPicture() callbacks.

	User::LeaveIfError(iTestError);
	}

void CTestStepDevVideoPlayAllocFailGetPictures::MdvpoInitComplete(TInt aError)
	{
	iTestError = aError;
	}

void CTestStepDevVideoPlayAllocFailGetPictures::MdvpoNewPictures()
	{
	TRAP(iTestError, DoNewPicturesL());
	}

void CTestStepDevVideoPlayAllocFailGetPictures::CheckNewPictureInfoL(TUint aNumPictures, const TTimeIntervalMicroSeconds& aFirst, const TTimeIntervalMicroSeconds& aLast)
	{
	TUint numPictures = 0;
	TTimeIntervalMicroSeconds first(0);
	TTimeIntervalMicroSeconds last(0);
	iDevVideoPlay->GetNewPictureInfo(numPictures, first, last);

	if ((numPictures!=aNumPictures) || (first!=aFirst) || (last!=aLast))
		{
		User::Leave(KErrGeneral);
		}
	}

void CTestStepDevVideoPlayAllocFailGetPictures::DoNewPicturesL()
	{
	User::LeaveIfError(iTestError);//Don't go any further if we have an error
	switch (iCurrentPictureNumber)
		{
		case 0:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 1:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(1), TTimeIntervalMicroSeconds(1));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 2:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(2), TTimeIntervalMicroSeconds(2));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 3:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(3), TTimeIntervalMicroSeconds(3));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 4:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(4), TTimeIntervalMicroSeconds(4));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 5:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(5), TTimeIntervalMicroSeconds(5));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 6:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(6));
			break;
			}
		case 7:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(2, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(7));
			break;
			}
		case 8:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(3, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(8));
			break;
			}
		case 9:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(4, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(9));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(3, TTimeIntervalMicroSeconds(7), TTimeIntervalMicroSeconds(9));
			picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(2, TTimeIntervalMicroSeconds(8), TTimeIntervalMicroSeconds(9));
			picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(9), TTimeIntervalMicroSeconds(9));
			picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			break;
			}
		default:
			User::Leave(KErrGeneral);
			break;
		};
	}


//----------------------------------------------------------------------------

// DEVVIDEORECORD //

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * 
 */
CTestStepDevVideoRecordAllocFail::CTestStepDevVideoRecordAllocFail(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * 
 */
CTestStepDevVideoRecordAllocFail::~CTestStepDevVideoRecordAllocFail()
	{
	}

// MMMFDevVideoRecordObserver
void CTestStepDevVideoRecordAllocFail::MdvroNewBuffers()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoRecordAllocFail::MdvroNewBuffers()"));
	}

void CTestStepDevVideoRecordAllocFail::MdvroReturnPicture(TVideoPicture* /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestStepDevVideoRecordAllocFail::MdvroReturnPicture()"));
	}

void CTestStepDevVideoRecordAllocFail::MdvroSupplementalInfoSent()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoRecordAllocFail::MdvroSupplementalInfoSent()"));
	}

void CTestStepDevVideoRecordAllocFail::MdvroFatalError(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestStepDevVideoRecordAllocFail::MdvroFatalError()\nError = %d"), aError);
	}

void CTestStepDevVideoRecordAllocFail::MdvroInitializeComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestStepDevVideoRecordAllocFail::MdvroInitializeComplete():  Error = %d"), aError);
	}

void CTestStepDevVideoRecordAllocFail::MdvroStreamEnd()
	{
	INFO_PRINTF1(_L("CTestStepDevVideoRecordAllocFail::MdvroStreamEnd()"));
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
TVerdict CTestStepDevVideoRecordAllocFail::DoTestStepL()
	{
	TVerdict ret = EFail;

	// call alloc test step
	ret = DoTestL();

	return	ret;
	}

//----------------------------------------------------------------------------

CTestStepDevVideoRecordAllocFailNew::CTestStepDevVideoRecordAllocFailNew(const TDesC& aTestName)
	:CTestStepDevVideoRecordAllocFail(aTestName)
	{
	}

CTestStepDevVideoRecordAllocFailNew* CTestStepDevVideoRecordAllocFailNew::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoRecordAllocFailNew* self = new(ELeave) CTestStepDevVideoRecordAllocFailNew(aTestName);
	return self;
	}

CTestStepDevVideoRecordAllocFailNew::~CTestStepDevVideoRecordAllocFailNew()
	{
	}

TVerdict CTestStepDevVideoRecordAllocFailNew::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of initialising CMMFDevVideoRecord with NewL()"));

	__MM_HEAP_MARK;
	TRAP( err, iDevVideoRecord = CMMFDevVideoRecord::NewL(*this) );
	delete iDevVideoRecord;
	iDevVideoRecord = NULL;
	__MM_HEAP_MARKEND;

	if (err != KErrNone)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, iDevVideoRecord = CMMFDevVideoRecord::NewL(*this);
				   iDevVideoRecord->SelectEncoderL(KUidDevVideoTestEncodeHwDevice);
				   iDevVideoRecord->SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice); );

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		if (iDevVideoRecord)
			{
			delete iDevVideoRecord;
			iDevVideoRecord = NULL;
			}
		completed = reachedEnd;
		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	if (iDevVideoRecord)
		{
		delete iDevVideoRecord;
		iDevVideoRecord = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//----------------------------------------------------------------------------

CTestStepDevVideoRecordAllocFailInfoEncoder::CTestStepDevVideoRecordAllocFailInfoEncoder(const TDesC& aTestName)
	:CTestStepDevVideoRecordAllocFail(aTestName)
	{
	}

CTestStepDevVideoRecordAllocFailInfoEncoder* CTestStepDevVideoRecordAllocFailInfoEncoder::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoRecordAllocFailInfoEncoder* self = new(ELeave) CTestStepDevVideoRecordAllocFailInfoEncoder(aTestName);
	return self;
	}

CTestStepDevVideoRecordAllocFailInfoEncoder::~CTestStepDevVideoRecordAllocFailInfoEncoder()
	{
	}

TVerdict CTestStepDevVideoRecordAllocFailInfoEncoder::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of VideoEncoderInfoLC()"));

	// moved creation and destruction of iDevVideoRecord inside the heap marks.
	// reason :- VideoEncoderInfoLC may allocate onto it
	
	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP (err, iDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
	
		if (err == KErrNone)
			{
			CVideoEncoderInfo* vidInfo = NULL;

			TRAP( err, vidInfo = iDevVideoRecord->VideoEncoderInfoLC(KUidDevVideoTestEncodeHwDevice);
					   CleanupStack::Pop(vidInfo); );

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
					User::Free(testAlloc);
			
				}
			else if (err != KErrNoMemory) // bad error code
				completed = ETrue;

			// see if valid result and break if wrong - might be premature result
		
			if (vidInfo)
				{
				delete vidInfo;
				vidInfo = NULL;
				}
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;
			
		if (iDevVideoRecord)
			{
			delete iDevVideoRecord;
			iDevVideoRecord = NULL;
			}

		completed = reachedEnd;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//----------------------------------------------------------------------------

CTestStepDevVideoRecordAllocFailInfoPreProc::CTestStepDevVideoRecordAllocFailInfoPreProc(const TDesC& aTestName)
	:CTestStepDevVideoRecordAllocFail(aTestName)
	{
	}

CTestStepDevVideoRecordAllocFailInfoPreProc* CTestStepDevVideoRecordAllocFailInfoPreProc::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoRecordAllocFailInfoPreProc* self = new(ELeave) CTestStepDevVideoRecordAllocFailInfoPreProc(aTestName);
	return self;
	}

CTestStepDevVideoRecordAllocFailInfoPreProc::~CTestStepDevVideoRecordAllocFailInfoPreProc()
	{
	}

TVerdict CTestStepDevVideoRecordAllocFailInfoPreProc::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of PreProcessorInfoLC()"));

	// will leave on error
	TRAP (err, iDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - Couldn't construct CMMFDevVideoRecord!"));
		return EInconclusive;
		}

	CPreProcessorInfo* preInfo = NULL;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, preInfo = iDevVideoRecord->PreProcessorInfoLC(KUidDevVideoTestPreProcHwDevice);
				   CleanupStack::Pop(preInfo); );

		REComSession::FinalClose();

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		
		if (preInfo)
			{
			delete preInfo;
			preInfo = NULL;
			}

		completed = reachedEnd;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	if (iDevVideoRecord)
		{
		delete iDevVideoRecord;
		iDevVideoRecord = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//----------------------------------------------------------------------------


CTestStepDevVideoRecordAllocFailFindPlugins::CTestStepDevVideoRecordAllocFailFindPlugins(const TDesC& aTestName)
	:CTestStepDevVideoRecordAllocFail(aTestName)
	{
	}

CTestStepDevVideoRecordAllocFailFindPlugins* CTestStepDevVideoRecordAllocFailFindPlugins::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoRecordAllocFailFindPlugins* self = new(ELeave) CTestStepDevVideoRecordAllocFailFindPlugins(aTestName);
	return self;
	}

CTestStepDevVideoRecordAllocFailFindPlugins::~CTestStepDevVideoRecordAllocFailFindPlugins()
	{
	}

TVerdict CTestStepDevVideoRecordAllocFailFindPlugins::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of finding Encoder and Pre processor plugins"));


	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		err = KErrNone;

		if (!err)
			TRAP(err, iDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
		if (!err)
			TRAP(err, GetEncodersL());
		if (!err)
			TRAP(err, GetPreProcessorsL());
		if (!err)
			TRAP(err, FindEncodersL());
		if (!err)
			TRAP(err, FindPreProcessorsL());

		REComSession::FinalClose();

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		delete iDevVideoRecord;
		iDevVideoRecord = NULL;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed || reachedEnd)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	delete iDevVideoRecord;
	iDevVideoRecord = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

void CTestStepDevVideoRecordAllocFailFindPlugins::CheckPluginsL(const RArray<TUid>& aFoundPlugins, const RArray<TUid>& aExpectedPlugins, const RArray<TUid>& aUnExpectedPlugins)
	{
	TBool foundUnExpected = EFalse;
	TInt numberToBeFoundInExpectedPlugins = aExpectedPlugins.Count();

	for (TInt i=0; i<aFoundPlugins.Count(); i++)
		{
		TUid uidToLookFor = aFoundPlugins[i];
		TInt j;
		for (j=0; j<aExpectedPlugins.Count(); j++)
			{
			if (aExpectedPlugins[j] == uidToLookFor)
				{
				numberToBeFoundInExpectedPlugins--;
				break;//j
				}
			}
		for (j=0; j<aUnExpectedPlugins.Count(); j++)
			{
			if (aUnExpectedPlugins[j] == uidToLookFor)
				{
				foundUnExpected = ETrue;
				break;//j
				}
			}
		}

	if ((foundUnExpected) || (numberToBeFoundInExpectedPlugins!=0))
		User::Leave(KErrGeneral);

	}


void CTestStepDevVideoRecordAllocFailFindPlugins::FindEncodersL()
	{
	RArray<TUid> foundEncoders;
	CleanupClosePushL(foundEncoders);
	RArray<TUid> expectedEncoders;
	CleanupClosePushL(expectedEncoders);
	RArray<TUid> unExpectedEncoders;
	CleanupClosePushL(unExpectedEncoders);

	
	// Supported mime type and Pre proc type (by exact match)
	_LIT8(KMimeType5, "video/zippyvideo");
	TUint32 requestedPreProcType = EPpMirror|EPpInputCrop;
	foundEncoders.Reset();
	expectedEncoders.Reset();
	unExpectedEncoders.Reset();
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
	User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
	User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
	User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
	User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

	iDevVideoRecord->FindEncodersL(KMimeType5(), requestedPreProcType, foundEncoders, ETrue);
	CheckPluginsL(foundEncoders, expectedEncoders, unExpectedEncoders);

	CleanupStack::PopAndDestroy(3);
	}

void CTestStepDevVideoRecordAllocFailFindPlugins::FindPreProcessorsL()
	{
	RArray<TUid> foundPreProx;
	CleanupClosePushL(foundPreProx);
	RArray<TUid> expectedPreProx;
	CleanupClosePushL(expectedPreProx);
	RArray<TUid> unExpectedPreProx;
	CleanupClosePushL(unExpectedPreProx);

	TUint32	requestedPreProcType = EPpMirror|EPpInputCrop;
	foundPreProx.Reset();
	expectedPreProx.Reset();
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
	User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
	User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
	User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));

	iDevVideoRecord->FindPreProcessorsL(requestedPreProcType, foundPreProx);
	CheckPluginsL(foundPreProx, expectedPreProx, unExpectedPreProx);

	CleanupStack::PopAndDestroy(3);
	}

void CTestStepDevVideoRecordAllocFailFindPlugins::GetEncodersL()
	{
	RArray<TUid> foundEncoders;
	CleanupClosePushL(foundEncoders);
	RArray<TUid> expectedEncoders;
	CleanupClosePushL(expectedEncoders);
	RArray<TUid> unExpectedEncoders;
	CleanupClosePushL(unExpectedEncoders);

	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

	iDevVideoRecord->GetEncoderListL(foundEncoders);

	CheckPluginsL(foundEncoders, expectedEncoders, unExpectedEncoders);

	CleanupStack::PopAndDestroy(3); //foundEncoders, expectedEncoders, unExpectedEncoders
	}

void CTestStepDevVideoRecordAllocFailFindPlugins::GetPreProcessorsL()
	{
	RArray<TUid> foundPreProx;
	CleanupClosePushL(foundPreProx);
	RArray<TUid> expectedPreProx;
	CleanupClosePushL(expectedPreProx);
	RArray<TUid> unExpectedPreProx;
	CleanupClosePushL(unExpectedPreProx);

	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));

	iDevVideoRecord->GetPreProcessorListL(foundPreProx);

	CheckPluginsL(foundPreProx, expectedPreProx, unExpectedPreProx);

	CleanupStack::PopAndDestroy(3);//foundPreProx, expectedPreProx, unExpectedPreProx
	}

//----------------------------------------------------------------------------


CTestStepDevVideoRecordAllocFailGetBuffers::CTestStepDevVideoRecordAllocFailGetBuffers(const TDesC& aTestName)
	:CTestStepDevVideoRecordAllocFail(aTestName)
	{
	}

CTestStepDevVideoRecordAllocFailGetBuffers* CTestStepDevVideoRecordAllocFailGetBuffers::NewL(const TDesC& aTestName)
	{
	CTestStepDevVideoRecordAllocFailGetBuffers* self = new(ELeave) CTestStepDevVideoRecordAllocFailGetBuffers(aTestName);
	return self;
	}

CTestStepDevVideoRecordAllocFailGetBuffers::~CTestStepDevVideoRecordAllocFailGetBuffers()
	{
	}

TVerdict CTestStepDevVideoRecordAllocFailGetBuffers::DoTestL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is an Alloc Failure test of retrieving and returning Buffers to the plugin"));


	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP(err, iDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
		if (!err)
			TRAP(err, PerformTestL());

		REComSession::FinalClose();

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
				User::Free(testAlloc);
			
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		// see if valid result and break if wrong - might be premature result
		delete iDevVideoRecord;
		iDevVideoRecord = NULL;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed || reachedEnd)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	delete iDevVideoRecord;
	iDevVideoRecord = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


void CTestStepDevVideoRecordAllocFailGetBuffers::PerformTestL()
	{
	iTestError = KErrNone;

	// select Encoder
	iDevVideoRecord->SelectEncoderL(KUidDevVideoTestEncodeHwDevice);

	// select Pre-processor
	iDevVideoRecord->SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice);

	//Tell plugin to start creating Buffers.
	iDevVideoRecord->SetImplementationSpecificEncoderOptionsL(KTestISEncBuffers);

	// initialize CDevVideoRecord and wait for response
	iDevVideoRecord->Initialize();

	User::LeaveIfError(iTestError);

	iDevVideoRecord->Start();//Tell plugin to start creating Buffers.
	// We should now get a series of NewBuffer() callbacks.

	User::LeaveIfError(iTestError);
	}

void CTestStepDevVideoRecordAllocFailGetBuffers::MdvpoInitComplete(TInt aError)
	{
	iTestError = aError;
	}

void CTestStepDevVideoRecordAllocFailGetBuffers::MdvpoNewBuffers()
	{
	TRAP(iTestError, DoNewBuffersL());
	}

void CTestStepDevVideoRecordAllocFailGetBuffers::CheckNewBufferInfoL(TUint aNumBuffers)
	{
	TUint numBuffers = 0;
	numBuffers = iDevVideoRecord->NumDataBuffers();

	if (numBuffers != aNumBuffers)
		{
		User::Leave(KErrGeneral);
		}
	}

void CTestStepDevVideoRecordAllocFailGetBuffers::DoNewBuffersL()
	{
	User::LeaveIfError(iTestError);//Don't go any further if we have an error
	switch (iCurrentBufferNumber)
		{
		case 0:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buffer);
			break;
			}
		case 1:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buffer);
			break;
			}
		case 2:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buffer);
			break;
			}
		case 3:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			break;
			}
		case 4:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			break;
			}
		case 5:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			break;
			}
		case 6:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			break;
			}
		case 7:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(2);
			break;
			}
		case 8:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(3);
			break;
			}
		case 9:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(4);
			TVideoOutputBuffer* buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			CheckNewBufferInfoL(3);
			buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			CheckNewBufferInfoL(2);
			buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			CheckNewBufferInfoL(1);
			buffer = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buffer);
			CheckNewBufferInfoL(0);
			break;
			}
		default:
			User::Leave(KErrGeneral);
			break;
		};
	}
	

