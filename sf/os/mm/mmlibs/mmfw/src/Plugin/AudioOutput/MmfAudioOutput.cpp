// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MmfAudioOutput.h"
#include <ecom/implementationproxy.h>
#include <mmf/server/mmfformat.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfhelper.h>
#endif
#include <mmf/plugin/mmfaudioiointerfaceuids.hrh>

void Panic(TInt aPanicCode)
	{
	_LIT(KMMFAudioOutputPanicCategory, "MMFAudioOutput");
	User::Panic(KMMFAudioOutputPanicCategory, aPanicCode);
	}

/**
Allocates and constructs a new audio output sink.

Static standard SymbianOS 2 phase constuction method.

@return A pointer to the new sink.
*/
MDataSink* CMMFAudioOutput::NewSinkL()
	{
	CMMFAudioOutput* self = new (ELeave) CMMFAudioOutput ;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return STATIC_CAST( MDataSink*, self ) ;
	}

/**
Standard SymbianOS ConstructL.

Used to initialise member varibles with device specific behaviour.
*/
void CMMFAudioOutput::ConstructL()
	{
	iInitializeState = KErrNone;
	iDataTypeCode = KMMFFourCCCodePCM16;
	iNeedsSWConversion = EFalse;
	iSourceSampleRate = 0;
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
	}

/**
Overridable constuction specific to this datasource.

The default implementation does nothing.

@param  aInitData
        The initialisation data.
*/
void CMMFAudioOutput::ConstructSinkL( const TDesC8& /*aInitData*/ )
	{
	}


/**
@deprecated

Gets audio from hardware device abstracted MMFDevsound (not used).

@param  aBuffer
        The data to write out to a Hardware Device.
@param  aSupplier
        The MDataSource consuming the data contained in aBuffer
*/
void CMMFAudioOutput::HWEmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/)
	{
	}

/**
Sends audio to MMFDevsound.

@param  aBuffer
        The data to write out.
@param  aSupplier
        The search criteria for the supplier.
@param  aMediaId
        The type of data supplied - currently ignored.
*/
void CMMFAudioOutput::EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/)
	{
	iSupplier = aSupplier;

	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);

	if (aSupplier == NULL)
		User::Leave(KErrArgument);

	// In order to avoid changes at the datapath data transfer state machine 
	// EmptyBufferL is still being called even
	// if the first time there is no buffer to send
	if (!iCanSendBuffers)
		{
		
		iCanSendBuffers = ETrue;
		return;
		}

	if (iNeedsSWConversion)
		{//need to perform channel & sample rate conversion before writing to clip
		CMMFDataBuffer* audio;

		//need to make sure aBuffer is not null before it is used
		if (aBuffer == NULL)
			{
			User::Leave(KErrArgument);
			}
		
		//buffer check is placed here before possible use of the buffer
		if (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
			{
			User::Leave(KErrNotSupported);
			}
		
		iConvertBuffer = CMMFDataBuffer::NewL(((CMMFDataBuffer*)aBuffer)->Data().Length());
		iChannelAndSampleRateConverter->Convert(*(CMMFDataBuffer*)aBuffer, *iConvertBuffer);
		audio = iConvertBuffer;

		//copy our converted data back into the real buffer to return to datapath
		TDes8& dest = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data() ;
		dest.SetLength(0);
		dest.Copy(audio->Data());
		}

	if (iState != EDevSoundReady && iState != EPaused) // If we're paused we still feed a buffer to DevSound
		User::Leave(KErrNotReady);

	iMMFDevSound->PlayData();

#if defined(__AUDIO_PROFILING)
	RDebug::ProfileEnd(0);
	User::Leave(KErrEof);
#endif  // defined(__AUDIO_PROFILING)


	// The following will never have been allocated unless
	// software conversion was required, and due to certain DevSound
	// implementations, this requirement can change dynamically.
	delete iConvertBuffer;
	iConvertBuffer = NULL;
	}



/**
Negotiates with the source to set, for example, the sample rate and number of channels.

Called if the sink's setup depends on source.

@param  aSource
        The data source with which to negotiate.
*/
void CMMFAudioOutput::NegotiateL(MDataSource& aSource)
	{
	if (aSource.DataSourceType() == KUidMmfFormatDecode)
		{//source is a clip so for now set sink settings to match source
		iSourceSampleRate = ((CMMFFormatDecode&)aSource).SampleRate();
		iSourceChannels = ((CMMFFormatDecode&)aSource).NumChannels(); 
		iSourceFourCC.Set(aSource.SourceDataTypeCode(TMediaId(KUidMediaTypeAudio)));
	
		((CMMFFormatDecode&)aSource).SuggestSourceBufferSize(KAudioOutputDefaultFrameSize);
		}

	// Query DevSound capabilities and Try to use DevSound sample rate and 
	// mono/stereo capability
	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);

	TMMFState prioritySettingsState = iPrioritySettings.iState; //should be EMMFStatePlaying
	//to use the GetSupportedInputDatatypes but we'll save it just in case it's not
	iPrioritySettings.iState = EMMFStatePlaying; //if playing does not support any output data types
	RArray<TFourCC> supportedDataTypes;
	//note Input data types becuase if we are playing audio ie audio output
	//the data is sent as an input to DevSound
	TRAPD(err, iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, iPrioritySettings));
	iPrioritySettings.iState = prioritySettingsState;
	if (err == KErrNone)
		{
		if (supportedDataTypes.Find(iSourceFourCC) == KErrNotFound)
			{//the source fourCC code could not be found in the list of 
			//data types supported by the Devsound therefor default to pcm16
			iDataTypeCode = KMMFFourCCCodePCM16;	
			}
		else
			{
			//the DevSound does support the same datatype as the source 
			//so set the fourcc to that of the source
			iDataTypeCode = iSourceFourCC;
			}
		}
	supportedDataTypes.Close();
	if (err == KErrNotSupported)
		{//if the Devsound does not support the GetSupportedOutputDataTypesL method
		//then assume that the DevSound is pcm16 only
		iDataTypeCode = KMMFFourCCCodePCM16;	
		}
	else if (err != KErrNone) //we had a real leave error from GetSupportedOuputDataTypesL
		{
		User::Leave(err);
		}
		
	// Prevent defect when SinkPrimeL is called before NegotiateL()
	// since characterization is ambiguous
	if(iState == EDevSoundReady)
		{
		iState = EIdle;
		}
	//INC40817 do the initialize here as capabilities may depend on datatype
	//note this implies client of audiooutput must call negotiate
	iMMFDevSound->InitializeL(*this, iDataTypeCode, EMMFStatePlaying);

	// In some implementations InitializeComplete is sent
	// in context, so check before starting activeSchedulerWait.
	if (iState != EDevSoundReady)
		{
		iInitializeState = KRequestPending;
		iActiveSchedulerWait->Start();
		}
		
	User::LeaveIfError(iInitializeState);
	
	// Reset the following flag in case DevSound's capabilities have
	// changed since we were last here: INC037165
	iNeedsSWConversion = EFalse;
	TMMFCapabilities devSoundCaps;
	devSoundCaps = iMMFDevSound->Capabilities();
	// Default PCM16
	iDevSoundConfig.iEncoding = EMMFSoundEncoding16BitPCM;
	// 1 = Monophonic and 2 == Stereo
	if (((iSourceChannels == 1) && (devSoundCaps.iChannels & EMMFMono)) ||
		((iSourceChannels == 2) && (devSoundCaps.iChannels & EMMFStereo)))
		iDevSoundConfig.iChannels = iSourceChannels;
	else //default or SW conversion, e.g. stereo on mono support
		{
		iDevSoundConfig.iChannels = EMMFMono;
		iNeedsSWConversion = ETrue;
		iSWConvertChannels = 1;
		iSWConvertSampleRate = iSourceSampleRate;
		}

	// Check for std sample rates.
	if ((iSourceSampleRate == 96000) && (devSoundCaps.iRate & EMMFSampleRate96000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate96000Hz;
	else if ((iSourceSampleRate == 88200) && (devSoundCaps.iRate & EMMFSampleRate88200Hz))
		iDevSoundConfig.iRate = EMMFSampleRate88200Hz;
	else if ((iSourceSampleRate == 64000) && (devSoundCaps.iRate & EMMFSampleRate64000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate64000Hz;
	else if ((iSourceSampleRate == 48000) && (devSoundCaps.iRate & EMMFSampleRate48000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate48000Hz;
	else if ((iSourceSampleRate == 44100) && (devSoundCaps.iRate & EMMFSampleRate44100Hz))
		iDevSoundConfig.iRate = EMMFSampleRate44100Hz;
	else if ((iSourceSampleRate == 32000) && (devSoundCaps.iRate & EMMFSampleRate32000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate32000Hz;
	else if ((iSourceSampleRate == 24000) && (devSoundCaps.iRate & EMMFSampleRate24000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate24000Hz;
	else if ((iSourceSampleRate == 22050) && (devSoundCaps.iRate & EMMFSampleRate22050Hz))
		iDevSoundConfig.iRate = EMMFSampleRate22050Hz;
	else if ((iSourceSampleRate == 16000) && (devSoundCaps.iRate & EMMFSampleRate16000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate16000Hz;
	else if ((iSourceSampleRate == 12000) && (devSoundCaps.iRate & EMMFSampleRate12000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate12000Hz;
	else if ((iSourceSampleRate == 11025) && (devSoundCaps.iRate & EMMFSampleRate11025Hz))
		iDevSoundConfig.iRate = EMMFSampleRate11025Hz;
	else if ((iSourceSampleRate == 8000) && (devSoundCaps.iRate & EMMFSampleRate8000Hz))
		iDevSoundConfig.iRate = EMMFSampleRate8000Hz;
	else // non standard sample rate
		{
		iNeedsSWConversion = ETrue;
		// we need to choose to the closest, and smaller standard sample rate
		// and eventually convert the audio samples to this standard sample rate
		//NB: this list must be in ascending order
		const TInt KNumSampleRates = 12;
		static const TUint supportedSR[KNumSampleRates][2] = {{8000,  EMMFSampleRate8000Hz},
									{11025, EMMFSampleRate11025Hz},
									{12000, EMMFSampleRate12000Hz},
									{16000, EMMFSampleRate16000Hz},
									{22050, EMMFSampleRate22050Hz},
									{24000, EMMFSampleRate24000Hz},
									{32000, EMMFSampleRate32000Hz},
									{44100, EMMFSampleRate44100Hz},
									{48000, EMMFSampleRate48000Hz},
									{64000, EMMFSampleRate64000Hz},
									{88200, EMMFSampleRate88200Hz},
									{96000, EMMFSampleRate96000Hz}};

		//Only support down sampling
   		if (iSourceSampleRate < supportedSR[0][0]) 
   			User::Leave(KErrNotSupported);
   		
   		
   		TInt sampleRateIndex = KNumSampleRates;
   
   		//find the source sampleRateIndex
   		for (sampleRateIndex--; sampleRateIndex > -1; sampleRateIndex--)
   			{
   			if(iSourceSampleRate >= supportedSR[sampleRateIndex][0])
   				{
   				break;
   				}
   			}
   
   		//find the highest sink sample rate below the source rate
   		for (; sampleRateIndex > -1; sampleRateIndex--)
   			{
   			if(devSoundCaps.iRate & supportedSR[sampleRateIndex][1])
   				{
   				iSWConvertSampleRate = supportedSR[sampleRateIndex][0];
   				iDevSoundConfig.iRate = supportedSR[sampleRateIndex][1];
   				break;
   				}
   			}
   
   		//if a suitable sink sample rate is not available
   		if (sampleRateIndex < 0) 
   			User::Leave(KErrNotSupported);
   
   		// set the channels as well
   		iSWConvertChannels = iDevSoundConfig.iChannels;
   		} // else // non standard sample rate

	if (iNeedsSWConversion)
		{//can only software convert if datatype is pcm16
		//note cannot set non standard sample rates on DevSound API
		//as the API does not allow this
		//we need to reinitialize the devsound with pcm16 in this case
		iDataTypeCode = KMMFFourCCCodePCM16;
		iState = EIdle;
		iMMFDevSound->InitializeL(*this, iDataTypeCode, EMMFStatePlaying);
		
		// In some implementations InitializeComplete is called
		// in context, so check before starting activeSchedulerWait.
		if (iState != EDevSoundReady)
			{
			iInitializeState = KRequestPending;
			iActiveSchedulerWait->Start();
			}
			
		User::LeaveIfError(iInitializeState);
		}
	}

/**
Sets the sink's priority settings.

@param  aPrioritySettings
        The sink's priority settings. Takes enumerations to determine audio playback priority. 
        Higher numbers mean high priority (can interrupt lower priorities).
*/
void CMMFAudioOutput::SetSinkPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iPrioritySettings = aPrioritySettings;
	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);
	else
		iMMFDevSound->SetPrioritySettings(iPrioritySettings);
	}

/**
Gets the sink's data type code.

Used by datapath MDataSource / MDataSink for codec matching.

@param  aMediaId
        The Media ID. Optional parameter to specifiy specific stream when datasource contains more
        than one stream of data.

@return	The 4CC of the data expected by this sink.
*/
TFourCC CMMFAudioOutput::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	return iDataTypeCode;
	}

/**
Sets the sink's data type code.

@param  aSinkFourCC
        The 4CC of the data to be supplied to this sink.
@param  aMediaId
        The Media ID. Optional parameter to specifiy specific stream when datasource contains more 
        than one stream of data.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CMMFAudioOutput::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/)
	{//will check with devsound to see if aSinkFourCC is supported
	 //when this is added to devsound
	iDataTypeCode = aSinkFourCC;
	return KErrNone;
	}

/**
Primes the sink.

This is a virtual function that each derived class must implement, but may be left blank for default 
behaviour.

Called by CMMFDataPath::PrimeL().
*/
void CMMFAudioOutput::SinkPrimeL()
	{
	if(iPlayStarted != EFalse)
		{
		return;
		}
	iPlayStarted = EFalse;
	iCanSendBuffers = EFalse;
	if (iState == EIdle)
		{
		if (!iMMFDevSound) 
			{
			User::Leave(KErrNotReady);
			}
		iState = EDevSoundReady;
		}
	}

/**
Pauses the sink.

This is a virtual function that each derived class must implement, but may be left blank for default 
behaviour.
*/
void CMMFAudioOutput::SinkPauseL()
	{
	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);
	else
		iMMFDevSound->Pause();
	iPlayStarted = EFalse;
	iState = EPaused;
	}

/**
Starts playing the sink.

This is a virtual function that each derived class must implement, but may be left blank for default
behaviour.
*/
void CMMFAudioOutput::SinkPlayL()
	{
	if (iState == EPaused)
		{
		TBool isResumeSupported = iMMFDevSound->IsResumeSupported();
		// CMMFAudioOutput is not supposed to be paused 
		// if Resume is not supported by DevSound
		if(!isResumeSupported)
			{
			User::Leave(KErrNotSupported);
			}
		
		User::LeaveIfError(iMMFDevSound->Resume());
		iPlayStarted = ETrue;
		}
	else if (iPlayStarted == EFalse)
		{
		ConfigDevSoundL();	

		// This is a one-shot to "prime" MMFDevSound as first buffer uninitialised
		iMMFDevSound->PlayInitL();
		iPlayStarted = ETrue;
		}
	if ((iNeedsSWConversion)&&(iSourceChannels>0))
		{//can only do SW convert  - therefore need to do a conversion
		//currently only pcm16 is supported so return with an error if format not pcm16
		if (!iChannelAndSampleRateConverterFactory)
			{
			iChannelAndSampleRateConverterFactory
				= new(ELeave)CMMFChannelAndSampleRateConverterFactory;
			iChannelAndSampleRateConverter =
				iChannelAndSampleRateConverterFactory->CreateConverterL( iSourceSampleRate, iSourceChannels,
																		iSWConvertSampleRate, iSWConvertChannels);
			}
		//need to create an intermediate buffer in which to place the converted data
		}
	iState = EDevSoundReady;
	}

/**
Stops the sink.

This is a virtual function that each derived class must implement, but may be left blank for default
behaviour.
*/
void CMMFAudioOutput::SinkStopL()
	{
	if (iState == EDevSoundReady)
		{//not waiting on a buffer being played so stop devsound now
		iState = EIdle;
		if (iPlayStarted)
			{
			if (!iMMFDevSound)
				{
				Panic(EMMFAudioOutputDevSoundNotLoaded);
				}
			else
				{
				iPlayStarted = EFalse;
				iMMFDevSound->Stop();
				}
			}
		}
	else if (iState == EPaused)	//DEF46250 need to handle pause separately as we should always stop regardless of the state of iFirstBufferSent
		{
		iPlayStarted = EFalse;
		iMMFDevSound->Stop();
		iState = EIdle; 
		}
	iCanSendBuffers = EFalse;
	} 

/**
Returns the playback state (EStopped, EPlaying, EPaused etc) of this sink
*/
TInt CMMFAudioOutput::State()
	{
	return iState;
	}

/**
Logs on the sink's thread.

Thread specific initialization procedure for this device. Runs automatically on thread construction.

@param  aEventHandler
        The event handler.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CMMFAudioOutput::SinkThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	iEventHandler = &aEventHandler;
	TInt err = KErrNone;
	if (!iDevSoundLoaded)
		TRAP(err, LoadL());
	return err;
	}

/**
Logs off the sink thread.

Thread specific destruction procedure for this device. Runs automatically on thread destruction.
*/
void CMMFAudioOutput::SinkThreadLogoff()
	{
	if(iMMFDevSound)
		{
		iMMFDevSound->Stop();
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	iDevSoundLoaded = EFalse;
	iState = EIdle;
	}

/**
Called by MDataSource to pass back a full buffer to the sink. 

Should never be called by a sink, as sinks empty buffers, not fill them.

@param  aBuffer
        The filled buffer.
*/
void CMMFAudioOutput::BufferFilledL(CMMFBuffer* /*aBuffer*/)
	{
	Panic(EMMFAudioOutputPanicBufferFilledLNotSupported);
	}

/**
Tests whether a sink buffer can be created.

The default implementation returns true.

@return A boolean indicating if the sink buffer can be created. ETrue if it can, otherwise EFalse.
*/
TBool CMMFAudioOutput::CanCreateSinkBuffer()
	{
	return ETrue;
	}

/**
Creates a sink buffer.

Intended for asynchronous usage (buffers supplied by Devsound device)

@param  aMediaId
        The Media ID.
@param	aReference
		A boolean indicating if MDataSink owns the buffer. ETrue if does, otherwise EFalse.

@return A sink buffer.
*/
CMMFBuffer* CMMFAudioOutput::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference)
	{
	//iDevSoundBuffer = CMMFDataBuffer::NewL(KAudioOutputDefaultFrameSize);
	iDevSoundBuffer = NULL;		//DevSound supplies this buffer in first callback
	aReference = ETrue;
	if ( iNeedsSWConversion )
		return iConvertBuffer;
	else
		return iDevSoundBuffer;
	}

/**
Standard SymbianOS destructor.
*/
CMMFAudioOutput::~CMMFAudioOutput()
	{
	// The following will never have been allocated unless
	// software conversion was required, and due to certain DevSound
	// implementations, this requirement can change dynamically.
	delete iChannelAndSampleRateConverterFactory;
	delete iConvertBuffer;

	if (iMMFDevSound)
		{
		iMMFDevSound->Stop();
		delete iMMFDevSound;
		}
	delete iActiveSchedulerWait;
	}

void CMMFAudioOutput::ConfigDevSoundL()
	{
	iMMFDevSound->SetConfigL(iDevSoundConfig);
	}


/**
@deprecated

This method should not be used - it is provided to maintain SC with v7.0s.

@param  aAudioType
        The 4CC of the data supplied by this source.
*/
void CMMFAudioOutput::SetDataTypeL(TFourCC aAudioType)
	{
	if (aAudioType != KMMFFourCCCodePCM16)
		{
		User::Leave(KErrNotSupported);
		}
	}


/**
@deprecated

This method should not be used - it is provided to maintain SC with v7.0s.

@return The 4CC of the data supplied by this source.
*/
TFourCC CMMFAudioOutput::DataType() const
	{
	return KMMFFourCCCodePCM16;
	}

/**
Queries about DevSound resume support

@return ETrue if DevSound does support resume, EFalse otherwise
*/
TBool CMMFAudioOutput::IsResumeSupported()
	{
	TBool isSupported = EFalse;
	if (iMMFDevSound)
		{
		isSupported = iMMFDevSound->IsResumeSupported();
		}
	return isSupported;
	}


/**
Loads audio device drivers and initialise this device.
*/
void CMMFAudioOutput::LoadL()
	{
	iPlayStarted = EFalse;
	if (iState != EDevSoundReady)
		iState = EIdle;

	iMMFDevSound = CMMFDevSound::NewL();
	
	//This is done to maintain compatibility with the media server
	iMMFDevSound->SetVolume(iMMFDevSound->MaxVolume() - 1);

	//note cannot perform further initlaisation here untill the datatype is known

	iDevSoundLoaded = ETrue;
	}

/**
DeviceMessage MMFDevSoundObserver
*/
void CMMFAudioOutput::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	}


/**
ToneFinished MMFDevSoundObserver called when a tone has finished or interrupted

Should never get called.
*/
void CMMFAudioOutput::ToneFinished(TInt /*aError*/)
	{
	//we should never get a tone error in MMFAudioOutput!
	__ASSERT_DEBUG(EFalse, Panic(EMMFAudioOutputPanicToneFinishedNotSupported));
	}


/**
RecordError MMFDevSoundObserver called when recording has halted.

Should never get called.
*/
void CMMFAudioOutput::RecordError(TInt /*aError*/)
	{
	//we should never get a recording error in MMFAudioOutput!
	__ASSERT_DEBUG(EFalse, Panic(EMMFAudioOutputPanicRecordErrorNotSupported));
	}

/**
InitializeComplete MMFDevSoundObserver called when devsound initialisation completed.
*/
void CMMFAudioOutput::InitializeComplete(TInt aError)
	{
	
	if (aError == KErrNone)
		{
		iState = EDevSoundReady;
		}
	
	if(iInitializeState == KRequestPending)
		{
		iInitializeState = aError;
		iActiveSchedulerWait->AsyncStop();
		}
	}

/**
BufferToBeEmptied MMFDevSoundObserver - should never get called.
*/
void CMMFAudioOutput::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EMMFAudioOutputPanicRecordErrorNotSupported));
	}

/**
BufferToBeFilled MMFDevSoundObserver.
Called when buffer used up.
*/
void CMMFAudioOutput::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	TInt err = KErrNone;

	TRAP(err, iSupplier->BufferEmptiedL(aBuffer));

	//This error needs handling properly
	__ASSERT_DEBUG(!err, Panic(err));
	}

/**
PlayError MMFDevSoundObserver.

Called when stopped due to error or EOF.
*/
void CMMFAudioOutput::PlayError(TInt aError)
	{
	iMMFDevsoundError = aError;

	//send EOF to client
	TMMFEvent event(KMMFEventCategoryPlaybackComplete, aError);
	SendEventToClient(event);

	//stop stack overflow / looping problem - AD
	if (aError == KErrCancel)
		return;

	// NB KErrInUse, KErrDied OR KErrAccessDenied may be returned by the policy server
	// to indicate that the sound device is in use by another higher priority client.
	if (aError == KErrInUse || aError == KErrDied || aError == KErrAccessDenied)
		return;

	if (iState == EIdle)
		{//probably have stopped audio output and have got an underflow from last buffer
		iMMFDevSound->Stop();
		iPlayStarted = EFalse;
		iCanSendBuffers = EFalse;
		}
	}


/**
ConvertError MMFDevSoundObserver.

Should never get called.
*/
void CMMFAudioOutput::ConvertError(TInt /*aError*/)
	{
	}


/**
Returns the number of bytes played.

@return	The number of bytes played. If 16-bit divide this number returned by 2 to get word length.
*/
TInt CMMFAudioOutput::BytesPlayed()
	{
	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);
	return iMMFDevSound->SamplesPlayed();
	}

/**
Returns the sound device.

Accessor function exposing public CMMFDevsound methods.

@return	A reference to a CMMFDevSound objector.
*/
CMMFDevSound& CMMFAudioOutput::SoundDevice()
	{
	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);
	return *iMMFDevSound;
	}

void CMMFAudioOutput::SendEventToClient(const TMMFEvent& aEvent)
	{
	iEventHandler->SendEventToClient(aEvent);
	}
// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs



const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidAudioOutputInterface,	CMMFAudioOutput::NewSinkL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

