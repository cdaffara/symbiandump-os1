// file.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "file.h"
#include "log.h"
#include <fshell/ltkutils.h>

CIoFile* CIoFile::NewLC(RFs& aFs, const TDesC& aName, RIoFile::TMode aMode)
	{
	CIoFile* self = new(ELeave) CIoFile();
	CleanupClosePushL(*self);
	self->ConstructL(aFs, aName, aMode);
	return self;
	}

CIoFile::~CIoFile()
	{
	iFile.Close();
	iTempReadBuf.Close();
	}

TBool CIoFile::IsType(RIoHandle::TType aType) const
	{
	return ((aType == RIoHandle::EEndPoint) || (aType == RIoHandle::EFile));
	}

void CIoFile::IorepReadL(MIoReader& aReader)
	{
	TInt fileSize;
	User::LeaveIfError(iFile.Size(fileSize));
	if (iPos < fileSize)
		{
		TDes& readBuf = aReader.IorReadBuf();
		iTempReadBuf.Zero();
		HBufC8* narrowBuf = HBufC8::NewLC(readBuf.MaxLength());
		TPtr8 narrowBufPtr(narrowBuf->Des());
		User::LeaveIfError(iFile.Read(narrowBufPtr, readBuf.MaxLength()));
		iPos += narrowBuf->Length();
		iTempReadBuf.AppendUtf8L(narrowBufPtr);
		if (iPos >= fileSize)
			{
			iTempReadBuf.FinalizeUtf8();
			}
		readBuf.Copy(iTempReadBuf);
		aReader.IorDataBuffered(readBuf.Length());
		CleanupStack::PopAndDestroy(narrowBuf);
		if (iPos >= fileSize)
			{
			aReader.IorReadComplete(KErrNone);
			}
		}
	else
		{
		aReader.IorReadComplete(KErrEof);
		}
	}

void CIoFile::IowepWriteL(MIoWriter& aWriter)
	{
	HBufC* buf = HBufC::NewLC(aWriter.IowWriteLength());
	TPtr bufPtr(buf->Des());
	aWriter.IowWrite(bufPtr);

	// Convert to UTF-8
	HBufC8* narrowBuf = LtkUtils::Utf8L(*buf);
	TInt err = iFile.Write(*narrowBuf);
	delete narrowBuf;
	aWriter.IowComplete(err);
	CleanupStack::PopAndDestroy(buf);
	}

void CIoFile::IowepWriteCancel(MIoWriter&)
	{
	}

CIoFile::CIoFile()
	{
	}

void CIoFile::ConstructL(RFs& aFs, const TDesC& aName, RIoFile::TMode aMode)
	{
	LOG(CIoLog::Printf(_L("Attempting to create file 0x%08x (\"%S\")"), this, &aName));
	switch (aMode)
		{
		case RIoFile::ERead:
			{
			User::LeaveIfError(iFile.Open(aFs, aName, EFileRead));
			break;
			}
		case RIoFile::EOverwrite:
			{
			User::LeaveIfError(iFile.Replace(aFs, aName, EFileWrite));
			break;
			}
		case RIoFile::EAppend:
			{
			TInt err = iFile.Open(aFs, aName, EFileWrite);
			if (err == KErrNotFound)
				{
				err = iFile.Create(aFs, aName, EFileWrite);
				}
			User::LeaveIfError(err);
			TInt pos = 0;
			User::LeaveIfError(iFile.Seek(ESeekEnd, pos));
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

