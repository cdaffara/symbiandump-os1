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
// source\server\mmfswcodecconvertdatapath.cpp
// 
//

#include "mmfSwCodecConvertDataPath.h"
#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/common/mmfpaniccodes.h>


CMMFSwCodecConvertDataPath* CMMFSwCodecConvertDataPath::NewL()
	{
	CMMFSwCodecConvertDataPath* self = new(ELeave) CMMFSwCodecConvertDataPath;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CMMFSwCodecConvertDataPath::ConstructL()
	{
	iDataPathConverter = new (ELeave) CDataPathConverter(*this,CActive::EPriorityUserInput);
	}


CMMFSwCodecConvertDataPath::~CMMFSwCodecConvertDataPath()
	{	
	delete iDataPathConverter;
	if (iCodec)
		{
		delete iSourceBuffer;
		if (!iCodec->IsNullCodec()) 
			delete iSinkBuffer;
		}
	}


TInt CMMFSwCodecConvertDataPath::SetObserver(MMMFHwDeviceObserver& aObserver)
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


TInt CMMFSwCodecConvertDataPath::AddCodec(CMMFSwCodec& aCodec)
	{
	if (iCodec)
		return KErrNotSupported; //doesn't support multiple codecs

	TInt err = KErrNone;
	iCodec = &aCodec;

	iSourceBufferSize = iCodec->SourceBufferSize();
	iSinkBufferSize = iCodec->SinkBufferSize();

	if ((!iSourceBufferSize)||(!iSinkBufferSize))
		err = KErrArgument; //codec plugin has not specified buffer size

	if (err == KErrNone)
		{
		TRAP(err,iSourceBuffer = CMMFDataBuffer::NewL(iSourceBufferSize));
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


TInt CMMFSwCodecConvertDataPath::Start()
	{
	TInt startError = KErrNone;
	if (!iCodec) 
		{//check that a codec has been added
		startError = KErrNotReady;
		}
	if (!startError)
		{
		// Start the player objects
		iSourceBuffer->SetLastBuffer(EFalse);
		iDataPathConverter->Start();
		iState = EPlaying;
		iNoMoreSourceData = EFalse;
   		}
	return startError;
	}



void CMMFSwCodecConvertDataPath::Stop()
	{
	iDataPathConverter->Cancel();
	iState = EStopped;
	}


void CMMFSwCodecConvertDataPath::Pause()
	{//pause is equivalent to stop for a data transfer
	iDataPathConverter->Cancel();
	iState = EStopped;
	}

/*** Main play loop ***/

void CMMFSwCodecConvertDataPath::FillSourceBufferL()
	{
	STATIC_CAST(CMMFDataBuffer*, iSourceBuffer)->SetRequestSizeL(iSourceBufferSize);
            
    // Ask immediately for data from the observer
	User::LeaveIfError(iHwDeviceObserver->FillThisHwBuffer(*iSourceBuffer));
	}

 /** 
 *  BufferFilledL.  
 *	(from MDataSink)
 * 
 *	called by the CMMFDataPath's MDataSource when it has filled the buffer
 *  @internalComponent
 *	@param aBuffer
 *	
 */
void CMMFSwCodecConvertDataPath::BufferFilledL(CMMFDataBuffer& aBuffer)
	{	
	iSourceBuffer = &aBuffer;
	iSourceBuffer->SetStatus(EFull);

	//need to check that the buffer size is not 0 - 
	//if so assume we've reached the end of the data
	if (!iSourceBuffer->BufferSize())
		{//no buffer  - could be end of source or could be that the source has no data
		//also need to check the sink buffer is available else there is still some
		//stuff to do before the sink buffer is freed
		if (iSinkBuffer->Status()==EAvailable)
			{
			iNoMoreSourceData = ETrue;
			}
		}
	else
		{
		if (iSourceBuffer->LastBuffer()) //also check last buffer flag
			iNoMoreSourceData = ETrue;
		iDataPathConverter->ChangeConvertState(CDataPathConverter::EFillingSinkBuffer);
		}
	}

/* 
 *  FillSinkBufferL
 * 
 *	Function to take the data from an already full source buffer and by using
 *	a codec if necessary fills the sink buffer
 *  @internalComponent
 */
void CMMFSwCodecConvertDataPath::FillSinkBufferL()
	{
	CMMFSwCodec::TCodecProcessResult codecProcessResult;

	if (iCodec->IsNullCodec())
		{//no codec so data can be sent direct to sink
		iSinkBuffer = iSourceBuffer;
		iSinkBuffer->SetStatus(EFull);	//sink buffer is full
		}	
	else 
		{	
		//pass buffer to codec for processing
		codecProcessResult = iCodec->ProcessL(*iSourceBuffer, *iSinkBuffer);
		if (iSourceBuffer->LastBuffer()) //if source is last buffer so is sink
			iSinkBuffer->SetLastBuffer(ETrue);
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
			iSourceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
			iSinkBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
		case CMMFSwCodec::TCodecProcessResult::EDstNotFilled:
		//could be the last buffer in which case dst might not get filled
			{
			iSourceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
			iSinkBuffer->SetStatus(EFull);	//sink buffer is full	
			}
		break;
		case CMMFSwCodec::TCodecProcessResult::EEndOfData:
			//no more data - send what we've got to the sink
			//note we can't always rely on this  - in many cases the codec will not know when
			//it has reached the end of data.
			{
			iSourceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
			iSinkBuffer->SetStatus(EFull);//sink buffer may not really be 'full' but its as full as it going to get
			//doesn't matter if sink buffer is not full
			}
		break;
		default:
			Panic(EMMFSwCodecWrapperBadCodec); //should never get here - bad codec
			}
		}
	iDataPathConverter->ChangeConvertState(CDataPathConverter::EEmptyingSinkBuffer);
	}


void CMMFSwCodecConvertDataPath::EmptySinkBufferL()
	{
	User::LeaveIfError(iHwDeviceObserver->EmptyThisHwBuffer(*iSinkBuffer));
	}


void CMMFSwCodecConvertDataPath::BufferEmptiedL(const CMMFDataBuffer& aBuffer)
	{
	if (&aBuffer != iSinkBuffer)
		User::Leave(KErrArgument);
	if (!iNoMoreSourceData) 
		iDataPathConverter->ChangeConvertState(CDataPathConverter::EFillingSourceBuffer);
	else //no more source data so signal EOF
		SoundDeviceException(KErrEof);
	}

void CMMFSwCodecConvertDataPath::SoundDeviceException(TInt aError)
	{
	// Inform the observer of the exception condition
	iHwDeviceObserver->Error(aError);

	Stop();

	// Let the observer know we're fully stopped
    iHwDeviceObserver->Stopped();	
	}

RMdaDevSound& CMMFSwCodecConvertDataPath::Device()
	{
	return iDummyDevSound;//convert doesn't have a RMdaDevSound
	}

/*** End of main play loop ***/




/************************************************************************
 *				CDataPathConverter	
 * This class performs the main data transfer between the source and the sink
 * This is done in a separate class as opposed to CMMFSwCodecConvertDataPath
 * because the class needs to be an active object to avoid recursive call stacks
 * in cases where the source and sink are not active objects - which is
 * the case with descriptors.  Making CMMFSwCodecConvertDataPath derive
 * from CActive is less desirable as it would involve multiple inheretence
 ************************************************************************/

CMMFSwCodecConvertDataPath::CDataPathConverter::CDataPathConverter(CMMFSwCodecConvertDataPath& aParent, TInt aPriority)
: CActive(aPriority), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	iConvertState = EIdle;
	}


CMMFSwCodecConvertDataPath::CDataPathConverter::~CDataPathConverter()
	{
	Cancel();
	}

/** 
 *  Start
 * 
 *	Starts active scheduler 'play' loop
 *  @internalComponent
 */
void CMMFSwCodecConvertDataPath::CDataPathConverter::Start()
	{
	// If we're not already active, complete a request on ourselves to kick off the state machine
	if (iConvertState == EIdle)
		iConvertState = EFillingSourceBuffer;
	if (!IsActive())
		{
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, KErrNone);
		SetActive();
		}
	}


void CMMFSwCodecConvertDataPath::CDataPathConverter::ChangeConvertState(TConvertState aNewConvertState)
	{
	TRequestStatus* stat = &iStatus;
	//change state
	iConvertState = aNewConvertState;
	if (!IsActive())
		{
		User::RequestComplete(stat, KErrNone);
		SetActive();
		}
	}

/*** Main Convert Loop ***/

void CMMFSwCodecConvertDataPath::CDataPathConverter::FillSourceBufferL()
	{
	iParent.FillSourceBufferL();
	}

void CMMFSwCodecConvertDataPath::CDataPathConverter::FillSinkBufferL()
	{
	iParent.FillSinkBufferL();
	}

void CMMFSwCodecConvertDataPath::CDataPathConverter::EmptySinkBufferL()
	{
	iParent.EmptySinkBufferL();
	}

/*** End of main convert loop ***/


void CMMFSwCodecConvertDataPath::CDataPathConverter::RunL()
	{
	switch (iConvertState)
		{
		case EFillingSourceBuffer:
			FillSourceBufferL();
			break;
		case EFillingSinkBuffer:
			FillSinkBufferL();
			break;
		case EEmptyingSinkBuffer:
			EmptySinkBufferL();
			break;
		case EIdle:
			break;
		}
	}

void CMMFSwCodecConvertDataPath::CDataPathConverter::DoCancel()
	{
	//don't need to do anything as we don't have any async requests to other objects
	}

TInt CMMFSwCodecConvertDataPath::CDataPathConverter::RunError(TInt aError)
	{
	iParent.SoundDeviceException(aError);
	return KErrNone;
	}




