// wsp_provider.h
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

#ifndef __WSP_PROVIDER_H__
#define __WSP_PROVIDER_H__

#include <es_prot.h>
#include <in_sock.h>
#include "wsp_socket.h"

class RWin32Factory;

class CWinSockProviderBase : public CServProviderBase, public MWin32SocketObserver
	{
protected:
	virtual ~CWinSockProviderBase();
protected: // From CServProviderBase.
	virtual void LocalName(TSockAddr& anAddr) const;
	virtual TInt SetLocalName(TSockAddr& anAddr);
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint aLevel, TUint aName, TDes8& anOption) const;
	virtual TInt SetOption(TUint aLevel, TUint aName, const TDesC8& anOption);
	virtual void AutoBind();
	virtual void Ioctl(TUint aLevel, TUint aName, TDes8* anOption);
	virtual void CancelIoctl(TUint aLevel, TUint aName);
	virtual TInt PassiveOpen(TUint aQue, const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType option, const TDesC8& aDisconnectData);
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType option);
protected:
	CWinSockProviderBase(RWin32Factory& aWin32Factory);
	void ConstructL();
	TInt FillSendBuffer(const TDesC8& aDataToSend);
protected:
	TInetAddr iRemoteAddr;
	RWin32Factory& iWin32Factory;
	RWin32Socket iWin32Socket;
	CWin32SocketWrapper* iSendSocketWrapper;
	CWin32SocketWrapper* iReceiveSocketWrapper;
	HBufC8* iSendBuffer;
	HBufC8* iReceiveBuffer;
	TPtrC8 iEsockReadBuffer;	// A constant pointer descriptor over iReceiveBuffer, used to pass received data up to ESock.
	TPtr8 iWinSockWriteBuffer;	// A pointer descriptor over iRecevieBuffer, used to receive data from WinSock.
	};


class CWinSockTcpProvider : public CWinSockProviderBase
	{
public:
	static CWinSockTcpProvider* NewL(RWin32Factory& aWin32Factory);
	static CWinSockTcpProvider* NewBlankLC(RWin32Factory& aWin32Factory);
private: // From CServProviderBase.
	virtual void RemName(TSockAddr& anAddr) const;
	virtual TUint Write(const TDesC8& aDesc, TUint options, TSockAddr* anAddr=NULL);
	virtual void GetData(TDes8 &aDesc, TUint options, TSockAddr *anAddr=NULL);
	virtual void Start();
	virtual void ActiveOpen(void);
	virtual TInt PassiveOpen(TUint aQue);
	virtual TInt GetOption(TUint aLevel, TUint aName, TDes8& anOption) const;
	virtual TInt SetOption(TUint aLevel, TUint aName, const TDesC8& anOption);
private: // From MWin32SocketObserver.
	virtual void HandleWin32SocketCompletion(TRequestType aRequestType, TInt aError);
private:
	virtual ~CWinSockTcpProvider();
	CWinSockTcpProvider(RWin32Factory& aWin32Factory);
	void ConstructL(RWin32Socket::TType aType);
	void SetListeningSap(CWinSockTcpProvider& aListeningSap);
	void HandleListeningSapStarted();
	void DoPassiveOpenL(TUint aQue);
	void StartReceive();
private:
	CWin32SocketWrapper* iConnectSocketWrapper;
	CWin32SocketWrapper* iAcceptSocketWrapper;
	CWinSockTcpProvider* iAcceptSap;
	CWinSockTcpProvider* iListeningSap;
	};


class CWinSockUdpProvider : public CWinSockProviderBase
	{
public:
	static CWinSockUdpProvider* NewL(RWin32Factory& aWin32Factory);
private: // From CServProviderBase.
	virtual void RemName(TSockAddr& anAddr) const;
	virtual TInt SetLocalName(TSockAddr& anAddr);
	virtual TUint Write(const TDesC8& aDesc, TUint options, TSockAddr* anAddr=NULL);
	virtual void GetData(TDes8 &aDesc, TUint options, TSockAddr *anAddr=NULL);
	virtual void Start();
	virtual void ActiveOpen(void);
	virtual TInt PassiveOpen(TUint aQue);
private: // From MWin32SocketObserver.
	virtual void HandleWin32SocketCompletion(TRequestType aRequestType, TInt aError);
private:
	virtual ~CWinSockUdpProvider();
	CWinSockUdpProvider(RWin32Factory& aWin32Factory);
	void ConstructL();
private:
	TInetAddr iReceiveFromAddress;
	};


#endif // __WSP_PROVIDER_H__
