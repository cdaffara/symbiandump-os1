// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// source\server\mmfswcodecplaydatapath.cpp
// 
//

#include "mmfSwCodecPlayDataPath.h"
#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#include <mmf/common/mmfpaniccodes.h>
#include "mmfSwCodecUtility.h"

const TInt KBytesPerSample = 2;
const TInt KMaxBytesInSec = 192000; //considering maximum samplerate 96KHz
CMMFSwCodecPlayDataPath* CMMFSwCodecPlayDataPath::NewL()
	{
	CMMFSwCodecPlayDataPath* self = new(ELeave) CMMFSwCodecPlayDataPath;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CMMFSwCodecPlayDataPath::ConstructL()
	{
	iAudioPlayer = new (ELeave) CDataPathPlayer(*this,CActive::EPriorityUserInput);
	iSoundDeviceErrorReceiver = new (ELeave) CSoundDevPlayErrorReceiver(*this, CActive::EPriorityUserInput);
	iUtility = CMMFSwCodecUtility::NewL();
	iVbrFlag = EFalse;
	}


CMMFSwCodecPlayDataPath::~CMMFSwCodecPlayDataPath()
	{
	delete iAudioPlayer;
	delete iSoundDeviceErrorReceiver;
	delete iUtility;

	iSoundDevice.Close();

	if (iCodec)
		{
		delete iSourceBuffer;
		if (!iCodec->IsNullCodec()) 
			{
			delete iSoundDeviceBuffer;
			}
		}

#ifdef __USE_MMF_TRANSFERBUFFERS__
	delete iTransferWindow;

	if(iTransferBuffer)
		{
		iTransferBuffer->Close();
		delete iTransferBuffer;
		}
#endif

#ifdef __USE_MMF_PTRBUFFERS__
	delete iPtrBufferMemoryBlock;
#endif
	}


TInt CMMFSwCodecPlayDataPath::SetObserver(MMMFHwDeviceObserver& aObserver)
	{
	TInt error;
	if (iHwDeviceObserver)
		{
		error =  KErrAlreadyExists;
		}
	else
		{
		iHwDeviceObserver = &aObserver;
		error  = KErrNone;
		}
	return error;
	}


TInt CMMFSwCodecPlayDataPath::AddCodec(CMMFSwCodec& aCodec)
	{
	if (iCodec)
		return KErrNotSupported; //doesn't support multiple codecs

	TInt err = KErrNone;
	
	iCodec = &aCodec;

	// Allocate data buffer
	iSourceBufferSize = iCodec->SourceBufferSize();
	iSoundDevBufferSize = iCodec->SinkBufferSize();

	if ((!iSourceBufferSize)||(!iSoundDevBufferSize))
		err = KErrArgument; //codec plugin has not specified buffer size

	if (err == KErrNone)
		{
#ifdef __USE_MMF_TRANSFERBUFFERS__
		TRAP(err,iSourceBuffer = CreateTransferBufferL(iSourceBufferSize, static_cast<CMMFTransferBuffer*>(iSourceBuffer)));
#endif
#ifdef __USE_MMF_PTRBUFFERS__
		TRAP(err,iSourceBuffer = CreatePtrBufferL(iSourceBufferSize));
#else
		TRAP(err,iSourceBuffer = CMMFDataBuffer::NewL(iSourceBufferSize));
#endif
		}
	
	if (err == KErrNone)
		{
		if (iCodec->IsNullCodec())
			{//use source buffer for sound device buffer	
			iSoundDeviceBuffer = NULL;
			}
		else
			{//codec needs separate source and sound device buffers
			TRAP(err,iSoundDeviceBuffer = CMMFDataBuffer::NewL(iSoundDevBufferSize));
			}
		}
	return err;
	}


TInt CMMFSwCodecPlayDataPath::Start()
	{
	TInt startError = KErrNone;

	if (!iCodec) 
		{//check that a codec has been added
		startError = KErrNotReady;
		}
	if ((!iSoundDevice.Handle())&&(!startError))
    	{//check that the sound drivers can be opened
   		startError = iSoundDevice.Open();
		}

	if (iState == EPaused)
		{//we are paused so need to resume play
		if (!startError)
			{
#ifdef _SCW_DEBUG
			RDebug::Print(_L("CMMFSwCodecPlayDataPath::Start-Resume"));
#endif
			iAudioPlayer->ResumePlaying();
			iState = EPlaying;
			}
		}
	else if (!startError)
		{
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CMMFSwCodecPlayDataPath::Start-Normal"));
#endif
		// get sample rate and channels from RMdaDevSound
		RMdaDevSound::TCurrentSoundFormatBuf format;
		iSoundDevice.GetPlayFormat(format);
		iSampleRate = format().iRate;
		iChannels = format().iChannels;
		
		iNoMoreSourceData = EFalse;
		iNoMoreSoundDeviceData = EFalse;
		iSourceBuffer->SetLastBuffer(EFalse);
		iBytesPlayed = 0;
		iState = EPlaying;
		iSoundDeviceErrorReceiver->Start();
		TRAP(startError,FillSourceBufferL()); //get initial buffer of audio data
		if (startError == KErrNone)
			{
			// Start the player objects
			iAudioPlayer->Start();
			}
		else
			{//failed to start up correctly go back to stopped state
			iState = EStopped;
			iSoundDeviceErrorReceiver->Stop();
			}
   		}
	return startError;
	}


// *** Main Play Loop ***

void CMMFSwCodecPlayDataPath::FillSourceBufferL()
	{//asks observer to fill the source buffer          
    // Ask immediately for data from the observer
#ifdef __CYCLE_MMF_DATABUFFERS__
	// Create a new buffer to replicate INC021405 Play-EOF-Play on HwAccelerated solution Panics
	// If the creation fails, we carry on regardless as the original buffer will not have been 
	// destroyed. Must do this as alloc fail tests will not run.
	if(iSourceBuffer)
		{
		iSourceBuffer = CycleAudioBuffer(iSourceBuffer);
		}
#endif // __CYCLE_MMF_DATABUFFERS__	
	User::LeaveIfError(iHwDeviceObserver->FillThisHwBuffer(*iSourceBuffer));
	
	}


void CMMFSwCodecPlayDataPath::BufferFilledL(CMMFDataBuffer& aBuffer)
	{//call back from observer to indicate buffer has been filled
	if (iState == EStopped)
		User::Leave(KErrNotReady);//ok if paused?

	iSourceBuffer = &aBuffer;
	iSourceBuffer->SetStatus(EFull);
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CMMFSwCodecPlayDataPath::BufferFilledL"));
#endif

	//need to check that the buffer size is not 0 - if so assume we've reached the end of the data
	if (!iSourceBuffer->BufferSize())
		{//no buffer  - could be end of source or could be that the source has no data??
		iNoMoreSourceData = ETrue;
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CMMFSwCodecPlayDataPath::BufferFilledL-NoMoreSourceData"));
#endif
		}
	//even if the buffer size is 0 we still 
	//need to perform the following to get the sound device callback
	FillSoundDeviceBufferL(); //get buffer in pcm16 format for sound device	

    /* iVbrFlag is added to datapath to avail the alternative dataflow wherein datapath makes sure that 
    destinationbuffer is filled to its maximum length before sending it to the sound driver. 
    Sending the buffer directly to the device causes underflow incase of Vorbis codecs.*/
    if (iVbrFlag)
    	{
    	/*There are two cases we need to deal here
      	1. When the output of the codec is 0 for header data.
           in that case, repeat till actual decoding of ogg packets and pages.
        2. When destination buffer is not filled even to its half length, get next source buffer
           and decode it. This is to avoid underflows when ever we receive little pcm for a 
           a given source buffer.
           */
	    if (iSoundDeviceBuffer->Data().Length() < iSoundDeviceBuffer->Data().MaxLength()/2 && !(iSoundDeviceBuffer->LastBuffer()))
			{
	    	iSourceBuffer->SetStatus(EAvailable); //source buffer is now available
	    	iSoundDeviceBuffer->SetPosition(iSoundDeviceBuffer->Data().Length());//this indicates the available space in the buffer to the codec
	    	FillSourceBufferL();	
	    	return;
	    	}
		else //data is sufficient to avoid underflows
			{
			iSoundDeviceBuffer->SetPosition(0);
			if(iSoundDeviceBuffer->Data().Length()==0 && iSoundDeviceBuffer->LastBuffer())
				{
				iNoMoreSoundDeviceData = ETrue;
				}
			}	
    	}

    // attenuate the amplitude of the samples if volume ramping has been changed
	// and is non-zero
	if (iCustomInterface)
		{
		TTimeIntervalMicroSeconds volumeRamp = iCustomInterface->VolumeRamp();
		if (volumeRamp != iVolumeRamp)
			{
			iVolumeRamp = volumeRamp;
			if (iVolumeRamp.Int64() != 0)
				{
				iUtility->ConfigAudioRamper(
					iVolumeRamp.Int64(), 
					iSampleRate, 
					iChannels);
				iRampAudioSample = ETrue;
				}
			else
				{
				iRampAudioSample = EFalse;
				}

			}
			if (iRampAudioSample)
				iRampAudioSample = iUtility->RampAudio(iSoundDeviceBuffer);
		}

	iAudioPlayer->PlayData(*iSoundDeviceBuffer); //play data to sound drivers

	if (iSourceBuffer->LastBuffer())//check last buffer flag
		{
		iNoMoreSourceData = ETrue;
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CMMFSwCodecPlayDataPath::BufferFilledL-LBNoMoreSourceData"));
#endif
		}
	}


void CMMFSwCodecPlayDataPath::FillSoundDeviceBufferL()
	{//use CMMFSwCodec to fill the sound device buffer
	
	CMMFSwCodec::TCodecProcessResult codecProcessResult;

	if (iCodec->IsNullCodec())
		{//no codec so data can be sent direct to sink
		iSoundDeviceBuffer = iSourceBuffer;
		iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full
		}	
	else 
		{	
		//pass buffer to codec for processing
		codecProcessResult = iCodec->ProcessL(*iSourceBuffer, *iSoundDeviceBuffer);
		
		if (iSourceBuffer->LastBuffer()) //if source is last buffer so is sound dev
			iSoundDeviceBuffer->SetLastBuffer(ETrue);
		if ((!iSoundDeviceBuffer->BufferSize())&&(codecProcessResult.iDstBytesAdded))
			{//the codec has added data but not set the buffer length
			iSoundDeviceBuffer->Data().SetLength(codecProcessResult.iDstBytesAdded);
			}
		//only supports EProcessComplete
		switch (codecProcessResult.iCodecProcessStatus)
			{
		case CMMFSwCodec::TCodecProcessResult::EProcessComplete:
		//finished procesing source data - all data in sink buffer
			{
			iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC
		case CMMFSwCodec::TCodecProcessResult::EProcessIncomplete:
		//finished procesing source data - all data in sink buffer
			{
			iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
#endif
		case CMMFSwCodec::TCodecProcessResult::EDstNotFilled:
		//could be the last buffer in which case dst might not get filled
			{
			iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
		case CMMFSwCodec::TCodecProcessResult::EEndOfData:
			//no more data - send what we've got to the sink
			//note we can't always rely on this  - in many cases the codec will not know when
			//it has reached the end of data.
			{
			iSoundDeviceBuffer->SetStatus(EFull);//sink buffer may not really be 'full' but its as full as it going to get
			iNoMoreSourceData = ETrue;
			//doesn't matter if sink buffer is not full
			}
		break;
		default:
			Panic(EMMFSwCodecWrapperBadCodec); //should never get here - bad codec
			}
		}
	}


void CMMFSwCodecPlayDataPath::BufferEmptiedL(const CMMFDataBuffer& aBuffer)
	{//call back from CDataPathPlayer when the sound device buffer has been emptied
	if (&aBuffer != iSoundDeviceBuffer) 
		Panic(EMMFSwCodecWrapperBadBuffer);
	if(iVbrFlag && (iSourceBuffer->Status() == EUnAvailable || iNoMoreSourceData))
		{//No more source data. Play rest of the decoded data.Inform codec not to consider the source buffer
		if(iSourceBuffer->Status()!=EUnAvailable)
			{
			iSourceBuffer->SetStatus(EUnAvailable);
			}
		FillSoundDeviceBufferL();
		if(iSoundDeviceBuffer->BufferSize() > 0)
			{
			// attenuate the amplitude of the samples if volume ramping has been changed
			// and is non-zero
			if (iCustomInterface)
				{
				TTimeIntervalMicroSeconds volumeRamp = iCustomInterface->VolumeRamp();
				if (volumeRamp != iVolumeRamp)
					{
					iVolumeRamp = volumeRamp;
					if (iVolumeRamp.Int64() != 0)
						{
						iUtility->ConfigAudioRamper(iVolumeRamp.Int64(), iSampleRate, iChannels);
						iRampAudioSample = ETrue;
						}
					else
						{
						iRampAudioSample = EFalse;
						}

					}
				if (iRampAudioSample)
					{
					iRampAudioSample = iUtility->RampAudio(iSoundDeviceBuffer);
					}
					
				}
			iAudioPlayer->PlayData(*iSoundDeviceBuffer); //play data to sound drivers
			return;
			}
		else
			{
			if(iNoMoreSourceData)
				{
				iNoMoreSoundDeviceData = ETrue;
				}
			iSourceBuffer->SetStatus(EAvailable);
			}
		}
	if (!iNoMoreSourceData) 
		FillSourceBufferL();
	}

//*** End of Main Play Loop ***


void CMMFSwCodecPlayDataPath::Stop()
	{
	iAudioPlayer->Cancel();
	iSoundDeviceErrorReceiver->Cancel();
    iSoundDevice.Close();

#ifdef __CYCLE_MMF_DATABUFFERS__
	// Create a new buffer to replicate INC021405 Play-EOF-Play on HwAccelerated solution Panics
	// If the creation fails, we carry on regardless as the original buffer will not have been 
	// destroyed. Must do this as alloc fail tests will not run.
	if(iSourceBuffer)
		{
		iSourceBuffer = CycleAudioBuffer(iSourceBuffer);
		}
#endif // __CYCLE_MMF_DATABUFFERS__	

	iState = EStopped;
	}


void CMMFSwCodecPlayDataPath::Pause()
	{
	//since a pause can happen anyway in the datatransfer -need to set to a known 
	//state so that when play is resumed the behaviour is predictable
	if (iSoundDevice.Handle())
		{
		iSoundDevice.PausePlayBuffer(); //needs new LDD
		iState = EPaused;
#ifdef _SCW_DEBUG
		RDebug::Print(_L("Pause"));
#endif
		}
	else
		{//an error must have occured 
		iState = EStopped;
		}
	}
	
	
TInt CMMFSwCodecPlayDataPath::EmptyBuffers()
	{
	TInt error = KErrNone;
	if (iSoundDevice.Handle() == 0)
		{
		error = KErrNotReady;
		}
	else
		{ 
		iAudioPlayer->Cancel();
		iSoundDevice.CancelPlayData();
		iSoundDeviceErrorReceiver->Stop();
		iState = EStopped;
		}
	return error;
	}	


RMdaDevSound& CMMFSwCodecPlayDataPath::Device()
	{
	return iSoundDevice;
	}


void CMMFSwCodecPlayDataPath::SoundDeviceException(TInt aError)
	{
	if(iIgnoreUnderflow)
		{
		if(!iVbrFlag && aError==KErrUnderflow && !iNoMoreSourceData)
			{
			//ignore underflow
			return;
			}
		//for VBR codec data,no more source does not mean that no more sounddevice data
		//so ignore underflows till the last buffer is played from the codec
		else if(iVbrFlag && aError==KErrUnderflow && !iNoMoreSoundDeviceData)
			{
			//ignore underflow
			return;
			}	
		}
	
	//this sends a request to the hw device observer usually Devsound
	//to update the bytes played
	//it is done here so that the sound driver can be closed prior to
	//updating the plicy and sending the error back
	TUid uidUpdateBytesPlayed;
	uidUpdateBytesPlayed.iUid = KMmfHwDeviceObserverUpdateBytesPlayed;
	TPtrC8 dummy(0,0);
	iHwDeviceObserver->MsgFromHwDevice(uidUpdateBytesPlayed,dummy);

	//this closes RMdaDevSound.
	Stop(); 

	//inform devsound so it can update policy
	iHwDeviceObserver->Stopped(); 

	// Inform the observer of the exception condition
	// We inform the hw device observer after the policy has been
	// updated incase the observer relied on the error to assume
	// the policy has been updated
	iHwDeviceObserver->Error(aError);
	}


void CMMFSwCodecPlayDataPath::SetPlayCustomInterface(MPlayCustomInterface& aCustomInterface)
	{
	iCustomInterface = &aCustomInterface;
	}
	
/**
Retrieves a custom interface to the device.
The reference CMMFSwCodecWrapper supports two  custom interfaces,
MEmptyBuffersCustomInterface and MSetVbrFlagCustomInterface

@param	aInterface
		Interface UID, defined with the custom interface.
		aInterface = KMmfUidEmptyBuffersCustomInterface for MEmptyBuffersCustomInterface,
					 KSetVbrFlagCustomInterfaceTypeUid for MSetVbrFlagCustomInterface
		
@return A pointer to the interface implementation, or NULL if the device can not
		implement the interface requested. The return value must be cast to the
		correct type by the user.
*/
TAny* CMMFSwCodecPlayDataPath::CustomInterface(TUid aInterface)
	{
	TAny* ret = NULL;
	if (aInterface.iUid == KMmfUidEmptyBuffersCustomInterface)
		{
		MEmptyBuffersCustomInterface* result = static_cast<MEmptyBuffersCustomInterface*> (this);
		ret = static_cast<TAny*>(result);
		}
	else if(aInterface.iUid == KSetVbrFlagCustomInterfaceTypeUid)
		{
		SetVbrFlag();
		}
	if (aInterface == KTimePlayedCustomInterfaceTypeUid)
		{
		MTimePlayedCustomInterface* result = static_cast<MTimePlayedCustomInterface*> (this);
		ret = static_cast<TAny*>(result);
		}
	if (aInterface == KIgnoreUnderflowCustomInterfaceTypeUid)
		{
		MIgnoreUnderflowEventsCustomInterface* result = static_cast<MIgnoreUnderflowEventsCustomInterface*> (this);
		ret = static_cast<TAny*>(result);
		}
	return ret;
	}

/**
Used to set iVbrFlag on the datapath.

This method is used to set the iVbrFlag in datapath. This flag is added to datapath to avail the 
alternative dataflow wherein datapath makes sure that destinationbuffer is filled to its maximum length
before sending it to the sound driver. Sending the buffer directly to the device causes underflow incase of VBR codecs.
*/		
void CMMFSwCodecPlayDataPath::SetVbrFlag()
	{
	iVbrFlag = ETrue;
	}

TInt CMMFSwCodecPlayDataPath::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	if(iSoundDevice.Handle())
		{
		TInt bytes = iSoundDevice.BytesPlayed();
		//Work around for overflow of bytes played from driver.
		//This code will be removed when Base provides the TimePlayed() API which returns the play time
		//Assuming GetTimePlayed() gets called in an interval not more than 3 secs, reset driver's bytes played when it is near KMaxInt
		if(bytes > (KMaxTInt - 3*KMaxBytesInSec))
			{
			iBytesPlayed = iBytesPlayed+bytes;
			iSoundDevice.ResetBytesPlayed();
			bytes = 0;
			}
		TInt64 samplesPlayed = (iBytesPlayed+bytes)/(KBytesPerSample*iChannels);
		aTime = (samplesPlayed*1000000)/iSampleRate;
		}
	else
		{
		aTime = 0;
		}
		
	return KErrNone;
	}

void CMMFSwCodecPlayDataPath::IgnoreUnderflowEvents()
	{
	iIgnoreUnderflow = ETrue;
	}
/************************************************************************
 *				CDataPathPlayer											*
 ************************************************************************/

CDataPathPlayer::CDataPathPlayer(CMMFSwCodecPlayDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}


CDataPathPlayer::~CDataPathPlayer()
	{
	Cancel();
	}


void CDataPathPlayer::Start()
	{
	// No implementation
	}


void CDataPathPlayer::ResumePlaying()
	{
	if (iParent.Device().Handle())
		{
		//should be ok to call this even if we are active
		iParent.Device().ResumePlaying(); 
		iResumePlaying = ETrue;
		}
#ifdef _SCW_DEBUG
	RDebug::Print(_L("Playing Resumed"));
#endif
	}


void CDataPathPlayer::PlayData(const CMMFDataBuffer& aData)
	{
	iDataFromSource = &aData;
	if (!IsActive())
		{
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CDataPathPlayer::PlayData"));
#endif
		iParent.Device().PlayData(iStatus,(STATIC_CAST(const CMMFDataBuffer*, iDataFromSource))->Data());
		SetActive();
		}
	}


void CDataPathPlayer::Stop()
	{
	if (!IsActive())	
		iParent.Device().FlushPlayBuffer(); // Otherwise won't be flushed
	Cancel();
	iParent.SoundDeviceException(KErrCancel);
	}


void CDataPathPlayer::RunL()
	{
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CDataPathPlayer::RunL error[%d]"), iStatus.Int());
#endif
	if (iStatus.Int()!=KErrNone)
		{ 	
		iParent.SoundDeviceException(iStatus.Int());
		}
	else
		{
		iParent.BufferEmptiedL(static_cast<const CMMFDataBuffer&>(*iDataFromSource));
		iResumePlaying = EFalse;
		}
	}


TInt CDataPathPlayer::RunError(TInt aError)
	{
	Error(aError);
	return KErrNone;
	}


void CDataPathPlayer::DoCancel()
	{
	if (iParent.Device().Handle())
		{
		iParent.Device().CancelPlayData();
		iParent.Device().FlushPlayBuffer();
		}
	}


void CDataPathPlayer::Error(TInt aError)
	{ 
	iParent.SoundDeviceException(aError);
	}


/************************************************************************
 *				CSoundDevPlayErrorReceiver											*
 ************************************************************************/

CSoundDevPlayErrorReceiver::CSoundDevPlayErrorReceiver(CMMFSwCodecPlayDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CSoundDevPlayErrorReceiver::~CSoundDevPlayErrorReceiver()
	{
	Cancel();
	}

void CSoundDevPlayErrorReceiver::Start()
	{
	iParent.Device().NotifyPlayError(iStatus);
	SetActive();
	}

void CSoundDevPlayErrorReceiver::Stop()
	{
	Cancel();
	}

void CSoundDevPlayErrorReceiver::RunL()
	{
	TInt reason = iStatus.Int();
	Start();
	// An error has been returned
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CSoundDevPlayErrorReceiver::RunL[%d]"), reason);
#endif
	iParent.SoundDeviceException(reason);
	}

void CSoundDevPlayErrorReceiver::DoCancel()
	{
	iParent.Device().CancelNotifyPlayError();
	}


