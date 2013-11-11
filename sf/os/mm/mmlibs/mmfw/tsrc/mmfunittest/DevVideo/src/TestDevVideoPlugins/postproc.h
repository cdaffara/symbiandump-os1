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


#ifndef __POSTPROC_H__
#define __POSTPROC_H__


#include <mmf/devvideo/videoplayhwdevice.h>
#include "TestPluginUIDs.hrh"

enum TPostProcPanicCategory
	{
	EPostProcPanicInitialize = 0,
	EPostProcPanicClockSource,
	EPostProcPanicScreenClipRegion,
	EPostProcPanicPauseClipFail,
	EPostProcPanicSetPosition,
	EPostProcPanicFreezePicture,
	EPostProcPanicReleaseFreeze,
	EPostProcPanicComplexityLevel,
	EPostProcPanicComplexityLevelInfo
	};

const TUid KUidDevVideoTestPostProcHwDevice = {KUidDevVideoTestPostProcHwDeviceDefine1};
const TUid KUidDevVideoTestPostProcHwDevice1 = {KUidDevVideoTestPostProcHwDeviceDefine1};
const TUid KUidDevVideoTestPostProcHwDevice2 = {KUidDevVideoTestPostProcHwDeviceDefine2};
const TUid KUidDevVideoTestPostProcHwDevice3 = {KUidDevVideoTestPostProcHwDeviceDefine3};
const TUid KUidDevVideoTestPostProcHwDevice4 = {KUidDevVideoTestPostProcHwDeviceDefine4};
const TUid KUidDevVideoTestPostProcHwDevice5 = {KUidDevVideoTestPostProcHwDeviceDefine5};
const TInt KPostProcDefaultScreenNumber = 0;
const TInt KPostProcSecondaryScreenNumber = 1;

class CMMFTestVideoPostProcHwDevice : public CMMFVideoPostProcHwDevice
	{
public:
	static CMMFVideoPostProcHwDevice* NewL(TAny* aInitParams);
	~CMMFTestVideoPostProcHwDevice();

	// from CMMFVideoHwDevice
    virtual TAny* CustomInterface(TUid aInterface);

	// from CMMFVideoPlayHwDevice
	virtual CPostProcessorInfo* PostProcessorInfoLC();
	virtual void GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats);
	virtual void SetOutputFormatL(const TUncompressedVideoFormat &aFormat);
	virtual void SetPostProcessTypesL(TUint32 aPostProcCombination);
	virtual void SetInputCropOptionsL(const TRect& aRect);
	virtual void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat);
	virtual void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions);
	virtual void SetRotateOptionsL(TRotationType aRotationType);
	virtual void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering);
	virtual void SetOutputCropOptionsL(const TRect& aRect);
	virtual void SetPostProcSpecificOptionsL(const TDesC8& aOptions);
	virtual void SetClockSource(MMMFClockSource* aClock);
	virtual void SetVideoDestScreenL(TBool aScreen);
	virtual void Initialize();
	virtual void StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion);
	virtual void SetScreenClipRegion(const TRegion& aRegion);
	virtual void SetPauseOnClipFail(TBool aPause);
	virtual void AbortDirectScreenAccess();
	virtual TBool IsPlaying();
	virtual void Redraw();
	virtual void Start();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
	virtual void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition);
	virtual void FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp);
	virtual void ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp);
	virtual TTimeIntervalMicroSeconds PlaybackPosition();
	virtual TUint PictureBufferBytes();
	virtual void GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters);
	virtual void SetComplexityLevel(TUint aLevel);
	virtual TUint NumComplexityLevels();
	virtual void GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo);
	virtual void ReturnPicture(TVideoPicture* aPicture);
	virtual TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);
	virtual void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TTimeIntervalMicroSeconds& aPresentationTimestamp);
	virtual void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TPictureId& aPictureId);
	virtual void CancelTimedSnapshot();
	virtual void GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats);
	virtual void InputEnd();
	virtual void CommitL();
	virtual void Revert();

	// from CMMFVideoPostProcHwDevice
    virtual void SetInputFormatL(const TUncompressedVideoFormat& aFormat);
    virtual void SetInputDevice(CMMFVideoDecodeHwDevice* aDevice);
    virtual void WritePictureL(TVideoPicture* aPicture);
	virtual void SetProxy(MMMFDevVideoPlayProxy& aProxy);

private:
	CMMFTestVideoPostProcHwDevice();
private:
	MMMFDevVideoPlayProxy* iProxy;

	RArray<TUncompressedVideoFormat> iVidFormats;
	RArray<TScaleFactor> iScaleFactors;
	RArray<TUint32> iCombinations;
	
	RArray<TVideoPicture> iVideoPictures;
	TInt iCurrentPicture;
	TBool iIsPlaying;
	TPictureId iPictureId;
	TPictureData iPictureData;
	TTimeIntervalMicroSeconds iPictureTimestamp;
	};

#endif
