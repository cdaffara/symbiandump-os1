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
* Description:  Global Audio Settings Publish & Subscribe keys (Domain API)
*                specifying the settings of global audio settings.
*
*/


#ifndef GLOBALAUDIOSETTINGSPSKEYS_H
#define GLOBALAUDIOSETTINGSPSKEYS_H

// INCLUDES
#include <e32def.h>

// CONSTANTS

/**
* Global Audio Settings Publish & Subscribe UID.
*/
const TUid KGASPSUidGlobalAudioSettings = {0x10207B7C};

/**
* This key is required for the Global Audio Settings to determine whether the
* warning tones in the current profiles are enabled or not.
*
* Possible integer values:
* 0 = "Warning Tones Disabled". Warning tones in the current profile are disabled.
* 1 = "Warning Tones Enabled". Warning tones in the current profile are enabled.
*/
const TUint32 KGASWarningTones = 0x00000001;

/**
* This key is required for the Global Audio Settings to determine whether the
* message tones in the current profiles are enabled or not.
*
* Possible integer values:
* 0 = "Message Tones Disabled". Message tones in the current profile are disabled.
* 1 = "Message Tones Enabled". Message tones in the current profile are enabled.
*/
const TUint32 KGASMessageTones = 0x00000002;

/**
* This key is required for the Global Audio Settings to determine whether the
* current profiles selected in the phone is silent profile or not.
*
* Possible integer values:
* 0 = "Not Silent Profile". Current profile is not silent profile.
* 1 = "Silent Profile". Current profile is silent profile.
*/
const TUint32 KGASSilentProfile = 0x00000003;

/**
* This key is required for the Global Audio Settings to determine whether the
* Vibra in the current profiles are enabled or not.
*
* Possible integer values:
* 0 = "Vibra Disabled". Vibra in the current profile are disabled.
* 1 = "Vibra Enabled". Vibra in the current profile are enabled.
*/
const TUint32 KGASVibra = 0x00000004;

/**
* This key is required for the Global Audio Settings to determine whether the
* PublicSilence selected in the phone or not.
*
* Possible integer values:
* 0 = "Not PublicSilence". PublicSilence not selected in the phone.
* 1 = "PublicSilence". PublicSilence selected in the phone
*/
const TUint32 KGASPublicSilence = 0x00000005;

/**
* This key is required for the Global Audio Settings to determine the ringing type
* selected
*
* Possible text values:
* z:\\resource\\No_Sound.wav = "Message tone is disabled". 
* filename of the tone selected= "Message tone is enabled". 
*/
const TUint32 KGASRingingType=0x00000006;

/**
* This key is required for the Global Audio Settings to determine the message alert tone
* selected
*
* Possible text values:
* z:\\resource\\No_Sound.wav = "Message tone is disabled". 
* filename of the tone selected= "Message tone is enabled". 
*/
const TUint32 KGASMessageToneName=0x00000007;

/**
* This key is required for the Global Audio Settings to determine the email alert tone
* selected
*
* Possible text values:
* z:\\resource\\No_Sound.wav = "Email alert tone is disabled". 
* filename of the tone selected= "Email alert tone is enabled". 
*/
const TUint32 KGASEmailToneName=0x00000008;

/**
* This key is required for the Global Audio Settings to determine the video call tone
* selected
*
* Possible text values:
* z:\\resource\\No_Sound.wav = "Video call tone is disabled". 
* filename of the tone selected= "Video call tone is enabled". 
*/
const TUint32 KGASVideoCallToneName=0x00000009;
/**
* This key is required for the Global Audio Settings to determine the ringing tone
* selected
*
* Possible text values:
* z:\\resource\\No_Sound.wav = "Ringing tone1 is disabled". 
* filename of the tone selected= "Ringing tone1 is enabled". 
*/
const TUint32 KGASRingingTone1Name=0x0000000a;

/**
* This key is required for the Global Audio Settings to determine the ringing tone
* for alternate line selected
*
* Possible text values:
* z:\\resource\\No_Sound.wav = "Ringing tone2 is disabled". 
* filename of the tone selected= "Ringing tone2 tone is enabled". 
*/
const TUint32 KGASRingingTone2Name=0x0000000b;

/**
* This key is required for the Global Audio Settings to determine the keypad volume
* selected
*
* Possible text values:
* EGASKeypadVolumeOff="No volume"
* EGASKeypadVolumeLevel1 = "Volume level 1"
* EGASKeypadVolumeLevel2 = "volume level 2"
*  EGASKeypadVolumeLevel3 = "volume level 3"
*/
const TUint32 KGASKeypadToneVolume=0x0000000c;
/**
* This key is required for the Global Audio Settings to determine whether the
* silent mode is active or not
*
* Possible integer values:
* 0 = "Silent mode Disabled". Silent mode is disabled
* 1 = "Warning Tones Enabled". Silent mode is enabled
*/
const TUint32 KGASSilenceMode = 0x0000000d;
#endif      // GLOBALAUDIOSETTINGSPSKEYS_H

// End of file
