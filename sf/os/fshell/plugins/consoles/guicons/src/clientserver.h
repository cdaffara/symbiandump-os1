// clientserver.h
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


#ifndef __CLIENTSERVER_H__
#define __CLIENTSERVER_H__

#include <e32cons.h>
#include <fshell/consoleproxy.h>
#include "guicons.h"

NONSHARABLE_CLASS(CGuiConsClient) : public CConsoleProxy
	{
public:
	CGuiConsClient();
	
	virtual TInt Create(const TDesC &aTitle,TSize aSize);
	};
	
NONSHARABLE_CLASS(CGuiConsServer) : public CConsoleProxyServer
	{
public:
	static CGuiConsServer* NewL(const TDesC& aServerName, MConsoleUi& aConsoleUi);
	
	virtual void ShutdownTimerExpired();	
	virtual ~CGuiConsServer();
private:
	CGuiConsServer(MConsoleUi& aConsoleUi);
	// from CConsoleProxyServer:
	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
private:
	MConsoleUi& iUi;
	};
	
NONSHARABLE_CLASS(CGuiConsSession)	: public CConsoleProxySession
	{
public:
	CGuiConsSession(MConsoleUi& aConsoleUi);
	virtual ~CGuiConsSession();
	
private: // from CGuiConsSession
	virtual MProxiedConsole* InstantiateConsoleL();
	virtual void ConsoleCreatedL(MProxiedConsole* aConsole);
private:
	MConsoleUi& iUi;
	};


#endif //__CLIENTSERVER_H__

