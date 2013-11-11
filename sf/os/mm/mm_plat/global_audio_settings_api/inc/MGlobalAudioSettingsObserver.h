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
* Description:  This file contains definitions of GlobalAudioSettings observer.
*
*/



#ifndef MGLOBALAUDIOSETTINGSOBSERVER_H
#define MGLOBALAUDIOSETTINGSOBSERVER_H

// CONSTANTS

// CLASS DECLARATION

/**
 *  An interface to Global Audio Settings observer callback functions.
 *
 *  This serves as the method of communication between the client and the
 *  CGlobalAudioSettings object.
 *
 *  The class is a mixin and is intended to be inherited by the client class
 *  that is interested in observing the global audio settings. The functions
 *  encapsulated by this class are called when events occur which causes device
 *  audio settings to be changed that affects the overall audio behavior.
 *
 *  @lib GlobalAudioSettings.lib
 *  @since 3.2
 */
class MAudioSettingsObserver
    {
public:
    enum TGASEventId
        {
          EGASWarningTones,
          EGASMessageTones,
          EGASSilentProfile,
          EGASVibra,
          EGASMessageToneName,
          EGASEmailToneName,
          EGASRingingType,
          EGASVideoCallToneName,
          EGASRingingTone1Name,
          EGASRingingTone2Name,
          EGASKeyPadVolume,
          EGASSilenceMode
        };

public:
    /**
    * Invoked by the Global Audio Settings object when audio setting has changed.
    *
    * @since 3.2
    * @param CGlobalAudioSettings& A reference to Global Audio Settings object
    *   which invoked the callback.
    * @param aSetting Specific id of the audio setting that changed.
    */
    virtual void SettingsChanged(CGlobalAudioSettings& aGlobalAudioSettings, TGASEventId aSetting) = 0;
    };

#endif      // MGLOBALAUDIOSETTINGSOBSERVER_H

// End of File
