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

#ifndef __SOUNDDEVICE_H__
#define __SOUNDDEVICE_H__

#include <e32base.h>
#include <f32file.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontroller.h>


//Debugging info

/*
Notes: 
This file defines the DevSound class which acts as an interface to
the hardware and is intended to invoke VCR functionality via the plug-ins.
The DevSound also contains a product specific policy.
*/

//UIDs for buffers: According to Symbian: Not used

// Public Media Server includes

/**
@publishedAll
@released

Sampling rates available.

When queried from DevSound by calling the function Capabilities(), iRate indicates
the sampling rates supported by the audio device.

DevSound by default is configured to 8KHz sampling rate.
So to configure DevSound to play/record audio at 16 KHz sampling rate:

The mode should not be OR'ed with each other, only single values are accepted. 
*/
enum TMMFSampleRate
	{
  	/** 8000 Hz Sampling Rate
	*/
	EMMFSampleRate8000Hz  = 0x00000001,
  	/** 11025 Hz Sampling Rate
	*/
	EMMFSampleRate11025Hz = 0x00000002,
  	/** 16000 Hz Sampling Rate
	*/
	EMMFSampleRate16000Hz = 0x00000004,
  	/** 22050 Hz Sampling Rate
	*/
	EMMFSampleRate22050Hz = 0x00000008,
  	/** 32000 Hz Sampling Rate
	*/
	EMMFSampleRate32000Hz = 0x00000010,
  	/** 44100 Hz Sampling Rate
	*/
	EMMFSampleRate44100Hz = 0x00000020,
  	/** 48000 Hz Sampling Rate
	*/
	EMMFSampleRate48000Hz = 0x00000040,
  	/** 88200 Hz Sampling Rate
	*/
	EMMFSampleRate88200Hz = 0x00000080,
  	/** 96000 Hz Sampling Rate
	*/
	EMMFSampleRate96000Hz = 0x00000100,
  	/** 12000 Hz Sampling Rate
	*/
	EMMFSampleRate12000Hz = 0x00000200,
  	/** 24000 Hz Sampling Rate
	*/
	EMMFSampleRate24000Hz = 0x00000400,
  	/** 64000 Hz Sampling Rate
	*/
	EMMFSampleRate64000Hz = 0x00000800
	};

/**
@publishedAll
@released

Mono Stereo playback and record modes available.

When queried from DevSound by calling the function Capabilities(), iChannels
indicates whether the underlying audio device supports playing back stereo
audio samples and recording audio samples in stereo mode or not.

DevSound by default is configured to Mono mode.
So to configure DevSound to play/record audio sample in stereo mode:

The mode should not be OR'ed with each other, only single values are accepted. 
*/
enum TMMFMonoStereo
	{
	/** Mono mode
	*/
	EMMFMono	= 0x00000001,
	/** Stereo mode
	*/
	EMMFStereo	= 0x00000002
	};

/**
@publishedAll
@released

Encoding modes available.

When queried from DevSound by calling the function Capabilities(), iEncoding
provides information about different encoding supported by audio device.

DevSound by default is configured to play PCM16 Bit audio data.
Setting encoding mode is not supported yet:
*/
enum TMMFSoundEncoding
	{
  	/** Signed 8 Bit PCM
	*/
	EMMFSoundEncoding8BitPCM		= 0x00000001,
  	/** Signed 16 Bit PCM
	*/
	EMMFSoundEncoding16BitPCM		= 0x00000002,
  	/** Signed 8 Bit ALaw
	*/
	EMMFSoundEncoding8BitALaw		= 0x00000004,
  	/** Signed 8 Bit MuLaw
	*/
	EMMFSoundEncoding8BitMuLaw		= 0x00000008,
	};

/**
@publishedAll
@released

Stereo support enum.
*/
enum TMMFStereoSupport
	{
	/** No stereo support
	*/
	EMMFNone						= 0x00000000,
	/** Interleaved stereo support
	*/
	EMMFInterleavedOnly				= 0x00000001,
	/** Non Interleaved stereo support
	*/
	EMMFNonInterleavedOnly			= 0x00000002,
	/** Interleaved and Non Interleaved stereo support
	*/
	EMMFBothNonAndInterleaved		= 0x00000003,
 	/** Joint stereo support
 	*/
 	EMMFJoint						= 0x00000004
	};

/**
@publishedAll
@released

Type class for DevSound implementation device capabilities.

CMMFDevSound will return Sampling Rates, Encoding and Stereo feature
supported in this structure when queried for capabilities. DevSound can
also be configured by setting parameters in this structure and passing the
structure to it.
*/
class TMMFCapabilities
	{
public:
	/** Sampling Rates supported
	*/
	TUint	iRate;
	/** Encodings supported
	*/
	TUint	iEncoding;
	/** Mono/Stereo support
	*/
	TUint	iChannels;
	/** Buffer size supported
	*/
	TInt	iBufferSize;
	};

/**
@publishedAll
@released

An interface to a set of DevSound callback functions.


This serves as the method of communication between the client and the
DevSound.

The class is a mixin and is intended to be inherited by the client class
that is interested in observing the DevSound operation. The functions
encapsulated by this class are called when specific events occur in the
process of initializing and playing/recording an audio sample or playing
tones.
*/
class MDevSoundObserver
	{
public:
	/**
	Handles initialization completion event.
	
	A derived class must provide an implementation to handle the initialization
	request.
	
	CMMFDevSound object calls this function when its InitializeL() function
	completes.
	
	@param  aError
	        Error code. KErrNone if successful. Other values are possible
	        indicating a problem initializing CMMFDevSound object.
	*/
	virtual void InitializeComplete(TInt aError)=0;

	/**
	Handles tone play completion event.
	
	A derived class must provide an implementation to handle the tone play
	completion request.
	
	CMMFDevSound object calls this function when an attempt to play tone has
	completed, successfully or otherwise.

	The following are the play tone functions; PlayToneL(), PlayDMTFStringL(),
	PlayToneSequenceL(), and PlayFixedSequenceL().
	
	@param	aError
	        Error code. The status of tone playback. KErrUnderflow playing of
	        the tone is complete. KErrAccessDenied the sound device is in use by
	        another higher priority client. KErrCancel playing of the audio
	        sample is stopped by DevSound client another higher priority client.
	*/
	virtual void ToneFinished(TInt aError)=0;

	/**
	Handles CMMFDevSound object's data request event.
	
	A derived class must provide an implementation to supply CMMFDevSound
	object the data that it needs to play or convert.
	
	CMMFDevSound object calls this function when and where it needs data for
	playing or converting. The observer should notify CMMFDevSound object as
	quickly as possible after the data is read into buffer, aBuffer by calling
	PlayData(), otherwise the implementation might callback function PlayError()
	on derived class object with error code KErrUnderflow.
	This does not apply to the very first call to PlayData(), however.
	It is possible for a user of DevSound to hold the first buffer sent in
	BufferToBeFilled() until ready to play.
	The use case for this is if a low latency audio response
	is required, as at this point all the resources used to play audio are open.
	If used in this way then it is important to be aware that when when the
	resources for audio are ready at the BufferToBeFilled() callback, a Devsound
	on a real device will be running at increased power consumption as the audio
	hw and any required DSPs will be powered up etc.
	
	@param  aBuffer
	        Buffer into which data should be read. The amount of data that is
	        needed is specified in CMMFBuffer::RequestSize().
	*/
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer)=0;

	/**
	Handles play completion or cancel event.

	A derived class must provide an implementation to handle the play
	completion or cancel request.

	CMMFDevSound object calls this function when an attempt to play audio sample
	has completed, successfully or otherwise.

	@param  aError
	        Error code. The status of playback. KErrUnderflow playing of the
	        audio sample is complete. KErrAccessDenied the sound device is in
	        use by another higher priority client.
	*/
	virtual void PlayError(TInt aError)=0;

	/**
	Handles CMMFDevSound object's data request event.

	A derived class must provide an implementation to process the data
	supplied by CMMFDevSound object while recording or converting.

	CMMFDevSound object calls this function when the buffer, aBuffer gets filled
	while recording or converting. The observer should notify CMMFDevSound
	object as quickly as possible after data in the buffer is processed by
	calling RecordData(), otherwise the implementation might callback
	the function RecordError() on derived class object with error code KErrOverflow.

	@param  aBuffer
	        Buffer containing processed (recorded or converted) data. The amount
	        of data that is available is specified in CMMFBuffer::RequestSize().
	*/
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer)=0;

	/**
	Handles record completion or cancel event.
	
	A derived class must provide an implementation to handle the record
	completion or cancel request.
	
	CMMFDevSound object calls this function when an attempt to record audio sample
	has completed, successfully or otherwise.
	
	@param  aError
	        Error code. The status of recording. KErrOverflow audio devices
	        runs out of internal buffer. KErrAccessDenied the sound device is
	        in use by another higher priority client.
	*/
	virtual void RecordError(TInt aError)=0;

	/**
	Handles conversion completion or cancel event.

	A derived class must provide an implementation to handle the conversion
	completion or cancel request.

	CMMFDevSound object calls this function when an attempt to convert data from
	source format to destination format has completed, successfully or otherwise.

	@param  aError
	        Error code. KErrCancel conversion operation is cancelled. KErrNone
	        conversion is complete. Other values are possible indicating a
	        problem converting data.
	*/
	virtual void ConvertError(TInt aError)=0;

	/**
	Handles device event.
	
	A derived class must provide an implementtion to handle the messages from
	audio hardware device.
	
	CMMFDevSound object calls this function when a message is received from the
	audio hardware device.
	
	@param	aMessageType
			Defines the type of message. Used to determine how to
			interpret the contents of aMsg.
	@param	aMsg
			Message that is packed in the Descriptor format.
	*/
	virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg)=0;


	/**
	Handles policy request completion event.
	
	A derived class must provide an implementation to handle the policy request
	completion event.
	
	CMMFDevSound object calls this function when an attempt to acquire sound
	device is rejected by audio policy server.
	
	@param	aEvent
			Not used
	*/
	inline virtual void SendEventToClient(const TMMFEvent& aEvent);

	};


/**
@publishedAll
@released

This is the interface from Symbian OS to the raw audio functions on the
device hardware.

DevSound is currently loaded as a DLL, and implements the VCR-type
functions to be expected for such an interface.

The audio functions include the following functionality:
- Initialisation and configuration of hardware devices, for example, setting microphone gain, 
setting setero balance and so on.
- The playing and recording of raw audio data.
- The playing and dynamic control of tones with user specified frequencies.
- The playing of DTMF strings.
*/
class CMMFDevSound : public CBase
	{
public:

	/**
	Constructs, and returns a pointer to, a new CMMFDevSound object.

	Leaves on failure.
	*/
	IMPORT_C static CMMFDevSound* NewL();

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this
	instance.
	*/
	IMPORT_C ~CMMFDevSound();

	/**
	Initializes CMMFDevSound object to play and record PCM16 raw audio data
	with sampling rate of 8 KHz.

	On completion of Initialization, calls InitializeComplete() on
	aDevSoundObserver.

	Leaves on failure.

	@param  aDevSoundObserver
	        A reference to DevSound Observer instance.
	@param  aMode
	        The mode for which this object will be used. Any value other than EMMFStatePlaying, EMMFStateRecording or EMMFStateTonePlaying is unsupported. 
	
	 
	*/
    IMPORT_C void InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode);


	/**
	Initializes DevSound object for the mode aMode for processing audio data
	with hardware device aHWDev.

	On completion of Initialization, the observer will be notified via call back
	InitializeComplete().

	Leaves on failure.

	@param  aDevSoundObserver
	        A reference to DevSound Observer instance.
	@param  aHWDev
	        The CMMFHwDevice implementation identifier.
	@param  aMode
	        The mode for which this object will be used. Any value other than EMMFStatePlaying, EMMFStateRecording or EMMFStateTonePlaying is unsupported.
	        
   	*/
	IMPORT_C void InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode);

	/**
	Initializes DevSound object for the mode aMode for processing audio data
	with hardware device supporting FourCC aDesiredFourCC.

	On completion of Initialization, the observer will be notified via call back
	InitializeComplete().

	Leaves on failure.

	@param  aDevSoundObserver
	        A reference to DevSound Observer instance.
	@param  aDesiredFourCC
	        The CMMFHwDevice implementation FourCC code.
	@param  aMode
	        The mode for which this object will be used. Any value other than EMMFStatePlaying, EMMFStateRecording or EMMFStateTonePlaying is unsupported.
	        
    */
	IMPORT_C void InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode);

	/**
	Returns the supported Audio settings ie. encoding, sample rates, mono/stereo operation, buffer 
	size etc.

	@return	The device settings.
	*/
	IMPORT_C TMMFCapabilities Capabilities();

	/**
	Returns the current device configuration.

	@return	The device settings.
	*/
	IMPORT_C TMMFCapabilities Config() const;

	/**
	Configure CMMFDevSound object with the settings in aConfig.

	Use this to set sampling rate, encoding and mono/stereo.

	@param  aCaps
	        The attribute values to which CMMFDevSound object will be configured to.
	*/
	IMPORT_C void SetConfigL(const TMMFCapabilities& aCaps);

	/**
	Returns an integer representing the maximum volume device supports.

	This is the maximum value which can be passed to CMMFDevSound::SetVolume.

	@return	The maximum volume. This value is platform dependent but is always greater than or equal
	to one.
	*/
	IMPORT_C TInt MaxVolume();

	/**
	Returns an integer representing the current volume.

	@return The current volume level.
	*/
	IMPORT_C TInt Volume();

	/**
	Changes the current playback volume to a specified value.

	The volume can be changed before or during playback and is effective
	immediately.

	@param  aVolume
	        The volume setting. This can be any value from 0 to the value
	        returned by a call to CMMFDevSound::MaxVolume(). If the
	        volume is not within this range, the volume is automatically set to
	        minimum or maximum value based on the value that is being passed.
	        Setting a zero value mutes the sound. Setting the maximum value
	        results in the loudest possible sound.
	*/
	IMPORT_C void SetVolume(TInt aVolume);

	/**
	Returns an integer representing the maximum gain the device supports.

	This is the maximum value which can be passed to CMMFDevSound::SetGain.

	@return The maximum gain. This value is platform dependent but is always greater than or equal
	to one.
	*/
	IMPORT_C TInt MaxGain();

	/**
	Returns an integer representing the current gain.

	@return The current gain level.
	*/
	IMPORT_C TInt Gain();

	/**
	Changes the current recording gain to a specified value.

	The gain can be changed before or during recording and is effective
	immediately.

	@param  aGain
	        The gain setting. This can be any value from zero to the value
	        returned by a call to CMMFDevSound::MaxGain(). If the
	        volume is not within this range, the gain is automatically set to
	        minimum or maximum value based on the value that is being passed.
	        Setting a zero value mutes the sound. Setting the maximum value
	        results in the loudest possible sound.
	*/
	IMPORT_C void SetGain(TInt aGain);

	/**
	Returns the speaker balance set for playing.

	Leaves on failure.

	@param  aLeftPercentage
	        On return contains the left speaker volume percentage.
	@param  aRightPercentage
	        On return contains the right speaker volume percentage.
	*/
	IMPORT_C void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	Sets the speaker balance for playing.

	The speaker balance can be changed before or during playback and is
	effective immediately.

	@param  aLeftPercentage
	        On return contains left speaker volume perecentage. This can be any
	        value from zero to 100. Setting a zero value mutes the sound on left
	        speaker.
	@param  aRightPercentage
	        On return contains right speaker volume perecentage. This can be any
	        value from zero to 100. Setting a zero value mutes the sound on
	        right speaker.
	*/
	IMPORT_C void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage);

	/**
	Returns the microphone gain balance set for recording.

	Leaves on failure.

	@param  aLeftPercentage
	        On return contains the left microphone gain percentage.
	@param  aRightPercentage
	        On return contains the right microphone gain percentage.
	*/
	IMPORT_C void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	Sets the microphone gain balance for recording.

	The microphone gain balance can be changed before or during recording and
	is effective immediately.

	@param  aLeftPercentage
	        The left microphone gain precentage. This can be any value from zero to
	        100. Setting a zero value mutes the gain on left microphone.
	@param  aRightPercentage
	        The right microphone gain precentage. This can be any value from zero to
	        100. Setting a zero value mutes the gain on right microphone.
	*/
	IMPORT_C void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage);

	/**
	Initializes the audio device and starts the play process.

	This function queries and acquires the audio policy before initializing audio device. If there was
	an error during policy initialization, PlayError() function will be called on	the observer with
	error code KErrAccessDenied, otherwise BufferToBeFilled() function will be called with a buffer
	reference. After reading data into the buffer reference passed, the client should call
	PlayData() to play data.

	The amount of data that can be played is specified in
	CMMFBuffer::RequestSize(). Any data that is read into buffer beyond this
	size will be ignored.

	Leaves on failure.

	@see    MDevSoundObserver::PlayError()
	@see    MDevSoundObserver::BufferToBeFilled()
	*/
	IMPORT_C void PlayInitL();

	/**
	Initializes audio device and starts the recording process. 

	This function queries and acquires the audio policy before initializing audio device. If there 
	was an error during policy initialization, RecordError() function will be called on the observer 
	with error code KErrAccessDenied, otherwise BufferToBeEmptied()	function will be called with a 
	buffer reference. This buffer contains recorded	or encoded data. After processing data in the 
	buffer reference passed, the client should call RecordData() to continue recording process.

	The amount of data that is available is specified in CMMFBuffer::RequestSize().

	Leaves on failure.

	@see    MDevSoundObserver::RecordError() 
	@see    MDevSoundObserver::BufferToBeEmptied()
	
	@capability	UserEnvironment
			For recording - the requesting client process must have the 
			UserEnvironment capability.
	*/
	IMPORT_C void RecordInitL();

	/**
	Plays data in the buffer at the current volume. 

	The client should fill the buffer with audio data before calling this function. The observer gets
	a reference to the buffer along with the callback function BufferToBeFilled(). When playing of
	the audio sample is complete, successfully or otherwise, the function PlayError() on the
	observer is called.

	The last buffer of the audio stream being played should have the last buffer flag set using
	CMMFBuffer::SetLastBuffer(TBool). If a subsequent attempt to play the clip is made, this flag 
	will need resetting by the client.

	@see    MDevSoundObserver::PlayError()
	*/
	IMPORT_C void PlayData();

	/**
	Contine the process of recording. 

	Once the buffer is filled with recorded	data, the Observer gets a reference to the buffer along 
	with the callback function BufferToBeEmptied(). After processing the buffer (copying over to a
	different buffer or writing to file) the client should call this function to continue the 
	recording process.
	
	@capability	UserEnvironment
			For recording - the requesting client process must have the 
			UserEnvironment capability.
	*/
	IMPORT_C void RecordData();

	/**
	Stops the ongoing operation (Play, Record, TonePlay, Convert).

	This function should be synchronous and invoke no callbacks through MDevSoundObserver.
	*/
	IMPORT_C void Stop();

	/**
	Temporarily Stops the ongoing operation (Play, Record, TonePlay, Convert).
	
	The behaviour of Pause() is currently undefined when the initialisation mode
	is not EMMFStateRecording, consequently different DevSound implementations exhibit
	different behaviour for pause during play.  For this reason, it is recommended that
	when pausing of audio during playing is required, Stop() is used instead of the call
	to Pause(). To resume audio playing after Stop(), call PlayInitL(). Among other things,
	this will internally reset the SamplesPlayed() result and the calling code may need to
	remember the previous result to give the correct "samples played from start" value.

	In the case of record, Pause is taken to mean halt any further recording
	but continue to call the MDevSoundObserver::BufferToBeEmptied() passing
	already recorded buffers. The last buffer flag should be set when
	all recorded buffers have been flushed.
	
	@see CMMFDevSound::Stop()
	@see CMMFDevSound::PlayInitL()
	@see CMMFDevSound::SamplesPlayed()
	*/
	IMPORT_C void Pause();

	/**
	Returns the number samples recorded so far.

	@return The samples recorded.
	*/
	IMPORT_C TInt SamplesRecorded();

	/**
	Returns the number of samples played so far.

	@return The samples played.
	*/
	IMPORT_C TInt SamplesPlayed();

	/**
	Initializes the audio device and starts playing a tone. The tone is played with the
	frequency and duration specified.

	Leaves on failure.

	@param  aFrequency
	        The frequency at which the tone will be played.
	@param  aDuration
	        The period over which the tone will be played. A zero value causes
	        the no tone to be played.
	*/
	IMPORT_C void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);

	/**
	Initializes audio device and starts playing a dual tone.

	The generated tone consists of two sine waves of different frequencies
	summed together.

	Dual Tone is played with the specified frequencies and for the specified duration.

	@param  aFrequencyOne
	        The first frequency of dual tone.
	@param  aFrequencyTwo
	        The second frequency of dual tone
	@param  aDuration
	        The period over which the tone will be played. A zero value causes
	        the no tone to be played (Verify this with test app).
	*/
	IMPORT_C void PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);

	/**
	Initializes the audio device and starts playing the DTMF string aDTMFString.

	Leaves on failure.

	@param  aDTMFString
	        The DTMF sequence in a descriptor.
	*/
	IMPORT_C void PlayDTMFStringL(const TDesC& aDTMFString);

	/**
	Initializes the audio device and starts playing a tone sequence.

	Leaves on failure.

	@param  aData
	        The tone sequence in a descriptor.
	*/
	IMPORT_C void PlayToneSequenceL(const TDesC8& aData);

	/**
	Initializes the audio device and starts playing the specified pre-defined tone
	sequence.

	Leaves on failure.

	@param  aSequenceNumber
	        The index identifying the specific pre-defined tone sequence. Index
	        values are relative to zero.
	        This can be any value from zero to the value returned by a call to
			FixedSequenceCount() - 1.
	        The function raises a panic if the sequence number is not within this
	        range.
	@see	FixedSequenceCount()
	*/
	IMPORT_C void PlayFixedSequenceL(TInt aSequenceNumber);

	/**
	Defines the number of times the audio is to be repeated during the tone
	playback operation.

	A period of silence can follow each playing of a tone. The tone playing can
	be repeated indefinitely.

	@param  aRepeatCount
	        The number of times the tone, together with the trailing silence,
	        is to be repeated. If this is set to KMdaRepeatForever, then the
	        tone, together with the trailing silence, is repeated indefinitely
	        or until Stop() is called. If this is set to zero, then the tone is
	        not repeated.
	@param  aRepeatTrailingSilence
	        An interval of silence which will be played after each tone.
	        Supported only during tone playing.
	*/
	IMPORT_C void SetToneRepeats(TInt aRepeatCount,
					const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);

	/**
	Defines the duration of tone on, tone off and tone pause to be used during the
	DTMF tone playback operation.

	Supported only during tone playing.

	@param  aToneOnLength
	        The period over which the tone will be played. If this is set to
	        zero, then the tone is not played.
	@param  aToneOffLength
	        The period over which the no tone will be played.
	@param  aPauseLength
	        The period over which the tone playing will be paused.
	*/
	IMPORT_C void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength);

	/**
	Defines the period over which the volume level is to rise smoothly from
	nothing to the normal volume level.

	The function is only available while the tone is playing.

	@param  aRampDuration
	        The period over which the volume is to rise. A zero value causes
	        the tone sample to be played at the normal level for the full
	        duration of the playback. A value, which is longer than the duration
	        of the tone sample means that the sample never reaches its normal
	        volume level.
	*/
	IMPORT_C void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	/**
	Defines the priority settings that should be used for this instance.

	@param  aPrioritySettings
	        A class type representing the client's priority, priority preference and state.
	*/
	IMPORT_C void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	/**
	Retrieves a custom interface to the device.

	@param  aInterfaceId
	        The interface UID, defined with the custom interface.
	
	@return A pointer to the interface implementation, or NULL if the device does not
	        implement the interface requested. The return value must be cast to the
	        correct type by the user.The user should be careful while caching the custom interface pointer,
	        as in some situations the lower level custom interface pointer may become NULL.
	*/
	IMPORT_C TAny* CustomInterface(TUid aInterfaceId);

	/**
	Returns the number of available pre-defined tone sequences.

	This is the number of fixed sequence supported by DevSound by default.

	@return The fixed sequence count. This value is implementation dependent but is always greater 
	        than or equal to zero.
	*/
	IMPORT_C TInt FixedSequenceCount();

	/**
	Returns the name assigned to a specific pre-defined tone sequence.

	This is the number of the fixed sequence supported by DevSound by default.

	The function raises a panic if sequence number specified is invalid.

	@param  aSequenceNumber
	        The index identifying the specific pre-defined tone sequence. Index values are relative 
	        to zero. This can be any value from zero to the value returned by a call to
	        FixedSequenceCount() - 1.
	        The function raises a panic if sequence number is not within this
	        range.

	@return A reference to a Descriptor containing the fixed sequence name indexed by
	        aSequenceNumber.
	        
	@see	FixedSequenceCount()
	*/
	IMPORT_C const TDesC& FixedSequenceName(TInt aSequenceNumber);

	/**
	Returns a list of the supported input datatypes that can be sent to DevSound for playing audio.
	The datatypes returned are those that the DevSound supports given the priority settings passed
	in aPrioritySettings.

	Note that if no supported data types are found this does not constitute failure, the function will
	return normally with no entries in aSupportedDataTypes.

	@param  aSupportedDataTypes
	        The array of supported data types that will be filled in by this function.
	        The supported data types of the DevSound are in the form of an array
	        of TFourCC codes. Any existing entries in the array will be overwritten on 
	        calling this function. If no supported data types are found given the priority 
	        settings, then the aSupportedDatatypes array will have zero entries.
	@param  aPrioritySettings
	        The priority settings used to determine the supported datatypes.  Note this
	        does not set the priority settings. For input datatypes the iState member
	        of the priority settings would be expected to be either
	        EMMFStatePlaying or EMMFStatePlayingRecording. The priority settings may
	        effect the supported datatypes depending on the audio routing.
	*/
	IMPORT_C void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;

	/**
	Returns a list of the supported output dataypes that can be received from DevSound for
	recording audio.  The datatypes returned are those that the DevSound supports given the priority 
	settings passed in aPrioritySettings.

	Note that if no supported data types are found this does not constitute failure, the function will 
	return normally with no entries in aSupportedDataTypes.

	@param  aSupportedDataTypes
	        The array of supported data types that will be filled in by this function.
	        The supported datatypes of the DevSound are in the form of an array
	        of TFourCC codes.
	        Any existing entries in the array will be overwritten on calling this function.
	        If no supported datatypes are found given the priority settings, then
	        the aSupportedDatatypes array will have zero entries.
	@param  aPrioritySettings
	        The priority settings used to determine the supported data types.  Note this
	        does not set the priority settings. For output data types the iState member
	        of the priority settings would expected to be either
	        EMMFStateRecording or EMMFStatePlayingRecording. The priority settings may
	        effect the supported datatypes depending on the audio routing.
	*/
	IMPORT_C void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;
	
	/** 
	Registers the client for notification of resource avalibility.

	@param  aEventType
	        The Notification event type for which the client needs notification.
	@param  aNotificationRegistrationData
	        The Notification Registration data has been reserved for future use and its value should be always NULL
	@return An error code indicating if the function call was successful. KErrNone on success, 
			KErrNotSupported if the event type is not supported, KErrArgument if the notification data
			is not null otherwise another of the system-wide error codes.
	*/
	inline TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	
	/**
	Cancels the Registered Notification.
	
	@param  aEventType
	        The Event type need to cancel
	@return An error code indicating if the function call was successful. KErrNone on success, KErrNotSupported
			if the event type is not supported otherwise another of the system-wide error codes.
	*/	
	inline TInt CancelRegisterAsClient(TUid aEventType);
	
	/**
	Returns the Notification data which the client needs to resume playing.
	
	@param aEventType
	       The Event type for which to get notification data  
	@param aNotificationData
			The reference data for which the client needs to resume the play. The actual data depends on the event type.
			Note that for the event type 'KMMFEventCategoryAudioResourceAvailable' the package buffer returned
			is TMMFTimeIntervalMicroSecondsPckg,but the contents should be converted to an integer and
			interpreted as the data returned is samples played ,but not as a microsecond value.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline TInt GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData);
	
	/**
	Wait for the clients to resume play back even after the default timeout expires.
	Unless the client cancels the notification request or completes no other client gets
	notification.
	
	@return An error code indicating if the function call was successful. KErrNone on success,
			KErrPermissionDenied if the client does not have MultimediaDD capaility, otherwise
	        another of the system-wide error codes.
	*/
	inline TInt WillResumePlay();
	
	/**

	Empties the play buffers below DevSound without causing the codec to be deleted.

	@return An error code indicating if the function call was successful. KErrNone on success,
			KErrNotSupported if called in a mode other than EMMFStatePlaying or if the function is otherwise not supported,
			KErrNotReady if this is called before playing,
			otherwise another of the system-wide error codes.
			If this function is not supported, it is advised to use Stop().
	*/
	inline TInt EmptyBuffers();

	/** Cancels the initialization process of a CMMFDevSound object

	@return An error code indicating if the function call was successful. 
			KErrNone on success,
			KerrNotReady if this is called before InitializeL() call or after      
			the object has been initialized, 
	*/
	inline TInt CancelInitialize();
	
	/**
	Sets client thread info for devsound to make proper policy and routing decisions
	
	@param  aTid
			The thread Id of real client.
	
	@capability MultimediaDD
			A process requires MultimediaDD capability to make this call. 
	*/
	inline TInt SetClientThreadInfo(TThreadId aTid);

	/**
	Retrieves the current play time from the audio renderer
	
	@param  aTime
	        A reference to TTimeIntervalMicroSeconds object which will be filled with the current play time by this function.  
	@return An error code indicating if the function call was successful. 
			KErrNone on success,
			KErrNotSupported if the underlying devsound implementation or the HwDevice does not support this API,
			otherwise any of the system-wide error codes.
			If this function is not supported, SamplesPlayed() needs to be used instead.
	
	@see	IsGetTimePlayedSupported()
	*/
	inline TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime);
	
	/**
	This method can be used by devsound clients to check if the devsound implementation supports
	GetTimePlayed API. If this method returns EFalse, SamplesPlayed needs to be used.
	
	@return ETrue if the underlying devsound implementation supports GetTimePlayed API,
			EFalse if the the underlying devsound implementation does not support GetTimePlayed API
	
	@see	GetTimePlayed()
	*/
	inline TBool IsGetTimePlayedSupported();

	/**
	This method can be used by devsound clients to check if the devsound implementation ignores the underflow
	errors from the sound driver and does not propagate them to its clients. 
	
	@return ETrue 	if devsound ignores all the underflow errors from the sound driver except for the last buffer
					That means DevSound propagates the underflow error from sound driver to its clients only when 
					client sets the last buffer flag on the CMMFBuffer type buffer.
			EFalse 	if devsound propagates the underflow errors from the sound driver in all the cases
	
	*/
	inline TBool QueryIgnoresUnderflow();

	/**
	This method can be used by devsound clients to check if current configuration supports resume
	This call is only valid when DevSound is initialized
	@return ETrue 	if current format does supports resume
			EFalse 	otherwise
	*/
	inline TBool IsResumeSupported();

	/**
	Resume play back, recording or tone playing after this has been paused
	@return An error code indicating if the function call was successful. 
			KErrNone on success,
			KErrNotSupported if current format configuration does not support this functionality
			KErrNotReady if DevSound is not paused
	*/
	inline TInt Resume();

protected:

	/**
	Default Constructor.
	*/
	//So that nobody can extend derive from CMMFDevSound.
	CMMFDevSound();

	/**
	Second phase constructor.
	*/
	void ConstructL();

	class CBody;
	
	/**
	DevSound body
	*/
	CBody* iBody;
	};

inline void MDevSoundObserver::SendEventToClient(const TMMFEvent& /*aEvent*/)
	{
	}

#include <mmf/server/sounddevice.inl>

#endif // __SOUNDDEVICE_H__
