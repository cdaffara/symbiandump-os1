// log.cpp
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

#include "log.h"
#ifdef IOSRV_LOGGING

#include "server.h"
#include "clientserver.h"

#ifndef IOSRV_LOGGING_USES_CLOGGER
_LIT(KLogFileName, "c:\\logs\\iosrv.txt");
_LIT8(KNewLine, "\r\n");
#endif

#if !defined(__WINS__) || defined (EKA2)
CIoLog* gLog = NULL;
#endif

CIoLog* CIoLog::NewL(RFs& aFs)
	{
	CIoLog* self = new(ELeave) CIoLog(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CIoLog::~CIoLog()
	{
#ifdef IOSRV_LOGGING_USES_CLOGGER
	iClogger.Close();
#else
	iFile.Close();
#endif
#if defined(__WINS__) && !defined(EKA2)
	Dll::SetTls(NULL);
#endif
	}


#define CASE_RETURN_LIT(XXX) case XXX: { _LIT(_KLit, #XXX); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }
	
const TDesC* CIoLog::StringifyError(TInt aError)
	{
	switch (aError)
		{
		CASE_RETURN_LIT(KErrNone);
		CASE_RETURN_LIT(KErrNotFound);
		CASE_RETURN_LIT(KErrGeneral);
		CASE_RETURN_LIT(KErrCancel);
		CASE_RETURN_LIT(KErrNoMemory);
		CASE_RETURN_LIT(KErrNotSupported);
		CASE_RETURN_LIT(KErrArgument);
		CASE_RETURN_LIT(KErrBadHandle);
		CASE_RETURN_LIT(KErrOverflow);
		CASE_RETURN_LIT(KErrUnderflow);
		CASE_RETURN_LIT(KErrAlreadyExists);
		CASE_RETURN_LIT(KErrPathNotFound);
		CASE_RETURN_LIT(KErrDied);
		CASE_RETURN_LIT(KErrInUse);
		CASE_RETURN_LIT(KErrServerTerminated);
		CASE_RETURN_LIT(KErrServerBusy);
		CASE_RETURN_LIT(KErrCompletion);
		CASE_RETURN_LIT(KErrNotReady);
		CASE_RETURN_LIT(KErrUnknown);
		CASE_RETURN_LIT(KErrCorrupt);
		CASE_RETURN_LIT(KErrAccessDenied);
		CASE_RETURN_LIT(KErrLocked);
		CASE_RETURN_LIT(KErrWrite);
		CASE_RETURN_LIT(KErrDisMounted);
		CASE_RETURN_LIT(KErrEof);
		CASE_RETURN_LIT(KErrDiskFull);
		CASE_RETURN_LIT(KErrBadDriver);
		CASE_RETURN_LIT(KErrBadName);
		CASE_RETURN_LIT(KErrCommsLineFail);
		CASE_RETURN_LIT(KErrCommsFrame);
		CASE_RETURN_LIT(KErrCommsOverrun);
		CASE_RETURN_LIT(KErrCommsParity);
		CASE_RETURN_LIT(KErrTimedOut);
		CASE_RETURN_LIT(KErrCouldNotConnect);
		CASE_RETURN_LIT(KErrCouldNotDisconnect);
		CASE_RETURN_LIT(KErrDisconnected);
		CASE_RETURN_LIT(KErrBadLibraryEntryPoint);
		CASE_RETURN_LIT(KErrBadDescriptor);
		CASE_RETURN_LIT(KErrAbort);
		CASE_RETURN_LIT(KErrTooBig);
		CASE_RETURN_LIT(KErrDivideByZero);
		CASE_RETURN_LIT(KErrBadPower);
		CASE_RETURN_LIT(KErrDirFull);
		CASE_RETURN_LIT(KErrHardwareNotAvailable);
#ifdef EKA2
		CASE_RETURN_LIT(KErrPermissionDenied);
#endif
//		CASE_RETURN_LIT(KErrExtensionNotSupported);
//		CASE_RETURN_LIT(KErrCommsBreak);
		DEFAULT_RETURN_LIT("*** ERROR UNKNOWN ***");
		}
	}
	
const TDesC* CIoLog::StringifyOpcode(TInt aOpCode)
	{
	switch (aOpCode)
		{
		CASE_RETURN_LIT(EIoHandleClose);
		CASE_RETURN_LIT(EIoHandleSetOwner);
		CASE_RETURN_LIT(EIoHandleSetUnderlyingConsole);
		CASE_RETURN_LIT(EIoHandleAttachedToConsole);
		CASE_RETURN_LIT(EIoCreateReader);
		CASE_RETURN_LIT(EIoOpenReaderByThreadId);
		CASE_RETURN_LIT(EIoOpenReaderByExplicitThreadId);
		CASE_RETURN_LIT(EIoDuplicateReader);
		CASE_RETURN_LIT(EIoCreateWriter);
		CASE_RETURN_LIT(EIoOpenWriterByThreadId);
		CASE_RETURN_LIT(EIoOpenWriterByExplicitThreadId);
		CASE_RETURN_LIT(EIoDuplicateWriter);
		CASE_RETURN_LIT(EIoSetReadWriteMode);
		CASE_RETURN_LIT(EIoSetReadMode);
		CASE_RETURN_LIT(EIoSetReaderToForeground);
		CASE_RETURN_LIT(EIoRead);
		CASE_RETURN_LIT(EIoSetLineSeparator);
		CASE_RETURN_LIT(EIoWrite);
		CASE_RETURN_LIT(EIoReadCancel);
		CASE_RETURN_LIT(EIoWriteCancel);
		CASE_RETURN_LIT(EIoIsForegroundReader);
		CASE_RETURN_LIT(EIoConsoleWaitForKey);
		CASE_RETURN_LIT(EIoConsoleWaitForKeyCancel);
		CASE_RETURN_LIT(EIoConsoleCaptureKey);
		CASE_RETURN_LIT(EIoConsoleCancelCaptureKey);
		CASE_RETURN_LIT(EIoConsoleCaptureAllKeys);
		CASE_RETURN_LIT(EIoConsoleCancelCaptureAllKeys);
		CASE_RETURN_LIT(EIoConsoleCursorPos);
		CASE_RETURN_LIT(EIoConsoleSetCursorPosAbs);
		CASE_RETURN_LIT(EIoConsoleSetCursorPosRel);
		CASE_RETURN_LIT(EIoConsoleSetCursorHeight);
		CASE_RETURN_LIT(EIoConsoleSetTitle);
		CASE_RETURN_LIT(EIoConsoleClearScreen);
		CASE_RETURN_LIT(EIoConsoleClearToEndOfLine);
		CASE_RETURN_LIT(EIoConsoleScreenSize);
		CASE_RETURN_LIT(EIoConsoleSetAttributes);
		CASE_RETURN_LIT(EIoEndPointAttachReader);
		CASE_RETURN_LIT(EIoEndPointAttachWriter);
		CASE_RETURN_LIT(EIoEndPointSetForegroundReadHandle);
		CASE_RETURN_LIT(EIoCreatePipe);
		CASE_RETURN_LIT(EIoCreateConsole);
		CASE_RETURN_LIT(EIoOpenConsole);
		CASE_RETURN_LIT(EIoConsoleImplementation);
		CASE_RETURN_LIT(EIoCreateFile);
		CASE_RETURN_LIT(EIoCreateNull);
		CASE_RETURN_LIT(EIoSetObjectName);
		CASE_RETURN_LIT(EIoCreatePersistentConsole);
		CASE_RETURN_LIT(EIoOpenPersistentConsoleByName);
		CASE_RETURN_LIT(EIoPersistentConsoleAttachReadEndPoint);
		CASE_RETURN_LIT(EIoPersistentConsoleAttachWriteEndPoint);
		CASE_RETURN_LIT(EIoPersistentConsoleDetachReadEndPoint);
		CASE_RETURN_LIT(EIoPersistentConsoleDetachWriteEndPoint);
		CASE_RETURN_LIT(EIoPersistentConsoleNotifyReadDetach);
		CASE_RETURN_LIT(EIoPersistentConsoleNotifyWriteDetach);
		CASE_RETURN_LIT(EIoPersistentConsoleCancelNotifyReadDetach);
		CASE_RETURN_LIT(EIoPersistentConsoleCancelNotifyWriteDetach);
		CASE_RETURN_LIT(EIoHandleIsType);
		CASE_RETURN_LIT(EIoHandleGetName);
		CASE_RETURN_LIT(EIoHandleEquals);
		CASE_RETURN_LIT(EIoReadHandleNotifyChange);
		CASE_RETURN_LIT(EIoReadHandleCancelNotifyChange);
		CASE_RETURN_LIT(EIoDuplicateReaderHandleFromThread);
		CASE_RETURN_LIT(EIoDuplicateWriterHandleFromThread);
		DEFAULT_RETURN_LIT("*** OPCODE UNKNOWN ***");
		}
	}

class TOverflowTruncate : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) {}
	};

class RMessageAccess : public RMsg
	{
public:
	TInt Handle() const { return iHandle; }
	};

void CIoLog::StartServiceLC(const RMsg& aMessage)
	{
	CIoLog& self = Self();
	ASSERT(!self.iInServiceL);
	self.iScratchBuf.SetLength(0);
	self.iScratchBuf = ClientNameL(aMessage);
	CleanupStack::PushL(TCleanupItem(EndService, &self));
	TOverflowTruncate overflow;
	self.iScratchBuf.AppendFormat(_L(" requested %S (opcode: %d, message handle: %d)"), &overflow, StringifyOpcode(aMessage.Function()), aMessage.Function(), static_cast<const RMessageAccess&>(aMessage).Handle());
	self.Write(self.iScratchBuf);
	self.iInServiceL = ETrue;
	}

void CIoLog::EndService(TAny* aSelf)
	{
	CIoLog& self = *(static_cast<CIoLog*>(aSelf));
	ASSERT(self.iInServiceL);
	self.Write(KNullDesC);
	self.iInServiceL = EFalse;
	}

void CIoLog::LogCompletion(const RMsg& aMessage, TInt aError)
	{
	TFullName threadName(_L("unknown"));
	TRAP_IGNORE(threadName = ClientNameL(aMessage));
	CIoLog& self = Self();
	self.iScratchBuf.SetLength(0);
	TOverflowTruncate overflow;
	self.iScratchBuf.AppendFormat(_L("Completing %S's request (opcode: %S, message handle: %d) with %S(%d)"), &overflow, &threadName, StringifyOpcode(aMessage.Function()), static_cast<const RMessageAccess&>(aMessage).Handle(), StringifyError(aError), aError);
	self.Write(self.iScratchBuf);
	}

void CIoLog::Write(const TDesC& aData)
	{
	CIoLog& self = Self();
#ifdef IOSRV_LOGGING_USES_CLOGGER
	if (aData.Length() == 0) return; // The trick of writing knulldesc to get a newline is not really appropriate with clogger
	self.iClogger.Log(aData);
#else
	if (self.iFile.SubSessionHandle())
		{
		self.iNarrowBuf.Copy(aData.Left(self.iNarrowBuf.MaxLength() - KNewLine().Length() - 1));
		if (self.iInServiceL)
			{
			self.iNarrowBuf.Insert(0, _L8("\t"));
			}
		self.iNarrowBuf.Append(KNewLine);
		self.iFile.Write(self.iNarrowBuf);
		self.iFile.Flush();
		}
#endif
	}

void CIoLog::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	TOverflowTruncate overflow;
	VA_LIST list;
	VA_START(list, aFmt);
	CIoLog& self = Self();
	self.iScratchBuf.SetLength(0);
	self.iScratchBuf.AppendFormatList(aFmt, list, &overflow);
	Write(self.iScratchBuf);
	}

CIoLog::CIoLog(RFs& aFs)
	: iFs(aFs)
	{
	}

void CIoLog::ConstructL()
	{
#ifdef IOSRV_LOGGING_USES_CLOGGER
	_LIT(KIoSrvLog, "iosrv");
	User::LeaveIfError(iClogger.Connect(KIoSrvLog));
#else
//	iFs.MkDirAll(KLogFileName);
	/*User::LeaveIfError*/(iFile.Replace(iFs, KLogFileName, EFileWrite));
#endif
#if defined(__WINS__) && !defined(EKA2)
	Dll::SetTls(this);
#else
	gLog = this;
#endif
	Write(KNullDesC);
	Write(_L("New log created"));
	}

CIoLog& CIoLog::Self()
	{
#if defined(__WINS__) && !defined(EKA2)
	return *(static_cast<CIoLog*>(Dll::Tls()));
#else
	return *gLog;
#endif
	}

#endif
