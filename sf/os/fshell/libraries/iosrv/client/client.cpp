// client.cpp
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

#include <fshell/iocli.h>
#include "clientserver.h"
#include <e32math.h>


#ifndef EKA2

//
// TServerStart.
//

TServerStart::TServerStart(TRequestStatus& aStatus)
	: iId(RThread().Id()), iStatus(&aStatus)
	{
	aStatus = KRequestPending;
	}

TPtrC TServerStart::AsCommand() const
	{
	return TPtrC(reinterpret_cast<const TText*>(this),sizeof(TServerStart)/sizeof(TText));
	}

#endif


//
// Statics.
//

static TInt StartServer()
	{
#ifdef EKA2
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
	RProcess server;
	TInt r = server.Create(KIoServerName, KNullDesC, serverUid);
	if (r != KErrNone)
		{
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		server.Kill(0);
		}
	else
		{
		server.Resume();
		}
	User::WaitForRequest(stat);		// wait for start or death
	r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
#else
	TRequestStatus started;
	TServerStart start(started);
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
#ifdef __WINS__
	RLibrary lib;
	TInt r = lib.Load(KIoServerName, serverUid);
	if (r != KErrNone)
		{
		return r;
		}
	TLibraryFunction ordinal1 = lib.Lookup(1);
	TThreadFunction serverFunc = reinterpret_cast<TThreadFunction>(ordinal1());
	TName name(KIoServerName);
	name.AppendNum(Math::Random(), EHex);
	RThread server;
	r=server.Create(name, serverFunc,
					KIoStackSize,
					&start, &lib, NULL,
					KIoInitHeapSize, KIoMaxHeapSize, EOwnerProcess);
	lib.Close();
#else
	RProcess server;
	TInt r=server.Create(KIoServerName, start.AsCommand(), serverUid);
#endif
	if (r != KErrNone)
		{
		return r;
		}
	TRequestStatus died;
	server.Logon(died);
	if (died != KRequestPending)
		{
		User::WaitForRequest(died);
		server.Kill(0);
		server.Close();
		return died.Int();
		}
	server.Resume();
	User::WaitForRequest(started, died);
	if (started == KRequestPending)
		{
		server.Close();
		return died.Int();
		}
	server.LogonCancel(died);
	server.Close();
	User::WaitForRequest(died);
	return KErrNone;
#endif // EKA2
	}

#ifndef EKA2
TInt E32Dll(TDllReason)
	{
	return 0;
	}
#endif


//
// RIoSession.
//

EXPORT_C TInt RIoSession::Connect()
	{
	TInt retry = 2;
	for (;;)
		{
		TInt r = CreateSession(KIoServerName, TVersion(0,0,0));
		if ((r != KErrNotFound) && (r != KErrServerTerminated))
			{
			return r;
			}
		if (--retry == 0)
			{
			return r;
			}
		r = StartServer();
		if ((r != KErrNone) && (r != KErrAlreadyExists))
			{
			return r;
			}
		}
	}
	
EXPORT_C void RIoSession::ConnectL()
	{
	User::LeaveIfError(Connect());
	}

EXPORT_C TInt RIoSession::SetObjectName(TInt aHandle, const TDesC& aName)
	{
#ifdef EKA2
	return SendReceive(EIoSetObjectName, TIpcArgs(aHandle, &aName));
#else
	TInt p[4];
	p[0] = aHandle;
	p[1] = (TInt)&aName;
	return SendReceive(EIoSetObjectName, &p[0]);
#endif
	}

EXPORT_C void RIoSession::SetObjectNameL(TInt aHandle, const TDesC& aName)
	{
	User::LeaveIfError(SetObjectName(aHandle, aName));
	}
	
EXPORT_C TInt RIoSession::FindFirstHandle(RIoHandle::TType aType, const TDesC& aMatchString, TInt& aFoundHandle, TName& aName)
	{
#ifdef EKA2
	TPckg<TInt> foundBuf(aFoundHandle);
	return SendReceive(EIoFindFirstHandle, TIpcArgs(aType, &aMatchString, &foundBuf, &aName));
#else
	TInt p[4];
	p[0] = aType;
	p[1] = &aMatchString;
	p[2] = &aFoundHandle;
	p[3] = &aName;
	return SendReceive(EIoFindFirstHandle, &p[0]);
#endif
	}
	
EXPORT_C TInt RIoSession::FindNextHandle(TInt& aFoundHandle, TName& aName)
	{
#ifdef EKA2
	TPckg<TInt> foundBuf(aFoundHandle);
	return SendReceive(EIoFindNextHandle, TIpcArgs(&foundBuf, &aName));
#else
	TInt p[4];
	p[0] = &aFoundHandle;
	p[1] = &aName;
	return SendReceive(EIoFindNextHandle, &p[0]);
#endif
	}


//
// RIoHandle.
//

EXPORT_C TInt RIoHandle::OpenFoundHandle(RIoSession& aSession, TInt aFoundHandle)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenFoundHandle, TIpcArgs(aFoundHandle));
#else
	TInt p[4];
	p[0] = aFoundHandle;
	return CreateSubSession(aSession, EIoOpenFoundHandle, &p[0]);
#endif
	}
	
EXPORT_C void RIoHandle::OpenFoundHandleL(RIoSession& aSession, TInt aFoundHandle)
	{
	User::LeaveIfError(OpenFoundHandle(aSession, aFoundHandle));
	}

EXPORT_C void RIoHandle::Close()
	{
	CloseSubSession(EIoHandleClose);
	}

EXPORT_C TInt RIoHandle::IsType(TType aType) const
	{
#ifdef EKA2
	return SendReceive(EIoHandleIsType, TIpcArgs(aType));
#else
	TInt p[4];
	p[0] = (TInt)aType;
	return SendReceive(EIoHandleIsType, &p[0]);
#endif
	
	}
	
EXPORT_C TBool RIoHandle::IsTypeL(TType aType) const
	{
	return User::LeaveIfError(IsType(aType));
	}
	
EXPORT_C TInt RIoHandle::ObjectName(TDes& aName) const
	{
#ifdef EKA2
	return SendReceive(EIoHandleGetName, TIpcArgs(&aName));
#else
	TInt p[4];
	p[0] = &aName;
	return SendReceive(EIoHandleGetName, &p[0]);
#endif
	}
	
EXPORT_C void RIoHandle::ObjectNameL(TDes& aName) const
	{
	User::LeaveIfError(ObjectName(aName));
	}
	
EXPORT_C TInt RIoHandle::Equals(const RIoHandle& aNother) const
	{
#ifdef EKA2
	return SendReceive(EIoHandleEquals, TIpcArgs(aNother.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = (TInt)aNother.SubSessionHandle();
	return SendReceive(EIoHandleEquals, &p[0]);
#endif
	}
	
EXPORT_C TBool RIoHandle::EqualsL(const RIoHandle& aNother) const
	{
	return User::LeaveIfError(Equals(aNother));
	}

EXPORT_C RIoSession RIoHandle::Session() const
	{
	RSessionBase session = RSubSessionBase::Session();
	return static_cast<RIoSession&>(session);
	}


//
// RIoReadWriteHandle.
//

EXPORT_C TInt RIoReadWriteHandle::SetMode(TMode aMode)
	{
#ifdef EKA2
	return SendReceive(EIoSetReadWriteMode, TIpcArgs(aMode));
#else
	TInt p[4];
	p[0] = (TInt)aMode;
	return SendReceive(EIoSetReadWriteMode, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadWriteHandle::SetModeL(TMode aMode)
	{
	User::LeaveIfError(SetMode(aMode));
	}

EXPORT_C TInt RIoReadWriteHandle::SetOwner(TThreadId aOwningThread)
	{
#ifdef EKA2
	return SendReceive(EIoHandleSetOwner, TIpcArgs(aOwningThread));
#else
	TInt p[4];
	p[0] = aOwningThread;
	return SendReceive(EIoHandleSetOwner, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadWriteHandle::SetOwnerL(TThreadId aOwningThread)
	{
	User::LeaveIfError(SetOwner(aOwningThread));
	}

EXPORT_C TInt RIoReadWriteHandle::SetUnderlyingConsole(RIoConsole& aConsole)
	{
#ifdef EKA2
	return SendReceive(EIoHandleSetUnderlyingConsole, TIpcArgs(aConsole.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = aConsole.SubSessionHandle();
	return SendReceive(EIoHandleSetUnderlyingConsole, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadWriteHandle::SetUnderlyingConsoleL(RIoConsole& aConsole)
	{
	User::LeaveIfError(SetUnderlyingConsole(aConsole));
	}

EXPORT_C TInt RIoReadWriteHandle::AttachedToConsole() const
	{
#ifdef EKA2
	return SendReceive(EIoHandleAttachedToConsole);
#else
	return SendReceive(EIoHandleAttachedToConsole, NULL);
#endif
	}
	
EXPORT_C TBool RIoReadWriteHandle::AttachedToConsoleL() const
	{
	return User::LeaveIfError(AttachedToConsole());
	}

//
// RIoReadHandle.
//

EXPORT_C TInt RIoReadHandle::Create(RIoSession& aSession)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreateReader);
#else
	return CreateSubSession(aSession, EIoCreateReader, NULL);
#endif
	}
	
EXPORT_C void RIoReadHandle::CreateL(RIoSession& aSession)
	{
	User::LeaveIfError(Create(aSession));
	}

EXPORT_C TInt RIoReadHandle::Open(RIoSession& aSession)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenReaderByThreadId);
#else
	return CreateSubSession(aSession, EIoOpenReaderByThreadId, NULL);
#endif
	}
	
EXPORT_C void RIoReadHandle::OpenL(RIoSession& aSession)
	{
	User::LeaveIfError(Open(aSession));
	}

EXPORT_C TInt RIoReadHandle::Open(RIoSession& aSession, TThreadId aThreadId)
	{
	TPckgBuf<TThreadId> threadIdPckg(aThreadId);
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenReaderByExplicitThreadId, TIpcArgs(&threadIdPckg));
#else
	TInt p[4];
	p[0] = &threadIdPckg;
	return CreateSubSession(aSession, EIoOpenReaderByExplicitThreadId, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadHandle::OpenL(RIoSession& aSession, TThreadId aThreadId)
	{
	User::LeaveIfError(Open(aSession, aThreadId));
	}

EXPORT_C TInt RIoReadHandle::Duplicate(RIoReadHandle& aReadHandle)
	{
#ifdef EKA2
	return SendReceive(EIoDuplicateReader, TIpcArgs(aReadHandle.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = aReadHandle.SubSessionHandle();
	return SendReceive(EIoDuplicateReader, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadHandle::DuplicateL(RIoReadHandle& aReadHandle)
	{
	User::LeaveIfError(Duplicate(aReadHandle));
	}

EXPORT_C TInt RIoReadHandle::DuplicateHandleFromThread(TThreadId aThreadId)
	{
	return SendReceive(EIoDuplicateReaderHandleFromThread, TIpcArgs(TUint(aThreadId)));
	}

EXPORT_C TInt RIoReadHandle::SetReadMode(TReadMode aMode)
	{
#ifdef EKA2
	return SendReceive(EIoSetReadMode, TIpcArgs(aMode));
#else
	TInt p[4];
	p[0] = (TInt)aMode;
	return SendReceive(EIoSetReadMode, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadHandle::SetReadModeL(TReadMode aMode)
	{
	User::LeaveIfError(SetReadMode(aMode));
	}

EXPORT_C TInt RIoReadHandle::SetToForeground()
	{
#ifdef EKA2
	return SendReceive(EIoSetReaderToForeground);
#else
	return SendReceive(EIoSetReaderToForeground, NULL);
#endif
	}

EXPORT_C void RIoReadHandle::SetToForegroundL()
	{
	User::LeaveIfError(SetToForeground());
	}

EXPORT_C TInt RIoReadHandle::Read(TDes& aDes)
	{
#ifdef EKA2
	return SendReceive(EIoRead, TIpcArgs(&aDes));
#else
	TInt p[4];
	p[0] = (TInt)&aDes;
	return SendReceive(EIoRead, &p[0]);
#endif
	}
	
EXPORT_C void RIoReadHandle::ReadL(TDes& aDes)
	{
	User::LeaveIfError(Read(aDes));
	}

EXPORT_C void RIoReadHandle::Read(TDes& aDes, TRequestStatus& aStatus)
	{
#ifdef EKA2
	return SendReceive(EIoRead, TIpcArgs(&aDes), aStatus);
#else
	TInt p[4];
	p[0] = (TInt)&aDes;
	SendReceive(EIoRead, &p[0], aStatus);
#endif
	}

EXPORT_C TInt RIoReadHandle::SetLineSeparator(const TDesC& aSeparator)
	{
#ifdef EKA2
	return SendReceive(EIoSetLineSeparator, TIpcArgs(&aSeparator));
#else
	TInt p[4];
	p[0] = (TInt)&aSeparator;
	return SendReceive(EIoSetLineSeparator, &p[0]);
#endif
	}

EXPORT_C void RIoReadHandle::SetLineSeparatorL(const TDesC& aSeparator)
	{
	User::LeaveIfError(SetLineSeparator(aSeparator));
	}

EXPORT_C void RIoReadHandle::ReadCancel()
	{
#ifdef EKA2
	SendReceive(EIoReadCancel);
#else
	SendReceive(EIoReadCancel, NULL);
#endif
	}

EXPORT_C TInt RIoReadHandle::IsForeground() const
	{
#ifdef EKA2
	return SendReceive(EIoIsForegroundReader);
#else
	return SendReceive(EIoIsForegroundReader, NULL);
#endif
	}

EXPORT_C TBool RIoReadHandle::IsForegroundL() const
	{
	return User::LeaveIfError(IsForeground());
	}

EXPORT_C void RIoReadHandle::NotifyChange(TPckgBuf<TUint>& aChangeType, TRequestStatus& aStatus)
	{
#ifdef EKA2
	SendReceive(EIoReadHandleNotifyChange, TIpcArgs(&aChangeType), aStatus);
#else
	TInt p[4];
	p[0] = (TInt)&aChangeType;;
	SendReceive(EIoReadHandleNotifyChange, &p[0], aStatus);
#endif
	}

EXPORT_C void RIoReadHandle::CancelNotifyChange()
	{
	SendReceive(EIoReadHandleCancelNotifyChange);
	}


//
// RIoConsoleReadHandle.
//

EXPORT_C RIoConsoleReadHandle::RIoConsoleReadHandle()
	{
	}

EXPORT_C RIoConsoleReadHandle::RIoConsoleReadHandle(RIoReadHandle& aHandle)
	{
	*(RIoReadHandle*)this = aHandle; // Take a bit-wise copy of the base class.
	}

EXPORT_C RIoConsoleReadHandle RIoConsoleReadHandle::operator=(RIoReadHandle& aHandle)
	{
	*(RIoReadHandle*)this = aHandle; // Take a bit-wise copy of the base class.
	return *this;
	}

EXPORT_C TUint RIoConsoleReadHandle::ReadKey()
	{
	TRequestStatus status;
	WaitForKey(status);
	User::WaitForRequest(status);
	if (status.Int())
		{
		return EKeyNull;
		}
	return KeyCode();
	}

EXPORT_C void RIoConsoleReadHandle::WaitForKey(TRequestStatus& aStatus)
	{
#ifdef EKA2
	return SendReceive(EIoConsoleWaitForKey, TIpcArgs(&iKeyBuf), aStatus);
#else
	TInt p[4];
	p[0] = (TInt)&iKeyBuf;
	SendReceive(EIoConsoleWaitForKey, &p[0], aStatus);
#endif
	}

EXPORT_C void RIoConsoleReadHandle::WaitForKeyCancel()
	{
#ifdef EKA2
	SendReceive(EIoConsoleWaitForKeyCancel);
#else
	SendReceive(EIoConsoleWaitForKeyCancel, NULL);
#endif
	}

EXPORT_C TUint RIoConsoleReadHandle::KeyCode() const
	{
	return iKeyBuf().iKeyCode;
	}

EXPORT_C TUint RIoConsoleReadHandle::KeyModifiers() const
	{
	return iKeyBuf().iModifiers;
	}

EXPORT_C TInt RIoConsoleReadHandle::CaptureKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers)
	{
#ifdef EKA2
	return SendReceive(EIoConsoleCaptureKey, TIpcArgs(aKeyCode, aModifierMask, aModifiers));
#else
	TInt p[4];
	p[0] = aKeyCode;
	p[1] = aModifierMask;
	p[2] = aModifiers;
	return SendReceive(EIoConsoleCaptureKey, &p[0]);
#endif
	}

EXPORT_C void RIoConsoleReadHandle::CaptureKeyL(TUint aKeyCode, TUint aModifierMask, TUint aModifiers)
	{
	User::LeaveIfError(CaptureKey(aKeyCode, aModifierMask, aModifiers));
	}

EXPORT_C TInt RIoConsoleReadHandle::CancelCaptureKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers)
	{
#ifdef EKA2
	return SendReceive(EIoConsoleCancelCaptureKey, TIpcArgs(aKeyCode, aModifierMask, aModifiers));
#else
	TInt p[4];
	p[0] = aKeyCode;
	p[1] = aModifierMask;
	p[2] = aModifiers;
	return SendReceive(EIoConsoleCancelCaptureKey, &p[0]);
#endif
	}

EXPORT_C TInt RIoConsoleReadHandle::CaptureAllKeys()
	{
#ifdef EKA2
	return SendReceive(EIoConsoleCaptureAllKeys);
#else
	return SendReceive(EIoConsoleCaptureAllKeys, NULL);
#endif
	}
	
EXPORT_C void RIoConsoleReadHandle::CaptureAllKeysL()
	{
	User::LeaveIfError(CaptureAllKeys());
	}

EXPORT_C TInt RIoConsoleReadHandle::CancelCaptureAllKeys()
	{
#ifdef EKA2
	return SendReceive(EIoConsoleCancelCaptureAllKeys);
#else
	return SendReceive(EIoConsoleCancelCaptureAllKeys, NULL);
#endif
	}


//
// RIoWriteHandle.
//

EXPORT_C TInt RIoWriteHandle::Create(RIoSession& aSession)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreateWriter);
#else
	return CreateSubSession(aSession, EIoCreateWriter, NULL);
#endif
	}
	
EXPORT_C void RIoWriteHandle::CreateL(RIoSession& aSession)
	{
	User::LeaveIfError(Create(aSession));
	}

EXPORT_C TInt RIoWriteHandle::Open(RIoSession& aSession)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenWriterByThreadId);
#else
	return CreateSubSession(aSession, EIoOpenWriterByThreadId, NULL);
#endif
	}
	
EXPORT_C void RIoWriteHandle::OpenL(RIoSession& aSession)
	{
	User::LeaveIfError(Open(aSession));
	}

EXPORT_C TInt RIoWriteHandle::Open(RIoSession& aSession, TThreadId aThreadId)
	{
	TPckgBuf<TThreadId> threadIdPckg(aThreadId);
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenWriterByExplicitThreadId, TIpcArgs(&threadIdPckg));
#else
	TInt p[4];
	p[0] = &threadIdPckg;
	return CreateSubSession(aSession, EIoOpenWriterByExplicitThreadId, &p[0]);
#endif
	}

EXPORT_C void RIoWriteHandle::OpenL(RIoSession& aSession, TThreadId aThreadId)
	{
	User::LeaveIfError(Open(aSession, aThreadId));
	}

EXPORT_C TInt RIoWriteHandle::Duplicate(RIoWriteHandle& aWriteHandle)
	{
#ifdef EKA2
	return SendReceive(EIoDuplicateWriter, TIpcArgs(aWriteHandle.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = aWriteHandle.SubSessionHandle();
	return SendReceive(EIoDuplicateWriter, &p[0]);
#endif
	}
	
EXPORT_C void RIoWriteHandle::DuplicateL(RIoWriteHandle& aWriteHandle)
	{
	User::LeaveIfError(Duplicate(aWriteHandle));
	}

EXPORT_C TInt RIoWriteHandle::DuplicateHandleFromThread(TThreadId aThreadId)
	{
	return SendReceive(EIoDuplicateWriterHandleFromThread, TIpcArgs(TUint(aThreadId)));
	}

EXPORT_C TInt RIoWriteHandle::Write(const TDesC& aDes)
	{
#ifdef EKA2
	return SendReceive(EIoWrite, TIpcArgs(&aDes));
#else
	TInt p[4];
	p[0] = (TInt)&aDes;
	return SendReceive(EIoWrite, &p[0]);
#endif
	}
	
EXPORT_C void RIoWriteHandle::WriteL(const TDesC& aDes)
	{
	User::LeaveIfError(Write(aDes));
	}

EXPORT_C void RIoWriteHandle::Write(const TDesC& aDes, TRequestStatus& aStatus)
	{
#ifdef EKA2
	return SendReceive(EIoWrite, TIpcArgs(&aDes), aStatus);
#else
	TInt p[4];
	p[0] = (TInt)&aDes;
	SendReceive(EIoWrite, &p[0], aStatus);
#endif
	}

EXPORT_C void RIoWriteHandle::WriteCancel()
	{
#ifdef EKA2
	SendReceive(EIoWriteCancel);
#else
	SendReceive(EIoWriteCancel, NULL);
#endif
	}


//
// RIoConsoleWriteHandle.
//

EXPORT_C RIoConsoleWriteHandle::RIoConsoleWriteHandle()
	{
	}

EXPORT_C RIoConsoleWriteHandle::RIoConsoleWriteHandle(RIoWriteHandle& aHandle)
	{
	*(RIoWriteHandle*)this = aHandle; // Take a bit-wise copy of the base class.
	}

EXPORT_C RIoConsoleWriteHandle RIoConsoleWriteHandle::operator=(RIoWriteHandle& aHandle)
	{
	*(RIoWriteHandle*)this = aHandle; // Take a bit-wise copy of the base class.
	return *this;
	}

EXPORT_C TInt RIoConsoleWriteHandle::GetCursorPos(TPoint& aPos) const
	{
	TPckg<TPoint> posPckg(aPos);
#ifdef EKA2
	TInt err = SendReceive(EIoConsoleCursorPos, TIpcArgs(&posPckg));
#else
	TInt p[4];
	p[0] = (TInt)&posPckg;
	TInt err = SendReceive(EIoConsoleCursorPos, &p[0]);
#endif
	return err;
	}
	
EXPORT_C TPoint RIoConsoleWriteHandle::GetCursorPosL() const
	{
	TPoint pos;
	User::LeaveIfError(GetCursorPos(pos));
	return pos;
	}

EXPORT_C TInt RIoConsoleWriteHandle::SetCursorPosAbs(const TPoint& aPoint)
	{
	TPckg<TPoint> posPckg(aPoint);
#ifdef EKA2
	return SendReceive(EIoConsoleSetCursorPosAbs, TIpcArgs(&posPckg));
#else
	TInt p[4];
	p[0] = (TInt)&posPckg;
	return SendReceive(EIoConsoleSetCursorPosAbs, &p[0]);
#endif
	}
	
EXPORT_C void RIoConsoleWriteHandle::SetCursorPosAbsL(const TPoint& aPos)
	{
	User::LeaveIfError(SetCursorPosAbs(aPos));
	}

EXPORT_C TInt RIoConsoleWriteHandle::SetCursorPosRel(const TPoint& aPoint)
	{
	TPckg<TPoint> posPckg(aPoint);
#ifdef EKA2
	return SendReceive(EIoConsoleSetCursorPosRel, TIpcArgs(&posPckg));
#else
	TInt p[4];
	p[0] = (TInt)&posPckg;
	return SendReceive(EIoConsoleSetCursorPosRel, &p[0]);
#endif
	}

EXPORT_C void RIoConsoleWriteHandle::SetCursorPosRelL(const TPoint& aPos)
	{
	User::LeaveIfError(SetCursorPosRel(aPos));
	}

EXPORT_C TInt RIoConsoleWriteHandle::SetCursorHeight(TInt aPercentage)
	{
#ifdef EKA2
	return SendReceive(EIoConsoleSetCursorHeight, TIpcArgs(aPercentage));
#else
	TInt p[4];
	p[0] = aPercentage;
	return SendReceive(EIoConsoleSetCursorHeight, &p[0]);
#endif
	}

EXPORT_C void RIoConsoleWriteHandle::SetCursorHeightL(TInt aPercentage)
	{
	User::LeaveIfError(SetCursorHeight(aPercentage));
	}

EXPORT_C TInt RIoConsoleWriteHandle::SetTitle(const TDesC& aTitle)
	{
#ifdef EKA2
	return SendReceive(EIoConsoleSetTitle, TIpcArgs(&aTitle));
#else
	TInt p[4];
	p[0] = (TInt)&aTitle;
	return SendReceive(EIoConsoleSetTitle, &p[0]);
#endif
	}
	
EXPORT_C void RIoConsoleWriteHandle::SetTitleL(const TDesC& aTitle)
	{
	User::LeaveIfError(SetTitle(aTitle));
	}

EXPORT_C TInt RIoConsoleWriteHandle::ClearScreen()
	{
#ifdef EKA2
	return SendReceive(EIoConsoleClearScreen);
#else
	return SendReceive(EIoConsoleClearScreen, NULL);
#endif
	}
	
EXPORT_C void RIoConsoleWriteHandle::ClearScreenL()
	{
	User::LeaveIfError(ClearScreen());
	}

EXPORT_C TInt RIoConsoleWriteHandle::ClearToEndOfLine()
	{
#ifdef EKA2
	return SendReceive(EIoConsoleClearToEndOfLine);
#else
	return SendReceive(EIoConsoleClearToEndOfLine, NULL);
#endif
	}
	
EXPORT_C void RIoConsoleWriteHandle::ClearToEndOfLineL()
	{
	User::LeaveIfError(ClearToEndOfLine());
	}

EXPORT_C TInt RIoConsoleWriteHandle::GetScreenSize(TSize& aSize) const
	{
	TPckg<TSize> sizePckg(aSize);
#ifdef EKA2
	TInt err = SendReceive(EIoConsoleScreenSize, TIpcArgs(&sizePckg));
#else
	TInt p[4];
	p[0] = (TInt)&sizePckg;
	TInt err = SendReceive(EIoConsoleScreenSize, &p[0]);
#endif
	return err;
	}
	
EXPORT_C TSize RIoConsoleWriteHandle::GetScreenSizeL() const
	{
	TSize size;
	User::LeaveIfError(GetScreenSize(size));
	return size;
	}
	
EXPORT_C TInt RIoConsoleWriteHandle::SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	{
#ifdef EKA2
	TInt err = SendReceive(EIoConsoleSetAttributes, TIpcArgs(aAttributes, aForegroundColor, aBackgroundColor));
#else
	TInt p[4];
	p[0] = aAttributes;
	p[1] = aForegroundColor;
	p[2] = aBackgroundColor;
	TInt err = SendReceive(EIoConsoleSetAttributes, &p[0]);
#endif
	return err;
	}

EXPORT_C void RIoConsoleWriteHandle::SetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	{
	User::LeaveIfError(SetAttributes(aAttributes, aForegroundColor, aBackgroundColor));
	}

EXPORT_C TInt RIoConsoleWriteHandle::SetAttributes(const ConsoleAttributes::TAttributes& aAttributes)
	{
	return SetAttributes(aAttributes.iAttributes, aAttributes.iForegroundColor, aAttributes.iBackgroundColor);
	}

EXPORT_C TInt RIoConsoleWriteHandle::SetIsStdErr(TBool aFlag)
	{
	return SendReceive(EIoSetIsStdErr, TIpcArgs(aFlag));
	}


//
// RIoEndPoint.
//

EXPORT_C TInt RIoEndPoint::Attach(RIoReadHandle& aReader, TReadMode aMode)
	{
#ifdef EKA2
	return SendReceive(EIoEndPointAttachReader, TIpcArgs(aReader.SubSessionHandle(), aMode));
#else
	TInt p[4];
	p[0] = (TInt)aReader.SubSessionHandle();
	p[1] = aMode;
	return SendReceive(EIoEndPointAttachReader, &p[0]);
#endif
	}

EXPORT_C void RIoEndPoint::AttachL(RIoReadHandle& aReader, TReadMode aMode)
	{
	User::LeaveIfError(Attach(aReader, aMode));
	}

EXPORT_C TInt RIoEndPoint::Attach(RIoWriteHandle& aWriter)
	{
#ifdef EKA2
	return SendReceive(EIoEndPointAttachWriter, TIpcArgs(aWriter.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = (TInt)aWriter.SubSessionHandle();
	return SendReceive(EIoEndPointAttachWriter, &p[0]);
#endif
	}
	
EXPORT_C void RIoEndPoint::AttachL(RIoWriteHandle& aWriter)
	{
	User::LeaveIfError(Attach(aWriter));
	}

EXPORT_C TInt RIoEndPoint::SetForegroundReadHandle(RIoReadHandle& aReader)
	{
#ifdef EKA2
	return SendReceive(EIoEndPointSetForegroundReadHandle, TIpcArgs(aReader.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = (TInt)aReader.SubSessionHandle();
	return SendReceive(EIoEndPointSetForegroundReadHandle, &p[0]);
#endif
	}
	
EXPORT_C void RIoEndPoint::SetForegroundReadHandleL(RIoReadHandle& aReader)
	{
	User::LeaveIfError(SetForegroundReadHandle(aReader));
	}

EXPORT_C RIoEndPoint::RIoEndPoint()
	{
	}


//
// RIoPipe.
//

EXPORT_C TInt RIoPipe::Create(RIoSession& aSession)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreatePipe);
#else
	return CreateSubSession(aSession, EIoCreatePipe, NULL);
#endif
	}

EXPORT_C void RIoPipe::CreateL(RIoSession& aSession)
	{
	User::LeaveIfError(Create(aSession));
	}

//
// RIoConsole.
//

EXPORT_C TInt RIoConsole::Create(RIoSession& aSession, const TDesC& aTitle, const TSize& aSize, TUint aOptions)
	{
	return Create(aSession, KNullDesC, aTitle, aSize, aOptions);
	}
	
EXPORT_C void RIoConsole::CreateL(RIoSession& aSession, const TDesC& aTitle, const TSize& aSize, TUint aOptions)
	{
	User::LeaveIfError(Create(aSession, aTitle, aSize, aOptions));
	}

EXPORT_C TInt RIoConsole::Create(RIoSession& aSession, const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, TUint aOptions)
	{
	TConsoleCreateParams params;
	params.iSize = aSize;
	params.iUnderlyingConsoleHandle = KNullHandle;
	params.iOptions = aOptions;
	TPckg<TConsoleCreateParams> paramsPckg(params);
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreateConsole, TIpcArgs(&aTitle, &paramsPckg, &aImplementation));
#else
	TInt p[4];
	p[0] = (TInt)&aTitle;
	p[1] = (TInt)&paramsPckg;
	p[2] = (TInt)&aImplementation;
	return CreateSubSession(aSession, EIoCreateConsole, &p[0]);
#endif
	}

EXPORT_C TInt RIoConsole::Create(RIoSession& aSession, const TDesC& aImplementation, RIoConsole& aUnderlyingConsole, const TDesC& aTitle, const TSize& aSize, TUint aOptions)
	{
	TConsoleCreateParams params;
	params.iSize = aSize;
	params.iUnderlyingConsoleHandle = aUnderlyingConsole.SubSessionHandle();
	params.iOptions = aOptions;
	TPckg<TConsoleCreateParams> paramsPckg(params);
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreateConsole, TIpcArgs(&aTitle, &paramsPckg, &aImplementation));
#else
	TInt p[4];
	p[0] = (TInt)&aTitle;
	p[1] = (TInt)&paramsPckg;
	p[2] = (TInt)&aImplementation;
	return CreateSubSession(aSession, EIoCreateConsole, &p[0]);
#endif
	}

EXPORT_C void RIoConsole::CreateL(RIoSession& aSession, const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, TUint aOptions)
	{
	User::LeaveIfError(Create(aSession, aImplementation, aTitle, aSize, aOptions));
	}

EXPORT_C void RIoConsole::CreateL(RIoSession& aSession, const TDesC& aImplementation, RIoConsole& aUnderlyingConsole, const TDesC& aTitle, const TSize& aSize, TUint aOptions)
	{
	User::LeaveIfError(Create(aSession, aImplementation, aUnderlyingConsole, aTitle, aSize, aOptions));
	}

EXPORT_C TInt RIoConsole::Open(RIoSession& aSession, RIoHandle& aHandle)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenConsole, TIpcArgs(aHandle.SubSessionHandle()));
#else
	TInt p[4];
	p[0] = aHandle.SubSessionHandle();
	return CreateSubSession(aSession, EIoOpenConsole, &p[0]);
#endif
	}
	
EXPORT_C void RIoConsole::OpenL(RIoSession& aSession, RIoHandle& aHandle)
	{
	User::LeaveIfError(Open(aSession, aHandle));
	}

EXPORT_C TInt RIoConsole::Implementation(TDes& aDes) const
	{
#ifdef EKA2
	return SendReceive(EIoConsoleImplementation, TIpcArgs(&aDes));
#else
	TInt p[4];
	p[0] = (TInt)&aDes;
	return SendReceive(EIoConsoleImplementation, &p[0]);
#endif
	}
	
EXPORT_C void RIoConsole::ImplementationL(TDes& aDes) const
	{
	User::LeaveIfError(Implementation(aDes));
	}

//
// RIoPersistentConsole.
//
EXPORT_C TInt RIoPersistentConsole::Create(RIoSession& aSession, const TDesC& aName, const TDesC& aTitle)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreatePersistentConsole, TIpcArgs(&aName, &aTitle));
#else
	TInt p[4];
	p[0] = (TInt)&aName;
	p[1] = (TInt)&aTitle;
	return CreateSubSession(aSession, EIoCreatePersistentConsole, &p[0]);
#endif
	}
	
EXPORT_C void RIoPersistentConsole::CreateL(RIoSession& aSession, const TDesC& aName, const TDesC& aTitle)
	{
	User::LeaveIfError(Create(aSession, aName, aTitle));
	}
	
EXPORT_C TInt RIoPersistentConsole::OpenByName(RIoSession& aSession, const TDesC& aName)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoOpenPersistentConsoleByName, TIpcArgs(&aName));
#else
	TInt p[4];
	p[0] = (TInt)&aTitle;
	return CreateSubSession(aSession, EIoOpenPersistentConsoleByName, &p[0]);
#endif
	}
	
EXPORT_C void RIoPersistentConsole::OpenByNameL(RIoSession& aSession, const TDesC& aName)
	{
	User::LeaveIfError(OpenByName(aSession, aName));
	}

EXPORT_C TInt RIoPersistentConsole::AttachReader(RIoEndPoint& aEndPoint, TCloseBehaviour aCloseBehaviour)
	{
#ifdef EKA2
	return SendReceive(EIoPersistentConsoleAttachReadEndPoint, TIpcArgs(aEndPoint.SubSessionHandle(), aCloseBehaviour));
#else
	TInt p[4];
	p[0] = aEndPoint.Handle();
	p[1] = aCloseBehaviour;
	return SendReceive(EIoPersistentConsoleAttachReadEndPoint, &p[0]);
#endif
	
	}
	
EXPORT_C void RIoPersistentConsole::AttachReaderL(RIoEndPoint& aEndPoint, TCloseBehaviour aCloseBehaviour)
	{
	User::LeaveIfError(AttachReader(aEndPoint, aCloseBehaviour));
	}
	
EXPORT_C void RIoPersistentConsole::DetachReader()
	{
#ifdef EKA2
	SendReceive(EIoPersistentConsoleDetachReadEndPoint, TIpcArgs());
#else
	TInt p[4];
	SendReceive(EIoPersistentConsoleDetachReadEndPoint, &p[0]);
#endif
	}
	
EXPORT_C void RIoPersistentConsole::NotifyReaderDetach(TRequestStatus& aStatus)
	{
	SendReceive(EIoPersistentConsoleNotifyReadDetach, aStatus);
	}
	
EXPORT_C void RIoPersistentConsole::CancelNotifyReaderDetach(TRequestStatus& aStatus)
	{
#ifdef EKA2
	SendReceive(EIoPersistentConsoleCancelNotifyReadDetach, TIpcArgs(&aStatus));
#else
	TInt p[4];
	p[0] = (TInt)&aStatus;
	SendReceive(EIoPersistentConsoleCancelNotifyReadDetach, &p[0]);
#endif
	}

EXPORT_C TInt RIoPersistentConsole::AttachWriter(RIoEndPoint& aEndPoint, TCloseBehaviour aCloseBehaviour)
	{
#ifdef EKA2
	return SendReceive(EIoPersistentConsoleAttachWriteEndPoint, TIpcArgs(aEndPoint.SubSessionHandle(), aCloseBehaviour));
#else
	TInt p[4];
	p[0] = aEndPoint.Handle();
	p[1] = aCloseBehaviour;
	return SendReceive(EIoPersistentConsoleAttachWriteEndPoint, &p[0]);
#endif
	}
	
EXPORT_C void RIoPersistentConsole::AttachWriterL(RIoEndPoint& aEndPoint, TCloseBehaviour aCloseBehaviour)
	{
	User::LeaveIfError(AttachWriter(aEndPoint, aCloseBehaviour));
	}
	
EXPORT_C void RIoPersistentConsole::DetachWriter()
	{
#ifdef EKA2
	SendReceive(EIoPersistentConsoleDetachWriteEndPoint, TIpcArgs());
#else
	TInt p[4];
	SendReceive(EIoPersistentConsoleDetachWriteEndPoint, &p[0]);
#endif
	}
	
EXPORT_C void RIoPersistentConsole::NotifyWriterDetach(TRequestStatus& aStatus)
	{
	SendReceive(EIoPersistentConsoleNotifyWriteDetach, aStatus);
	}
	
EXPORT_C void RIoPersistentConsole::CancelNotifyWriterDetach(TRequestStatus& aStatus)
	{
#ifdef EKA2
	SendReceive(EIoPersistentConsoleCancelNotifyWriteDetach, TIpcArgs(&aStatus));
#else
	TInt p[4];
	p[0] = &aStatus;
	SendReceive(EIoPersistentConsoleCancelNotifyWriteDetach, &p[0]);
#endif
	}
	
EXPORT_C TInt RIoPersistentConsole::GetAttachedReaderAndWriterNames(TName& aReaderName, TName& aWriterName)
	{
#ifdef EKA2
	return SendReceive(EIoPersistentConsoleGetAttachedNames, TIpcArgs(&aReaderName, &aWriterName));
#else
	TInt p[4];
	p[0] = &aReaderName;
	p[1] = &aWriterName;
	return SendReceive(EIoPersistentConsoleGetAttachedNames, &p[0]);
#endif
	}
	
EXPORT_C void RIoPersistentConsole::GetAttachedReaderAndWriterNamesL(TName& aReaderName, TName& aWriterName)
	{
	User::LeaveIfError(GetAttachedReaderAndWriterNames(aReaderName, aWriterName));
	}

EXPORT_C TInt RIoPersistentConsole::GetCreator(TThreadId& aThreadId)
	{
	TPckg<TThreadId> theadPckg(aThreadId);
#ifdef EKA2
	return SendReceive(EIoPersistentConsoleGetCreatorThreadId, TIpcArgs(&theadPckg));
#else
	TInt p[4];
	p[0] = &theadPckg;
	return SendReceive(EIoPersistentConsoleGetCreatorThreadId, &p[0]);
#endif
	}
	
EXPORT_C TThreadId RIoPersistentConsole::GetCreatorL()
	{
	TThreadId id;
	User::LeaveIfError(GetCreator(id));
	return id;
	}

//
// RIoFile.
//

EXPORT_C TInt RIoFile::Create(RIoSession& aSession, const TDesC& aFileName, TMode aMode)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreateFile, TIpcArgs(&aFileName, (TInt)aMode));
#else
	TInt p[4];
	p[0] = (TInt)&aFileName;
	p[1] = (TInt)aMode;
	return CreateSubSession(aSession, EIoCreateFile, &p[0]);
#endif
	}
	
EXPORT_C void RIoFile::CreateL(RIoSession& aSession, const TDesC& aFileName, TMode aMode)
	{
	User::LeaveIfError(Create(aSession, aFileName,aMode));
	}


//
// RIoNull.
//

EXPORT_C TInt RIoNull::Create(RIoSession& aSession)
	{
#ifdef EKA2
	return CreateSubSession(aSession, EIoCreateNull);
#else
	return CreateSubSession(aSession, EIoCreateNull, NULL);
#endif
	}

EXPORT_C void RIoNull::CreateL(RIoSession& aSession)
	{
	User::LeaveIfError(Create(aSession));
	}

//
// TIoHandleSet
//

EXPORT_C TIoHandleSet::TIoHandleSet(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr)
	: iIoSession(aIoSession), iStdin(aStdin), iStdout(aStdout), iStderr(aStderr), iSpare(0)
	{
	}

EXPORT_C RIoSession& TIoHandleSet::IoSession() const
	{
	return iIoSession;
	}

EXPORT_C RIoReadHandle& TIoHandleSet::Stdin() const
	{
	return iStdin;
	}

EXPORT_C RIoWriteHandle& TIoHandleSet::Stdout() const
	{
	return iStdout;
	}

EXPORT_C RIoWriteHandle& TIoHandleSet::Stderr() const
	{
	return iStderr;
	}

