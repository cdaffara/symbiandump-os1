// client.cpp
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

//______________________________________________________________________________
//						RConsoleProxy
static TInt StartServer(const TDesC& aServerName, const TDesC& aServerArgs)
	{
	TPtrC processName(aServerName);
	if (processName.Length() && processName[0] == '!')
		{
		processName.Set(processName.Mid(1));
		}
	RProcess server;
	TInt r = server.Create(processName, aServerArgs);
	if (r!=KErrNone) return r;
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
	}
	
EXPORT_C TInt RConsoleProxy::Connect(const TDesC& aServerName, const TDesC& aServerArgs)
	{
	TInt retry = 2;
	for (;;)
		{
		TInt r = CreateSession(aServerName, TVersion(0,0,0));
		if ((r != KErrNotFound) && (r != KErrServerTerminated))
			{
			return r;
			}
		if (--retry == 0)
			{
			return r;
			}
		r = StartServer(aServerName, aServerArgs);
		if ((r != KErrNone) && (r != KErrAlreadyExists))
			{
			return r;
			}
		}
	}

EXPORT_C TInt RConsoleProxy::Connect(RServer2 aServer)
	{
	return CreateSession(aServer, TVersion(0,0,0));
	}
	
EXPORT_C TInt RConsoleProxy::Connect(TConsoleCreateFunction aConsoleCreate, const TDesC& aThreadNameBase, TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize, RServer2& aServer, RThread& aServerThread)
	{
	TConsoleProxyServerNewLParams newLParams(KNullDesC, CActive::EPriorityStandard, aConsoleCreate);
	TServerParams params;
	params.iServerNewL = &CConsoleProxyServer::NewL;
	params.iServerParams = &newLParams;
	return DoConnect(&params, aThreadNameBase, aStackSize, aHeapMinSize, aHeapMaxSize, aServer, aServerThread);
	}
	
EXPORT_C TInt RConsoleProxy::Connect(TServerNewL aServerConstructor, TAny* aServerParams, const TDesC& aThreadNameBase, TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize, RServer2& aServer, RThread& aServerThread)
	{
	TServerParams params;
	params.iServerNewL = aServerConstructor;
	params.iServerParams = aServerParams;
	return DoConnect(&params, aThreadNameBase, aStackSize, aHeapMinSize, aHeapMaxSize, aServer, aServerThread);
	}
	
TInt RConsoleProxy::DoConnect(TServerParams* aParams, const TDesC& aThreadNameBase, TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize, RServer2& aServer, RThread& aServerThread)
	{
	TName threadName;
	RThread server;
	TInt threadId = 0;
	_LIT(KThreadIdFmt, "%08x");
	TInt err;
	do
		{
		threadName = aThreadNameBase.Left(threadName.MaxLength()-8);
		threadName.AppendFormat(KThreadIdFmt, threadId);
		err = server.Create(threadName, &ServerThreadFunction, aStackSize, aHeapMinSize, aHeapMaxSize, aParams);
		++threadId;
		} while (err==KErrAlreadyExists);
	if (err!=KErrNone) return err;
	
	TRequestStatus rendezvous;
	server.Rendezvous(rendezvous);
	if (rendezvous == KRequestPending)
		{
		server.Resume();
		}
	User::WaitForRequest(rendezvous);
	err = rendezvous.Int();
	if (server.ExitType() != EExitPending && err >= 0) err = KErrDied;
	if (err==KErrNone)
		{
		err = Connect(aParams->iServer);
		}
	aServer = aParams->iServer;
	aServerThread = server;
	return err;
	}


EXPORT_C TInt RConsoleProxy::Server(RServer2& aServer)
	{
	TInt r = SendReceive(EGetServer);
	return aServer.SetReturnedHandle(r);
	}

EXPORT_C void RConsoleProxy::Create(const TDesC& aConsoleTitle, TSize aSize, TRequestStatus& aStatus)
	{
	SendReceive(ECreate, TIpcArgs(&aConsoleTitle, aSize.iWidth, aSize.iHeight), aStatus);
	}

EXPORT_C void RConsoleProxy::Read(TRequestStatus& aStatus)
	{
	SendReceive(ERead, aStatus);
	}
	
EXPORT_C void RConsoleProxy::Read(TPckg<TKeyCode>& aKeyCode, TPckg<TUint>& aKeyModifiers, TRequestStatus& aStatus)
	{
	SendReceive(EReadKey, TIpcArgs(&aKeyCode, &aKeyModifiers), aStatus);
	}

EXPORT_C void RConsoleProxy::ReadCancel()
	{
	Send(EReadCancel);
	}

EXPORT_C void RConsoleProxy::Write(const TDesC& aDescriptor, TRequestStatus& aStatus)
	{
	SendReceive(EWrite, TIpcArgs(&aDescriptor), aStatus);
	}

EXPORT_C void RConsoleProxy::CursorPos(TPckg<TPoint>& aPos, TRequestStatus& aStatus) const
	{
	SendReceive(EGetCursorPos, TIpcArgs(&aPos), aStatus);
	}

EXPORT_C void RConsoleProxy::SetCursorPosAbs(const TPoint& aPoint, TRequestStatus& aStatus)
	{
	SendReceive(ESetCursorPosAbs, TIpcArgs(aPoint.iX, aPoint.iY), aStatus);
	}

EXPORT_C void RConsoleProxy::SetCursorPosRel(const TPoint& aPoint, TRequestStatus& aStatus)
	{
	SendReceive(ESetCursorPosRel, TIpcArgs(aPoint.iX, aPoint.iY), aStatus);
	}

EXPORT_C void RConsoleProxy::SetCursorHeight(TInt aPercentage, TRequestStatus& aStatus)
	{
	SendReceive(ESetCursorHeight, TIpcArgs(aPercentage), aStatus);
	}

EXPORT_C void RConsoleProxy::SetTitle(const TDesC& aTitle, TRequestStatus& aStatus)
	{
	SendReceive(ESetTitle, TIpcArgs(&aTitle), aStatus);
	}

EXPORT_C void RConsoleProxy::ClearScreen(TRequestStatus& aStatus)
	{
	SendReceive(EClearScreen, aStatus);
	}

EXPORT_C void RConsoleProxy::ClearToEndOfLine(TRequestStatus& aStatus)
	{
	SendReceive(EClearToEndOfLine, aStatus);
	}

EXPORT_C void RConsoleProxy::GetScreenSize(TPckg<TSize>& aSize, TRequestStatus& aStatus) const
	{
	SendReceive(EGetScreenSize, TIpcArgs(&aSize), aStatus);
	}

EXPORT_C void RConsoleProxy::SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, TRequestStatus& aStatus)
	{
	SendReceive(ESetAttributes, TIpcArgs(aAttributes, aForegroundColor, aBackgroundColor), aStatus);
	}
	
EXPORT_C TInt RConsoleProxy::IsConstructed(TBool& aLazy)
	{
	TPckg<TBool> constructed(aLazy);
	return SendReceive(EIsConstructed, TIpcArgs(&constructed));
	}

EXPORT_C TInt RConsoleProxy::GetKeyCode(TKeyCode& aCode) const
	{
	TPckg<TKeyCode> kc(aCode);
	return SendReceive(EGetKeyCode, TIpcArgs(&(kc)));
	}

EXPORT_C TInt RConsoleProxy::GetKeyModifiers(TUint& aModifiers) const
	{
	TPckg<TUint> mod(aModifiers);
	return SendReceive(EGetKeyCode, TIpcArgs(&mod));
	}

EXPORT_C TInt RConsoleProxy::Create(const TDesC& aConsoleTitle, TSize aSize)
	{
	TRequestStatus stat;
	Create(aConsoleTitle, aSize, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::Write(const TDesC& aDescriptor)
	{
	TRequestStatus stat;
	Write(aDescriptor, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::CursorPos(TPoint& aPos) const
	{
	TRequestStatus stat;
	TPckg<TPoint> pos(aPos);
	CursorPos(pos, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::SetCursorPosAbs(const TPoint& aPoint)
	{
	TRequestStatus stat;
	SetCursorPosAbs(aPoint, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::SetCursorPosRel(const TPoint& aPoint)
	{
	TRequestStatus stat;
	SetCursorPosRel(aPoint, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::SetCursorHeight(TInt aPercentage)
	{
	TRequestStatus stat;
	SetCursorHeight(aPercentage, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::SetTitle(const TDesC& aTitle)
	{
	TRequestStatus stat;
	SetTitle(aTitle, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::ClearScreen()
	{
	TRequestStatus stat;
	ClearScreen(stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::ClearToEndOfLine()
	{
	TRequestStatus stat;
	ClearToEndOfLine(stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RConsoleProxy::GetScreenSize(TSize& aSize) const
	{
	TRequestStatus stat;
	TPckg<TSize> size(aSize);
	GetScreenSize(size, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

//______________________________________________________________________________
//						CConsoleProxy
EXPORT_C CConsoleProxy* CConsoleProxy::NewL(const RConsoleProxy& aProxySession)
	{
	CConsoleProxy* self = new(ELeave)CConsoleProxy();
	CleanupStack::PushL(self);
	self->ConstructL(aProxySession);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CConsoleProxy::CConsoleProxy()
	: iKeyCodePckg(iKeyCode), iKeyModifiersPckg(iKeyModifiers)
	{
	}

EXPORT_C TInt CConsoleProxy::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == ConsoleAttributes::KSetConsoleAttributesExtension)
		{
		TRequestStatus status;
		ConsoleAttributes::TAttributes* attributes = (ConsoleAttributes::TAttributes*)a1;
		iConsole.SetAttributes(attributes->iAttributes, attributes->iForegroundColor, attributes->iBackgroundColor, status);
		User::WaitForRequest(status);
		return status.Int();
		}
	if (aExtensionId == LazyConsole::KLazyConsoleExtension)
		{
		TBool* constructed = (TBool*)a1;
		return iConsole.IsConstructed(*constructed);
		}
	return CConsoleBase::Extension_(aExtensionId, a0, a1);
	}
	
EXPORT_C void CConsoleProxy::ConstructL(const RConsoleProxy& aProxySession)
	{
	RConsoleProxy cons = aProxySession;
	User::LeaveIfError(cons.Duplicate(RThread(), EOwnerThread));
	iConsole = cons;
	}

EXPORT_C CConsoleProxy::~CConsoleProxy()
	{
	iConsole.Close();
	}

EXPORT_C TInt CConsoleProxy::Create(const TDesC &aTitle, TSize aSize)
	{
	return iConsole.Create(aTitle, aSize);
	}

EXPORT_C void CConsoleProxy::Read(TRequestStatus &aStatus)
	{
	iConsole.Read(iKeyCodePckg, iKeyModifiersPckg, aStatus);
	}

EXPORT_C void CConsoleProxy::ReadCancel()
	{
	iConsole.ReadCancel();
	}

EXPORT_C void CConsoleProxy::Write(const TDesC &aDes)
	{
	iConsole.Write(aDes);
	}

EXPORT_C TPoint CConsoleProxy::CursorPos() const
	{
	TPoint pos;
	iConsole.CursorPos(pos);
	return pos;
	}


EXPORT_C void CConsoleProxy::SetCursorPosAbs(const TPoint &aPoint)
	{
	iConsole.SetCursorPosAbs(aPoint);
	}

EXPORT_C void CConsoleProxy::SetCursorPosRel(const TPoint &aPoint)
	{
	iConsole.SetCursorPosRel(aPoint);
	}

EXPORT_C void CConsoleProxy::SetCursorHeight(TInt aPercentage)
	{
	iConsole.SetCursorHeight(aPercentage);
	}

EXPORT_C void CConsoleProxy::SetTitle(const TDesC &aTitle)
	{
	iConsole.SetTitle(aTitle);
	}

EXPORT_C void CConsoleProxy::ClearScreen()
	{
	iConsole.ClearScreen();
	}

EXPORT_C void CConsoleProxy::ClearToEndOfLine()
	{
	iConsole.ClearToEndOfLine();
	}

EXPORT_C TSize CConsoleProxy::ScreenSize() const
	{
	TSize size;
	iConsole.GetScreenSize(size);
	return size;
	}

EXPORT_C TKeyCode CConsoleProxy::KeyCode() const
	{
	return iKeyCode;
	}

EXPORT_C TUint CConsoleProxy::KeyModifiers() const
	{
	return iKeyModifiers;
	}
