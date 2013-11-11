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
#include "TSU_MMF_SWCDWRAP_HwDevObserver.h"
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>

void Panic()
	{
	_LIT(KMMF_SWCDWRAP_UnitTestPanic, "TSU_MMF_SWCDWRAP");
	User::Panic(KMMF_SWCDWRAP_UnitTestPanic, 0);
	}

CTestMMFHwDeviceObserver::CTestMMFHwDeviceObserver(CMMFHwDevice* aHwDevice) : iError(KErrNone)
	{
	iHwDevice = aHwDevice;
	}

CTestMMFHwDeviceObserver::~CTestMMFHwDeviceObserver()
	{
	iFileServerSession.Close();
	}


TInt CTestMMFHwDeviceObserver::FillThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/)
	{
#ifndef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER //Adapter loads the drivers
	Panic(); //should never get called
#endif
	return KErrNone;
	}


TInt CTestMMFHwDeviceObserver::EmptyThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/)
	{
	Panic(); //should never get called
	return KErrNone;
	}


TInt CTestMMFHwDeviceObserver::MsgFromHwDevice(TUid aMessageType, const TDesC8& /*aMsg*/)
	{
	if (aMessageType.iUid != KMmfHwDeviceObserverUpdateBytesPlayed)
		{
		Panic(); //should never get called
		}		
	return KErrNone;
	}


void CTestMMFHwDeviceObserver::Stopped()
	{
	Panic(); //should never get called
	}


void CTestMMFHwDeviceObserver::Error(TInt aError)
	{
	if (!iError) 
		iError = aError; //log error in observer error
	}


void CTestMMFHwDeviceObserver::StopActiveScheduler()
	{
	CActiveScheduler* activeScheduler;
	activeScheduler = CActiveScheduler::Current();
	activeScheduler->Stop();
	}


void CTestMMFHwDeviceObserver::FileReadComplete(TInt /*aError*/)
	{
	Panic(); //should never get called
	}


void CTestMMFHwDeviceObserver::FileWriteComplete(TInt /*aError*/)
	{
	Panic(); //should never get called
	}


TInt CTestMMFHwDeviceObserver::ErrorStatus()
	{
	return iError;
	}


TUint CTestMMFHwDeviceObserver::NumberOfBuffersObserved()
	{
	return iCurrentBufferNumber;
	}


TInt CTestMMFHwDeviceObserver::CTestFileReader::Read(CMMFDataBuffer& aBuffer)
	{
	iDataBuffer = &aBuffer;
	iFile.Read(iPosition, iDataBuffer->Data(), iStatus);
	if (!IsActive())
		{
		SetActive();
		}
	return KErrNone;
	}

void CTestMMFHwDeviceObserver::CTestFileReader::RunL()
	{
	iPosition+=iDataBuffer->BufferSize();
	iParent->FileReadComplete(iStatus.Int());
	}

void CTestMMFHwDeviceObserver::CTestFileReader::DoCancel()
	{
	}


TInt CTestMMFHwDeviceObserver::CTestFileWriter::Write(CMMFDataBuffer& aBuffer)
	{
	iDataBuffer = &aBuffer;
	iFile.Write(iPosition, iDataBuffer->Data(), iStatus);
	SetActive();
	return KErrNone;
	}


void CTestMMFHwDeviceObserver::CTestFileWriter::RunL()
	{
	iPosition+=iDataBuffer->BufferSize();
	iParent->FileWriteComplete(iStatus.Int());
	}

void CTestMMFHwDeviceObserver::CTestFileWriter::DoCancel()
	{
	}



