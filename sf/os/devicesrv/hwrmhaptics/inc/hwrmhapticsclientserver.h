/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains haptics common panic codes and commands
*
*/


#ifndef HWRMHAPTICSCLIENTSERVER_H
#define HWRMHAPTICSCLIENTSERVER_H

#include <e32base.h>

/**
 * Constant used in message response handling, when 
 * a response is received from an adaptation plugin.
 */
const TInt KHapticsMessageResponseArgsOffset = 1;

/**
 * IPC-commands used in message passing between client 
 * and server.
 */
enum THWRMHapticsRequest
    {
    EHWRMHaptics = 2000,
    EHWRMHapticsService,
    EHWRMHapticsPlayEffect,
    EHWRMHapticsSuppActuators,
    EHWRMHapticsOpenActuator,
    EHWRMHapticsSetLicenseProp,
    EHWRMHapticsReserve,
    EHWRMHapticsRelease,
    EHWRMHapticsCleanup,
    EHWRMHapticsGetStatus,
    EHWRMHapticsStatusNotification,
    EHWRMHapticsBridgeCommand
    };

/**
 * Specifies the different types of status to be notified to
 * client, when it has requested notification from server.
 * This type enumeration is needed, since the observer in 
 * the client side is implemented as a single active object.
 * With the type value, the observer can recognize which status
 * was received. The observation is implemented with a single
 * active object to minimize the IPC-commands send to the
 * server.
 */
enum THWRMHapticsStatusTypes
    {
    /**
     * Haptics status. This value is used by haptics server,
     * when it notifies a client from haptics status changes. 
     * Haptics client uses this value, when it requests status
     * information, and only haptics status notification is desired.
     *
     * @see MHWRMHapticsObserver::THWRMHapticsStatus
     */
    EHWRMHapticsSessionStatus,

    /**
     * Actuator status. This value is used by haptics server,
     * when it notifies a client from actuator status changes.
     * Haptics client uses this value, when it requests status
     * information, and only actuator status notification is
     * desired.
     *
     * @see MHWRMHapticsActuatorObserver::THWRMActuatorEvents
     */
    EHWRMHapticsActuatorStatus,

    /**
     * Both status types. This value is used only by haptics
     * client, when it requests status information, and both 
     * status notification types are desired.
     */
    EHWRMHapticsBothStatus
    };

/**
 * Defines the haptics server panic codes.
 */
enum THWRMHapticsPanic
    {
    /**
     * Illegal function called via client-server interface.
     */
    EPanicIllegalFunction = 0,
    
    /**
     * Bad handle in client-server interface.
     */
    EPanicBadHandle,

    /**
     * Actuator has not yet been opened successfully.
     */
    EPanicNotReady
    };

#endif  // HWRMHAPTICSCLIENTSERVER_H
            
// End of File
