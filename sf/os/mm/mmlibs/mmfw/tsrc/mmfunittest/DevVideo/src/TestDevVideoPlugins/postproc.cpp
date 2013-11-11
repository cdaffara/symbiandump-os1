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
//

#include "postproc.h"
#include "../TestDevVideoPlayTestData.h"

_LIT(KDevVideoPostProcPanicCategory, "DevVideoPostProcessor");
void DevVideoPostProcPanic(TInt aReason)
	{
	User::Panic(KDevVideoPostProcPanicCategory, aReason);
	}

CMMFVideoPostProcHwDevice* CMMFTestVideoPostProcHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestVideoPostProcHwDevice* s = new(ELeave) CMMFTestVideoPostProcHwDevice;
	return (STATIC_CAST(CMMFVideoPostProcHwDevice*, s));
	}

CMMFTestVideoPostProcHwDevice::CMMFTestVideoPostProcHwDevice()
	{
	}

CMMFTestVideoPostProcHwDevice::~CMMFTestVideoPostProcHwDevice()
	{
	iVideoPictures.Close();
	
	iVidFormats.Reset();
	iVidFormats.Close();

	iScaleFactors.Reset();
	iScaleFactors.Close();

	iCombinations.Reset();
	iCombinations.Close();
	}

TAny* CMMFTestVideoPostProcHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidCustomInterfaceTwo)
		{
		return this;//just want to return something non-null!
		}
	else
		{
		return NULL;
		}
	}

CPostProcessorInfo* CMMFTestVideoPostProcHwDevice::PostProcessorInfoLC()
	{
	// construct array of test types
	for (TUint i = 0; i < KTestPostProcInfoCount; i++)
		{
		// append the video formats
		TUncompressedVideoFormat vid = KTestPostProcInfoFormatArray[i];
		User::LeaveIfError(iVidFormats.Append(vid));

		// append the combinations
		TUint32 comb = KTestPostProcInfoCombsArray[i];
		User::LeaveIfError(iCombinations.Append(comb));

		// append the scale factors
		TScaleFactor scale = KTestPostProcInfoScaleFactorsArray[i];
		User::LeaveIfError(iScaleFactors.Append(scale));
		}
	
	// construct the video decoder info object
	CPostProcessorInfo* vInfo = CPostProcessorInfo::NewL(
		KUidDevVideoTestPostProcHwDevice,
		KTestPostProcInfoManufacturer,
		KTestPostProcInfoIdentifier,
		TVersion(KTestPostProcInfoVersionMaj, KTestPostProcInfoVersionMin, KTestPostProcInfoVersionBuild),
		iVidFormats.Array(),
		iCombinations.Array(),
		ETrue,	// accelerated
		ETrue,	// supports direct display
		KTestPostProcInfoYuvToRgbCaps,
		KTestPostProcInfoRotations,
		ETrue,	// scaling
		iScaleFactors.Array(),
		ETrue,	// anti-aliasing
		KTestPostProcInfoISInfo );
	CleanupStack::PushL(vInfo);
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	vInfo->AddSupportedScreenL(KPostProcDefaultScreenNumber);
	vInfo->AddSupportedScreenL(KPostProcSecondaryScreenNumber);
#endif
	vInfo->SetSupportsContentProtected(ETrue);
	return vInfo;
	}

void CMMFTestVideoPostProcHwDevice::GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats)
	{
	// append in order 3, 2, 1
	User::LeaveIfError(aFormats.Append(KTestVidFormat3));
	User::LeaveIfError(aFormats.Append(KTestVidFormat2));
	User::LeaveIfError(aFormats.Append(KTestVidFormat1));
	}

void CMMFTestVideoPostProcHwDevice::SetOutputFormatL(const TUncompressedVideoFormat &aFormat)
	{
	if (!(aFormat == KTestVidFormat2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetPostProcessTypesL(TUint32 aPostProcCombination)
	{
	if (!(aPostProcCombination == KTestProcessType2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetInputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestInputCropRectD, KTestInputCropRectC, KTestInputCropRectB, KTestInputCropRectA);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat)
	{
	// check options first
	if (!CompareYuvRgbOptions(aOptions, KTestYuvToRgb2))
		User::Leave(KErrCorrupt);

	// now check formats
	if ( !(CompareYuvFormats(aYuvFormat, KTestYuvFormat2)) || 
		 !(aRgbFormat == KTestRgbFormat2) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions)
	{
	if (!CompareYuvRgbOptions(aOptions, KTestYuvToRgb2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetRotateOptionsL(TRotationType aRotationType)
	{
	if (!(aRotationType == KTestRotate2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering)
	{
	TSize testScale(KTestScaleY, KTestScaleX);
	if (!(aTargetSize == testScale) || !aAntiAliasFiltering)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetOutputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestOutputCropRectD, KTestOutputCropRectC, KTestOutputCropRectB, KTestOutputCropRectA);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetPostProcSpecificOptionsL(const TDesC8& aOptions)
	{
	if (!(aOptions == KTestPostProcOptions2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPostProcHwDevice::SetClockSource(MMMFClockSource* aClock)
	{
	__ASSERT_ALWAYS(aClock, DevVideoPostProcPanic(EPostProcPanicClockSource)); 

	// call Time() to check that clock can be used
	TTimeIntervalMicroSeconds currTime(0); // done this way to remove compiler warning
	currTime = aClock->Time();
	}

void CMMFTestVideoPostProcHwDevice::SetVideoDestScreenL(TBool aScreen)
	{
	if (aScreen)
		{
		// Must be normal devvideo test.  Leave with special number to show we got the
		// value through ok
		User::Leave(KErrHardwareNotAvailable);
		}
	else
		{
		// Special test plugin feature: we're being told to start outputing
		// video pictures up to DevVideo.
		// Create 10 video pictures
		iVideoPictures.Reset();
		for (TInt i=0; i<10; i++)
			{
			// Create a video picture that only has a valid timestamp
			TVideoPicture picture;
			picture.iTimestamp = TTimeIntervalMicroSeconds(i);
			picture.iOptions = TVideoPicture::ETimestamp;
			User::LeaveIfError(iVideoPictures.Append(picture));
			}
		}
	}

void CMMFTestVideoPostProcHwDevice::Initialize()
	{
	iProxy->MdvppInitializeComplete(this, KErrNone);
	}

void CMMFTestVideoPostProcHwDevice::StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& /*aScreenDevice*/, const TRegion& aClipRegion)
	{
	TRect dsaRect(KTestDSARectA, KTestDSARectB, KTestDSARectC, KTestDSARectD);
	TRegionFix<1> dsaReg(dsaRect);

	// probably no need to check aScreenDevice
	if ( /*!(&aScreenDevice) || */!(dsaRect == aVideoRect) || 
		 !(dsaReg.BoundingRect() == aClipRegion.BoundingRect()) )
		 User::Leave(KErrNotSupported);

	}

void CMMFTestVideoPostProcHwDevice::SetScreenClipRegion(const TRegion& aRegion)
	{
	TRect dsaRect(KTestDSARectA, KTestDSARectB, KTestDSARectC, KTestDSARectD);
	TRegionFix<1> dsaReg(dsaRect);

	__ASSERT_ALWAYS(dsaReg.BoundingRect() == aRegion.BoundingRect(), 
		DevVideoPostProcPanic(EPostProcPanicScreenClipRegion)); 
	}

void CMMFTestVideoPostProcHwDevice::SetPauseOnClipFail(TBool aPause)
	{
	__ASSERT_ALWAYS(aPause, DevVideoPostProcPanic(EPostProcPanicPauseClipFail));
	}

void CMMFTestVideoPostProcHwDevice::AbortDirectScreenAccess()
	{
	// do nothing - there is no way to check this
	}

TBool CMMFTestVideoPostProcHwDevice::IsPlaying()
	{
	return iIsPlaying;
	}

void CMMFTestVideoPostProcHwDevice::Redraw()
	{
	// do nothing - there is no way to check this
	}

void CMMFTestVideoPostProcHwDevice::Start()
	{
	// set iIsPlaying flag
	iIsPlaying = ETrue;

	// if we're doing the picture tests, start sending pictures to DevVideo
	if (iVideoPictures.Count() > 0)
		{
		iProxy->MdvppNewPicture(&(iVideoPictures[0]));
		}
	}

void CMMFTestVideoPostProcHwDevice::Stop()
	{
	iIsPlaying = EFalse;
	}

void CMMFTestVideoPostProcHwDevice::Pause()
	{
	iIsPlaying = EFalse;
	}

void CMMFTestVideoPostProcHwDevice::Resume()
	{
	iIsPlaying = ETrue;
	}

void CMMFTestVideoPostProcHwDevice::SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition)
	{
	if (aPlaybackPosition == TTimeIntervalMicroSeconds(KTestPositionFatal))
		{
		iProxy->MdvppFatalError(this, KErrDied);
		}
	else
		{
		__ASSERT_ALWAYS(aPlaybackPosition == TTimeIntervalMicroSeconds(KTestPosition), DevVideoPostProcPanic(EPostProcPanicSetPosition));
		}
	}

void CMMFTestVideoPostProcHwDevice::FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp)
	{
	__ASSERT_ALWAYS(aTimestamp == TTimeIntervalMicroSeconds(KTestPosition), DevVideoPostProcPanic(EPostProcPanicFreezePicture));
	}

void CMMFTestVideoPostProcHwDevice::ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp)
	{
	__ASSERT_ALWAYS(aTimestamp == TTimeIntervalMicroSeconds(KTestPosition), DevVideoPostProcPanic(EPostProcPanicReleaseFreeze));
	}

TTimeIntervalMicroSeconds CMMFTestVideoPostProcHwDevice::PlaybackPosition()
	{
	return TTimeIntervalMicroSeconds(KTestPlayPosition);
	}

TUint CMMFTestVideoPostProcHwDevice::PictureBufferBytes()
	{
	return KTestPictureBytes;
	}

void CMMFTestVideoPostProcHwDevice::GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters)
	{
	aCounters = GetTestPictureCounters();
	}

void CMMFTestVideoPostProcHwDevice::SetComplexityLevel(TUint aLevel)
	{
	__ASSERT_ALWAYS(aLevel == KTestComplexityLevel2, DevVideoPostProcPanic(EPostProcPanicComplexityLevel));
	}

TUint CMMFTestVideoPostProcHwDevice::NumComplexityLevels()
	{
	return KTestNumComplexityLevels2;
	}

void CMMFTestVideoPostProcHwDevice::GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo)
	{
	__ASSERT_ALWAYS(aLevel == KTestComplexityLevel2, DevVideoPostProcPanic(EPostProcPanicComplexityLevelInfo));

	aInfo = GetTestLevelInfo(aLevel);;
	}

void CMMFTestVideoPostProcHwDevice::ReturnPicture(TVideoPicture* /*aPicture*/)
	{
	switch (iCurrentPicture)
		{
	case 0:
		// Send two more pictures
		iCurrentPicture = 1;
		iProxy->MdvppNewPicture(&(iVideoPictures[1]));
		iCurrentPicture = 2;
		iProxy->MdvppNewPicture(&(iVideoPictures[2]));
		break;
	case 2:
		// Send three more pictures
		iCurrentPicture = 3;
		iProxy->MdvppNewPicture(&(iVideoPictures[3]));
		iCurrentPicture = 4;
		iProxy->MdvppNewPicture(&(iVideoPictures[4]));
		iCurrentPicture = 5;
		iProxy->MdvppNewPicture(&(iVideoPictures[5]));
		break;
	case 5:
		// Send the rest of the pictures
		iCurrentPicture = 6;
		iProxy->MdvppNewPicture(&(iVideoPictures[6]));
		iCurrentPicture = 7;
		iProxy->MdvppNewPicture(&(iVideoPictures[7]));
		iCurrentPicture = 8;
		iProxy->MdvppNewPicture(&(iVideoPictures[8]));
		iCurrentPicture = 9;
		iProxy->MdvppNewPicture(&(iVideoPictures[9]));
		break;
	default:
		break;
		};
	}

TBool CMMFTestVideoPostProcHwDevice::GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat)
	{
	TBool ret = EFalse;

	if (aFormat.iDataFormat == EYuvRawData)
		{
		if (CompareYuvFormats(aFormat.iYuvFormat, KTestYuvFormat1))
			{
			ret = ETrue;
			aPictureData.iDataFormat = EYuvRawData;
			aPictureData.iDataSize = TSize(KTestPictureDataSizeX, KTestPictureDataSizeY);
			aPictureData.iRawData = NULL;
			}
		}

	return ret;
	}

void CMMFTestVideoPostProcHwDevice::GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TTimeIntervalMicroSeconds& aPresentationTimestamp)
	{
	if ((aFormat.iDataFormat == EYuvRawData) && (aPresentationTimestamp == TTimeIntervalMicroSeconds(KTestSnapshotTimestamp)) )
		{
		if (CompareYuvFormats(aFormat.iYuvFormat, KTestYuvFormat1))
			{
			aPictureData->iDataFormat = EYuvRawData;
			aPictureData->iDataSize = TSize(KTestPictureDataSizeX, KTestPictureDataSizeY);
			aPictureData->iRawData = NULL;

			// call the snapshot callback
			iProxy->MdvppTimedSnapshotComplete(KErrNone, aPictureData, aPresentationTimestamp, iPictureId);
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CMMFTestVideoPostProcHwDevice::GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TPictureId& aPictureId)
	{
	if ((aFormat.iDataFormat == EYuvRawData) && 
		(aPictureId.iIdType == KTestSnapshotId.iIdType) &&
		(aPictureId.iId == KTestSnapshotId.iId) )
		{
		if (CompareYuvFormats(aFormat.iYuvFormat, KTestYuvFormat1))
			{
			aPictureData->iDataFormat = EYuvRawData;
			aPictureData->iDataSize = TSize(KTestPictureDataSizeX, KTestPictureDataSizeY);
			aPictureData->iRawData = NULL;

			iPictureTimestamp = 0;

			// call the snapshot callback
			iProxy->MdvppTimedSnapshotComplete(KErrNone, aPictureData, iPictureTimestamp, aPictureId);
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

void CMMFTestVideoPostProcHwDevice::CancelTimedSnapshot()
	{
	// call the snapshot callback with error
	iProxy->MdvppTimedSnapshotComplete(KErrAbort, &iPictureData, iPictureTimestamp, iPictureId);
	}

void CMMFTestVideoPostProcHwDevice::GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats)
	{
	// append in order 3, 2, 1
	User::LeaveIfError(aFormats.Append(KTestVidFormat1));
	User::LeaveIfError(aFormats.Append(KTestVidFormat2));
	User::LeaveIfError(aFormats.Append(KTestVidFormat3));
	}

void CMMFTestVideoPostProcHwDevice::InputEnd()
	{
	iProxy->MdvppStreamEnd();
	}

void CMMFTestVideoPostProcHwDevice::SetInputFormatL(const TUncompressedVideoFormat& aFormat)
	{
	if (!(aFormat == KTestVidFormat2))
		{
		User::Leave(KErrCorrupt);
		}
	}

void CMMFTestVideoPostProcHwDevice::SetInputDevice(CMMFVideoDecodeHwDevice* /*aDevice*/)
	{
	}

void CMMFTestVideoPostProcHwDevice::WritePictureL(TVideoPicture* aPicture)
	{
	iProxy->MdvppReturnPicture(aPicture);
	}

void CMMFTestVideoPostProcHwDevice::CommitL()
	{
	}

void CMMFTestVideoPostProcHwDevice::Revert()
	{
	}

void CMMFTestVideoPostProcHwDevice::SetProxy(MMMFDevVideoPlayProxy& aProxy)
	{
	iProxy = &aProxy;
	}

