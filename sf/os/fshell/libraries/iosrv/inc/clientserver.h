// clientserver.h
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

#ifndef __CLIENTSERVER_H__
#define __CLIENTSERVER_H__

#include <e32std.h>
#include <fshell/common.mmh>

_LIT(KIoServerName, "iosrv");

const TUid KServerUid3 = {FSHELL_UID_IOSRV};

#ifdef __WINS__
const TInt KIoStackSize = 0x2000;		// 8KB
const TInt KIoInitHeapSize = 0x1000;	// 4KB
const TInt KIoMaxHeapSize = 0x200000;	// 2MB
#endif

enum TIoOpcodes
	{
	EIoCreateReader,
	EIoHandleClose,
	EIoHandleSetOwner,
	EIoHandleSetUnderlyingConsole,
	EIoHandleAttachedToConsole,
	EIoOpenReaderByThreadId,
	EIoOpenReaderByExplicitThreadId,
	EIoDuplicateReader,
	EIoCreateWriter,
	EIoOpenWriterByThreadId,
	EIoOpenWriterByExplicitThreadId,
	EIoDuplicateWriter,
	EIoSetReadWriteMode,
	EIoSetReadMode,
	EIoSetReaderToForeground,
	EIoRead,
	EIoSetLineSeparator,
	EIoWrite,
	EIoReadCancel,
	EIoWriteCancel,
	EIoIsForegroundReader,
	EIoConsoleWaitForKey,
	EIoConsoleWaitForKeyCancel,
	EIoConsoleCaptureKey,
	EIoConsoleCancelCaptureKey,
	EIoConsoleCaptureAllKeys,
	EIoConsoleCancelCaptureAllKeys,
	EIoConsoleCursorPos,
	EIoConsoleSetCursorPosAbs,
	EIoConsoleSetCursorPosRel,
	EIoConsoleSetCursorHeight,
	EIoConsoleSetTitle,
	EIoConsoleClearScreen,
	EIoConsoleClearToEndOfLine,
	EIoConsoleScreenSize,
	EIoConsoleSetAttributes,
	EIoEndPointAttachReader,
	EIoEndPointAttachWriter,
	EIoEndPointSetForegroundReadHandle,
	EIoCreatePipe,
	EIoCreateConsole,
	EIoOpenConsole,
	EIoConsoleImplementation,
	EIoCreateFile,
	EIoCreateNull,
	EIoSetObjectName,
	EIoCreatePersistentConsole,
	EIoOpenPersistentConsoleByName,
	EIoPersistentConsoleAttachReadEndPoint,
	EIoPersistentConsoleAttachWriteEndPoint,
	EIoPersistentConsoleDetachReadEndPoint,
	EIoPersistentConsoleDetachWriteEndPoint,
	EIoPersistentConsoleNotifyReadDetach,
	EIoPersistentConsoleNotifyWriteDetach,
	EIoPersistentConsoleCancelNotifyReadDetach,
	EIoPersistentConsoleCancelNotifyWriteDetach,
	EIoPersistentConsoleGetAttachedNames,
	EIoPersistentConsoleGetCreatorThreadId,
	EIoHandleIsType,
	EIoFindFirstHandle,
	EIoFindNextHandle,
	EIoOpenFoundHandle,
	EIoHandleGetName,
	EIoHandleEquals,
	EIoReadHandleNotifyChange,
	EIoReadHandleCancelNotifyChange,
	EIoDuplicateReaderHandleFromThread,
	EIoDuplicateWriterHandleFromThread,
	EIoSetIsStdErr,
	};

class TServerStart
	{
public:
	TServerStart();
	TServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;
	TInt GetCommand();
	void SignalL();
private:
	TThreadId iId;
	TRequestStatus* iStatus;
	};

class TConsoleCreateParams
	{
public:
	TSize iSize;
	TInt iUnderlyingConsoleHandle;
	TUint iOptions;
	};

#endif // __CLIENTSERVER_H__
