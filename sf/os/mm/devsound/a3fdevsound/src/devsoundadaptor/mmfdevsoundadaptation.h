/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MMFDEVSOUNDADAPTATION_H
#define MMFDEVSOUNDADAPTATION_H

//  INCLUDES
#include <mmf/server/sounddevice.h>
#include <a3f/a3f_trace_utils.h>

// CLASS FORWARD
class MGlobalProperties;

const TInt KCallbackNone 				= 0;
const TInt KCallbackProcessingFinished	= 1;
const TInt KCallbackProcessingUnitError	= 2;
const TInt KCallbackRecordPauseComplete	= 3;
const TInt KCallbackAutoPauseResume 	= 4; 
const TInt KCallbackFlushComplete		= 5;

// CLASS DECLARATION

/**
@publishedAll
@released

An interface to a set of DevSound adaptation observer callback functions.

This serves as the method of communication between the client and the
DevSound.

The class is a mixin and is intended to be inherited by the client class
that is interested in observing the DevSound operation. The functions
encapsulated by this class are called when specific events occur in the
process of initializing and playing/recording an audio sample or playing
tones.
*/
class MDevSoundAdaptationObserver
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

	@param  aError
			Error code. The status of tone playback. KErrUnderflow playing of
			the tone is complete. KErrAccessDenied the sound device is in use by
			another higher priority client. KErrCancel playing of the audio
			sample is stopped by DevSound client another higher priority client.
	*/
	virtual void ToneFinished(TInt aError)=0;

	/**
	Handles CMMFDevSound object's data request event.

	A derived class must provide an implementation to supply CMMFDevSound
	object the data that it needs to play.

	CMMFDevSound object calls this function when and where it needs data for
	playing. The observer should notify CMMFDevSound object as
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
	supplied by CMMFDevSound object while recording.

	CMMFDevSound object calls this function when the buffer, aBuffer gets filled
	while recording. The observer should notify CMMFDevSound
	object as quickly as possible after data in the buffer is processed by
	calling RecordData(), otherwise the implementation might callback
	the function RecordError() on derived class object with error code KErrOverflow.

	@param  aBuffer
			Buffer containing processed (recorded) data. The amount
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
	Handles device event.

	A derived class must provide an implementtion to handle the messages from
	audio hardware device.

	CMMFDevSound object calls this function when a message is received from the
	audio hardware device.

	@param  aMessageType
			Defines the type of message. Used to determine how to
			interpret the contents of aMsg.
	@param  aMsg
			Message that is packed in the Descriptor format.
	*/
	virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg)=0;

	/**
	A derived class must provide an implementation to handle the low layer operation completion
	@param	aError	The status of operation in progress
	@param	aCanStartNewOperation	If EFalse indicates that the operation has more than a cycle
	*/
	virtual void AsynchronousOperationComplete(TInt aError, TBool aCanStartNewOperation) = 0;


	/**
	Underlying interface has been (or is about to be) deleted.
	Implementations will generally cancel outstanding messages

	@param	aInterfaceId	Uid of the Interface which has been deleted
	*/
	virtual void InterfaceDeleted(TUid aInterfaceId) = 0;

	/*
	Underlying physical adaptation has sent a callback that will result either on a commit
	and need to scheduled or in the case of RecordPauseComplete, completes the message.

	@param aType the callback type
	@param aError KErrNone if successful, else corresponding error
		code
	*/
	virtual void CallbackFromAdaptorReceived(TInt aType, TInt aError) = 0;
	
	/*
	Underlying physical adaptation has sent a callback indicating that a preemption process 
 	has started, any incomming commands will be queued during the preemption.  
	*/
	virtual void PreemptionStartedCallbackReceived() = 0;

	/*
	Underlying physical adaptation has sent a callback indicating that a preemption process
	has finished. Queued commands during preemption can now be processed. 

	@param	aCanStartNewOperation	If EFalse indicates that the operation has more than a cycle
	*/
	virtual void PreemptionFinishedCallbackReceived(TBool aCanStartNewOperation) = 0;

	/*
	 * The adaptor has control of the context.
	 * @return returns True if the adaptor is the entity responsible for calling Commit()
	 */
	virtual TBool AdaptorControlsContext() const = 0;

	/*
	 * Callback indicating that a clash with pre-emption has occurred during the commit cycle
	 * and to push the current request that was being processed onto front of queue.
	 */

	virtual void PreemptionClash() = 0;

	/*
	 * Callback indicating the clash with pre-emption during the commit cycle was with state change,
	 * so the current request that was previously pushed onto the front of queue should be removed
	 * without processing.
	 */

	virtual void PreemptionClashWithStateChange() = 0;

	/*
	 * Callback to indicate an error has been noticed. This is to be cached until subsequent
	 * AsynchronousOperationComplete(), and handled then if needs.
	 */
	virtual void NotifyError(TInt aError) = 0;

	};

// CLASS DECLARATION

/**
@publishedAll
@released
* This class implements DevSound behavior in a hardware independent way.
*
*  @lib MmfDevSoundAdaptation.lib
*  @since
*/
class CMMFDevSoundAdaptation : public CBase
{

public:  // Constructors and destructor

	/**
	* Constructs, and returns a pointer to, a new CMMFDevSoundAdaptation
	* object.
	* Leaves on failure..
	* @param MDevSoundAdaptationObserver& aDevSoundObserver A reference to DevSound
	*        Observer instance.
	* @param MGlobalPrpoerties a reference to Audio Server global properties
	* @return CMMFDevSoundAdaptation* A pointer to newly created object.
	*/
	IMPORT_C static CMMFDevSoundAdaptation* NewL(MDevSoundAdaptationObserver& aDevSoundObserver,
														MGlobalProperties& aGlobalProperties);

	/**
	* Destructor.
	*/
	IMPORT_C virtual ~CMMFDevSoundAdaptation();

public: // New functions

	/*
	2nd phase open.
	Async open - if returns successfully, then will result in AsynchronousOperationComplete()
	*/
	IMPORT_C void PostOpenL();

	/**
	* Initializes to raw audio data PCM16 and Sampling Rate of 8 KHz.
	* On completion of Initialization, calls InitializeComplete() on
	* aDevSoundObserver.
	* Leaves on failure.
	* @since
	* @param MDevSoundAdaptationObserver& aDevSoundObserver A reference to DevSound
	*        Observer instance.
	* @param TMMFState aMode Mode for which this object will be used.
	* @return void
	*/
	IMPORT_C void InitializeL(TMMFState aMode);

	/**
	* Initializes DevSound object for the mode aMode for processing audio
	* data with hardware device aHWDev.
	* On completion of Initialization, calls InitializeComplete() on
	* aDevSoundObserver.
	* Leaves on failure.
	* @since
	* @param MDevSoundAdaptationObserver& aDevSoundObserver A reference to DevSound
	*        Observer instance.
	* @param TUid aHWDev The CMMFHwDevice implementation identifier.
	* @param TMMFState aMode The mode for which this object will be used
	* @return void
	*/
	IMPORT_C void InitializeL(TUid aHWDev,
							  TMMFState aMode);

	/**
	* Initializes DevSound object for the mode aMode for processing audio
	* data with hardware device supporting FourCC aDesiredFourCC.
	* Leaves on failure.
	* @since
	* @param MDevSoundAdaptationObserver& aDevSoundObserver A reference to DevSound
	*        Observer instance.
	* @param TFourCC aDesiredFourCC The CMMFHwDevice implementation FourCC
	*        code.
	* @param TMMFState aMode The mode for which this object will be used
	* @return KErrNone if successfull, else corresponding error code
	* @return void
	*/
	IMPORT_C void InitializeL(TFourCC aDesiredFourCC,
							  TMMFState aMode);

	/**
	* Cancels the initialization of a DevSound object.
	* @since
	* @return An error code indicating if the function call was successful.
	*   	  KErrNone on success,
	*		  KerrNotReady if this is called before InitializeL() call or after
	*		  the object has been initialized,
	*/
	IMPORT_C TInt CancelInitialize();

	/**
	* Returns the supported Audio settings ie. encoding, sample rates,
	* mono/stereo operation, buffer size etc..
	* @since
	* @return TMMFCapabilities The device settings.
	*/
	IMPORT_C TInt Capabilities(TMMFCapabilities& aCap);

	/**
	* Returns the current device configuration.
	* @since
	* @return TMMFCapabilities The device settings.
	*/
	IMPORT_C TMMFCapabilities Config() const;

	/**
	* Configure CMMFDevSound object with the settings in aConfig. Use this
	* to set sampling rate, encoding and mono/stereo.
	* Leaves on failure.
	* @since
	* @param const TMMFCapabilities& aConfig The attribute values to which
	*        CMMFDevSound object will be configured to.
	* @return void
	*/
	IMPORT_C void SetConfigL(const TMMFCapabilities& aCaps);

	/**
	* Returns an integer representing the maximum volume device supports.
	* This is the maximum value which can be passed to
	* CMMFDevSound::SetVolume.
	* @since
	* @return TInt The maximum volume. This value is platform dependent but
	*        is always greater than or equal to one.
	*/
	IMPORT_C TInt MaxVolume();

	/**
	* Returns an integer representing the current volume.
	* @since
	* @return TInt The current volume level.
	*/
	IMPORT_C TInt Volume();

	/**
	* Changes the current playback volume to a specified value. The volume
	* can be changed before or during playback and is effective immediately.
	* @since
	* @param TInt aVolume The volume setting. This can be any value from 0
	*        to the value returned by a call to
	*        CMMFDevSound::MaxVolume(). If the volume is not
	*        within this range, the volume is automatically set
	*        to minimum or maximum value based on the value
	*        that is being passed. Setting a zero value mutes
	*        the sound. Setting the maximum value results in
	*        the loudest possible sound.
	* @param aAsyncCompletion Returns ETrue to say the call is asynchronous, with result sent
	*        to AsynchronousOperationComplete()
	* @return KErrNone if successful, otherwise system-wide error code
	*/
	IMPORT_C TInt SetVolume(TInt aVolume, TBool& aAyncCompletion);

	/**
	* Returns an integer representing the maximum gain the device supports.
	* This is the maximum value which can be passed to CMMFDevSound::SetGain
	* @since
	* @return TInt The maximum gain. This value is platform dependent but is
	*        always greater than or equal to one.
	*/
	IMPORT_C TInt MaxGain();

	/**
	* Returns an integer representing the current gain.
	* @since
	* @return TInt The current gain level.
	*/
	IMPORT_C TInt Gain();

	/**
	* Changes the current recording gain to a specified value. The gain can
	* be changed before or during recording and is effective immediately.
	* @since
	* @param TInt aGain The gain setting. This can be any value from zero to
	*        the value returned by a call to
	*        CMMFDevSound::MaxGain(). If the volume
	*        is not within this range, the gain is automatically
	*        set to minimum or maximum value based on the value
	*        that is being passed. Setting a zero value mutes the
	*        sound. Setting the maximum value results in the
	*        loudest possible sound.
	* @param aAsyncCompletion Returns ETrue to say the call is asynchronous, with result sent
	*        to AsynchronousOperationComplete()
	* @return KErrNone if successful, otherwise system-wide error code
	*/
	IMPORT_C TInt SetGain(TInt aGain, TBool& aAyncCompletion);

	/**
	* Returns the speaker balance set for playing.
	* Leaves on failure.
	* @since
	* @param TInt &aLeftPercentage On return contains the left speaker
	*        volume percentage.
	* @param TInt &aRightPercentage On return contains the right speaker
	*        volume percentage.
	* @return void
	*/
	IMPORT_C void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	* Sets the speaker balance for playing. The speaker balance can be
	* changed before or during playback and is effective immediately.
	* Leaves on failure.
	* @since
	* @param TInt aLeftPercentage The left speaker volume percentage. This
	*        can be any value from zero to 100. Setting
	*        a zero value mutes the sound on left
	*        speaker.
	* @param TInt aRightPercentage The right speaker volume percentage.
	*        This can be any value from zero to 100.
	*        Setting a zero value mutes the sound on
	*        right speaker.
	* @param aAsyncCompletion Returns ETrue to say the call is asynchronous, with result sent
	*        to AsynchronousOperationComplete()
	* @return void
	*/
	IMPORT_C void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage, TBool& aAyncCompletion);

	/**
	* Returns the microphone gain balance set for recording.
	* Leaves on failure.
	* @since
	* @param TInt &aLeftPercentage On return contains the left microphone
	*        gain percentage.
	* @param TInt &aRightPercentage On return contains the right microphone
	*        gain percentage.
	* @return void
	*/
	IMPORT_C void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	* Sets the microphone balance for recording. The microphone balance can
	* be changed before or during recording and is effective immediately.
	* Leaves on failure.
	* @since
	* @param TInt aLeftPercentage The left microphone gain percentage. This
	*        can be any value from zero to 100. Setting
	*        a zero value mutes the sound from left
	*        microphone.
	* @param TInt aRightPercentage The right microphone gain percentage.
	*        This can be any value from zero to 100.
	*        Setting a zero value mutes the sound from
	*        right microphone.
	* @param aAsyncCompletion Returns ETrue to say the call is asynchronous, with result sent
	*        to AsynchronousOperationComplete()
	* @return void
	*/
	IMPORT_C void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage, TBool& aAyncCompletion);

	/**
	* Initializes the audio device and starts the play process. This
	* function queries and acquires the audio policy before initializing
	* audio device. If there was an error during policy initialization,
	* PlayError() function will be called on the observer with error code
	* KErrAccessDenied, otherwise BufferToBeFilled() function will be called
	* with a buffer reference. After reading data into the buffer reference
	* passed, the client should call PlayData() to play data.
	* The amount of data that can be played is specified in
	* CMMFBuffer::RequestSize(). Any data that is read into buffer beyond
	* this size will be ignored.
	* Leaves on failure.
	* @since
	* @return void
	*/
	IMPORT_C void PlayInitL();

	/**
	* Initializes the audio device and starts the record process. This
	* function queries and acquires the audio policy before initializing
	* audio device. If there was an error during policy initialization,
	* RecordError() function will be called on the observer with error code
	* KErrAccessDenied, otherwise BufferToBeEmptied() function will be called
	* with a buffer reference. This buffer contains recorded or encoded
	* data. After processing data in the buffer reference passed, the client
	* should call RecordData() to continue recording process.
	* The amount of data that is available is specified in
	* CMMFBuffer::RequestSize().
	* Leaves on failure.
	* @since
	* @return void
	*/
	IMPORT_C void RecordInitL();

	/**
	* Plays data in the buffer at the current volume.
	* The client should fill the buffer with audio data before calling this
	* function. The observer gets a reference to the buffer along with the
	* callback function BufferToBeFilled(). When playing of the audio sample
	* is complete, successfully or otherwise, the function PlayError() on
	* the observer is called.
	* The last buffer of the audio stream being played should have the last
	* buffer flag set using CMMFBuffer::SetLastBuffer(TBool). If a
	* subsequent attempt to play the clip is made, this flag will need
	* resetting by the client.
	* @return void
	*/
	IMPORT_C void PlayData();

	/**
	* Contine the process of recording.
	* Once the buffer is filled with recorded data, the Observer gets a
	* reference to the buffer along with the callback function
	* BufferToBeEmptied(). After processing the buffer (copying over to a
	* different buffer or writing to file) the client should call this
	* function to continue the recording process.
	* @return void
	*/
	IMPORT_C void RecordData();

	/**
	* Stops the ongoing operation (Play, Record, TonePlay).
	* @since
	* @return KErrNone if successful, system wide error otherwise
	*/
	IMPORT_C TBool Stop();

	/**
	* Temporarily Stops the ongoing operation (Play, Record, TonePlay).
	* @since
	* @return KErrNone if successful, system wide error otherwise
	*/
	IMPORT_C TInt Pause();

	/**
	* Returns the Sample recorded so far
	* @since
	* @return TInt Returns the samples recorded.
	*/
	IMPORT_C TInt SamplesRecorded();

	/**
	* Returns the Sample played so far
	* @since
	* @return TInt Returns the samples played.
	*/
	IMPORT_C TInt SamplesPlayed();

	/**
	* Initializes the audio device and starts playing a tone. The tone is
	* played with the frequency and duration specified.
	* Leaves on failure.
	* @since
	* @param TInt aFrequency The frequency at which the tone will be played.
	* @param const TTimeIntervalMicroSeconds &aDuration The period over
	*        which the tone will be played. A zero value causes the no tone
	*        to be played.
	* @return void
	*/
	IMPORT_C void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);

	/**
	* Initializes audio device and starts playing a dual tone. Dual Tone is
	* played with the specified frequencies and for the specified duration.
	* Leaves on failure.
	* @since
	* @param TInt aFrequencyOne The first frequency of dual tone.
	* @param TInt aFrequencyTwo The second frequency of dual tone.
	* @param const TTimeIntervalMicroSeconds &aDuration The period over
	*        which the tone will be played. A zero value causes the no tone
	*        to be played.
	* @return void
	*/
	IMPORT_C void PlayDualToneL(TInt aFrequencyOne,
								TInt aFrequencyTwo,
								const TTimeIntervalMicroSeconds& aDuration);

	/**
	* Initializes the audio device and starts playing the DTMF string
	* aDTMFString.
	* Leaves on failure.
	* @since
	* @param const TDesC &aDTMFString The DTMF sequence in a descriptor.
	* @return void
	*/
	IMPORT_C void PlayDTMFStringL(const TDesC& aDTMFString);

	/**
	* Initializes the audio device and starts playing a tone sequence.
	* Leaves on failure.
	* @since
	* @param const TDesC8 &aData The tone sequence in a descriptor.
	* @return void
	*/
	IMPORT_C void PlayToneSequenceL(const TDesC8& aData);

	/**
	* Initializes the audio device and starts playing the specified
	* pre-defined tone sequence.
	* Leaves on failure.
	* @since
	* @param TInt aSequenceNumber The index identifying the specific
	*        pre-defined tone sequence. Index values are relative to zero.
	*        This can be any value from zero to the value returned by a call
	*        to FixedSequenceCount() - 1. The function raises a panic if the
	*        sequence number is not within this range.
	* @return void
	*/
	IMPORT_C void PlayFixedSequenceL(TInt aSequenceNumber);

	/**
	* Defines the number of times the audio is to be repeated during the
	* tone playback operation. A period of silence can follow each playing
	* of a tone. The tone playing can be repeated indefinitely
	* @since
	* @param TInt aRepeatCount The number of times the tone, together with
	*        the trailing silence, is to be repeated. If this is set to
	*        KMdaRepeatForever, then the tone, together with the trailing
	*        silence, is repeated indefinitely or until Stop() is called.
	*        If this is set to zero, then the tone is not repeated.
	* @param const TTimeIntervalMicroSeconds &aRepeatTrailingSilence An
	*        interval of silence which will be played after each tone.
	*        Supported only during tone playing.
	* @return KErrNone if successful, system wide error otherwise
	*/
	IMPORT_C TInt SetToneRepeats(TInt aRepeatCount,
				  const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);

	/**
	* Defines the duration of tone on, tone off and tone pause to be used
	* during the DTMF tone playback operation.
	* Supported only during tone playing.
	* @since
	* @param TTimeIntervalMicroSeconds32 &aToneOnLength The period over
	*        which the tone will be played. If this is set to zero, then the
	*        tone is not played.
	* @param TTimeIntervalMicroSeconds32 &aToneOffLength The period over
	*        which the no tone will be played.
	* @param TTimeIntervalMicroSeconds32 &aPauseLength The period over which
	*        the tone playing will be paused.
	* @return KErrNone if successful, system wide error otherwise
	*/
	IMPORT_C TInt SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
								 TTimeIntervalMicroSeconds32& aToneOffLength,
								 TTimeIntervalMicroSeconds32& aPauseLength);

	/**
	* Defines the period over which the volume level is to rise smoothly
	* from nothing to the normal volume level.
	* The function is only available before playing.
	* @since
	* @param const TTimeIntervalMicroSeconds &aRampDuration The period over
	*        which the volume is to rise. A zero value causes the tone
	*        sample to be played at the normal level for the full duration
	*        of the playback. A value, which is longer than the duration of
	*        the tone sample means that the sample never reaches its normal
	*        volume level.
	* @return KErrNone if successful, system wide error otherwise
	*/
	IMPORT_C TInt SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	/**
	* Defines the priority settings that should be used for this instance.
	* @since
	* @param const TMMFPrioritySettings &aPrioritySettings A class type
	*        representing the client's priority, priority preference and
	*        state
	* @return KErrNone if successful, system wide error otherwise
	*/
	IMPORT_C TInt SetPrioritySettings(
				  const TMMFPrioritySettings& aPrioritySettings);

	/**
	* Retrieves a custom interface to the device.
	* @since
	* @param TUid aInterfaceId The interface UID, defined with the custom
	*        interface.
	* @return TAny* A pointer to the interface implementation, or NULL if
	*        the device does not implement the interface requested. The
	*        return value must be cast to the correct type by the user.
	*/
	IMPORT_C TAny* CustomInterface(TUid aInterfaceId);

	/**
	* Returns the number of available pre-defined tone sequences.
	* This is the number of fixed sequence supported by DevSound by default.
	* @since
	* @return TInt  The fixed sequence count. This value is implementation
	*        dependent.
	*/
	IMPORT_C TInt FixedSequenceCount();

	/**
	* Returns the name assigned to a specific pre-defined tone sequence.
	* This is the number of the fixed sequence supported by DevSound by
	* default.
	* The function raises a panic if sequence number specified is invalid.
	* @since
	* @param TInt aSequenceNumber The index identifying the specific
	*        pre-defined tone sequence. Index values are relative to zero.
	*        This can be any value from zero to the value returned by a call
	*        to CMdaAudioPlayerUtility::FixedSequenceCount() - 1. The
	*        function raises a panic if sequence number is not within this
	*        range.
	* @return const TDesC & A reference to a Descriptor containing the fixed
	*        sequence name indexed by aSequenceNumber.
	*/
	IMPORT_C const TDesC& FixedSequenceName(TInt aSequenceNumber);

	/**
	* Returns a list of the supported input datatypes that can be sent to
	* DevSound for playing audio. The datatypes returned are those that the
	* DevSound supports given the priority settings passed in
	* aPrioritySettings. Note that if no supported data types are found this
	* does not constitute failure, the function will return normally with no
	* entries in aSupportedDataTypes.
	* @since
	* @param RArray< TFourCC > &aSupportedDataTypes The array of supported
	*        data types that will be filled in by this function. The
	*        supported data types of the DevSound are in the form of an
	*        array of TFourCC codes. Any existing entries in the array will
	*        be overwritten on calling this function. If no supported data
	*        types are found given the priority settings, then the
	*        aSupportedDatatypes array will have zero entries.
	* @param const TMMFPrioritySettings &aPrioritySettings The priority
	*        settings used to determine the supported datatypes. Note this
	*        does not set the priority settings. For input datatypes the
	*        iState member of the priority settings would be expected to be
	*        either EMMFStatePlaying or EMMFStatePlayingRecording. The
	*        priority settings may effect the supported datatypes depending
	*        on the audio routing.
	* @return void
	*/
	IMPORT_C void GetSupportedInputDataTypesL(
				  RArray<TFourCC>& aSupportedDataTypesconst,
				  const TMMFPrioritySettings& aPrioritySettings) const;

	/**
	* Returns a list of the supported output dataypes that can be received
	* from DevSound for recording audio. The datatypes returned are those
	* that the DevSound supports given the priority settings passed in
	* aPrioritySettings. Note that if no supported data types are found this
	* does not constitute failure, the function will return normally with no
	* entries in aSupportedDataTypes.
	* @since
	* @param RArray< TFourCC > &aSupportedDataTypes The array of supported
	*        data types that will be filled in by this function. The
	*        supported datatypes of the DevSound are in the form of an array
	*        of TFourCC codes. Any existing entries in the array will be
	*        overwritten on calling this function. If no supported datatypes
	*        are found given the priority settings, then the
	*        aSupportedDatatypes array will have zero entries.
	* @param const TMMFPrioritySettings &aPrioritySettings The priority
	*        settings used to determine the supported data types. Note this
	*        does not set the priority settings. For output data types the
	*        iState member of the priority settings would expected to be
	*        either EMMFStateRecording or EMMFStatePlayingRecording. The
	*        priority settings may effect the supported datatypes depending
	*        on the audio routing.
	* @return void
	*/
	IMPORT_C void GetSupportedOutputDataTypesL(
				  RArray<TFourCC>& aSupportedDataTypes,
				  const TMMFPrioritySettings& aPrioritySettings) const;

	/**
	* Sets client configuration
	* @since
    * @param aActualProcessId the effective devsound client (supplied with SetClientThreadInfo)
    * @param aProcessId the client of DevSound
	* @return KErrNone if successful, system wide error otherwise
	*/
    IMPORT_C TInt SetClientConfig(const TProcessId& aProcessId);
    IMPORT_C TInt SetClientConfig(const TProcessId& aActualProcessId, const TProcessId& aProcessId);

	/**
	* Empties the buffers below DevSound without deleting the codec.
	* @since
	* @return KErrNone if successful, otherwise system-wide error code.
	*/
	IMPORT_C TInt EmptyBuffers();

	/*
	* Make sure any deletions is commited
	* @since
	* @
	*
	*/
	IMPORT_C TBool CloseDevSound();

	/**
	* Called when a ProcessingFinished callback is received
	* @since
	* @param aAsyncCompletion Returns ETrue to say the call is asynchronous, with result sent
	*			to AsynchronousOperationComplete()
	* @return an error code KErrNone if successful
	*/
	IMPORT_C TInt ProcessingFinishedReceived(TBool& aAyncCompletion);
	
	IMPORT_C TInt ProcessingError(TBool& aAyncCompletion);
	
	//provides interface for register a notification event
	IMPORT_C TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	//provides interface to cancel the registered notification
	IMPORT_C TInt CancelRegisterAsClient(TUid aEventType);
	//Gets the notification data for client to resume
	IMPORT_C TInt GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData);
	//waits for the client to resume playback even after the timeout expires
	IMPORT_C TInt WillResumePlay();

	/**
	* Gets the current play time from the audio renderer
	* @since 
	* @param TTimeIntervalMicroSeconds& aTime On return contains the current play time
	* @return an error code KErrNone if successful
	*/
	IMPORT_C TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime);

	/**
	* Queries if the low layers does support resume operation.
	* @since
	* @return TBool ETrue if Resume is supported
	*               EFalse otherwise
	*/
	IMPORT_C TBool IsResumeSupported();
	
	/**
	* Resume the operation (Play, Record, TonePlay) temporarily paused .
	* @since
	* @return TInt KErrNone if succesful
	*              KErrNotSupported if the operation is not supported by this implementation
	*/
	IMPORT_C TInt Resume();

	/*
	Used to send a stop call when error in buffer
	*/
	IMPORT_C void BufferErrorEvent();
	
	/**
	* Used rollback the adapror active state to the previous state prior a Commit call
	* @return void
	*/
	IMPORT_C void RollbackAdaptorActiveStateToBeforeCommit();

protected:

	// So that nobody can extend
	CMMFDevSoundAdaptation();

	// Second phase constructor
	void ConstructL(MDevSoundAdaptationObserver& aDevSoundObserver,
				MGlobalProperties& aGlobalProperties);

protected:  // Data
	// Actual implementation class.
	class CBody;

	//DevSoundAdaptation body implementation
	CBody* iBody;
	};

#endif      // MMFDEVSOUNDADAPTATION

// End of File
