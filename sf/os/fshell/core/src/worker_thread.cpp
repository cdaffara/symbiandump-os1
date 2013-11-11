// worker_thread.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

/* 

Some notes on how CThreadPool works:
 * There are 2 types of CWorkerThread - ones that share a specific heap, and those that have their own heap.
 * Tasks that need a shared heap can only be run using a CWorkerThread that is bound to that heap (one will be created if necessary)
 * Tasks that don't need a shared heap can be run on any CWorkerThread that has its own heap.
 * Excess CWorkerThreads are deleted after they've been idle for KCacheTime.
 * CThreadPool uses an RFastLock on all operations that could be called from multiple threads.
 * All member data of CThreadPool (including CWorkerThreads and their members) is kept on iThreadPoolAllocator, which was the heap
   that the CThreadPool was created on. User::SwitchAllocator() is used where necessary to manipulate CThreadPool data from other threads
   while the Lock is held
 * When a task is queued, a non-busy CWorkerThread is found (or created).
 * When a task is completed in a worker thread, it calls iParentThread.RequestComplete(iCompletionStatus, err). It then calls
   CThreadPool::WorkerFinished() to set the CWorkerThread to be non-busy again.
 * When a CWorkerThread is created it arranges (via SignalSelf()) to add a CThreadDeathWatcher to the main thread.
   It follows that notifications about tasks failed because of thread death are always serviced by the main thread, which must therefore
   remain responsive. (In the non-dead case, the completion notification is sent directly from the worker thread).

*/

#include "worker_thread.h"
#include <e32debug.h>

#define WT_LOG(args...)

// These defines are for debugging only
//#define WT_LOG(args...) RDebug::Print(args)
// These probably don't even work any more - use with extreme caution!
//#define NO_REUSE
//#define IMMEDIATE_CLEANUP

const TInt KCacheTime = 1000000; // 1 second

// Thread death watchers run in the context of the thread pool main thread
class CThreadDeathWatcher : public CActive
	{
public:
	CThreadDeathWatcher(CWorkerThread* aWorker)
		: CActive(CActive::EPriorityHigh), iWorker(aWorker)
		{
		}
	~CThreadDeathWatcher()
		{
		WT_LOG(_L("Deleting thread death watcher for worker %d"), TUint(iWorker->GetThreadId()));
		Cancel();
		}
	void StartWatching()
		{
		CActiveScheduler::Add(this);
		iWorker->iWorkerThread.Logon(iStatus);
		SetActive();
		}
	CWorkerThread* WorkerThread() const
		{
		return iWorker;
		}
private:
	void DoCancel()
		{
		iWorker->iWorkerThread.LogonCancel(iStatus);
		}
	void RunL()
		{
		iWorker->SignalClientThatThreadHasDied();
		iWorker->iParentPool->WorkerDied(iWorker);
		// WorkerDied may cause this object to be deleted, so don't add anything that accesses member data below this point!
		}

private:
	CWorkerThread* iWorker;
	};

//



CThreadPool* CThreadPool::NewL()
	{
	CThreadPool* self = new(ELeave) CThreadPool();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CThreadPool::CThreadPool()
	: CActive(CActive::EPriorityLow) // Cleanup doesn't need to be a priority
	{
	CActiveScheduler::Add(this);
	}

void CThreadPool::ConstructL()
	{
	User::LeaveIfError(iMainThread.Open(RThread().Id()));
	iIdleTimer = CPeriodic::NewL(CActive::EPriorityLow-1); // Interactions around iPendingThreadLogons mean it's probably a good idea to have this priority below that of CThreadPool itself
	iThreadPoolAllocator = &User::Allocator();
	User::LeaveIfError(iLock.CreateLocal());
	iThreads.ReserveL(2);
	iThreadDeathWatchers.ReserveL(2);
	// We don't create any workers by default
	}

CThreadPool::~CThreadPool()
	{
	WT_LOG(_L("Deleting thread pool. %d threads created during its lifetime, %d currently and %d watchers"), iCountThreadsCreated, iThreads.Count(), iThreadDeathWatchers.Count());
	Cancel();
	for (TInt i = 0; i < iThreads.Count(); i++)
		{
		iThreads[i]->Shutdown();
		}
	iThreadDeathWatchers.ResetAndDestroy();
	iThreads.ResetAndDestroy();
	iLock.Close();
	delete iIdleTimer;
	iMainThread.Close();
	iPendingThreadLogons.Close();
	}

void CThreadPool::Lock()
	{
	WT_LOG(_L("Getting lock from thread %d"), TUint(RThread().Id()));
	iLock.Wait();
	WT_LOG(_L("Got lock from thread %d"), TUint(RThread().Id()));
	}

void CThreadPool::SwitchToThreadPoolHeap()
	{
	//ASSERT(iLock.IsHeld());
	RAllocator* allocator = &User::Allocator();
	if (allocator != iThreadPoolAllocator)
		{
		WT_LOG(_L("Thread %d switching from 0x%x to thread pool allocator 0x%x"), TUint(RThread().Id()), allocator, iThreadPoolAllocator);
		iTempThreadAllocator = User::SwitchAllocator(iThreadPoolAllocator);
		}
	}

void CThreadPool::RestoreHeap()
	{
	if (iTempThreadAllocator)
		{
		WT_LOG(_L("Thread %d restoring heap 0x%x"), TUint(RThread().Id()), iTempThreadAllocator);
		User::SwitchAllocator(iTempThreadAllocator);
		iTempThreadAllocator = NULL;
		}
	}

void CThreadPool::Unlock()
	{
	RestoreHeap();
	WT_LOG(_L("Releasing lock from thread %d"), TUint(RThread().Id()));
	iLock.Signal();
	}

void CThreadPool::LockLC()
	{
	Lock();
	CleanupStack::PushL(TCleanupItem(&DoUnlock, &iLock));
	}

#define _LOFF(p,T,f) ((T*)(((TUint8*)(p))-_FOFF(T,f))) // Why isn't this defined user-side?

void CThreadPool::DoUnlock(TAny* aLock)
	{
	CThreadPool* self = _LOFF(aLock, CThreadPool, iLock);
	self->Unlock();
	}

MThreadedTask* CThreadPool::NewTaskInSeparateThreadL(const TDesC& aThreadName, TBool aSharedHeap, MTaskRunner::TThreadFunctionL aThreadFunction, TAny* aThreadContext)
	{
	WT_LOG(_L("1. NewTaskInSeparateThreadL task %d (%S)"), iTaskCounter, &aThreadName);

	LockLC();
	// Hunt for a non-busy thread
	CWorkerThread* foundThread = NULL;
	RAllocator* requiredAllocator = aSharedHeap ? &User::Allocator() : NULL;
#ifndef NO_REUSE // This code normally does run, except during debugging when NO_REUSE is defined
	for (TInt i = 0; i < iThreads.Count(); i++)
		{
		CWorkerThread* thread = iThreads[i];
		if (!thread->Busy() && thread->SharedAllocator() == requiredAllocator)
			{
			// If the worker thread is sharing an allocator, it must be sharing the *same* one as this current thread
			ASSERT(thread->Running());
			foundThread = thread;
			break;
			}
		}
#endif

	if (foundThread == NULL)
		{
		SwitchToThreadPoolHeap();
		iPendingThreadLogons.ReserveL(iPendingThreadLogons.Count() + 1); // So the Append below can't fail
		iThreadDeathWatchers.ReserveL(iThreadDeathWatchers.Count() + 1);
		iThreads.ReserveL(iThreads.Count() + 1);
		foundThread = CWorkerThread::NewLC(this, requiredAllocator);
		CThreadDeathWatcher* threadWatcher = new(ELeave) CThreadDeathWatcher(foundThread);
		iCountThreadsCreated++;
		WT_LOG(_L("Created new worker thread %d"), TUint(foundThread->GetThreadId()));
		iThreads.Append(foundThread);
		iPendingThreadLogons.Append(threadWatcher);
		iThreadDeathWatchers.Append(threadWatcher);
		CleanupStack::Pop(foundThread);	

		SignalSelf(); // So the iPendingThreadLogons gets sorted out in context of main thread
		RestoreHeap();
		}

	WT_LOG(_L("Using worker thread %d for task %d (%S)"), TUint(foundThread->GetThreadId()), iTaskCounter, &aThreadName);

	User::LeaveIfError(foundThread->Setup(iTaskCounter, aThreadName, aThreadFunction, aThreadContext));
	foundThread->SetBusy(ETrue);
	iTaskCounter++;
	CleanupStack::PopAndDestroy(&iLock);
	return foundThread;
	}

void CThreadPool::WorkerDied(CWorkerThread* aWorker)
	{
	Lock();
	// This is now always called in the main thread
	ASSERT(RThread().Id() == iMainThread.Id());

	for (TInt i = 0; i < iThreadDeathWatchers.Count(); i++)
		{
		if (iThreadDeathWatchers[i]->WorkerThread() == aWorker)
			{
			// Clean up the watcher first
			delete iThreadDeathWatchers[i];
			iThreadDeathWatchers.Remove(i);

			// The indexes in both arrays are guaranteed to be the same
			ASSERT(iThreads[i] == aWorker);
			delete aWorker;
			iThreads.Remove(i);

			break;
			}
		}
	Unlock();
	}

void CThreadPool::WorkerFinished(CWorkerThread* aWorker)
	{
	Lock();
	aWorker->SetBusy(EFalse);
#if defined(NO_REUSE)
	// Nothing
#else
	// This is the normal case - queue ourself to run (on the main thread) so we can trigger the idle timer from our RunL
	// Can't do that directly as timers are all thread-local
	SignalSelf();
#endif
	Unlock();
	}


void CThreadPool::SignalSelf()
	{
	// TODO: Optimise if we actually are the main thread?

	// Must be holding lock
	iPendingCallbacks++;
	TRequestStatus* stat = &iStatus;
	if (iPendingCallbacks == 1) // Can't use IsActive() as we might not be in the same thread
		{
		iStatus = KRequestPending;
		SetActive();
		}
	iMainThread.RequestComplete(stat, KErrNone); // Fortunately RThread::RequestComplete doesn't set the status to KRequestPending before completing it (unlike User::RequestComplete)
	}

void CThreadPool::CleanupAnyWorkersSharingAllocator(RAllocator* aAllocator)
	{
	Lock();
	SwitchToThreadPoolHeap();
	for (TInt i = iThreads.Count() - 1; i >= 0; i--)
		{
		CWorkerThread* worker = iThreads[i];
		if (worker->SharedAllocator() == aAllocator)
			{
			ASSERT(!worker->Busy());
			worker->Shutdown();
			// We don't delete the worker here, let CThreadDeathWatcher handle that
			}
		}
	Unlock();
	}

void CThreadPool::PerformHouseKeeping()
	{
	// Time to do some housekeeping. Algorithm is:
	// * Keep a single spare non-busy shared worker around, but only for the main heap (ie a CWorkerThread whose SharedAllocator() equals iThreadPoolAllocator)
	// * Bin everything else that isn't busy

	WT_LOG(_L("Idle timer expired, cleaning up spare workers"));
	Lock();
	ASSERT(&User::Allocator() == iThreadPoolAllocator); // We're running in the thread pool's main thread so no need to switch allocators

#ifdef IMMEDIATE_CLEANUP
	// Everything gets nuked if this is defined
	TBool foundSpareWorker = ETrue;
#else
	TBool foundSpareWorker = EFalse;
#endif
	for (TInt i = iThreads.Count() - 1; i >= 0; i--)
		{
		CWorkerThread* worker = iThreads[i];
		if (!worker->Busy())
			{
			RAllocator* allocator = worker->SharedAllocator();
			if (allocator == iThreadPoolAllocator && !foundSpareWorker)
				{
				// This one can stay
				foundSpareWorker = ETrue;
				}
			else
				{
				// Everything else (that isn't busy) gets cleaned up
				delete iThreadDeathWatchers[i];
				iThreadDeathWatchers.Remove(i);
				worker->Shutdown();
				delete worker;
				iThreads.Remove(i);
				}
			}
		}
	Unlock();
	}

void CThreadPool::DoCancel()
	{
	// There's nothing we can cancel as we complete ourself
	}

void CThreadPool::RunL()
	{
	Lock();
	iPendingCallbacks--;
	while (iPendingCallbacks)
		{
		// Consume any further signals currently pending - we're somewhat abusing the active scheduler by completing the same repeatedly without it running in between so we have to consume the abuse here to balance things
		User::WaitForRequest(iStatus);
		iPendingCallbacks--;
		}

	// We kinda overload our RunL, using it for both registering logons and triggering the idle cleanup. Ah well.

	if (iPendingThreadLogons.Count())
		{
		for (TInt i = 0; i < iPendingThreadLogons.Count(); i++)
			{
			iPendingThreadLogons[i]->StartWatching();
			}
		SwitchToThreadPoolHeap();
		iPendingThreadLogons.Reset();
		}

#ifdef IMMEDIATE_CLEANUP
	Unlock();
	// Delete the thread straight away
	PerformHouseKeeping();
#else
	// Normally go through the timer
	iIdleTimer->Cancel(); // Reset it if it's already counting
	iIdleTimer->Start(KCacheTime, KCacheTime, TCallBack(&TimerCallback, this));
	Unlock();
#endif
	}

TInt CThreadPool::TimerCallback(TAny* aSelf)
	{
	CThreadPool* self = static_cast<CThreadPool*>(aSelf);
	self->iIdleTimer->Cancel(); // Stop the thing being periodic
	self->PerformHouseKeeping();
	return 0;
	}

////

const TInt KMaxHeapSize = 1024 * 1024;

// CWorkerThreadDispatchers run in the context of the worker thread they're owned by
class CWorkerThreadDispatcher : public CActive
	{
public:
	CWorkerThreadDispatcher(CWorkerThread* aThread)
		: CActive(CActive::EPriorityStandard), iThread(aThread)
		{
		CActiveScheduler::Add(this);
		iStatus = KRequestPending;
		SetActive();
		}

	void RunL()
		{
		if (iStatus.Int() == KErrNone)
			{
			iStatus = KRequestPending;
			SetActive();
			iThread->ThreadRun();
			}
		else
			{
			// Time to die
			iThread->iAsWait.AsyncStop();
			}
		}

	void DoCancel() {} // Can never happen because we can only ever be destroyed as a result of the AsyncStop in RunL from which we can never be active

private:
	CWorkerThread* iThread;
	};

CWorkerThread* CWorkerThread::NewLC(CThreadPool* aParentPool, RAllocator* aSharedAllocator)
	{
	CWorkerThread* self = new(ELeave) CWorkerThread(aParentPool, aSharedAllocator);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CWorkerThread::CWorkerThread(CThreadPool* aParentPool, RAllocator* aSharedAllocator)
	: iParentPool(aParentPool), iSharedAllocator(aSharedAllocator)
	{
	iWorkerThread.SetHandle(0);
	}

void CWorkerThread::ConstructL()
	{
	TInt err = KErrNone;
	TName name;
	name.Format(_L("WorkerThread_%x"), this);
	if (iSharedAllocator)
		{
		err = iWorkerThread.Create(name, &ThreadFn, KDefaultStackSize, iSharedAllocator, this);
		}
	else
		{
		// Create a new heap with default heap size
		err = iWorkerThread.Create(name, &ThreadFn, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, this);
		}
	User::LeaveIfError(err);

	TRequestStatus stat;
	iWorkerThread.Rendezvous(stat);

	ASSERT(stat == KRequestPending);
	if (stat == KRequestPending)
		{
		iWorkerThread.Resume();
		}
	else
		{
		iWorkerThread.Kill(stat.Int());
		}

	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	// Thread is now ready to do stuff
	}

CWorkerThread::~CWorkerThread()
	{
	WT_LOG(_L("Deleting worker thread %d"), TUint(GetThreadId()));
	ASSERT(iWorkerThread.Handle() == 0 || iWorkerThread.ExitType() != EExitPending);
	iParentThread.Close();
	iWorkerThread.Close();
	}

TInt CWorkerThread::Setup(TInt aTaskId, const TDesC& aThreadName, MTaskRunner::TThreadFunctionL aThreadFunction, TAny* aThreadContext)
	{
	ASSERT(!Busy());

	// Things to do in preparation for running a command:
	// 1. Check what thread we're running in now and set ourselves and our thread death watcher on its scheduler (may not be the thread that created us originally)
	// 2. Store the required context for the worker thread to access

	TInt err = iParentThread.Open(RThread().Id());
	if (!err)
		{
		iTaskId = aTaskId;
		iName = &aThreadName;
		iFn = aThreadFunction;
		iContext = aThreadContext;
		}
	return err;
	}

TInt CWorkerThread::ExecuteTask(TRequestStatus& aCompletionStatus)
	{
	WT_LOG(_L("2. + Go task %d (%S)"), iTaskId, iName);

	ASSERT(iCompletionStatus == NULL);
	aCompletionStatus = KRequestPending;
	iCompletionStatus = &aCompletionStatus;

	// Setup rendezvous and completion requestStatuses before signalling iDispatchStatus
	TRequestStatus rendezvousStat;
	iWorkerThread.Rendezvous(rendezvousStat);

	// Signal the worker thread to do its thing
	TRequestStatus* dispatchStat = iDispatchStatus;
	iWorkerThread.RequestComplete(dispatchStat, KErrNone);

	User::WaitForRequest(rendezvousStat);

	TInt err = rendezvousStat.Int();
	if (iWorkerThread.ExitType() != EExitPending && rendezvousStat.Int() >= 0)
		{
		err = KErrDied;
		}

	WT_LOG(_L("6. - Go task %d (%S) err=%d"), iTaskId, iName, err);

	if (err != KErrNone)
		{
		// We don't signal completion if there was an error prior to the rendezvous, we just return it here
		iCompletionStatus = NULL;
		}
	return err;
	}

TThreadId CWorkerThread::GetThreadId() const
	{
	return iWorkerThread.Id();
	}

void CWorkerThread::AbortTask()
	{
	// Undo setup
	iName = NULL;
	iFn = NULL;
	iContext = NULL;
	iParentThread.Close();
	iParentPool->WorkerFinished(this);
	}

void CWorkerThread::ThreadRun()
	{
	// Runs in the worker thread

	if (!UsingSharedAllocator())
		{
		__UHEAP_MARK;
		}

	TInt i = 0;
	TInt err = KErrNone;
	do
		{
		TName threadName;
		threadName.Format(_L("%S_%02d"), iName, i++);
		err = User::RenameThread(threadName);
		WT_LOG(_L("3. Running thread for task %d (%S)"), iTaskId, &threadName);
		}
	while (err == KErrAlreadyExists);

	// Execute the actual function
	WT_LOG(_L("4. + ThreadRun for task %d (%S)"), iTaskId, iName);
	TRAP(err, (*iFn)(iContext));
	WT_LOG(_L("7. - ThreadRun for task %d (%S) signalling thread %d with err=%d"), iTaskId, iName, TUint(iParentThread.Id()), err);

	if (!UsingSharedAllocator())
		{
		// Do this before saying we've actually finished the task, otherwise we risk deadlocking on the thread pool lock when IMMEDIATE_CLEANUP is defined
		// because CleanupAnyWorkersSharingAllocator takes the lock, but the lock can already be held by that point around the Shutdown that IMMEDIATE_CLEANUP does.
		iParentPool->CleanupAnyWorkersSharingAllocator(&User::Allocator()); // Otherwise the heap check will fail
		}

	// And signal back the result
	iParentPool->WorkerFinished(this);
	iParentThread.RequestComplete(iCompletionStatus, err);

	// Finally put our name back to what it was
#ifdef _DEBUG
	_LIT(KDebugName, "WorkerThread_%x (was %S)");
	TName threadName = RThread().Name();
	TPtrC oldName = threadName.Left(threadName.MaxLength() - KDebugName().Length());
	TName newName;
	newName.Format(KDebugName, this, &oldName);
	User::RenameThread(newName);
#else
	TName threadName;
	threadName.Format(_L("WorkerThread_%x"), this);
	User::RenameThread(threadName);
#endif

	if (!UsingSharedAllocator())
		{
		__UHEAP_MARKEND;
		}
	}

TBool CWorkerThread::Busy() const
	{
	return iBusy;
	}

void CWorkerThread::SetBusy(TBool aBusy)
	{
	iBusy = aBusy;
	}

TBool CWorkerThread::UsingSharedAllocator() const
	{
	return iSharedAllocator != NULL;
	}

RAllocator* CWorkerThread::SharedAllocator() const
	{
	return iSharedAllocator;
	}

void CWorkerThread::SignalClientThatThreadHasDied()
	{
	WT_LOG(_L("Task %d died with exittype %d reason %d"), iTaskId, iWorkerThread.ExitType(), iWorkerThread.ExitReason());
	TInt err = iWorkerThread.ExitReason();
	if (err >= 0) err = KErrDied;
	if (iCompletionStatus)
		{
		iParentThread.RequestComplete(iCompletionStatus, err);
		}
	}

TInt CWorkerThread::ThreadFn(TAny* aSelf)
	{
	CWorkerThread* self = static_cast<CWorkerThread*>(aSelf);
	if (self->UsingSharedAllocator())
		{
		// If we're sharing the main fshell heap, we have to play by the rules and not crash
		User::SetCritical(User::EProcessCritical);

		// We also need to temporarily switch to the thread pool's allocator, because our CTrapCleanup, CActiveScheduler etc conceptually belong with the worker thread object and not with the heap we're sharing (which could be different... damn repeat command again)
		//self->iParentPool->Lock();
		//self->iParentPool->SwitchToThreadPoolHeap();
		}
	else
		{
		__UHEAP_MARK;
		}
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	//WT_LOG(_L("Worker thread %d creating trapcleanup 0x%x"), TUint(RThread().Id()), cleanup);
	if (cleanup)
		{
		TRAP(err, self->ThreadFnL());
		//WT_LOG(_L("Worker thread %d deleting trapcleanup 0x%x"), TUint(RThread().Id()), cleanup);
		delete cleanup;
		}
	if (!self->UsingSharedAllocator())
		{
		__UHEAP_MARKEND;
		}
	return err;
	}

void CWorkerThread::ThreadFnL()
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CWorkerThreadDispatcher* dispatcher = new(ELeave) CWorkerThreadDispatcher(this);
	CleanupStack::PushL(dispatcher);
	iDispatchStatus = &dispatcher->iStatus;
	RThread::Rendezvous(KErrNone);
	iAsWait.Start();
	CleanupStack::PopAndDestroy(2, scheduler); // dispatcher, scheduler
	}

void CWorkerThread::Shutdown()
	{
	// Can be called from (potentially) any thread
	WT_LOG(_L("Shutting down worker thread %d whose exittype is %d"), TUint(GetThreadId()), iWorkerThread.ExitType());

	if (Busy())
		{
		WT_LOG(_L("Thread is still busy - killing it"));
		iWorkerThread.Kill(KErrAbort);
		// The thread death watcher should take care of everything else, eventually
		}
	else if (iWorkerThread.ExitType() == EExitPending)
		{
		TRequestStatus stat;
		iWorkerThread.Logon(stat);
		iWorkerThread.RequestComplete(iDispatchStatus, KErrCancel);
		User::WaitForRequest(stat);
		WT_LOG(_L("Shut down worker %d exit=%d"), TUint(GetThreadId()), iWorkerThread.ExitType());
		}
	}

