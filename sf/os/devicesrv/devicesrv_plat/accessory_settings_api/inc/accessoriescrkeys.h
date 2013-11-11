/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides accessory related settings to the user.
*
*/


#ifndef ACCESSORIESCRKEYS_H
#define ACCESSORIESCRKEYS_H

#include <e32def.h>

/**
* Accessory settings CR UID
*/
const TUid KCRUidAccessorySettings = {0x101F8779};

/**
* Automatic answer in carkit
*
* Possible values are:
* 0 = No automatic answer in headset
* 1 = Automatic answer in headset
*/
const TUint32 KSettingsCarKitAutomaticAnswer = 0x00000001;

/**
* Automatic answer in headset
*
* Possible values are:
* 0 = No automatic answer in headset
* 1 = Automatic answer in headset
*/
const TUint32 KSettingsHeadsetAutomaticAnswer = 0x00000002;

/**
* Automatic answer in bluetooth
*
* Possible values are:
* 0 = No automatic answer in bluetooth
* 1 = Automatic answer in bluetooth
*/
const TUint32 KSettingsBTAudioAutomaticAnswer = 0x00000003;

/**
* Information about if the loopset is in use.
*
* Possible values are:
* 0 = Loopset is not in use
* 1 = Loopset is in use
*/
const TUint32 KSettingsLoopsetInUse = 0x00000004;

/**
* Automatic answer in loopset
*
* Possible values are:
* 0 = No automatic answer in loopset
* 1 = Automatic answer in loopset
*/
const TUint32 KSettingsLoopsetAutomaticAnswer = 0x00000005;

/**
* Information about which accessory is in use.
*
* Possible values are:
* 0 = headset
* 1 = loopset
* 2 = TTY
**/
const TUint32 KSettingsAccessoryInUse =  0x00000006;

/**
* Automatic answer in music stand
*
* Possible values are:
* 0 = No automatic answer in music stand
* 1 = Automatic answer in music stand
*/
const TUint32 KSettingsMusicStandAutomaticAnswer =  0x00000007;

/**
* Automatic answer in car kit
*
* Possible values are:
* 0 = No automatic answer in car kit
* 1 = Automatic answer in car kit
*/
const TUint32 KSettingsTTYAutomaticAnswer =  0x00000008;

/**
* Use loopset
*
* Possible values are:
* 0 = Loopset not in use
* 1 = Loopset in use
*/
const TUint32 KSettingsUseLoopset =  0x00000009;

/**
* Wireless Carkit automatic answer
*
* Possible values are:
* 0 = Automatic answer is off
* 1 = Automatic answer is on
*/
const TUint32 KSettingsWirelessCarkitAutomaticAnswer =  0x00000010;

/**
* Headset Ringing Tone Routing
*
* Possible values are:
* 0 = Routing is Headset and Handset
* 1 = Routing is Headset
*/
const TUint32 KSettingsHeadsetRingingToneRouting = 0x00000011;

/**
* Headphones Ringing Tone Routing
*
* Possible values are:
* 0 = Routing is Headset and Handset
* 1 = Routing is Headset
*/
const TUint32 KSettingsHeadphonesRingingToneRouting = 0x00000012;

/**
* Default Accessory.
* Possible values are:
* 0 = Headset
* 1 = Headphones
* 2 = Wired Car kit
* 3 = Wireless carkit
* 4 = Music Stand
* 5 = Tvout
* 6 = Loopset
* 7 = Textphone
*/
const TUint32 KSettingsAccDefaultInfo = 0x00000013;

/**
* Wired carkit accessory support
*
* Possible values are:
* 0 = accessory is not supported
* 1 = accessory is supported
*/
const TUint32 KSettingsAccWiredCarkitSupported = 0x00000014;

/**
* Wireless carkit accessory support
*
* Possible values are:
* 0 = accessory is not supported
* 1 = accessory is supported
*/
const TUint32 KSettingsAccWirelessCarkitSupported = 0x00000015;

/**
* Loopset accessory support
*
* Possible values are:
* 0 = accessory is not supported
* 1 = accessory is supported
*/
const TUint32 KSettingsAccLoopsetSupported = 0x00000016;

/**
* Profile settings CR UID.
*/
const TUid KCRUidProfileSettings = {0x101F877D};

/**
* Bluetooth default profile, profile id.
*
*/
const TUint32 KSettingsBTDefaultProfile = 0x00000001;

/**
* Headset default profile, profile id.
*/
const TUint32 KSettingsHeadsetDefaultProfile = 0x00000002;

/**
* TTY default profile, profile id.
*/
const TUint32 KSettingsTTYDefaultProfile = 0x00000003;

/**
* Loopset default profile, profile id.
*/
const TUint32 KSettingsLoopsetDefaultProfile = 0x00000004;

/**
* Music Stand default profile, profile id.
*/
const TUint32 KSettingsMusicStandDefaultProfile = 0x00000005;

/**
* Car Kit default profile, profile id.
*/
const TUint32 KSettingsCarKitDefaultProfile = 0x00000006;

/**
* Wireless Car Kit default profile, profile id.
*/
const TUint32 KSettingsWirelessCarkitDefaultProfile = 0x00000007;

/**
* Headphones default profile, profile id.
*/
const TUint32 KSettingsHeadphonesDefaultProfile = 0x00000008;

/**
* Tv out default profile, profile id.
*/
const TUint32 KSettingsTvOutDefaultProfile = 0x00000009;

/**
* Settings for Lights features
*/
const TUid KCRUidAccessoryLightSettings = {0x10207194};

/**
* Defines the display light duration for Music Stand accessory.
*
* 0 = (Normal)
* 1 = (Always ON)
*/
const TUint32 KSettingsMusicStandLights = 0x00000001;

/**
* Defines the display light duration for Car kit accessory.
*
* 0 = (Normal)
* 1 = (Always ON)
*/
const TUint32 KSettingsCarKitLights = 0x00000002;


/** Following keys belong to category KCRUidTvoutSettings  */

/**
* TV-out settings CR UID
*/
const TUid KCRUidTvoutSettings = {0x1020730B};

/**
* TV-out aspect ratio.
* Possible values are:
* 0 = 4x3
* 1 = 16x9
*/
const TUint32 KSettingsTvAspectRatio = 0x00000001;

/**
* TV-out system info.
* Possible values are:
* 0 = PAL
* 1 = PALM
* 2 = NTSC
*/
const TUint32 KSettingsTvSystemInfo = 0x00000002;

/**
* TV-out default text.
* String value.
*/
const TUint32 KSettingsTvDefaultText = 0x00000003;


/**
* TV-out Flicker Filter.
* Possible values are:
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsTvoutFlickerFilter = 0x00000004;

/**
* TV-out Horizontal overscan
* Default value: 500
*/
const TUint32 KSettingsTvoutHorizontalOverscan = 0x00000005;

/**
* TV-out Vertical overscan
* Default value: 500
*/
const TUint32 KSettingsTvoutVerticalOverscan = 0x00000006;

/**
* PALM option visibility
* 0 = not visible (default)
* 1 = visible
*/
const TUint32 KSettingsTvoutPalmVisibility = 0x00000007;



#endif // ACCESSORIESCRKEYS_H
