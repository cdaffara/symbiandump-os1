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




// INCLUDE FILES
#include "mmfdevsoundadaptation.h"
#include "mmfdevsoundadaptationbody.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CMMFDevSoundAdaptation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMMFDevSoundAdaptation::CMMFDevSoundAdaptation()
	{
	TRACE_CREATE();
	DP_CONTEXT(CMMFDevSoundAdaptation::CMMFDevSoundAdaptation *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF_HEAP_STATUS(PRF_MEM, HEAP_DS_Session);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CMMFDevSoundAdaptation::ConstructL(MDevSoundAdaptationObserver& aDevSoundObserver, 
										MGlobalProperties& aGlobalProperties)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody = CBody::NewL(aDevSoundObserver, aGlobalProperties);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMMFDevSoundAdaptation* CMMFDevSoundAdaptation::NewL(MDevSoundAdaptationObserver& aDevSoundObserver,
															MGlobalProperties& aGlobalProperties)
	{
	DP_STATIC_CONTEXT(CMMFDevSoundAdaptation::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CMMFDevSoundAdaptation* self = new (ELeave)CMMFDevSoundAdaptation();
	CleanupStack::PushL(self);
	self->ConstructL(aDevSoundObserver, aGlobalProperties);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::ConstructL
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMMFDevSoundAdaptation::~CMMFDevSoundAdaptation()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::~CMMFDevSoundAdaptation *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	delete iBody;
	PRF_HEAP_STATUS(PRF_MEM, HEAP_DS_Session);
	DP_OUT();
	}

// PostOpenL() - asynchronous 2nd-phase open operation
EXPORT_C void CMMFDevSoundAdaptation::PostOpenL()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PostOpenL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PostOpenL();
	DP_OUT();
	}
	
// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::InitializeL
// Initializes CMMFDevSoundProxy object to play and record PCM16 raw audio data
// with sampling rate of 8 KHz.On completion of Initialization, calls
// InitializeComplete() on aDevSoundObserver.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::InitializeL(TMMFState aMode)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::InitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->InitializeL(aMode);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::InitializeL
// Initializes DevSound object for the mode aMode for processing audio data
// with hardware device aHWDev. On completion of Initialization, the observer
// will be notified via call back InitializeComplete().
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::InitializeL(TUid aHWDev,
								TMMFState aMode)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::InitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->InitializeL(aHWDev, aMode);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::InitializeL
// Initializes DevSound object for the mode aMode for processing audio data
// with hardware device supporting FourCC aDesiredFourCC. On completion of
// Initialization, the observer will be notified via callback
// InitializeComplete().
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::InitializeL(TFourCC aDesiredFourCC,
								TMMFState aMode)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::InitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->InitializeL(aDesiredFourCC, aMode);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CancelInitialize
// Cancels the initialization of DevSound object
// returns an error code
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::CancelInitialize()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CancelInitialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->CancelInitialize();
	DP0_RET(retVal, "%d");		
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::Capabilities
// Returns the supported Audio settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::Capabilities(TMMFCapabilities& aCap)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Capabilities *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->Capabilities(aCap);
	DP0_RET(retVal, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::Config
// Returns the current audio settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TMMFCapabilities CMMFDevSoundAdaptation::Config() const
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Config *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TMMFCapabilities retVal = iBody->Config();										
	DP0_RET(retVal, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetConfigL
// Configure CMMFDevSoundProxy object for the settings in aConfig.
// Use this to set sampling rate, Encoding and Mono/Stereo.
// As part of defect 20796, the iRecordFormat has been set under the iPlayFormat,
// before it was not set at all.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::SetConfigL(
								const TMMFCapabilities& aConfig)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetConfigL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->SetConfigL(aConfig);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::MaxVolume
// Returns an integer representing the maximum volume.
// This is the maximum value which can be passed to CMMFDevSoundProxy::SetVolume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::MaxVolume()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::MaxVolume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->MaxVolume();
	DP0_RET(retVal, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::Volume
// Returns an integer representing the current volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::Volume()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Volume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->Volume();
	DP0_RET(retVal, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetVolume
// Changes the current playback volume to a specified value.
// The volume can be changed before or during playback and is effective
// immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetVolume(TInt aVolume, TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetVolume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->SetVolume(aVolume, aAsyncCompletion);
	DP0_RET(error, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::MaxGain
// Returns an integer representing the maximum gain.
// This is the maximum value which can be passed to CMMFDevSoundProxy::SetGain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::MaxGain()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::MaxGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->MaxGain();
	DP0_RET(retVal, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::Gain
// Returns an integer representing the current gain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::Gain()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Gain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->Gain();
	DP0_RET(retVal, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetGain
// Changes the current recording gain to a specified value.
// The gain can be changed before or during recording and is effective
// immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetGain(TInt aGain, TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->SetGain(aGain, aAsyncCompletion);
	DP0_RET(error, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::GetPlayBalanceL
// Returns the speaker balance set for playing.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::GetPlayBalanceL(
								TInt& aLeftPercentage,
								TInt& aRightPercentage)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::GetPlayBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->GetPlayBalanceL(aLeftPercentage, aRightPercentage);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetPlayBalanceL
// Sets the speaker balance for playing. The speaker balance can be changed
// before or during playback and is effective immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::SetPlayBalanceL(
								TInt aLeftPercentage,
								TInt aRightPercentage,
								TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetPlayBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->SetPlayBalanceL(aLeftPercentage, aRightPercentage, aAsyncCompletion);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::GetRecordBalanceL
// Returns the microphone gain balance set for recording.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::GetRecordBalanceL(
								TInt& aLeftPercentage,
								TInt& aRightPercentage)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::GetRecordBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->GetRecordBalanceL(aLeftPercentage, aRightPercentage);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetRecordBalanceL
// Sets the microphone gain balance for recording.
// The microphone gain balance can be changed before or during recording and
// is effective immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::SetRecordBalanceL(
								TInt aLeftPercentage,
								TInt aRightPercentage,
								TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetRecordBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->SetRecordBalanceL(aLeftPercentage, aRightPercentage, aAsyncCompletion);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayInitL
// Initializes audio device and start play process. This method queries and
// acquires the audio policy before initializing audio device. If there was an
// error during policy initialization, PlayError() method will be called on
// the observer with error code KErrAccessDenied, otherwise BufferToBeFilled()
// method will be called with a buffer reference. After reading data into the
// buffer reference passed, the client should call PlayData() to play data.
//
// The amount of data that can be played is specified in
// CMMFBuffer::RequestSize(). Any data that is read into buffer beyond this
// size will be ignored.
//
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayInitL()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayInitL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayInitL();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::RecordInitL
// Initializes audio device and start record process. This method queries and
// acquires the audio policy before initializing audio device. If there was an
// error during policy initialization, RecordError() method will be called on
// the observer with error code KErrAccessDenied, otherwise BufferToBeEmptied()
// method will be called with a buffer reference. This buffer contains recorded
// or encoded data. After processing data in the buffer reference passed, the
// client should call RecordData() to continue recording process.
//
// The amount of data that is available is specified in
// CMMFBuffer::RequestSize().
//
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::RecordInitL()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::RecordInitL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->RecordInitL();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayData
// Plays data in the buffer at the current volume. The client should fill
// the buffer with audio data before calling this method. The Observer gets
// reference to buffer along with callback BufferToBeFilled(). When playing of
// the audio sample is complete, successfully or otherwise, the method
// PlayError() on observer is called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayData()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayData();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::RecordData
// Contine the process of recording. Once the buffer is filled with recorded
// data, the Observer gets reference to buffer along with callback
// BufferToBeEmptied(). After processing the buffer (copying over to a
// different buffer or writing to file) the client should call this
// method to continue recording process.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::RecordData()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::RecordData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->RecordData();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::Stop
// Stops the ongoing operation (Play, Record, TonePlay)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMMFDevSoundAdaptation::Stop()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool retVal = iBody->Stop();
	DP0_RET(retVal, "%d");		
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::Pause
// Temporarily Stops the ongoing operation (Play, Record, TonePlay)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::Pause()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Pause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->Pause();
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SamplesRecorded
// Returns the sample recorded so far.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SamplesRecorded()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SamplesRecorded *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->SamplesRecorded();
	DP0_RET(retVal, "%d");	
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SamplesPlayed
// Returns the sample played so far.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SamplesPlayed()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SamplesPlayed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->SamplesPlayed();
	DP0_RET(retVal, "%d");	
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayToneL
// Initializes audio device and start playing tone. Tone is played with
// frequency and for duration specified.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayToneL(
								TInt aFrequency,
								const TTimeIntervalMicroSeconds& aDuration)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayToneL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayToneL(aFrequency, aDuration);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayDualToneL
// Initializes audio device and start playing a dual tone.
// The tone consists of two sine waves of different frequencies summed together
// Dual Tone is played with specified frequencies and for specified duration.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayDualToneL(
								TInt aFrequencyOne,
								TInt aFrequencyTwo,
								const TTimeIntervalMicroSeconds& aDuration)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayDualToneL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayDualToneL(aFrequencyOne, aFrequencyTwo, aDuration);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayDTMFStringL
// Initializes audio device and start playing DTMF string aDTMFString.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayDTMFStringL(
								const TDesC& aDTMFString)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayDTMFStringL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayDTMFStringL(aDTMFString);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayToneSequenceL
// Initializes audio device and start playing tone sequence.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayToneSequenceL(
								const TDesC8& aData)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayToneSequenceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayToneSequenceL(aData);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::PlayFixedSequenceL
// Initializes audio device and start playing the specified pre-defined tone
// sequence.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::PlayFixedSequenceL(
								TInt aSequenceNumber)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::PlayFixedSequenceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->PlayFixedSequenceL(aSequenceNumber);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetToneRepeats
// Defines the number of times the audio is to be repeated during the tone
// playback operation. A period of silence can follow each playing of tone.
// The tone playing can be repeated indefinitely.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetToneRepeats(
								TInt aRepeatCount,
								const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetToneRepeats *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->SetToneRepeats(aRepeatCount, aRepeatTrailingSilence);
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetDTMFLengths
// Defines the duration of tone on, tone off and tone pause to be used during the
// DTMF tone playback operation.
// Supported only during tone playing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetDTMFLengths(
								TTimeIntervalMicroSeconds32& aToneOnLength,
								TTimeIntervalMicroSeconds32& aToneOffLength,
								TTimeIntervalMicroSeconds32& aPauseLength)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetDTMFLengths *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->SetDTMFLengths(aToneOnLength, aToneOffLength, aPauseLength);
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetVolumeRamp
// Defines the period over which the volume level is to rise smoothly from
// nothing to the normal volume level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetVolumeRamp(
								const TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetVolumeRamp *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->SetVolumeRamp(aRampDuration);
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::SetPrioritySettings
// Defines the priority settings that should be used for this instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetPrioritySettings(
								const TMMFPrioritySettings& aPrioritySettings)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::SetPrioritySettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->SetPrioritySettings(aPrioritySettings);
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CustomInterface
// see sounddevice.h
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CMMFDevSoundAdaptation::CustomInterface(
								TUid aInterfaceId)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CustomInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
    // TODO - Need to revisit this to allow for async operation
	TAny* retVal = iBody->CustomInterface(aInterfaceId);
	DP0_RET(retVal, "0x%x");	
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::FixedSequenceCount
// Returns the number of available pre-defined tone sequences.
// This is the number of fixed sequence supported by DevSound by default.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::FixedSequenceCount()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::FixedSequenceCount *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->FixedSequenceCount();
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::FixedSequenceName
// Returns the name assigned to a specific pre-defined tone sequence.
// This is the number of fixed sequence supported by DevSound by default.
// The function raises a panic if sequence number specified invalid.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CMMFDevSoundAdaptation::FixedSequenceName(
								TInt aSequenceNumber)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::FixedSequenceName *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return(iBody->FixedSequenceName(aSequenceNumber));						
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::GetSupportedInputDataTypesL
// see sounddevice.h
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::GetSupportedInputDataTypesL(
RArray<TFourCC>& aSupportedDataTypes,
const TMMFPrioritySettings& aPrioritySettings) const
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::GetSupportedInputDataTypesL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->GetSupportedInputDataTypesL(aSupportedDataTypes, aPrioritySettings);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::GetSupportedOutputDataTypesL
// see sounddevice.h
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMMFDevSoundAdaptation::GetSupportedOutputDataTypesL(
								RArray<TFourCC>& aSupportedDataTypes,
								const TMMFPrioritySettings& aPrioritySettings) const
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::GetSupportedOutputDataTypesL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->GetSupportedOutputDataTypesL(aSupportedDataTypes, aPrioritySettings);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// SetClientConfig
// Sets client capabilities for this instance of DevSound Adaptation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::SetClientConfig(
                                const TProcessId& aProcessId)
    {
    DP_CONTEXT(CMMFDevSoundAdaptation::SetClientConfig *CD1*, CtxDevSound, DPLOCAL);
    DP_IN();
    TInt retVal = iBody->SetClientConfig(aProcessId);
    DP0_RET(retVal, "%d");
    }

EXPORT_C TInt CMMFDevSoundAdaptation::SetClientConfig(
                                const TProcessId& aActualProcessId,
                                const TProcessId& aProcessId)
    {
    DP_CONTEXT(CMMFDevSoundAdaptation::SetClientConfig *CD1*, CtxDevSound, DPLOCAL);
    DP_IN();
    TInt retVal = iBody->SetClientConfig(aActualProcessId, aProcessId);
    DP0_RET(retVal, "%d");
    }

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::EmptyBuffers
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------

EXPORT_C TInt CMMFDevSoundAdaptation::EmptyBuffers()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::EmptyBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->EmptyBuffers();
	DP0_RET(retVal, "%d");			
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CloseDevSound
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMMFDevSoundAdaptation::CloseDevSound()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::::CloseDevSound *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool rerVal= iBody->CloseDevSound();
	DP0_RET(rerVal, "%d");	
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::ProcessingFinished
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::ProcessingFinishedReceived(TBool& asyncOperation)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::::ProcessingFinished *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt retVal = iBody->ProcessingFinishedReceived(asyncOperation);
	DP0_RET(retVal, "%d");	
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::ProcessingFinished
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::ProcessingError(TBool& asyncOperation)
    {
    DP_CONTEXT(CMMFDevSoundAdaptation::::ProcessingError *CD1*, CtxDevSound, DPLOCAL);
    DP_IN();
    TInt retVal = iBody->ProcessingError(asyncOperation);
    DP0_RET(retVal, "%d");  
    }

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::RegisterAsClient
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->RegisterAsClient(aEventType, aNotificationRegistrationData);
	DP0_RET(error, "%d");
	}
	
	
// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CancelRegisterAsClient
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->CancelRegisterAsClient(aEventType);
	DP0_RET(error, "%d");
	}
	

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::GetResourceNotificationData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::GetResourceNotificationData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->GetResourceNotificationData(aEventType, aNotificationData);
	DP0_RET(error, "%d");	
	}


// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::WillResumePlay
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::WillResumePlay()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->WillResumePlay();
	DP0_RET(error, "%d");	
	}		

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::GetTimePlayed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::GetTimePlayed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iBody->GetTimePlayed(aTime);
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::IsResumeSupported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMMFDevSoundAdaptation::IsResumeSupported()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::IsResumeSupported *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool isSupported = iBody->IsResumeSupported();
	DP0_RET(isSupported , "%d");	
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::IsResumeSupported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMMFDevSoundAdaptation::Resume()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Resume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt error = iBody->Resume();
	DP0_RET(error, "%d");
	}

EXPORT_C void CMMFDevSoundAdaptation::BufferErrorEvent()
	{
	iBody->BufferErrorEvent();
	}

EXPORT_C void CMMFDevSoundAdaptation::RollbackAdaptorActiveStateToBeforeCommit()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::RollbackAdaptorActiveStateToBeforeCommit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody->RollbackAdaptorActiveStateToBeforeCommit();
	DP_OUT();
	}
	
// End of file
