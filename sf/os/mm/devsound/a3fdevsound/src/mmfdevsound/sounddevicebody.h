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



#ifndef SOUNDDEVICEBODY_H
#define SOUNDDEVICEBODY_H

//  INCLUDES
#include <mmf/server/sounddevice.h>
#include "mmfdevsoundproxy.h"
#include <mmf/server/mmfdevsoundcustominterfacesupport.h>
#include <mmf/server/mmfdevsoundcustominterface.h>
#include <a3f/mmfdevsoundcustominterfaceextensions.h>
#include "MmfDevSoundCIMuxUtility.h"

/**
*  This is the Body for CMMFDevSound.
*
*  @lib MMFDevSound.lib
*/

NONSHARABLE_CLASS( CMMFDevSound::CBody ) : public CBase,
										   public MMMFDevSoundCancelInitialize,
										   public MMMFDevSoundCustomInterfaceChannel,
										   public MMMFDevSoundCustomInterfaceObserver,
										   public MMMFDevSoundEmptyBuffers,
										   public MAutoPauseResumeSupport,
										   public MMMFDevSoundTimePlayed,
										   public MMMFDevSoundQueryIgnoresUnderflow,
										   public MAudioClientThreadInfo, 
										   public MMMFDevSoundTruePause

	{
public:  // Constructors and destructor

	/**
	* Factory function - returning instance of the class.
	*/
	static CBody* NewL();

	/**
	* Destructor.

	* Deletes all objects and releases all resources owned by this
	* instance.
	*/
	~CBody();

	/**
	* Initializes to raw audio data PCM16 and Sampling Rate of 8 KHz.
	* On completion of Initialization, calls InitializeComplete() on
	* aDevSoundObserver.
	* @param aDevSoundObserver. A reference to DevSound observer instance.
	* @param aMode. Mode for which this object will be used.
	*/
	void InitializeL(MDevSoundObserver& aDevSoundObserver,
	                 TMMFState aMode);

	/**
	* Initializes DevSound object for the mode aMode for processing audio
	* data with hardware device supporting FourCC aDesiredFourCC.
	* @param  aDevSoundObserver. A reference to DevSound observer instance.
	* @param  aDesiredFourCC. The CMMFHwDevice implementation FourCC code.
	* @param  aMode. The mode for which this object will be used
	*/
	void InitializeL(MDevSoundObserver& aDevSoundObserver,
	                 TFourCC aDesiredFourCC,
	                 TMMFState aMode);

	/**
	* Returns the supported Audio settings ie. encoding, sample rates,
	* mono/stereo operation, buffer size etc..
	* @return TMMFCapabilities. The device settings.
	*/
	inline TMMFCapabilities Capabilities();

	/**
	* Returns the current device configuration.
	* @return TMMFCapabilities. The device settings.
	*/
	inline TMMFCapabilities Config() const;

	/**
	* Configure CMMFDevSound object with the settings in aConfig. Use this
	* to set sampling rate, encoding and mono/stereo.
	* @param aConfig. The attribute values to which CMMFDevSound object will be configured to.
	*/
	inline void SetConfigL(const TMMFCapabilities& aCaps);

	/**
	* Returns an integer representing the maximum volume device supports.
	* This is the maximum value which can be passed to
	* CMMFDevSound::SetVolume.
	* @return TInt The maximum volume. This value is platform dependent but is always greater than or equal to one.
	*/
	inline TInt MaxVolume();

	/**
	* Returns an integer representing the current volume.
	* @return TInt The current volume level.
	*/
	inline TInt Volume();

	/**
	* Changes the current playback volume to a specified value. The volume
	* can be changed before or during playback and is effective immediately.
	* @param aVolume. The volume setting. This can be any value from 0
	*        to the value returned by a call to
	*        CMMFDevSound::MaxVolume(). If the volume is not
	*        within this range, the volume is automatically set
	*        to minimum or maximum value based on the value
	*        that is being passed. Setting a zero value mutes
	*        the sound. Setting the maximum value results in
	*        the loudest possible sound.
	*/
	inline void SetVolume(TInt aVolume);

	/**
	* Returns an integer representing the maximum gain the device supports.
	* This is the maximum value which can be passed to CMMFDevSound::SetGain
	* @return TInt The maximum gain. This value is platform dependent but is
	*        always greater than or equal to one.
	*/
	inline TInt MaxGain();

	/**
	* Returns an integer representing the current gain.
	* @return TInt The current gain level.
	*/
	inline TInt Gain();

	/**
	* Changes the current recording gain to a specified value. The gain can
	* be changed before or during recording and is effective immediately.
	* @param aGain. The gain setting. This can be any value from zero to
	*        the value returned by a call to
	*        CMMFDevSound::MaxGain(). If the volume
	*        is not within this range, the gain is automatically
	*        set to minimum or maximum value based on the value
	*        that is being passed. Setting a zero value mutes the
	*        sound. Setting the maximum value results in the
	*        loudest possible sound.
	*/
	inline void SetGain(TInt aGain);

	/**
	* Returns the speaker balance set for playing.
	* @param aLeftPercentage. On return contains the left speaker volume percentage.
	* @param aRightPercentage. On return contains the right speaker volume percentage.
	*/
	inline void GetPlayBalanceL(TInt& aLeftPercentage,TInt& aRightPercentage);

	/**
	* Sets the speaker balance for playing. The speaker balance can be
	* changed before or during playback and is effective immediately.
	* @param aLeftPercentage. The left speaker volume percentage. This
	*        can be any value from zero to 100. Setting
	*        a zero value mutes the sound on left
	*        speaker.
	* @param aRightPercentage. The right speaker volume percentage.
	*        This can be any value from zero to 100.
	*        Setting a zero value mutes the sound on
	*        right speaker.
	*/
	inline void SetPlayBalanceL(TInt aLeftPercentage,
	                            TInt aRightPercentage);

	/**
	* Returns the microphone gain balance set for recording.
	* @param aLeftPercentage On return contains the left microphone
	*        gain percentage.
	* @param aRightPercentage On return contains the right microphone
	*        gain percentage.
	*/
	inline void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	* Sets the microphone balance for recording. The microphone balance can
	* be changed before or during recording and is effective immediately.
	* @param aLeftPercentage. The left microphone gain percentage. This
	*        can be any value from zero to 100. Setting
	*        a zero value mutes the sound from left
	*        microphone.
	* @param aRightPercentage. The right microphone gain percentage.
	*        This can be any value from zero to 100.
	*        Setting a zero value mutes the sound from
	*        right microphone.
	*/
	inline void SetRecordBalanceL(TInt aLeftPercentage,
	                              TInt aRightPercentage);

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
	*/
	inline void PlayInitL();

	/**
	* Initializes the audio device and starts the record process. This
	* function queries and acquires the audio policy before initializing
	* audio device. If there was an error during policy initialization,
	* RecordError() function will be called on the observer with error code
	* KErrAccessDenied, otherwise BufferToBeEmptied() function will be
	* called with a buffer reference. This buffer contains recorded or
	* encoded data. After processing data in the buffer reference passed,
	* the client should call RecordData() to continue recording process.
	* The amount of data that is available is specified in
	* CMMFBuffer::RequestSize().
	*/
	inline void RecordInitL();

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
	*/
	inline void PlayData();

	/**
	* Contine the process of recording.
	* Once the buffer is filled with recorded data, the Observer gets a
	* reference to the buffer along with the callback function
	* BufferToBeEmptied(). After processing the buffer (copying over to a
	* different buffer or writing to file) the client should call this
	* function to continue the recording process.
	*/
	inline void RecordData();

	/**
	* Stops the ongoing operation (Play, Record, TonePlay).
	*/
	inline void Stop();

	/**
	* Temporarily Stops the ongoing operation (Play, Record, TonePlay).
	*/
	inline void Pause();

	/**
	* Returns the Sample recorded so far
	* @return TInt Returns the samples recorded.
	*/
	inline TInt SamplesRecorded();

	/**
	* Returns the Sample played so far
	* @return TInt Returns the samples played.
	*/
	inline TInt SamplesPlayed();

	/**
	* Initializes the audio device and starts playing a tone. The tone is
	* played with the frequency and duration specified.
	* Leaves on failure.
	* @param aFrequency. The frequency at which the tone will be played.
	* @param const TTimeIntervalMicroSeconds &aDuration The period over
	*        which the tone will be played. A zero value causes the no tone
	*        to be played.
	*/
	inline void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);

	/**
	* Initializes audio device and starts playing a dual tone. Dual Tone is
	* played with the specified frequencies and for the specified duration.
	* @param aFrequencyOne The first frequency of dual tone.
	* @param aFrequencyTwo The second frequency of dual tone.
	* @param aDuration The period over which the tone will be played. 
	* A zero value causes the no tone to be played.
	*/
	inline void PlayDualToneL(
	            TInt aFrequencyOne,
	            TInt aFrequencyTwo,
	            const TTimeIntervalMicroSeconds& aDuration);

	/**
	* Initializes the audio device and starts playing the DTMF string
	* aDTMFString.
	* @param aDTMFString The DTMF sequence in a descriptor.
	*/
	inline void PlayDTMFStringL(const TDesC& aDTMFString);

	/**
	* Initializes the audio device and starts playing a tone sequence.
	* @param aData The tone sequence in a descriptor.
	*/
	inline void PlayToneSequenceL(const TDesC8& aData);

	/**
	* Defines the number of times the audio is to be repeated during the
	* tone playback operation. A period of silence can follow each playing
	* of a tone. The tone playing can be repeated indefinitely
	* @param aRepeatCount. The number of times the tone, together with
	*        the trailing silence, is to be repeated. If this is set to
	*        KMdaRepeatForever, then the tone, together with the trailing
	*        silence, is repeated indefinitely or until Stop() is called.
	*        If this is set to zero, then the tone is not repeated.
	* @param aRepeatTrailingSilence An interval of silence which will be played after each tone.
	*        Supported only during tone playing.
	*/
	inline void SetToneRepeats(
	            TInt aRepeatCount,
	            const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);

	/**
	* Defines the duration of tone on, tone off and tone pause to be used
	* during the DTMF tone playback operation.
	* Supported only during tone playing.
	* @param aToneOnLength The period over which the tone will be played.
	* If this is set to zero, then the tone is not played.
	* @param aToneOffLength The period over which the no tone will be played.
	* @param aPauseLength The period over which the tone playing will be paused.
	*/
	inline void SetDTMFLengths(
	            TTimeIntervalMicroSeconds32& aToneOnLength,
	            TTimeIntervalMicroSeconds32& aToneOffLength,
	            TTimeIntervalMicroSeconds32& aPauseLength);

	/**
	* Defines the period over which the volume level is to rise smoothly
	* from nothing to the normal volume level.
	* The function is only available before playing.
	* @param aRampDuration The period over which the volume is to rise. 
	* 	 A zero value causes the tone sample to be played at the 
	* 	 normal level for the full duration of the playback. 
	* 	 A value, which is longer than the duration of
	*        the tone sample means that the sample never reaches its normal
	*        volume level.
	*/
	inline void SetVolumeRamp(
	            const TTimeIntervalMicroSeconds& aRampDuration);

	/**
	* Defines the priority settings that should be used for this instance.
	* @param aPrioritySettings. A class type representing the client's priority, 
	* priority preference and state
	*/
	inline void SetPrioritySettings(
	            const TMMFPrioritySettings& aPrioritySettings);

	/**
	* Retrieves a custom interface to the device.
	* @param aInterfaceId. The interface UID, defined with the custom
	*        interface.
	* @return TAny* A pointer to the interface implementation, or NULL if
	*        the device does not implement the interface requested. The
	*        return value must be cast to the correct type by the user.
	*/
	TAny* CustomInterface(TUid aInterfaceId);

	/**
	* Returns a list of the supported input datatypes that can be sent to
	* DevSound for playing audio. The datatypes returned are those that the
	* DevSound supports given the priority settings passed in
	* aPrioritySettings. Note that if no supported data types are found this
	* does not constitute failure, the function will return normally with no
	* entries in aSupportedDataTypes.
	* @param aSupportedDataTypes The array of supported
	*        data types that will be filled in by this function. The
	*        supported data types of the DevSound are in the form of an
	*        array of TFourCC codes. Any existing entries in the array will
	*        be overwritten on calling this function. If no supported data
	*        types are found given the priority settings, then the
	*        aSupportedDatatypes array will have zero entries.
	* @param aPrioritySettings The priority settings used to determine the 
	* 	 supported datatypes. Note this does not set the priority settings. 
	* 	 For input datatypes the iState member of the priority settings would 
	* 	 be expected to be either EMMFStatePlaying or EMMFStatePlayingRecording.
	* 	 The priority settings may effect the supported datatypes depending
	*        on the audio routing.
	*/
	inline void GetSupportedInputDataTypesL(
	            RArray<TFourCC>& aSupportedDataTypesconst,
	            const TMMFPrioritySettings& aPrioritySettings) const;

	/**
	* Returns a list of the supported output dataypes that can be received
	* from DevSound for recording audio. The datatypes returned are those
	* that the DevSound supports given the priority settings passed in
	* aPrioritySettings. Note that if no supported data types are found this
	* does not constitute failure, the function will return normally with no
	* entries in aSupportedDataTypes.
	* @param aSupportedDataTypes. The array of supported
	*        data types that will be filled in by this function. The
	*        supported datatypes of the DevSound are in the form of an array
	*        of TFourCC codes. Any existing entries in the array will be
	*        overwritten on calling this function. If no supported datatypes
	*        are found given the priority settings, then the
	*        aSupportedDatatypes array will have zero entries.
	* @param aPrioritySettings. The priority settings used to determine the supported 
	*        data types. Note this does not set the priority settings. For output data types the
	*        iState member of the priority settings would expected to be
	*        either EMMFStateRecording or EMMFStatePlayingRecording. The
	*        priority settings may effect the supported datatypes depending
	*        on the audio routing.
	*/
	inline void GetSupportedOutputDataTypesL(
	            RArray<TFourCC>& aSupportedDataTypes,
	            const TMMFPrioritySettings& aPrioritySettings) const;
	            
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

	@param  aEventType. The Event type need to cancel
	@return An error code indicating if the function call was successful. KErrNone on success, KErrNotSupported
			if the event type is not supported otherwise another of the system-wide error codes.
	*/	
	inline TInt CancelRegisterAsClient(TUid aEventType);

	/**
	Returns the Notification data which the client needs to resume playing.

	@param aEventType. The Event type for which to get notification data  
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
	        otherwise another of the system-wide error codes.
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
	
	//from MMMFDevSoundCancelInitialize
	/** 
		
	Cancels the initialization process of a CMMFDevSound object

	@return An error code indicating if the function call was successful. 
			KErrNone on success,
			KerrNotReady if this is called before InitializeL() call or after      
			the object has been initialized 
	*/
	inline TInt CancelInitialize();

	// from MMMFDevSoundCustomInterfaceChannel
	/**
	Implements a synchronous custom command
	@param aUid
		   The UID of the custom command
	@param aParam1
		   A buffer of data to be supplied to the receiver
	@param aParam2
		   A buffer of data to be supplied to the receiver
	@param aOutParam
		   A buffer that will be written into by the receiver
		   and returned to the client
	@return the result of the custom command
	 */
	TInt SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam);

	/**
	Implements an asynchronous custom command
	@param aUid
		   The UID of the custom command
	@param aStatus
		   The request status of the active object that
		   will be called upon the completion of the request
	@param aParam1
		   A buffer of data to be supplied to the receiver
	@param aParam2
		   A buffer of data to be supplied to the receiver
	@param aOutParam
		   A buffer that will be written into by the receiver
		   and returned to the client
	 */
	void AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam);

	//from MMMFDevSoundCustomInterfaceObserver
	void CloseCustomInterface(TInt aInterfaceId);

	//from MMMFDevSoundTimePlayed
	TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime);
	
	//MMMFDevSoundQueryIgnoresUnderflow
	TBool QueryIgnoresUnderflow();
	
	// Set the real client thread with thread Id
	TInt SetClientThreadInfo(TThreadId aTid);

	// from MMMFDevSoundTruePause
	TBool IsResumeSupported();
	TInt Resume();
protected:

	/**
	* Constructor
	*/
	CBody();

	/**
	* Symbian constructor
	*/
	void ConstructL();

	TInt FindCustomInterface(TUid aInterfaceId);

	// custom interface storage
	RArray<TMMFDevSoundCustomInterfaceData> iCustomInterfaceArray;
	CMMFDevSoundCIMuxUtility* iMuxUtility;
	// Reference to DevSound proxy
	RMMFDevSoundProxy* iDevSoundProxy;
	// CI Extension
	MDevSoundCIClientExtension* iCIExtension;
	};

#include "sounddevicebody.inl"

#endif // SOUNDDEVICEBODY_H

// End of File
