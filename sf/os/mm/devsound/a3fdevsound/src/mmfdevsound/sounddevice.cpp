// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include <mmf/server/sounddevice.h>
#include "sounddevicebody.h"

/*
 *  Default Constructor.
 */
CMMFDevSound::CMMFDevSound()
	{
	}

/*
 *  Destructor.
 */
EXPORT_C CMMFDevSound::~CMMFDevSound()
	{
	delete iBody;
	}

/*
 *  CMMFDevSound::NewL
 *
 *  Constructs and returns a pointer to a new CMMFDevSound object.
 */
EXPORT_C CMMFDevSound* CMMFDevSound::NewL()
	{
	CMMFDevSound* self = new (ELeave) CMMFDevSound;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*
 *  CMMFDevSound::ConstructL
 *
 *  Second phase constructor.
 */
void CMMFDevSound::ConstructL()
	{
	iBody = CBody::NewL();
	}

/*
 *  CMMFDevSound::InitializeL
 *
 *  Initializes CMMFDevSound object. On completion of Initialization will
 *  call InitializeComplete() on aDevSoundObserver.
 *
 *  @param  aDevSoundObserver. A reference to the DevSound Observer instance.
 *  @param  aMode. A mode for which this object will be used.
 */
EXPORT_C void CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver,TMMFState aMode)

	{
	iBody->InitializeL(aDevSoundObserver,aMode);
	}

/*
 *  CMMFDevSound::InitializeL
 *
 *  Initializes CMMFDevSound object with hardware device aHWDev. On completion
 *  of Initialization will call InitializeComplete() on aDevSoundObserver.
 *
 *	Method is deprecated from OS release 9.5
 *
 *  @param  aDevSoundObserver. A reference to the DevSound Observer instance.
 *  @param  aHWDev. CMMFHwDevice implementation identifier.
 *  @param  aMode. A mode for which this object will be used.
 */
EXPORT_C void CMMFDevSound::InitializeL(
							MDevSoundObserver& /*aDevSoundObserver*/,
							TUid /*aHWDev*/,
							TMMFState /*aMode*/)
	{
	User::Leave(KErrNotSupported);
	}

/*
 *  CMMFDevSound::InitializeL
 *
 *  Initializes CMMFDevSound object with hardware device with hardware
 *  device's FourCC code. On completion of Initialization will call
 *  InitializeComplete() on aDevSoundObserver.
 *
 *  @param  aDevSoundObserver. A reference to the DevSound Observer instance.
 *  @param  aDesiredFourCC. CMMFHwDevice implementation FourCC.
 *  @param  aMode. A mode for which this object will be used.
 */
EXPORT_C void CMMFDevSound::InitializeL(
							MDevSoundObserver& aDevSoundObserver,
							TFourCC aDesiredFourCC,
							TMMFState aMode)
	{
	iBody->InitializeL(aDevSoundObserver, aDesiredFourCC, aMode);
	}

/*
 *  CMMFDevSound::Capabilities
 *
 *  Returns supported Audio settings.
 *
 *  @return TMMFCapabilities device settings.
 */
EXPORT_C TMMFCapabilities CMMFDevSound::Capabilities()
	{
	return iBody->Capabilities();
	}

/*
 *  CMMFDevSound::Config
 *
 *  Returns current audio settings.
 *
 *  @return TMMFCapabilities device settings.
 */
EXPORT_C TMMFCapabilities CMMFDevSound::Config() const
	{
	return iBody->Config();
	}

/*
 *  CMMFDevSound::SetConfigL
 *
 *  ConfigureS CMMFDevSound object with the settings in aConfig.
 *
 *  Use this to set sampling rate, Encoding and Mono/Stereo.
 *
 *  @param  aConfig. CMMFDevSound configuration settings.
 */
EXPORT_C void CMMFDevSound::SetConfigL(const TMMFCapabilities& aConfig)
	{
	iBody->SetConfigL(aConfig);
	}

/*
 *  CMMFDevSound::MaxVolume
 *
 *  Returns an integer representing the maximum volume.
 *
 *  This is the maximum volume which can be passed to CMMFDevSound::SetVolume.
 *
 *  @return TInt
 *          The maximum volume. This value is platform dependent but is always
 *          greater than or equal to one.
 */
EXPORT_C TInt CMMFDevSound::MaxVolume()
	{
	return iBody->MaxVolume();
	}

/*
 *  CMMFDevSound::Volume
 *
 *  Returns an integer representing current volume level.
 *
 *  @return TInt
 *          Current volume level.
 */
EXPORT_C TInt CMMFDevSound::Volume()
	{
	return iBody->Volume();
	}

/*
 *  CMMFDevSound::SetVolume
 *
 *  Changes current volume level to the specified value.
 *
 *  The volume can be changed before or during playback and is effective
 *  immediately.
 *
 *  @param  TInt
 *          The volume setting. This can be any value between zero and the
 *          value returned by the call to CMMFDevSound::MaxVolume(). If the
 *          volume is out of range, it is automatically set to the minimum or
 *          maximum level closest to the value being passed in. Setting a zero
 *          value mutes the sound.
 */
EXPORT_C void CMMFDevSound::SetVolume(TInt aVolume)
	{
	iBody->SetVolume(aVolume);
	}

/*
 *  CMMFDevSound::MaxGain
 *
 *  Returns an integer representing the maximum microphone gain.
 *
 *  This is the maximum value which can be passed to CMMFDevSound::SetGain.
 *
 *  @return TInt
 *          The maximum gain. This value is platform dependent but is always
 *          greater than or equal to one.
 */
EXPORT_C TInt CMMFDevSound::MaxGain()
	{
	return iBody->MaxGain();
	}

/*
 *  CMMFDevSound::Gain
 *
 *  Returns an integer representing current gain.
 *
 *  @return TInt
 *          The current gain level.
 */
EXPORT_C TInt CMMFDevSound::Gain()
	{
	return iBody->Gain();
	}

/*
 *  CMMFDevSound::SetGain
 *
 *  Changes current recording gain to a specified value.
 *
 *  The gain can be changed before or during recording and is effective
 *  immediately.
 *
 *  @param  aGain. This can be any value between zero and the
 *          value returned by the call to CMMFDevSound::MaxGain(). If the
 *          gain is out of range, it is automatically set to minimum or maximum
 *          value closest to the value that is being passed.
 *          Setting a zero value mutes the microphone.
 */
EXPORT_C void CMMFDevSound::SetGain(TInt aGain)
	{
	iBody->SetGain(aGain);
	}

/*
 *  CMMFDevSound::GetPlayBalanceL
 *
 *  Returns the speaker balance set for playing.
 *
 *  @param  aLeftPercentage. On return contains the left speaker volume percentage.
 *  @param  aRightPercentage. On return contains the left speaker volume percentage.
 */
EXPORT_C void CMMFDevSound::GetPlayBalanceL(TInt& aLeftPercentage,TInt& aRightPercentage)
	{
	iBody->GetPlayBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  CMMFDevSound::SetPlayBalanceL
 *
 *  Sets the speaker balance for playing.
 *
 *  The speaker balance can be changed before or during playback and is
 *  effective immediately.
 *
 *  @param  aLeftPercentage. Left speaker volume perecentage. This can be any value between
 *          zero and 100. Setting a zero value mutes the sound on the left
 *          speaker.
 *  @param  aRightPercentage. Right speaker volume perecentage. This can be any value between
 *          zero and 100. Setting a zero value mutes the sound on the right
 *          speaker.
 */
EXPORT_C void CMMFDevSound::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	iBody->SetPlayBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  CMMFDevSound::GetRecordBalanceL
 *
 *  Returns the microphone gain balance set for recording.
 *
 *  @param  aLeftPercentage. On return contains the left microphone gain percentage.
 *  @param  aRightPercentage. On return contains the right microphone gain percentage.
 */
EXPORT_C void CMMFDevSound::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iBody->GetRecordBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  CMMFDevSound::SetRecordBalanceL
 *
 *  Sets the microphone gain balance for recording.
 *
 *  The microphone gain balance can be changed before or during recording and
 *  is effective immediately.
 *
 *  @param  aLeftPercentage. Left microphone gain precentage. This can be any value between zero
 *          and 100. Setting a zero value mutes the gain on the left microphone.
 *  @param  aRightPercentage. Right microphone gain precentage. This can be any value between zero
 *          and 100. Setting a zero value mutes the gain on the right microphone
 */
EXPORT_C void CMMFDevSound::SetRecordBalanceL(TInt aLeftPercentage,TInt aRightPercentage)
	{
	iBody->SetRecordBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *  CMMFDevSound::PlayInitL
 *
 *  Initializes audio device and starts the playback. Before playback can be
 *  started, its current client's access priority is first verified by the
 *  audio policy. In case of an error during the policy initialization, the
 *  PlayError() method will be called on the observer with KErrAccessDenied
 *  error code, otherwise BufferToBeFilled() method will be called with a
 *  buffer reference. After filling the buffer with the data, the client
 *  should call PlayData() to start playback.
 *
 *  The amount of data that can be played is specified in
 *  CMMFBuffer::RequestSize(). Any data that is read into the buffer beyond
 *  this size will be ignored.
 *
 */
EXPORT_C void CMMFDevSound::PlayInitL()
	{
	iBody->PlayInitL();
	}

/*
 *  CMMFDevSound::RecordInitL
 *
 *  Initializes audio device and starts the recording. Before recording can be
 *  started, its current client's access priority is first verified by the
 *  audio policy. In case of an error during the policy initialization, the
 *  RecordError() method will be called on the observer with KErrAccessDenied
 *  error code, otherwise BufferToBeEmptied() method will be called with a
 *  buffer reference. This buffer contains recorded or encoded data. After
 *  processing the data in the buffer, the client should call RecordData()
 *  to continue recording process.
 *
 *  The amount of data that is available is specified in
 *  CMMFBuffer::RequestSize().
 *
 */
EXPORT_C void CMMFDevSound::RecordInitL()
	{
	iBody->RecordInitL();
	}

/*
 *  CMMFDevSound::PlayData
 *
 *  Plays data in the buffer. The client should fill the buffer with a stream
 *  of sampled audio data before calling this method. The observer gets the
 *  reference to the buffer along with BufferToBeFilled() callback. When
 *  playing of the audio sample is complete, with success or not, the
 *  PlayError() method is called on the observer.
 */
EXPORT_C void CMMFDevSound::PlayData()
	{
	iBody->PlayData();
	}

/*
 *  CMMFDevSound::RecordData
 *
 *  Records audio data. Once the buffer is filled with recorded sampled audio
 *  data, the observer gets reference to the buffer along with
 *  BufferToBeEmptied() callback. After processing of the buffer (copying over
 *  to a different buffer or writing to a file) the client should call this
 *  method again to continue recording process.
 */
EXPORT_C void CMMFDevSound::RecordData()
	{
	iBody->RecordData();
	}

/*
 *  CMMFDevSound::Stop
 *
 *  Stops the ongoing operation (Play, Record, TonePlay)
 */
EXPORT_C void CMMFDevSound::Stop()
	{
	iBody->Stop();
	}

/*
 *  CMMFDevSound::Pause
 *
 *  Temporarily suspends the ongoing operation (Play, Record, TonePlay)
 */
EXPORT_C void CMMFDevSound::Pause()
	{
	iBody->Pause();
	}

/*
 *  CMMFDevSound::SamplesRecorded
 *
 *  Returns the number of recorded samples up to this point.
 *
 *  @return TInt
 *          Value representing recorded samples.
 */
EXPORT_C TInt CMMFDevSound::SamplesRecorded()
	{
	return iBody->SamplesRecorded();
	}

/*
 *  CMMFDevSound::SamplesPlayed
 *
 *  Returns the number of played samples up to this point.
 *
 *  @return TInt
 *          Value representing played samples.
 */
EXPORT_C TInt CMMFDevSound::SamplesPlayed()
	{
	return iBody->SamplesPlayed();
	}

/*
 *  CMMFDevSound::PlayToneL
 *
 *  Initializes audio device and starts playing a single tone according with
 *  the specified frequency and duration attributes.
 *
 *  @param  aFrequency. Frequency at with the tone will be played.
 *  @param  aDuration. The period over which the tone will be played. A zero value causes
 *          no tone to be played.
 */
EXPORT_C void CMMFDevSound::PlayToneL(TInt aFrequency,const TTimeIntervalMicroSeconds& aDuration)
	{
	iBody->PlayToneL(aFrequency, aDuration);
	}

/*
 *  CMMFDevSound::PlayDualToneL
 *
 *  Initializes audio device and starts playing a dual tone.
 *  The tone consists of two sine waves of different frequencies summed
 *  together. Both frequencies and the duration are specified in the passed
 *  in attributes.
 *
 *  @param  aFrequencyOne. Value representing first frequency of the dual tone.
 *
 *  @param  aFrequencyTwo. Value representing second frequency of the dual tone.
 *
 *  @param  aDuration. The period over which the tone will be played. A zero value causes
 *          no tone to be played.
 */
EXPORT_C void CMMFDevSound::PlayDualToneL(
								TInt aFrequencyOne,
								TInt aFrequencyTwo,
								const TTimeIntervalMicroSeconds& aDuration)
	{
	iBody->PlayDualToneL(aFrequencyOne, aFrequencyTwo, aDuration);
	}

/*
 *  CMMFDevSound::PlayDTMFStringL
 *
 *  Initializes audio device and starts playing DTMF string.
 *
 *  @param  aDTMFString. DTMF sequence in a descriptor.
 */
EXPORT_C void CMMFDevSound::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	iBody->PlayDTMFStringL(aDTMFString);
	}

/*
 *  CMMFDevSound::PlayToneSequenceL
 *
 *  Initializes audio device and starts playing tone sequence.
 *
 *  @param  aData. Tone sequence in a descriptor.
 */
EXPORT_C void CMMFDevSound::PlayToneSequenceL(const TDesC8& aData)
	{
	iBody->PlayToneSequenceL(aData);
	}

/*
 *  CMMFDevSound::PlayFixedSequenceL
 *
 *  Initializes audio device and starts playing the specified tone sequence.
 *
 *	Method is deprecated from OS release 9.5
 *
 *  @param  aSequenceNumber. The index identifying the specific pre-defined tone sequence. The
 *          index values are relative to zero. This can be any value between
 *          zero and the value returned by the call to
 *          CMdaAudioPlayerUtility::FixedSequenceCount() - 1. The function
 *          raises a panic if sequence number is out of range.
 */
EXPORT_C void CMMFDevSound::PlayFixedSequenceL(TInt /*aSequenceNumber*/)
	{
	User::Leave(KErrNotSupported);
	}

/*
 *  CMMFDevSound::SetToneRepeats
 *
 *  Defines the number of times the audio is to be repeated during the tone
 *  playback operation.
 *
 *  A period of silence can follow each playing of tone. The tone playing can
 *  be repeated indefinitely.
 *
 *  @param  aRepeatCount. The number of times the tone, together with the trailing silence,
 *          is to be repeated. If this is set to KMdaRepeatForever, then the
 *          tone, together with the trailing silence, is repeated indefinitely
 *          or until Stop() is called. If this is set to zero, then the tone is
 *          not repeated.  Supported only during the tone playing.

 *  @param  aRepeatTrailingSilence. The duration of the trailing silence.
 */
EXPORT_C void CMMFDevSound::SetToneRepeats(TInt aRepeatCount,const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	iBody->SetToneRepeats(aRepeatCount, aRepeatTrailingSilence);
	}

/*
 *  CMMFDevSound::SetDTMFLengths
 *
 *  Defines the duration of 'tone on/tone off' and 'tone pause' to be used
 *  during the DTMF playback.
 *
 *  Supported only during tone playing.
 *
 *  @param  aToneOnLength. The period over which the tone will be played. If this is set to
 *          zero, then the tone is not played.
 *
 *  @param  aToneOffLength. The period over which 'no tone' will be played.
 *
 *  @param  aPauseLength. The period over which the tone playing will be paused.
 */
EXPORT_C void CMMFDevSound::SetDTMFLengths(
									TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength)
	{
	iBody->SetDTMFLengths(aToneOnLength, aToneOffLength, aPauseLength);
	}

/*
 *  CMMFDevSound::SetVolumeRamp
 *
 *  Defines the period over which the volume level will rise smoothly from
 *  mute to the normal volume level.
 *
 *  @param  aRampDuration. The period over which the volume is to rise. A zero value causes
 *          the tone sample to be played at the normal level for the full duration of the 
 *          playback. A value, which is longer than the duration
 *          of the tone sample, will result in the sample never reaching its
 *          normal volume level.
 */
EXPORT_C void CMMFDevSound::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	iBody->SetVolumeRamp(aRampDuration);
	}

/*
 *  CMMFDevSound::SetPrioritySettings
 *
 *  Defines the priority settings that should be used for this instance.
 *
 *  @param  aPrioritySettings. A structure representing client's priority, priority
 *          preference and the state.
 */
EXPORT_C void CMMFDevSound::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iBody->SetPrioritySettings(aPrioritySettings);
	}

/*
 *  CMMFDevSound::CustomInterface
 *
 *  Sends request to the DevSound Server to start custom interface specified
 *  by the TUid attribute.
 *
 *  @param  aInterface. Unique ID of the custom interface
 *
 *  @return TAny*
 *          Pointer to the custom interface object.
 */
EXPORT_C TAny* CMMFDevSound::CustomInterface(TUid aInterface)
	{
	return iBody->CustomInterface(aInterface);
	}

/*
 *  CMMFDevSound::FixedSequenceCount
 *
 *  Returns the number of available pre-defined tone sequences.
 *
 *	Method is deprecated from OS release 9.5
 *
 *  This is the number of fixed sequence supported by the DevSound by default.
 *
 *  @return TInt
 *          The fixed sequence count. This value is implementation dependent
 *          but is always greater than or equal to zero.
 */
EXPORT_C TInt CMMFDevSound::FixedSequenceCount()
	{
	return 0;
	}

/*
 *  CMMFDevSound::FixedSequenceName
 *
 *  Returns the name assigned to a specific pre-defined tone sequence.
 *
 *	Method is deprecated from OS release 9.5
 *
 *  @param  aSequenceNumber. The index identifying the specific pre-defined tone sequence.
 *          Index values are relative to zero. This can be any value between
 *          zero and the value returned by the call to
 *          CMdaAudioPlayerUtility::FixedSequenceCount() - 1. The function
 *          raises a panic if sequence number is out of range.
 *
 *  @return TDesC&
 *          A reference to a descriptor containing fixed sequence name
 *          indexed by aSequenceNumber.
 */
EXPORT_C const TDesC& CMMFDevSound::FixedSequenceName(TInt /*aSequenceNumber*/)
	{
	return KNullDesC;
	}

/*
 *  CMMFDevSound::GetSupportedInputDataTypesL
 *
 *  Returns a list of supported input data types that can be sent to the
 *  DevSound for playing audio.
 *
 *  @param  aSupportedDataTypes. An array of supported data types.
 *  @param  aPrioritySettings. Structure containing priority settings.
 */
EXPORT_C void CMMFDevSound::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,const TMMFPrioritySettings& aPrioritySettings) const
	{
	iBody->GetSupportedInputDataTypesL(aSupportedDataTypes,
									aPrioritySettings);
	}

/*
 *  CMMFDevSound::GetSupportedOutputDataTypesL
 *
 *  Returns a list of supported output data types that can be received from
 *  the DevSound for recording audio.
 *
 *  @param  aSupportedDataTypes. An array of supported data types.
 *  @param aPrioritySettings. Structure containing priority settings.
 */
EXPORT_C void CMMFDevSound::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,const TMMFPrioritySettings& aPrioritySettings) const
	{
	iBody->GetSupportedOutputDataTypesL(aSupportedDataTypes,aPrioritySettings);
	}

/********************************************************************************
 *				Non Exported public functions ends here		*					
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
	CActive(EPriorityStandard)
	{
	}


// End of File
