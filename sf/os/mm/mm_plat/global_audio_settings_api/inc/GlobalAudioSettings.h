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
* Description:  This file contains definitions of GlobalAudioSettings.
*
*/



#ifndef GLOBALAUDIOSETTINGS_H
#define GLOBALAUDIOSETTINGS_H

#include <e32base.h>

class CGlobalAudioSettingsImpl;
class MAudioSettingsObserver;
class MAudioClientsListObserver;

/**
*  API for accessing global settings that affects Audio behavior.
*
*  This is API is intended to be used by Adaptation to read system attributes
*  that affect audio behavior. This API also provides notification when any
*  of these attribute values change.
*
*  @lib GlobalAudioSettings.lib
*  @since S60 3.2
*/
class CGlobalAudioSettings : public CBase
    {
    public:
        enum TAudioClientListType
            {
            EActiveAudioClients,
            EPausedAudioClients
            };
        
        enum TGASRingingType
            {
        /** The tone is played in a loop.
            */
            EGASRingingTypeRinging      = 0,
            /**
            * The tone is played in a loop. On the 1st round, the volume is gradually
            * increment from the lowest level to the set level.
            */
            EGASRingingTypeAscending    = 1,
            /** The tone is played only once.
            */
            EGASRingingTypeRingingOnce  = 2,
            /**
            * The phone only beeps once instead of playing the tone if one has been set.
            */
            EGASRingingTypeBeepOnce     = 3,
            /** The phone is silent.
            * This Ringing type is set when the phone is in silence mode.
            */
            EGASRingingTypeSilent       = 4
            };
        
        enum TGASKeypadVolume
                   {
                   EGASKeypadVolumeOff    = 0,
                   EGASKeypadVolumeLevel1 = 1,
                   EGASKeypadVolumeLevel2 = 2,
                   EGASKeypadVolumeLevel3 = 3
                   };
    public:
        /**
        * function for creating the GlobalAudioSettings
        *
        */
        IMPORT_C static CGlobalAudioSettings* NewL(MAudioSettingsObserver& aAudioSettingsObserver);

        /**
        * Destructor.
        *
        */
        IMPORT_C ~CGlobalAudioSettings();

        /**
        * Returns warning tones status set in the current profile.
        *
        * If warning tones are disabled in the current profile, Adaptation
        * should reject play requests having preferences mapped to warning tones.
        *
        * @since S60 3.2
        * @return TBool aEnable. ETrue if warning tones are enabled else EFalse.
        */
        IMPORT_C TBool IsWarningTonesEnabled();

        /**
        * Returns message tones status set in the current profile.
        *
        * If message tones are disabled in the current profile, Adaptation
        * should reject play requests having preferences mapped to message tones.
        *
        * @since S60 3.2
        * @return TBool aEnable. ETrue if message tones are enabled else EFalse.
        */
        IMPORT_C TBool IsMessageTonesEnabled();

        /**
        * Returns current silent profile status.
        *
        * @since S60 3.2
        * @return TBool aEnable. ETrue if current profile is silent profile
        * else EFalse.
        */
        IMPORT_C TBool IsSilentProfileEnabled();

        /**
        * Returns vibra status set in the current profile..
        *
        * If vibra is enabled in the current profile and vibra is not part of the
        * content, Adaptation should play vibra periodically for play requests
        * having preferences mapped to vibra settings.
        *
        * @since S60 3.2
        * @return TBool aEnable. ETrue if current profile vibra active else EFalse.
        */
        IMPORT_C TBool IsVibraEnabled();

        /**
        * Returns PublicSilence feature.
        *
        * If PublicSilence feature is enabled and if current profile is Silent
        * Profile, For all play requests having preferences mapped to Public
        * Silence setting, audio should only be played out via private accessory.
        * If no private accessory is connected, then playback should be rejected.
        *
        * @since S60 3.2
        * @return TBool aEnable. ETrue if PublicSilentMode is set else EFalse.
        */
        IMPORT_C TBool IsPublicSilenceEnabled();
        
        /**
        * Returns Message Alert Tone 
        *
        * 
        * The filename selected as the message alert tone in the
        * current profile.
        * 
        *
        * 
        * @return TDesC& aMessageAlertToneName. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
        IMPORT_C TDesC& MessageAlertTone();
        
        /**
        * Returns Email Alert Tone status
        *
        * 
        * The filename selected as the email alert tone in the
        * current profile 
        * 
        *
        * 
        *  @return TBool aEnable. ETrue if current profile message tone is on else EFalse.
        */
        IMPORT_C TBool IsMessageAlertToneEnabled();
        /**
        * Returns Email Alert Tone 
        *
        * 
        * The filename selected as the email alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TDesC& aEmailAlertToneName. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
        IMPORT_C TDesC& EmailAlertTone();
        
        /**
       * Returns Email Alert Tone status
       *
       * 
       * The filename selected as the email alert tone in the
       * current profile 
       * 
       *
       * 
       *  @return TBool aEnable. ETrue if current profile email alert tone is on else EFalse.
       */
        IMPORT_C TBool IsEmailAlertToneEnabled();
        /**
        * Returns the RingingType
        *
        * 
        * Ringing type set in the current profile
        * 
        * 
        *
        * 
        * @return TGASRingingType aRingingType. 
        */
        
        IMPORT_C  TGASRingingType RingingType();
        
        /**
        * Returns Videocall Alert Tone 
        *
        * 
        * The filename selected as the video call alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TDesC& aVideoCallAlertToneName. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
        
        IMPORT_C TDesC& VideoCallAlertTone();
        /**
        * Returns Videocall Alert Tone status
        *
        * 
        * The filename selected as the video call alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TBool aEnable. ETrue if current profile videocall alert tone is on else EFalse.
        */
        IMPORT_C TBool IsVideoCallAlertToneEnabled();
        /**
        * Returns Ringing Alert Tone 
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile 
        * 
        *
        * @return TDesC& aRingingAlertTone1Name. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        * 
        */
        
        IMPORT_C TDesC& RingingAlertTone1();
        /**
        * Returns Ringing Alert Tone1 status
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TBool aEnable. ETrue if current profile ringing alert tone is on else EFalse.
        */
        IMPORT_C TBool IsRingingAlertTone1Enabled();
                
        /**
        * Returns Ringing Alert Tone 2
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile for the alternate line
        * 
        *
        * 
        * @return TDesC& aRingingAlertTone2Name. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
        
        IMPORT_C TDesC& RingingAlertTone2();
        /**
        * Returns Ringing Alert Tone 2
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile for the alternate line
        * 
        *
        * 
        * @return TBool aEnable. ETrue if current profile ringing alert tone of alternate line is on else EFalse.
        */
        IMPORT_C TBool IsRingingAlertTone2Enabled();
                
        /**
        * Returns Ringing Keypad Volume
        *
        * 
        * The key tone volume selected  in the
        * current profile 
        * 
        *
        * 
        * @return TGASKeypadVolume aKeypadToneVolume.returns EGASKeypadVolumeOff sound is off,else selected volume level 
        * is returned
        */
        
        IMPORT_C TGASKeypadVolume KeyPadToneVolume();
        
        /**
        * Returns silent mode status.
        *
        * 
        * @return TBool aEnable. ETrue if the silent mode is enabled else EFalse.
        * Ringingtype is set to EGASRingingTypeSilent when silence mode is enabled
        */
        
        IMPORT_C TBool IsSilenceModeEnabled();
        /**
        * Registers audio clients list observer.
        *
        * When registered successfully, the observer will start receiveing
        * callback defined in MAudioClientsListObserver when audio clients list
        * changes.
        *
        * @since S60 3.2
        * @param MAudioClientsListObserver& aObserver. A reference to class
        *           implementing MAudioClientsListObserver interface.
        * @return TInt. KErrNone if successful. KErrAlreadyExists if the observer
        *           is already registered. Otherwise a systemwide error code.
        */
        IMPORT_C TInt RegisterAudioClientsListObserver(
                                    MAudioClientsListObserver& aObserver );
                
        /**
        * Unregisters audio clients list observer.
        *
        * When unregistered, the observer will no longer receive callback when
        * audio clients list changes.
        *
        * @since S60 3.2
        * @param MAudioClientsListObserver& aObserver. A reference to class
        *           implementing MAudioClientsListObserver interface.
        * @return TInt. KErrNone if successful. KErrNotFound if the observer
        *           is not registered.
        */
        IMPORT_C TInt UnregisterAudioClientsListObserver(
                                    MAudioClientsListObserver& aObserver);

        /**
        * Gets audio clients list.
        *
        * On return of this function, the parameter aList will contain active
        * audio client application process ids.
        *
        * The parameter aList will be reset in the case
        *
        * @since S60 3.2
        * @param TAudioClientListType aFilter. Type of client list.
        * @param RArray<TProcessId>& aList. A reference to RArray.
        * @return TInt. KErrNone if successful. KErrBadName if
        *           aType is not one of the TAudioClientListType enumeration.
        *           Otherwise a systemwide error code.
        *           In case of error, the array aList will be reset.
        */
        IMPORT_C TInt GetAudioClientsList(
                                TAudioClientListType aType,
                                RArray<TProcessId>& aList );

    private:
        CGlobalAudioSettings();
        /**
        * Second Phase Costructor for GlobalAudioSettings Instance
        * @since 3.2
        * @param void
        */
        void ConstructL( MAudioSettingsObserver& aAudioSettingsObserver );
    
    private:
        CGlobalAudioSettingsImpl* iBody;
    };

#endif
// End of file
