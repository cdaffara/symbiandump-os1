/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#ifndef __T_CAMERA_PARAMS_H_INCLUDED__
#define __T_CAMERA_PARAMS_H_INCLUDED__

_LIT(KUnknown, "Unknown");
// CCamera::TFormat
_LIT(KFormatMonochrome, 		"EFormatMonochrome");
_LIT(KFormat16bitRGB444, 		"EFormat16bitRGB444");
_LIT(KFormat16BitRGB565, 		"EFormat16BitRGB565");
_LIT(KFormat32BitRGB888, 		"EFormat32BitRGB888");
_LIT(KFormatJpeg, 				"EFormatJpeg");
_LIT(KFormatExif, 				"EFormatExif");
_LIT(KFormatFbsBitmapColor4K, 	"EFormatFbsBitmapColor4K");
_LIT(KFormatFbsBitmapColor64K, 	"EFormatFbsBitmapColor64K");
_LIT(KFormatFbsBitmapColor16M, 	"EFormatFbsBitmapColor16M");
_LIT(KFormatUserDefined, 		"EFormatUserDefined");
_LIT(KFormatYUV420Interleaved, 	"EFormatYUV420Interleaved");
_LIT(KFormatYUV420Planar, 		"EFormatYUV420Planar");
_LIT(KFormatYUV422, 			"EFormatYUV422");
_LIT(KFormatYUV422Reversed, 	"EFormatYUV422Reversed");
_LIT(KFormatYUV444, 			"EFormatYUV444");
_LIT(KFormatYUV420SemiPlanar, 	"EFormatYUV420SemiPlanar");
_LIT(KFormatFbsBitmapColor16MU, "EFormatFbsBitmapColor16MU");

// CCamera::TExposure params
_LIT(KExposureAuto,				"EExposureAuto");
_LIT(KExposureNight,			"EExposureNight");
_LIT(KExposureBacklight,		"EExposureBacklight");
_LIT(KExposureCenter,			"EExposureCenter");
_LIT(KExposureSport,			"EExposureSport");
_LIT(KExposureVeryLong,			"EExposureVeryLong");
_LIT(KExposureSnow,				"EExposureSnow");
_LIT(KExposureBeach,			"EExposureBeach");
_LIT(KExposureProgram,			"EExposureProgram");
_LIT(KExposureAperturePriority,	"EExposureAperturePriority");
_LIT(KExposureShutterPriority,	"EExposureShutterPriority");
_LIT(KExposureManual,			"EExposureManual");
_LIT(KExposureSuperNight,		"EExposureSuperNight");
_LIT(KExposureInfra,			"EExposureInfra");

// CCamera::TFlash params
_LIT(KFlashNone, 				"EFlashNone");
_LIT(KFlashAuto, 				"EFlashAuto");
_LIT(KFlashForced, 	 			"EFlashForced");
_LIT(KFlashFillIn,			 	"EFlashFillIn");
_LIT(KFlashRedEyeReduce, 		"EFlashRedEyeReduce");
_LIT(KFlashSlowFrontSync,		"EFlashSlowFrontSync");
_LIT(KFlashSlowRearSync, 		"EFlashSlowRearSync");
_LIT(KFlashManual, 				"EFlashManual");

// CCamera::TWhiteBalance params
_LIT(KWBAuto,					"EWBAuto");
_LIT(KWBCloudy,					"EWBCloudy");
_LIT(KWBTungsten,				"EWBTungsten");
_LIT(KWBFluorescent, 			"EWBFluorescent");
_LIT(KWBFlash,					"EWBFlash");
_LIT(KWBSnow,					"EWBSnow");
_LIT(KWBBeach,					"EWBBeach");
_LIT(KWBManual,					"EWBManual");

// TCameraOrientation
_LIT(KOrientationOutwards, 		"EOrientationOutwards");
_LIT(KOrientationInwards, 		"EOrientationInwards");
_LIT(KOrientationMobile,		"EOrientationMobile");
_LIT(KOrientationUnknown,		"EOrientationUnknown");

#endif // __T_CAMERA_PARAMS_H_INCLUDED__
