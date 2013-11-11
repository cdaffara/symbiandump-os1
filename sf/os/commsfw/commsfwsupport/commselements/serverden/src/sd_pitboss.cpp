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

#include "sd_log.h"
#include "sd_roles.h"
#include "sd_msgs.h"
#include "sd_objectbroker.h"
#include <rsshared.h>
#include <elements/cfmacro.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenPitBsC, "ElemSvrDenPitBsC");
#endif

using namespace Den;
using namespace CommsFW;

//
// CCommonPitBoss
//


EXPORT_C void CCommonPitBoss::ConstructL()
	{
	User::LeaveIfError(iImmediateShutdownLock.CreateLocal());
	iWorkerDataGlobals = CWorkerDataGlobals::NewL();
	iConfLevelMonitor = CConfigurationLevelMonitor::NewL(this);
	TWorkerThreadRegister* mainProperties = WorkerDataGlobals().GetWorkerGlobals(TWorkerThreadPublicInfo::EMainThread);
	mainProperties->iHeap = &User::Heap();
	mainProperties->iDealer = OwnerThread()->Dealer();
	mainProperties->iPlayer = OwnerThread()->Player();
	}

EXPORT_C CCommonDealer* CCommonPitBoss::GetDealer(TWorkerId aId)
	{
	return WorkerDataGlobals().GetWorkerGlobals(aId)->iDealer;
	}

EXPORT_C CCommonPlayer* CCommonPitBoss::GetPlayer(const TWorkerIntroductionMsg& aMsg)
	{
	const TWorkerThreadPublicInfo& msgInfo = aMsg.WorkerInfo();
	TWorkerThreadRegister& workerReg = *WorkerDataGlobals().GetWorkerGlobals(msgInfo.iWorkerId);
	return workerReg.iPlayer;
	}

EXPORT_C TBool CCommonPitBoss::ModuleConfigurationComplete() const
	{
	return iConfLevelMonitor == NULL;
	}

EXPORT_C TBool CCommonPitBoss::IsShuttingDown() const
	{
	return iShuttingDown;
	}

void CCommonPitBoss::StartShutdown()
	{
	iShuttingDown = ETrue;
	}

EXPORT_C void CCommonPitBoss::SessionShutdownComplete()
	{
	iSessionShutdownComplete = ETrue;
	ShutdownIfReady();
	}

void CCommonPitBoss::ShutdownIfReady()
	{
	__ASSERT_DEBUG(iOwnerThread->WorkerId() == TWorkerThreadPublicInfo::EMainThread, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 1));
	COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::ShutdownIfReady() - iShuttingDown == %d, iSessionShutdownComplete = %d, iPeerShutdownComplete = %d"),
		iShuttingDown, iSessionShutdownComplete, iPeerShutdownComplete));
	if(iShuttingDown && iSessionShutdownComplete && iPeerShutdownComplete)
		{
		iOwnerThread->TriggerThreadShutdownCallback();
		}
	}

/**
Immediate shutdowns expose a number of additional race risks; in the face of one happening to
any thread at all we stop attempting certain kinds of cleanup. This should be acceptable
behaviour since at it will be the whole of ESock shutting down.
To avoid races between the testing and setting of this state, testing for it gains a lock
which must be explicitly released.
@see CPitBoss::ReleaseImmediateShutdownPresent()
*/
TBool CCommonPitBoss::TestAndLockImmediateShutdownPresent() const
	{
	iImmediateShutdownLock.Wait();
	return iImmediateShutdownMark;
	}

void CCommonPitBoss::ReleaseImmediateShutdownPresent() const
	{
	iImmediateShutdownLock.Signal();
	}

void CCommonPitBoss::SetImmediateShutdownPresent()
	{
	iImmediateShutdownLock.Wait();
	iImmediateShutdownMark = ETrue;
	iImmediateShutdownLock.Signal();
	}

/**
Tests whether immediate shutdown is signaled. This function does not
lock and is used where this knowledge is desired but no action is taken that might
race with other threads.
@see CPitBoss::TestAndLockImmediateShutdownPresent()
@see CPitBoss::ReleaseImmediateShutdownPresent()
*/
EXPORT_C TBool CCommonPitBoss::TestImmediateShutdownPresent() const
	{
	return iImmediateShutdownMark;
	}

EXPORT_C CCommonPitBoss::~CCommonPitBoss()
	{
	delete iConfLevelMonitor;

	/* The reason for this bit of code is that when we shut down there's a race between the Pitboss
	and an optimal Dealer where the Pitboss doesn't discover the optimal dealer is going down (PB
	goes down first) and thus doesn't close the resources it have on it before the Pitboss is destroyed.
	*/
	for(TWorkerId id = TWorkerThreadPublicInfo::EFirstPlayerThread; id <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++id)
		{
		if(WorkerExists(id))
			{
			FreeWorkerReferences(id);
			}
		}
	delete iWorkerDataGlobals;
	iImmediateShutdownLock.Close();
//#ifdef _DEBUG
	iForsakenHeapList.Close();
//#endif
	}

EXPORT_C CCommonPitBoss::CCommonPitBoss(CCommonWorkerThread* aOwnerThread)
:	iOwnerThread(aOwnerThread),
	iNextUniqueId(1)
	{
	__ASSERT_DEBUG(iOwnerThread->WorkerId() == TWorkerThreadPublicInfo::EMainThread, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 2));
	__ASSERT_DEBUG(iOwnerThread->Player() == NULL, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 3));
	}

EXPORT_C void CCommonPitBoss::PostMessage(CommsFW::TWorkerId aWorkerId, CommsFW::TCFMessage& aMessage)
	{
	iOwnerThread->PostMessage(aWorkerId, aMessage);
	}

/**
Check whether a worker Id is legal and a worker with that Id is installed.
*/
EXPORT_C TBool CCommonPitBoss::WorkerExists(TWorkerId aId) const
	{
	return WorkerDataGlobals().WorkerPresent(aId);
	}


/**
Given the "local" worker thread's id (normally but not necessarily the current worker
thread and that of another "foreign" worker thread, if they have different heaps then
switch heaps and return the previous one, otherwise return NULL.
*/
EXPORT_C RAllocator* CCommonPitBoss::MaybeSwitchHeap(TWorkerId aForeignWorkerId)
	{
	const TWorkerThreadPublicInfo& foreignInfo = *WorkerDataGlobals().GetWorkerGlobals(aForeignWorkerId);
	__ASSERT_DEBUG(foreignInfo.iHeap, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 4));
	RHeap* heap = &User::Heap();
	if(heap != foreignInfo.iHeap)
		{
		COMMONLOG((iOwnerThread->WorkerId(), KECommonSessDetailTag, _L8("CPitBoss::MaybeSwitchHeap - Switching heap to %08x."),foreignInfo.iHeap));
		return User::SwitchAllocator(foreignInfo.iHeap);
		}
	else
		{
		COMMONLOG((iOwnerThread->WorkerId(), KECommonSessDetailTag, _L8("CPitBoss::MaybeSwitchHeap - No heap switch happened - Heap %08x."),heap));
		}

	return NULL;
	}

/**
Called by any Player/thread to add a sub-session to a session. It will switch the local heap to that
of the peer before performing operations on the session pointer. It is essential that the session lock is used
around this call.
@see CSubSessionIx::Lock
@see CSubSessionIx::UnLock
*/
EXPORT_C TInt CCommonPitBoss::AddSubSession(CWorkerSubSession* aSubSession, CWorkerSession* aSession, TInt& aHandle)
	{
	RAllocator* prevAllocator = MaybeSwitchHeap(aSession->WorkerId());
	TInt err = aSession->SubSessions().Add(static_cast<CWorkerSubSession*>(aSubSession), aHandle);
	COMMONLOG((iOwnerThread->WorkerId(),KECommonSessDetailTag, _L8("CPitBoss::AddSubSession(%08x, %08x, => %08x)"), aSubSession, aSession, aHandle));

	if(prevAllocator)
		{
		User::SwitchAllocator(prevAllocator);
		}
	return err;
	}

/**
Called by any Player/thread to remove a sub-session from a session. It will switch the local heap to that
of the peer before performing operations on the session pointer. It is essential that the session lock is used
around this call.
@see CSubSessionIx::Lock
@see CSubSessionIx::UnLock
*/
EXPORT_C void CCommonPitBoss::RemoveSubSession(TInt aHandle, CWorkerSession* aSession)
	{
	RAllocator* prevAllocator = MaybeSwitchHeap(aSession->WorkerId());
	COMMONLOG((iOwnerThread->WorkerId(),KECommonSessDetailTag, _L8("CPitBoss::RemoveSubSession(%08x, %08x)"), aHandle, aSession));
	VERIFY(aSession->SubSessions().Remove(aHandle) != NULL);
	if(prevAllocator)
		{
		User::SwitchAllocator(prevAllocator);
		}
	}

CCommonDealer* CCommonPitBoss::Dealer(TWorkerId aWorkerId) const
	{
	return WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iDealer;
	}

CCommonPlayer* CCommonPitBoss::Player(TWorkerId aWorkerId) const
	{
	return WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iPlayer;
	}

EXPORT_C const RThread& CCommonPitBoss::RThreadRef(TWorkerId aWorkerId) const
	{
	__ASSERT_DEBUG(aWorkerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 5));
	__ASSERT_DEBUG(WorkerExists(aWorkerId), User::Panic(KSpecAssert_ElemSvrDenPitBsC, 6));	// can't do this to an absent thread
	return WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iThread;
	}

void CCommonPitBoss::AddPendingIntroductionResponse()
	{
	++iPendingIntroResponses;
	}

void CCommonPitBoss::RemovePendingIntroductionResponse()
	{
	--iPendingIntroResponses;
	}

EXPORT_C void CCommonPitBoss::BindMessageReceived(const CommsFW ::TCFModuleName& aPeerName, TWorkerId aPeerId)
	{
	WorkerDataGlobals().GetWorkerGlobals(aPeerId)->iModuleName = aPeerName;
	AddPendingIntroductionResponse();
	}

	/**
Used during binding when the PitBoss receives a introduction response message from a worker.
The PitBoss will set-up housekeeping datastructures for the worker and add the supported
protocols to its list of protocol pairings.
@see TWorkerMsg::EMainIntroductionResp
*/
EXPORT_C void CCommonPitBoss::ProcessWorkerIntroductionL(const TWorkerIntroductionMsg& aMsg)
	{
	// Note the arrival of the response - if we leave below then whatever the cause we shouldn't diagnose the peer
	// as having failed to respond at all
	RemovePendingIntroductionResponse();

	const TWorkerThreadPublicInfo& msgInfo = aMsg.WorkerInfo();
	__ASSERT_DEBUG(msgInfo.iWorkerId > TWorkerThreadPublicInfo::EMainThread && msgInfo.iWorkerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 7));
	__ASSERT_DEBUG(!WorkerDataGlobals().WorkerPresent(msgInfo.iWorkerId), User::Panic(KSpecAssert_ElemSvrDenPitBsC, 8));
	TWorkerThreadRegister& workerReg = *WorkerDataGlobals().GetWorkerGlobals(msgInfo.iWorkerId);
	// Passing the thread id rather than opening a reference looks like a race possibility. However
	// the worker should not have executed any protocol code yet so shouldn't have died. And even if
	// it has the RootServer hasn't had a chance to unbind us, so the id must still be valid.
	// Copy the base class details from the worker's response and flesh out frequently looked-up details
	static_cast<TWorkerThreadPublicInfo&>(workerReg) = msgInfo;
	workerReg.PeerCleanupPending(msgInfo.iWorkerId);	// until the dies we regard it as pending cleanup on itself, so that the exit of peers doesn't provoke premature cleanup
	workerReg.iPeerDeathNotifier = new(ELeave) CPeerDeathNotifier(msgInfo.iWorkerId, workerReg.iThread, *this);
	workerReg.iDealer = workerReg.iWorker->Dealer();
	workerReg.iPlayer = workerReg.iWorker->Player();

	//Domain specific stuff here
	DoProcessWorkerIntroductionL(aMsg);

#ifdef _DEBUG
	// We only switch on the configured simulated allocation failures once the bindings are complete, because
	// it's too hard to recover from them earlier. This is a regrettable but hopefully harmless limitation in
	// practice, ie if we're OOM during boot then recovery strategies aren't obvious.
	workerReg.iHasGlobalAllocFails = aMsg.FailType() != RAllocator::ENone;
	if(workerReg.iHasGlobalAllocFails)
		{
		workerReg.iHeap->__DbgSetAllocFail(aMsg.FailType(), aMsg.FailRate());
		}
#endif
	}

/**
The PitBoss monitors the Comms Configurator sequence level and when the core components
have been configured (this includes ESock) this method is called to delete any data structures
used only during startup of ESock.
@see CConfigurationLevelMonitor
@see CPitBoss::iPendingIntroResponses
*/
EXPORT_C void CCommonPitBoss::OnCPMsConfigured()
	{
	// Replaced ASSERT with Log entry
	// shouldn't get here without all bindings complete; if we do then it means a module
	// load must have timed out & so the configurator moved on. So the module in question
	// probably isn't bound correctly, etc.

	if (iPendingIntroResponses != 0)
		{
		COMMONLOG((iOwnerThread->WorkerId(), KECommonBootingTag, _L8("CPitBoss::OnCPMsConfigured: %d module(s) not bound correctly"),iPendingIntroResponses));
		}

//#ifdef _DEBUG
//	if(iPendingIntroResponses == 0 && iOwnerThread->AllocFailType() != RAllocator::ENone)
//		{
//		WorkerDataGlobals().GetWorkerGlobals(TWorkerThreadPublicInfo::EMainThread)->iHeap->__DbgSetAllocFail(iOwnerThread->AllocFailType(), iOwnerThread->AllocFailRate());
//		}
//#endif

	delete iConfLevelMonitor;
	iConfLevelMonitor = NULL;

	DoOnCPMsConfigured();
	BroadcastConfigurationComplete(EModuleInitialisation);
	}

/**
If a worker dies the PitBoss will call this method. It will clean up the housekeeping datastructures
related to the worker and it will spawn the RedShirt thread which will try to delete the workers own
data structures. It is a best effort attempt that doesn't guarantee to free up all the dead workers memory
and the RedShirt could be PANICed by the kernel, which is why a short lived seperate thread is doing it.
*/
EXPORT_C void CCommonPitBoss::OnPeerDeath(TWorkerId aWorkerId)
	{
	COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::OnPeerDeath() worker %d died"), aWorkerId));

	TWorkerThreadRegister& worker = *WorkerDataGlobals().GetWorkerGlobals(aWorkerId);
	delete worker.iPeerDeathNotifier;
	worker.iPeerDeathNotifier = NULL;

	// If worker ran a Player all its protocol pairings are now dead. We can't know whether the thread actually
	// did run a Player as (presuming it exited cleanly) it cleaned up, but it's adequately cheap & a rare case
	DoOnPeerDeath(aWorkerId);

	worker.PeerCleanupCompleted(aWorkerId);	// worker is no longer waiting for itself to exit

	// Check how the worker died - if it appears to be involuntary then we consider its heap to be
	// forsaken and don't check it for leaks later
	if(worker.iThread.ExitType() != EExitKill || worker.iThread.ExitReason() != KErrNone)
		{
		iForsakenHeapList.Add(worker.iHeap);
		}

	// Main dealer needs to cleanup
	Dealer(TWorkerThreadPublicInfo::EMainThread)->CleanupDeadWorker(aWorkerId);

	CCommonWorkerThread* deadWorker = WorkerDataGlobals().WorkerThread(aWorkerId);
	if(deadWorker)	// always clean-up now
		{
		// Send in the Red Shirt thread to complete any blocked operations on any remaining
		// subsession with KErrAbort. If it dies in vain then we care as briefly as
		// Captain Kirk ever did about one of his anonymous security guards.
		RThread redShirt;
		if(DoCreateRedShirt(redShirt, aWorkerId, *deadWorker) == KErrNone)
			{
			TRequestStatus rendezStatus;
			redShirt.Rendezvous(rendezStatus);
			COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::CompleteWorkerThreadCleanup() - releasing RedShirt to complete Player's blocked requests")));
			redShirt.Resume();
			User::WaitForRequest(rendezStatus);
			COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::CompleteWorkerThreadCleanup() - RedShirt completed, status %d"), rendezStatus.Int()));
			redShirt.Close();
			}
		else
			{
			COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::CompleteWorkerThreadCleanup() - failed creating RedShirt to clean Player")));
			}
		}

	// Tell all peer threads to clean up
	for(CommsFW::TWorkerId peerId = TWorkerThreadPublicInfo::EFirstPlayerThread; peerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++peerId)
		{
		if(peerId != aWorkerId && WorkerExists(peerId))
			{
			// If the cleanup of any peer was waiting for confirmation from the one which just died then regard
			// it as complete now
			HandleWorkerCleanupCompletionByPeer( peerId, aWorkerId); //Deliberately mirrored sense
			// May already have closed transport to peer
			if(iOwnerThread->PeerReachable(peerId))
				{
				COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::OnPeerDeath() instructing peer %d to cleanup"), peerId));
				worker.PeerCleanupPending(peerId);
				TWorkerCleanupDeadPeerMsg msg(aWorkerId);
				iOwnerThread->PostMessage(peerId, msg);
				}
			else
				COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::OnPeerDeath() peer %d no longer bound to main thread"), peerId));
			}
		}

	// Initial clean-up of worker is by PitBoss now complete; final clean-up may
	// not occur until any cleanup requests have been completed
	COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("Destroying peer #%d handler"),aWorkerId));
	iOwnerThread->DropTransportToPeer(aWorkerId);
	HandleWorkerCleanupCompletionByPeer(aWorkerId, TWorkerThreadPublicInfo::EMainThread);
	}

EXPORT_C TInt CCommonPitBoss::DoCreateRedShirt(RThread& aRedShirt, TWorkerId aWorkerId, CCommonWorkerThread& aDeadWorker)
	{
	//Default implementation - uses CCommonWorkerThread::PostMortemCleanupThreadEntry
	return aRedShirt.Create(KNullDesC, CCommonWorkerThread::PostMortemCleanupThreadEntry, 8192, static_cast<RHeap*>(WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iHeap), &aDeadWorker);
	}

EXPORT_C TBool CCommonPitBoss::ResolvePlayerRoleToId(const TPlayerRole& aRoleId, CommsFW::TWorkerId& aWorkerId) const
	{
	for(TWorkerId id = TWorkerThreadPublicInfo::EFirstPlayerThread; id <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++id)
		{
		if (WorkerExists(id) && Player(id)->PlayerRole().Role() == aRoleId.Role())
			{
			aWorkerId = id;
			return ETrue;
			}
		}
#ifdef _DEBUG
	aWorkerId = TWorkerThreadPublicInfo::ENullWorkerId;	// help find buggy code which ignores returns
#endif
	return EFalse;
	}


EXPORT_C TBool CCommonPitBoss::ResolveWorkerNameToId(const TDesC8& aWorkerName, TWorkerId& aWorkerId) const
	{
	for(TWorkerId id = TWorkerThreadPublicInfo::EFirstPlayerThread; id <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++id)
		{
		if(WorkerExists(id) && WorkerDataGlobals().GetWorkerGlobals(id)->iModuleName.CompareF(aWorkerName) == 0)
			{
			aWorkerId = id;
			return ETrue;
			}
		}
#ifdef _DEBUG
	aWorkerId = TWorkerThreadPublicInfo::ENullWorkerId;	// help find buggy code which ignores returns
#endif
	return EFalse;
	}

EXPORT_C void CCommonPitBoss::HandleWorkerCleanupCompletionByPeer(TWorkerId aWorkerId, TWorkerId aPeerId)
	{
	COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::HandleWorkerCleanupCompletionByPeer() cleanup of worker %d by peer %d completed"), aWorkerId, aPeerId));
	TWorkerThreadRegister& properties(*WorkerDataGlobals().GetWorkerGlobals(aWorkerId));
	if(properties.IsValid())	// Might already have processed the worker's own shutdown
		{
		properties.PeerCleanupCompleted(aPeerId);
		if(properties.AllPeerCleanupsCompleted())
			{
			CompleteWorkerThreadCleanup(aWorkerId);
			}
		}
	}

void CCommonPitBoss::CompleteWorkerThreadCleanup(TWorkerId aWorkerId)
	{
	__ASSERT_DEBUG(aWorkerId != TWorkerThreadPublicInfo::EMainThread, User::Panic(KSpecAssert_ElemSvrDenPitBsC, 9));
	COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::CompleteWorkerThreadCleanup() - final cleanup phase for worker %d"), aWorkerId));

	// Free resources
	FreeWorkerReferences(aWorkerId);

	// Test whether all cleanups done
	TWorkerId peerId;
	for(peerId = TWorkerThreadPublicInfo::EFirstPlayerThread; peerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++peerId)
		{
		if(WorkerExists(peerId))
			{
			break;
			}
		}
	if(peerId > TWorkerThreadPublicInfo::EMaxWorkerThreadId)
		{
		iPeerShutdownComplete = ETrue;
		ShutdownIfReady();
		}
	}

void CCommonPitBoss::FreeWorkerReferences(TWorkerId aWorkerId)
	{
	DoFreeWorkerReferences(aWorkerId);

	TWorkerThreadRegister& properties(*WorkerDataGlobals().GetWorkerGlobals(aWorkerId));
	delete properties.iPeerDeathNotifier;
//	properties.iHeap->Close();
	properties.Clear();
	}

#ifdef _DEBUG
// If a heap has been configured from boot to have a failure mode then we don't override this here as
// the lifetime failure testing is more important than the specific test case doing a SetFailNext
EXPORT_C void CCommonPitBoss::SetFailNextForAllHeaps(TInt aFailNext)
	{
	for(TWorkerId workerId = TWorkerThreadPublicInfo::EMainThread; workerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++workerId)
		{
		if(WorkerExists(workerId))
			{
			if(WorkerDataGlobals().GetWorkerGlobals(workerId)->iHasGlobalAllocFails)
				{
				if(aFailNext == KResetAllocFails)
					{
					// Reset lifetime failure mode.
					WorkerDataGlobals().GetWorkerGlobals(workerId)->iHeap->__DbgSetAllocFail(RAllocator::EReset, 0);
					}
				}
			else
				{
				WorkerDataGlobals().GetWorkerGlobals(workerId)->iHeap->__DbgSetAllocFail((aFailNext < 0)? RAllocator::EReset: RAllocator::EFailNext, aFailNext);
				}
			}
		}
	}

EXPORT_C TBool CCommonPitBoss::TestFailNextForAllHeaps() const
	{
	class RPeekHeap : public RHeap
		{
 	public:
		RAllocator::TAllocFail FailNextMode() const
			{
			return iFailType;
			}
		};

	for(TWorkerId workerId = TWorkerThreadPublicInfo::EMainThread; workerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++workerId)
		{
 		if(WorkerExists(workerId) && !WorkerDataGlobals().GetWorkerGlobals(workerId)->iHasGlobalAllocFails)
 			{
 			RPeekHeap* heap = static_cast<RPeekHeap*>(WorkerDataGlobals().GetWorkerGlobals(workerId)->iHeap);
 			if(heap->FailNextMode() != RAllocator::EFailNext)
 				{
 				return EFalse;	// at least one heap isn't in fail-next mode
 				}
 			}
 		}
 	return ETrue;
 	}
#else
EXPORT_C void CCommonPitBoss::SetFailNextForAllHeaps(TInt /*aFailNext*/)
	{}
EXPORT_C TBool CCommonPitBoss::TestFailNextForAllHeaps() const
	{return EFalse;}
#endif

/** Called synchronously by worker threads just prior to their exit to signal that their exit completed normally and
that external cleanup is not required
*/
EXPORT_C void CCommonPitBoss::PeerWorkerExiting(TWorkerId aWorker)
	{
	// Remove the pointers to the major control objects which a cleanup thread would use in the event of abnormal exit
	TWorkerThreadRegister& properties(*WorkerDataGlobals().GetWorkerGlobals(aWorker));
	properties.iWorker = NULL;
	properties.iDealer = NULL;
	properties.iPlayer = NULL;
	}



EXPORT_C void CCommonPitBoss::BroadcastConfigurationComplete(TConfigurationCompletionType aType)
	{
	TWorkerConfigurationComplete msg(aType);
	for(CommsFW::TWorkerId peerId = TWorkerThreadPublicInfo::EFirstPlayerThread; peerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++peerId)
		{
		if(Dealer(peerId))
			{
			COMMONLOG((iOwnerThread->WorkerId(), KECommonServerTag, _L8("CPitBoss::OnCPMsConfigured() - notifying peer %d"), peerId));
			if(OwnerThread()->PeerReachable(peerId))
				{
				// Need to make temporary copy of the message as TCFLegacyMessagePacker modifies
				// it in place and will assert in debug builds if it detects its own special
				// override code indicating that a message is being packed twice if we broadcast
				// to more than one peer.
				TWorkerConfigurationComplete tempMsg(msg);

				OwnerThread()->PostMessage(peerId, tempMsg);
				}
			}
		}
	// Process parked requests for the main Dealer
	Dealer(TWorkerThreadPublicInfo::EMainThread)->ProcessConfigurationComplete(aType);
	}

/**
*/
EXPORT_C TBool CCommonPitBoss::FindOptimalDealer(TWorkerId aWorkerId, CCommonWorkerDealer*& aDealer)
	{
	if(WorkerExists(aWorkerId))
		{
		CCommonDealer* hostThreadDealer = Dealer(*WorkerDataGlobals().GetWorkerGlobals(aWorkerId));
		if (hostThreadDealer)
			{
			aDealer = hostThreadDealer->WorkerThread().WorkerDealer();
			if (aDealer)
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

//
// CPitBoss::CPeerDeathNotifier
//

CPeerDeathNotifier::CPeerDeathNotifier(TWorkerId aWorkerId, RThread aThread, MPeerDeathObserver& aObserver)
: CActive(EPriorityLow),
  iWorkerId(aWorkerId),
  iThread(aThread),
  iObserver(aObserver)
	{
	iThread.Logon(iStatus);
	CActiveScheduler::Add(this);
	SetActive();
	}

void CPeerDeathNotifier::DoCancel()
	{
	iThread.LogonCancel(iStatus);
	}

void CPeerDeathNotifier::RunL()
	{
	iObserver.OnPeerDeath(iWorkerId);
	}


//
// CConfigurationLevelMonitor
//

CConfigurationLevelMonitor* CConfigurationLevelMonitor::NewL(MConfiguratorObserver* aObserver)
	{
	CConfigurationLevelMonitor* self = new(ELeave) CConfigurationLevelMonitor(aObserver);
	TInt err = self->iConfLevelProperty.Attach(KUidSystemCategory, aObserver->PropertyKey().iUid, EOwnerThread);
	if(err != KErrNone)
		{
		delete self;
		User::Leave(err);
		}
	CActiveScheduler::Add(self);
	self->Subscribe();
	return self;
	}

CConfigurationLevelMonitor::CConfigurationLevelMonitor(MConfiguratorObserver* aObserver)
: CActive(EPriorityLow),
  iObserver(aObserver)
	{
	}

CConfigurationLevelMonitor::~CConfigurationLevelMonitor()
	{
	Cancel();
	iConfLevelProperty.Close();
	}

void CConfigurationLevelMonitor::Subscribe()
	{
	iConfLevelProperty.Subscribe(iStatus);
	SetActive();
	}

/**
Monitors the Comms Configurator configuration level. Will notify the listener when
core components have been configured.
@see CPitBoss::OnCPMsConfigured
*/
void CConfigurationLevelMonitor::RunL()
	{
	Subscribe();
	TInt level;
	if(iConfLevelProperty.Get(level) == KErrNone)
		{
		if(level >= RootServer::EConfigurationComplete && !iCoreComponentsConfigured)
			{
			iCoreComponentsConfigured = ETrue;
			iObserver->OnCPMsConfigured();
			}
		if(level >= RootServer::EConfigurationComplete)
			{
			Cancel();
			}
		}
	}

void CConfigurationLevelMonitor::DoCancel()
	{
	iConfLevelProperty.Cancel();
	}

//
// CPitBoss::TWorkerThreadRegister
//

TWorkerThreadRegister::TWorkerThreadRegister()
	{
	PeerCleanupPending(TWorkerThreadPublicInfo::EMainThread);
#ifdef _DEBUG
	iHasGlobalAllocFails = EFalse;
#endif
	}

void TWorkerThreadRegister::Clear()
	{
	TWorkerThreadPublicInfo::Clear();
	iDealer = NULL;
	iPlayer = NULL;
	}

void TWorkerThreadRegister::PeerCleanupPending(TWorkerId aPeerId)
	{
	iPendingPeerCleanups |= (1 << aPeerId);
	}

void TWorkerThreadRegister::PeerCleanupCompleted(TWorkerId aPeerId)
	{
	iPendingPeerCleanups &= ~(1 << aPeerId);
	}

TBool TWorkerThreadRegister::AllPeerCleanupsCompleted() const
	{
	return iPendingPeerCleanups == 0;
	}


//
// THeapSwitcher
//
EXPORT_C THeapSwitcher::THeapSwitcher(CCommonPitBoss& aPitBoss, const Messages::TNodeId& aTarget)
	{
	MaybeSwitch(aPitBoss, aTarget.Thread());
	}

EXPORT_C THeapSwitcher::THeapSwitcher(CCommonPitBoss& aPitBoss, TWorkerId aForeignWorkerId)
	{
	MaybeSwitch(aPitBoss, aForeignWorkerId);
	}


EXPORT_C THeapSwitcher::~THeapSwitcher()
	{
	RevertToOwnHeap();
	}

EXPORT_C void THeapSwitcher::RevertToOwnHeap()
	{
	if(iPrev)
		{
		User::SwitchHeap(iPrev);
		iPrev = NULL;
		}
	}

EXPORT_C void THeapSwitcher::MaybeSwitch(CCommonPitBoss& aPitBoss, TWorkerId aForeignWorkerId)
	{
	iPrev = aPitBoss.MaybeSwitchHeap(aForeignWorkerId);
	}

/*
EXPORT_C void CCommonWorkerThread::MaybeIncorporateFCL(TPlayerRoleMasks aPlane, TInt aPeerKindex, const Messages::TNodeId& aPeerId)
	{
	DoMaybeIncorporateFCL(aPlane, aPeerKindex,aPeerId);
	}
*/

