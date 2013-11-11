// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MmfAudioInput.h"
#include <ecom/implementationproxy.h>
#include <mmf/server/mmfformat.h>
#include <mmf/plugin/mmfaudioiointerfaceuids.hrh>


void Panic(TInt aPanicCode)
	{
	_LIT(KMMFAudioInputPanicCategory, "MMFAudioInput");
	User::Panic(KMMFAudioInputPanicCategory, aPanicCode);
	}

/**
Static standard SymbianOS 2 phase constuction method.

Constucts this audio device.

@return A pointer to a MDataSource returned on successful construction.
*/
MDataSource* CMMFAudioInput::NewSourceL()
	{
	CMMFAudioInput* self = new (ELeave) CMMFAudioInput ;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return  STATIC_CAST( MDataSource*, self );
	}

/**
Standard SymbianOS ConstructL.

Used to initialise member varibles with device specific behaviour.
*/
void CMMFAudioInput::ConstructL()
	{
	iDataTypeCode = KMMFFourCCCodePCM16;
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
	}

/**
Standard SymbianOS destructor.
*/
CMMFAudioInput::~CMMFAudioInput()
	{
	delete iActiveSchedulerWait;
	if (iMMFDevSound)
		{
		iMMFDevSound->Stop();
		delete iMMFDevSound;
		}
	}


/**
Overridable constuctor specific to this datasource.

@param  aInitData
        The initialisation data.
*/
void CMMFAudioInput::ConstructSourceL( const TDesC8& /*aInitData*/ ) 
	{
	}

/**
@deprecated

Gets audio from hardware device abstracted MMFDevsound (not used).

@param  aBuffer
        The data to read in from a Hardware Device
@param  aConsumer
        The MDataSink consuming the data contained in aBuffer.
*/
void CMMFAudioInput::HWFillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/)
	{
	}


/**
Gets audio from MMFDevsound.

@pre
iMMFDevSound must be loaded.

@param  aBuffer
        The data to read in from a Devsound device.
@param  aConsumer
        The MDataSink consuming the data contained in aBuffer.
@param  aMediaId
        Type of data supplied - currently ignored.
*/
void CMMFAudioInput::FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /* aMediaId */)
	{
	iConsumer = aConsumer;

	if (!iMMFDevSound)
		Panic(EMMFAudioInputDevSoundNotLoaded);
	
	if ((iState == EPaused) && (iPausePending != EFalse) && (iFirstBufferRequested) )
  		{
  		User::Leave(KErrNotReady);
  		}

	if ((aBuffer != NULL) && (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type())))
		User::Leave(KErrNotSupported);

	if (aConsumer == NULL)
		User::Leave(KErrArgument);

	//this is a one-shot "prime" funtion for MMFDevSound as first buffer is uninitialised
	if (!iFirstBufferRequested)
		{
		iMMFDevSound->RecordInitL();
		iFirstBufferRequested = ETrue;
		return;
		}

	if (iState != EDevSoundReady && iState != EPaused)
		{
		User::Leave(KErrNotReady);
		}

	iMMFDevSound->RecordData();
	}

/**
Indicates the data sink has emptied the buffer.

Called by the data path's MDataSink when it has emptied the buffer.

The default implementation is empty.

@param  aBuffer
        The data buffer that has been emptied (not used).
*/
void CMMFAudioInput::BufferEmptiedL(CMMFBuffer* /*aBuffer*/)
	{
	}


/**
Tests whether a source buffer can be created.

The default imlpementation returns true.

@return	A boolean indicating if the buffer can be made. ETrue if the buffer can be created, false
        otherwise.
*/
TBool CMMFAudioInput::CanCreateSourceBuffer()
	{
	return ETrue;
	}

/**
Creates a source buffer.

Intended for asynchronous usage (buffers supplied by Devsound device)

@param  aMediaId
		The Media ID. Not used in the default implementation.
@param  aReference
        A boolean indicating if MDataSource owns the buffer. ETrue if it does, EFalse if it does 
        not.

@return The buffer created (this will always be NULL when asychronous).
*/
CMMFBuffer* CMMFAudioInput::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference)
	{
	CMMFDataBuffer* buf = NULL;

	aReference = ETrue; // This is a reference from DevSound
	return buf;
	}

/**
Creates a source buffer.

Intended for synchronous usage.

@param  aMediaId
        The Media ID. Not used in the default implementation.

@return The buffer created
*/
CMMFBuffer* CMMFAudioInput::CreateSourceBufferL(TMediaId /*aMediaId*/)
	{
	CMMFDataBuffer* buf = CMMFDataBuffer::NewL(KAudioInputDefaultFrameSize);
	return buf;
	}


/**
Primes the source.

This is a virtual function that each derived class must implement, but may be left blank for
default behaviour.

Overridable PrimeL method. Additional Prime method specific to this DataSource.
*/
void CMMFAudioInput::SourcePrimeL()
	{
	iState = EDevSoundReady;
	}

/**
Pauses the source.

This is a virtual function that each derived class must implement, but may be left blank for default 
behaviour.

Overridable PauseL method. Additional Pause method specific to this DataSource.
*/
void CMMFAudioInput::SourcePauseL()
	{
	if (iState == EDevSoundReady)
		{//not waiting on a buffer being played so stop devsound now
		iState = EPaused;
		if (iFirstBufferRead)
			{
			if (!iMMFDevSound)
				Panic(EMMFAudioInputDevSoundNotLoaded);
			else
				iMMFDevSound->Pause();
			}
		else
			iPausePending = ETrue; // Wait for recording to get started.
		}
	//else if Devsound isn't ready then no point in stopping it
	}


/**
Stops the source.

This is a virtual function that each derived class must implement, but may be left blank for default 
behaviour.

Overridable StopL method. Additional Stop method specific to this DataSource.
*/
void CMMFAudioInput::SourceStopL()
	{
	iStopped = ETrue;
	// This is done in Audio Output as well, not sure whether its needed here or not.
	// Pause will be called before SourceStopL() and pause will take care of closing
	// DevSound
	if (iState == EDevSoundReady  || iState == EPaused)
		{//not waiting on a buffer being played so stop devsound now
		iState = EIdle;
		if (iFirstBufferRequested)
			{
			if (!iMMFDevSound)
				Panic(EMMFAudioInputDevSoundNotLoaded);
			else
				{
				iMMFDevSound->Stop();
				}

			iFirstBufferRequested = EFalse;
			iFirstBufferRead = EFalse;
			}
		}
	//else if Devsound isn't ready then no point in stopping it
	}

/**
Plays the source.

This is a virtual function that each derived class must implement, but may be left blank for default
behaviour.

Overridable PlayL method. Additional Play method specific to this DataSource.
*/
void CMMFAudioInput::SourcePlayL()
	{
	}

/**
Logs on the source thread.

Thread specific initialization procedure for this device. Runs automatically on thread construction.

@param  aEventHandler
        The event handler.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CMMFAudioInput::SourceThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	iEventHandler = &aEventHandler;
	TInt err = KErrNone;
	if (!iDevSoundLoaded)
		TRAP(err, LoadL());
	return err;
	}

/**
Logs off the source thread.

Thread specific destruction procedure for this device. Runs automatically on thread destruction.
*/
void CMMFAudioInput::SourceThreadLogoff()
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

/*
@internaTechnology

@pre
dev sound should be created and loaded.
*/
void CMMFAudioInput::ConfigDevSoundL()
	{
	//[precondition dev sound created ]
	ASSERT( iMMFDevSound );

	// Query DevSound capabilities and Try to use DevSound sample rate and 
	// mono/stereo capability
	TMMFCapabilities devSoundCaps = iMMFDevSound->Capabilities();
	// get current config
	TMMFCapabilities devSoundConfig = iMMFDevSound->Config();

	// Default PCM16
	devSoundConfig.iEncoding = EMMFSoundEncoding16BitPCM;

	// 1 = Monophonic and 2 == Stereo
	if (((iSinkChannels == 1) && (devSoundCaps.iChannels & EMMFMono)) ||
		((iSinkChannels == 2) && (devSoundCaps.iChannels & EMMFStereo)))
		devSoundConfig.iChannels = iSinkChannels;


	// Check for std sample rates.
	if ((iSinkSampleRate == 96000) && (devSoundCaps.iRate & EMMFSampleRate96000Hz))
		devSoundConfig.iRate = EMMFSampleRate96000Hz;
	else if ((iSinkSampleRate == 88200) && (devSoundCaps.iRate & EMMFSampleRate88200Hz))
		devSoundConfig.iRate = EMMFSampleRate88200Hz;
	else if ((iSinkSampleRate == 64000) && (devSoundCaps.iRate & EMMFSampleRate64000Hz))
		devSoundConfig.iRate = EMMFSampleRate64000Hz;
	else if ((iSinkSampleRate == 48000) && (devSoundCaps.iRate & EMMFSampleRate48000Hz))
		devSoundConfig.iRate = EMMFSampleRate48000Hz;
	else if ((iSinkSampleRate == 44100) && (devSoundCaps.iRate & EMMFSampleRate44100Hz))
		devSoundConfig.iRate = EMMFSampleRate44100Hz;
	else if ((iSinkSampleRate == 32000) && (devSoundCaps.iRate & EMMFSampleRate32000Hz))
		devSoundConfig.iRate = EMMFSampleRate32000Hz;
	else if ((iSinkSampleRate == 24000) && (devSoundCaps.iRate & EMMFSampleRate24000Hz))
		devSoundConfig.iRate = EMMFSampleRate24000Hz;
	else if ((iSinkSampleRate == 22050) && (devSoundCaps.iRate & EMMFSampleRate22050Hz))
		devSoundConfig.iRate = EMMFSampleRate22050Hz;
	else if ((iSinkSampleRate == 16000) && (devSoundCaps.iRate & EMMFSampleRate16000Hz))
		devSoundConfig.iRate = EMMFSampleRate16000Hz;
	else if ((iSinkSampleRate == 12000) && (devSoundCaps.iRate & EMMFSampleRate12000Hz))
		devSoundConfig.iRate = EMMFSampleRate12000Hz;
	else if ((iSinkSampleRate == 11025) && (devSoundCaps.iRate & EMMFSampleRate11025Hz))
		devSoundConfig.iRate = EMMFSampleRate11025Hz;
	else if ((iSinkSampleRate == 8000) && (devSoundCaps.iRate & EMMFSampleRate8000Hz))
		devSoundConfig.iRate = EMMFSampleRate8000Hz; 
	else 
		{
		// pick the maximum sample rate available
		if (devSoundCaps.iRate & EMMFSampleRate96000Hz)
			devSoundConfig.iRate = EMMFSampleRate96000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate88200Hz)
			devSoundConfig.iRate = EMMFSampleRate88200Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate64000Hz)
			devSoundConfig.iRate = EMMFSampleRate64000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate48000Hz)
			devSoundConfig.iRate = EMMFSampleRate48000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate44100Hz)
			devSoundConfig.iRate = EMMFSampleRate44100Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate32000Hz)
			devSoundConfig.iRate = EMMFSampleRate32000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate24000Hz)
			devSoundConfig.iRate = EMMFSampleRate24000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate22050Hz)
			devSoundConfig.iRate = EMMFSampleRate22050Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate16000Hz)
			devSoundConfig.iRate = EMMFSampleRate16000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate12000Hz)
			devSoundConfig.iRate = EMMFSampleRate12000Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate11025Hz)
			devSoundConfig.iRate = EMMFSampleRate11025Hz;
		else if (devSoundCaps.iRate & EMMFSampleRate8000Hz)
			devSoundConfig.iRate = EMMFSampleRate8000Hz;
		else
			ASSERT(EFalse); // if we don't support any sample rates, there is not much we can do
		
		}

	iMMFDevSound->SetConfigL(devSoundConfig);
	}

/**
Negotiates with the sink.

Called if the source's setup depends on sink.

@param  aSink
        The Data sink. Takes an MDataSink reference so a DataSource can negotiate with this
        MDataSource.
*/
void CMMFAudioInput::NegotiateSourceL(MDataSink& aSink)
	{
	if (aSink.DataSinkType() == KUidMmfFormatEncode) 
		{//sink is a clip so for now set sink settings to match sink
		iSinkSampleRate = ((CMMFFormatEncode&)aSink).SampleRate(); 
		iSinkChannels = ((CMMFFormatEncode&)aSink).NumChannels();
		iSinkFourCC.Set(aSink.SinkDataTypeCode(TMediaId(KUidMediaTypeAudio)));

		// if the sink's sample rate is undefined, try to obtain and use a
		// default sample rate from the sink. If this is zero, use 8K
		if (iSinkSampleRate == 0)
			{
			iSinkSampleRate = ((CMMFFormatEncode&)aSink).GetDefaultSampleRate(); 
			if (iSinkSampleRate == 0)
				iSinkSampleRate = 8000;
			}
		
		}

	if (iMMFDevSound == NULL)
  		User::Leave(KErrNotReady);

	TMMFState prioritySettingsState = iPrioritySettings.iState; //should be EMMFStateRecording
	//to use the GetSupportedOutputDatatypes but we'll save it just in case it's not
	iPrioritySettings.iState = EMMFStateRecording; //if playing does not support any output data types
	RArray<TFourCC> supportedDataTypes;
	//note Output data types becuase if we are recording audio ie audio input
	//the data is sent as an output from DevSound
	TRAPD(err, iMMFDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, iPrioritySettings));
	iPrioritySettings.iState = prioritySettingsState;
	if (err == KErrNone)
		{
		if (supportedDataTypes.Find(iSinkFourCC) == KErrNotFound)
			{//the source fourCC code could not be found in the list of 
			//data types supported by the Devsound therefor default to pcm16
			iDataTypeCode = KMMFFourCCCodePCM16;	
			}
		else
			{
			//the DevSound does support the same datatype as the source 
			//so set the fourcc to that of the source
			iDataTypeCode = iSinkFourCC;
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

    // Prevent defect when SourcePrimeL is called before NegotiateSourceL()
	// since characterization is ambiguous
    if(iState == EDevSoundReady)
      {
      iState = EIdle;
      }

  	// moved from LoadL - fix for DEF037168 - AD
	iMMFDevSound->InitializeL(*this, iDataTypeCode, EMMFStateRecording);

	// In some implementations InitializeComplete is sent
	// in context, so check before starting activeSchedulerWait.
	if (iState != EDevSoundReady)
		{
		iInitializeState = KRequestPending;
		iActiveSchedulerWait->Start();
		}		
	User::LeaveIfError(iInitializeState);

	iMMFDevSound->SetPrioritySettings(iPrioritySettings);

	// Attempt to configure DevSound to the same settings as the sink.
	// Need to do this after calling CMMFDevSound::InitializeL() as
	// this sets up the device capabilities 
	// (returned by iMMFDevSound->Capabilities()).
	ConfigDevSoundL();
	}

/**
Sets the source's priority settings.

@param  aPrioritySettings
        The source priority settings. Takes enumerations to determine audio record priority. Higher 
        numbers mean high priority (can interrupt lower priorities).
*/
void CMMFAudioInput::SetSourcePrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iPrioritySettings = aPrioritySettings;
	if (!iMMFDevSound)
		Panic(EMMFAudioInputDevSoundNotLoaded);
	else
		iMMFDevSound->SetPrioritySettings(iPrioritySettings);
	}


/**
Gets the data type code for the source specified by the media ID.

@param  aMediaId
        An optional parameter to specifiy a specific stream when the datasource contains more than
        one stream of data.

@return The 4CC of the data supplied by this source.
*/
TFourCC CMMFAudioInput::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return iDataTypeCode;
	}

/**
Sets the data type code for the source.

@param  aSourceFourCC
        The 4CC of the data supplied by this source.
@param  aMediaId
        The Media ID. An optional parameter to specifiy specific stream when datasource contains
        more than one stream of data.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CMMFAudioInput::SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId /*aMediaId*/)
	{
	iDataTypeCode = aSourceFourCC;
	return KErrNone;
	}

/**
Gets the number of bytes played.

@return	The number of bytes played. If 16-bit divide this number returned by 2 to get word length.
*/
TInt CMMFAudioInput::BytesPlayed()
	{
	if (!iMMFDevSound)
		Panic(EMMFAudioInputDevSoundNotLoaded);
	return iMMFDevSound->SamplesPlayed();
	}

/**
Returns the sound device.

@pre
Dev Sound should be loaded.

Accessor function exposing public CMMFDevsound methods.

@return A reference to a CMMFDevSound objector.
*/
CMMFDevSound& CMMFAudioInput::SoundDevice()
	{
	if (!iMMFDevSound)
		{
		Panic(EMMFAudioInputDevSoundNotLoaded);
		}
	return *iMMFDevSound;
	}


/**
@deprecated

This method should not be used - it is provided to maintain SC with v7.0s.

@param  aAudioType
        The 4CC of the data supplied by this source.
*/
void CMMFAudioInput::SetDataTypeL(TFourCC aAudioType)
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
TFourCC CMMFAudioInput::DataType() const
	{
	return KMMFFourCCCodePCM16;
	}


/**
Loads audio device drivers and initialise this device.
*/
void CMMFAudioInput::LoadL()
	{
	//[ do all the work that can fail first
	// before we modify the internal state ]

	iMMFDevSound = CMMFDevSound::NewL();
	iFirstBufferRequested = EFalse;
	iFirstBufferRead = EFalse;
	if (iState != EDevSoundReady) 
		{
		iState = EIdle;
		}

	iDevSoundLoaded = ETrue;

	//[ assert dev sound has been constructed]
	ASSERT( iMMFDevSound );
	ASSERT( iDevSoundLoaded );
    ASSERT( !iFirstBufferRead );
	}

void CMMFAudioInput::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /* aMsg */)
	{
	}

void CMMFAudioInput::InitializeComplete(TInt aError)
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
ToneFinished MMFDevSoundObserver - should never get called.
*/
void CMMFAudioInput::ToneFinished(TInt /*aError*/)
	{
	//we should never get here during a record session!
	__ASSERT_DEBUG(EFalse,Panic(EMMFAudioInputPanicToneFinishedNotSupported));
	}


/**
BuffferToBeEmptied MMFDevSoundObserver
Called when stopped due to error.
*/
void CMMFAudioInput::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	iDevSoundBuf = aBuffer;

	if (iFirstBufferRequested)
		{
		if (iPausePending)
			{
			aBuffer->SetLastBuffer(ETrue);
			iPausePending = EFalse;
			}

#ifdef _DEBUG
		TRAPD(err, iConsumer->BufferFilledL(aBuffer));
		__ASSERT_DEBUG(!err, Panic(err));
#else
		TRAP_IGNORE(iConsumer->BufferFilledL(aBuffer));
#endif

		iFirstBufferRead = ETrue;
		}
	}	


/**
RecordError MMFDevSoundObserver
Called when stopped due to error.
*/
void CMMFAudioInput::RecordError(TInt aError)
	{
	//[  two event categories will be used
	// which mirrors the datapath response ]

	//[ record the error ]
	iMMFDevsoundError = aError;
	TMMFEvent event( KMMFEventCategoryPlaybackComplete, aError);
	
	//[ send the event to the client.
	SendEventToClient(event);    
	
	// clear flags if there is an error.
	iPausePending = EFalse;

	//[ we are not going to stop devsound ]
	}


/**
BufferToBeFilled MMFDevSoundObserver - should never get called.
*/
void CMMFAudioInput::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	//we should never get here during a play session!
	__ASSERT_DEBUG(EFalse, Panic(EMMFAudioInputPanicPlayerDataUsedNotSupported));
	}


/**
PlayError MMFDevSoundObserver - should never get called.
*/
void CMMFAudioInput::PlayError(TInt /*aError*/)
	{
	//we should never get here during a record session!
	__ASSERT_DEBUG(EFalse, Panic(EMMFAudioInputPanicPlayErrorNotSupported));
	}


/**
ConvertError MMFDevSoundObserver - should never get called.
*/
void CMMFAudioInput::ConvertError(TInt /*aError*/)
	{
	}


void CMMFAudioInput::SendEventToClient(const TMMFEvent& aEvent)
	{
	iEventHandler->SendEventToClient(aEvent);
	}

// _________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidAudioInputInterface,	CMMFAudioInput::NewSourceL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

