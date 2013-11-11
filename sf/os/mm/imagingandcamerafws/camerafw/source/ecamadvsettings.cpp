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

#include <ecam/ecamadvsettingsintf.h>
#include "ecamversion.h"
#include <ecam/ecamconstants.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsconst.h>
#include <ecamadvsettingsdef.h>
#endif
#include <ecam/implementationfactoryintf.h>
const TInt KBaselinedFocusRanges = 0x1F;
const TInt KBaselinedWhiteBalanceModes = 0x01FF;
const TUint KBaselinedPixelAspectsMask = (CCamera::CCameraAdvancedSettings::EEPixelAspect59To54 << 1) - 1;
const TUint KBaselinedFlashModeMask = (CCamera::EFlashManual << 1) - 1;
const TUint KBaselinedPreCaptureWarning = (CCamera::CCameraAdvancedSettings::EPCWGeneralWarning << 1) - 1;
const TUint KBaselinedDriveMode = (CCamera::CCameraAdvancedSettings::EDriveModeBurst << 1) - 1;

/**
Factory function for creating the CCameraPresets object.

@param aCamera 
	   a reference to a CCamera object providing the settings.

@return a pointer to a fully constructed CCameraPresets object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.

@note  Clients using MCameraObserver are not recommended to use this extension class since they cannot handle events.
*/
EXPORT_C CCamera::CCameraPresets* CCamera::CCameraPresets::NewL(CCamera& aCamera)
	{
	CCamera::CCameraPresets* self = new (ELeave)CCamera::CCameraPresets(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self; 
	}
	
/**	
CCameraPresets Constructor.

@param aOwner
       a reference to a CCamera object providing the settings. 
*/
EXPORT_C CCamera::CCameraPresets::CCameraPresets(CCamera& aOwner):iOwner(aOwner), iImpl(NULL), iImpl2(NULL)
	{
	}

/**
CCameraPresets second phase constructor. 

Function used to initialise internal state of the object. Uses reference to the camera to retrieve 
Camera presets interface pointer.

@leave KErrNoMemory Out of memory. 
*/ 
EXPORT_C void CCamera::CCameraPresets::ConstructL() 
	{
	iImpl = static_cast<MCameraPresets*>(iOwner.CustomInterface(KECamMCameraPresetsUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
	
	iImpl2 = static_cast<MCameraPresets2*>(iOwner.CustomInterface(KECamMCameraPresets2Uid));
	}

/**
Destructor
*/	
EXPORT_C CCamera::CCameraPresets::~CCameraPresets()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	if (iImpl2 != NULL)
		{
		iImpl2->Release();	
		}
	}	
     
/** 
Gets the presets supported by the camera. These are identified by UIDs 
and relate to a known predefined outcome.
The settings associated with a particular preset and their specific values 
and ranges are specific to each type of camera hardware and are therefore not defined by the API. 

@param  aPresets
        An empty array of TUids which would be populated by the implementation with 
        the specific supported values. If the array is empty on return, 
    	the camera does not support presets.   

@leave KErrNoMemory Out of memory. 

@note   if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added uid values (new presets added). So, any extra uid value(unrecognised)
	    passed from the implementation will be filtered at this point.
	    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values.
	    
@note   Any preset uid, if added in future, has to have a uid value greater than KUidECamPresetFactoryDefaultUidValue
	    otherwise unrecognized preset uids could not be checked.
*/
EXPORT_C void CCamera::CCameraPresets::GetSupportedPresetsL(RArray<TUid>& aPresets) const
	{
	iImpl->GetSupportedPresetsL(aPresets);
	
    /* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new presets added). So, any extra uid value(unrecognised)
    passed from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
    FilterUnrecognisedUids(aPresets, KUidECamPresetFactoryDefaultUidValue);
	}
	
/** 
Sets a specific preset supported by the device.  
All clients, implementing the MCameraObserver2 interface will receive a notification 
with the UID of the specific preset, signalling a new preset has been selected.

@param  aPreset 
        The UID of the new requested preset.
*/
EXPORT_C void CCamera::CCameraPresets::SetPreset(TUid aPreset)
	{
	iImpl->SetPreset(aPreset);	
	}

/** 
Gets the current preset set on the camera.

@return  The UID of the current preset. If there is no active preset then the 
         returned value is KNullUid. 

@note   if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that application is not 
		prepared to receive extra added uid values (new presets added). So, any extra uid(unrecognised) value received from the 
		implementation will be mapped to KUidECamPresetFactoryDefault at this point.
	    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values
*/
EXPORT_C TUid CCamera::CCameraPresets::Preset() const
	{
	TUid preset = iImpl->Preset();
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that application is not 
	prepared to receive extra added uid values (new presets added). So, any extra uid(unrecognised) value received from the 
	implementation will be mapped to KUidECamPresetFactoryDefault at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		if(preset.iUid > KUidECamPresetFactoryDefaultUidValue)
			{
			preset = KUidECamPresetFactoryDefault;
			}
		}
	
	return preset;	
	}
	
/**
Get all settings affected by the current preset. This is to say that all settings which 
are related to the preset in question will be included in the list, even though the value might 
not have changed. 

@param  aSettings
        An empty array of TUids which would be populated by the implementation with 
        the specific settings.

@leave KErrNoMemory Out of memory. 

@note   if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
	    from the implementation will be filtered at this point.
	    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values
*/
EXPORT_C void CCamera::CCameraPresets::GetAffectedSettingsL(RArray<TUid>& aSettings) const
	{
	iImpl->GetAffectedSettingsL(aSettings);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
    FilterUnrecognisedUids(aSettings, KUidECamEventCameraSettingAutoFocusType2UidValue);
	}
	
/**
Get all settings associated with a specific preset, identified by a UID. 
This function does not require a preset to have been set prior the call as in 
GetAffectedSettingsL() function.
The returned array will contain the UIDs of all settings which are associated and 
potentially affected by that particular preset.

@param  aPreset 
        the UID of the preset in question.
        
@param  aSettings
        An empty array of TUids which would be populated by the implementation with 
        the UIDs of the settings associated with that preset. 

@leave KErrArgument If the provided preset UID is not recognised.
@leave KErrNoMemory Out of memory. 

@note  if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
	   from the implementation will be filtered at this point.
	   To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values
*/
EXPORT_C void CCamera::CCameraPresets::GetAssociatedSettingsL(TUid aPreset, RArray<TUid>& aSettings) const
	{
	iImpl->GetAssociatedSettingsL(aPreset, aSettings);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
    FilterUnrecognisedUids(aSettings, KUidECamEventCameraSettingAutoFocusType2UidValue);
	}

void CCamera::CCameraPresets::FilterUnrecognisedUids(RArray<TUid>& aUids, const TInt aBaselineUid) const
	{
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aUids.Count(); index++)
			{
			/** aBaselineUid is the baseline. Any uid with greater uid value means that it has 
			been added in later versions */
			if(aUids[index].iUid > aBaselineUid)
				{
				aUids.Remove(index);
				index--;
				}
			}
		}
	}
	
/**
Retrieves those settings for which ranges have been restricted in order to let the camera work in a given preset mode.
The client will be notified of range restrictions through ECAM event KUidECamEventRangeRestricted. After receiving this 
notification, the client may use this method to retrieve those settings whose ranges have been restricted.

@param  aRangeRestrictedSettings
		An array of uids which represents those settings whose ranges have been restricted. These settings are ECam 
		component wide. For each of the settings, the client can query about the restricted ranges in the usual way.
		Empty array indicates that there are no range restricted settings for the given preset.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraPresets::GetRangeRestrictedSettingsL(RArray<TUid>& aRangeRestrictedSettings) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetRangeRestrictedSettingsL(aRangeRestrictedSettings);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves those settings which have been restricted (settings no longer supported) in order to let the camera work in 
a given preset mode. 
The client will be notified of feature restrictions through ECAM event KUidECamEventFeatureRestricted. After receiving 
this notification, the client may use this method to retrieve these settings.

@param  aFeatureRestrictedSettings
		An array of uids which represents those settings which have been restricted. These settings are ECam 
		component wide. Empty array indicates that there are no settings which have been been restricted for 
		the given preset.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraPresets::GetFeatureRestrictedSettingsL(RArray<TUid>& aFeatureRestrictedSettings) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetFeatureRestrictedSettingsL(aFeatureRestrictedSettings);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves information about whether the preset unlock feature is supported or not. Unlocking the preset helps in making
further setting changes after the camera works in a particular preset mode.

@param  aUnlockSupported
		ETrue indicates preset unlock feature is supported.
		EFalse indicates preset lock feature is not supported.
		
@leave  May leave with any error code.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraPresets::IsPresetUnlockSupportedL(TBool& aUnlockSupported) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->IsPresetUnlockSupportedL(aUnlockSupported);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}		
	}

/**
Locks the preset for any further setting changes.

@note   Event KUidECamEventPresetLocked is used to notify clients that the preset has been locked.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraPresets::LockPresetL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->LockPreset();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Unlocks the preset to apply further setting changes.

@note   Event KUidECamEventPresetUnlocked is used to notify clients that the preset has been unlocked.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraPresets::UnlockPresetL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->UnlockPreset();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Factory function for creating the CCameraAdvancedSettings object.

@param aCamera 
	a reference to a CCamera object providing the settings.

@return a pointer to a fully constructed CCameraAdvancedSettings object.

@leave KErrNoMemory Out of memory or any other system-wide error code.

@note  Clients using MCameraObserver are not recommended to use this extension class since they cannot handle events.
*/
EXPORT_C CCamera::CCameraAdvancedSettings* CCamera::CCameraAdvancedSettings::NewL(CCamera& aCamera)
	{
	CCamera::CCameraAdvancedSettings* self = new (ELeave)CCamera::CCameraAdvancedSettings(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self; 
	}
	
/**	
CCameraAdvancedSettings Constructor.

@param aOwner
       a reference to a CCamera object providing the settings. 
*/
EXPORT_C CCamera::CCameraAdvancedSettings::CCameraAdvancedSettings(CCamera& aOwner) : iOwner(aOwner), iImpl(NULL), iImpl2(NULL), iImpl3(NULL), iImpl4(NULL)
	{
	}

/**
CCameraAdvancedSettings second phase constructor.
 
Function used to initialise internal state of the object. Uses reference to the camera to retrieve 
Camera advanced settings interface pointer.

@leave KErrNoMemory Out of memory.
*/ 
EXPORT_C void CCamera::CCameraAdvancedSettings::ConstructL() 
	{
	iImpl = static_cast<MCameraAdvancedSettings*>(iOwner.CustomInterface(KECamMCameraAdvancedSettingsUid));
	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
	iImpl2 = static_cast<MCameraAdvancedSettings2*>(iOwner.CustomInterface(KECamMCameraAdvancedSettings2Uid));
	iImpl3 = static_cast<MCameraAdvancedSettings3*>(iOwner.CustomInterface(KECamMCameraAdvancedSettings3Uid));
	iImpl4 = static_cast<MCameraAdvancedSettings4*>(iOwner.CustomInterface(KECamMCameraAdvancedSettings4Uid));
	}
	
/**
Destructor
*/	
EXPORT_C CCamera::CCameraAdvancedSettings::~CCameraAdvancedSettings()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	if (iImpl2 != NULL)
		{
		iImpl2->Release();	
		}
	if (iImpl3 != NULL)
		{
		iImpl3->Release();	
		}
	if (iImpl4 != NULL)
		{
		iImpl4->Release();	
		}
	}

/** 
Gets the type of this camera. 
@see TCameraType

@return the type of this camera 
*/	
EXPORT_C CCamera::CCameraAdvancedSettings::TCameraType CCamera::CCameraAdvancedSettings::CameraType() const
	{
	return iImpl->CameraType();
	}
	
/** 
Get the type of a specific camera denoted by its index. A pluggable camera
may not necessarily be physically present. The type denotes whether the slot allocated
to that index is for pluggable or onboard camera. 
@see TCameraType

@param aCameraIndex
       An integer in the range of [0: CCamera::CamerasAvailable()-1].
       
@return the TCameraType value for the specific camera. 
        If the index is out of range, the return value is ECameraUnknown.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TCameraType CCamera::CCameraAdvancedSettings::CameraType(TInt aCameraIndex) 
	{
	return iImpl->CameraType(aCameraIndex);
	}
	
/** 
Checks whether the current camera is present.
       
@return Whether the camera is currently present. 
		ETrue if camera is present, EFalse otherwise. 
		For example ECameraOnBoard (built-in) cameras are always present.   
*/	
EXPORT_C TBool CCamera::CCameraAdvancedSettings::IsCameraPresent() const
	{
	return iImpl->IsCameraPresent();
	}
	
/** 
Checks whether the camera, denoted by its index, is currently present.
The index uniquely identifies the camera on the device.

@param aCameraIndex
       An integer in the range of [0:CCamera::CamerasAvailable()-1] specifying the
       camera device to use
       
@return Whether the camera is currently present. 
		ETrue if camera is present, EFalse otherwise. 
		For example  built-in (ECameraOnBoard) cameras 
		are always present. 
*/	
EXPORT_C TBool CCamera::CCameraAdvancedSettings::IsCameraPresent(TInt aCameraIndex) 
	{
	return iImpl->IsCameraPresent(aCameraIndex);
	}
	
/**
Gets current camera index. The index uniquely identifies the camera on the device.
 
@return camera index in the inclusive range of [0:CCamera::CamerasAvailable() - 1].
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::CameraIndex() const
	{
	return iImpl->CameraIndex(); 
	}
	
/** 
Gets all of the supported stabilization modes on the camera. The result is a bitfield
of the valid TStabilizationMode flags. 
@see TStabilizationMode

@return a bitfield of all supported stabilization modes. 
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedStabilizationModes() const
	{
	return iImpl->SupportedStabilizationModes();
	}
	
/** 
Gets current stabilization mode on the camera. 
The result is a valid TStabilizationMode value. 

@return current stabilization mode of type TStabilizationMode. 
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TStabilizationMode CCamera::CCameraAdvancedSettings::StabilizationMode() const
	{
	return iImpl->StabilizationMode();
	}
	
/** 
Sets a specific stabilization mode on the camera.

Stabilization mode change fires a KUidECamEventCameraSettingStabilizationMode
event to all MCameraObserver2 clients of this specific camera.

@param aStabilizationMode
       new stabilization mode of TStabilizationMode type.
*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::SetStabilizationMode(CCamera::CCameraAdvancedSettings::TStabilizationMode aStabilizationMode)
	{
	iImpl->SetStabilizationMode(aStabilizationMode);
	}
	
/** 
Gets all of the supported focus modes on the camera. The result is a bitfield
of the valid TFocusMode flags. 
@see TFocusMode 

@return a bitfield of all supported focus modes. 
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedFocusModes() const
	{
	return iImpl->SupportedFocusModes();
	}
	
/** 
Gets current focus mode on the camera. 
The result is a valid TFocusMode value. 

@return current focus mode. 
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TFocusMode CCamera::CCameraAdvancedSettings::FocusMode() const
	{
	return iImpl->FocusMode();
	}
	
/** 
Sets a specific focus mode on the camera.
Focus mode change fires a KUidECamEventCameraSettingFocusMode event
to all MCameraObserver2 clients of the camera.

@param aFocusMode 
       new focus mode of TFocusMode type.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode)
	{
	iImpl->SetFocusMode(aFocusMode);
	}
	
/** 
Gets all supported focus ranges on the camera.

@return an integer - a bitfield of all supported TFocusRange values.

@note  if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value passed 
	   from the implementation will be filtered at this point.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values 
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedFocusRanges() const
	{
	TInt supportedFocusRanges = iImpl->SupportedFocusRanges();
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value passed 
	   from the implementation will be filtered at this point.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		supportedFocusRanges &= KBaselinedFocusRanges;
		}
	
	return supportedFocusRanges;
	}
    
/** 
Gets current focus range on the camera.

@return the current TFocusRange value. 

@note  if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value received 
	   from the implementation will be dropped and EFocusRangeAuto would be passed instead.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values
*/	
EXPORT_C CCamera::CCameraAdvancedSettings::TFocusRange CCamera::CCameraAdvancedSettings::FocusRange() const
	{
	CCamera::CCameraAdvancedSettings::TFocusRange focusRange = iImpl->FocusRange();
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value received 
	   from the implementation will be dropped and EFocusRangeAuto would be passed instead.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		if(static_cast<TInt>(focusRange) > KBaselinedFocusRanges)
			{
			focusRange = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
			}
		}
	
	return focusRange;
	}
	
/** 
Sets a specific focus range on the camera.
The focus range change fires both, KUidECamEventCameraSettingFocusRange and 
KUidECamEventCameraSettingFocusRange2 event to all MCameraObserver2 clients of the camera.
@see KUidECamEventCameraSettingFocusRange

@param aFocusRange
       newly selected focus range.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange)
	{
	iImpl->SetFocusRange(aFocusRange);
	}
	
/** 
Gets all supported auto focus types on the camera.

@return an integer - a bitfield of all supported TAutoFocusType values. 
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedAutoFocusTypes() const
	{
	return iImpl->SupportedAutoFocusTypes();
	}

/** 
Gets current auto focus type on the camera. 
@see TAutoFocusType

@return a CCamera::TAutoFocusType value. 
*/	
EXPORT_C CCamera::CCameraAdvancedSettings::TAutoFocusType CCamera::CCameraAdvancedSettings::AutoFocusType() const
	{
	return iImpl->AutoFocusType();
	}
	
/** 
Sets a specific auto focus type on the camera.
The focus type change fires both, KUidECamEventCameraSettingAutoFocusType and 
KUidECamEventCameraSettingAutoFocusType2 event to all MCameraObserver2 clients of the camera.
@see KUidECamEventCameraSettingAutoFocusType

@param aAutoFocusType
       Autofocus selection.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType aAutoFocusType)
	{
	iImpl->SetAutoFocusType(aAutoFocusType);
	}
	
/** 
Gets all supported auto focus areas on the camera.

@return an integer - a bitfield of al supported TAutoFocusArea values. 
*/	
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedAutoFocusAreas() const
	{
	return iImpl->SupportedAutoFocusAreas();
	}
	
/** 
Gets current chosen auto focus area on the camera.

@return a CCamera::TAutoFocusArea value. 
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TAutoFocusArea CCamera::CCameraAdvancedSettings::AutoFocusArea() const
	{
	return iImpl->AutoFocusArea();
	}
	
/** 
Sets a specific auto focus area on the camera.
Focus area change fires a KUidECamEventCameraSettingAutoFocusArea event
to all MCameraObserver2 clients of the camera.

@param aAutoFocusArea
       Autofocus area selection.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetAutoFocusArea(CCamera::CCameraAdvancedSettings::TAutoFocusArea aAutoFocusArea)
	{
	iImpl->SetAutoFocusArea(aAutoFocusArea);
	}
	
/**
Get focus distance in millimetres.

@return the current focus distance in millimetres, directly from user setting of lenses.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::FocusDistance() const
	{
	return iImpl->FocusDistance();
	}

/**
Set focus distance in millimetres. Focus distance change fires a KUidECamEventCameraSettingFocusDistance event
to all MCameraObserver2 clients of the camera.

@param aDistance 
	   the new distance value in millimetres.

@note  KUidECamEvent2CameraSettingFocusDistance may be used to provide the focussing feedback as well. This means that 
	   the feedback will state whether the proper focussing has been achieved after setting the given focus distance.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetFocusDistance(TInt aDistance)
	{
	iImpl->SetFocusDistance(aDistance);
	}
	
/**
Get minimum focus distance in millimetres 

@return the minimum (35 camera equivalent) focal length of a camera. 
@note Current Focal length is calculated as focalLength = opticalZoom * minFocalLength;  
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::GetMinFocalLength() const
	{
	return iImpl->GetMinFocalLength();
	}
	
/**
Gets the set of camera supported ISO rates.

@param aSupportedIsoRates
       an array of integers which gets filled with the supported ISO rates. 
       
@note  When camera device is incapable of revealing the ISO rates supported, it has to be assumed that 
	   camera will work only on the permanently set value. If this value is not known, empty array may be 
	   returned; corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const
	{
	iImpl->GetSupportedIsoRatesL(aSupportedIsoRates);
	}
	
/**
Gets current ISO rate.

@return current ISO rate as a TInt value. 
		Negative value returned means error case (system wide error code) and positive value means current ISO rate.

@note The returned value may be checked with the list of supported ISO rates. If value returned does not belong to 
	  this list, then it may be treated as an error case.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::IsoRate() const
	{
	return iImpl->IsoRate();
	}

/**
Set current ISO rate for the camera.
Triggers KUidECamEventCameraSettingIsoRate to all MCameraObserver2 clients of the camera.

@param aRate
	   required new ISO rate.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetIsoRate(TInt aRate)
	{
	iImpl->SetIsoRate(aRate);
	}
	
/** 
Gets the current discrete aperture steps (F-stops) supported by the camera.

@param  aFStops
        A reference to an empty array of TInt which would be populated by the implementation with 
        the specific supported values. If the array is empty on return, 
        the camera supports all integer values in the aperture range. Each value is multiplied by 
        a factor of KECamFineResolutionFactor.

@param  aInfo 
        a reference to TValueInfo, which establishes the type of the returned data.     

@leave KErrNoMemory Out of memory. 

@note  When camera device is incapable of revealing the aperture openings supported, it has to be assumed that 
	   camera will work only on the parmanently set value. If this value is not known, empty array may be 
	   returned and TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetAperturesL(RArray<TInt>& aFStops, TValueInfo& aInfo) const
	{
	iImpl->GetAperturesL(aFStops, aInfo);
	}

/** 
Get current aperture value.
The default aperture value is ECAM implementation specific and could be either auto aperture or any other supported value.
 
@return Current aperture value as an integer, multiplied by KECamFineResolutionFactor. 
	    For example the function will return 280 for the actual aperture of F2.8. 
	    Negative value returned means error case (system wide error code) and positive value means current aperture.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::Aperture() const
	{
	return iImpl->Aperture();
	}

/** 
Set a new aperture value. 
All MCameraObserver2 clients of the camera receive a KUidECamEventCameraSettingAperture 
event notification when aperture value is changed.

@param aFStop 
	   a new aperture value in the supported by the camera range.       

@note The aperture parameter value is an integer, multiplied by KECamFineResolutionFactor. 
	  For example to set an aperture of F2.8, call SetAperture(280).

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetAperture(TInt aFStop) 
	{
	iImpl->SetAperture(aFStop); 
	}

/**
Gets the set of supported shutter speeds.

@param  aShutterSpeeds
		a reference to an RArray of TInt representing the discrete shutter speeds supported 
		currently by the camera.

@param  aInfo 
		a reference to TValueInfo, which establishes the type of the returned data.     

@return the populated array with all shutter speeds in microseconds. 

@leave KErrNoMemory Out of memory. 

@note  When camera device is incapable of revealing the shutter speeds supported, it has to be assumed that 
	   camera will work only on the parmanently set value. If this value is not known, empty array may be 
	   returned and TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetShutterSpeedsL(RArray<TInt>& aShutterSpeeds, TValueInfo& aInfo) const 
	{
	iImpl->GetShutterSpeedsL(aShutterSpeeds, aInfo);
	}

/**
Gets the current shutter speed

@return the current shutter speed in microseconds. 
		Negative value returned means error case (system wide error code) and positive value means current shutter speed.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::ShutterSpeed() const 
	{
	return iImpl->ShutterSpeed();
	}

/**
Sets the current shutter speed. When set, all MCameraObserver2 clients of the camera
receive a KUidECamEventCameraSettingShutterSpeed event

@param aShutterSpeed
       the required shutter speed in microseconds.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetShutterSpeed(TInt aShutterSpeed) 
	{
	iImpl->SetShutterSpeed(aShutterSpeed); 
	}

/**
Get all supported metering modes on this camera represented as bitfield of type TMeteringMode.

@return the set of supported metering modes.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedMeteringModes() const
	{
	return iImpl->SupportedMeteringModes();
	}

/**
Get current metering mode.

@return a value of type TMeteringMode.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TMeteringMode CCamera::CCameraAdvancedSettings::MeteringMode() const
	{
	return iImpl->MeteringMode();
	}

/**
Set the current metering mode. When set, all MCameraObserver2 clients are notified 
with a KUidECamEventCameraSettingMeteringMode event. 

@param aMeteringMode
		new selection for metering mode of type TMeteringMode.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetMeteringMode(CCamera::CCameraAdvancedSettings::TMeteringMode aMeteringMode)
	{
	iImpl->SetMeteringMode(aMeteringMode);
	}

/** 
Get all supported drive modes as bitfields of TDriveMode type. 

@return the set of supported drive modes.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedDriveModes() const
	{
	TInt supportedDriveModes = iImpl->SupportedDriveModes();

	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
  	application is not prepared to receive extra added enum values. So, any extra enum value passed 
   	from the implementation will be filtered at this point.
   	To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
   	to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		supportedDriveModes &= KBaselinedDriveMode;
		}

	return supportedDriveModes;
	}

/** 
Gets currently active drive mode.

@return current drive mode.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TDriveMode CCamera::CCameraAdvancedSettings::DriveMode() const
	{
	CCamera::CCameraAdvancedSettings::TDriveMode driveMode = iImpl->DriveMode();

	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value received 
	   from the implementation will be dropped and EDriveModeAuto would be passed instead.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		if(static_cast<TUint>(driveMode) > KBaselinedDriveMode)
			{
			driveMode = CCamera::CCameraAdvancedSettings::EDriveModeAuto;
			}
		}
	
	return driveMode;
	}

/**
Set the current metering mode. When set all MCameraObserver2 clients  are notified with 
KUidECamEventCameraSettingDriveMode. 

@param aDriveMode
	   new selection for drive mode value of type TDriveMode.

@note  Unless reduced Latency scheme is not used (ie CaptureImageL(TInt aSequenceNumber) or StartPerformantVideoCaptureL())
	   if an image/video capture is still outstanding, this method may report error KErrInUse.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetDriveMode(CCamera::CCameraAdvancedSettings::TDriveMode aDriveMode)
	{
	iImpl->SetDriveMode(aDriveMode);
	}

/**
Get all supported bracket modes as bitfields.

@return the set of all supported bracket modes as an integer.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedBracketModes() const
	{
	return iImpl->SupportedBracketModes();
	}

/**
Get current bracket mode.

@return the current bracket mode TBracketMode.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TBracketMode CCamera::CCameraAdvancedSettings::BracketMode() const
	{
	return iImpl->BracketMode();
	}

/**
Set new bracket mode. All MCameraObserver2 clients are notified with 
KUidECamEventCameraSettingBracketMode. 

@param aBracketMode
	   new selection for bracket mode of type TBracketMode.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetBracketMode(CCamera::CCameraAdvancedSettings::TBracketMode aBracketMode)
	{
	iImpl->SetBracketMode(aBracketMode);
	}

/**
Get all supported bracket parameters as bitfields.

@return the set of all currently supported bracket modes.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedBracketParameters() const
	{
	return iImpl->SupportedBracketParameters();
	}

/**
Get current bracket parameter.

@return the current bracket mode TBracketParameter.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TBracketParameter CCamera::CCameraAdvancedSettings::BracketParameter() const
	{
	return iImpl->BracketParameter();
	}

/**
Set new bracket parameter
When set all clients are notified with 
KUidECamEventCameraSettingBracketParameter. 

@param aBracketParameter
	   new selection for parameter type of type TBracketParameter.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetBracketParameter(CCamera::CCameraAdvancedSettings::TBracketParameter aBracketParameter)
	{
	iImpl->SetBracketParameter(aBracketParameter);
	}

/**
Get all supported bracket steps as bitfields.

@return the set of all supported bracket modes.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedBracketSteps() const
	{
	return iImpl->SupportedBracketSteps();
	}

/**
Get current bracket step.

@return the current bracket mode TBracketStep.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TBracketStep CCamera::CCameraAdvancedSettings::BracketStep() const
	{
	return iImpl->BracketStep();
	}

/**
Set new bracket step. All MCameraObserver2 clients are notified with 
KUidECamEventCameraSettingBracketStep. 

@param aBracketStep
	   new selection for step of type TBracketStep.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetBracketStep(CCamera::CCameraAdvancedSettings::TBracketStep aBracketStep)	
	{
	iImpl->SetBracketStep(aBracketStep);
	}

/** 
Gets the settings for which frames to merge. Valid only in EDriveModeBracketMerge mode. 
@note there must be at least two images to merge. All are assumed to form a sequence and 
	are identified using the first frame index and number of frames e.g. to  merge two frames - one 
	on and one +1, when in EBracketMode3Image, one sets the start index to 1 and frames to two.
@note It is very much TBracketMode setting dependent operation. 

@param  aStartIndex
		the index of the start frame, starts from 0.
		
@param  aFrames
		the number of frames to be merged.		
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const	
	{
	iImpl->GetBracketMerge(aStartIndex, aFrames);
	}

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
EXPORT_C void CCamera::CCameraAdvancedSettings::SetBracketMerge(TInt aStartIndex, TInt aFrames)
	{
	iImpl->SetBracketMerge(aStartIndex, aFrames);
	}
	
/**
Get camera all supported flash modes CCamera::TFlash

@return the set of all supported flash modes as bitfields in an integer.
@note Some of the flash modes are available only for clients using either CCamera::New2L() or CCamera::NewDuplicate2L()
@see CCamera::TFlash
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedFlashModes() const
	{
	TInt modes = iImpl->SupportedFlashModes();
	if (iOwner.CameraVersion() == KCameraDefaultVersion)
	    {
	    modes &= KBaselinedFlashModeMask;
	    }
	return modes;
	}

/** 
Gets the currently set flash mode.

@return  The currently set flash mode. 
@note   Clients not using the CCamera::New2L() or CCamera::NewDuplicate2L() would be given CCamera::EFlashAuto 
        if current flash mode exceeds CCamera::EFlashManual 
@see CCamera::TFlash
*/
EXPORT_C CCamera::TFlash CCamera::CCameraAdvancedSettings::FlashMode() const
	{
	TInt mode = iImpl->FlashMode();
	if (iOwner.CameraVersion() == KCameraDefaultVersion && mode > KBaselinedFlashModeMask)
	    {
	    mode = EFlashAuto;
	    }
	return static_cast<CCamera::TFlash>(mode);
	}

/** 
Sets the flash mode.

Triggers a KUidECamEventCameraSettingFlashMode event to all camera 
MCameraObserver2 clients.

@param  aMode
        The required flash mode. 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetFlashMode(CCamera::TFlash aMode)
	{
	iImpl->SetFlashMode(aMode);
	}

/** 
Gets whether the flash red eye reduction is switched on.

@return The present state - ETrue for switched on and EFalse for switched off. 
*/
EXPORT_C TBool CCamera::CCameraAdvancedSettings::RedEyeReduceOn() const
	{
	return iImpl->RedEyeReduceOn();
	}

/** 
Sets the flash red eye reduction on or off.

Triggers a KUidECamEventCameraSettingFlashRedEyeReduce event to all camera 
MCameraObserver2 clients.

@param  aState
        The required state ETrue for switching it on and EFalse for switching it off. 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetRedEyeReduceOn(TBool aState)
	{
	iImpl->SetRedEyeReduceOn(aState);
	}

/** 
Get flash compensation steps as integers multiplied by KECamFineResolutionFactor.
For example 0.5 EV is 50. 

@param aFlashCompensationSteps
       an RArray of integers which is populated on return to reflect the supported 
       flash compensation steps,
       
@param aInfo 
       an TValueInfo reference, which establishes the organization of 
	   the returned data. 

@see TValueInfo 
	     
@leave KErrNoMemory Out of memory. 

@note  When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive; 
	   corresponding getter/setters for this feature should not be used in such a case. 
	   
@note  When camera device is incapable of revealing the flash compensation steps supported, 
	   it has to be assumed that camera will work only on the parmanently set value. If this value is not known, empty 
	   array may be returned and TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetFlashCompensationStepsL(RArray<TInt>& aFlashCompensationSteps, TValueInfo& aInfo) const
	{
	iImpl->GetFlashCompensationStepsL(aFlashCompensationSteps, aInfo);
	}

/** 
@deprecated Use TInt GetFlashCompensationStep(TInt& aFlashCompensationStep); 

Get current flash power compensation step.

@return current flash compensation step.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::FlashCompensationStep() const
	{
	return iImpl->FlashCompensationStep();
	}
	
/** 
Get current flash power compensation step.

@param aFlashCompensationStep
	   Reference to the current flash compensation step.

@return system wide error code.

@note  Use this method in place of deprecated TInt FlashCompensationStep()

*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::GetFlashCompensationStep(TInt& aFlashCompensationStep) const
	{
	return iImpl->GetFlashCompensationStep(aFlashCompensationStep);
	}

/** 
Set current flash compensation step as an integer multiplied by KECamFineResolutionFactor. 
For example to set a compensation of -0.3 EV, one should use a parameter with value -30.  
All clients receive a KUidECamEventCameraSettingFlashCompensationStep event, when the value has changed. 

@param aFlashCompensationStep
       a new value for the flash compensation step.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetFlashCompensationStep(TInt aFlashCompensationStep)	
	{
	iImpl->SetFlashCompensationStep(aFlashCompensationStep);
	}

/**
Get current flash power compensation range measured in a already selected compensation step magnitude.
@note This range may change if a different compensation step is selected. 
For example if flash compensation range is in the range -1EV 1.5EV and the selected flash compensation 
step is selected to be 0.3 EV, the result of this call will be aNegativeCompensation = -3 and aPositiveCompensation = 5.
as there can be only three full steps for negative compensation (1/0.3) and five for flash power boost (1.5/0.3).
In this way developers, having pre-selected a step value from the supported set, would need to provide 
just the multiplier (in steps) and the direction (the sign). Steps are always assumed integers.	

@param aNegativeCompensation
       a reference to an integer returning the maximum number of steps available for negative compensation.
        
@param aPositiveCompensation
       a reference to an integer returning the maximum number of steps available for positive compensation.     
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetFlashCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const
	{
	iImpl->GetFlashCompensationRangeInSteps(aNegativeCompensation, aPositiveCompensation);
	}

/** 
@deprecated Use TInt GetFlashCompensation(TInt& aFlashCompensation); 

Get the current flash compensation value as integer steps. Positive values boost flash power,
negative reduce flash power. The change is not cumulative i.e. the change is stateless.
Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.

@note if returned value is 2 (compensation steps) and the current flash compensation step is 0.3 EV, 
then the actual compensation effect will be 0.6 EV. 
	   
@return the current number of compensation steps as an integer.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::FlashCompensation() const
	{
	return iImpl->FlashCompensation();
	}
	
/** 
Get the current flash compensation value as integer steps. Positive values boost flash power,
negative reduce flash power. The change is not cumulative i.e. the change is stateless.
Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.

@note if retrieved value is 2 (compensation steps) and the current flash compensation step is 0.3 EV, 
then the actual compensation effect will be 0.6 EV. 

@param aFlashCompensation
	   Reference to the current number of compensation steps as an integer.
	   
@return system wide error code.

@note Use this method in place of deprecated TInt FlashCompensation()

*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::GetFlashCompensation(TInt& aFlashCompensation) const
	{
	return iImpl->GetFlashCompensation(aFlashCompensation);
	}

/**  
Set the current flash compensation value as integer steps. 
Positive values increase power, negative reduce power. The change is not acumulative e.g. the change is stateless.
Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
Triggers a KUidECamEventCameraSettingFlashCompensation event.

@param 	aFlashCompensationSteps  
        a required compensation steps - negative value reduce the flash power
        positive boosts up the flash power.         
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetFlashCompensation(TInt aFlashCompensationSteps)
	{
	iImpl->SetFlashCompensation(aFlashCompensationSteps);
	}

/**
Check whether there is an external flash source.

@return ETrue if an external flash source is present, EFalse otherwise  
*/ 
EXPORT_C TBool CCamera::CCameraAdvancedSettings::IsExternalFlashPresent() const	
	{
	return iImpl->IsExternalFlashPresent();
	}

/** 
Gets the current discrete manual flash power levels supported by the camera in range 0-100 
as a percentage of maximum power level.

@param  aManualFlashPowerLevels
        An empty array of TInt which would be populated by the implementation with 
        the specific supported values. If the array is empty on return, 
        the camera does not support this functionality.
        
@param  aInfo 
        a reference to TValueInfo, which establishes the type of the returned data.             
	
@leave KErrNoMemory Out of memory. 

@note  When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive;
	   corresponding getter/setters for this feature should not be used in such a case. 
	   When camera device is incapable of revealing the manual flash power levels supported, 
	   it has to be assumed that camera will work only on the permanently set value. If this value is not known, empty 
	   array may be returned and TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetManualFlashPowerLevelsL(RArray<TInt>& aManualFlashPowerLevels, TValueInfo& aInfo) const
	{
	return iImpl->GetManualFlashPowerLevelsL(aManualFlashPowerLevels, aInfo);
	}

/**
Gets the current manual flash power level on the camera.

@return the current manual flash power level as a value in the range [0:100]. 
		Negative value returned means error case (system wide error code) and positive value means current manual flash power level.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::ManualFlashPowerLevel() const
	{
	return iImpl->ManualFlashPowerLevel();
	}

/**
Sets the current manual flash power level on the camera.
Triggers a KUidECamEventCameraSettingFlashManualPower event to all MCameraObserver2 clients.

@param  aManualFlashPowerLevel
        one of the values returned in GetManualFlashPowerLevelsL(). 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetManualFlashPowerLevel(TInt aManualFlashPowerLevel)
	{
	iImpl->SetManualFlashPowerLevel(aManualFlashPowerLevel);
	}

/**
Get Supported exposure modes - bitfields of CCamera::TExposure

@return the set of supported exposure modes.  
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedExposureModes() const
	{
	return iImpl->SupportedExposureModes();
	}

/** 
Gets the currently set exposure setting value.

@return  The currently set exposure setting value. 
*/
EXPORT_C CCamera::TExposure CCamera::CCameraAdvancedSettings::ExposureMode() const
	{
	return iImpl->ExposureMode();
	}

/** 
Sets the exposure mode of the camera.
Triggers a KUidECamEventCameraSettingExposureMode event to all MCameraObserver2 clients.

@param  aExposureMode
        The required exposure adjustment. 	        
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetExposureMode(CCamera::TExposure aExposureMode)
	{
	iImpl->SetExposureMode(aExposureMode);
	}

/** 
Get exposure compensation steps as integers multiplied by KECamFineResolutionFactor.
For example 0.3 EV is 30.

@param aExposureCompensationSteps
       an RArray of integers which is populated to reflect the supported 
       exposure compensation steps, all values have been multiplied by KECamFineResolutionFactor before 
       truncated to integers.
       
@param aInfo 
	   a reference to TValueInfo, which establishes the type of the returned data.            

@leave KErrNoMemory Out of memory. 

@note  When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive; 
	   corresponding getter/setters for this feature should not be used in such a case.	 
	   When camera device is incapable of revealing the exposure compensation steps supported, 
	   it has to be assumed that camera will work only on the permanently set value. If this value is not known, empty 
	   array may be returned and TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetExposureCompensationStepsL(RArray<TInt>& aExposureCompensationSteps, TValueInfo& aInfo) const
	{
	iImpl->GetExposureCompensationStepsL(aExposureCompensationSteps, aInfo);
	}

/** 
@deprecated Use TInt GetExposureCompensationStep(TInt& aExposureCompensationStep); 

Get current exposure compensation step.

@return the current exposure compensation step.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::ExposureCompensationStep() const
	{
	return iImpl->ExposureCompensationStep();
	}

/** 
Get current exposure compensation step.

@param aExposureCompensationStep
	   Reference to the current exposure compensation step.

@return system wide error code.

@note Use this method in place of deprecated TInt ExposureCompensationStep() 

*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::GetExposureCompensationStep(TInt& aExposureCompensationStep) const
	{
	return iImpl->GetExposureCompensationStep(aExposureCompensationStep);
	}

/** 
Set current exposure compensation step as an integer multiplied by KECamFineResolutionFactor.
All MCameraObserver2 clients receive a KUidECamEventCameraSettingExposureCompensationStep event,  
when the value has changed. 

@param aExposureCompensationStep
       a new value for the exposure compensation step.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetExposureCompensationStep(TInt aExposureCompensationStep)
	{
	iImpl->SetExposureCompensationStep(aExposureCompensationStep);
	}

/**
Get current exposure compensation range in steps. It depends on the previously 
selected exposure compensation step. 

@see GetFlashCompensationRangeInSteps()

@param  aNegativeCompensation
        reference to an integer returning the maximum number of steps 
        available for negative compensation,
        
@param aPositiveCompensation
        reference to an integer returning the maximum number of steps 
        available for positive compensation, 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetExposureCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const
	{
	iImpl->GetExposureCompensationRangeInSteps(aNegativeCompensation, aPositiveCompensation);
	}

/** 
@deprecated Use TInt GetExposureCompensation(TInt& aExposureCompensation);  

Get the current exposure compensation steps. Positive values increase exposure times, 
negative reduce exposure times. The change is not cumulative i.e. the change is stateless.
Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
	   
@note if returned value is 2 (compensation steps) and the current exposure compensation step is 0.3 EV, 
then the actual compensation effect will be 0.6 EV. 
	   
@return  current number of compensation steps as an integer.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::ExposureCompensation() const
	{
	return iImpl->ExposureCompensation();
	}

/** 
Get the current exposure compensation steps. Positive values increase exposure times, 
negative reduce exposure times. The change is not cumulative i.e. the change is stateless.
Each call assumes no previous compensation has been performed i.e. that there is a zero compensation.
	   
@note if retrieved value is 2 (compensation steps) and the current exposure compensation step is 0.3 EV, 
then the actual compensation effect will be 0.6 EV. 

@param aExposureCompensation
	   Reference to the current number of compensation steps as an integer.
	   
@return  system wide error code.

@note Use this method in place of deprecated TInt ExposureCompensation()

*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::GetExposureCompensation(TInt& aExposureCompensation) const
	{
	return iImpl->GetExposureCompensation(aExposureCompensation);
	}

/** 
Set the current exposure compensation value as integer steps.
Triggers a KUidECamEventCameraSettingExposureCompensation event to all MCameraObserver2 clients.

@param 	aExposureCompensationSteps  
        a required number of compensation steps - negative value reduce the exposure time
        positive increases the exposure time. 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetExposureCompensation(TInt aExposureCompensationSteps)
	{
	iImpl->SetExposureCompensation(aExposureCompensationSteps);
	}

/**
Gets camera supported set of white balance adjustments.

@return bitfield of all supported CCamera::TWhiteBalance values.  

@note  if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value(unrecognised) passed 
	   from the implementation will be filtered at this point.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedWhiteBalanceModes() const
	{
	TInt supportedWhiteBalanceModes = iImpl->SupportedWhiteBalanceModes();
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value(unrecognised) passed 
	   from the implementation will be filtered at this point.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		supportedWhiteBalanceModes &= KBaselinedWhiteBalanceModes;
		}
	
	return supportedWhiteBalanceModes;
	}

/** 
Gets the current white balance value.

@return The current white balance value.

@note  if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value(unrecognised) received 
	   from the implementation will be dropped and EWBAuto would be passed instead.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values
*/
EXPORT_C CCamera::TWhiteBalance CCamera::CCameraAdvancedSettings::WhiteBalanceMode() const
	{
	CCamera::TWhiteBalance whiteBalance = iImpl->WhiteBalanceMode();
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values. So, any extra enum value(unrecognised) received 
	   from the implementation will be dropped and EWBAuto would be passed instead.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		if(static_cast<TInt>(whiteBalance) > KBaselinedWhiteBalanceModes)
			{
			whiteBalance = CCamera::EWBAuto;
			}
		}
	
	return whiteBalance;
	}
	
/** 
Sets the white balance adjustment of the camera.

No effect if this is not supported, see TCameraInfo::iWhiteBalanceModesSupported.
Triggers KUidECamEventCameraSettingWhiteBalanceMode event to all MCameraObserver2 clients.

@param  aWhiteBalanceMode
        The required white balance mode.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode)
	{
	iImpl->SetWhiteBalanceMode(aWhiteBalanceMode);
	}

/** 
Gets the current state for aperture and exposure lock.

@return ETrue if aperture and exposure values are locked together, 
       EFalse if these are not locked. 
*/
EXPORT_C TBool CCamera::CCameraAdvancedSettings::ApertureExposureLockOn() const
	{
	return iImpl->ApertureExposureLockOn();
	}

/** 
Sets the current state for aperture and exposure lock.
Triggers a KUidECamEventAELock event to all MCameraObserver2 clients.

@param aAELock
       a value whether to lock exposure and aperture together.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetApertureExposureLockOn(TBool aAELock)
	{
	iImpl->SetApertureExposureLockOn(aAELock);
	}

/**
Gets the current state for button clicking sound effect. 

@return  ETrue to switch clicking sound on, EFalse sound off  
*/
EXPORT_C TBool CCamera::CCameraAdvancedSettings::ShootClickOn() const
	{
	return iImpl->ShootClickOn();
	}

/**
Sets the button clicking sound on /off. Triggers a KUidECamEventSoundClick event 
to all MCameraObserver2 clients. 

@param aShootClickOn
       ETrue to switch clicking sound on, EFalse sound is switched off.  
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetShootClickOn(TBool aShootClickOn)
	{
	iImpl->SetShootClickOn(aShootClickOn);
	}

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

@see   TValueInfo  

@note  When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive;
	   corresponding getter/setters for this feature should not be used in such a case.	 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetTimerIntervalsL(RArray<TInt>& aTimerIntervals, TValueInfo& aInfo) const
	{
	iImpl->GetTimerIntervalsL(aTimerIntervals, aInfo);
	}

/**
Get current timer value. Active only when drive mode is EDriveModeTimed.
Timer resolution is in microseconds. 

@return current time interval value. 
		Negative value returned means error case (system wide error code) and positive value means current timer interval.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::TimerInterval() const
	{
	return iImpl->TimerInterval();
	}
  
/**
Set current timer value. Active only when drive mode EDriveModeTimed.
This is the time interval (delay) in microseconds between user pressing the button and image is taken.  
The setting of the value triggers a KUidECamEventCameraSettingTimerInterval event 
to all MCameraObserver2 clients. 

@param aTimerInterval
       The selected timer interval in microseconds.        
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetTimerInterval(TInt aTimerInterval)
	{
	iImpl->SetTimerInterval(aTimerInterval);
	}
 
/**
Get camera supported time lapse period range. Active only when drive mode EDriveModeTimeLapse.
The time lapse is denoted as the uniform time period between consecutive frames.

@param aTimeLapseMin
       The minimum time value. 
       
@param aTimeLapseMax
       The maximum time value.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetTimeLapsePeriodRange(TTime& aTimeLapseMin, TTime& aTimeLapseMax) const
	{
	iImpl->GetTimeLapsePeriodRange(aTimeLapseMin, aTimeLapseMax); 
	}

/**
Get current time lapse value. Active only when drive mode EDriveModeTimeLapse.
The time lapse is denoted as the uniform time period 
between consecutive frames and operation is configurable by its start, end and a fixed interval.

@param aStart
		the start of the  timelapse period.
		
@param aEnd
		the end of the  timelapse period; start < end.
		
@param aInterval
		the set parameter between two successive snapshots.		
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetTimeLapse(TTime& aStart, TTime& aEnd, TTime& aInterval) const
	{
	iImpl->GetTimeLapse(aStart, aEnd, aInterval);
	}

/**
Set current time lapse value. Active only when drive mode EDriveModeTimeLapse.
The time lapse is denoted as the uniform time period between consecutive frames. 
Setting triggers a KUidECamEventCameraSettingTimeLapse event to all MCameraObserver2 camera clients.

@param aStart
		the start of the  timelapse period.
		
@param aEnd
		the end of the  timelapse period; start < end.
		
@param aInterval
		the set parameter between two successive snapshots.	
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetTimeLapse(const TTime& aStart, const TTime& aEnd, const TTime& aInterval)
	{
	iImpl->SetTimeLapse(aStart, aEnd, aInterval);
	}

/**
Get current picture orientation

@return a TPictureOrientation value.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TPictureOrientation CCamera::CCameraAdvancedSettings::PictureOrientation() const
	{
	return iImpl->PictureOrientation();
	}

/**
Set a new picture orientation
This triggers a KUidECamEventCameraSettingPictureOrientation event to all MCameraObserver2 clients.

@param aOrientation
       a value of TPictureOrientation denoting the new orientation.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetPictureOrientation(CCamera::CCameraAdvancedSettings::TPictureOrientation aOrientation)
	{
	iImpl->SetPictureOrientation(aOrientation);
	}
	
/**
Get supported pixel aspect ratio.

@return a bitfield of all supported TPixelAspectRatio values.
@note Some of the pixel aspect ratios are available only for clients using either CCamera::New2L() or CCamera::NewDuplicate2L()
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedPixelAspectRatios() const	
	{
	TInt supportedRatios = iImpl->SupportedPixelAspectRatios();
	/**
	    Mask out new features for old clients
	*/
	if (iOwner.CameraVersion() == KCameraDefaultVersion)
	    {
	    supportedRatios &= KBaselinedPixelAspectsMask;
	    }
	return supportedRatios;
	}

/**
Get current pixel aspect ratio.

@return a TPixelAspectRatio value.
@note Some of the pixel aspect ratios are available only for clients using either CCamera::New2L() or CCamera::NewDuplicate2L()
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TPixelAspectRatio CCamera::CCameraAdvancedSettings::PixelAspectRatio() const
	{
	TInt ratio = iImpl->PixelAspectRatio();
	/**
	    Mask out new features for old clients
	*/
	if (iOwner.CameraVersion() == KCameraDefaultVersion)
	    {
	    ratio &= KBaselinedPixelAspectsMask;
	    }	
	return static_cast<CCamera::CCameraAdvancedSettings::TPixelAspectRatio>( ratio );
	}

/**
Set a new pixel aspect ratio.
This triggers a KUidECamEventPixelAspectRatio event to all MCameraObserver2 clients.

@param aPixelAspectRatio
       a value of TPixelAspectRatio denoting the new pixel aspect ratio.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::TPixelAspectRatio aPixelAspectRatio)
	{
	iImpl->SetPixelAspectRatio(aPixelAspectRatio);
	}

/**
Get supported YUV ranges.

@return a bitfileld of all supported TYuvRange values.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedYuvRanges() const
	{
	return iImpl->SupportedYuvRanges();
	}

/**
Get current YUV range.

@return a TYuvRange value.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TYuvRange CCamera::CCameraAdvancedSettings::YuvRange() const
	{
	return iImpl->YuvRange();
	}

/**
Set a new YUV range.
This triggers a KUidECamEventYuvRange event to all MCameraObserver2 clients.

@param aYuvRange
       a value of TYuvRange denoting the new YUV range.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetYuvRange(CCamera::CCameraAdvancedSettings::TYuvRange aYuvRange)
	{
	iImpl->SetYuvRange(aYuvRange);
	}

/**
Get the number of images captured normally under EDriveModeBurst condition.
@note: due to memory or image size limitations the actual number may be less. 

@return the number of images set to capture in burst mode. 
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::BurstImages() const
	{
	return iImpl->BurstImages();
	}

/**
Set the number of images captured normally under EDriveModeBurst condition.
Triggers a KUidECamEventBurstImages event to all MCameraObserver2 clients.
@note: due to memory or image size limitations the actual number may be less. 

@param aImages
	   the number of images set to capture in burst mode 

@note  Unless reduced Latency scheme is not used (ie CaptureImageL(TInt aSequenceNumber) or StartPerformantVideoCaptureL())
	   if an image/video capture is still outstanding, this method may report error KErrInUse.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetBurstImages(TInt aImages)	
	{
	iImpl->SetBurstImages(aImages);
	}

/** 
Gets the optical zoom levels.

@param  aOpticalZoomSteps
        Array to hold optical zoom values multiplied by KECamFineResolutionFactor to retain precision. 
        So that if zoom is not supported the array will return a single element of value 
        KECamFineResolutionFactor.

@param  aInfo 
		a reference to TValueInfo, which establishes the type of the returned data.     

@note   Such approach allows for support for both linear and non-linear zoom steps.
        When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive;
        corresponding getter/setters for this feature should not be used in such a case.

@leave KErrNoMemory Out of memory. 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetOpticalZoomStepsL(RArray<TInt>& aOpticalZoomSteps, TValueInfo& aInfo) const
	{
	iImpl->GetOpticalZoomStepsL(aOpticalZoomSteps, aInfo);
	}

/** 
Gets the currently set zoom value.

@return  The currently set optical zoom value. The value is multiplied by 
		 KECamFineResolutionFactor to retain precision. 
		 Negative value returned means error case (system wide error code) and positive value means current optical zoom.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::OpticalZoom() const
	{
	return iImpl->OpticalZoom();
	}

/** 
Sets the zoom using a specific value. Triggers a KUidECamEventCameraSettingOpticalZoom 
event to all MCameraObserver2 clients.

@param aOpticalZoom 
       Required zoom value.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetOpticalZoom(TInt aOpticalZoom)
	{
	iImpl->SetOpticalZoom(aOpticalZoom);
	}

/** 
Gets the digital zoom levels.

@param  aDigitalZoomSteps
        Array to hold digital zoom values multiplied by KECamFineResolutionFactor to retain precision. 
        So that if zoom is not supported the array will return a single element of value 
        KECamFineResolutionFactor.
        
@param  aInfo 
		a reference to TValueInfo, which establishes the type of the returned data.
	
@note   Such approach allows for support for both linear and non-linear zoom steps.
		When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive;
		corresponding getter/setters for this feature should not be used in such a case.

@leave KErrNoMemory Out of memory. 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo) const
	{
	iImpl->GetDigitalZoomStepsL(aDigitalZoomSteps, aInfo);
	}

/** 
Gets the currently set digital zoom value.

@return  The currently set digital zoom value. The value is multiplied by 
		 KECamFineResolutionFactor to retain precision. 
		 Negative value returned means error case (system wide error code) and positive value means current digital zoom.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::DigitalZoom() const
	{
	return iImpl->DigitalZoom();
	}

/** 
Sets the digital zoom value. Triggers a KUidECamEventCameraSettingDigitalZoom event
to all MCameraObserver2 clients.

@param aDigitalZoom 
       Required zoom value.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetDigitalZoom(TInt aDigitalZoom)
	{
	iImpl->SetDigitalZoom(aDigitalZoom);
	}

/** 
Checks whether exposure value is locked or not.

@return whether exposure value is locked or not. 
		ETrue if locked, EFalse otherwise. 
*/
EXPORT_C TBool CCamera::CCameraAdvancedSettings::ExposureLockOn() const
	{
	return iImpl->ExposureLockOn();
	}

/** 
Sets exposure lock state. Triggers a KUidECamEventCameraSettingExposureLock event
 to all MCameraObserver2 clients.

@param aState 
       Required new state.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetExposureLockOn(TBool aState)
	{
	iImpl->SetExposureLockOn(aState);
	}

/** 
Checks whether AutoFocus value is locked or not.

@return whether AutoFocus value is locked or not. 
		ETrue if locked, EFalse otherwise. 
*/
EXPORT_C TBool CCamera::CCameraAdvancedSettings::AutoFocusLockOn() const
	{
	return iImpl->AutoFocusLockOn();
	}

/** 
Sets autofocus lock state. Triggers a KUidECamEventCameraSettingAutoFocusLock event
to all MCameraObserver2 clients.

@param aState 
       Required new state.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetAutoFocusLockOn(TBool aState)
	{
	iImpl->SetAutoFocusLockOn(aState);
	}

/** 
Gets an array of all the advanced settings parameters supported by the camera. 
These are identified by UIDs and relate to the set or subset of it of all defined settings UIDs. 

@param  aSettings
        An empty array of TUids which would be populated by the implementation with 
        the UIDs of the supported parameters. If the array is empty on return, 
        the camera does not support any settings.  

@leave KErrNoMemory Out of memory. 

@note   if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
	    from the implementation will be filtered at this point.
	    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const	
	{
	iImpl->GetSupportedSettingsL(aSettings); 
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aSettings.Count(); index++)
			{
			/** KUidECamEventCameraSettingAutoFocusType2UidValue is the baseline. Any settings with greater uid value means that it has 
			been added in later versions */
			if(aSettings[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
				{
				aSettings.Remove(index);
				index--;
				}
			}
		}
	}
   
/** 
Gets an array of all the advanced settings parameters currently active on the camera. 
These are identified by UIDs and relate to the set or subset of it of all supported
settings UIDs. 

@param  aActiveSettings
        An empty array of TUids which would be populated by the implementation with 
        the active setting UIDs. If the array is empty on return, 
        the camera does not support any settings.  

@leave KErrNoMemory Out of memory.

@note   if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
	    from the implementation will be filtered at this point.
	    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const
	{
	iImpl->GetActiveSettingsL(aActiveSettings);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aActiveSettings.Count(); index++)
			{
			/** KUidECamEventCameraSettingAutoFocusType2UidValue is the baseline. Any settings with greater uid value means that it has 
			been added in later versions */
			if(aActiveSettings[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
				{
				aActiveSettings.Remove(index);
				index--;
				}
			}
		}
	}

/** 
Gets an array of all the advanced settings parameters currently disabled on the camera. 
These are identified by UIDs and relate to the set or subset of it of all supported
settings UIDs. 

@param  aDisabledSettings
        An empty array of TUids which would be populated by the implementation with 
        the disabled setting UIDs. If the array is empty on return, 
        the camera does not support any settings.  

@leave KErrNoMemory Out of memory.

@note   if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
	    from the implementation will be filtered at this point.
	    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const	
	{
	iImpl->GetDisabledSettingsL(aDisabledSettings); 
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aDisabledSettings.Count(); index++)
			{
			/** KUidECamEventCameraSettingAutoFocusType2UidValue is the baseline. Any settings with greater uid value means that it has 
			been added in later versions */
			if(aDisabledSettings[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
				{
				aDisabledSettings.Remove(index);
				index--;
				}
			}
		}
	}
		
/** 
Retrieves the state for automatic size selection option. Default value is EFalse.

@return ETrue if the automatic selection is switched on. Default value is EFalse.
*/
EXPORT_C TBool CCamera::CCameraAdvancedSettings::AutomaticSizeSelectionChangeOn() const
	{
	return iImpl->AutomaticSizeSelectionChangeOn();
	}
	
/** 
Allow camera to proactively change image size due external factors. 
Default value is EFalse. Triggers a KUidECamEventCameraSettingAutomaticSizeSelection event notification.

@param aSetOn
       whether the option should be switched on
 */     
EXPORT_C void CCamera::CCameraAdvancedSettings::SetAutomaticSizeSelectionChangeOn(TBool aSetOn)
	{
	iImpl->SetAutomaticSizeSelectionChangeOn(aSetOn);
	}

/** 
Retrieves the timeout values camera supported by the camera when in continuous auto focus mode. 
Timeouts are in microseconds.

@param  aTimeouts
        An empty array to hold timeout values.
        
@param  aInfo 
		a reference to TValueInfo, which establishes the type of the returned data.
		
@leave  KErrNoMemory Out of memory.

@note   When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive;
		corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& aTimeouts, TValueInfo& aInfo) const
	{
	iImpl->GetSupportedContinuousAutoFocusTimeoutsL(aTimeouts, aInfo);
	}
	
/**
Gets the current value for continuous autofocus timeout. Timeouts are in microseconds.

@return  the timeout value in microseconds. 
		 Negative value returned means error case (system wide error code) and positive value means current value for 
		 continuous autofocus timeout.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::ContinuousAutoFocusTimeout() const
	{
	return iImpl->ContinuousAutoFocusTimeout();
	}

/**
Sets the current value for continuous autofocus timeout. Timeouts are in microseconds.
All MCameraObserver2 clients of the camera receive a KUidECamEventCameraSettingsContinuousAutoFocusTimeout 
event notification when timeout value is changed.

@param 	aTimeout
		the timeout value in microseconds. 
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetContinuousAutoFocusTimeout(TInt aTimeout)
	{
	iImpl->SetContinuousAutoFocusTimeout(aTimeout);
	}

/**
Gets all supported stabilization effects on the camera.

@return an integer - a bitfield of all supported TStabilizationEffect values. 
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TStabilizationEffect CCamera::CCameraAdvancedSettings::StabilizationEffect() const
	{
	return iImpl->StabilizationEffect();
	}
	
/**
Gets the current stabilization effect on the camera.

@return a TStabilizationEffect value.
*/
EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedStabilizationEffects() const
	{
	return iImpl->SupportedStabilizationEffects();
	}

/**
Sets a specific stabilization effect on the camera. 
When a value is set, MCameraObserver2 clients for that camera will receive a 
KUidECamEventCameraSettingsStabilizationEffect event notification.

@param aEffect
       stabilization effect selection of type TStabilizationEffect.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetStabilizationEffect(CCamera::CCameraAdvancedSettings::TStabilizationEffect aEffect)
	{
	iImpl->SetStabilizationEffect(aEffect);
	}
	
/**
Gets all supported stabilization algorithm values on the camera.

@return an integer - a bitfield of all supported TStabilizationAlgorithmComplexity values. 
*/

EXPORT_C TInt CCamera::CCameraAdvancedSettings::SupportedStabilizationComplexityValues() const
	{
	return iImpl->SupportedStabilizationComplexityValues();
	}
	
/**
Gets current active stabilization algorithm selection on the camera.

@return a TStabilizationAlgorithmComplexity value.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity CCamera::CCameraAdvancedSettings::StabilizationComplexity() const
	{
	return iImpl->StabilizationComplexity();
	}

/**
Sets a specific stabilization algorithm on the camera. 
When a value is set, MCameraObserver2 clients for that camera will receive a 
KUidECamEventSettingsStabilizationAlgorithmComplexity event notification.

@param aComplexity
       stabilization effect selection of type TStabilizationAlgorithmComplexity.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity aComplexity)
	{
	iImpl->SetStabilizationComplexity(aComplexity);
	}
	
/**
Gets the units in which the white balance is measured on the camera. 
@note The methods used to get or set these differ depending on the supported unit type.  
It is expected that a camera will support only a single type or none.

@return a value of TWBUnits type.   
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TWBUnits CCamera::CCameraAdvancedSettings::SupportedWBUnits()  const
	{
	return iImpl->SupportedWBUnits();
	}

/**
Get white balance value represented as a RGB triplet. 

@see TRgb

@param aValue
	   a reference to TRgb object which will contain the current white balance.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetWBRgbValue(TRgb& aValue) const
	{
	iImpl->GetWBRgbValue(aValue);
	}

/**
Set white balance value using a RGB triplet.  
Change in value causes an event notification KUidECamEventCameraSettingsWBValue 
to be sent to all MCameraObserver2 clients of this camera. 

@param aValue
	   a const reference to TRgb object, which contains the new white balance.
	   
@see TRgb
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetWBRgbValue(const TRgb& aValue)
	{
	iImpl->SetWBRgbValue(aValue);
	}

/**
Get the white balance values, as temperature measured in Kelvin, supported on the camera.

@param  aWBColorTemperatures
    	A reference to an empty array of TInt which would be populated by the implementation with 
    	the specific supported values. 

@param  aInfo 
        a reference to TValueInfo, which establishes the type of the returned data. 

@note   When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive; 
		corresponding getter/setters for this feature should not be used in such a case.    
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetWBSupportedColorTemperaturesL(RArray<TInt>& aWBColorTemperatures, TValueInfo& aInfo) const
	{
	iImpl->GetWBSupportedColorTemperaturesL(aWBColorTemperatures, aInfo);
	}

/**
Get the white balance as a temperature in Kelvin

@return current white balance value as a temperature in Kelvins.
		Negative value returned means error case (system wide error code) and positive value means current value for 
		white balance as a temperature in Kelvin.
*/	
EXPORT_C TInt CCamera::CCameraAdvancedSettings::WBColorTemperature() const
	{
	return iImpl->WBColorTemperature();
	}
	
/**
Set the white balance as a temperature in Kelvin

@param aColorTemperature
	   white balance value as a temperature in Kelvins.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetWBColorTemperature(TInt aColorTemperature)
	{
	iImpl->SetWBColorTemperature(aColorTemperature);
	}

/** 
Checks whether the flash is ready.
 
@param 	aReady
		A reference to a boolean set by the implementation to ETrue if the flash is ready, 
		EFalse otherwise.
       
@return KErrNotSupported if the implementation of this method is not supported.
*/	
EXPORT_C TInt CCamera::CCameraAdvancedSettings::IsFlashReady(TBool& aReady) const
	{
	aReady = EFalse;
	if(iImpl2 != NULL)
		{
		return iImpl2->IsFlashReady(aReady);
		}
	return KErrNotSupported;
	}

/**
Get the number of focus steps for current focus mode.

@param  aFocusModeSteps
     	A reference to an empty array of TInt which would be populated by the implementation with 
     	the specific supported values. 

@param  aInfo 
        a reference to TValueInfo, which establishes the type of the returned data. 

@leave  KErrNotSupported if the implementation of this method is not supported. May also leave as a result of other system errors.

@note   When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive;
		corresponding getter/setters for this feature should not be used in such a case.
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetCurrentFocusModeStepsL(RArray<TInt>& aFocusModeSteps, TValueInfo& aInfo) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetCurrentFocusModeStepsL(aFocusModeSteps, aInfo);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}		
	}
	
/**
Gets all supported ISO types on the device.

@param  aSupportedISORateTypes 
		A reference to an integer which is a bitfield of all supported TISORateType values. EISONone means feature is not supported.
		
@leave  KErrNotSupported if the implementation of this method is not present. May leave as a result of some other error. 
		
@note   When concrete implementation is provided, but camera device does not support this feature, then aSupportedISORateTypes retrieves EISONone.

*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetSupportedISORateTypeL(aSupportedISORateTypes);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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
		
@leave  KErrNotSupported if the implementation of this method is not present.

@note   Triggers KUidECamEventCameraSettingIsoRateType to all MCameraObserver2 clients of the camera. 
		HandleEvent is used to report the result or any possible error. New setter functions leave only when 
		implementation is not there.  

*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::SetISORateL(CCamera::CCameraAdvancedSettings::TISORateType aISORateType, TInt aParam)
	{
	if(iImpl3 != NULL)
		{
		iImpl3->SetISORate(aISORateType, aParam);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

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
		Otherwise KErrNotFound is retrieved indicating the incapability of camera.

@leave  KErrNotSupported if the implementation of this method is not present. 
		May leave as a result of some other error. 

@note   When concrete implementation is provided, but camera device does not support this feature, then aISORateType retrieves EISONone.
		
@note	Since camera hardware may be incapable of providing the actual ISO value when one of the auto ISO type has 
		been set, then, in these cases, the 3rd argument is retrieved as KErrNotFound.

*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetISORateL(CCamera::CCameraAdvancedSettings::TISORateType& aISORateType, TInt& aParam, TInt& aISORate) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetISORateL(aISORateType, aParam, aISORate);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Provide reference screen for orientation information.

@param  aScreenDevice
		A reference to the screen device.

@leave  KErrNotSupported if the implementation of this method is not present.

@note	Triggers KUidECamEventCameraSettingReferenceScreen to all MCameraObserver2 clients of the camera. 
		HandleEvent is used to report the result or any possible error. New setter functions leave only when 
		implementation is not there.

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetReferenceScreenL(CWsScreenDevice& aScreenDevice)
	{
	if(iImpl3 != NULL)
		{
		iImpl3->SetReferenceScreen(aScreenDevice);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

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

@leave  KErrNotSupported if the implementation of this method is not present. 
		KErrNoMemory if out of memory. May leave as a result of some other error. 

@note   This method retrieves the supported digital zoom steps irrespective of any stabilization influence. 
		In case of stabilization etc. influence, the setting function should set the best possible digital zoom value 
		and return error KErrECamDigitalZoomLimited along with dedicated event.
		
@note	When concrete implementation is provided, but camera device does not support this feature, empty array may be returned and 
		TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case. 

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, 
						TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetDigitalZoomStepsForStillL(aDigitalZoomSteps, aInfo, aSizeIndex, aFormat, aIsInfluencePossible);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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

@leave  KErrNotSupported if the implementation of this method is not present. 
		KErrNoMemory if out of memory. May leave as a result of some other error. 

@note   This method retrieves the supported digital zoom steps irrespective of any stabilization influence. 
		In case of stabilization etc. influence, the setting function should set the best possible digital zoom value 
		and return error KErrECamDigitalZoomLimited along with dedicated event.
		
@note	When concrete implementation is provided, but camera device does not support this feature, empty array may be returned and 
		TValueInfo may be ENotActive; corresponding getter/setters for this feature should not be used in such a case. 

*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetDigitalZoomStepsForVideoL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, 
						TInt aFrameRateIndex, TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible, 
						CCamera::TExposure aExposure) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetDigitalZoomStepsForVideoL(aDigitalZoomSteps, aInfo, aFrameRateIndex, aSizeIndex, aFormat, aIsInfluencePossible, aExposure);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves the pre capture warnings supported for a given camera mode

@param  aCameraMode
		Desired camera mode for which the supported pre capture warnings may be retrieved.
		
@param 	aPreCaptureWarningSupported
		A bitfield of all supported TPreCaptureWarning to be issued in the given camera mode. 
		If no pre capture warning supported for the given camera mode, EPCWNone is retrieved.
		
@leave  May leave with any error. 

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::TCameraMode aCameraMode, TInt& aPreCaptureWarningSupported) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetPreCaptureWarningSupportedL(aCameraMode, aPreCaptureWarningSupported);
		
		/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	  	application is not prepared to receive extra added enum values. So, any extra enum value passed 
	   	from the implementation will be filtered at this point.
	   	To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   	to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
		if(iOwner.CameraVersion() == KCameraDefaultVersion)
			{
			aPreCaptureWarningSupported &= KBaselinedPreCaptureWarning;
			}	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Subscribe in order to receive event which indicates warnings on occurrence of some specific unfavourable 
conditions before image/video capture.

@param  aPreCaptureWarning
		A bitfield specifying all the TPreCaptureWarning types to be subscribed for.
		
@leave  KErrNotSupported if the implementation of this method or the feature is not supported. 
		May also leave as a result of other errors.

@note	When any of the subscribed warnings (represented by aPreCaptureWarning) get generated by the camera device, 
		event KUidECamEventCameraSettingPreCaptureWarning is issued. TECAMEvent2 class should be used in order to 
		provide the status of every PreCaptureWarning.

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SubscribeToPreCaptureWarningL(TInt aPreCaptureWarning) 
	{
	if(iImpl3 != NULL)
		{
		iImpl3->SubscribeToPreCaptureWarningL(aPreCaptureWarning);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Unsubscribe so that further events are not received when warnings get issued.

@leave  KErrNotSupported if the implementation of this method or the feature is not supported. 
		May also leave as a result of other errors.

*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::UnSubscribePreCaptureWarningL()
	{
	if(iImpl3 != NULL)
		{
		iImpl3->UnSubscribePreCaptureWarningL();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}	

/**
Get the status of every warnings defined.

@param  aPreCaptureWarning
		A reference to the integer - bitfield  representing all the TPreCaptureWarning types issued.

@leave  KErrNotSupported if the implementation of this method or the feature is not supported. 
		May also leave as a result of other errors.

@note   This method may be called after receiving the event KUidECamEventCameraSettingPreCaptureWarning OR
		user may also opt for polling on this.

*/		
EXPORT_C void CCamera::CCameraAdvancedSettings::GetPreCaptureWarningL(TInt& aPreCaptureWarning) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetPreCaptureWarningL(aPreCaptureWarning);	
		
		/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	  	application is not prepared to receive extra added enum values. So, any extra enum value passed 
	   	from the implementation will be filtered at this point.
	   	To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   	to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
		if(iOwner.CameraVersion() == KCameraDefaultVersion)
			{
			aPreCaptureWarning &= KBaselinedPreCaptureWarning;
			}
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
	
/**
Retrieve the different supported AF assistant light. 

@param  aSupportedAFAssistantLight
        A reference to integer - bitfield indicating the supported AF assistant light.
	    If EAFAssistantLightOff, this means AF assistant light is not supported. 
	    If EAFAssistantLightManualOn, then manual AF assistant light is supported.  
	    If EAFAssistantLightAuto, auto assistant light is supported. 
	    If combination of EAFAssistantLightManualOn||EAFAssistantLightAuto , then both manual and Auto assistant light are supported.
	   
@leave  KErrNotSupported if the implementation of this method is not supported. 
		May also leave as a result of other errors. 

@note	When concrete implementation is provided, but camera device does not support this feature, 
		then aSupportedAFAssistantLight retrieves EAFAssistantLightOff. Corresponding getter/setters for this feature should not be called then.

*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetSupportedAFAssistantLightL(aSupportedAFAssistantLight);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}
			
/**
Get the type ( and state) of AF assistant light currently set.

@param  aAFAssistantLight
		A reference to AF assistant light.
		If EAFAssistantLightOff, then manual and auto assistant light are switched off.
		If EAFAssistantLightManualOn, manual assistant light is switched on. 
		If EAFAssistantLightAuto, AF assistant light is set to auto.
	   
@leave  KErrNotSupported if the implementation of this method or the feature is not supported. 
		May also leave as a result of other errors.

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight& aAFAssistantLight) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetAFAssistantLightL(aAFAssistantLight);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Set a particular type ( and state) of AF assistant light.

@param  aAFAssistantLight
		Type of AF assistant light to be set.
		If EAFAssistantLightOff, switch off the manual or auto assistant light.
		If EAFAssistantLightManualOn, manually switch on the assistant light. 
		If EAFAssistantLightAuto, camera will automatically switch it on/off as per the conditions.
	   
@leave  KErrNotSupported if the implementation of this method is not present.

@note   Triggers KUidECamEventCameraSettingAFAssistantLight to all MCameraObserver2 clients of the camera. 
		HandleEvent is used to report the result or any possible error. New setter functions leave only when 
		implementation is not there.

*/
EXPORT_C void CCamera::CCameraAdvancedSettings::SetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight aAFAssistantLight)
	{
	if(iImpl3 != NULL)
		{
		iImpl3->SetAFAssistantLight(aAFAssistantLight);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Retrieves the supported continuous zoom types.

@param  aSupportedContinuousZoomType
		Retrieves a bitfield of TUint which indicates the supported continuous zoom type as given by 
		CCamera::CCameraAdvancedSettings::TContinuousZoomType
		
@leave  May leave with any error code.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedContinuousZoomTypeL(TUint& aSupportedContinuousZoomType) const
	{
	if(iImpl4 != NULL)
		{
		iImpl4->GetSupportedContinuousZoomTypeL(aSupportedContinuousZoomType);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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

@publishedPartner
@prototype
*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetFocalLengthInfoL(TInt& aMinFocalLength, TInt& aCurrentFocalLength, TInt& aMaxFocalLength) const
	{
	if(iImpl4 != NULL)
		{
		iImpl4->GetFocalLengthInfoL(aMinFocalLength, aCurrentFocalLength, aMaxFocalLength);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves the total number of operation preferences supported by the implementation. Operation preferences are 
specified in terms of performance vectors, that is, speed, quality, low memory consumption and low power consumption.

@param  aNumOperationPreferenceSupported
		Retrieves the number of operation preferences supported.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetNumOperationPreferenceL(TUint& aNumOperationPreferenceSupported) const
	{
	if(iImpl4 != NULL)
		{
		iImpl4->GetNumOperationPreferenceL(aNumOperationPreferenceSupported);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::EnumerateOperationPreferenceL(TUint aOperationPreferenceIndex, CCamera::
						CCameraAdvancedSettings::TPerformanceLevel& aSpeedLevel, CCamera::CCameraAdvancedSettings::
						TPerformanceLevel& aQualityLevel, CCamera::CCameraAdvancedSettings::TPerformanceLevel& 
						aLowMemoryConsumptionLevel, CCamera::CCameraAdvancedSettings::TPerformanceLevel& aLowPowerConsumptionLevel) const
						
	{
	if(iImpl4 != NULL)
		{
		iImpl4->EnumerateOperationPreferenceL(aOperationPreferenceIndex, aSpeedLevel, aQualityLevel, 
																	aLowMemoryConsumptionLevel, aLowPowerConsumptionLevel);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}
	
/**
Set a particular operation preference.

@param  aOperationPreferenceIndex
		An index which reveals a set of levels to be used for performance vectors, that is, speed, quality, low memory 
		consumption and low power consumption.

@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEventCameraSettingOperationPreference is used to notify clients about setting an operation preference.

@publishedPartner
@prototype
*/										
EXPORT_C void CCamera::CCameraAdvancedSettings::SetOperationPreferenceL(TUint aOperationPreferenceIndex)
	{
	if(iImpl4 != NULL)
		{
		iImpl4->SetOperationPreference(aOperationPreferenceIndex);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Get the current operation preference being used.

@param  aOperationPreferenceIndex
		Currently used operation preference index.
					
@leave  May leave with any error code. 

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::GetOperationPreferenceL(TInt& aOperationPreferenceIndex) const
	{
	if(iImpl4 != NULL)
		{
		iImpl4->GetOperationPreferenceL(aOperationPreferenceIndex);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves the event uids which the underlying implementation supports. Client may use these event notifications as
milestones in their application.

@param  aSupportedEvents
		Retrieves as array of TUid. Every member of the array represents a supported event uid. These events are ECAM 
		component wide. 
		
@leave  May leave with any error code.

@note   This method may retrieve unrecognized events which may be introduced later on.

@publishedPartner
@prototype
*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetSupportedEventsL(RArray<TUid>& aSupportedEvents) const
	{
	if(iImpl4 != NULL)
		{
		iImpl4->GetSupportedEventsL(aSupportedEvents);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}
	
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

@publishedPartner
@prototype
*/	
EXPORT_C void CCamera::CCameraAdvancedSettings::GetIndirectFeatureChangesL(TUid aRequestedSetting, RArray<TUid>& aIndirectFeatureChanges) const
	{
	if(iImpl4 != NULL)
		{
		iImpl4->GetIndirectFeatureChangesL(aRequestedSetting, aIndirectFeatureChanges);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Creates an instance of a CCameraContinuousZoom object and returns it to the client. Ownership of the object is passed to the client.

@param  aObserver
		Reference to a continuous zoom observer which will be used to issue callbacks to the client.

@param  aContinuousZoomType
		The continuous zoom type with which the continuous zoom object will be initialised.

@param  aContinuousZoom
		Retrieves pointer to the continuous zoom object. Client assumes ownership of the object.

@note   Client must not delete the continuous zoom object if CCameraContinuousZoom::StartContinuousZoomL() has been called as MContinuousZoomObserver callbacks
		are still expected by the implementation. Clients should therefore only delete the object once the MContinuousZoomObserver::ContinuousZoomCompleted()
		callback has been received or has explicitly stopped the continuous zoom via StopContinuousZoom().

@internalTechnology
@prototype
*/
EXPORT_C void CCamera::CCameraAdvancedSettings::CreateContinuousZoomL(MContinuousZoomObserver& aObserver, TContinuousZoomType aContinuousZoomType, CCameraContinuousZoom*& aContinuousZoom)
	{
	if(iImpl4 != NULL)
		{
		MImplementationFactory* implFactory = NULL;

		iImpl4->CreateContinuousZoomImplFactoryL(implFactory);

		CleanupReleasePushL(*implFactory);
		CCameraContinuousZoom* zoom = CCameraContinuousZoom::CreateL(aObserver, aContinuousZoomType, *implFactory);
		CleanupStack::Pop(implFactory);

		implFactory->Release();
		aContinuousZoom = zoom;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}
	
/**
Register the ECAM component wide events in order to decide which events the client needs to be notified about.
Previously registered events will be overridden and not augmented.

@param  aEventFilter
	    This is of type TECAMEventFilterScheme. 
	    If EECAMEventFilterSchemeBlackList, the client will not be notified only for the events packed in the array 'aEvents'.
	    If EECAMEventFilterSchemeWhiteList, the client will be notified only for the events packed in the array 'aEvents'.
	    
@param  aEvents
		An array of events provided by the client. Helps in filtering the events for notification purposes. An empty array
		indicates that there are no events to register for a particular filtering scheme as given by aEventFilter.
		
@leave  May leave with any error code.

@note   Method RegisterEvents if called with EECAMEventFilterSchemeBlackList will wipe away any white listed events set 
		previously and make them inactive. 

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::RegisterEventsL(TECAMEventFilterScheme /*aEventFilter*/, const RArray<TUid>& /*aEvents*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieve the list of ECAM component wide events for a particular event filtering scheme.

@param  aEventFilter
	    This is of type TECAMEventFilterScheme. 
	    If EECAMEventFilterSchemeBlackList, retrieve the events which are not supposed to be used for notification.
	    If EECAMEventFilterSchemeWhiteList, retrieve the events which are supposed to be used for notification.
	    
@param  aEvents
		An array of events given to the clients. Helps in filtering the events for notification purposes. An empty array
		indicates that no events have been registered so far.
		
@param  aInfo
		Retrieved TValueInfo. 
		This will be ENotActive if no event has been yet registered under the 'aEventFilter' filter scheme.
		This will be EDiscreteSteps if some events have been registered under the 'aEventFilter' filter scheme. 
				
@leave  May leave with any error code.

@note   There should be consistency in the information retrieved when GetRegisteredEvents is consecutively called,  
		first for blacklisted events and  secondly, for white listed events. Only one of them should be used at a time and 
		hence, for the other one, aInfo will be ENotActive.

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::GetRegisterEventsL(TECAMEventFilterScheme /*aEventFilter*/, RArray<TUid>& /*aEvents*/, TValueInfo& /*aInfo*/) const
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the supported flicker removal values.

@param  aSupportedFlickerRemovalValue
		A bitfield of all supported TFlickerRemoval to be provided to the client.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::GetSupportedFlickerRemovalValueL(TUint& /*aSupportedFlickerRemovalValue*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Get the current flicker removal value being used.

@param  aFlickerRemovalValue
		Currently used TFlickerRemoval value.

@leave  May leave with any error code. 

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::GetFlickerRemovalValueL(CCamera::CCameraAdvancedSettings::TFlickerRemoval& /*aFlickerRemovalValue*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Set the flicker removal value.

@param  aFlickerRemovalValue 
		The TFlickerRemoval value to be set.
		
@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEventCameraSettingFlickerRemovalValue is used to notify clients about the flicker removal value 
		setting operation.

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::SetFlickerRemovalValueL(CCamera::CCameraAdvancedSettings::TFlickerRemoval /*aFlickerRemovalValue*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the supported neutral density filter.

@param  aSupportedNDFilter
		A bitfield of all supported TNDFilter to be provided to the client.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::GetSupportedNDFilterL(TUint& /*aSupportedNDFilter*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Get the current neutral density filter being used.

@param  aNDFilter
		Currently used TNDFilter.

@leave  May leave with any error code. 

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::GetNDFilterL(CCamera::CCameraAdvancedSettings::TNDFilter& /*aNDFilter*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Set the neutral density filter.

@param  aNDFilter 
		The TNDFilter value to be set.
		
@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEventCameraSettingNDFilter is used to notify clients about the neutral density setting operation.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SetNDFilterL(CCamera::CCameraAdvancedSettings::TNDFilter /*aNDFilter*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Get the type of LED effect being used for a particular LED event.

@param  aLEDEvent
		The TLEDEvent for which the current LED effect has to be retrieved.
		
@param  aLEDEffect
		The TLEDEffect which is being used for the given LED event.
		If this is TLEDEffectCustom, then a custom LED effect would have been separately specified by the client.
		
@leave  May leave with any error code.   

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::GetLEDEffectL(CCamera::CCameraAdvancedSettings::TLEDEvent /*aLEDEvent*/, 
															CCamera::CCameraAdvancedSettings::TLEDEffect& /*aLEDEffect*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
/**
Set the LED effect for a particular LED event.

@param  aLEDEvent
		The TLEDEvent for which the LED effect has to be set.

@param  aLEDEffect
		The TLEDEffect which has to be set for the given LED event. It should not be TLEDEffectCustom.
		Use the method SetLEDCustomEffectL() to provide the custom LED effect.
		
@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEventCameraSettingLEDEffect is used to notify clients about the LED effect setting operation.

@note   The event will provide error code KErrArgument if TLEDEffectCustom is used as the TLEDEffect. Use the method 
		SetLEDCustomEffectL() to provide the custom LED effect.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SetLEDEffectL(CCamera::CCameraAdvancedSettings::TLEDEvent /*aLEDEvent*/, 
																CCamera::CCameraAdvancedSettings::TLEDEffect /*aLEDEffect*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Set the custom LED effect for a particular LED event. 

@param  aLEDEvent
		The TLEDEvent for which the custom LED effect has to be set.

@param  aLEDSpecialEffectSteps
		An array of custom LED special effect steps. Every member of this array is of type TECamLEDSpecialEffectStep.
		The array as a whole constitues a custom LED effect.
				
@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEventCameraSettingLEDCustomEffect is used to notify clients about the LED effect setting operation.

@note   Successful setting of a custom LED effect means that the GetLEDEffectL will retrieve TLEDEffectCustom for the 
		LEDEvent aLEDEvent.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SetLEDCustomEffectL(CCamera::CCameraAdvancedSettings::TLEDEvent /*aLEDEvent*/,
						const RArray<CCamera::CCameraAdvancedSettings::TECamLEDSpecialEffectStep>& /*aLEDSpecialEffectSteps*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Reserve method which are less strict than existing Reserve methods and hence, expects implementation to be considerate towards camera access/control.

@param  aMaxTimeToWait
		Maximum specified time for the client to hold the camera control before releasing (KUidECamEvent2CameraRequestTimedTakeOver)
		or being forcefully overtaken (KUidECamEvent2CameraRequestForcedTimedTakeOver).

@param  aKickOut
		ETrue indicates that if the requester's priority is greater than the current reserver's, then 
		KUidECamEvent2CameraRequestForcedTimedTakeOver will be issued to the current reserver. Otherwise, 
		KUidECamEvent2CameraRequestTimedTakeOver will be issued.
		
		EFalse indicates that KUidECamEvent2CameraRequestTimedTakeOver will be issued to the current reserver.
		
@leave  KErrNotSupported If the implementation of this method is not present.
		
@note   Event KUidECamEventNewReserveComplete notifies the client who made the new Reserve request.
        Error value KErrECamHighPriorityReserveRequesterExists indicates another such reserve request is outstanding 
	    and has higher priority than this one.
	    Error value KErrCancel indicates a new reserve requester with a higher priority than the current requester has 
	    been made and that the current requester has been cancelled.

@note   Event KUidECamEvent2CameraRequestForcedTimedTakeOver notifies the current reserver client about its camera control 
        being forcefully overtaken after a specified time period.
	   
@note   Event KUidECamEvent2CameraRequestTimedTakeOver notifies the current reserver client about a request to release its 
	    camera control within a specified time period.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::ReserveL(const TTimeIntervalMicroSeconds32& /*aMaxTimeToWait*/, TBool /*aKickOut*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Sets the priority of the client. The client issues this call when it receives the notification 
KUidECamEvent2CameraSettingChangeClientPriority. The target priority is also supplied with this notification as iParam.

@param  aPriority
		The target priority.
		
@note   For example, a higher priority client is currently holding the camera and a lower priority legacy 
		application is continuously requesting to get hold of it. After receiving a number of repeated requests by the 
		same legacy client, the supportive ECam implementation issues the notification 
		KUidECamEvent2CameraSettingChangeClientPriority. After receiving this notification, the client issues this method
		SetClientPriorityL(TInt aPriority).
		
@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SetClientPriorityL(TInt /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Restores the priority of the client. The client issues this call when it receives the notification 
KUidECamEventCameraSettingRestoreClientPriority. ECam's responsibility is to save the original priority of the client.

@note   Once the legacy client has released the camera (after changing its priority), implementation will notify the 
		client to restore its priority by event KUidECamEventCameraSettingRestoreClientPriority. After receiving this 
		notification, the client issues this method	RestoreClientPriorityL().

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::RestoreClientPriorityL()
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the supported manual gain for a particular color channel.

@param  aSupportedManualGain
		An array of manual gain. The TInt represent the unit in 'db' and multiplied by KECamFineResolutionFactor. Empty
		array indicates that feature is not supported.
		
@param  aColorChannel
		The TColorChannel for which the supported manual gain values have to be retrieved.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::GetSupportedManualGainL(RArray<TInt>& /*aSupportedManualGain*/, 
														CCamera::CCameraAdvancedSettings::TColorChannel /*aColorChannel*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Get the current manual gain value to be used for a particular color channel.

@param  aManualGain
		Currently used manual gain value. The TInt represents the unit in 'db' and multiplied by KECamFineResolutionFactor.
		
@param  aColorChannel
		The color channel for which the manual gain value has to be used. 

@leave  May leave with any error code. 

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::GetManualGainL(TInt& /*aManualGain*/, 
													CCamera::CCameraAdvancedSettings::TColorChannel /*aColorChannel*/) const
	{
	User::Leave(KErrNotSupported);	
	}												

/**
Set the manual gain value for a particular color channel.

@param  aManualGain 
		The manual gain value to be set. The TInt represents the unit in 'db' and multiplied by KECamFineResolutionFactor.
		
@param  aColorChannel
		Represents the color channel for which the manual gain value has to be set.
		
@leave  KErrNotSupported if the implementation of this method is not present.

@note   Event KUidECamEvent2CameraSettingManualGain is used to notify clients about the manual gain setting operation.

@note   The client will be expected to use either RBG color channel or YUV color channel in order to apply manual gain. 
		If the client sets manual gain values for both the color space, it is up to the implementation to decide which 
		values to use.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SetManualGainL(TInt /*aManualGain*/, 
															CCamera::CCameraAdvancedSettings::TColorChannel /*aColorChannel*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Control the white balance lock. It can be either locked or unlocked.
The white balance can be locked after aiming at a white or grey card. This helps in using the same white balance 
setting for the next image shots.

@param  aEnableLock
	    ETrue instructs to lock the white balance.
	    EFalse instructs to unlock the white balance.
	    
@leave  KErrNotSupported If the implementation of this method is not present.
	   
@note   Event KUidECamEventCameraSettingLockWhiteBalance is used to notify clients about setting the state 
		of white balance lock.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SetWhiteBalanceLockL(TBool /*aEnableLock*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Get the current state for the white balance lock.

@param  aIsLocked
		A TBool specifies whether the white balance has been locked or unlocked.
		ETrue indicates that the white balance has been locked.
		EFalse indicates that the white balance has been unlocked.
					
@leave  May leave with any error code. 

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::GetWhiteBalanceLockStateL(TBool& /*aIsLocked*/) const
	{
	User::Leave(KErrNotSupported);	
	}	
	
/**
Instructs the implementation to continuously save the current state before releasing the camera or being overtaken.

@param  aLatestCameraState
		A RWriteStream reference which will be used by the implementation to continuously save the latest camera state.
		
@note   The RWriteStream gives the implementation the facility to seek and overwrite any incorrectly updated state (in 
		the event of power failure, for example). Given stream sink shall support repositioning. 		

@note   The ECam client should be careful not to use the saved state for a particular camera in order to restore a 
		different camera later on. Though the implementation will be expected to take care of such things since the 
		content of the files are only meaningful for the implementation. 

@note   The RWriteStream reference is not valid across the process. 

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::EnableStateSavingL(RWriteStream& /*aLatestCameraState*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Instructs the implementation to stop saving the current state in the RWriteStream& as given by 
EnableStateSavingL(RWriteStream& aLatestCameraState).

@leave  May leave with any error code.

@publishedPartner
@prototype
*/	
void CCamera::CCameraAdvancedSettings::DisableStateSavingL()
	{
	User::Leave(KErrNotSupported);
	}	

/**
Instructs the implementation to resume from the last saved state. This may be used after reserving the camera.

@param  aLatestCameraState
		A RReadStream reference which will be used by the implementation to restore the latest saved camera state information.
					
@note   The RReadStream gives the implementation the facility to seek and overwrite any incorrectly updated state (in 
		the event of power failure, for example).

@note   The ECam client should be careful not to use the saved state for a particular camera in order to restore a 
		different camera later on. Though the implementation will be expected to take care of such things since the 
		content of the files are only meaningful for the implementation. 
		
@note	The RReadStream reference is not valid across the process.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::RestoreLatestStateL(RReadStream& /*aLatestCameraState*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Instructs the implementation to save the current state which will be used by the client as a custom user state.
The state saving will be done only once when this call is issued. Many such custom state could be created by the 
clients.

@param  aCustomCameraState
		A RWriteStream reference which will be used by the implementation to save the current camera state as a milestone.
		
@note   The ECam client should be careful not to use the saved state for a particular camera in order to restore a 
		different camera later on. Though the implementation will be expected to take care of such things since the 
		content of the files are only meaningful for the implementation. 

@note   The RWriteStream reference is not valid across the process. 

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::SaveCameraStateL(RWriteStream& /*aCustomCameraState*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Instructs the implementation to stop saving the custom state in the RWriteStream& as given by 
SaveCameraStateL(RWriteStream& aCustomCameraState).

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::DisableCameraStateSavingL(RWriteStream& /*aCustomCameraState*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Instructs the implementation to restore a custom saved state. 
The camera can be restored in any of the custom camera states saved earlier.

@param  aCustomCameraState
		A RReadStream reference which will be used by the implementation to restore a custom camera state
		which was saved earlier.
					
@note   The ECam client should be careful not to use the saved state for a particular camera in order to restore a 
		different camera later on. Though the implementation will be expected to take care of such things since the 
		content of the files are only meaningful for the implementation. 
		
@note	The RReadStream reference is not valid across the process.

@leave  May leave with any error code.

@publishedPartner
@prototype
*/
void CCamera::CCameraAdvancedSettings::RestoreCameraStateL(RReadStream& /*aCustomCameraState*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/**
Constructor for the TECamLEDSpecialEffectStep class.
Sets the size and version of this class.
*/
CCamera::CCameraAdvancedSettings::TECamLEDSpecialEffectStep::TECamLEDSpecialEffectStep()
	{
	iSize = sizeof(CCamera::CCameraAdvancedSettings::TECamLEDSpecialEffectStep);
	iVersion = KECamLEDSpecialEffectStepCurrentVersion;
	iBlinkingFrequency = 0;
	iIntensity = 0;
	iFrequencyStep = 0;
	iDuration = 0;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables. 
Intended to be used for implementation of methods where this class reference is passed as function arguments.
Implementation of such methods can find out whether the actual class passed is the base or the derived one. For example, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
correctly handled if the derived class variables handling is done in a proper 'if-else' statement. 

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
TUint CCamera::CCameraAdvancedSettings::TECamLEDSpecialEffectStep::Size() const
	{
	return iSize;
	}

/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.
*/
TUint CCamera::CCameraAdvancedSettings::TECamLEDSpecialEffectStep::Version() const
	{
	return iVersion;
	}

/**
Constructor for the TContinuousZoomSupportInfo class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo::TContinuousZoomSupportInfo()
	{
	iSize = sizeof(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo);
	iVersion = KContinuousZoomSupportInfoCurrentVersion;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables. 
Intended to be used for implementation of methods where this class reference is passed as function arguments.
Implementation of such methods can find out whether the actual class passed is the base or the derived one. For example, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
correctly handled if the derived class variables handling is done in a proper 'if-else' statement. 

@return  The size of the class.

@note  The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo::Size() const
	{
	return iSize;
	}

/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return  The version of the class.

@note  The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo::Version() const
	{
	return iVersion;
	}

/**
Constructor for the TContinuousZoomParameters class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraAdvancedSettings::TContinuousZoomParameters::TContinuousZoomParameters()
	{
	iSize = sizeof(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters);
	iVersion = KContinuousZoomParametersCurrentVersion;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables. 
Intended to be used for implementation of methods where this class reference is passed as function arguments.
Implementation of such methods can find out whether the actual class passed is the base or the derived one. For example, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
correctly handled if the derived class variables handling is done in a proper 'if-else' statement. 

@return  The size of the class.

@note  The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraAdvancedSettings::TContinuousZoomParameters::Size() const
	{
	return iSize;
	}

/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return  The version of the class.

@note  The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraAdvancedSettings::TContinuousZoomParameters::Version() const
	{
	return iVersion;
	}


/**
Factory function that creates a new continuous zoom object.

@param  aObserver
	    Continuous zoom observer which is passed to the implementation so it can provide callbacks to the client.

@param  aContinuousZoomType
	    The continuous zoom type that the implementation will be initialised with.

@param  aImplFactory
		A constant reference to the MImplementationFactory derived object.

@leave KErrNoMemory if out of memory; also any system wide error.

@return A pointer to a fully constructed continuous zoom object.
*/	
CCamera::CCameraContinuousZoom* CCamera::CCameraContinuousZoom::CreateL(MContinuousZoomObserver& aObserver, CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType, const MImplementationFactory& aImplFactory)
	{
	CCameraContinuousZoom* self = new (ELeave) CCameraContinuousZoom();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aContinuousZoomType, aImplFactory);
	CleanupStack::Pop(self);
	return self;
	}

/**	
CCameraContinuousZoom Constructor.
*/
CCamera::CCameraContinuousZoom::CCameraContinuousZoom()
	: iImpl(NULL)
	{
	}

/**
CCameraContinuousZoom second phase constructor. 

Function used to initialise internal state of the object.

@param  aObserver
	    Continuous zoom observer which is passed to the implementation so it can provide callbacks to the client.

@param  aContinuousZoomType
	    The continuous zoom type that the implementation will be initialised with.

@param  aImplFactory
		A constant reference to the MImplementationFactory derived object.

@leave  KErrNoMemory Out of memory. 
*/ 
void CCamera::CCameraContinuousZoom::ConstructL(MContinuousZoomObserver& aObserver, CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType, const MImplementationFactory& aImplFactory)
	{
	TAny* implPtr = NULL;
	TInt zoomTypeValue = static_cast<TInt>(aContinuousZoomType);
	TECamImplFactoryParam param(zoomTypeValue);

	User::LeaveIfError(aImplFactory.GetImpl1(implPtr, KECamMCameraContinuousZoomUid, param));

	iImpl = static_cast<MCameraContinuousZoom*>(implPtr);

	iImpl->SetContinuousZoomObserverAndHandle(aObserver, this);
	}

/**
Destructor
*/	
EXPORT_C CCamera::CCameraContinuousZoom::~CCameraContinuousZoom()
	{
	if(iImpl != NULL)
		{
		iImpl->Release();
		}
	}

/**
Starts the continuous zoom operation. Clients wil receive MContinuousZoomObserver::ContinuousZoomProgress() callback for intermediate
zoom factors achieved. Depending on the implementation, the client may or may not receive this callback for every intermediate zoom factor.
Upon completion, the client will receive MContinuousZoomObserver::ContinuousZoomComplete() callback.

@param  aContinuousZoomParameters
		The desired parameters to be used for the continuous zoom operation.

@note  If the implementation does not support re-configuring of zoom parameters whilst an existing continuous zoom operation is active then
	   StartContinuousZoomL() will leave with KErrInUse.

@note  If client has selected EDirectionTele zoom direction and the current zoom factor is greater than the target zoom factor, StartContinuousZoomL()
	   will leave with KErrArgument. Similarly, StartContinuousZoomL() will also leave with KErrArgument if client has selected EDirectionWide zoom
	   direction and current zoom factor is less than target zoom factor.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraContinuousZoom::StartContinuousZoomL(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters aContinuousZoomParameters)
	{
	iImpl->StartContinuousZoomL(aContinuousZoomParameters);
	}

/**
Stop any exisiting continuous zoom operation.
Since this method is synchronous, no callback shall be issued for the concerned continuous zoom operation.
*/
EXPORT_C void CCamera::CCameraContinuousZoom::StopContinuousZoom()
	{
	iImpl->StopContinuousZoom();
	}

/**
Retrieves information about the supported settings related to continuous zoom support.

@param  aContinuousZoomInfo
		The information of supported continuous zoom functionality.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraContinuousZoom::GetContinuousZoomSupportInfoL(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo& aContinuousZoomInfo) const
	{
	iImpl->GetContinuousZoomSupportInfoL(aContinuousZoomInfo);
	}

/**
Retrieves the unique id of the continuous zoom object.
This is used to identify the continuous zoom handle returned to clients via the MContinuousZoomObserver callback.

@param  aZoomId
		The unique id of this Continuous Zoom object.
*/
EXPORT_C void CCamera::CCameraContinuousZoom::GetContinuousZoomId(TInt& aZoomId) const
	{
	iImpl->GetContinuousZoomId(aZoomId);
	}
