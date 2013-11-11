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

#include "encoder.h"
#include "../TestDevVideoPlayTestData.h"

_LIT(KDevVideoEncoderPanicCategory, "DevVideoEncoder");
void DevVideoEncoderPanic(TInt aReason)
	{
	User::Panic(KDevVideoEncoderPanicCategory, aReason);
	}

CMMFVideoEncodeHwDevice* CMMFTestVideoEncodeHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestVideoEncodeHwDevice* s = new(ELeave) CMMFTestVideoEncodeHwDevice;
	return (STATIC_CAST(CMMFVideoEncodeHwDevice*, s));
	}

CMMFTestVideoEncodeHwDevice::CMMFTestVideoEncodeHwDevice()
	{
	}

CMMFTestVideoEncodeHwDevice::~CMMFTestVideoEncodeHwDevice()
	{
	iDataBuffers.Reset();
	iDataBuffers.Close();

	// destroy objects in RArray
	for (TInt i = 0; i < iOutputVidFormats.Count(); i++)
		{
		delete iOutputVidFormats[i];
		}

	iOutputVidFormats.Reset();
	iOutputVidFormats.Close();

	iInputVidFormats.Reset();
	iInputVidFormats.Close();

	iPictureRates.Reset();
	iPictureRates.Close();
	}

TAny* CMMFTestVideoEncodeHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidCustomInterfaceThree)
		{
		return this;//just want to return something non-null!
		}
	else
		{
		return NULL;
		}
	}

CPreProcessorInfo* CMMFTestVideoEncodeHwDevice::PreProcessorInfoLC()
	{
	return NULL;
	}

void CMMFTestVideoEncodeHwDevice::SetInputFormatL(const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize)
	{
	TSize testSize(KTestInputSize1X, KTestInputSize1Y);

	if (!(aFormat == KTestVidFormat1) || !(aPictureSize == testSize))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate)
	{
	if (aCameraHandle == KTestCamHandleFatal)
		iProxy->MdvrpFatalError(this, KErrDied);
	else if ((KTestCamHandleEnc != aCameraHandle) || (KTestPictureRate != aPictureRate))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetSourceMemoryL(TReal /*aMaxPictureRate*/, TBool /*aConstantPictureRate*/, TBool /*aProcessRealtime*/)
	{
	DevVideoEncoderPanic(EEncoderPanicSourceMem);
	}

void CMMFTestVideoEncodeHwDevice::SetClockSource(MMMFClockSource* aClock) 
	{
	__ASSERT_ALWAYS(aClock, DevVideoEncoderPanic(EEncoderPanicClockSource)); 

	// call Time() to check that clock can be used
	TTimeIntervalMicroSeconds currTime(0); // done this way to remove compiler warning
	currTime = aClock->Time();
	}

void CMMFTestVideoEncodeHwDevice::SetPreProcessTypesL(TUint32 aPreProcessTypes)
	{
	if (!(aPreProcessTypes == KTestProcessType1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetRgbToYuvOptionsL(TRgbRange aRange, const TYuvFormat& aOutputFormat)
	{
	// check against test data
	if ((aRange != KTestRgbRange1) || !CompareYuvFormats(aOutputFormat, KTestYuvFormat1) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetYuvToYuvOptionsL(const TYuvFormat& aInputFormat, const TYuvFormat& aOutputFormat)
	{
	if (!CompareYuvFormats(aInputFormat, KTestYuvFormat1) || !CompareYuvFormats(aOutputFormat, KTestYuvFormat2) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetRotateOptionsL(TRotationType aRotationType)
	{
	if (!(aRotationType == KTestRotate1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering)
	{
	TSize testScale(KTestScaleX, KTestScaleY);
	if (!(aTargetSize == testScale) || !aAntiAliasFiltering)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetInputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestInputCropRectA, KTestInputCropRectB, KTestInputCropRectC, KTestInputCropRectD);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetOutputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestOutputCropRectA, KTestOutputCropRectB, KTestOutputCropRectC, KTestOutputCropRectD);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetOutputPadOptionsL(const TSize& aOutputSize, const TPoint& aPicturePos)
	{
	TSize testPad(KTestPadX, KTestPadY);
	TPoint testPoint(KTestPadPointX, KTestPadPointY);

	if (!(testPad == aOutputSize) || !(testPoint == aPicturePos))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetColorEnhancementOptionsL(const TColorEnhancementOptions& aOptions)
	{
	if (!CompareColorEnhancements(aOptions, KTestColorEnhance1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetFrameStabilisationOptionsL(const TSize& aOutputSize, TBool aFrameStabilisation)
	{
	TSize testSize(KTestScaleX, KTestScaleY);
	if (!(aOutputSize == testSize) || !aFrameStabilisation)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetCustomPreProcessOptionsL(const TDesC8& aOptions)
	{
	if (!(aOptions == KTestCustomPreProc1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::Initialize()
	{
	iProxy->MdvrpInitializeComplete(this, KErrNone);
	}

void CMMFTestVideoEncodeHwDevice::WritePictureL(TVideoPicture* aPicture)
	{
	// change the timestamp to the encoder timestamp
	aPicture->iTimestamp = KTestEncPictureTimestamp;

	iProxy->MdvrpReturnPicture(aPicture);
	}

void CMMFTestVideoEncodeHwDevice::InputEnd()
	{
	iProxy->MdvrpStreamEnd();
	}

void CMMFTestVideoEncodeHwDevice::Start()
	{
	iRecPosition = KTestRecTimeStartEnc;
	iIsRecording = ETrue;

	// if we're doing the buffer tests, start sending buffers to DevVideo
	if (iDataBuffers.Count() > 0)
		{
		iProxy->MdvrpNewBuffer(&(iDataBuffers[0]));
		}
	}

void CMMFTestVideoEncodeHwDevice::Stop()
	{
	iRecPosition = KTestRecTimeStop;
	iIsRecording = EFalse;
	}

void CMMFTestVideoEncodeHwDevice::Pause()
	{
	iRecPosition = KTestRecTimePauseEnc;
	iIsRecording = EFalse;
	}

void CMMFTestVideoEncodeHwDevice::Resume()
	{
	iRecPosition = KTestRecTimeResumeEnc;
	iIsRecording = ETrue;
	}

void CMMFTestVideoEncodeHwDevice::Freeze()
	{
	}

void CMMFTestVideoEncodeHwDevice::ReleaseFreeze()
	{
	}

TTimeIntervalMicroSeconds CMMFTestVideoEncodeHwDevice::RecordingPosition()
	{
	return iRecPosition;
	}

void CMMFTestVideoEncodeHwDevice::GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& aCounters)
	{	
	CMMFDevVideoRecord::TPictureCounters pic = GetTestEncPictureCounters(); 
	
	// adjust picture counters to differentiate encoder from preproc
	pic.iInputPictures = KTestEncInputPictures;
	aCounters = pic;
	}

void CMMFTestVideoEncodeHwDevice::GetFrameStabilisationOutput(TRect& aRect)
	{
	aRect = TRect(KTestFrameStableX1, KTestFrameStableY1, KTestFrameStableX2, KTestFrameStableY2);
	}

TUint CMMFTestVideoEncodeHwDevice::NumComplexityLevels()
	{
	return KTestNumComplexityLevels1;
	}

void CMMFTestVideoEncodeHwDevice::SetComplexityLevel(TUint aLevel)
	{
	__ASSERT_ALWAYS(aLevel == KTestComplexityLevel1, DevVideoEncoderPanic(EEncoderPanicComplexityLevel));
	}

CVideoEncoderInfo* CMMFTestVideoEncodeHwDevice::VideoEncoderInfoLC()
	{
	// construct array of test types
	for (TUint i = 0; i < KTestEncoderInfoCount; i++)
		{
		// construct the video types for iOutputVidTypes
		// use the same test data as for the decoder
		CCompressedVideoFormat* vidCV = NULL;
		TPtrC8 mimeType = KTestDecoderInfoMimeArray[i];
		vidCV = GetTestCVFormatL(mimeType);
		CleanupStack::PushL(vidCV);
		User::LeaveIfError(iOutputVidFormats.Append(vidCV));
		CleanupStack::Pop(vidCV);	// CCompressedVideo object is destroyed in destructor

		// append the uncompressed video formats
		TUncompressedVideoFormat vidUC = KTestPostProcInfoFormatArray[i];
		User::LeaveIfError(iInputVidFormats.Append(vidUC));

		// append the max picture rates
		TPictureRateAndSize rate;
		GetTestEncoderInfoRate(i, rate);
		User::LeaveIfError(iPictureRates.Append(rate));
		}
	
	// construct the video Encoder info object
	CVideoEncoderInfo* vInfo = CVideoEncoderInfo::NewL(
		KUidDevVideoTestEncodeHwDevice,
		KTestEncoderInfoManufacturer,
		KTestEncoderInfoIdentifier,
		TVersion(KTestEncoderInfoVersionMaj, KTestEncoderInfoVersionMin, KTestEncoderInfoVersionBuild),
		ETrue, // accelerated
		ETrue, // supports direct capture
		iInputVidFormats.Array(),
		iOutputVidFormats.Array(),
		TSize(KTestEncoderInfoMaxSizeX, KTestEncoderInfoMaxSizeY),
		KTestUnitType1,
		KTestEncapType1,
		KTestNumBitrateLayers,
		ETrue, // supports supplemental enhancement info
		KTestEncoderInfoMaxUEPLevels,
		KTestEncoderInfoMaxBitrate,
		iPictureRates.Array(),
		KTestEncoderInfoMaxILSSteps,
		KTestEncoderInfoPictureOptions,
		EFalse, // aSupportsPictureLoss,
		ETrue, // aSupportsSliceLoss,
		KTestEncoderInfoCSInfo,
		KTestEncoderInfoISInfo );
		

	CleanupStack::PushL(vInfo);
	return vInfo;
	}

void CMMFTestVideoEncodeHwDevice::SetOutputFormatL(const CCompressedVideoFormat& aFormat, 
					  TVideoDataUnitType aDataUnitType, 					
					  TVideoDataUnitEncapsulation aDataEncapsulation,
					  TBool aSegmentationAllowed)
	{
	// check expected parameters - TClasses first
	if (!((aDataUnitType == KTestUnitType1) && (aDataEncapsulation == KTestEncapType1) && aSegmentationAllowed))
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

void CMMFTestVideoEncodeHwDevice::SetOutputRectL(const TRect& aRect)
	{
	TRect testRect1(KTestInputCropRectA, KTestInputCropRectB, KTestInputCropRectC, KTestInputCropRectD);

	if (!(aRect == testRect1))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetInputDevice(CMMFVideoPreProcHwDevice* /*aDevice*/)
	{
	}

void CMMFTestVideoEncodeHwDevice::SetErrorsExpected(TBool aBitErrors, TBool aPacketLosses)
	{
	__ASSERT_ALWAYS(aBitErrors && !aPacketLosses, DevVideoEncoderPanic(EEncoderPanicErrorsExpected)); 
	}

void CMMFTestVideoEncodeHwDevice::SetMinRandomAccessRate(TReal aRate)
	{
	__ASSERT_ALWAYS(aRate == KTestRandomAccessRate, DevVideoEncoderPanic(EEncoderPanicRandomAccessRate)); 
	}

void CMMFTestVideoEncodeHwDevice::SetNumBitrateLayersL(TUint aNumLayers)
	{
	if (aNumLayers != KTestNumBitrateLayers)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetScalabilityLayerTypeL(TUint aLayer, TScalabilityType aScalabilityType)
	{
	if ((aLayer != KTestLayer) || (aScalabilityType != KTestScaleType))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetGlobalReferenceOptions(TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay)
	{
	__ASSERT_ALWAYS((aMaxReferencePictures == KTestMaxRefPics) && (aMaxPictureOrderDelay == KTestMaxPicDelay), DevVideoEncoderPanic(EEncoderPanicRandomAccessRate)); 
	}

void CMMFTestVideoEncodeHwDevice::SetLayerReferenceOptions(TUint aLayer, TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay)
	{
	__ASSERT_ALWAYS((aMaxReferencePictures == KTestMaxRefPics) && (aMaxPictureOrderDelay == KTestMaxPicDelay) && (aLayer == KTestLayer), DevVideoEncoderPanic(EEncoderPanicRandomAccessRate)); 
	}

void CMMFTestVideoEncodeHwDevice::SetBufferOptionsL(const TEncoderBufferOptions& aOptions)
	{
	if ( !CompareEncBufferOptions(aOptions, GetTestEncBufferOptions()) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetCodingStandardSpecificOptionsL(const TDesC8& aOptions)
	{
	if (!(aOptions == KTestCSEncoderOptions))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetImplementationSpecificEncoderOptionsL(const TDesC8& aOptions)
	{
	if (aOptions == KTestISEncoderOptions)
		{
		// running standard test
		}
	else if (aOptions == KTestISEncBuffers)
		{
		// Special test plugin feature: we're being told to start outputting
		// video buffers up to DevVideo.
		// Create 10 video buffers
		iDataBuffers.Reset();
		for (TInt i = 0; i < 10; i++)
			{
			// Create a video output buffer that only has a valid timestamp
			TVideoOutputBuffer buf;
			buf.iCaptureTimestamp = TTimeIntervalMicroSeconds(i);
			buf.iImplementationSpecificData.Set(KTestISEncBufferData);
			User::LeaveIfError(iDataBuffers.Append(buf));
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

HBufC8* CMMFTestVideoEncodeHwDevice::CodingStandardSpecificInitOutputLC()
	{
	HBufC8* bob = KTestCSInitOutput().AllocL();
	CleanupStack::PushL(bob);

	return bob;
	}

HBufC8* CMMFTestVideoEncodeHwDevice::ImplementationSpecificInitOutputLC()
	{
	HBufC8* fred = KTestISInitOutput().AllocL();
	CleanupStack::PushL(fred);

	return fred;
	}

void CMMFTestVideoEncodeHwDevice::SetErrorProtectionLevelsL(TUint aNumLevels, TBool aSeparateBuffers)
	{
	if ((aNumLevels != KTestProtectLevels) || !aSeparateBuffers)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetErrorProtectionLevelL(TUint aLevel, TUint aBitrate, TUint aStrength)
	{
	if ((aLevel != KTestLevel) || (aBitrate != KTestBitrate) || (aStrength != KTestStrength))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetChannelPacketLossRate(TUint aLevel, 
							  TReal aLossRate, 
							  TTimeIntervalMicroSeconds32 aLossBurstLength)
	{
	__ASSERT_ALWAYS((aLevel == KTestLevel) && (aLossRate == KTestLossRate) && (aLossBurstLength == TTimeIntervalMicroSeconds32(KTestLossBurstLength)), DevVideoEncoderPanic(EEncoderPanicPacketLossRate)); 
	}

void CMMFTestVideoEncodeHwDevice::SetChannelBitErrorRate(TUint aLevel, TReal aErrorRate, TReal aStdDeviation)
	{
	__ASSERT_ALWAYS((aLevel == KTestLevel) && (aErrorRate == KTestErrorRate) && (aStdDeviation == KTestStdDeviation), DevVideoEncoderPanic(EEncoderPanicBitErrorRate));	
	}

void CMMFTestVideoEncodeHwDevice::SetSegmentTargetSize(TUint aLayer, TUint aSizeBytes, TUint aSizeMacroblocks)
	{
	__ASSERT_ALWAYS((aLayer == KTestLayer) && (aSizeBytes == KTestSizeBytes) && (aSizeMacroblocks == KTestSizeMacroblocks), DevVideoEncoderPanic(EEncoderPanicSegmentTargetSize));	
	}

void CMMFTestVideoEncodeHwDevice::SetRateControlOptions(TUint aLayer, const TRateControlOptions& aOptions)
	{
	__ASSERT_ALWAYS(CompareRateControlOptions(aOptions, GetTestRateControlOptions()) && (aLayer == KTestLayer), DevVideoEncoderPanic(EEncoderPanicRateControlOptions)); 
	}

void CMMFTestVideoEncodeHwDevice::SetInLayerScalabilityL(TUint aLayer, TUint aNumSteps,
							TInLayerScalabilityType aScalabilityType,
							const TArray<TUint>& aBitrateShare,
							const TArray<TUint>& aPictureShare)
	{
	// check the non-array classes first
	if ((aLayer != KTestLayer) || (aNumSteps != KTestNumSteps) || (aScalabilityType != KTestLayerScaleType) )
		User::Leave(KErrCorrupt);

	TBool successBit = EFalse;
	TBool successPic = EFalse;

	// bitrateShare array
	if (aBitrateShare.Count() == 2)
		{
		if ((aBitrateShare[0] == KTestBitrateShare1) && (aBitrateShare[1] == KTestBitrateShare2))
			{
			successBit = ETrue;
			}
		}

	// pictureShare array
	if (aPictureShare.Count() == 2)
		{
		if ((aPictureShare[0] == KTestPictureShare1) && (aPictureShare[1] == KTestPictureShare2))
			{
			successPic = ETrue;
			}
		}

	if (!successBit || !successPic)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoEncodeHwDevice::SetLayerPromotionPointPeriod(TUint aLayer, TUint aPeriod)
	{
	__ASSERT_ALWAYS((aLayer == KTestLayer) && (aPeriod == KTestPointPeriod), DevVideoEncoderPanic(EEncoderPanicPointPromPeriod)); 
	}

HBufC8* CMMFTestVideoEncodeHwDevice::CodingStandardSpecificSettingsOutputLC()
	{
	HBufC8* chaz = KTestCSSettingsOutput().AllocL();
	CleanupStack::PushL(chaz);

	return chaz;
	}

HBufC8* CMMFTestVideoEncodeHwDevice::ImplementationSpecificSettingsOutputLC()
	{
	HBufC8* dave = KTestISSettingsOutput().AllocL();
	CleanupStack::PushL(dave);

	return dave;
	}

void CMMFTestVideoEncodeHwDevice::SendSupplementalInfoL(const TDesC8& aData)
	{
	if (!(aData == KTestSuppInfo))
		User::Leave(KErrCorrupt);

	iProxy->MdvrpSupplementalInfoSent();	
	}

void CMMFTestVideoEncodeHwDevice::SendSupplementalInfoL(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp)
	{
	if (!(aData == KTestSuppInfo))
		User::Leave(KErrCorrupt);

	TTimeIntervalMicroSeconds suppTime(KTestSuppTime);
	TTimeIntervalMicroSeconds suppTimeCancel(KTestSuppTimeCancel);

	// check time stamp
	if (!(aTimestamp == TTimeIntervalMicroSeconds(suppTime)) && 
		!(aTimestamp == TTimeIntervalMicroSeconds(suppTimeCancel) ))
		User::Leave(KErrCorrupt);

	iSuppTime = ETrue;

	// don't send callback if cancel supp time requested
	if (aTimestamp == suppTime)
		iProxy->MdvrpSupplementalInfoSent();
	}

void CMMFTestVideoEncodeHwDevice::CancelSupplementalInfo()
	{
	if (iSuppTime)
		iProxy->MdvrpSupplementalInfoSent();
	
	iSuppTime = EFalse;
	}

void CMMFTestVideoEncodeHwDevice::GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes)
	{
	aNumFreeBuffers = KTestFreeBuffers;
	aTotalFreeBytes = KTestFreeBytes;
	}

void CMMFTestVideoEncodeHwDevice::ReturnBuffer(TVideoOutputBuffer* /*aBuffer*/)
	{
	switch (iCurrentBuffer)
		{
	case 0:
		// Send two more buffers
		iCurrentBuffer = 1;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[1]));
		iCurrentBuffer = 2;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[2]));
		break;
	case 2:
		// Send three more buffers
		iCurrentBuffer = 3;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[3]));
		iCurrentBuffer = 4;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[4]));
		iCurrentBuffer = 5;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[5]));
		break;
	case 5:
		// Send the rest of the buffers
		iCurrentBuffer = 6;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[6]));
		iCurrentBuffer = 7;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[7]));
		iCurrentBuffer = 8;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[8]));
		iCurrentBuffer = 9;
		iProxy->MdvrpNewBuffer(&(iDataBuffers[9]));
		break;
	default:
		break;
		};
	}

void CMMFTestVideoEncodeHwDevice::PictureLoss()
	{
	}

void CMMFTestVideoEncodeHwDevice::PictureLoss(const TArray<TPictureId>& aPictures)
	{
	TBool success = EFalse;

	if (aPictures.Count() == 3)
		{
		if (ComparePictureIDs(aPictures[0], KTestPictureId1) &&
			ComparePictureIDs(aPictures[1], KTestPictureId2) &&
			ComparePictureIDs(aPictures[2], KTestPictureId3) )
			{
			success = ETrue;
			}
		}

	__ASSERT_ALWAYS(success, DevVideoEncoderPanic(EEncoderPanicPictureLossSpec)); 
	}

void CMMFTestVideoEncodeHwDevice::SliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture)
	{
	__ASSERT_ALWAYS((aFirstMacroblock == KTestFirstMacroblock) && 
					(aNumMacroblocks == KTestNumMacroblocks) && 
					ComparePictureIDs(aPicture, KTestPictureId1), DevVideoEncoderPanic(EEncoderPanicSliceLoss)); 
	}

void CMMFTestVideoEncodeHwDevice::ReferencePictureSelection(const TDesC8& aSelectionData)
	{
	__ASSERT_ALWAYS((aSelectionData == KTestRefPictureInfo), DevVideoEncoderPanic(EEncoderPanicRefPictureInfo));
	}

void CMMFTestVideoEncodeHwDevice::CommitL()
	{
	}

void CMMFTestVideoEncodeHwDevice::Revert()
	{
	}

void CMMFTestVideoEncodeHwDevice::SetProxy(MMMFDevVideoRecordProxy& aProxy)
	{
	ASSERT(iProxy == NULL);
	iProxy = &aProxy;
	}

