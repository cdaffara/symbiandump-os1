// ServerSocket.h
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

#if !defined(AFX_SERVERSOCKET_H__1B6002D0_3BFE_4640_A9E6_566461E06718__INCLUDED_)
#define AFX_SERVERSOCKET_H__1B6002D0_3BFE_4640_A9E6_566461E06718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Socket.h"


class CClientSocket;


class MSocketClientAcceptor
	{
public:
	virtual void HandleNewClient(CClientSocket& aClientSocket) = 0;
	};


class CServerSocket : public CSocket
	{
public:
	static CServerSocket* New(CWindow& aWindow);
	virtual ~CServerSocket();
	void Listen(unsigned short aPort, unsigned int aBacklogSize, MSocketClientAcceptor& aClientAcceptor);
private:
	CServerSocket(CWindow& aWindow);
	void Construct();
	void HandleAccept();
private: // From CSocket.
	virtual LRESULT HandleSocketMessage(LPARAM aLParam);
private:
	MSocketClientAcceptor* iClientAcceptor;
	};

#endif // !defined(AFX_SERVERSOCKET_H__1B6002D0_3BFE_4640_A9E6_566461E06718__INCLUDED_)
