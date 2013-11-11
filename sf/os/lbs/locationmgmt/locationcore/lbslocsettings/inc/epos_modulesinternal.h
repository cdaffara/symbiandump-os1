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



#ifndef EPOS_MODULESINTERNAL_H_
#define EPOS_MODULESINTERNAL_H_

#include <centralrepository.h>
#include <lbscommon.h>
#include <lbs/positioningpluginfwinternalcrkeys.h>
#include "LocationSettingsPrivateCRKeys.h"

// Positioning Plug-In ECom interface UID
const TUid KPosPsyInterfaceUid = {0x101f7a7c}; 

// Bitmask values to identify if CPosModuleUpdate is changed
const TInt KPosModUpdateAvailability = 0x0001;
const TInt KPosModUpdateCost         = 0x0002;
const TInt KPosModUpdateVisibility   = 0x0004;

enum TModuleVisibility
    {
    EModuleInvisible = 0,
    EModuleVisible
    };

enum TModuleStatus
    {
    EModuleNotAvailable = 0,
    EModuleAvailable
    };

// Field delimeter for the repository keys
const TUint KFieldDelimiter = ',';          // comma
_LIT( KAttributedKeyItemFormat, "%08X,%u,%u" );
_LIT( KIdListKeyItemFormat, "%08X" );

const TInt KUidFieldLength = 8;             // UID is eight hex digits
const TInt KStatusFieldLength = 1;          // 0 or 1
const TInt KCostFieldLength = 10;           // int32 value - up to 10 digits
const TInt KDelimiterLength = 1;

const TInt KAttributedKeyMaxItemLength = 22;  // "Uid,Status,Cost,"
const TInt KIdListKeyMaxItemLength = 9;  	// "Uid,"

// Transaction-related constants
const TInt KCenrepLockWait = 100 * 1000; 	// 0.1 sec
const TInt KMaxCenrepLockAttempts = 5; 		// 0.5 sec max

// Error-handling
_LIT( KPosModulesPanicCategory, "LocationSettings" );

enum TPosModulesPanic
    {
    /** A parameter was set that was out of the expected range (for example a negative value)*/
    EPosLocSetPanicParameterOutOfRange,
    /** An attempt was made to set a CPosModules settings observer when it is already set  */
    EPosLocSetPanicObserverIsAlreadySet,
    /** The CPosModules destructor was called before the settings observer was unset */
    EPosLocSetPanicObserverNotStopped
    };

GLDEF_C void Panic( TInt aReason );

// Internal datatypes

GLDEF_C TInt ParseModuleUid( const TPtrC& aStr, TUint32& aId );
GLDEF_C void ReplaceCommasWithWhitespace( TPtr aString );

#endif      // EPOS_MODULESINTERNAL_H_
