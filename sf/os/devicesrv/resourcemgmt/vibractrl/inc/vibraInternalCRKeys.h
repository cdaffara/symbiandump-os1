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
* Description:  This file contains internal central repository key definitions
*                for the vibra
*
*/


#ifndef VIBRAINTERNALCRKEYS_H
#define VIBRAINTERNALCRKEYS_H

// NOTE:
// In Series 60 3.0 and above, please include corresponding HWRM CR header file
// HWRMVibraSDKCRKeys.h instead of this file.

// CONSTANTS
const TUid KCRUidVibraCtrl = {0x10200C8B};

// Vibra Settings API

/**
* Vibra setting in active profile. 
* 
* Possible integer values:
*
* 0 (EOff) Vibra is off in active profile (default value)
* 1 (EOn) Vibra is on in active profile
*/
const TUint32 KVibraCtrlProfileVibraEnabled    = 0x00000001;

/**
* Maximum vibrating time supported by device. 
* 
* Value is milliseconds as an integer. 
*
* Value is read-only.
*
* Default value: 10000.
*/
const TUint32 KVibraCtrlMaxTime    = 0x00000002;

#endif  // VIBRAINTERNALCRKEYS_H
            
// End of File
