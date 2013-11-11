// Server.h
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

#if !defined(AFX_SERVER_H__ECF36F93_36B9_491D_A482_5A1FCBE29932__INCLUDED_)
#define AFX_SERVER_H__ECF36F93_36B9_491D_A482_5A1FCBE29932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Base.h"
#include "ServerSocket.h"
#include "SocketCommandHandler.h"
#include "Preferences.h"
#include "ConsoleWindow.h"


class CConsoleWindow;


class CServer : public CBase, public MSocketClientAcceptor, public MSocketClosureObserver, public MSocketCommandHandler, public MWindowObserver
	{
public:
	static CServer* New(HINSTANCE aAppHandle);
	virtual ~CServer();
	void HandleException(TException aException);
	TPreferences& Preferences();
	const TPreferences& Preferences() const;
	void Listen();
private:
	CServer(HINSTANCE aAppHandle);
	void Construct();
private: // From MClientAcceptor.
	virtual void HandleNewClient(CClientSocket& aClientSocket);
private: // From MSocketClosureObserver.
	virtual void HandleSocketClosure(CSocket& aSocket);
private: // From MSocketCommandHandler.
	virtual void HandleSocketCommand(TPacketHeader::TPacketType aCommand, const char* aPacket, int aPacketLength, CSocketCommandReader& aReader);
	virtual void HandleSocketClosure(CSocketCommandReader& aReader);
private: // From MWindowObserver.
	virtual void HandleWindowClosure(CWindow& aWindow);
	virtual LRESULT HandleWindowCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
private:
	HINSTANCE iAppHandle;
	CServerSocket* iSocket;
	CConsoleWindow* iConsole;
	TPreferences iPreferences;
	unsigned short iPort;
	};

#endif // !defined(AFX_SERVER_H__ECF36F93_36B9_491D_A482_5A1FCBE29932__INCLUDED_)
