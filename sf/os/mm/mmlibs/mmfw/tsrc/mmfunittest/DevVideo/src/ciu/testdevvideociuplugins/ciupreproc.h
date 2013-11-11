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

#ifndef CIUPREPROC_H
#define CIUPREPROC_H

#include <mmf/devvideo/videorecordhwdevice.h>
#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include "testciupluginuids.hrh"
#include "ciudevvideotestinterface.h"

enum TPreProcPanicCategory
	{
	EPreProcPanicInitialize = 0,
	EPreProcPanicClockSource,
	EPreProcPanicScreenClipRegion,
	EPreProcPanicPauseClipFail,
	EPreProcPanicSetPosition,
	EPreProcPanicFreezePicture,
	EPreProcPanicReleaseFreeze,
	EPreProcPanicComplexityLevel,
	EPreProcPanicComplexityLevelInfo
	};

const TUid KUidDevVideoCiuTestPreProcHwDevice = {KUidDevVideoCiuTestPreProcHwDeviceDefine1};

class CMMFTestCiuVideoPreProcHwDevice : public CMMFVideoPreProcHwDevice,
										public MMmfVideoInputSnapshot,
										public MDevVideoCiuTestInterfaceInputSnapshot
	{
public:
	static CMMFVideoPreProcHwDevice* NewL(TAny* aInitParams);
	~CMMFTestCiuVideoPreProcHwDevice();

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

	// from CMMFVideoPreProcHwDevice
	virtual void SetOutputFormatL(const TUncompressedVideoFormat& aFormat);
	virtual void SetOutputDevice(CMMFVideoEncodeHwDevice* aDevice);
	virtual void ReturnPicture(TVideoPicture* aPicture);
	virtual void SetProxy(MMMFDevVideoRecordProxy& aProxy);

	// from MMmfVideoInputSnapshot
	virtual void MmvisSetObserver(MMmfVideoInputSnapshotObserver* aObserver);
	virtual void MmvisGetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);
	virtual void MmvisCancelSnapshot();
	
	// from MDevVideoCiuTestInterfaceInputSnapshot
	virtual MMmfVideoInputSnapshotObserver* MmvisSnapshotObserver();
	virtual void MmvisSetSnapshot(const TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);
	virtual void MmvisRequestSnapshotCompleteCallback();
	virtual TBool MmvisIsCancelSnapshot();
private:
	CMMFTestCiuVideoPreProcHwDevice();
	
private:
	MMMFDevVideoRecordProxy* iProxy;
	TBool iIsRecording;	
	MMmfVideoInputSnapshotObserver* iInputSnapshotObserver;
	TPictureData iPictureData;
	TUncompressedVideoFormat iVideoFormat;
	TBool iCancelSnapshot;
	};

#endif
