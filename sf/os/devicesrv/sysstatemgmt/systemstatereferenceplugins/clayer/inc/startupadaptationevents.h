/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, Startup Adaptation API
* This header file defines the startup adaptation API events
* and their parameters.
* This file contains the following sections:
* - Event definitions
* - Event parameter definitions
*
*/



#ifndef __STARTUPADAPTATIONEVENTS_H__
#define __STARTUPADAPTATIONEVENTS_H__

#include <e32cmn.h>
/*
* @file
* @publishedPartner
* @released
*/

/*****************************************************
*       Series 60 Customer
*       Needs customer implementation
*****************************************************/

/**
* All command and event definitions of Startup Adaptation API belong to
* StartupAdaptation namespace.
*/
namespace StartupAdaptation
    {

    // DATA TYPES

    /**
    * Event definitions.
    * All events contain an event id and a set of parameters. The event ids
    * are defined first and after that parameters for each event.
    *
    * The field @param means the parameter for the event.
    */

    /**
    * Events originated from Domestic OS.
    */
    enum TEvent
        {
        /**
        * In case a licensee-specific fatal error occurs, this event can be
        * used to report the error to S60 starter server.
        *
        * Events raised in ESWStateShuttingDown or ESWStateFatalStartupError
        * state are ignored.
        *
        * @param TFatalErrorTypePckg Identifies the reason of the error.
        */
        EFatalError = 100,

        /**
        * This event can be used to indicate to S60 starter server that a reset
        * is required. After receiving this event, S60 starter server will
        * start the normal reset sequence.
        */
        EDOSOriginatedReset,

        /**
        * This event can be used to indicate to S60 starter server that a
        * shutdown is required, for instance if battery level is very low.
        * After receiving this event, S60 starter server will start the normal
        * shutdown sequence.
        */
        EDOSOriginatedShutdown,

        /**
        * In case there is a change in the SIM cards state after the security
        * check phase, the state change should be reported to S60 starter
        * server using this event. Exception is total removal of the SIM card,
        * which is reported using the fatal error event.
        *
        * @param TSimEventTypePckg Identifies the event that has occurred.
        */
        ESimEvent
        };

    /**
    * Event parameter definitions.
    * Parameters for event are collected into a package buffer.
    */

    /**
    * The list of errors that may occur any time during or after startup.
    */
    enum TFatalErrorType
        {
        /**
        * The SIM card has been physically removed from the terminal
        * (unrecoverable situation).
        */
        ESimRemoved = 100,
        /**
        * Another unrecoverable error.
        */
        EUnclassifiedError
        };

    /**
    * Buffer descriptor containing TFatalErrorType object.
    */
    typedef TPckgBuf<TFatalErrorType> TFatalErrorTypePckg;


    /**
    * The list of SIM events that may occur after critical phase.
    */
    enum TSimEventType
        {
        /**
        * The SIM card is fully usable.
        */
        ESimUsable = 100,
        /**
        * The SIM card is not fully usable, but the emergency number can be
        * read.
        */
        ESimReadable,
        /**
        * The SIM card is disconnected, but still physically attached in the
        * terminal (recoverable situation).
        */
        ESimNotReady
        };

    /**
    * Buffer descriptor containing TSimEventType object.
    */
    typedef TPckgBuf<TSimEventType> TSimEventTypePckg;

    }

#endif // __STARTUPADAPTATIONEVENTS_H__
