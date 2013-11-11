// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFSTANDARDCUSTOMCOMMANDS_ENUMS_H
#define __MMFSTANDARDCUSTOMCOMMANDS_ENUMS_H


/**
@internalComponent
*/
enum TMMFAudioResourceNotificationMessages
	{
	EMMFAudioResourceRegisterNotification,
	EMMFAudioResourceCancelRegisterNotification,
	EMMFAudioResourceGetNotificationData,
	EMMFAudioResourceWillResumePlay
	};

/**
@internalComponent
*/
enum TMMFAudioPlayDeviceMessages
	{
	EMMFAudioPlayDeviceSetVolume,
	EMMFAudioPlayDeviceGetMaxVolume,
	EMMFAudioPlayDeviceGetVolume,
	EMMFAudioPlayDeviceSetVolumeRamp,
	EMMFAudioPlayDeviceSetBalance,
	EMMFAudioPlayDeviceGetBalance
	};

/**
@internalComponent
*/
enum TMMFAudioRecordDeviceMessage
	{
	EMMFAudioRecordDeviceSetGain,
	EMMFAudioRecordDeviceGetMaxGain,
	EMMFAudioRecordDeviceGetGain,
	EMMFAudioRecordDeviceSetBalance,
	EMMFAudioRecordDeviceGetBalance
	};

/**
@internalComponent
*/
enum TMMFAudioPlayControllerMessages
	{
	EMMFAudioPlayControllerSetPlaybackWindow,
	EMMFAudioPlayControllerDeletePlaybackWindow,
	EMMFAudioPlayControllerGetLoadingProgress
	};

/**
@internalComponent
*/
enum TMMFAudioRecordControllerMessages
	{
	EMMFAudioRecordControllerGetRecordTimeAvailable,
	EMMFAudioRecordControllerSetMaxDuration,
	EMMFAudioRecordControllerSetMaxFileSize,
	EMMFAudioRecordControllerCrop,
	EMMFAudioRecordControllerAddMetaDataEntry,
	EMMFAudioRecordControllerRemoveMetaDataEntry,
	EMMFAudioRecordControllerReplaceMetaDataEntry
	};


/**
@internalComponent
*/
enum TMMFVideoControllerMessages
	{
	EMMFVideoControllerGetAudioCodec,
	EMMFVideoControllerGetVideoBitRate,
	EMMFVideoControllerGetAudioBitRate,
	EMMFVideoControllerGetVideoFrameSize,
	EMMFVideoControllerSetFrameRate,
	EMMFVideoControllerGetFrameRate,
	EMMFVideoControllerGetVideoMimeType
	};


/**
@internalComponent
*/
enum TMMFVideoPlayControllerMessages
	{
	EMMFVideoPlayControllerPrepare,
	EMMFVideoPlayControllerGetFrame,
	EMMFVideoPlayControllerSetDisplayWindow,
	EMMFVideoPlayControllerGetAudioEnabled,
	EMMFVideoPlayControllerUpdateDisplayRegion,
	EMMFVideoPlayControllerDSAEvent,
	EMMFVideoPlayControllerPlay,
	EMMFVideoPlayControllerRefreshFrame,
	EMMFVideoPlayControllerGetLoadingProgress,
	EMMFVideoPlayControllerSetRotation,
	EMMFVideoPlayControllerGetRotation,
	EMMFVideoPlayControllerSetScaleFactor,
	EMMFVideoPlayControllerGetScaleFactor,
	EMMFVideoPlayControllerSetCropRegion,
	EMMFVideoPlayControllerGetCropRegion
	};

/**
@internalComponent
*/
enum TMMFDRMIntentMessages
	{
	EMMFVideoDRMExtGetFrame
	};


/**
@internalComponent
*/
enum TMMFVideoScreenDeviceMessages
	{
	EMMFVideoSetInitScreenNumber
	};


/**
Used for commands between RMMFVideoPlayControllerExtCustomCommands and 
CMMFVideoPlayControllerExtCustomCommandParser classes.

@internalComponent
*/
enum TMMFVideoPlayControllerExtCustomCommandConfigMessages
	{
	EMMFVideoPlayControllerSetPlayVelocity = 0,
	EMMFVideoPlayControllerPlayVelocity,
	EMMFVideoPlayControllerStepFrame,
	EMMFVideoPlayControllerGetPlayRateCapabilities,
	EMMFVideoPlayControllerSetVideoEnabled,
	EMMFVideoPlayControllerVideoEnabled,
	EMMFVideoPlayControllerSetAudioEnabled,
	EMMFVideoPlayControllerSetAutoScale
	};
/**
@internalComponent
*/
class TMMFVideoPlayAutoScaleParams
	{
public:
	TAutoScaleType iScaleType;
	TInt iHorizPos;
	TInt iVertPos;
	};

/**
@internalComponent
*/
enum TMMFVideoRecorderExtCustomCommandConfigMessages
	{
	EMMFVideoRecordControllerSetVideoEnabled = 0,
	EMMFVideoRecordControllerVideoEnabled,
	EMMFVideoRecordControllerSetVideoQuality,	
	EMMFVideoRecordControllerVideoQuality,
	EMMFVideoRecordControllerSetVideoFrameRateFixed,
	EMMFVideoRecordControllerVideoFrameRateFixed
	};

/**
@internalComponent
*/
enum TMMFAudioPlayControllerSetRepeatsMessages
	{
	EMMFAudioPlayControllerSetRepeats
	};

#endif
