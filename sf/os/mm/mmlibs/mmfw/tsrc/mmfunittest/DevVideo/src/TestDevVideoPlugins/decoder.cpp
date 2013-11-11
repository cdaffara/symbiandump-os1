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

#include "decoder.h"
#include "../TestDevVideoPlayTestData.h"

_LIT(KDevVideoDecoderPanicCategory, "DevVideoDecoder");
void DevVideoDecoderPanic(TInt aReason)
	{
	User::Panic(KDevVideoDecoderPanicCategory, aReason);
	}

CMMFVideoDecodeHwDevice* CMMFTestVideoDecodeHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestVideoDecodeHwDevice* s = new(ELeave) CMMFTestVideoDecodeHwDevice;
	return (STATIC_CAST(CMMFVideoDecodeHwDevice*, s));
	}

CMMFTestVideoDecodeHwDevice::CMMFTestVideoDecodeHwDevice()
	:iExtensionUid(KUidDevVideoPlayHwDeviceExtensionScanCopy)
	{
	}

CMMFTestVideoDecodeHwDevice::~CMMFTestVideoDecodeHwDevice()
	{
	// destroy objects in RArray
	for (TInt i = 0; i < iVidFormats.Count(); i++)
		{
		delete iVidFormats[i];
		}

	iVidFormats.Reset();
	iVidFormats.Close();

	iPictureRates.Reset();
	iPictureRates.Close();

	iCombinations.Reset();
	iCombinations.Close();
	
	iPostProcVidFormats.Reset();
	iPostProcVidFormats.Close();
	
	iScaleFactors.Reset();
	iScaleFactors.Close();
		
	delete iBufferDataArea;
	}

TAny* CMMFTestVideoDecodeHwDevice::CustomInterface(TUid aInterface)
	{
   	if (aInterface == KUidCustomInterfaceOne)
   		{
  		return this;//just want to return something non-null!
   		}
	else if (aInterface == iExtensionUid)
		{
		return static_cast<MMMFVideoPlayHwDeviceExtensionScanCopy*>(this);
		}
	else
		{
		return NULL;
		}
	}

// post processor info may be obtained from this plugin or the post processor plugin
CPostProcessorInfo* CMMFTestVideoDecodeHwDevice::PostProcessorInfoLC()
	{
	// construct array of test types
	for (TUint i = 0; i < KTestPostProcInfoCount; i++)
		{
		// append the video formats
		TUncompressedVideoFormat vid = KTestPostProcInfoFormatArray[i];
		User::LeaveIfError(iPostProcVidFormats.Append(vid));

		// append the combinations
		TUint32 comb = KTestPostProcInfoCombsArray[i];
		User::LeaveIfError(iCombinations.Append(comb));

		// append the scale factors
		TScaleFactor scale = KTestPostProcInfoScaleFactorsArray[i];
		User::LeaveIfError(iScaleFactors.Append(scale));
		}
		
	// construct the video decoder info object
	CPostProcessorInfo* info = CPostProcessorInfo::NewL( KUidDevVideoTestDecodeHwDevice,
														 KTestPostProcInfoManufacturer,
														 KTestPostProcInfoIdentifier,
														 TVersion(KTestPostProcInfoVersionMaj,
														 		  KTestPostProcInfoVersionMin,
														 		  KTestPostProcInfoVersionBuild),
														 iPostProcVidFormats.Array(),
														 iCombinations.Array(),
														 ETrue, // accelerated
														 ETrue, // direct display support
														 KTestPostProcInfoYuvToRgbCaps,
														 KTestPostProcInfoRotations,
														 ETrue,	// scaling
														 iScaleFactors.Array(),
														 ETrue,	// anti-aliasing
														 KTestDecoderInfoISInfo );
	CleanupStack::PushL(info);
	
	return info;
	}

void CMMFTestVideoDecodeHwDevice::GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats)
	{
	// append in order 1, 2, 3
	User::LeaveIfError(aFormats.Append(KTestVidFormat1));
	User::LeaveIfError(aFormats.Append(KTestVidFormat2));
	User::LeaveIfError(aFormats.Append(KTestVidFormat3));
	}

void CMMFTestVideoDecodeHwDevice::SetOutputFormatL(const TUncompressedVideoFormat &aFormat)
	{
	if (!(aFormat == KTestVidFormat1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetPostProcessTypesL(TUint32 aPostProcCombination)
	{
	if (!(aPostProcCombination == KTestProcessType1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetInputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestInputCropRectA, KTestInputCropRectB, KTestInputCropRectC, KTestInputCropRectD);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat)
	{
	// check options first
	if (!CompareYuvRgbOptions(aOptions, KTestYuvToRgb1))
		User::Leave(KErrCorrupt);

	// now check formats
	if ( !(CompareYuvFormats(aYuvFormat, KTestYuvFormat1)) || 
		 !(aRgbFormat == KTestRgbFormat1) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions)
	{
	if (!CompareYuvRgbOptions(aOptions, KTestYuvToRgb1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetRotateOptionsL(TRotationType aRotationType)
	{
	if (!(aRotationType == KTestRotate1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering)
	{
	TSize testScale(KTestScaleX, KTestScaleY);
	if (!(aTargetSize == testScale) || !aAntiAliasFiltering)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetOutputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestOutputCropRectA, KTestOutputCropRectB, KTestOutputCropRectC, KTestOutputCropRectD);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetPostProcSpecificOptionsL(const TDesC8& aOptions)
	{
	if (!(aOptions == KTestPostProcOptions1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::SetClockSource(MMMFClockSource* aClock)
	{
	__ASSERT_ALWAYS(aClock, DevVideoDecoderPanic(EDecoderPanicClockSource)); 

	// call Time() to check that clock can be used
	TTimeIntervalMicroSeconds currTime(0); // done this way to remove compiler warning
	currTime = aClock->Time();
	}

void CMMFTestVideoDecodeHwDevice::SetVideoDestScreenL(TBool /*aScreen*/)
	{
	}

void CMMFTestVideoDecodeHwDevice::Initialize()
	{
	iProxy->MdvppInitializeComplete(this, KErrNone);
	}

void CMMFTestVideoDecodeHwDevice::StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& /*aScreenDevice*/, const TRegion& aClipRegion)
	{
	TRect dsaRect(KTestDSARectA, KTestDSARectB, KTestDSARectC, KTestDSARectD);
	TRegionFix<1> dsaReg(dsaRect);

	// probably no need to check aScreenDevice
	if ( /*!(&aScreenDevice) || */!(dsaRect == aVideoRect) || 
		 !(dsaReg.BoundingRect() == aClipRegion.BoundingRect()) )
		 User::Leave(KErrNotSupported);
	}

void CMMFTestVideoDecodeHwDevice::SetScreenClipRegion(const TRegion& aRegion)
	{
	TRect dsaRect(KTestDSARectA, KTestDSARectB, KTestDSARectC, KTestDSARectD);
	TRegionFix<1> dsaReg(dsaRect);

	__ASSERT_ALWAYS(dsaReg.BoundingRect() == aRegion.BoundingRect(), 
		DevVideoDecoderPanic(EDecoderPanicScreenClipRegion)); 
	}

void CMMFTestVideoDecodeHwDevice::SetPauseOnClipFail(TBool aPause)
	{
	__ASSERT_ALWAYS(aPause, DevVideoDecoderPanic(EDecoderPanicPauseClipFail));
	}

void CMMFTestVideoDecodeHwDevice::AbortDirectScreenAccess()
	{
	// do something here?
	}

TBool CMMFTestVideoDecodeHwDevice::IsPlaying()
	{
	return iIsPlaying;
	}

void CMMFTestVideoDecodeHwDevice::Redraw()
	{
	// do something here?
	}

void CMMFTestVideoDecodeHwDevice::Start()
	{
	iIsPlaying = ETrue;
	}

void CMMFTestVideoDecodeHwDevice::Stop()
	{
	iIsPlaying = EFalse;
	}

void CMMFTestVideoDecodeHwDevice::Pause()
	{
	iIsPlaying = EFalse;
	}

void CMMFTestVideoDecodeHwDevice::Resume()
	{
	iIsPlaying = ETrue;
	}

void CMMFTestVideoDecodeHwDevice::SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition)
	{
	if (aPlaybackPosition == TTimeIntervalMicroSeconds(KTestPositionFatal))
		{
		iProxy->MdvppFatalError(this, KErrDied);
		}
	else
		{
		__ASSERT_ALWAYS(aPlaybackPosition == TTimeIntervalMicroSeconds(KTestPosition), DevVideoDecoderPanic(EDecoderPanicSetPosition));
		}
	}

void CMMFTestVideoDecodeHwDevice::FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp)
	{
	__ASSERT_ALWAYS(aTimestamp == TTimeIntervalMicroSeconds(KTestPosition), DevVideoDecoderPanic(EDecoderPanicFreezePicture));
	}

void CMMFTestVideoDecodeHwDevice::ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp)
	{
	__ASSERT_ALWAYS(aTimestamp == TTimeIntervalMicroSeconds(KTestPosition), DevVideoDecoderPanic(EDecoderPanicReleaseFreeze));
	}

TTimeIntervalMicroSeconds CMMFTestVideoDecodeHwDevice::PlaybackPosition()
	{
	return TTimeIntervalMicroSeconds(KTestPlayPosition);
	}

TUint CMMFTestVideoDecodeHwDevice::PictureBufferBytes()
	{
	return KTestPictureBytes;
	}

void CMMFTestVideoDecodeHwDevice::GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters)
	{
	aCounters = GetTestPictureCounters();
	}

void CMMFTestVideoDecodeHwDevice::SetComplexityLevel(TUint aLevel)
	{
	__ASSERT_ALWAYS(aLevel == KTestComplexityLevel1, DevVideoDecoderPanic(EDecoderPanicComplexityLevel));
	}

TUint CMMFTestVideoDecodeHwDevice::NumComplexityLevels()
	{
	return KTestNumComplexityLevels1;
	}

void CMMFTestVideoDecodeHwDevice::GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo)
	{
	__ASSERT_ALWAYS(aLevel == KTestComplexityLevel1, DevVideoDecoderPanic(EDecoderPanicComplexityLevelInfo));

	aInfo = GetTestLevelInfo(aLevel);
	}

void CMMFTestVideoDecodeHwDevice::ReturnPicture(TVideoPicture* /*aPicture*/)
	{
	}

TBool CMMFTestVideoDecodeHwDevice::GetSnapshotL(TPictureData& /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/)
	{
	return EFalse;
	}

// this method should be called on the post processor not on the decoder
// ending up here is a programming error and hence a PANIC condition
void CMMFTestVideoDecodeHwDevice::GetTimedSnapshotL(TPictureData* /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/, const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/)
	{
	DevVideoDecoderPanic(EDecoderPanicTimedSnapshot);
	}

// this method should be called on the post processor not on the decoder
// ending up here is a programming error and hence a PANIC condition
void CMMFTestVideoDecodeHwDevice::GetTimedSnapshotL(TPictureData* /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/, const TPictureId& /*aPictureId*/)
	{
	DevVideoDecoderPanic(EDecoderPanicTimedSnapshotId);
	}

// this method should be called on the post processor not on the decoder
// ending up here is a programming error and hence a PANIC condition
void CMMFTestVideoDecodeHwDevice::CancelTimedSnapshot()
	{
	DevVideoDecoderPanic(EDecoderPanicCancelTimedSnapshot);
	}

// this method should be called on the post processor not on the decoder
// ending up here is a programming error and hence a PANIC condition
void CMMFTestVideoDecodeHwDevice::GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& /*aFormats*/)
	{
	DevVideoDecoderPanic(EDecoderPanicSupportedSnapshotFormats);
	}

void CMMFTestVideoDecodeHwDevice::InputEnd()
	{
	iProxy->MdvppStreamEnd();
	}

CVideoDecoderInfo* CMMFTestVideoDecodeHwDevice::VideoDecoderInfoLC()
	{
	// construct array of test types
	for (TUint i = 0; i < KTestDecoderInfoCount; i++)
		{
		// construct the video types for iVidTypes
		CCompressedVideoFormat* vid = NULL;
		TPtrC8 mimeType = KTestDecoderInfoMimeArray[i];
		vid = GetTestCVFormatL(mimeType);
		CleanupStack::PushL(vid);
		User::LeaveIfError(iVidFormats.Append(vid));
		CleanupStack::Pop(vid);	// CCompressedVideo object is destroyed in destructor

		// append the max picture rates
		TPictureRateAndSize rate;
		GetTestEncoderInfoRate(i, rate);
		User::LeaveIfError(iPictureRates.Append(rate));
		}
	
	// construct the video decoder info object
	CVideoDecoderInfo* vInfo = CVideoDecoderInfo::NewL(
		KUidDevVideoTestDecodeHwDevice,
		KTestDecoderInfoManufacturer,
		KTestDecoderInfoIdentifier,
		TVersion(KTestDecoderInfoVersionMaj, KTestDecoderInfoVersionMin, KTestDecoderInfoVersionBuild),
		iVidFormats.Array(),
		ETrue,	// accelerated
		ETrue,	// supports direct display
		TSize(KTestDecoderInfoMaxSizeX,KTestDecoderInfoMaxSizeY),
		KMaxTUint, //aMaxBitrate
		iPictureRates.Array(),
		ETrue,	// aSupportsPictureLoss
		EFalse, // aSupportsSliceLoss
		KTestDecoderInfoCSInfo,
		KTestDecoderInfoISInfo );
	CleanupStack::PushL(vInfo);
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	vInfo->AddSupportedScreenL(KDecoderDefaultScreenNumber);
	vInfo->AddSupportedScreenL(KDecoderSecondaryScreenNumber);
#endif
	vInfo->SetSupportsContentProtected(ETrue);
	return vInfo;
	}


TVideoPictureHeader* CMMFTestVideoDecodeHwDevice::GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TVideoInputBuffer* aDataUnit)
	{
	// check KTestDataUnitType, KTestDataUnitEncap
	if ((aDataUnitType != KTestDataUnitType) || 
		(aEncapsulation != KTestDataUnitEncap) ||
		(aDataUnit->iOptions != KTestInputBufferOptions) )
		{
		User::Leave(KErrCorrupt);
		}
	
	// repackage picture header
	iPictureHeader.iOptions = KTestPictureHeaderOptions;
	iPictureHeader.iPresentationTimestamp = aDataUnit->iPresentationTimestamp;
	iPictureHeader.iOptional = &(aDataUnit->iData);
	return &iPictureHeader;
	}

void CMMFTestVideoDecodeHwDevice::ConfigureDecoderL(const TVideoPictureHeader& aVideoPictureHeader)
	{
	TTimeIntervalMicroSeconds testTime(KTestInputBufferTimestamp);

	// check the picture header
	if (  (aVideoPictureHeader.iOptions != KTestPictureHeaderOptions)
		||(!(aVideoPictureHeader.iPresentationTimestamp == testTime))
		||(!(*(aVideoPictureHeader.iOptional) == KTestInputBufferData())))
		{
		User::Leave(KErrCorrupt);
		}

	iPictureHeader = aVideoPictureHeader;
	}

void CMMFTestVideoDecodeHwDevice::ReturnHeader(TVideoPictureHeader* aHeader)
	{
	__ASSERT_ALWAYS(aHeader, DevVideoDecoderPanic(EDecoderPanicPictureHeader)); 
	__ASSERT_ALWAYS(aHeader->iOptions == KTestPictureHeaderOptions, DevVideoDecoderPanic(EDecoderPanicPictureHeaderOptions));
	__ASSERT_ALWAYS(aHeader->iPresentationTimestamp == TTimeIntervalMicroSeconds(KTestPictureHeaderTimestamp), DevVideoDecoderPanic(EDecoderPanicPictureHeaderTimestamp));
	}

void CMMFTestVideoDecodeHwDevice::SetInputFormatL(const CCompressedVideoFormat& aFormat, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TBool aDataInOrder)
	{
	// check expected parameters - TClasses first
	if (!((aDataUnitType == KTestUnitType1) && (aEncapsulation == KTestEncapType1) && (aDataInOrder)))
		User::Leave(KErrCorrupt);
	
	// construct a temporary compressed video class	[will leave on error]
	CCompressedVideoFormat *compVideo = GetTestCVFormatL(KTestMimeType1);
	CleanupStack::PushL(compVideo);

	// compare to received class
	if (!(aFormat == *compVideo))
		User::Leave(KErrCorrupt);

	// destroy temporary class
	CleanupStack::PopAndDestroy(compVideo);
	}

void CMMFTestVideoDecodeHwDevice::SynchronizeDecoding(TBool aSynchronize)
	{
	__ASSERT_ALWAYS(aSynchronize, DevVideoDecoderPanic(EDecoderPanicSynchronizeDecoding)); 
	}

void CMMFTestVideoDecodeHwDevice::SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions)
	{
	CMMFDevVideoPlay::TBufferOptions buffOptions = GetTestBufferOptions();

	if (!CompareBufferOptions(aOptions, buffOptions))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoDecodeHwDevice::GetBufferOptions(CMMFDevVideoPlay::TBufferOptions& aOptions)
	{
	aOptions = GetTestBufferOptions();
	}

void CMMFTestVideoDecodeHwDevice::SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams)
	{
	__ASSERT_ALWAYS(aHrdVbvSpec == KTestHrdVbvSpec, DevVideoDecoderPanic(EDecoderPanicHrdVbvSpec)); 
	__ASSERT_ALWAYS(aHrdVbvParams == KTestHrdVbvParams, DevVideoDecoderPanic(EDecoderPanicHrdVbvParams));
	}

void CMMFTestVideoDecodeHwDevice::SetOutputDevice(CMMFVideoPostProcHwDevice* /*aDevice*/)
	{
	}

TTimeIntervalMicroSeconds CMMFTestVideoDecodeHwDevice::DecodingPosition()
	{
	return TTimeIntervalMicroSeconds(KTestDecodePosition);
	}

TUint CMMFTestVideoDecodeHwDevice::PreDecoderBufferBytes()
	{
	return KTestPreDecoderBytes;
	}

void CMMFTestVideoDecodeHwDevice::GetBitstreamCounters(CMMFDevVideoPlay::TBitstreamCounters& aCounters)
	{
	aCounters = GetTestBitstreamCounters();
	}

TUint CMMFTestVideoDecodeHwDevice::NumFreeBuffers()
	{
	return KTestNumFreeBuffers;
	}

TVideoInputBuffer* CMMFTestVideoDecodeHwDevice::GetBufferL(TUint aBufferSize)
	{
	if (!iBufferDataArea)
		{
		TPtrC8 testBufferString(KTestBufferString);
		iBufferDataArea = testBufferString.AllocL();
		}

	TPtr8 dataAreaPtr = iBufferDataArea->Des();
	TInt reqBufferSize = aBufferSize;
	if (reqBufferSize > dataAreaPtr.MaxLength())
		User::Leave(KErrTooBig);

	// initialize iInputBuffer with test data
	iInputBuffer.iOptions = KTestBufferOptions;
	iInputBuffer.iDecodingTimestamp = aBufferSize;
	iInputBuffer.iData.Set(dataAreaPtr);

	// call new buffer callback
	iProxy->MdvppNewBuffers();

	return &iInputBuffer;
	}

void CMMFTestVideoDecodeHwDevice::WriteCodedDataL(TVideoInputBuffer* aBuffer)
	{
	TTimeIntervalMicroSeconds testTime(KTestBufferSize);

	// check received buffer against test data
	if (!(aBuffer->iOptions == KTestBufferOptions) ||
		!(aBuffer->iDecodingTimestamp == testTime) ||
		!(aBuffer->iData == KTestBufferString))
		{
		User::Leave(KErrCorrupt);
		}
	}

void CMMFTestVideoDecodeHwDevice::WriteCodedDataL(TVideoInputBuffer* aBuffer, TFramePortion aPortion)
	{
	TTimeIntervalMicroSeconds testTime(KTestBufferSize);

	// check received buffer against test data
	if  ((aPortion != EFramePortionEndFragment) ||
		!(aBuffer->iOptions == KTestBufferOptions) ||
		!(aBuffer->iDecodingTimestamp == testTime) ||
		!(aBuffer->iData == KTestBufferString))
		{
		User::Leave(KErrCorrupt);
		}
	}

void CMMFTestVideoDecodeHwDevice::ScanAndCopyCodedDataL(TPtr8 aCodedData, TVideoInputBuffer* aBuffer, TInt& aConsumed, TFramePortion aPortion)
	{
	//compare ptr data to test data
	if ((aPortion != EFramePortionEndFragment) ||
		(aCodedData != KTestBufferString))
		{
		User::Leave(KErrCorrupt);
		}
	//copy data into buffer
	aBuffer->iData.Set(aCodedData);
	aConsumed = aBuffer->iData.Length();
	}

void CMMFTestVideoDecodeHwDevice::CommitL()
	{
	}

void CMMFTestVideoDecodeHwDevice::Revert()
	{
	}

void CMMFTestVideoDecodeHwDevice::SetProxy(MMMFDevVideoPlayProxy& aProxy)
	{
	ASSERT(iProxy == NULL);
	iProxy = &aProxy;
	}
