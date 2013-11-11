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



#ifndef RMMFDEVSOUNDPROXY_H
#define RMMFDEVSOUNDPROXY_H

//  INCLUDES
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <mmf/common/mcustomcommand.h>
#include "mmfdevsoundcallbackhandler.h"
#include "mmfaudioserverproxy.h"

_LIT(KMMFDevSoundProxyPanicCategory, "MmfDevSoundProxy");

enum TMMFDevSoundProxyPanicCodes
	{
	EMMFDevSoundProxyPlayDataWithoutInitialize,
	EMMFDevSoundProxyRecordDataWithoutInitialize,
	EMMFDevSoundProxyInitCompleteInWrongState,
	EMMFDevSoundProxyToneFinishedInWrongState,
	EMMFDevSoundProxyPlayErrorInWrongState,
	EMMFDevSoundProxyRecordErrorInWrongState,
	EMMFDevSoundProxyUnexpectedConvError,
	EMMFDevSoundProxyCapabilitiesInWrongState,
	EMMFDevSoundProxyConfigInWrongState,
	EMMFDevSoundProxyBTBFInWrongState,
	EMMFDevSoundProxyBTBEInWrongState,
	EMMFDevSoundProxyPlayDataInWrongState,
	EMMFDevSoundProxyRecordDataInWrongState,
	EMMFDevSoundProxyNonEmptyLastBuffer,
	};


// FORWARD DECLARATIONS
class RMMFDevSoundProxy; // declared here.

// CLASS DECLARATION

/**
*  Main interface to DevSound server.
*
*  @lib MmfDevSoundProxy.lib
*  @since 
*/
NONSHARABLE_CLASS( RMMFDevSoundProxy ) : public RMmfSessionBase, 
										 public MCustomCommand,
										 public MDevSoundObserver,
										 public MMMFDevSoundCancelInitialize
	{
public:  // Constructors and destructor
	/**
	* Constructor.
	* @since 
	*/
	IMPORT_C RMMFDevSoundProxy();

public: // New functions

	/**
	* Open a DevSound server session
	* @since 
	* @return KErrNone if successfull, else corresponding error code
	*/
	IMPORT_C TInt Open();
	
	/**
	* Post open - called following successful open to complete open process.
	* Allows async opening
	* @return KErrNone successful, otherwise one of the standard system errors
	*/
	IMPORT_C TInt PostOpen(); 

	/**
	* Launch DevSound instances that might have been waiting for audio
	* policy.
	* @since 
	* @return KErrNone if successfull, else corresponding error code
	*/
	IMPORT_C TInt SetDevSoundInfo();

	/**
	* Initialize DevSound for the mode aMode.
	* Leaves on failure.
	* @since 
	* @param TMMFState aMode The mode for which this object will be used
	* @param aDevSoundCIObserver Observer which will receive Custom Interface events
	* 
	*/
	IMPORT_C void InitializeL(MDevSoundObserver& aDevSoundObserver,
	                          TMMFState aMode,
	                          MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver);

	/**
	* Initializes DevSound object for the mode aMode for processing audio
	* data with hardware device aHWDev.
	* Leaves on failure.
	* @since 
	* @param TUid aHWDev The CMMFHwDevice implementation identifier.
	* @param TMMFState aMode The mode for which this object will be used
	* @param aDevSoundCIObserver Observer which will receive Custom Interface events
	* 
	*/
	IMPORT_C void InitializeL(MDevSoundObserver& aDevSoundObserver,
	                          TUid aHWDev,
	                          TMMFState aMode,
	                          MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver);

	/**
	* Initializes DevSound object for the mode aMode for processing audio
	* data with hardware device supporting FourCC aDesiredFourCC.
	* Leaves on failure.
	* @since 
	* @param TFourCC aDesiredFourCC The CMMFHwDevice implementation FourCC
	*        code.
	* @param TMMFState aMode The mode for which this object will be used
	* @param aDevSoundCIObserver Observer which will receive Custom Interface events
	* 
	*/
	IMPORT_C void InitializeL(MDevSoundObserver& aDevSoundObserver,
	                          TFourCC aDesiredFourCC,
	                          TMMFState aMode,
	                          MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver);

	/**
	* Returns the supported Audio settings ie. encoding, sample rates,
	* mono/stereo operation, buffer size etc..
	* @since 
	* @return TMMFCapabilities The device settings.
	*/
	IMPORT_C TMMFCapabilities Capabilities();

	/**
	* Returns the current device configuration.
	* @since 
	* @return TMMFCapabilities The device settings.
	*/
	IMPORT_C TMMFCapabilities Config();

	/**
	* Configure CMMFDevSound object with the settings in aConfig. Use this
	* to set sampling rate, encoding and mono/stereo.
	* Leaves on failure.
	* @since 
	* @param const TMMFCapabilities& aConfig The attribute values to which
	*        CMMFDevSound object will be configured to.
	* @return KErrNone if successfull, else corresponding error code
	*/
	IMPORT_C void SetConfigL(const TMMFCapabilities& aConfig);

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
	* can be changed before or during playback and is effective immediately
	* @since 
	* @param TInt aVolume The volume setting. This can be any value from 0
	*        to the value returned by a call to
	*        CMMFDevSound::MaxVolume(). If the volume is not
	*        within this range, the volume is automatically set
	*        to minimum or maximum value based on the value
	*        that is being passed. Setting a zero value mutes
	*        the sound. Setting the maximum value results in
	*        the loudest possible sound.
	* @return KErrNone if successfull, else corresponding error code
	*/
	IMPORT_C TInt SetVolume(TInt aVolume);

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
	* @return KErrNone if successfull, else corresponding error code
	*/
	IMPORT_C TInt SetGain(TInt aGain);

	/**
	* Returns the speaker balance set for playing.
	* Leaves on failure.
	* @since 
	* @param TInt &aLeftPercentage On return contains the left speaker
	*        volume percentage.
	* @param TInt &aRightPercentage On return contains the right speaker
	*        volume percentage.
	*/
	IMPORT_C void GetPlayBalanceL(TInt& aLeftPercentage,
	                              TInt& aRightPercentage);

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
	*/
	IMPORT_C void SetPlayBalanceL(TInt aLeftPercentage,
	                              TInt aRightPercentage);

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
	IMPORT_C void GetRecordBalanceL(TInt& aLeftPercentage,
	                                TInt& aRightPercentage);

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
	* @return void
	*/
	IMPORT_C void SetRecordBalanceL(TInt aLeftPercentage,
	                                TInt aRightPercentage);

	/**
	* Close the server session
	* @since 
	* @return void
	*/
	IMPORT_C void Close();

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
	* @return void
	*/
	IMPORT_C void Stop();

	/**
	* Temporarily Stops the ongoing operation (Play, Record, TonePlay).
	* @since 
	* @return void
	*/
	IMPORT_C void Pause();

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
	IMPORT_C void PlayToneL(TInt aFrequency,
	                        const TTimeIntervalMicroSeconds& aDuration);

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
	IMPORT_C void PlayDualToneL(
	              TInt aFrequencyOne,
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
	* @return void
	*/
	IMPORT_C void SetDTMFLengths(
	              TTimeIntervalMicroSeconds32& aToneOnLength,
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
	* @return void
	*/
	IMPORT_C void SetVolumeRamp(
	              const TTimeIntervalMicroSeconds& aRampDuration);

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
	              RArray<TFourCC>& aSupportedDataTypes,
	              const TMMFPrioritySettings& aPrioritySettings);

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
	              const TMMFPrioritySettings& aPrioritySettings);

	/**
	* Returns the number samples recorded so far.
	* @since 
	* @return TInt The samples recorded.
	*/
	IMPORT_C TInt SamplesRecorded();

	/**
	* Returns the number samples played so far.
	* @since 
	* @return TInt The samples played.
	*/
	IMPORT_C TInt SamplesPlayed();

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
	* @return void
	*/
	IMPORT_C void SetToneRepeats(
	              TInt aRepeatCount,
	              const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);

	/**
	* Defines the priority settings that should be used for this instance.
	* @since 
	* @param const TMMFPrioritySettings &aPrioritySettings A class type
	*        representing the client's priority, priority preference and
	*        state
	* @return void
	*/
	IMPORT_C void SetPrioritySettings(
	              const TMMFPrioritySettings& aPrioritySettings);

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
	* Returns data buffer from the DevSound server for playback.
	* @since 
	* @param TMMFDevSoundProxyHwBufPckg& aSetPckg A reference to pckg to
	*        receive buffer information.
	* @return KErrNone if successfull, otherwise a corresponding error code
	*/

	IMPORT_C TInt BufferToBeFilledData(
			TBool aRequestChunk, TMMFDevSoundProxyHwBufPckg& aSetPckg);

	/**
	* Returns data buffer from the DevSound server for recording.
	* @since 
	* @param TMMFDevSoundProxyHwBufPckg& aSetPckg A reference to pckg to
	*        receive buffer information.
	* @return KErrNone if successfull, otherwise a corresponding error code
	*/
	IMPORT_C TInt BufferToBeEmptiedData(TMMFDevSoundProxyHwBufPckg& aSetPckg);
	IMPORT_C TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	IMPORT_C TInt CancelRegisterAsClient(TUid aEventType);
	IMPORT_C TInt GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData);
	IMPORT_C TInt WillResumePlay();
	IMPORT_C TInt EmptyBuffers();
	IMPORT_C TInt CancelInitialize();
	IMPORT_C TInt SetClientThreadInfo(TThreadId& aTid);

	/**
	* Sends a custom command synchronously to the DevSound server. This
	* method will not return until the server has serviced the command.
	* @since 
	* @param   "const TMMFMessageDestinationPckg& aDestination"
	*          The destination of the custom command. This consists of the
	*          unique ID of the interface of command handler.
	* @param   "TInt aFunction"
	*          The number of the function to be called on the command
	*          handler.
	* @param   "const TDesC8& aDataTo1"
	*          A reference to data to be copied to the command handler.
	*          The exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "const TDesC8& aDataTo2"
	*          A reference to data to be copied to the command handler.
	*          The exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "TDes8& aDataFrom"
	*          A reference to an area of memory to which the command handler
	*          will write any data to be passed back to the client. Cannot
	*          be NULL.
	* @return   "TInt"
	*          The result of the custom command. The exact range of values
	*          is dependent on the command handler interface.
	*/
	IMPORT_C TInt CustomCommandSync(
	              const TMMFMessageDestinationPckg&  aDestination,
	              TInt aFunction,
	              const TDesC8& aDataTo1,
	              const TDesC8& aDataTo2,
	              TDes8& aDataFrom);

	/**
	* Sends a custom command synchronously to the DevSound server. This
	* method will not return until the server has serviced the command.
	* @since 
	* @param   "const TMMFMessageDestinationPckg& aDestination"
	*          The destination of the custom command. This consists of the
	*          unique ID of the interface of command handler.
	* @param   "TInt aFunction"
	*          The number of the function to be called on the command
	*          handler.
	* @param   "const TDesC8& aDataTo1"
	*          A reference to data to be copied to the command handler.
	*          The exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "const TDesC8& aDataTo2"
	*          A reference to data to be copied to the command handler.
	*          The exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @return   "TInt"
	*          The result of the custom command. The exact range of values
	*          is dependent on the command handler interface.
	*/
	IMPORT_C TInt CustomCommandSync(
	              const TMMFMessageDestinationPckg&  aDestination,
	              TInt aFunction,
	              const TDesC8& aDataTo1,
	              const TDesC8& aDataTo2);

	/**
	* Sends a custom command asynchronously to the DevSound server.
	* Note: This method will return immediately. The ::RunL() of the active
	* object owning the aStatus parameter will be called when the command
	* is completed by the command handler.
	* @since 
	* @param   "const TMMFMessageDestinationPckg& aDestination"
	*          The destination of the custom command. This consists of the
	*          unique ID of the interface of command handler.
	* @param   "TInt aFunction"
	*          The number of the function to be called on the command
	*          handler.
	* @param   "const TDesC8& aDataTo1"
	*          A reference to data to be copied to the command handler.
	*          The exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "const TDesC8& aDataTo2"
	*          A reference to data to be copied to the command handler.
	*          The exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "TDes8& aDataFrom"
	*          A reference to an area of memory to which the command handler
	*          will write any data to be passed back to the client. Cannot
	*          be NULL.
	* @param   "TRequestStatus& aStatus"
	*          The TRequestStatus of an active object. This will contain the
	*          result of the custom command on completion. The exact range
	*          of result values is dependent on the custom command
	*          interface.
	*/
	IMPORT_C void CustomCommandAsync(
	              const TMMFMessageDestinationPckg& aDestination,
	              TInt aFunction,
	              const TDesC8& aDataTo1,
	              const TDesC8& aDataTo2,
	              TDes8& aDataFrom,
	              TRequestStatus& aStatus);

	/**
	* Sends a custom command asynchronously to the DevSound server.
	* Note: This method will return immediately. The ::RunL() of the active
	* object owning the aStatus parameter will be called when the command
	* is completed by the command handler.
	* @since 
	* @param   "const TMMFMessageDestinationPckg& aDestination"
	*          The destination of the custom command. This consists of the
	*          unique ID of the interface of command handler.
	* @param   "TInt aFunction"
	*          The number of the function to be called on the command
	*          handler.
	* @param   "const TDesC8& aDataTo1"
	*          A reference to data to be copied to the command handler. The
	*          exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "const TDesC8& aDataTo2"
	*          A reference to data to be copied to the command handler. The
	*          exact contents of the data are dependent on the command
	*          hanlder on the DevSound server. Can be NULL.
	* @param   "TRequestStatus& aStatus"
	*          The TRequestStatus of an active object. This will contain the
	*          result of the custom command on completion. The exact range
	*          of result values is dependent on the custom command
	*          interface.
	*/
	IMPORT_C void CustomCommandAsync(
	              const TMMFMessageDestinationPckg& aDestination,
	              TInt aFunction,
	              const TDesC8& aDataTo1,
	              const TDesC8& aDataTo2,
	              TRequestStatus& aStatus);

	IMPORT_C TInt SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam);
	IMPORT_C void AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam);
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

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);

private:
	void StartReceivingMsgQueueHandlerEventsL(MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver);

private:    // Data

	TMMFDevSoundProxySettingsPckg iDspsPckg;
	CMMFDataBuffer* iBuffer; //Not Owned
	HBufC* iSeqName;

	TMMFMessageDestinationPckg iDestinationPckg;

	enum TState
		{
		EIdle = 0,
		EInitializing,
		EInitialized,
		EPlaying,
		EPlayingBufferWait,
		ETonePlaying, 
		ERecording,
		ERecordingBufferWait,
		ERecordingInLastBufferCycle,
		ERecordingResumingInLastBufferCycle,
		};

	enum TTonePlayingMode
		{
		ESimple = 0,
		EDual,
		EDTMFString,
		ESequence,
		EFixedSequence 
		};

	// Allows distinguish for operations supported for some tones
	TTonePlayingMode iToneMode;
	// Contains state information of this DLL instance. Allows client-size state checking.
	TState iState;
	// Reference to the devsound proxy object
	RMMFAudioServerProxy* iAudioServerProxy;
	// Reference to observer where the events need to be forwarded to.
	MDevSoundObserver* iDevSoundObserver;
	// Message queue
	RMsgQueue<TMMFDevSoundQueueItem> iMsgQueue;
	// Reference to the message queue handler
	CMsgQueueHandler* iMsgQueueHandler;
	TMMFMessageDestinationPckg iCustIntPckg;
	};

#endif // RMMFDEVSOUNDPROXY_H

// End of File
