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
* Description:  Declaration of DosServer enumerations
*
*/


#ifndef __DSYENUMS_H__
#define __DSYENUMS_H__

enum TStartupSTResult
    {
    EStartupSTNotStarted = 0,
    EStartupSTNotReady = 1,
    EStartupSTFail = 2,
    EStartupSTOk = 3
    };

enum TSysApMtcStateType 
    {
    ESysApMtcChargingState,
    ESysApMtcAlarmState,
    ESysApMtcPowerOffState, 
    ESysApMtcNormalState,
    ESysApMtcRfInactiveState
    };

// RFS Types, must be same as in RFS module
enum TRfsType
    {
    ERfsNormal  = 0,
    ERfsDeep    = 1
    };

#endif  // DSYENUMS_H

// End of file
