/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef __MMF_TMMFAUDIO_DATA_SETTINGS_H__
#define __MMF_TMMFAUDIO_DATA_SETTINGS_H__
#include <e32std.h>

/*
* Generic Audio Parameters
*
* class TMMFAudioDataSettings
*
*/
class TMMFAudioDataSettings
	{
public:
	enum TAudioCaps
		{
		// Sampled audio capabilities
		ESampleRateFixed =		0x00000001,
		ESampleRateAnyInRange = 0x00000002,
		ESampleRate8000Hz =		0x00000010,
		ESampleRate11025Hz =	0x00000040,
		ESampleRate12000Hz =	0x00000080,
		ESampleRate16000Hz =	0x00000100,
		ESampleRate22050Hz =	0x00000400,
		ESampleRate24000Hz =	0x00000800,
		ESampleRate32000Hz =	0x00001000,
		ESampleRate44100Hz =	0x00004000,
		ESampleRate48000Hz =	0x00010000,
		ESampleRate96000Hz =	0x00020000,
		ESampleRate64000Hz =	0x00040000,
		// Channel related - applies to sampled
		EChannelsMono =			0x02000000,
		EChannelsStereo =		0x04000000,
		// Routing related
		ERoutingControl =		0x08000000,
		// True if data flow is synchronised with real time (e.g. stream)
		ERealTime =				0x10000000
		};
	enum TAudioFlags
		{
		ENoLocalRouting =		0x00000001,
		ENoNetworkRouting =		0x00000002
		};
public:
	inline TMMFAudioDataSettings();
	inline void Query(); // Call this when you just want to have caps and maxvolume filled
public:
	// Fixed values - must be queried
	TInt iCaps;
	TInt iMaxVolume; // One means fixed volume (zero not valid)
	// Variables
	TInt iSampleRate;
	TInt iChannels;
	TInt iVolume;
	TInt iFlags;
	};

#endif //__MMF_TMMFAUDIO_DATA_SETTINGS_H__
