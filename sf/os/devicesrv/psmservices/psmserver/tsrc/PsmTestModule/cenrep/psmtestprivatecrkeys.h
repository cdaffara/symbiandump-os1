/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for domain cenrep keys for PSM settings
*
*/



#ifndef PSMSERVERDOMAINCRKEYS_H
#define PSMSERVERDOMAINCRKEYS_H

#include <e32cmn.h>

/**
 * PSM Settings API
 *
 * Contains PSM configuration settings
 */
const TUint32 KCRUidPsmtestCenRepUidValue = 0x10200EE3;
const TUid KCRUidPsmtestCenRepUid = { KCRUidPsmtestCenRepUid };

/**
 * PSM Mode notification
 *
 * Indicates the state of PowerSave mode
 *  value 0: Power Save Mode is not active
 *  value 1: Power Save Mode is active
 * 
 * Value is an integer.
 *
 * Default value: 0
 */
const TUint32 KPsmPowerSaveModeStatus    = 0x00000001;

#endif //PSMSERVERDOMAINCRKEYS_H

// End of File
