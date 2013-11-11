// clientserver.cpp
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

#include "clientserver.h"


_LIT(KDefaultServerName, "guicons");

//______________________________________________________________________________
// This function prodes the regular console DLL interface; it MUST be the first
// export.
EXPORT_C TAny* NewConsole()
	{
	return new CGuiConsClient;
	}

//______________________________________________________________________________
//						CGuiConsClient
CGuiConsClient::CGuiConsClient()
	{
	}
	
TInt CGuiConsClient::Create(const TDesC &aTitle, TSize aSize)
	{
	//TODO parse title for server name, font, etc
	TInt r = iConsole.Connect(KDefaultServerName);
	if (r==KErrNone)
		{
		r = CConsoleProxy::Create(aTitle, aSize);
		}
	return r;		
	}


EXPORT_C CServer2* StartGuiConsServerL(const TDesC& aServerName, MConsoleUi& aConsoleUi)
	{
	CConsoleProxyServer* server = CGuiConsServer::NewL(aServerName, aConsoleUi);
	return server;
	}

//______________________________________________________________________________
//						CGuiConsServer
CGuiConsServer* CGuiConsServer::NewL(const TDesC& aServerName, MConsoleUi& aConsoleUi)
	{
	CGuiConsServer* self = new(ELeave)CGuiConsServer(aConsoleUi);
	CleanupStack::PushL(self);
	self->ConstructL(aServerName);
	CleanupStack::Pop(self);
	return self;
	}

CGuiConsServer::~CGuiConsServer()
	{
	}
	
void CGuiConsServer::ShutdownTimerExpired()
	{
	// we don't want to do anything here, we want the UI app to remain after all consoles have closed.
	}

CGuiConsServer::CGuiConsServer(MConsoleUi& aConsoleUi)
	// set the AO priority higher than CCoeRedrawer so that multiple writes from the client only cause
	// a single redraw of the console, improving performance.
	: CConsoleProxyServer(NULL, EActivePriorityRedrawEvents+1)
	, iUi(aConsoleUi)
	{
	}

CSession2* CGuiConsServer::NewSessionL(const TVersion&, const RMessage2&) const
	{
	return new(ELeave)CGuiConsSession(iUi);
	}


	

//______________________________________________________________________________
//						CGuiConsSession
CGuiConsSession::CGuiConsSession(MConsoleUi& aConsoleUi)
	: CConsoleProxySession((TConsoleCreateFunction)NULL), iUi(aConsoleUi)
	{
	}

CGuiConsSession::~CGuiConsSession()
	{
	}

MProxiedConsole* CGuiConsSession::InstantiateConsoleL()
	{
	CCoeEnv::Static()->RootWin().SetOrdinalPosition(0);

	const TDesC& fontFile(iUi.GetConsoleFont());
	CConsoleControl* control;
	if (fontFile.Length())
		{
		control = CConsoleControl::NewL(iUi.GetConsoleBufferSize(), fontFile, &iUi);
		}
	else
		{
		control = CConsoleControl::NewL(iUi.GetConsoleBufferSize(), &iUi);
		}
	CleanupStack::PushL(control);
	MProxiedConsole* result = new(ELeave)CGuiConsole(*control);
	CleanupStack::Pop(control);
	return result;
	}
	
void CGuiConsSession::ConsoleCreatedL(MProxiedConsole* aConsole)
	{
	iUi.HandleNewConsoleL(&((CGuiConsole*)aConsole)->Control());
	}

