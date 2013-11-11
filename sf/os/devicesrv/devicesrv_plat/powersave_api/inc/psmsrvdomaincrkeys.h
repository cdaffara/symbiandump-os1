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
* Description:  This file contains domain central repository key definitions
 *                for Power Save Mode
 *
*/


#ifndef PSMSRVDOMAINCRKEYS_H
#define PSMSRVDOMAINCRKEYS_H

//  INCLUDES
#include <e32cmn.h>

// CONSTANTS
const TUid KCRUidPowerSaveMode = {0x2000B189};

// PSM Notification API

/**
 * Current power save mode
 *
 * This value is used by PSM Server to store currently active power save mode. 
 * it is also used by applications that needs to know when power save mode changes 
 * or wants to know which is current power save mode.
 *
 * Possible integer values are defined in psmtypes.h in enumeration TPsmsrvMode:
 *
 * Default value: 0
 */
const TUint32 KPsmCurrentMode                   = 0x00000001;

#endif  // PSMSRVDOMAINCRKEYS_H
            
// End of File
