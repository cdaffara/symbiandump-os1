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
#ifndef  ECAMADVSETTINGS_CONST_H
#define  ECAMADVSETTINGS_CONST_H

#include <ecamadvsettingsuids.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsuidsconst.hrh>
#endif


/**
Notifies the client that continuous zoom limit has been reached.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@publishedPartner
@prototype
*/
static const TUid KUidECamEventCameraSettingContinuousZoomReachedLimit = {KUidECamEventCameraSettingContinuousZoomReachedLimitUidValue};

/**
Notifies the client about the setting of performance operation preference.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@publishedPartner
@prototype
*/
static const TUid KUidECamEventCameraSettingOperationPreference = {KUidECamEventCameraSettingOperationPreferenceUidValue};



/** 
Used to for video telephony.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

@publishedPartner
@prototype
*/
static const TUid  KUidECamPresetVideoTelephony		= {KUidECamPresetVideoTelephonyUidValue};

/** 
Used to clarify that camera is not under any preset mode. Possible scenario: client sets camera in a particular preset 
mode and then makes some setting changes on top of it. Then theoretically camera is out of that preset. Hence, 
KUidECamPresetNone will be used in such cases.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

@publishedPartner
@prototype
*/
static const TUid  KUidECamPresetNone		= {KUidECamPresetNoneUidValue};
 
/** Notifications related to presets */
/**
Used to notify clients about possible range restrictions, when camera works under a particular preset mode.
This is not a particular preset uid.

@note   Call CCamera::CCameraPresets::GetRangeRestrictedSettingsL(RArray<TUid>& aRangeRestrictedSettings) to retrieve 
		the list of settings whose range have been restricted.

@publishedPartner
@prototype
*/
static const TUid  KUidECamEventRangeRestricted  = {KUidECamEventRangeRestrictedUidValue};

/**
Used to notify clients about possible feature restrictions, when camera works under a particular preset mode.
This is not a particular preset uid.

@note   Call CCamera::CCameraPresets::GetFeatureRestrictedSettingsL(RArray<TUid>& aFeatureRestrictedSettings) to retrieve 
		the list of settings which have been restricted.

@publishedPartner
@prototype
*/
static const TUid  KUidECamEventFeatureRestricted  = {KUidECamEventFeatureRestrictedUidValue};

/**
Used to notify clients that locking of the preset operation has completed, when camera works under a particular preset mode.
This is not a particular preset uid.

@publishedPartner
@prototype
*/
static const TUid  KUidECamEventPresetLocked  = {KUidECamEventPresetLockedUidValue};

/**
Used to notify clients that unlocking of the preset operation has completed, when camera works under a particular preset mode.
This is not a particular preset uid.

@publishedPartner
@prototype
*/
static const TUid  KUidECamEventPresetUnlocked  = {KUidECamEventPresetUnlockedUidValue};

	
#endif // ECAMADVSETTINGS_CONST_H



