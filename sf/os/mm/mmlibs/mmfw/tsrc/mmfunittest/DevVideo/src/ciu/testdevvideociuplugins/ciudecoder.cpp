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

#include "ciudecoder.h"

const TYuvFormat KTestYuvFormat = {EYuvRange0, EYuv420Chroma1, EYuvDataPlanar, NULL, NULL, 10, 20};

_LIT(KDevVideoDecoderPanicCategory, "DevVideoDecoder");
void DevVideoDecoderPanic(TInt aReason)
	{
	User::Panic(KDevVideoDecoderPanicCategory, aReason);
	}

CMMFVideoDecodeHwDevice* CMMFTestVideoCiuDecodeHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestVideoCiuDecodeHwDevice* s = new(ELeave) CMMFTestVideoCiuDecodeHwDevice;
	return static_cast<CMMFVideoDecodeHwDevice*>(s);
	}

CMMFTestVideoCiuDecodeHwDevice::CMMFTestVideoCiuDecodeHwDevice()
	{
	}

CMMFTestVideoCiuDecodeHwDevice::~CMMFTestVideoCiuDecodeHwDevice()
	{
	delete iRedrawBuffer;
	}

TAny* CMMFTestVideoCiuDecodeHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidMmfVideoWindowControl)
		{
		MMmfVideoWindowControl* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceVideoWindowControl)
		{
		MDevVideoCiuTestInterfaceVideoWindowControl* interface = this;
		return interface;
		}
	else if (aInterface == KUidMmfVideoResourceManagement)
		{
		MMmfVideoResourceHandler* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceResourceManagement)
		{
		MDevVideoCiuTestInterfaceResourceManagement* interface = this;
		return interface;
		}
	else if (aInterface == KUidMmfVideoResourcePriority)
		{
		MMmfVideoResourcePriority* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceResourcePriority)
		{
		MDevVideoCiuTestInterfaceResourcePriority* interface = this;
		return interface;
		}
	else if (aInterface == KUidMmfVideoPropertiesManagement)
		{
		MMmfVideoPropertiesNotifier* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfacePropertiesManagement)
		{
		MDevVideoCiuTestInterfacePropertiesManagement* interface = this;
		return interface;
		}	
	else if (aInterface == KUidMmfVideoHwDevVideoPolicyExtension)
		{
		return static_cast<MMmfVideoHwDeviceVideoPolicyExtension*>(this);
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceClientResourceNotifier)
		{
		return static_cast<MDevVideoCiuTestInterfaceClientResourceNotifier*>(this);
		}	
#ifdef SYMBIAN_BUILD_GCE
	else if (aInterface == KUidMMFVideoSurfaceSupport)
		{
		MMMFVideoSurfaceSupport* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceVideoSurfaceSupport)
		{
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* interface = this;
		return interface;
		}
	else if (aInterface == KUidMMFVideoClientBufferSupport)
		{
		MMMFVideoClientBufferSupport* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceVideoClientBufferSupport)
		{
		MDevVideoCiuTestInterfaceVideoClientBufferSupport* interface = this;
		return interface;
		}
	else if (aInterface == KUidMMFVideoSurfaceHandleControl)
		{
		MMmfVideoSurfaceHandleControl* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl)
		{
		MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl* interface = this;
		return interface;
		}
#endif // SYMBIAN_BUILD_GCE
	else if (aInterface == KUidMmfVideoPlayRateControl)
		{
		MMmfVideoPlayRateControl* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceVideoPlayRateControl)
		{
		MDevVideoCiuTestInterfaceVideoPlayRateControl* interface = this;
		return interface;
		}
	else if (aInterface == KUidMmfVideoHwDeviceTunnel)
		{
		MMmfVideoHwDeviceTunnel* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceHwDeviceTunnel)
		{
		MDevVideoCiuTestInterfaceHwDeviceTunnel* interface = this;
		return interface;
		}
	else
		{
		return NULL;
		}
	}

// post processor info will be obtained from the post processor plugin
CPostProcessorInfo* CMMFTestVideoCiuDecodeHwDevice::PostProcessorInfoLC()
	{
	return NULL;
	}

void CMMFTestVideoCiuDecodeHwDevice::GetOutputFormatListL(RArray<TUncompressedVideoFormat>& /*aFormats*/)
	{	}

void CMMFTestVideoCiuDecodeHwDevice::SetOutputFormatL(const TUncompressedVideoFormat& /*aFormat*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetPostProcessTypesL(TUint32 /*aPostProcCombination*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetInputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /*aOptions*/, const TYuvFormat& /*aYuvFormat*/, TRgbFormat /*aRgbFormat*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /*aOptions*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetRotateOptionsL(TRotationType /*aRotationType*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetScaleOptionsL(const TSize& /*aTargetSize*/, TBool /*aAntiAliasFiltering*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetOutputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetPostProcSpecificOptionsL(const TDesC8& /*aOptions*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetClockSource(MMMFClockSource* /*aClock*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetVideoDestScreenL(TBool /*aScreen*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::Initialize()
	{
	iProxy->MdvppInitializeComplete(this, KErrNone);
	}

void CMMFTestVideoCiuDecodeHwDevice::StartDirectScreenAccessL(const TRect& /*aVideoRect*/, CFbsScreenDevice& /*aScreenDevice*/, const TRegion& /*aClipRegion*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetScreenClipRegion(const TRegion& /*aRegion*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetPauseOnClipFail(TBool /*aPause*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::AbortDirectScreenAccess()
	{
	}

TBool CMMFTestVideoCiuDecodeHwDevice::IsPlaying()
	{
	return ETrue;
	}

void CMMFTestVideoCiuDecodeHwDevice::Redraw()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::Start()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::Stop()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::Pause()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::Resume()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetPosition(const TTimeIntervalMicroSeconds& /*aPlaybackPosition*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::FreezePicture(const TTimeIntervalMicroSeconds& /*aTimestamp*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::ReleaseFreeze(const TTimeIntervalMicroSeconds& /*aTimestamp*/)
	{
	}

TTimeIntervalMicroSeconds CMMFTestVideoCiuDecodeHwDevice::PlaybackPosition()
	{
	return 0;
	}

TUint CMMFTestVideoCiuDecodeHwDevice::PictureBufferBytes()
	{
	return 0;
	}

void CMMFTestVideoCiuDecodeHwDevice::GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& /*aCounters*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetComplexityLevel(TUint /*aLevel*/)
	{
	}

TUint CMMFTestVideoCiuDecodeHwDevice::NumComplexityLevels()
	{
	return 0;
	}

void CMMFTestVideoCiuDecodeHwDevice::GetComplexityLevelInfo(TUint /*aLevel*/, CMMFDevVideoPlay::TComplexityLevelInfo& /*aInfo*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::ReturnPicture(TVideoPicture* /*aPicture*/)
	{
	}

TBool CMMFTestVideoCiuDecodeHwDevice::GetSnapshotL(TPictureData& /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/)
	{
	return EFalse;
	}

void CMMFTestVideoCiuDecodeHwDevice::GetTimedSnapshotL(TPictureData* /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/, const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::GetTimedSnapshotL(TPictureData* /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/, const TPictureId& /*aPictureId*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::CancelTimedSnapshot()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& /*aFormats*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::InputEnd()
	{
	}

CVideoDecoderInfo* CMMFTestVideoCiuDecodeHwDevice::VideoDecoderInfoLC()
	{
	return NULL;
	}


TVideoPictureHeader* CMMFTestVideoCiuDecodeHwDevice::GetHeaderInformationL(TVideoDataUnitType /*aDataUnitType*/, TVideoDataUnitEncapsulation /*aEncapsulation*/, TVideoInputBuffer* /*aDataUnit*/)
	{
	return NULL;
	}

void CMMFTestVideoCiuDecodeHwDevice::ConfigureDecoderL(const TVideoPictureHeader& /*aVideoPictureHeader*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::ReturnHeader(TVideoPictureHeader* /*aHeader*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetInputFormatL(const CCompressedVideoFormat& /*aFormat*/, TVideoDataUnitType /*aDataUnitType*/, TVideoDataUnitEncapsulation /*aEncapsulation*/, TBool /*aDataInOrder*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SynchronizeDecoding(TBool /*aSynchronize*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& /*aOptions*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::GetBufferOptions(CMMFDevVideoPlay::TBufferOptions& /*aOptions*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetHrdVbvSpec(THrdVbvSpecification /*aHrdVbvSpec*/, const TDesC8& /*aHrdVbvParams*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::SetOutputDevice(CMMFVideoPostProcHwDevice* /*aDevice*/)
	{
	}

TTimeIntervalMicroSeconds CMMFTestVideoCiuDecodeHwDevice::DecodingPosition()
	{
	return 0;
	}

TUint CMMFTestVideoCiuDecodeHwDevice::PreDecoderBufferBytes()
	{
	return 0;
	}

void CMMFTestVideoCiuDecodeHwDevice::GetBitstreamCounters(CMMFDevVideoPlay::TBitstreamCounters& /*aCounters*/)
	{
	}

TUint CMMFTestVideoCiuDecodeHwDevice::NumFreeBuffers()
	{
	return 0;
	}

TVideoInputBuffer* CMMFTestVideoCiuDecodeHwDevice::GetBufferL(TUint /*aBufferSize*/)
	{
	return NULL;
	}

void CMMFTestVideoCiuDecodeHwDevice::WriteCodedDataL(TVideoInputBuffer* /*aBuffer*/)
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::CommitL()
	{
	}

void CMMFTestVideoCiuDecodeHwDevice::Revert()
	{
	}

// From MmfDevVideoWindowControl
void CMMFTestVideoCiuDecodeHwDevice::MmvwcSetLogicalVideoWindow(const TRect& aWindow)
   	{
   	iLogicalVideoWindow = aWindow;
   	}
    	
void CMMFTestVideoCiuDecodeHwDevice::MmvwcSetContentOffsetL(const TPoint& aOffset)
	{
	if(aOffset.iX < 0 || aOffset.iY < 0)
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iContentOffset = aOffset;
		}	
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvwcSetContentAlignment(const THorizAlign aHorizAlign, const TVertAlign aVertAlign)
	{
	iContentAlignment.iHorizAlign = aHorizAlign;
	iContentAlignment.iVertAlign = aVertAlign;
	}

// From MDevVideoCiuTestInterfaceVideoWindowControl
TRect CMMFTestVideoCiuDecodeHwDevice::MmvwcLogicalVideoWindow()
	{
	return iLogicalVideoWindow;
	}
	
TPoint CMMFTestVideoCiuDecodeHwDevice::MmvwcContentOffset()
	{
	return iContentOffset;
	}
	
MDevVideoCiuTestInterfaceVideoWindowControl::TAlignment CMMFTestVideoCiuDecodeHwDevice::MmvwcContentAlignment()
	{
	return iContentAlignment;
	}

// from MMmfVideoResourceHandler
void CMMFTestVideoCiuDecodeHwDevice::MmvrhSetObserver(MMmfVideoResourceObserver* aObserver)
	{
	iResourceObserver = aObserver;
	}
	
// from MDevVideoCiuTestInterfaceVideoWindowControl
MMmfVideoResourceObserver* CMMFTestVideoCiuDecodeHwDevice::MdvrmObserver()
	{
	return iResourceObserver;
	}

void CMMFTestVideoCiuDecodeHwDevice::MdvrmRequestResourceLostCallback()
	{
	// We just want to check that the callback will be received, a proper UID Media Device isn't needed.
	iResourceObserver->MmvroResourcesLost(KNullUid);
	}

void CMMFTestVideoCiuDecodeHwDevice::MdvrmRequestResourceRestoredCallback()
	{
	// We just want to check that the callback will be received, a proper UID Media Device isn't needed.
	iResourceObserver->MmvroResourcesRestored(KNullUid);
	}

// from MMmfVideoResourcePriority
void CMMFTestVideoCiuDecodeHwDevice::MmvpSetResourcePriority(TInt aPriority)
	{
	iResourcePriority = aPriority;
	}
	
// from MDevVideoCiuTestInterfaceResourcePriority
TInt CMMFTestVideoCiuDecodeHwDevice::MdvrpResourcePriority()
	{
	return iResourcePriority;
	}

// from MmfVideoPropertiesNotifier
void CMMFTestVideoCiuDecodeHwDevice::MmvpnSetObserver(MMmfVideoPropertiesObserver* aObserver)
	{
	iPropertiesObserver = aObserver;
	}
	
// from MDevVideoCiuTestInterfacePropertiesManagement
MMmfVideoPropertiesObserver* CMMFTestVideoCiuDecodeHwDevice::MmvpoObserver()
	{
	return iPropertiesObserver;
	}

// from MDevVideoCiuTestInterfacePropertiesManagement	
void CMMFTestVideoCiuDecodeHwDevice::MmvpoUpdateVideoPropertiesCallback()
	{
	// We just want to check that the callback will be received, so TYuvFormat and TSize values don't matter.
	iPropertiesObserver->MmvpoUpdateVideoProperties(KTestYuvFormat, TSize(0,0));
	}

void CMMFTestVideoCiuDecodeHwDevice::SetProxy(MMMFDevVideoPlayProxy& aProxy)
	{
	ASSERT(iProxy == NULL);
	iProxy = &aProxy;
	}

// MMmfVideoHwDeviceVideoPolicyExtension implementation
void CMMFTestVideoCiuDecodeHwDevice::MmvhvpeSetVideoClient(MMmfVideoHwDevClientNotifier* aClient)
	{
	iClientNotifier = aClient;
	}
	
void CMMFTestVideoCiuDecodeHwDevice::MmvhvpeSetVideoPriority(TInt aPriority)
	{
	iVideoPriority = aPriority;
	}

// MDevVideoCiuTestInterfaceClientResourceNotifier implementation
void CMMFTestVideoCiuDecodeHwDevice::McrnRequestRevokeCallback()
	{
	iClientNotifier->MmvhcnVideoAccessRevoked();	
	}
	
void CMMFTestVideoCiuDecodeHwDevice::McrnRequestResourceCallback()
	{
	iClientNotifier->MmvhcnReleaseVideoResource();
	}

MMmfVideoHwDevClientNotifier* CMMFTestVideoCiuDecodeHwDevice::McrnVideoClient()
	{
	return iClientNotifier;
	}

TInt CMMFTestVideoCiuDecodeHwDevice::McrnVideoPriority()
	{
	return iVideoPriority;
	}



#ifdef SYMBIAN_BUILD_GCE

// from MmmfVideoSurfaceSupport
void CMMFTestVideoCiuDecodeHwDevice::MmvssSetObserver(MMMFVideoSurfaceObserver& aObserver)
	{
	iSurfaceSupportObserver = &aObserver;
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvssUseSurfaces()
	{
	iUseSurface = ETrue;
	}
	
void CMMFTestVideoCiuDecodeHwDevice::MmvssGetSurfaceParametersL(TSurfaceId& aSurfaceId, 
						TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio)
	{
	TSurfaceId surfaceId = {11111, 22222, 33333, 44444};
	TRect cropRect(100, 50, 600, 200);
	TVideoAspectRatio par(1,2);
	aSurfaceId = surfaceId;
	aCropRect = cropRect;
	aPixelAspectRatio = par;
	
	iVSurfaceConf.iSurfaceId = surfaceId;
	iVSurfaceConf.iCropRectangle = cropRect;
	iVSurfaceConf.iPixelAspectRatio = par;
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvssSurfaceRemovedL(const TSurfaceId& aSurfaceId)
	{
	iVSurfaceConf.iSurfaceId = aSurfaceId;
	}	

// from MDevVideoCiuTestInterfaceVideoSurfaceSupport
MMMFVideoSurfaceObserver* CMMFTestVideoCiuDecodeHwDevice::MvsoObserver()
	{
	return iSurfaceSupportObserver;
	}

TBool CMMFTestVideoCiuDecodeHwDevice::MdvssUseSurface()
	{
	return iUseSurface;
	}
	
TMMFVideoSurfaceConfig CMMFTestVideoCiuDecodeHwDevice::MdvssSurfaceParameters()
	{
	return iVSurfaceConf;
	}

TSurfaceId CMMFTestVideoCiuDecodeHwDevice::MdvssSurfaceRemoved()
	{
	return 	iVSurfaceConf.iSurfaceId;
	}

// from MDevVideoCiuTestInterfaceVideoSurfaceSupport
void CMMFTestVideoCiuDecodeHwDevice::MmvsoSurfaceCreatedCallback()
	{
	// We just want to check that the callback will be received.
	iSurfaceSupportObserver->MmvsoSurfaceCreated();
	}

// from MDevVideoCiuTestInterfaceVideoSurfaceSupport
void CMMFTestVideoCiuDecodeHwDevice::MmvsoSurfaceParametersChangedCallback()
	{
	// We just want to check that the callback will be received.
	iSurfaceSupportObserver->MmvsoSurfaceParametersChanged();
	}

// from MDevVideoCiuTestInterfaceVideoSurfaceSupport
void CMMFTestVideoCiuDecodeHwDevice::MmvsoRemoveSurfaceCallback()
	{
	// We just want to check that the callback will be received.
	iSurfaceSupportObserver->MmvsoRemoveSurface();
	}

// from MMMFVideoClientBufferSupport
void CMMFTestVideoCiuDecodeHwDevice::MvcbsUseClientBuffers(TBool aClientBuffers)
	{
	iClientBuffers = aClientBuffers;	
	}
	
void CMMFTestVideoCiuDecodeHwDevice::MvcbsSupplyBuffer(TVideoFrameBuffer& aBuffer)
	{
	iBuffer = &aBuffer;	
	}
	
void CMMFTestVideoCiuDecodeHwDevice::MvcbsReleaseBuffers()
	{
	iReleased = ETrue;	
	}
	
// from MDevVideoCiuTestInterfaceVideoClientBufferSupport
TBool CMMFTestVideoCiuDecodeHwDevice::MdvcbsClientBuffers() const
	{
	return 	iClientBuffers;
	}
	
TVideoFrameBuffer* CMMFTestVideoCiuDecodeHwDevice::MdvcbsSupplyBuffer() const
	{	
	return iBuffer;
	}
	
TBool CMMFTestVideoCiuDecodeHwDevice::MdvcbsReleaseBuffers() const
	{
	return iReleased;	
	}

// from MmvshVideoSurfaceHandleControl
void CMMFTestVideoCiuDecodeHwDevice::MmvshcSetSurfaceHandle(const TSurfaceId& aSurfaceHandle)
	{
	iSurfaceId = aSurfaceHandle;
	}	

TSurfaceId CMMFTestVideoCiuDecodeHwDevice::MdvshcGetPlaySurfaceHandle()
	{
	return iSurfaceId;
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvshcRedrawBufferToSurface(TPtrC8& aRedrawBuffer)
	{
	iRedrawBuffer = aRedrawBuffer.Alloc();
	}

TDesC8& CMMFTestVideoCiuDecodeHwDevice::MdvshcGetRedrawBuffer()
	{
	return *iRedrawBuffer;	
	}

#endif // SYMBIAN_BUILD_GCE

// from MMmfVideoPlayRateControl
void CMMFTestVideoCiuDecodeHwDevice::MmvprcSetObserver(MMmfVideoPlayRateObserver& aObserver)
	{
	iPlayRateObserver = &aObserver;
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvprcGetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities)
	{
	// The following values are for testing purpose
	aCapabilities.iPlayBackward = ETrue;
	aCapabilities.iPlayForward = EFalse;
	aCapabilities.iStepBackward = ETrue;
	aCapabilities.iStepForward = EFalse;
	
	// Set the internal capabilities
	iPlayRateCapabilities = aCapabilities;
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvprcSetPlayRateL(const TInt aRate)
	{
	iPlayRate = aRate;
	}


TInt CMMFTestVideoCiuDecodeHwDevice::MmvprcPlayRateL()
	{
	return iPlayRate;
	}

void CMMFTestVideoCiuDecodeHwDevice::MmvprcStepFrameL(const TInt aStep)
	{
	iStepFrame = aStep;	
	}	

// from MDevVideoCiuTestInterfaceVideoPlayRateControl
MMmfVideoPlayRateObserver* CMMFTestVideoCiuDecodeHwDevice::MdvproObserver()
	{
	return iPlayRateObserver;
	}

TVideoPlayRateCapabilities CMMFTestVideoCiuDecodeHwDevice::MdvproGetPlayRateCapabilities()
	{
	return iPlayRateCapabilities;
	}
	
TInt CMMFTestVideoCiuDecodeHwDevice::MdvproStepFrame()
	{
	return iStepFrame;
	}

// from MMmfVideoPlayRateObserver
void CMMFTestVideoCiuDecodeHwDevice::MdvproStepFrameCompleteCallback()
	{
	// We want to check that the callback will be received, and the value of TTimeIntervalMicroSeconds is passed.
	TTimeIntervalMicroSeconds timeStamp = 10000;
	iPlayRateObserver->MmvproStepFrameComplete(timeStamp);
	}

// from MMmfVideoPlayRateObserver
void CMMFTestVideoCiuDecodeHwDevice::MdvproKeyFrameModeRequestCallback()
	{
	// We just want to check that the callback will be received.
	iPlayRateObserver->MmvproKeyFrameModeRequest();
	}

// from MMmfVideoHwDeviceTunnel
CMMFVideoHwDevice* CMMFTestVideoCiuDecodeHwDevice::MmvhdtHwDevice()
	{
	// return a value that can be tested for later
	return reinterpret_cast<CMMFVideoHwDevice*>(0x11111111);
	}

// from MMmfVideoHwDeviceTunnel
void CMMFTestVideoCiuDecodeHwDevice::MmvhdtSetupTunnelL(CMMFVideoHwDevice* aHwDevice)
	{
	iTunnelHwDevice = aHwDevice;
	}

// from MDevVideoCiuTestInterfaceHwDeviceTunnel
CMMFVideoHwDevice* CMMFTestVideoCiuDecodeHwDevice::MhdtHwDevice()
	{
	return iTunnelHwDevice;
	}
