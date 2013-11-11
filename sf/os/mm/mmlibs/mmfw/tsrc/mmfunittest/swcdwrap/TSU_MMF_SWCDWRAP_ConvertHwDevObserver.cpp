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
#include "TSU_MMF_SWCDWRAP_ConvertHwDevObserver.h"

CTestConvertMMFHwDeviceObserver::CTestConvertMMFHwDeviceObserver(CMMFHwDevice* aHwDevice) : CTestMMFHwDeviceObserver(aHwDevice)
	{
	}

CTestConvertMMFHwDeviceObserver::~CTestConvertMMFHwDeviceObserver()
	{
	iSourceFile.Close();
	iSinkFile.Close();
	delete iFileReader;
	delete iFileWriter;
	}


TInt CTestConvertMMFHwDeviceObserver::SetConvertFiles(const TDesC& aSourceFileName, const TDesC& aSinkFileName, TUint aNumberOfBuffers)
	{
	TInt error = KErrNone;

	if (!iFileServerSession.Handle())
		{//open a handle to the file server
		error = iFileServerSession.Connect();
		}

	if (!error)
		{//have a file server session so source file open file
		error = iSourceFile.Open(iFileServerSession, aSourceFileName, EFileRead);
		}
	
	if (!error)
		{
		//make sure destination file is deleted before we start
		error = iFileServerSession.Delete(aSinkFileName); 
		if (error == KErrNotFound)
			error = KErrNone; //KErrNotFound is not an error, means file doesn't exist
		if (!error)
			{
			error = iSinkFile.Create(iFileServerSession, aSinkFileName, EFileWrite);
			}
		}

	if (!error)
		{
		if (!iFileReader)
			TRAP(error,iFileReader = new(ELeave) CTestFileReader(iSourceFile, this));
		}

	if (!error)
		{
		if (!iFileWriter)
			TRAP(error,iFileWriter = new(ELeave) CTestFileWriter(iSinkFile, this));	
		}

	if (!error)
		{
		iNumberOfBuffers = aNumberOfBuffers;
		}
	
	return error; 
	}


TInt CTestConvertMMFHwDeviceObserver::FillThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	//called on playing when buffer needs to be filled with data to play
	TInt error = KErrNone;
	iCurrentBufferNumber++; //first buffer is buffer #1
	iCurrentHwDeviceDataBuffer = &(static_cast<CMMFDataBuffer&>(aHwDataBuffer));

	error = FillThisHwBufferFromFile(*iCurrentHwDeviceDataBuffer);
	
	if (!iError) 
		iError = error; //log error in observer error
	return error;
	}


TInt CTestConvertMMFHwDeviceObserver::FillThisHwBufferFromFile(CMMFDataBuffer& aHwDataBuffer)
	{
	return iFileReader->Read(aHwDataBuffer);
	}


void CTestConvertMMFHwDeviceObserver::FileReadComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		if (iCurrentBufferNumber >= iNumberOfBuffers)
			iCurrentHwDeviceDataBuffer->SetLastBuffer(ETrue);
		TInt error = iHwDevice->ThisHwBufferFilled(*iCurrentHwDeviceDataBuffer);
		if (!iError) 
			iError = error; //log error in observer error
		}
	else
		{
		iError = aError;
		}
	}

void CTestConvertMMFHwDeviceObserver::FileWriteComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		TInt error = KErrNone;
		if (iCurrentBufferNumber >= iNumberOfBuffers)
			{
			error = iHwDevice->Stop();
			StopActiveScheduler();
			}
		else
			{//tell hw device we've emptied it and get another buffer
			error = iHwDevice->ThisHwBufferEmptied(*iCurrentHwDeviceDataBuffer);
			}
		if (!iError) 
			iError = error; //log error in observer error
		}
	else
		{
		iError = aError;
		}
	}


TInt CTestConvertMMFHwDeviceObserver::EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	//called on recording when buffer needs to be written to file
	TInt error = KErrNone;
	iCurrentHwDeviceDataBuffer = &(static_cast<CMMFDataBuffer&>(aHwDataBuffer));

	if (iCurrentBufferNumber <= iNumberOfBuffers)
		{
		error = iFileWriter->Write(*iCurrentHwDeviceDataBuffer);
		}

	if (!iError) 
			iError = error; //log error in observer error
	return error;
	}


void CTestConvertMMFHwDeviceObserver::Stopped()
	{
	}


RFile& CTestConvertMMFHwDeviceObserver::ReadFile()
	{
	return iSourceFile;
	}

RFile& CTestConvertMMFHwDeviceObserver::WriteFile()
	{
	return iSinkFile;
	}

