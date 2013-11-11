/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides P&S Key Defintions for Autoaudio ASY
*
*/

#ifndef _AUTOAUDIO_PS_KEYS_
#define _AUTOAUDIO_PS_KEYS_

#include <e32std.h>

/// header for defining P&S keys for automotive audio use cases
/// accessory server is responsible for defining below P&S keys during start-up
/// Automotive Server is responsible for publishing key values

/// key for monitoring the connection status of RTP Streaming
/// client : AutoAudio ASY, publisher : automotive server
/// value : TPSAutoAudioConnectionStatus
const TUint KPSAutoKeyRTPStreamingConnectionStatus = 0x2; 

enum TPSAutoAudioConnectionStatus 
{
    EAudioConnectionStatusNone,	         // Notifies Accessory Disconnection
    EAudioConnectionStatusUnidirectional, // application sound only
    EAudioConnectionStatusBidirectional   // application sound + phone call
};


/// key for monitoring the connection status of USB Audio
/// client : accessory FW, publisher : USB audio class controller
/// value : TPSAutoAudioConnectionStatus
// const TUint KPSAutoKeyUSBAudioConnectionStatus = 0x3; 

/// key for monitoring the successful launch of USB Audio.
/// client : USB audio class controller, publisher : USB Audio streaming implementation
/// value : TPSAutoAudioRunningStatus
// const TUint KPSAutoKeyUSBAudioRunningStatus = 0x4;

#endif // _AUTOAUDIO_PS_KEYS_
