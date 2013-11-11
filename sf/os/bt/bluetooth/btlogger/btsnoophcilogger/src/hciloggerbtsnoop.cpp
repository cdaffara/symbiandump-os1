// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hciloggerbtsnoop.h"

static const TInt KSnoopLogFileHeaderSize = 16; // bytes
static const TInt KSnoopLogRecordHeaderSize = 24; // bytes
static const TUint KSnoopLogVersion = 1;

static const TInt KHCILoggerFileCloseTime = 5000000; // 5 seconds

_LIT8(KSnoopLogBlankDescriptor, "");
_LIT8(KSnoopLogFileHeaderFormat, "btsnoop\0%M%M");
_LIT8(KSnoopLogRecordHeaderFormat, "%M%M%M%M%M%M");

#ifdef __WINS__
_LIT(KSnoopLogFileH1,		"\\shared\\hcilogger\\hcilog_H1.log");
_LIT(KSnoopLogFileH4,		"\\shared\\hcilogger\\hcilog_H4.log");
_LIT(KSnoopLogFileBCSP,		"\\shared\\hcilogger\\hcilog_Bcsp.log");
_LIT(KSnoopLogFileH5,		"\\shared\\hcilogger\\hcilog_H5.log");
_LIT(KSnoopLogFileDefault,	"\\shared\\hcilogger\\hcilog_unknown.log");
#else
_LIT(KSnoopLogFileH1,		"e:\\shared\\hcilogger\\hcilog_H1.log");
_LIT(KSnoopLogFileH4,		"e:\\shared\\hcilogger\\hcilog_H4.log");
_LIT(KSnoopLogFileBCSP,		"e:\\shared\\hcilogger\\hcilog_Bcsp.log");
_LIT(KSnoopLogFileH5,		"e:\\shared\\hcilogger\\hcilog_H5.log");
_LIT(KSnoopLogFileDefault,	"e:\\shared\\hcilogger\\hcilog_unknown.log");
#endif	//__WINS__

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
#else
GLDEF_C TInt E32Dll()
#endif
	{
	return KErrNone;
	}

CHCILoggerBtSnoop* CHCILoggerBtSnoop::NewLC()
	{
	CHCILoggerBtSnoop* self = new (ELeave) CHCILoggerBtSnoop;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHCILoggerBtSnoop::CHCILoggerBtSnoop()
	{
	}

void CHCILoggerBtSnoop::ConstructL()
	{
	iFileCloseTimer = CHCILoggerFileCloseTimer::NewL(iLogFile);
	for (TInt i = 0; i <= TInt(KHCILoggerControllerToHost | KHCILoggerCommandOrEvent); i++)
		{
		CHCILoggerBufferedFrame* temp = new(ELeave) CHCILoggerBufferedFrame;
		CleanupStack::PushL(temp);
		iBuffers.AppendL(temp); // array now "owns" temp
		CleanupStack::Pop(temp);
		}
	}

void CHCILoggerBtSnoop::DoInitialise(TInt aType)
	{
	iDatalinkType = aType;
	OpenFile(); // if it fails, just ignore it - try later when writing
	}

CHCILoggerBtSnoop::~CHCILoggerBtSnoop()
	{
	iBuffers.ResetAndDestroy();
	iLogFile.Close();
	iFs.Close();
	delete iFileCloseTimer;
	}

void CHCILoggerBtSnoop::LogFrame(TUint aFrameFlags, const TDesC8& aDesc)
	{
	TInt packetFlags = aFrameFlags & (KHCILoggerControllerToHost | KHCILoggerCommandOrEvent);
	CHCILoggerBufferedFrame* buffer = iBuffers[packetFlags];
	if (aFrameFlags & KHCILoggerFrameFragmented)
		{
		buffer->BufferData(aDesc);
		return;
		}

	TBtSnoopPacketRecord record;
	record.iOriginalLength = aDesc.Length();
	record.iIncludedLength = record.iOriginalLength;
	record.iPacketFlags = aFrameFlags;
	record.iCumulativeDrops = iCumulativeDrops;
	iCurrentTime.HomeTime();
	record.iTimestampMicroseconds = iCurrentTime.Int64();

	// if the file is not open, try to open it
	if (!iLogFile.SubSessionHandle())
		{
		if (OpenFile() != KErrNone)
			{
			return;
			}
		}

	record.iOriginalLength += buffer->OriginalLength();
	TDesC8& des = buffer->Buffer();
	if (buffer->OriginalLength() > des.Length())
		{ // some data lost
		record.iIncludedLength = des.Length();
		FileWriteRecord(record, des); 
		}
	else
		{ // no data lost
		record.iIncludedLength = record.iOriginalLength;
		FileWriteRecord(record, des, aDesc);
		}
	buffer->Reset();
	}

TInt CHCILoggerBtSnoop::OpenFile()
	{
	TPtrC fileName;
	switch (iDatalinkType)
		{
	case KHCILoggerDatalinkTypeH1:
		fileName.Set(KSnoopLogFileH1);
		break;
	case KHCILoggerDatalinkTypeH4:
		fileName.Set(KSnoopLogFileH4);
		break;
	case KHCILoggerDatalinkTypeBCSP:
		fileName.Set(KSnoopLogFileBCSP);
		break;
	case KHCILoggerDatalinkTypeH5:
		fileName.Set(KSnoopLogFileH5);
		break;
	default:
		fileName.Set(KSnoopLogFileDefault);
		break;
		};

	TInt err = KErrNone;

	if (!iFs.Handle())
	    {
        err = iFs.Connect();
        if (err)
            {
            return err;
            }
	    }

	err = iLogFile.Open(iFs, fileName, EFileWrite | EFileShareAny);
	if (err == KErrNotFound)
		{ // if it doesn't already exist, create it
		//Note that the potential KErrPathNotFound error is deliberately not handled, 
		//since the shared\hcilogger directory not being present is used to indicate 
		//that logging is not desired.
		err = iLogFile.Create(iFs, fileName, EFileWrite | EFileShareAny);
		}
	TInt size = 0;
	if (err == KErrNone)
		{
		err = iLogFile.Size(size);
		}
	if (err == KErrNone)
		{
		if (size < KSnoopLogFileHeaderSize)
			{
			err = FileWriteHeader(); // write will automatically start at pos 0,
			}						 // hence overwriting the file
		else
			{ // want to append the data to the current file
			TInt writePos = KSnoopLogFileHeaderSize; // KSnoopLogFileHeaderSize is const, so can't pass in Seek()
			iLogFile.Seek(ESeekEnd, writePos);
			}
		}
	if (err)
		{
		iLogFile.Close(); // just in case
		}
	return err;
	}

TInt CHCILoggerBtSnoop::FileWriteHeader()
	{
	TBuf8<KSnoopLogFileHeaderSize> buf;
	buf.Format(KSnoopLogFileHeaderFormat, KSnoopLogVersion, iDatalinkType);
	TInt err = iLogFile.Write(buf);
	iFileCloseTimer->ResetTimer();
	return err;
	}

void CHCILoggerBtSnoop::FileWriteRecord(const TBtSnoopPacketRecord& aRecord, const TDesC8& aBufferedData, const TDesC8& aNewData)
	{
	TBuf8<KSnoopLogRecordHeaderSize> buf;
	buf.Format(KSnoopLogRecordHeaderFormat, aRecord.iOriginalLength,
											aRecord.iIncludedLength,
											aRecord.iPacketFlags,
											aRecord.iCumulativeDrops,
											I64HIGH(aRecord.iTimestampMicroseconds),
											I64LOW(aRecord.iTimestampMicroseconds));

	// if these fail, just ignore them
	iLogFile.Write(buf);	// header
	iLogFile.Write(aBufferedData);	// data
	iLogFile.Write(aNewData);	// data
	iFileCloseTimer->ResetTimer();
	}

void CHCILoggerBtSnoop::FileWriteRecord(const TBtSnoopPacketRecord& aRecord, const TDesC8& aNewData)
	{
	FileWriteRecord(aRecord, KSnoopLogBlankDescriptor, aNewData);
	}

// CHCILoggerFileCloseTimer functions //

CHCILoggerFileCloseTimer* CHCILoggerFileCloseTimer::NewL(RFile& aLogFile)
	{
	CHCILoggerFileCloseTimer* self = new(ELeave) CHCILoggerFileCloseTimer(aLogFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHCILoggerFileCloseTimer::CHCILoggerFileCloseTimer(RFile& aLogFile)
: CTimer(EPriorityStandard), iLogFile(aLogFile)
	{
	}

void CHCILoggerFileCloseTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CHCILoggerFileCloseTimer::ResetTimer()
	{
	if (IsActive())
		{
		Cancel();
		}
	After(TTimeIntervalMicroSeconds32(KHCILoggerFileCloseTime));
	}

void CHCILoggerFileCloseTimer::RunL()
	{
	iLogFile.Close();
	}

void CHCILoggerFileCloseTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

CHCILoggerFileCloseTimer::~CHCILoggerFileCloseTimer()
	{
	Cancel();
	}

CHCILoggerBufferedFrame::CHCILoggerBufferedFrame()
	{
	iBuffer = HBufC8::New(0);
	}

CHCILoggerBufferedFrame::~CHCILoggerBufferedFrame()
	{
	delete iBuffer;
	}

void CHCILoggerBufferedFrame::BufferData(const TDesC8& aDesc)
	{
	if (iOriginalLength == iBuffer->Length())
		{ // then no allocations have failed so far
		TInt newLength = iOriginalLength + aDesc.Length();
		// add aDesc to buffer
		HBufC8* buf = iBuffer;
		if(buf != NULL)
			{
			if(newLength > buf->Des().MaxLength())
				{
				buf = buf->ReAlloc(newLength);
				}
			if(buf != NULL)
				{ // successful allocation
				buf->Des().Append(aDesc);
				iBuffer = buf;
				}
			// else if it failed, iBuffer stays the same
			}
		}
	iOriginalLength += aDesc.Length(); // add this on anyway
	}

void CHCILoggerBufferedFrame::Reset()
	{
	iBuffer->Des().Zero();
	iOriginalLength = 0;
	}

TInt CHCILoggerBufferedFrame::OriginalLength() const
	{
	return iOriginalLength;
	}

TDesC8& CHCILoggerBufferedFrame::Buffer() const
	{
	return *iBuffer;
	}

