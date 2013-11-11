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
* Description:   Private Publish&Subscribe definitions 
*
*/



#ifndef SENSRVINTERNALPSKEYS_H
#define SENSRVINTERNALPSKEYS_H

// INCLUDES
#include <e32std.h>

// =============================================================================
// Sensor server test PubSub API
// =============================================================================
const TUid KPSUidSensrvTest = {0x10205100}; 

/**
* Error code for first stub creation
*/
const TUint32 KSensrvLeaveFirstStubConstruction =  1;

/**
* Error code for second stub creation
*/
const TUint32 KSensrvLeaveSecondStubConstruction =  2;

/**
* Stub construction delay in seconds. 
*/
const TUint32 KSensrvStubConstructionDelay = 3;


/**
* Channel count
*/
const TUint32 KSensrvFirstStubChannelCount = 4;
const TUint32 KSensrvSecondStubChannelCount = 5;

const TUint32 KSensrvLatestOpenedChannel =  6;
const TUint32 KSensrvLatestClosedChannel = 7;
const TUint32 KSensrvLatestStartListeningChannel = 8;
const TUint32 KSensrvLatestStopListeningChannel = 9;

/**
* Data buffering frequency of channel stub.
* Interval in milliseconds. 
*/
const TUint32 KSensrvSamplingInterval = 10;

const TUint32 KSensrvStubConstructionJam = 11;

const TUint32 KSensrvStartListeningJam = 12;

const TUint32 KSensrvStopListeningJam = 13;

const TUint32 KSensrvOpenChannelAsyncJam = 14;

const TUint32 KSensrvCloseChannelAsyncJam = 15;

const TUint32 KSensrvChannelOpenedError = 16;

const TUint32 KSensrvChannelClosedError = 17;

const TUint32 KSensrvStartListeningLeave = 18;

const TUint32 KSensrvStopListeningLeave = 19;

const TUint32 KSensrvDataListeningJam = 20;

const TUint32 KSensrvDataListeningBufferFilledError = 21;

const TUint32 KSensrvForceBufferFilledJam = 22;

const TUint32 KSensrvForceBufferFilledLeave = 23;

const TUint32 KSensrvOpenChannelMethodJam = 24;

const TUint32 KSensrvCloseChannelMethodJam = 25;

const TUint32 KSensrvOpenChannelError = 26;

const TUint32 KSensrvCloseChannelError = 27;

const TUint32 KSensrvChannelOpenedWithError = 28;

const TUint32 KSensrvPropertyChanged = 29;

const TUint32 KSensrvOpenChannelMethodPanic = 30;

const TUint32 KSensrvOpenChannelAsyncPanic = 31;

const TUint32 KSensrvCloseChannelMethodPanic = 32;

const TUint32 KSensrvCloseChannelAsyncPanic = 33;

const TUint32 KSensrvStartListeningMethodPanic = 34;

const TUint32 KSensrvDataListeningPanic = 35;

const TUint32 KSensrvStopListeningMethodPanic = 36;

const TUint32 KSensrvStopListeningAsyncPanic = 37;

const TUint32 KSensrvForceBufferFilledPanic = 38;

const TUint32 KSensrvSSYUnloadingCount = 39;

const TUint32 KSensrvBufferFilledNegativeCount = 40;

const TUint32 KSensrvGetAllPropertyCount = 41;

const TUint32 KSensrvGetPropertyLeave = 42;

const TUint32 KSensrvGetPropertyJam = 43;

const TUint32 KSensrvSetPropertyLeave = 44;

const TUint32 KSensrvSetPropertyJam = 45;

const TUint32 KSensrvGetAllPropertiesLeave = 46;

const TUint32 KSensrvGetAllPropertiesJam = 47;

const TUint32 KSensrvGetPropertyPanic = 48;

const TUint32 KSensrvSetPropertyPanic = 49;

const TUint32 KSensrvGetAllPropertiesPanic = 50;

const TUint32 KSensrvCheckPropertyDependenciesLeave = 51;

const TUint32 KSensrvCheckPropertyDependenciesJam = 52;

const TUint32 KSensrvCheckPropertyDependenciesPanic = 53;

const TUint32 KSensrvGetDataOverflowTest = 54;

const TUint32 KSensrvGetDataNotFoundTest = 55;

const TUint32 KSensrvTestProcessCommand = 56;

const TUint32 KSensrvSingleBufferFilledNegativeCount = 57;

const TUint32 KSensrvCreateNewChannelsFirst = 58;

const TUint32 KSensrvCreateNewChannelsSecond = 59;

const TUint32 KSensrvRemoveNewChannelsFirst = 60;

const TUint32 KSensrvRemoveNewChannelsSecond = 61;

const TUint32 KSensrvRegisterZeroChannels = 62;

const TUint32 KSensrvRegisterInvalidChannels = 63;

const TUint32 KSensrvFirstStubDoubleTappingChannelCount = 64;

const TUint32 KSensrvSetPropertyArray = 65;

const TUint32 KSensrvCheckPluginLoaded = 66;

/**
* Call Signal() when DataReceived is called.
* ETrue: call always when DataReceived is called.
* EFalse: call after user defined repeat count.
*/
const TUint32 KSensrvSignalAftedDataReseived = 100;

/**
* ETrue: wait test completion
* EFalse: do not wait test copmpletion
*/
const TUint32 KSensrvWaitTestCompletion = 101;


/**
* Notify when test case completed
*/ 
const TUint32 KSensrvTestComplete1 = 200;
const TUint32 KSensrvTestComplete2 = 201;
const TUint32 KSensrvTestComplete3 = 202;
const TUint32 KSensrvTestComplete4 = 203;
const TUint32 KSensrvTestComplete5 = 204;
const TUint32 KSensrvTestComplete6 = 205;
const TUint32 KSensrvTestComplete7 = 206;
const TUint32 KSensrvTestComplete8 = 207;
const TUint32 KSensrvTestComplete9 = 208;
const TUint32 KSensrvTestComplete10 = 209; 
const TUint32 KSensrvTestComplete11 = 210; 
const TUint32 KSensrvTestComplete12 = 211; 
const TUint32 KSensrvTestComplete13 = 212; 
const TUint32 KSensrvTestComplete14 = 213; 
const TUint32 KSensrvTestComplete15 = 214; 

#endif      // SENSRVINTERNALPSKEYS_H

// End of File

