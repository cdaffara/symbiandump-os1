// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mdfvideoencoderhwdeviceadapter.h
// 
//

#ifndef MDFVIDEOENCODEHWDEVICEADAPTER_H
#define MDFVIDEOENCODEHWDEVICEADAPTER_H

#include <mmf/devvideo/videorecordhwdevice.h>
#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfpuloader.h>
#include <mdf/codecapivideoresolverutils.h>
#include "mdfvideoencodehwdeviceadapter.hrh"
#include <mmf/devvideo/devvideobase.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/devvideo/devvideohwdeviceadaptersetup.h>
#endif

class CMMFDescriptorBuffer;

/**
Video encoder hardware device. 
This is the DevVideo encoder plugin component talking to a processing unit.
@internalComponent
*/
class CMdfVideoEncodeHwDeviceAdapter :  public CMMFVideoEncodeHwDevice,
										public MMdfInputPortObserver,
										public MMdfOutputPortObserver,
										public MMdfProcessingUnitObserver,
										public MDevVideoHwDeviceAdapterSetup
	{
public:
	static CMdfVideoEncodeHwDeviceAdapter* NewL();
	~CMdfVideoEncodeHwDeviceAdapter();
	// from CMMFVideoHwDevice
    TAny* CustomInterface(TUid aInterface);

	// from CMMFVideoRecordHwDevice
	CPreProcessorInfo* PreProcessorInfoLC();
	void SetInputFormatL(const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize);
	void SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate);
	void SetSourceMemoryL(TReal aMaxPictureRate, TBool aConstantPictureRate, TBool aProcessRealtime);
	void SetClockSource(MMMFClockSource* aClock);
	void SetPreProcessTypesL(TUint32 aPreProcessTypes);
	void SetRgbToYuvOptionsL(TRgbRange aRange, const TYuvFormat& aOutputFormat);
	void SetYuvToYuvOptionsL(const TYuvFormat& aInputFormat, const TYuvFormat& aOutputFormat);
	void SetRotateOptionsL(TRotationType aRotationType);
	void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering);
	void SetInputCropOptionsL(const TRect& aRect);
	void SetOutputCropOptionsL(const TRect& aRect);
	void SetOutputPadOptionsL(const TSize& aOutputSize, const TPoint& aPicturePos);
	void SetColorEnhancementOptionsL(const TColorEnhancementOptions& aOptions);
	void SetFrameStabilisationOptionsL(const TSize& aOutputSize, TBool aFrameStabilisation);
	void SetCustomPreProcessOptionsL(const TDesC8& aOptions);
	void Initialize();
	void WritePictureL(TVideoPicture* aPicture);
	void InputEnd();
	void Start();
	void Stop();
	void Pause();
	void Resume();
	void Freeze();
	void ReleaseFreeze();
	TTimeIntervalMicroSeconds RecordingPosition();
	void GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& aCounters);
	void GetFrameStabilisationOutput(TRect& aRect);
	TUint NumComplexityLevels();
	void SetComplexityLevel(TUint aLevel);
	void CommitL();
	void Revert();

	// from CMMFVideoEncodeHwDevice
	CVideoEncoderInfo* VideoEncoderInfoLC();
	void SetOutputFormatL(const CCompressedVideoFormat& aFormat, 
								  TVideoDataUnitType aDataUnitType, 
								  TVideoDataUnitEncapsulation aDataEncapsulation,
								  TBool aSegmentationAllowed=EFalse);
	void SetOutputRectL(const TRect& aRect);
	void SetInputDevice(CMMFVideoPreProcHwDevice* aDevice);
	void SetErrorsExpected(TBool aBitErrors, TBool aPacketLosses);
	void SetMinRandomAccessRate(TReal aRate);
	void SetNumBitrateLayersL(TUint aNumLayers);
	void SetScalabilityLayerTypeL(TUint aLayer, TScalabilityType aScalabilityType);
	void SetGlobalReferenceOptions(TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay);
	void SetLayerReferenceOptions(TUint aLayer, TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay);
	void SetBufferOptionsL(const TEncoderBufferOptions& aOptions);
	void SetCodingStandardSpecificOptionsL(const TDesC8& aOptions);
	void SetImplementationSpecificEncoderOptionsL(const TDesC8& aOptions);
	HBufC8* CodingStandardSpecificInitOutputLC();
	HBufC8* ImplementationSpecificInitOutputLC();
	void SetErrorProtectionLevelsL(TUint aNumLevels, TBool aSeparateBuffers);
	void SetErrorProtectionLevelL(TUint aLevel, TUint aBitrate, TUint aStrength);
	void SetChannelPacketLossRate(TUint aLevel, 
									  TReal aLossRate, 
									  TTimeIntervalMicroSeconds32 aLossBurstLength);
	void SetChannelBitErrorRate(TUint aLevel, TReal aErrorRate, TReal aStdDeviation);
	void SetSegmentTargetSize(TUint aLayer, TUint aSizeBytes, TUint aSizeMacroblocks);
	void SetRateControlOptions(TUint aLayer, const TRateControlOptions& aOptions);
	void SetInLayerScalabilityL(TUint aLayer, 
									TUint aNumSteps,
									TInLayerScalabilityType aScalabilityType,
									const TArray<TUint>& aBitrateShare,
									const TArray<TUint>& aPictureShare);
	void SetLayerPromotionPointPeriod(TUint aLayer, TUint aPeriod);
	HBufC8* CodingStandardSpecificSettingsOutputLC();
	HBufC8* ImplementationSpecificSettingsOutputLC();
	void SendSupplementalInfoL(const TDesC8& aData);
	void SendSupplementalInfoL(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp);
	void CancelSupplementalInfo();
	void GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes);
	void ReturnBuffer(TVideoOutputBuffer* aBuffer);
	void PictureLoss();
	void PictureLoss(const TArray<TPictureId>& aPictures);
	void SliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	void ReferencePictureSelection(const TDesC8& aSelectionData);
	void SetProxy(MMMFDevVideoRecordProxy& aProxy);

	// from MMdfInputPortObserver
	void MipoWriteDataComplete(const MMdfInputPort* aInputPort,CMMFBuffer* aBuffer, TInt aErrorCode);
	void MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort, TInt aErrorCode);
	void MipoRestartTunnelComplete(const MMdfInputPort* aInputPort, TInt aErrorCode);

	// from MMdfOutputPortObserver
	void MopoReadDataComplete(const MMdfOutputPort* aOutputPort, CMMFBuffer* aBuffer, TInt aErrorCode);
	void MopoDisconnectTunnelComplete(const MMdfOutputPort* aOutputPort, TInt aErrorCode);
	void MopoRestartTunnelComplete(const MMdfOutputPort* aOutputPort, TInt aErrorCode);	

	// from MMdfProcessingUnitObserver
	void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	
	// from MDevVideoHwDeviceAdapterSetup
	void LoadProcessingUnitL(const CImplementationInformation& aImplInfo);
private:
	CMdfVideoEncodeHwDeviceAdapter();
	void InitializeL();
	void ConstructL();

private:
	// call back class for the DevVideo
	MMMFDevVideoRecordProxy* iProxy;
	
	// the encoder itself
	CMdfProcessingUnit* iEncoderPU;

	// the processing unit loader	
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;

	// encoder port arrays
	RPointerArray<MMdfInputPort>  iEncoderPUInputPortsArray;
	RPointerArray<MMdfOutputPort> iEncoderPUOutputPortsArray;

	// encoder settings
	TUncompressedVideoFormat iFormat;
	TSize iPictureSize;
	TReal32 iFrameRate;
	CMMFDevVideoRecord::TPictureCounters iPictureCounters;

	// video format arrays (for VideoEncoderInfoLC())
	RArray<CCompressedVideoFormat*> iOutputVideoFormats;
	RArray<TUncompressedVideoFormat> iInputVideoFormats;
	RArray<TPictureRateAndSize> iPictureRates;

	// output data buffers
	RArray<TVideoOutputBuffer> iDataBuffers;
	
	TBool iInputStreamEnd;
	
	// buffers used to communicate to the encoder PU
	CMMFDescriptorBuffer* iInputBuffer;
	CMMFDescriptorBuffer* iOutputBuffer;
	
	TVideoPicture* iCurrentPicture;	
		
	// Uid of PU to load
	TUid iPuUid;

	CCodecApiVideoOpaqueData* iPuData;	
	HBufC* iManufacturer;
	};

#endif // MDFVIDEOENCODEHWDEVICEADAPTER_H
