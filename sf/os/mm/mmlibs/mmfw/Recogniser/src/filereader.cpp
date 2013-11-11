// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "readers.h"

//
//
//
CFileReader::CFileReader(RFile* aFile)
 :	CBufferReader(iFileBuffer, CReader::EFile),
 	iFile(aFile)
	{
	}


//
//
//
CFileReader::~CFileReader()
	{
	}


//
//
//
CFileReader* CFileReader::NewLC(RFile* aFile)
	{
	CFileReader* self = new(ELeave) CFileReader(aFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


//
//
//
void CFileReader::ConstructL()
	{
	TInt pos = 0;
	User::LeaveIfError(iFile->Seek(ESeekStart, pos));
	User::LeaveIfError(iFile->Read(iFileBuffer));
	}


//
// Checks if there is aAmount of data left in the buffer.
// It is important to call the base-class implementation first
// to ensure correct operation.
//
TBool CFileReader::CheckEnoughData(TInt aAmount)
	{
	if (CBufferReader::CheckEnoughData(aAmount))
		{
		return ETrue;
		}
		
	// Try to read more data.
	TInt bufPos = CBufferReader::Position();
	TInt err = PhysicallySeekAndRead(bufPos - iFileBuffer.Length());
	if (err == KErrNone)
		{
		// The read may have succeeded but that 
		// still doesn't mean we have enough data.
		return (aAmount <= iFileBuffer.Length());
		}
	
	return EFalse;
	}


//
//
//
void CFileReader::Reset()
	{
	CBufferReader::Reset(); // This will reset iBufPos.
	
	if (iFilePos != 0)
		{
		// We need to seek to the start and fill the buffer.
		iFilePos = 0;
		TInt filepos = 0;
		TInt err = iFile->Seek(ESeekStart, filepos);
		if (err == KErrNone)
			{
			err = iFile->Read(iFileBuffer);
			}
			
		if (err != KErrNone)
			{
			iFileBuffer.Zero();
			}
		}
	else
		{
		// There's no need to seek and read.
		iFilePos = 0;
		}
	}


//
//
//
void CFileReader::SeekL(TInt aOffset)
	{
	TInt err = CReader::Seek(aOffset);
	if (err == KErrUnderflow)
		{
		TInt bufPos = CBufferReader::Position();
		aOffset += bufPos - iFileBuffer.Length();
		User::LeaveIfError(PhysicallySeekAndRead(aOffset));
		}
	}


//
// It could be possible for a 64-bit field in formats such as MPEG4
// to have values that would fit in a 32-bit variable. In this case
// we can use it for seeking. This function checks if a 64-bit value
// is compatible with RFile's 32-bit operations.
//
void CFileReader::SeekL(TInt64 aOffset)
	{
	if (aOffset < KMinTInt64)
		{
		User::Leave(KErrNotSupported);
		}
		
	if (aOffset > KMaxTInt64)
		{
		User::Leave(KErrNotSupported);
		}
	
	if (aOffset < (TInt64)KMaxTInt)
	    {
        TInt low = (TInt)I64LOW(aOffset);
        SeekL(low);
	    }
	else
	    {
        TInt err = CReader::Seek(aOffset);
        if (err == KErrUnderflow)
            {
            TInt64 bufPos = CBufferReader::Position();
            aOffset += bufPos - iFileBuffer.Length();
            User::LeaveIfError(PhysicallySeekAndRead(aOffset));
            }
	    }
	}
	
//
// This function seeks forward/backward aOffset bytes
// and fills the buffer from that point.
//
TInt CFileReader::PhysicallySeekAndRead(TInt aOffset)
	{
	TInt err;
	
	// New buffer contents so read from the start of it.
	CBufferReader::Reset();
	
	iFilePos = aOffset;
	err = iFile->Seek(ESeekCurrent, aOffset);
	
	if (err != KErrNone)
		{
		return err;
		}

	err = iFile->Read(iFileBuffer);
	if (err != KErrNone)
		{
		return err;
		}
	return (iFileBuffer.Length() == 0 ? KErrEof : KErrNone);
	}
TInt CFileReader::PhysicallySeekAndRead(TInt64 aOffset)
    {
    TInt err;
    // New buffer contents so read from the start of it.
    CBufferReader::Reset();
    
    iFilePos = aOffset;
    RFile64* tempfile;
    tempfile = static_cast<RFile64*> (iFile);
 
    err = tempfile->Seek(ESeekCurrent, iFilePos);
	if (err != KErrNone)
		{
		return err;
		}

	err = iFile->Read(iFileBuffer);
	if (err != KErrNone)
		{
		return err;
		}
		
	return (iFileBuffer.Length() == 0 ? KErrEof : KErrNone);
	}

