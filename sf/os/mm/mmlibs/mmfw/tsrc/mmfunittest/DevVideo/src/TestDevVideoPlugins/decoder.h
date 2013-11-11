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


#ifndef __DECODER_H__
#define __DECODER_H__

#include <mmf/devvideo/videoplayhwdevice.h>
#include "TestPluginUIDs.hrh"

enum TDecoderPanicCategory
	{
	EDecoderPanicInitialize = 0,
	EDecoderPanicClockSource,
	EDecoderPanicSynchronizeDecoding,
	EDecoderPanicHrdVbvSpec,
	EDecoderPanicHrdVbvParams,
	EDecoderPanicScreenClipRegion,
	EDecoderPanicPauseClipFail,
	EDecoderPanicSetPosition,
	EDecoderPanicFreezePicture,
	EDecoderPanicReleaseFreeze,
	EDecoderPanicComplexityLevel,
	EDecoderPanicComplexityLevelInfo,
	EDecoderPanicPictureHeader,
	EDecoderPanicPictureHeaderOptions,
	EDecoderPanicPictureHeaderTimestamp,
	EDecoderPanicTimedSnapshot,
	EDecoderPanicTimedSnapshotId,
	EDecoderPanicCancelTimedSnapshot,
	EDecoderPanicSupportedSnapshotFormats
	};

const TUid KUidDevVideoTestDecodeHwDevice = {KUidDevVideoTestDecodeHwDeviceDefine1}; //This is the only real plugin
const TUid KUidDevVideoTestDecodeHwDevice1 = {KUidDevVideoTestDecodeHwDeviceDefine1};
const TUid KUidDevVideoTestDecodeHwDevice2 = {KUidDevVideoTestDecodeHwDeviceDefine2};
const TUid KUidDevVideoTestDecodeHwDevice3 = {KUidDevVideoTestDecodeHwDeviceDefine3};
const TUid KUidDevVideoTestDecodeHwDevice4 = {KUidDevVideoTestDecodeHwDeviceDefine4};
const TUid KUidDevVideoTestDecodeHwDevice5 = {KUidDevVideoTestDecodeHwDeviceDefine5};
const TInt KDecoderDefaultScreenNumber = 0;
const TInt KDecoderSecondaryScreenNumber = 1;

class CMMFTestVideoDecodeHwDevice : public CMMFVideoDecodeHwDevice, public MMMFVideoPlayHwDeviceExtensionScanCopy
	{
public:
	static CMMFVideoDecodeHwDevice* NewL(TAny* aInitParams);
	~CMMFTestVideoDecodeHwDevice();

	// from CMMFVideoHwDevice
    TAny* CustomInterface(TUid aInterface);

	// from CMMFVideoPlayHwDevice
	CPostProcessorInfo* PostProcessorInfoLC();
	void GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats);
	void SetOutputFormatL(const TUncompressedVideoFormat &aFormat);
	void SetPostProcessTypesL(TUint32 aPostProcCombination);
	void SetInputCropOptionsL(const TRect& aRect);
	void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat);
	void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions);
	void SetRotateOptionsL(TRotationType aRotationType);
	void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering);
	void SetOutputCropOptionsL(const TRect& aRect);
	void SetPostProcSpecificOptionsL(const TDesC8& aOptions);
	void SetClockSource(MMMFClockSource* aClock);
	void SetVideoDestScreenL(TBool aScreen);
	void Initialize();
	void StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion);
	void SetScreenClipRegion(const TRegion& aRegion);
	void SetPauseOnClipFail(TBool aPause);
	void AbortDirectScreenAccess();
	TBool IsPlaying();
	void Redraw();
	void Start();
	void Stop();
	void Pause();
	void Resume();
	void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition);
	void FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp);
	void ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp);
	TTimeIntervalMicroSeconds PlaybackPosition();
	TUint PictureBufferBytes();
	void GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters);
	void SetComplexityLevel(TUint aLevel);
	TUint NumComplexityLevels();
	void GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo);
	void ReturnPicture(TVideoPicture* aPicture);
	TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);
	void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TTimeIntervalMicroSeconds& aPresentationTimestamp);
	void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TPictureId& aPictureId);
	void CancelTimedSnapshot();
	void GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats);
	void InputEnd();
	void CommitL();
	void Revert();

	// from CMMFVideoDecodeHwDevice
    CVideoDecoderInfo* VideoDecoderInfoLC();
	TVideoPictureHeader* GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TVideoInputBuffer* aDataUnit);
    void ReturnHeader(TVideoPictureHeader* aHeader);
    void SetInputFormatL(const CCompressedVideoFormat& aFormat, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TBool aDataInOrder);
    void SynchronizeDecoding(TBool aSynchronize);
    void SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions);
    void GetBufferOptions(CMMFDevVideoPlay::TBufferOptions& aOptions);
    void SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams);
    void SetOutputDevice(CMMFVideoPostProcHwDevice* aDevice);
    TTimeIntervalMicroSeconds DecodingPosition();
    TUint PreDecoderBufferBytes();
    void GetBitstreamCounters(CMMFDevVideoPlay::TBitstreamCounters& aCounters);
    TUint NumFreeBuffers();
    TVideoInputBuffer* GetBufferL(TUint aBufferSize);
    void WriteCodedDataL(TVideoInputBuffer* aBuffer);
	void SetProxy(MMMFDevVideoPlayProxy& aProxy);
	void ConfigureDecoderL(const TVideoPictureHeader& aVideoPictureHeader);

//from MMMFVideoPlayHwDeviceExtensionScanCopy
	void WriteCodedDataL(TVideoInputBuffer* aBuffer, TFramePortion aPortion = EFramePortionUnknown);
	void ScanAndCopyCodedDataL(TPtr8 aCodedData, TVideoInputBuffer* aBuffer, TInt& aConsumed, TFramePortion aPortion = EFramePortionUnknown);

private:
	CMMFTestVideoDecodeHwDevice();
private:
	MMMFDevVideoPlayProxy* iProxy;

	RArray<CCompressedVideoFormat*> iVidFormats;
	RArray<TPictureRateAndSize> iPictureRates;
	// For post-proc info 
	RArray<TScaleFactor> iScaleFactors;
	RArray<TUncompressedVideoFormat> iPostProcVidFormats;
	RArray<TUint32> iCombinations;
	
	TVideoPictureHeader iPictureHeader;
	TVideoInputBuffer iInputBuffer;
	TBool iIsPlaying;
	TUid iExtensionUid;

	HBufC8* iBufferDataArea;
	};

#endif
