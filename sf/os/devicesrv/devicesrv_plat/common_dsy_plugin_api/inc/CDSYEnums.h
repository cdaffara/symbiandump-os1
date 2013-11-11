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
* Description:  CDSY enumeration identifiers
*
*/


#ifndef CDSYENUMS_H
#define CDSYENUMS_H

// INCLUDES
// CONSTANTS

enum TCDSYRfsType
    {
    ECDSYRfsNormal  = 0,
    ECDSYRfsDeep    = 1
    };

enum TCDSYMTCState
    {
    ECDSYMtcChargingState,
    ECDSYMtcAlarmState,
    ECDSYMtcPowerOffState, 
    ECDSYMtcNormalState,
	ECDSYMtcRfInactiveState,
    ECDSYMtcBTSAPState
    };


// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



#endif //#ifndef CDSYENUMS_H



//  End of File
