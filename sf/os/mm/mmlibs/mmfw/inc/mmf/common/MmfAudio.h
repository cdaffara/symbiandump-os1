// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\MmfAudio.h
// 
//

#ifndef __MMF_COMMON_AUDIO_H__
#define __MMF_COMMON_AUDIO_H__

// Standard EPOC32 includes
#include <e32base.h>

// Public Media Server includes

#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>

/**
@publishedAll
@released

Audio Loading started event. Use by a controller to signal to a
client that audio loading has begun.

@since 7.0s
*/
const TUid KMMFEventCategoryAudioLoadingStarted = {0x101F8002};

/**
@publishedAll
@released

Audio Loading completed event. Use by a controller to signal to a
client that audio loading has completed.

@since 7.0s
*/
const TUid KMMFEventCategoryAudioLoadingComplete = {0x101F8003};

/**
@publishedAll
@released

The class defining a callback interface for receiving loading notifications.

since 7.0s
*/
class MAudioLoadingObserver
	{
public:

	/*
	Audio Loading/rebuffering has begun

	@since 7.0s
	*/
	virtual void MaloLoadingStarted() = 0;
	/*
	Audio Loading/rebuffering has completed

	@since 7.0s
	 */
	virtual void MaloLoadingComplete() = 0;
	};


/**
@publishedAll
@released

An interface to a set of audio player callback functions.

The class is a mixin and is intended to be inherited by the client
class which is waiting for the audio resource. The functions
encapsulated by this class are called when specific events occur in
the process of resource availablity. A reference to this object is
passed as a parameter when registering for notification by 
the audio player.

*/
class MMMFAudioResourceNotificationCallback
	{
public:

	/**
	Called when the audio resource becomes available but
	only if the resource was previously unavailable (e.g. if
	the audio resource is available when the client registers,
	then this callback is not received).

	@param  aNotificationEventId
	        The UID of the event that is being handled.
	@param  aNotificationData
	        The data that corresponds to the notification event.
	*/
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData) = 0;
	
	};

/**
@publishedAll
@released

A class used when sending custom commands from the client API
to the audio controller to get or set the audio configuration
*/
class TMMFAudioConfig
	{
public:
	inline TMMFAudioConfig();

	/**
    Not implemented.
	*/
	inline void SetReturnSize(TInt aReturnSize);
public:
	// Audio device command parameters

	/**
	 The volume setting. This can be any value from zero to iMaxVolume
	*/
	TInt iVolume;
	/**
	The maximum volume setting of the audio device.

	This value is platform dependent but is always greater than or equal to one. This is the 
	maximum value that should be passed to iVolume.
	*/
	TInt iMaxVolume;

	/**
	The balance between left and right stereo channels. Must be between -100 and 100
	*/
	TInt iBalance;

	/**
	The gain of the audio device. This can be any value from zero to iMaxGain
	*/
	TInt iGain;

	/**
	The maximum gain of the audio device.
	*/
	TInt iMaxGain;

	/**
	If true, the audio clip is cropped from the current position to the end and the remainder of the clip 
	is discarded. If false, the audio clip is cropped from the start of the file to the current position. 
	The audio data prior to the current position is discarded.
	*/
	TBool iCropToEnd ;

	/**
	The number of channels of the audio device (signifying mono/stereo).
	*/
	TUint iChannels;

	/**
	The sample rate of the audio device.
	*/
	TUint iSampleRate;

	/**
	The format UID to be used.
	*/
	TUid iFormatUid;

	/**
	The source data codec type.
	*/
	TFourCC iSourceDataTypeCode;

	/**
	The sink data codec type.
	*/
	TFourCC iSinkDataTypeCode;

	/**
	The start position for audio playback in micro seconds.
	*/
	TTimeIntervalMicroSeconds iStartPosition ;

	/**
	The end position for audio playback in micro seconds.
	*/
	TTimeIntervalMicroSeconds iEndPosition ;

	/**
	The period over which the volume is to rise. 

	A zero value causes the tone sample to be played at the normal level for the full
	duration of the playback. A value which is longer than the duration
	of the audio sample means that the sample never reaches its normal volume level.

	Supported only during tone playing.
	*/
	TTimeIntervalMicroSeconds iRampDuration ;

	/**
	The recordable time available to the audio device.

	This value may be the estimated time available.
	*/
	TTimeIntervalMicroSeconds iRecordTimeAvailable ;

	/**
	The maximum duration of the recorded clip, in microseconds
	*/
	TTimeIntervalMicroSeconds iMaxDuration ;

	/**
	The maximum size of the recorded clip, in bytes.
	*/
	TInt iMaxFileSize ;

	/**
	The current state of the audio device.
	*/
	TInt iState ;

	/**
	The current completion status of loading/rebuffering.
	*/
	TInt iLoadingCompletePercentage;

	enum
		{
		/**
		Specifies the maximum buffer length that can be passed with notification request
		*/
		KNotificationDataBufferSize=256
		};
	/**
	The notification event to register for the client.
	*/
	TUid iEventType;
	/**
	The notification data from the client.
	*/
	TBufC8<KNotificationDataBufferSize> iNotificationRegistrationData;
	/**
	The notification data for the client.
	*/
	TBuf8<KNotificationDataBufferSize> iNotificationData;
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	};

/**
Initialises the object with arbitrary values.
*/
inline TMMFAudioConfig::TMMFAudioConfig() {};


/*
@publishedAll
@released

A class used when sending custom commands from the client API
to the audio controller to set repeats
*/
class TMMFAudioSetRepeatsConfig
	{
public:
	IMPORT_C TMMFAudioSetRepeatsConfig();

public:
	
	/**
    The number of times the audio sample, together with the trailing silence, is to be repeated. 
    If this is set to KMdaRepeatForever, then the audio sample, together with the trailing silence, is
    repeated indefinitely or until Stop() is called. If this is set to zero, then the audio sample
    is not repeated. The behaviour is undefined for negative values (other than KMdaRepeatForever).
	*/
	TInt iRepeatNumberOfTimes;
	
	/**
	The time interval of the training silence.
	*/	
	TTimeIntervalMicroSeconds iTrailingSilence;
	
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	};

#endif
