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

/**
 @file
 @internalTechnology
 @prototype
*/
#ifndef  ECAMCAPTURECONTROL_CONST_H
#define  ECAMCAPTURECONTROL_CONST_H
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsuidsdef.hrh>
#endif
/**
Maximum number of possible focussing spots

@internalTechnology
*/
static const TInt KMaxNumberOfFocusSpots 	= sizeof(TUint) * 8;

/** 
Constant used to provide maximum number of total sub-frames which would re-construct the image properly.

@internalTechnology
*/
static const TInt KECamMaxTotalSubFrames 	= 1024;

/** 
The current Version of the TImageBufferInfo class.

@internalTechnology
*/
static const TUint KECamImageBufferInfoCurrentVersion = 1;

/** 
Notification that streamed image and hence sub-frames scheme has been enabled.

@internalTechnology
*/
static const TUid  KUidECamEventImageCaptureControlEnableSubFrames = {KUidECamEventImageCaptureControlEnableSubFramesUidValue};

/** 
Notification that streamed image and hence sub-frames scheme has been disabled.

@internalTechnology
*/
static const TUid  KUidECamEventImageCaptureControlDisableSubFrames = {KUidECamEventImageCaptureControlDisableSubFramesUidValue};

/**
Notifies the client about the setting of color space. Camera will print or capture images in given color space.

@internalTechnology
*/
static const TUid KUidECamEventImageCaptureControlColorSpace = {KUidECamEventImageCaptureControlColorSpaceUidValue};

/** 
Notifies the client that pano mode has been started. 

@internalTechnology
*/
static const TUid KUidECamEventImageCaptureControlStartPanoMode = {KUidECamEventImageCaptureControlStartPanoModeUidValue};

/** 
Notifies the client about the setting of Panorama direction.

@internalTechnology
*/
static const TUid  KUidECamEventImageCaptureControlPanoDirection = {KUidECamEventImageCaptureControlPanoDirectionUidValue};

/** 
Notification informing that some of the embedded still capture settings have failed.
To retrieve the list of failed embedded still capture settings, client should use GetFailedEmbeddedStillCaptureSettingsL()

@internalTechnology
*/
static const TUid  KUidECamEventFailedEmbeddedStillCaptureSetting = {KUidECamEventFailedEmbeddedStillCaptureSettingUidValue};

/**
Notifies the setting of fading effect for the video captured frames.

@internalTechnology
*/
static const TUid KUidECamEventVideoCaptureControlFadingEffect = {KUidECamEventVideoCaptureControlFadingEffectUidValue};

/** 
Conversion Coefficient setting

@internalTechnology
*/
static const TUid KUidECamEventVideoCaptureControlConversionCoefficient = {KUidECamEventVideoCaptureControlConversionCoefficientUidValue};

	
#endif //ECAMCAPTURECONTROL_CONST_H
