// pipe.cpp
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

#include "server.h"
#include "pipe.h"
#include "log.h"

#ifdef IOSRV_LOGGING
#define PIPE_NAME TName pipeName(Name())
#define READER_NAME(x) TName readerName((x).IorName())
#define WRITER_NAME(x) TName writerName((x).IowName())
#else
#define PIPE_NAME
#define READER_NAME(x)
#define WRITER_NAME(x)
#endif

CIoPipe* CIoPipe::NewLC()
	{
	CIoPipe* self = new(ELeave) CIoPipe();
	LOG(CIoLog::Printf(_L("Pipe 0x%08x created"), self));
	CleanupClosePushL(*self);
	return self;
	}

CIoPipe::~CIoPipe()
	{
	iPendingWriters.Close();
	}

TBool CIoPipe::IsType(RIoHandle::TType aType) const
	{
	return ((aType == RIoHandle::EEndPoint) || (aType == RIoHandle::EPipe));
	}

void CIoPipe::IorepReadL(MIoReader& aReader)
	{
	if (&aReader == AttachedReader())
		{
		CheckReady();
		DoCopy();
		}
	}

void CIoPipe::IowepWriteL(MIoWriter& aWriter)
	{
	User::LeaveIfError(iPendingWriters.Append(&aWriter));
	CheckReady();
	DoCopy();
	}

void CIoPipe::IowepWriteCancel(MIoWriter& aWriter)
	{
	const TInt numPendingWriters = iPendingWriters.Count();
	for (TInt i = 0; i < numPendingWriters; ++i)
		{
		if (&aWriter == iPendingWriters[i])
			{
			iPendingWriters.Remove(i);
			break;
			}
		}
	}

void CIoPipe::ForegroundReaderChanged()
	{
	DoCopy();
	}

void CIoPipe::HandleReaderDetached(MIoReader&)
	{
	CheckReady();
	}

void CIoPipe::HandleWriterDetached(MIoWriter& aWriter)
	{
	const TInt numPendingWriters = iPendingWriters.Count();
	for (TInt i = 0; i < numPendingWriters; ++i)
		{
		if (&aWriter == iPendingWriters[i])
			{
			iPendingWriters.Remove(i);
			break;
			}
		}
	CheckReady();
	}

CIoPipe::CIoPipe()
	{
	}

void CIoPipe::DoCopy()
	{
	if ((AttachedReader() == NULL) || !AttachedReader()->IorReadPending())
		{
		return;
		}

	READER_NAME(*AttachedReader());
	TInt readBufOffset = 0;
	TDes& readBuf = AttachedReader()->IorReadBuf();
	TBool readBuffered(EFalse);

	while (iPendingWriters.Count() > 0)
		{
		MIoWriter& writer = *(iPendingWriters[0]);
		WRITER_NAME(writer);
		const TInt lengthToWrite = writer.IowWriteLength();
		const TInt length = Min(lengthToWrite, readBuf.MaxLength() - readBuf.Length());
		TPtr readPtr(const_cast<TText*>(readBuf.Ptr()) + readBufOffset, 0, length);
		TInt err = writer.IowWrite(readPtr);
		if (err == KErrNone)
			{
			LOG(CIoLog::Printf(_L("Copied %d chars from writer \"%S\" (0x%08x) to reader \"%S\" (0x%08x)"), readPtr.Length(), &writerName, &writer, &readerName, AttachedReader()));
			readBufOffset += readPtr.Length();
			readBuf.SetLength(readBufOffset);
			readBuffered = ETrue;
			if (lengthToWrite == length)
				{
				LOG(CIoLog::Printf(_L("Writer \"%S\" (0x%08x) fully copied"), &writerName, &writer));
				writer.IowComplete(KErrNone);
				iPendingWriters.Remove(0);
				}
			else if (readBuf.MaxLength() == readBuf.Length())
				{
				LOG(CIoLog::Printf(_L("Read buffer full (\"%S\" 0x%08x)"), &readerName, AttachedReader()));
				break;
				}
			}
		else
			{
			LOG(CIoLog::Printf(_L("Failed to copy \"%S\" from writer \"%S\" (0x%08x) to reader \"%S\" (0x%08x): %S(%d)"), &readPtr, &writerName, &writer, &readerName, AttachedReader(), CIoLog::StringifyError(err), err));
			writer.IowComplete(err);
			}
		}

	if (readBuffered)
		{
		AttachedReader()->IorDataBuffered(readBuf.Length());
		}
	}

void CIoPipe::CheckReady()
	{
	if ((NumAttachedWriters() == 0) && AttachedReader() && AttachedReader()->IorReadPending())
		{
		if (AttachedReader()->IorDataIsBuffered())
			{
			AttachedReader()->IorReadComplete(KErrNone);
			}
		else
			{
			AttachedReader()->IorReadComplete(KErrEof);
			}
		}
	else if (AttachedReader() == NULL)
		{
		while (iPendingWriters.Count())
			{
			MIoWriter* writer = iPendingWriters[0];
			iPendingWriters.Remove(0);
			writer->IowComplete(KErrNotReady);
			}
		}
	}

