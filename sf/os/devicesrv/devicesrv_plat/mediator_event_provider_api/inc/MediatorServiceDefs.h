/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common constant definitions for Mediator functionality
*
*/


#ifndef MEDIATORSERVICEDEFS_H
#define MEDIATORSERVICEDEFS_H

#include <e32base.h>

namespace MediatorService
    {
        // The structure used when listing registered events.
    class TEvent
        {
    public:
        // The identifier of the event.
        TInt iEventId;
        // The capabilities required for consuming the event.
        TCapabilitySet iCaps;
        // The version information of the event.
        TVersion iVersion;
        };

    // The structure used when listing registered commands.
    class TCommand
        {
    public:
        // The identifier of the command.
        TInt iCommandId;
        // The capabilities required for consuming the command.
        TCapabilitySet iCaps;
        // The version information of the command.
        TVersion iVersion;
        // The timeout value for the command, how long a response is waited.
        TInt iTimeout;
        };

    // Typedef's for different lists
    typedef RArray<TUid> RDomainList;
    typedef RArray<TUid> RCategoryList;
    typedef RArray<TEvent> REventList;
    typedef RArray<TCommand> RCommandList;
    }

// CONSTANTS

// The default timeout value in milliseconds for waiting response to a command.
const TInt KMediatorResponseTimeout = 1000;
// Timeout value for commands that are not to be completed by timeout mechanism
const TInt KMediatorTimeoutInfinite = -1;

// ERROR CODES

/* Base error value for Mediator error codes */
const TInt KMediatorErrBase = -32545; // The last legal error code is KMediatorErrBase - 24.

/* Domain UID cannot be found */
const TInt KErrMediatorDomainNotFound        = KMediatorErrBase - 1;
					
/* Category UID cannot be found */
const TInt KErrMediatorCategoryNotFound      = KMediatorErrBase - 2;

/* Command ID cannot be found */    
const TInt KErrMediatorCommandNotFound       = KMediatorErrBase - 3;

/* Event ID cannot be found */
const TInt KErrMediatorEventNotFound         = KMediatorErrBase - 4;

/* Command is already registered */
const TInt KErrMediatorCommandAlreadyExists  = KMediatorErrBase - 5;

/* Event is already registered */
const TInt KErrMediatorEventAlreadyExists    = KMediatorErrBase - 6;

/* Subscription already exists */
const TInt KErrMediatorAlreadySubscribed     = KMediatorErrBase - 7;

/* Subscription cannot be found */
const TInt KErrMediatorNoSubscription        = KMediatorErrBase - 8;

/* Event is not registered by this Secure Id */
const TInt KErrMediatorSecureIdMismatch      = KMediatorErrBase - 9;

/* Version (major) mismatch */
const TInt KErrMediatorVersionMismatch       = KMediatorErrBase - 10;

/* Mediator command timeout */
const TInt KErrMediatorTimeout               = KMediatorErrBase - 11;

/* Issued command has been removed before completion */ 
const TInt KErrMediatorCommandRemoved        = KMediatorErrBase - 12;          

#endif	// MEDIATORSERVICEDEFS_H

// End of File
