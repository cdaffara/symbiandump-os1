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
    
#include <e32base.h>
#include <cfshared.h>
#include "sd_log.h"
#include "sd_roles.h"
#include "sd_msgs.h"
#include "sd_std.h"
#include "sd_errors.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenPlayrC, "ElemSvrDenPlayrC");
#endif

using namespace Den;
using namespace CommsFW;

//
// CSockSessionProxy
//

EXPORT_C CCommonSessionProxy::CCommonSessionProxy(CWorkerSession* aSession, CCommonPlayer& aPlayer)
:	iSession(aSession),
	iPlayer(aPlayer),
	iNumSubSessClosing(ELivingSession),
	iSubSessionCloseThrottle(*this)
	{
	//COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CSockSessionProxy %08x:\tCSockSessionProxy(), iSockSession %08x"), this, iSession) );
	}

EXPORT_C CCommonSessionProxy::~CCommonSessionProxy()
	{
	//LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSessionProxy %08x:\t~CSockSessionProxy(), iSockSession %08x"), this, iSockSession) );
	if(iLink.iNext)
		{
		iLink.Deque();
		}
	}

void CCommonSessionProxy::BeginSessionClose()
	{
	COMMONLOG((Player().WorkerId(),KECommonBootingTag, _L8("CCommonSessionProxy %08x:\tBeginSessionClose(), iSockSession %08x"), this, iSession) );
	/* Only do something if the message is within the deadline and we're sure
	   the session pointer is safe to use */
	CCommonWorkerThread& worker = iPlayer.WorkerThread();
	worker.IncProlongBindingLife();
	__ASSERT_DEBUG(!IsClosing(), User::Panic(KSpecAssert_ElemSvrDenPlayrC, 1));
	iNumSubSessClosing = 1;	// dummy subsession to prevent premature suicide during this close loop

	
	DeleteSubSessionsWithThrottling();
	}

void CCommonSessionProxy::DeleteSubSessionsWithThrottling()
    {
    // Why Throttle?  The original scenario is a process which opens a large number of sockets (100 in the
    // original case) and terminates without closing them, leaving BeginSessionClose() to clean them up.
    // In addition, the worker in question (pdummy1) synchronously deletes subsessions (1) and uses a shared
    // shared heap configuration (i.e. limited memory).  As each deletion results in a message being sent
    // (to an SCPR), and there is no opportunity to drain these due to the synchronous deletion, the transport
    // queue overflows and cannot be grown resulting in a panic.  So use a (*low* priority) active object to
    // delete a limited number of subsessions per RunL().
    //
    // (1) a call to DeleteMe() results in an immediate upcall to NotifySubSessionDestroyed(). 
    
    TInt count = KSubSessionThrottleSize;
    
    // The object container is stored as a packed array, so working backwards through it avoids invalidating
    // the iterator when removing entries (and as a bonus is more efficient)

    CCommonPlayer::TSubSessionContainer& subSessions(iPlayer.SubSessions());
    for(TInt i = subSessions.Count() - 1; i >= 0; --i)
        {
        CWorkerSubSession* subSession = subSessions[i];
        if(subSession->Session() == iSession)
            {
            ++iNumSubSessClosing;
            if(!subSession->IsClosing())
                {
                subSession->DeleteMe();
                // Throttle the deletions as appropriate
                if (--count <= 0)
                    {
                    COMMONLOG((Player().WorkerId(),KECommonBootingTag, _L8("CCommonSessionProxy %08x:\tDeleteSubSessionBunch(): throttled subsession deletion"), this) );
                    // Re-prime the one shot
                    iSubSessionCloseThrottle.Call();
                    return;
                    }
                }
            }
        }
    NotifySubSessionDestroyed();    // all, done, remove the dummy subsession
    }

EXPORT_C void CCommonSessionProxy::NotifySubSessionDestroyed()
	{
	//COMMONLOG((Player().WorkerId(),KECommonBootintgTag, _L8("CCommonSessionProxy %08x:\tNotifySubSessionDestroyed(), iSockSession %08x"), this, iSession) );
	if(IsClosing() &&--iNumSubSessClosing <= 0)
	    {
        __ASSERT_DEBUG(iNumSubSessClosing == 0, User::Panic(KSpecAssert_ElemSvrDenPlayrC, 2));
        CCommonWorkerThread& worker = iPlayer.WorkerThread();
        worker.CompleteSessionClose(iSession);
        delete this;
	    }
	}

//
// CCommonSessionProxy::CSubSessionCloseThrottle
//

CCommonSessionProxy::CSubSessionCloseThrottle::CSubSessionCloseThrottle(CCommonSessionProxy& aProxy)
  : CAsyncOneShot(EPriorityLow), iProxy(aProxy)     // This must be low priority!
    {
    }

void CCommonSessionProxy::CSubSessionCloseThrottle::RunL()
    {
    // Delete some more subsessions
    iProxy.DeleteSubSessionsWithThrottling();
    }

//
// CCommonPlayer
//

/**
The Player destructor doesn't have much to do as a lot of the cleanup is done during the
normal shutdown routines. Here the Player merely deletes all sub-sessions it owns.
*/
EXPORT_C CCommonPlayer::~CCommonPlayer()
	{
	// The object container is stored as a packed array, so working backwards through it avoids invalidating
	// the iterator when removing entries (and as a bonus is more efficient)
	COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CCommonPlayer::~CCommonPlayer()")));

	for(TInt i = iSubSessions.Count() - 1; i >= 0; --i)
		{
		CWorkerSubSession* subSession = iSubSessions[i];
		COMMONLOG((WorkerId(),KECommonBootingTag, _L8("-- destroying %08x"), subSession));
		subSession->DeleteMe();
		}

	// Subsessions belong to sessions and get removed when those close. The only time a Player should be going down
	// with leftover subsessions is an immediate shutdown, else either a session wasn't destroyed properly or a subsession
	// got leaked off it. We do this after the cleanup loop above so the ptrs are helpfully listed
	__ASSERT_DEBUG((iSubSessions.Count() == 0 || PitBoss().TestImmediateShutdownPresent()), User::Panic(KSpecAssert_ElemSvrDenPlayrC, 3));

	iSubSessions.ResetAndDestroy();
	}

EXPORT_C CCommonPlayer::CCommonPlayer(CCommonWorkerThread* aOwnerThread, TPlayerRole aPlayerRole)
:	iOwnerThread(aOwnerThread),
	iPlayerRole(aPlayerRole)
	{
	COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CCommonPlayer::CCommonPlayer()")));
	iSessionProxies.SetOffset(CCommonSessionProxy::GetLinkOffset());
	}

/**
MultiTool function to return the current session proxy for a session, but also to create it
if it doesn't exist. This avoids having the same pattern in multiple places where we first check
to see if it exists then creates it if not.
*/
EXPORT_C CCommonSessionProxy* CCommonPlayer::CurrentSessionProxyL()
	{
	if(!iCurrentSessionProxy)
		{
		iCurrentSessionProxy = FindOrCreateSessionProxyL(Session());
		}
	return iCurrentSessionProxy;
	}

CCommonSessionProxy* CCommonPlayer::FindOrCreateSessionProxyL(CWorkerSession* aSession)
	{
	CCommonSessionProxy* proxy = FindSessionProxy(aSession);
	if(proxy==NULL)
		{
		// No existing proxy; create a new one and append to list
		proxy = DoCreateSessionProxyL(aSession);
		iSessionProxies.AddLast(*proxy);
		}
	return proxy;
	}

CCommonSessionProxy* CCommonPlayer::FindSessionProxy(CWorkerSession* aSession)
	{
	TDblQueIter<CCommonSessionProxy> iter(iSessionProxies);
	CCommonSessionProxy* proxy;
	while((proxy = iter++) != NULL)
		{
		if(proxy->Session() == aSession)
			{
			break;
			}
		}
	return proxy;
	}

/**
The Player can unbind from another worker thread if it doesn't have any sub-sessions
belonging to a session in the peer's Dealer and there are no interface cookies left registered
at all
*/
TBool CCommonPlayer::CanUnbindFromWorker(TWorkerId aWorker)
	{
	COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CCommonPlayer::CanUnbindFromWorker(%d): %d subsess"), aWorker, iSubSessions.Count()));

	TInt regItfCount = WorkerThread().Transport()->RegisteredCount();
	TInt persistentItfCount = Messages::TlsGlobals::Get().PersistentItfCount();
	if(regItfCount > persistentItfCount)
		{
		COMMONLOG((WorkerId(), KECommonBootingTag, _L8("-- can't; %d interfaces still registered (%d transient)"), regItfCount, regItfCount - persistentItfCount) );
		return EFalse;
		}

	for(TInt idx = iSubSessions.Count() - 1; idx >= 0; --idx)
		{
		const CWorkerSubSession* ss = iSubSessions[idx];
		COMMONLOG((WorkerId(),KECommonBootingTag, _L8("-- subsess %08x worker=%d"), ss, ss->Session()->WorkerId()));
		if(ss->Session()->WorkerId() == aWorker)
			{
			COMMONLOG((WorkerId(),KECommonBootingTag, _L8("-- can't; subsess %08x belongs to this peer"), ss) );
			return EFalse;
			}
		}
    
	if (!iSessionProxies.IsEmpty())
        {
        COMMONLOG((WorkerId(), KECommonBootingTag, _L8("-- can't; session proxies still present")));
        return EFalse;
        }
    

    return ETrue;
	}

EXPORT_C void CCommonPlayer::MaybeSetPlayerShutdownComplete(TBool aForceShutdownNow)
	{
	//A forced shutdown trumps all other considerations
	TBool shutdownComplete = aForceShutdownNow
	        || (SubSessions().Count() == 0  && iSessionProxies.IsEmpty() && IsPlayerShutdownComplete());

	COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CPlayer::MaybeSetPlayerShutdownComplete(), shutdownComplete = %d [forced=%d, #subSess=%d]"),
		shutdownComplete, aForceShutdownNow, SubSessions().Count()));

	WorkerThread().SetPlayerShutdownComplete(shutdownComplete);
	}

EXPORT_C CWorkerSubSession* CCommonPlayer::SubSession(const TSubSessionUniqueId& aSubSessionUniqueId) const
	{
	__ASSERT_DEBUG(aSubSessionUniqueId!=0, User::Panic(KDenFaultPanic, ECommonBadSubSessionUniqueId));
	CWorkerSubSession* ss = NULL;
	for (TInt i = iSubSessions.Count() - 1; i>=0 && ss==NULL ; i--)
		{
		if (iSubSessions[i]->UniqueId()==aSubSessionUniqueId)
			{
			ss = iSubSessions[i];
			}
		}
	return ss;
	}

EXPORT_C void CCommonPlayer::ProcessMessageL(const RSafeMessage& aMessage, CWorkerSubSession* aSubSession)
	{
	SetSession(static_cast<CWorkerSession*>(aMessage.Session()));
	iCurrentSessionProxy = NULL;
	Reset();
	iCurrentMessage = &aMessage;

	DoProcessMessageL(aMessage,aSubSession);
	}

/**
Panic the client in response to an operation on an existing subsession, after
which the request mustn't be completed
*/
void CCommonPlayer::PanicClient(CWorkerSubSession& /*aSubSession*/, TInt aPanic)
	{
	SafeMessage().PanicClient(KDenFaultPanic, aPanic);
	DontCompleteCurrentRequest();
	}

/**
Panic the client in response to an operation on an existing subsession, after
which the request mustn't be completed

This version allows panic categories to be passed in to support expected legacy
behaviour.
*/
void CCommonPlayer::PanicClient(CWorkerSubSession& /*aSubSession*/, const TDesC &aCategory, TInt aPanic)
	{
	SafeMessage().PanicClient(aCategory, aPanic);
	DontCompleteCurrentRequest();
	}

EXPORT_C void CCommonPlayer::PanicClient(TInt aPanic)
	{
	SafeMessage().PanicClient(KDenFaultPanic, aPanic);
	DontCompleteCurrentRequest();
	}

EXPORT_C void CCommonPlayer::PanicClient(const TDesC& aCategory, TInt aPanic)
	{
	SafeMessage().PanicClient(aCategory, aPanic);
	DontCompleteCurrentRequest();
	}

EXPORT_C void CCommonPlayer::DontCompleteCurrentRequest()
	{
	iComplete=EFalse;
	}

EXPORT_C TBool CCommonPlayer::ShouldCompleteCurrentRequest() const
	{
	return iComplete;
	}

EXPORT_C void CCommonPlayer::SetReturn(TInt aReturn/*, TInt anError, TUid anOwner*/)
	{
	iReturn=aReturn;

	}

/**
Remove all subsessions belonging to sessions of the nominated worker. No locks required; only the
Player accesses the object container.
@see TWorkerMsg::ECleanupDeadPeer
*/
EXPORT_C void CCommonPlayer::CleanupDeadWorker(TWorkerId aWorkerId)
	{
	(void)aWorkerId;
	//TBDAA - it's in ss_glob.h ASSERT_HOME_THREAD;

	// The object container is stored as a packed array, so working backwards through it avoids invalidating
	// the iterator when removing entries (and as a bonus is more efficient)
	for(TInt i = iSubSessions.Count() - 1; i >= 0; --i)
		{
		iSubSessions[i]->CleanupDeadWorker(aWorkerId);
		}
	}
/**
If an incoming shutdown request is of type EImmediate, informs the
Worker Thread that Player shutdown is complete, otherwise do nothing here.
*/
void CCommonPlayer::ProcessShutdownRequest(TCFShutdownType aType)
	{
	COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CPlayer::ProcessShutdownRequest(%d)"), aType));
	WorkerThread().SetPlayerShutdownComplete(aType == CommsFW::EImmediate);
	}

/**
Walk through all of the subsessions, telling each to complete all blocked requests
with KErrServerAbort. We don't attempt to free them - although with a shared
heap this could reduce the amount of leakage it seems likely that the more we do
with these data structures, the greater the risk that the contagion spreads to
other users of the heap. Clients that attempt almost any further operation after KErrAbort
will get panicked by the Dealer in the usual bad handle way - this is harsh but fair, and
better than simply leaving them hung.
*/
TInt CCommonPlayer::PostMortemCleanup()
	{
	// No locks are required - we're only here because the owner has died
	for(TInt i = 0; i < iSubSessions.Count(); ++i)
		{
		CWorkerSubSession* subSession = static_cast<CWorkerSubSession*>((iSubSessions[i]));
		subSession->FinalCompleteAllBlockedMessages(KErrAbort);
		}
	return KErrNone;
	}

