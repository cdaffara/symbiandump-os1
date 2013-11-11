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

#ifndef CIUDEVIDEOTESTINTERFACE_H
#define CIUDEVIDEOTESTINTERFACE_H

#include <mmf/devvideo/devvideoplay.h>
#include <mmf/devvideo/devvideoplayratecustominterface.h>

#ifdef SYMBIAN_BUILD_GCE
#include <videoframebuffer.h>
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <mmf/devvideo/devvideovideosurfacecustominterface.h>
#endif // SYMBIAN_BUILD_GCE

#include "testciupluginuids.hrh"

//
//	These interfaces may be implemented by the test hw devices to allow the test suite
//	to call methods that fill the gaps in the actual custom interfaces.
//	
//	e.g. if a CI has a "set" method without a corresponding "get", then the corresponding interface
//		can define the "getter" to allow the test suite to verify any values set on the hw device.
//	
//	Alternatively, observer / notifier callbacks may be issued using the interface.
//

//
// Test interface for MMmfVideoWindowControl
//
const TUid KUidDevVideoCiuTestInterfaceVideoWindowControl = {KUidDevVideoCiuTestInterfaceVideoWindowControlDefine};

class MDevVideoCiuTestInterfaceVideoWindowControl
	{
public:
	struct TAlignment
		{
		MMmfVideoWindowControl::THorizAlign	iHorizAlign;
		MMmfVideoWindowControl::TVertAlign	iVertAlign;
		};

	virtual TRect MmvwcLogicalVideoWindow() = 0;
    virtual TPoint MmvwcContentOffset()  = 0;
    virtual TAlignment MmvwcContentAlignment()  = 0;
	};

//
// Test Interface for MMmfVideoResourceHandler and MMmfVideoResourceObserver
//
const TUid KUidDevVideoCiuTestInterfaceResourceManagement = {KUidDevVideoCiuTestInterfaceResourceManagementDefine};

class MDevVideoCiuTestInterfaceResourceManagement
	{
public:
	virtual MMmfVideoResourceObserver* MdvrmObserver() = 0;
	virtual void MdvrmRequestResourceLostCallback() = 0;	
	virtual void MdvrmRequestResourceRestoredCallback() = 0;
	};

//
// Test Interface for MMmfVideoResourcePriority
//
const TUid KUidDevVideoCiuTestInterfaceResourcePriority = {KUidDevVideoCiuTestInterfaceResourcePriorityDefine};

class MDevVideoCiuTestInterfaceResourcePriority
	{
public:
	virtual TInt MdvrpResourcePriority() = 0;
	};

// 
// Test Interface for MMmfVideoPropertiesNotifier and MMmfVideoPropertiesObserver
// 
const TUid KUidDevVideoCiuTestInterfacePropertiesManagement = {KUidDevVideoCiuTestInterfacePropertiesManagementDefine};

class MDevVideoCiuTestInterfacePropertiesManagement
	{
public:
	virtual MMmfVideoPropertiesObserver* MmvpoObserver() = 0;
	virtual void MmvpoUpdateVideoPropertiesCallback() = 0;	
	};

const TUid KUidDevVideoCiuTestInterfaceMediaProtected = {KUidDevVideoCiuTestInterfaceMediaProtectedDefine};
//
// Test interface for MMmfVideoMediaProtected
//
class MDevVideoCiuTestInterfaceMediaProtected
	{
public:
	//
	// Verifies that MMmfVideoMediaProtected::MmvsoMediaIsProtectedL(TBool aProtected)
	//
	virtual TBool MmvsoMediaIsProtected() = 0;
	};


const TUid KUidDevVideoCiuTestInterfaceInputSnapshot = {KUidDevVideoCiuTestInterfaceInputSnapshotDefine};
//
// Test interface for MMmfVideoInputSnapshot
//
class MDevVideoCiuTestInterfaceInputSnapshot
	{
public:
	//
	// Get the observer
	//
	virtual MMmfVideoInputSnapshotObserver* MmvisSnapshotObserver() = 0;
	//
	// Setter for MmvisGetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat)
	//
	virtual void MmvisSetSnapshot(const TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat) = 0;
	//
	// MMmfVideoInputSnapshotObserver
	//
	virtual void MmvisRequestSnapshotCompleteCallback() = 0;
	//
	// CancelSnapshot call verification
	//
	virtual TBool MmvisIsCancelSnapshot() = 0;
	};


const TUid KUidDevVideoCiuTestInterfaceColourSpace = {KUidDevVideoCiuTestInterfaceColourSpaceDefine};
//
// Test interface for MMmfVideoPlayHwDeviceColourInfoCustomHeader
//
class MDevVideoCiuTestInterfaceColourSpace
	{
public:
	//
	// Set the colour space header info
	//
	virtual void MmvpciSetColourSpaceHeaderInfoL(const TVideoDataUnitType aDataUnitType,
												const TVideoDataUnitEncapsulation aDataUnitEncapsulation, 
												TVideoInputBuffer* aDataUnit, 
												const TYuvFormat& aYuvFormat) = 0;
	};
	


const TUid KUidDevVideoCiuTestInterfaceClientResourceNotifier = {KUidDevVideoCiuTestInterfaceClientResourceNotifierDefine};
//
// Test interface for MMmfVideoHwDeviceVideoPolicyExtension and 
// MMmfVideoHwDevClientNotifier method calls
//
class MDevVideoCiuTestInterfaceClientResourceNotifier
	{
public:
	//
	// Request the video access revoked callback. NB: ensure that the notifier is set
	// before making this call
	//
	virtual void McrnRequestRevokeCallback() = 0;
	//
	// Request the release video resource callback. NB: ensure that the notifier is set
	// before making this call
	//
	virtual void McrnRequestResourceCallback() = 0;
	//
	// Retrieve the client notifier
	//
	virtual MMmfVideoHwDevClientNotifier* McrnVideoClient() = 0;
	//
	// Retrieve the priority
	//
	virtual TInt McrnVideoPriority() = 0;
	};

const TUid KUidDevVideoCiuTestInterfaceBufferManagement = {KUidDevVideoCiuTestInterfaceBufferManagementDefine};
//
// Test interface for MMmfVideoBufferManagement method calls
//
class MDevVideoCiuTestInterfaceBufferManagement
	{
public:
	//
	// Retrieve the observer
	//
	virtual MMmfVideoBufferManagementObserver* MdvbmObserver() = 0;
	//
	// Retrieve the enable flag
	//
	virtual TBool MdvbmIsEnabled() = 0;
	//
	// Set the buffer of a given size
	//
	virtual void MdvbmSetBuffer(TSize& aSize, TVideoPicture& aBuffer) = 0;
	//
	// Request the new buffers callback. NB: ensure that the observer
	// is set before making this call.
	//
	virtual void MdvbmRequestNewBuffersCallback() = 0;
	//
	// Request the release buffers callback. NB: ensure that the observer
	// is set before making this call.
	//
	virtual void MdvbmRequestReleaseBuffersCallback() = 0;
	};

	
// 
// Test Interface for MMmfVideoH324AnnexKMode
// 
const TUid KUidDevVideoCiuTestInterfaceH324AnnexKMode = {KUidDevVideoCiuTestInterfaceH324AnnexKModeDefine};

class MDevVideoCiuTestInterfaceH324AnnexKMode
	{
public:
	virtual TBool MmvhakmH324MAnnexKModeL() = 0;
	};

#ifdef SYMBIAN_BUILD_GCE

/**
@internalTechnology
@prototype

Class used when sending custom commands from the client API
to the video surface controller to get or set the video configuration.
*/
class TMMFVideoSurfaceConfig
	{
public:
	/**
	The surface ID for the display.
	*/
	TSurfaceId					iSurfaceId;
	/**
	The crop region currently applied to the image.
	*/
	TRect						iCropRectangle;
	/**
	The video picture pixel aspect ratio.
	*/
	TVideoAspectRatio			iPixelAspectRatio;
	};
	
//
// Test Interface for MMMFVideoSurfaceSupport and MMMFVideoSurfaceObserver
//
const TUid KUidDevVideoCiuTestInterfaceVideoSurfaceSupport = {KUidDevVideoCiuTestInterfaceVideoSurfaceSupportDefine};

class MDevVideoCiuTestInterfaceVideoSurfaceSupport
	{
public:
	virtual MMMFVideoSurfaceObserver* MvsoObserver() = 0;
	virtual TBool MdvssUseSurface() = 0;
	virtual TMMFVideoSurfaceConfig MdvssSurfaceParameters() =0;
	virtual TSurfaceId MdvssSurfaceRemoved() =0;
	virtual void MmvsoSurfaceCreatedCallback() = 0;	
	virtual void MmvsoSurfaceParametersChangedCallback() = 0;
	virtual void MmvsoRemoveSurfaceCallback() = 0;
	};
	
//
// Test Interface for MMMFVideoClientBufferSupport
//
const TUid KUidDevVideoCiuTestInterfaceVideoClientBufferSupport = {KUidDevVideoCiuTestInterfaceVideoClientBufferSupportDefine};

class MDevVideoCiuTestInterfaceVideoClientBufferSupport
	{
public:
	virtual TBool MdvcbsClientBuffers() const = 0;
	virtual TVideoFrameBuffer* MdvcbsSupplyBuffer() const = 0;
	virtual TBool MdvcbsReleaseBuffers() const = 0;
	};

//
// Test Interface for MMmfVideoPlaySurfaceHandleControl
//
const TUid KUidDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl = {KUidDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControlDefine};

class MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl
	{
public:
	virtual TSurfaceId MdvshcGetPlaySurfaceHandle() = 0;
	virtual TDesC8& MdvshcGetRedrawBuffer() = 0;
	};

#endif // SYMBIAN_BUILD_GCE
	
//
// Test Interface for MMmfVideoPlayRateObserver and MMmfVideoPlayRateControl
//
const TUid KUidDevVideoCiuTestInterfaceVideoPlayRateControl = {KUidDevVideoCiuTestInterfaceVideoPlayRateControlDefine};

class MDevVideoCiuTestInterfaceVideoPlayRateControl
	{
public:
	virtual MMmfVideoPlayRateObserver* MdvproObserver() = 0;
	virtual TVideoPlayRateCapabilities MdvproGetPlayRateCapabilities() = 0;
	virtual TInt  MdvproStepFrame() = 0;
	virtual void MdvproStepFrameCompleteCallback() = 0;	
	virtual void MdvproKeyFrameModeRequestCallback() = 0;
	};

// Test interface for MMmfVideoHwDeviceTunnel

const TUid KUidDevVideoCiuTestInterfaceHwDeviceTunnel = {KUidDevVideoCiuTestInterfaceHwDeviceTunnelDefine};

class MDevVideoCiuTestInterfaceHwDeviceTunnel
	{
public:
	virtual CMMFVideoHwDevice* MhdtHwDevice() = 0;
	};

#endif
