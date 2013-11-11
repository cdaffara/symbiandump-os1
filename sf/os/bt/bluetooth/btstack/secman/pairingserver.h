// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef PAIRINGSERVER_H
#define PAIRINGSERVER_H

#include <e32base.h>
#include <es_prot.h>
#include "oobdata.h"
#include "simplepairingresult.h"
#include "SecManNotifiers.h"

class CPhysicalLinksManager;
class CBTProxySAP;

class CPairingSession;
class CPairingSubSession;
class COobDataSession;
class CLinkMgrProtocol;

const TUint KOOBBtAddrOffset = 2;
const TUint KOOBOptionalDataOffset = 8;

/**
This class is the server which provides the services associated with
Bluetooth pairing.
*/
NONSHARABLE_CLASS(CPairingServer)
	: public CPolicyServer
	{
friend class CPairingSession;
public:
	static CPairingServer* NewL(COobDataManager& aOobDataManager, CSimplePairingResultList& aSimplePairingResultList, CAuthenticationResultList& aAuthenticationResultList);
	~CPairingServer();

	void SetPhysicalLinksManager(CPhysicalLinksManager& aLinksMan);
	void ClearPhysicalLinkMgr();
	void SetLinksMgrProtocol(CLinkMgrProtocol& aLinkMgrProtocol);
	void ClearLinksMgrProtocol();

private: // used by sessions.
	CObjectCon* NewContainerL();
	void DeleteContainer(CObjectCon* aContainer);

	CPhysicalLinksManager& LinksManL() const;
	COobDataManager& OobDataManager() const;
	CSimplePairingResultList& SimplePairingResultList() const;
	CAuthenticationResultList& AuthenticationResultList() const;

private:
	CPairingServer(COobDataManager& aOobDataManager, CSimplePairingResultList& aSimplePairingResultList, CAuthenticationResultList& aAuthenticationResultList);
	void ConstructL();
	void AddSession();
	void DropSession();

private: // from CPolicyServer
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

private: // unowned
	COobDataManager&			iOobDataManager;
	CSimplePairingResultList&	iSimplePairingResultList;
	CAuthenticationResultList&	iAuthenticationResultList;
	CPhysicalLinksManager*		iLinksMan;
	CLinkMgrProtocol*		 	iLinkMgrProtocol;

private: // owned
	CObjectConIx*			iContainerIndex;
	TInt					iSessionCount;
	};


/**
This class provides the session representation for the Pairing server.
The class mostly manages and dispatches requests to subsessions.
*/
NONSHARABLE_CLASS(CPairingSession)
	: public CSession2
	{
public:
	static CPairingSession* NewL(CPairingServer& aPairingServer);
	~CPairingSession();

private:
	CPairingSession(CPairingServer& aPairingServer);
	void CreateL();

	CPairingServer& Server() const;

	TBool DispatchSessMessageL(const RMessage2& aMessage);
	void DispatchSubSessMessageL(const RMessage2& aMessage);

	CPairingSubSession* SubSessionFromHandle(TInt aHandle);

	void NewOobDataSubSessionL(const RMessage2& aMessage);
	void NewDedicatedBondingSubSessionL(const RMessage2& aMessage);
	void NewSimplePairingResultSubSessionL(const RMessage2& aMessage);
	void NewAuthenticationResultSubSessionL(const RMessage2& aMessage);
	void SubSessionConstructL(const RMessage2& aMessage, CPairingSubSession* aSubSession);
	void DeleteSubSession(const RMessage2& aMessage);

#ifdef _DEBUG
	// We don't use __DECLARE_TEST because we don't want to export this function.
	void __DbgTestInvariant() const;
#endif // _DEBUG

private: // from CSession2
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);

private:
	CObjectCon* iContainer;
	CObjectIx*	iSubSessions;

	// Unowned
	CPairingServer& 	iPairingServer;
	};


/**
This abstract class provides the base class representation for all subsessions on the
pairing server.
*/
NONSHARABLE_CLASS(CPairingSubSession)
	: public CObject
	{
public:
	~CPairingSubSession();
	virtual void DispatchSubSessMessageL(const RMessage2& aMessage) = 0;

protected:
	CPairingSubSession(CPairingSession& aSession);
	void ConstructL();

	CPairingSession& Session() const;

private:
	CPairingSession&	iSession;
	};


/**
This is the pairing server sub-session for handling OOB data requests.
This covers both inbound OOB data provision, and outbound OOB data retrieval.
*/
NONSHARABLE_CLASS(COobDataSession)
	: public CPairingSubSession
	, private XOobLocalDataObserver
	{
public:
	static COobDataSession* NewLC(CPairingSession& aSession, COobDataManager& aOobDataManager);
	~COobDataSession();

private:
	COobDataSession(CPairingSession& aSession, COobDataManager& aOobDataManager);

	void ProvideParsedRemoteOobDataL(const RMessage2& aMessage);
	void ProvideRawRemoteOobDataL(const RMessage2& aMessage);
	void ClearRemoteOobDataL(const RMessage2& aMessage);
	void ReadLocalOobDataL(const RMessage2& aMessage);
	void CancelReadLocalOobData(const RMessage2& aMessage);

	void LocalOobDataRetrievedL(const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer);

	void TryCancelReadLocalOobData();

	COobDataManager& OobDataManager() const;

private: // from CPairingSubSession
	void DispatchSubSessMessageL(const RMessage2& aMessage);

private: // from XOobLocalDataObserver
	void XoldoLocalOobDataRetrieved(TInt aResult, const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer);

private: // unowned
	COobDataManager&	iOobDataManager;

private: // owned
	RMessage2			iReadLocalOobDataMsg;
	};

/**
This is the pairing server sub-session for handling dedicated bonding requests.
To the client side this represents a single dedicated bond attempt...
*/
NONSHARABLE_CLASS(CDedicatedBondingSession)
	: public CPairingSubSession
	, public MAccessRequestResponseHandler
	, public MSocketNotify
	{
public:
	static CDedicatedBondingSession* NewLC(CPairingSession& aSession, CPhysicalLinksManager& aPhysicalLinksManager);
	~CDedicatedBondingSession();

private:
	CDedicatedBondingSession(CPairingSession& aSession, CPhysicalLinksManager& aPhysicalLinksManager);
	void ConstructL();

	void StartBondingL(const RMessage2& aMessage);
	void DoAccessRequestL();
	void Shutdown();

	void Complete(TInt aError);

	static TInt StaticShutdown(TAny* aPtr);
	TBool ShuttingDown() const;

private: // from CPairingSubSession
	void DispatchSubSessMessageL(const RMessage2& aMessage);

private: // from MAccessRequestResponseHandler
	void AccessRequestComplete(TInt aResult);

private: // from MSocketNotify
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP, const TDesC8& aConnectData);
	void CanClose(TDelete aDelete=EDelete);
	void CanClose(const TDesC8& aDisconnectData, TDelete aDelete=EDelete);
	void Error(TInt aError, TUint anOperationMask=EErrorAllOperations);
	void Disconnect(void);
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8* aBuf);
	void NoBearer(const TDesC8& aConnectionParams);
	void Bearer(const TDesC8& aConnectionInfo);

private:
	enum TState
		{
		EMintCondition,
		EInitiated,
		EInitialConnectionPending,
		EInitialConnection,
		EZombie,
		EFinalConnectionPending,
		EFinalConnection,
		EShutdownRequested,
		EShutdownPending,
		EShutdown,
		};

private:
	TState					iState;

private: // unowned
	CPhysicalLinksManager&	iPhysicalLinksManager;

private: // owned
	RMessage2				iStartBondingMsg;
	CBTProxySAP*			iProxySap;
	CAsyncCallBack*			iAsyncShutdown;
	};


NONSHARABLE_CLASS(CSimplePairingResultSession)
	: public CPairingSubSession
	, public MBluetoothSecurityResultObserver
	{
public:
	static CSimplePairingResultSession* NewLC(CPairingSession& aSession, CSimplePairingResultList& aResultList);
	~CSimplePairingResultSession();

private:
	CSimplePairingResultSession(CPairingSession& aSession, CSimplePairingResultList& aResultList);
	void ConstructL();

	void SimplePairingResultL(const RMessage2& aMessage);
	void CancelSimplePairingResult(const RMessage2& aMessage);

	void ReturnResultL(const TBTDevAddr& aDevAddr);

private: // from CPairingSubSession
	void DispatchSubSessMessageL(const RMessage2& aMessage);

private: // from MBluetoothSecurityResultObserver
	TInt MbsroResult(const TBTDevAddr& aDevAddr, TInt aResult);

private: // unowned
	CSimplePairingResultList&	iResultList;

private: // owned
	RMessage2			iResultMsg;
	};



NONSHARABLE_CLASS(CAuthenticationResultSession)
	: public CPairingSubSession
	, public MBluetoothSecurityResultObserver
	{
public:
	static CAuthenticationResultSession* NewLC(CPairingSession& aSession, CAuthenticationResultList& aResultList);
	~CAuthenticationResultSession();

private:
	CAuthenticationResultSession(CPairingSession& aSession, CAuthenticationResultList& aResultList);
	void ConstructL();

	void AuthenticationResultL(const RMessage2& aMessage);
	void CancelAuthenticationResult(const RMessage2& aMessage);

	void ReturnResultL(const TBTDevAddr& aDevAddr);

private: // from CPairingSubSession
	void DispatchSubSessMessageL(const RMessage2& aMessage);

private: // from MBluetoothSecurityResultObserver
	TInt MbsroResult(const TBTDevAddr& aDevAddr, TInt aResult);

private: // unowned
	CAuthenticationResultList&	iResultList;

private: // owned
	RMessage2			iResultMsg;
	};


#endif // PAIRINGSERVER_H

