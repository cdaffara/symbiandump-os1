// Socket.cpp
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
#include "Socket.h"
#include "DynamicBuffer.h"


CSocket::CSocket(CWindow& aWindow)
	: iWindow(aWindow), iSocket(INVALID_SOCKET), iClosureObserver(NULL)
	{
	}

CSocket::CSocket(CWindow& aWindow, SOCKET aSocket)
	: iWindow(aWindow), iSocket(aSocket), iClosureObserver(NULL)
	{
	}

CSocket::~CSocket()
	{
	if (iSocket != INVALID_SOCKET)
		{
		closesocket(iSocket);
		}

	iWindow.RemoveMessageHandler(*this);
	}

void CSocket::SetClosureObserver(MSocketClosureObserver* aClosureObserver)
	{
	iClosureObserver = aClosureObserver;
	}

void CSocket::Construct()
	{
	if (iSocket == INVALID_SOCKET)
		{
		if ((iSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			{
			throw KExceptionSocketConstructFailed;
			}
		BOOL b = 1;
		setsockopt(iSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&b, sizeof(BOOL));
		}

	iWindow.AddMessageHandler(*this);
	}

void CSocket::HandleClosure()
	{
	if (iClosureObserver)
		{
		iClosureObserver->HandleSocketClosure(*this);
		}
	}

LRESULT CSocket::HandleWindowMessage(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	if ((aMessage == KSocketMessage) && (aWParam == iSocket))
		{
		if (WSAGETSELECTEVENT(aLParam) == FD_CLOSE)
			{
			HandleClosure();
			}
		else
			{
			HandleSocketMessage(aLParam);
			}
		return 0;
		}
	return 1;
	}

