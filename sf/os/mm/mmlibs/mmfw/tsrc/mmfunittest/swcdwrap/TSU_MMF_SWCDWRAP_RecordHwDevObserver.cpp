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
#include "TSU_MMF_SWCDWRAP_RecordHwDevObserver.h"

CTestRecordMMFHwDeviceObserver::CTestRecordMMFHwDeviceObserver(CMMFHwDevice* aHwDevice) : CTestMMFHwDeviceObserver(aHwDevice)
	{
	}

CTestRecordMMFHwDeviceObserver::~CTestRecordMMFHwDeviceObserver()
	{
	iSinkFile.Close();
	delete iFileWriter;
	}


TInt CTestRecordMMFHwDeviceObserver::WriteBufferDirectToFileAsync(const TDesC& aFileName, TUint aNumberOfBuffers)
	{
	TInt error = KErrNone;

	if (!iFileServerSession.Handle())
		{//open a handle to the file server
		error = iFileServerSession.Connect();
		}

	if (!error)
		{
		//make sure file to be recorded is deleted before we start
		error = iFileServerSession.Delete(aFileName); 
		if (error == KErrNotFound)
			error = KErrNone; //KErrNotFound is not an error, means file doesn't exist

		if (!error)
			{
			error = iSinkFile.Create(iFileServerSession, aFileName, EFileWrite);
			if (!error)
				{
				if (!iFileWriter)
					TRAP(error,iFileWriter = new(ELeave) CTestFileWriter(iSinkFile, this));	
				}
			if (!error)
				{
				iNumberOfBuffers = aNumberOfBuffers;
				}
			}
		}	
	return error; 
	}


void CTestRecordMMFHwDeviceObserver::FileWriteComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		TInt error = KErrNone;
		if (iCurrentBufferNumber >= iNumberOfBuffers)
			{
			error = iHwDevice->Stop();
			StopActiveScheduler();
			}
        else if (iCurrentHwDeviceDataBuffer->LastBuffer() || iCurrentHwDeviceDataBuffer->BufferSize()==0)
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


TInt CTestRecordMMFHwDeviceObserver::EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer)
	{
	//called on recording when buffer needs to be written to file
	TInt error = KErrNone;
	iCurrentBufferNumber++;
	iCurrentHwDeviceDataBuffer = &(static_cast<CMMFDataBuffer&>(aHwDataBuffer));

	if (iCurrentBufferNumber <= iNumberOfBuffers)
		{
		error = iFileWriter->Write(*iCurrentHwDeviceDataBuffer);
		}
	if (!iError) 
			iError = error; //log error in observer error
	return error;
	}


void CTestRecordMMFHwDeviceObserver::Stopped()
	{
	}


RFile& CTestRecordMMFHwDeviceObserver::WriteFile()
	{
	return iSinkFile;
	}



