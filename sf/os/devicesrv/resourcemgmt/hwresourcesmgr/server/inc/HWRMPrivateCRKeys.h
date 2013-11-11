/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMPRIVATECRKEYS_H
#define HWRMPRIVATECRKEYS_H

//  INCLUDES
//#include "HWRMInternalCRKeys.h"

// CONSTANTS
const TUid KCRUidHWRMSettings = {0x10205043};

// HWRM Settings API keys

/**
* Timeout in milliseconds for HWRM plugin calls. 
*
* Must have a positive value.
*/
const TUint32 KHWRMPluginTimeout     = 0x00000001;

/**
* Stores audio server SID. All vibra profile setting checks
* in all vibra calls from that client are bypassed.
*/
const TUint32 KHWRMAudioVibraSID     = 0x00000002;

/**
* Stores HWRM local variation flags bitmask:
* Bit 1: KHWRMInternalSettingsChargerBlockVibra
*/
const TUint32 KHWRMInternalSettings  = 0x00000003;

/** 
* Defines bit 1 in KHWRMInternalSettings. 
* If this bit is 1, vibra will be blocked by charger 
* (I.e. all StartVibraL calls will return KErrLocked if charger is connected). 
* If this bit is 0, charger doesn't affect vibra. By default this is 1.
* Note when changing this value, that DOS side might have some low level vibra blocks,
* which this key doesn't affect. The value should reflect those blocks if they exist.
*/
const TInt KHWRMInternalSettingsChargerBlockVibra = 0x01;  // 2^0

/** 
* Defines bit 2 in KHWRMInternalSettings. 
* If this bit is 1, tactile feedback vibration will be blocked by charger 
* (I.e. all PulseL calls will return KErrLocked if charger is connected). 
* If this bit is 0, charger doesn't affect feedback vibration. By default this is 1.
* Note when changing this value, that DOS side might have some low level vibra blocks,
* which this key doesn't affect. The value should reflect those blocks if they exist.
*/
const TInt KHWRMInternalSettingsChargerBlockFeedback = 0x02;  // 2^1

/** 
* Defines bit 3 in KHWRMInternalSettings. 
* If this bit is 1, tactile feedback vibration will be blocked by accessories 
* (accessory server or other component is responsible for setting KHWRMAccessoryVibraBlocked). 
* If this bit is 0, accessories do not affect feedback vibration. By default this is 1.
* Note when changing this value, that DOS side might have some low level vibra blocks,
* which this key doesn't affect. The value should reflect those blocks if they exist.
*/
const TInt KHWRMInternalSettingsAccessoryBlockFeedback = 0x04;  // 2^2

/** 
* Defines bit 4 in KHWRMInternalSettings. 
* If this bit is 1, tactile feedback vibration will be blocked by cooling off period. 
* (I.e. all PulseL calls will return KErrLocked if vibra is cooling off). 
* If this bit is 0, cooling off period doesn't affect vibra. By default this is 1.
*/
const TInt KHWRMInternalSettingsCoolOffBlockFeedback = 0x08;  // 2^3

/** 
* Defines bit 5 in KHWRMInternalSettings. 
* If this bit is 1, HWRM does not generate separate start and stop commands to control 
* tactile feedback pulse requests (i.e. when client app uses PulseVibraL API), but instead
* it sends new pulse command to plugin. It is hence on licensee's responsibility to control 
* vibra start and stop. Licensee implementation should handle pulse requests in such a way, that
* if there is ongoing vibration when pulse request arrives, pulse request should just overlap
* with ongoing vibration, not to stop it. If this cannot be done, bit should be configured as 0. 
* If this bit is 0, HWRM does generate start and stop commands to plugin also in case of 
* PulseVibraL call as it does for StartVibraL calls. By default this is 1.
*/
const TInt KHWRMInternalSettingsDeviceSupportsPulse = 0x10;  // 2^4


/**
* HWRM process priority
*
* Value must correspond to enumerated values in TProcessPriority enumeration.
* Legal values are from the range EPriorityLow-EPriorityHigh. Default value EPriorityForeground.
*/
const TUint32 KHWRMPriority  = 0x00000004;


// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// 

#endif  // HWRMPRIVATECRKEYS_H
            
// End of File
