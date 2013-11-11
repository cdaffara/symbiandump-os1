// wsp_protocol.h
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

#ifndef __WSP_PROTOCOL_H__
#define __WSP_PROTOCOL_H__

#undef SYMBIAN_ENABLE_SPLIT_HEADERS // More tb92 stopgaps, this time for MNifIfUser in nifman.h/nifman_internal.h
#include <es_prot.h>
#include <comms-infras/nifif.h>

class RWin32Factory;

class CWinSockProviderBase;

class CWinSockProtocol : public CProtocolBase, public MNifIfUser
	{
public:
	~CWinSockProtocol();
	static CWinSockProtocol* NewL(TUint aSocketType, RWin32Factory& aWin32Factory);
	static void ProtocolIdentity(TServerProtocolDesc* aDesc, TUint aSocketType, TUint aProtocol);
public: // From CProtocolBase.
	CServProviderBase* NewSAPL(TUint aProtocol);
	void Identify(TServerProtocolDesc*) const;
	virtual CHostResolvProvdBase* NewHostResolverL();
	virtual CServiceResolvProvdBase* NewServiceResolverL();
	virtual CNetDBProvdBase* NewNetDatabaseL();
	void BindL(CProtocolBase* aProtocol, TUint anId);
	virtual void BindToL(CProtocolBase* protocol);
	TInt Send(RMBufChain&, CProtocolBase* aSourceProtocol);
	void Process(RMBufChain&, CProtocolBase* aSourceProtocol);
	TInt Send(TDes8&, TSockAddr* to, TSockAddr* from, CProtocolBase* aSourceProtocol);
	void Process(TDes8&, TSockAddr* from, TSockAddr* to, CProtocolBase* aSourceProtocol);
	TInt GetOption(TUint level, TUint name, TDes8& anOption, CProtocolBase* aSourceProtocol);
	TInt SetOption(TUint level, TUint name, const TDesC8& option, CProtocolBase* aSourceProtocol);
	void Error(TInt anError, CProtocolBase* aSourceProtocol);
private: // From MNifIfUser.
	virtual void IfUserBindFailure(TInt aResult, TAny* aId);
	virtual void IfUserNewInterfaceL(CNifIfBase* aIf, TAny* aId);
	virtual void IfUserInterfaceDown(TInt aResult, CNifIfBase* aIf);
	virtual void IfUserOpenNetworkLayer();
	virtual void IfUserCloseNetworkLayer();
	virtual CProtocolBase* IfUserProtocol();
 	virtual TBool IfUserIsNetworkLayerActive();
private:
	CWinSockProtocol(TUint aProtocol, RWin32Factory& aWin32Factory);
private:
	TUint iSocketType;
	TUint iProtocol;
	RWin32Factory& iWin32Factory;
	};


#endif // __WSP_PROTOCOL_H__
