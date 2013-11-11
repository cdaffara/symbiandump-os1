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

#include <mmf/server/sounddevice.h>
#include "SoundDeviceBody.h"

/*
 *
 *	Default Constructor.
 *
 *
 */
CMMFDevSound::CMMFDevSound()
	{
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Destructor.
 *
 *	Deletes all objects and releases all resource owned by this
 *	instance.
 *
 */
EXPORT_C CMMFDevSound::~CMMFDevSound()
	{
	delete iBody;
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Constructs, and returns a pointer to, a new CMMFDevSound object.
 *
 *	Leaves on failure.
 *
 */
EXPORT_C CMMFDevSound* CMMFDevSound::NewL()
	{
	CMMFDevSound* self = new (ELeave) CMMFDevSound;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Second phase constructor.
 *
 */
void CMMFDevSound::ConstructL()
	{
	iBody = CBody::NewL();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Initializes CMMFDevSound object to play and record PCM16 raw audio data
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
EXPORT_C void CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)

	{
	iBody->InitializeL(aDevSoundObserver,aMode);
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
EXPORT_C void CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)
	{
	iBody->InitializeL(aDevSoundObserver, aHWDev, aMode);
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
EXPORT_C void CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, 
										  TFourCC aDesiredFourCC, TMMFState aMode)
	{
	iBody->InitializeL(aDevSoundObserver, aDesiredFourCC, aMode);
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
EXPORT_C TMMFCapabilities CMMFDevSound::Capabilities()
	{
	return iBody->Capabilities();
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
EXPORT_C TMMFCapabilities CMMFDevSound::Config() const
	{
	return iBody->Config();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Configure CMMFDevSound object for the settings in aConfig.
 *
 *	Use this to set sampling rate, Encoding and Mono/Stereo.
 *
 *	@param	"aConfig"
 *			Attribute values to which CMMFDevSound object will be configured to.
 *
 */
EXPORT_C void CMMFDevSound::SetConfigL(const TMMFCapabilities& aConfig)
	{
	iBody->SetConfigL(aConfig);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns an integer representing the maximum volume.
 *
 *	This is the maximum value which can be passed to CMMFDevSound::SetVolume.
 *
 *	@return	"TInt"
 *			The maximum volume. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
EXPORT_C TInt CMMFDevSound::MaxVolume()
	{
	return iBody->MaxVolume();
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
EXPORT_C TInt CMMFDevSound::Volume()
	{
	return iBody->Volume();
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
 *			returned by a call to CMMFDevSound::MaxVolume(). If the
 *			volume is not within this range, the volume is automatically set to
 *			minimum or maximum value based on the value that is being passed.
 *			Setting a zero value mutes the sound. Setting the maximum value
 *			results in the loudest possible sound.
 *
 */
EXPORT_C void CMMFDevSound::SetVolume(TInt aVolume)
	{
	iBody->SetVolume(aVolume);
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Returns an integer representing the maximum gain.
 *
 *	This is the maximum value which can be passed to CMMFDevSound::SetGain.
 *
 *	@return	"TInt"
 *			The maximum gain. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
EXPORT_C TInt CMMFDevSound::MaxGain()
	{
	return iBody->MaxGain();
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
EXPORT_C TInt CMMFDevSound::Gain()
	{
	return iBody->Gain();
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
 *			returned by a call to CMMFDevSound::MaxGain(). If the
 *			volume is not within this range, the gain is automatically set to
 *			minimum or maximum value based on the value that is being passed.
 *			Setting a zero value mutes the sound. Setting the maximum value
 *			results in the loudest possible sound.
 *
 */
EXPORT_C void CMMFDevSound::SetGain(TInt aGain)
	{
	iBody->SetGain(aGain);
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
EXPORT_C void CMMFDevSound::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iBody->GetPlayBalanceL(aLeftPercentage, aRightPercentage);
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
EXPORT_C void CMMFDevSound::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	iBody->SetPlayBalanceL(aLeftPercentage, aRightPercentage);
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
EXPORT_C void CMMFDevSound::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iBody->GetRecordBalanceL(aLeftPercentage, aRightPercentage);
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
EXPORT_C void CMMFDevSound::SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	iBody->SetRecordBalanceL(aLeftPercentage, aRightPercentage);
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
EXPORT_C void CMMFDevSound::PlayInitL()
	{
	iBody->PlayInitL();
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
EXPORT_C void CMMFDevSound::RecordInitL()
	{
	iBody->RecordInitL();
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
EXPORT_C void CMMFDevSound::PlayData()
	{
	iBody->PlayData();
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
EXPORT_C void CMMFDevSound::RecordData()
	{
	iBody->RecordData();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Stops the ongoing operation (Play, Record, TonePlay, Convert)
 *
 */
EXPORT_C void CMMFDevSound::Stop()
	{
	iBody->Stop();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Temporarily Stops the ongoing operation (Play, Record, TonePlay, Convert)
 *
 */
EXPORT_C void CMMFDevSound::Pause()
	{
	iBody->Pause();
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
EXPORT_C TInt CMMFDevSound::SamplesRecorded()
	{ 
	return iBody->SamplesRecorded();
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
EXPORT_C TInt CMMFDevSound::SamplesPlayed()
	{ 
	return iBody->SamplesPlayed();
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
EXPORT_C void CMMFDevSound::PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	iBody->PlayToneL(aFrequency, aDuration);
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
EXPORT_C void CMMFDevSound::PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	iBody->PlayDualToneL(aFrequencyOne, aFrequencyTwo, aDuration);
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
EXPORT_C void CMMFDevSound::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	iBody->PlayDTMFStringL(aDTMFString);
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
EXPORT_C void CMMFDevSound::PlayToneSequenceL(const TDesC8& aData)
	{
	iBody->PlayToneSequenceL(aData);
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
EXPORT_C void CMMFDevSound::PlayFixedSequenceL(TInt aSequenceNumber)
	{
	iBody->PlayFixedSequenceL(aSequenceNumber);
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
EXPORT_C void CMMFDevSound::SetToneRepeats(TInt aRepeatCount,
				const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	iBody->SetToneRepeats(aRepeatCount, aRepeatTrailingSilence);
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
EXPORT_C void CMMFDevSound::SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
								TTimeIntervalMicroSeconds32& aToneOffLength,
								TTimeIntervalMicroSeconds32& aPauseLength) 
	{
	iBody->SetDTMFLengths(aToneOnLength, aToneOffLength, aPauseLength);
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
EXPORT_C void CMMFDevSound::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{	
	iBody->SetVolumeRamp(aRampDuration);
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
EXPORT_C void CMMFDevSound::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iBody->SetPrioritySettings(aPrioritySettings);
	}

EXPORT_C TAny* CMMFDevSound::CustomInterface(TUid aInterface)
	{
	return iBody->CustomInterface(aInterface);
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
EXPORT_C TInt CMMFDevSound::FixedSequenceCount()
	{
	return iBody->FixedSequenceCount();
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
EXPORT_C const TDesC& CMMFDevSound::FixedSequenceName(TInt aSequenceNumber)
	{
	return iBody->FixedSequenceName(aSequenceNumber);
	}


/*
 *  -doxygen comments in header-
 */
EXPORT_C void CMMFDevSound::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,const TMMFPrioritySettings& aPrioritySettings) const
	{
	iBody->GetSupportedInputDataTypesL(aSupportedDataTypes, aPrioritySettings);
	}


/*
 *  -doxygen comments in header-
 */
EXPORT_C void CMMFDevSound::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const
	{
	iBody->GetSupportedOutputDataTypesL(aSupportedDataTypes, aPrioritySettings);
	}


/********************************************************************************
 *				Non Exported public functions ends here							*
 ********************************************************************************/

/******************************************************************************
 *	Function Name:	E32Dll
 *	
 *	Description:	Entry point for applications.
 *
 ******************************************************************************/

enum TDllReason {};
EXPORT_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}

// CMMFDevSoundEventHandler::NewL() has been declared in export table
// but since it is the only class method to be so, and .h is in source
// it is not actually usable. Just declare the following to keep linker happy

// Need dummy abstract type - this is not the real class
class RMMFAudioPolicyProxy;

class CMMFDevSoundEventHandler : public CActive
	{
public:
	IMPORT_C static CMMFDevSoundEventHandler* NewL(RMMFAudioPolicyProxy*);
private:
	CMMFDevSoundEventHandler();
	};
	
EXPORT_C CMMFDevSoundEventHandler* CMMFDevSoundEventHandler::NewL(RMMFAudioPolicyProxy*)
	{
	_LIT(KModule, "DevSound");
	User::Panic(KModule, 1000);
	return NULL;
	}

// default constructor - keep compilers happy
CMMFDevSoundEventHandler::CMMFDevSoundEventHandler():
	CActive(0)
	{
	}
