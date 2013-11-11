/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains definitions of Audio Clients observer.
*
*/



#ifndef MAUDIOCLIENTSLISTOBSERVER_H
#define MAUDIOCLIENTSLISTOBSERVER_H

// CONSTANTS

// CLASS DECLARATION

/**
 *  An interface to Audio Clients observer callback functions.
 *
 *  This serves as the method of communication between the client and the
 *  CGlobalAudioSettings object.
 *
 *  The class is a mixin and is intended to be inherited by the client class
 *  that is interested in observing the audio clients list. The functions
 *  encapsulated by this class are called when events occur which causes audio
 *  clients list to change.
 *
 *  @lib GlobalAudioSettings.lib
 *  @since 3.2
 */
class MAudioClientsListObserver
    {
    public:
        static const TUint KActiveAudioClientsListChanged = 1;
        static const TUint KPausedAudioClientsListChanged = 2;
        /**
        * Invoked by the Global Audio Settings object when audio clients list
        * changes.
        *
        * @since 3.2
        * @param CGlobalAudioSettings& A reference to Global Audio Settings object
        *   which invoked the callback.
        * @param aEvent Specific id of the audio clients list that changed.
        */
        virtual void Event( const CGlobalAudioSettings& aGlobalAudioSettings, TUint aEvent ) = 0;
    };

#endif      // MAUDIOCLIENTSLISTOBSERVER_H

// End of File
