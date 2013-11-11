/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains private central repository key definitions
*               for the tilt compensation
*
*/


#ifndef TCPRIVATECRKEYS_H
#define TCPRIVATECRKEYS_H

//  INCLUDES
#include <e32cmn.h>
 
// CONSTANTS
const TUid KCRUidTiltCompensationParameters = {0x2001FCBA};

// Tilt compensatin parameters API

/**
* Tilt compensation parameter k.
* 
* 
* Value type: floating point.
*   
*/
const TUint32 KTiltCompensationParameterK    = 0x00000001;

/**
* Tilt compensation parameter m. 
* 
* 
* Value type: floating point.
*    
*/
const TUint32 KTiltCompensationParameterM    = 0x00000002;

/**
* Tilt compensation parameter treshold angle 1. 
* 
* 
* Value type: floating point.
*   
*/

const TUint32 KTiltCompensationParameterTresholdAngle1    = 0x00000003;

/**
* Tilt compensation parameter treshold angle 2. 
* 
* 
* Value type: floating point.
*   
*/

const TUint32 KTiltCompensationParameterTresholdAngle2    = 0x00000004;

/**
* Tilt compensation accelerometer window size. 
* 
* 
* Value type: floating point.
*   
*/

const TUint32 KTiltCompensationAccWinSize    = 0x00000005;

/**
* Tilt compensation magnetometer window size. 
* 
* 
* Value type: floating point.
*   
*/

const TUint32 KTiltCompensationMagWinSize    = 0x00000006;


#endif  // TCPRIVATECRKEYS_H
            
// End of File
