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

#include <e32base.h>
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
#include <OMX_Core.h>
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
#include "MmfDevSoundSessionBody.h"
#include "MmfDevSoundSessionXtnd.h"
#include "MmfDevSoundServer.h"
#include <mdaaudiotoneplayer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmffourcc.h>
#include <mmfhwdeviceimplementationuids.hrh>
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh> // KUidRefDevSoundTaskConfig & KMmfUidEmptyBuffersCustomInterface
#include <mmf/server/mmfdevsoundcustominterfacesupport.h>

const TUid KEmptyBuffersCustomInterfaceTypeUid	= {KMmfUidEmptyBuffersCustomInterface};

/*
 *
 *	Default Constructor.
 *
 *	No default implementation. CMMFDevSoundProxy implements 2-phase construction.
 *
 */
CMMFDevSoundSvrImp::CMMFDevSoundSvrImp(CMMFDevSoundSessionXtnd* aParent)
: iParent(*aParent)
	{
	iMode= EMMFDevSoundStateIdle;
	//Set reasonable default values for DTMF 
	iDTMFGen.SetToneDurations(250000,50000,250000);
	}

/*
 *
 *	Destructor.
 *
 *	Deletes all objects and releases all resource owned by this
 *	instance.
 *
 */
CMMFDevSoundSvrImp::~CMMFDevSoundSvrImp()
	{
	delete iToneBuffer1;
	delete iToneBuffer2;
	delete iDevSoundEventHandler; 
	if( iAudioPolicyProxy != NULL)
		{
		iAudioPolicyProxy->Close(); 
		delete iAudioPolicyProxy;
		}
	delete iDevSoundUtil;
	delete iFixedSequences;
	delete iCMMFHwDevice;
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	OMX_Deinit();
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	}

/*
 *
 *	Constructs, and returns a pointer to, a new CMMFDevSoundSvrImp object.
 *
 *	Leaves on failure.
 *
 */
CMMFDevSoundSvrImp* CMMFDevSoundSvrImp::NewL(CMMFDevSoundSessionXtnd* aParent)
	{
	CMMFDevSoundSvrImp* self = new (ELeave) CMMFDevSoundSvrImp(aParent);
	return self;
	}

/*
 *
 *	3rd phase constructor - assumes that iParent has already been set up properly
 *                          (During ConstructL() it has yet to be
 *
 */
void CMMFDevSoundSvrImp::Construct3L(RServer2& aPolicyServerHandle)
	{
	// all these data properties should be NULL, but add ASSERTs to verity
	ASSERT(iAudioPolicyProxy==NULL);
	iAudioPolicyProxy = new (ELeave) RMMFAudioPolicyProxy();
	ASSERT(iDevSoundEventHandler==NULL);
	iDevSoundEventHandler = CMMFDevSoundEventHandler::NewL(iAudioPolicyProxy);
	User::LeaveIfError(iAudioPolicyProxy->Open(aPolicyServerHandle));
	iDevSoundEventHandler->SetDevSoundInfo(&iParent);

	iDevSoundUtil = CMMFDevSoundUtility::NewL();
	// Initialize Fixed sequence related
	iDevSoundUtil->InitializeFixedSequenceL(&iFixedSequences);
	
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	OMX_Init();
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	PreInitializeL();
	}

/**
 * internal procedure to perform all initialization prior to setting the 
 * data type 4CC code
 */
void CMMFDevSoundSvrImp::PreInitializeL()
	{
	// Set default values for priority settings: Note: Client must 
	// over ride default settings by calling SetPrioirtySettings
	iAudioPolicyPrioritySettings.iState = EMMFStateStopped;
	iAudioPolicyPrioritySettings.iPref = EMdaPriorityPreferenceNone;
	iAudioPolicyPrioritySettings.iPriority = 0;

	// Get device capabilities and current settings from Audio Policy:
	User::LeaveIfError(iAudioPolicyProxy->GetPlayFormatsSupported(iPlayFormatsSupported));
	User::LeaveIfError(iAudioPolicyProxy->GetPlayFormat(iPlayFormat));
	User::LeaveIfError(iAudioPolicyProxy->GetRecordFormatsSupported(iRecordFormatsSupported));
	User::LeaveIfError(iAudioPolicyProxy->GetRecordFormat(iRecordFormat));

	//default to play until we know we are recording
	User::LeaveIfError(InitializeFormat(iPlayFormatsSupported, iPlayFormat));
	}

/*
 *
 *	Initializes CMMFDevSoundProxy object to play and record PCM16 raw audio data
 *	with sampling rate of 8 KHz.
 *
 *	On completion of Initialization, calls InitializeComplete() on
 *	aDevSoundObserver.
 *
 *	Leaves on failure.
 *
 *	@param	"MDevSoundObserver& aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"TMMFState aMode"
 *			Mode for which this object will be used.
 *
 */
void CMMFDevSoundSvrImp::InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)

	{
	// if no HwDevice id specified, load default null implementation
	TUid rawUid = {KMmfUidHwDevicePCM16ToPCM16};
	InitializeL(aDevSoundObserver, rawUid, aMode);
	__ASSERT_DEBUG(!(iHasPolicy&&(iMode == EMMFDevSoundStatePlaying)), Panic(TMMFDevSoundSessionPolicyNotInvalidated));
	}

/*
 *
 *	Configure CMMFDevSoundProxy object for the settings in aConfig.
 *
 *	Use this to set sampling rate, Encoding and Mono/Stereo.
 *
 *	@param	"TMMFCapabilities& aConfig"
 *			Attribute values to which CMMFDevSoundProxy object will be configured to.
 *
 *  As part of defect 20796, the iRecordFormat has been set under the iPlayFormat, 
 *  before it was not set at all.
 *
 */
void CMMFDevSoundSvrImp::SetConfigL(const TMMFCapabilities& aConfig)
	{
	TUint attributeValue = aConfig.iRate;
	// WINS supports from 8000 Hz to 96000 Hz
	// The ToneGenerator currently supports sample rate of 8000Hz only. 
	if(iMode == EMMFDevSoundStateTonePlaying && (attributeValue != EMMFSampleRate8000Hz ))
		{
		User::Leave(KErrNotSupported);
		}
	
	if (attributeValue & EMMFSampleRate96000Hz) 
		{ 
		iPlayFormat().iRate = 96000; 
		iRecordFormat().iRate = 96000;
		iDeviceConfig.iRate = EMMFSampleRate96000Hz;
		}
	else if (attributeValue & EMMFSampleRate88200Hz) 
		{ 
		iPlayFormat().iRate = 88200; 
		iRecordFormat().iRate = 88200;
		iDeviceConfig.iRate = EMMFSampleRate88200Hz;
		}
	else if (attributeValue & EMMFSampleRate64000Hz) 
		{ 
		iPlayFormat().iRate = 64000; 
		iRecordFormat().iRate = 64000;
		iDeviceConfig.iRate = EMMFSampleRate64000Hz;
		}
	else if (attributeValue & EMMFSampleRate48000Hz) 
		{ 
		iPlayFormat().iRate = 48000; 
		iRecordFormat().iRate = 48000;
		iDeviceConfig.iRate = EMMFSampleRate48000Hz;
		}
	else if (attributeValue & EMMFSampleRate44100Hz) 
		{ 
		iPlayFormat().iRate = 44100; 
		iRecordFormat().iRate = 44100;
		iDeviceConfig.iRate = EMMFSampleRate44100Hz;
		}
	else if (attributeValue & EMMFSampleRate32000Hz) 
		{ 
		iPlayFormat().iRate = 32000; 
		iRecordFormat().iRate = 32000; 
		iDeviceConfig.iRate = EMMFSampleRate32000Hz;
		}
	else if (attributeValue & EMMFSampleRate24000Hz)
		{
		iPlayFormat().iRate = 
		iRecordFormat().iRate = 24000;
		iDeviceConfig.iRate = EMMFSampleRate24000Hz;
		}
	else if (attributeValue & EMMFSampleRate22050Hz)
		{ 
		iPlayFormat().iRate = 22050; 
		iRecordFormat().iRate = 22050; 
		iDeviceConfig.iRate = EMMFSampleRate22050Hz;
		}
	else if (attributeValue & EMMFSampleRate16000Hz)
		{
		iPlayFormat().iRate = 16000;
		iRecordFormat().iRate = 16000; 
		iDeviceConfig.iRate = EMMFSampleRate16000Hz;
		}
	else if (attributeValue & EMMFSampleRate12000Hz)
		{
		iPlayFormat().iRate = 
		iRecordFormat().iRate = 12000;
		iDeviceConfig.iRate = EMMFSampleRate12000Hz;
		}
	else if (attributeValue & EMMFSampleRate11025Hz)
		{
		iPlayFormat().iRate = 11025;
		iRecordFormat().iRate = 11025;
		iDeviceConfig.iRate = EMMFSampleRate11025Hz;
		}
	else if (attributeValue & EMMFSampleRate8000Hz)
		{
		iPlayFormat().iRate = 8000;
		iRecordFormat().iRate = 8000;
		iDeviceConfig.iRate = EMMFSampleRate8000Hz;
		}
	else if (attributeValue) 
		{ //if no attribute value assume its not set
		User::Leave(KErrNotSupported);
		}

	attributeValue = aConfig.iEncoding;
	// Map from MMF Encoding enums to RMdaDevSound enum
	if(attributeValue & EMMFSoundEncoding8BitPCM) 
		{
		iPlayFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitPCM;
		iRecordFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitPCM;
		iDeviceConfig.iEncoding = EMMFSoundEncoding8BitPCM;
		}
	else if(attributeValue & EMMFSoundEncoding8BitALaw)
		{
		iPlayFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitALaw;
		iRecordFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitALaw;
		iDeviceConfig.iEncoding = EMMFSoundEncoding8BitALaw;
		}
	else if(attributeValue & EMMFSoundEncoding8BitMuLaw) 
		{
		iPlayFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitMuLaw;
		iRecordFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitMuLaw;
		iDeviceConfig.iEncoding = EMMFSoundEncoding8BitMuLaw;
		}
	else if(attributeValue & EMMFSoundEncoding16BitPCM)
		{
		iPlayFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
		iRecordFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
		iDeviceConfig.iEncoding = EMMFSoundEncoding16BitPCM;
		}
	else if (attributeValue) 
		{ //if no attribute value assume its not set
		User::Leave(KErrNotSupported);
		}

	// Mono/Stereo settings
	attributeValue = aConfig.iChannels;
	if(iMode == EMMFDevSoundStateTonePlaying && (attributeValue != EMMFMono ))
		{
		User::Leave(KErrNotSupported);
		}
	if(attributeValue & EMMFStereo) 
		{
		iPlayFormat().iChannels = 2;
		iRecordFormat().iChannels = 2;
		iDeviceConfig.iChannels = EMMFStereo;
		}
	else if(attributeValue & EMMFMono)
		{
		iPlayFormat().iChannels = 1;
		iRecordFormat().iChannels = 1;
		iDeviceConfig.iChannels = EMMFMono;
		}
	else if (attributeValue) 
		{ //if no attribute value assume its not set
		User::Leave(KErrNotSupported);
		}
	}

/*
 *
 *	Changes the current playback volume to a specified value.
 *
 *	The volume can be changed before or during playback and is effective
 *	immediately.
 *
 *	@param	"TInt aVolume"
 *			The volume setting. This can be any value from zero to the value
 *			returned by a call to CMdaAudioPlayerUtility::MaxVolume(). If the
 *			volume is not within this range, the volume is automatically set to
 *			minimum or maximum value based on the value that is being passed.
 *			Setting a zero value mutes the sound. Setting the maximum value
 *			results in the loudest possible sound.
 *
 */
void CMMFDevSoundSvrImp::SetVolume(TInt aVolume)
	{

	// Check and make sure that the volume is in valid range
	if (aVolume < 0)
		aVolume = 0;

	if (aVolume > MaxVolume())
		aVolume = MaxVolume();

	iVolume = aVolume;

	SetDeviceVolume(iVolume);
	}

/*
 *
 *	Changes the current recording gain to a specified value.
 *
 *	The gain can be changed before or during recording and is effective
 *	immediately.
 *
 *	@param	"TInt aGain"
 *			The volume setting. This can be any value from zero to the value
 *			returned by a call to CMdaAudioPlayerUtility::MaxVolume(). If the
 *			volume is not within this range, the gain is automatically set to
 *			minimum or maximum value based on the value that is being passed.
 *			Setting a zero value mutes the sound. Setting the maximum value
 *			results in the loudest possible sound.
 *
 */
void CMMFDevSoundSvrImp::SetGain(TInt aGain)
	{
	// make sure it falls with the correct range
	TInt maxGain = iRecordFormatsSupported().iMaxVolume;
	if (aGain > maxGain)
		aGain = maxGain;
	else if (aGain < 0)
		aGain = 0;
	iGain = aGain;
	SetDeviceRecordLevel(iGain);
	}

/*
 *
 *	Sets the speaker balance for playing.
 *
 *	The speaker balance can be changed before or during playback and is
 *	effective immediately.
 *
 *	@param	"TInt& aLeftPercentage"
 *			On return contains left speaker volume perecentage. This can be any
 *			value from zero to 100. Setting a zero value mutes the sound on left
 *			speaker.
 *
 *	@param	"TInt& aRightPercentage"
 *			On return contains right speaker volume perecentage. This can be any
 *			value from zero to 100. Setting a zero value mutes the sound on
 *			right speaker.
 *
 */
void CMMFDevSoundSvrImp::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	if (aLeftPercentage < 0)
		aLeftPercentage = 0;
	else if (aLeftPercentage > 100)
		aLeftPercentage = 100;
	if (aRightPercentage < 0)
		aRightPercentage = 0;
	else if (aRightPercentage > 100)
		aRightPercentage = 100;
	iLeftPlayBalance = aLeftPercentage;
	iRightPlayBalance = aRightPercentage;
	}

/*
 *
 *	Sets the microphone gain balance for recording.
 *
 *	The microphone gain balance can be changed before or during recording and
 *	is effective immediately.
 *
 *	@param	"TInt aLeftPercentage"
 *			Left microphone gain precentage. This can be any value from zero to
 *			100. Setting a zero value mutes the gain on left microphone.
 *
 *	@param	"TInt aRightPercentage"
 *			Right microphone gain precentage. This can be any value from zero to
 *			100. Setting a zero value mutes the gain on right microphone.
 *
 */
void CMMFDevSoundSvrImp::SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	if (aLeftPercentage < 0)
		aLeftPercentage = 0;
	else if (aLeftPercentage > 100)
		aLeftPercentage = 100;
	if (aRightPercentage < 0)
		aRightPercentage = 0;
	else if (aRightPercentage > 100)
		aRightPercentage = 100;
	iLeftRecordBalance = aLeftPercentage;
	iRightRecordBalance = aRightPercentage;
	}

/*
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
void CMMFDevSoundSvrImp::PlayInitL()
	{
	if (!iDevSoundObserver)
		User::Leave(KErrNotReady);

	if (iAudioPolicyPrioritySettings.iState == EMMFStatePlayData)
		{
		// If policy has not been obtaing then ignore the request.
		// If it has then do a stop and start action on HW device
		// without informing policy.
		
		if (iHasPolicy)
			{
			if (iCMMFHwDevice)
				{
				TInt err = iCMMFHwDevice->Stop();
				if (err == KErrNone)
					{
					err = iCMMFHwDevice->Start(EDevDecode, EDevOutFlow);
					}
				
				if (err != KErrNone)
					{
					Error(err);
					}
				}
			}
		return;
		}

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStatePlayData;
	RequestPolicy();
	}

/*
 *
 *	Initializes audio device and start record process. 
 *  This method:
 *  1. Queries and acquires the audio policy before initializing audio device. 
 *     If there was an error during policy initialization, RecordError() method will 
 *     be called on the observer with error code KErrAccessDenied, otherwise BufferToBeEmptied()
 *	   method will be called with a buffer reference. This buffer contains recorded
 *	   or encoded data. After processing data in the buffer reference passed, the
 *	   client should call RecordData() to continue recording process.
 *
 *  2. Checks if the requesting client process has a UserEnvironment capability.
 *     If it does not, the audio device will not be initialized and an error 
 *     code KErrAccessDenied will be sent to the client.
 *
 *	The amount of data that is available is specified in
 *	CMMFBuffer::RequestSize().
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSvrImp::RecordInitL(const RMmfIpcMessage& aMessage)
	{
	if (!iDevSoundObserver)
		User::Leave(KErrNotReady);
	
	// Checkes if the client has the UserEnvironment capability
	if (!aMessage.HasCapability(ECapabilityUserEnvironment))
		{
		User::Leave(KErrPermissionDenied);
		}

	if (iAudioPolicyPrioritySettings.iState == EMMFStateRecordData)
		{
		if (iHasPolicy)
			{
			if (iCMMFHwDevice)
				{
				TInt err = iCMMFHwDevice->Stop();
				if (err == KErrNone)
					{
					err = iCMMFHwDevice->Start(EDevDecode, EDevOutFlow);
					}
				
				if (err != KErrNone)
					{
					Error(err);
					}
				
				if (iHwDeviceBuffer)
					{
					iHwDeviceBuffer->SetLastBuffer(EFalse);
					}
				}
			}
		
		return;
		}

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStateRecordData;
	RequestPolicy();
	}

/*
 *
 *	Plays data in the buffer at the current volume. The client should fill
 *	the buffer with audio data before calling this method. The Observer gets
 *	reference to buffer along with callback BufferToBeFilled(). When playing of
 *	the audio sample is complete, successfully or otherwise, the method
 *	PlayError() on observer is called.
 *
 */
TBool CMMFDevSoundSvrImp::PlayData(const RMmfIpcMessage& aMessage)
	{
	ASSERT(iDevSoundObserver);

	if (iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		// Initialisation has been successful but an error (possibly underflow) has occurred
		// which the controller has yet to respond to and that has changed the state to stopped.
		// Allow the call to complete and processing to continue...  
		return ETrue;
		}

	if((iAudioPolicyPrioritySettings.iState != EMMFStatePlayData) && (iAudioPolicyPrioritySettings.iState != EMMFStatePaused))
		{
		PanicClient(aMessage, EMMFDevSoundPlayDataWithoutInitialize);
		return EFalse;
		}
	if (iMode== EMMFDevSoundStateIdle)
		{
		return ETrue;
		}
	TInt error = KErrNone;
    if(iCMMFHwDevice)
	    {
		if (iPaused)
			{
			iPaused = EFalse;
			//note PlayData does not leave or return an error code so the Start() fails we cannot
			//report the error back at this point
			error = iCMMFHwDevice->Start(EDevDecode, EDevOutFlow);//restart hw device after pause
			}
	   else if ((iMode == EMMFDevSoundStatePlaying) && iHasPolicy)
		    {
		    TInt len = iHwDeviceBuffer->Data().Length();
		    if (iHwDeviceBuffer->LastBuffer())
		    	{
		    	iLastBufferReceived = ETrue;
		    	}
			// Pass the data buffer to HwDevice
			error = iCMMFHwDevice->ThisHwBufferFilled(*iHwDeviceBuffer);
            }
		}
	if (error != KErrNone)
		{
		Error(error);
		if(iCMMFHwDevice)
			{
			iCMMFHwDevice->Stop();	
			}

		}
	return ETrue;
	}
 
/*
 *
 *	Stops the ongoing operation (Play, Record, TonePlay, Convert)
 *
 */
void CMMFDevSoundSvrImp::Stop()
	{
	iPaused = EFalse;

	if (!iHasPolicy)
		{
		UpdatePolicyState(EMMFStateStopped);
		return;
		}

	 // Stop the hw device first - this unloads sound drivers
    if (iCMMFHwDevice)
    	{
		iCMMFHwDevice->Stop();
		}

	iDevSoundEventHandler->CancelReceiveEvents();

	UpdatePolicyState(EMMFStateStopped);
	__ASSERT_DEBUG(!(iHasPolicy&&(iMode == EMMFDevSoundStatePlaying)), Panic(TMMFDevSoundSessionPolicyNotInvalidated)); 
	}

/*
 *
 *	Temporarily Stops the ongoing operation (Play, Record, TonePlay, Convert)
 *
 */
void CMMFDevSoundSvrImp::Pause()
	{
	iPaused = ETrue;

	if (!iHasPolicy)
		{
		return;
		}

	// Pause the HW device first
	if (iCMMFHwDevice)
		{
		iCMMFHwDevice->Pause();
		}
	}

/*
 *
 *	Returns the sample recorded so far.
 *
 *	@return "TInt"
 *			Returns the samples recorded.
 *
 */
TInt CMMFDevSoundSvrImp::SamplesRecorded()
	{
	TInt samples = 0;
	
	if(iRecordCustomInterface)
		{
		samples = iRecordCustomInterface->BytesRecorded();
		if(NumberOfChannels() > 1)
			{
			samples /= NumberOfChannels();
			}
		if(BytesPerAudioSample() > 1)
			{
			samples /= BytesPerAudioSample();
			}
		}

	return samples;
	}

/*
 *
 *	Returns the sample played so far.
 *
 *	@return "TInt"
 *			Returns the samples recorded.
 *
 */
TInt CMMFDevSoundSvrImp::SamplesPlayed()
	{
	TInt samples = 0;
	if(iPlayCustomInterface)
		{
		TUint bytesPlayed = iPlayCustomInterface->BytesPlayed();
		if (bytesPlayed)
			iPlayedBytesCount = bytesPlayed;

		samples = iPlayedBytesCount;
		if(NumberOfChannels() > 1)
			samples /= NumberOfChannels();

		if(BytesPerAudioSample() > 1)
			samples /= BytesPerAudioSample();
		}
	//note always pcm16 becuase the iPlayedBytesCount originates from 
	//RMdaDevSound which is always pcm16
	return samples; //each sample is 2 bytes
	}


/*
 *
 *	Initializes audio device and start playing tone. Tone is played with
 *	frequency and for duration specified.
 *
 *	Leaves on failure.
 *
 *	@param	"TInt aFrequency"
 *			Frequency at with the tone will be played.
 *
 *	@param	"TTimeIntervalMicroSeconds& aDuration"
 *			The period over which the tone will be played. A zero value causes
 *			the no tone to be played (Verify this with test app).
 *
 */
void CMMFDevSoundSvrImp::PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	if (iMode != EMMFDevSoundStateTonePlaying)
		{
		User::Leave(KErrNotSupported); //tone playing only supported in tone play state
		}

	// Check whether frequency and duration is valid or not
	TInt64 zeroInt64(0);
	if ((aFrequency<0) || (aDuration.Int64() < zeroInt64))
		{
		User::Leave(KErrArgument);
		}

	if (!iDevSoundObserver)
		{
		User::Leave(KErrNotReady);
		}

	iToneGen.SetFrequencyAndDuration(aFrequency,aDuration);

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStatePlayTone;
	RequestPolicy();
	}

/*
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
void CMMFDevSoundSvrImp::PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	if (!iDevSoundObserver)
		{
		User::Leave(KErrNotReady);
		}
	
	// Check whether frequencies and duration are valid or not
	TInt64 zeroInt64(0);
	if ((aFrequencyOne<0) || (aFrequencyTwo<0) || (aDuration.Int64() < zeroInt64))
		{
		iDevSoundObserver->ToneFinished(KErrArgument);
		return;
		}


	iDualToneGen.SetFrequencyAndDuration(aFrequencyOne, aFrequencyTwo, aDuration);

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStatePlayDualTone;
	RequestPolicy();
	}

/*
 *
 *	Initializes audio device and start playing DTMF string aDTMFString.
 *
 *	Leaves on failure.
 *
 *	@param	"TDesC& aDTMFString"
 *			DTMF sequence in a descriptor.
 *
 */
void CMMFDevSoundSvrImp::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	if (!iDevSoundObserver)
		User::Leave(KErrNotReady);

	if (iMode!= EMMFDevSoundStateTonePlaying)
		User::Leave(KErrNotSupported); //tone playing only supported in tone play state

	iDTMFGen.SetString(aDTMFString);

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStatePlayDTMFString;
	RequestPolicy();
	}

/*
 *
 *	Initializes audio device and start playing tone sequence.
 *
 *	Leaves on failure.
 *
 *	@param	"TDesC8& aData"
 *			Tone sequence in a descriptor.
 *
 */
void CMMFDevSoundSvrImp::PlayToneSequenceL(const TDesC8& aData)
	{
	if (!iDevSoundObserver)
		User::Leave(KErrNotReady);

	if (iMode!= EMMFDevSoundStateTonePlaying)
		User::Leave(KErrNotSupported); //tone playing only supported in tone play state

	// Check whether the sequence is valid or not
	if (!iDevSoundUtil->RecognizeSequence(aData))
		User::Leave(KErrCorrupt);

	iSequenceGen.SetSequenceData(aData);

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStatePlayToneSequence;
	RequestPolicy();
	}

/*
 *
 *	Initializes audio device and start playing the specified pre-defined tone
 *	sequence.
 *
 *	Leaves on failure.
 *
 *	@param	"TInt aSequenceNumber"
 *			The index identifying the specific pre-defined tone sequence. Index
 *			values are relative to zero.
 *			This can be any value from zero to the value returned by a call to
 *			FixedSequenceCount() - 1.
 *			The function raises a panic if sequence number is not within this
 *			range.
 *
 *	@see	FixesSequenceCount()
 *
 */
void CMMFDevSoundSvrImp::PlayFixedSequenceL(TInt aSequenceNumber)
	{
	if (!iDevSoundObserver)
		User::Leave(KErrNotReady);

	if (iMode!= EMMFDevSoundStateTonePlaying)
		User::Leave(KErrNotSupported); //tone playing only supported in tone play state

	ASSERT((aSequenceNumber >= 0)&&(aSequenceNumber < iFixedSequences->Count()));

	iFixedSequence.Set(iFixedSequences->MdcaPoint(aSequenceNumber));
	iSequenceGen.SetSequenceData(iFixedSequence);

	// Get audio policy
	iAudioPolicyPrioritySettings.iState = EMMFStatePlayToneSequence;
	RequestPolicy();
	}

/*
 *
 *	Defines the duration of tone on, tone off and tone pause to be used during the
 *	DTMF tone playback operation.
 *
 *	Supported only during tone playing.
 *
 *	@param	"TTimeIntervalMicroSeconds32& aToneOnLength"
 *			The period over which the tone will be played. If this is set to
 *			zero, then the tone is not played.
 *
 *	@param	"TTimeIntervalMicroSeconds32& aToneOffLength"
 *			The period over which the no tone will be played.
 *
 *	@param	"TTimeIntervalMicroSeconds32& aPauseLength"
 *			The period over which the tone playing will be paused.
 *
 */
void CMMFDevSoundSvrImp::SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
								TTimeIntervalMicroSeconds32& aToneOffLength,
								TTimeIntervalMicroSeconds32& aPauseLength) 
	{

	if(aToneOnLength.Int() < KMdaInfiniteDurationDTMFToneOnLength)
		aToneOnLength = TTimeIntervalMicroSeconds32(0);
	if(aToneOffLength.Int() < 0)
		aToneOffLength = TTimeIntervalMicroSeconds32(0);
	if(aPauseLength.Int() < 0)
		aPauseLength = TTimeIntervalMicroSeconds32(0);

	iDTMFGen.SetToneDurations(aToneOnLength,aToneOffLength,aPauseLength);
	}

/*
 *
 *	Defines the period over which the volume level is to rise smoothly from
 *	nothing to the normal volume level.
 *
 *	@param	"TTimeIntervalMicroSeconds& aRampDuration"
 *			The period over which the volume is to rise. A zero value causes 
 *			the tone sample to be played at the normal level for the full
 *			duration of the playback. A value, which is longer than the duration
 *			of the tone sample, that the sample never reaches its normal
 *			volume level.
 *
 *
 */
void CMMFDevSoundSvrImp::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	// save ramp duration for tone generator
	iRampDuration = aRampDuration;

	SetDeviceVolumeRamp(iRampDuration);
	}

/**
 *	Sets volume ramp on HwDevice.
 */
TInt CMMFDevSoundSvrImp::SetDeviceVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	TInt error = KErrNone;
	if (iPlayCustomInterface) 
		iPlayCustomInterface->SetVolumeRamp(aRampDuration);
	else
		error = KErrNotReady;
	return error;
    }

/**
 *	@see sounddevice.h
 */
void CMMFDevSoundSvrImp::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& /*aPrioritySettings*/) const
	{
	//aPrioritySettings not used on ref DevSound
	//search for playing datatypes
	iDevSoundUtil->SeekHwDevicePluginsL(aSupportedDataTypes, EMMFStatePlaying);
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	// append list of codec plugins
	iDevSoundUtil->SeekCodecPluginsL(aSupportedDataTypes, EMMFStatePlaying, ETrue);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	}

/**
 *	@see sounddevice.h
 */
void CMMFDevSoundSvrImp::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& /*aPrioritySettings*/) const
	{
	//aPrioritySettings not used on ref DevSound
	// search for recording datatypes
	iDevSoundUtil->SeekHwDevicePluginsL(aSupportedDataTypes, EMMFStateRecording);
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	// append list of codec plugins
	iDevSoundUtil->SeekCodecPluginsL(aSupportedDataTypes, EMMFStateRecording, ETrue);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	}
	
TInt CMMFDevSoundSvrImp::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{	
	return iAudioPolicyProxy->RequestResourceNotification(aEventType,aNotificationRegistrationData);
	}
	
TInt CMMFDevSoundSvrImp::CancelRegisterAsClient(TUid aEventType)
	{	
	return iAudioPolicyProxy->CancelRequestResourceNotification(aEventType);
	}

TInt CMMFDevSoundSvrImp::GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData)
	{	
	TInt err = KErrNone;
	err = iAudioPolicyProxy->IsRegisteredResourceNotification(aEventType);
	if(err == KErrNone)
		{
		TMMFTimeIntervalMicroSecondsPckg pckg = TTimeIntervalMicroSeconds(SamplesPlayed());
		aNotificationData.Copy(pckg);
		}
	return err;
	}
	
TInt CMMFDevSoundSvrImp::WillResumePlay()
	{
	return iAudioPolicyProxy->StopNotification();
	}	
	
TInt CMMFDevSoundSvrImp::EmptyBuffers()
	{
	TInt err = KErrNone;
	if (iMode != EMMFStatePlaying)
		{
		return KErrNotSupported;
		}
	if (!iCMMFHwDevice)
		{
		return KErrNotReady;			
		}	
	MEmptyBuffersCustomInterface* emptybuffers = reinterpret_cast<MEmptyBuffersCustomInterface*>(iCMMFHwDevice->CustomInterface(KEmptyBuffersCustomInterfaceTypeUid));	
	if (emptybuffers)
    	{
       	err = emptybuffers->EmptyBuffers();
     	}
    else
		{
     	err = KErrNotSupported;
     	}
    return err;	
	}	

TInt CMMFDevSoundSvrImp::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds time(0);
	if(iCMMFHwDevice)
		{
		if(!iTimePlayedCustomInterface)
			{
			iTimePlayedCustomInterface = reinterpret_cast<MTimePlayedCustomInterface*>(iCMMFHwDevice->CustomInterface(KTimePlayedCustomInterfaceTypeUid));
			if(iTimePlayedCustomInterface == NULL)
				{
				return KErrNotSupported;
				}
			}
		err = iTimePlayedCustomInterface->GetTimePlayed(time);
	    if(err == KErrNone && time.Int64() > 0)
	    	{
	    	iTimePlayed = time.Int64();
	    	}
		}
	aTime = iTimePlayed.Int64();
	return err;
	}

/********************************************************************************
 *				Implementations of Non Exported public functions begins here	*
 ********************************************************************************/

//
//				Audio Policy specific implementation begins here				//
//

/*
 *
 *	Called by Audio Policy Server when a request to play is approved by the 
 *	Audio Policy Server.
 *
 *	Leaves on failure??.
 *
 */
void CMMFDevSoundSvrImp::StartPlayDataL()
	{
	ASSERT(iMode== EMMFDevSoundStatePlaying);
	if(iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		return;
		}
	iHasPolicy = ETrue;

	TInt error = KErrNone;

	if(iCMMFHwDevice)
		{
		UpdatePolicyState(EMMFStatePlayData);		
        // Set volume and play format values
        error = SetPlayFormat(iPlayFormat);
        if (error == KErrNone)
			error = SetDeviceVolume(iVolume);
        if (error == KErrNone)
			error = SetDeviceVolumeRamp(iRampDuration);

		// Initialize attribute values
		iPlayedBytesCount = 0;
		iTimePlayed = 0;
		iLastBufferReceived = EFalse;

        // Start HwDevice
        if (error == KErrNone)
	        error = iCMMFHwDevice->Start(EDevDecode, EDevOutFlow);
		}
	else
		error = KErrNotReady;
	
	if (error != KErrNone)
		{
		Error(error);

		}
	}

/*
 *
 *	Called by Audio Policy Server when a request to record is approved by the 
 *	Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSvrImp::StartRecordDataL()
	{
	ASSERT(iMode== EMMFDevSoundStateRecording);
	if(iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		return;
		}
	iHasPolicy = ETrue;

	 if(iCMMFHwDevice)
		{
		UpdatePolicyState(EMMFStateRecordData);
        TInt error = KErrNone;
        error = SetRecordFormat(iRecordFormat);
		if (error != KErrNone)
			{
			Error(error);
			return;

			}
        error = SetDeviceRecordLevel(iGain);
		if (error != KErrNone)
			{
			Error(error);
			return;

			}
        error = iCMMFHwDevice->Start(EDevEncode, EDevInFlow);
		if (iHwDeviceBuffer)
			iHwDeviceBuffer->SetLastBuffer(EFalse);

		if (error != KErrNone)
			{
			Error(error);
			return;
			}
        }
	else
		iDevSoundObserver->RecordError(KErrNotReady);
	}

/*
 *
 *	Called by Audio Policy Server when a request to play tone is approved by
 *	the Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSvrImp::StartPlayToneL()
	{
	ASSERT(iMode == EMMFDevSoundStateTonePlaying);
	if(iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		return;
		}
	iHasPolicy = ETrue;
	
	if(iCMMFHwDevice)
		{
		UpdatePolicyState(EMMFStatePlayTone);
        TInt error = KErrNone;
        // Set volume and play format values
        error = SetPlayFormat(iPlayFormat);
		if (error != KErrNone)
			{
			Error(error);
			return;

			}
        if (iHwDeviceID.iUid == KMmfUidHwDevicePCM16ToPCM16)
            error = SetDeviceVolume(iVolume);
		else
			error = KErrGeneral;//hw device should always be pcm16 for tone

		// turn off volume ramping - this is done in software below
        if (error == KErrNone)
			error = SetDeviceVolumeRamp(TTimeIntervalMicroSeconds(0));

        if (error != KErrNone)
            {
            Error(error);
            return;

            }

        // Initialize attribute values
		iPlayedBytesCount = 0;
		iTimePlayed = 0;
        // Configure tone generator
        iToneGen.Configure(
			iPlayFormat().iRate,
			iPlayFormat().iChannels,
		    iRepeatCount,
			I64LOW((iRepeatTrailingSilence.Int64()*iPlayFormat().iRate)/1000000),
			I64LOW((iRampDuration.Int64()*iPlayFormat().iRate)/1000000)
			);

		iCurrentGenerator = &iToneGen;

        // Start playback
        DoPlayL();
		}
	else
		iDevSoundObserver->ToneFinished(KErrNotReady);
	}

/*
 *
 *	Called by Audio Policy Server when a request to play a dual tone is approved by
 *	the Audio Policy Server.
 *
 */
void CMMFDevSoundSvrImp::StartPlayDualToneL()
	{
	ASSERT(iMode == EMMFDevSoundStateTonePlaying);
	if(iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		return;
		}
	iHasPolicy = ETrue;

	 if(iCMMFHwDevice)
		{
		UpdatePolicyState(EMMFStatePlayDualTone);
        TInt error = KErrNone;
        // Set volume and play format values
        error = SetPlayFormat(iPlayFormat);
		if (error != KErrNone)
			{
			Error(error);
			return;
			}
        if (iHwDeviceID.iUid == KMmfUidHwDevicePCM16ToPCM16)
            error = SetDeviceVolume(iVolume);
		else 
			error = KErrGeneral;//hw device should always be pcm16 for tone

		// turn off volume ramping - this is done in software below
        if (error == KErrNone)
			error = SetDeviceVolumeRamp(TTimeIntervalMicroSeconds(0));

        if (error != KErrNone)
            {
            Error(error);
            return;

            }

        // Initialize attribute values
		iPlayedBytesCount = 0;
		iTimePlayed = 0;
        // Configure dual tone generator
		iDualToneGen.Configure(
			iPlayFormat().iRate,
			iPlayFormat().iChannels,
			iRepeatCount,
			I64LOW((iRepeatTrailingSilence.Int64()*iPlayFormat().iRate)/KOneMillionMicroSeconds),
			I64LOW((iRampDuration.Int64()*iPlayFormat().iRate)/KOneMillionMicroSeconds)
			);

		iCurrentGenerator = &iDualToneGen;

        // Start playback
        DoPlayL();
		}
	else
		iDevSoundObserver->ToneFinished(KErrNotReady);
	}

/*
 *
 *	Called by Audio Policy Server when a request to play DTMF String is approved
 *	by the Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSvrImp::StartPlayDTMFStringL()
	{
	ASSERT(iMode == EMMFDevSoundStateTonePlaying);
	if(iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		return;
		}
	iHasPolicy = ETrue;

	if(iCMMFHwDevice)
		{
		UpdatePolicyState(EMMFStatePlayDTMFString);
        TInt error = KErrNone;
        // Set volume and play format values
        error = SetPlayFormat(iPlayFormat);
		if (error != KErrNone)
            {
            Error(error);
            return;

            }
        error = SetDeviceVolume(iVolume);

		// turn off volume ramping - this is done in software below
        if (error == KErrNone)
			error = SetDeviceVolumeRamp(TTimeIntervalMicroSeconds(0));

        if (error != KErrNone)
            {
            Error(error);
            return;
            }

        // Initialize attribute values
		iPlayedBytesCount = 0;
		iTimePlayed = 0;
	    iDTMFGen.Configure(
			iPlayFormat().iRate,
			iPlayFormat().iChannels,
		    iRepeatCount,
			I64LOW((iRepeatTrailingSilence.Int64()*iPlayFormat().iRate)/1000000),
		    I64LOW((iRampDuration.Int64()*iPlayFormat().iRate)/1000000)
			);

		iCurrentGenerator = &iDTMFGen;

        // Start playback
		//need to trap this as we can leave with KErrUnderflow
		//if there was no data to play - the error has already
		//been sent to the observer and we don't want to call RunError
        TRAP(error,DoPlayL());
		if ((error != KErrUnderflow)&&(error != KErrNone))
			{
			User::Leave(error);
			}
		}
	else
		iDevSoundObserver->ToneFinished(KErrNotReady);
	}

/*
 *
 *	Called by Audio Policy Server when a request to play tone sequence is
 *	approved by the Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSvrImp::StartPlayToneSequenceL()
	{
	ASSERT(iMode == EMMFDevSoundStateTonePlaying);
	if(iAudioPolicyPrioritySettings.iState == EMMFStateStopped)
		{
		return;
		}
	iHasPolicy = ETrue;

	if(iCMMFHwDevice)
		{
		UpdatePolicyState(EMMFStatePlayToneSequence);
        TInt error = KErrNone;
        // Set volume and play format values
        if (iHwDeviceID.iUid == KMmfUidHwDevicePCM16ToPCM16)
            error = SetPlayFormat(iPlayFormat);
			else error = KErrGeneral;//hw device should always be pcm16 for tone
		if (error != KErrNone)
            {
            Error(error);
            return;
            }

        if (iHwDeviceID.iUid == KMmfUidHwDevicePCM16ToPCM16)
            error = SetDeviceVolume(iVolume);
		else 
			error = KErrGeneral;//hw device should always be pcm16 for tone

		// turn off volume ramping - this is done in software below
        if (error == KErrNone)
			error = SetDeviceVolumeRamp(TTimeIntervalMicroSeconds(0));

        if (error != KErrNone)
            {
            Error(error);
            return;
            }

        // Initialize attribute values
		iPlayedBytesCount = 0;
		iTimePlayed = 0;
		iSequenceGen.Configure(
			iPlayFormat().iRate,
			iPlayFormat().iChannels,
			iRepeatCount,
			I64LOW((iRepeatTrailingSilence.Int64()*iPlayFormat().iRate)/1000000),
			I64LOW((iRampDuration.Int64()*iPlayFormat().iRate)/1000000)
			);

		iCurrentGenerator = &iSequenceGen;

        // Start playback
        DoPlayL();
		}
	else
		iDevSoundObserver->ToneFinished(KErrNotReady);
	}

/*
 *
 *	Called by Audio Policy Server when the current DevSound instance looses the
 *	policy because of another instance with a higher priority wants the device.
 *
 */
void CMMFDevSoundSvrImp::SendEventToClient(const TMMFEvent& aEvent)
	{
	if (aEvent.iErrorCode != KErrNone)
		{
		iHasPolicy = EFalse;
		if (iMode == EMMFDevSoundStatePlaying)
			{
			UpdateBytesPlayed();
			iDevSoundObserver->PlayError(aEvent.iErrorCode);
			UpdatePolicyState(EMMFStateStopped);
			}
		else if (iMode == EMMFDevSoundStateRecording)
			{
			iDevSoundObserver->RecordError(aEvent.iErrorCode);
			UpdatePolicyState(EMMFStateStopped);
			}
		else if (iMode == EMMFDevSoundStateTonePlaying)
			{
			iDevSoundObserver->ToneFinished(aEvent.iErrorCode);
			}			
		iCMMFHwDevice->Stop();//unloads sound device
		// Have audio Policy launch higher priority request
		iAudioPolicyProxy->LaunchRequests();
		}
	else
		{
		iHasPolicy = ETrue;
		iDevSoundObserver->SendEventToClient(aEvent);
		}
	}


/**
 *
 *	Sets volume on HwDevice.
 *	
 *	@return	"TInt"
 *			Error value returned by HwDevice.
 *
 */
TInt CMMFDevSoundSvrImp::SetDeviceVolume(TInt aVolume)
	{
	TInt error = KErrNone;
	if (iPlayCustomInterface) 
		iPlayCustomInterface->SetVolume(aVolume);
	else error = KErrNotReady;
	return error;
    }

/**
 *
 *	Sets PlayFormat on HwDevice.
 *	
 *
 *	@return	"TInt"
 *			Error value returned by HwDevice.
 *
 */
TInt CMMFDevSoundSvrImp::SetPlayFormat(RMdaDevSound::TCurrentSoundFormatBuf& aPlayFormat)
	{
	TInt error = KErrNone;
	if (iCMMFHwDevice)
		{
		TTaskConfig taskConfig;
		taskConfig.iUid = KUidRefDevSoundTaskConfig;
		taskConfig.iRate = aPlayFormat().iRate;
		
		if (aPlayFormat().iChannels == 1)
			{
			taskConfig.iStereoMode = ETaskMono;
			}
		else if (aPlayFormat().iChannels == 2)
			{
			taskConfig.iStereoMode = ETaskInterleaved;
			}
		else
			{
			return KErrArgument;
			}

		error = iCMMFHwDevice->SetConfig(taskConfig);
		//note the iEncoding and iBufferSize are already determined by the 
		//CMMFHwDevice plugin and so are not set.
		}
	else
		{
		error = KErrNotReady;
		}
	return error;
    }


/**
 *
 *	Sets RecordFormat on HwDevice.
 *	
 *
 *	@return	"TInt"
 *			Error value returned by HwDevice.
 *
 */
TInt CMMFDevSoundSvrImp::SetRecordFormat(RMdaDevSound::TCurrentSoundFormatBuf& aRecordFormat)
	{
	TInt error = KErrNone;
	if (iCMMFHwDevice)
		{
		TTaskConfig taskConfig;
		taskConfig.iUid = KUidRefDevSoundTaskConfig;
		taskConfig.iRate = aRecordFormat().iRate;

		if (aRecordFormat().iChannels == 1)
			{
			taskConfig.iStereoMode = ETaskMono;
			}
		else if (aRecordFormat().iChannels == 2)
			{
			taskConfig.iStereoMode = ETaskInterleaved;
			}
		else
			{
			return KErrArgument;
			}

		error = iCMMFHwDevice->SetConfig(taskConfig);
		//note the iEncoding and iBufferSize are already determined by the 
		//CMMFHwDevice plugin and so are not set.
		}
	else
		{
		error = KErrNotReady;
		}
	return error;
    }


/**
 *
 *	Sets record level on HwDevice.
 *	
 *
 *	@return	"TInt"
 *			Error value returned by HwDevice.
 *
 */
TInt CMMFDevSoundSvrImp::SetDeviceRecordLevel(TInt aGain)
	{
	TInt error = KErrNone;
	if (iRecordCustomInterface) 
		iRecordCustomInterface->SetGain(aGain);
	else error = KErrNotReady;
	return error;

    }


/**
 *
 *	MMMFHwDeviceObserver mixin implementation.
 *
 *	The CMMFHwDevice implementation object calls this method during decoding
 *	(playing), when it needs the encoded data in the buffer
 *	aHwDataBuffer.
 *
 *	@return	"TInt"
 *			Error code. KErrNone if success.
 *
 */
TInt CMMFDevSoundSvrImp::FillThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	TInt err = KErrNone;
    // Keep a reference to this Hw data Buffer. We need to send the 
	// reference back to HwDevice implementation
	iHwDeviceBuffer = static_cast<CMMFDataBuffer*> (&aHwDataBuffer);
	// Set the request length, From HwDevice this comes with buffer
	// length.
	TInt len = iHwDeviceBuffer->Data().MaxLength();
	// Ignore error. since buffer size = Buffer Length 
	TRAP(err, iHwDeviceBuffer->SetRequestSizeL(len));

	if (iMode== EMMFDevSoundStatePlaying) // Get Data from Observer
		{
		if (iLastBufferReceived)
			{
			iHwDeviceBuffer->Data().SetLength(0);
			// Pass the buffer to the he device
            err = iCMMFHwDevice->ThisHwBufferFilled(*iHwDeviceBuffer);
			}
		else
			// Pass the buffer to the observer
			iDevSoundObserver->BufferToBeFilled(&aHwDataBuffer);
		}
	else if (iMode == EMMFDevSoundStateTonePlaying)
		{
        // Hw device will call this method right after its Start was called.
        // When it calls this for the first time it hasn't played one single
        // buffer yet so check that.
        // In this case there's no need to set the active buffer as it's already
        // waiting to be played.
        if (!iFirstCallFromHwDevice)
            SetActiveToneBuffer();

		// If there is no data in the active buffer, tone play is finished.
		// DevSound just have to wait for completion event from audio device.
		if (iActiveToneBuffer->Data().Length() > 0)
            { 
			TInt tonelen = iActiveToneBuffer->Data().Length();

			// don't enter more data than can be handled by the receiving buffer
			if (len >= tonelen) len = tonelen;

            // Copy data from tone buffer to hw device buffer
            Mem::Copy((TAny*)(iHwDeviceBuffer->Data().Ptr()), (TAny*)(iActiveToneBuffer->Data().Ptr()), len);
            
            iHwDeviceBuffer->Data().SetLength(len);
            // Play data and try to generate next data block
			err = iCMMFHwDevice->ThisHwBufferFilled(*iHwDeviceBuffer);
            if (err != KErrNone)
            	return err;
            // Check again whether this is the first call from Hw device.
            // FillFreeToneBuffer assumes the iActiveToneBuffer has already
            // been played.
            if (!iFirstCallFromHwDevice)
                err = FillFreeToneBuffer();
            else
                iFirstCallFromHwDevice = EFalse;  // Reset flag

 			}
		else if (iFirstCallFromHwDevice)
			{//we have no data in the tone buffer and thus have no 
			//outstanding requests to play
			err = KErrUnderflow; //simulate underrun
			}
		else
			{
			iHwDeviceBuffer->Data().SetLength(0);
			iHwDeviceBuffer->SetLastBuffer(ETrue);
            // Send an empty last buffer to HwDevice to get a play completion event with KErrUnderflow
			err = iCMMFHwDevice->ThisHwBufferFilled(*iHwDeviceBuffer);
			}
	
		// If there was an error filling the buffer could be corrupt data
		// notify the client and stop playing.Set err to KErrNone. 
		if (err != KErrNone)
			{
			Error(err);//Updates Bytes played informs client
			err = KErrNone;
			iCMMFHwDevice->Stop();//unloads sound device
            }
		}
	else
		{
		err = KErrGeneral;
		Error(err);
		iCMMFHwDevice->Stop();//unloads sound device

		}
	return err;
	}


/**
 *
 *	MMMFHwDeviceObserver mixin implementation.
 *
 *	The CMMFHwDevice implementation object calls this method during encoding
 *	(recording), when it fills the buffer aHwDataBuffer with
 *	encoded data.
 *
 *	@return	"TInt"
 *			Error code. KErrNone if success.
 *
 */
TInt CMMFDevSoundSvrImp::EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	TInt err = KErrNone;
	if(iMode== EMMFDevSoundStateRecording)
		{
		// Keep a reference to this Hw data Buffer. We need to send the 
		// reference back to HwDevice implementation
		iHwDeviceBuffer = static_cast<CMMFDataBuffer*>(&aHwDataBuffer);

		// Set the request length, From HwDevice this comes with buffer
		// length. MMF will use RequestSize attribute of the buffer.
		// We can avoid this by setting in HwDevice implemenation
		TInt len = iHwDeviceBuffer->Data().Length();
		TRAP(err, iHwDeviceBuffer->SetRequestSizeL(len));
	
		// if we're pausing (i.e. flushing) set the last buffer flag
		// when we get an empty buffer from the logical driver
		if(iPaused  && iHwDeviceBuffer->Data().Length() == 0)
		{
		iPaused = EFalse;

		iHwDeviceBuffer->SetLastBuffer(ETrue);

		iDevSoundEventHandler->CancelReceiveEvents();

		}

		// Send Data from Observer
		iDevSoundObserver->BufferToBeEmptied(iHwDeviceBuffer);
		}
	else
		{
		err = KErrGeneral;
		Error(err);
		iCMMFHwDevice->Stop();//unloads sound device

		}

	return err;
	}


/**
 *
 *	MMMFHwDeviceObserver mixin implementation.
 *
 *	The CMMFHwDevice implementation object calls this method when a message from
 *	the hardware device implementation is received.
 *
 *	@return	"TInt"
 *			Error code. KErrNone if success.
 *
 */
TInt CMMFDevSoundSvrImp::MsgFromHwDevice(TUid aMessageType, const TDesC8& /*aMsg*/)
	{
	TInt result = KErrNotSupported;
	if (aMessageType.iUid == KMmfHwDeviceObserverUpdateBytesPlayed)
		{//this is used by sw codec wrapper to request a bytes played update
		//bytes played won't be updated in Stopped() or Error() on sw cdoec wrapper
		//as the sound device is closed. Non swCodec wrapper Hw device plugins
		//can get there bytes updated on Stopped() and/or Error()
		UpdateBytesPlayed();
		result = KErrNone;
		}
	return result;
	}

/**
 *
 *	MMMFHwDeviceObserver mixin implementation.
 *
 *	The CMMFHwDevice implementation object calls this method when the current
 *	encode or decode task is finished or stopped.  The policy state is updated
 *
 */
void CMMFDevSoundSvrImp::Stopped()
	{
	//for swcodec wrap hw devices bytes played updated in MsgFromHwDevice
	//but non Swcodec wrappers hw devices may do it differently
	//also don't know if non Swcodec wrap hw device will call Stopped or Error first
	UpdateBytesPlayed();

	iLastBufferReceived = EFalse;
	UpdatePolicyState(EMMFStateCompleted);
	}

/**
 *  MMMFHwDeviceObserver mixin implementation
 *  Processes error from hw device
 */
void CMMFDevSoundSvrImp::Error(TInt aError)
	{
	if (iMode== EMMFDevSoundStatePlaying)
		{
		//for swcodec wrap hw devices bytes played updated in MsgFromHwDevice
		//but non Swcodec wrappers hw devices may do it differently
		//also don't know if non Swcodec wrap hw device will call Stopped or Error first
		UpdateBytesPlayed();

        iDevSoundObserver->PlayError(aError);
        UpdatePolicyState(EMMFStateStopped);
		}
	else if (iMode== EMMFDevSoundStateRecording)
		{
        iDevSoundObserver->RecordError(aError);
        UpdatePolicyState(EMMFStateStopped);
		}
	else if (iMode== EMMFDevSoundStateTonePlaying)
		{
        iDevSoundObserver->ToneFinished(aError);
        UpdatePolicyState(EMMFStateStopped);
		}
	//else can't handle error
	}


/********************************************************************************
 *				Non Exported public functions ends here							*
 ********************************************************************************/


/********************************************************************************
 *				Private functions begins here									*
 ********************************************************************************/

TInt CMMFDevSoundSvrImp::InitializeFormat(RMdaDevSound::TSoundFormatsSupportedBuf& aSupportedFormat,
		RMdaDevSound::TCurrentSoundFormatBuf& aFormat)
	{
	// Choose an encoding
	TUint32 enc = aSupportedFormat().iEncodings;
	// Always defaults to this
	if (enc & RMdaDevSound::EMdaSoundEncoding16BitPCM)
		aFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
	else if (enc & RMdaDevSound::EMdaSoundEncoding8BitALaw)
		aFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitALaw;
	else if (enc & RMdaDevSound::EMdaSoundEncoding8BitMuLaw)
		aFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitMuLaw;
	else if (enc & RMdaDevSound::EMdaSoundEncoding8BitPCM)
		aFormat().iEncoding = RMdaDevSound::EMdaSoundEncoding8BitPCM;

	// default to Monophonic playback:
	aFormat().iChannels=1;

	// Store the device capabilities (WINS supports from 8000 Hz to 44100 Hz)
	if ((aSupportedFormat().iMinRate <= 8000) && (8000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate = EMMFSampleRate8000Hz;
	if ((aSupportedFormat().iMinRate <= 11025) && (11025 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate11025Hz;
	if ((aSupportedFormat().iMinRate <= 12000) && (12000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate12000Hz;
	if ((aSupportedFormat().iMinRate <= 16000) && (16000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate16000Hz;
	if ((aSupportedFormat().iMinRate <= 22050) && (22050 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate22050Hz;
	if ((aSupportedFormat().iMinRate <= 24000) && (24000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate24000Hz;
	if ((aSupportedFormat().iMinRate <= 32000) && (32000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate32000Hz;
	if ((aSupportedFormat().iMinRate <= 44100) && (44100 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate44100Hz;
	if ((aSupportedFormat().iMinRate <= 48000) && (48000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate48000Hz;
	if ((aSupportedFormat().iMinRate <= 64000) && (64000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate64000Hz;
	if ((aSupportedFormat().iMinRate <= 88200) && (88200 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate88200Hz;
	if ((aSupportedFormat().iMinRate <= 96000) && (96000 <= aSupportedFormat().iMaxRate))
		iDeviceCapabilities.iRate |= EMMFSampleRate96000Hz;

	// Store the encodings supported
	iDeviceCapabilities.iEncoding = 0;
	if (enc & RMdaDevSound::EMdaSoundEncoding16BitPCM)
		iDeviceCapabilities.iEncoding |= EMMFSoundEncoding16BitPCM;
	if (enc & RMdaDevSound::EMdaSoundEncoding8BitALaw)
		iDeviceCapabilities.iEncoding |= EMMFSoundEncoding8BitALaw;
	if (enc & RMdaDevSound::EMdaSoundEncoding8BitMuLaw)
		iDeviceCapabilities.iEncoding |= EMMFSoundEncoding8BitMuLaw;
	if (enc & RMdaDevSound::EMdaSoundEncoding8BitPCM)
		iDeviceCapabilities.iEncoding |= EMMFSoundEncoding8BitPCM;

	// Mono and Stereo support
	if (aSupportedFormat().iChannels == 2)
		iDeviceCapabilities.iChannels = EMMFStereo;
	iDeviceCapabilities.iChannels |= EMMFMono;

	iDeviceCapabilities.iBufferSize = aSupportedFormat().iMaxBufferSize;
	// Default
	iDeviceConfig.iRate = EMMFSampleRate8000Hz;
	iDeviceConfig.iEncoding = EMMFSoundEncoding16BitPCM;
	iDeviceConfig.iChannels = EMMFMono;

	return KErrNone;
	}

/*
 *
 *	Makes request to Policy Server (asynchronous call)
 *
 */
void CMMFDevSoundSvrImp::RequestPolicy()
	{
	iDevSoundEventHandler->CancelReceiveEvents();
	iDevSoundEventHandler->ReceiveEvents();
	iAudioPolicyPrioritySettings.iCapabilities = iParent.CheckClientCapabilities();
	iAudioPolicyProxy->MakeRequest(iAudioPolicyPrioritySettings);
	}

/*
 *
 *	Creates buffer and begin playback using the specified tone generator.
 *
 */
void CMMFDevSoundSvrImp::DoPlayL()
	{
	// Delete any buffer from previous call and try to create maximum buffer 
	// size. Double Buffer the Tone data.
	if (iToneBuffer1)
		{
		delete iToneBuffer1; 
		iToneBuffer1 = NULL; 
		}
	//note the tone buffer needs to be the same as the pcm16->pcm16 'null'
	//hw device plugin
	// Buffer size = (SampleRate * BytesPerSample * Channels) / 4
	TInt useBufferOfSize = ((SamplingFrequency() * 2 * NumberOfChannels())/KDevSoundFramesPerSecond + (KDevSoundDeltaFrameSize-1)) &~ (KDevSoundDeltaFrameSize-1);
	//clamp buffer to desired limits
	if(useBufferOfSize < KDevSoundMinFrameSize) 
		useBufferOfSize = KDevSoundMinFrameSize;
	else if(useBufferOfSize > KDevSoundMaxFrameSize) 
		useBufferOfSize = KDevSoundMaxFrameSize;

	//clamp buffer to limits of hardware
	if(useBufferOfSize < Max(iPlayFormatsSupported().iMinBufferSize, iRecordFormatsSupported().iMinBufferSize))
		useBufferOfSize = Max(iPlayFormatsSupported().iMinBufferSize, iRecordFormatsSupported().iMinBufferSize);
	else if(useBufferOfSize > Min(iPlayFormatsSupported().iMaxBufferSize, iRecordFormatsSupported().iMaxBufferSize))
		useBufferOfSize = Min(iPlayFormatsSupported().iMaxBufferSize, iRecordFormatsSupported().iMaxBufferSize);

	iToneBuffer1 = CMMFDataBuffer::NewL(useBufferOfSize);
	User::LeaveIfError(iCurrentGenerator->FillBuffer(iToneBuffer1->Data()));

	if (iToneBuffer2)
		{
		delete iToneBuffer2; 
		iToneBuffer2 = NULL;
		}
	iToneBuffer2 = CMMFDataBuffer::NewL(useBufferOfSize);
	User::LeaveIfError(iCurrentGenerator->FillBuffer(iToneBuffer2->Data()));

	// Assign active buffer
	iActiveToneBuffer = iToneBuffer1;

	// Hw device hasn't played anything yet so don't change
    // active buffer. This is checked in FillThisHwBuffer.
    iFirstCallFromHwDevice = ETrue;

    // Start HwDevice to play data
    User::LeaveIfError(iCMMFHwDevice->Start(EDevDecode, EDevOutFlow));
	
	}

/*
 *
 *	This method assigns the other buffer as active buffer. The tone audio 
 *	generator should fill data in the other buffer by now.
 *
 */
void CMMFDevSoundSvrImp::SetActiveToneBuffer()
	{
	if (iActiveToneBuffer == iToneBuffer1)
		iActiveToneBuffer = iToneBuffer2;
	else if (iActiveToneBuffer == iToneBuffer2)
		iActiveToneBuffer = iToneBuffer1;
	}

/*
 *
 *	This method fills data into the free buffer.
 *
 *	@return	"TInt"
 *			Error code. KErrNone if success.
 *
 */
TInt CMMFDevSoundSvrImp::FillFreeToneBuffer()
	{
	TInt err(KErrNone);
	if (iActiveToneBuffer == iToneBuffer1)
		err = iCurrentGenerator->FillBuffer(iToneBuffer2->Data());
	else if (iActiveToneBuffer == iToneBuffer2)
		err = iCurrentGenerator->FillBuffer(iToneBuffer1->Data());
	return err;
	}

/*
 *
 *	Updates the policy state based on Audio policy settings of this devsound instance
 *
 */
TInt CMMFDevSoundSvrImp::UpdatePolicyState(TMMFAudioPolicyState aNewState)
	{
	iAudioPolicyPrioritySettings.iState = aNewState;
	TInt error = iAudioPolicyProxy->UpdateState(iAudioPolicyPrioritySettings);
	if ((error == KErrNone) && (aNewState == EMMFStateStopped))
		{
		iHasPolicy = EFalse;
		}

	return error;
	}

/*
 *
 *	Initializes audio device node by setting volume, and sampling rate.
 *
 *	@return	"TInt"
 *			Error Code. KErrNone if success.
 *
 */
TInt CMMFDevSoundSvrImp::InitTask()
	{
	// No Implementation
	return KErrNone;
	}



/*
 *
 *	Returns an integer representing Sampling Frequency the device is currently
 *	configured to.
 *
 *	@return	"TInt"
 *			Sampling Frequency.
 *
 */
TInt CMMFDevSoundSvrImp::SamplingFrequency()
	{
	if(iDeviceConfig.iRate == EMMFSampleRate8000Hz)
		return 8000;
	else if(iDeviceConfig.iRate == EMMFSampleRate11025Hz)
		return 11025;
	else if(iDeviceConfig.iRate == EMMFSampleRate12000Hz)
		return 12000;
	else if(iDeviceConfig.iRate == EMMFSampleRate16000Hz)
		return 16000;
	else if(iDeviceConfig.iRate == EMMFSampleRate22050Hz)
		return 22050;
	else if(iDeviceConfig.iRate == EMMFSampleRate24000Hz)
		return 24000;
	else if(iDeviceConfig.iRate == EMMFSampleRate32000Hz)
		return 32000;
	else if(iDeviceConfig.iRate == EMMFSampleRate44100Hz)
		return 44100;
	else if(iDeviceConfig.iRate == EMMFSampleRate48000Hz)
		return 48000;
	else if(iDeviceConfig.iRate == EMMFSampleRate88200Hz)
		return 88200;
	else if(iDeviceConfig.iRate == EMMFSampleRate96000Hz)
		return 96000;
	else
		return 8000; //default
	}
 
/*
 *
 *	Returns an integer representing number of channels the device is currently
 *	configured to.
 *
 *	@return	"TInt"
 *			Number of audio channels 1 if mono, 2 if stereo.
 *
 */
TInt CMMFDevSoundSvrImp::NumberOfChannels()
	{
	if(iDeviceConfig.iChannels == EMMFMono)
		return 1;
	else
		return 2;
	}

/*
 *
 *	Returns an integer representing number of bytes in each audio sample
 *	
 *
 *	@return	"TInt"
 *			Number of of bytes in each audio sample.
 *
 */
TInt CMMFDevSoundSvrImp::BytesPerAudioSample()
	{
	TInt bytes=1;
	switch (iDeviceConfig.iEncoding)
		{
		case EMMFSoundEncoding8BitPCM:
		case EMMFSoundEncoding8BitALaw:
		case EMMFSoundEncoding8BitMuLaw:
			{
			bytes=1;
			}
		break;
		case EMMFSoundEncoding16BitPCM:
			{
			bytes=2;
			}
		break;
		}
	return bytes;
	}


/********************************************************************************
 *				Private functions ends here										*
 ********************************************************************************/


// CustomCommand* implementation:
// The following come through from the parent object intact - currently it seems
// easier to pass this through with an RMessage, as the result parameter (at least)
// is easier to deal with that way. This might change in the future. 
// [TODO - if we extended this to CMMFHwDevice, it might be easier to use some sort
// of TMMFCustomCommandParamBlock throughout, which as well as the uid, in params etc
// by value, also provided a virtual(?) function to set output param. Could then
// use in both plat sec and original code

void CMMFDevSoundSvrImp::DoSyncCustomCommandL(const RMmfIpcMessage& aMessage)
	{
	TInt command = aMessage.Int0();
	
	// we don't support any commands at the momment
	User::Leave(KErrNotSupported);
	}
	
void CMMFDevSoundSvrImp::DoSyncCustomCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TInt command = aMessage.Int0();
	
	// we don't support any commands at the momment
	User::Leave(KErrNotSupported);	
	}
	
void CMMFDevSoundSvrImp::DoAsyncCustomCommandL(const RMmfIpcMessage& aMessage)
	{
	TInt command = aMessage.Int0();
	
	// we don't support any commands at the momment
	User::Leave(KErrNotSupported); // this will still complete for an async message	
	}
	
void CMMFDevSoundSvrImp::DoAsyncCustomCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TInt command = aMessage.Int0();
	
	// we don't support any commands at the moment
	User::Leave(KErrNotSupported); // this will still complete for an async message	
	}
	
