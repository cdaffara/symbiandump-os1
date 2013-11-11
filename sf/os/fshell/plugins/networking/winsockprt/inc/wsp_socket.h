// wsp_socket.h
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

#ifndef __WSP_SOCKET_H__
#define __WSP_SOCKET_H__

#include <winsock2.h>
#include "wsp_subsession.h"


class RWin32Factory;
class TInetAddr;

class RWin32Socket : public RWin32SubSession
	{
public:
	enum TType
		{
		EBlank,
		ETcp,
		EUdp
		};
public:
	TInt Open(RWin32Factory& aFactory, TType aType);
	void Close();
	TInt Connect(const TInetAddr& aAddress, TRequestStatus& aStatus);
	void CancelConnect();
	TInt Send(const TDesC8& aSendBuffer, TRequestStatus& aStatus);
	void CancelSend();
	TInt SendTo(const TDesC8& aSendBuffer, const TInetAddr& aAddress, TRequestStatus& aStatus);
	void CancelSendTo();
	TInt Receive(TDes8& aReceiveBuffer, TRequestStatus& aStatus);
	void CancelReceive();
	TInt ReceiveFrom(TDes8& aReceiveBuffer, TInetAddr& aAddress, TRequestStatus& aStatus);
	void CancelReceiveFrom();
	TInt GetSocketName(TInetAddr& aAddress) const;
	TInt GetPeerName(TInetAddr& aAddress) const;
	TInt Bind(const TInetAddr& aAddress);
	TInt Listen(TUint aQueueSize);
	TInt Accept(RWin32Socket& aNewSocket, TRequestStatus& aStatus);
	void CancelAccept();
	TInt GetOption(TInt aLevel, TInt aName, char* aValue, TInt* aLength) const;
	TInt SetOption(TInt aLevel, TInt aName, const char* aValue, TInt aLength);
	};


class CWin32Socket : public CWin32SubSession
	{
public:
	enum TOppCode
		{
		EConnect,
		ECancelConnect,
		ESend,
		ECancelSend,
		ESendTo,
		ECancelSendTo,
		EReceive,
		ECancelReceive,
		EReceiveFrom,
		ECancelReceiveFrom,
		EGetSocketName,
		EBind,
		EGetPeerName,
		EListen,
		EAccept,
		ECancelAccept,
		EGetOption,
		ESetOption
		};
public:
	static CWin32Socket* NewL(CWin32Scheduler& aScheduler, RWin32Socket::TType aType);
	virtual ~CWin32Socket();
private: // From CWin32ActiveObject.
	virtual void Run();
private: // From CWin32SubSession.
	virtual void ServiceL(TWin32Message& aMessage);
private:
	CWin32Socket(CWin32Scheduler& aScheduler, RWin32Socket::TType aType);
	void ConstructL();
	void Connect(TWin32Message& aMessage);
	void Send(TWin32Message& aMessage);
	void SendTo(TWin32Message& aMessage);
	void Receive(TWin32Message& aMessage);
	void ReceiveFrom(TWin32Message& aMessage);
	void GetSocketName(TWin32Message& aMessage) const;
	void Bind(TWin32Message& aMessage);
	void GetPeerName(TWin32Message& aMessage) const;
	void Listen(TWin32Message& aMessage);
	void Accept(TWin32Message& aMessage);
	void GetOption(TWin32Message& aMessage) const;
	void SetOption(TWin32Message& aMessage);
	void DoSend();
	void CloseSocket();
	void HandleConnectionComplete(TInt aError);
	void HandleAcceptReady(TInt aError);
	static void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	static void CALLBACK SendToCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	static void CALLBACK ReceiveCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	static void CALLBACK ReceiveFromCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	void ConvertAddress(const TInetAddr& aESockAddress, SOCKADDR_IN& aWinSockAddress) const;
	void ConvertAddress(const SOCKADDR_IN& aWinSockAddress, TInetAddr& aESockAddress) const;
private:
	RWin32Socket::TType iType;
	SOCKET iSocket;
	unsigned long iFlags;
	WSABUF iSendBuffer;
	WSABUF iReceiveBuffer;
	WSAOVERLAPPED iSendOverlapped;
	WSAOVERLAPPED iReceiveOverlapped;
	SOCKADDR_IN iReceiveFromAddress;
	TInt iReceiveFromAddressLength;
	TInetAddr* iReceiveFromClientAddress;
	TInt iBytesSent;
	TWin32Message* iConnectMessage;
	TWin32Message* iSendMessage;
	TWin32Message* iReceiveMessage;
	TWin32Message* iAcceptMessage;
	CWin32Socket* iBlankAcceptSocket;
	};


class MWin32SocketObserver
	{
public:
	enum TRequestType
		{
		ENone,
		EConnect,
		ESend,
		ESendTo,
		EReceive,
		EReceiveFrom,
		EAccept
		};
public:
	virtual void HandleWin32SocketCompletion(TRequestType aRequestType, TInt aError) = 0;
	};


class CWin32SocketWrapper : public CActive
	{
public:
	static CWin32SocketWrapper* NewL(MWin32SocketObserver& aObserver, RWin32Socket& aWin32Socket);
	~CWin32SocketWrapper();
	TInt Connect(const TInetAddr& aAddress);
	TInt Send(const TDesC8& aSendBuffer);
	TInt SendTo(const TDesC8& aSendBuffer, const TInetAddr& aAddress);
	TInt Receive(TDes8& aReceiveBuffer);
	TInt ReceiveFrom(TDes8& aReceiveBuffer, TInetAddr& aAddress);
	TInt Accept(RWin32Socket& aNewSocket);
private: // From CActive.
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
public:
	CWin32SocketWrapper(MWin32SocketObserver& aObserver, RWin32Socket& aWin32Socket);
	void ConstructL();
private:
	MWin32SocketObserver& iObserver;
	RWin32Socket iWin32Socket; // Shallow copy to give each a unique TWin32Message object.
	MWin32SocketObserver::TRequestType iRequestType;
	};


#endif // __WSP_SOCKET_H__
