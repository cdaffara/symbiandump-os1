// SocketCommandHandler.h
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

#if !defined(AFX_SOCKETCOMMANDHANDLER_H__C4FCCE6D_0FA7_4D9F_AA7D_449E22C3ED74__INCLUDED_)
#define AFX_SOCKETCOMMANDHANDLER_H__C4FCCE6D_0FA7_4D9F_AA7D_449E22C3ED74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Base.h"
#include "ClientSocket.h"


class CDynamicReadBuffer;
class CSocketCommandReader;


class TPacketHeader
	{
public:
	enum TPacketType
		{
		ECommandCreateConsoleWindow,
		ECommandAttachKeyEventSocket,
		ECommandWrite,
		ECommandGetCursorPos,
		ECommandSetAbsCursorPos,
		ECommandSetRelCursorPos,
		ECommandSetCursorHeight,
		ECommandGetScreenSize,
		ECommandSetTitle,
		ECommandClearScreen,
		ECommandClearToEndOfLine
		};
public:
	TPacketType iType;
	int iSize;
	};


class MSocketCommandHandler
	{
public:
	virtual void HandleSocketCommand(TPacketHeader::TPacketType aCommand, const char* aPacket, int aPacketLength, CSocketCommandReader& aReader) = 0;
	virtual void HandleSocketClosure(CSocketCommandReader& aReader) = 0;
	};


class CSocketCommandReader : public CBase, public MSocketReadHandler, public MSocketClosureObserver
	{
public:
	static CSocketCommandReader* New(CClientSocket& aSocket, MSocketCommandHandler& aHandler);
	virtual ~CSocketCommandReader();
	void ChangeHandler(MSocketCommandHandler& aHandler);
	void ReadCommand();
	void SendResponse(char* aMessage, int aLength);
	CClientSocket& ReleaseSocket();
	CClientSocket& Socket() const;
private:
	CSocketCommandReader(CClientSocket& aSocket, MSocketCommandHandler& aHandler);
	void Construct();
	void QueueRead();
private: // From MSocketReadHandler.
	virtual void SocketReadComplete();
private: // From MSocketClosureObserver.
	virtual void HandleSocketClosure(CSocket& aSocket);
private:
	enum TState
		{
		EIdle,
		EReadingHeader,
		EReadingBody
		};
private:
	CClientSocket* iSocket;
	MSocketCommandHandler* iHandler;
	TState iState;
	TPacketHeader iPacketHeader;
	CDynamicReadBuffer* iReadBuf;
	};


#endif // !defined(AFX_SOCKETCOMMANDHANDLER_H__C4FCCE6D_0FA7_4D9F_AA7D_449E22C3ED74__INCLUDED_)
