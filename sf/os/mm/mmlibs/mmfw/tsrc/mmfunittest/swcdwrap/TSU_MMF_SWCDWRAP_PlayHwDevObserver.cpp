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

// Test system includes
#include "TSU_MMF_SWCDWRAP_PlayHwDevObserver.h"

CTestPlayMMFHwDeviceObserver::CTestPlayMMFHwDeviceObserver(CMMFHwDevice* aHwDevice) : CTestMMFHwDeviceObserver(aHwDevice)
	{
	iStopActiveSchedulerOnStop = ETrue;
	iCallPauseAndResumeAfterSendingLastBuffer = EFalse;
	}

CTestPlayMMFHwDeviceObserver::~CTestPlayMMFHwDeviceObserver()
	{
	iSourceFile.Close();
	delete iDataBuffer;
	delete iFileReader;
	}


TInt CTestPlayMMFHwDeviceObserver::FillBufferFromFileSync(const TDesC& aFileName, TUint aBufferSize)
	{
	TInt error = KErrNone;
	iStopped = EFalse;

	if (!iFileServerSession.Handle())
		{//open a handle to the file server
		error = iFileServerSession.Connect();
		}

	if (!error)
		{//have a file server session so open file
		error = iSourceFile.Open(iFileServerSession, aFileName, EFileRead);
		}
	
	if (!error)
		{//create buffer to store source data
		if (iDataBuffer) 
			{
			delete iDataBuffer;
			iDataBuffer = NULL;
			}
		TRAP(error, iDataBuffer = HBufC8::NewL(aBufferSize));
		}

	if (!error)
		{//read file into the source buffer
		TPtr8 dataBufferDes(const_cast<TUint8*>(iDataBuffer->Ptr()), aBufferSize);
		iSourceFile.Read(dataBufferDes);
		iAsyncMode = EFalse;
		}

	return error; 
	}


TInt CTestPlayMMFHwDeviceObserver::ReadBufferDirectFromFileAsync(const TDesC& aFileName, TUint aNumberOfBuffers)
	{
	TInt error = KErrNone;
	iStopped = EFalse;

	if (!iFileServerSession.Handle())
		{//open a handle to the file server
		error = iFileServerSession.Connect();
		}

	if (!error)
		{//have a file server session so open file
		error = iSourceFile.Open(iFileServerSession, aFileName, EFileRead);
		}

	if (!error)
		{
		if (!iFileReader)
			TRAP(error,iFileReader = new(ELeave) CTestFileReader(iSourceFile, this));
		}

	if (!error)
		{
		iNumberOfBuffers = aNumberOfBuffers;
		iAsyncMode = ETrue;
		}
	
	return error; 
	}


TInt CTestPlayMMFHwDeviceObserver::FillThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	//called on playing when buffer needs to be filled with data to play
	TInt error = KErrNone;
	iCurrentBufferNumber++; //first buffer is buffer #1
	iCurrentHwDeviceDataBuffer = &(static_cast<CMMFDataBuffer&>(aHwDataBuffer));

	if (iAsyncMode)
		error = FillThisHwBufferFromFile(*iCurrentHwDeviceDataBuffer);
	else 
		error = FillThisHwBufferFromDes(*iCurrentHwDeviceDataBuffer);
	if (!iError) 
		iError = error; //log error in observer error
	return error;
	}


TInt CTestPlayMMFHwDeviceObserver::FillThisHwBufferFromDes(CMMFDataBuffer& aHwDataBuffer)
	{
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(&aHwDataBuffer);

	TUint bufferSize = 0;
	if (dataBuffer->RequestSize())
		bufferSize = dataBuffer->RequestSize();
	else bufferSize = dataBuffer->Data().MaxLength();

	TUint sourceDataLength = static_cast<TUint>(iDataBuffer->Des().MaxLength());

	if (iBufferPosition < sourceDataLength)
		{
		if ((sourceDataLength - iBufferPosition) <= bufferSize)
			{//reached end of data
			bufferSize = (sourceDataLength - iBufferPosition); 
			dataBuffer->SetLastBuffer(ETrue);
			}
		dataBuffer->Data().Copy((iDataBuffer->Ptr()+iBufferPosition), bufferSize);
		iBufferPosition+=bufferSize;
		}
	else
		{//have exceeded length of data
		dataBuffer->SetLastBuffer(ETrue);
		}

	return iHwDevice->ThisHwBufferFilled(*dataBuffer);
	}


TInt CTestPlayMMFHwDeviceObserver::FillThisHwBufferFromFile(CMMFDataBuffer& aHwDataBuffer)
	{
	return iFileReader->Read(aHwDataBuffer);
	}


void CTestPlayMMFHwDeviceObserver::FileReadComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		if (iCurrentBufferNumber >= iNumberOfBuffers)
			{
			iCurrentHwDeviceDataBuffer->SetLastBuffer(ETrue);
			}
		if (iCurrentHwDeviceDataBuffer->BufferSize()
			< static_cast<TUint>(iCurrentHwDeviceDataBuffer->Data().MaxLength()))
			{//EOF
			iCurrentHwDeviceDataBuffer->SetLastBuffer(ETrue);
			}
		TInt error = iHwDevice->ThisHwBufferFilled(*iCurrentHwDeviceDataBuffer);
		if (iCallPauseAndResumeAfterSendingLastBuffer&&iCurrentHwDeviceDataBuffer->LastBuffer())
			{
			TInt err = iHwDevice->Pause();
			iCallPauseAndResumeAfterSendingLastBuffer = EFalse;//only call pause once
			if (!err)//resume
				err = iHwDevice->Start(EDevDecode, EDevOutFlow);		
			if (!error)//note could potentialy have 2 errors here err & error
				error = err; //if so swallow the second error err and keep first
			}
		if (!iError) 
			iError = error; //log error in observer error
		}
	else
		{
		iError = aError;
		}
	}


void CTestPlayMMFHwDeviceObserver::Stopped()
	{
	if (iStopActiveSchedulerOnStop)
		{
		StopActiveScheduler();
		}
	iStopped = ETrue;
	}


RFile& CTestPlayMMFHwDeviceObserver::ReadFile()
	{
	return iSourceFile;
	}

void CTestPlayMMFHwDeviceObserver::CancelFileRead()
	{
	iFileReader->Cancel();
	}

TBool CTestPlayMMFHwDeviceObserver::IsStopped()
	{
	return iStopped;
	}

void CTestPlayMMFHwDeviceObserver::StopActiveSchedulerOnStop(TBool aStopActiveSchedulerOnStop)
	{
	iStopActiveSchedulerOnStop = aStopActiveSchedulerOnStop;
	}

void CTestPlayMMFHwDeviceObserver::CallPauseAndResumeAfterSendingLastBuffer()
	{
	iCallPauseAndResumeAfterSendingLastBuffer = ETrue;
	}

