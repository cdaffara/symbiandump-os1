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
 @publishedPartner
 @released
*/

#ifndef  ECAMADVSETTINGSINTF_H
#define  ECAMADVSETTINGSINTF_H

#include <ecamadvsettings.h>
#include <ecam/ecamimageprocessingintf.h>
#include <ecam/ecamadvsettingsintfuids.hrh>

/** This is the UID which is used to obtain the interface MCameraPresets, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface.
 */   
static const TUid KECamMCameraPresetsUid 			=  {KECamMCameraPresetsUidValue};

/** 
This is the UID which is used to obtain the interface MCameraPresets2, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. 
@publishedPartner
@prototype 
*/   
static const TUid KECamMCameraPresets2Uid 			=  {KECamMCameraPresets2UidValue};

/** This is the UID which is used to obtain the interface MCameraAdvancedSettings, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. */   
static const TUid KECamMCameraAdvancedSettingsUid 	=  {KECamMCameraAdvancedSettingsUidValue};

/** This is the UID which is used to obtain the interface MCameraAdvancedSettings2, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. */   
static const TUid KECamMCameraAdvancedSettings2Uid 	=  {KECamMCameraAdvancedSettings2UidValue};

/** 
This is the UID which is used to obtain the interface MCameraAdvancedSettings3, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface.
*/   
static const TUid KECamMCameraAdvancedSettings3Uid 	=  {KECamMCameraAdvancedSettings3UidValue};

/** 
This is the UID which is used to obtain the interface MCameraAdvancedSettings4, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface.
@publishedPartner
@prototype 
*/   
static const TUid KECamMCameraAdvancedSettings4Uid 	=  {KECamMCameraAdvancedSettings4UidValue};

/** 
This is the UID which is used to obtain the interface MCameraContinuousZoom, via the 
CCamera::CCameraAdvancedSettings::CreateContinuousZoomImpl() call, which provides 
implementation of the M-class interface.
@internalTechnology
@prototype 
*/   
static const TUid KECamMCameraContinuousZoomUid 	=  {KECamMCameraContinuousZoomUidValue};

/** 
Mixin class for implementation by providers of the Advanced Settings Camera Extension API.
CCamera advanced settings class exposes an API for controlling individually 
digital camera advanced settings. These settings directly relate to the 
image acquisition phase both for still images and video.

@publishedPartner
@released 	
*/	
class MCameraAdvancedSettings
	{
	
public:
	
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
   	/** 
	Gets the type of this camera. 
	@see TCameraType

	@return a TCameraType value. 
	*/
  	virtual CCamera::CCameraAdvancedSettings::TCameraType CameraType() const=0;
  	
  	/** 
	Get the type of a specific  camera denoted by its index. A pluggable camera
	may not necessarily be physically present. The type denotes whether the slot allocated
	to that index is for pluggable or onboard camera. 

    @param aCameraIndex
           An integer in the range of [0: CCamera::CamerasAvailable()-1].
           
	@return the TCameraType value for the specific camera. 
	        If the index is out of range, the return value is ECameraUnknown.
	*/
	virtual CCamera::CCameraAdvancedSettings::TCameraType CameraType(TInt aCameraIndex) const=0;
	
   	/** 
	Checks whether the current camera is present.
           
	@return Whether the camera is currently present. 
			ETrue if camera is present, EFalse otherwise. 
			For example ECameraOnBoard (built-in) cameras are always present.   
	*/	
	virtual TBool IsCameraPresent() const=0;
	
	/** 
	Checks whether the camera, denoted by its index, is currently present.
	The index uniquely identifies the camera on the device.
	
    @param aCameraIndex
           An integer in the range of [0:CCamera::CamerasAvailable()-1] specifying the
	       camera device to use
           
	@return Whether the camera is currently present. 
			ETrue if camera is present, EFalse otherwise. 
			For example built-in (ECameraOnBoard) cameras 
			are always present. 
	*/	
   	virtual TBool IsCameraPresent(TInt aCameraIndex) const=0;

    /**
    Gets current camera index. The index uniquely identifies the camera on the device.
	 
	@return camera index in the inclusive range of [0:CCamera::CamerasAvailable() - 1].
    */
  	virtual TInt CameraIndex() const=0;	
  	
    /** 
	Gets all of the supported stabilization modes on the device. The result is a bitfield
	of the valid TStabilizationMode flags. 

	@return a bitfield of all supported stabilization modes. 
	*/
    virtual TInt SupportedStabilizationModes() const=0;
    
    /** 
	Gets current stabilization mode on the device. 
	The result is a valid TStabilizationMode value. 

	@return current stabilization mode of type TStabilizationMode. 
	*/
    virtual CCamera::CCameraAdvancedSettings::TStabilizationMode StabilizationMode() const=0;
    
    /** 
	Sets a specific stabilization mode on the device.
	
	Stabilization mode change fires a KUidECamEventCameraSettingStabilizationMode
	event to all MCameraObserver2 clients of this specific camera.
	
	@param aStabilizationMode
	       new stabilization mode of TStabilizationMode type.
	*/
    virtual void SetStabilizationMode(CCamera::CCameraAdvancedSettings::TStabilizationMode aStabilizationMode)=0;
    
    /** 
	Gets all of the supported focus modes on the device. The result is a bitfield
	of the valid TFocusMode flags. 

	@return a bitfield of all supported focus modes. 
	*/
    virtual TInt SupportedFocusModes() const=0;
    
    /** 
	Gets current focus mode on the device. 
	The result is a valid TFocusMode value. 

	@return current focus mode. 
	*/
    virtual CCamera::CCameraAdvancedSettings::TFocusMode FocusMode() const=0;
    
    /** 
	Sets a specific focus mode on the device.
	Focus mode change fires a KUidECamEventCameraSettingFocusMode event
	to all MCameraObserver2 clients of the camera.
	
	@param aFocusMode 
	       new focus mode of TFocusMode type.
	*/
    virtual void SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode)=0;
    
	/** 
	Gets all supported focus ranges on the device.

	@return an integer - a bitfield of all supported TFocusRange values. 
	*/
    virtual TInt SupportedFocusRanges() const=0;
    
    /** 
	Gets current focus range on the device.

	@return the current TFocusRange value. 
	*/
    virtual CCamera::CCameraAdvancedSettings::TFocusRange FocusRange() const=0;
    
    /** 
	Sets a specific focus range on the device.
	The focus range change fires both, KUidECamEventCameraSettingFocusRange and 
	KUidECamEventCameraSettingFocusRange2 event	to all MCameraObserver2 clients of the camera.
	@see KUidECamEventCameraSettingFocusRange
    
    @param aFocusRange
           newly selected focus range.
	*/
    virtual void SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange)=0;
    
    /** 
	Gets all supported auto focus types on the device.

	@return an integer - a bitfield of all supported TAutoFocusType values. 
	*/
    virtual TInt SupportedAutoFocusTypes() const=0;
    
    /** 
	Gets current auto focus type on the device.

	@return a CCamera::TAutoFocusType value. 
	*/
    virtual CCamera::CCameraAdvancedSettings::TAutoFocusType AutoFocusType() const=0;
    
    /** 
	Sets a specific auto focus type on the device.
	The focus type change fires both, KUidECamEventCameraSettingAutoFocusType and 
	KUidECamEventCameraSettingAutoFocusType2 event to all MCameraObserver2 clients of the camera.
	@see KUidECamEventCameraSettingAutoFocusType

	@param aAutoFocusType
	       Autofocus selection.
    */
    virtual void SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType aAutoFocusType)=0;

    /** 
	Gets all supported auto focus areas on the device.

	@return an integer - a bitfield of al supported TAutoFocusArea values. 
	*/
    virtual TInt SupportedAutoFocusAreas() const=0;
    
    /** 
	Gets current chosen auto focus area on the device.

	@return a CCamera::TAutoFocusArea value. 
	*/
    virtual CCamera::CCameraAdvancedSettings::TAutoFocusArea AutoFocusArea() const=0;
    
    /** 
	Sets a specific auto focus area on the device.
	Focus area change fires a KUidECamEventCameraSettingAutoFocusArea event
	to all MCameraObserver2 clients of the camera.

	@param aAutoFocusArea
	       Autofocus area selection.
    */
    virtual void SetAutoFocusArea(CCamera::CCameraAdvancedSettings::TAutoFocusArea aAutoFocusArea)=0;

    /**
    Get focus distance in millimetres.
    
    @return the current focus distance in millimetres, directly from user setting of lenses.
    */
	virtual TInt FocusDistance() const=0;
	
	/**
    Set focus distance in millimetres. Focus distance change fires a KUidECamEventCameraSettingFocusDistance event
    to all MCameraObserver2 clients of the camera.
    
    @param aDistance 
    		the current value in millimetres, directly from user setting of lenses.
    */
	virtual void SetFocusDistance(TInt aDistance)=0;

	/**
    Get minimum focus distance in millimetres. 
    
    @return the minimum (35 camera equivalent) focal length of a device. 
    @note Current Focal length is calculated as 
     		focalLength =  opticalZoom * minFocalLength;  
    */
	virtual TInt GetMinFocalLength() const=0;
 
	/**
    Gets the set of camera supported ISO rates.
    
    @param aSupportedIsoRates
           an array of integers which gets filled in with the supported ISO rates. 
    
    @note  When camera device is incapable of revealing the ISO rates supported, it has to be assumed that 
		   camera will work only on the parmanently set value. If this value is not known, empty array should be 
		   returned, and the corresponding getter/setters for this feature for this feature should not be used. 
    */
    virtual void GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const=0;
    
    /**
    Gets current ISO rate.
    
    @return current ISO rate as a TInt value.
    
    @note  In case there is an error, a negative error value from system wide error should be returned.
    */
    virtual TInt IsoRate() const=0;
    
    /**
    Set current ISO rate for the camera.
    Triggers a KUidECamEventCameraSettingIsoRate event to all MCameraObserver2 clients of the camera.
    
    @param aRate
    	   required new ISO rate.
    */
    virtual void SetIsoRate(TInt aRate)=0;
	
	/** 
	Gets the current discrete aperture steps (F-stops) supported by the device.
	
	@param  aFStops
	        A reference to an empty array of TInt which would be populated by the implementation with 
	        the specific supported values. If the array is empty on return, 
	        the camera supports all integer values in the aperture range. Each value is multiplied by 
	        a factor of KECamFineResolutionFactor.
	
	@param  aInfo 
	        a reference to TValueInfo, which establishes the type of the returned data.     
	
	@leave KErrNoMemory Out of memory.
	
	@note  When camera device is incapable of revealing the aperture openings supported, it has to be assumed that 
		   camera will work only on the parmanently set value. If this value is not known, empty array should be 
		   returned and TValueInfo should be ENotActive, and the corresponding getter/setters for this feature should not be used.
 	*/
    virtual void GetAperturesL(RArray<TInt>& aFStops, TValueInfo& aInfo) const=0;
    
    /** 
    Get current aperture value.
    The default aperture value is ECAM implementation specific and could be either auto aperture or any other supported value.

    @return Current aperture value as an integer, multiplied by KECamFineResolutionFactor. 
    	    For example the function will return 280 for the actual aperture of F2.8.
    
    @note  In case there is an error, a negative error value from system wide error should be returned. 
    */
    virtual TInt Aperture() const=0; 
    
    /** 
    Set a new aperture value. 
    All MCameraObserver2 clients of the camera receive a KUidECamEventCameraSettingAperture 
    event notification when aperture value is changed.
    
    @note The aperture parameter value is an integer, multiplied by KECamFineResolutionFactor. 
    	  For example to set an aperture of F2.8, call SetAperture(280).
    
    @param aFStop 
    	   a new aperture value in the supported by the device range.       
    */
    virtual void SetAperture(TInt aFStop)=0; 
	
	/**
	Gets the set of supported shutter speeds
	
	@param aShutterSpeeds
			a reference to an RArray of TInt representing the discrete shutter speeds supported 
			currently by the device.

	@param  aInfo 
			a reference to TValueInfo, which establishes the type of the returned data.     

	@return the populated array with all shutter speeds in microseconds. 

	@leave KErrNoMemory Out of memory.
	
	@note  When camera device is incapable of revealing the shutter speeds supported, it has to be assumed that 
		   camera will work only on the parmanently set value. If this value is not known, empty array should be 
		   returned and TValueInfo should be ENotActive, and the corresponding getter/setters for this feature should not be used.
	*/
	virtual void GetShutterSpeedsL(RArray<TInt>& aShutterSpeeds, TValueInfo& aInfo) const=0; 
		
	/**
	Gets the current shutter speed
	
	@return the current shutter speed in microseconds. 
	
	@note  In case there is an error, a negative error value from system wide error should be returned.
	*/
    virtual TInt ShutterSpeed() const=0; 
    
    /**
	Sets the current shutter speed. When set, all MCameraObserver2 clients of the camera
	receive a KUidECamEventCameraSettingShutterSpeed event
	
	@param aShutterSpeed
	       the required shutter speed in microseconds.
	
	*/
    virtual void SetShutterSpeed(TInt aShutterSpeed)=0; 
    
	/**
	Get all supported metering modes on this device represented as bitfield of type TMeteringMode.
	
	@return the set of supported metering modes.
	*/
    virtual TInt SupportedMeteringModes() const=0;
    
    /**
    Get current metering mode.
    
    @return a value of type TMeteringMode.
    */
    virtual CCamera::CCameraAdvancedSettings::TMeteringMode MeteringMode() const=0;
    
    /**
    Set the current metering mode. When set, all MCameraObserver2 clients are notified 
    with a KUidECamEventCameraSettingMeteringMode event.
    
    @param aMeteringMode
    		a new selection for metering mode of type TMeteringMode.
    */
    virtual void SetMeteringMode(CCamera::CCameraAdvancedSettings::TMeteringMode aMeteringMode)=0;
    
    /** 
    Get all supported drive modes as bitfields of TDriveMode type. 
    
    @return the set of supported drive modes.
    */
    virtual TInt SupportedDriveModes() const=0;
    
    /** 
    Gets currently active drive mode.
    
    @return current drive mode.
    */
    virtual CCamera::CCameraAdvancedSettings::TDriveMode DriveMode() const=0;
    
    /**
    Set the current metering mode. When set all MCameraObserver2 clients are notified with a
    KUidECamEventCameraSettingDriveMode event. 
    
    @param aDriveMode
    		new selection for drive mode value of type TDriveMode.
    
    @note  Unless reduced Latency scheme is not used (ie CaptureImageL(TInt aSequenceNumber) or PerformantStopVideoCaptureL())
	   	   if an image/video capture is still outstanding, this method may report error KErrInUse.
    */
    virtual void SetDriveMode(CCamera::CCameraAdvancedSettings::TDriveMode aDriveMode)=0;

	/**
	Get all supported bracket modes as bitfields.
	
	@return the set of all supported bracket modes.
	*/
    virtual TInt SupportedBracketModes() const=0;
    
    /**
	Get current bracket mode.

	@return the current bracket mode TBracketMode.
	*/
    virtual CCamera::CCameraAdvancedSettings::TBracketMode BracketMode() const=0;
    
    /**
    Set new bracket mode. All MCameraObserver2 clients are notified with a
    KUidECamEventCameraSettingBracketMode event. 
    
    @param aBracketMode
    		new selection for bracket mode of type TBracketMode.
    */
    virtual void SetBracketMode(CCamera::CCameraAdvancedSettings::TBracketMode aBracketMode)=0;
    
 	/**
	Get all supported bracket parameters as bitfields.
	
	@return the set of all currently supported bracket modes.
	*/
    virtual TInt SupportedBracketParameters() const=0;
    
    /**
	Get current bracket parameter.

	@return the current bracket mode TBracketParameter.
	*/
    virtual CCamera::CCameraAdvancedSettings::TBracketParameter BracketParameter() const=0;
    
    /**
    Set new bracket parameter
    When set all clients are notified with a
    KUidECamEventCameraSettingBracketParameter event. 
    
    @param aBracketParameter
    		new selection for parameter type of type TBracketParameter.
    */
    virtual void SetBracketParameter(CCamera::CCameraAdvancedSettings::TBracketParameter aBracketParameter)=0;
    
	/**
	Get all supported bracket steps as bitfields.
	
	@return the set of all supported bracket modes.
	*/
    virtual TInt SupportedBracketSteps() const=0;
    
    /**
	Get current bracket step.

	@return the current bracket mode TBracketStep.
	*/
    virtual CCamera::CCameraAdvancedSettings::TBracketStep BracketStep() const=0;
    
    /**
    Set new bracket step. All MCameraObserver2 clients are notified with 
    KUidECamEventCameraSettingBracketStep. 
    
    @param aBracketStep
    		new selection for step of type TBracketStep.
    */
    virtual void SetBracketStep(CCamera::CCameraAdvancedSettings::TBracketStep aBracketStep)=0;
    
    /** 
	Gets the settings for which frames to merge. Valid only in EDriveModeBracketMerge mode. 
	@note there must be at least two images to merge. All are assumed to form a sequence and 
		are identified using the first frame index and number of frames e.g. to  merge two frames - one 
		on and one +1, when in EBracketMode3Image, one sets the start index to 1 and frames to two.
	@note It is very much TBracketMode setting dependent. 
	
	@param  aStartIndex
			the index of the start frame, starts from 0.
			
	@param  aFrames
			the number of frames to be merged.		
	*/
	virtual void GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const=0;
	
	/** 
	Sets the settings for which frames to merge. Valid only in EDriveModeBracketMerge mode. 
	@note there must be at least two images to merge. All are assumed to form a sequence and 
		are identified using the first frame index and number of frames e.g. to  merge two frames - one 
		on and one +1, when in EBracketMode3Image, one sets the start index to 1 and frames to 2.
		MCameraObserver2 clients are notified with a KUidECamEventBracketMerge event.
	
	@note It is very TBracketMode setting dependent. 
		
	@param  aStartIndex
			the index of the start frame, starts from 0.
			
	@param  aFrames
			the number of frames to be merged.		
	*/
	virtual void SetBracketMerge(TInt aStartIndex, TInt aFrames)=0;
    
    /**
    Get camera all supported flash modes CCamera::TFlash
    
    @return the set of all supported flash modes as bitfields in an integer.
    */
    virtual TInt SupportedFlashModes() const=0;
    
    /** 
	Gets the currently set flash mode.

	@return  The currently set flash mode. 
	*/
	virtual CCamera::TFlash FlashMode() const=0;

    /** 
	Sets the flash mode.

    Triggers a KUidECamEventCameraSettingFlashMode event to all camera 
    MCameraObserver2 clients.
    
	@param  aMode
	        The required flash mode. 
	*/
	virtual void SetFlashMode(CCamera::TFlash aMode)=0;

    /** 
	Gets whether the flash red eye reduction is switched on.
    
	@return The present state - ETrue for switched on and EFalse for switched off. 
	*/
    virtual TBool RedEyeReduceOn() const=0;
    
    /** 
	Sets the flash red eye reduction on or off.

    Triggers a KUidECamEventCameraSettingFlashRedEyeReduce event to all camera 
    MCameraObserver2 clients.
    
	@param  aState
	        The required state ETrue for switching it on and EFalse for switching it off. 
	*/
    virtual void SetRedEyeReduceOn(TBool aState)=0;
    
	/** 
	Get flash compensation steps as integers multiplied by KECamFineResolutionFactor.
	For example 0.5 EV is 50. 
	
	@param aFlashCompensationSteps
	       an RArray of integers which is populated on return to reflect the supported 
	       flash compensation steps.
	       
   	@param aInfo 
   	       an TValueInfo reference, which establishes the organization of the returned data. 
   	
   	@see TValueInfo   
   		     
	@leave KErrNoMemory Out of memory. 
	
	@note  When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive, 
		   and the corresponding getter/setters for this feature should not be used. 
		   When camera device is incapable of revealing the flash compensation steps supported, 
		   it has to be assumed that camera will work only on the parmanently set value. If this value is not known, empty 
		   array should be returned and TValueInfo should be ENotActive, and the corresponding getter/setters for this feature should not be used.
	*/
	virtual void GetFlashCompensationStepsL(RArray<TInt>& aFlashCompensationSteps, TValueInfo& aInfo) const=0; 
	
	/**
	@deprecated Use TInt GetFlashCompensationStep(TInt& aFlashCompensationStep);
	
	Get current flash power compensation step.
	
	@return current flash compensation step.
	*/
	virtual TInt FlashCompensationStep() const=0;

	/** 
	Get current flash power compensation step.
	
	@param aFlashCompensationStep
		   Reference to the current flash power compensation step.	
	
	@return system wide error code.
	*/
	virtual TInt GetFlashCompensationStep(TInt& aFlashCompensationStep) const=0;
	
    /** 
	Set current flash compensation step as an integer multiplied by KECamFineResolutionFactor.
	For example to set a compensation of -0.3 EV, one should use a parameter with value -30.  
	All clients receive a KUidECamEventCameraSettingFlashCompensationStep event, when the value has changed. 
	
	@param aFlashCompensationStep
	       a new value for the flash compensation step.
	*/
	virtual void SetFlashCompensationStep(TInt aFlashCompensationStep)=0;
	
	/**
	Get current flash power compensation range measured in a already selected compensation step.
	@note This range may change if a different compensation step is selected. 
	For example if flash compensation range is in the range -1EV 1.5EV and the selected flash compensation 
	step is selected to be 0.3 EV, the result of this call will be 
	aNegativeCompensation = -3 and aPositiveCompensation = 5.
	as there can be only three full steps for negative compensation (1/0.3) and five for flash power boost (1.5/0.3).
    In this way developers, having pre-selected a step value from the supported set, would need to provide 
    just the multplier (in steps) and the direction (the sign). Steps are always assumed integers.	
	
	@param  aNegativeCompensation
	        reference to an integer returning the maximum number of steps available for negative compensation.
	        
	@param aPositiveCompensation
	        reference to an integer returning the maximum number of steps available for positive compensation.     
	*/
	virtual void GetFlashCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const=0;

	/** 
	@deprecated Use TInt GetFlashCompensation(TInt& aFlashCompensation);
	
	Get the current flash compensation value as integer steps. Positive values boost flash power,
	negative reduce flash power. The change is not cumulative i.e. the change is stateless.
	Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
	   
	@note if returned value is 2 (compensation steps) and the current flash compensation step is 0.3 EV, 
	then the actual compensation effect will be 0.6 EV. 
	   
	@return  the current number compensation steps as an integer.
	*/
	virtual TInt FlashCompensation() const=0;

	/** 
	Get the current flash compensation value as integer steps. Positive values boost flash power,
	negative reduce flash power. The change is not cumulative i.e. the change is stateless.
	Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
	   
	@note if retrieved value is 2 (compensation steps) and the current flash compensation step is 0.3 EV, 
	then the actual compensation effect will be 0.6 EV. 
	
	@param   aFlashCompensation
			 Reference to the current number of compensation steps as an integer.
	
	@return  system wide error code.
	*/
	virtual TInt GetFlashCompensation(TInt& aFlashCompensation) const=0;
	
	/** 
	Set the current flash compensation value as integer steps. 
	Positive values increase power, negative reduce power. The change is not cumulative i.e. the change is stateless.
	Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
	Triggers a KUidECamEventCameraSettingFlashCompensation event.
	
	@param 	aFlashCompensationSteps  
	        a required compensation steps - negative value reduce the flash power
	        positive boosts up the flash power. 
	*/
	virtual void SetFlashCompensation(TInt aFlashCompensationSteps)=0;
	
	/**
	Check whether there is an external flash source.
	
	@return ETrue if an external flash source is present, EFalse otherwise  
	*/ 
	virtual TBool IsExternalFlashPresent() const=0;
	   
	/** 
	Gets the current discrete manual flash power levels supported by the device in range 0-100 
	as a percentage of maximum power level.
	
	@param  aManualFlashPowerLevels
	        An empty array of TInt which would be populated by the implementation with 
	        the specific supported values. If the array is empty on return, 
	        the camera does not support this functionality.
	@param  aInfo 
	        a reference to TValueInfo, which establishes the type of the returned data.             
		
	@leave KErrNoMemory Out of memory. 
	
	@note  When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive,
		   and the corresponding getter/setters for this feature should not be used. 
		   When camera device is incapable of revealing the manual flash power levels supported, 
		   it has to be assumed that camera will work only on the parmanently set value. If this value is not known, empty 
		   array should be returned and TValueInfo should be ENotActive, and the corresponding getter/setters for this feature should not be used.
	*/
    virtual void GetManualFlashPowerLevelsL(RArray<TInt>& aManualFlashPowerLevels, TValueInfo& aInfo) const=0;
    
    /**
    Gets the current manual flash power level on the device.
	
	@return the current manual flash power level as a value in the range [0:100]. 
	
	@note  In case there is an error, a negative error value from system wide error should be returned.
	*/
    virtual TInt ManualFlashPowerLevel() const=0;
    
    /**
    Sets the current manual flash power level on the device.
    Triggers a KUidECamEventCameraSettingFlashManualPower event to all MCameraObserver2 clients.
	
	@param  aManualFlashPowerLevel
	        one of the values returned in GetManualFlashPowerLevelsL(). 
	*/
    virtual void SetManualFlashPowerLevel(TInt aManualFlashPowerLevel)=0;
    
	/**
	Get Supported exposure modes - bitfields of CCamera::TExposure
	
	@return the set of supported exposure modes.  
	*/
	virtual TInt SupportedExposureModes() const=0;
	
	/** 
	Gets the currently set exposure setting value.

	@return  The currently set exposure setting value. 
	*/
	virtual CCamera::TExposure ExposureMode() const=0;

	/** 
	Sets the exposure mode of the device.

	Triggers a KUidECamEventCameraSettingExposureMode event to all MCameraObserver2 clients.

	@param  aExposureMode
	        The required exposure adjustment. 	        
	*/
	virtual void SetExposureMode(CCamera::TExposure aExposureMode)=0;
	
	/** 
	Get exposure compensation steps as integers multiplied by KECamFineResolutionFactor.
	For example 0.3 EV is 30.
	
	@param aExposureCompensationSteps
	       an RArray of integers which is populated to reflect the supported 
	       exposure compensation steps, all values have been multiplied by KECamFineResolutionFactor before 
	       truncated to integers.
	       
	@param aInfo 
		   a reference to TValueInfo, which establishes the type of the returned data.            
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	
	@note  When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive,
		   and the corresponding getter/setters for this feature should not be used.
		   When camera device is incapable of revealing the exposure compensation steps supported, 
		   it has to be assumed that camera will work only on the parmanently set value. If this value is not known, empty 
		   array should be returned and TValueInfo should be ENotActive, and the corresponding getter/setters for this feature should not be used.
	*/
	virtual void GetExposureCompensationStepsL(RArray<TInt>& aExposureCompensationSteps, TValueInfo& aInfo) const=0; 
	
	/** 
	@deprecated Use TInt GetExposureCompensationStep(TInt& aExposureCompensationStep);
	
	Get current exposure compensation step.
	
	@return current exposure compensation step.
	*/
	virtual TInt ExposureCompensationStep() const=0;
	
	/** 
	Get current exposure compensation step.
	
	@param aExposureCompensationStep
		   Reference to the current exposure compensation step.
	
	@return system wide error code.
	*/
	virtual TInt GetExposureCompensationStep(TInt& aExposureCompensationStep) const=0;
	
	/** 
	Set current exposure compensation step as an integer multiplied by KECamFineResolutionFactor.
	All MCameraObserver2 clients receive a KUidECamEventCameraSettingExposureCompensationStep event,  
	when the value has changed. 
	
	@param aExposureCompensationStep
	       a new value for the exposure compensation step.
	*/
	virtual void SetExposureCompensationStep(TInt aExposureCompensationStep)=0;
	
	/**
	Get current exposure compensation range in steps. It depends on the previously 
	selected exposure compensation step. 
	
	@param  aNegativeCompensation
	        reference to an integer returning the maximum number of steps 
	        available for negative compensation.
	        
	@param aPositiveCompensation
	        reference to an integer returning the maximum number of steps 
	        available for positive compensation. 
	        
	@see GetFlashCompensationRangeInSteps()
	*/
	virtual void GetExposureCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const=0;

	/** 
	@deprecated Use TInt GetExposureCompensation(TInt& aExposureCompensation);
	
	Get the current exposure compensation value as integer steps. Positive values increase exposure times, 
	negative reduce exposure times. The change is not cumulative i.e. the change is stateless.
	Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
			   
	@note if returned value is 2 (compensation steps) and the current exposure compensation step is 0.3 EV, 
	then the actual compensation effect will be 0.6 EV. 
	   
	@return  current number compensation steps as an integer.
	*/
	virtual TInt ExposureCompensation() const=0;

	/** 
	Get the current exposure compensation value as integer steps. Positive values increase exposure times, 
	negative reduce exposure times. The change is not cumulative i.e. the change is stateless.
	Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
			   
	@note if retrieved value is 2 (compensation steps) and the current exposure compensation step is 0.3 EV, 
	then the actual compensation effect will be 0.6 EV. 
	
	@param aExposureCompensation
		   Reference to the current number of compensation steps as an integer.
		   
	@return  system wide error code.
	*/
	virtual TInt GetExposureCompensation(TInt& aExposureCompensation) const=0;
	
	/** 
	Set the current exposure compensation value as integer steps.
	Triggers a KUidECamEventCameraSettingExposureCompensation event to all MCameraObserver2 clients.
	
	@param 	aExposureCompensationSteps  
	        a required compensation value negative value reduce the exposure time
	        positive increases the exposure time. 
	
	*/
	virtual void SetExposureCompensation(TInt aExposureCompensationSteps)=0;

    /**
    Gets camera supported set of white balance adjustments.
    
    @return bitfield of all supported CCamera::TWhiteBalance values.  
    */
	virtual TInt SupportedWhiteBalanceModes() const=0;
	
	/** 
	Gets the current white balance value.

	@return  The current white balance value.
	*/
	virtual CCamera::TWhiteBalance WhiteBalanceMode() const=0;
	
	/** 
	Sets the white balance adjustment of the device.

	No effect if this is not supported, see TCameraInfo::iWhiteBalanceModesSupported.
	Triggers a KUidECamEventCameraSettingWhiteBalanceMode event to all MCameraObserver2 clients.

	@param  aWhiteBalanceMode
	        The required white balance mode.
	*/
	virtual void SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode)=0;

    /** 
    Gets the current state for aperture and exposure lock.
    
    @return ETrue if aperture and exposure values are locked together, 
           EFalse if these are not locked. 
    */
	virtual TBool ApertureExposureLockOn() const=0;
	
	/** 
    Sets the current state for aperture and exposure lock.
    Triggers a KUidECamEventAELock event to all MCameraObserver2 clients.
    
    @param aAELock
           a value whether to lock exposure and aperture together.
    */
	virtual void SetApertureExposureLockOn(TBool aAELock)=0;

	/**
	Gets the current state for button clicking sound effect.
	
	@return  boolean, ETrue to switch clicking sound on, EFalse sound off  
	*/
	virtual TBool ShootClickOn() const=0;
	
	/**
	Sets the button clicking sound effect on /off. Triggers a KUidECamEventSoundClick event 
	to all MCameraObserver2 clients. 
	
	@param aShootClickOn
	       boolean, ETrue to switch clicking sound on, EFalse sound is switched off.  
	*/
	virtual void SetShootClickOn(TBool aShootClickOn)=0;
	
	/**
	Get camera supported timer values. Active only when drive mode EDriveModeTimed.
	Time is in microseconds. As time interval is expected to be relatively short, 
	integer value is considered sufficient. 
	
	@param aTimerIntervals 
	       an RArray of integers which is populated to reflect the supported 
	       timer interval steps.
	       
 	@param aInfo 
 	       an TValueInfo reference, which establishes the organization of 
   		   the returned data. 
   	
   	@see TValueInfo  
   	
   	@note  When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive,
   		   and the corresponding getter/setters for this feature should not be used. 
	*/
	virtual void GetTimerIntervalsL(RArray<TInt>& aTimerIntervals, TValueInfo& aInfo) const=0;
	
	/**
	Get current timer value. Active only when drive mode is EDriveModeTimed.
	Timer resolution is in microseconds. 
	
	@return current time interval value. 
	
	@note  In case there is an error, a negative error value from system wide error should be returned.
	*/
    virtual TInt TimerInterval() const=0;  
    
    /**
	Set current timer value. Active only when drive mode EDriveModeTimed.
	This is the time interval (delay) in microseconds between user pressing the button and image is taken.  
	The setting of the value triggers a KUidECamEventCameraSettingTimerInterval event 
	to all MCameraObserver2 clients. 
	
	@param aTimerInterval
	       The selected timer interval in microseconds        
	*/
    virtual void SetTimerInterval(TInt aTimerInterval)=0; 

	/**
	Get camera supported time lapse period range. Active only when drive mode EDriveModeTimeLapse.
	The time lapse is denoted as the uniform time period between consecutive frames.
	
	@param aTimeLapseMin
	       The minimum time value. 
	       
	@param aTimeLapseMax
	       The maximum time value.
	*/
	virtual void GetTimeLapsePeriodRange(TTime& aTimeLapseMin, TTime& aTimeLapseMax) const=0;
	
	/**
	Get current time lapse value. Active only when drive mode EDriveModeTimeLapse.
	The time lapse is denoted as the uniform time period 
	between consecutive frames and operation is configurable by its start, end and a fixed interval.
	@param aStart
			the start of the  timelapse period
	@param aEnd
			the end of the  timelapse period; start < end.
	@param aInterval
			the set parameter between two successive snapshots.		
	*/
    virtual void GetTimeLapse(TTime& aStart, TTime& aEnd, TTime& aInterval) const=0;  
    
    /**
	Set current time lapse value. Active only when drive mode EDriveModeTimeLapse.
	The time lapse is denoted as the uniform time period between consecutive frames. 
	Setting triggers a KUidECamEventCameraSettingTimeLapse event to all MCameraObserver2 camera clients.
	
	@param aStart
			the start of the timelapse period.
	@param aEnd
			the end of the timelapse period; start < end.
	@param aInterval
			the set parameter between two successive snapshots.	
	*/
    virtual void SetTimeLapse(const TTime& aStart, const TTime& aEnd, const TTime& aInterval)=0; 
    
	/**
	Get current picture orientation.
	
	@return a TPictureOrientation value.
	*/
    virtual CCamera::CCameraAdvancedSettings::TPictureOrientation PictureOrientation() const=0;
    
    /**
    Set a new picture orientation
    This triggers a KUidECamEventCameraSettingPictureOrientation event to all MCameraObserver2 clients.
    
    @param aOrientation
           a value of TPictureOrientation denoting the new orientation.
    */
    virtual void SetPictureOrientation(CCamera::CCameraAdvancedSettings::TPictureOrientation aOrientation)=0; 

    /**
	Get supported pixel aspect ratio.
	
	@return a bitfield of all supported TPixelAspectRatio values.
	*/
    virtual TInt SupportedPixelAspectRatios() const=0;
	/**
	Get current pixel aspect ratio.
	
	@return a TPixelAspectRatio value.
	*/
    virtual CCamera::CCameraAdvancedSettings::TPixelAspectRatio PixelAspectRatio() const=0;
    
    /**
    Set a new pixel aspect ratio.
    This triggers a KUidECamEventPixelAspectRatio event to all MCameraObserver2 clients.
    
    @param aPixelAspectRatio
           a value of TPixelAspectRatio denoting the new pixel aspect ratio.
    */
    virtual void SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::TPixelAspectRatio aPixelAspectRatio)=0; 

	/**
	Get supported YUV ranges.
	
	@return a bitfileld of all supported TYuvRange values.
	*/
    virtual TInt SupportedYuvRanges() const=0;
    
	/**
	Get the current YUV range.
	
	@return a TYuvRange value.
	*/
    virtual CCamera::CCameraAdvancedSettings::TYuvRange YuvRange() const=0;
    
    /**
    Set a new YUV range.
    This triggers a KUidECamEventYuvRange event to all MCameraObserver2 clients.
    
    @param aYuvRange
           a value of TYuvRange denoting the new YUV range.
    */
    virtual void SetYuvRange(CCamera::CCameraAdvancedSettings::TYuvRange aYuvRange)=0;
	
	/**
	Get the number of images captured normally under EDriveModeBurst condition.
	@note: due to memory or image size limitations the actual number may be less. 
	
	@return the number of images set to capture in burst mode. 
	*/
    virtual TInt BurstImages() const=0;
    
    /**
    Set the number of images captured normally under EDriveModeBurst condition.
    Triggers a KUidECamEventBurstImages event to all MCameraObserver2 clients.
	@note: due to memory or image size limitations the actual number may be less. 
	
	@param aImages
		   the number of images set to capture in burst mode. 
		   
	@note  Unless reduced Latency scheme is not used (ie CaptureImageL(TInt aSequenceNumber) or PerformantStopVideoCaptureL())
	   	   if an image/video capture is still outstanding, this method may report error KErrInUse.
    */
    virtual void SetBurstImages(TInt aImages)=0;	

    /** 
	Gets the optical zoom levels.

	@param  aOpticalZoomSteps
	        Array to hold optical zoom values multiplied by KECamFineResolutionFactor to retain precision. 
	        So that if zoom is not supported the array will return a single element of value 
	        KECamFineResolutionFactor.

	@param  aInfo 
			a reference to TValueInfo, which establishes the type of the returned data.     

	@note   Such approach allows for support for both linear and non-linear zoom steps.
			When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive, 
			and the corresponding getter/setters for this feature should not be used.
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors. 
	*/
	virtual void GetOpticalZoomStepsL(RArray<TInt>& aOpticalZoomSteps, TValueInfo& aInfo) const=0;
	
	/** 
	Gets the currently set zoom value.

	@return  The currently set optical zoom value. The value is multiplied by 
			 KECamFineResolutionFactor to retain precision.
			 
	@note  In case there is an error, a negative error value from system wide error should be returned. 
	*/
	virtual TInt OpticalZoom() const=0;
	
	/** 
	Sets the zoom using a specific value. Triggers a KUidECamEventCameraSettingOpticalZoom 
	event to all MCameraObserver2 clients.

	@param aOpticalZoom 
	       Required zoom value.
	*/
	virtual void SetOpticalZoom(TInt aOpticalZoom)=0;

	/** 
	Gets the digital zoom levels.

	@param  aDigitalZoomSteps
	        Array to hold digital zoom values multiplied by KECamFineResolutionFactor to retain precision. 
	        So that if zoom is not supported the array will return a single element of value 
	        KECamFineResolutionFactor.
	        
	@param  aInfo 
			a reference to TValueInfo, which establishes the type of the returned data.
		
	@note   Such approach allows for support for both linear and non-linear zoom steps.
			When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive
			and the corresponding getter/setters for this feature should not be used.
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
	virtual void GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo) const=0;
	
	/** 
	Gets the currently set digital zoom value.

	@return  The currently set digital zoom value. The value is multiplied by 
			 KECamFineResolutionFactor to retain precision. 
			 
	@note  In case there is an error, a negative error value from system wide error should be returned.
	*/
	virtual TInt DigitalZoom() const=0;
	
	/** 
	Sets the digital zoom value. Triggers a KUidECamEventCameraSettingDigitalZoom event
	 to all MCameraObserver2 clients.

	@param aDigitalZoom 
	       Required zoom value.
	
	*/
	virtual void SetDigitalZoom(TInt aDigitalZoom)=0;

	/** 
	Checks whether exposure value is locked or not.

	@return whether exposure value is locked or not. 
			ETrue if locked, EFalse otherwise. 
	*/
	virtual TBool ExposureLockOn() const=0;
	
	/** 
	Sets exposure lock state. Triggers a KUidECamEventCameraSettingExposureLock event
	 to all MCameraObserver2 clients.

	@param aState 
	       Required new state.
	*/
	virtual void SetExposureLockOn(TBool aState)=0;
		
	/** 
	Checks whether AutoFocus value is locked or not.

	@return whether AutoFocus value is locked or not. 
			ETrue if locked, EFalse otherwise. 
	*/
	virtual TBool AutoFocusLockOn() const=0;
	
	/** 
	Sets autofocus lock state. Triggers a KUidECamEventCameraSettingAutoFocusLock event
	to all MCameraObserver2 clients.

	@param aState 
	       Required new state.
	*/
	virtual void SetAutoFocusLockOn(TBool aState)=0;
		
	/** 
	Gets an array of all the advanced settings parameters supported by the device. 
	These are identified by UIDs and relate to the set or subset of it of all defined settings UIDs. 
	
	@param  aSettings
	        An empty array of TUids which would be populated by the implementation with 
	        the UIDs of the supported parameters. If the array is empty on return, 
	        the camera does not support any settings.  
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
    virtual void GetSupportedSettingsL(RArray<TUid>& aSettings) const=0;
    
    /** 
	Gets an array of all the advanced settings parameters currently active on the device. 
	These are identified by UIDs and relate to the set or subset of it of all supported
	settings UIDs. 
	
	@param  aActiveSettings
	        An empty array of TUids which would be populated by the implementation with 
	        the active setting UIDs. If the array is empty on return, 
	        the camera does not support any settings.  
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors. 
	*/
    virtual void GetActiveSettingsL(RArray<TUid>& aActiveSettings) const=0;
    
    /** 
	Gets an array of all the advanced settings parameters currently disabled on the device. 
	These are identified by UIDs and relate to the set or subset of it of all supported
	settings UIDs. 
	
	@param  aDisabledSettings
	        An empty array of TUids which would be populated by the implementation with 
	        the disabled setting UIDs. If the array is empty on return, 
	        the camera does not support any settings.  
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors. 
	*/
    virtual void GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const=0;
	
	/** 
	Retrieves the state for automatic size selection option. Default value is EFalse.
	
	@return ETrue if the automatic selection is switched on. Default value is EFalse.
	*/
	virtual TBool AutomaticSizeSelectionChangeOn() const=0;	

	/** 
	Allow camera to proactively change image size due external factors. 
	Default value is EFalse. Triggers a KUidECamEventCameraSettingAutomaticSizeSelection event notification.
	@param aSetOn
	       whether the option should be switched on
	 */     
	virtual void SetAutomaticSizeSelectionChangeOn(TBool aSetOn)=0;	

    /** 
    Retrieves the timeout values camera supported by the camera when in continuous auto focus mode. 
    Timeouts are in microseconds.
    
   	@param  aTimeouts
	        An empty array to hold timeout values.
	        
	@param  aInfo 
			a reference to TValueInfo, which establishes the type of the returned data.
			
	@leave  KErrNoMemory Out of memory. May also leave as a result of other system errors.
	
	@note   When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive
			and the corresponding getter/setters for this feature should not be used.
    */
    virtual void GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& aTimeouts, TValueInfo& aInfo) const=0;
    
    /**
    Gets the current value for continuous autofocus timeout. Timeouts are in microseconds.
    
    @return  the timeout value in microseconds. 
    
    @note  In case there is an error, a negative error value from system wide error should be returned.
    */
    virtual TInt ContinuousAutoFocusTimeout() const=0; 
       
    /**
    Sets new value for continuous autofocus timeout. Timeouts are in microseconds.
    All MCameraObserver2 clients of the camera receive a 
    KUidECamEventCameraSettingsContinuousAutoFocusTimeout 
    event notification when timeout value is changed.
    
    @param  aTimeout
	        a new timeout value in microseconds. 
    */
    virtual void SetContinuousAutoFocusTimeout(TInt aTimeout)=0;
   
    /**
    Gets all supported stabilization effects on the device.
    
    @return an integer - a bitfield of all supported TStabilizationEffect values. 
    */
	virtual TInt SupportedStabilizationEffects() const=0;
	
    /**
    Gets current active stabilization effect on the device.
    
    @return a TStabilizationEffect value.
    */
	virtual CCamera::CCameraAdvancedSettings::TStabilizationEffect StabilizationEffect() const=0;
    
    /**
    Sets a specific stabilization effect on the device. 
    When a value is set, MCameraObserver2 clients for that camera will receive a 
    KUidECamEventCameraSettingsStabilizationEffect event notification.
    
    @param aEffect
           stabilization effect selection of type TStabilizationEffect.
    */
    virtual void SetStabilizationEffect(CCamera::CCameraAdvancedSettings::TStabilizationEffect aEffect)=0;

    /**
    Gets all supported stabilization algorithm values on the device.
    
    @return an integer - a bitfield of all supported TStabilizationAlgorithmComplexity values. 
    */
	virtual TInt SupportedStabilizationComplexityValues() const=0;
    
    /**
    Gets current active stabilization algorithm selection on the device.
    
    @return a TStabilizationAlgorithmComplexity value.
    */
	virtual CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity StabilizationComplexity() const=0;
    
    /**
    Sets a specific stabilization algorithm on the device. 
    When a value is set, MCameraObserver2 clients for that camera will receive a 
    KUidECamEventSettingsStabilizationAlgorithmComplexity event notification.
    
    @param aComplexity
           stabilization effect selection of type TStabilizationAlgorithmComplexity.
    */
	virtual void SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity aComplexity)=0;

    /**
    Gets the units in which the white balance is measured on the device. The methods used to get 
    or set these differ depending on the supported unit type.  
    It is expected that a device will support only a single type or none.
    
    @return a value of TWBUnits type.   
    */
    virtual CCamera::CCameraAdvancedSettings::TWBUnits SupportedWBUnits() const=0;
    
    /**
    Get white balance value represented as a RGB triplet (TRgb)
    
    @param aValue
    	   a reference to TRgb object which will contain the current white balance.
    */
	virtual void GetWBRgbValue(TRgb& aValue) const=0;
    
    /**
    Set white balance value using a RGB triplet (TRgb). 
    Change in value causes an event notification KUidECamEventCameraSettingsWBValue 
    to be sent to all MCameraObserver2 clients of this camera. 
    
    @param aValue
    	   a const reference to TRgb object, which contains the new white balance.
    */
    virtual void SetWBRgbValue(const TRgb& aValue)=0;
    
    /**
    Get the white balance values, as temperature measured in Kelvin, supported on the device.
	
	@param  aWBColorTemperatures
        	A reference to an empty array of TInt which would be populated by the implementation with 
        	the specific supported values. 
	
	@param  aInfo 
	        a reference to TValueInfo, which establishes the type of the returned data.     
	        
	@note   When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive,
			and the corresponding getter/setters for this feature should not be used.
    */
	virtual void GetWBSupportedColorTemperaturesL(RArray<TInt>& aWBColorTemperatures, TValueInfo& aInfo) const=0;
    
    /**
    Get the white balance as a temperature in Kelvin.
    
    @return current white balance value as a temperature in Kelvins.
    
    @note  In case there is an error, a negative error value from system wide error should be returned.
    */
 	virtual TInt WBColorTemperature() const=0;   
 	
 	/**
    Set white balance value using a temperature, measured in Kelvin. 
    Change in value causes an event notification KUidECamEventCameraSettingsWBValue 
    to be sent to all MCameraObserver2 clients of this camera. 
    
    @param aWBColorTemperature
    	   the new white balance value in Kelvin.
    */
	virtual void SetWBColorTemperature(TInt aWBColorTemperature)=0;
	};


/** 
Mixin class for implementation by providers of some enhanced functionalities of the 
Advanced Settings Camera Extension API.

@publishedPartner
@released	
*/	
class MCameraAdvancedSettings2
	{
	
public:
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/** 
 	Checks whether the flash is ready.
     
    @param 	aReady
    		A reference to a boolean set by the implementation to ETrue if the flash is ready, 
    		EFalse otherwise.
            
 	@return KErrNotSupported if the implementation of this method is not supported.
 	*/	
 	virtual TInt IsFlashReady(TBool& aReady) const=0;
 	
 	/**
    Get the number of focus steps for current focus mode.
 	
 	@param  aFocusModeSteps
         	A reference to an empty array of TInt which would be populated by the implementation with 
         	the specific supported values. 
 	
 	@param  aInfo 
 	        a reference to TValueInfo, which establishes the type of the returned data. 
 	
 	@leave  KErrNotSupported if the implementation of this method is not supported. May also leave as a result of other system errors.
    
    @note   When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive, 
            and the corresponding getter/setters for this feature should not be used.
    */
 	virtual void GetCurrentFocusModeStepsL(RArray<TInt>& aFocusModeSteps, TValueInfo& aInfo) const=0;
	};
	
/** 
Mixin class for implementation by providers of 'some of the enhanced functionalities' of the 
Advanced Settings Camera Extension API.

@publishedPartner
@released	
*/	
class MCameraAdvancedSettings3
	{
	
public:
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
 	Gets all supported ISO types on the device.
 	
 	@param  aSupportedISORateTypes 
 			A reference to an integer which is a bitfield of all supported TISORateType values. 
 			EISONone means feature is not supported.
  	
  	@leave  May leave as a result of some error. 
  	*/	
	virtual void GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const=0;
	
	/** 
	Set the type of ISO rate and the exposure parameter or value specified.
	 
	@param 	aISORateType
			The type of ISO rate to be set.

	@param  aParam
			Depending on the value of aISORateType, possible values of aParam are one of the following:-
			The value of ISO rate to be used in case of manual type of ISO rate (EISOManual). 
			OR
			Redundant parameter in case of unprioritised type of auto ISO (EISOAutoUnPrioritised). It is left to the camera hardware/firmware 
			to decide how the ISO rate is selected. No priority regarding exposure is specified.
			OR
			Highest ISO rate to be picked by the camera while deciding for the best exposure in case of ISO prioritised 
			type of auto ISO (EISOAutoISOPrioritised). ISO rate closest to this (and lower) may be used so that best possible exposure is achieved.
			OR
			Slowest shutter speed to be picked by the camera while deciding for the best exposure in case of shutter speed 
			prioritised type of auto ISO (EISOAutoShutterSpeedPrioritised). After using this shutter speed, ISO rate is chosen by the camera to achieve 
			proper exposure. Shutter speed closest to this (and faster) may be used so that best possible exposure is achieved.
			OR
			Minimum aperture opening (deepest depth of field) to be picked by the camera while deciding 
			for the best exposure in case of aperture prioritised type of auto ISO (EISOAutoAperturePrioritised). After using this aperture opening, ISO
			rate is chosen by the camera to achieve proper exposure. Aperture opening closest to this (and wider) may be 
			used to achieve best possible exposure.
	
	@note   Triggers KUidECamEventCameraSettingIsoRateType to all MCameraObserver2 clients of the camera. 
			Uses HandleEvent to report the result or any possible error. 
	*/	
	virtual void SetISORate(CCamera::CCameraAdvancedSettings::TISORateType aISORateType, TInt aParam)=0;
	
	/** 
 	Get the type of ISO rate, exposure parameter and value set.
 
	@param 	aISORateType
			A reference to the type of ISO rate set. EISONone means feature is not supported.

	@param  aParam
			Depending on the value of aISORateType, possible values of aParam are one of the following:-
			A reference to the redundant parameter in case of manual type of ISO rate(EISOManual) 
			OR
			A reference to the redundant parameter in case of unprioritised type of auto ISO(EISOAutoUnPrioritised)
			OR
    		A reference to the highest ISO rate that may be picked up in case of ISO prioritised type of auto ISO(EISOAutoISOPrioritised) 
    		OR
			A reference to the slowest shutter speed that may be picked up in case of shutter speed prioritised type of auto ISO(EISOAutoShutterSpeedPrioritised) 
			OR
			A reference to the minimum aperture opening that may be picked up in case of aperture prioritised type of auto ISO(EISOAutoAperturePrioritised)

	@param  aISORate   
			A reference to the value of ISO rate currently being used, if camera device is capable of doing that. 
			Otherwise KErrNotFound should be retrieved indicating the incapability of camera.
	
	@leave  May leave as a result of some other error.
	
	@note   Since camera hardware may be incapable of providing the actual ISO value when one of the auto ISO type has 
			been set, then, in these cases, the 3rd argument should be retrieved as KErrNotFound.    
	*/	
	virtual void GetISORateL(CCamera::CCameraAdvancedSettings::TISORateType& aISORateType, TInt& aParam, TInt& aISORate) const=0;
	
	/**
	Provide reference screen for orientation information.

	@param  aScreenDevice
			A reference to the screen device.

	@note	For consistency, when DSA view finder runs, it also provides a screen device. So, the DSA view finder's internal implementation 
			should call this method with the screen device passed ( 2nd argument to DSA view finder) to avoid disparity in knowing the refeence screen.
			Triggers KUidECamEventCameraSettingReferenceScreen to all MCameraObserver2 clients of the camera. 
			Uses HandleEvent to report the result or any possible error. 
	*/	
	virtual void SetReferenceScreen(CWsScreenDevice& aScreenDevice)=0;
	
	/**
	Get the digital zoom steps for the still image when a particular image format and size are specified.
	
	@param  aDigitalZoomSteps
			A reference to an empty array of TInt to hold digital zoom step values for still image and multiplied by 
			KECamFineResolutionFactor to retain precision. If list returned is empty, this means feature is not supported.
	
	@param  aInfo
			A reference to TValueInfo, which establishes the type of the returned data.
			
	@param  aSizeIndex
			A value providing the size index which must be in the range 0 to TCameraInfo::iNumImageSizesSupported-1
			inclusive.
			
	@param  aFormat
			A value providing the image format which must be one of the formats supported. (see 
			TCameraInfo::iImageFormatsSupported)
	
	@param  aIsInfluencePossible
			If True, signals that digital zoom step values may be influenced by some hardware factor like stabilization etc.
			If False, no influence possible.
	
	@leave  KErrNoMemory Out of memory. May leave with any other error code.
		
	@note   This method retrieves the supported digital zoom steps irrespective of any stabilization influence. 
			In case of stabilization etc. influence, the setting function should set the best possible digital zoom value 
			and return error KErrECamDigitalZoomLimited along with dedicated event.
			
	@note	When camera device doesn't support this feature, empty array should be returned and TValueInfo should be ENotActive,
			and the corresponding getter/setters for this feature should not be used.
	
	@note	Implementation recommendation for old methods which are used to retrieve the supported digital zoom values, is
			to provide only safe values suitable in every cases.
	*/
	virtual void GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aSizeIndex, 
					CCamera::TFormat aFormat, TBool& aIsInfluencePossible) const=0;
	
	/**
	Get the digital zoom steps for the video when a particular video frame format, size and rate are specified.
	
	@param  aDigitalZoomSteps
			A reference to an empty array of TInt to hold digital zoom step values for video and multiplied by 
			KECamFineResolutionFactor to retain precision. If list returned is empty, this means feature is not supported.
         	
	@param  aInfo
			A reference to TValueInfo, which establishes the type of the returned data.
			
	@param  aFrameRateIndex
			A value providing the rate index must be in the range 0 to TCameraInfo::iNumVideoFrameRatesSupported-1
	        inclusive.
	
	@param  aSizeIndex
			A value providing the size index which must be in the range 0 to TCameraInfo::iNumVideoFrameSizesSupported-1
			inclusive.
			
	@param  aFormat
			A value providing the format which must be one of the video frame formats supported. (see
	        TCameraInfo::iVideoFrameFormatsSupported)
			
	@param  aIsInfluencePossible
			If True, signals that digital zoom step values may be influenced by some hardware factor like stabilization etc.
			If False, no influence possible.
			
	@param  aExposure
			The exposure mode.
	
	@leave  KErrNoMemory Out of memory. May leave with any other error code.
		
	@note   This method retrieves the supported digital zoom steps irrespective of any stabilization influence. 
			In case of stabilization etc. influence, the setting function should set the best possible digital zoom value 
			and return error KErrECamDigitalZoomLimited along with dedicated event.
			
	@note	When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive,
			and the corresponding getter/setters for this feature should not be used.
			
	@note	Implementation recommendation for old methods which are used to retrieve the supported digital zoom values, is
			to provide only safe values suitable in every cases.
	*/
	virtual void GetDigitalZoomStepsForVideoL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aFrameRateIndex, 
					TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible, CCamera::TExposure aExposure) const=0;
	/**
	Retrieves the pre capture warnings supported for a given camera mode
	
	@param  aCameraMode
			Desired camera mode for which the supported pre capture warnings may be retrieved.
			
	@param 	aPreCaptureWarningSupported
			A bitfield of all supported TPreCaptureWarning to be issued in the given camera mode. 
			If no pre capture warning supported for the given camera mode, EPCWNone is retrieved.
			
	@leave  May leave with any error. 
	*/
	virtual void GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::TCameraMode aCameraMode, TInt& aPreCaptureWarningSupported) const=0;
	
	/**
	Subscribe in order to receive event which indicates warnings on occurrence of some specific unfavourable 
	conditions before image/video capture.
	
	@param  aPreCaptureWarning
			A bitfield specifying all the TPreCaptureWarning types to be subscribed for. 
			
	@note	When any of the subscribed warnings (represented by aPreCaptureWarning) get generated by the camera device, 
			event KUidECamEventCameraSettingPreCaptureWarning is issued. TECAMEvent2 class should be used in order to 
			provide the status of every PreCaptureWarning. 
			
	@leave  May leave with any error.
	*/
	virtual void SubscribeToPreCaptureWarningL(TInt aPreCaptureWarning)=0;
	
	/**
	Unsubscribe so that further events are not received when warnings get issued.
	
	@leave  May leave with any error.
	*/
	virtual void UnSubscribePreCaptureWarningL()=0;
	
	/**
	Get the status of every warnings defined.
	
	@param  aPreCaptureWarning
			A reference to the integer - bitfield  representing all the TPreCaptureWarning types issued.
	
	@leave  May leave with any error.
	
	@note   This method may be called after receiving the event KUidECamEventCameraSettingPreCaptureWarning OR
			user may also opt for polling on this.
	*/
	virtual void GetPreCaptureWarningL(TInt& aPreCaptureWarning) const=0;
	
	/**
	Retrieve the different supported AF assistant light. 

	@param  aSupportedAFAssistantLight
	        A reference to integer - bitfield indicating the supported AF assistant light.
		    If EAFAssistantLightOff, this means AF assistant light is not supported. 
		    If EAFAssistantLightManualOn, then manual AF assistant light is supported.  
		    If EAFAssistantLightAuto, auto assistant light is supported. 
		    If combination of EAFAssistantLightManualOn||EAFAssistantLightAuto , then both manual and Auto assistant light are supported.
		    
	@leave  May leave with any error.
	*/
	virtual void GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const=0;
			 
	/**
	Get the type ( and state) of AF assistant light currently set.

	@param  aAFAssistantLight
			A reference to AF assistant light.
			If EAFAssistantLightOff, then manual and auto assistant light are switched off.
			If EAFAssistantLightManualOn, manual assistant light is switched on. 
			If EAFAssistantLightAuto, AF assistant light is set to auto.
			
	@leave  May leave with any error. 
	*/
	virtual void GetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight& aAFAssistantLight) const=0;
	
	/**
	Set a particular type ( and state) of AF assistant light.

	@param  aAFAssistantLight
			Type of AF assistant light to be set.
			If EAFAssistantLightOff, switch off the manual or auto assistant light.
			If EAFAssistantLightManualOn, manually switch on the assistant light. 
			If EAFAssistantLightAuto, camera will automatically switch it on/off as per the conditions.
		   
	@note   Triggers KUidECamEventCameraSettingAFAssistantLight to all MCameraObserver2 clients of the camera. 
			Uses HandleEvent to report the result or any possible error.
	*/	
	virtual void SetAFAssistantLight(CCamera::CCameraAdvancedSettings::TAFAssistantLight aAFAssistantLight)=0; 
	};
	
/** 
Mixin class for implementation by providers of 'advanced camera feature extensions' of the 
Advanced Settings Camera Extension API.

@publishedPartner
@prototype	
*/	
class MCameraAdvancedSettings4
	{
	
public:
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	Retrieves the supported continuous zoom types.

	@param  aSupportedContinuousZoomType
			Retrieves a bitfield of TUint which indicates the supported continuous zoom type as given by 
			CCamera::CCameraAdvancedSettings::TContinuousZoomType
			
	@leave  May leave with any error code.
	*/
	virtual void GetSupportedContinuousZoomTypeL(TUint& aSupportedContinuousZoomType) const=0;
	
	/**
	Retrieves the minimum, current and maximum focal length in millimeters. This information is useful to find out 
	which zoom directions can be used on the fly.
	
	@param  aMinFocalLength
			Minimum focal length if positive. 
			Error value if negative (for example, KErrNotFound if information not available).
			
	@param  aCurrentFocalLength
			Current focal length if positive. 
			Error value if negative (for example, KErrNotFound if information not available).
	
	@param  aMaxFocalLength
			Maximum focal length if positive. 
			Error value if negative (for example, KErrNotFound if information not available).
			
	@leave  May leave with any error code.
	*/	
	virtual void GetFocalLengthInfoL(TInt& aMinFocalLength, TInt& aCurrentFocalLength, TInt& aMaxFocalLength) const=0;
	
	/**
	Retrieves the total number of operation preferences supported by the implementation. Operation preferences are 
	specified in terms of performance vectors, that is, speed, quality, low memory consumption and low power consumption.
	
	@param  aNumOperationPreferenceSupported
			Retrieves the number of operation preferences supported.
	
	@leave  May leave with any error code.
	*/
	virtual void GetNumOperationPreferenceL(TUint& aNumOperationPreferenceSupported) const=0;
	
	/**
	Enumerate the available operation preferences.
	
	@param  aOperationPreferenceIndex
			A particular index which represents an operation preference. The level of different performance vectors may be known 
			through other arguments. This varies from 0 to n-1, where n is given by GetNumOperationPreferenceL(n).
	
	@param  aSpeedLevel
			A TPerformanceLevel which specifies the speed level related to the index aOperationPreferenceIndex.
			
	@param  aQualityLevel
			A TPerformanceLevel which specifies the quality level related to the index aOperationPreferenceIndex.
			
	@param  aLowMemoryConsumptionLevel
			A TPerformanceLevel which specifies the low memory consumption level related to the index aOperationPreferenceIndex.
			The lower the memory consumption, the higher the level.
			
	@param  aLowPowerConsumptionLevel
			A TPerformanceLevel which specifies the low power consumption level related to the index aOperationPreferenceIndex.
			The lower the power consumption, the higher the level.
			
	@note   It is up to the implementation how the performance levels are achieved. For example, shutter opening, JPEQ quality 
			and parallel buffering in case of streamed image output can be controlled in order to provide the desired 
			performance.
			
	@leave  May leave with any error code.
	*/
	virtual void EnumerateOperationPreferenceL(TUint aOperationPreferenceIndex, CCamera::CCameraAdvancedSettings::
						TPerformanceLevel& aSpeedLevel, CCamera::CCameraAdvancedSettings::TPerformanceLevel& aQualityLevel, 
						CCamera::CCameraAdvancedSettings::TPerformanceLevel& aLowMemoryConsumptionLevel, 
						CCamera::CCameraAdvancedSettings::TPerformanceLevel& aLowPowerConsumptionLevel) const=0;
	
	/**
	Set a particular operation preference.
	
	@param  aOperationPreferenceIndex
			An index which reveals a set of levels to be used for performance vectors, that is, speed, quality, low memory 
			consumption and low power consumption.
	
	@note   Event KUidECamEventCameraSettingOperationPreference is used to notify clients about setting an operation preference.
	*/										
	virtual void SetOperationPreference(TUint aOperationPreferenceIndex)=0;
	
	/**
	Get the current operation preference being used.
	
	@param  aOperationPreferenceIndex
			Currently used operation preference index.
						
	@leave  May leave with any error code. 
	*/
	virtual void GetOperationPreferenceL(TInt& aOperationPreferenceIndex) const=0;
	
	/**
	Retrieves the event uids which the underlying implementation supports. The client may use these events notifications as
	milestones in their application.
	
	@param  aSupportedEvents
			Retrieves as array of TUid. Every member of the array represents a supported event uid. These events are ECAM 
			component wide. 
			
	@leave  May leave with any error code.
	
	@note   This method may retrieve unrecognized events which may be introduced later on.
	*/	
	virtual void GetSupportedEventsL(RArray<TUid>& aSupportedEvents) const=0;
	
	/**
	Retrieves the indirect feature changes which occur because of a particular requested feature change.
	Since the camera setting operation is asynchronous in nature, changing a particular camera feature, in certain cases, 
	involves indirectly changing another feature. In order to notify the ECam client about this indirect feature change, 
	event KUidECamEvent2IndirectFeatureChange is issued. After this notification, the client may use this method to retrieve 
	the	full list of indirect feature changes.

	@param  aRequestedSetting
			The actual requested feature change. This uid is supposed to be ECAM component wide and not restricted to 
			advanced camera settings.

	@param  aIndirectFeatureChanges
			An array of uids which retrieves the indirect feature changes. These uids are supposed to be ECAM component wide 
			and not restricted to advanced camera settings.
			
	@leave  May leave with any error code.
	*/
	virtual void GetIndirectFeatureChangesL(TUid aRequestedSetting, RArray<TUid>& aIndirectFeatureChanges) const=0;
	
	/**
	Retrieves the concrete factory handle for the continuous zoom implementation.
		   
	@param  aImplFactoryPtr
		    The concrete factory handle for the continuous zoom implementation.
		   
	@leave  May leave with any error code.
	*/
	virtual void CreateContinuousZoomImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	};

/** 
Mixin class for implementation by providers of the Presets Camera Extension API.

This API is used to simplify user - camera interaction by allowing simultaneous
setting of various advanced camera hardware settings using a single parameter.

A preset is identified by a single UID and relates to a known predefined outcome. 
For example the 'Night' Preset is used to set the camera into a night mode 
so that the user can take photos in dark conditions. 

The settings associated with a particular preset and their specific values and ranges 
are specific to each type of camera hardware and are therefore not defined by the API. 

@publishedPartner
@released 	
*/
class MCameraPresets 
	{
	
public:

	/** 
	Releases the interface. 
	*/
    virtual void Release()=0; 
    
	/** 
	Gets the presets supported by the device. These are identified by UIDs 
	and relate to a known expected outcome.
	The settings associated with a particular preset and their specific values and ranges are 
	specific to each type of camera hardware and are therefore not defined by the API. 
	
	@param  aPresets
	        An empty array of TUids which the API implementation must populate with the specific supported preset values. If the array is empty on return, 
        	the camera does not support presets.   
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
    virtual void GetSupportedPresetsL(RArray<TUid>& aPresets) const=0;
    
    /** 
	Sets a specific preset supported by the camera.  
	All clients, implementing the MCameraObserver2 interface will receive a notification 
	with the UID of the specific preset, signalling a new preset has been selected.
	
	@param  aPreset 
	        The UID of the new requested preset.
	
	@note A particular preset is only active if it was established (through a call to 
	MCameraPresets ::SetPreset()) as the most recent operation affecting camera settings. 
	If some other camera setting is changed after calling MCameraPresets ::SetPreset() 
	then the preset is no longer active. 
	*/
    virtual void SetPreset(TUid aPreset)=0;
    
    /** 
	Gets the preset that was established by the last call to MCameraPresets ::SetPreset()
	
	@return  The UID of the preset. If there is no active preset then the 
	         returned value is KNullUid.
	         
	@note A particular preset is only active if it was established (through a call to 
	MCameraPresets ::SetPreset()) as the most recent operation affecting camera settings. 
	If some other camera setting is changed after calling MCameraPresets ::SetPreset() 
	then the preset is no longer active. 
	*/
    virtual TUid Preset() const=0;
    
    /**
    Gets all settings affected by the current preset. All settings that are related to 
    the preset in question will be included in the list, including any whose values 
    have not changed as a result of applying the preset.
    
    @param  aSettings
	        An empty array of TUids which the API implementation must populate with the specific settings.
	
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
    virtual void GetAffectedSettingsL(RArray<TUid>& aSettings) const=0;
   
    /**
    Gets all settings associated with a specific preset. This function does not require a preset to have been set prior the call as in 
    GetAffectedSettingsL() function.
    The returned array will contain the UIDs of all settings which are associated and 
    potentially affected by that particular preset.
    
    @param  aPreset 
            the UID of the preset in question.
            
    @param  aSettings
	        An empty array of TUids which would be populated by the implementation with 
	        the UIDs of the settings associated with that preset. 
	
	@leave KErrArgument if the preset UID is not recognised 
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
    virtual void GetAssociatedSettingsL(TUid aPreset, RArray<TUid>& aSettings) const=0;
	};
	
/**
Mixin class for implementation of extra methods of the Presets Camera Extension API.

@publishedPartner
@prototype
*/
class MCameraPresets2 
	{
	
public:

	/** 
	Releases the interface. 
	*/
    virtual void Release()=0; 
    
    /**
    Retrieves those settings for which ranges have been restricted in order to let the camera work in a given preset mode.
    The client will be notified of range restrictions through uid KUidECamEventRangeRestricted. After receiving this 
    notification, the client may use this method to retrieve those settings whose ranges have been restricted.
    
    @param  aRangeRestrictedSettings
    		An array of uid which represents those settings whose ranges have been restricted. These settings are ECam 
    		component wide. For each of the settings, the client can query about the restricted range as per the usual way.
    
    @leave  May leave with any error code.
    */
    virtual void GetRangeRestrictedSettingsL(RArray<TUid>& aRangeRestrictedSettings) const=0;
    
    /**
    Retrieves those settings which have been restricted (settings no longer supported) in order to let the camera work in a given preset mode.
    The client will be notified of feature restrictions through uid KUidECamEventFeatureRestricted. After receiving this 
    notification, the client may use this method to retrieve these settings.
    
    @param  aFeatureRestrictedSettings
    		An array of uid which represents those settings which have been restricted. These settings are ECam 
    		component wide.
    
    @leave  May leave with any error code.
    */
    virtual void GetFeatureRestrictedSettingsL(RArray<TUid>& aFeatureRestrictedSettings) const=0;
    
    /**
    Retrieves information about whether the preset unlock feature is supported or not. Unlocking the preset helps in making some 
    further changes in the setting after the camera works in a particular preset mode
    
    @param  aUnlockSupported
    		ETrue indicates preset unlock feature is supported.
    		EFalse indicates preset lock feature is not supported.
    		
    @leave  May leave with any error code.
    */
    virtual void IsPresetUnlockSupportedL(TBool& aUnlockSupported) const=0;
	
	/**
	Locks the preset for any further setting changes.

	@note   Event KUidECamEventPresetLocked is used to notify clients that the preset has been locked.
	*/
	virtual void LockPreset()=0;
	
	/**
	Unlocks the preset to apply further setting changes.

	@note   Event KUidECamEventPresetUnlocked is used to notify clients that the preset has been unlocked.
	*/
	virtual void UnlockPreset()=0;
	};

/**
Mixin class for implementation of extra methods of the Continuous Zoom API.

@internalTechnology
@prototype
*/
class MCameraContinuousZoom
	{
public:

	/**
	Passes both the continuous zoom observer and pointer to the continuous zoom class to the implementation so callbacks 
	can be sent to the client along with the continuous zoom handle.
		
	@param  aObserver
			The reference to the continuous zoom observer.
		
	@param  aContinuousZoomHandle
			The pointer to the continuous zoom class object.
				
	@see    MContinuousZoomObserver
	*/
	virtual void SetContinuousZoomObserverAndHandle(MContinuousZoomObserver& aObserver, CCamera::CCameraContinuousZoom* aContinuousZoomHandle)=0;

	/**
	Starts the continuous zoom operation. Clients will receive MContinuousZoomObserver::ContinuousZoomProgress() callback for intermediate zoom factors
	achieved. It is up to the implementation to choose the zoom factors for which it will issue this callback. Implementation will not use this callback to notify
	reaching the target zoom factor. This will be done via MContinuousZoomObserver::ContinuousZoomComplete() callback.

	@param  aContinuousZoomParameters
			The desired parameters to be used for the continuous zoom operation.

	@note  If the implementation does not support re-configuring of zoom parameters whilst an existing continuous zoom operation is active then
		   StartContinuousZoomL() will leave with KErrInUse.

	@note  If client has selected EDirectionTele zoom direction and the current zoom factor is greater than the target zoom factor, StartContinuousZoomL()
		   will leave with KErrArgument. Similarly, StartContinuousZoomL() will also leave with KErrArgument if client has selected EDirectionWide zoom
		   direction and current zoom factor is less than target zoom factor.

	@leave  May leave with any error code.
	*/
	virtual void StartContinuousZoomL(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters aContinuousZoomParameters)=0;

	/**
	Stop any exisiting continuous zoom operation.
	Since this method is synchronous, no callback shall be issued for the concerned continuous zoom operation.
	*/
	virtual void StopContinuousZoom()=0;

	/**
	Retrieves information about the supported settings related to continuous zoom support.

	@param  aContinuousZoomInfo
			The information of supported continuous zoom functionality.

	@leave  May leave with any error code.
	*/
	virtual void GetContinuousZoomSupportInfoL(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo& aContinuousZoomInfo) const=0;

	/**
	Retrieves the unique id of the continuous zoom object.
	This is used to identify the continuous zoom handle returned to clients via the MContinuousZoomObserver callback.

	@param  aZoomId
			The unique id of this Continuous Zoom object.
	*/
	virtual void GetContinuousZoomId(TInt& aZoomId) const=0;

	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	};

#endif // ECAMADVSETTINGSINTF_H
