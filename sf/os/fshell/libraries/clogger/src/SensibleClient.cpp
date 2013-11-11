// SensibleClient.cpp
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

#include "SensibleClient.h"
#include "SensibleClient_internals.h"
#include "cliserv.h"
#include "SensibleCompat.h"

_LIT(KClientPanic, "SensibleClient");
#define DebugPanic() User::Panic(KClientPanic, - __LINE__)
#define AssertPanic(x) User::Panic(KClientPanic, x)
enum TPanic
	{
	EReadOffEndOfCallback, // A call to TCallbackParser::Next when there's nothing more in the callback buffer
	EBadType, // A call to TCallbackParser::NextX where X doesn't match the type of the next thing in the buffer
	};

#ifdef EKA2

static TInt StartServer()
//
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
//
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	RProcess server;
	TInt r=server.Create(KMyServerImg,KNullDesC,serverUid);
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we are expected to return a negative error code from this function so if the server died 
	// with a positive (or zero) code, map it to KErrServerTerminated
	r = (server.ExitType() != EExitPending && stat.Int() >= 0) ? KErrServerTerminated : stat.Int();
	server.Close();
	return r;
	}



#else

#include <e32math.h>

inline TServerStart::TServerStart(TRequestStatus& aStatus)
	:iId(RThread().Id()),iStatus(&aStatus)
	{aStatus=KRequestPending;}
inline TPtrC TServerStart::AsCommand() const
	{return TPtrC(reinterpret_cast<const TText*>(this),sizeof(TServerStart)/sizeof(TText));}

static TInt StartServer()
//
// Start the server process/thread which lives in an EPOCEXE object
//
	{
	TRequestStatus started;
	TServerStart start(started);

	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

#ifdef __WINS__
	//
	// In WINS the server is a DLL, the exported entrypoint returns a TInt
	// which represents the real entry-point for the server thread
	//
	RLibrary lib;
	TInt r=lib.Load(KMyServerImg,serverUid);
	if (r!=KErrNone)
		return r;
	TLibraryFunction ordinal1=lib.Lookup(1);
	TThreadFunction serverFunc=reinterpret_cast<TThreadFunction>(ordinal1());
	//
	// To deal with the unique thread (+semaphore!) naming in EPOC, and that we may
	// be trying to restart a server that has just exited we attempt to create a
	// unique thread name for the server.
	// This uses Math::Random() to generate a 32-bit random number for the name
	//
	TName name(KMyServerName);
	name.AppendNum(Math::Random(),EHex);
	RThread server;
	r=server.Create(name,serverFunc,
					KMyServerStackSize,
					&start,&lib,NULL,
					KMyServerInitHeapSize,KMyServerMaxHeapSize,EOwnerProcess);
	lib.Close();	// if successful, server thread has handle to library now
#else
	//
	// EPOC is easy, we just create a new server process. Simultaneous launching
	// of two such processes should be detected when the second one attempts to
	// create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KMyServerImg,start.AsCommand(),serverUid);
#endif
	if (r!=KErrNone)
		return r;
	TRequestStatus died;
	server.Logon(died);
	if (died!=KRequestPending)
		{
		// logon failed - server is not yet running, so cannot have terminated
		User::WaitForRequest(died);	// eat signal
		server.Kill(0);				// abort startup
		server.Close();
		return died.Int();
		}
	//
	// logon OK - start the server
	server.Resume();
	User::WaitForRequest(started,died);		// wait for start or death
	if (started==KRequestPending)
		{
		// server has died, never made it to the startup signal
		server.Close();
		return died.Int();
		}
	//
	// server started (at last). Cancel and consume the death-notification
	// before reporting success
	server.LogonCancel(died);
	server.Close();
	User::WaitForRequest(died);		// eat the signal (from the cancel)
	return KErrNone;
	}

TInt E32Dll(TDllReason)
	{
	return 0;
	}


#endif

RSensibleSessionBody::RSensibleSessionBody()
	: iDispatcher(NULL)
	{}

TInt RSensibleSessionBody::Connect(TInt aMessageSlots, TBool aStartCallbackDispatcher)
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt r = KErrNone;
	TInt retry=2;
	for (;;)
		{
		r=DoCreateSession(KMyServerName, TVersion(0,0,0), aMessageSlots);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			break;
		if (--retry==0)
			break;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			break;
		}
	if (r == KErrNone && aStartCallbackDispatcher)
		{
		if (StartCallbackDispatcher() != KErrNone)
			{
			Close();
			return KErrNoMemory;
			}
		}
	return r;
	}

TInt RSensibleSessionBody::StartCallbackDispatcher()
	{
	if (!iDispatcher)
		{
		iDispatcher = new CServerCallbackDispatcher(*this);
		}
	else if (!iDispatcher->IsActive())
		{
		iDispatcher->Register();
		}
	return iDispatcher == NULL ? KErrNoMemory : KErrNone;
	}

void RSensibleSessionBody::StopCallbackDispatcher()
	{
	if (iDispatcher) iDispatcher->Cancel();
	}

void RSensibleSessionBody::DispatchCallbackL(TServerCallback& /*aCallback*/, TCallbackParser& /*aParser*/)
	{
	// Implemented by subclass
	}

void RSensibleSessionBody::ServerDiedL()
	{
	// Implemented by subclass
	}

void RSensibleSessionBody::Close()
	{
	delete iDispatcher;
	iDispatcher = NULL;
	}

// Not happy this actually works or is worth it
/*
void CCleanupAndComplete::RunL()
	{
	if (iClientStatus)
		{
		// We use this class with a null pointer to allow non-asynchronous (ie blind) APIs to still have the ability to cleanup resource when they are completed
		User::RequestComplete(iClientStatus, iStatus.Int());
		}
	delete this;
	}

void CCleanupAndComplete::DoCancel()
	{
	//TODO what? Probably delete this in some way
	//delete this;
	}

CCleanupAndComplete::CCleanupAndComplete(TRequestStatus* aClientStatus)
	: CActive(EPriorityStandard), iClientStatus(aClientStatus)
	{
	CActiveScheduler::Add(this);
	if (iClientStatus)
		{
		*iClientStatus = KRequestPending;
		}
	}
	
CCleanupAndComplete* CCleanupAndComplete::NewLC(TRequestStatus* aClientStatus)
	{
	CCleanupAndComplete* self = new(ELeave) CCleanupAndComplete(aClientStatus);
	CleanupStack::PushL(self);
	return self;
	}
		
TInt CCleanupAndComplete::Create(TRequestStatus*& aStatus, const TDesC8& aDes, TInt& aIpcArg)
	{
	CCleanupAndComplete* self = new CCleanupAndComplete(aStatus);
	TInt err = KErrNone;
	if (!self) err = KErrNoMemory;
	HBufC8* buf = NULL;
	if (!err)
		{
		buf = aDes.Alloc();
		}
	if (!buf) err = KErrNoMemory;
	if (!err)
		{
		err = self->iWithoutDestructor.Append(buf);
		}
	if (err) 
		{
		delete buf;
		delete self;
		return KErrNoMemory;
		}
	aIpcArg = (TInt)buf;
	aStatus = &self->iStatus;
	self->SetActive();
	return KErrNone;
	}

TInt CCleanupAndComplete::Create(TRequestStatus*& aStatus, const TDesC16& aDes, TInt& aIpcArg)
	{
	CCleanupAndComplete* self = new CCleanupAndComplete(aStatus);
	TInt err = KErrNone;
	if (!self) err = KErrNoMemory;
	HBufC16* buf = NULL;
	if (!err)
		{
		buf = aDes.Alloc();
		}
	if (!buf) err = KErrNoMemory;
	if (!err)
		{
		err = self->iWithoutDestructor.Append(buf);
		}
	if (err) 
		{
		delete buf;
		delete self;
		return KErrNoMemory;
		}
	aIpcArg = (TInt)buf;
	aStatus = &self->iStatus;
	self->SetActive();
	return KErrNone;
	}

void CCleanupAndComplete::AddL(CBase* aWithDestructor)
	{
	User::LeaveIfError(iWithDestructor.Append(aWithDestructor));
	}

void CCleanupAndComplete::AddL(TAny* aWithoutDestructor)
	{
	User::LeaveIfError(iWithoutDestructor.Append(aWithoutDestructor));
	}

CCleanupAndComplete::~CCleanupAndComplete()
	{
	Cancel();
	iWithDestructor.ResetAndDestroy();
	for (TInt i = 0; i < iWithoutDestructor.Count(); i++)
		{
		delete iWithoutDestructor[i];
		}
	}

void CCleanupAndComplete::SetActive()
	{
	CActive::SetActive();
	}

*/

CServerCallbackDispatcher::CServerCallbackDispatcher(RSensibleSessionBody& aSession)
	: CActive(EPriorityStandard), iSession(aSession), iContextPtr(NULL, 0)
	{
	CActiveScheduler::Add(this);
	Register();
	}
	
void CServerCallbackDispatcher::Register()
	{
	iState = EWaitingForCallback;
	IPC(args, &iNextCallback, 0,0,0);
	iSession.DoSendReceive(ERegisterCallbackNotifier, args, iStatus);
	SetActive();
	}

void CServerCallbackDispatcher::RunL()
	{
	DISOWN(iCachedCallbackResult8);
	DISOWN(iCachedCallbackResult16);

	if (iStatus == KErrServerTerminated)
		{
		iSession.ServerDiedL();
		return;
		}
	else if (iStatus != KErrNone)
		{
		//TODO Do something...
		__DEBUGGER();
		return;
		}

	TServerCallback cb = iNextCallback();
	TCallbackParser p(cb);

	if (iState == EWaitingForCallback && cb.iContextLength != 0)
		{
		iState = EWaitingForContext;
		DISOWN(iContext);
		iContext = HBufC8::NewL(cb.iContextLength);
		iContextPtr = iContext->Des();
		IPC(args, &iContextPtr, 0,0,0);
		iSession.DoSendReceive(EGetCallbackContext, args, iStatus);
		SetActive();
		return;
		}

	if (iState == EWaitingForContext)
		{
		p.SetContext(*iContext);
		}

	Register();
	iSession.DispatchCallbackL(cb, p);
	}

void CServerCallbackDispatcher::DoCancel()
	{
	IPC(args, 0,0,0,0);
	iSession.DoSendReceive(ECancelCallbackNotifier, args);
	}

CServerCallbackDispatcher::~CServerCallbackDispatcher()
	{
	Cancel();
	DISOWN(iCachedCallbackResult8);
	DISOWN(iCachedCallbackResult16);
	DISOWN(iContext);
	}


TCallbackParser::TCallbackParser(const TServerCallback& aCallback)
	: iCallback(aCallback), iContext(NULL), iNextPtr(aCallback.iData.Ptr()), iInContext(EFalse)
	{}

void TCallbackParser::SetContext(TDesC8& aContext)
	{
	iContext = &aContext;
	}

#define DOGET(T, type, name) T name; TAny* res = Next(sizeof(T), #type); Mem::Copy(&name, res, sizeof(T));
#define GET(T, type) DOGET(T, type, __result); return __result;

TInt TCallbackParser::NextInt()
	{
	GET(TInt, i);
	}

TUint TCallbackParser::NextUint()
	{
	GET(TUint, u);
	}

TPoint TCallbackParser::NextPoint()
	{
	GET(TPoint, P);
	}

TSize TCallbackParser::NextSize()
	{
	GET(TSize, S);
	}

TRgb TCallbackParser::NextRgb()
	{
	GET(TRgb, G);
	}

TRect TCallbackParser::NextRect()
	{
	GET(TRect, R);
	}

TPtrC TCallbackParser::NextDesC()
	{
	DOGET(TInt, i, len);
	const TUint16* ptr = reinterpret_cast<const TUint16*>(Next(len*2, "D"));
	if (TUint(ptr)&1)
		{
		// Need to allow for padding
		ptr = (TUint16*)(((TUint8*)ptr)+1);
		iNextPtr++; // And also move iNextPtr to the right place for the next read
		}
	return TPtrC(ptr, len);
	}

TPtrC8 TCallbackParser::NextDesC8()
	{
	DOGET(TInt, i, len);
	const TUint8* ptr = reinterpret_cast<const TUint8*>(Next(len, "8"));
	return TPtrC8(ptr, len);
	}

void* TCallbackParser::Next(TInt aSize, char* aType)
	{
	const TUint8* endPtr = iInContext ? iContext->Ptr() + iContext->Size() : iCallback.iData.Ptr() + iCallback.iData.Size();

	if (iNextPtr + aSize + 1 > endPtr)
		{
		// No room for arg and type
		if (!iInContext && iContext)
			{
			// try moving to context
			iNextPtr = (TUint8*)iContext->Ptr();
			iInContext = ETrue;
			}
		else
			{
			// Either there's no context, or we're already in it and reading off the end
			AssertPanic(EReadOffEndOfCallback);
			}
		}

	TUint8 nextType = *iNextPtr; //iInContext ? iCallback.iContextTypes[iIdx] : iCallback.iDataTypes[iIdx];
	iNextPtr++;
	void* result = (void*)iNextPtr;
	
	__ASSERT_ALWAYS(nextType == *aType, AssertPanic(EBadType)); // Types must match

	iNextPtr = iNextPtr + aSize;
	return result;
	}
