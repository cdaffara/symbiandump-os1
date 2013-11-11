// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
#ifndef  ECAMADVSETTINGS_H
#define  ECAMADVSETTINGS_H

#include <e32base.h>
#include <ecam.h>
#include <ecamadvsettingsuids.hrh>
#include <gdi.h>
#include <s32strm.h>
#include <ecam/ecamconstants.h>
#include <ecamimageprocessing.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsconst.h>
#include <ecamadvsettingsdef.h>
#endif

class MCameraPresets;
class MCameraAdvancedSettings;
class MCameraAdvancedSettings2;
class MCameraAdvancedSettings3;
class MCameraAdvancedSettings4;
class MCameraPresets2;
class MCameraContinuousZoom;

/* General purpose constants */


/**
	UID used to identify the CCamera Advanced Settings API.
	This API is used to control specific individual advanced settings of camera hardware.
	These settings directly relate to the image acquisition phase.

	@see CCamera::CCameraAdvancedSettings
*/
static const TUid KECamAdvancedSettingUid 	= {KECamAdvancedSettingUidValue};

/**
	UID used to identify the CCamera Presets API.
	This API is used to simplify user - camera interaction by allowing simultaneous 
	setting of various advanced camera hardware settings using a single predefined parameter. 
	@see CCamera::CCameraPresets
*/
static const TUid  KECamPresetsUid 			= {KECamPresetsUidValue};

/** All clients receive these events, irrespective of camera index.  */
/** Camera slots in for 8 cameras. */
/** Camera with index 0 Plugged-in */
static const TUid  KUidECamEventGlobalCamera00PluggedIn = {KUidECamEventGlobalCamera00PluggedInUidValue};
/** Camera with index 1 Plugged-in */
static const TUid  KUidECamEventGlobalCamera01PluggedIn = {KUidECamEventGlobalCamera01PluggedInUidValue};
/** Camera with index 2 Plugged-in */
static const TUid  KUidECamEventGlobalCamera02PluggedIn = {KUidECamEventGlobalCamera02PluggedInUidValue};
/** Camera with index 3 Plugged-in */
static const TUid  KUidECamEventGlobalCamera03PluggedIn = {KUidECamEventGlobalCamera03PluggedInUidValue};
/** Camera with index 4 Plugged-in */
static const TUid  KUidECamEventGlobalCamera04PluggedIn = {KUidECamEventGlobalCamera04PluggedInUidValue};
/** Camera with index 5 Plugged-in */
static const TUid  KUidECamEventGlobalCamera05PluggedIn = {KUidECamEventGlobalCamera05PluggedInUidValue};
/** Camera with index 6 Plugged-in */
static const TUid  KUidECamEventGlobalCamera06PluggedIn = {KUidECamEventGlobalCamera06PluggedInUidValue};
/** Camera with index 7 Plugged-in */
static const TUid  KUidECamEventGlobalCamera07PluggedIn = {KUidECamEventGlobalCamera07PluggedInUidValue};

/** Camera slots plugged out for 8 cameras. */
/** Camera with index 0 Plugged-out */
static const TUid  KUidECamEventGlobalCamera00PluggedOut = {KUidECamEventGlobalCamera00PluggedOutUidValue};
/** Camera with index 1 Plugged-out */
static const TUid  KUidECamEventGlobalCamera01PluggedOut = {KUidECamEventGlobalCamera01PluggedOutUidValue};
/** Camera with index 2 Plugged-out */
static const TUid  KUidECamEventGlobalCamera02PluggedOut = {KUidECamEventGlobalCamera02PluggedOutUidValue};
/** Camera with index 3 Plugged-out */
static const TUid  KUidECamEventGlobalCamera03PluggedOut = {KUidECamEventGlobalCamera03PluggedOutUidValue};
/** Camera with index 4 Plugged-out */
static const TUid  KUidECamEventGlobalCamera04PluggedOut = {KUidECamEventGlobalCamera04PluggedOutUidValue};
/** Camera with index 5 Plugged-out */
static const TUid  KUidECamEventGlobalCamera05PluggedOut = {KUidECamEventGlobalCamera05PluggedOutUidValue};
/** Camera with index 6 Plugged-out */
static const TUid  KUidECamEventGlobalCamera06PluggedOut = {KUidECamEventGlobalCamera06PluggedOutUidValue};
/** Camera with index 7 Plugged-out */
static const TUid  KUidECamEventGlobalCamera07PluggedOut = {KUidECamEventGlobalCamera07PluggedOutUidValue};

/** Camera slots available for 8 cameras. */
/** Signals a camera previously has been reserved and then released again. 
    In this way available clients may attempt to reserve the camera */
/** Camera with index 0 Available. */
static const TUid  KUidECamEventGlobalCamera00Available  = {KUidECamEventGlobalCamera00AvailableUidValue};
/** Camera with index 1 Available. */
static const TUid  KUidECamEventGlobalCamera01Available  = {KUidECamEventGlobalCamera01AvailableUidValue};
/** Camera with index 2 Available. */
static const TUid  KUidECamEventGlobalCamera02Available  = {KUidECamEventGlobalCamera02AvailableUidValue};
/** Camera with index 3 Available. */
static const TUid  KUidECamEventGlobalCamera03Available  = {KUidECamEventGlobalCamera03AvailableUidValue};
/** Camera with index 4 Available. */
static const TUid  KUidECamEventGlobalCamera04Available  = {KUidECamEventGlobalCamera04AvailableUidValue};
/** Camera with index 5 Available. */
static const TUid  KUidECamEventGlobalCamera05Available  = {KUidECamEventGlobalCamera05AvailableUidValue};
/** Camera with index 6 Available. */
static const TUid  KUidECamEventGlobalCamera06Available  = {KUidECamEventGlobalCamera06AvailableUidValue};
/** Camera with index 7 Available. */
static const TUid  KUidECamEventGlobalCamera07Available  = {KUidECamEventGlobalCamera07AvailableUidValue};

/** Individual camera events. These are used by the camera to signal its observers. */
/** Lens changed. */
static const TUid  KUidECamEventLensChanged 			= {KUidECamEventLensChangedUidValue};
/** Flash source added. */
static const TUid  KUidECamEventFlashSourceAdded 		= {KUidECamEventFlashSourceAddedUidValue};
/** Flash source removed. */
static const TUid  KUidECamEventFlashSourceRemoved		= {KUidECamEventFlashSourceRemovedUidValue};
/** Image resolution changed. */
static const TUid  KUidECamEventCameraSettingImageSize 	= {KUidECamEventCameraSettingImageSizeUidValue};
/** Image Format changed. */
static const TUid  KUidECamEventCameraSettingImageFormat 		= {KUidECamEventCameraSettingImageFormatUidValue};
/** Camera orientation changed. */
static const TUid  KUidECamEventCameraSettingCameraOrientation  = {KUidECamEventCameraSettingCameraOrientationUidValue};
/** Optimal focus */
static const TUid KUidECamEventCameraSettingsOptimalFocus 		= {KUidECamEventCameraSettingsOptimalFocusUidValue};

/** Advanced settings. */
/** Stabilization mode.  */
static const TUid  KUidECamEventCameraSettingStabilizationMode 	= {KUidECamEventCameraSettingStabilizationModeUidValue};
/** Focus mode. */
static const TUid  KUidECamEventCameraSettingFocusMode 			= {KUidECamEventCameraSettingFocusModeUidValue};
/** Focus range. 
@deprecated

Replaced by KUidECamEventCameraSettingFocusRange2

In the first release of this event, KUidECamEventCameraSettingFocusRange was incorrectly given an already existing uid. 
In order to preserve compatibility this event is marked as deprecated and has been replaced by KUidECamEventCameraSettingFocusRange2.
Camera servers predating this API change will continue to send the original event which is indistinguishable by value. 
Caching the old value and comparing it with the current value, will allow the client to determine that the value has changed.
Camera servers which have been updated should send both KUidECamEventCameraSettingFocusRange and KUidECamEventCameraSettingFocusRange2 on a focus range change. 
Camera clients should therefore process both events(old and new). 

@see KUidECamEventCameraSettingFocusRange2
*/
static const TUid  KUidECamEventCameraSettingFocusRange 		= {KUidECamEventCameraSettingFocusRangeUidValue};
/** Autofocus type.
@deprecated

Replaced by KUidECamEventCameraSettingAutoFocusType2

In the first release of this event, KUidECamEventCameraSettingAutoFocusType was incorrectly given an already existing uid. 
In order to preserve compatibility this event is marked as deprecated and has been replaced by KUidECamEventCameraSettingAutoFocusType2.
Camera servers predating this API change will continue to send the original event which is indistinguishable by value. 
Caching the old value and comparing it with the current value, will allow the client to determine that the value has changed.
Camera servers which have been updated should send both KUidECamEventCameraSettingAutoFocusType and KUidECamEventCameraSettingAutoFocusType2 on a focus range change. 
Camera clients should therefore process both events(old and new). 

@see KUidECamEventCameraSettingAutoFocusType2*/
static const TUid  KUidECamEventCameraSettingAutoFocusType 		= {KUidECamEventCameraSettingAutoFocusTypeUidValue};
/** Autofocus area. */
static const TUid  KUidECamEventCameraSettingAutoFocusArea 		= {KUidECamEventCameraSettingAutoFocusAreaUidValue};
/** Autofocus lock state.  */
static const TUid  KUidECamEventCameraSettingAutoFocusLock		= {KUidECamEventCameraSettingAutoFocusLockUidValue};
/** Focus distance */
static const TUid  KUidECamEventCameraSettingFocusDistance 		= {KUidECamEventCameraSettingFocusDistanceUidValue};
/** Sensitivity - ISO rate. */
static const TUid  KUidECamEventCameraSettingIsoRate 			= {KUidECamEventCameraSettingIsoRateUidValue};
/** Aperture. */
static const TUid  KUidECamEventCameraSettingAperture 			= {KUidECamEventCameraSettingApertureUidValue};
/** Aperture range. */
static const TUid  KUidECamEventCameraSettingApertureRange 		= {KUidECamEventCameraSettingApertureRangeUidValue};
/** Shutter speed. */
static const TUid  KUidECamEventCameraSettingShutterSpeed 		= {KUidECamEventCameraSettingShutterSpeedUidValue};
/** Shutter speed range.  */
static const TUid  KUidECamEventCameraSettingShutterSpeedRange 	= {KUidECamEventCameraSettingShutterSpeedRangeUidValue};
/**  Metering mode. */
static const TUid  KUidECamEventCameraSettingMeteringMode 		= {KUidECamEventCameraSettingMeteringModeUidValue};
/** Drive mode. */
static const TUid  KUidECamEventCameraSettingDriveMode 			= {KUidECamEventCameraSettingDriveModeUidValue};
/** Braket mode. */
static const TUid  KUidECamEventCameraSettingBracketMode 		= {KUidECamEventCameraSettingBracketModeUidValue};
/** Bracket step. */
static const TUid  KUidECamEventCameraSettingBracketStep 		= {KUidECamEventCameraSettingBracketStepUidValue};
/** Bracket parameter. */
static const TUid  KUidECamEventCameraSettingBracketParameter 	= {KUidECamEventCameraSettingBracketParameterUidValue};
/** Bracket merge. */
static const TUid  KUidECamEventBracketMerge					= {KUidECamEventBracketMergeUidValue};
/** Picture orientation.  */
static const TUid  KUidECamEventCameraSettingPictureOrientation = {KUidECamEventCameraSettingPictureOrientationUidValue};
/** Automatic size selection option */
static const TUid  KUidECamEventCameraSettingAutomaticSizeSelection = {KUidECamEventCameraSettingAutomaticSizeSelectionUidValue};
/** Flash setting. */
static const TUid  KUidECamEventCameraSettingFlashMode 			= {KUidECamEventCameraSettingFlashModeUidValue};
/** Flash compensation step. */
static const TUid  KUidECamEventCameraSettingFlashCompensationStep 		= {KUidECamEventCameraSettingFlashCompensationStepUidValue};
/** Flash compensation. */
static const TUid  KUidECamEventCameraSettingFlashCompensation 	= {KUidECamEventCameraSettingFlashCompensationUidValue};
/** Manual flash power. */
static const TUid  KUidECamEventCameraSettingFlashManualPower 	= {KUidECamEventCameraSettingFlashManualPowerUidValue};
/** Red eye reduce mode. */
static const TUid  KUidECamEventCameraSettingFlashRedEyeReduce 	= {KUidECamEventCameraSettingFlashRedEyeReduceUidValue};
/** Exposure mode setting. */
static const TUid  KUidECamEventCameraSettingExposureMode 		= {KUidECamEventCameraSettingExposureModeUidValue};
/** Exposure compensation step. */
static const TUid  KUidECamEventCameraSettingExposureCompensationStep 	= {KUidECamEventCameraSettingExposureCompensationStepUidValue};
/** Exposure compensation. */
static const TUid  KUidECamEventCameraSettingExposureCompensation 		= {KUidECamEventCameraSettingExposureCompensationUidValue};
/** Exposure lock state. */
static const TUid  KUidECamEventCameraSettingExposureLock 		= {KUidECamEventCameraSettingExposureLockUidValue};
/** AE lock option. */
static const TUid  KUidECamEventAELock 							= {KUidECamEventAELockUidValue};
/** White balance.  */
static const TUid  KUidECamEventCameraSettingWhiteBalanceMode 	= {KUidECamEventCameraSettingWhiteBalanceModeUidValue};
/** Timer changed. */
static const TUid  KUidECamEventCameraSettingTimerInterval 		= {KUidECamEventCameraSettingTimerIntervalUidValue};
/** Timer lapse period.  */
static const TUid  KUidECamEventCameraSettingTimeLapse			= {KUidECamEventCameraSettingTimeLapseUidValue};
/** Optical zoom. */
static const TUid  KUidECamEventCameraSettingOpticalZoom 		= {KUidECamEventCameraSettingOpticalZoomUidValue};
/** Optical zoom range.  */
static const TUid  KUidECamEventCameraSettingOpticalZoomRange 	= {KUidECamEventCameraSettingOpticalZoomRangeUidValue};
/** Digital zoom. */
static const TUid  KUidECamEventCameraSettingDigitalZoom 		= {KUidECamEventCameraSettingDigitalZoomUidValue};
/** Digital zoom range.  */
static const TUid  KUidECamEventCameraSettingDigitalZoomRange 	= {KUidECamEventCameraSettingDigitalZoomRangeUidValue};
/** Clicking sound state.  */
static const TUid  KUidECamEventSoundClick 						= {KUidECamEventSoundClickUidValue};
/** Pixel aspect ratio. */
static const TUid  KUidECamEventPixelAspectRatio 				= {KUidECamEventPixelAspectRatioUidValue};
/** Yuv range. */
static const TUid  KUidECamEventYuvRange 						= {KUidECamEventYuvRangeUidValue};
/** Burst images. */
static const TUid  KUidECamEventBurstImages						= {KUidECamEventBurstImagesUidValue};
/** Stabilization effect - magnitude of stabilization */
static const TUid KUidECamEventCameraSettingsStabilizationEffect 		= {KUidECamEventCameraSettingsStabilizationEffectUidValue};
/** Stabilization algorithm */
static const TUid KUidECamEventSettingsStabilizationAlgorithmComplexity = {KUidECamEventSettingsStabilizationAlgorithmComplexityUidValue};
/** Continuous Autofocus timeout */
static const TUid KUidECamEventCameraSettingsContinuousAutoFocusTimeout = {KUidECamEventCameraSettingsContinuousAutoFocusTimeoutUidValue};
/** White Balance manual setting */
static const TUid KUidECamEventCameraSettingsWBValue 					= {KUidECamEventCameraSettingsWBValueUidValue};	
/** Flash ready */
static const TUid KUidECamEventFlashReady 						= {KUidECamEventFlashReadyUidValue};	
/** Flash not ready */
static const TUid KUidECamEventFlashNotReady 					= {KUidECamEventFlashNotReadyUidValue};
/** Focus range. 
This is the new UID value to notify change on focus range and takes immediate effect as previous UID KUidECamEventCameraSettingFocusRange has been deprecated.
@see KUidECamEventCameraSettingFocusRange */
static const TUid  KUidECamEventCameraSettingFocusRange2 		= {KUidECamEventCameraSettingFocusRange2UidValue};
/** Autofocus type.
This is the new UID value to notify change on autofocus type and takes immediate effect as previous UID KUidECamEventCameraSettingAutoFocusType has been deprecated.
@see KUidECamEventCameraSettingAutoFocusType */
static const TUid  KUidECamEventCameraSettingAutoFocusType2 		= {KUidECamEventCameraSettingAutoFocusType2UidValue};

/** 
ISO Rate type. 

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

*/
static const TUid  KUidECamEventCameraSettingIsoRateType 		= {KUidECamEventCameraSettingIsoRateTypeUidValue};

/** 
Reference Screen setting. 

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

*/
static const TUid  KUidECamEventCameraSettingReferenceScreen    = {KUidECamEventCameraSettingReferenceScreenUidValue};

/** 
Pre Capture Warning indication.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

*/
static const TUid  KUidECamEventCameraSettingAFAssistantLight	= {KUidECamEventCameraSettingAFAssistantLightUidValue};



class CCameraContinuousZoom;
/**	
A mixin class to be implemented by the client in order to use the Continuous Zoom API. The callbacks are invoked by the implementation
whenever the continuous zoom operations are ready to be notified.

@see CCameraContinuousZoom

@internalTechnology
@prototype
*/	
class MContinuousZoomObserver
	{	
public:
	
	/**
	This callback is sent when a new zoom factor is achieved. Client may not receive a callback for every new zoom factor as it is up
	to the implementation to choose the zoom factors for which it will issue this callback. Should an error occur, this implies that the
	continuous zoom operation has been stopped.

	@param  aContinuousZoomHandle
			Reference to CCameraContinuousZoom class object which was used to start the continuous zoom operation.

	@param  aZoomFactor
		    The new zoom factor value, multiplied by KECamFineResolutionFactor, which has been achieved.

	@param  aError
			Appropriate error code.
	*/
	virtual void ContinuousZoomProgress(CCamera::CCameraContinuousZoom& aContinuousZoomHandle, TInt aZoomFactor, TInt aError)=0;

	/**
	Implementation sends this callback when the Continuous Zoom operation has been completed. This callback will be sent when
	target zoom factor is achieved. This zoom factor may in some cases be less than the target zoom factor due to the complexity
	of the continuous zoom operation. Once this callback is received, client need not issue StopContinuousZoom() and can call
	StartContinuousZoom() again.

	@param  aContinuousZoomHandle
			Reference to CCameraContinuousZoom class object which was used to start the continuous zoom operation.

	@param  aFinalZoomFactor
			The final zoom factor value once the continuous zoom operation has completed. This zoom factor may in some cases be
			less than the target zoom factor due to the complexity of the continuous zoom operation.

	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void ContinuousZoomCompleted(CCamera::CCameraContinuousZoom& aContinuousZoomHandle, TInt aFinalZoomFactor, TInt aError)=0;

	/**
	Gets a custom interface for future callbacks. This method will be called by the implementation to get a new interface
	which would support future callbacks.

	@param  aInterface
		    The Uid of the particular interface function required for callbacks.
		   
	@param  aPtrInterface
		    The implementation has to type-cast the retrieved custom interface pointer to the appropriate type.	

	@return The error code.
	*/
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface)=0;
	};


/** 
CCamera advanced settings class exposes an API for controlling individually 
digital camera advanced settings. These settings directly relate to the 
image acquisition phase both for still images and video.

@note This class is not intended for sub-classing  and used to standardise existing
       varieties of implementations.
       
@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedAll
@released
*/
class CCamera::CCameraAdvancedSettings : public CBase
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;

public:
   
	/** Specifies camera type. */ 
	enum TCameraType
		{
		/** Unable to identify. */  
		ECameraUnknown 		= 0x00, 
		/** Camera is non-detachable from device. Camera is always present. */
		ECameraOnBoard	 	= 0x01,
		/** Camera is detachable from device. Camera is not always present. */
		ECameraPluggable 	= 0x02
		};
	
	/** Specifies stabilization mode of the camera. */
	enum TStabilizationMode
		{
		/** Not present or switched off, default */
		EStabilizationModeOff			= 0x00,
		/** Stabilization only in horizontal direction. */
		EStabilizationModeHorizontal	= 0x01,
		/** Stabilization only in vertical direction. */
		EStabilizationModeVertical		= 0x02,
		/** Rotational stabilization. */
		EStabilizationModeRotation		= 0x04,
		/** Automatic stabilization. */
		EStabilizationModeAuto			= 0x08,
		/** Manual stabilization. */
		EStabilizationModeManual		= 0x10   
		};
	
	/** Supported magnitudes of stabilization effect when in manual mode. */	
	enum TStabilizationEffect
		{
		/** Stabilization effect is off. */ 
		EStabilizationOff		= 0x00, 
		/** Stabilization effect is automatic, default. */
		EStabilizationAuto		= 0x01,
		/** Stabilization effect is very small in magnitude. */
		EStabilizationFine 		= 0x02,
		/** Stabilization effect is average in magnitude. */
		EStabilizationMedium	= 0x04,
		/** Stabilization effect is large in magnitude. */
		EStabilizationStrong	= 0x08
		};
	
	/** Supported stabilization algorithms, graded on complexity. */	
	enum TStabilizationAlgorithmComplexity
		{
		/** Stabilization algorithm selection is automatic, default. */ 
		EStabilizationComplexityAuto	= 0x00, 
		/** Stabilization algorithm is simple - less precise but fast. */
		EStabilizationComplexityLow		= 0x01,
		/** Stabilization algorithm is of medium complexity. */
		EStabilizationComplexityMedium	= 0x02,
		/** Stabilization algorithm is of high complexity. */
		EStabilizationComplexityHigh	= 0x04
		};
		
	/** Supported focus modes. */	
	enum TFocusMode
		{
		/** Focus mode is unknown. */ 
		EFocusModeUnknown	= 0x00, 
		/** Focus is automatic, default. */
		EFocusModeAuto		= 0x01,
		/** Focus is fixed. */
		EFocusModeFixed 	= 0x02,
		/** Focus is manually set. */
		EFocusModeManual	= 0x04
		};
	
	/** Supported focus ranges. */	
	enum TFocusRange
		{
		/** Auto. Default */
		EFocusRangeAuto 		= 0x00,
		/** Focus operates in close range (macro). */
		EFocusRangeMacro		= 0x01,
		/** Normal operation. */
		EFocusRangeNormal		= 0x02,
		/** Extended (tele) operation. */
		EFocusRangeTele			= 0x04,
		/** Focus at larger areas at short to medium distance. */
		EFocusRangePortrait		= 0x08,
		/** Optimised macro operation, 
		where depth of field is very shallow and observation area changes quickly. */
		EFocusRangeSuperMacro	= 0x10,
		/** 
		All objects at distances from half of the hyperfocal distance out to infinity will be in focus.
		This gives gives maximum depth of field.
		
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
		
		*/ 
		EFocusRangeHyperfocal 	= 0x20,
		/** 
		When there is a near obstacle or better focus wanted for far away objects.
		
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
		
		*/ 
		EFocusRangeInfinite		= 0x40
		};	
			
	/** Specifies the supported autofocus types. */
	enum TAutoFocusType
		{
		/** Autofocus is switched off. */
		EAutoFocusTypeOff	 		= 0x00,
		/** Operates on a single shot, consumes less power. */
		EAutoFocusTypeSingle	 	= 0x01,
		/** Continious autofocus, more precise but consumes more power. 
		    also known as AF Servo. */
		EAutoFocusTypeContinuous	= 0x02
		};
		
	/** Specifies the autofocus area. */
	enum TAutoFocusArea
		{
		/** Automatic. Default value */
		EAutoFocusTypeAuto				= 0x00,
		/** Single area, suitable for relatively flat surfaces and portrait. */
		EAutoFocusTypeSingleArea		= 0x01,
		/** Multiple points of an object, more weighting for centre points. */
		EAutoFocusTypeMultiAreaCentered	= 0x02
		};

	/** Specifies the Metering mode for the camera. 
	EMeteringModeAuto is the default value. */	
	enum TMeteringMode
		{
		/** Automatic mode, default. */	
		EMeteringModeAuto				= 0x0000, 
		/** Metering in which the center is given a weighted priority. */	
		EMeteringModeCenterWeighted		= 0x0001,
		/** Metering of a central area/spot only, also known as partial. */	
		EMeteringModeSpot				= 0x0002, 
		/** Metering is evaluated over a matrix(several spots) and calculated 
		    as a function of that. Usually best but with increased 
		    power consumption. Alternative name multi-metering mode. */	
		EMeteringModeEvaluative			= 0x0004
		};	

	/** Specifies the drive mode for the camera. This determines how and in what sucession are images 
    shot. EDriveModeSingleShot is the default.*/				
	enum TDriveMode
		{
		/** Automatic. Default */
		EDriveModeAuto				= 0x0000,
		/** Camera takes a single image/shot. */	
		EDriveModeSingleShot		= 0x0001,
		/** Camera continuously captures images (as fast as it can) until stopped or out of storage medium. */	
		EDriveModeContinuous		= 0x0002,
		/** Camera is in bracketing mode, producing individual frames. @see TBracketMode */	
		EDriveModeBracket			= 0x0004,
		/** Camera is in bracketing mode, but producing a single image. @see TBracketMode */	
		EDriveModeBracketMerge		= 0x0008,
		/** camera captures a single shot after specified time period. @see Timer() */	
		EDriveModeTimed				= 0x0010,
		/** Camera captures a set of images with an uniform interval between them. @see TimeLapse() */	
		EDriveModeTimeLapse			= 0x0020,
		/** Camera captures a set of images as fast as it can, but in batches(bursts). */	
		EDriveModeBurst				= 0x0040,
		/** Camera captures a set of images before and after camera capture key press event. Client can specify
		the amount of pre-capture and post-capture images to be saved via TDriveModeDependentAttributes.
		The total amount of images to be saved will be pre-capture images + 1 + post-capture images.

		@note This drive mode is only available to clients using the CCamera::New2L() or CCamera::NewDuplicate2L()
			  in addition to the new image capture APIs ie. CCameraPreImageCaptureControl, CCameraImageCapture and
			  CCameraPostImageCaptureControl.

		@publishedPartner
		@prototype
		*/
		EDriveModeTimeNudgeCapture	= 0x0080
		};

	/** Specifies Bracket mode. */				
	enum TBracketMode
		{
		/** Bracket mode is switched off. Default value. */	
		EBracketModeOff			= 0x0000,
		/** Bracket mode on. Three consecutive pictures are taken in order
			under (-1), on (0), over (+1), where correction magnitude 
			is defined by bracket step size and by TBracketParameter 
			@see TBracketStep
			@see TBracketParameter */	
		EBracketMode3Image		= 0x0001, 
		/** Bracket mode on. Five consecutive pictures are taken in order
			under (-2),under (-1), on (0), over (+1), over (+2), where correction magnitude 
			is defined by bracket step size and by TBracketParameter
			@see TBracketStep 
			@see TBracketParameter */	
		EBracketMode5Image		= 0x0002  
		};	

	/** Supported parameters used for bracketing.
	@note Bracket mode parameter value changes by 
	a selected uniform step between successive image shots. 
	@see TBracketStep 
	@see TBracketMode.	*/			
	enum TBracketParameter
		{
		/** None. */	
		EBracketParameterNone			= 0x0000,
		/** Exposure settings change. */	
		EBracketParameterExposure		= 0x0001,
		/** Flash power change. */		
		EBracketParameterFlashPower	    = 0x0002,
		/** Colour balance settings change. */		
		EBracketParameterColourBalance	= 0x0004,
		/** Aperture settings change. */
		BracketParameterAperture		= 0x0008, 
		/** Autofocus settings change. */ 
		BracketParameterAutoFocus		= 0x0010
		};
		
	/** Specifies the magnitude of bracketing step. 
	The actual value and availability is parameter dependent.
	@see TBracketParameter 
	*/				
	enum TBracketStep
		{
		/** Not supported. */
		EBracketStepNonConfig	= 0x00, 
		/** Small value. */	
		EBracketStepSmall		= 0x01,
		/** Medium value. */	
		EBracketStepMedium 	    = 0x02,
		/** Large value. */	
		EBracketStepLarge		= 0x04
		};

	/** Specifies the orientation of the picture. */		
	enum TPictureOrientation
		{
		/** No information about picture orientation. */	
		EPictureOrientationUnknown		= 0x00,
		/** Portrait - height larger than width. */	
		EPictureOrientationPortrait		= 0x01,
		/** Landscape width larger than height. */	
		EPictureOrientationLandscape	= 0x02,
		/** Square width equals the height. */	
		EPictureOrientationSquare		= 0x04
		};	

	/** Specifies the pixel aspect ratio  
	@note It is specified as a fraction of  
	(x) horizontal pixel size divided by vertical (y) pixel size. 
	The pixel aspect ratio for square pixels is 1/1. 
	*/		
	enum TPixelAspectRatio
		{
		/** Pixel ratio undetermined */
		EPixelAspectUnknown		= 0x00,
		/** Pixel Ratio 1:1. */	
		EPixelAspect1To1		= 0x01,
		/** Pixel Ratio 12:11 */	
		EPixelAspect12To11		= 0x02,
		/** Pixel Ratio 11:10. */	
		EEPixelAspect11To10		= 0x04,
		/** Pixel Ratio 59:54. */	
		EEPixelAspect59To54		= 0x08,
		/** Pixel Ratio 16:11.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/	
		EEPixelAspect16To11		= 0x10,
		/** Pixel Ratio 10:11.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/	
		EEPixelAspect10To11		= 0x20,		
		/** Pixel Ratio 40:33. 
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/	
		EEPixelAspect40To33		= 0x40
		};
	
	/** Specifies YUV colour space dynamic range. 
	@note video compressors often use narrower than the default range.
	The nominal Y range is [16:235] and the U and V ranges [16:240]. 
	*/		
	enum TYuvRange
		{
		/** Yuv range undetermined */
		EYuvRangeUnknown		= 0x00,
		/** Yuv Full Range. The nominal Y,U and V colours range is [0:255].  */	
		EYuvRangeFull			= 0x01,
		/** Yuv Cropped Range. The nominal Y range is [16:235] and the U and V ranges [16:240]. */	
		EYuvRangeVideoCropped	= 0x02
		};
	
	/** Specifies the units supported by the camera for manual white balance setting. 
	 */				
	enum TWBUnits
		{
		/** The units type is undetermined or not supported. */ 
		EWBUnknown,
		/** The white balance is represented as temperature degrees in Kelvin. */   	
		EWBColorTemperature,
		/** The white balance is represented as RGB triples.  */ 
		EWBRgb
		};
		
	/** 
	Specifies the ISO type supported by the camera. ISO refers to the sensivity of the 
	image sensor and is one of the factors influencing the exposure.
	
	*/		
	enum TISORateType
		{
		/** ISO Not supported. Camera uses a fixed ISO rate internally and never reveals this information. */
		EISONone						 = 0x00,
		/** Camera gives a list of manual ISO rates to the user to set. Recommendation is to start camera in the manual ISO mode, by default. */
		EISOManual 						 = 0x01,
		/** Camera chooses the ISO on its own without prioritising any particular exposure parameters. */
		EISOAutoUnPrioritised 			 = 0x02,	
		/** Camera chooses the ISO on its own by prioritising ISO. */
		EISOAutoISOPrioritised 		 	 = 0x04,	
		/** Camera chooses the ISO on its own after prioritising shutter speed. */
		EISOAutoShutterSpeedPrioritised  = 0x08,
		/** Camera chooses the ISO on its own after prioritising aperture opening. */	
		EISOAutoAperturePrioritised 	 = 0x10	
		};
			  	
	/** 
	Specifies the unfavourable circumstances as pre capture warnings which may be issued before image/video capture. 
	
	*/	
	enum TPreCaptureWarning
		{
		/** No warnings */
		EPCWNone 						= 0x0000,
		/** warning to indicate that camera sensor is over exposed */
		EPCWOverExposure 				= 0x0001,
		/** warning to indicate that camera sensor is under exposed */
		EPCWUnderExposure 				= 0x0002,
		/** warning to indicate that camera operates under insufficient lightning conditions */
		EPCWLowLight 					= 0x0004,
		/** warning to indicate that focussing is not optimum. This is valid for manual/fixed focus only */
		EPCWBadFocus 					= 0x0008, 
		/** warning to indicate that camera might not be firmly gripped (Tripod required). This is same as long exposure */ 
		EPCWHandShakePossible 			= 0x0010,  
		/** warning to indicate that camera has not enough power to survive the flash operations (battery discharged) */
		EPCWNotEnoughPowerForFlash 		= 0x0020,
		/** warning to indicate that the current white balance is not the desired one */
		EPCWWrongWhiteBalanceValue 		= 0x0040,
		/** warning to indicate that the current flash value is not the desired one */
		EPCWWrongFlashValue 			= 0x0080,
		/** warning to indicate that the current digital zoom value is not the desired one */
		EPCWWrongDigiZoomValue 			= 0x0100,
		/** warning to indicate that the desired digital zoom affects the quality of image */
		EPCWDigiZoomBadQuality        	= 0x0200, 
		/** warning to indicate that flash is not ready. Recommendation: user may subscribe for this warning or issue 
		IsFlashReady() also. Using both options is not encouraged */
		EPCWFlashNotReady				= 0x0400,
		/** unknown reason */
		EPCWGeneralWarning 				= 0x0800,
		/** warning to indicate that flash has been overheated.
		
		@note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		
		@publishedPartner
		@prototype
		*/
		EPCWFlashOverheated				= 0x1000,
		/** warning to indicate that flash will not be fired because of enough ambient light.
		
		@note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		
		@publishedPartner
		@prototype
		*/
		EPCWFlashNotNeeded				= 0x2000
		};
		
	/**
	Describes the camera mode.
	
	*/	
	enum TCameraMode
		{
		/** Camera is Idle */
		EModeIdle				= 0x00,
		/** DSA based Viewfinder runs */
		EModeDSAViewFinder		= 0x01,
		/** Client based Viewfinder runs */
		EModeClientViewFinder	= 0x02		
		};
		
	/** 
	Specifies the different types of auto focus assistant light	
	*/
	enum TAFAssistantLight
		{
		/** switch off the AF assistant light from either of 'manual on' or 'auto' mode */
		EAFAssistantLightOff 			= 0x00, 
		/** switch on the AF assistant light manually */
		EAFAssistantLightManualOn 		= 0x01,
		/** Put the AF assistant light in auto mode. It will be automatically decided when to switch on or off. */
		EAFAssistantLightAuto 			= 0x02
		};
	
	/**
	Specifies the different types of continuous zoom supported.
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/
	enum TContinuousZoomType
		{
		/** Continuous zoom not supported */
		EContinuousZoomNotSupported		= 0x00,
		/** Continuous zoom 'Optical' only, No 'Digital' */
		EContinuousZoomOpticalOnly		= 0x01,
		/** Continuous zoom 'Optical' and 'Digital' */
		EContinuousZoomMixed			= 0x02,
		/** Continuous zoom 'Digital' only, No 'Optical' */
		EContinuousZoomDigitalOnly		= 0x04
		};
	
	/**
	Specifies the zoom direction for continuous zoom operation. 
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/	
	enum TZoomDirection
		{
		/** Implies that feature not supported. */
		EZoomDirectionNone		=0x00,
		/** Implies zooming out. */
		EZoomDirectionWide		=0x01,
		/** Implies zooming in. */
		EZoomDirectionTele		=0x02
		};
	
	/**
	Different levels for performance vectors like speed, quality, low memory consumption and low power consumption.
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/	
	enum TPerformanceLevel
		{
		/** High level preference. */
		ELevelHigh			= 1,
		/** Medium level preference. */
		ELevelMedium		= 2,
		/** Low level preference. */
		ELevelLow			= 3,
		/** Dont care (No preference). */
		ELevelDontCare		= 4
		};
	
	/**
	Specifies flicker removal options.
	The enumeration list may be extended in future. 
	
	@internalTechnology
	*/
	enum TFlickerRemoval
		{
		/** Not Supported. */
		EFlickerRemovalNone  		= 0x00,
		/** Flicker removal for Field frequency of 50Hz. */
		EFlickerRemoval50Hz 		= 0x01,
		/** Flicker removal for Field frequency of 60Hz. */
		EFlickerRemoval60Hz			= 0x02,
		/** Auto flicker removal. */
		EFlickerRemovalAuto 		= 0x04,
		/** Switch Off flicker removal. */
		EFlickerRemovalSwitchOff   	= 0x08
		};
	
	/**
	Specifes the type of neutral density filters available. 
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TNDFilter
		{
		/** Not Supported. */
		ENDFilterNone  			= 0x00,
		/** Auto ND Filter. */
		ENDFilterAuto	  		= 0x01,
		/** Switched off ND Filter. */
		ENDFilterSwitchedOff	= 0x02,
		/** High level ND Filter. */
		ENDFilterHigh	 		= 0x04,
		/** Medium level ND Filter. */
		ENDFilterMedium	   		= 0x08,
		/** Low level ND Filter. */
		ENDFilterLow			= 0x10,
		/** General ND Filter. */
		ENDFilterGeneral		= 0x20
		};
	
	/**
	Specifies the various events for which the LED effects could be used.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TLEDEvent
		{
		/** Not supported. */
		ELEDEventNone 				= 0x00,
		/** LED effect needed for self timer. */
		ELEDEventSelfTimer 			= 0x01,
		/** LED effect needed for starting of viewfinder. */
		ELEDEventViewfinderStart 	= 0x02,
		/** LED effect needed for stoping of viewfinder. */
		ELEDEventViewfinderStop		= 0x04,
		/** LED effect needed for still image capture. */
		ELEDEventStillImageCapture	= 0x08,
		/** LED effect needed for video recording. */
		ELEDEventVideoRecording		= 0x10
		};
	
	/**
	Specifes the type of LED effects available. 
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TLEDEffect
		{
		/** Not supported. */
		ELEDEffectNone				=0x00,
		/** LED always on. */
		ELEDEffectHardwired			=0x01,
		/** LED auto. */
		ELEDEffectAuto				=0x02,
		/** LED Effect manually switched on. */
		ELEDEffectManualSwitchOn	=0x04,
		/** LED Effect switched off. */
		ELEDEffectSwitchOff			=0x08,
		/** LED Custom Effect. */
		ELEDEffectCustom			=0x10
		};
	
	/**
	Color channels on which manual gain can be selectively applied.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/	
	enum TColorChannel
		{
		/** Red Color Channel (RGB model). */
		EColorChannelRed,
		/** Green Color Channel (RGB model). */
		EColorChannelGreen,
		/** Blue Color Channel (RGB model). */
		EColorChannelBlue,
		/** Luminance Y component (YUV model). */ 
		EChannelLumaY,
		/** Chrominance U component (YUV model). */ 
		EChannelChromaU,
		/** Chrominance V component (YUV model). */ 
		EChannelChromaV
		};
	
	/**
	Class used to provide a particular custom LED effect.
	
	@internalTechnology
	*/
	class TECamLEDSpecialEffectStep
	{
	public:
		TECamLEDSpecialEffectStep();
		
		TUint Size() const;
		TUint Version() const;
		
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
				
		// reserved for future expansion.
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;
		
	public:
		/** Blinking frequency of LED in Hertz. */
		TUint iBlinkingFrequency;
		/** Represents the intensity of LED in millicandela. */
		TUint iIntensity;
		/** Frequency step with which the blinking frequency changes with time. May be positive as well as negative. */
		TInt iFrequencyStep;
		/** Time duration for which the particular LED special effect step would be valid. */
		TTimeIntervalMicroSeconds32 iDuration;
	};

	/**
	Class used to provide supported continuous zoom information.
	
	@internalTechnology
	@prototype
	*/
	class TContinuousZoomSupportInfo
		{
	public:
		IMPORT_C TContinuousZoomSupportInfo();

		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;

	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;

		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;

	public:
		/** Maximum continuous zoom speed supported. Minimum speed is 0. */
		TInt iMaxSpeedSupported;
		/**  Minimum continuous zoom acceleration supported. A negative value signifies deceleration. */
		TInt iMinAccelerationSupported;
		/**  Maximum continuous zoom acceleration supported. */
		TInt iMaxAccelerationSupported;
		/** Minimum continuous zoom value. For digital zoom, this could be 0 unless viewfinder frames are not cropped and scaled up by default.
		Represented as a concrete value multiplied by KECamFineResolutionFactor. */
		TInt iContinuousZoomMinLimit;
		/** Maximum continuous zoom value. Represented as a concrete value multiplied by KECamFineResolutionFactor. */
		TInt iContinuousZoomMaxLimit;
		};

	/**
	Class used to provide essential parameters for continuous zoom operation.
	
	@internalTechnology
	@prototype
	*/
	class TContinuousZoomParameters
		{
	public:
		IMPORT_C TContinuousZoomParameters();

		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;

	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;

		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;

	public:
		/** Type of continuous zoom to be used. */
		TContinuousZoomType iContinuousZoomType;
		/**  Zoom direction to be used. */
		TZoomDirection iZoomDirection;
		/**  Continuous zoom speed to be used. Represented as a concrete value multiplied by KECamFineResolutionFactor. */
		TInt iContinuousZoomSpeed;
		/** Continuous zoom acceleration to be used. Represented as a concrete value multiplied by KECamFineResolutionFactor. */
		TInt iContinuousZoomAcceleration;
		/** Limit of continuous zoom range. Represented as a concrete value multiplied by KECamFineResolutionFactor. */
		TInt iContinuousZoomLimit;
		};

public:

    IMPORT_C static CCameraAdvancedSettings* NewL(CCamera& aCamera);

  	IMPORT_C TCameraType CameraType() const;
  	
	IMPORT_C TCameraType CameraType(TInt aCameraIndex);

	IMPORT_C TBool IsCameraPresent() const;
	
   	IMPORT_C TBool IsCameraPresent(TInt aCameraIndex);

  	IMPORT_C TInt CameraIndex() const;	
  	
    IMPORT_C TInt SupportedStabilizationModes() const;
    
    IMPORT_C TStabilizationMode StabilizationMode() const;
    
    IMPORT_C void SetStabilizationMode(TStabilizationMode aStabilizationMode);
    
    IMPORT_C TInt SupportedFocusModes() const;
    
    IMPORT_C TFocusMode FocusMode() const;
    
    IMPORT_C void SetFocusMode(TFocusMode aFocusMode);
    
    IMPORT_C TInt SupportedFocusRanges() const;
    
    IMPORT_C TFocusRange FocusRange() const;
    
    IMPORT_C void SetFocusRange(TFocusRange aFocusRange);
    
    IMPORT_C TInt SupportedAutoFocusTypes() const;
    
    IMPORT_C TAutoFocusType AutoFocusType() const;
    
    IMPORT_C void SetAutoFocusType(TAutoFocusType aAutoFocusType);

    IMPORT_C TInt SupportedAutoFocusAreas() const;
    
    IMPORT_C TAutoFocusArea AutoFocusArea() const;
    
    IMPORT_C void SetAutoFocusArea(TAutoFocusArea aAutoFocusArea);

	IMPORT_C TInt FocusDistance() const;
	
	IMPORT_C void SetFocusDistance(TInt aDistance);

	IMPORT_C TInt GetMinFocalLength() const;
 
    IMPORT_C void GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const;
    
    IMPORT_C TInt IsoRate() const;
    
    IMPORT_C void SetIsoRate(TInt aRate);

    IMPORT_C void GetAperturesL(RArray<TInt>& aFStops, TValueInfo& aInfo) const;
    
    IMPORT_C TInt Aperture() const; 
    
    IMPORT_C void SetAperture(TInt aFStop); 
	
	IMPORT_C void GetShutterSpeedsL(RArray<TInt>& aShutterSpeeds, TValueInfo& aInfo) const; 
		
    IMPORT_C TInt ShutterSpeed() const; 
    
    IMPORT_C void SetShutterSpeed(TInt aShutterSpeed); 
    
    IMPORT_C TInt SupportedMeteringModes() const;
    
    IMPORT_C TMeteringMode MeteringMode() const;
    
    IMPORT_C void SetMeteringMode(TMeteringMode aMeteringMode);
    
    IMPORT_C TInt SupportedDriveModes() const;
    
    IMPORT_C TDriveMode DriveMode() const;
    
    IMPORT_C void SetDriveMode(TDriveMode aDriveMode);

    IMPORT_C TInt SupportedBracketModes() const;
    
    IMPORT_C TBracketMode BracketMode() const;
    
    IMPORT_C void SetBracketMode(TBracketMode aBracketMode);
    
    IMPORT_C TInt SupportedBracketParameters() const;
    
    IMPORT_C TBracketParameter BracketParameter() const;
    
    IMPORT_C void SetBracketParameter(TBracketParameter aBracketParameter);
    
    IMPORT_C TInt SupportedBracketSteps() const;
    
    IMPORT_C TBracketStep BracketStep() const;
    
    IMPORT_C void SetBracketStep(TBracketStep aBracketStep);
    
	IMPORT_C void GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const;
	
	IMPORT_C void SetBracketMerge(TInt aStartIndex, TInt aFrames);
    
    IMPORT_C TInt SupportedFlashModes() const;
    
	IMPORT_C CCamera::TFlash FlashMode() const;

	IMPORT_C void SetFlashMode(CCamera::TFlash aMode);

    IMPORT_C TBool RedEyeReduceOn() const;
    
    IMPORT_C void SetRedEyeReduceOn(TBool aState);
    
	IMPORT_C void GetFlashCompensationStepsL(RArray<TInt>& aFlashCompensationSteps, TValueInfo& aInfo) const; 
	
	IMPORT_C TInt FlashCompensationStep() const;
	
	IMPORT_C TInt GetFlashCompensationStep(TInt& aFlashCompensationStep) const;	
	
	IMPORT_C void SetFlashCompensationStep(TInt aFlashCompensationStep);
	
	IMPORT_C void GetFlashCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const;

	IMPORT_C TInt FlashCompensation() const;
	
	IMPORT_C TInt GetFlashCompensation(TInt& aFlashCompensation) const;
	
	IMPORT_C void SetFlashCompensation(TInt aFlashCompensationInSteps);
	
	IMPORT_C TBool IsExternalFlashPresent() const;
	   
    IMPORT_C void GetManualFlashPowerLevelsL(RArray<TInt>& aManualFlashPowerLevels, TValueInfo& aInfo) const;
    
    IMPORT_C TInt ManualFlashPowerLevel() const;

    IMPORT_C void SetManualFlashPowerLevel(TInt aManualFlashPowerLevel);
    
	IMPORT_C TInt SupportedExposureModes() const;
	
	IMPORT_C CCamera::TExposure ExposureMode() const;

	IMPORT_C void SetExposureMode(CCamera::TExposure aExposureMode);
	
	IMPORT_C void GetExposureCompensationStepsL(RArray<TInt>& aExposureCompensationSteps, TValueInfo& aInfo) const; 
	
	IMPORT_C TInt ExposureCompensationStep() const;
	
	IMPORT_C TInt GetExposureCompensationStep(TInt& aExposureCompensationStep) const;
	
	IMPORT_C void SetExposureCompensationStep(TInt aExposureCompensationStep);
	
	IMPORT_C void GetExposureCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const;

	IMPORT_C TInt ExposureCompensation() const;
	
	IMPORT_C TInt GetExposureCompensation(TInt& aExposureCompensation) const;
	
	IMPORT_C void SetExposureCompensation(TInt aExposureCompensationInSteps);

	IMPORT_C TInt SupportedWhiteBalanceModes() const;
	
	IMPORT_C CCamera::TWhiteBalance WhiteBalanceMode() const;
	
	IMPORT_C void SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode);

	IMPORT_C TBool ApertureExposureLockOn() const;
	
	IMPORT_C void SetApertureExposureLockOn(TBool aAELock);

	IMPORT_C TBool ShootClickOn() const;
	
	IMPORT_C void SetShootClickOn(TBool aShootClickOn);
	
	IMPORT_C void GetTimerIntervalsL(RArray<TInt>& aTimerIntervals, TValueInfo& aInfo) const;
	
    IMPORT_C TInt TimerInterval() const;  
    
    IMPORT_C void SetTimerInterval(TInt aTimerInterval); 

	IMPORT_C void GetTimeLapsePeriodRange(TTime& aTimeLapseMin, TTime& aTimeLapseMax) const;
	
	IMPORT_C void GetTimeLapse(TTime& aStart, TTime& aEnd, TTime& aInterval) const;  
    
    IMPORT_C void SetTimeLapse(const TTime& aStart, const TTime& aEnd, const TTime& aInterval); 
    
    IMPORT_C TPictureOrientation PictureOrientation() const;
    
    IMPORT_C void SetPictureOrientation(TPictureOrientation aOrientation); 

    IMPORT_C TInt SupportedPixelAspectRatios() const;

    IMPORT_C TPixelAspectRatio PixelAspectRatio() const;
    
    IMPORT_C void SetPixelAspectRatio(TPixelAspectRatio aPixelAspectRatio); 

    IMPORT_C TInt SupportedYuvRanges() const;
    
    IMPORT_C TYuvRange YuvRange() const;
    
    IMPORT_C void SetYuvRange(TYuvRange aYuvRange);
	
    IMPORT_C TInt BurstImages() const;
    
    IMPORT_C void SetBurstImages(TInt aImages);	

	IMPORT_C void GetOpticalZoomStepsL(RArray<TInt>& aOpticalZoomSteps, TValueInfo& aInfo) const;
	
	IMPORT_C TInt OpticalZoom() const;
	
	IMPORT_C void SetOpticalZoom(TInt aOpticalZoom);

	IMPORT_C void GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo) const;
	
	IMPORT_C TInt DigitalZoom() const;
	
	IMPORT_C void SetDigitalZoom(TInt aDigitalZoom);

	IMPORT_C TBool ExposureLockOn() const;
	
	IMPORT_C void SetExposureLockOn(TBool aState);
		
	IMPORT_C TBool AutoFocusLockOn() const;
	
	IMPORT_C void SetAutoFocusLockOn(TBool aState);
		
    IMPORT_C void GetSupportedSettingsL(RArray<TUid>& aSettings) const;
    
    IMPORT_C void GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
    
    IMPORT_C void GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;
	
	IMPORT_C TBool AutomaticSizeSelectionChangeOn() const;	

	IMPORT_C void SetAutomaticSizeSelectionChangeOn(TBool aSetOn);		

    IMPORT_C void GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& aTimeouts, TValueInfo& aInfo) const;
    
    IMPORT_C TInt ContinuousAutoFocusTimeout() const; 
       
    IMPORT_C void SetContinuousAutoFocusTimeout(TInt aTimeout);
   
	IMPORT_C TInt SupportedStabilizationEffects() const;
	
	IMPORT_C TStabilizationEffect StabilizationEffect() const;
    
    IMPORT_C void SetStabilizationEffect(TStabilizationEffect aEffect);

	IMPORT_C TInt SupportedStabilizationComplexityValues() const;
    
	IMPORT_C TStabilizationAlgorithmComplexity StabilizationComplexity() const;
    
	IMPORT_C void SetStabilizationComplexity(TStabilizationAlgorithmComplexity aComplexity);

    IMPORT_C TWBUnits SupportedWBUnits() const;
    
	IMPORT_C void GetWBRgbValue(TRgb& aValue) const;
    
    IMPORT_C void SetWBRgbValue(const TRgb& aValue);
    
	IMPORT_C void GetWBSupportedColorTemperaturesL(RArray<TInt>& aWBColorTemperatures, TValueInfo& aInfo) const;
    
 	IMPORT_C TInt WBColorTemperature() const;   
 	
	IMPORT_C void SetWBColorTemperature(TInt aWBColorTemperature);
	
	IMPORT_C ~CCameraAdvancedSettings();
	
 	IMPORT_C TInt IsFlashReady(TBool& aReady) const;
	
	IMPORT_C void GetCurrentFocusModeStepsL(RArray<TInt>& aFocusModeSteps, TValueInfo& aInfo) const;
	
	IMPORT_C void GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const;
	
	IMPORT_C void SetISORateL(TISORateType aISORateType, TInt aParam);
	
	IMPORT_C void GetISORateL(TISORateType& aISORateType, TInt& aParam, TInt& aISORate) const;
	
	IMPORT_C void SetReferenceScreenL(CWsScreenDevice& aScreenDevice);
	
	IMPORT_C void GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aSizeIndex, 
												CCamera::TFormat aFormat, TBool& aIsInfluencePossible) const;
	
	IMPORT_C void GetDigitalZoomStepsForVideoL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aFrameRateIndex, 
			TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible, CCamera::TExposure aExposure) const;
	
	IMPORT_C void GetPreCaptureWarningSupportedL(TCameraMode aCameraMode, TInt& aPreCaptureWarningSupported) const;
	
	IMPORT_C void SubscribeToPreCaptureWarningL(TInt aPreCaptureWarning);
	
	IMPORT_C void UnSubscribePreCaptureWarningL();
	
	IMPORT_C void GetPreCaptureWarningL(TInt& aPreCaptureWarning) const;
	
	IMPORT_C void GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const;
			 
	IMPORT_C void GetAFAssistantLightL(TAFAssistantLight& aAFAssistantLight) const;
	
	IMPORT_C void SetAFAssistantLightL(TAFAssistantLight aAFAssistantLight); 
	
	IMPORT_C void GetSupportedContinuousZoomTypeL(TUint& aSupportedContinuousZoomType) const;
	
	IMPORT_C void GetFocalLengthInfoL(TInt& aMinFocalLength, TInt& aCurrentFocalLength, TInt& aMaxFocalLength) const;
	
	IMPORT_C void GetNumOperationPreferenceL(TUint& aNumOperationPreferenceSupported) const;
	
	IMPORT_C void EnumerateOperationPreferenceL(TUint aOperationPreferenceIndex, TPerformanceLevel& aSpeedLevel, 
											TPerformanceLevel& aQualityLevel, TPerformanceLevel& aLowMemoryConsumptionLevel, 
											TPerformanceLevel& aLowPowerConsumptionLevel) const;
											
	IMPORT_C void SetOperationPreferenceL(TUint aOperationPreferenceIndex);
	
	IMPORT_C void GetOperationPreferenceL(TInt& aOperationPreferenceIndex) const;
	
	IMPORT_C void GetSupportedEventsL(RArray<TUid>& aSupportedEvents) const;
	
	IMPORT_C void GetIndirectFeatureChangesL(TUid aRequestedSetting, RArray<TUid>& aIndirectFeatureChanges) const;
	
	IMPORT_C void CreateContinuousZoomL(MContinuousZoomObserver& aObserver, TContinuousZoomType aContinuousZoomType, CCameraContinuousZoom*& aContinuousZoom);

private:
	IMPORT_C CCameraAdvancedSettings(CCamera& aOwner);
	IMPORT_C void ConstructL();
	
private:
	enum TECAMEventFilterScheme
		{
		/** Black listing will mean not to receive specific events */
		EECAMEventFilterSchemeBlackList, 
		/** White listing will mean to receive only specific events */
		EECAMEventFilterSchemeWhiteList
		};

	void RegisterEventsL(TECAMEventFilterScheme aEventFilter, const RArray<TUid>& aEvents);
	void GetRegisterEventsL(TECAMEventFilterScheme aEventFilter, RArray<TUid>& aEvents, TValueInfo& aInfo) const;
	
	void GetSupportedFlickerRemovalValueL(TUint& aSupportedFlickerRemovalValue) const;
	void GetFlickerRemovalValueL(TFlickerRemoval& aFlickerRemovalValue) const;
	void SetFlickerRemovalValueL(TFlickerRemoval aFlickerRemovalValue);
	
	void GetSupportedNDFilterL(TUint& aSupportedNDFilter) const;
	void GetNDFilterL(TNDFilter& aNDFilter) const;
	void SetNDFilterL(TNDFilter aNDFilter);
	
	void GetLEDEffectL(TLEDEvent aLEDEvent, TLEDEffect& aLEDEffect) const;
	void SetLEDEffectL(TLEDEvent aLEDEvent, TLEDEffect aLEDEffect);
	void SetLEDCustomEffectL(TLEDEvent aLEDEvent, const RArray<TECamLEDSpecialEffectStep>& aLEDSpecialEffectSteps);

	void ReserveL(const TTimeIntervalMicroSeconds32& aMaxTimeToWait, TBool aKickOut);
	void SetClientPriorityL(TInt aPriority);
	void RestoreClientPriorityL();
	
	void GetSupportedManualGainL(RArray<TInt>& aSupportedManualGain, TColorChannel aColorChannel) const;
	void GetManualGainL(TInt& aManualGain, TColorChannel aColorChannel) const;
	void SetManualGainL(TInt aManualGain, TColorChannel aColorChannel);

	void SetWhiteBalanceLockL(TBool aEnableLock);
	void GetWhiteBalanceLockStateL(TBool& aIsLocked) const;
	
	void EnableStateSavingL(RWriteStream& aLatestCameraState);
	void DisableStateSavingL();
	void RestoreLatestStateL(RReadStream& aLatestCameraState);
	void SaveCameraStateL(RWriteStream& aCustomCameraState);
	void DisableCameraStateSavingL(RWriteStream& aCustomCameraState);
	void RestoreCameraStateL(RReadStream& aCustomCameraState);

private:	
	CCamera&        			iOwner; 
    MCameraAdvancedSettings* 	iImpl;   // not owned
    MCameraAdvancedSettings2* 	iImpl2;  // not owned
    
    MCameraAdvancedSettings3* 	iImpl3;  // not owned
    
    MCameraAdvancedSettings4* 	iImpl4;  // not owned
	};		

/**
Supported presets with the assumption for scope and outcome
*/

/** Default settings */
static const TUid  KUidECamPresetFactoryDefault	= {KUidECamPresetFactoryDefaultUidValue};
/** Daytime shot with normal exposure time, one shot autofocus, 
medium ISO rating and no flash. */
static const TUid  KUidECamPresetOutdoor		= {KUidECamPresetOutdoorUidValue};
/** Daytime shot with short exposure time, continuous autofocus, 
high Iso rating and no flash. */
static const TUid  KUidECamPresetOutdoorSport	= {KUidECamPresetOutdoorSportUidValue};
/** Daytime shot with very short exposure time, high shutter speed, 
high Iso rating and no flash. */
static const TUid  KUidECamPresetSnow 			= {KUidECamPresetSnowUidValue};
/** Daytime shot with very short exposure time, high shutter speed, 
high contrast, reflections and no flash. */
static const TUid  KUidECamPresetBeach 			= {KUidECamPresetBeachUidValue};
/** Night time shot with long central exposure time and high flash power. */
static const TUid  KUidECamPresetNightPortrait 	= {KUidECamPresetNightPortraitUidValue};
/** Night time shot with long exposure time and no flash. */
static const TUid  KUidECamPresetNightFireworks = {KUidECamPresetNightFireworksUidValue};
/** Daytime shot with medium exposure time, medium to high Iso rating, 
overcast daylight, low contrast, fill-in flash. */
static const TUid  KUidECamPresetFog 			= {KUidECamPresetFogUidValue};
/** Fluorescent lightning with medium exposure time, 
medium to low shutter speed with flash. */
static const TUid  KUidECamPresetIndoor			= {KUidECamPresetIndoorUidValue};
/** Fluorescent lightning with continuous autofocus, wide aperture, 
high Iso rating with flash. */
static const TUid  KUidECamPresetIndoorSport	= {KUidECamPresetIndoorSportUidValue};
/** 
Used in low light situations. Tries to capture all the photons from a low light scene. 
The image may be processed longer to achieve this. This case may cause blurring, but that could be a 
wanted effect. 

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetNightPartyIndoor	= {KUidECamPresetNightPartyIndoorUidValue};
/** 
Used to capture images of things which can't be seen by eye. 
For example: snowflakes, underwater photography, small insect photos

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetNightCloseUp		= {KUidECamPresetNightCloseUpUidValue};
/** 
Used for revealing assumed details. For example: business card photos, small written text photos, 
underwater photography, insect photos.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetNightMacro			= {KUidECamPresetNightMacroUidValue};
/** 
Used for taking clear pictures of text or drawings. Also used to capture business cards or of a whiteboard. 
Aim is to optimise text and drawing readability and user expectation of "white" paper. 
Camera shake may reduce quality in case no flash available or not adequate(distance too much from subject).

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetTextDocument		= {KUidECamPresetTextDocumentUidValue};
/** 
Used for providing clear pictures of barcode. Aim is to convert barcode into some form which can be easily decoded. 
Camera shake may reduce this barcode readability and hence, barcode conversion accuracy and reliability.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetBarcode			= {KUidECamPresetBarcodeUidValue};
/** 
Used when factory default preset in certain cases may not be available or is too restrictive. 
Automatic preset expects the hardware to use automatic detection for most of the features.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetAuto	 			= {KUidECamPresetAutoUidValue};
/** 
Used for portraits of human subjects in good lighting conditions. Focuses on the people and captures natural 
skin tones. Background details may not be softened.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetPortrait 			= {KUidECamPresetPortraitUidValue};
/** 
Used for sharp landscape shots with clear outlines, colours and contrast, for example forest, sky. 
The main aim is to get the details of the scene.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetLandscape			= {KUidECamPresetLandscapeUidValue};
/** 
Used to retain the ambience/mood produced by, for example, warm lighting conditions like sunset, candlelight etc.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

*/
static const TUid  KUidECamPresetAmbienceMood		= {KUidECamPresetAmbienceMoodUidValue};
 
/**
This API is used to simplify user - camera interaction by allowing simultaneous
setting of various advanced camera hardware settings using a single parameter.

Preset is identified by a single UID and relates to a known predefined outcome. 
For example the 'Night' Preset will be used to set the camera into a night mode 
so that the user can take night photos. 

The particular set of settings associated with the specific preset and their specific values 
and ranges are camera hardware specific and outside the scope of this API. 

@note This class is not intended for sub-classing  and used to standardise existing
      varieties of implementations.
      
@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedAll
@released
*/
class CCamera::CCameraPresets : public CBase 
	{
    /** allow access to private constructors. */
	friend class CCamera;
	
public:

    IMPORT_C static CCameraPresets* NewL(CCamera& aCamera);
    
    IMPORT_C void GetSupportedPresetsL(RArray<TUid>& aPresets) const;
    
    IMPORT_C void SetPreset(TUid aPreset);
    
    IMPORT_C TUid Preset() const;
    
    IMPORT_C void GetAffectedSettingsL(RArray<TUid>& aSettings) const;
   
    IMPORT_C void GetAssociatedSettingsL(TUid aPreset, RArray<TUid>& aSettings) const;
    
    IMPORT_C ~CCameraPresets();
    
    IMPORT_C void GetRangeRestrictedSettingsL(RArray<TUid>& aRangeRestrictedSettings) const;
    
    IMPORT_C void GetFeatureRestrictedSettingsL(RArray<TUid>& aFeatureRestrictedSettings) const;
    
    IMPORT_C void IsPresetUnlockSupportedL(TBool& aUnlockSupported) const;
	
	IMPORT_C void LockPresetL();
	
	IMPORT_C void UnlockPresetL();
	    
private:
	IMPORT_C CCameraPresets(CCamera& aOwner);
	IMPORT_C void ConstructL();
	
	void FilterUnrecognisedUids(RArray<TUid>& aUids, const TInt aBaselineUid) const;

private:	
	CCamera&        iOwner; 
    MCameraPresets* iImpl;  // not owned
    MCameraPresets2* iImpl2;  // not owned
	};


/**
This API is used to provide advanced continuous zoom support to the user.

This class is not directly created by the client but instead created via 
CCameraAdvancedSettings::CreateContinuousZoomL(). Ownership of the object
is passed back to the client.

@note  This class is not intended for sub-classing and used to standardise existing
       varieties of implementations.
      
@note  If the class methods leave, the output type parameter value is not guaranteed to be valid.

@internalTechnology
@prototype
*/
class CCamera::CCameraContinuousZoom : public CBase
	{
	friend void CCamera::CCameraAdvancedSettings::CreateContinuousZoomL(MContinuousZoomObserver& aObserver, TContinuousZoomType aContinuousZoomType, CCameraContinuousZoom*& aContinuousZoom);

public:
	IMPORT_C void StartContinuousZoomL(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters aContinuousZoomParameters);

	IMPORT_C void StopContinuousZoom();

	IMPORT_C void GetContinuousZoomSupportInfoL(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo& aContinuousZoomInfo) const;

	IMPORT_C void GetContinuousZoomId(TInt& aZoomId) const;

	IMPORT_C ~CCameraContinuousZoom();

private:
	static CCameraContinuousZoom* CreateL(MContinuousZoomObserver& aObserver, CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType, const MImplementationFactory& aImplFactory);

	CCameraContinuousZoom();
	void ConstructL(MContinuousZoomObserver& aObserver, CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType, const MImplementationFactory& aImplFactory);

private:
	MCameraContinuousZoom*		iImpl;
	};

#endif // ECAMADVSETTINGS_H



