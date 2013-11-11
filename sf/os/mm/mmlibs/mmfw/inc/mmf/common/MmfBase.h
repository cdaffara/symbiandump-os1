// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __MMFBASE_H__
#define __MMFBASE_H__

// Standard EPOC32 includes
#include <e32base.h>


/** 
@publishedAll
@released

The priority for clients accessing a sound output device for audio playback or audio streaming.

This is a value between EMdaPriorityMin and EMdaPriorityMax. The higher value indicates a more 
important request. It is used to resolve conflicts when more than one client tries to access the
same hardware resource simultaneously.

One of these values is passed in the first parameter to CMdaAudioOutputStream::SetPriority().

@see CMdaAudioOutputStream::SetPriority()
*/
enum TMdaPriority
	{
  	/**
	The lowest priority (= – 100). This indicates that the client can be interrupted 
	by any other client.
	*/
    EMdaPriorityMin = -100,
	/**
	Normal priority. This indicates that the client can be interrupted but only by higher priority clients.
	*/
    EMdaPriorityNormal = 0,
	/**
    The highest priority (= 100). This indicates that the client cannot be interrupted by other clients.
	*/
    EMdaPriorityMax = 100
	};

/**
@publishedAll
@released

Audio priority preference values. These are to be interpreted as discrete values and not separate flags.
*/
enum TMdaPriorityPreference
	{
	/**
	Default value - no specific meaning.
	*/
    EMdaPriorityPreferenceNone =	0,
	/**
	The audio data is time sensitive. The playback operation may fail if it cannot happen when 
	requested but degraded output such as mixing or muting is allowed. Note that this is the
	default behaviour anyway, but this declaration is maintained for compatability.	*/
    EMdaPriorityPreferenceTime =	1,
	/**
    The audio data should be played at the best possible quality (for example, it should not be degraded by 
	muting or mixing). This is an advisory to the adaptation and can be ignored.
	*/
    EMdaPriorityPreferenceQuality = 2,
	/**
    The audio data is both time and quality sensitive. Identical in behaviour to EMdaPriorityPreferenceQuality.
	*/
    EMdaPriorityPreferenceTimeAndQuality = 3
	};

/**
@publishedAll
@released

Holds the current state of DevSound.
*/
enum TMMFState
{
	/** Idle state.
	*/
	EMMFStateIdle,
	/** The MMF is currently playing.
	*/
	EMMFStatePlaying,
	/** The MMF is playing a tone.
	*/
	EMMFStateTonePlaying,
	/** The MMF is currently recording.
	*/
	EMMFStateRecording,
	/** The MMF is playing and recording.
	*/
	EMMFStatePlayingRecording,
	/** The MMF is converting data.
	*/
	EMMFStateConverting
};

/**
@publishedAll
@released

A class type representing the audio client's priority,
priority preference and state settings.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 

*/
class TMMFPrioritySettings
	{
public:
	TMMFPrioritySettings();
	/**
    The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
    EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
	*/
	TInt iPriority;

	/**
    The Priority Preference - an additional audio policy parameter. The suggested default is 
    EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
    values may be supported by given phones and/or platforms, but should not be depended upon by 
    portable code.
	*/
	TInt iPref;

	/**
    The state of the MMF player such as idle, playing, recording and so on. See the TMMFState enum for possible states.
	*/
	TMMFState iState;
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	};

/**
@publishedAll
@released

This is a TPckgBuf package of a TMMFPrioritySettings.
*/
typedef TPckgBuf<TMMFPrioritySettings> TMMFPrioritySettingsPckg;

/**
Initializes the object with arbitrary values.
*/
inline TMMFPrioritySettings::TMMFPrioritySettings() : iPriority(EMdaPriorityNormal), iPref(EMdaPriorityPreferenceNone), iState(EMMFStateIdle), iReserved1(0)
	{
	}

#endif
