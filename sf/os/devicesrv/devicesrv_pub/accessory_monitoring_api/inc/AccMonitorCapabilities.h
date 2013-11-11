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
* Description:  Accessory capability definitions for AccMonitor
*
*/



#ifndef ACCMONITORCAPABILTIES_H
#define ACCMONITORCAPABILTIES_H

// INCLUDES
#include <e32base.h>

// EXTERNAL DATA TYPES
/**
* Type definition TAccMonCapability defines an unsigned integer as an
* accessory capability. Accessory capabilities for Accessory Monitoring API
* are listed below.
*/
typedef TUint32 TAccMonCapability;

/**
* Device Types
*
* Device Type is a general definition for a type of an accessory.
*/

/**
* Device type capability: KAccMonNoDevice
*
* Description: Constant definition for no device.
*/
const TAccMonCapability KAccMonNoDevice = 0;

/**
* Device type capability: KAccMonHeadset
*
* Description: Constant definition for device type headset.
*/
const TAccMonCapability KAccMonHeadset = 1;

/**
* Device type capability: KAccMonCarKit
*
* Description: Constant definition for device type car kit.
*/
const TAccMonCapability KAccMonCarKit = 2;

/**
* Device type capability: KAccMonOffice
*
* Description: Constant definition for device type office, e.g. Music Stand.
*/
const TAccMonCapability KAccMonOffice = 3;

/**
* Device type capability: KAccMonLoopset
*
* Description: Constant definition for device type Loopset.
*/
const TAccMonCapability KAccMonLoopset = 6;

/**
* Device type capability: KAccMonTTY
*
* Description: Constant definition for device type TTY.
*/
const TAccMonCapability KAccMonTTY = 9;

/**
* Device type capability: KAccMonAVDevice
*
* Description: Constant definition for device type audio/video device. For
* example a video cable is an A/V device.
*/
const TAccMonCapability KAccMonAVDevice = 10;

/**
* Physical Connections
*
* Physical connections are used to describe how the accessory is attached to
* S60 device.
*/

/**
* Physical connection capability: KAccMonWired
*
* Description: Constant definition for physical connection Wired connector.
* If capability is defined accessory has a wired a connector as a physical
* connection.
*/
const TAccMonCapability KAccMonWired = 17;

/**
* Physical connection capability: KAccMonBluetooth
*
* Description: Constant definition for physical connection Bluetooth
* connector. If capability is defined accessory has bluetooth for its
* physical connection.
*/
const TAccMonCapability KAccMonBluetooth = 18;

/**
* Physical connection capability: KAccMonUSB
*
* Description: Constant definition for physical connection USB
* connector. If capability is defined accessory has USB for its
* physical connection.
*/
const TAccMonCapability KAccMonUSB = 19;

/**
* Physical connection capability: KAccMonHDMI
*
* Description: Constant definition for physical connection HDMI
* connector. If capability is defined accessory has HDMI for its
* physical connection.
*/
const TAccMonCapability KAccMonHDMI = 20;

/**
* Audio Capabilities
*
* Description: Audio capabilities describe the audio features some accessory
* has.
*/

/**
* Audio capability: KAccMonStereoAudio
*
* Description: Constant definition for stereo audio. If capability is defined
* this accessory supports stereo audio output.
*/
const TAccMonCapability KAccMonStereoAudio = 81;

/**
* Audio capability: KAccMonMonoAudio
*
* Description: Constant definition for mono audio. If capability is defined
* this accessory supports mono audio output.
*/
const TAccMonCapability KAccMonMonoAudio = 82;

/**
* Audio capability: KAccMonSpeakerAudioOutput
*
* Description: Constant for accessory speaker feature. If capability is
* defined the accessory has a speaker integrated to this accessory.
*/
const TAccMonCapability KAccMonSpeakerAudioOutput = 83;

/**
* Audio capability: KAccMonMicAudioInput
*
* Description: Constant for accessory mic feature. If capability is defined
* the accessory has a mic integrated to this accessory.
*/
const TAccMonCapability KAccMonMicAudioInput = 84;

/**
* Audio capability: KAccMonAudioOut
*
* Description: Constant definition for audio out. Audio out capability is
* defined if the accessory is capable for outputting audio to some other
* device. Values are defined with enumerations. If value is defined to 0,
* audio out is defined as standard lineout.
*/
const TAccMonCapability KAccMonAudioOut = 86;

/**
* Enumerator for capability TAccMonAudioOut.
* @see TAccMonAudioOut
*/
enum TAccMonAudioOut
    {
    EAccMonAudioOutLineout = 0
    };

/**
* Bluetooth features
*
* Bluetooth features describe the bluetooth features some accessory has.
*/

/**
* Bluetooth capability: KAccMonBTHSP
* Description: Constant definition for Bluetooth profile HS (Headset
* profile). Accessory supports HS profile.
*/
const TAccMonCapability KAccMonBTHSP = 97;

/**
* Bluetooth capability: KAccMonBTHFP
*
* Description: Constant definition for Bluetooth profile HF (Hands Free
* profile). Accessory supports HF profile.
*/
const TAccMonCapability KAccMonBTHFP = 98;

/**
* Bluetooth capability: KAccMonBTAVRCP
*
* Description: Constant definition for Bluetooth profile AVRC (Audio/Video
* Remote Control profile). Accessory supports AVRC profile.
*/
const TAccMonCapability KAccMonBTAVRCP = 99;

/**
* Bluetooth capability: KAccMonBTA2DP
*
* Description: Constant definition for Bluetooth profile A2D (Advanced Audio
* Distribution profile). Accessory supports A2D profile.
*/
const TAccMonCapability KAccMonBTA2DP = 100;

/**
* Video Features
* Video Features describe the video features some accessory has.
*/

/**
* Video capability: KAccMonVideoOut
*
* Description: Constant for video capability video out. If capability is
* defined the accessory has a video out possibility. Values are defined with
* enumerations. If value is defined to 0 video format is C-Video.
*/
const TAccMonCapability KAccMonVideoOut = 113;

/**
* Enumerator for capability TAccMonVideoOut.
* @see TAccMonVideoOut
*/
enum TAccMonVideoOut
    {
    EAccMonVideoOutCVideo = 0//C-Video (composite)
    };


#endif // ACCMONITORCAPABILTIES_H

// End of File
