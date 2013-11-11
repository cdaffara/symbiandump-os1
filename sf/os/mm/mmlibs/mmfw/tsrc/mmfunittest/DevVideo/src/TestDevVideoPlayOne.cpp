
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
// TestDevVideoPlay.cpp
// 
//

#include "TestDevVideoPlay.h"
#include "TestDevVideoPlayTestData.h"
#include "TestDevVideoPlugins/decoder.h"
#include "TestDevVideoPlugins/postproc.h"
#include <e32math.h>

const TInt KTestScreenCount = 2;
// DevVideoPlay base class

const TUid KUidMDFVideoDecoderHwDeviceAdapter = { 0x102737ED };

CTestDevVideoPlayStep::CTestDevVideoPlayStep(const TDesC& aTestName, TTestType aTestType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;		
	iTestType = aTestType;		// included in case needed in future
	}
	
CTestDevVideoPlayStep::~CTestDevVideoPlayStep() 
	{
	delete iDevVideoPlay;
	}

TVerdict CTestDevVideoPlayStep::DoTestStepL()
	{
	// construct DevVideoPlay object
	__UHEAP_MARK;
	TRAPD(err, iDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Could not create a CMMFDevVideoPlay class!"));
		return EInconclusive;
		}
	
	TVerdict ret = EFail;

	// call the test step
	ret = DoTestL(*iDevVideoPlay);
		
	INFO_PRINTF1(_L("CMMFDevVideoPlay: Destroying"));
	delete iDevVideoPlay;
	iDevVideoPlay = NULL;
	__MM_HEAP_MARKEND;
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMMFDevVideoPlay failed with error %d"), iError );
	
	return ret;
	}


// MMMFDevVideoPlayObserver
void CTestDevVideoPlayStep::MdvpoNewBuffers()
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoNewBuffers()"));
	}

void CTestDevVideoPlayStep::MdvpoReturnPicture(TVideoPicture* /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoReturnPicture()"));
	}

void CTestDevVideoPlayStep::MdvpoSupplementalInformation(const TDesC8& /*aData*/, 
								  const TTimeIntervalMicroSeconds& /*aTimestamp*/, 
								  const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoSupplementalInformation()"));
	}

void CTestDevVideoPlayStep::MdvpoPictureLoss()
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoPictureLoss()"));
	}

void CTestDevVideoPlayStep::MdvpoPictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoPictureLoss(TArray)"));
	}

void CTestDevVideoPlayStep::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoSliceLoss()"));
	}

void CTestDevVideoPlayStep::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoReferencePictureSelection()"));
	}

void CTestDevVideoPlayStep::MdvpoTimedSnapshotComplete(TInt /*aError*/, 
								TPictureData* /*aPictureData*/, 
								const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
								const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoTimedSnapshotComplete()"));
	}

void CTestDevVideoPlayStep::MdvpoNewPictures()
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoNewPictures()"));
	}

void CTestDevVideoPlayStep::MdvpoFatalError(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoPlayStep::MdvpoFatalError():  Error = %d"), aError);
	}

void CTestDevVideoPlayStep::MdvpoInitComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoPlayStep::MdvpoInitComplete():  Error = %d"), aError);
	}

void CTestDevVideoPlayStep::MdvpoStreamEnd()
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayStep::MdvpoStreamEnd()"));
	}

//------------------------------------------------------------------

CTestDevVideoPlayCommonFormat::CTestDevVideoPlayCommonFormat(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayCommonFormat* CTestDevVideoPlayCommonFormat::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayCommonFormat* self = new(ELeave) CTestDevVideoPlayCommonFormat(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayCommonFormat::DoTestL(CMMFDevVideoPlay& /*aDevVideoPlay*/)
	{
	TVerdict ret = EPass;
	TBool expErr = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Find common format"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	// make a fake list of formats
	RArray<TUncompressedVideoFormat> vFormats1;
	RArray<TUncompressedVideoFormat> vFormats2;

	TUncompressedVideoFormat commonFormat;
	TUncompressedVideoFormat otherFormat;

	// push arrays to cleanup stack
	CleanupClosePushL(vFormats1);
	CleanupClosePushL(vFormats2);

	// setup vFormats1 
	User::LeaveIfError(vFormats1.Append(KTestVidFormat1));
	User::LeaveIfError(vFormats1.Append(KTestVidFormat2));

	// setup vFormats2
	User::LeaveIfError(vFormats2.Append(KTestVidFormat2));
	User::LeaveIfError(vFormats2.Append(KTestVidFormat3));

	TArray<TUncompressedVideoFormat> t1 = vFormats1.Array();
	TArray<TUncompressedVideoFormat> t2 = vFormats2.Array();
	TBool formatFound = EFalse;
		
	// attempt to find a common format
	formatFound = CMMFDevVideoPlay::FindCommonFormat(t1, t2, commonFormat);

	// now attempt to find a common format that doesn't exist
	vFormats1.Reset();
	User::LeaveIfError(vFormats1.Append(KTestVidFormat1));
	User::LeaveIfError(vFormats1.Append(KTestVidFormat1));
	TBool otherFormatFound = ETrue;

	// perform search for non-existent common format
	t1 = vFormats1.Array();	// need to get Array class again
	otherFormatFound = CMMFDevVideoPlay::FindCommonFormat(t1, t2, otherFormat);

	// close the arrays
	CleanupStack::PopAndDestroy(2, &vFormats1);

	// check format
	if ( (!formatFound) || !(commonFormat == KTestVidFormat2) )
		{
		ret = EFail;
		ERR_PRINTF1(_L("Error:  No common format found..."));
		}
	else if (otherFormatFound )
		{
		ret = EFail;
		ERR_PRINTF1(_L("Error:  Found a common format when there wasn't one!"));
		}
	else
		{
		INFO_PRINTF3(_L("FindCommonFormat(), %d = %d"), formatFound, expErr);
		}
	return ret;
	}


//------------------------------------------------------------------



//------------------------------------------------------------------

CTestDevVideoPlayFindDecoders::CTestDevVideoPlayFindDecoders(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayFindDecoders* CTestDevVideoPlayFindDecoders::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayFindDecoders* self = new(ELeave) CTestDevVideoPlayFindDecoders(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayFindDecoders::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  FindDecoders"));

	RArray<TUid> foundDecoders;
	CleanupClosePushL(foundDecoders);
	RArray<TUid> expectedDecoders;
	CleanupClosePushL(expectedDecoders);
	RArray<TUid> unExpectedDecoders;
	CleanupClosePushL(unExpectedDecoders);

	TInt error = KErrNone;
	TUint32 requestedPostProcType = 0;
	
	// Unsupported mime type
	if (!error)
		{
		requestedPostProcType = 0;
		_LIT8(KMimeType1, "VIDEO/mpeg21");
		foundDecoders.Reset();
		expectedDecoders.Reset();
		unExpectedDecoders.Reset();
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

		TRAP(error, aDevVideoPlay.FindDecodersL(KMimeType1(), requestedPostProcType, foundDecoders, EFalse));

		if (!error)
			error = CheckDecoders(foundDecoders, expectedDecoders, unExpectedDecoders);
		}

	// Unsupported post proc type
	if (!error)
		{
		requestedPostProcType = EPpOutputCrop|EPpInputCrop;
		foundDecoders.Reset();
		_LIT8(KMimeType2, "videO/duMMy");
		expectedDecoders.Reset();
		unExpectedDecoders.Reset();
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

		TRAP(error, aDevVideoPlay.FindDecodersL(KMimeType2(), requestedPostProcType, foundDecoders, EFalse));
		if (error == KErrNotFound)//we actually expect KErrNotFound here
			error = KErrNone;
		else
			error = KErrGeneral;

		if (!error)
			error = CheckDecoders(foundDecoders, expectedDecoders, unExpectedDecoders);
		}

	// Unsupported mime type (by exact match)
	if (!error)
		{
		_LIT8(KMimeType3, "video/H263-2000; profile=45;level=40");
		requestedPostProcType = 0;
		foundDecoders.Reset();
		expectedDecoders.Reset();
		unExpectedDecoders.Reset();
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

		TRAP(error, aDevVideoPlay.FindDecodersL(KMimeType3(), requestedPostProcType, foundDecoders, ETrue));
		if (error == KErrNotFound)//we actually expect KErrNotFound here
			error = KErrNone;
		else
			error = KErrGeneral;

		if (!error)
			error = CheckDecoders(foundDecoders, expectedDecoders, unExpectedDecoders);
		}
	
	// Supported mime type (by unexact match)
	if (!error)
		{
		_LIT8(KMimeType4, "video/H263-2000; profile=1234;level=20");
		requestedPostProcType = 0;
		foundDecoders.Reset();
		expectedDecoders.Reset();
		unExpectedDecoders.Reset();
		User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
		User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

		TRAP(error, aDevVideoPlay.FindDecodersL(KMimeType4(), requestedPostProcType, foundDecoders, EFalse));

		if (!error)
			error = CheckDecoders(foundDecoders, expectedDecoders, unExpectedDecoders);
		}
	
	// Supported mime type and post proc type (by exact match)
	if (!error)
		{
		_LIT8(KMimeType5, "video/zippyvideo");
		requestedPostProcType = EPpMirror|EPpInputCrop;
		foundDecoders.Reset();
		expectedDecoders.Reset();
		unExpectedDecoders.Reset();
		User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

		TRAP(error, aDevVideoPlay.FindDecodersL(KMimeType5(), requestedPostProcType, foundDecoders, ETrue));

		if (!error)
			error = CheckDecoders(foundDecoders, expectedDecoders, unExpectedDecoders);
		}
	
	//Added to check that the mimetype comparison is not case sensitive
	if (!error)
		{
		_LIT8(KMimeType6, "VIDEO/ZIPPYvideo");
		requestedPostProcType = EPpMirror|EPpInputCrop;
		foundDecoders.Reset();
		expectedDecoders.Reset();
		unExpectedDecoders.Reset();
		User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
		User::LeaveIfError(unExpectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

		INFO_PRINTF1(_L("To check that the mime type comparison is not case sensitive"));
		TRAP(error, aDevVideoPlay.FindDecodersL(KMimeType6(), requestedPostProcType, foundDecoders, ETrue));
		INFO_PRINTF2(_L("error = %d"),error);
		if (!error)
			error = CheckDecoders(foundDecoders, expectedDecoders, unExpectedDecoders);
	
		}

	CleanupStack::PopAndDestroy(3); //foundDecoders, expectedDecoders, unExpectedDecoders

	if (!error)
		ret = EPass;
		
	return ret;
	}

TInt CTestDevVideoPlayFindDecoders::CheckDecoders(const RArray<TUid>& aFoundDecoders, 
												  const RArray<TUid>& aExpectedDecoders, 
												  const RArray<TUid>& aUnExpectedDecoders)
	{
	TInt ret = KErrGeneral;

	TBool foundUnExpected = EFalse;
	TInt numberToBeFoundInExpectedDecoders = aExpectedDecoders.Count();

	for (TInt i=0; i<aFoundDecoders.Count(); i++)
		{
		TUid uidToLookFor = aFoundDecoders[i];
		for (TInt j=0; j<aExpectedDecoders.Count(); j++)
			{
			if (aExpectedDecoders[j] == uidToLookFor)
				{
				numberToBeFoundInExpectedDecoders--;
				break;//j
				}
			}
		for (TInt k=0; k<aUnExpectedDecoders.Count(); k++)
			{
			if (aUnExpectedDecoders[k] == uidToLookFor)
				{
				foundUnExpected = ETrue;
				break;//k
				}
			}
		}

	if ((!foundUnExpected) && (numberToBeFoundInExpectedDecoders==0))
		ret = KErrNone;

	return ret;
	}



//------------------------------------------------------------------

CTestDevVideoPlayFindPostProc::CTestDevVideoPlayFindPostProc(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayFindPostProc* CTestDevVideoPlayFindPostProc::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayFindPostProc* self = new(ELeave) CTestDevVideoPlayFindPostProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayFindPostProc::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  FindPostProc"));

	RArray<TUid> foundPostProx;
	CleanupClosePushL(foundPostProx);
	RArray<TUid> expectedPostProx;
	CleanupClosePushL(expectedPostProx);
	RArray<TUid> unExpectedPostProx;
	CleanupClosePushL(unExpectedPostProx);

	TInt error = KErrNone;
	TUint32 requestedPostProcType = 0;	

	// First ask for post processor isn't supported
	if (!error)
		{
		requestedPostProcType = EPpInputCrop|EPpMirror|EPpScale|EPpOutputPad|EPpNoiseFilter;
		foundPostProx.Reset();
		expectedPostProx.Reset();
		unExpectedPostProx.Reset();
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));
		
		TRAP(error, aDevVideoPlay.FindPostProcessorsL(requestedPostProcType, foundPostProx));
		if (error == KErrNotFound)//we actually expect KErrNotFound here
			error = KErrNone;
		else
			error = KErrGeneral;

		if (!error)
			error = CheckPostProx(foundPostProx, expectedPostProx, unExpectedPostProx);
		}

	// Next ask for all post processors
	if (!error)
		{
		requestedPostProcType = 0;
		foundPostProx.Reset();
		expectedPostProx.Reset();
		unExpectedPostProx.Reset();
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));

		TRAP(error, aDevVideoPlay.FindPostProcessorsL(requestedPostProcType, foundPostProx));
		if (!error)
			error = CheckPostProx(foundPostProx, expectedPostProx, unExpectedPostProx);
		}

	// Next ask for a specific subset of post processors
	if (!error)
		{
		requestedPostProcType = EPpMirror;
		foundPostProx.Reset();
		expectedPostProx.Reset();
		unExpectedPostProx.Reset();
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));

		TRAP(error, aDevVideoPlay.FindPostProcessorsL(requestedPostProcType, foundPostProx));
		if (!error)
			error = CheckPostProx(foundPostProx, expectedPostProx, unExpectedPostProx);
		}
	
	// Next ask for another specific subset of post processors
	if (!error)
		{
		requestedPostProcType = EPpMirror|EPpInputCrop;
		foundPostProx.Reset();
		expectedPostProx.Reset();
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
		User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
		User::LeaveIfError(unExpectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));

		TRAP(error, aDevVideoPlay.FindPostProcessorsL(requestedPostProcType, foundPostProx));
		if (!error)
			error = CheckPostProx(foundPostProx, expectedPostProx, unExpectedPostProx);
		}

	CleanupStack::PopAndDestroy(3);//foundPostProx, expectedPostProx, unExpectedPostProx
	
	if (!error)
		ret = EPass;
		
	return ret;
	}


TInt CTestDevVideoPlayFindPostProc::CheckPostProx(const RArray<TUid>& aFoundPostProx, 
												  const RArray<TUid>& aExpectedPostProx, 
												  const RArray<TUid>& aUnExpectedPostProx)
	{
	TInt ret = KErrGeneral;

	TBool foundUnExpected = EFalse;
	TInt numberToBeFoundInExpectedPostProx = aExpectedPostProx.Count();

	for (TInt i=0; i<aFoundPostProx.Count(); i++)
		{
		TUid uidToLookFor = aFoundPostProx[i];
		for (TInt j=0; j<aExpectedPostProx.Count(); j++)
			{
			if (aExpectedPostProx[j] == uidToLookFor)
				{
				numberToBeFoundInExpectedPostProx--;
				break;//j
				}
			}
		for (TInt k=0; k<aUnExpectedPostProx.Count(); k++)
			{
			if (aUnExpectedPostProx[k] == uidToLookFor)
				{
				foundUnExpected = ETrue;
				break;//k
				}
			}
		}

	if ((!foundUnExpected) && (numberToBeFoundInExpectedPostProx==0))
		ret = KErrNone;

	return ret;
	}



//------------------------------------------------------------------

CTestDevVideoPlayListDecoders::CTestDevVideoPlayListDecoders(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayListDecoders* CTestDevVideoPlayListDecoders::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayListDecoders* self = new(ELeave) CTestDevVideoPlayListDecoders(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayListDecoders::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  ListDecoders"));
		
	RArray<TUid> foundDecoders;
	CleanupClosePushL(foundDecoders);
	RArray<TUid> expectedDecoders;
	CleanupClosePushL(expectedDecoders);

	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice1));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice2));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice3));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice4));
	User::LeaveIfError(expectedDecoders.Append(KUidDevVideoTestDecodeHwDevice5));

	TRAPD(err, aDevVideoPlay.GetDecoderListL(foundDecoders));

	if (!err)
		{		
		// Make sure that the video hardware device adapter is NOT in the array
		TBool foundAdapter = EFalse;
		for (TInt j = 0; j < foundDecoders.Count(); j++)
			{
			if (foundDecoders[j] == KUidMDFVideoDecoderHwDeviceAdapter)
				{
				foundAdapter = ETrue;
				break;
				}
			}

		// Make sure that all the UIDs of the test plugins are in the array..
		TInt numberToBeFound = expectedDecoders.Count();
		for (TInt i=0; i<expectedDecoders.Count(); i++)
			{
			TUid uidToFind = expectedDecoders[i];
			for (TInt j=0; j<foundDecoders.Count(); j++)
				{
				if (foundDecoders[j] == uidToFind)
					{
					numberToBeFound--;
					break;//j
					}
				}
			}
		if ((numberToBeFound == 0) && (!foundAdapter))
			{
			ret = EPass;
			}
		}

	CleanupStack::PopAndDestroy(2);
	
	return ret;
	}




//------------------------------------------------------------------

CTestDevVideoPlayListPostProc::CTestDevVideoPlayListPostProc(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayListPostProc* CTestDevVideoPlayListPostProc::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayListPostProc* self = new(ELeave) CTestDevVideoPlayListPostProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayListPostProc::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  ListPostProc"));
		
	RArray<TUid> foundPostProx;
	CleanupClosePushL(foundPostProx);
	RArray<TUid> expectedPostProx;
	CleanupClosePushL(expectedPostProx);

	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice1));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice2));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice3));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice4));
	User::LeaveIfError(expectedPostProx.Append(KUidDevVideoTestPostProcHwDevice5));

	TRAPD(err, aDevVideoPlay.GetPostProcessorListL(foundPostProx));

	if (!err)
		{
		// Make sure that all the UIDs of the test plugins are in the array..
		TInt numberToBeFound = expectedPostProx.Count();
		for (TInt i=0; i<expectedPostProx.Count(); i++)
			{
			TUid uidToFind = expectedPostProx[i];
			for (TInt j=0; j<foundPostProx.Count(); j++)
				{
				if (foundPostProx[j] == uidToFind)
					{
					numberToBeFound--;
					break;//j
					}
				}
			}
		if (numberToBeFound == 0)
			ret = EPass;
		}

	CleanupStack::PopAndDestroy(2);

	return ret;
	}




//------------------------------------------------------------------

CTestDevVideoPlayGetDecoderInfo::CTestDevVideoPlayGetDecoderInfo(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetDecoderInfo* CTestDevVideoPlayGetDecoderInfo::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetDecoderInfo* self = new(ELeave) CTestDevVideoPlayGetDecoderInfo(aTestName, aTestType);
	return self;
	}

TBool CTestDevVideoPlayGetDecoderInfo::ValidDecoderInfo(CVideoDecoderInfo* aInfo)
	{
	// do null pointer check
	if (!aInfo) 
		{
		ERR_PRINTF1(_L("Error - Decoder Info is NULL!"));
		return EFalse;
		}
	
	// check the decoder against test data
	// need to check every call so as to keep code coverage
	// and prevent a situation where if the first test fails
	// then subsequent tests will not be called
	TBool ret = ETrue;

	// UID check
	if (aInfo->Uid() != KUidDevVideoTestDecodeHwDevice)
		{
		ERR_PRINTF1(_L("Error - Decoder UID is incorrect!"));
		ret = EFalse;
		}
	// Manufacturer check
	if (!(aInfo->Manufacturer() == KTestDecoderInfoManufacturer))
		{
		ERR_PRINTF1(_L("Error - Manufacturer is incorrect!"));
		ret = EFalse;
		}
	// Identifier check
	if (!(aInfo->Identifier() == KTestDecoderInfoIdentifier))
		{
		ERR_PRINTF1(_L("Error - Identifier is incorrect!"));
		ret = EFalse;
		}
	// Version check
	TVersion decVer = aInfo->Version();
	if ( (decVer.iMajor != KTestDecoderInfoVersionMaj) ||
		 (decVer.iMinor != KTestDecoderInfoVersionMin) ||
		 (decVer.iBuild != KTestDecoderInfoVersionBuild) )
		{
		ERR_PRINTF1(_L("Error - Version is incorrect!"));
		ret = EFalse;
		}
	// Accelerated check
	if (!aInfo->Accelerated())
		{
		ERR_PRINTF1(_L("Error - Accelerated = EFalse!"));
		ret = EFalse;
		}
	// Direct display check
	if (!aInfo->SupportsDirectDisplay())
		{
		ERR_PRINTF1(_L("Error - Supports Direct Display = EFalse!"));
		ret = EFalse;
		}

	// Max picture size
	TSize maxSize = aInfo->MaxPictureSize();
	if (maxSize.iWidth != KTestDecoderInfoMaxSizeX || maxSize.iHeight != KTestDecoderInfoMaxSizeY)
		{
		ERR_PRINTF1(_L("Error - Max Picture Size is wrong!"));
		ret = EFalse;
		}

	// Max Bitrate
	if (aInfo->MaxBitrate() != KMaxTUint)
		{
		ERR_PRINTF1(_L("Error - Max Bitrate is wrong!"));
		ret = EFalse;
		}
	
	// Supports picture loss
	if (!aInfo->SupportsPictureLoss())
		{
		ERR_PRINTF1(_L("Error - Supports Picture Loss = EFalse!"));
		ret = EFalse;
		}

	// Supports slice loss
	if (aInfo->SupportsSliceLoss())
		{
		ERR_PRINTF1(_L("Error - Supports Slice Loss = ETrue!"));
		ret = EFalse;
		}

	// Coding standard info
	if (!(aInfo->CodingStandardSpecificInfo() == KTestDecoderInfoCSInfo))
		{
		ERR_PRINTF1(_L("Error - Coding standard specific info is incorrect!"));
		ret = EFalse;
		}
	// Implementation info
	if (!(aInfo->ImplementationSpecificInfo() == KTestDecoderInfoISInfo))
		{
		ERR_PRINTF1(_L("Error - Implementation specific info is incorrect!"));
		ret = EFalse;
		}

	if (!aInfo->SupportsContentProtected())//Check if flag is set indicating Content Protection is supported.
		{
		ERR_PRINTF1(_L("Error - Failed To get flag indicating support of Content Protection!"));
		ret = EFalse;
		}

	// get some test compressed video formats
	CCompressedVideoFormat* testFormat = NULL;
	TInt err = KErrNone;

	// get formats supported
	const RPointerArray<CCompressedVideoFormat>& vidFormats = aInfo->SupportedFormats();

	// get supported max picture rates
	TPictureRateAndSize testRate;
	const RArray<TPictureRateAndSize>& maxRates = aInfo->MaxPictureRates();

	// test formats supported
	for (TUint i = 0; i < KTestDecoderInfoCount; i++)
		{
		TPtrC8 mimeType = KTestDecoderInfoMimeArray[i];
		TRAP(err, testFormat = GetTestCVFormatL(mimeType));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Error - Failed to construct CCompressedVideoFormat: %d!"), i);
			ret = EFalse;
			if(testFormat != NULL)
				{
				delete testFormat;
				testFormat=NULL;
				}
			break;
			}

		if (!(*testFormat == *(vidFormats[i])) || !(aInfo->SupportsFormat(*testFormat)))
			{
			ERR_PRINTF2(_L("Error - Video Format %d is corrupt!"), i);
			ret = EFalse;
			}

		delete testFormat; 
		testFormat = NULL;

		// Test supported max picture rates
		GetTestEncoderInfoRate(i, testRate);
		if (!(testRate.iPictureSize == maxRates[i].iPictureSize) || 
			!(testRate.iPictureRate == maxRates[i].iPictureRate) )
			{
			ERR_PRINTF2(_L("Error - picture rate %d is corrupt!"), i);
			ret = EFalse;
			}
		}
	//Screen information check
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	RArray<TInt> supportedScreens;
	TRAP(err, aInfo->GetSupportedScreensL(supportedScreens));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("GetSupportedScreensL leaves with %d"), err);
		ret = EFalse;
		}
	if(supportedScreens.Count() != KTestScreenCount)
		{
		ERR_PRINTF1(_L("Error - Unexpected screen count"));
		ret = EFalse;
		}
	err = supportedScreens.Find(KDecoderDefaultScreenNumber);
	if(err == KErrNotFound)
		{
		ERR_PRINTF1(_L("Error - No support for primary screen"));
		ret = EFalse;
		}
	err = supportedScreens.Find(KDecoderSecondaryScreenNumber);
	if(err == KErrNotFound)
		{
		ERR_PRINTF1(_L("Error - No support for secondary screen"));
		ret = EFalse;
		}
	supportedScreens.Close();
#endif
	// finally check a format which isn't supposed to be supported
	TRAP(err, testFormat = GetTestCVFormatL(_L8("Frederick Bloggs and his amazing coloured dog")));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - Failed to construct final CCompressedVideoFormat!"));
		ret = EFalse;
		}
	else 
		{
		if (aInfo->SupportsFormat(*testFormat))
			{
			ERR_PRINTF1(_L("Error - Failed negative SupportsFormat() check!"));
			ret = EFalse;
			}
		}

	if(testFormat != NULL)
		{
		delete testFormat;
		testFormat=NULL;
		}
	return ret;
	}

TVerdict CTestDevVideoPlayGetDecoderInfo::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetDecoderInfo"));

	// expected results		
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
	CVideoDecoderInfo *decInfo = NULL;
	TBool valid = EFalse;
	
	// get decoder information from test decoder plugin
	
	TRAP(err, 
		decInfo = aDevVideoPlay.VideoDecoderInfoLC(KUidDevVideoTestDecodeHwDevice); 
		valid = ValidDecoderInfo(decInfo);
		CleanupStack::PopAndDestroy(decInfo); );
	if (err != expErr)
		{
		ERR_PRINTF3(_L("VideoDecoderInfoLC() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	// check the Video Decoder
	else if (!valid)
		{
		ERR_PRINTF1(_L("CVideoDecoderInfo class is corrupt!"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("CVideoDecoderInfo passed check successfully"));
		INFO_PRINTF3(_L("VideoDecoderInfoLC(), %d = %d"), err, expErr);
		}
		
	return ret;
	}




//------------------------------------------------------------------

CTestDevVideoPlayGetPostProcInfo::CTestDevVideoPlayGetPostProcInfo(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetPostProcInfo* CTestDevVideoPlayGetPostProcInfo::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetPostProcInfo* self = new(ELeave) CTestDevVideoPlayGetPostProcInfo(aTestName, aTestType);
	return self;
	}

TBool CTestDevVideoPlayGetPostProcInfo::ValidPostProcInfo(CPostProcessorInfo* aInfo)
	{
	// do null pointer check
	if (!aInfo) 
		{
		ERR_PRINTF1(_L("Error - Post Processor Info is NULL!"));
		return EFalse;
		}
	
	// check the post processor against test data
	// need to check every call so as to keep code coverage
	// and prevent a situation where if the first test fails
	// then subsequent tests will not be called
	TBool ret = ETrue;

	// UID check
	if (aInfo->Uid() != KUidDevVideoTestPostProcHwDevice)
		{
		ERR_PRINTF1(_L("Error - Post Processor UID is incorrect!"));
		ret = EFalse;
		}
	// Manufacturer check
	if (!(aInfo->Manufacturer() == KTestPostProcInfoManufacturer))
		{
		ERR_PRINTF1(_L("Error - Manufacturer is incorrect!"));
		ret = EFalse;
		}
	// Identifier check
	if (!(aInfo->Identifier() == KTestPostProcInfoIdentifier))
		{
		ERR_PRINTF1(_L("Error - Identifier is incorrect!"));
		ret = EFalse;
		}
	// Version check
	TVersion posVer = aInfo->Version();
	if ( (posVer.iMajor != KTestPostProcInfoVersionMaj) ||
		 (posVer.iMinor != KTestPostProcInfoVersionMin) ||
		 (posVer.iBuild != KTestPostProcInfoVersionBuild) )
		{
		ERR_PRINTF1(_L("Error - Version is incorrect!"));
		ret = EFalse;
		}
	// Accelerated check
	if (!aInfo->Accelerated())
		{
		ERR_PRINTF1(_L("Error - Accelerated = EFalse!"));
		ret = EFalse;
		}
	// Direct display check
	if (!aInfo->SupportsDirectDisplay())
		{
		ERR_PRINTF1(_L("Error - Supports Direct Display = EFalse!"));
		ret = EFalse;
		}
	// YUVToRGB Capabilities
	if (!CompareYuvRgbCaps(aInfo->YuvToRgbCapabilities(), KTestPostProcInfoYuvToRgbCaps))
		{
		ERR_PRINTF1(_L("Error - YUV To RGB capabilities is incorrect!"));
		ret = EFalse;
		}

	// Supported rotations
	if (aInfo->SupportedRotations() != KTestPostProcInfoRotations)
		{
		ERR_PRINTF1(_L("Error - Supported Rotations is incorrect!"));
		ret = EFalse;
		}

	// Supports arbitrary scaling check
	if (!aInfo->AntiAliasedScaling())
		{
		ERR_PRINTF1(_L("Error - Anti-Aliased Scaling = EFalse!"));
		ret = EFalse;
		}
	// Supports anti-aliased scaling check
	if (!aInfo->SupportsArbitraryScaling())
		{
		ERR_PRINTF1(_L("Error - Supports Arbitrary Scaling = EFalse!"));
		ret = EFalse;
		}
	// Implementation info
	if (!(aInfo->ImplementationSpecificInfo() == KTestPostProcInfoISInfo))
		{
		ERR_PRINTF1(_L("Error - Implementation specific info is incorrect!"));
		ret = EFalse;
		}

	// get some test compressed video formats
	TUncompressedVideoFormat testFormat;
	TUint32 testComb = 0;
	TScaleFactor testFactor = {0, 0};

	// get supported formats
	const RArray<TUncompressedVideoFormat>& vidFormats = aInfo->SupportedFormats();
	
	// get supported combinations
	const RArray<TUint32>& suppCombs = aInfo->SupportedCombinations();

	// get supported scale factors
	const RArray<TScaleFactor>& scaleFactors = aInfo->SupportedScaleFactors();

	// test formats, combinations, and scale factors supported
	for (TUint i = 0; i < KTestPostProcInfoCount; i++)
		{
		testFormat = KTestPostProcInfoFormatArray[i];
		if (!(testFormat == (vidFormats[i])) || !(aInfo->SupportsFormat(testFormat)))
			{
			ERR_PRINTF2(_L("Error - Video Format %d is corrupt!"), i);
			ret = EFalse;
			}

		testComb = KTestPostProcInfoCombsArray[i];
		if (!(testComb == (suppCombs[i])) || !(aInfo->SupportsCombination(testComb)))
			{
			ERR_PRINTF2(_L("Error - Combination %d is corrupt!"), i);
			ret = EFalse;
			}

		testFactor = KTestPostProcInfoScaleFactorsArray[i];
		if (!CompareScaleFactors(testFactor, scaleFactors[i]))
			{
			ERR_PRINTF2(_L("Error - Scale factor %d is corrupt!"), i);
			ret = EFalse;
			}
		}

	//Screen information check
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	RArray<TInt> supportedScreens;
	TRAPD(err, aInfo->GetSupportedScreensL(supportedScreens));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("GetSupportedScreensL leaves with %d"), err);
		ret = EFalse;
		}
	if(supportedScreens.Count() != KTestScreenCount)
		{
		ERR_PRINTF1(_L("Error - Unexpected screen count"));
		ret = EFalse;
		}
	err = supportedScreens.Find(KPostProcDefaultScreenNumber);
	if(err == KErrNotFound)
		{
		ERR_PRINTF1(_L("Error - No support for primary screen"));
		ret = EFalse;
		}
	err = supportedScreens.Find(KPostProcSecondaryScreenNumber);
	if(err == KErrNotFound)
		{
		ERR_PRINTF1(_L("Error - No support for secondary screen"));
		ret = EFalse;
		}
	supportedScreens.Close();
#endif
	// check a format that isn't supposed to be supported
	testFormat = KTestPostProcInfoNegativeFormat;
	testFormat.iYuvFormat = KTestYuvFormat1;
	if (aInfo->SupportsFormat(testFormat))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsFormat() check!"));
		ret = EFalse;
		}
	
	// finally check a combination that isn't supposed to be supported
	testComb = KTestPostProcInfoNegativeComb;
	if (aInfo->SupportsCombination(testComb))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsCombination() check!"));
		ret = EFalse;
		}

	if (!aInfo->SupportsContentProtected())//Check if flah is set indicating Content Protection is supported.
		{
		ERR_PRINTF1(_L("Error - Failed To get flag indicating support of Content Protection!"));
		ret = EFalse;
		}

	return ret;
	}

TVerdict CTestDevVideoPlayGetPostProcInfo::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetPostProcInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
	CPostProcessorInfo *procInfo = NULL;
	TBool valid = EFalse;
	
	// get decoder information from test decoder plugin
	TRAP(err, 
		procInfo = aDevVideoPlay.PostProcessorInfoLC(KUidDevVideoTestPostProcHwDevice);
	        procInfo->SetSupportsContentProtected(ETrue);	
		valid = ValidPostProcInfo(procInfo);
		CleanupStack::PopAndDestroy(procInfo); );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("PostProcessorInfoLC() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	// check the Video Post Processor
	else if (!valid)
		{
		ERR_PRINTF1(_L("CPostProcessorInfo class is corrupt!"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("CPostProcessorInfo passed check successfully"));
		INFO_PRINTF3(_L("PostProcessorInfoLC(), %d = %d"), err, expErr);
		}

	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlaySelectDecoder::CTestDevVideoPlaySelectDecoder(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySelectDecoder* CTestDevVideoPlaySelectDecoder::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySelectDecoder* self = new(ELeave) CTestDevVideoPlaySelectDecoder(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySelectDecoder::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SelectDecoder"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	iError = KErrNone;

	// this will leave if an error occurs
	TRAP( iError, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice) );

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("SelectDecoderL() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SelectDecoderL(), %d = %d"), iError, expErr);
	
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlaySelectPostProc::CTestDevVideoPlaySelectPostProc(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySelectPostProc* CTestDevVideoPlaySelectPostProc::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySelectPostProc* self = new(ELeave) CTestDevVideoPlaySelectPostProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySelectPostProc::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SelectPostProc"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	iError = KErrNone;
	
	// this will leave if an error occurs
	TRAP( iError, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice) );

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("SelectPostProcessorL() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SelectPostProcessorL(), %d = %d"), iError, expErr);
	
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlayGetHeaderInfo::CTestDevVideoPlayGetHeaderInfo(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetHeaderInfo* CTestDevVideoPlayGetHeaderInfo::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetHeaderInfo* self = new(ELeave) CTestDevVideoPlayGetHeaderInfo(aTestName, aTestType);
	return self;
	}

TBool CTestDevVideoPlayGetHeaderInfo::ValidPictureHeader(const TVideoPictureHeader* aPictureHeader)
	{
	TBool ret = EFalse;

	if (!aPictureHeader)
		{
		ERR_PRINTF1(_L("Error - Picture Header is NULL!"));
		return EFalse;
		}

	TTimeIntervalMicroSeconds testTime(KTestInputBufferTimestamp);

	// check the picture header
	if (aPictureHeader->iOptions != KTestPictureHeaderOptions)
		{
		ERR_PRINTF1(_L("Error - Options are incorrect!"));
		}

	else if (!(aPictureHeader->iPresentationTimestamp == testTime))
		{
		ERR_PRINTF1(_L("Error - Timestamp is incorrect!"));
		}
	else if (!(*(aPictureHeader->iOptional) == KTestInputBufferData()))
		{	
		ERR_PRINTF1(_L("Error - Optional data is corrupt!"));
		}
	else 
		{
		INFO_PRINTF1(_L("Picture Header is valid"));
		ret = ETrue;
		}

	return ret;
	}

TVerdict CTestDevVideoPlayGetHeaderInfo::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetHeaderInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
//	THwDeviceId hwDecoder = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// construct a test video input buffer with a valid timestamp
	TPtrC8 testInputBufferDataLitCPtr(KTestInputBufferData);
	HBufC8* testInputBufferData = testInputBufferDataLitCPtr.AllocLC();

	TVideoInputBuffer testBuffer;
	testBuffer.iOptions = KTestInputBufferOptions;
	testBuffer.iPresentationTimestamp = KTestInputBufferTimestamp;
	testBuffer.iData.Set(testInputBufferData->Des());

	TVideoPictureHeader* picHeader = NULL;
	// get header information from test data unit
	TRAP(err, picHeader = aDevVideoPlay.GetHeaderInformationL(KTestDataUnitType, KTestDataUnitEncap, &testBuffer) );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("GetHeaderInformationL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else if (!ValidPictureHeader(picHeader))
		{
		ERR_PRINTF1(_L("Picture header corrupt!"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Header information retrieved successfully"));
		INFO_PRINTF3(_L("GetHeaderInformationL(), %d = %d"), err, expErr);	
		}

	CleanupStack::PopAndDestroy(testInputBufferData);
	
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlayReturnHeader::CTestDevVideoPlayReturnHeader(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayReturnHeader* CTestDevVideoPlayReturnHeader::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayReturnHeader* self = new(ELeave) CTestDevVideoPlayReturnHeader(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayReturnHeader::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  ReturnHeader"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
//	THwDeviceId hwDecoder = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	TVideoPictureHeader picHeader;
	picHeader.iOptions = KTestPictureHeaderOptions;
	TTimeIntervalMicroSeconds testTime(KTestPictureHeaderTimestamp);
	picHeader.iPresentationTimestamp = testTime;

	// return picture header [can use a local TVideoPictureHeader because we
	// are testing a fake plugin - normally this would be a header supplied by
	// the video plugin].
	aDevVideoPlay.ReturnHeader(&picHeader);
	
	// if plugin didn't PANIC then test has passed
	INFO_PRINTF1(_L("Header information returned successfully"));
			
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlayConfigureDecoder::CTestDevVideoPlayConfigureDecoder(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayConfigureDecoder* CTestDevVideoPlayConfigureDecoder::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayConfigureDecoder* self = new(ELeave) CTestDevVideoPlayConfigureDecoder(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayConfigureDecoder::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr;

	expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  ConfigureDecoderL"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	//construct a test picture header
	TVideoPictureHeader pictureHeader;
	pictureHeader.iOptions = KTestPictureHeaderOptions;
	pictureHeader.iPresentationTimestamp = KTestInputBufferTimestamp;

	TPtrC8 testInputBufferDataLitCPtr(KTestInputBufferData);
	pictureHeader.iOptional = &testInputBufferDataLitCPtr;

	// get header information from test data unit
	TRAP(err, aDevVideoPlay.ConfigureDecoderL(pictureHeader));

	if (err != expErr)
		{
		ERR_PRINTF3(_L("ConfigureDecoderL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("Header information successfully configured in Decoder"));
			}
		INFO_PRINTF3(_L("ConfigureDecoderL(), error %d = expected %d"), err, expErr);	
		}
	
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlaySetInputFormatUC::CTestDevVideoPlaySetInputFormatUC(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetInputFormatUC* CTestDevVideoPlaySetInputFormatUC::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetInputFormatUC* self = new(ELeave) CTestDevVideoPlaySetInputFormatUC(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetInputFormatUC::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetInputFormatUC"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
		return EInconclusive;
		}

	// this will leave if an error occurs in either plugin
	TRAP(err, aDevVideoPlay.SetInputFormatL(hwPostProc, KTestVidFormat2) );

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetInputFormatL() on Post-Processor gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetInputFormat(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetInputFormat::CTestDevVideoPlaySetInputFormat(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetInputFormat* CTestDevVideoPlaySetInputFormat::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetInputFormat* self = new(ELeave) CTestDevVideoPlaySetInputFormat(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetInputFormat::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetInputFormat"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
		return EInconclusive;
		}

	// get a temporary compressed video class
	CCompressedVideoFormat *tempCFormat = NULL;
	TRAP(err, tempCFormat = GetTestCVFormatL(KTestMimeType1));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize compressed video class"));
		return EInconclusive;
		}

	// this will leave if an error occurs
	TRAP(err, aDevVideoPlay.SetInputFormatL(hwDecoder, *tempCFormat, KTestUnitType1, KTestEncapType1, ETrue) );

	// delete compressed video class
	delete tempCFormat;	
	tempCFormat = NULL;

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetInputFormatL() on Decoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetInputFormat(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlayGetOutputFormatList::CTestDevVideoPlayGetOutputFormatList(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetOutputFormatList* CTestDevVideoPlayGetOutputFormatList::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetOutputFormatList* self = new(ELeave) CTestDevVideoPlayGetOutputFormatList(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetOutputFormatList::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetOutputFormatList"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
		return EInconclusive;
		}

	RArray<TUncompressedVideoFormat> decodeList;
	RArray<TUncompressedVideoFormat> postProcList;

	// Get output format list for decoder
	TRAP(err, aDevVideoPlay.GetOutputFormatListL(hwDecoder, decodeList));
		
	// Get output format list for post-processor
	TRAP(err2, aDevVideoPlay.GetOutputFormatListL(hwPostProc, postProcList));
	
	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("GetOutputFormatListL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("GetOutputFormatListL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		{
		TBool decPass = EFalse;
		TBool posPass = EFalse;

		// expecting KTestVidFormat1,2,3 - check
		TInt count = decodeList.Count();
		if (count == 3) 
			{
			if ((decodeList[0] == KTestVidFormat1) &&
				(decodeList[1] == KTestVidFormat2) &&
				(decodeList[2] == KTestVidFormat3) )
				{
				INFO_PRINTF1(_L("Decoder output list checks ok"));
				decPass = ETrue;
				}
			}

		// expecting KTestVidFormat3,2,1 - check
		count = postProcList.Count();
		if (count == 3) 
			{
			if ((postProcList[0] == KTestVidFormat3) &&
				(postProcList[1] == KTestVidFormat2) &&
				(postProcList[2] == KTestVidFormat1) )
				{
				INFO_PRINTF1(_L("Post-Processor output list checks ok"));
				posPass = ETrue;
				}
			}
		
		if (!decPass)
			{
			ERR_PRINTF1(_L("Decoder output list corrupt!"));
			ret = EFail;
			}
		
		if (!posPass)
			{
			ERR_PRINTF1(_L("Post-Processor output list corrupt!"));
			ret = EFail;
			}
	
		INFO_PRINTF4(_L("GetOutputFormatListL(), %d, %d = %d"), err, err2, expErr);
		}	

	// close the arrays
	decodeList.Reset();		
	decodeList.Close();
	
	postProcList.Reset();	
	postProcList.Close();
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetOutputFormat::CTestDevVideoPlaySetOutputFormat(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetOutputFormat* CTestDevVideoPlaySetOutputFormat::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetOutputFormat* self = new(ELeave) CTestDevVideoPlaySetOutputFormat(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetOutputFormat::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetOutputFormat"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
		return EInconclusive;
		}

	// set output format on decoder
	TRAP(err, aDevVideoPlay.SetOutputFormatL(hwDecoder, KTestVidFormat1) )
	
	// set output format on post-processor
	TRAP(err2, aDevVideoPlay.SetOutputFormatL(hwPostProc, KTestVidFormat2) )


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetOutputFormatL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetOutputFormatL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetOutputFormat(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetPostProcTypes::CTestDevVideoPlaySetPostProcTypes(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetPostProcTypes* CTestDevVideoPlaySetPostProcTypes::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetPostProcTypes* self = new(ELeave) CTestDevVideoPlaySetPostProcTypes(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetPostProcTypes::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetPostProcTypes"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
		return EInconclusive;
		}

	// set post-process types on decoder
	TRAP(err, aDevVideoPlay.SetPostProcessTypesL(hwDecoder, KTestProcessType1) )
	
	// set post-process types on post-processor
	TRAP(err2, aDevVideoPlay.SetPostProcessTypesL(hwPostProc, KTestProcessType2) )


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetPostProcessTypesL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetPostProcessTypesL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetPostProcessTypes(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlayGetDecoderPostProcInfo::CTestDevVideoPlayGetDecoderPostProcInfo(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetDecoderPostProcInfo* CTestDevVideoPlayGetDecoderPostProcInfo::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetDecoderPostProcInfo* self = new(ELeave) CTestDevVideoPlayGetDecoderPostProcInfo(aTestName, aTestType);
	return self;
	}

TBool CTestDevVideoPlayGetDecoderPostProcInfo::ValidPostProcInfo(CPostProcessorInfo* aInfo)
	{
	// do null pointer check
	if (!aInfo) 
		{
		ERR_PRINTF1(_L("Error - Post Processor Info is NULL!"));
		return EFalse;
		}
	
	// check the post processor against test data
	// need to check every call so as to keep code coverage
	// and prevent a situation where if the first test fails
	// then subsequent tests will not be called
	TBool ret = ETrue;

	// UID check
	if (aInfo->Uid() != KUidDevVideoTestDecodeHwDevice)
		{
		ERR_PRINTF1(_L("Error - Decoder UID is incorrect!"));
		ret = EFalse;
		}
	// Manufacturer check
	if (!(aInfo->Manufacturer() == KTestPostProcInfoManufacturer))
		{
		ERR_PRINTF1(_L("Error - Manufacturer is incorrect!"));
		ret = EFalse;
		}
	// Identifier check
	if (!(aInfo->Identifier() == KTestPostProcInfoIdentifier))
		{
		ERR_PRINTF1(_L("Error - Identifier is incorrect!"));
		ret = EFalse;
		}
	// Version check
	TVersion posVer = aInfo->Version();
	if ( (posVer.iMajor != KTestPostProcInfoVersionMaj) ||
		 (posVer.iMinor != KTestPostProcInfoVersionMin) ||
		 (posVer.iBuild != KTestPostProcInfoVersionBuild) )
		{
		ERR_PRINTF1(_L("Error - Version is incorrect!"));
		ret = EFalse;
		}
	// Accelerated check
	if (!aInfo->Accelerated())
		{
		ERR_PRINTF1(_L("Error - Accelerated = EFalse!"));
		ret = EFalse;
		}
	// Direct display check
	if (!aInfo->SupportsDirectDisplay())
		{
		ERR_PRINTF1(_L("Error - Supports Direct Display = EFalse!"));
		ret = EFalse;
		}
	// YUVToRGB Capabilities
	if (!CompareYuvRgbCaps(aInfo->YuvToRgbCapabilities(), KTestPostProcInfoYuvToRgbCaps))
		{
		ERR_PRINTF1(_L("Error - YUV To RGB capabilities is incorrect!"));
		ret = EFalse;
		}
	//Content Protection	
	if (!aInfo->SupportsContentProtected())//Check if flag is set indicating Content Protection is supported.
		{
		ERR_PRINTF1(_L("Error - Failed To get flag indicating support of Content Protection!"));
		ret = EFalse;
		}

	// Supported rotations
	if (aInfo->SupportedRotations() != KTestPostProcInfoRotations)
		{
		ERR_PRINTF1(_L("Error - Supported Rotations is incorrect!"));
		ret = EFalse;
		}

	// Supports arbitrary scaling check
	if (!aInfo->AntiAliasedScaling())
		{
		ERR_PRINTF1(_L("Error - Anti-Aliased Scaling = EFalse!"));
		ret = EFalse;
		}
	// Supports anti-aliased scaling check
	if (!aInfo->SupportsArbitraryScaling())
		{
		ERR_PRINTF1(_L("Error - Supports Arbitrary Scaling = EFalse!"));
		ret = EFalse;
		}
	// Implementation info
	if (!(aInfo->ImplementationSpecificInfo() == KTestDecoderInfoISInfo))
		{
		ERR_PRINTF1(_L("Error - Implementation specific info is incorrect!"));
		ret = EFalse;
		}

	// get some test compressed video formats
	TUncompressedVideoFormat testFormat;
	TUint32 testComb = 0;
	TScaleFactor testFactor = {0, 0};

	// get supported formats
	const RArray<TUncompressedVideoFormat>& vidFormats = aInfo->SupportedFormats();
	
	// get supported combinations
	const RArray<TUint32>& suppCombs = aInfo->SupportedCombinations();

	// get supported scale factors
	const RArray<TScaleFactor>& scaleFactors = aInfo->SupportedScaleFactors();

	// test formats, combinations, and scale factors supported
	for (TUint i = 0; i < KTestPostProcInfoCount; i++)
		{
		testFormat = KTestPostProcInfoFormatArray[i];
		if (!(testFormat == (vidFormats[i])) || !(aInfo->SupportsFormat(testFormat)))
			{
			ERR_PRINTF2(_L("Error - Video Format %d is corrupt!"), i);
			ret = EFalse;
			}

		testComb = KTestPostProcInfoCombsArray[i];
		if (!(testComb == (suppCombs[i])) || !(aInfo->SupportsCombination(testComb)))
			{
			ERR_PRINTF2(_L("Error - Combination %d is corrupt!"), i);
			ret = EFalse;
			}

		testFactor = KTestPostProcInfoScaleFactorsArray[i];
		if (!CompareScaleFactors(testFactor, scaleFactors[i]))
			{
			ERR_PRINTF2(_L("Error - Scale factor %d is corrupt!"), i);
			ret = EFalse;
			}
		}

	// check a format that isn't supposed to be supported
	testFormat = KTestPostProcInfoNegativeFormat;
	testFormat.iYuvFormat = KTestYuvFormat1;
	if (aInfo->SupportsFormat(testFormat))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsFormat() check!"));
		ret = EFalse;
		}
	
	// finally check a combination that isn't supposed to be supported
	testComb = KTestPostProcInfoNegativeComb;
	if (aInfo->SupportsCombination(testComb))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsCombination() check!"));
		ret = EFalse;
		}

	if (!aInfo->SupportsContentProtected())
		{
		ERR_PRINTF1(_L("Error - Failed to Set Content Protected Support!"));
		ret = EFalse;
		}

	return ret;
	}

TVerdict CTestDevVideoPlayGetDecoderPostProcInfo::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetPostProcInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
	CPostProcessorInfo *procInfo = NULL;
	TBool valid = EFalse;
	
	// get decoder information from test decoder plugin
	TRAP(err, 
		procInfo = aDevVideoPlay.PostProcessorInfoLC(KUidDevVideoTestDecodeHwDevice); 
		procInfo->SetSupportsContentProtected(ETrue);
		valid = ValidPostProcInfo(procInfo);
		CleanupStack::PopAndDestroy(procInfo); );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("PostProcessorInfoLC() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	// check the Video Decoder
	else if (!valid)
		{
		ERR_PRINTF1(_L("CPostProcessorInfo class is corrupt!"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("CPostProcessorInfo passed check successfully"));
		INFO_PRINTF3(_L("PostProcessorInfoLC(), %d = %d"), err, expErr);
		}

	return ret;
	}
