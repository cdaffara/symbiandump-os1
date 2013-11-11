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
 @internalTechnology
*/

#ifndef SYMBIAN_DEN_THREAD_H
#define SYMBIAN_DEN_THREAD_H

#include <cfextras.h>
#include <cfshared.h>
#include <elements/cftransport.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenThredH, "ElemSvrDenThredH");
#endif

namespace CommsFW
{
struct TCFModuleInfo;
class TCFBindMsg;
class TCFUnbindMsg;
class TCFShutdownMsg;
class TCFForwardMsg;
}

namespace Den
{
class CWorkerSession;
class CWorkerRootServChannelHandler;
class CCommonDealer;
class CCommonWorkerDealer;
class CCommonPlayer;
class CCommonPitBoss;
class CShutdownWatchDog;
class CCommonWorkerThread;
class CPeerDeathNotifier;
class TPlayerRole;
class TWorkerIntroductionMsg;

class TWorkerThreadPublicInfo : public CommsFW::TWorkerThreadDataBase
	{
public:
    enum TType
        {
	    ENullWorkerId      = 0xFFFF,     //< Non-existent worker
		EMainThread        = 0,		//< the Pit Boss and Main Dealer run here
		EFirstPlayerThread = 1,	//< Lowest possible player id
		EMaxWorkerThreadId = CommsFW::KMaxWorkerId	//< Highest possible player id (NB: this is an inclusive bound. Be very careful in array-bound contexts!)
		};

	//RServer2 iSessionSustainer;	//< Used by the PitBoss to keep open a handle to a dealer so it can clean up its resources gracefully before closing the server, if the dealer thread dies.
	CCommonWorkerThread* iWorker;
	};

/**
@class TWorkerThreadRegister
Structure containing information pertaining to a particular worker thread.
*/
NONSHARABLE_CLASS(TWorkerThreadRegister) : public TWorkerThreadPublicInfo
	{
	friend class CCommonPitBoss;
	//friend class CCommonWorkerThread;

public:
	TWorkerThreadRegister();

private:
	void Clear();
	void PeerCleanupPending(CommsFW::TWorkerId aPeerId);
	void PeerCleanupCompleted(CommsFW::TWorkerId aPeerId);
	TBool AllPeerCleanupsCompleted() const;

private:
	CPeerDeathNotifier*	iPeerDeathNotifier; //< When the PitBoss binds to the worker thread this will be set up to monitor the thread, in case it dies.
	TBool iShuttingDown;					//< Unused, should be removed.
	CCommonDealer* iDealer;						//< Dealer instance of the worker thread (if exists).
	CCommonPlayer* iPlayer;						//< Player instance of the worker thread (if exists).
	CommsFW::TCFModuleName iModuleName;

private:
	/**
	If a worker thread  dies the Pitboss will set a bit corresponding to the dead worker thread ID (/number)
	in iPendingPeerCleanups in the TWorkerThreadRegister instance for each other worker and send them a TWorkerCleanupDeadPeerMsg.
	The worker threads will respond by sending TWorkerMsg::ECleanupDeadPeerResp back when they have cleaned up anything
	related to the dead worker and the PitBoss will reset the bit in their TWorkerThreadRegister instance.
	@see HandleWorkerCleanupCompletionByPeer()
	*/
	TUint32 iPendingPeerCleanups;

#ifdef _DEBUG
	TBool iHasGlobalAllocFails;
#endif
	};

typedef CommsFW::CWorkerThreadRegister<TWorkerThreadRegister, CommsFW::KMaxWorkerId> CWorkerRegister;


/**
@class CCommonWorkerThread
The Worker Thread is the holder of the relevant ESock objects in the CPM instance,
e.g. the PitBoss, the Dealer and the Player (depending on configuration). It is also
in charge of inter-thread communication as it holds the object enabling communication
with the RootServer (CWorkerRootServChannelHandler) and a list of objects for
communication with other Worker threads (CWorkerTransport).

@see CWorkerRootServChannelHandler
@see CWorkerTransport
*/
class CCommonWorkerThread : public CBase, public CommsFW::MLegacyMessageReceiver
/**
@internalComponent
*/
	{
	friend class CWorkerRootServChannelHandler;

public:
	IMPORT_C ~CCommonWorkerThread();

	/**
	@return the pointer to the Dealer in this thread. This method might return NULL if the worker thread doesnt have a Dealer.
	*/
	CCommonDealer* Dealer() const
		{
		return iDealer;
		}
	IMPORT_C CCommonWorkerDealer* WorkerDealer() const;

	/**
	@return the pointer to the Player in this thread. This method might return NULL	if the worker thread doesnt have a Player.
	*/
	CCommonPlayer* Player() const
		{
		return iPlayer;
		}

	CommsFW::TWorkerId WorkerId() const
		{
		return iWorkerId;
		}

	Messages::CGlobals&	TransportGlobals()
		{
		__ASSERT_DEBUG(iGlobals, User::Panic(KSpecAssert_ElemSvrDenThredH, 1));
		return *iGlobals;
		}

	/**
	Return the pointer to the domain PitBoss. This will be initialised when:
	    -# If this is Main thread: In CCommonWorkerThread::ConstructL(TCFModuleInfo* aModuleInfo).
		-# In other common worker threads: When receiving TWorkerMsg::EMainIntroduction.
	*/
	CCommonPitBoss& PitBoss() const
		{
		__ASSERT_DEBUG(iPitBoss, User::Panic(KSpecAssert_ElemSvrDenThredH, 2));
		return *iPitBoss;
		}

	/**
	Use this to discover whether this Worker Thread is EMainThread ("Esock_Main") which is
	the main Dealer also containing the PitBoss.
	@see TWorkerThreadInfo
	*/
	TBool IsMainThread()
	    {
	    return WorkerId()==TWorkerThreadRegister::EMainThread;
	    }

	TWorkerThreadRegister* WorkerProperties(CommsFW::TWorkerId aWorker)
		{
		return iWorkerRegister->GetWorkerGlobals(aWorker);
		}

	IMPORT_C void SetDealerShutdownComplete(TBool aComplete);
	TBool DealerShutdownComplete() const
		{
		return iDealerShutdownComplete;
		}

	IMPORT_C void SetPlayerShutdownComplete(TBool aComplete);
	TBool PlayerShutdownComplete() const
		{
		return iPlayerShutdownComplete;
		}

	IMPORT_C TBool ShuttingDown() const;
	IMPORT_C void SetShuttingDown();
	IMPORT_C void SessionShutdownComplete();
	IMPORT_C void MaybeTriggerThreadShutdownCallback();
	void TriggerThreadShutdownCallback();
	IMPORT_C void DestroyDealer();
	IMPORT_C  void DestroyPlayer();
	void DropTransportToPeer(TInt aPeer);
	IMPORT_C TBool MaybeCompleteUnbindings();

	// RS control binding processing & message forwarding
	void CFUnbindMessageReceived(const CommsFW::TCFUnbindMsg& aMsg);
	void CFShutdownMessageReceived(const CommsFW::TCFShutdownMsg& aMsg);

	// Test whether a message
	TBool PeerReachable(CommsFW::TWorkerId aPeerId) const
		{
		return iTransport->PeerReachable(aPeerId);
		}

	// Send a message to a peer
    IMPORT_C void PostMessage(CommsFW::TWorkerId aWorkerId, CommsFW::TCFMessage& aMessage);
	CommsFW::CCommsTransport* Transport() const
		{
		return iTransport;
		}

	//< Thread nominates self as the default optimal dealer
	TBool DefaultOptimalDealer() const
		{
		return iDefaultOptimalDealer;
		}

	//
	IMPORT_C void DispatchL(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId);
	virtual TBool DoDispatchL(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId)=0;
	IMPORT_C void OnDispatchLeave(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId, TInt aFirstDispatchLeaveReason);

	// Process a non-internal message from a peer worker. Returns error code
	IMPORT_C void IncProlongBindingLife();
	IMPORT_C void DecProlongBindingLife();
	IMPORT_C void CompleteSessionClose(CWorkerSession* aSession);
	IMPORT_C void ConstructL(CommsFW::TCFModuleInfo& aModuleInfo, CShutdownWatchDog& aShutdownWatchDog);
	//Default post mortem cleanup handler
	IMPORT_C static TInt PostMortemCleanupThreadEntry(TAny* aArg);

protected:
	virtual CCommonPitBoss* DoCreatePitBossL(CCommonWorkerThread* aWorkerThread)=0;
	virtual CCommonPlayer* DoCreatePlayerL(CCommonWorkerThread* aWorkerThread, TPlayerRole aPlayerRole)=0;
	virtual CCommonDealer* DoCreateDealerL(CCommonWorkerThread* aWorkerThread, TPlayerRole aPlayerRole)=0;
	virtual void DoCompleteUnbinding(CommsFW::TWorkerId aWorker) = 0;
	virtual void DoSetShuttingDown() = 0;
	virtual void DoPostMortemCleanup() = 0;
	virtual void CFBindMessageReceived(const CommsFW::TCFBindMsg& aMsg) = 0;
	virtual void CFMessageForward(const CommsFW::TCFForwardMsg& aMessage) = 0;

	virtual void DoProcessWorkerIntroductionL(const TWorkerIntroductionMsg& aMessage) = 0;

	IMPORT_C void SendIntroMessage(const CommsFW::TCFModuleName& aPeerName, CommsFW::TWorkerId aPeerId);
	virtual void DoFillIntroMessage(CommsFW::TWorkerId aPeerId, TWorkerIntroductionMsg& aIntroMsg) = 0;

	IMPORT_C CCommonWorkerThread();
	IMPORT_C static void DeleteHBufC8(TAny* aHBufC);

	IMPORT_C void DetermineRoleL(HBufC8* aIniData, TBool& aIsDealer, TBool& aIsPlayer, TPlayerRole &aPlayerRole);
	virtual void DoDeterminePlayerRoleL(HBufC8* aIniData, TPlayerRole &aPlayerRole)=0;

	void ProcessIniDataL();
	IMPORT_C TInt DecodePeerId(const CommsFW::TCFSubModuleAddress* aSubModule1, const CommsFW::TCFSubModuleAddress* aSubModule2, CommsFW::TWorkerId& aPeerId);
	IMPORT_C void MaybeCompleteUnbinding(CommsFW::TWorkerId aWorker, TBool aPeerIsDead);

protected:
	/** This is the handler used for bi-directional communication with the Root Server. */
	CWorkerRootServChannelHandler* iChannelHandler;

	CWorkerRegister* iWorkerRegister;
	CommsFW::CCommsTransport* iTransport;

	/**
	Indentification of this thread. No other instance must have the same id.
	@see TWorkerThreadInfo
	*/
	CommsFW::TWorkerId iWorkerId;

	CCommonDealer* iDealer;
	CCommonPlayer* iPlayer;
	CCommonPitBoss* iPitBoss;

	TBool iDefaultOptimalDealer;


	/** Set when a CommsFW::TCFShutdownMsg is received from the Root Server. */
	TBool iWorkerShuttingDown;

	/**
	Set by the Dealer instance to signal to the Worker Thread that it has
	finished with the shutdown bookkeeping and is ready to be deleted.
	*/
	TBool iDealerShutdownComplete;

	/**
	Set by the Player instance to signal to the Worker Thread that it has
	finished with the shutdown bookkeeping and is ready to be deleted.
	*/
	TBool iPlayerShutdownComplete;

	/**
	If the value of this TInt is larger than 0 any unbind requests will not be served,
	but postponed until iProlongBindingLife is 0. This is to ensure that if e.g. a
	TWorkerMsg::ECleanupDeadPeer or TPlayerMsg::ESessionClose is received they will be fully
	served before completing any unbind and thus deleting channel handlers.
	*/
	TInt iProlongBindingLife;

	CShutdownWatchDog* iShutdownWatchDog;
	Messages::CGlobals* iGlobals;

public:
#ifdef _DEBUG
	RAllocator::TAllocFail AllocFailType() const
		{
		return iFailType;
		}
	TInt AllocFailRate() const
		{
		return iFailRate;
		}
#endif

//These must not be conditional (BC)
private:
	RAllocator::TAllocFail iFailType;
	TInt iFailRate;
	};

NONSHARABLE_CLASS(CShutdownWatchDog) : public CPeriodic
/**
@internalComponent
*/
	{
public:
	IMPORT_C void Shutdown(TBool aImmediate);
	IMPORT_C static CShutdownWatchDog* NewL(CCommonWorkerThread* aWorker, const TCallBack& aShutDownCb);

private:
	CShutdownWatchDog(CCommonWorkerThread* aWorker, const TCallBack& aShutDownCb);
	static TInt TryShutdown(TAny* aSelf);

private:
	TBool iImmediate;
	CCommonWorkerThread* iWorker;
	TCallBack iShutDownCb;
	};

} //namespace Den

#endif
//SYMBIAN_DEN_THREAD_H


