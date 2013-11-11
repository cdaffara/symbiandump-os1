// RemoteConsole.h
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

#if !defined(AFX_REMOTECONSOLE_H__EE1B18FE_34CC_42EE_8102_A6C62658E7EA__INCLUDED_)
#define AFX_REMOTECONSOLE_H__EE1B18FE_34CC_42EE_8102_A6C62658E7EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Base.h"
#include "ClientSocket.h"
#include "SocketCommandHandler.h"
#include "Server.h"
#include "ConsoleWindow.h"


class CRemoteConsole : public CBase, public MSocketClosureObserver, public MWindowObserver, public MConsoleCharHandler, public MSocketCommandHandler
	{
public:
	static CRemoteConsole* Instance(int aId);
	static CRemoteConsole* New(HINSTANCE aAppHandle, CSocketCommandReader& aCommandReader, LPCTSTR aTitle, int aWidth, int aHeight, TPreferences& aPreferences);
	virtual ~CRemoteConsole();
	int Id() const;
	void AttachKeyEventSocket(CClientSocket& aSocket);
private:
	CRemoteConsole(CSocketCommandReader& aCommandReader, TPreferences& aPreferences);
	void Construct(HINSTANCE aAppHandle, LPCTSTR aTitle, int aWidth, int aHeight);
private: // From MSocketClosureObserver.
	virtual void HandleSocketClosure(CSocket& aSocket);
private: // From MWindowObserver.
	virtual void HandleWindowClosure(CWindow& aWindow);
	virtual LRESULT HandleWindowCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
private: // From MConsoleCharHandler.
	virtual void HandleConsoleChar(TCHAR aChar, UINT aModifiers);
	virtual void HandleConsoleString(LPCTSTR aString, int aLength);
private: // From MSocketCommandHandler.
	virtual void HandleSocketCommand(TPacketHeader::TPacketType aCommand, const char* aPacket, int aPacketLength, CSocketCommandReader& aReader);
	virtual void HandleSocketClosure(CSocketCommandReader& aReader);
private:
	bool GetCaptureFileName(LPTSTR aFileName) const;
	void GetDefaultCaptureFileName(LPTSTR aFileName) const;
	void SetDefaultCaptureFilePath(LPCTSTR aFileName) const;
private:
	static bool sWindowClassRegistered;
	static int sNextId;
	static CRemoteConsole* sFirst;
	static CRemoteConsole* sLast;
	int iId;
	CRemoteConsole* iNext;
	CRemoteConsole* iPrevious;
	CSocketCommandReader* iCommandReader;
	CClientSocket* iKeyEventSocket;
	CConsoleWindow* iConsole;
	bool iClosing;
	TPreferences& iPreferences;
	};

#endif // !defined(AFX_REMOTECONSOLE_H__EE1B18FE_34CC_42EE_8102_A6C62658E7EA__INCLUDED_)
