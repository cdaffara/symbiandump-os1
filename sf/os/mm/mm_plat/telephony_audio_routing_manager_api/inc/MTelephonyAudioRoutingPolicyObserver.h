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
* Description:  M-observer class that the policy client must implement to receive
*                requests from the client.
*
*/


#ifndef MTELEPHONYAUDIOROUTINGPOLICYOBSERVER_H
#define MTELEPHONYAUDIOROUTINGPOLICYOBSERVER_H

// INCLUDES
#include "TelephonyAudioRouting.h"

class CTelephonyAudioRoutingManager;

// CLASS DECLARATION

/**
*  Defines functions that policy client must implement in order to receive
*  output change request from Telephony applications.
*
*  @lib TelephonyAudioRoutingManager.lib
*  @since Series 60 3.1
*/
class MTelephonyAudioRoutingPolicyObserver
    {
    public:

        /**
        * Request to change current output. When the request is serviced by the
        * Adaptation, this will result in
        * CTelephonyAudioRoutingManager::OutputChangeRequestCompleted call by
        * the adaptation.
        *
        * For each request made by the client, there will be one function call,
        * CTelephonyAudioRoutingManager::OutputChangeRequestCompleted from Adaptation.
        *
        * If there are two requests back to back, Adaptation will service first
        * request, call CTelephonyAudioRoutingManager::OutputChangeRequestCompleted
        * and then consider the second request. This will be true for multiple
        * back to back requests. (Back to back = pending requests in request queue).
        *
        * @since Series 60 3.1
        * @param CTelephonyAudioRoutingManager& aTelephonyAudioRoutingManager.
        * @param CTelephonyAudioRouting::TAudioOutput aOutput requested by client.
        *           ENotActive if last instance of Telephony Audio Routing API
        *           client is deleted.
        * @return void.
        */
        virtual void OutputChangeRequested( CTelephonyAudioRoutingManager& aTelephonyAudioRoutingManager, CTelephonyAudioRouting::TAudioOutput aOutput) = 0;
    };

#endif      // MTELEPHONYAUDIOROUTINGPOLICYOBSERVER_H

// End of File