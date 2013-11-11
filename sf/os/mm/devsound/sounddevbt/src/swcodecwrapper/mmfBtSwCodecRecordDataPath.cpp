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
// source\server\MmfBtSwCodecRecordDatapath.cpp
// 
//

#include "mmfBtSwCodecRecordDataPath.h"
#include "mmfbtswcodecwrapper.h"
#include <mmfpaniccodes.h>
#include "MMFBtRoutingSoundDevice.h"


CMMFSwCodecRecordDataPath* CMMFSwCodecRecordDataPath::NewL(CRoutingSoundRecordDevice* aSoundDevice)
	{
	CMMFSwCodecRecordDataPath* self = new(ELeave) CMMFSwCodecRecordDataPath(aSoundDevice);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CMMFSwCodecRecordDataPath::ConstructL()
	{
	iAudioRecorder = new (ELeave) CDataPathRecorder(*this,CActive::EPriorityUserInput);
	iSoundDeviceErrorReceiver = new (ELeave) CSoundDevRecordErrorReceiver(*this, CActive::EPriorityUserInput);
	}


CMMFSwCodecRecordDataPath::~CMMFSwCodecRecordDataPath()
	{	
	delete iAudioRecorder;
	delete iSoundDeviceErrorReceiver;

	TRequestStatus status;
	iSoundDevice->CloseDevice(iDeviceUid, status);
	//TODO there should be a timeout for the line below
	User::WaitForRequest(status);

	if (iCodec)
		{
		delete iSoundDeviceBuffer;
		if (!iCodec->IsNullCodec())
			{
			delete iSinkBuffer;
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


TInt CMMFSwCodecRecordDataPath::SetObserver(MMMFHwDeviceObserver& aObserver)
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


TInt CMMFSwCodecRecordDataPath::AddCodec(CMMFSwCodec& aCodec)
	{
	if (iCodec)
		{
		return KErrNotSupported; //doesn't support multiple codecs
		}

	TInt err = KErrNone;
	iCodec = &aCodec;

	iSinkBufferSize = iCodec->SinkBufferSize();
	iSoundDevBufferSize = iCodec->SourceBufferSize();

	if ((!iSinkBufferSize)||(!iSoundDevBufferSize))
		{
		err = KErrArgument; //codec plugin has not specified buffer size
		}

	if (err == KErrNone)
		{
#ifdef __USE_MMF_TRANSFERBUFFERS__
		TRAP(err,iSoundDeviceBuffer = CreateTransferBufferL(iSoundDevBufferSize, static_cast<CMMFTransferBuffer*>(iSoundDeviceBuffer)));
#endif

#ifdef __USE_MMF_PTRBUFFERS__
		TRAP(err,iSoundDeviceBuffer = CreatePtrBufferL(iSoundDevBufferSize));
#else
		TRAP(err,iSoundDeviceBuffer = CMMFDataBuffer::NewL(iSoundDevBufferSize));
#endif
		}

	if (err == KErrNone)
		{
		// Allocate data buffer
		if (iCodec->IsNullCodec())
			{//don't need a separate sink buffer if null codec
			iSinkBuffer = NULL; //sink buffer is the sound device buffer	
			}
		else
			{//need a separate sink buffer for the codec
			TRAP(err,iSinkBuffer = CMMFDataBuffer::NewL(iSinkBufferSize));
			}	
		}
	return err;
	}


TInt CMMFSwCodecRecordDataPath::Start()
	{
	TInt startError = KErrNone;
	if (!iCodec || !(iSoundDevice->Handle()))
		{//check that a codec has been added and the sound device has been opened
		startError = KErrNotReady;
		}

	if (!startError)
		{
		// Start the player objects
		iAudioRecorder->Start();
		iSoundDeviceErrorReceiver->Start();
		iSoundDeviceBuffer->SetLastBuffer(EFalse);
		TRAP(startError, FillSoundDeviceBufferL()); //get audio recorder to fill buffer
		if (startError == KErrNone)
			{
			iRecordedBytesCount = 0; //used for debug purposes
			iState = EPlaying;
			}
   		}
	return startError;
	}


// *** Main Play Loop ***
void CMMFSwCodecRecordDataPath::FillSoundDeviceBufferL()
	{
#ifdef __CYCLE_MMF_DATABUFFERS__
	// Create a new buffer to replicate INC021405 Play-EOF-Play on HwAccelerated solution Panics
	// If the creation fails, we carry on regardless as the original buffer will not have been 
	// destroyed. Must do this as alloc fail tests will not run.
	if(iSoundDeviceBuffer)
		{
		iSoundDeviceBuffer = CycleAudioBuffer(iSoundDeviceBuffer);
		}
#endif // __CYCLE_MMF_DATABUFFERS__	
	iAudioRecorder->RecordData(*iSoundDeviceBuffer);
	}


void CMMFSwCodecRecordDataPath::BufferFilledL(CMMFDataBuffer& aBuffer)
	{	
	iSoundDeviceBuffer = &aBuffer;
	iSoundDeviceBuffer->SetStatus(EFull);
	 // Store this to avoid casting several times
    TUint length = iSoundDeviceBuffer->BufferSize();
	// Update bytes recorded
	iRecordedBytesCount += length;

	//If paused then sound driver will keep sending buffers till
	//its flushed - if last buffer then set last buffer flag
	if (length < iSoundDevBufferSize)
		{//assume it's the last buffer
		iSoundDeviceBuffer->SetLastBuffer(ETrue);
		}

	//buffer ok can send to sink
	FillSinkBufferL(); //convert to sink data type using codec
	User::LeaveIfError(iHwDeviceObserver->EmptyThisHwBuffer(*iSinkBuffer)); //pass onto sink
	}


/* 
 *  FillSinkBufferL
 * 
 *	Function to take the data from an already full source buffer and by using
 *	a codec if necessary fills the sink buffer
 *  @internalComponent
 */
void CMMFSwCodecRecordDataPath::FillSinkBufferL()
	{
	CMMFSwCodec::TCodecProcessResult codecProcessResult;

	if (iCodec->IsNullCodec())
		{//no codec so sound device buffer can be used directly as sink buffer
		iSinkBuffer = iSoundDeviceBuffer;
		iSinkBuffer->SetStatus(EFull);	//sink buffer is full
		}	
	else 
		{
		//pass buffer to codec for processing
		codecProcessResult = iCodec->ProcessL(*iSoundDeviceBuffer, *iSinkBuffer);
		if (iSoundDeviceBuffer->LastBuffer()) //if sound device is last buffer so is sound dev
			{
			iSinkBuffer->SetLastBuffer(ETrue);
			}
		if ((!iSinkBuffer->BufferSize())&&(codecProcessResult.iDstBytesAdded))
			{//the codec has added data but not set the buffer length
			iSinkBuffer->Data().SetLength(codecProcessResult.iDstBytesAdded);
			}
		//only supports EProcessComplete
		switch (codecProcessResult.iCodecProcessStatus)
			{
		case CMMFSwCodec::TCodecProcessResult::EProcessComplete:
		//finished procesing source data - all data in sink buffer
			{
			iSoundDeviceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
			iSinkBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
		case CMMFSwCodec::TCodecProcessResult::EDstNotFilled:
		//finished procesing source data - sink buffer not full could be EOF
			{
			iSoundDeviceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
			iSinkBuffer->SetStatus(EFull);	//sink buffer may not really be full however	
			}
		break;
		case CMMFSwCodec::TCodecProcessResult::EEndOfData:
			//no more data - send what we've got to the sink
			//note we can't always rely on this  - in many cases the codec will not know when
			//it has reached the end of data.
			{
			iSoundDeviceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
			iSinkBuffer->SetStatus(EFull);//sink buffer may not really be 'full' but its as full as it going to get
			//doesn't matter if sink buffer is not full
			}
		break;
		default:
			Panic(EMMFSwCodecWrapperBadCodec); //should never get here - bad codec
			}
		}
	}


void CMMFSwCodecRecordDataPath::BufferEmptiedL(const CMMFDataBuffer& aBuffer)
	{
	if (&aBuffer != iSinkBuffer)
		{
		Panic(EMMFSwCodecWrapperBadBuffer);
		}
	if (!aBuffer.LastBuffer())
		{
		FillSoundDeviceBufferL();
		}
	//else the last buffer has been emptied - the observer should stop the
	//hw device.
	}

//*** End of Main Play Loop ***


void CMMFSwCodecRecordDataPath::Stop()
	{
	iAudioRecorder->Stop();//stop audio reocrder

	iSoundDeviceErrorReceiver->Cancel(); //stop receiving events

	TRequestStatus status;
	iSoundDevice->CloseDevice(iDeviceUid, status);
	User::WaitForRequest(status);

#ifdef __CYCLE_MMF_DATABUFFERS__
	// Create a new buffer to replicate INC021405 Play-EOF-Play on HwAccelerated solution Panics
	// If the creation fails, we carry on regardless as the original buffer will not have been 
	// destroyed. Must do this as alloc fail tests will not run.
	if(iSoundDeviceBuffer)
		{
		iSoundDeviceBuffer = CycleAudioBuffer(iSoundDeviceBuffer);
		}
#endif // __CYCLE_MMF_DATABUFFERS__	

	iState = EStopped;
	}


void CMMFSwCodecRecordDataPath::Pause()
	{
	// flush it anyway, whether we're active or not
	// if we are active, then this should result in a call to RunL() pretty soon
	//note that the Pause() in the context of record means buffers are
	//continued to be obtained from the sound driver that have already 
	//been recorded - it just doesn't record any new audio data
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CMMFSwcodecRecordDataPath::Pause"));
#endif
	iSoundDevice->FlushBuffer();
	}

CRoutingSoundRecordDevice* CMMFSwCodecRecordDataPath::Device()
	{
	return iSoundDevice;
	}


void CMMFSwCodecRecordDataPath::SoundDeviceException(TInt aError)
	{
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

TUint CMMFSwCodecRecordDataPath::RecordedBytesCount()
	{
	return iRecordedBytesCount;
	}


/************************************************************************
 *				CDataPathRecorder										*
 ************************************************************************/

CDataPathRecorder::CDataPathRecorder(CMMFSwCodecRecordDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}


CDataPathRecorder::~CDataPathRecorder()
	{
	Cancel();
	}


void CDataPathRecorder::Start()
	{
	// No implementation
	}


void CDataPathRecorder::RecordData(CMMFDataBuffer& aData)
	{
	iDataFromSource = &aData;
	if (!IsActive())
		{
		iParent.Device()->RecordData(aData.Data(), iStatus);
		SetActive();
		}
	}


void CDataPathRecorder::Stop()
	{
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CDataPathRecorder Stop"));
#endif
	Cancel();
	}


void CDataPathRecorder::RunL()
	{
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CDataPathRecorder::RunL error[%d]"), iStatus.Int());
#endif
	if (!iStatus.Int())
		{
		iParent.BufferFilledL((CMMFDataBuffer&)*iDataFromSource);
		}
	//if we don't have a sound driver handle then we have stopped
	//but the client still thinks we are recording so swallow error
	else if (iStatus.Int() != KErrBadHandle)
		{ 	
		iParent.SoundDeviceException(iStatus.Int());
		}	
	}


TInt CDataPathRecorder::RunError(TInt aError)
	{
	Error(aError);
	return KErrNone;
	}


void CDataPathRecorder::DoCancel()
	{
#ifdef _SCW_DEBUG
	RDebug::Print(_L("CDataPathRecorder Cancel"));
#endif
	if (iParent.Device()->Handle())
		{
		iParent.Device()->CancelRecordData();
		iParent.Device()->FlushBuffer();
		}
	}


void CDataPathRecorder::Error(TInt aError)
	{ 
	iParent.SoundDeviceException(aError);
	}


/************************************************************************
 *				CSoundDevRecordErrorReceiver							*
 ************************************************************************/

CSoundDevRecordErrorReceiver::CSoundDevRecordErrorReceiver(CMMFSwCodecRecordDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}


CSoundDevRecordErrorReceiver::~CSoundDevRecordErrorReceiver()
	{
	Cancel();
	}


void CSoundDevRecordErrorReceiver::Start()
	{	
	if (!IsActive()) 
		{
		iParent.Device()->NotifyError(iStatus);
		SetActive();
		}
	}


void CSoundDevRecordErrorReceiver::Stop()
	{
	Cancel();
	}


void CSoundDevRecordErrorReceiver::RunL()
	{
	// An error has been returned--Flush to release mic.
	iParent.Device()->FlushBuffer();
	iParent.SoundDeviceException(iStatus.Int());
	}


void CSoundDevRecordErrorReceiver::DoCancel()
	{
	iParent.Device()->CancelNotifyError();
	}


