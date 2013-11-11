// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ciuencoder.h"

_LIT(KDevVideoEncoderPanicCategory, "DevVideoEncoder");
void DevVideoEncoderPanic(TInt aReason)
	{
	User::Panic(KDevVideoEncoderPanicCategory, aReason);
	}

CMMFVideoEncodeHwDevice* CMMFTestCiuVideoEncodeHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestCiuVideoEncodeHwDevice* s = new(ELeave) CMMFTestCiuVideoEncodeHwDevice;
	return static_cast<CMMFVideoEncodeHwDevice*>(s);
	}

CMMFTestCiuVideoEncodeHwDevice::CMMFTestCiuVideoEncodeHwDevice()
// The following will be set on construction to their default values
// iAnnexKMode (EFalse)
	{
	}

CMMFTestCiuVideoEncodeHwDevice::~CMMFTestCiuVideoEncodeHwDevice()
	{
	}

TAny* CMMFTestCiuVideoEncodeHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidMmfVideoBufferManagement)
		{
		MMmfVideoBufferManagement* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceBufferManagement)
		{
		MDevVideoCiuTestInterfaceBufferManagement* interface = this;
		return interface;
		}
	else if (aInterface == KUidMmfVideoH324AnnexKMode)
		{
		MMmfVideoH324AnnexKMode* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceH324AnnexKMode)
		{
		MDevVideoCiuTestInterfaceH324AnnexKMode* interface = this;
		return interface;
		}
	else
		{
		return NULL;
		}
	}

CPreProcessorInfo* CMMFTestCiuVideoEncodeHwDevice::PreProcessorInfoLC()
	{
	return NULL;
	}

void CMMFTestCiuVideoEncodeHwDevice::SetInputFormatL(const TUncompressedVideoFormat& /*aFormat*/, const TSize& /*aPictureSize*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetSourceCameraL(TInt /*aCameraHandle*/, TReal /*aPictureRate*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetSourceMemoryL(TReal /*aMaxPictureRate*/, TBool /*aConstantPictureRate*/, TBool /*aProcessRealtime*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetClockSource(MMMFClockSource* /*aClock*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetPreProcessTypesL(TUint32 /*aPreProcessTypes*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetRgbToYuvOptionsL(TRgbRange /*aRange*/, const TYuvFormat& /*aOutputFormat*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetYuvToYuvOptionsL(const TYuvFormat& /*aInputFormat*/, const TYuvFormat& /*aOutputFormat*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetRotateOptionsL(TRotationType /*aRotationType*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetScaleOptionsL(const TSize& /*aTargetSize*/, TBool /*aAntiAliasFiltering*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetInputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetOutputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetOutputPadOptionsL(const TSize& /*aOutputSize*/, const TPoint& /*aPicturePos*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetColorEnhancementOptionsL(const TColorEnhancementOptions& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetFrameStabilisationOptionsL(const TSize& /*aOutputSize*/, TBool /*aFrameStabilisation*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetCustomPreProcessOptionsL(const TDesC8& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Initialize()
	{
	iProxy->MdvrpInitializeComplete(this, KErrNone);
	}

void CMMFTestCiuVideoEncodeHwDevice::WritePictureL(TVideoPicture* /*aPicture*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::InputEnd()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Start()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Stop()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Pause()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Resume()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Freeze()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::ReleaseFreeze()
	{
	}

TTimeIntervalMicroSeconds CMMFTestCiuVideoEncodeHwDevice::RecordingPosition()
	{
	return 0;
	}

void CMMFTestCiuVideoEncodeHwDevice::GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& /*aCounters*/)
	{
	}
	
void CMMFTestCiuVideoEncodeHwDevice::GetFrameStabilisationOutput(TRect& /*aRect*/)
	{
	}

TUint CMMFTestCiuVideoEncodeHwDevice::NumComplexityLevels()
	{
	return 0;
	}

void CMMFTestCiuVideoEncodeHwDevice::SetComplexityLevel(TUint /*aLevel*/)
	{
	}

CVideoEncoderInfo* CMMFTestCiuVideoEncodeHwDevice::VideoEncoderInfoLC()
	{
	return NULL;
	}

void CMMFTestCiuVideoEncodeHwDevice::SetOutputFormatL(const CCompressedVideoFormat& /*aFormat*/,
													  TVideoDataUnitType /*aDataUnitType*/,
													  TVideoDataUnitEncapsulation /*aDataEncapsulation*/,
													  TBool /*aSegmentationAllowed*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetOutputRectL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetInputDevice(CMMFVideoPreProcHwDevice* /*aDevice*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetErrorsExpected(TBool /*aBitErrors*/, TBool /*aPacketLosses*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetMinRandomAccessRate(TReal /*aRate*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetNumBitrateLayersL(TUint /*aNumLayers*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetScalabilityLayerTypeL(TUint /*aLayer*/, TScalabilityType /*aScalabilityType*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetGlobalReferenceOptions(TUint /*aMaxReferencePictures*/, TUint /*aMaxPictureOrderDelay*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetLayerReferenceOptions(TUint /*aLayer*/, TUint /*aMaxReferencePictures*/, TUint /*aMaxPictureOrderDelay*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetBufferOptionsL(const TEncoderBufferOptions& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetCodingStandardSpecificOptionsL(const TDesC8& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetImplementationSpecificEncoderOptionsL(const TDesC8& /*aOptions*/)
	{
	}

HBufC8* CMMFTestCiuVideoEncodeHwDevice::CodingStandardSpecificInitOutputLC()
	{
	return NULL;
	}

HBufC8* CMMFTestCiuVideoEncodeHwDevice::ImplementationSpecificInitOutputLC()
	{
	return NULL;
	}

void CMMFTestCiuVideoEncodeHwDevice::SetErrorProtectionLevelsL(TUint /*aNumLevels*/, TBool /*aSeparateBuffers*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetErrorProtectionLevelL(TUint /*aLevel*/, TUint /*aBitrate*/, TUint /*aStrength*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetChannelPacketLossRate(TUint /*aLevel*/,
															TReal /*aLossRate*/,
															TTimeIntervalMicroSeconds32 /*aLossBurstLength*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetChannelBitErrorRate(TUint /*aLevel*/, TReal /*aErrorRate*/, TReal /*aStdDeviation*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetSegmentTargetSize(TUint /*aLayer*/, TUint /*aSizeBytes*/, TUint /*aSizeMacroblocks*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetRateControlOptions(TUint /*aLayer*/, const TRateControlOptions& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetInLayerScalabilityL(TUint /*aLayer*/, TUint /*aNumSteps*/,
							TInLayerScalabilityType /*aScalabilityType*/,
							const TArray<TUint>& /*aBitrateShare*/,
							const TArray<TUint>& /*aPictureShare*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetLayerPromotionPointPeriod(TUint /*aLayer*/, TUint /*aPeriod*/)
	{
	}

HBufC8* CMMFTestCiuVideoEncodeHwDevice::CodingStandardSpecificSettingsOutputLC()
	{
	return NULL;
	}

HBufC8* CMMFTestCiuVideoEncodeHwDevice::ImplementationSpecificSettingsOutputLC()
	{
	return NULL;
	}

void CMMFTestCiuVideoEncodeHwDevice::SendSupplementalInfoL(const TDesC8& /*aData*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SendSupplementalInfoL(const TDesC8& /*aData*/, const TTimeIntervalMicroSeconds& /*aTimestamp*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::CancelSupplementalInfo()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::GetOutputBufferStatus(TUint& /*aNumFreeBuffers*/, TUint& /*aTotalFreeBytes*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::ReturnBuffer(TVideoOutputBuffer* /*aBuffer*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::PictureLoss()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::PictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::ReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::CommitL()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::Revert()
	{
	}

void CMMFTestCiuVideoEncodeHwDevice::SetProxy(MMMFDevVideoRecordProxy& aProxy)
	{
	ASSERT(iProxy == NULL);
	iProxy = &aProxy;
	}

// MMmfVideoBufferManagement implementation
void CMMFTestCiuVideoEncodeHwDevice::MmvbmSetObserver(MMmfVideoBufferManagementObserver* aObserver)
	{
	iObserver = aObserver;
	}

void CMMFTestCiuVideoEncodeHwDevice::MmvbmEnable(TBool aEnable)
	{
	iEnable = aEnable;
	}

void CMMFTestCiuVideoEncodeHwDevice::MmvbmSetBufferOptionsL(const MMmfVideoBufferManagement::TBufferOptions& aOptions)
	{
	iBufferOptions = aOptions;
	}

void CMMFTestCiuVideoEncodeHwDevice::MmvbmGetBufferOptions(MMmfVideoBufferManagement::TBufferOptions& aOptions)
	{
	aOptions = iBufferOptions;
	}

TVideoPicture* CMMFTestCiuVideoEncodeHwDevice::MmvbmGetBufferL(const TSize& aSize)
	{
	iBufferSize = aSize;
	return iVideoPictureBuffer;
	}

void CMMFTestCiuVideoEncodeHwDevice::MmvbmReleaseBuffer(TVideoPicture* aBuffer)
	{
	iVideoPictureBuffer = aBuffer;
	}

// MDevVideoCiuTestInterfaceBufferManagement test interface implementation
MMmfVideoBufferManagementObserver* CMMFTestCiuVideoEncodeHwDevice::MdvbmObserver()
	{
	return iObserver;
	}
	
TBool CMMFTestCiuVideoEncodeHwDevice::MdvbmIsEnabled()
	{
	return iEnable;
	}

void CMMFTestCiuVideoEncodeHwDevice::MdvbmSetBuffer(TSize& aSize, TVideoPicture& aBuffer)
	{
	iBufferSize = aSize;
	iVideoPictureBuffer = &aBuffer;
	}
	
void CMMFTestCiuVideoEncodeHwDevice::MdvbmRequestNewBuffersCallback()
	{
	iObserver->MmvbmoNewBuffers();
	}
	
void CMMFTestCiuVideoEncodeHwDevice::MdvbmRequestReleaseBuffersCallback()
	{
	iObserver->MmvbmoReleaseBuffers();
	}

void CMMFTestCiuVideoEncodeHwDevice::MmvhakmSetH324MAnnexKModeL(TBool aAnnexKMode)
	{
	iAnnexKMode = aAnnexKMode;
	}

TBool CMMFTestCiuVideoEncodeHwDevice::MmvhakmH324MAnnexKModeL()
	{
	return iAnnexKMode;
    }
