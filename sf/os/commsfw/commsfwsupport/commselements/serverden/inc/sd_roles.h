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
// THIS WHOLE FILE IS INTERNAL COMPONENT - DO NOT ADD NON-INTERNAL ITEMS
//
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_DEN_ROLES_H
#define SYMBIAN_DEN_ROLES_H

#include <e32property.h>
#include <elements/sd_std.h>
#include <elements/sd_thread.h>
#include <elements/interfacetable.h>

namespace Den
{

class CCommonPlayer;
class CCommonDealer;
class CWorkerSubSession;
class TWorkerIntroductionMsg;

class CWorkerDataGlobals : public CommsFW::CWorkerThreadDataGlobals<TWorkerThreadRegister, CommsFW::KMaxWorkerId>
	{
public:
	static CWorkerDataGlobals* NewL()
		{
		CWorkerDataGlobals* self = new(ELeave) CWorkerDataGlobals();
		CleanupStack::PushL(self);
		self->ConstructL(sizeof(TWorkerThreadRegister), CommsFW::KMaxWorkerId);
		CleanupStack::Pop(self);
		return self;

		// Because the curr
		}

	CCommonWorkerThread* WorkerThread(CommsFW::TWorkerId aWorker) const
		{
		return GetWorkerGlobals(aWorker)->iWorker;
		}
	};

/**
Interface used by the CPeerDeathNotifier to notify a subscriber of thread death.
@see CPitBoss
*/
NONSHARABLE_CLASS(MPeerDeathObserver)
	{
public:
	virtual void OnPeerDeath(CommsFW::TWorkerId aWorkerId) = 0;
	};

/**
Monitors a worker thread and notifies the observer if it dies.
@see RThread::Logon()
*/
NONSHARABLE_CLASS(CPeerDeathNotifier) : public CActive
	{
public:
	CPeerDeathNotifier(CommsFW::TWorkerId aWorkerId, RThread aThread, MPeerDeathObserver& aObserver);
private:
	RUndertaker iUndertaker;		//< Unused, should be removed
private:
	void DoCancel();
	void RunL();
private:
	CommsFW::TWorkerId iWorkerId;			//< The worker id of the thread being monitored.
	RThread iThread;				//< The handle to the thread being monitored.
	MPeerDeathObserver& iObserver;	//< The observer to Notify.
	};


/**
Interface used by the CConfigurationLevelMonitor to notify a subscriber when
the Reference Configurator has configured the core components (configuration level == RootServer::ECoreComponentsStarted).
@see CPitBoss
*/
NONSHARABLE_CLASS(MConfiguratorObserver)
	{
public:
	virtual void OnCPMsConfigured() = 0;
	virtual TUid PropertyKey()=0;
	};

/**
Subscribes to the configuration level property published by the reference configurator.
Calls the subscribed MConfiguratorObserver when the Reference Configurator has configured
the core components (configuration level == RootServer::ECoreComponentsStarted).
@see CPitBoss
*/
NONSHARABLE_CLASS(CConfigurationLevelMonitor) : public CActive
	{
public:
	static CConfigurationLevelMonitor* NewL(MConfiguratorObserver* aObserver);
	~CConfigurationLevelMonitor();
protected:
	CConfigurationLevelMonitor(MConfiguratorObserver* aObserver);
	void Subscribe();
	void RunL();
	void DoCancel();
protected:
	/** The configuration level property published by the reference configurator.
	*/
	RProperty iConfLevelProperty;

	/** The observer waiting for notification that core components have been configured. */
	MConfiguratorObserver* iObserver;

	/** Set ETrue when the level has reached RootServer::ECoreComponentsStarted.
	Used to ensure the observer is only called once even though the level keeps
	being published beyond RootServer::ECoreComponentsStarted.
	*/
	TBool iCoreComponentsConfigured;
	};

/**
The PitBoss has responsibility for global resources and gross error handling (panics, etc).
The Pit Boss thread must be started by the Configurator before any worker threads. The
instance loading it must be the only one having the WorkerId 0 (TWorkerThreadInfo::EMainThread).
The PitBoss has two main responsibilities:
    -# Maintain global data structures, accessible to all Workers to which the PitBoss
    pointer is published (through the TWorkerMainIntroductionMsg).
    Mostly the workers will access this data using PitBoss access functions which are thread-safe,
     taking special measures to guarantee this where needed.
    -# Manage Worker thread start and death: Registering Workers and
    do a best effort cleanup if they die due to some error condition.

Apart from these, the PitBoss also provides some minor but important services. For example as
all instances can have their own heap sometimes it happens that one Worker needs to do
something that might cause allocation or freeing on another workers heap, such as inserting a
sub-session into a Dealers session. So the PitBoss offer a function that can determine whether
a different heap is needed (maybe the two workers have different heaps, maybe they share one heap)
and does the switch.
It also has AddSubSession() and RemoveSubSession() methods used by the Player, assuring this
happens on the right heap.
*/
class CCommonPitBoss : public CBase,
                       public MPeerDeathObserver,
                       public MConfiguratorObserver,
                       public NetInterfaces::TInterfaceControl
	{
	friend class TWorkerThreadRegister;
public:
	IMPORT_C ~CCommonPitBoss();

protected:
	IMPORT_C CCommonPitBoss(CCommonWorkerThread* aOwnerThread);

public:
	IMPORT_C void BindMessageReceived(const CommsFW::TCFModuleName& aPeerName, CommsFW::TWorkerId aPeerId);
	IMPORT_C void ProcessWorkerIntroductionL(const TWorkerIntroductionMsg& aMsg);
	IMPORT_C void BroadcastConfigurationComplete(TConfigurationCompletionType aType);

	IMPORT_C TBool ModuleConfigurationComplete() const;
	TInt NextSessionId();

	IMPORT_C void ConstructL();

	IMPORT_C TBool IsShuttingDown() const;
	void StartShutdown();
	IMPORT_C void SessionShutdownComplete();
	IMPORT_C TBool FindOptimalDealer(CommsFW::TWorkerId aWorkerId, CCommonWorkerDealer*& aDealer);
	void ShutdownIfReady();

	IMPORT_C void PeerWorkerExiting(CommsFW::TWorkerId aWorker);

	TBool TestAndLockImmediateShutdownPresent() const;
	void ReleaseImmediateShutdownPresent() const;
	void SetImmediateShutdownPresent();
	IMPORT_C TBool TestImmediateShutdownPresent() const;

    CWorkerDataGlobals& WorkerDataGlobals() const
    	{
    	return *iWorkerDataGlobals;
    	}

	CCommonDealer* Dealer(const TWorkerThreadRegister& aRegister)
		{
		return aRegister.iDealer;
		}

	CCommonPlayer* Player(const TWorkerThreadRegister& aRegister)
		{
		return aRegister.iPlayer;
		}

    CCommonWorkerThread* OwnerThread()
    	{
    	return iOwnerThread;
		}

	IMPORT_C CCommonDealer* GetDealer(CommsFW::TWorkerId aId);
	IMPORT_C CCommonPlayer* GetPlayer(const TWorkerIntroductionMsg& aMsg);

    IMPORT_C void PostMessage(CommsFW::TWorkerId aWorkerId, CommsFW::TCFMessage& aMessage);
	CCommonDealer* Dealer(CommsFW::TWorkerId aWorkerId) const;
	CCommonPlayer* Player(CommsFW::TWorkerId aWorkerId) const;
	inline CCommonWorkerThread& WorkerThread() const;
	IMPORT_C const RThread& RThreadRef(CommsFW::TWorkerId aWorkerId) const;
	/** Resolve a Worker thread's CFModule name to its thread id (module names are far less volatile than thread assignments
	which in the future may become highly dynamic. So module names are used to couple tier ids to threads
	*/
	IMPORT_C TBool ResolveWorkerNameToId(const TDesC8& aWorkerName, CommsFW::TWorkerId& aWorkerId) const;
	/** Resolve a PlayerRole to its thread id (module names are far less volatile than thread assignments
	which in the future may become highly dynamic. So module names are used to couple tier ids to threads
	*/
	IMPORT_C TBool ResolvePlayerRoleToId(const TPlayerRole& aRoleId, CommsFW::TWorkerId& aWorkerId) const;

	IMPORT_C TBool WorkerExists(CommsFW::TWorkerId aId) const;

	// Session container manipulation - used by Player
	IMPORT_C TInt AddSubSession(CWorkerSubSession* aSubSession, CWorkerSession* aSession, TInt& aHandle);
	IMPORT_C void RemoveSubSession(TInt aHandle, CWorkerSession* aSession);

	IMPORT_C void HandleWorkerCleanupCompletionByPeer(CommsFW::TWorkerId aWorkerId, CommsFW::TWorkerId aPeerId);
	void CompleteWorkerThreadCleanup(CommsFW::TWorkerId aWorkerId);

	// MPeerDeathNotification
	IMPORT_C void OnPeerDeath(CommsFW::TWorkerId aWorkerId);
	virtual void DoOnPeerDeath(CommsFW::TWorkerId aWorkerId) = 0;
 	//Defaut RedShirt, uses PostMortemCleanupThreadEntry
	IMPORT_C virtual TInt DoCreateRedShirt(RThread& aRedShirt, CommsFW::TWorkerId aWorkerId, CCommonWorkerThread& aDeadWorker);

	// MConfiguratorObserver
	IMPORT_C void OnCPMsConfigured();
	virtual void DoOnCPMsConfigured()=0;

	TUid PropertyKey()
		{
		return iPropertyKey;
		}

	inline TSubSessionUniqueId NextSubSessionUniqueId();
	inline TSessionUniqueId NextSessionUniqueId();

	void FreeWorkerReferences(CommsFW::TWorkerId aWorkerId);

	IMPORT_C RAllocator* MaybeSwitchHeap(CommsFW::TWorkerId aForeignWorkerId);
	void AddPendingIntroductionResponse();
	void RemovePendingIntroductionResponse();

	IMPORT_C void SetFailNextForAllHeaps(TInt aFailNext);
	IMPORT_C TBool TestFailNextForAllHeaps() const;

protected:
	virtual void DoProcessWorkerIntroductionL(const TWorkerIntroductionMsg& aMsg) = 0;
	virtual void DoFreeWorkerReferences(CommsFW::TWorkerId aWorkerId) = 0;

protected:
	CCommonWorkerThread* iOwnerThread;
	CWorkerDataGlobals* iWorkerDataGlobals;
	/**
	Incremented when a request to bind to another instance has been received and decremented when a
	binding is complete.
	When the reference configurator configuration level have reached the point signifying that all core
	components have been configured and this TInt is 0, it is safe to delete data structures used only during
	startup.
	@see OnCoreComponentsConfigured()
	*/
	TInt iPendingIntroResponses;


	/**
	ETrue if the PitBoss is in the process of shutting down. Set when optional or graceful shutdown
	is received by ESock_Main from the RootServer.
	*/
	TBool iShuttingDown;

	/**
	Set ETrue when immediate shutdown is received by ESock_Main from the RootServer or if
	optional/graceful shutdown has been received at some point and the last session is closed.
	*/
	TBool iSessionShutdownComplete;

	/**
	Set ETrue when all Worker threads have finished tidying up during a shut down procedure.
	The ESock_Main instance will only shut down when this is ETrue.
	*/
	TBool iPeerShutdownComplete;

	/**
	This is used by Worker threads (as well as ESock_Main) during immediate shutdown. As a Worker
	could potentially delete a sub-session at the same time as the Dealer trying to mark it orphaned,
	these two operations need to be protected during immediate shutdown (but not normally).
	*/
	mutable RWorkerLock iImmediateShutdownLock;

	/**
	Set ETrue when immediate shutdown is received from the RootServer.
	*/
	TBool iImmediateShutdownMark;

	/**
	Monitor configuration level to know when it is safe to delete data used solely during startup/binding.
	Note that if we should support loading new Socket Server modules at run time, sometime after system start,
	this will need to be changed so the list is always available to new modules.
	*/
	CConfigurationLevelMonitor* iConfLevelMonitor;

	/**
	Session/subsession unique id counter. This value represents a next (free)
	id that will be assigned to a newly created CWorkerSubSession object.
	*/
	TUint iNextUniqueId;
	TUid iPropertyKey;

//TODO: consider if this list is worth having in Den (urel + udeb) or just in esock (urel)
//#ifdef _DEBUG
	// We want to check for leaks whenever a heap is about to be destroyed, however if a thread using
	// the heap panicked or was killed then this will give false leak indications, so we maintain this
	// list of heaps known to be "forsaken" in this way.
	class RForsakenHeapList : private RPointerArray<RAllocator>
		{
	public:
		TInt Add(RAllocator* aHeap)
			{
			if(!IsForsaken(aHeap))
				{
				return Append(aHeap);
				}
			return KErrNone;
			}
		TBool IsForsaken(const RAllocator* aHeap) const
			{
			return Find(aHeap) >= 0;
			}
		void Close()
			{
			RPointerArray<RAllocator>::Close();
			}
		};
	RForsakenHeapList iForsakenHeapList;
//#endif
	};

const TInt KResetAllocFails = -73738;

/** Helper class for switching the current thread to share that of a peer worker thread. This is necessary for heap manipulating
actions such as adding to a container owned by the peer, as well as the more obvious case of deleting an object created by them.
Construct a THeapSwitcher instance passing the identity of the peer thread to switch to using their heap. Destroy the THeapSwitcher
instance or explicitly call RevertToOwnHeap() to resume using the normal heap for the caller thread.
*/
class THeapSwitcher
	{
public:
	IMPORT_C THeapSwitcher(CCommonPitBoss& aPitBoss, const Messages::TNodeId& aTarget);
	IMPORT_C THeapSwitcher(CCommonPitBoss& aPitBoss, CommsFW::TWorkerId aForeignWorkerId);
	IMPORT_C ~THeapSwitcher();
	IMPORT_C void RevertToOwnHeap();

private:
	IMPORT_C void MaybeSwitch(CCommonPitBoss& aPitBoss, CommsFW::TWorkerId aForeignWorkerId);
	RAllocator* iPrev;
	};

/**
The Dealer is an interface to the Symbian OS server (CServer2) and as such one of its most
important possessions is the list of CSession2's kept by the CServer2. On a conceptual level
one can say that the Dealer owns the CWorkerSession's due to their inherited relationship.
Sub-sessions are also supported in the form of the CPlayerSubSession and its derivatives. These are owned
by the provider container (CSubSessionIx) in the CWorkerSession.
*/
class CCommonDealer : public CBase
	{
public:
	IMPORT_C static CCommonDealer* NewL(CCommonServer* aServer);
	IMPORT_C ~CCommonDealer();

	IMPORT_C const TDesC& ServerName() const;
	IMPORT_C void DeleteSession(CWorkerSession* aSession);
	IMPORT_C void CleanupDeadWorker(CommsFW::TWorkerId aPeerId);

	TInt SubsessionCountInPlayer(CommsFW::TWorkerId aPeerId);

	TBool CanShutdown() const
		{
		return iServer->CanShutdown();
		};

	CCommonWorkerThread& WorkerThread()
		{
		return iServer->WorkerThread();
		}

	CommsFW::TWorkerId WorkerId() const
		{
		return iServer->WorkerId();
		}

	/**
	   Returns whether the dealer is the main dealer or not.
	   A main dealer is a dealer which can receive IPC messages directly from a client.
	   Non-main dealers can not, and must have IPC messages handed to them by a main dealer.
	*/
	virtual TBool MainDealer() const
		{
		return ETrue;
		}

	TBool CanUnbindFromWorker(CommsFW::TWorkerId aWorker);
	IMPORT_C void ProcessConfigurationComplete(TConfigurationCompletionType aType);
	void ProcessShutdownRequest(CommsFW::TCFShutdownType aType);
	void ProcessSubSessions(CommsFW::TWorkerId aPeerId, CWorkerSession::TSubSessionProcessor aSubSessionProcessor, TAny* aArg);

	enum TParkReason
		{
		/** During initial configuration a client request may have an indeterminate outcome, in that it relates to a
		protocol that isn't known but might be yet to load. This would expose clients to the need to cope with a startup race
		so instead such requests are "parked" until start-up is complete and an authorative answer can be made.
		*/
		EIndeterminateDuringBoot,
		/** The hosting of tier managers by worker threads is only determined upon the first need. The actual work is done by
		the tier resolver; until this completes all requests needing this are parked
		*/
		EAwaitingTierToWorkerMapping,
		/** Some test cases concern competing client requests, such as a connection start and stop. A test harness can specify that
		a sequence of requests is parked and then released together. This is only supported when _DEBUG_SOCKET_FUNCTIONS is defined,
		ie usually only in UDEB
		*/
		EDebugParking,
		};

	IMPORT_C TInt ParkRequest(CWorkerSession* aSession, const RMessage2& aMessage, TParkReason aReason) const;	// Requests are parked by sessions, which consider servers const

#if defined (__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)
	void ReleaseDebugParkedRequests(CWorkerSession* aSess, TInt aSubSessHandle);
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

protected:
	IMPORT_C CCommonDealer(CCommonServer* aServer);
	IMPORT_C void ConstructL();
	void ReleaseParkedRequests(TParkReason aReason);

protected:
	class TParkedRequest
		{
	public:
		TParkedRequest(CWorkerSession* aSession, const RMessage2& aMessage, TParkReason aReason);
	public:
		CWorkerSession* iSession;
		RMessage2 iMessage;
		TParkReason iReason;
		};

	typedef RArray<TParkedRequest> RParkedRequestArray;
	mutable RParkedRequestArray iParkedRequests;
	CCommonServer* iServer;
	};

/**
All Worker Threads, which are not the main Dealer can contain a Dealer, a Player, or both
(often referred to as a DealerPlayer). If the Worker contains a DealerPlayer, the Dealer is
also said to be a WorkerDealer or an Optimal Dealer, as clients can create connections directly
to it and thus avoid a thread context shift between Dealer and Player when IPC messages are processed.
*/
NONSHARABLE_CLASS(CCommonWorkerDealer) : public CCommonDealer
	{
protected:
	static const TInt KMaxEligibleList = 8; //< Maximum length for queue of clients eligible to connect to WorkerDealer

public:
	IMPORT_C static CCommonWorkerDealer* NewL(CCommonServer* aServer);
	IMPORT_C ~CCommonWorkerDealer();
	IMPORT_C void AddEligiblePidL(TProcessId aId);
	IMPORT_C void RemoveEligiblePid(TProcessId aId);
	IMPORT_C TBool IsEligible(TProcessId aId);
	TBool ProcessShutdownRequest(CommsFW::TCFShutdownType aType);

	/**
	This dealer is not a main dealer, so return false. It can only receive messages from a main dealer and
	not directly from a client.
	
	@see CCommonDealer::MainDealer
	*/
	virtual TBool MainDealer() const
		{
		return EFalse;
		}

protected:
	IMPORT_C explicit CCommonWorkerDealer(CCommonServer* aServer);
	IMPORT_C void ConstructL();

private:
	/**
	CCommonWorkerDealer::XEligibleClients
	Contains a list of clients eligible to connect to a WorkerDealer.
	Uses locking in the accessor functions to prevent unpleasant situations.
	This is an X-class because it has an unusual pattern of being embedded in
	the containing C-class yet having a necessary ConstructL() and destructor.
	*/
	NONSHARABLE_CLASS(XEligibleClients)
		{
	public:
		XEligibleClients();
		~XEligibleClients();
		void ConstructL();
		void AddL(TProcessId aId);
		void Remove(TProcessId aId);
		TBool IsEligible(TProcessId aId);
	private:
		/**
		The list of IDs authorized to create a session directly on this WorkerDealer.
		@see ::AddL
		@see ::Remove
		@see ::IsEligible
		*/
		TProcessId iEligibleClients[KMaxEligibleList];

		/**
		Provide atomic access to the list (iEligibleClients)
		@see ::AddL
		@see ::Remove
		*/
		RWorkerLock iLock;
		};

private:
	//RServer2 iSessionSustainer;				// Unused, should be removed.
	XEligibleClients iEligibleClients;		//< List of clients eligible to connect to the WorkerDealer.
	};

/**
Represents the session for Player-side actions, such as tracking protocol
references and session observers. Has responsibility for handling session
close, including signalling close complete back to the Dealer
*/
class CCommonSessionProxy : public CBase
	{
private:
	enum { ELivingSession = 0x80000000 };
	
   NONSHARABLE_CLASS(CSubSessionCloseThrottle) : public CAsyncOneShot
        {
        public:
           CSubSessionCloseThrottle(CCommonSessionProxy& aProxy);
            void RunL();
        private:
            CCommonSessionProxy& iProxy;
        };
   
   static const TInt KSubSessionThrottleSize = 15; 
   friend class CSubSessionCloseThrottle;
   
public:
	IMPORT_C ~CCommonSessionProxy();

	static TInt GetLinkOffset()
		{
		return _FOFF(CCommonSessionProxy, iLink);
		}

	const CWorkerSession* Session() const
		{
		return iSession;
		}

	CCommonPlayer& Player() const
		{
		return iPlayer;
		}

	TBool IsClosing() const
		{
		return iNumSubSessClosing != ELivingSession;
		}

	void BeginSessionClose();
	IMPORT_C void NotifySubSessionDestroyed();

protected:
	IMPORT_C CCommonSessionProxy(CWorkerSession* aSession, CCommonPlayer& aPlayer);

private:
	void DeleteSubSessionsWithThrottling();

private:
	CWorkerSession* iSession;   		//< The session which is represented by proxy instance
	CCommonPlayer& iPlayer;
	TInt iNumSubSessClosing;			//< Number of subsessions in current thread to complete
	TDblQueLink iLink;
	CSubSessionCloseThrottle iSubSessionCloseThrottle;
	};

/**
class CCommonPlayer
The main responsibility is adding/removing sub-sessions from the provider container
in the related session object as well as keeping a list of all sub-sessions handled
by the Player instance (iSubSessions).
*/
class CCommonPlayer : public CBase
	{
public:
	typedef RPointerArray<CWorkerSubSession> TSubSessionContainer;

public:
	IMPORT_C ~CCommonPlayer();

	IMPORT_C void ProcessMessageL(const RSafeMessage& aMsg, CWorkerSubSession* aSubSession);
	virtual void DoProcessMessageL(const RSafeMessage& aMsg, CWorkerSubSession* aSubSession) = 0;

	void PanicClient(CWorkerSubSession& aSubSession, TInt aPanic);
	void PanicClient(CWorkerSubSession& aSubSession, const TDesC& aCategory, TInt aPanic);
	IMPORT_C void PanicClient(TInt aPanic);
	IMPORT_C void PanicClient(const TDesC& aCategory, TInt aPanic);
	
	IMPORT_C void CleanupDeadWorker(CommsFW::TWorkerId aPeerId);

	inline TPlayerRole PlayerRole() const;
	void SetPlayerRole(const TPlayerRole& aType);

	inline CCommonWorkerThread& WorkerThread() const;
	inline CommsFW::CCommsTransport& Transport() const;
	inline CommsFW::TWorkerId WorkerId() const;
	inline CCommonPitBoss& PitBoss() const;
	
	inline const RSafeMessage& SafeMessage() const;

	inline CWorkerSession* Session() const;
	void SetSession(CWorkerSession* aWorkerSession)
		{
		iSession=aWorkerSession;
		}

	CCommonWorkerThread* OwnerThread()
		{
		return iOwnerThread;
		}

	inline TSubSessionContainer& SubSessions();

	TBool CanUnbindFromWorker(CommsFW::TWorkerId aWorker);

	IMPORT_C void MaybeSetPlayerShutdownComplete(TBool aForceShutdownNow);
	virtual TBool IsPlayerShutdownComplete() = 0;

	void ProcessShutdownRequest(CommsFW::TCFShutdownType aType);

	IMPORT_C void DontCompleteCurrentRequest();
	IMPORT_C TBool ShouldCompleteCurrentRequest() const;

	IMPORT_C  void SetReturn(TInt aReturn);

	TInt PostMortemCleanup();

	IMPORT_C CWorkerSubSession* SubSession(const TSubSessionUniqueId& aSubSessionUniqueId) const;

	IMPORT_C CCommonSessionProxy* CurrentSessionProxyL();
	CCommonSessionProxy* FindOrCreateSessionProxyL(CWorkerSession* aSession);
	CCommonSessionProxy* FindSessionProxy(CWorkerSession* aSession);

protected:
	IMPORT_C CCommonPlayer(CCommonWorkerThread* aOwnerThread, TPlayerRole aPlayerRole);
	TInt  Return()
		{
		return iReturn;
		}

	void Reset()
		{
		iComplete=ETrue;
		iReturn=KErrNone;
		}

	virtual CCommonSessionProxy* DoCreateSessionProxyL(CWorkerSession* aSession) = 0;

private:
	CCommonWorkerThread* iOwnerThread;					//< Thread owning this Player
	CCommonSessionProxy* iCurrentSessionProxy;		//< Current session proxy - only valid within current ProcessL().
	TDblQue<CCommonSessionProxy> iSessionProxies;		//< All session proxies belonging to this Player
	CWorkerSession* iSession;						//< Current session, which might possibly belong to Dealer in different thread - only valid within current ProcessL().
	TSubSessionContainer iSubSessions;				//< All sub-sessions belonging to this Player.
	TPlayerRole iPlayerRole;
	const RSafeMessage* iCurrentMessage;				//< Current RMessage - only valid within current ProcessL(). 

protected:
	TBool iComplete;								//< Whether to complete the current message at the end of ProcessL() - only valid within current ProcessL().
	TInt  iReturn;									//< Return code to use for completing current RMessage - only valid within current ProcessL().
	};


#include <elements/sd_roles.inl>

} //namespace Den

#endif
//SYMBIAN_DEN_ROLES_H



