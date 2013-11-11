// ClientSocket.h
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

#if !defined(AFX_CLIENTSOCKET_H__BF5D862F_0F6A_4476_88DD_81F067F299E5__INCLUDED_)
#define AFX_CLIENTSOCKET_H__BF5D862F_0F6A_4476_88DD_81F067F299E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Socket.h"

class CDynamicWriteBuffer;


class MSocketReadHandler
	{
public:
	virtual void SocketReadComplete() = 0;
	};


class CClientSocket : public CSocket
	{
public:
	static CClientSocket* New(CWindow& aWindow, SOCKET aSocket);
	virtual ~CClientSocket();
	void Read(char* aBuf, int aLength, MSocketReadHandler& aHandler);
	void Write(char* aBuf, int aLength);
private:
	CClientSocket(CWindow& aWindow, SOCKET aSocket);
	void Construct();
	void HandleRead(bool aNotifySynchronously);
	void HandleWrite();
	void NotifyReadComplete();
protected: // From CSocket.
	virtual LRESULT HandleSocketMessage(LPARAM aLParam);
private:
	MSocketReadHandler* iReadHandler;
	char* iReadBuf;
	int iReadLength;
	CDynamicWriteBuffer* iWriteBuf;
	};

#endif // !defined(AFX_CLIENTSOCKET_H__BF5D862F_0F6A_4476_88DD_81F067F299E5__INCLUDED_)
