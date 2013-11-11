// MiscServer.cpp
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

#include "CloggerServer.h"
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>

#ifdef FSHELL_EZLIB_SUPPORT
#include <EZCompressor.h>
//#include <EZDecompressor.h>
#include <EZlib.h>
#include <EZGzip.h>
#endif

_LIT8(KDiskFull, "\r\n" MINTAGSTART "Log truncated due to disk full\r\n");

void CLogWriter::WriteBuf(TInt aBuf)
	{
	ASSERT(!IsActive() && iEnabled);
		{
		iBuf = aBuf;
		iFile.Write(iServer.GetBuf(aBuf), iStatus);
		SetActive();
		}
	}

TBool CLogWriter::IsBusyWriting()
	{
	return IsActive();
	}

void CLogWriter::RunL()
	{
	if (iStatus == KErrDiskFull)
		{
		//TODO Make this occur before the disk is actually completely empty?
		TInt filePos = 0;
		TInt err = iFile.Seek(ESeekCurrent, filePos);
		if (err == KErrNone && filePos > KDiskFull().Length()) // ensure the file has enough room in it for our message
			{
			// Rewind the log file and overwrite the tail of it with a warning to say the disk is full
			filePos = filePos - KDiskFull().Length();
			err = iFile.Seek(ESeekStart, filePos);
			if (err == KErrNone)
				{
				iFile.Write(KDiskFull, iStatus);
				SetActive();
				return;
				}
			}
		}
	iServer.CompletedWritingBuf(this, iBuf);
	}

void CLogWriter::DoCancel()
	{
	// There's no way to cancel a file write (or any semantic sense to do so) so by doing nothing we get the SyncWaitForCompletion behaviour we wanted
	}

CLogWriter::~CLogWriter()
	{
	// Don't cancel, CloggerServer is supposed to guarantee we won't be deleted when still writing
	//Cancel();
	}

void CLogWriter::CloseWriter()
	{
	delete this;
	}

void CLogWriter::SetEnabled(TBool aEnabled)
	{
	if (!iFile.SubSessionHandle())
		{
		// If the log file is not open, we can't enable ourselves even if the client asks us to
		aEnabled = EFalse;
		}
	iEnabled = aEnabled;
	}

CRDebugWriter::CRDebugWriter(CCloggerServer& aServer)
	: iServer(aServer)
	{}

void CRDebugWriter::WriteBufL(const TDesC8& aBuf)
	{
	TInt maxLen = 256;

	// RDebug::Print only accepts 256 chars on the kernel side, so split the write up
	TInt pos = 0;
	while (pos < aBuf.Length())
		{
		TPtrC8 frag = aBuf.Mid(pos, Min(maxLen, aBuf.Length() - pos));
		//Exec::DebugPrint((TAny*)&frag, 1); // This is quicker than calling RDebugPrint - cuts down on a memory copy
		// ^ Except that you can't do it from non-euser code...
		LtkUtils::RawPrint(frag);
		pos += frag.Length();
		}
	}

void CRDebugWriter::CloseWriter()
	{
	delete this;
	}

///////////////

TInt CSyncWriterWrapper::ThreadFunction(TAny* aSelf)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (!cleanup) return KErrNoMemory;
	
	CSyncWriterWrapper* self = static_cast<CSyncWriterWrapper*>(aSelf);
	for (;;)
		{
		User::WaitForRequest(self->iThreadStatus);
		//User::After(1); //DEBUG TO TEST SLOW WRITERS
		if (self->iThreadStatus != KErrNone)
			{
			break;
			}

		const TDesC8& buf = self->iServer.GetBuf(self->iBuf);

		TRAPD(err, self->iWriter.WriteBufL(buf));

		self->iThreadStatus = KRequestPending;
		TRequestStatus* stat = &self->iStatus;
		self->iMainThread.RequestComplete(stat, err);
		}

	delete cleanup;
	return self->iThreadStatus.Int();
	}

void CSyncWriterWrapper::WriteBuf(TInt aBuf)
	{
	ASSERT(!IsActive() && iEnabled);
		{
		iBuf = aBuf;
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* stat = &iThreadStatus;
		iWorkerThread.RequestComplete(stat, KErrNone);
		}
	}

TBool CSyncWriterWrapper::IsBusyWriting()
	{
	return IsActive();
	}

void CSyncWriterWrapper::RunL()
	{
	iServer.CompletedWritingBuf(this, iBuf);
	}

void CSyncWriterWrapper::DoCancel()
	{
	// There's no way to cancel a sync writer
	}

CSyncWriterWrapper::~CSyncWriterWrapper()
	{
	// Don't cancel, CloggerServer is supposed to guarantee we won't be deleted when still writing
	//Cancel();
	if (iWorkerThread.Handle())
		{
		TRequestStatus* stat = &iThreadStatus;
		iWorkerThread.RequestComplete(stat, KErrCancel);
		TRequestStatus rv;
		iWorkerThread.Rendezvous(rv);
		User::WaitForRequest(rv);
		iWorkerThread.Close();
		}
	iMainThread.Close();
	if (iOwnWriter)
		{
		iWriter.CloseWriter();
		}
	}

void CSyncWriterWrapper::CloseWriter()
	{
	delete this;
	}

void CSyncWriterWrapper::SetEnabled(TBool aEnabled)
	{
	iEnabled = aEnabled;
	}

CSyncWriterWrapper::CSyncWriterWrapper(CCloggerServer& aServer, MSyncWriter& aWriter)
	: CActive(EPriorityHigh), // It's more important to keep writing than to handle clientserver calls
	iServer(aServer), iWriter(aWriter), iBuf(-1), iEnabled(EFalse)
	{
	CActiveScheduler::Add(this);
	}

CSyncWriterWrapper* CSyncWriterWrapper::NewL(CCloggerServer& aServer, MSyncWriter& aWriter, TInt aWriterId)
	{
	CSyncWriterWrapper* self = new(ELeave) CSyncWriterWrapper(aServer, aWriter);
	CleanupStack::PushL(self);
	self->ConstructL(aWriterId);
	CleanupStack::Pop(self);
	self->iOwnWriter = ETrue;
	return self;
	}

void CSyncWriterWrapper::ConstructL(TInt aWriterId)
	{
	User::LeaveIfError(iMainThread.Open(RThread().Id()));
	_LIT(KName, "SyncWriter%i");
	TBuf<16> name;
	name.Format(KName, aWriterId);
	User::LeaveIfError(iWorkerThread.Create(name, &ThreadFunction, 2048, NULL, this));
	if (aWriterId >= 0)
		{
		iWorkerThread.SetPriority(EPriorityMore); // To ensure buffers aren't being filled without us having a chance to empty them
		}
	else
		{
		iWorkerThread.SetPriority(EPriorityMuchLess); // We use the sync writer wrapper around the background compress of rotated log files, and that operation should be low priority
		}
	iWorkerThread.Resume();
	}

//////////

CLogCompressor* CLogCompressor::NewLC(CCloggerServer& aServer)
	{
	CLogCompressor* self = new(ELeave) CLogCompressor(aServer);
	CleanupStack::PushL(self);
	User::LeaveIfError(self->iFs.Connect());
	self->iFs.ShareAuto();
	return self;
	}

CLogCompressor::CLogCompressor(CCloggerServer& aServer)
	: iServer(aServer)
	{
	}

CLogCompressor::~CLogCompressor()
	{
	}

void CLogCompressor::WriteBufL(const TDesC8& /*aBuf*/)
	{
	TDes& filename = iServer.GetFilenameToRotate();

	HBufC* compressedFile = HBufC::NewLC(filename.Length()+3);
	_LIT(KFmt, "%S.gz");
	compressedFile->Des().Format(KFmt, &filename);

	RFile input;

	//_LIT(KInfo,"Compressing file %S to %S\n");
	//console->Printf(KInfo,&inputFile,compressedFile);

	User::LeaveIfError(input.Open(iFs, filename, EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(input);

	TRAPD(err, DoGzipL(input, *compressedFile));

	if (err)
		{
		// If the compress failed, delete any fragment (don't care if this fails)
		iFs.Delete(*compressedFile);
		}

	//_LIT(KHoorah,"Hoorah");
	//console->Printf(KHoorah);

	CleanupStack::PopAndDestroy(&input);
	if (err == KErrNone)
		{
		// Don't delete the original unless the compress succeeded!
		err = iFs.Delete(filename);
		}
	if (err == KErrNone)
		{
		// We've sucessfully compressed our file, so update filename to reflect that
		filename = *compressedFile;
		}
	// Don't do anything if the delete fails - the file(s) will be cleaned up when the max number of rotated logs is exceeded
	CleanupStack::PopAndDestroy(compressedFile);
	}

void CLogCompressor::CloseWriter()
	{
	delete this;
	}

void CLogCompressor::DoGzipL(RFile& aInput, const TDesC& aOutput)
	{
#ifdef FSHELL_EZLIB_SUPPORT
	const TInt bufferSize = 0x8000; // 32 KB
	CEZFileToGZip* com = CEZFileToGZip::NewLC(iFs, aOutput, aInput, bufferSize);
	while (com->DeflateL()){/*do nothing*/}
	CleanupStack::PopAndDestroy(com);
#else
	User::Leave(KErrNotSupported);
#endif
	}

////

CMessageQueueWriter* CMessageQueueWriter::NewL()
	{
	CMessageQueueWriter* self = new(ELeave) CMessageQueueWriter;
	TInt err = self->iQ.CreateGlobal(_L("Clogger.LogMessageQueue"), 4);

	if (err)
		{
		DISOWN(self);
		User::Leave(err);
		}
	return self;
	}

void CMessageQueueWriter::WriteBufL(const TDesC8& aBuf)
	{
	TBuf8<128> buf;
	TPtrC8 lineFrag(aBuf);
	while (lineFrag.Length())
		{
		TInt spaceFree = buf.MaxLength() - buf.Length();
		buf.Append(lineFrag.Left(spaceFree));
		TInt err = iQ.Send(buf);
		if (err == KErrOverflow)
			{
			// Drop a frame
			TBuf8<128> buf2;
			iQ.Receive(buf2);
			// And resend
			err = iQ.Send(buf);
			}

		lineFrag.Set(lineFrag.Mid(Min(spaceFree, lineFrag.Length())));
		buf.Zero();
		}
	}

void CMessageQueueWriter::CloseWriter()
	{
	delete this;
	}

CMessageQueueWriter::CMessageQueueWriter()
	{
	}

CMessageQueueWriter::~CMessageQueueWriter()
	{
	iQ.Close();
	}

CDebugRouterClient* CDebugRouterClient::NewL(CCloggerServer& aServer)
	{
	CDebugRouterClient* self = new(ELeave) CDebugRouterClient(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDebugRouterClient::ConstructL()
	{
	TInt err = RCloggerDebugRouter::LoadDriver();
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	User::LeaveIfError(iDebugRouter.Open());
	}

void CDebugRouterClient::OpenChunkL()
	{
	if (!iSharedChunk.Handle())
		{
		User::LeaveIfError(iDebugRouter.OpenChunk(iSharedChunk));
		iTempBuf.CreateL(2048);
		}
	}

CDebugRouterClient::CDebugRouterClient(CCloggerServer& aServer)
: CActive(CActive::EPriorityStandard + 1), iServer(aServer) // Priority just higher than the server object - it's slightly more important we keep the device driver buffer serviced than we handle normal logging requests
	{
	CActiveScheduler::Add(this);
	}

CDebugRouterClient::~CDebugRouterClient()
	{
	Cancel();
	if (iDebugRouter.Handle())
		{
		iDebugRouter.EnableDebugRouting(RCloggerDebugRouter::EDisable); // I don't really know whether to call this before or after the cancel
		iDebugRouter.Close();
		}
	RCloggerDebugRouter::CloseDriver();
	iSharedChunk.Close();
	iTempBuf.Close();
	}

TPtrC8 Read(TDes8& aTempBuf, TPtrC8& aData, TInt aLength, TPtrC8& aOverflowData)
	{
	if (aLength <= aData.Length())
		{
		// Can read it from this buffer
		TPtrC8 res(aData.Left(aLength));
		aData.Set(aData.Mid(aLength));
		return res;
		}
	else /*if (aLength > aData.Length())*/
		{
		// Descriptor spans wrap point, so need to copy into temp buf
		aTempBuf.Copy(aData.Left(aTempBuf.MaxLength())); // If anyone's crazy enough to write a platsec diagnostic string longer than 2k, it gets truncated
		TInt overflowLen = aLength - aData.Length();
		aData.Set(aOverflowData); // Wrap aData
		aOverflowData.Set(TPtrC8());
		if (overflowLen > aData.Length())
			{
			ASSERT(EFalse); // Shouldn't happen
			// in urel, return everything we've got
			return aData;
			}
		aTempBuf.Append(aData.Left(overflowLen));
		aData.Set(aData.Mid(overflowLen));
		return TPtrC8(aTempBuf);
		}
	}

void CDebugRouterClient::RunL()
	{
	TUint chunkSize = iSharedChunk.Size();
	const TUint KDataStartOffset = sizeof(SDebugChunkHeader);
	SDebugChunkHeader* chunkHeader = (SDebugChunkHeader*)iSharedChunk.Base();
	TUint start = chunkHeader->iStartOffset;
	TUint end = chunkHeader->iEndOffset;
	TUint overflows = chunkHeader->iOverflows;

	TBool wrap = (start > end);
	TUint endLen = wrap ? chunkSize - start : end - start;
	TUint startLen = wrap ? end - KDataStartOffset : 0;

	TPtrC8 endData(iSharedChunk.Base() + start, endLen);
	TPtrC8 startData;
	if (wrap) startData.Set(iSharedChunk.Base() + KDataStartOffset, startLen);
	TPtrC8 data(endData);

	while (data.Length())
		{
		TPtrC8 header = Read(iTempBuf, data, sizeof(SCloggerTraceInfo), startData);
		if (header.Length() < (TInt)sizeof(SCloggerTraceInfo))
			{
			ASSERT(EFalse); // for udeb
			break; // Something's broken
			}
		SCloggerTraceInfo info;
		Mem::Copy(&info, header.Ptr(), sizeof(SCloggerTraceInfo));
		ASSERT(info.iTraceType == 'K' || info.iTraceType == 'U' || info.iTraceType == 'P');
		TPtrC8 msg = Read(iTempBuf, data, info.iLength, startData);
		iServer.LogKernMessage(info.iTraceType, info.iTickCount, info.iThreadId, msg);
		}
	if (overflows)
		{
		_LIT8(KErr, "RDebug::Print buffer overflowed, %u calls not logged");
		iServer.LogError(KErr, overflows);
		}
	// Zero the memory so it's easier to read in the crashlog
	memclr(iSharedChunk.Base() + start, endLen);
	if (startLen) memclr(iSharedChunk.Base() + KDataStartOffset, startLen);
	
	StartRouting(-1); // Magic number to indicate no need to call EnableDebugRouting again
	}

void CDebugRouterClient::DoCancel()
	{
	iDebugRouter.CancelReceive();
	//iDebugRouter.EnableDebugRouting(EFalse);
	// We only call Cancel() outside of our destructor when doing a FlushBuffers, in that case we'd like to make use of the device drivers buffering if possible, and not tell it to stop completely
	}

void CDebugRouterClient::StartRouting(TBool aConsumeLogs)
	{
	if (aConsumeLogs != -1)
		{
		RCloggerDebugRouter::TEnableOption enable = aConsumeLogs ? RCloggerDebugRouter::EEnableRoutingAndConsume : RCloggerDebugRouter::EEnableRouting;
		iDebugRouter.EnableDebugRouting(enable);
		}
	if (!IsActive())
		{
		iDebugRouter.ReceiveData(iStatus);
		SetActive();
		}
	}

void CDebugRouterClient::StopRouting()
	{
	Cancel();
	iDebugRouter.EnableDebugRouting(RCloggerDebugRouter::EDisable);
	}

TInt CDebugRouterClient::RegisterCrashDumpAreas(const TDesC8& aCrashDumpAreas)
	{
	return iDebugRouter.RegisterCrashDumpAreas(aCrashDumpAreas);
	}

TInt CDebugRouterClient::CreateKernChunkForClient(RThread* aClient, TInt aMaxSize, TInt aCommittedSize, RChunk& aOurChunk)
	{
	SCreateChunkParams params;
	params.iHandleOfOtherThread = aClient ? aClient->Handle() : 0;
	params.iMaxSize = aMaxSize;
	params.iCommittedSize = aCommittedSize;
	params.iChunkHandle = 0; // Not strictly necessary to set this
	params.iOtherThreadChunkHandle = 0; // Not strictly necessary to set this
	
	TInt err = iDebugRouter.CreateChunk(params);
	if (err == KErrNone)
		{
		aOurChunk.SetHandle(params.iChunkHandle);
		return aClient ? params.iOtherThreadChunkHandle : KErrNone;
		}
	return err;
	// Return either an error, or the handle for the other thread, or KErrNone if there was no error and we didn't specify another thread
	}

TInt CDebugRouterClient::AdjustChunk(RChunk& aChunk, TInt aNewSize)
	{
	return iDebugRouter.AdjustChunk(aChunk, aNewSize);
	}
