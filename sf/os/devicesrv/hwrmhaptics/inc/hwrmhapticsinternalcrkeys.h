/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Central Repository keys for hwrmhaptics.
*
*/


#ifndef HWRMHAPTICSINTERNALCRKEYS_H
#define HWRMHAPTICSINTERNALCRKEYS_H

#include <e32std.h>

const TUid KCRUidHWRMHapticsPlugins = {0x200184B9};

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'any' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginAnyPacketizer                     = 0x00000001;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginAnyAdaptation                     = 0x00000002;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginDevicePacketizer                  = 0x00000003;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginDeviceAdaptation                  = 0x00000004;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginPrimaryDisplayPacketizer      = 0x00000005;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginPrimaryDisplayAdaptation      = 0x00000006;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginSecondaryDisplayPacketizer    = 0x00000007;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginSecondaryDisplayAdaptation    = 0x00000008;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginGamePacketizer                = 0x00000009;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginGameAdaptation                = 0x0000000A;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginGameLeftPacketizer            = 0x0000000B;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginGameLeftAdaptation            = 0x0000000C;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginGameRightPacketizer           = 0x0000000D;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginGameRightAdaptation           = 0x0000000E;

/**
* Implementation UID of the plugin, which is used to encode/decode messages
* for 'device' actuator.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginExternalVibraPacketizer       = 0x0000000F;

/**
* Implementation UID of the plugin, which is used to send messages to
* adaptation layer.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 0x00000000
**/
const TUint32 KHWRMHapticsPluginExternalVibraAdaptation       = 0x00000010;

#endif  // HWRMHAPTICSINTERNALCRKEYS_H

// End of File
