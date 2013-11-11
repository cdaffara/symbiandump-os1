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

/**
 @file
 @internalComponent 
*/

#if !defined(__SS_ROLES_H__)
#define __SS_ROLES_H__

#include <elements/sd_roles.h>
#include <ss_std.h>
#include <comms-infras/ss_thread.h>
#include <ss_pman.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/cftransport.h>

class CPlayer;
class CWorkerThread;
class MShimControlClient;
class CommsFW::COwnEntryList;
class TWorkerTierMappingsLoaded;

namespace CommsDat
{
	class CMDBSession;
}

/** Socket type values equal to or greater than this constant are reserved for internal use
*/
const TUint KReservedSockTypesBase=0x80000000;

/** Used internally to identify tier entries in the protocol pairing lists
*/
const TUint KTierEntryProxyAddrFam=0xFFFFFFFD;
const TUint KTierEntryProxySockType=0xFFFFFFFD;
const TUint KTierEntryLoadCompleteProtocolType=0x0;

/** Used internally to identify specific Player roles in the protocol pairing lists
*/
const TUint KPlayerRoleProxyAddrFam=0xFFFFFFFC;
const TUint KPlayerRoleProxySockType=0xFFFFFFFC;
const TUint KPlayerRoleTierResolver=0;

namespace ESock
	{
	class CCommsFactoryContainer;
	class RTierThreadMap;
	}

/**
@class CPitBoss
The PitBoss has responsibility for global resources and gross error handling (panics, etc).
The Pit Boss thread must be started by the Configurator before any worker threads. The ESock
instance loading it must be the only one having the WorkerId 0 (TWorkerThreadInfo::EMainThread).
The PitBoss has two main responsibilities:
    -# Maintain global data structures, accessible to all Workers to which the PitBoss pointer is published (through the TWorkerMainIntroductionMsg). Mostly the workers will access this data using PitBoss access functions which are thread-safe, taking special measures to guarantee this where needed.
    -# Manage Worker thread start and death: Registering Workers and installing protocols when they start and do a best effort cleanup if they die due to some error condition.

Apart from these, the PitBoss also provides some minor but important services. For example as
all ESock instances can have their own heap sometimes it happens that one Worker needs to do
something that might cause allocation or freeing on another workers heap, such as inserting a
sub-session into a Dealers session. So the PitBoss offer a function that can determine whether
a different heap is needed (maybe the two workers have different heaps, maybe they share one heap)
and does the switch.
It also has AddSubSession() and RemoveSubSession() methods used by the Player, assuring this
happens on the right heap for operations like Socket Transfers.
*/
NONSHARABLE_CLASS(CPitBoss) : public Den::CCommonPitBoss
	{
public:
	static CPitBoss* NewL(CWorkerThread* aOwnerThread);
	~CPitBoss();

	TBool GetWorkerForProtocol(TUint aAddrFamily, TUint aSockType, TUint aProtocol, CommsFW::TWorkerId& aWorker) const;
	TBool GetWorkerForTier(TInt aTierId, CommsFW::TWorkerId& aWorker) const;
	inline TBool TierMappingsLoaded() const;
	void RequestLoadTierMapping();
	void OnTierMappingLoadedL(const TWorkerTierMappingsLoaded* aMappingMsg, CommsFW::TWorkerId aSenderId);

	inline TBool GetWorkerForPlayerRole(TUint aRoleId, CommsFW::TWorkerId& aWorker) const;
	TBool GetWorkerForProtocol(TUint aIndex, CommsFW::TWorkerId& aWorker) const;
	TBool GetWorkerForProtocolByName(const TProtocolName& aName, CommsFW::TWorkerId& aWorker) const;
	TBool GetWorkerForNullSocket(CommsFW::TWorkerId& aWorker) const;
	TUint GetNumProtocols();
	TInt GetLocalProtocolIndex(TInt aPitBossIndex) const;

	TBool FindOptimalDealer(const TSessionPref& aPref, Den::CCommonWorkerDealer*& aDealer);
	const CommsFW::COwnEntryList* GetCompleteList();

private:
	/**
	@class CPitBoss::TProtocolPairing
	Protocol pairings couple a protocol description (address family, socket type, protocol number & name)
	to the Worker thread(s) that host it (indexed by TSubSessInfo::TSubSessionPlane).
	It implements a single linked list in that is also contains a pointer
	to the next pairing. So all worker threads can access this list of pairings, but still avoid
	locking, workers are allowed to read from the list. The only thread allowed to insert/modify
	the list is the one containing the PitBoss (ESock_Main).
	Non-protocol objects such as tier managers and the tier resolver are also indexed by this scheme,
	using reserved negative values for the address family & socket type.
	*/
	NONSHARABLE_CLASS(TProtocolPairing)
		{
	public:
		TUint iAddrFamily;				//< Address Family
		TUint iSockType;				//< Socket Type
		TUint iProtocol;				//< Protocol Number
		TProtocolName iName;			//< Name of protocol
		CommsFW::TWorkerId iWorkerId;			//< Worker ID for each plane-thread supporting this protocol/addr/sock_type
		TProtocolPairing* iNextPair;	//< Next pairing in the list
		TProtocolPairing* iNextDead;	//< Removed protocols (eg owning Player died) are queued in a separate dead list
		};

	NONSHARABLE_CLASS(TProtocolPairingOwner)
		{
	public:
		TProtocolPairingOwner();
		void Append(TProtocolPairing* aNode);
		void Release();
	public:
		TProtocolPairing* iHead;
		};

private:
	CPitBoss(CWorkerThread* aOwnerThread);
	void ConstructL();
	virtual void DoOnCPMsConfigured();
	virtual void DoOnPeerDeath(CommsFW::TWorkerId aWorkerId);
	virtual TInt DoCreateRedShirt(RThread& aRedShirt, CommsFW::TWorkerId aWorkerId, Den::CCommonWorkerThread& aDeadWorker);
	virtual void DoProcessWorkerIntroductionL(const Den::TWorkerIntroductionMsg& aMsg);
	virtual void DoFreeWorkerReferences(CommsFW::TWorkerId aWorkerId);

	TProtocolPairing* FindProtocolPairing(TUint aAddrFamily, TUint aSockType, TUint aProtocol) const;
	void AddProtocolToListL(TUint aAddrFamily, TUint aSockType, TUint aProtocol, const TProtocolName& aName, CommsFW::TWorkerId aWorker, TProtocolPairingOwner& aList);
	void PopulateAndAddProtocolPairListL(TProtocolName& tierDesc, ESock::RTierThreadMap* map);
	void IncorporateProtocolListL(TProtocolPairingOwner& aList);
	void AddTierPairingToListL(TInt aTierUid, const TDesC& aTierName, CommsFW::TWorkerId aWorker, TProtocolPairingOwner& aList);
	inline void AddPlayerRolePairingL(TUint aRoleId, const TDesC& aRoleDesc, CommsFW::TWorkerId aWorker, TProtocolPairingOwner& aList);
	void RemoveProtocolPairingsForWorker(CommsFW::TWorkerId aWorkerId);
	void SendLoadTierMappingRequest();

	void AddFactoryProxyProtocols(const ESock::CCommsFactoryContainer& aFC, const TDesC& aFCName, CommsFW::TWorkerId aWorkerId);
	void AddPendingIntroductionResponse();
	void RemovePendingIntroductionResponse();
	void FreeWorkerReferences(CommsFW::TWorkerId aWorkerId);


private:
	/**
	When the main thread starts it will search the drives for a complete list of .ESK files and stores them here.
	A pointer to the list will later be sent to workers upon binding, with TWorkerMsg::EMainIntroduction. This way
	there will only ever be one scan for .ESK files on the filesystem. */
	CommsFW::COwnEntryList* iCompleteEskList;

	/**
	List of "protocol pairings" is a lookup list for Workers, mainly Dealers, who need to search for
	specific protocols by name or number and possibly look up the Player that supports the protocols.
	@see ::AddProtocolPairingL
	@see ::FindProtocolPairing
	@see ::RemoveProtocolPairingsForWorker
	*/
	TProtocolPairing* iProtocolPairHead;

	/**
	List of pairings no longer supported by any active Player.
	@see ::RemoveProtocolPairingsForWorker
	*/
	TProtocolPairing* iDeadPairHead;

	/** The mapping of tiers to the worker threads to host their managers is loaded by the tier resolver upon demand,
	with a little complexity from the interplay of roles
	*/
	enum TLoadTierMappingPhase
		{
		EStart = 0,			// not yet requested
		EDealerRequest,		// one or more Dealers asked, request not yet issued to tier resolver (during boot)
		EResolverRequested,	// request sent to tier resolver
		EComplete			// response from tier resolver digested
		};
	TLoadTierMappingPhase iLoadTierMappingPhase;
	CommsFW::TWorkerId iDefaultOptimalDealer;
	};

/**
Represents the session for Player-side actions, such as tracking protocol
references and session observers. Has responsibility for handling session
close, including signalling close complete back to the Dealer
*/
NONSHARABLE_CLASS(CSockSessionProxy) : public Den::CCommonSessionProxy
	{
public:
	static CSockSessionProxy* NewL(Den::CWorkerSession* aSockSession, CPlayer& aPlayer);
	~CSockSessionProxy();

	void AddProtocolL(CProtocolBase* aProtocol);
	void RemoveProtocolL(CProtocolBase* aProtocol);

private:
	CSockSessionProxy(Den::CWorkerSession* aSockSession, CPlayer& aPlayer);
	void ConstructL();

private:
	CArrayFixFlat<CProtocolBase*>* iProtocols;
	};

/**
@class CPlayer
The main responsibility of the Player is receiving the provider operations/queries
from the Dealer and serving them using the protocol plug-ins.
It is also responsible for adding/removing sub-sessions from the provider container
in the related session object as well as keeping a list of all sub-sessions handled
by the Player instance (iSubSessions).
*/
NONSHARABLE_CLASS(CPlayer) : public Den::CCommonPlayer
	{
	friend class CWorkerThread;

public:
	static CPlayer* NewL(CWorkerThread* aOwnerThread, Den::TPlayerRole aPlayerRole);
	~CPlayer();

	CSockSubSession* SubSession(const Den::TSubSessionUniqueId& aSubSessionUniqueId) const;
	CSockSession* CurrentSession() const;
	CWorkerThread& WorkerThread() const;

	TInt ProtocolInfo(TUint aIndex, TProtocolDesc& aProtocol);

	// A Player may not support all kinds of behaviour, eg control + data may be provided by separate threads
	// In future real-time data threads
	inline TBool HasDataPlane() const;
	inline TBool HasSubConnPlane() const;
	inline TBool HasConnPlane() const;
	inline TBool HasMetaConnPlane() const;
	inline TBool HasTierMgrPlane() const;
	inline TBool HasTierResolver() const;

	ESock::CSocket* NewSocketL(TBool aCompleteClientRequest, TInt& aHandle);
	void DeleteSocket(ESock::CSocket& aSocket);

	virtual TBool IsPlayerShutdownComplete();
	virtual void DoProcessMessageL(const Den::RSafeMessage& aMsg, Den::CWorkerSubSession* aSubSession);
	Den::CCommonSessionProxy* DoCreateSessionProxyL(Den::CWorkerSession* aSession);

	inline CSockSessionProxy* CurrentSessionProxyL();
	inline CSockManData* SockManGlobals() const;

	TDes8* BorrowTemporaryBuffer(TInt aSize);
#ifdef _DEBUG
	TBool RunPostBootChecks();
#endif
protected:
	TInt WriteSubSessionHandle(TInt aHandle);

private:
	CPlayer(CWorkerThread* aOwnerThread, Den::TPlayerRole aPlayerRole);
	void ProtocolInfo();
	void ProtocolInfoByName();
	TInt ProtocolInfo(const TDesC &aName,TServerProtocolDesc &aProtocol);

private:
	void CommsApiExtBindIfaceL(const RMessage2& aMessage, CSockSubSession& aSubSession);
	void CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage, CSockSubSession& aSubSession);
	void CloseExtensionInterface(const RMessage2& aMessage, CSockSubSession& aSubSession);
	ESock::CSocket* NewSocketL(TUint anAddrFamily, TUint aSocketType, TUint aProtocol);
	void NewSocketDefaultL();
	void NewSocketWithConnectionL();
	void TransferSocketL(ESock::CSocket* aSocket);
	void LoadProtocolL(TUint anAddrFamily,TUint aSocketType,TUint aProtocol);
	void UnLoadProtocolL(TUint anAddrFamily,TUint aSocketType,TUint aProtocol);
	ESock::CHostResolver* NewHostResolverL(TUint anAddrFamily,TUint aSocketType);
	void NewHostResolverDefaultL(TUint anAddrFamily,TUint aSocketType);
	void NewHostResolverWithConnectionL(TUint anAddrFamily,TUint aSocketType, TInt aHandle);
	void NewServiceResolverL(TUint anAddrFamily,TUint aSocketType,TUint aProtocol);
	void NewNetDatabaseL(TUint anAddrFamily,TUint aSocketType);
	void NewSocketWithSubConnectionL();
	ESock::CSubConnection* NewSubConnectionWithConnectionL();
	ESock::CSubConnection* NewSubConnectionL(ESock::CConnection& aConnection);
	MShimControlClient* CSubConnectionProviderFromHandleL(ESock::CConnection& aConnection, TSubConnectionUniqueId aSubConnectionUniqueId);

	void NewConnectionL();
	void NewConnectionWithNameL(CSockSubSession* aSubSession);
	void SetupNewConnectionL(ESock::CConnection* aConn);

	void FreeSocketMemory();
	void InstallExtensionL();

	void SendToSocketL(TInt aHandle);
	
private:
	RBuf8 iTransferBuffer;
	};

#include <comms-infras/ss_roles.inl>

#endif //__SS_ROLES_H__


