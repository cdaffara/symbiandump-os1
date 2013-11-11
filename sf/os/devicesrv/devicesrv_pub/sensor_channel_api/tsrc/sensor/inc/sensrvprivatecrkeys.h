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
* Description:   Definitions for private cenrep keys.
*
*/



#ifndef SENSRVPRIVATECRKEYS_H
#define SENSRVPRIVATECRKEYS_H

/**
* Sensor Server Settings API
*
* Contains sensor server settings
*/
const TUid KCRUidSensorServerSettings = {0x1020507E};

/**
* Specifies transaction timeout in seconds for transactions that
* involve SSY. If this timeout is exceeded on any transaction,
* SSY is assumed jammed or crashed, and it is unloaded.
* 
* Value is an integer.
*
* Default value: 60
*/
const TUint32 KSensrvTransactionTimeoutKey        = 0x00000001;

/**
* Specifies SSY thread stack size in bytes.
* 
* Value is an integer.
*
* Default value: 4096
*/
const TUint32 KSensrvSsyThreadStackSizeKey        = 0x00000002;

/**
* Specifies SSY thread maximum heap size in bytes.
* Must be larger than or equal to the initial heap size of 4096.
* 
* Value is an integer.
*
* Default value: 65535
*/
const TUint32 KSensrvSsyThreadHeapMaxSizeKey      = 0x00000003;

/**
* Specifies the time in seconds SSY thread is allowed for
* graceful shutdown during SSY unloading.
* 
* Value is an integer.
*
* Default value: 15
*/
const TUint32 KSensrvThreadTerminationPeriodKey   = 0x00000004;

/**
* Specifies the maximum time in seconds a SSY is allowed to
* stay loaded when it is inactive, i.e. none of its channels
* has any clients.
* 
* Value is an integer.
*
* Default value: 60
*/
const TUint32 KSensrvSsyInactivityPeriodKey       = 0x00000005;

/**
* Specifies the multiplier to be used in determining sensor
* server channel buffer size. The maximum data rate per second
* of the channel is multiplied by this value to get the channel
* buffer size.
*
* Note that actual maximum period of buffered data at maximum
* rate is less than defined amount in seconds, as part of the
* buffer must always be reserved for writing new data.
* 
* Value is an integer.
*
* Default value: 5
*/
const TUint32 KSensrvBufferSizeMultiplierKey      = 0x00000006;

/**
* Sensor server policy API
*
* Contains clients priority settings.
*/
const TUid KCRUidSensorServerPolicy = {0x10282DEE};

#endif  // SENSRVPRIVATECRKEYS_H
            
// End of File
