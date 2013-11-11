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

#ifndef SOUNDDEVICEBODY_INL
#define SOUNDDEVICEBODY_INL

#include <mmf/plugin/devsoundplugin.h>

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes CMMFDevSound::CBody object to play and record PCM16 raw audio data
 *	with sampling rate of 8 KHz.
 *
 *	On completion of Initialization, calls InitializeComplete() on
 *	aDevSoundObserver.
 *
 *	Leaves on failure.
 *
 *	@param	"aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"aMode"
 *			Mode for which this object will be used.
 *
 */
inline void CMMFDevSound::CBody::InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)

	{
	iPlugin->InitializeL(aDevSoundObserver,aMode);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes DevSound object for the mode aMode for processing audio data
 *	with hardware device aHWDev.
 *
 *	On completion of Initialization, the observer will be notified via call back
 *	InitializeComplete().
 *
 *	Leaves on failure.
 *
 *	@param	"aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"aHWDev"
 *			CMMFHwDevice implementation identifier.
 *
 *	@param	"aMode"
 *			Mode for which this object will be used.
 *
 */
inline void CMMFDevSound::CBody::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)
	{
	iPlugin->InitializeL(aDevSoundObserver, aHWDev, aMode);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes DevSound object for the mode aMode for processing audio data
 *	with hardware device supporting FourCC aDesiredFourCC.
 *
 *	On completion of Initialization, the observer will be notified via call back
 *	InitializeComplete().
 *
 *	Leaves on failure.
 *
 *	@param	"aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"aDesiredFourCC"
 *			CMMFHwDevice implementation FourCC.
 *
 *	@param	"aMode"
 *			Mode for which this object will be used.
 *
 */
inline void CMMFDevSound::CBody::InitializeL(MDevSoundObserver& aDevSoundObserver, 
										  TFourCC aDesiredFourCC, TMMFState aMode)
	{
	iPlugin->InitializeL(aDevSoundObserver, aDesiredFourCC, aMode);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the supported Audio settings.
 *
 *	@return	"TMMFCapabilities"
 *			Device settings.
 *
 */
inline TMMFCapabilities CMMFDevSound::CBody::Capabilities()
	{
	return iPlugin->Capabilities();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the current audio settings.
 *
 *	@return	"TMMFCapabilities"
 *			Device settings.
 *
 */
inline TMMFCapabilities CMMFDevSound::CBody::Config() const
	{
	return iPlugin->Config();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Configure CMMFDevSound::CBody object for the settings in aConfig.
 *
 *	Use this to set sampling rate, Encoding and Mono/Stereo.
 *
 *	@param	"aConfig"
 *			Attribute values to which CMMFDevSound::CBody object will be configured to.
 *
 */
inline void CMMFDevSound::CBody::SetConfigL(const TMMFCapabilities& aConfig)
	{
	iPlugin->SetConfigL(aConfig);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns an integer representing the maximum volume.
 *
 *	This is the maximum value which can be passed to CMMFDevSound::CBody::SetVolume.
 *
 *	@return	"TInt"
 *			The maximum volume. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
inline TInt CMMFDevSound::CBody::MaxVolume()
	{
	return iPlugin->MaxVolume();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns an integer representing the current volume.
 *
 *	@return	"TInt"
 *			The current volume level.
 *
 */
inline TInt CMMFDevSound::CBody::Volume()
	{
	return iPlugin->Volume();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Changes the current playback volume to a specified value.
 *
 *	The volume can be changed before or during playback and is effective
 *	immediately.
 *
 *	@param	"aVolume"
 *			The volume setting. This can be any value from zero to the value
 *			returned by a call to CMMFDevSound::CBody::MaxVolume(). If the
 *			volume is not within this range, the volume is automatically set to
 *			minimum or maximum value based on the value that is being passed.
 *			Setting a zero value mutes the sound. Setting the maximum value
 *			results in the loudest possible sound.
 *
 */
inline void CMMFDevSound::CBody::SetVolume(TInt aVolume)
	{
	iPlugin->SetVolume(aVolume);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns an integer representing the maximum gain.
 *
 *	This is the maximum value which can be passed to CMMFDevSound::CBody::SetGain.
 *
 *	@return	"TInt"
 *			The maximum gain. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
inline TInt CMMFDevSound::CBody::MaxGain()
	{
	return iPlugin->MaxGain();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns an integer representing the current gain.
 *
 *	@return	"TInt"
 *			The current gain level.
 *
 */
inline TInt CMMFDevSound::CBody::Gain()
	{
	return iPlugin->Gain();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Changes the current recording gain to a specified value.
 *
 *	The gain can be changed before or during recording and is effective
 *	immediately.
 *
 *	@param	"TInt aGain"
 *			The gain setting. This can be any value from zero to the value
 *			returned by a call to CMMFDevSound::CBody::MaxGain(). If the
 *			volume is not within this range, the gain is automatically set to
 *			minimum or maximum value based on the value that is being passed.
 *			Setting a zero value mutes the sound. Setting the maximum value
 *			results in the loudest possible sound.
 *
 */
inline void CMMFDevSound::CBody::SetGain(TInt aGain)
	{
	iPlugin->SetGain(aGain);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the speaker balance set for playing.
 *
 *	Leaves on failure.
 *
 *	@param	"aLeftPrecentage"
 *			On return contains the left speaker volume percentage.
 *
 *	@param	"aRightPercentage"
 *			On return contains the right speaker volume percentage.
 *
 */
inline void CMMFDevSound::CBody::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iPlugin->GetPlayBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Sets the speaker balance for playing.
 *
 *	The speaker balance can be changed before or during playback and is
 *	effective immediately.
 *
 *	@param	"aLeftPercentage"
 *			On return contains left speaker volume perecentage. This can be any
 *			value from zero to 100. Setting a zero value mutes the sound on left
 *			speaker.
 *
 *	@param	"aRightPercentage"
 *			On return contains right speaker volume perecentage. This can be any
 *			value from zero to 100. Setting a zero value mutes the sound on
 *			right speaker.
 *
 */
inline void CMMFDevSound::CBody::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	iPlugin->SetPlayBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the microphone gain balance set for recording.
 *
 *	Leaves on failure.
 *
 *	@param	"aLeftPercentage"
 *			On return contains the left microphone gain percentage.
 *
 *	@param	"aRightPercentage"
 *			On return contains the right microphone gain percentage.
 *
 */
inline void CMMFDevSound::CBody::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iPlugin->GetRecordBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Sets the microphone gain balance for recording.
 *
 *	The microphone gain balance can be changed before or during recording and
 *	is effective immediately.
 *
 *	@param	"aLeftPercentage"
 *			Left microphone gain precentage. This can be any value from zero to
 *			100. Setting a zero value mutes the gain on left microphone.
 *
 *	@param	"aRightPercentage"
 *			Right microphone gain precentage. This can be any value from zero to
 *			100. Setting a zero value mutes the gain on right microphone.
 *
 */
inline void CMMFDevSound::CBody::SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	iPlugin->SetRecordBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start play process. This method queries and
 *	acquires the audio policy before initializing audio device. If there was an
 *	error during policy initialization, PlayError() method will be called on
 *	the observer with error code KErrAccessDenied, otherwise BufferToBeFilled()
 *	method will be called with a buffer reference. After reading data into the
 *	buffer reference passed, the client should call PlayData() to play data.
 *
 *	The amount of data that can be played is specified in
 *	CMMFBuffer::RequestSize(). Any data that is read into buffer beyond this
 *	size will be ignored.
 *
 *	Leaves on failure.
 *
 */
inline void CMMFDevSound::CBody::PlayInitL()
	{
	iPlugin->PlayInitL();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start record process. This method queries and
 *	acquires the audio policy before initializing audio device. If there was an
 *	error during policy initialization, RecordError() method will be called on
 *	the observer with error code KErrAccessDenied, otherwise BufferToBeEmptied()
 *	method will be called with a buffer reference. This buffer contains recorded
 *	or encoded data. After processing data in the buffer reference passed, the
 *	client should call RecordData() to continue recording process.
 *
 *	The amount of data that is available is specified in
 *	CMMFBuffer::RequestSize().
 *
 *	Leaves on failure.
 *
 */
inline void CMMFDevSound::CBody::RecordInitL()
	{
	iPlugin->RecordInitL();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Plays data in the buffer at the current volume. The client should fill
 *	the buffer with audio data before calling this method. The Observer gets
 *	reference to buffer along with callback BufferToBeFilled(). When playing of
 *	the audio sample is complete, successfully or otherwise, the method
 *	PlayError() on observer is called.
 *
 */
inline void CMMFDevSound::CBody::PlayData()
	{
	iPlugin->PlayData();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Contine the process of recording. Once the buffer is filled with recorded
 *	data, the Observer gets reference to buffer along with callback
 *	BufferToBeEmptied(). After processing the buffer (copying over to a
 *	different buffer or writing to file) the client should call this
 *	method to continue recording process.
 *
 */
inline void CMMFDevSound::CBody::RecordData()
	{
	iPlugin->RecordData();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Stops the ongoing operation (Play, Record, TonePlay, Convert)
 *
 */
inline void CMMFDevSound::CBody::Stop()
	{
	iPlugin->Stop();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Temporarily Stops the ongoing operation (Play, Record, TonePlay, Convert)
 *
 */
inline void CMMFDevSound::CBody::Pause()
	{
	iPlugin->Pause();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the sample recorded so far.
 *
 *	@return "TInt"
 *			Returns the samples recorded.
 *
 */
inline TInt CMMFDevSound::CBody::SamplesRecorded()
	{ 
	return iPlugin->SamplesRecorded();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the sample played so far.
 *
 *	@return "TInt"
 *			Returns the samples recorded.
 *
 */
inline TInt CMMFDevSound::CBody::SamplesPlayed()
	{ 
	return iPlugin->SamplesPlayed();
	}


/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start playing tone. Tone is played with
 *	frequency and for duration specified.
 *
 *	Leaves on failure.
 *
 *	@param	"aFrequency"
 *			Frequency at with the tone will be played.
 *
 *	@param	"aDuration"
 *			The period over which the tone will be played. A zero value causes
 *			the no tone to be played (Verify this with test app).
 *
 */
inline void CMMFDevSound::CBody::PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	iPlugin->PlayToneL(aFrequency, aDuration);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start playing a dual tone. 
 *  The tone consists of two sine waves of different frequencies summed together
 *  Dual Tone is played with specified frequencies and for specified duration.
 *
 *	@param	"aFrequencyOne"
 *			First frequency of dual tone
 *
 *	@param	"aFrequencyTwo"
 *			Second frequency of dual tone
 *
 *	@param	"aDuration"
 *			The period over which the tone will be played. A zero value causes
 *			the no tone to be played (Verify this with test app).
 */
inline void CMMFDevSound::CBody::PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	iPlugin->PlayDualToneL(aFrequencyOne, aFrequencyTwo, aDuration);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start playing DTMF string aDTMFString.
 *
 *	Leaves on failure.
 *
 *	@param	"aDTMFString"
 *			DTMF sequence in a descriptor.
 *
 */
inline void CMMFDevSound::CBody::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	iPlugin->PlayDTMFStringL(aDTMFString);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start playing tone sequence.
 *
 *	Leaves on failure.
 *
 *	@param	"TDesC8& aData"
 *			Tone sequence in a descriptor.
 *
 */
inline void CMMFDevSound::CBody::PlayToneSequenceL(const TDesC8& aData)
	{
	iPlugin->PlayToneSequenceL(aData);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes audio device and start playing the specified pre-defined tone
 *	sequence.
 *
 *	Leaves on failure.
 *
 *	@param	"aSequenceNumber"
 *			The index identifying the specific pre-defined tone sequence. Index
 *			values are relative to zero.
 *			This can be any value from zero to the value returned by a call to
 *			FixedSequenceCount() - 1.
 *			The function raises a panic if sequence number is not within this
 *			range.
 *	@see	FixedSequenceCount()
 *
 */
inline void CMMFDevSound::CBody::PlayFixedSequenceL(TInt aSequenceNumber)
	{
	iPlugin->PlayFixedSequenceL(aSequenceNumber);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Defines the number of times the audio is to be repeated during the tone
 *	playback operation.
 *
 *	A period of silence can follow each playing of tone. The tone playing can
 *	be repeated indefinitely.
 *
 *	@param	" aRepeatCount"
 *			The number of times the tone, together with the trailing silence,
 *			is to be repeated. If this is set to KMdaRepeatForever, then the
 *			tone, together with the trailing silence, is repeated indefinitely
 *			or until Stop() is called. If this is set to zero, then the tone is
 *			not repeated.
 *
 *			Supported only during tone playing.
 *
 */
inline void CMMFDevSound::CBody::SetToneRepeats(TInt aRepeatCount,
				const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	iPlugin->SetToneRepeats(aRepeatCount, aRepeatTrailingSilence);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Defines the duration of tone on, tone off and tone pause to be used during the
 *	DTMF tone playback operation.
 *
 *	Supported only during tone playing.
 *
 *	@param	"aToneOnLength"
 *			The period over which the tone will be played. If this is set to
 *			zero, then the tone is not played.
 *
 *	@param	"aToneOffLength"
 *			The period over which the no tone will be played.
 *
 *	@param	"aPauseLength"
 *			The period over which the tone playing will be paused.
 *
 */
inline void CMMFDevSound::CBody::SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
								TTimeIntervalMicroSeconds32& aToneOffLength,
								TTimeIntervalMicroSeconds32& aPauseLength) 
	{
	iPlugin->SetDTMFLengths(aToneOnLength, aToneOffLength, aPauseLength);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Defines the period over which the volume level is to rise smoothly from
 *	nothing to the normal volume level.
 *
 *	@param	"aRampDuration"
 *			The period over which the volume is to rise. A zero value causes 
 *			the tone sample to be played at the normal level for the full
 *			duration of the playback. A value, which is longer than the duration
 *			of the tone sample, that the sample never reaches its normal
 *			volume level.
 *
 *
 */
inline void CMMFDevSound::CBody::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{	
	iPlugin->SetVolumeRamp(aRampDuration);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Defines the priority settings that should be used for this instance.
 *
 *	@param	"aPrioritySettings"
 *			An class type representing the client's priority, priority 
 *			preference and state.
 *
 */
inline void CMMFDevSound::CBody::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iPlugin->SetPrioritySettings(aPrioritySettings);
	}

inline TAny* CMMFDevSound::CBody::CustomInterface(TUid aInterface)
	{
	return iPlugin->CustomInterface(aInterface);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the number of available pre-defined tone sequences.
 *
 *	This is the number of fixed sequence supported by DevSound by default.
 *
 *	@return	"TInt"
 *			The fixed sequence count. This value is implementation dependent
 *			but is always greater than or equal to zero.
 *
 */
inline TInt CMMFDevSound::CBody::FixedSequenceCount()
	{
	return iPlugin->FixedSequenceCount();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns the name assigned to a specific pre-defined tone sequence.
 *
 *	This is the number of fixed sequence supported by DevSound by default.
 *
 *	The function raises a panic if sequence number specified invalid.
 *
 *	@return	"TDesC&"
 *			A reference to a Descriptor containing the fixed sequence
 *			name indexed by aSequenceNumber.
 *
 *	@param	"aSequenceNumber"
 *			The index identifying the specific pre-defined tone sequence. Index
 *			values are relative to zero.
 *			This can be any value from zero to the value returned by a call to
 *			FixedSequenceCount() - 1.
 *			The function raises a panic if sequence number is not within this
 *			range.
 *
 *	@see	FixedSequenceCount()
 *
 */
inline const TDesC& CMMFDevSound::CBody::FixedSequenceName(TInt aSequenceNumber)
	{
	return iPlugin->FixedSequenceName(aSequenceNumber);
	}


/*
 *  -doxygen comments in header-
 */
inline void CMMFDevSound::CBody::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,const TMMFPrioritySettings& aPrioritySettings) const
	{
	iPlugin->GetSupportedInputDataTypesL(aSupportedDataTypes, aPrioritySettings);
	}


/*
 *  -doxygen comments in header-
 */
inline void CMMFDevSound::CBody::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const
	{
	iPlugin->GetSupportedOutputDataTypesL(aSupportedDataTypes, aPrioritySettings);
	}

#endif // SOUNDDEVICEBODY_INL
