// wsp_socket.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <in_sock.h>
#include "wsp_socket.h"
#include "wsp_session.h"
#include "wsp_factory.h"
#include "wsp_panic.h"
#include "wsp_log.h"
#undef SetPort // Defined the Microsoft header "winspool.h".
#include <emulator.h>

//
// RWin32Socket.
//

TInt RWin32Socket::Open(RWin32Factory& aFactory, TType aType)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Open: aType: %d, this: 0x%x"), aType, this));
	TPckgC<TType> typePckg(aType);
	return CreateSubSession(aFactory, CWin32Factory::ENewSocket, typePckg);
	}

void RWin32Socket::Close()
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Close: this: 0x%x"), this));
	RWin32SubSession::Close();
	}

TInt RWin32Socket::Connect(const TInetAddr& aAddress, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Connect: this: 0x%x"), this));
	iWin32Message.Set(CWin32Socket::EConnect, aAddress, aStatus);
	return MakeRequest(iWin32Message);
	}

TInt RWin32Socket::Send(const TDesC8& aSendBuffer, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Send: this: 0x%x, bytes to send: %d"), this, aSendBuffer.Length()));
	iWin32Message.Set(CWin32Socket::ESend, aSendBuffer, aStatus);
	return MakeRequest(iWin32Message);
	}

TInt RWin32Socket::SendTo(const TDesC8& aSendBuffer, const TInetAddr& aAddress, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::SendTo: this: 0x%x, bytes to send: %d"), this, aSendBuffer.Length()));
	TPtr8 addressPtr(const_cast<TUint8*>(aAddress.Ptr()), aAddress.Length(), aAddress.MaxLength()); // Slight hack - need to send two read buffers, but the framework doesn't support this, so send the address as a write buffer.
	iWin32Message.Set(CWin32Socket::ESendTo, aSendBuffer, addressPtr, aStatus);
	return MakeRequest(iWin32Message);
	}

TInt RWin32Socket::Receive(TDes8& aReceiveBuffer, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Receive: this: 0x%x, bytes to receive: %d"), this, aReceiveBuffer.Length()));
	iWin32Message.Set(CWin32Socket::EReceive, aReceiveBuffer, aStatus);
	return MakeRequest(iWin32Message);
	}

TInt RWin32Socket::ReceiveFrom(TDes8& aReceiveBuffer, TInetAddr& aAddress, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::ReceiveFrom: this: 0x%x, bytes to receive: %d"), this, aReceiveBuffer.Length()));
	TPtrC8 addressPtr((TUint8*)&aAddress, 0); // Slight hack - need to send two write buffers, but the framework doesn't support this, so send the address as a read buffer.
	iWin32Message.Set(CWin32Socket::EReceiveFrom, addressPtr, aReceiveBuffer, aStatus);
	return MakeRequest(iWin32Message);
	}

void RWin32Socket::CancelConnect()
	{
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Socket::ECancelConnect);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}

void RWin32Socket::CancelSend()
	{
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Socket::ECancelSend);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}

void RWin32Socket::CancelSendTo()
	{
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Socket::ECancelSendTo);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}

void RWin32Socket::CancelReceive()
	{
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Socket::ECancelReceive);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}

void RWin32Socket::CancelReceiveFrom()
	{
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Socket::ECancelReceiveFrom);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}

TInt RWin32Socket::GetSocketName(TInetAddr& aAddress) const
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::GetSocketName: this: 0x%x"), this));
	TRequestStatus status;
	iWin32Message.Set(CWin32Socket::EGetSocketName, aAddress, status);
	TInt err = MakeRequest(iWin32Message);
	if (err)
		{
		return err;
		}
	else
		{
		User::WaitForRequest(status);
		return status.Int();
		}
	}

TInt RWin32Socket::Bind(const TInetAddr& aAddress)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Bind: this: 0x%x"), this));
	TRequestStatus status;
	iWin32Message.Set(CWin32Socket::EBind, aAddress, status);
	TInt err = MakeRequest(iWin32Message);
	if (err)
		{
		return err;
		}
	else
		{
		User::WaitForRequest(status);
		return status.Int();
		}
	}

TInt RWin32Socket::GetPeerName(TInetAddr& aAddress) const
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::GetPeerName: this: 0x%x"), this));
	TRequestStatus status;
	iWin32Message.Set(CWin32Socket::EGetPeerName, aAddress, status);
	TInt err = MakeRequest(iWin32Message);
	if (err)
		{
		return err;
		}
	else
		{
		User::WaitForRequest(status);
		return status.Int();
		}
	}

TInt RWin32Socket::Listen(TUint aQueueSize)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Listen: this: 0x%x"), this));
	TPckgC<TUint> queueSizePckg(aQueueSize);
	TRequestStatus status;
	iWin32Message.Set(CWin32Socket::EListen, queueSizePckg, status);
	TInt err = MakeRequest(iWin32Message);
	if (err)
		{
		return err;
		}
	else
		{
		User::WaitForRequest(status);
		return status.Int();
		}
	}

TInt RWin32Socket::Accept(RWin32Socket& aNewSocket, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::Accept: this: 0x%x"), this));
	TPckgC<TInt> newSocketHandlePckg(aNewSocket.iHandle);
	iWin32Message.Set(CWin32Socket::EAccept, newSocketHandlePckg, aStatus);
	return MakeRequest(iWin32Message);
	}

void RWin32Socket::CancelAccept()
	{
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Socket::ECancelAccept);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}

class TWin32SocketOptionsC
	{
public:
	TInt aLevel;
	TInt aName;
	const char* aValue;
	TInt aLength;
	};

class TWin32SocketOptions
	{
public:
	TInt aLevel;
	TInt aName;
	char* aValue;
	TInt* aLength;
	};

TInt RWin32Socket::GetOption(TInt aLevel, TInt aName, char* aValue, TInt* aLength) const
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::GetOption: this: 0x%x"), this));
	TWin32SocketOptions options = {aLevel, aName, aValue, aLength};
	TPckg<TWin32SocketOptions> optionsPckg(options);
	TRequestStatus status;
	iWin32Message.Set(CWin32Socket::EGetOption, optionsPckg, status);
	TInt err = MakeRequest(iWin32Message);
	if (err)
		{
		return err;
		}
	else
		{
		User::WaitForRequest(status);
		return status.Int();
		}
	}

TInt RWin32Socket::SetOption(TInt aLevel, TInt aName, const char* aValue, TInt aLength)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Socket::SetOption: this: 0x%x"), this));
	TWin32SocketOptionsC options = {aLevel, aName, aValue, aLength};
	TPckgC<TWin32SocketOptionsC> optionsPckg(options);
	TRequestStatus status;
	iWin32Message.Set(CWin32Socket::ESetOption, optionsPckg, status);
	TInt err = MakeRequest(iWin32Message);
	if (err)
		{
		return err;
		}
	else
		{
		User::WaitForRequest(status);
		return status.Int();
		}
	}

//
// CWin32Socket.
//

CWin32Socket* CWin32Socket::NewL(CWin32Scheduler& aScheduler, RWin32Socket::TType aType)
	{
	CWin32Socket* self = new(ELeave) CWin32Socket(aScheduler, aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWin32Socket::CWin32Socket(CWin32Scheduler& aScheduler, RWin32Socket::TType aType)
	: CWin32SubSession(aScheduler), iType(aType)
	{
	}

void CWin32Socket::ConstructL()
	{
	CWin32SubSession::ConstructL();

	switch (iType)
		{
		case RWin32Socket::EBlank:
			{
			// Nothing to do.
			break;
			}
		case RWin32Socket::ETcp:
			{
			__ASSERT_DEBUG(iSocket == 0, Panic(EWinSockPrtCWin32SocketCreateSocketUnexpectedValidSocket));
			Emulator::Lock();
			iSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
			Emulator::Unlock();
			if (iSocket == INVALID_SOCKET)
				{
				User::Leave(MapWinSockError(WSAGetLastError()));
				}
			break;
			}
		case RWin32Socket::EUdp:
			{
			__ASSERT_DEBUG(iSocket == 0, Panic(EWinSockPrtCWin32SocketCreateSocketUnexpectedValidSocket));
			Emulator::Lock();
			iSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, NULL, WSA_FLAG_OVERLAPPED);
			Emulator::Unlock();
			if (iSocket == INVALID_SOCKET)
				{
				User::Leave(MapWinSockError(WSAGetLastError()));
				}
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtCWin32SocketConstructLInvalidSocketType));
			}
		}
	}

CWin32Socket::~CWin32Socket()
	{
	CloseSocket();
	}

void CWin32Socket::ServiceL(TWin32Message& aMessage)
	{
	switch (aMessage.OppCode())
		{
		case EConnect:
			{
			Connect(aMessage);
			break;
			}
		case ECancelConnect:
			{
			if (iConnectMessage)
				{
				Complete(iConnectMessage, KErrCancel);
				}
			break;
			}
		case ESend:
			{
			Send(aMessage);
			break;
			}
		case ECancelSend:
			{
			if (iSendMessage)
				{
				Complete(iSendMessage, KErrCancel);
				}
			break;
			}
		case ESendTo:
			{
			SendTo(aMessage);
			break;
			}
		case ECancelSendTo:
			{
			if (iSendMessage)
				{
				Complete(iSendMessage, KErrCancel);
				}
			break;
			}
		case EReceive:
			{
			Receive(aMessage);
			break;
			}
		case ECancelReceive:
			{
			if (iReceiveMessage)
				{
				Complete(iReceiveMessage, KErrCancel);
				}
			break;
			}
		case EReceiveFrom:
			{
			ReceiveFrom(aMessage);
			break;
			}
		case ECancelReceiveFrom:
			{
			if (iReceiveMessage)
				{
				Complete(iReceiveMessage, KErrCancel);
				}
			break;
			}
		case EGetSocketName:
			{
			GetSocketName(aMessage);
			break;
			}
		case EBind:
			{
			Bind(aMessage);
			break;
			}
		case EGetPeerName:
			{
			GetPeerName(aMessage);
			break;
			}
		case EListen:
			{
			Listen(aMessage);
			break;
			}
		case EAccept:
			{
			Accept(aMessage);
			break;
			}
		case ECancelAccept:
			{
			if (iAcceptMessage)
				{
				Complete(iAcceptMessage, KErrCancel);
				}
			break;
			}
		case EGetOption:
			{
			GetOption(aMessage);
			break;
			}
		case ESetOption:
			{
			SetOption(aMessage);
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtInvalidSocketOppCode));
			}
		}
	}

void CWin32Socket::Connect(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG(iConnectMessage == NULL, Panic(EWinSockPrtCWin32SocketMultipleConnectRequests));
	iConnectMessage = &aMessage;
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::Connect: this: 0x%x"), this));

	if (WSAEventSelect(iSocket, iEvent, FD_CONNECT) == SOCKET_ERROR)
		{
		TInt err = MapWinSockError(WSAGetLastError());
		CloseSocket();
		Complete(iConnectMessage, err);
		return;
		}

	const TInetAddr& address = static_cast<const TInetAddr&>(iConnectMessage->ReadBuffer());
	SOCKADDR_IN winSockAddress;
	ConvertAddress(address, winSockAddress);

	if (WSAConnect(iSocket, (LPSOCKADDR)&winSockAddress, sizeof(struct sockaddr_in), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
		{
		TInt nError = WSAGetLastError();
		if (nError != WSAEWOULDBLOCK)
			{
			CloseSocket();
			Complete(iConnectMessage, MapWinSockError(nError));
			}
		}
	else
		{
		Complete(iConnectMessage, KErrNone);
		}
	}

void CWin32Socket::Send(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG((iSendMessage == NULL) && (iBytesSent == 0), Panic(EWinSockPrtCWin32SocketMultipleSendRequests));
	iSendMessage = &aMessage;
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::Send: this: 0x%x, bytes to send: %d"), this, iSendMessage->ReadBuffer().Length()));
	DoSend();
	}

void CWin32Socket::SendTo(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG((iSendMessage == NULL) && (iBytesSent == 0), Panic(EWinSockPrtCWin32SocketMultipleSendToRequests));
	iSendMessage = &aMessage;
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::SendTo: this: 0x%x, bytes to send: %d"), this, iSendMessage->ReadBuffer().Length()));
	TPtrC8 bufPtr(iSendMessage->ReadBuffer());
	iSendBuffer.buf = reinterpret_cast<char*>(const_cast<TUint8*>(bufPtr.Ptr()));
	iSendBuffer.len = bufPtr.Length();
	iSendOverlapped.hEvent = (void*) this;
	TInetAddr address;
	address.Copy(iSendMessage->WriteBuffer());
	SOCKADDR_IN winSockAddress;
	ConvertAddress(address, winSockAddress);
	DWORD numberOfBytesSent;
	TInt ret = WSASendTo(iSocket, &iSendBuffer, 1, &numberOfBytesSent, 0, (LPSOCKADDR)&winSockAddress, sizeof(struct sockaddr_in), &iSendOverlapped, &SendToCompletion);
	if (ret == SOCKET_ERROR)
		{
		TInt err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
			{
			WSP_LOG(WspLog::Printf(_L("\tsocket error: %d"), err));
			Complete(iSendMessage, MapWinSockError(err));
			}
		}
	}

void CWin32Socket::Receive(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG(iReceiveMessage == NULL, Panic(EWinSockPrtCWin32SocketMultipleReceiveRequests));
	iReceiveMessage = &aMessage;
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::Receive: this: 0x%x, bytes to get: %d"), this, iReceiveMessage->WriteBuffer().MaxLength()));
	iReceiveBuffer.buf = reinterpret_cast<char*>(const_cast<TUint8*>(iReceiveMessage->WriteBuffer().Ptr()));
	iReceiveBuffer.len = iReceiveMessage->WriteBuffer().MaxLength();
	iFlags = 0;
	iReceiveOverlapped.hEvent = (void*) this;
	DWORD numberOfBytesReceived;
	TInt ret = WSARecv(iSocket, &iReceiveBuffer, 1, &numberOfBytesReceived, &iFlags, &iReceiveOverlapped, &ReceiveCompletion);
	if (ret == SOCKET_ERROR)
		{
		TInt err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
			{
			WSP_LOG(WspLog::Printf(_L("\tsocket error: %d"), err));
			Complete(iReceiveMessage, MapWinSockError(err));
			}
		}
	}

void CWin32Socket::ReceiveFrom(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG(iReceiveMessage == NULL, Panic(EWinSockPrtCWin32SocketMultipleReceiveFromRequests));
	iReceiveMessage = &aMessage;
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::ReceiveFrom: this: 0x%x, bytes to get: %d"), this, iReceiveMessage->WriteBuffer().MaxLength()));
	iReceiveBuffer.buf = reinterpret_cast<char*>(const_cast<TUint8*>(iReceiveMessage->WriteBuffer().Ptr()));
	iReceiveBuffer.len = iReceiveMessage->WriteBuffer().MaxLength();
	iFlags = 0;
	iReceiveOverlapped.hEvent = (void*) this;
	iReceiveFromAddressLength = sizeof(struct sockaddr_in);
	iReceiveFromClientAddress = (TInetAddr*)iReceiveMessage->ReadBuffer().Ptr();
	DWORD numberOfBytesReceived;
	TInt ret = WSARecvFrom(iSocket, &iReceiveBuffer, 1, &numberOfBytesReceived, &iFlags, (LPSOCKADDR)&iReceiveFromAddress, &iReceiveFromAddressLength, &iReceiveOverlapped, &ReceiveFromCompletion);
	if (ret == SOCKET_ERROR)
		{
		TInt err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
			{
			WSP_LOG(WspLog::Printf(_L("\tsocket error: %d"), err));
			Complete(iReceiveMessage, MapWinSockError(err));
			}
		}
	}

void CWin32Socket::GetSocketName(TWin32Message& aMessage) const
	{
	SOCKADDR_IN winSockAddress;
	TInt length = sizeof(struct sockaddr_in);
	if (getsockname(iSocket, (LPSOCKADDR)&winSockAddress, &length))
		{
		aMessage.Complete(MapWinSockError(WSAGetLastError()));
		}
	else
		{
		__ASSERT_DEBUG(length == sizeof(struct sockaddr_in), Panic(EWinSockPrtCWin32SocketGetSocketNameInvalidSocketAddress));
		TInetAddr& address = static_cast<TInetAddr&>(aMessage.WriteBuffer());
		ConvertAddress(winSockAddress, address);
		aMessage.Complete(KErrNone);
		}
	}

void CWin32Socket::Bind(TWin32Message& aMessage)
	{
	const TInetAddr& address = static_cast<const TInetAddr&>(aMessage.ReadBuffer());
	SOCKADDR_IN winSockAddress;
	ConvertAddress(address, winSockAddress);
	if (bind(iSocket, (LPSOCKADDR)&winSockAddress, sizeof(struct sockaddr_in)))
		{
		aMessage.Complete(MapWinSockError(WSAGetLastError()));
		}
	else
		{
		aMessage.Complete(KErrNone);
		}
	}

void CWin32Socket::GetPeerName(TWin32Message& aMessage) const
	{
	SOCKADDR_IN winSockAddress;
	TInt length = sizeof(struct sockaddr_in);
	if (getpeername(iSocket, (LPSOCKADDR)&winSockAddress, &length))
		{
		aMessage.Complete(MapWinSockError(WSAGetLastError()));
		}
	else
		{
		__ASSERT_DEBUG(length == sizeof(struct sockaddr_in), Panic(EWinSockPrtCWin32SocketGetPeerNameInvalidSocketAddress));
		TInetAddr& address = static_cast<TInetAddr&>(aMessage.WriteBuffer());
		ConvertAddress(winSockAddress, address);
		aMessage.Complete(KErrNone);
		}
	}

void CWin32Socket::Listen(TWin32Message& aMessage)
	{
	TPckgBuf<TUint> queueSizeBuf;
	queueSizeBuf.Copy(aMessage.ReadBuffer());
	TUint& queueSize = queueSizeBuf();
	if (listen(iSocket, queueSize))
		{
		aMessage.Complete(MapWinSockError(WSAGetLastError()));
		}
	else
		{
		aMessage.Complete(KErrNone);
		}
	}

void CWin32Socket::Accept(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG(iAcceptMessage == NULL, Panic(EWinSockPrtCWin32SocketMultipleAcceptRequests));
	__ASSERT_DEBUG(iBlankAcceptSocket == NULL, Panic(EWinSockPrtCWin32SocketAcceptBlankSocketNotNull));
	iAcceptMessage = &aMessage;
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::Accept: this: 0x%x"), this));
	if (WSAEventSelect(iSocket, iEvent, FD_ACCEPT) == SOCKET_ERROR)
		{
		Complete(iAcceptMessage, MapWinSockError(WSAGetLastError()));
		}
	TPckgBuf<TInt> newSocketHandleBuf;
	newSocketHandleBuf.Copy(iAcceptMessage->ReadBuffer());
	iBlankAcceptSocket = (CWin32Socket*)newSocketHandleBuf();
	}

void CWin32Socket::GetOption(TWin32Message& aMessage) const
	{
	__ASSERT_DEBUG(aMessage.WriteBuffer().Length() == sizeof(TWin32SocketOptions), EWinSockPrtCWin32SocketGetOptionInvalidParameters);
	TWin32SocketOptions* options = reinterpret_cast<TWin32SocketOptions*>(const_cast<TUint8*>(aMessage.WriteBuffer().Ptr()));
	TInt err = getsockopt(iSocket, options->aLevel, options->aName, options->aValue, options->aLength);
	TWin32Message* messagePrt = &aMessage;
	if (err)
		{
		Complete(messagePrt, MapWinSockError(WSAGetLastError()));
		}
	else
		{
		Complete(messagePrt, KErrNone);
		}
	}

void CWin32Socket::SetOption(TWin32Message& aMessage)
	{
	__ASSERT_DEBUG(aMessage.ReadBuffer().Length() == sizeof(TWin32SocketOptionsC), EWinSockPrtCWin32SocketSetOptionInvalidParameters);
	const TWin32SocketOptionsC* options = reinterpret_cast<const TWin32SocketOptionsC*>(aMessage.ReadBuffer().Ptr());
	TInt err = setsockopt(iSocket, options->aLevel, options->aName, options->aValue, options->aLength);
	TWin32Message* messagePrt = &aMessage;
	if (err)
		{
		Complete(messagePrt, MapWinSockError(WSAGetLastError()));
		}
	else
		{
		Complete(messagePrt, KErrNone);
		}
	}

void CWin32Socket::DoSend()
	{
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::DoSend: this: 0x%x, bytes to send: %d"), this, iSendMessage->ReadBuffer().Length() - iBytesSent));
	TPtrC8 bufPtr(iSendMessage->ReadBuffer());
	iSendBuffer.buf = reinterpret_cast<char*>(const_cast<TUint8*>(bufPtr.Ptr())) + iBytesSent;
	iSendBuffer.len = bufPtr.Length() - iBytesSent;
	iSendOverlapped.hEvent = (void*) this;
	DWORD numberOfBytesSent;
	Emulator::Lock();
	TInt ret = WSASend(iSocket, &iSendBuffer, 1, &numberOfBytesSent, 0, &iSendOverlapped, &SendCompletion);
	Emulator::Unlock();
	if (ret == SOCKET_ERROR)
		{
		Emulator::Lock();
		TInt err = WSAGetLastError();
		Emulator::Unlock();
		if (err != WSA_IO_PENDING)
			{
			WSP_LOG(WspLog::Printf(_L("\tsocket error: %d"), err));
			Complete(iSendMessage, MapWinSockError(err));
			}
		}
	}

void CWin32Socket::CloseSocket()
	{
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::CloseSocket: this: 0x%x"), this));
	if (iSocket != INVALID_SOCKET)
		{
		closesocket(iSocket);
		iSocket = INVALID_SOCKET;
		}
	}

void CWin32Socket::Run()
	{
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::Run: this: 0x%x"), this));
	WSANETWORKEVENTS eventStatus;
	if (WSAEnumNetworkEvents(iSocket, iEvent, &eventStatus) == 0)
		{
		if (eventStatus.lNetworkEvents & FD_CONNECT)
			{
			HandleConnectionComplete(MapWinSockError(eventStatus.iErrorCode[FD_CONNECT_BIT]));
			}
		else if (eventStatus.lNetworkEvents & FD_ACCEPT)
			{
			HandleAcceptReady(MapWinSockError(eventStatus.iErrorCode[FD_ACCEPT_BIT]));
			}
		else
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWSANetworkEvent));
			}
		}
	else
		{
		__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWSAEnumNetworkEventsError));
		}
	}

void CWin32Socket::HandleConnectionComplete(TInt aError)
	{
	ReleaseEventHandle(); // No longer needed because all further asynchronous I/O is done using overlapped buffer callbacks.
	if (iConnectMessage)  // May have been cancelled.
		{
		Complete(iConnectMessage, aError);
		}
	}

void CWin32Socket::HandleAcceptReady(TInt aError)
	{
	if (aError)
		{
		Complete(iAcceptMessage, aError);
		}
	else
		{
		if (iAcceptMessage)  // May have been cancelled.
			{
			iBlankAcceptSocket->iSocket = WSAAccept(iSocket, NULL, NULL, NULL, 0);
			if (iBlankAcceptSocket->iSocket == INVALID_SOCKET)
				{
				Complete(iAcceptMessage, MapWinSockError(WSAGetLastError()));
				}
			else
				{
				Complete(iAcceptMessage, KErrNone);
				}
			}

		iBlankAcceptSocket = NULL;
		if (WSAEventSelect(iSocket, iEvent, 0) == SOCKET_ERROR)
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtCWin32SocketHandleAcceptReadyUnexpectedEventSelectError));
			}
		}
	}

void CALLBACK CWin32Socket::SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD /*dwFlags*/)
	{
	Emulator::Reenter(); // This is a win32 callback so we're outside of the emulator at this point
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::SendCompletion: error: %d, object: 0x%x"), dwError, lpOverlapped->hEvent));
	switch (dwError)
		{
		case 0:
			{
			CWin32Socket* socket = reinterpret_cast<CWin32Socket*>(lpOverlapped->hEvent);
			if (socket && socket->iSendMessage) // May have been cancelled.
				{
				socket->iBytesSent += cbTransferred;
				if (socket->iBytesSent < socket->iSendMessage->ReadBuffer().Length())
					{
					socket->DoSend();
					}
				else
					{
					socket->Complete(socket->iSendMessage, KErrNone);
					socket->iBytesSent = 0;
					}
				}
			break;
			}
		case WSAEINVAL:
			{
			// Ignore - probably a completion on a closed socket due to cancellation.
			break;
			}
		case WSAECONNRESET:
			{
			// Ignore - remote host has terminated the connection.
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWSASendCompletionError));
			}
		}
	Emulator::Escape(); // And return to how we were
	}

void CALLBACK CWin32Socket::SendToCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD /*dwFlags*/)
	{
	Emulator::Reenter(); // This is a win32 callback so we're outside of the emulator at this point
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::SendToCompletion: error: %d, object: 0x%x"), dwError, lpOverlapped->hEvent));
	switch (dwError)
		{
		case 0:
			{
			CWin32Socket* socket = reinterpret_cast<CWin32Socket*>(lpOverlapped->hEvent);
			if (socket->iSendMessage) // May have been cancelled.
				{
				__ASSERT_DEBUG(cbTransferred == (TUint)socket->iSendMessage->ReadBuffer().Length(), Panic(EWinSockPrtCWin32SocketSendToCompletionAllDataNotSent));
				socket->Complete(socket->iSendMessage, KErrNone);
				}
			break;
			}
		case WSAEINVAL:
			{
			// Ignore - probably a completion on a closed socket due to cancellation.
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWSASendToCompletionError));
			}
		}
	Emulator::Escape(); // And return to how we were
	}

void CALLBACK CWin32Socket::ReceiveCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD /*dwFlags*/)
	{
	if (dwError == WSAEINVAL)
		{
		// Ignore - probably a completion on a closed socket due to cancellation.
		// Also, might not be a valid context from which to call Emulator::Reenter().
		return;
		}

	Emulator::Reenter(); // This is a win32 callback so we're outside of the emulator at this point
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::ReceiveCompletion: error: %d, object: 0x%x"), dwError, lpOverlapped->hEvent));
	switch (dwError)
		{
		case 0:
			{
			CWin32Socket* socket = reinterpret_cast<CWin32Socket*>(lpOverlapped->hEvent);
			if (socket && socket->iReceiveMessage) // May have been cancelled.
				{
				socket->iReceiveMessage->WriteBuffer().SetLength(cbTransferred);
				socket->Complete(socket->iReceiveMessage, KErrNone);
				}
			break;
			}
		case WSAECONNRESET:
			{
			// Remote host has reset the connection.
			// Ignore - seen these after Opera gets asked to receive a cookie.
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWSAReceiveCompletionError));
			}
		}
	Emulator::Escape(); // And return to how we were
	}

void CALLBACK CWin32Socket::ReceiveFromCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD /*dwFlags*/)
	{
	Emulator::Reenter(); // This is a win32 callback so we're outside of the emulator at this point
	WSP_LOG(WspLog::Printf(_L("CWin32Socket::ReceiveFromCompletion: error: %d, object: 0x%x"), dwError, lpOverlapped->hEvent));
	switch (dwError)
		{
		case 0:
			{
			CWin32Socket* socket = reinterpret_cast<CWin32Socket*>(lpOverlapped->hEvent);
			if (socket && socket->iReceiveMessage) // May have been cancelled.
				{
				socket->iReceiveMessage->WriteBuffer().SetLength(cbTransferred);
				__ASSERT_DEBUG(socket->iReceiveFromAddressLength == sizeof(struct sockaddr_in), Panic(EWinSockPrtCWin32SocketReceiveFromCompletionInvalidAddressLength));
				TInetAddr address;
				socket->ConvertAddress(socket->iReceiveFromAddress, address);
				socket->iReceiveFromClientAddress->Copy(address);
				socket->iReceiveFromClientAddress = NULL;
				socket->Complete(socket->iReceiveMessage, KErrNone);
				}
			break;
			}
		case WSAEINVAL:
			{
			// Ignore - probably a completion on a closed socket due to cancellation.
			break;
			}
		case WSAECONNRESET:
			{
			// Remote host has reset the connection.
			// Ignore - seen these after Opera gets asked to receive a cookie.
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWSAReceiveCompletionError));
			}
		}
	Emulator::Escape(); // And return to how we were
	}

void CWin32Socket::ConvertAddress(const TInetAddr& aESockAddress, SOCKADDR_IN& aWinSockAddress) const
	{
	aWinSockAddress.sin_family = AF_INET;
	aWinSockAddress.sin_addr.s_addr = htonl(aESockAddress.Address());
	aWinSockAddress.sin_port = htons(static_cast<TUint16>(aESockAddress.Port()));
	}

void CWin32Socket::ConvertAddress(const SOCKADDR_IN& aWinSockAddress, TInetAddr& aESockAddress) const
	{
	aESockAddress.SetAddress(htonl(aWinSockAddress.sin_addr.s_addr));
	aESockAddress.SetPort(htons(aWinSockAddress.sin_port));
	}



//
// CWin32SocketWrapper.
//

CWin32SocketWrapper* CWin32SocketWrapper::NewL(MWin32SocketObserver& aObserver, RWin32Socket& aWin32Socket)
	{
	CWin32SocketWrapper* self = new(ELeave) CWin32SocketWrapper(aObserver, aWin32Socket);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWin32SocketWrapper::~CWin32SocketWrapper()
	{
	Cancel();
	}

CWin32SocketWrapper::CWin32SocketWrapper(MWin32SocketObserver& aObserver, RWin32Socket& aWin32Socket)
	: CActive(CActive::EPriorityStandard), iObserver(aObserver), iWin32Socket(aWin32Socket)
	{
	}

void CWin32SocketWrapper::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

TInt CWin32SocketWrapper::Connect(const TInetAddr& aAddress)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtSocketWrapperConnectWhilstActive));
	iRequestType = MWin32SocketObserver::EConnect;
	TInt err = iWin32Socket.Connect(aAddress, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

TInt CWin32SocketWrapper::Send(const TDesC8& aSendBuffer)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtSocketWrapperSendWhilstActive));
	iRequestType = MWin32SocketObserver::ESend;
	TInt err = iWin32Socket.Send(aSendBuffer, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

TInt CWin32SocketWrapper::SendTo(const TDesC8& aSendBuffer, const TInetAddr& aAddress)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtSocketWrapperSendToWhilstActive));
	iRequestType = MWin32SocketObserver::ESendTo;
	TInt err = iWin32Socket.SendTo(aSendBuffer, aAddress, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

TInt CWin32SocketWrapper::Receive(TDes8& aReceiveBuffer)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtSocketWrapperReceiveWhilstActive));
	iRequestType = MWin32SocketObserver::EReceive;
	TInt err = iWin32Socket.Receive(aReceiveBuffer, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

TInt CWin32SocketWrapper::ReceiveFrom(TDes8& aReceiveBuffer, TInetAddr& aAddress)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtSocketWrapperReceiveFromWhilstActive));
	iRequestType = MWin32SocketObserver::EReceiveFrom;
	TInt err = iWin32Socket.ReceiveFrom(aReceiveBuffer, aAddress, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

TInt CWin32SocketWrapper::Accept(RWin32Socket& aNewSocket)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtSocketWrapperAcceptWhilstActive));
	iRequestType = MWin32SocketObserver::EAccept;
	TInt err = iWin32Socket.Accept(aNewSocket, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

void CWin32SocketWrapper::DoCancel()
	{
	switch (iRequestType)
		{
		case MWin32SocketObserver::EConnect:
			{
			iWin32Socket.CancelConnect();
			break;
			}
		case MWin32SocketObserver::ESend:
			{
			iWin32Socket.CancelSend();
			break;
			}
		case MWin32SocketObserver::ESendTo:
			{
			iWin32Socket.CancelSendTo();
			break;
			}
		case MWin32SocketObserver::EReceive:
			{
			iWin32Socket.CancelReceive();
			break;
			}
		case MWin32SocketObserver::EReceiveFrom:
			{
			iWin32Socket.CancelReceiveFrom();
			break;
			}
		case MWin32SocketObserver::EAccept:
			{
			iWin32Socket.CancelAccept();
			break;
			}
		case MWin32SocketObserver::ENone:
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtSocketWrapperInvalidSocketOppCode));
			break;
			}
		}
	}

void CWin32SocketWrapper::RunL()
	{
	MWin32SocketObserver::TRequestType requestType = iRequestType;
	iRequestType = MWin32SocketObserver::ENone;
	iObserver.HandleWin32SocketCompletion(requestType, iStatus.Int());
	}

TInt CWin32SocketWrapper::RunError(TInt /*aError*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtSocketWrapperUnexpectedRunError));
	return 0;
	}
