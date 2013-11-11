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
* Description:  Internal definitions file
*
*/


#ifndef SENSRVDEFINES_H
#define SENSRVDEFINES_H

#include <e32base.h>
#include "sensrvtrace.h"

/**
* Sensor server process name
*/
_LIT(KSensrvProcessName, "!SensorServer"); 

/**
* Sensor server main thread name
*/
_LIT(KSensrvMainThreadName, "SensrvMain");

/**
* SSY thread name prefix. A number is appended to this.
*/
_LIT(KSensrvSsyThreadNameBase, "Ssy@");

/**
* Length of the KSensrvSsyThreadNameBase _LIT
*/
const TInt KSensrvSsyThreadNameBaseLen(4);

/**
* SSY Thread name maxlength
*/
const TInt KSensrvSsyThreadNameMaxLen(16);

/**
* Sensor server executable name
*/
_LIT( KSensrvExeName, "sensorserver.exe" );

/**
* Drive identifier of the drive where sensor server executable is.
*/
_LIT( KSensrvExeDrive, "z:" );

/**
* Panic category
*/
_LIT( KSensrvPanicCategory, "SensorServer" );

/**
* Number of times sensor server client should try reconneting if session is not found.
*/
const TInt KSensrvClientTryCount(2);

/**
* Major version number
*/
const TInt KSensrvVersionMajor(1);

/**
* Minor version number
*/
const TInt KSensrvVersionMinor(0);

/**
* Build number
*/
const TInt KSensrvVersionBuild(0);

/**
* Priority for server's active objects
*/
const TInt KSensrvCActivePriority(CActive::EPriorityHigh);

/**
* Async message slots to be used in client server comms. 
* - Data listener
* - Condition listener
* - Property listener
* - New channel listener?
*/
const TInt KSensrvDefaultAsyncSlots(4); 

/**
* Separate TInt needed or DLL's won't compile because of static data if UID is used in some policy lit
*/
const TInt KSensrvUid2Int(0x1020507E);  

/**
* Sensor server UID
*/
const TUid KSensrvUid2 = { KSensrvUid2Int }; 

/**
* Millisecond to microsecond multiplier
*/
const TInt KMillisToMicros(1000);

/**
* Second to microsecond multiplier
*/
const TInt KSecondsToMicros(1000000);

/**
* Default timeout of transactions, in microseconds
*/
const TInt KSensrvDefaultTransactionTimeout(60000000); 

/**
* SID of the test framework. This is the only process that
* is allowed to shutdown the sensor server.
*/
const TUint32 KSensrvTestFrameworkUid(0x102073E4);

/**
* Default data rate multiplier.  
* CSensrvChannel.iMaxBufferingCount is set to channel data rate
* times multiplier divided by KSensrvDataBufferLengthMultiplier.
*/
const TInt KSensrvDefaultDataRateMultiplier(5);

/**
* Buffer length multiplier.  
* Buffer lenght is set to CSensrvChannel.iMaxBufferingCount times this.
* For practical reason, buffering count should always be less than half of buffer size, as
* otherwise getting new block will invalidate some of the just written block.
* To allow some wiggle-room for possible client delays, minimum practical value for this is 3.
*/
const TInt KSensrvDataBufferLengthMultiplier(4);

/**
* CSensrvChannel.iMaxBufferingCount will be set to this if it cannot be 
* calculated.
*/
const TInt KSensrvDefaultMaxBufferingCount(64);

/**
* Buffer length must be at least this much.
*/
const TInt KSensrvMinimumBufferLength(3);

/**
* Minimum buffering count for clients and SSY
*/
const TInt KSensrvMinimumBufferingCount(1);

/**
* Minumum acceptable buffering period in milliseconds.
* For practical reasons 1/64th of second is the effective
* minimum anyway, as that is the limit of normal timer resolution.
*/ 
const TInt KSensrvMinimumBufferingPeriod(15);

/**
* Maximum allowed buffering period in milliseconds
*/
const TInt KSensrvMaximumBufferingPeriod(30*60*1000); // 30 minutes

/**
* Default SSY stack size
*/
const TInt KSensrvDefaultSsyStackSize(4096);

/**
* Default SSY heap max size
*/
const TInt KSensrvDefaultSsyHeapMaxSize(65535);

/**
* SSY heap initial size
*/
const TInt KSensrvSsyHeapInitialSize(4096);

/**
* SSY thread graceful cleanup time, microseconds
*/
const TInt KSensrvSsyGracefulCleanupTime(15000000);

/**
* SSY thread inactivity period befor unloading, microseconds
*/
const TInt KSensrvSsyInactivityPeriod(60000000);

/**
* Maximum count of conditions with same index in a set
*/
const TInt KSensrvMaxConditionIndexCount(2);

/**
* Ecom rescan timer period, also used for proxy deletion
*/
const TInt KSensrvEcomRescanTimerPeriod(1000000);


/**
* Sensor server panic codes
*/
enum TSensrvPanic
    {
    // Run error panics
    ESensrvPanicMediatorRunError           = 0,        
    ESensrvPanicThreadMonitorRunError      = 1,
    ESensrvPanicEcomChangeRunError         = 3,
    
    // State panics
    ESensrvPanicInvalidPluginState         = 10,     
    ESensrvPanicInvalidChannelState        = 11,  
    ESensrvPanicInvalidTransactionState    = 12,    
    
    // Misc panics
    ESensrvPanicUnknownTransactionType     = 20, 
    ESensrvPanicWrongProxy                 = 21,
    ESensrvPanicSsyControlExists           = 22,
    ESensrvPanicUnexpectedSsyCallback      = 23,
    ESensrvPanicOutOfBounds                = 24,
    ESensrvPanicPreviousBlockNotYetWritten = 25,
    ESensrvPanicRequestedFreeBlockTooSmall = 26,
    ESensrvPanicDuplicateListener          = 27,
    ESensrvPanicBufferTooSmall             = 28,
    ESensrvPanicInvalidRemoveType          = 29, 
    ESensrvPanicInvalidFunction            = 30, 
    ESensrvPanicUnexpectedBranch           = 31,
    ESensrvPanicInvalidDataSize            = 32,
    
    // NULL panics
    ESensrvPanicNullServerMediator         = 100, 
    ESensrvPanicNullSsyMediator            = 101,       
    ESensrvPanicNullSsyControl             = 102,
    ESensrvPanicNullChannel                = 103,           
    ESensrvPanicNullTransaction            = 104,       
    ESensrvPanicNullProxy                  = 105,
    ESensrvPanicNullMutex                  = 106,
    ESensrvPanicNullMessage                = 107,
    ESensrvPanicNullSession                = 108,
    ESensrvPanicNullProvider               = 109,
    ESensrvPanicNullListener               = 110,
    ESensrvPanicNullBuffer                 = 111,
    ESensrvPanicNullCallback               = 112,
    ESensrvPanicNullScheduler              = 113,
    ESensrvPanicNullMetData                = 114,
    ESensrvPanicNullEvaluator              = 115,
    ESensrvPanicNullImplInfos              = 116
    };

#endif // SENSRVDEFINES_H
