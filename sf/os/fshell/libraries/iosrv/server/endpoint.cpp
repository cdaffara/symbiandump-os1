// endpoint.cpp
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
#include "endpoint.h"
#include "log.h"

#ifdef IOSRV_LOGGING
#define ENDPOINT_NAME TName endPointName(Name())
#define READER_NAME(x) TName readerName((x).IorName())
#define WRITER_NAME(x) TName writerName((x).IowName())
#else
#define ENDPOINT_NAME
#define READER_NAME(x)
#define WRITER_NAME(x)
#endif

CIoEndPoint::~CIoEndPoint()
	{
	ENDPOINT_NAME;
	LOG(CIoLog::Printf(_L("End point \"%S\" (0x%08x) destroying"), &endPointName, this));
	iAttachedReaders.Close();
	}
	
TName CIoEndPoint::IoepName() const
	{
	return Name();
	}
	
TBool CIoEndPoint::IoepIsType(RIoHandle::TType aType) const
	{
	return IsType(aType);
	}

void CIoEndPoint::IorepAttachL(MIoReader& aReader, RIoEndPoint::TReadMode aMode)
	{
	ENDPOINT_NAME;
	READER_NAME(aReader);
	LOG(CIoLog::Printf(_L("Attaching reader \"%S\" (0x%08x) to end point \"%S\" (0x%08x)"), &readerName, &aReader, &endPointName, this));
	User::LeaveIfError(Open());
	CleanupClosePushL(*this);
	if (aMode == RIoEndPoint::EForeground)
		{
		User::LeaveIfError(iAttachedReaders.Insert(&aReader, 0));
		}
	else
		{
		User::LeaveIfError(iAttachedReaders.Append(&aReader));
		}
	TInt err = HandleReaderAttached(aReader);
	if (err!=KErrNone)
		{
		TInt index = FindReader(aReader);
		iAttachedReaders.Remove(index);
		User::Leave(err);
		}
	if (aMode == RIoEndPoint::EForeground)
		{
		HandleForegroundReaderChanged();
		}
	CleanupStack::Pop(this);
	}

void CIoEndPoint::IorepDetach(MIoReader& aReader)
	{
	ENDPOINT_NAME;
	READER_NAME(aReader);
	TInt index = FindReader(aReader);
	if (index >= 0)
		{
		LOG(CIoLog::Printf(_L("Detaching reader \"%S\" (0x%08x) from end point \"%S\" (0x%08x)"), &readerName, &aReader, &endPointName, this));
		iAttachedReaders.Remove(index);
		HandleReaderDetached(aReader);
		if ((index == 0) && (iAttachedReaders.Count() > 0))
			{
			HandleForegroundReaderChanged();
			}
		Close();
		}
	else
		{
		LOG(CIoLog::Printf(_L("Reader \"%S\" (0x%08x) not attached to end point \"%S\" (0x%08x) - detach request ignored"), &readerName, &aReader, &endPointName, this));
		}
	}

void CIoEndPoint::IorepSetForegroundReaderL(MIoReader& aReader)
	{
	TInt index = FindReader(aReader);
	User::LeaveIfError(index);
	if (index > 0)
		{
		iAttachedReaders.Remove(index);
		iAttachedReaders.Insert(&aReader, 0); // Shouldn't fail due to the above removal.
		HandleForegroundReaderChanged();
		}
	}

TBool CIoEndPoint::IorepIsForegroundL(const MIoReader& aReader) const
	{
	TInt index = FindReader(aReader);
	User::LeaveIfError(index);
	return (index == 0);
	}
	
void CIoEndPoint::IowepAttachL(MIoWriter& aWriter)
	{
	User::LeaveIfError(Open());
	++iAttachedWriterCount;
	ENDPOINT_NAME;
	WRITER_NAME(aWriter);
	LOG(CIoLog::Printf(_L("Attached writer \"%S\" (0x%08x) to end point \"%S\" (0x%08x) (%d writers now attached)"), &writerName, &aWriter, &endPointName, this, iAttachedWriterCount));
	TInt err = HandleWriterAttached(aWriter);
	if (err!=KErrNone)
		{
		--iAttachedWriterCount;
		User::Leave(err);
		}
	}

void CIoEndPoint::IowepDetach(MIoWriter& aWriter)
	{
	--iAttachedWriterCount;
	ENDPOINT_NAME;
	WRITER_NAME(aWriter);
	LOG(CIoLog::Printf(_L("Detached writer \"%S\" (0x%08x) from end point \"%S\" (0x%08x) (%d writers now attached)"), &writerName, &aWriter, &endPointName, this, iAttachedWriterCount));
	HandleWriterDetached(aWriter);
	Close();
	}

TInt CIoEndPoint::HandleReaderAttached(MIoReader&)
	{
	return KErrNone;
	}

void CIoEndPoint::HandleReaderDetached(MIoReader&)
	{
	}
	
void CIoEndPoint::ForegroundReaderChanged()
	{
	}

TInt CIoEndPoint::HandleWriterAttached(MIoWriter&)
	{
	return KErrNone;
	}

void CIoEndPoint::HandleWriterDetached(MIoWriter&)
	{
	}

void CIoEndPoint::HandleForegroundReaderChanged()
	{
	MIoReader* fg = AttachedReader();
	if (fg)
		{
		fg->IorReaderChange(RIoReadHandle::EGainedForeground);
		}
	ForegroundReaderChanged();
	}

CIoEndPoint::CIoEndPoint()
	{
	}

TInt CIoEndPoint::NumAttachedReaders() const
	{
	return iAttachedReaders.Count();
	}

MIoReader* CIoEndPoint::AttachedReader(TInt aIndex)
	{
	if ((aIndex >= 0) && (aIndex < iAttachedReaders.Count()))
		{
		return iAttachedReaders[aIndex];
		}
	return NULL;
	}

TInt CIoEndPoint::NumAttachedWriters() const
	{
	return iAttachedWriterCount;
	}

TInt CIoEndPoint::FindReader(MIoReader& aReader) const
	{
	const TInt numReaders = iAttachedReaders.Count();
	for (TInt i = 0; i < numReaders; ++i)
		{
		if (iAttachedReaders[i] == &aReader)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CIoEndPoint::FindReader(const MIoReader& aReader) const
	{
	return FindReader(const_cast<MIoReader&>(aReader));
	}
