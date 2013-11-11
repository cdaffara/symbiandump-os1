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


#ifndef CIUENCODER_H
#define CIUENCODER_H

#include <mmf/devvideo/videorecordhwdevice.h>
#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include "ciudevvideotestinterface.h"
#include "testciupluginuids.hrh"

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

const TUid KUidDevVideoCiuTestEncodeHwDevice = {KUidDevVideoCiuTestEncodeHwDeviceDefine1}; //This is the only real plugin
const TUid KUidDevVideoCiuTestEncodeHwDevice1 = {KUidDevVideoCiuTestEncodeHwDeviceDefine1};


class CMMFTestCiuVideoEncodeHwDevice : public CMMFVideoEncodeHwDevice,
									   public MMmfVideoBufferManagement,
									   public MDevVideoCiuTestInterfaceBufferManagement,
                                       public MMmfVideoH324AnnexKMode,
									   public MDevVideoCiuTestInterfaceH324AnnexKMode

	{
public:
	static CMMFVideoEncodeHwDevice* NewL(TAny* aInitParams);
	~CMMFTestCiuVideoEncodeHwDevice();

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

	// from MMmfVideoBufferManagement
	virtual void MmvbmSetObserver(MMmfVideoBufferManagementObserver* aObserver);
	virtual void MmvbmEnable(TBool aEnable);
	virtual void MmvbmSetBufferOptionsL(const MMmfVideoBufferManagement::TBufferOptions& aOptions);
	virtual void MmvbmGetBufferOptions(MMmfVideoBufferManagement::TBufferOptions& aOptions);
	virtual TVideoPicture* MmvbmGetBufferL(const TSize& aSize);
	virtual void MmvbmReleaseBuffer(TVideoPicture* aBuffer);	
	
	// from MDevVideoCiuTestInterfaceBufferManagement
	virtual MMmfVideoBufferManagementObserver* MdvbmObserver();
	virtual TBool MdvbmIsEnabled();
	virtual void MdvbmSetBuffer(TSize& aSize, TVideoPicture& aBuffer);
	virtual void MdvbmRequestNewBuffersCallback();
	virtual void MdvbmRequestReleaseBuffersCallback();

	// from MMmfVideoH324AnnexKMode
	virtual void MmvhakmSetH324MAnnexKModeL(TBool aAnnexKMode);

	// from MDevVideoCiuTestInterfaceH324AnnexKMode
	virtual TBool MmvhakmH324MAnnexKModeL();
	
private:
	CMMFTestCiuVideoEncodeHwDevice();
private:
	MMMFDevVideoRecordProxy* iProxy;
	
	MMmfVideoBufferManagementObserver* iObserver;
	TBool iEnable;
	MMmfVideoBufferManagement::TBufferOptions iBufferOptions;
	TVideoPicture* iVideoPictureBuffer;
	TSize iBufferSize;
	TBool iAnnexKMode;
	};

#endif
