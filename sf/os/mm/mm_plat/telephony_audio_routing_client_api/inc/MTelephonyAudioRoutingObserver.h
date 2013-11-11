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
* Description:  M-observer class that the client must implement to receive
*                completion notification for asynchronous requests and event notifications.
*
*/


#ifndef MTELEPHONYAUDIOROUTINGOBSERVER_H
#define MTELEPHONYAUDIOROUTINGOBSERVER_H

// INCLUDES
#include "TelephonyAudioRouting.h"

// CLASS DECLARATION
class CTelephonyAudioRouting;

/**
*  Defines functions that client must implement in order to receive
*  request completion and event notifications.
*
*  @lib TelephonyAudioRouting.lib
*  @since Series 60 3.1
*/
class MTelephonyAudioRoutingObserver
    {
    public:

        /**
        * Signals that available audio outputs have changed. The applications
        * has to call CTelephonyAudioRouting::AvailableOutputs to get the
        * updated list.
        *
        * @since Series 60 3.1
        * @param CTelephonyAudioRouting& aTelephonyAudioRouting reference.
        * @return void
        */
        virtual void AvailableOutputsChanged( CTelephonyAudioRouting& aTelephonyAudioRouting) = 0;

        /**
        * Signals that current output has changed. The applications has to call
        * CTelephonyAudioRouting::Output to get the current active output.
        *
        * The application will get this callback only when the change in output is
        * due some other application's CTelephonyAudioRouting::SetOutputL call OR
        * because of user has deleted audio link through long key press on the
        * BT device OR when adaptation routes audio from device to BT accessory
        * automatically when user initiated long key press event occurs.
        *
        * @since Series 60 3.1
        * @param CTelephonyAudioRouting& aTelephonyAudioRouting reference.
        * @return void
        */
        virtual void OutputChanged( CTelephonyAudioRouting& aTelephonyAudioRouting) = 0;

        /**
        * Signals that CTelephonyAudioRouting::SetOutputL request issued by this
        * application (on CTelephonyAudioRouting instance) was serviced by the
        * Adaptation.
        * Only when the observer receives this callback, the application should consider
        * CTelephonyAudioRouting::SetOutputL message to be completed.
        *
        * @since Series 60 3.1
        * @param CTelephonyAudioRouting& aTelephonyAudioRouting reference.
        * @param aError KErrNone if output requested by application is set, else
        *        system-wide error code.
        * @return void
        */
        virtual void SetOutputComplete( CTelephonyAudioRouting& aTelephonyAudioRouting, TInt aError) = 0;

    };

#endif      // MTELEPHONYAUDIOROUTINGOBSERVER_H

// End of File

