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

/**
 @file
 @internalTechnology
 @released
*/
#ifndef ECAM_DEF_H
#define ECAM_DEF_H

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamuidsconst.hrh>
#include <ecamuidsdef.hrh>
#endif
/** 
Viewfinder fading effect has been set.
This event should be packed in TECAMEvent2 class.

Note: TECAMEvent2::iParam represents viewfinder handle.

@internalTechnology
*/
static const TUid KUidECamEvent2ViewFinderFadingEffect = {KUidECamEvent2ViewFinderFadingEffectUidValue};
				   
/** 
Event providing focussing feedback. The focussing feedback will be provided whenever the focussing state changes for the
selected spot combination. 
This event should be packed in TECAMEvent2 class.
Note: TECAMEvent2::iParam represents bitfield of chosen spots which are in focus.
Note: TECAMEvent2::iParam1 represents bitfield of chosen spots which are not in focus.

@internalTechnology
*/
static const TUid  KECamEvent2ImageCaptureControlFocussingInformation = {KECamEvent2ImageCaptureControlFocussingInformationUidValue};

/** 
Focussing spot combination. This event tells about completion of the setting operation for the spot combination.
This event should be packed in TECAMEvent2 class.

@note  static_cast<CCamera::CCameraAdvancedSettings::TFocusMode>(TECAMEvent2::iParam) represents the focus mode for 
	   which the spot combination has to be set for receiving focussing feedback.

@internalTechnology
*/
static const TUid KUidECamEvent2ImageCaptureControlSpotCombination = {KUidECamEvent2ImageCaptureControlSpotCombinationUidValue};

/** 
Viewfinder magnification has been set.
This event should be packed in TECAMEvent2 class.

Note: TECAMEvent2::iParam represents viewfinder handle.

@internalTechnology
*/
static const TUid KUidECamEvent2ViewFinderMagnification = {KUidECamEvent2ViewFinderMagnificationUidValue};

/**
Notifies the current camera reserver that the camera control will be forcefully overtaken by another requesting client 
after a specific time interval.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@note   TTimeIntervalMicroSeconds32(TECAMEvent2::iParam) represents the maximum time to wait. 
		TECAMEvent2::iParam needs to be passed as argument in order to construct the TTimeIntervalMicroSeconds32 object.

Note: TECAMEvent2::iParam1 represents the priority of the requestor client to whom the camera control will be forcibly 
passed after a specific time interval.

@internalTechnology
*/
static const TUid KUidECamEvent2CameraRequestForcedTimedTakeOver = {KUidECamEvent2CameraRequestForcedTimedTakeOverUidValue};

/**
Notifies the current camera reserver that another client is requesting for camera control in a specific time interval.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@note   TTimeIntervalMicroSeconds32(TECAMEvent2::iParam) represents the maximum requested time within which the current 
		reserver may release the camera if it wishes to do so.
		TECAMEvent2::iParam needs to be passed as argument in order to construct the TTimeIntervalMicroSeconds32 object.

Note: TECAMEvent2::iParam1 represents the priority of the requestor client to whom the camera control will be passed 
should the current reserver wish to do so.

@internalTechnology
*/
static const TUid KUidECamEvent2CameraRequestTimedTakeOver = {KUidECamEvent2CameraRequestTimedTakeOverUidValue};

/**
Notifies the manual gain setting completion for the particular channel.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

Note: TECAMEvent2::iParam represents the specific channel for which the manual gain value has been set.

@internalTechnology
*/
static const TUid KUidECamEvent2CameraSettingManualGain = {KUidECamEvent2CameraSettingManualGainUidValue};

/**
Retrieves the optimal focussing feedback while using manual focus. This will be issued as a result of setting operation
CCamera::CCameraAdvancedSettings::SetFocusDistance(TInt aDistance).

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@note   If TECAMEvent2::iParam > 1, information is unavailable; if TECAMEvent2::iParam < 0, error case. 
		Otherwise, static_cast<TBool>(TECAMEvent2::iParam) retrieves whether optimal focussing has been achieved or not.

@internalTechnology
*/
static const TUid KUidECamEvent2CameraSettingFocusDistance = {KUidECamEvent2CameraSettingFocusDistanceUidValue};

/**
Instructs the client to change its priority in order to allow the legacy client to get hold of the camera. Client should
restore their priority when they receive the notification 'KUidECamEventCameraSettingRestoreClientPriority'.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

Note: TECAMEvent2::iParam represents the target priority to which the client should set itself using the method SetClientPriorityL()

@internalTechnology
*/
static const TUid KUidECamEvent2CameraSettingChangeClientPriority  = {KUidECamEvent2CameraSettingChangeClientPriorityUidValue};

/** 
Event indicating image enhancement setting has been applied. 
This event should be packed in TECAMEvent2 class.

Note: TECAMEvent2::iParam represents viewfinder handle.

@internalTechnology
*/
static const TUid KUidECamEvent2ViewFinderImageEnhancement = {KUidECamEvent2ViewFinderImageEnhancementUidValue};
	
#endif // ECAM_DEF_H
