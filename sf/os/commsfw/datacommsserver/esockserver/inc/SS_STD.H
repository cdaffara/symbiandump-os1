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

#if !defined(__SS_STD_H__)
#define __SS_STD_H__

#include <es_prot.h>
#include <es_enum.h>
#include <es_panic.h>

#include <es_panic_internal.h>
#include <es_panic_partner.h>

#include <connpref.h>	// for TConnPref
#include <cflog.h>
#include <elements/sd_std.h>
#include <comms-infras/api_ext_list.h>
#include <comms-infras/api_ext_msg.h>
#include <comms-infras/ss_log.h>


/**
@internalComponent
*/
//#define ESOCK_LIB_NAME _L("ESOCK")
#define ESOCK_SERVER_LIB_NAME _L("ESOCKSVR")

class CSockSession;
class CSockSessionProxy;
class CWorkerThread;
class CPlayer;
class CSockManData;
class CPitBoss;

/**
@internalAll
*/
const TUint KDefaultHeapSize=0x1000;

/**
@internalAll
*/
const TUint KMaxHeapSize=0x80000;


/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptBlocking=0x00000001;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptDontRoute=0x00000002;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptKeepAlive=0x00000004;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptDebug=0x00000008;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptOOBInLine=0x00000010;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptReUseAddr=0x00000020;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptBroadCast=0x00000040;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptSendBufSet=0x00000080;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KOptRecBufSet=0x00000100;

/**
Internal socket options bitmap.
@internalComponent
*/
const TUint KESocketDefaultOptions=KOptBlocking;

/**
Maximum length for any Dealer's server name
@internalComponent
*/
const TInt KMaxSockSvrNameLength = 32;

/**
Delay from the SessionClose messages are sent to the players till the session is deleted.
@internalComponent
*/
const TUint32 KSessionCloseDelay = 16*1000*1000;
const TUint32 KSessionClosePlayerDeadline = 8*1000*1000;

const TUint KMaxConnectionInfoSize = 16;
const TInt KMaxFactoryNameSize = 32;


namespace ESock
{
	class CSubConnection;
	class CSocket;
	class CConnection;
	class CHostResolver;
	class CServiceResolver;
	class CNetDatabase;
	class CCommsProviderBase;
}

namespace Den
{
	class RSafeMessage;
}

namespace SubSessActivities
	{
	class CESockClientActivityBase;
	}

namespace NetInterfaces
	{
	class TInterfaceControl;
	}


NONSHARABLE_CLASS(CSockSubSession) : public Den::CWorkerSubSession
/**
@class CSockSubSession
@internalComponent
*/
	{
//	friend class CSockSession;
//	friend class CPlayer;
//	friend class SubSessActivities::CESockClientActivityBase;

public:
	// Reference counting methods formerly supplied by CObject.
    void DeleteMe(); //Utility fn
    virtual ~CSockSubSession();

    CSockSession* Session();
    const CSockSession* Session() const;

	virtual void CommsApiExtBindIfaceL(const RMessage2& aMessage);
	virtual void CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage);
	virtual void CloseExtensionInterface(const RMessage2& aMessage);
	virtual void CancelAndCloseAllClientExtIfaces();

	/** Special non-zero subsession pointer, used in a very few cases to select additional behaviour on a forwarded request. For example on
	creating a subsession this flags that tier resolution is first required
	*/
	static CSockSubSession* KFlagSpecialForwardCase();
	
	// migrate to using SafeMessage
	virtual inline const RMessage2& Message() const
		{
		return static_cast<RMessage2&>(const_cast<Den::RSafeMessage&>(Den::CWorkerSubSession::SafeMessage()));
		};

protected:
	void CancelClientExtItfRequests() const;

protected:
	CSockSubSession(CSockSession* aSession, CPlayer* aPlayer, TSubSessionUniqueId aSubSessionUniqueId);
	void ConstructL(CProtocolBase* aProtocol);

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	inline void IncludePerformanceData(TInt aDeltaClientRxBytes, TInt aDeltaClientRxBuffBytes, TInt aDeltaClientTxBytes);
#endif
	};


/**
@class MSockSessionObserver
Notifier class used by observers of CSockSession events.
@internalComponent
*/
class MSockSessionObserver
	{
public:
	/** Event indicating session is about to close */
	virtual void SessionClosing(const CSockSession* aSession)=0;
	};


NONSHARABLE_CLASS(CSockSession) : public Den::CWorkerSession, public MProvdSecurityChecker

/**
@class CSockSession
Server side representation of RSocketServ.
@internalTechnology
*/
	{
#ifdef __CFLOG_ACTIVE
	friend class CSocketServer;	// for iProcessName
#endif

	friend class CSockSubSession;
    //friend class CPitBoss;
public:
	CWorkerThread& WorkerThread() const;
	CPitBoss& PitBoss() const;

	static CSockSession* NewL(TProcessId aProcessId, TUidType aUid, Den::TSessionUniqueId aSessionUniqueId);
	void ConstructL(TProcessId aProcessId);
	ESock::CSocket * CSocketFromHandle(TUint aHandle);
    TBool FlowAndSCPRFromSocketHandle(TUint aHandle, Messages::TNodeId& aFlow, Messages::TNodeId& aSCPR);
	ESock::CHostResolver * CHostResolverFromHandle(TUint aHandle);
	ESock::CServiceResolver * CServiceResolverFromHandle(TUint aHandle);
	ESock::CNetDatabase * CNetDatabaseFromHandle(TUint aHandle);
	ESock::CConnection* CConnectionFromHandle(TUint aHandle);
    ESock::CSubConnection* CSubConnectionFromHandle(TUint aHandle);
	void TransferSocketL();
	virtual ~CSockSession();
	void AddProtocolL(CProtocolBase * const aProtocol);
	void RemoveProtocolL(CProtocolBase * const aProtocol);
	void NumProtocolsL(void);
	void ProtocolInfoL(void);
	void ProtocolInfoByNameL(void);
	void PanicClient(TESockPanic aPanic);

	void ForwardTierRequestL(const RMessage2& aMessage);
	virtual void ServiceL(const RMessage2& aMessage);
	void RequestOptimalDealerL();
	// Methods implementing MProvdSecurityChecker methods
	TInt CheckPolicy(const TSecurityPolicy& aPolicy, const char *aDiagnostic);

	// from CWorkerSession
	void Disconnect(const RMessage2& aMessage);

protected:
	CSockSession(TUidType aUid, Den::TSessionUniqueId aSessionUniqueId);
	void NotifyObservers();
   	inline CSockManData* SockManGlobals() const;

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	void IncludePerformanceData(TInt aDeltaClientRxBytes, TInt aDeltaClientRxBuffBytes, TInt aDeltaClientTxBytes);
#endif

private:
	void CloseSubSessionL(const RMessage2& aMessage, Den::TSubSessInfo aType);

#ifdef _DEBUG_SOCKET_FUNCTIONS
	void SSDbgFunctionL(const RMessage2& aMessage);
#endif // _DEBUG_SOCKET_FUNCTIONS

#if defined (_DEBUG_SOCKET_FUNCTIONS)
	void DispatchDebugMessageL(const RMessage2& aMessage);
#endif // _DEBUG_SOCKET_FUNCTIONS

private:
	/** Used exclusively for handling ESSRequestOptimalDealer IPC
	Assigned in CSockSession::RequestOptimalDealer and
	CSockSession destructor. */
	Den::CCommonWorkerDealer* iOptimalDealer;

	TProcessId iEligiblePid;	    // ProcessId of authorised process, set by ESSRequestOptimalDealer.
	LOG_STMT( TBuf8<KMaxName> iProcessName; )

#if defined(_DEBUG_SOCKET_FUNCTIONS)
	class TDebugRequestParking
		{
	public:
		TInt iSubSessHandle;
		TInt iNumOutstanding;
		};
	TDebugRequestParking iDebugParking;
#endif
	};

NONSHARABLE_CLASS(CSocketServer) : public Den::CCommonServer
/**
@class CSocketServer
@internalComponent
*/
	{
public:
	static CSocketServer* NewL(CWorkerThread* aOwnerThread);
	~CSocketServer();
	CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/);

protected:
	CSocketServer(CWorkerThread* aOwnerThread, TInt aPriority);
	virtual CSession2* DoNewSessionL(const TProcessId& aProcessId, const TUidType& aProcessUid) const;
	virtual TVersion CurrentVersion() const;
#ifdef _DEBUG
private:
	mutable TBool iPostBootChecksRun;
#endif
	};


//
namespace ESock
    {
    class CSubConnectionFlowBase;
    }
class TAcceptQEntry
/**
@class TAcceptQEntry
@internalComponent
*/
	{
public:
	ESock::CSubConnectionFlowBase* iSSP;
	HBufC8* iConnectData;
	TInt iConnectDataError;
	};



template<class TInfo, TInt QueueSize>
class TNotificationQueue
/**
@class TNotificationQueue
Class used to maintain a notification queue for CConnection.
@internalComponent
*/
	{
public:
	inline TNotificationQueue();
	inline TBool IsEmpty() const { return iQueueLength == 0; }
	void Enque(const TInfo& aInfo);
	TBool Deque(TInfo& aInfo);

	/**
	Use only with non-pointers!!! If a pointer based queue should
	be reseted use the ResetWithPointers() method!!!
	*/
	void Reset();


	/**
	When storing pointers in the queue these methods are essential in order
	to avoid memory leaking.
	Before inserting a pointer make sure with the IsFull() method that the queue
	is not full. If it is full remove the oldest element with the
	'GetTheOldestElem()' call and after that the enque can
	be called (work with pointer reference in order to be able to
	NULLed the queue element).

	When reseting the queue use the ResetAndDestroy() function which will
	delete the heap cells pointed by the pointers in the array and calls
	the Reset() method.

	Use these only with pointers!!!
	*/
	inline TBool IsFull() const { return iQueueLength == QueueSize;}
	inline TInfo& GetTheOldestElem();
	void ResetAndDestroy();
private:
	TInfo iQueue[QueueSize];
	TUint iQueueLength;
	TUint iQueueStart;
	};

#include <es_notq.inl>

/**
@internalComponent
*/
const TUint KMaxProgressQueueLength = 8;

/**
@internalComponent
*/
typedef TNotificationQueue<Elements::TStateChange, KMaxProgressQueueLength>	TProgressQueue;

/**
@internalComponent
*/
const TUint KMaxInterfaceNotificationQueueLength = 8;

/**
@internalComponent
*/
typedef TNotificationQueue<TInterfaceNotification, KMaxInterfaceNotificationQueueLength>	TInterfaceChangeQueue;

/**
@internalComponent
*/
const TUint KMaxSubConnectionEventQueueLength = 8;

/**
@internalComponent
*/
typedef TNotificationQueue<TSubConnectionEvent*, KMaxSubConnectionEventQueueLength> TSubConnectionEventQueue;

/**
@internalComponent
*/
GLREF_C void Fault(TESockFault aFault);

/**
@internalComponent
*/
IMPORT_C void Panic(TProtocolPanic aPanic);

class CSockManData;
class CSockSession;

NONSHARABLE_CLASS(CSocketScheduler) : public CActiveScheduler
/**
@class CSocketScheduler
@internalTechnology
*/
	{

public:
	static CSocketScheduler* New();
//	~CSocketScheduler();
//	virtual void Error(TInt aError) const;
//    static TBool CheckLockedL(CSockSession* aSession);
//    static void SetLockedL(CSockSession* aSession);
//    static TInt Unlock(CSockSession* aSession);
//    void SetCurrentMessage(const RMessage2* aMessage);
//    void ClearCurrentMessage(const RMessage2* aMessage);
//    virtual void WaitForAnyRequest();
//	inline static void SetGlobals(CSockManData* aGlobals);
//	inline static void SetWaitHook(TCallBack* aCall);
	inline static CSockManData* Globals();
private:
//    CSockManData* iGlobals;
	// Within any RunL() only one message (at most, since most RunL()s are for
	// neither a server message delivery nor a forward to an ESock peer) can
	// be the "current message" for a thread. It's referred to by pointer
	// because there's no way to check for completion of an aliased copy and
	// double completion is fatal; keeping this goal of sharing a single instance
	// clearly in sight is desirable. When a message is forwarded from a
	// Dealer to a Player then responsibility for handling passes to the Player.
//	mutable const RMessage2* iCurrentMessage;
//	TCallBack* iWaitHook;
//    CSockSession* iLockedSession;
//    CArrayFixFlat<RMessage2>* iLockedMessageFifo;
	};

#include <ss_std.inl>

#endif
