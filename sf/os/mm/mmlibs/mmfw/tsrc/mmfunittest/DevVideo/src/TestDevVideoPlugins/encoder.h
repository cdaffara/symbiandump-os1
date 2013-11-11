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


#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <mmf/devvideo/videorecordhwdevice.h>
#include "TestPluginUIDs.hrh"

enum TEncoderPanicCategory
	{
	EEncoderPanicInitialize = 0,
	EEncoderPanicClockSource,
	EEncoderPanicSynchronizeDecoding,
	EEncoderPanicHrdVbvSpec,
	EEncoderPanicHrdVbvParams,
	EEncoderPanicScreenClipRegion,
	EEncoderPanicPauseClipFail,
	EEncoderPanicSetPosition,
	EEncoderPanicFreezePicture,
	EEncoderPanicReleaseFreeze,
	EEncoderPanicComplexityLevel,
	EEncoderPanicComplexityLevelInfo,
	EEncoderPanicPictureHeader,
	EEncoderPanicPictureHeaderOptions,
	EEncoderPanicPictureHeaderTimestamp,
	EEncoderPanicTimedSnapshot,
	EEncoderPanicTimedSnapshotId,
	EEncoderPanicCancelTimedSnapshot,
	EEncoderPanicSupportedSnapshotFormats,
	EEncoderPanicSourceCam,
	EEncoderPanicSourceMem,
	EEncoderPanicErrorsExpected,
	EEncoderPanicRandomAccessRate,
	EEncoderPanicNumBitrateLayers,
	EEncoderPanicPacketLossRate,
	EEncoderPanicBitErrorRate,
	EEncoderPanicSegmentTargetSize,
	EEncoderPanicRateControlOptions,
	EEncoderPanicPointPromPeriod,
	EEncoderPanicRecordPosition,
	EEncoderPanicPictureLossSpec,
	EEncoderPanicSliceLoss,
	EEncoderPanicRefPictureInfo
	};

const TUid KUidDevVideoTestEncodeHwDevice = {KUidDevVideoTestEncodeHwDeviceDefine1}; //This is the only real plugin
const TUid KUidDevVideoTestEncodeHwDevice1 = {KUidDevVideoTestEncodeHwDeviceDefine1};
const TUid KUidDevVideoTestEncodeHwDevice2 = {KUidDevVideoTestEncodeHwDeviceDefine2};
const TUid KUidDevVideoTestEncodeHwDevice3 = {KUidDevVideoTestEncodeHwDeviceDefine3};
const TUid KUidDevVideoTestEncodeHwDevice4 = {KUidDevVideoTestEncodeHwDeviceDefine4};
const TUid KUidDevVideoTestEncodeHwDevice5 = {KUidDevVideoTestEncodeHwDeviceDefine5};

class CMMFTestVideoEncodeHwDevice : public CMMFVideoEncodeHwDevice
	{
public:
	static CMMFVideoEncodeHwDevice* NewL(TAny* aInitParams);
	~CMMFTestVideoEncodeHwDevice();

	// from CMMFVideoHwDevice
    virtual TAny* CustomInterface(TUid aInterface);

	// from CMMFVideoRecordHwDevice
	virtual CPreProcessorInfo* PreProcessorInfoLC();
	virtual void SetInputFormatL(const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize);
	virtual void SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate);
	virtual void SetSourceMemoryL(TReal aMaxPictureRate, TBool aConstantPictureRate, TBool aProcessRealtime);
	virtual void SetClockSource(MMMFClockSource* aClock);
	virtual void SetPreProcessTypesL(TUint32 aPreProcessTypes);
	virtual void SetRgbToYuvOptionsL(TRgbRange aRange, const TYuvFormat& aOutputFormat);
	virtual void SetYuvToYuvOptionsL(const TYuvFormat& aInputFormat, const TYuvFormat& aOutputFormat);
	virtual void SetRotateOptionsL(TRotationType aRotationType);
	virtual void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering);
	virtual void SetInputCropOptionsL(const TRect& aRect);
	virtual void SetOutputCropOptionsL(const TRect& aRect);
	virtual void SetOutputPadOptionsL(const TSize& aOutputSize, const TPoint& aPicturePos);
	virtual void SetColorEnhancementOptionsL(const TColorEnhancementOptions& aOptions);
	virtual void SetFrameStabilisationOptionsL(const TSize& aOutputSize, TBool aFrameStabilisation);
	virtual void SetCustomPreProcessOptionsL(const TDesC8& aOptions);
	virtual void Initialize();
	virtual void WritePictureL(TVideoPicture* aPicture);
	virtual void InputEnd();
	virtual void Start();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
	virtual void Freeze();
	virtual void ReleaseFreeze();
	virtual TTimeIntervalMicroSeconds RecordingPosition();
	virtual void GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& aCounters);
	virtual void GetFrameStabilisationOutput(TRect& aRect);
	virtual TUint NumComplexityLevels();
	virtual void SetComplexityLevel(TUint aLevel);
	virtual void CommitL();
	virtual void Revert();

	// from CMMFVideoEncodeHwDevice
	virtual CVideoEncoderInfo* VideoEncoderInfoLC();
	virtual void SetOutputFormatL(const CCompressedVideoFormat& aFormat, 
								  TVideoDataUnitType aDataUnitType, 
								  TVideoDataUnitEncapsulation aDataEncapsulation,
								  TBool aSegmentationAllowed=EFalse);
	virtual void SetOutputRectL(const TRect& aRect);
	virtual void SetInputDevice(CMMFVideoPreProcHwDevice* aDevice);
	virtual void SetErrorsExpected(TBool aBitErrors, TBool aPacketLosses);
	virtual void SetMinRandomAccessRate(TReal aRate);
	virtual void SetNumBitrateLayersL(TUint aNumLayers);
	virtual void SetScalabilityLayerTypeL(TUint aLayer, TScalabilityType aScalabilityType);
	virtual void SetGlobalReferenceOptions(TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay);
	virtual void SetLayerReferenceOptions(TUint aLayer, TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay);
	virtual void SetBufferOptionsL(const TEncoderBufferOptions& aOptions);
	virtual void SetCodingStandardSpecificOptionsL(const TDesC8& aOptions);
	virtual void SetImplementationSpecificEncoderOptionsL(const TDesC8& aOptions);
	virtual HBufC8* CodingStandardSpecificInitOutputLC();
	virtual HBufC8* ImplementationSpecificInitOutputLC();
	virtual void SetErrorProtectionLevelsL(TUint aNumLevels, TBool aSeparateBuffers);
	virtual void SetErrorProtectionLevelL(TUint aLevel, TUint aBitrate, TUint aStrength);
	virtual void SetChannelPacketLossRate(TUint aLevel, 
										  TReal aLossRate, 
										  TTimeIntervalMicroSeconds32 aLossBurstLength);
	virtual void SetChannelBitErrorRate(TUint aLevel, TReal aErrorRate, TReal aStdDeviation);
	virtual void SetSegmentTargetSize(TUint aLayer, TUint aSizeBytes, TUint aSizeMacroblocks);
	virtual void SetRateControlOptions(TUint aLayer, const TRateControlOptions& aOptions);
	virtual void SetInLayerScalabilityL(TUint aLayer, 
										TUint aNumSteps,
										TInLayerScalabilityType aScalabilityType,
										const TArray<TUint>& aBitrateShare,
										const TArray<TUint>& aPictureShare);
	virtual void SetLayerPromotionPointPeriod(TUint aLayer, TUint aPeriod);
	virtual HBufC8* CodingStandardSpecificSettingsOutputLC();
	virtual HBufC8* ImplementationSpecificSettingsOutputLC();
	virtual void SendSupplementalInfoL(const TDesC8& aData);
	virtual void SendSupplementalInfoL(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp);
	virtual void CancelSupplementalInfo();
	virtual void GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes);
	virtual void ReturnBuffer(TVideoOutputBuffer* aBuffer);
	virtual void PictureLoss();
	virtual void PictureLoss(const TArray<TPictureId>& aPictures);
	virtual void SliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	virtual void ReferencePictureSelection(const TDesC8& aSelectionData);
	virtual void SetProxy(MMMFDevVideoRecordProxy& aProxy);

private:
	CMMFTestVideoEncodeHwDevice();
private:
	MMMFDevVideoRecordProxy* iProxy;
	TBool iIsRecording;
	TBool iSuppTime;
	TTimeIntervalMicroSeconds iRecPosition;

	RArray<TVideoOutputBuffer> iDataBuffers;
	TInt iCurrentBuffer;

	RArray<CCompressedVideoFormat*> iOutputVidFormats;
	RArray<TUncompressedVideoFormat> iInputVidFormats;
	RArray<TPictureRateAndSize> iPictureRates;
	};

#endif
