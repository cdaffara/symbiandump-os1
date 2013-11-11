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
* Description:  This file contains definitions of Audio Clients List
*                Publish and Subscribe keys.
*
*/



#ifndef AUDIOCLIENTSLISTPSKEYS_H
#define AUDIOCLIENTSLISTPSKEYS_H

// INCLUDES
#include <e32base.h>

/***********
* Category
***********/

#include <mmfbase.hrh>
//#define KUidMmfAudioServerDllUnicodeDefine 0x101F457f
const TUid KPSUidMMFAudioServer = {KUidMmfAudioServerDllUnicodeDefine};

/***********
* Keys
***********/

/**
* Internal. DO NOT USE THIS.
*/
const TUint32 KAudioServerDisplayVolume=0x00000001;

/**
* KAudioPolicyApplicationAudioStatePlaying
* Internal use only.
*/
const TUint32 KAudioPolicyApplicationAudioStatePlaying = 0x00000002;

/**
* KAudioPolicyApplicationAudioStateRecording
* Internal use only.
*/
const TUint32 KAudioPolicyApplicationAudioStateRecording = 0x00000003;

/**
* KAudioPolicyApplicationAudioStatePaused
* Internal use only.
*/
const TUint32 KAudioPolicyApplicationAudioStatePaused = 0x00000004;

/**
* KAudioPolicyAudioClients
* Contains a list of PIDs of all applications that are currently doing audio
* playback or record.
*/
const TUint32 KAudioPolicyAudioClients = 0x00000005;

/**
* KAudioPolicyForceMutedClients
* Internal use only.
*/
const TUint32 KAudioPolicyForceMutedClients = 0x00000006;

#endif      // AUDIOCLIENTSLISTPSKEYS_H

// End of file
