// ServerSocket.cpp
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

#include "Misc.h"
#include "stdafx.h"
#include "console_host.h"
#include "ServerSocket.h"
#include "ClientSocket.h"


CServerSocket* CServerSocket::New(CWindow& aWindow)
	{
	std::auto_ptr<CServerSocket> self(new(EThrow) CServerSocket(aWindow));
	self->Construct();
	return self.release();
	}

CServerSocket::CServerSocket(CWindow& aWindow)
	: CSocket(aWindow), iClientAcceptor(NULL)
	{
	}

void CServerSocket::Listen(unsigned short aPort, unsigned int aBacklogSize, MSocketClientAcceptor& aClientAcceptor)
	{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons((u_short)aPort);

	if (bind(iSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
		{
		throw KExceptionSocketBindFailed;
		}

	if (listen(iSocket, aBacklogSize) == SOCKET_ERROR)
		{
		throw KExceptionSocketListenFailed;
		}

	iClientAcceptor = &aClientAcceptor;
	}

CServerSocket::~CServerSocket()
	{
	}

void CServerSocket::Construct()
	{
	CSocket::Construct();

	if (WSAAsyncSelect(iSocket, iWindow.Handle(), KSocketMessage, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		{
		throw KExceptionSocketSelectFailed;
		}
	}

void CServerSocket::HandleAccept()
	{
	ASSERT(iClientAcceptor);
	SOCKET newSocket = WSAAccept(iSocket, NULL, NULL, NULL, 0);
	if (newSocket == INVALID_SOCKET)
		{
		int err = WSAGetLastError();
		throw KExceptionSocketAcceptFailed;
		}
	std::auto_ptr<CClientSocket> clientSocket(CClientSocket::New(iWindow, newSocket));
	iClientAcceptor->HandleNewClient(*clientSocket.get());
	clientSocket.release();
	}

LRESULT CServerSocket::HandleSocketMessage(LPARAM /*aLParam*/)
	{
	HandleAccept();
	return 0;
	}

