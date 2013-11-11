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
* Description:  Event enumerations and UIDs of Publish And Subscribe. PubSub
*                clients can include this file and listen to these events. 
*                These events will be routed through Publish And Subscribe.
*
*/


#ifndef __PSVARIABLES_H__
#define __PSVARIABLES_H__

// INCLUDES
#include <e32cmn.h>

// CONSTANTS

/**
 * First value of each enumeration
 */
const TInt KEnumerationFirstValue           = 0;

/**
 * GPRS availability. Indicates wheter GPRS is available or not
 *
 * Possible values:
 * - EPSGprsAvailable            GPRS network is available
 * - EPSGprsNotAvailable         GPRS netwotk is not available
 * - EPSGprsAvailabilityUnknown  GPRS network availability is not known
 *
 * Usage for requesting event notifications:
 * @code
 * void MyClass::RequestNotifications()
 *     {
 *     RProperty::Attach( KUidSystemCategory, KPSUidGprsAvailabilityValue );
 *     RProperty::Subscribe( iStatus ); // From CActive
 *     SetActive();
 *     }
 *
 * void MyClass::RunL()
 *     {
 *     TInt gprsAvailability( EPSGprsAvailabilityUninitialized );
 *     RProperty::Get( KUidSystemCategory, 
 *                     KPSUidGprsAvailabilityValue, 
 *                     gprsAvailability );
 *     //gprsAvailability contains current value of GPRS network availability
 *     }
 * @endcode
 */
const TInt KPSUidGprsAvailabilityValue      = 0x100052DA;
const TUid KPSUidGprsAvailability           = {KPSUidGprsAvailabilityValue};

enum EPSGprsAvailability
    {
    EPSGprsAvailabilityUninitialized        = KEnumerationFirstValue,
    EPSGprsAvailable,
    EPSGprsNotAvailable,
    EPSGprsAvailabilityUnknown
    };

/**
 * Current GPRS connection status. Indicates current status of GPRS network
 * 
 * Possible values:
 * - EPSGprsUnattached             Not attached to GPRS network
 * - EPSGprsAttach                 Attached to GPRS network
 * - EPSGprsContextActive          One context active on GPRS network
 * - EPSGprsSuspend                GPRS network suspended but not closed
 * - EPSGprsContextActivating      GPRS context activating but not yet active
 * - EPSGprsMultibleContextActive  Multiple active contexts on GPRS network
 *
 * Usage for requesting event notifications:
 * @code
 * void MyClass::RequestNotifications()
 *     {
 *     RProperty::Attach( KUidSystemCategory, KPSUidGprsStatusValue );
 *     RProperty::Subscribe( iStatus ); // From CActive
 *     SetActive();
 *     }
 *
 * void MyClass::RunL()
 *     {
 *     TInt gprsStatus( EPSGprsStatusUninitialized );
 *     RProperty::Get( KUidSystemCategory, 
 *                     KPSUidGprsStatusValue, 
 *                     gprsStatus );
 *     //gprsStatus contains current value of GPRS network status
 *     }
 * @endcode
 */
const TInt KPSUidGprsStatusValue            = 0x100052DB;
const TUid KPSUidGprsStatus                 = {KPSUidGprsStatusValue};
enum EPSGprsStatus
    {
    EPSGprsStatusUninitialized              = KEnumerationFirstValue,
    EPSGprsUnattached,
    EPSGprsAttach,
    EPSGprsContextActive,
    EPSGprsSuspend,
    EPSGprsContextActivating,
    EPSGprsMultibleContextActive
    };

/**
 * Current WCDMA connection status. Indicates current status of WCDMA network
 * 
 * Possible values:
 * - EPSWcdmaUnattached             Not attached to WCDMA network
 * - EPSWcdmaAttach                 Attached to WCDMA network
 * - EPSWcdmaContextActive          One context active on WCDMA network
 * - EPSWcdmaSuspend                WCDMA network suspended but not closed
 * - EPSWcdmaContextActivating      WCDMA context activating but not active
 * - EPSWcdmaMultibleContextActive  Multiple active contexts on WCDMA network
 *
 * Usage for requesting event notifications:
 * @code
 * void MyClass::RequestNotifications()
 *     {
 *     RProperty::Attach( KUidSystemCategory, KPSUidWcdmaStatusValue );
 *     RProperty::Subscribe( iStatus ); // From CActive
 *     SetActive();
 *     }
 *
 * void MyClass::RunL()
 *     {
 *     TInt wcdmaStatus( EPSWcdmaStatusUninitialized );
 *     RProperty::Get( KUidSystemCategory, 
 *                     KPSUidWcdmaStatusValue, 
 *                     wcdmaStatus );
 *     //wcdmaStatus contains current value of WCDMA network status
 *     }
 * @endcode
 */
const TInt KPSUidWcdmaStatusValue           = 0x100052FF;
const TUid KPSUidWcdmaStatus                = {KPSUidWcdmaStatusValue};
enum EPSWcdmaStatus
    {
    EPSWcdmaStatusUninitialized             = KEnumerationFirstValue,
    EPSWcdmaUnattached,
    EPSWcdmaAttach,
    EPSWcdmaContextActive,
    EPSWcdmaSuspend,
    EPSWcdmaContextActivating,
    EPSWcdmaMultipleContextActive
    };

#endif // __PSVARIABLES_H__
