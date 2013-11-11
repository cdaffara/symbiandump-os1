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

#ifndef CIUDECODER_H
#define CIUDECODER_H

#include <mmf/devvideo/videoplayhwdevice.h>
#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include <mmf/common/mmfvideo.h>
#include <mmf/devvideo/devvideoconstants.h>
#include <mmf/devvideo/devvideoplayratecustominterface.h>
#include <mmf/devvideo/devvideohwdevicetunnelcustominterface.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/devvideo/devvideovideosurfacecustominterface.h>
#include <mmf/devvideo/devvideoclientbuffersupport.h>
#include <mmf/devvideo/devvideosurfacehandlecustominterface.h>
#endif // SYMBIAN_BUILD_GCE

#include "ciudevvideotestinterface.h"
#include "testciupluginuids.hrh"

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

const TUid KUidDevVideoCiuTestDecodeHwDevice = {KUidDevVideoCiuTestDecodeHwDeviceDefine1}; //This is the only real plugin

// Used to test the priortiy request and resource notification control interfaces
class CMMFTestVideoCiuDecodeHwDevice : public CMMFVideoDecodeHwDevice,
										public MMmfVideoWindowControl,
										public MDevVideoCiuTestInterfaceVideoWindowControl,
										public MMmfVideoResourceHandler,
										public MDevVideoCiuTestInterfaceResourceManagement,
										public MMmfVideoResourcePriority,
										public MDevVideoCiuTestInterfaceResourcePriority,
										public MMmfVideoPropertiesNotifier,
										public MDevVideoCiuTestInterfacePropertiesManagement,
										public MMmfVideoHwDeviceVideoPolicyExtension,
										public MDevVideoCiuTestInterfaceClientResourceNotifier,
										public MMmfVideoPlayRateControl,
										public MDevVideoCiuTestInterfaceVideoPlayRateControl,
										public MMmfVideoHwDeviceTunnel,
										public MDevVideoCiuTestInterfaceHwDeviceTunnel
#ifdef SYMBIAN_BUILD_GCE
,public MMMFVideoSurfaceSupport,
										public MDevVideoCiuTestInterfaceVideoSurfaceSupport,
										public MMMFVideoClientBufferSupport,
										public MDevVideoCiuTestInterfaceVideoClientBufferSupport,
										public MMmfVideoSurfaceHandleControl,
										public MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl
#endif // SYMBIAN_BUILD_GCE
	{
public:
	static CMMFVideoDecodeHwDevice* NewL(TAny* aInitParams);
	~CMMFTestVideoCiuDecodeHwDevice();

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

	//from MMmfVideoWindowControl
    void MmvwcSetLogicalVideoWindow(const TRect& aWindow);
    void MmvwcSetContentOffsetL(const TPoint& aOffset);
    void MmvwcSetContentAlignment(const THorizAlign aHorizAlign, const TVertAlign aVertAlign);
    // From MDevVideoCiuTestInterfaceVideoWindowControl
    TRect MmvwcLogicalVideoWindow();
	TPoint MmvwcContentOffset();
    TAlignment MmvwcContentAlignment();
    
    // from MMmfVideoResourceHandler
    void MmvrhSetObserver(MMmfVideoResourceObserver* aObserver);
    // from MDevVideoCiuTestInterfaceResourceManagement
    MMmfVideoResourceObserver* MdvrmObserver();
    void MdvrmRequestResourceLostCallback();
    void MdvrmRequestResourceRestoredCallback();
    
    // from MMmfVideoResourcePriority
    void MmvpSetResourcePriority(TInt aPriority);
    // from MDevVideoCiuTestInterfaceResourcePriority
    TInt MdvrpResourcePriority();
    
    // from MmfVideoPropertiesNotifier
    void MmvpnSetObserver(MMmfVideoPropertiesObserver* aObserver);
    // from MDevVideoCiuTestInterfacePropertiesManagement
    MMmfVideoPropertiesObserver* MmvpoObserver();
    void MmvpoUpdateVideoPropertiesCallback();
		
	// from MMmfVideoHwDeviceVideoPolicyExtension
 	virtual void MmvhvpeSetVideoClient(MMmfVideoHwDevClientNotifier* aClient);
	virtual void MmvhvpeSetVideoPriority(TInt aPriority);

	// from MDevVideoCiuTestInterfaceClientResourceNotifier
	virtual void McrnRequestRevokeCallback();
	virtual void McrnRequestResourceCallback();
	virtual MMmfVideoHwDevClientNotifier* McrnVideoClient();
	virtual TInt McrnVideoPriority();

	// from MMmfVideoPlayRateControl
	void MmvprcGetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities);
	void MmvprcSetPlayRateL(const TInt aRate);
	TInt MmvprcPlayRateL();
	void MmvprcStepFrameL(const TInt aStep);
	void MmvprcSetObserver(MMmfVideoPlayRateObserver& aObserver);
	
	// from MDevVideoCiuTestInterfaceVideoPlayRateControl
	MMmfVideoPlayRateObserver* MdvproObserver();
	TVideoPlayRateCapabilities MdvproGetPlayRateCapabilities();
	TInt  MdvproStepFrame();
	void MdvproStepFrameCompleteCallback();	
	void MdvproKeyFrameModeRequestCallback();

	// from MMmfVideoHwDeviceTunnel
	CMMFVideoHwDevice* MmvhdtHwDevice();
	void MmvhdtSetupTunnelL(CMMFVideoHwDevice* aHwDevice);

	// from MDevVideoCiuTestInterfaceHwDeviceTunnel
	CMMFVideoHwDevice* MhdtHwDevice();

#ifdef SYMBIAN_BUILD_GCE    
    // from MmmfVideoSurfaceSupport
    void MmvssSetObserver(MMMFVideoSurfaceObserver& aObserver);
    void MmvssUseSurfaces();
    void MmvssGetSurfaceParametersL(TSurfaceId& aSurfaceId, TRect& aCropRect, 
    					TVideoAspectRatio& aPixelAspectRatio);
    void MmvssSurfaceRemovedL(const TSurfaceId& aSurfaceId);
    
    //from MDevVideoCiuTestInterfaceVideoSurfaceSupport
    MMMFVideoSurfaceObserver* MvsoObserver();
    TBool MdvssUseSurface();
    TMMFVideoSurfaceConfig MdvssSurfaceParameters();
    TSurfaceId MdvssSurfaceRemoved();
	void MmvsoSurfaceCreatedCallback();	
	void MmvsoSurfaceParametersChangedCallback();
	void MmvsoRemoveSurfaceCallback();
	
	// from MMMFVideoClientBufferSupport
	void MvcbsUseClientBuffers(TBool aClientBuffers);
	void MvcbsSupplyBuffer(TVideoFrameBuffer& aBuffer);
	void MvcbsReleaseBuffers();
	
	// from MDevVideoCiuTestInterfaceVideoClientBufferSupport
	TBool MdvcbsClientBuffers() const;
	TVideoFrameBuffer* MdvcbsSupplyBuffer() const;
	TBool MdvcbsReleaseBuffers() const;
	
 	// from MMvshVideoSurfaceHandleControl
 	void MmvshcSetSurfaceHandle(const TSurfaceId& aSurfaceId);
 	void MmvshcRedrawBufferToSurface(TPtrC8& aRedrawBuffer);

	// from MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl
	TSurfaceId MdvshcGetPlaySurfaceHandle();
 	TDesC8& MdvshcGetRedrawBuffer();

#endif // SYMBIAN_BUILD_GCE
	
private:
	CMMFTestVideoCiuDecodeHwDevice();
private:
	MMMFDevVideoPlayProxy* iProxy;

	// To test MMmfVideoWindowControl Set Methods
	TRect iLogicalVideoWindow;
	TPoint iContentOffset;
	TAlignment iContentAlignment;
	
	// To test Resource Management Custom Interfaces
	MMmfVideoResourceObserver* iResourceObserver;
	TInt iResourcePriority;
	TInt iError;
	
	// To test Properties Management Custom Interfaces
	MMmfVideoPropertiesObserver* iPropertiesObserver;

 	MMmfVideoHwDevClientNotifier* iClientNotifier;
	TInt iVideoPriority;

	// To test Video Play Rate Control Custom Interfaces
	MMmfVideoPlayRateObserver* iPlayRateObserver;
	TVideoPlayRateCapabilities iPlayRateCapabilities;
	TInt iStepFrame;
	TInt iPlayRate;

	CMMFVideoHwDevice* iTunnelHwDevice;  // Not owned
	
#ifdef SYMBIAN_BUILD_GCE	
	// To test Video Surface Support Management Custom Interfaces
	MMMFVideoSurfaceObserver* iSurfaceSupportObserver;
	TBool iUseSurface;
	TMMFVideoSurfaceConfig iVSurfaceConf;
	
	// To test video client buffer support custom interfaces
	TBool iClientBuffers;
	TBool iReleased;
	TVideoFrameBuffer* iBuffer;
	TSurfaceId iSurfaceId;
	HBufC8* iRedrawBuffer;
#endif // SYMBIAN_BUILD_GCE
	};

#endif
