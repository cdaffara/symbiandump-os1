// server.h
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

#ifndef __SERVER_H__
#define __SERVER_H__

#ifdef EKA2
#define RMsg RMessage2
#else
#define RMsg RMessage
#endif

#include <e32base.h>
#include <f32file.h>
#include <e32hashtab.h>
#include "iocli.h"
#include "clientserver.h"
#include "log.h"
#include "config.h"

enum TIoPanicReason
	{
	EPanicBadDescriptor = 0,
	EPanicBadHandle = 1,
	EPanicUnknownOpcode = 2,
	EPanicReadAlreadyPending = 3,
	EPanicReadKeyAlreadyPending = 4,
	EPanicReadWhenNotAttached = 5,
	EPanicWriteAlreadyPending = 6,
	EPanicWriteWhenNotAttached = 7,
	EPanicReadKeyWhenNotAttached = 8,
	EPanicCursorPosWhenNotAttached = 9,
	EPanicSetCursorPosAbsWhenNotAttached = 10,
	EPanicSetCursorPosRelWhenNotAttached = 11,
	EPanicSetCursorHeightWhenNotAttached = 12,
	EPanicSetTitleWhenNotAttached = 13,
	EPanicClearScreenWhenNotAttached = 14,
	EPanicClearToEndOfLineWhenNotAttached = 15,
	EPanicScreenSizeWhenNotAttached = 16,
	EPanicInvalidPipeId = 17,
	EPanicSetConsoleModeWhenNotAttached = 18,
	EPanicNotAnEndPoint = 19,
	EPanicNotAPipe = 20,
	EPanicNotAConsole = 21,
	EPanicNotAReadWriteObject = 22,
	EPanicNotAReadObject = 23,
	EPanicNotAWriteObject = 24,
	EPanicNotAPersistentConsole = 25,
	EPanicNotAValidContainerType = 26,
	EPanicFindNextWithNoFindFirst = 27,
	EPanicOpenFoundWithNoFind = 28,
	EPanicSetTitleAlreadyPending = 29,
	EPanicCursorPosAlreadyPending = 30,
	EPanicSetCursorPosAlreadyPending = 31,
	EPanicSetCursorHeightAlreadyPending = 32,
	EPanicClearScreenAlreadyPending = 33,
	EPanicClearToEndOfLineAlreadyPending = 34, 
	EPanicScreenSizeAlreadyPending = 35,
	EPanicSetReadModeAlreadyPending = 36,
	EPanicCannotReadFromUnderlyingConsole = 37,
	EPanicSetAttributesWhenNotAttached = 38,
	EPanicSetAttributesAlreadyPending = 39,
	EPanicSetModeAlreadyPending = 40
	};

void PanicClient(const RMsg& aMessage, TIoPanicReason aReason);

class CIoObject;
class CIoLog;
class CIoPipe;
class CIoConsole;
class CIoFile;
class CIoNull;
class CIoPersistentConsole;
class CIoReadWriteObject;
class CIoReadObject;
class CIoWriteObject;
class MIoWriteEndPoint;

class CShutdownTimer : public CTimer
	{
	enum {KShutdownDelay = 0x2000000};
public:
	CShutdownTimer();
	void ConstructL();
	void Start();
private:
	void RunL();
	};


#ifdef EKA2
class CIoServer : public CServer2
#else
class CIoServer : public CServer
#endif
	{
public:
#ifdef EKA2
	static CServer2* NewLC();
#else
	static CServer* NewLC();
#endif
	~CIoServer();
	void AddSession();
	void DropSession();
	CIoPipe& CreatePipeLC();
	CIoConsole& CreateConsoleLC(const TDesC& aImplementation, const TDesC& aTitle, TSize aSize, MIoWriteEndPoint* aUnderlyingConsole, TUint aOptions);
	CIoFile& CreateFileLC(const TDesC& aName, RIoFile::TMode aMode);
	CIoNull& CreateNullLC();
	CIoPersistentConsole& CreatePersistentConsoleLC(const TDesC& aName, const TDesC& aTitle, const RMsg& aMessage);
	CIoReadObject& CreateReadObjLC();
	CIoWriteObject& CreateWriteObjLC();
	CIoReadObject* NextReadObj(TThreadId aOwningThread) const;
	CIoWriteObject* NextWriteObj(TThreadId aOwningThread) const;
	CIoReadObject* LastOpenedReadObj(TThreadId aOwningThread) const;
	CIoWriteObject* LastOpenedWriteObj(TThreadId aOwningThread) const;
	CIoPersistentConsole& FindPersistentConsoleL(const TDesC& aName);
	CIoObject* FindObjectByName(RIoHandle::TType aType, TInt& aFindHandle, const TDesC& aMatch, TName& aName) const;
	CIoObject& OpenObjectLC(TInt aFindHandle);
	const TIoConfig& Config();
	void PersistentConsoleAddL(const TDesC16& aName, const CIoPersistentConsole& aCons);
	void PersistentConsoleRemove(const TDesC16& aName, const CIoPersistentConsole& aCons);
private:
	CIoServer();
	void ConstructL();
#ifdef EKA2
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
#else
	virtual CSharableSession* NewSessionL(const TVersion& aVersion) const;
#endif
	TInt RunError(TInt aError);
	CIoReadWriteObject* DoFindObj(RIoHandle::TType aType, TThreadId aOwningThread, TBool aNext) const;
private:
	RFs iFs;
	TInt iSessionCount;
	CShutdownTimer iShutdownTimer;
	CObjectConIx* iIoObjectContainerIndex; 
	CObjectCon* iIoObjectContainer;
	TUint iNextReadObjId;
	TUint iNextWriteObjId;
	TIoConfig iConfig;
	RPtrHashMap<TDesC16, CIoPersistentConsole> iPersistentConsoleNames;
#ifdef IOSRV_LOGGING
	CIoLog* iLog;
#endif
	};

	
TInt DesLengthL(const RMsg& aMessage, TInt aParam);
TInt MaxDesLengthL(const RMsg& aMessage, TInt aParam);
void MessageReadL(const RMsg& aMessage, TInt aParam, TDes8& aDes);
void MessageReadL(const RMsg& aMessage, TInt aParam, TDes8& aDes, TInt aOffset);
void MessageReadL(const RMsg& aMessage, TInt aParam, TDes16& aDes);
void MessageReadL(const RMsg& aMessage, TInt aParam, TDes16& aDes, TInt aOffset);
void MessageWriteL(const RMsg& aMessage, TInt aParam, const TDesC8& aDes);
void MessageWriteL(const RMsg& aMessage, TInt aParam, const TDesC16& aDes);
TInt MessageWrite(const RMsg& aMessage, TInt aParam, const TDesC8& aDes);
TInt MessageWrite(const RMsg& aMessage, TInt aParam, const TDesC16& aDes);
TBool MessagePending(const RMsg& aMessage);
TThreadId ClientThreadIdL(const RMsg& aMessage);
TFullName ClientNameL(const RMsg& aMessage);
void Complete(const RMsg& aMessage, TInt aError);
void CompleteIfPending(const RMsg& aMessage, TInt aError);

#endif // __SERVER_H__

