/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains Central Repository key definitions
*                that require some action from Accessory Framework, if their
*        value changes.
*
*/


#ifndef ACCSETTINGSDOMAINCRKEYS_H
#define ACCSETTINGSDOMAINCRKEYS_H

// INCLUDES

// CONSTANTS
const TUid KCRUidAccServer = {0x1020505A};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

/**
* Phone may support some HW devices that are not properly identified
* when they are connected to phone. To properly identify such devices
* user has to tell by using appropriate user interface setting the type
* of connected device.
*
* Possible integer values:
*
* 0 TTY
* 1 Loopset
* and maybe some phone specific values
*/
const TUint32 KAccServerHWDevices = 0x00000001;

/**
* Display lights setting for wired headset mode.
*
* User may force lights on when wired headset is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerWiredHSLights = 0x00000002;

/**
* Display lights setting for wireless headset mode.
*
* User may force lights on when wireless headset is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerWirelessHSLights = 0x00000003;

/**
* Display lights setting for wired car kit mode.
*
* User may force lights on when wired car kit is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerWiredCarKitLights = 0x00000004;

/**
* Display lights setting for wireless car kit mode.
*
* User may force lights on when wireless car kit is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerWirelessCarKitLights = 0x00000005;

/**
* Display lights setting for text device mode.
*
* User may force lights on when text device is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerTextDeviceLights = 0x00000006;

/**
* Display lights setting for loopset.
*
* User may force lights on when loopset is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerLoopsetLights = 0x00000007;

/**
* Display lights setting for music stand.
*
* User may force lights on when music stand is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerMusicStandLights = 0x00000008;

/**
* Display lights setting for synchronization stand.
*
* User may force lights on when synchronization stand is connected to phone.
*
* Possible integer values:
*
* 0 lights off
* 1 lights on
*
* Default value: 0
*/
const TUint32 KAccServerSyncStandLights = 0x00000009;


#endif  // ACCSETTINGSDOMAINCRKEYS_H

// End of File
