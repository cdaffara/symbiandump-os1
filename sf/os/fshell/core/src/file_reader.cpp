// file_reader.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "file_reader.h"


CFileReader* CFileReader::NewL(TInt aBlockSize, RFs& aFs, TBool aDirect)
	{
	CFileReader* self = new(ELeave) CFileReader(aFs, aDirect, aBlockSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFileReader::~CFileReader()
	{
	Cancel();
	iFile.Close();
	delete iBuf;
	}

void CFileReader::Read(const TDesC& aFileName, MFileReaderObserver& aObserver)
	{
	Read(aFileName, 0, aObserver);
	}

void CFileReader::Read(const TDesC& aFileName, TInt aOffset, MFileReaderObserver& aObserver)
	{
	ASSERT(!IsActive());
	iObserver = &aObserver;
	iFirstRead = ETrue;
	if (iDirect)
		{
		iDirectOffset = aOffset;
		iFileName = aFileName;
		ReadNextDirectChunk();
		}
	else
		{
		iFile.Close();
		TInt err = iFile.Open(iFs, aFileName, EFileRead);
		if (err)
			{
			iObserver->HandleFileReadError(err);
			}
		else
			{
			err = iFile.Seek(ESeekStart, aOffset);
			if (err)
				{
				iObserver->HandleFileReadError(err);
				}
			ReadNextChunk();
			}
		}
	}

CFileReader::CFileReader(RFs& aFs, TBool aDirect, TInt aBlockSize)
	: CActive(CActive::EPriorityStandard), iFs(aFs), iBlockSize(aBlockSize), iPtr(NULL, 0), iDirect(aDirect)
	{
	CActiveScheduler::Add(this);
	}

void CFileReader::ConstructL()
	{
	iBuf = HBufC8::NewL(iBlockSize);
	iPtr.Set(iBuf->Des());
	}

void CFileReader::ReadNextChunk()
	{
	ASSERT(!IsActive());
	iPtr.Zero();
	iFile.Read(iPtr, iBlockSize, iStatus);
	SetActive();
	}

void CFileReader::ReadNextDirectChunk()
	{
	ASSERT(!IsActive());
	iPtr.Zero();
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CFileReader::DoCancel()
	{
	// Can't cancel reads, to do nothing.
	}

void CFileReader::RunL()
	{
	TInt err;
	if (iDirect)
		{
		err = iFs.ReadFileSection(iFileName, iDirectOffset, iPtr, iBlockSize);
		if (err == KErrNone)
			{
			iDirectOffset += iPtr.Length();
			}
		}
	else
		{
		err = iStatus.Int();
		}

	if (err)
		{
		iObserver->HandleFileReadError(err);
		return;
		}

	TBool keepGoing(EFalse);
	if (iFirstRead)
		{
		iFirstRead = EFalse;
		iObserver->HandleFileData(iPtr, MFileReaderObserver::EFirst, keepGoing);
		}
	else if (iPtr.Length() < iBlockSize)
		{
		iFile.Close();
		TBool dummy;
		iObserver->HandleFileData(iPtr, MFileReaderObserver::ELast, dummy);
		}
	else
		{
		iObserver->HandleFileData(iPtr, MFileReaderObserver::EMiddle, keepGoing);
		}

	if (keepGoing)
		{
		iDirect ? ReadNextDirectChunk() : ReadNextChunk();
		}
	}

