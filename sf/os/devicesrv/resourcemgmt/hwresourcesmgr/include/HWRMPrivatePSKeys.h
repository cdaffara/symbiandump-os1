/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMPRIVATEPSKEYS_H
#define HWRMPRIVATEPSKEYS_H

// INCLUDES
#include <e32std.h>

// =============================================================================
// HW Resource Notification Private PubSub API
// =============================================================================
const TUid KPSUidHWResourceNotification = {0x101F7A01}; 

/**
* Vibra usage status.
* This data is interpreted as CHWRMVibra::TVibraStatus.s
*/
const TUint32 KHWRMVibraStatus = 0x00001001;

/**
* Light usage statuses for various light targets.
* This data is dependent on CHWRMLight::TLightTarget enumeration.
*
* Each 4 bit set is interpreted as CHWRMLight:TLightStatus
*/
const TUint32 KHWRMLightStatus  = 0x00002001;

// =============================================================================
// HWRM Internal Blocking API
// =============================================================================
const TUid KPSUidHWRMPrivate = {0x10205077}; 

/**
* Vibra blocking internally by HWRM.
*
* HWRM Server sets this P&S key to a non-zero value when it needs
* vibra to be blocked. Keys are handled as bitmask, so it should be 
* defined accordingly (i.e. not to define same bits as e.g. KHWRMAccessoryVibraBlocked)

*/
//const TUint32 KHWRMInternalVibraBlocked = 0x00003001;
const TUint32 KHWRMInternalVibraBlocked = 0x00004000;



#endif      // HWRMPRIVATEPSKEYS_H

// End of File

