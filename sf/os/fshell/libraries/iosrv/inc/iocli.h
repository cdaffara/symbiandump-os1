// iocli.h
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

#ifndef __IOCLI_H__
#define __IOCLI_H__

#include <e32std.h>
#include <e32keys.h>
#include <fshell/consoleextensions.h>

class RIoSession;
class RIoConsole;

class RIoHandle : public RSubSessionBase
	{
public:
	enum TType
		{
		EReadWriteObject,
		EReadObject,
		EWriteObject,
		EEndPoint,
		EConsole,
		EFile,
		ENull,
		EPipe,
		EPersistentConsole,
		};
public:
	IMPORT_C TInt OpenFoundHandle(RIoSession& aSession, TInt aFoundHandle);
	IMPORT_C void OpenFoundHandleL(RIoSession& aSession, TInt aFoundHandle);
	IMPORT_C void Close();
	IMPORT_C TInt IsType(TType aType) const;
	IMPORT_C TBool IsTypeL(TType aType) const;
	IMPORT_C TInt ObjectName(TDes& aName) const;
	IMPORT_C void ObjectNameL(TDes& aName) const;
	IMPORT_C TInt Equals(const RIoHandle& aNother) const;
	IMPORT_C TBool EqualsL(const RIoHandle& aNother) const;
	IMPORT_C RIoSession Session() const;
	};
	
class RIoSession : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C void ConnectL();
	IMPORT_C TInt SetObjectName(TInt aHandle, const TDesC& aName);
	IMPORT_C void SetObjectNameL(TInt aHandle, const TDesC& aName);
	IMPORT_C TInt FindFirstHandle(RIoHandle::TType aType, const TDesC& aMatchString, TInt& aFoundHandle, TName& aName);
	IMPORT_C TInt FindNextHandle(TInt& aFoundHandle, TName& aName);
	};

class RIoReadWriteHandle : public RIoHandle
	{
public:
	enum TMode
		{
		EText,
		EBinary
		};
public:
	IMPORT_C TInt SetMode(TMode aMode);
	IMPORT_C void SetModeL(TMode aMode);
	IMPORT_C TInt SetOwner(TThreadId aOwningThread);
	IMPORT_C void SetOwnerL(TThreadId aOwningThread);
	IMPORT_C TInt SetUnderlyingConsole(RIoConsole& aConsole);
	IMPORT_C void SetUnderlyingConsoleL(RIoConsole& aConsole);
	IMPORT_C TInt AttachedToConsole() const;
	IMPORT_C TBool AttachedToConsoleL() const;
	};

class RIoReadHandle : public RIoReadWriteHandle
	{
public:
	enum TReadMode
		{
		EFull,
		ELine,
		EOneOrMore
		};
	enum TChangeType
		{
		EGainedForeground	= 0x01,
		EConsoleSizeChanged	= 0x02,
		};
public:
	IMPORT_C TInt Create(RIoSession& aSession);
	IMPORT_C void CreateL(RIoSession& aSession);
	IMPORT_C TInt Open(RIoSession& aSession);
	IMPORT_C void OpenL(RIoSession& aSession);
	IMPORT_C TInt Open(RIoSession& aSession, TThreadId aThreadId);
	IMPORT_C void OpenL(RIoSession& aSession, TThreadId aThreadId);
	IMPORT_C TInt Duplicate(RIoReadHandle& aReadHandle);
	IMPORT_C void DuplicateL(RIoReadHandle& aReadHandle);
	IMPORT_C TInt DuplicateHandleFromThread(TThreadId aThreadId);

	IMPORT_C TInt SetReadMode(TReadMode aMode);
	IMPORT_C void SetReadModeL(TReadMode aMode);
	IMPORT_C TInt SetToForeground();
	IMPORT_C void SetToForegroundL();
	IMPORT_C TInt Read(TDes& aDes);
	IMPORT_C void ReadL(TDes& aDes);
	IMPORT_C void Read(TDes& aDes, TRequestStatus& aStatus);
	IMPORT_C TInt SetLineSeparator(const TDesC& aSeparator);
	IMPORT_C void SetLineSeparatorL(const TDesC& aSeparator);
	IMPORT_C void ReadCancel();
	IMPORT_C TInt IsForeground() const;
	IMPORT_C TBool IsForegroundL() const;
	IMPORT_C void NotifyChange(TPckgBuf<TUint>& aChangeType, TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyChange();
	};

class RIoConsoleReadHandle : public RIoReadHandle
	{
public:
	IMPORT_C RIoConsoleReadHandle();
	IMPORT_C RIoConsoleReadHandle(RIoReadHandle& aHandle);
	IMPORT_C RIoConsoleReadHandle operator=(RIoReadHandle& aHandle);
	IMPORT_C TUint ReadKey();
	IMPORT_C void WaitForKey(TRequestStatus& aStatus);
	IMPORT_C void WaitForKeyCancel();
	IMPORT_C TUint KeyCode() const;
	IMPORT_C TUint KeyModifiers() const;
	IMPORT_C TInt CaptureKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers);
	IMPORT_C void CaptureKeyL(TUint aKeyCode, TUint aModifierMask, TUint aModifiers);
	IMPORT_C TInt CancelCaptureKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers);
	IMPORT_C TInt CaptureAllKeys();
	IMPORT_C void CaptureAllKeysL();
	IMPORT_C TInt CancelCaptureAllKeys();
public:
	class TConsoleKey
		{
	public:
		TUint iKeyCode;
		TUint iModifiers;
		};
private:
	TPckgBuf<TConsoleKey> iKeyBuf;
	};

class RIoWriteHandle : public RIoReadWriteHandle
	{
public:
	IMPORT_C TInt Create(RIoSession& aSession);
	IMPORT_C void CreateL(RIoSession& aSession);
	IMPORT_C TInt Open(RIoSession& aSession);
	IMPORT_C void OpenL(RIoSession& aSession);
	IMPORT_C TInt Open(RIoSession& aSession, TThreadId aThreadId);
	IMPORT_C void OpenL(RIoSession& aSession, TThreadId aThreadId);
	IMPORT_C TInt Duplicate(RIoWriteHandle& aWriteHandle);
	IMPORT_C void DuplicateL(RIoWriteHandle& aWriteHandle);
	IMPORT_C TInt DuplicateHandleFromThread(TThreadId aThreadId);
	IMPORT_C TInt Write(const TDesC& aDes);
	IMPORT_C void WriteL(const TDesC& aDes);
	IMPORT_C void Write(const TDesC& aDes, TRequestStatus& aStatus);
	IMPORT_C void WriteCancel();
	};

class RIoConsoleWriteHandle : public RIoWriteHandle
	{
public:
	IMPORT_C RIoConsoleWriteHandle();
	IMPORT_C RIoConsoleWriteHandle(RIoWriteHandle& aHandle);
	IMPORT_C RIoConsoleWriteHandle operator=(RIoWriteHandle& aHandle);
	IMPORT_C TInt GetCursorPos(TPoint& aPos) const;
	IMPORT_C TPoint GetCursorPosL() const;
	IMPORT_C TInt SetCursorPosAbs(const TPoint& aPos);
	IMPORT_C void SetCursorPosAbsL(const TPoint& aPos);
	IMPORT_C TInt SetCursorPosRel(const TPoint& aPos);
	IMPORT_C void SetCursorPosRelL(const TPoint& aPos);
	IMPORT_C TInt SetCursorHeight(TInt aPercentage);
	IMPORT_C void SetCursorHeightL(TInt aPercentage);
	IMPORT_C TInt SetTitle(const TDesC& aTitle);
	IMPORT_C void SetTitleL(const TDesC& aTitle);
	IMPORT_C TInt ClearScreen();
	IMPORT_C void ClearScreenL();
	IMPORT_C TInt ClearToEndOfLine();
	IMPORT_C void ClearToEndOfLineL();
	IMPORT_C TInt GetScreenSize(TSize& aSize) const;
	IMPORT_C TSize GetScreenSizeL() const;
	IMPORT_C TInt SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor = ConsoleAttributes::EUnchanged, ConsoleAttributes::TColor aBackgroundColor = ConsoleAttributes::EUnchanged);
	IMPORT_C void SetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor = ConsoleAttributes::EUnchanged, ConsoleAttributes::TColor aBackgroundColor = ConsoleAttributes::EUnchanged);
	IMPORT_C TInt SetAttributes(const ConsoleAttributes::TAttributes& aAttributes);
	IMPORT_C TInt SetIsStdErr(TBool aFlag);
	};

class RIoEndPoint : public RIoHandle
	{
public:
	enum TReadMode
		{
		EForeground,
		EBackground
		};
public:
	IMPORT_C TInt Attach(RIoReadHandle& aReader, TReadMode aMode);
	IMPORT_C void AttachL(RIoReadHandle& aReader, TReadMode aMode);
	IMPORT_C TInt Attach(RIoWriteHandle& aWriter);
	IMPORT_C void AttachL(RIoWriteHandle& aWriter);
	IMPORT_C TInt SetForegroundReadHandle(RIoReadHandle& aReader);
	IMPORT_C void SetForegroundReadHandleL(RIoReadHandle& aReader);
	IMPORT_C RIoEndPoint();
	};

class RIoPipe : public RIoEndPoint
	{
public:
	IMPORT_C TInt Create(RIoSession& aSession);
	IMPORT_C void CreateL(RIoSession& aSession);
	};

class RIoConsole : public RIoEndPoint
	{
public:
	enum TOptions
		{
		ENormal		= 0,
		ELazyCreate	= 0x01,
		};
public:
	IMPORT_C TInt Create(RIoSession& aSession, const TDesC& aTitle, const TSize& aSize, TUint aConsoleOptions = ENormal);
	IMPORT_C void CreateL(RIoSession& aSession, const TDesC& aTitle, const TSize& aSize, TUint aConsoleOptions = ENormal);
	IMPORT_C TInt Create(RIoSession& aSession, const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, TUint aConsoleOptions = ENormal);
	IMPORT_C TInt Create(RIoSession& aSession, const TDesC& aImplementation, RIoConsole& aUnderlyingConsole, const TDesC& aTitle, const TSize& aSize, TUint aConsoleOptions = ENormal);
	IMPORT_C void CreateL(RIoSession& aSession, const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, TUint aConsoleOptions = ENormal);
	IMPORT_C void CreateL(RIoSession& aSession, const TDesC& aImplementation, RIoConsole& aUnderlyingConsole, const TDesC& aTitle, const TSize& aSize, TUint aConsoleOptions = ENormal);
	IMPORT_C TInt Open(RIoSession& aSession, RIoHandle& aHandle);
	IMPORT_C void OpenL(RIoSession& aSession, RIoHandle& aHandle);
	IMPORT_C TInt Implementation(TDes& aDes) const;
	IMPORT_C void ImplementationL(TDes& aDes) const;
	};
	
class RIoPersistentConsole : public RIoConsole
	{
public:
	enum TCloseBehaviour
		{
		EDetachOnHandleClose,
		EKeepAttachedOnHandleClose,
		};
public:
	IMPORT_C TInt Create(RIoSession& aSession, const TDesC& aName, const TDesC& aTitle);
	IMPORT_C void CreateL(RIoSession& aSession, const TDesC& aName, const TDesC& aTitle);
	
	IMPORT_C TInt OpenByName(RIoSession& aSession, const TDesC& aName);
	IMPORT_C void OpenByNameL(RIoSession& aSession, const TDesC& aName);
	
	IMPORT_C TInt AttachReader(RIoEndPoint& aEndPoint, TCloseBehaviour aDetachOnClose);
	IMPORT_C void AttachReaderL(RIoEndPoint& aEndPoint, TCloseBehaviour aDetachOnClose);
	IMPORT_C void DetachReader();
	IMPORT_C void NotifyReaderDetach(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyReaderDetach(TRequestStatus& aStatus);

	IMPORT_C TInt AttachWriter(RIoEndPoint& aEndPoint, TCloseBehaviour aDetachOnClose);
	IMPORT_C void AttachWriterL(RIoEndPoint& aEndPoint, TCloseBehaviour aDetachOnClose);
	IMPORT_C void DetachWriter();
	IMPORT_C void NotifyWriterDetach(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyWriterDetach(TRequestStatus& aStatus);
	
	IMPORT_C TInt GetAttachedReaderAndWriterNames(TName& aReaderName, TName& aWriterName);
	IMPORT_C void GetAttachedReaderAndWriterNamesL(TName& aReaderName, TName& aWriterName);
	
	IMPORT_C TInt GetCreator(TThreadId& aThreadId);
	IMPORT_C TThreadId GetCreatorL();
	};

class RIoFile : public RIoEndPoint
	{
public:
	enum TMode
		{
		ERead,
		EOverwrite,
		EAppend
		};
public:
	IMPORT_C TInt Create(RIoSession& aSession, const TDesC& aFileName, TMode aMode);
	IMPORT_C void CreateL(RIoSession& aSession, const TDesC& aFileName, TMode aMode);
	};

class RIoNull : public RIoEndPoint
	{
public:
	IMPORT_C TInt Create(RIoSession& aSession);
	IMPORT_C void CreateL(RIoSession& aSession);
	};

// Convenience class to wrap up a set of stdin, stdout, stderr handles
class TIoHandleSet
	{
public:
	IMPORT_C TIoHandleSet(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr);
	IMPORT_C RIoSession& IoSession() const;
	IMPORT_C RIoReadHandle& Stdin() const;
	IMPORT_C RIoWriteHandle& Stdout() const;
	IMPORT_C RIoWriteHandle& Stderr() const;
private:
	RIoSession& iIoSession;
	RIoReadHandle& iStdin;
	RIoWriteHandle& iStdout;
	RIoWriteHandle& iStderr;
	TUint32 iSpare;
	};

#endif
