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
//

#include "tonedatapath.h"


CToneDataPath* CToneDataPath::NewL()
	{
	CToneDataPath* self = new(ELeave) CToneDataPath;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CToneDataPath::ConstructL()
	{
	iAudioPlayer = new (ELeave) CToneDataPathPlayer(*this,CActive::EPriorityUserInput);
	iSoundDeviceErrorReceiver = new (ELeave) CToneSoundDevPlayErrorReceiver(*this, CActive::EPriorityUserInput);
	}


CToneDataPath::~CToneDataPath()
	{
	delete iAudioPlayer;
	delete iSoundDeviceErrorReceiver;
	
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


TInt CToneDataPath::SetObserver(MMMFHwDeviceObserver& aObserver)
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


TInt CToneDataPath::AddCodec(CToneCodec& aCodec)
	{
	if (iCodec)
		{
		return KErrNotSupported; //doesn't support multiple codecs
		}

	TInt err = KErrNone;
	
	iCodec = &aCodec;

	// Allocate data buffer
	iSourceBufferSize = iCodec->SourceBufferSize();
	iSoundDevBufferSize = iCodec->SinkBufferSize();

	if ((!iSourceBufferSize)||(!iSoundDevBufferSize))
		{
		err = KErrArgument; //codec plugin has not specified buffer size
		}

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

TInt CToneDataPath::Start()
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
			RDebug::Print(_L("CToneDataPath::Start-Resume"));
#endif
			iAudioPlayer->ResumePlaying();
			iState = EPlaying;
			}
		}
	else if (!startError)
		{
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CToneDataPath::Start-Normal"));
#endif
		// get sample rate and channels from RMdaDevSound
		RMdaDevSound::TCurrentSoundFormatBuf format;
		iSoundDevice.GetPlayFormat(format);
		iSampleRate = format().iRate;
		iChannels = format().iChannels;
		
		iNoMoreSourceData = EFalse;
		iSourceBuffer->SetLastBuffer(EFalse);
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

void CToneDataPath::FillSourceBufferL()
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


void CToneDataPath::BufferFilledL(CMMFDataBuffer& aBuffer)
	{//call back from observer to indicate buffer has been filled
	
	if (iState == EStopped)
		{
		User::Leave(KErrNotReady);//ok if paused?
		}

	iSourceBuffer = &aBuffer;
	iSourceBuffer->SetStatus(EFull);
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CToneDataPath::BufferFilledL"));
#endif

	//need to check that the buffer size is not 0 - if so assume we've reached the end of the data
	iBuffSize = iSourceBuffer->BufferSize();
	if (!iBuffSize)
		{//no buffer  - could be end of source or could be that the source has no data??
		iNoMoreSourceData = ETrue;
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CToneDataPath::BufferFilledL-NoMoreSourceData"));
#endif
		}
	//even if the buffer size is 0 we still 
	//need to perform the following to get the sound device callback
	FillSoundDeviceBufferL(); //get buffer in pcm16 format for sound device	
	
	iAudioPlayer->PlayData(*iSoundDeviceBuffer); //play data to sound drivers
	}


void CToneDataPath::FillSoundDeviceBufferL()
	{//use CToneCodec to fill the sound device buffer
	
	CToneCodec::TCodecProcessResult codecProcessResult;

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
			{
			iSoundDeviceBuffer->SetLastBuffer(ETrue);
			}
		if ((!iSoundDeviceBuffer->BufferSize())&&(codecProcessResult.iDstBytesAdded))
			{//the codec has added data but not set the buffer length
			iSoundDeviceBuffer->Data().SetLength(codecProcessResult.iDstBytesAdded);
			}
		//only supports EProcessComplete
		switch (codecProcessResult.iCodecProcessStatus)
			{
		case CToneCodec::TCodecProcessResult::EProcessComplete:
		//finished procesing source data - all data in sink buffer
			{
			iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC
		case CToneCodec::TCodecProcessResult::EProcessIncomplete:
		//finished procesing source data - all data in sink buffer
			{
			iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
#endif
		case CToneCodec::TCodecProcessResult::EDstNotFilled:
		//could be the last buffer in which case dst might not get filled
			{
			iSoundDeviceBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
		case CToneCodec::TCodecProcessResult::EEndOfData:
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
			//Panic(EMMFSwCodecWrapperBadCodec); //should never get here - bad codec
			break;
			}
		}
	}


void CToneDataPath::BufferEmptiedL(const CMMFDataBuffer& aBuffer)
	{//call back from CToneDataPathPlayer when the sound device buffer has been emptied
	if (&aBuffer != iSoundDeviceBuffer) 
		{
		Panic(EToneBadBuffer);
		}

	if (!iNoMoreSourceData) 
		{
		FillSourceBufferL();
		}
	}

//*** End of Main Play Loop ***


void CToneDataPath::Stop()
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


void CToneDataPath::Pause()
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
	
	
TInt CToneDataPath::EmptyBuffers()
	{
	TInt error = KErrNone;
	if (iSoundDevice.Handle() == 0)
		{
		error = KErrNotReady;		
		}
	else
		{
		iSoundDevice.FlushPlayBuffer();
		}
	return error;
	}	


RMdaDevSound& CToneDataPath::Device()
	{
	return iSoundDevice;
	}


void CToneDataPath::SoundDeviceException(TInt aError)
	{
	if(iIgnoreUnderflow)
		{
		if((aError == KErrUnderflow) && (!iNoMoreSourceData))
			{
			//ignore underflow
			return;
			}
		}

	//this sends a request to the hw device observer
	//to update the bytes played
	//it is done here so that the sound driver can be closed prior to
	//updating the policy and sending the error back
	TUid uidUpdateBytesPlayed;
	uidUpdateBytesPlayed.iUid = KToneHwDeviceObserverUpdateBytesPlayed;
	TPtrC8 dummy(0,0);
	
	ASSERT(iHwDeviceObserver);
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
	
	RDebug::Print(_L("CToneDataPath::iHwDeviceObserver->Error(%d)"),aError);
	}

/**
Retrieves a custom interface to the device.
The reference CToneDataPath supports three  custom interfaces,
MEmptyBuffersCustomInterface, MSetVbrFlagCustomInterface and MIgnoreUnderflowEventsCustomInterface

@param	aInterface
		Interface UID, defined with the custom interface.
		aInterface = KMmfUidEmptyBuffersCustomInterface for MEmptyBuffersCustomInterface,
					 KSetVbrFlagCustomInterfaceTypeUid for MSetVbrFlagCustomInterface
		
@return A pointer to the interface implementation, or NULL if the device can not
		implement the interface requested. The return value must be cast to the
		correct type by the user.
*/
TAny* CToneDataPath::CustomInterface(TUid aInterface)
	{
	TAny* ret = NULL;

	if (aInterface == KIgnoreUnderflowCustomInterfaceTypeUid)
		{
		MIgnoreUnderflowEventsCustomInterface* result = static_cast<MIgnoreUnderflowEventsCustomInterface*> (this);
		ret = static_cast<TAny*>(result);
		}
	return ret;
	}


void CToneDataPath::IgnoreUnderflowEvents()
	{
	iIgnoreUnderflow = ETrue;
	}



/************************************************************************
 *				CDataPathPlayer											*
 ************************************************************************/

CToneDataPathPlayer::CToneDataPathPlayer(CToneDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}


CToneDataPathPlayer::~CToneDataPathPlayer()
	{
	Cancel();
	}


void CToneDataPathPlayer::Start()
	{
	// No implementation
	}


void CToneDataPathPlayer::ResumePlaying()
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


void CToneDataPathPlayer::PlayData(const CMMFDataBuffer& aData)
	{
	iDataFromSource = &aData;
	if (!IsActive())
		{
#ifdef _SCW_DEBUG
		RDebug::Print(_L("CToneDataPathPlayer::PlayData"));
#endif
		iParent.Device().PlayData(iStatus,(static_cast<const CMMFDataBuffer*> (iDataFromSource))->Data());
		SetActive();
		}
	}


void CToneDataPathPlayer::Stop()
	{
	if (!IsActive())
		{
		iParent.Device().FlushPlayBuffer(); // Otherwise won't be flushed
		}
	Cancel();
	iParent.SoundDeviceException(KErrCancel);
	}


void CToneDataPathPlayer::RunL()
	{
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CToneDataPathPlayer::RunL error[%d]"), iStatus.Int());
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


TInt CToneDataPathPlayer::RunError(TInt aError)
	{
	Error(aError);
	return KErrNone;
	}


void CToneDataPathPlayer::DoCancel()
	{
	if (iParent.Device().Handle())
		{
		iParent.Device().CancelPlayData();
		iParent.Device().FlushPlayBuffer();
		}
	}


void CToneDataPathPlayer::Error(TInt aError)
	{ 
	iParent.SoundDeviceException(aError);
	}



/************************************************************************
 *				CToneSoundDevPlayErrorReceiver							*
 ************************************************************************/

CToneSoundDevPlayErrorReceiver::CToneSoundDevPlayErrorReceiver(CToneDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CToneSoundDevPlayErrorReceiver::~CToneSoundDevPlayErrorReceiver()
	{
	Cancel();
	}

void CToneSoundDevPlayErrorReceiver::Start()
	{
	iParent.Device().NotifyPlayError(iStatus);
	SetActive();
	}

void CToneSoundDevPlayErrorReceiver::Stop()
	{
	Cancel();
	}

void CToneSoundDevPlayErrorReceiver::RunL()
	{
	TInt reason = iStatus.Int();
	Start();

	// An error has been returned
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CToneSoundDevPlayErrorReceiver::RunL[%d]"), reason);
#endif
	iParent.SoundDeviceException(reason);
	}

void CToneSoundDevPlayErrorReceiver::DoCancel()
	{
	iParent.Device().CancelNotifyPlayError();
	}



/*
 * CycleAudioBufferL
 *
 * Sets up a usable buffer for passing to MMF
 *	
 * This method has been written such that it must allocate a new buffer before
 * replacing the existing one. The purpose of this is to force creation of a 
 * new buffer. Simply deleting and then re-allocing may result in the same 
 * address being used.
 * 
 * Only cycles if there is enough memory
 *
 */
#ifdef __CYCLE_MMF_DATABUFFERS__
CMMFDataBuffer* CToneDataPath::CycleAudioBuffer(CMMFDataBuffer* aBuffer)
	{
	CMMFDataBuffer* buffer = NULL;
	TUint bufferSize = aBuffer->Data().MaxLength();

#ifdef __USE_MMF_TRANSFERBUFFERS__
	TRAPD(err, buffer = CreateTransferBufferL(bufferSize, static_cast<CMMFTransferBuffer*>(aBuffer)));
#else
	TRAPD(err,buffer = CMMFDataBuffer::NewL(bufferSize));

	if (err == KErrNone)
		{
		delete aBuffer;
		}
#endif
	if (err != KErrNone)
		{//there was a problem creating buffer eg OOM so use same buffer
		buffer = aBuffer;
		}

	return buffer;

	}
#endif

/*
 * DoCleanupRHandleBase
 *
 * This method will initially Close the handle and then delete it.
 *
 */
#ifdef __USE_MMF_TRANSFERBUFFERS__
inline static void DoCleanupRHandleBase(TAny* aRHandleBase)
	{
	ASSERT(aRHandleBase);
	RHandleBase* rHandleBase = static_cast<RHandleBase*> (aRHandleBase);
	TRAPD(error, rHandleBase->Close());
	delete aRHandleBase;
	}

CMMFTransferBuffer* CToneDataPath::CreateTransferBufferL(TUint aBufferSize, CMMFTransferBuffer* aOldBuffer)
	{
	CMMFTransferBuffer* buffer = NULL;

	RTransferBuffer* transBuffer = new  (ELeave) RTransferBuffer;
	
	TCleanupItem bufferCleanupItem(DoCleanupRHandleBase, transBuffer); //closes and deletes.
	CleanupStack::PushL(bufferCleanupItem);

	RTransferWindow* transWindow = new (ELeave) RTransferWindow;
	
	TCleanupItem windowCleanupItem(DoCleanupRHandleBase, transWindow); //closes and deletes.
	CleanupStack::PushL(windowCleanupItem);

	User::LeaveIfError(transBuffer->Create(aBufferSize));
	User::LeaveIfError(transWindow->Create(aBufferSize));
	User::LeaveIfError(transWindow->MapInBuffer(*transBuffer));

	buffer = CMMFTransferBuffer::NewL(*transWindow);

	delete aOldBuffer; //closes RTransferWindow
	delete iTransferWindow;

	if(iTransferBuffer)
		{
		iTransferBuffer->Close();
		}
	delete iTransferBuffer;

	iTransferBuffer = transBuffer;
	iTransferWindow = transWindow;

	CleanupStack::Pop(transWindow); 
	CleanupStack::Pop(transBuffer); 

	return buffer;
	}
#endif


#ifdef __USE_MMF_PTRBUFFERS__
CMMFPtrBuffer* CToneDataPath::CreatePtrBufferL(TUint aBufferSize)
	{
	CMMFPtrBuffer* buffer = NULL;
	if (iPtrBufferMemoryBlock)
		{
		delete iPtrBufferMemoryBlock;//incase already exisits
		}
	iPtrBufferMemoryBlock = HBufC8::NewL(aBufferSize);
	TPtr8 ptrMemoryBlock(iPtrBufferMemoryBlock->Des());
	buffer = CMMFPtrBuffer::NewL(ptrMemoryBlock);
	return buffer;
	}
#endif  // __USE_MMF_PTRBUFFERS__



