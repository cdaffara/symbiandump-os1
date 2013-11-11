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
 @internalTechnology
 @released
*/
#ifndef  ECAMADVSETTINGS_DEF_H
#define  ECAMADVSETTINGS_DEF_H
#include <ecamadvsettingsuids.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsuidsconst.hrh>
#include <ecamadvsettingsuidsdef.hrh>
#endif

/**
The current Version of the TECamLEDSpecialEffectStep class.

@internalTechnology
*/
static const TUint KECamLEDSpecialEffectStepCurrentVersion = 1;



/**
Notifies the client that the flicker removal value has been set.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@internalTechnology
*/
static const TUid KUidECamEventCameraSettingFlickerRemovalValue = {KUidECamEventCameraSettingFlickerRemovalValueUidValue};

/**
Notifies the client about the setting of neutral density filter.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@internalTechnology
*/
static const TUid KUidECamEventCameraSettingNDFilter = {KUidECamEventCameraSettingNDFilterUidValue};

/**
Notifies the client about the setting of LED effect.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@internalTechnology
*/
static const TUid KUidECamEventCameraSettingLEDEffect = {KUidECamEventCameraSettingLEDEffectUidValue};

/**
Notifies the client about the setting of LED 'custom' effect.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@internalTechnology
*/
static const TUid KUidECamEventCameraSettingLEDCustomEffect = {KUidECamEventCameraSettingLEDCustomEffectUidValue};

/**
Notifies the client about the white balance lock setting.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@internalTechnology
*/
static const TUid KUidECamEventCameraSettingLockWhiteBalance = {KUidECamEventCameraSettingLockWhiteBalanceUidValue};

/**
Instructs the client to restore its original priority.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@internalTechnology
*/
static const TUid KUidECamEventCameraSettingRestoreClientPriority = {KUidECamEventCameraSettingRestoreClientPriorityUidValue};

/**
The current Version of the TContinuousZoomSupportInfo class.

@internalTechnology
@prototype
*/
static const TUint KContinuousZoomSupportInfoCurrentVersion = 1;

/**
The current Version of the TContinuousZoomParameters class.

@internalTechnology
@prototype
*/
static const TUint KContinuousZoomParametersCurrentVersion = 1;

/**
	UID used to identify the CCameraContinuousZoom API.
	This API is used to provide more advanced control over continuous zoom.
	@see CCameraContinuousZoom

@internalTechnology
@prototype
*/
static const TUid  KECamContinuousZoomUid 			= {KECamContinuousZoomUidValue};
	
#endif // ECAMADVSETTINGS_DEF_H



