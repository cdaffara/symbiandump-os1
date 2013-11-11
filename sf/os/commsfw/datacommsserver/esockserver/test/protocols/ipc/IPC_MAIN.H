// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined(__IPC_MAIN_H__)
#define __IPC_MAIN_H__

#include <es_sock.h>
#include <es_prot.h>
#include <e32base_private.h>

class CIpcProtocolFamily: public CProtocolFamilyBase
/**
@internalComponent
*/
	{
private:
	CIpcProtocolFamily();	
public:
	static CIpcProtocolFamily * NewL();
	TInt Install();
	TInt Remove();
	CProtocolBase * NewProtocolL(TUint aSockType,TUint aProtocol);
	TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
	};

class CIpcProvdBase;
class CIpcProtocolHolder : public CBase
/**
@internalComponent
*/
	{
friend class CIpcProtocol;
private:
	CIpcProtocolHolder();
public:
	static CIpcProtocolHolder* NewL();
// Protocol internal functions.
	void SocketRemoved(TInt aPort);
	TInt GetNextFreePort();
	TInt CheckAndAllocatePortNumber(TInt aPort);
	void Add(CIpcProvdBase* aSAP);
	CIpcProvdBase* FindPeerForConnection(TInt aPort);
	~CIpcProtocolHolder();
private:
	TDblQue<CIpcProvdBase> iSAPs;
	TInt iNumSockets;
	CBitMapAllocator* iPortNumbers;
	};

class CIpcProtocol: public CProtocolBase
/**
@internalComponent
*/
	{
public:
	virtual ~CIpcProtocol();
	static CIpcProtocol *NewL(TInt aProtocol);

// Calls from the socket server which we implement
	CServProviderBase *NewSAPL(TUint aProtocol);
	void InitL(TDesC& aTag);
	void StartL(void);
	TBool CanCreateSockets();
	void Identify(TServerProtocolDesc *)const;

// Calls from the socket server which we don't implement
	virtual CHostResolvProvdBase *NewHostResolverL();
	virtual CServiceResolvProvdBase *NewServiceResolverL();
	virtual CNetDBProvdBase* NewNetDatabaseL();

// Calls from the socket server and other protocols which we just panic
	void BindL(CProtocolBase *aProtocol, TUint anId);
	virtual void BindToL(CProtocolBase *protocol);
	TInt Send(RMBufChain &,CProtocolBase* aSourceProtocol);
	void Process(RMBufChain &,CProtocolBase* aSourceProtocol);
	TInt Send(TDes8 &, TSockAddr *to,TSockAddr *from,CProtocolBase* aSourceProtocol);
	void Process(TDes8 & ,TSockAddr *from,TSockAddr *to,CProtocolBase* aSourceProtocol);
	TInt GetOption(TUint level,TUint name,TDes8 &anOption,CProtocolBase* aSourceProtocol);
	TInt SetOption(TUint level,TUint name,const TDesC8& option,CProtocolBase* aSourceProtocol);
	void Error(TInt anError,CProtocolBase* aSourceProtocol);
private:
	CIpcProtocol(TInt aProtocol);
private:
	TInt iProtocol;
	CIpcProtocolHolder* iStreamProtocolSAPs;
	};

class CIpcProvdBase : public CServProviderBase
/**
@internalComponent
*/
	{
friend class CIpcProtocolHolder;
public:
	enum TIpcProvdState {ECreated=0,EConnected,EWaitingConnect,EDisconnected};
	virtual void RemName(TSockAddr &anAddr)const;
	virtual void LocalName(TSockAddr &anAddr)const;
	virtual TInt SetLocalName(TSockAddr &anAddr);
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8 &anOption)const;
	virtual TInt SetOption(TUint level,TUint name,const TDesC8 &anOption);
	virtual void AutoBind( void );
	virtual void Ioctl(TUint level,TUint name,TDes8 *anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt PassiveOpen(TUint aQue,const TDesC8 &aConnectionData);
	virtual void Shutdown(TCloseType option,const TDesC8 &aDisconnectData);
	virtual void ActiveOpen(const TDesC8 &aConnectionData);
	virtual ~CIpcProvdBase();
	virtual void Start();
	virtual void ActiveOpen(void);
	virtual void Shutdown(TCloseType option);
	virtual TInt PassiveOpen(TUint aQue);
	virtual void Disconnect();
	void Connected(CIpcProvdBase& aSocket);
// Pure virtual
	virtual CIpcProvdBase* GetCloneSocket()=0;
	virtual void NewData(TInt aLen)=0;
	virtual void CanSend()=0;
protected:
	CIpcProvdBase(CIpcProtocolHolder* aProtocol);
protected:
	TIpcProvdState iState;
	CIpcProtocolHolder* iProtocol;
	TInt iLocalAddr;
	TInt iRemoteAddr;
	CIpcProvdBase* iConnection;
public:
	TDblQueLink iLink;
	};

class CIpcStreamProvd : public CIpcProvdBase
/**
@internalComponent
*/
	{
public:
	void NewData(TInt aLen);
	void CanSend();
	virtual TUint Write(const TDesC8& aDesc,TUint options, TSockAddr* anAddr=NULL);
	virtual void GetData(TDes8 &aDesc,TUint options,TSockAddr *anAddr=NULL);
	static CIpcStreamProvd* NewL(CIpcProtocolHolder* aProtocol);
	virtual CIpcProvdBase* GetCloneSocket();
private:
	CIpcStreamProvd(CIpcProtocolHolder* aProtocol);
	CCirBuffer iBuffer;
	};

/**
@internalComponent
*/
enum TIPCProtPanic
	{
	ECantBind,
	ECantBindTo,
	ESendCallCantBind,
	EProcessCallCantBind,
	EErrorUpCallCantBind,
	ESetOptionCallCantBind,
	EGetOptionCallCantBind,
	ECantCreateNetDatabase,
	ECantCreateServiceResolver,
	ECantCreateHostResolver,
	EBadWriteCall,
	EBadGetDataCall,
	EReadGetTooMuch,
	ECloseWithoutDeleteingAllSockets,
	EBMABroken
	};

/**
@internalComponent
*/
void Panic(TIPCProtPanic aPanic);

#endif
