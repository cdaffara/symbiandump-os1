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



#ifndef MMFDEVSOUNDADAPTATIONBODY_H
#define MMFDEVSOUNDADAPTATIONBODY_H

#include <e32base.h>
#include <mmf/server/mmfhwdevice.h>
#include <a3f/a3f_trace_utils.h>
#include "mmfdevsoundadaptation.h"
#include <a3f/tonedata.h>
#include <a3f/audioprocessingunittypeuids.h>


class CDevAudio;
class CDevAudioControl;
class MGlobalProperties;

// CONSTANTS
const TInt KDevsoundDefaultMaxGain = 10;
const TUint KDevSoundMaxCaps = 0xFFFFFFFF;

const TUint KDataForPlay = 0;
const TUint KDataForRecord = 1;

/**
 * Class implementing CMMFDevSoundAdaptation behaviour.
 *
 * @lib mmfdevsoundadaptation.lib
 */
class CMMFDevSoundAdaptation::CBody : public CBase
	{
	friend class CMMFDevSoundAdaptation;

public:

	static CBody* NewL(MDevSoundAdaptationObserver& aDevSoundObserver,
						MGlobalProperties& aGlobalProperties);

	~CBody();

public: // New functions

	/*
	* 2nd phase async open operation
	*/
	void PostOpenL();

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
	void InitializeL(TMMFState aMode);

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
	void InitializeL(TUid aHWDev, TMMFState aMode);

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
	void InitializeL(TFourCC aDesiredFourCC,
					TMMFState aMode);

	/**
	* Initializes DevSound object with codec format aFormat
	* Leaves on failure.
	* @since
	* @param aDevSoundObserver A reference to DevSound Observer instance.
	* @param aFormat codec format
	* @param aMode The mode for which this object will be used
	*/
	void DoInitializeL(TUid aFormat,
				TMMFState aMode);

	/**
	* Cancels the initialization of a DevSound object.
	* @since
	* @return An error code indicating if the function call was successful.
	*    	  KErrNone on success,
	*		  KerrNotReady if this is called before InitializeL() call or after
	*		  the object has been initialized,
	*/
	TInt CancelInitialize();

	/**
	* Returns the supported Audio settings ie. encoding, sample rates,
	* mono/stereo operation, buffer size etc..
	* @since
	* @return TMMFCapabilities The device settings.
	*/
	TInt Capabilities(TMMFCapabilities& aCap);

	/**
	* Returns the current device configuration.
	* @since
	* @return TMMFCapabilities The device settings.
	*/
	TMMFCapabilities Config() const;

	/**
	* Configure CMMFDevSound object with the settings in aConfig. Use this
	* to set sampling rate, encoding and mono/stereo.
	* Leaves on failure.
	* @since
	* @param const TMMFCapabilities& aConfig The attribute values to which
	*        CMMFDevSound object will be configured to.
	* @return void
	*/
	void SetConfigL(const TMMFCapabilities& aCaps);

	/**
	* Returns an integer representing the maximum volume device supports.
	* This is the maximum value which can be passed to
	* CMMFDevSound::SetVolume.
	* @since
	* @return TInt The maximum volume. This value is platform dependent but
	*        is always greater than or equal to one.
	*/
	TInt MaxVolume();

	/**
	* Returns an integer representing the current volume.
	* @since
	* @return TInt The current volume level.
	*/
	TInt Volume();

	/**
	* Changes the current playback volume to a specified value. The volume
	* can be changed before or during playback and is effective immediately.
	* @since
	* @param aVolume The volume setting. This can be any value from 0
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
	TInt SetVolume(TInt aVolume, TBool& aAyncCompletion);

	/**
	* Gets the maximum gain the device supports.
	* This is the maximum value which can be passed to CMMFDevSound::SetGain
	* @since
	* @return error
	*/
	TInt MaxGain();

	/**
	* Returns an integer representing the current gain.
	* @since
	* @return TInt The current gain level.
	*/
	TInt Gain();

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
	* @return void
	*/
	TInt SetGain(TInt aGain, TBool& aAyncCompletion);

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
	void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

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
	void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage, TBool& aAsyncCompletion);

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
	void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

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
	void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage, TBool& aAsyncCompletion);

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
	void PlayInitL();

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
	void RecordInitL();

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
	void PlayData();

	/**
	* Contine the process of recording.
	* Once the buffer is filled with recorded data, the Observer gets a
	* reference to the buffer along with the callback function
	* BufferToBeEmptied(). After processing the buffer (copying over to a
	* different buffer or writing to file) the client should call this
	* function to continue the recording process.
	* @return void
	*/
	void RecordData();

	/**
	* Stops the ongoing operation (Play, Record, TonePlay).
	* @since
	* @return KErrNone if successful, system wide error otherwise
	*/
	TBool Stop();

	/**
	* Temporarily Stops the ongoing operation (Play, Record, TonePlay).
	* @since
	* @return KErrNone if successful, system wide error otherwise
	*/
	TInt Pause();

	/**
	* Returns the Sample recorded so far
	* @since
	* @return TInt Returns the samples recorded.
	*/
	TInt SamplesRecorded();

	/**
	* Returns the Sample played so far
	* @since
	* @return TInt Returns the samples played.
	*/
	TInt SamplesPlayed();

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
	void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);

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
	void PlayDualToneL(TInt aFrequencyOne,
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
	void PlayDTMFStringL(const TDesC& aDTMFString);

	/**
	* Initializes the audio device and starts playing a tone sequence.
	* Leaves on failure.
	* @since
	* @param const TDesC8 &aData The tone sequence in a descriptor.
	* @return void
	*/
	void PlayToneSequenceL(const TDesC8& aData);

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
	void PlayFixedSequenceL(TInt aSequenceNumber);

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
	TInt SetToneRepeats(TInt aRepeatCount,
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
	TInt SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
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
	TInt SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	/**
	* Defines the priority settings that should be used for this instance.
	* @since
	* @param const TMMFPrioritySettings &aPrioritySettings A class type
	*        representing the client's priority, priority preference and
	*        state
	* @return KErrNone if successful, system wide error otherwise
	*/
	TInt SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	/**
	* Retrieves a custom interface to the device.
	* @since
	* @param TUid aInterfaceId The interface UID, defined with the custom
	*        interface.
	* @return TAny* A pointer to the interface implementation, or NULL if
	*        the device does not implement the interface requested. The
	*        return value must be cast to the correct type by the user.
	*/
	TAny* CustomInterface(TUid aInterfaceId);

	/**
	* Returns the number of available pre-defined tone sequences.
	* This is the number of fixed sequence supported by DevSound by default.
	* @since
	* @return TInt  The fixed sequence count. This value is implementation
	*        dependent.
	*/
	TInt FixedSequenceCount();

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
	const TDesC& FixedSequenceName(TInt aSequenceNumber);

	/**
	* Returns a list of the supported input datatypes that can be sent to
	* DevSound for playing audio. The datatypes returned are those that the
	* DevSound supports given the priority settings passed in
	* aPrioritySettings. Note that if no supported data types are found this
	* does not constitute failure, the function will return normally with no
	* entries in aSupportedDataTypes.
	* Leaves on failure.
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
	void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypesconst,
								const TMMFPrioritySettings& aPrioritySettings) const;

	/**
	* Returns a list of the supported output dataypes that can be received
	* from DevSound for recording audio. The datatypes returned are those
	* that the DevSound supports given the priority settings passed in
	* aPrioritySettings. Note that if no supported data types are found this
	* does not constitute failure, the function will return normally with no
	* entries in aSupportedDataTypes.
	* Leaves on failure.
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
	void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,
								const TMMFPrioritySettings& aPrioritySettings) const;

	/**
	* Sets client configuration
	* @since
	* @param aActualProcessId the effective devsound client (supplied with SetClientThreadInfo)
	* @param aProcessId the client of DevSound
	* @return KErrNone if successful, system wide error otherwise
	*/
    TInt SetClientConfig(const TProcessId& aProcessId);
    TInt SetClientConfig(const TProcessId& aActualProcessId, const TProcessId& aProcessId);

	/**
	* Return to idle state, preparing for destruction.
	* @since
	* @return void
	*/
	TBool CloseDevSound();

	/**
	* Called when a ProcessingFinished callback is received
	* @param aAsyncCompletion Returns ETrue to say the call is asynchronous, with result sent
	*		to AsynchronousOperationComplete()
	* @return an error code KErrNone if successful
	*/
	TInt ProcessingFinishedReceived(TBool& aAyncCompletion);

	TInt ProcessingError(TBool& aAyncCompletion);
	/**
	* Empties the buffers below DevSound without deleting the codec
	* @since
	* @return KErrNone if successful, otherwise system-wide error code.
	*/
	TInt EmptyBuffers();
	
	//provides interface for register a notification event
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	//provides interface to cancel the registered notification
	TInt CancelRegisterAsClient(TUid aEventType);
	//Gets the notification data for client to resume
	TInt GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData);
	//waits for the client to resume playback even after the timeout expires
	TInt WillResumePlay();
	
	/**
	* Gets the current play time from the audio renderer
	* @since 
	* @param TTimeIntervalMicroSeconds& aTime On return contains the current play time
	* @return an error code KErrNone if successful
	*/
	TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime);

	/**
	* Queries if the low layers does support resume operation.
	* @since
	* @return TBool ETrue if Resume is supported
	*               EFalse otherwise
	*/
	TBool IsResumeSupported();
	
	/**
	* Resume the operation (Play, Record, TonePlay) temporarily paused .
	* @since
	* @return TInt KErrNone if succesful
	*              KErrNotSupported if the operation is not supported by this implementation
	*/
	TInt Resume();
	
	/*
	Used to send a stop call when there is a error in the buffer
	*/
	void BufferErrorEvent();

	/**
	* Used rollback the adapror active state to the previous state prior a Commit call
	* @return void
	*/
	void RollbackAdaptorActiveStateToBeforeCommit();

private:

	CBody(MDevSoundAdaptationObserver& aDevSoundObserver,
		MGlobalProperties& aGlobalProperties);

	void ConstructL(MGlobalProperties& aGlobalProperties);

private: // data

	/**
	* pointer to devsound framework for callbacks
	* Not own.
	*/
    MDevSoundAdaptationObserver& iDevSoundObserver;

	/**
	* holds mode for which current instance is initialized for
	*/
	TMMFState iMode;

	/**
	* holds format for which current instance is initialized for
	*/
	TUid iFormat;

	/**
	* holds information of supported configurations
	* (channels, sampling rates, buffersize...)
	*/
	TMMFCapabilities iMmfCapabilities;

	/**
	* holds information of current device configuration
	* (channels, sampling rates, buffersize...)
	*/
	TMMFCapabilities iMmfConfig;

	/**
	* holds client audio type information
	*/
	TMMFPrioritySettings iPrioritySettings;

	/**
	* pointer to devaudio
	* Own.
	*/
	CDevAudio  *iDevAudio;

	TToneData iToneData;

	// Pointer to global properties
	// Not owned.
	MGlobalProperties*	iGlobalProperties;
	};

#endif		// MMFDEVSOUNDADAPTATIONBODY_H

// End of File
