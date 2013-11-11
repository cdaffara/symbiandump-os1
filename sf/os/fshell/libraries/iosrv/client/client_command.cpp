// client_command.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/ltkutils.h>
#include "command_base.h"

namespace IoUtils
	{

_LIT(KNewLine, "\r\n");
_LIT(KOutgoingMarker, ">");
_LIT(KIncomingMarker, "<");


//
// CServerWatcher.
//

NONSHARABLE_CLASS(CServerWatcher) : public CActive
	{
public:
	static CServerWatcher* NewL(const TDesC& aServerExeName, CClientBase& aClient);
	~CServerWatcher();
	void Kill(TInt aReason);
private:
	CServerWatcher(CClientBase& aClient);
	void ConstructL(const TDesC& aServerExeName);
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	CClientBase& iClient;
	RProcess iServerProcess;
	};

CServerWatcher* CServerWatcher::NewL(const TDesC& aServerExeName, CClientBase& aClient)
	{
	CServerWatcher* self = new(ELeave) CServerWatcher(aClient);
	CleanupStack::PushL(self);
	self->ConstructL(aServerExeName);
	CleanupStack::Pop(self);
	return self;
	}

CServerWatcher::~CServerWatcher()
	{
	Cancel();
	iServerProcess.Close();
	}

void CServerWatcher::Kill(TInt aReason)
	{
	Cancel();
	iServerProcess.Kill(aReason);
	}

CServerWatcher::CServerWatcher(CClientBase& aClient)
	: CActive(CActive::EPriorityStandard), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

void CServerWatcher::ConstructL(const TDesC& aServerExeName)
	{
	_LIT(KWildCard, "*");
	TName processName(aServerExeName);
	processName.Append(KWildCard);
	TFindProcess findProcess(processName);
	TFullName name;
	StaticLeaveIfErr(findProcess.Next(name), _L("Unable to find server process '%S'"), &aServerExeName);
	StaticLeaveIfErr(iServerProcess.Open(findProcess), _L("Unable to open server process '%S'"), &aServerExeName);
	if (findProcess.Next(name) != KErrNotFound)
		{
		StaticLeaveIfErr(KErrArgument, _L("Found more than one instance of '%S'"), &aServerExeName);
		}
	iServerProcess.Logon(iStatus);
	if (iStatus != KRequestPending)
		{
		User::WaitForRequest(iStatus);
		StaticLeaveIfErr(KErrGeneral, _L("Failed to logon to '%S' - %d"), &aServerExeName, iStatus.Int());
		}
	SetActive();
	}

void CServerWatcher::RunL()
	{
	TExitCategoryName exitCategory(iServerProcess.ExitCategory());
	iClient.HandleServerDeath(iServerProcess.ExitType(), iServerProcess.ExitReason(), exitCategory);
	}

void CServerWatcher::DoCancel()
	{
	iServerProcess.LogonCancel(iStatus);
	}


//
// CServerReader.
//

NONSHARABLE_CLASS(CServerReader) : public CActive
	{
public:
	static CServerReader* NewL(RIoReadHandle& aReadHandle, CClientBase& aClient);
	~CServerReader();
private:
	CServerReader(RIoReadHandle& aReadHandle, CClientBase& aClient);
	void ConstructL();
	void Queue();
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	RIoReadHandle& iReadHandle;
	CClientBase& iClient;
	TBuf<0x200> iLine;
	};

CServerReader* CServerReader::NewL(RIoReadHandle& aReadHandle, CClientBase& aClient)
	{
	CServerReader* self = new(ELeave) CServerReader(aReadHandle, aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CServerReader::~CServerReader()
	{
	Cancel();
	}

CServerReader::CServerReader(RIoReadHandle& aReadHandle, CClientBase& aClient)
	: CActive(CActive::EPriorityStandard), iReadHandle(aReadHandle), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

void CServerReader::Queue()
	{
	iLine.Zero();
	iReadHandle.Read(iLine, iStatus);
	SetActive();
	}

void CServerReader::ConstructL()
	{
	iReadHandle.SetReadModeL(RIoReadHandle::ELine);
	iReadHandle.SetLineSeparatorL(KNewLine());
	Queue();
	}

void CServerReader::RunL()
	{
	TInt err = iStatus.Int();
	if (err == KErrEof)
		{
		iClient.HandleServerReadComplete(KErrServerTerminated, iLine);
		}
	else
		{
		iClient.HandleServerReadComplete(err, iLine);
		}
	Queue();
	}

void CServerReader::DoCancel()
	{
	iReadHandle.ReadCancel();
	}


//
// CServerWriter.
//

NONSHARABLE_CLASS(CServerWriter) : public CActive
	{
public:
	static CServerWriter* NewL(RIoWriteHandle& aWriteHandle, CClientBase& aClient);
	~CServerWriter();
	void Write(const TDesC& aLine);
private:
	CServerWriter(RIoWriteHandle& aWriteHandle, CClientBase& aClient);
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	RIoWriteHandle& iWriteHandle;
	CClientBase& iClient;
	HBufC* iBuf;
	};

CServerWriter* CServerWriter::NewL(RIoWriteHandle& aWriteHandle, CClientBase& aClient)
	{
	return new(ELeave) CServerWriter(aWriteHandle, aClient);
	}

CServerWriter::~CServerWriter()
	{
	Cancel();
	delete iBuf;
	}

CServerWriter::CServerWriter(RIoWriteHandle& aWriteHandle, CClientBase& aClient)
	: CActive(CActive::EPriorityStandard), iWriteHandle(aWriteHandle), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

void CServerWriter::Write(const TDesC& aLine)
	{
	ASSERT(!IsActive());
	TInt bufferRequired = aLine.Length();
	TBool newLineRequired(EFalse);
	if ((aLine.Length() < KNewLine().Length()) || (aLine.Right(KNewLine().Length()) != KNewLine))
		{
		newLineRequired = ETrue;
		bufferRequired += KNewLine().Length();
		}
	if (iBuf && (iBuf->Des().MaxLength() < bufferRequired))
		{
		iBuf = iBuf->ReAllocL(bufferRequired);
		}
	else
		{
		iBuf = HBufC::NewL(bufferRequired);
		}
	TPtr ptr(iBuf->Des());
	ptr.Copy(aLine);
	if (newLineRequired)
		{
		ptr.Append(KNewLine);
		}
	iWriteHandle.Write(*iBuf, iStatus);
	SetActive();
	}

void CServerWriter::RunL()
	{
	iClient.HandleServerWriteComplete(iStatus.Int());
	}

void CServerWriter::DoCancel()
	{
	iWriteHandle.WriteCancel();
	}


EXPORT_C CClientBase::CClientBase(TUint aFlags, const TDesC& aServerExeName, const TDesC& aPersistentConsoleName, const TDesC& aServerPrompt)
	: CCommandBase(EManualComplete | aFlags), iServerExeName(aServerExeName), iPersistentConsoleName(aPersistentConsoleName), iServerPrompt(aServerPrompt)
	{
	}

EXPORT_C CClientBase::~CClientBase()
	{
	delete iCommand;
	delete iServerReader;
	delete iServerWriter;
	delete iServerWatcher;
	iServerWritePipe.Close();
	iServerWriteHandle.Close();
	iServerReadPipe.Close();
	iServerReadHandle.Close();
	iPcons.Close();
	iServerProcess.Close();
	iLines.ResetAndDestroy();
	}
	
_LIT(KWritePipeNameFmt, "%S client->server pipe");
_LIT(KWriterNameFmt, "%S server writer");
_LIT(KReadPipeNameFmt, "%S server->client pipe");
_LIT(KReaderNameFmt, "%S server reader");

EXPORT_C void CClientBase::DoRunL()
	{
	TInt err = iPcons.OpenByName(IoSession(), iPersistentConsoleName);
	if (err==KErrNotFound)
		{
		if (iVerbose)
			{
			Printf(_L("'%S' not found, creating...\r\n"), &iPersistentConsoleName);
			}
		LeaveIfErr(iPcons.Create(IoSession(), iPersistentConsoleName, iPersistentConsoleName), _L("Couldn't create persistent console '%S'"), &iPersistentConsoleName);
		TRAPL(iServerProcess.CreateL(iServerExeName, KNullDesC(), IoSession(), iPcons, &Env()), _L("Couldn't create server process"));
		}
	else
		{
		LeaveIfErr(err, _L("Cannot open persistent console '%S'"), &iPersistentConsoleName);
		}

	iServerWatcher = CServerWatcher::NewL(iServerExeName, *this);
		
	TName objName;		

	iServerWritePipe.CreateL(IoSession());
	objName.AppendFormat(KWritePipeNameFmt, &Name());
	IoSession().SetObjectNameL(iServerWritePipe.SubSessionHandle(), objName);

	iServerWriteHandle.CreateL(IoSession());
	objName.Zero();
	objName.AppendFormat(KWriterNameFmt, &Name());
	IoSession().SetObjectNameL(iServerWriteHandle.SubSessionHandle(), objName);
	iServerWriteHandle.SetModeL(RIoWriteHandle::EText);
	iServerWritePipe.AttachL(iServerWriteHandle);
	// attach the pcons reader to the other end of our server write pipe
	LeaveIfErr(iPcons.AttachReader(iServerWritePipe, RIoPersistentConsole::EDetachOnHandleClose), _L("Cannot connect reader to persistent console %S"), &iPersistentConsoleName);
	iServerWriter = CServerWriter::NewL(iServerWriteHandle, *this);

	iServerReadPipe.CreateL(IoSession());
	objName.Zero();
	objName.AppendFormat(KReadPipeNameFmt, &Name());
	IoSession().SetObjectNameL(iServerReadPipe.SubSessionHandle(), objName);
	
	iServerReadHandle.CreateL(IoSession());
	objName.Zero();
	objName.AppendFormat(KReaderNameFmt, &Name());
	IoSession().SetObjectNameL(iServerReadHandle.SubSessionHandle(), objName);
	iServerReadPipe.AttachL(iServerReadHandle, RIoEndPoint::EForeground);
	// attach the pcons writer to the other end of our server read pipe
	LeaveIfErr(iPcons.AttachWriter(iServerReadPipe, RIoPersistentConsole::EDetachOnHandleClose), _L("Cannot connect writer to persistent console %S"), &iPersistentConsoleName);
	iServerReader = CServerReader::NewL(iServerReadHandle, *this);
	
	if (iServerProcess.Process().Handle() != KNullHandle && iServerProcess.Process().Handle() != RProcess().Handle())
		{
		// We created a new server process, but it's not yet been resumed.
		iServerProcess.Detach(); // Note, iServerWatch has already logged onto the process so there's no need to use RChildProcess::Run.
		iWaitingForServerPrompt = ETrue;
		}
	else
		{
		// The server was already running - no need to wait for a prompt, go ahead and write the command.
		SendCommand();
		}
	}

EXPORT_C void CClientBase::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCommand, "command");

	if (UsingCif()) // Test this dynamically for the time being because not all sub-classes have been migrated to CIFs.
		{
		aArguments.AppendStringL(iCommand, KCommand);
		}
	else
		{
		_LIT(KCommandDescription, "The command to run.");
		aArguments.AppendStringL(iCommand, KCommand, KCommandDescription, KValueTypeFlagLast);
		}
	}

EXPORT_C void CClientBase::HandleLeave(TInt aError)
	{
	CCommandBase::HandleLeave(aError);
	}

void CClientBase::HandleServerReadComplete(TInt aError, TDes& aLine)
	{
	if (aError == KErrEof)
		{
		Complete(KErrNone);
		}
	else if (aError)
		{
		iServerWatcher->Kill(KErrAbort);
		Complete(aError, _L("Failed to read server response"));
		}
	else if (aLine == iServerPrompt)
		{
		if (iWaitingForServerPrompt)
			{
			iWaitingForServerPrompt = EFalse;
			SendCommand();
			}
		else
			{
			TRAPD(err, HandleServerResponseL(iLines));
			Complete(err);
			}
		}
	else
		{
		if (iVerbose)
			{
			Write(KIncomingMarker);
			Write(aLine);
			}
		if (!iWaitingForServerPrompt)
			{
			aLine.TrimRight();
			HBufC* buf = aLine.AllocLC();
			iLines.AppendL(buf);
			CleanupStack::Pop(buf);
			}
		}
	}

void CClientBase::HandleServerWriteComplete(TInt aError)
	{
	if (aError)
		{
		iServerWatcher->Kill(KErrAbort);
		Complete(aError, _L("Failed to write command to server"));
		}
	}

void CClientBase::HandleServerDeath(TExitType aExitType, TInt aExitReason, const TDesC& aExitCategory)
	{
	if (aExitType == EExitPanic)
		{
		Complete(KErrServerTerminated, _L("Server '%S' PANIC - %S %d"), &iServerExeName, &aExitCategory, aExitReason);
		}
	else if (aExitReason)
		{
		Complete(aExitReason, _L("Server '%S' exitied abnormally"), &iServerExeName);
		}
	else
		{
		Complete();
		}
	}

void CClientBase::SendCommand()
	{
	if (iVerbose)
		{
		Write(KOutgoingMarker);
		Write(*iCommand);
		Write(KNewLine);
		}
	iServerWriter->Write(*iCommand);
	}

	} // namespace IoUtils

