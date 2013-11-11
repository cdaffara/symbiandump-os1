// server.cpp
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

#include <consoleproxy.h>
#include "server.h"

_LIT(KPronxyConsolePanic, "proxyconsole");
	
void Panic(TConsoleProxyPanic aReason)
	{
	User::Panic(KPronxyConsolePanic, aReason);
	}


void ServerThreadL(TServerParams* aParams)
	{
	CActiveScheduler* as = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
	
	CConsoleProxyServer* server = (aParams->iServerNewL)(aParams->iServerParams);
	CleanupStack::PushL(server);
	
	aParams->iServer = server->Server();
	RThread::Rendezvous(KErrNone);
	
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(2, as);
	}
		

TInt ServerThreadFunction(TAny* aArgs)
	{
	__UHEAP_MARK;
	User::SetCritical(User::ENotCritical);
	
	TServerParams* params = (TServerParams*)aArgs;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
		
	TRAPD(err, ServerThreadL(params));
		
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}
		
//______________________________________________________________________________
//						CConsoleProxyServer
CConsoleProxyServer* CConsoleProxyServer::NewL(TAny* aParams)
	{
	TConsoleProxyServerNewLParams* params = (TConsoleProxyServerNewLParams*)aParams;
	return NewL(params->iName, params->iAoPriority, params->iConsoleCreate);
	}
	
EXPORT_C CConsoleProxyServer* CConsoleProxyServer::NewL(const TDesC& aName, TInt aAoPriority, TConsoleCreateFunction aConsoleCreate)
	{
	CConsoleProxyServer* self = new(ELeave)CConsoleProxyServer(aConsoleCreate, aAoPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C void CConsoleProxyServer::ConstructL(const TDesC& aName)
	{
	StartL(aName);
	iShutdownTimer = CShutdownTimer::NewL(*this);
	iShutdownTimer->Start();
	}

EXPORT_C CConsoleProxyServer::CConsoleProxyServer(TConsoleCreateFunction aConsoleCreate, TInt aAoPriority)
	: CServer2(aAoPriority, CServer2::ESharableSessions)
	, iConsoleCreate(aConsoleCreate)
	{
	}

EXPORT_C CConsoleProxyServer::~CConsoleProxyServer()
	{
	delete iShutdownTimer;
	iShutdownTimer = NULL; // Can be referenced by session destructors which run after this, as part of ~CServer2, which will call ~CConsoleProxySession which can call DropSession...
	}
	
EXPORT_C void CConsoleProxyServer::ShutdownTimerExpired()
	{
	CActiveScheduler::Stop();
	}

EXPORT_C CSession2* CConsoleProxyServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new(ELeave)CConsoleProxySession(iConsoleCreate);
	}

void CConsoleProxyServer::AddSession()
	{
	iShutdownTimer->Cancel();
	++iSessionCount;
	}

void CConsoleProxyServer::DropSession()
	{
	--iSessionCount;
	if ((iSessionCount==0) && iShutdownTimer && (!iShutdownTimer->IsActive()))
		{
		iShutdownTimer->Start();
		}
	}

//______________________________________________________________________________
//						CConsoleProxySession
EXPORT_C CConsoleProxySession::CConsoleProxySession(TConsoleCreateFunction aConsoleCreate)
	: iConsoleCreate(aConsoleCreate)
	{
	}
	
EXPORT_C CConsoleProxySession::CConsoleProxySession(MProxiedConsole* aConsole)
	: iConsole(aConsole)
	{
	iConsole->Open();
	}

EXPORT_C CConsoleProxySession::~CConsoleProxySession()
	{
	Server()->DropSession();
	if (!iReadMessage.IsNull())
		{
		iReadMessage.Complete(KErrDisconnected);
		if (iConsole) iConsole->ReadCancel();
		}
	if (iConsole) iConsole->Close();
	}

EXPORT_C void CConsoleProxySession::CreateL()
	{
	Server()->AddSession();
	}

EXPORT_C void CConsoleProxySession::ServiceL(const RMessage2& aMessage)
	{
	if (!iConsole && (aMessage.Function() != RConsoleProxy::ECreate))
		{
		User::Leave(KErrNotReady);
		}
	RBuf buf;
	CleanupClosePushL(buf);
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case RConsoleProxy::ECreate:
		CreateL(aMessage);
		break;
	case RConsoleProxy::ERead:
		if (!iReadMessage.IsNull()) User::Leave(KErrAlreadyExists);
		iReadType = EReadBasic;
		iReadMessage = aMessage;
		iConsole->Read(*this);
		complete = EFalse;
		break;
	case RConsoleProxy::EReadKey:
		if (!iReadMessage.IsNull()) User::Leave(KErrAlreadyExists);
		iReadType = EReadKeys;
		iReadMessage = aMessage;
		iConsole->Read(*this);
		complete = EFalse;
		break;
	case RConsoleProxy::EReadCancel:
		if (!iReadMessage.IsNull())
			{
			iReadMessage.Complete(KErrCancel);
			iConsole->ReadCancel();
			}
		break;
	case RConsoleProxy::EWrite:
		buf.CreateL(aMessage.GetDesLengthL(0));
		aMessage.ReadL(0, buf);
		iConsole->Console()->Write(buf);
		break;
	case RConsoleProxy::EGetCursorPos:
		{
		TPckgBuf<TPoint> pos;
		pos() = iConsole->Console()->CursorPos();
		aMessage.WriteL(0, pos);
		break;
		}
	case RConsoleProxy::ESetCursorPosAbs:
		{
		TPoint pos(aMessage.Int0(), aMessage.Int1());
		iConsole->Console()->SetCursorPosAbs(pos);
		break;
		}
	case RConsoleProxy::ESetCursorPosRel:
		{
		TPoint pos(aMessage.Int0(), aMessage.Int1());
		iConsole->Console()->SetCursorPosRel(pos);
		break;
		}
	case RConsoleProxy::ESetCursorHeight:
		iConsole->Console()->SetCursorHeight(aMessage.Int0());
		break;
	case RConsoleProxy::ESetTitle:
		buf.CreateL(aMessage.GetDesLengthL(0));
		aMessage.ReadL(0, buf);
		iConsole->Console()->SetTitle(buf);
		break;
	case RConsoleProxy::EClearScreen:
		iConsole->Console()->ClearScreen();
		break;
	case RConsoleProxy::EClearToEndOfLine:
		iConsole->Console()->ClearToEndOfLine();
		break;
	case RConsoleProxy::EGetScreenSize:
		{
		TPckgBuf<TSize> size;
		if (aMessage.GetDesLengthL(0)!=size.Length()) User::Leave(KErrArgument);
		size() = iConsole->Console()->ScreenSize();
		aMessage.WriteL(0, size);
		break;
		}
	case RConsoleProxy::EGetKeyCode:
		{
		TPckg<TKeyCode> kc(iKeyCode);
		if (aMessage.GetDesLengthL(0)!=kc.Length()) User::Leave(KErrArgument);
		aMessage.WriteL(0, kc);
		break;
		}
	case RConsoleProxy::EGetKeyModifiers:
		{
		TPckg<TUint> mod(iKeyModifiers);
		if (aMessage.GetDesLengthL(0)!=mod.Length()) User::Leave(KErrArgument);
		aMessage.WriteL(0, mod);
		break;
		}
	case RConsoleProxy::ESetAttributes:
		{
		if (!iConsole) User::Leave(KErrNotReady);
		ConsoleAttributes::TAttributes attributes((TUint)aMessage.Int0(), (ConsoleAttributes::TColor)aMessage.Int1(), (ConsoleAttributes::TColor)aMessage.Int2());
		TInt err = ConsoleAttributes::Set(iConsole->Console(), attributes);
		aMessage.Complete(err);
		complete = EFalse;
		break;
		}
	case RConsoleProxy::EIsConstructed:
		{
		if (aMessage.GetDesLengthL(0)!=sizeof(TBool)) User::Leave(KErrArgument);
		if (!iConsole) User::Leave(KErrNotReady);
		if (!LazyConsole::IsLazy(iConsole->Console())) User::Leave(KErrExtensionNotSupported);
		TPckgBuf<TBool> constructed = LazyConsole::IsConstructed(iConsole->Console());
		aMessage.WriteL(0, constructed);
		complete = ETrue;
		break;
		}
	default:
		aMessage.Complete(KErrNotSupported);
		}
	CleanupStack::PopAndDestroy(&buf);
	if (complete)
		{
		aMessage.Complete(KErrNone);
		}
	}
	
EXPORT_C void CConsoleProxySession::ReadComplete(TInt aStatus)
	{
	iKeyCode = iConsole->Console()->KeyCode();
	iKeyModifiers = iConsole->Console()->KeyModifiers();
	if (!iReadMessage.IsNull())
		{
		TInt err = aStatus;
		if (iReadType == EReadKeys)
			{
			if (err == KErrNone)
				{
				err = iReadMessage.Write(0, TPckg<TKeyCode>(iKeyCode));
				}
			if (err == KErrNone)
				{
				err = iReadMessage.Write(1, TPckg<TUint>(iKeyModifiers));
				}	
			}
		
		iReadMessage.Complete(err);
		}
	}
	
EXPORT_C void CConsoleProxySession::DoCreateL(const TDesC& aTitle, const TSize& aSize)
	{
	__ASSERT_ALWAYS(!iConsole, Panic(EConsoleAlreadyCreated));
	MProxiedConsole* cons = InstantiateConsoleL();
	__ASSERT_ALWAYS(cons, Panic(ENoConsoleInstatiated));
	CleanupClosePushL(*cons);
	TName procName = RProcess().Name(); // econseik sets the process name to the console title...
	TInt err = cons->Console()->Create(aTitle, aSize);
	User::RenameProcess(procName.Left(procName.Locate('['))); // ...so restore it just in case
	User::LeaveIfError(err);
	
	ConsoleCreatedL(cons);
	
	iConsole = cons;
	CleanupStack::Pop(cons);
	}
	
EXPORT_C void CConsoleProxySession::ConsoleCreatedL(MProxiedConsole*)
	{
	}

void CConsoleProxySession::CreateL(const RMessage2& aMessage)
	{
	if (iConsole) User::Leave(KErrAlreadyExists);
	RBuf title;
	title.CreateL(aMessage.GetDesLengthL(0));
	CleanupClosePushL(title);
	aMessage.ReadL(0, title);
	
	TSize size(aMessage.Int1(), aMessage.Int2());
	
	DoCreateL(title, size);
	
	CleanupStack::PopAndDestroy(&title);
	}

EXPORT_C MProxiedConsole* CConsoleProxySession::InstantiateConsoleL()
	{
	return CConsoleWrapper::NewL(iConsoleCreate);
	}


//______________________________________________________________________________
//						MProxiedConsole
EXPORT_C MProxiedConsole* MProxiedConsole::DefaultL(CConsoleBase* aConsole)
	{
	return CConsoleWrapper::NewL(aConsole);
	}
//______________________________________________________________________________
//						CConsoleWrapper
CConsoleWrapper* CConsoleWrapper::NewL(TConsoleCreateFunction aConsoleCreate)
	{
	CConsoleWrapper* self = new(ELeave)CConsoleWrapper;
	CleanupStack::PushL(self);
	self->ConstructL(aConsoleCreate);
	CleanupStack::Pop(self);
	return self;
	}
	
CConsoleWrapper* CConsoleWrapper::NewL(CConsoleBase* aConsole)
	{
	CConsoleWrapper* self = new(ELeave)CConsoleWrapper;
	self->iConsole = aConsole;
	return self;	
	}
	
CConsoleWrapper::~CConsoleWrapper()
	{
	Cancel();
	delete iConsole;
	}

void CConsoleWrapper::Open()
	{
	++iRefCount;
	}

void CConsoleWrapper::Close()
	{
	--iRefCount;
	if (!iRefCount)
		{
		delete this;
		}
	}

CConsoleBase* CConsoleWrapper::Console()
	{
	return iConsole;
	}

void CConsoleWrapper::Read(CConsoleProxySession& aSession)
	{
	if (!IsActive())
		{
		iReader = &aSession;
		iConsole->Read(iStatus);
		SetActive();
		}
	}

void CConsoleWrapper::ReadCancel()
	{
	Cancel();
	iReader = NULL;
	}

void CConsoleWrapper::RunL()
	{
	iReader->ReadComplete(iStatus.Int());
	}

void CConsoleWrapper::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CConsoleWrapper::ConstructL(TConsoleCreateFunction aConsoleCreate)
	{
	iConsole = aConsoleCreate();
	User::LeaveIfNull(iConsole);
	}

CConsoleWrapper::CConsoleWrapper()
	: CActive(CActive::EPriorityStandard)
	, iRefCount(1)
	{
	CActiveScheduler::Add(this);
	}


//______________________________________________________________________________
//						CShutdownTimer
CShutdownTimer* CShutdownTimer::NewL(CConsoleProxyServer& aServer)
	{
	CShutdownTimer* self = new(ELeave)CShutdownTimer(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CShutdownTimer::CShutdownTimer(CConsoleProxyServer& aServer)
	: CTimer(CActive::EPriorityLow), iServer(aServer)
	{
	CActiveScheduler::Add(this);
	}
	
void CShutdownTimer::Start()
	{
	After(KServerShutdownTimer);
	}

void CShutdownTimer::RunL()
	{
	iServer.ShutdownTimerExpired();
	}

