// SocketCommandHandler.cpp
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
#include "SocketCommandHandler.h"
#include "DynamicBuffer.h"

const int KInitialBufferLength = 1;


CSocketCommandReader* CSocketCommandReader::New(CClientSocket& aSocket, MSocketCommandHandler& aHandler)
	{
	std::auto_ptr<CSocketCommandReader> self(new(EThrow) CSocketCommandReader(aSocket, aHandler));
	self->Construct();
	return self.release();
	}

CSocketCommandReader::~CSocketCommandReader()
	{
	delete iSocket;
	delete iReadBuf;
	}

void CSocketCommandReader::ChangeHandler(MSocketCommandHandler& aHandler)
	{
	iHandler = &aHandler;
	}

void CSocketCommandReader::ReadCommand()
	{
	ASSERT(iState == EIdle);
	iState = EReadingHeader;
	QueueRead();
	}

void CSocketCommandReader::SendResponse(char* aMessage, int aLength)
	{
	iSocket->Write(aMessage, aLength);
	}

CClientSocket& CSocketCommandReader::ReleaseSocket()
	{
	CClientSocket* socket = iSocket;
	iSocket = NULL;
	return *socket;
	}

CClientSocket& CSocketCommandReader::Socket() const
	{
	ASSERT(iSocket);
	return *iSocket;
	}

CSocketCommandReader::CSocketCommandReader(CClientSocket& aSocket, MSocketCommandHandler& aHandler)
	: iSocket(&aSocket), iHandler(&aHandler), iState(EIdle)
	{
	}

void CSocketCommandReader::Construct()
	{
	iReadBuf = CDynamicReadBuffer::New(KInitialBufferLength);
	iSocket->SetClosureObserver(this);
	}

void CSocketCommandReader::QueueRead()
	{
	switch (iState)
		{
		case EReadingHeader:
			{
			iSocket->Read((char*)&iPacketHeader, sizeof(TPacketHeader), *this);
			break;
			}
		case EReadingBody:
			{
			iSocket->Read(iReadBuf->Buffer(iPacketHeader.iSize + 2), iPacketHeader.iSize, *this); // + 2 for string terminator.
			break;
			}
		default:
			{
			ASSERT(FALSE);
			}
		}
	}

void CSocketCommandReader::SocketReadComplete()
	{
	switch (iState)
		{
		case EReadingHeader:
			{
			if (iPacketHeader.iSize > 0)
				{
				iState = EReadingBody;
				QueueRead();
				}
			else
				{
				// Empty body.
				iState = EIdle;
				iHandler->HandleSocketCommand(iPacketHeader.iType, NULL, 0, *this);
				}
			break;
			}
		case EReadingBody:
			{
			iState = EIdle;
			*(TCHAR*)((char*)iReadBuf->Buffer() + iPacketHeader.iSize) = 0; // Add terminator.
			iHandler->HandleSocketCommand(iPacketHeader.iType, iReadBuf->Buffer(), iPacketHeader.iSize, *this);
			break;
			}
		default:
			{
			ASSERT(FALSE);
			}
		}
	}

void CSocketCommandReader::HandleSocketClosure(CSocket& /*aSocket*/)
	{
	iHandler->HandleSocketClosure(*this);
	}
