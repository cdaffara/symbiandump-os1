/*
* Copyright (c) 2006 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM server client-server API. 
*
*/


#ifndef PSMCLIENTSERVER_H
#define PSMCLIENTSERVER_H

#include <e32base.h>

/**
 * Opcodes used in message passing between client and server
 *
 * NOTE: If TPsmServerRequest enumeration is changed, 
 * PSM Server's policy defined in psmserver.h must also be
 * checked and changed correspondingly, if necessary.
 */
enum TPsmServerRequest
    {

    /**
     * Change power save mode, Notification API
     */
    EPsmServerNotifyModeChange,

    /**
     * Cancels ongoing mode change, Notification API
     */
    EPsmServerCancelModeChange,

    /**
     * Change psm settings, Settings API
     */
    EPsmServerChangeSettings,

    /**
     * Gets psm settings for currently active PSM mode, Settings API
     */
    EPsmServerGetSettings,

    /**
     * Creates backup for psm settings, Settings API
     */
    EPsmServerBackupSettings,

    /**
     * Request notification of power save mode change
     */
    EPsmServerModeChangeNotificationRequest,

    /**
     * Cancels notification request of power save mode change
     */
    EPsmServerCancelModeChangeNotificationRequest,

    /**
     * Used only for testing purpose and in debug builds only. 
     * Not declared under _DEBUG because command ids will be 
     * different in udeb and urel version, will lead to confusion
     */
    EDebugHeapMark,
    EDebugHeapMarkEnd,
    EDebugSetHeapFailure,
    EDebugHeapReset,   

// ---------------------------------------------------------------
// End of enum
// ---------------------------------------------------------------

    /**
     * Indicates end of enumeration values. 
     * Only used in security check and is not supported
     * as a real operation.
     */
    EPsmServerReqEnd 
    };


#endif // PSMCLIENTSERVER_H 
