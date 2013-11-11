// ProxyServer.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include "ProxyServer.h"
#include <fshell/memoryaccess.h>

#define LOG(x...)
#define LOG8(x...)
//#include <e32debug.h>
//#define LOG(x...) RDebug::Print(x)
//#define LOG8(x...) RDebug::Printf(x)

//const TInt KShutdownProxy = 1000;
//const TInt KPingProxy = 1001;

NONSHARABLE_CLASS(CAsyncWaiter) : public CActive
	{
public:
	CAsyncWaiter(RUnderlyingSession& aRealSession, const RMessage2& aOriginalMessage, const TIpcArgs& aArgs);
	~CAsyncWaiter();
	//void ForwardMessageL();

protected:
	void RunL();
	void DoCancel();

private:
	RUnderlyingSession& iRealSession;
	RMessage2 iMsg;
	TIpcArgs iArgs;
	};

CProxyServer* CProxyServer::NewInSeparateThreadL(const TDesC& aServerToReplace, MMessageHandler* aHandler)
	{
	CProxyServer* self = new(ELeave) CProxyServer(aServerToReplace, aHandler);
	CleanupStack::PushL(self);
	self->ThreadConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CProxyServer::CProxyServer(const TDesC& aServerToReplace, MMessageHandler* aHandler)
	: CServer2(0, ESharableSessions), iServerName(aServerToReplace), iHandler(aHandler)
	{
	// Name the server after tid and this pointer, should be unique enough
	iRealServerName.AppendNum(RThread().Id(), EHex);
	iRealServerName.Append('.');
	iRealServerName.AppendNum((TInt)this, EHex);
	}

void CProxyServer::ThreadConstructL()
	{
	User::LeaveIfError(iServerThread.Create(iRealServerName, &StartServerThreadFunction, 8192, NULL, this));
	TRequestStatus stat;
	iServerThread.Rendezvous(stat);
	if (stat == KRequestPending)
		{
		iServerThread.Resume();
		}
	else
		{
		iServerThread.Kill(stat.Int());
		}
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	}

void CProxyServer::ConstructL()
	{
	iShutdownCallback = new(ELeave) CAsyncCallBack(CActive::EPriorityHigh);
	iShutdownCallback->Set(TCallBack(&Shutdown, this));

	User::LeaveIfError(iMemAccess.Open());
	if (iRealServerName.Length() > iServerName.Length()) User::Leave(KErrTooBig); // Mem access doesn't like this

	_LIT(KTempName, "TemporaryReallyLongServerNameThatLeavesUsSpaceToManuever");
	StartL(KTempName);

	TServerKernelInfoBuf buf;
	TInt err = iMemAccess.GetObjectInfo(EServer, iServerName, buf);
	User::LeaveIfError(err);
	TUint8* realServer = buf().iAddressOfKernelObject;
	err = iMemAccess.GetObjectInfo(EServer, KTempName, buf);
	User::LeaveIfError(err);
	TUint8* myServer = buf().iAddressOfKernelObject;

	// Should really have some kind of swap operation here...
	TBuf8<KMaxName> name8;
	name8.Copy(iRealServerName);
	iMemAccess.InPlaceObjectRename(EServer, realServer, name8);
	name8.Copy(iServerName);
	iMemAccess.InPlaceObjectRename(EServer, myServer, name8);
	iProxying = ETrue;
	}

TInt CProxyServer::Shutdown(TAny* aSelf)
	{
	CProxyServer* self = static_cast<CProxyServer*>(aSelf);
	delete self;
	CActiveScheduler::Stop();
	return 0;
	}

void CProxyServer::Destroy()
	{
	if (iShutdownCallback && iServerThread.Handle() && iServerThread.Id() != RThread().Id()) // Don't try shutting down the thread if the server is actually running in the main thread
		{
		TRequestStatus stat;
		// This is far harder than it should be... need to make sure we don't reference any member vars after calling the shutdown callback
		RThread serverThread;
		Mem::Swap(&serverThread, &iServerThread, sizeof(RThread));
		serverThread.Logon(stat);
		iShutdownCallback->CallBack();
		User::WaitForRequest(stat);
		serverThread.Close();
		}
	else
		{
		delete this;
		}
	}

CProxyServer::~CProxyServer()
	{
	if (iProxying)
		{
		TServerKernelInfoBuf buf;
		TInt err = iMemAccess.GetObjectInfo(EServer, iRealServerName, buf);
		if (!err)
			{
			TUint8* realServer = buf().iAddressOfKernelObject;
			TBuf8<KMaxName> name8;
			name8.Copy(iServerName);
			iMemAccess.InPlaceObjectRename(EServer, realServer, name8);
			// Temporarily both us and the real server will have name iServerName. When we finish destructing, we'll clean up and the system should be back in a sensible state
			}
		}
	delete iShutdownCallback;
	iServerThread.Close();
	iMemAccess.Close();
	}

CSession2* CProxyServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	CProxySession* res = new(ELeave) CProxySession();
	CleanupStack::PushL(res);
	res->ConstructL(iRealServerName, aVersion);
	CleanupStack::Pop(res);
	return res;
	}


void CProxyServer::ServerThreadRunL()
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	ConstructL();
	RThread::Rendezvous(KErrNone);

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(scheduler);
	}

TInt CProxyServer::StartServerThreadFunction(TAny* aSelf)
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		TRAP(err, static_cast<CProxyServer*>(aSelf)->ServerThreadRunL());
		delete cleanup;
		}
	return err;
	}

MMessageHandler* CProxyServer::Handler() const
	{
	return iHandler;
	}

//

void CProxySession::ConstructL(const TDesC& aServerName, const TVersion& aVersion)
	{
	User::LeaveIfError(iSession.CreateSession(aServerName, aVersion));
	}

CProxyServer& CProxySession::Server()
	{
	return *const_cast<CProxyServer*>(static_cast<const CProxyServer*>(CSession2::Server()));
	}

const CProxyServer& CProxySession::Server() const
	{
	return *static_cast<const CProxyServer*>(CSession2::Server());
	}

void CProxySession::ServiceL(const RMessage2 &aMessage)
	{
	MMessageHandler* handler = Server().Handler();
	TBool handled = EFalse;
	if (handler)
		{
		handled = handler->HandleMessageL(this, aMessage);
		}

	if (!handled)
		{
		ForwardUnhandledMessageL(aMessage);
		}
	}

TInt Val(const RMessage2& aMessage, TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		return aMessage.Int0();
	case 1:
		return aMessage.Int1();
	case 2:
		return aMessage.Int2();
	case 3:
		return aMessage.Int3();
	default:
		return 0;
		}
	}

void CleanupArgs(TAny* aArgs)
	{
	TIpcArgs& args = *static_cast<TIpcArgs*>(aArgs);
	for (TInt i = 0; i < KMaxMessageArguments; i++)
		{
		if (args.iFlags & (TIpcArgs::EFlagDes<<(i*TIpcArgs::KBitsPerType)))
			{
			LOG8("+++ deleting %x", args.iArgs[i]);
			delete (TAny*)args.iArgs[i];
			}
		}
	args.iFlags = 0;
	}

void CProxySession::ForwardUnhandledMessageL(const RMessage2& aMessage)
	{
	TIpcArgs args;
	CleanupStack::PushL(TCleanupItem(&CleanupArgs, &args));
	
	LOG(_L("Forwarding message fn=%d"), aMessage.Function());

	for (TInt i = 0; i < KMaxMessageArguments; i++)
		{
		// For each arg, try and figure out if it's a descriptor and if so what type
		TInt len = aMessage.GetDesLength(i);
		if (len < 0)
			{
			// Not a descriptor
			args.Set(i, Val(aMessage, i));
			LOG(_L("Int arg %d: %d"), i, args.iArgs[i]);
			}
		else
			{
			// It's a descriptor, now see if it's 16-bit or 8-bit
			TBuf<1> wbuf;
			TInt err = aMessage.Read(i, wbuf); // This check only works if platsec enforcement is turned on (!) but fshell.bat was the only thing that generally ran with enforcement off, and I've fixed that
			if (err == KErrNone)
				{
				// 16-bit
				// Gaah where's HBuf when you need it
				TPtr* buf = (TPtr*)User::AllocL(len*2 + sizeof(TPtr));
				new(buf) TPtr((TUint16*)(buf+1), len);
				aMessage.ReadL(i, *buf);

				// Figure out if it's writeable by trying to write back what we just read
				TBool writeable = (aMessage.Write(i, *buf) == KErrNone);

				if (writeable)
					{
					LOG(_L("TDes16 arg %d: %S"), i, buf);
					args.Set(i, buf);
					}
				else
					{
					LOG(_L("TDesC16 arg %d: %S"), i, buf);
					args.Set(i, (const TDesC16*)buf);
					}
				}
			else
				{
				// 8-bit
				// Gaah where's HBuf when you need it
				TPtr8* buf = (TPtr8*)User::AllocL(len + sizeof(TPtr8));
				new(buf) TPtr8((TUint8*)(buf+1), len);
				aMessage.ReadL(i, *buf);

				// Figure out if it's writeable by trying to write back what we just read
				TBool writeable = (aMessage.Write(i, *buf) == KErrNone);

				if (writeable)
					{
					LOG8("TDes8 arg %d: %S", i, buf);
					args.Set(i, buf);
					}
				else
					{
					LOG8("TDesC8 arg %d: %S", i, buf);
					args.Set(i, (const TDesC8*)buf);
					}
				}
			}
		}
	ForwardMessageArgsL(aMessage, args);
	CleanupStack::Pop(&args); // ForwardMessageArgs takes ownership
	}

void CProxySession::ForwardMessageArgsL(const RMessage2& aMessage, const TIpcArgs& aArgs)
	{
	CAsyncWaiter* waiter = new(ELeave) CAsyncWaiter(iSession, aMessage, aArgs);
	// That's all that's needed
	}

void CProxySession::Disconnect(const RMessage2 &aMessage)
	{
	iSession.Close();
	//TODO Need to wait for any CAsyncWaiters to be completed? If so, do it here and defer the super call until that time
	CSession2::Disconnect(aMessage);
	}

// CAsyncWaiter

CAsyncWaiter::CAsyncWaiter(RUnderlyingSession& aRealSession, const RMessage2& aOriginalMessage, const TIpcArgs& aArgs)
	: CActive(CActive::EPriorityStandard), iRealSession(aRealSession), iMsg(aOriginalMessage), iArgs(aArgs)
	{
	CActiveScheduler::Add(this);
	LOG(_L("Sending to real server: fn=%d, args=%x,%x,%x,%x flags=%x"), aOriginalMessage.Function(), iArgs.iArgs[0], iArgs.iArgs[1], iArgs.iArgs[2], iArgs.iArgs[3], iArgs.iFlags);
	iRealSession.SendReceive(aOriginalMessage.Function(), iArgs, iStatus);
	SetActive();
	}

CAsyncWaiter::~CAsyncWaiter()
	{
	CleanupArgs(&iArgs);
	}

void CAsyncWaiter::RunL()
	{
	const TInt serverErr = iStatus.Int();
	TInt writeErr = KErrNone;

	// First check for any TDes args that we need to write back to the real client
	for (TInt i = 0; i < KMaxMessageArguments; i++)
		{
		if ((iArgs.iFlags & (TIpcArgs::EFlagDes<<(i*TIpcArgs::KBitsPerType))) && !(iArgs.iFlags & (TIpcArgs::EFlagConst<<(i*TIpcArgs::KBitsPerType))))
			{
			if (iArgs.iFlags & (TIpcArgs::EFlag16Bit<<(i*TIpcArgs::KBitsPerType)))
				{
				TDes16* des = (TDes16*)iArgs.iArgs[i];
				writeErr = iMsg.Write(i, *des);
				LOG(_L("Writing back %S returned %d"), des, writeErr);
				}
			else
				{
				TDes8* des = (TDes8*)iArgs.iArgs[i];
				writeErr = iMsg.Write(i, *des);
				LOG8("Writing back %S returned %d", des, writeErr);
				}

			if (writeErr) break; // Who knows how the real server would have handled such a case
			}
		}

	LOG(_L("Completing original request function %d with writeErr=%d serverErr=%d"), iMsg.Function(), writeErr, serverErr);
	if (writeErr) iMsg.Complete(writeErr);
	else iMsg.Complete(serverErr);

	delete this; // Our work here is done
	}

void CAsyncWaiter::DoCancel()
	{
	// We never call Cancel on our waiters
	}

/*
EXPORT_C TInt ShutdownProxyNotifier()
	{
	RDebugNotifier notifier;
	TInt err = notifier.Connect();
	if (err == KErrNotFound)
		{
		// Oh dear, !Notifier isn't running. Meaning we renamed it then crashed, probably. Try renaming the real one
		RMemoryAccess::LoadDriver();
		RMemoryAccess mem;
		err = mem.Open();
		if (err) return err;
		TServerKernelInfoBuf buf;
		TInt err = mem.GetObjectInfo(EServer, KRealNotifierServerName, buf);
		if (!err)
			{
			TUint8* realServer = buf().iAddressOfKernelObject;
			mem.InPlaceObjectRename(EServer, realServer, _L8("!Notifier"));
			}
		mem.Close();
		return err;
		}

	err = notifier.ShutdownProxy();
	if (err == KErrServerTerminated) err = KErrNone; // It's expected to get KErrServerTerminated, because we deliberately don't complete the message. This way the client is more likely to get the completion once the server has actually gone, and not slightly before
	notifier.Close();

	return err;
	}

EXPORT_C TBool NotifierProxyIsRunning()
	{
	RDebugNotifier notifier;
	TInt err = notifier.Connect();
	if (err) return EFalse; // Not even original notifier is running?!
	err = notifier.PingProxy();
	notifier.Close();
	return err == KErrNone; // The real proxy will return KErrNotSupported in this scenario
	}
*/
