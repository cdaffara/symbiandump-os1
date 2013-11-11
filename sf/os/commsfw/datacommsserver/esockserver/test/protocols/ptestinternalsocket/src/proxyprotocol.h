// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__PROXYPROTOCOL_H)
#define __PROXYPROTOCOL_H

#include "es_ptestinternalsocket.h"
#include <es_prot.h>
#include <comms-infras/nifif.h>
#include <dns_qry.h>


enum TDPanic
	{
	EBadProtocol = 1000,
	EBadCall = 1001,
	ENotBound = 1002,
	ENotStarted = 1003,
	EInterfaceNotDeleted = 1004,
	EIdAndNoHolder = 1005,
	EIfAndHolder = 1006,
	ETestPanic = 1007
	};
	
void Panic(TDPanic aPanic);

NONSHARABLE_CLASS(CProxyProtocol): public CProtocolBase
	{
public:

	~CProxyProtocol();
	
	static CProxyProtocol *NewL(TInt aProtocol, TServerProtocolDesc* aProtoDesc);
	CServProviderBase *NewSAPL(TUint aProtocol);
	TBool CanCreateSockets();
	void InitL(TDesC& aTag);
	void BindL(CProtocolBase *aProtocol, TUint anId);
	virtual void BindToL(CProtocolBase *protocol);
	void StartL(void);
	TInt Send(RMBufChain &,CProtocolBase* aSourceProtocol);
	void Process(RMBufChain &,CProtocolBase* aSourceProtocol);
	TInt Send(TDes8 &, TSockAddr *to,TSockAddr *from,CProtocolBase* aSourceProtocol);
	void Process(TDes8 & ,TSockAddr *from,TSockAddr *to,CProtocolBase* aSourceProtocol);
	void Identify(TServerProtocolDesc *)const;
	TInt GetOption(TUint level,TUint name,TDes8 &anOption,CProtocolBase* aSourceProtocol);
	TInt SetOption(TUint level,TUint name,const TDesC8& option,CProtocolBase* aSourceProtocol);
	void Error(TInt anError,CProtocolBase* aSourceProtocol);
	TBool IsStarted(){return iStarted;}
private:
	CProxyProtocol(TServerProtocolDesc* aProtoDesc);
	
private:
	TInt iType;
	TServerProtocolDesc* iProtoDesc;
	TBool iStarted;
	CArrayFixFlat<CProtocolBase *> iProtocols;
	};

class MTestStepNotify
	{
public:
	virtual void AboutToShutdown() = 0;
	};

NONSHARABLE_CLASS(CProxyProvd) : public CServProviderBase
	{
public:
	static CProxyProvd *NewL(CProxyProtocol &aProtocol);
	virtual void Ioctl(TUint level,TUint name,TDes8 *anOption);
	void SetTestStepNotify(MTestStepNotify* aNotify) { iTestStepNotify = aNotify; }
	void TestComplete(TProxyProtocolTestResult result);
	
	void LocalName(TSockAddr &anAddr)const;
	virtual TInt SetLocalName(TSockAddr &anAddr);
	virtual void RemName(TSockAddr &anAddr)const;
	virtual TInt SetRemName(TSockAddr &anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8 &anOption)const;
	virtual TInt SetOption(TUint level,TUint name,const TDesC8 &anOption);
	
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt Write(RMBufChain& aData, TUint options, TSockAddr* anAddr=NULL);
	virtual TUint Write(const TDesC8& aDesc, TUint options, TSockAddr* anAddr);
	virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint options,TSockAddr* anAddr=NULL);
	virtual void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL);
	virtual void ActiveOpen(void);
	virtual void ActiveOpen(const TDesC8 &aConnectionData);
	virtual TInt PassiveOpen(TUint aQue);
	virtual TInt PassiveOpen(TUint aQue,const TDesC8 &aConnectionData);
	virtual void AutoBind( void );
	virtual void Shutdown(TCloseType option);
	virtual void Shutdown(TCloseType option,const TDesC8 &aDisconnectData);
	virtual void Start();

private:
	CProxyProvd(CProxyProtocol &aProtocol);
	
private:
	TBool iIsBound;
	CProxyProtocol *iProtocol;
	MTestStepNotify *iTestStepNotify;
	TSockAddr iAddr;
	TBool iInputStopped;
	TInt iListenErrorCode;
	TBool iCancelIoctl; // Set when an IOCTL has been cancelled.
	
	TBool iBlockOnClose; //Used by and test Ioctl to cause the socket to hang on close
	TBool iCompleteIoctl;
	};

#endif //__PROXYPROTOCOL_H

