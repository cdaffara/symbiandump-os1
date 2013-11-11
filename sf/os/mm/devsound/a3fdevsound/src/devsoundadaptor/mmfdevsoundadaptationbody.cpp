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




#include <e32base.h>

#include "mmfdevsoundadaptationbody.h"
#include "cdevaudiocontrol.h"
#include "cdevaudio.h"
#include "a3ffourcclookup.h"

#include <a3f/audioformatuids.h>

#include "mglobalproperties.h"

const TInt KDefaultOnDTMFLength = 250000;
const TInt KDefaultOffDTMFLength = 50000;
const TInt KDefaultPauseDTMFLength = 250000;
const TInt KDefaultBufferSize = 4096;

// ============================ MEMBER FUNCTIONS =============================// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::CBody
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMMFDevSoundAdaptation::CBody::CBody(MDevSoundAdaptationObserver& aDevSoundObserver, 
									MGlobalProperties& aGlobalProperties)
	:iDevSoundObserver(aDevSoundObserver)
	{
	TRACE_CREATE();
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::CBody *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iGlobalProperties = &aGlobalProperties;
	
	iMode = EMMFStateIdle;
	// Initialize default config for cases when the DevSound's client query using 
	// CMMDevSound::Config() before setting the configuration through 
	// CMMDevSound::SetConfigL(TMMFCappabilites aConfig)
	iMmfConfig.iRate = EMMFSampleRate8000Hz;
	iMmfConfig.iEncoding = EMMFSoundEncoding16BitPCM;
	iMmfConfig.iChannels = EMMFMono;
	iMmfConfig.iBufferSize = KDefaultBufferSize;

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::ConstructL
// Symbian 2nd phase constructor can leave.
// assumes that iParent has already been set up properly
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::ConstructL(MGlobalProperties& aGlobalProperties)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// create DevAudio
	iDevAudio = CDevAudio::NewL(iDevSoundObserver, aGlobalProperties);
	
	//Default values for DTMF tones
	iToneData.SetDtmfLengths(KDefaultOnDTMFLength,KDefaultOffDTMFLength,KDefaultPauseDTMFLength);
	
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMMFDevSoundAdaptation::CBody* CMMFDevSoundAdaptation::CBody::NewL(MDevSoundAdaptationObserver& aDevSoundObserver,
																MGlobalProperties& aGlobalProperties)
	{
	DP_STATIC_CONTEXT(CMMFDevSoundAdaptation::CBody::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CMMFDevSoundAdaptation::CBody* self = new (ELeave) CBody(aDevSoundObserver, aGlobalProperties);
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalProperties);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::~CBody
// Destructor
// -----------------------------------------------------------------------------
//
CMMFDevSoundAdaptation::CBody::~CBody()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::~CBody *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iDevAudio)
		{
		delete iDevAudio;
		}
	DP_OUT();
	}
	
// CMMFDevSoundAdaptation:CBody::PostOpenL()
// 2nd phase post open - used to setup internal attributes asynchronously
//
void CMMFDevSoundAdaptation::CBody::PostOpenL()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PostOpenL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	User::LeaveIfError(iDevAudio->PostOpen());
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::InitializeL
// Initializes CMMFDevSoundProxy object to play and record PCM16 raw audio data
// with sampling rate of 8 KHz.
// On completion of Initialization, calls InitializeComplete() on
// aDevSoundObserver.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::InitializeL(TMMFState aMode)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::InitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (aMode == EMMFStateTonePlaying)
		{
		DoInitializeL(KUidFormatTone, aMode); 
		}
	else
		{
		DoInitializeL(KUidFormatPCM16, aMode);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::InitializeL
// Initializes DevSound object for the mode aMode for processing audio data
// with hardware device aHWDev.
// On completion of Initialization, the observer will be notified via call back
// InitializeComplete().
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::InitializeL(TUid /*aHWDev*/,
								TMMFState /*aMode*/)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::InitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	User::LeaveIfError(KErrNotSupported);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::InitializeL
// Initializes DevSound object for the mode aMode for processing audio data
// with hardware device supporting FourCC aDesiredFourCC.
// On completion of Initialization, the observer will be notified via call back
// InitializeComplete().
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::InitializeL(
								TFourCC aDesiredFourCC,
								TMMFState aMode)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::InitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TUid format = {0};
	TInt err = KErrNone;
	
	err = iGlobalProperties->GetFourCCConvertor().FourCCToFormat(aDesiredFourCC, format);
	if(err == KErrNone)
		{
		DoInitializeL(format, aMode);
		}
	else
		{
		User::LeaveIfError(KErrNotSupported);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::DoInitializeL
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::DoInitializeL(
								TUid aFormat,
								TMMFState aMode)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::DoInitializeL *CD1*, CtxDevSound, DPLOCAL);
	DP2_IN("format=0x%x, mode=%d",aFormat.iUid,aMode);


	ASSERT(iDevAudio);

	TInt err(KErrNone);

	if ((aMode == EMMFStatePlaying) || (aMode == EMMFStateRecording)
			|| (aMode == EMMFStateTonePlaying))
		{
		err = iDevAudio->Initialize(aFormat, aMode);
		}
	else
		{
		err = KErrNotSupported;
		}
	User::LeaveIfError(err);

	iMode = aMode;

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::CancelInitialize
// Cancels the initialization of DevSound object
// returns an error code
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::CancelInitialize()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::CancelInitialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err=iDevAudio->CancelInitialize();
	DP0_RET(err, "%d");
	}


// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Capabilities
// Returns the supported Audio settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::Capabilities(TMMFCapabilities& aCap)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::Capabilities *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// query real values, on error just return last known (default) values
	TInt err = iDevAudio->GetAudioControl()->GetCapabilities(aCap);
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Config()
// Returns the current audio settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TMMFCapabilities CMMFDevSoundAdaptation::CBody::Config() const
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::Config *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// Query real values, on error just return last known (default) values
	TMMFCapabilities config;
	TInt err = iDevAudio->GetAudioControl()->GetConfig(config);
	// TBD: for now, just return set value for encoding - codec does not give this
	config.iEncoding = iMmfConfig.iEncoding;
	if ( err == KErrNone )
		{
		DP0_RET(config, "0");
		}
	DP0_RET(iMmfConfig, "0");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetConfigL
// Configure device for the settings in aConfig.
// Use this to set sampling rate, Encoding and Mono/Stereo.
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::SetConfigL(const TMMFCapabilities& aConfig)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetConfigL *CD1*, CtxDevSound, DPLOCAL);
	DP3_IN("rate 0x%x, channels 0x%x, encoding 0x%x",
			aConfig.iRate, aConfig.iChannels, aConfig.iEncoding);
	User::LeaveIfError(iDevAudio->GetAudioControl()->SetConfig(aConfig));
	// success, update local copy
	iMmfConfig.iRate = aConfig.iRate;
	iMmfConfig.iChannels = aConfig.iChannels;
	iMmfConfig.iEncoding = aConfig.iEncoding;
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::MaxVolume
// Returns an integer representing the maximum volume.
// This is the maximum value which can be passed to CMMFDevSoundProxy::SetVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::MaxVolume()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::MaxVolume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iDevAudio->DevSoundMaxVolume(), "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Volume
// Returns an integer representing the current volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::Volume()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::Volume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iDevAudio->DevSoundVolume(), "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetVolume
// Changes the current playback volume to a specified value.
// The volume can be changed before or during playback and is effective
// immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetVolume(TInt aVolume, TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetVolume *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aVolume = %d", aVolume);
	TInt error = iDevAudio->SetDevSoundVolume(aVolume, aAsyncCompletion);
	DP0_RET(error, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::MaxGain
// Returns an integer representing the maximum gain.
// This is the maximum value which can be passed to CMMFDevSoundProxy::SetGain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::MaxGain()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::MaxGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iDevAudio->DevSoundMaxGain(), "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Gain
// Returns an integer representing the current gain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::Gain()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::Gain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iDevAudio->DevSoundGain(), "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetGain
// Changes the current recording gain to a specified value.
// The gain can be changed before or during recording and is effective
// immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetGain(TInt aGain, TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aGain = %d", aGain);
	TInt error = iDevAudio->SetDevSoundGain(aGain, aAsyncCompletion);
	DP0_RET(error, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::GetPlayBalanceL
// Returns the speaker balance set for playing.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::GetPlayBalanceL(TInt& aLeftPercentage,
								                    TInt& aRightPercentage)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::GetPlayBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iDevAudio->GetDevSoundPlayBalance(aLeftPercentage, aRightPercentage);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetPlayBalanceL
// Sets the speaker balance for playing.
// The speaker balance can be changed before or during playback and is
// effective immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::SetPlayBalanceL(TInt aLeftPercentage,
													TInt aRightPercentage,
													TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetPlayBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP2_IN("aLeftPercentage = %d, aRightPercentage=%d", aLeftPercentage, aRightPercentage);

	User::LeaveIfError(iDevAudio->SetDevSoundPlayBalance(aLeftPercentage, aRightPercentage, aAsyncCompletion));

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::GetRecordBalanceL
// Returns the microphone gain balance set for recording.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::GetRecordBalanceL(TInt& aLeftPercentage,
								                      TInt& aRightPercentage)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::GetRecordBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iDevAudio->GetDevSoundRecordBalance(aLeftPercentage, aRightPercentage);

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetRecordBalanceL
// Sets the microphone gain balance for recording.
// The microphone gain balance can be changed before or during recording and
// is effective immediately.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::SetRecordBalanceL(TInt aLeftPercentage,
													  TInt aRightPercentage,
													  TBool& aAsyncCompletion)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetRecordBalanceL *CD1*, CtxDevSound, DPLOCAL);
	DP2_IN("aLeftPercentage = %d, aRightPercentage=%d", aLeftPercentage, aRightPercentage);

	User::LeaveIfError(iDevAudio->SetDevSoundRecordBalance(aLeftPercentage, aRightPercentage,aAsyncCompletion));

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayInitL
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
void CMMFDevSoundAdaptation::CBody::PlayInitL()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayInitL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iMode==EMMFStatePlaying)
		{
		User::LeaveIfError(iDevAudio->GetAudioControl()->ProcessInit());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStatePlaying! (iMode=%d)",iMode);
		User::Leave(KErrNotReady);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::RecordInitL
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
void CMMFDevSoundAdaptation::CBody::RecordInitL()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::RecordInitL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iMode==EMMFStateRecording)
		{
		User::LeaveIfError(iDevAudio->GetAudioControl()->ProcessInit());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateRecording! (iMode=%d)",iMode);
		User::Leave(KErrNotReady);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayData
// Plays data in the buffer at the current volume. The client should fill
// the buffer with audio data before calling this method. The Observer gets
// reference to buffer along with callback BufferToBeFilled(). When playing of
// the audio sample is complete, successfully or otherwise, the method
// PlayError() on observer is called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::PlayData()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iMode==EMMFStatePlaying)
		{
		iDevAudio->GetAudioControl()->ProcessData();
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStatePlaying! (iMode=%d)",iMode);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::RecordData
// Contine the process of recording. Once the buffer is filled with recorded
// data, the Observer gets reference to buffer along with callback
// BufferToBeEmptied(). After processing the buffer (copying over to a
// different buffer or writing to file) the client should call this
// method to continue recording process.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::RecordData()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::RecordData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iMode==EMMFStateRecording)
		{
		iDevAudio->GetAudioControl()->ProcessData();
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateRecording! (iMode=%d)",iMode);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Stop
// Stops the ongoing operation (Play, Record, TonePlay)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMMFDevSoundAdaptation::CBody::Stop()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool isCompleted = EFalse;
	
	// Need for sequences when calling stop and is already stopped
	TDevSoundAdaptorState state = iDevAudio->ActiveState();
	if( (iMode != EMMFStateIdle) && 
		(state == EDevSoundAdaptorActive_Active || 
		state == EDevSoundAdaptorPaused_Primed ) )
		{
		TInt err = iDevAudio->GetAudioControl()->Stop();
		if (err != KErrNone)
			{
			isCompleted = ETrue;
			}
		}
	else
		{
		isCompleted = ETrue;	
		}
	DP_OUT();
	return isCompleted;
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Pause
// Temporarily Stops the ongoing operation (Play, Record, TonePlay)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::Pause()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::Pause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = iDevAudio->GetAudioControl()->Pause();
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SamplesRecorded
// Returns the sample recorded so far.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SamplesRecorded()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SamplesRecorded *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iMode==EMMFStateRecording)
		{
		DP_OUT();
		return(iDevAudio->GetAudioControl()->GetSamples());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateRecording! (iMode=%d)",iMode);
		DP0_RET(0, "%d");
		}
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SamplesPlayed
// Returns the sample played so far.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SamplesPlayed()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SamplesPlayed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iMode==EMMFStatePlaying)
		{
		DP_OUT();
		return(iDevAudio->GetAudioControl()->GetSamples());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStatePlaying! (iMode=%d)",iMode);
		DP0_RET(0, "%d");
		}
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayToneL
// Initializes audio device and start playing tone. Tone is played with
// frequency and for duration specified.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::PlayToneL(
								TInt aFrequency,
								const TTimeIntervalMicroSeconds& aDuration)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayToneL *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aFrequency = %d", aFrequency);

	if(iMode==EMMFStateTonePlaying)
		{
		iToneData.SetType(TToneData::ESimple);
		iToneData.SetFrequencyOne(aFrequency);
		iToneData.SetDuration(aDuration);
		User::LeaveIfError(iDevAudio->GetAudioControl()->SetToneData(iToneData));
		User::LeaveIfError(iDevAudio->GetAudioControl()->ProcessInit());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateTonePlaying! (iMode=%d)",iMode);
		User::Leave(KErrNotSupported);
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayDualToneL
// Initializes audio device and start playing a dual tone.
// The tone consists of two sine waves of different frequencies summed together
// Dual Tone is played with specified frequencies and for specified duration.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::PlayDualToneL(
								TInt aFrequencyOne,
								TInt aFrequencyTwo,
								const TTimeIntervalMicroSeconds& aDuration)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayDualToneL *CD1*, CtxDevSound, DPLOCAL);
	DP2_IN("aFrequencyOne = %d, aFrequencyTwo=%d", aFrequencyOne, aFrequencyTwo);

	if(iMode==EMMFStateTonePlaying)
		{
		iToneData.SetType(TToneData::EDual); 
		iToneData.SetFrequencyOne(aFrequencyOne);
		iToneData.SetFrequencyTwo(aFrequencyTwo);
		iToneData.SetDuration(aDuration);
		User::LeaveIfError(iDevAudio->GetAudioControl()->SetToneData(iToneData));
		User::LeaveIfError(iDevAudio->GetAudioControl()->ProcessInit());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateTonePlaying! (iMode=%d)",iMode);
		User::Leave(KErrNotSupported);
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayDTMFStringL
// Initializes audio device and start playing DTMF string aDTMFString.
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayDTMFStringL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iMode==EMMFStateTonePlaying)
		{
		iToneData.SetType(TToneData::EDtmfString);
		iToneData.SetDTMFString(const_cast<TDesC&>(aDTMFString));
		User::LeaveIfError(iDevAudio->GetAudioControl()->SetToneData(iToneData));
		User::LeaveIfError(iDevAudio->GetAudioControl()->ProcessInit());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateTonePlaying! (iMode=%d)",iMode);
		User::Leave(KErrNotSupported);
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayToneSequenceL
// Initializes audio device and start playing tone sequence.
//
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::PlayToneSequenceL(const TDesC8& aData)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayToneSequenceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iMode==EMMFStateTonePlaying)
		{
		iToneData.SetType(TToneData::ESequence); 
		iToneData.SetSequenceData(const_cast<TDesC8&>(aData));
		User::LeaveIfError(iDevAudio->GetAudioControl()->SetToneData(iToneData));
		User::LeaveIfError(iDevAudio->GetAudioControl()->ProcessInit());
		}
	else
		{
		DP1(DLERR,"Current mode is not EMMFStateTonePlaying! (iMode=%d)",iMode);
		User::Leave(KErrNotSupported);
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::PlayFixedSequenceL
// Initializes audio device and start playing the specified pre-defined tone
// sequence.
//
// Leaves on failure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::PlayFixedSequenceL(TInt /*aSequenceNumber*/)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::PlayFixedSequenceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	User::Leave(KErrNotSupported);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetToneRepeats
// Defines the number of times the audio is to be repeated during the tone
// playback operation. A period of silence can follow each playing of tone.
// The tone playing can be repeated indefinitely.
// Supported only during tone playing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetToneRepeats(
								TInt aRepeatCount,
								const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetToneRepeats *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aRepeatCount = %d", aRepeatCount);

	TInt err = KErrNone;
	iToneData.SetRepeatCount(aRepeatCount);
	iToneData.SetRepeatTrailingSilence(aRepeatTrailingSilence);
		
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetDTMFLengths
// Defines the duration of tone on, tone off and tone pause to be used during
// the DTMF tone playback operation.
// Supported only during tone playing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetDTMFLengths(
								TTimeIntervalMicroSeconds32& aToneOnLength,
								TTimeIntervalMicroSeconds32& aToneOffLength,
								TTimeIntervalMicroSeconds32& aPauseLength)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetDTMFLengths *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = KErrNone;
	if (aToneOnLength.Int() < KMdaInfiniteDurationDTMFToneOnLength)
		{
		aToneOnLength = TTimeIntervalMicroSeconds32(0);
		}
	if (aToneOffLength.Int() < 0)
		{
		aToneOffLength = TTimeIntervalMicroSeconds32(0);
		}
	if (aPauseLength.Int() < 0)
		{
		aPauseLength = TTimeIntervalMicroSeconds32(0);
		}
	
	iToneData.SetDtmfLengths(aToneOnLength, aToneOffLength, aPauseLength);
	
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetVolumeRamp
// Defines the period over which the volume level is to rise smoothly from
// nothing to the normal volume level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetVolumeRamp(
								const TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetVolumeRamp *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = iDevAudio->SetVolumeRamp(aRampDuration);
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetPrioritySettings
// Defines the priority settings that should be used for this instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetPrioritySettings(
								const TMMFPrioritySettings& aPrioritySettings)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetPrioritySettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = iDevAudio->SetPrioritySettings(aPrioritySettings);
	if (err == KErrNone)
		{
		iPrioritySettings = aPrioritySettings;
		}
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::CustomInterface
// @see sounddevice.h
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CMMFDevSoundAdaptation::CBody::CustomInterface(TUid aInterfaceId)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::CustomInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if ( iDevAudio )
		{
		DP_OUT();
		return(iDevAudio->GetAudioControl()->CustomInterface(aInterfaceId));
		}
	else
		{
		DP0_RET(NULL, "%d");
		}
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::FixedSequenceCount
// Returns the number of available pre-defined tone sequences.
// This is the number of fixed sequence supported by DevSound by default.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::FixedSequenceCount()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::FixedSequenceCount *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(0, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::FixedSequenceName
// Returns the name assigned to a specific pre-defined tone sequence.
// This is the number of fixed sequence supported by DevSound by default.
// The function raises a panic if sequence number specified invalid.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CMMFDevSoundAdaptation::CBody::FixedSequenceName(
								TInt /*aSequenceNumber*/)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::FixedSequenceName *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KNullDesC, "");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::GetSupportedInputDataTypesL
// @see sounddevice.h
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::GetSupportedInputDataTypesL(
								RArray<TFourCC>& aSupportedDataTypes,
								const TMMFPrioritySettings& /*aPrioritySettings*/) const
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::GetSupportedInputDataTypesL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	//aPrioritySettings not used on ref DevSound
	//search for playing datatypes
	iDevAudio->DevSoundSupportedDataTypesL(aSupportedDataTypes, KDataForPlay);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::GetSupportedOutputDataTypesL
// @see sounddevice.h
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMMFDevSoundAdaptation::CBody::GetSupportedOutputDataTypesL(
								RArray<TFourCC>& aSupportedDataTypes,
								const TMMFPrioritySettings& /*aPrioritySettings*/) const
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::GetSupportedOutputDataTypesL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	//aPrioritySettings not used on ref DevSound
	// search for recording datatypes
	iDevAudio->DevSoundSupportedDataTypesL(aSupportedDataTypes, KDataForRecord);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::SetClientConfig
// Sets client capabilities for this instance of DevSound Adaptation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::SetClientConfig(
                                const TProcessId& aActualProcessId,
                                const TProcessId& aProcessId)
    {
    DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetClientConfig *CD1*, CtxDevSound, DPLOCAL);
    DP_IN();

    TInt err = iDevAudio->SetClientConfig(aActualProcessId, aProcessId);
    DP0_RET(err, "%d");
    }

TInt CMMFDevSoundAdaptation::CBody::SetClientConfig(
                                const TProcessId& aProcessId)
    {
    DP_CONTEXT(CMMFDevSoundAdaptation::CBody::SetClientConfig *CD1*, CtxDevSound, DPLOCAL);
    DP_IN();

    TInt err = iDevAudio->SetClientConfig(aProcessId);
    DP0_RET(err, "%d");
    }

TBool CMMFDevSoundAdaptation::CBody::CloseDevSound()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::CloseDevSound *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iDevAudio->GetAudioControl()->DestroyChain();
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::ProcessingFinishedReceived
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CMMFDevSoundAdaptation::CBody::ProcessingFinishedReceived(TBool& asyncOperation)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::ProcessingFinishedReceived *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iDevAudio->GetAudioControl()->ProcessingFinishedReceived(asyncOperation);
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::ProcessingFinishedReceived
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TInt CMMFDevSoundAdaptation::CBody::ProcessingError(TBool& asyncOperation)
    {
    DP_CONTEXT(CMMFDevSoundAdaptation::CBody::ProcessingError *CD1*, CtxDevSound, DPLOCAL);
    DP_IN();
    DP_OUT();
    return iDevAudio->GetAudioControl()->ProcessingError(asyncOperation);
    }

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::EmptyBuffers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::EmptyBuffers()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::EmptyBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	// Empty buffers is only supported on playing
	if (iMode == EMMFStateRecording || iMode == EMMFStateTonePlaying)
		{
		err = KErrNotSupported;
		}
	else if (!iDevAudio)
		{
		err = KErrNotReady;
		}
	else
		{
		TDevSoundAdaptorState state = iDevAudio->ActiveState();
		if (state == EDevSoundAdaptorActive_Active || 
			state == EDevSoundAdaptorPaused_Primed )
			{
			err = iDevAudio->GetAudioControl()->RequestEmptyBuffers();
			}
		else
			{
			err = KErrNotReady;
			}
		}
	DP0_RET(err, "%d");
	}	




// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::RegisterAsClient
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iDevAudio)
		{
		err = iDevAudio->RegisterAsClient(aEventType, aNotificationRegistrationData);
		}
	else
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}
	
	
	
	
	
// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::CancelRegisterAsClient
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iDevAudio)
		{
		err = iDevAudio->CancelRegisterAsClient(aEventType);
		}
	else 
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}	




// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::GetResourceNotificationData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::GetResourceNotificationData(TUid /*aEventType*/, TDes8& aNotificationData)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::GetResourceNotificationData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iDevAudio)
		{
		TMMFTimeIntervalMicroSecondsPckg pckg = TTimeIntervalMicroSeconds(SamplesPlayed());
		aNotificationData.Copy(pckg);
		}
	else
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}	




// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::WillResumePlay
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::WillResumePlay()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iDevAudio)
		{
		err = iDevAudio->WillResumePlay();
		}
	else
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}	

// CMMFDevSoundAdaptation::CBody::GetTimePlayed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::GetTimePlayed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = iDevAudio->GetAudioControl()->GetTimePlayed(aTime);
	DP0_RET(err, "%d");
	}
		
// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::IsResumeSupported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMMFDevSoundAdaptation::CBody::IsResumeSupported()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::IsResumeSupported *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool isSupported = iDevAudio->IsResumeSupported();
	DP0_RET(isSupported, "%d");
	}

// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::CBody::Resume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMMFDevSoundAdaptation::CBody::Resume()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::Resume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iDevAudio->GetAudioControl()->Resume();
	DP0_RET(err, "%d");
	}

void CMMFDevSoundAdaptation::CBody::BufferErrorEvent()
	{
	ASSERT(iDevAudio);
	iDevAudio->GetAudioControl()->BufferErrorEvent();
	}

void CMMFDevSoundAdaptation::CBody::RollbackAdaptorActiveStateToBeforeCommit()
	{
	DP_CONTEXT(CMMFDevSoundAdaptation::CBody::RollbackAdaptorActiveStateToBeforeCommit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TDevSoundAdaptorState previousState = iDevAudio->PreviousState();
	// Set previous state to the active state set after state changing Commit call
	iDevAudio->SetPreviousState(iDevAudio->ActiveState());
	// Set active state to the previous state set before state changing Commit call
	iDevAudio->SetActiveState(previousState);
	DP_OUT();
	}
