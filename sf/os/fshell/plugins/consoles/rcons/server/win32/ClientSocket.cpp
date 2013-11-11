// ClientSocket.cpp
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
#include "ClientSocket.h"
#include "DynamicBuffer.h"

const int KInitialBufferLength = 1;
const int KNotifyReadComplete = 9000;


CClientSocket* CClientSocket::New(CWindow& aWindow, SOCKET aSocket)
	{
	std::auto_ptr<CClientSocket> self(new(EThrow) CClientSocket(aWindow, aSocket));
	self->Construct();
	return self.release();
	}

CClientSocket::CClientSocket(CWindow& aWindow, SOCKET aSocket)
	: CSocket(aWindow, aSocket), iReadHandler(NULL), iReadBuf(NULL), iReadLength(0)
	{
	}

void CClientSocket::Read(char* aBuf, int aLength, MSocketReadHandler& aHandler)
	{
	ASSERT(iReadHandler == NULL);
	iReadBuf = aBuf;
	iReadLength = aLength;
	iReadHandler = &aHandler;
	HandleRead(FALSE);
	}

void CClientSocket::Write(char* aBuf, int aLength)
	{
	iWriteBuf->Add(aBuf, aLength);
	HandleWrite();
	}

CClientSocket::~CClientSocket()
	{
	delete iWriteBuf;
	}

void CClientSocket::Construct()
	{
	CSocket::Construct();

	if (WSAAsyncSelect(iSocket, iWindow.Handle(), KSocketMessage, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		{
		throw KExceptionSocketSelectFailed;
		}

	iWriteBuf = CDynamicWriteBuffer::New(KInitialBufferLength);
	}

void CClientSocket::HandleRead(bool aNotifySynchronously)
	{
	if (iReadLength > 0)
		{
		int ret = recv(iSocket, iReadBuf, iReadLength, 0);
		if (ret == SOCKET_ERROR)
			{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
				HandleClosure();
				}
			}
		else if (ret == 0)
			{
			HandleClosure();
			}
		else
			{
			iReadLength -= ret;
			ASSERT(iReadLength >= 0);
			if (iReadLength == 0)
				{
				if (aNotifySynchronously)
					{
					NotifyReadComplete();
					}
				else
					{
					PostMessage(iWindow.Handle(), KSocketMessage, iSocket, KNotifyReadComplete);
					}
				}
			else
				{
				iReadBuf += ret;
				}
			}
		}
	}

void CClientSocket::HandleWrite()
	{
	if (iWriteBuf->Length() > 0)
		{
		int ret = send(iSocket, iWriteBuf->Buffer(), iWriteBuf->Length(), 0);
		if (ret == SOCKET_ERROR)
			{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
				HandleClosure();
				}
			}
		else if (ret == 0)
			{
			HandleClosure();
			}
		else
			{
			iWriteBuf->Remove(ret);
			}
		}
	}

void CClientSocket::NotifyReadComplete()
	{
	ASSERT(iReadHandler);
	MSocketReadHandler* readHandler = iReadHandler;
	iReadBuf = NULL;
	iReadLength = 0;
	iReadHandler = NULL;
	readHandler->SocketReadComplete();
	}

LRESULT CClientSocket::HandleSocketMessage(LPARAM aLParam)
	{
	switch (WSAGETSELECTEVENT(aLParam))
		{
		case FD_READ:
			HandleRead(TRUE);
			break;
		case FD_WRITE:
			HandleWrite();
			break;
		case KNotifyReadComplete:
			NotifyReadComplete();
			break;
		default:
			ASSERT(FALSE);
		}
	return 0;
	}

