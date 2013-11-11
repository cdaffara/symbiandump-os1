// worker_thread.h
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

#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <e32base.h>

class MThreadedTask
	{
public:
	virtual TThreadId GetThreadId() const=0;
	virtual TInt ExecuteTask(TRequestStatus& aCompletionStatus)=0;
	virtual void AbortTask()=0; // If you decide you don't want to call ExecuteTask
	};

class MTaskRunner
	{
public:
	typedef void (*TThreadFunctionL)(TAny*);
	virtual MThreadedTask* NewTaskInSeparateThreadL(const TDesC& aThreadName, TBool aSharedHeap, TThreadFunctionL aThreadFunction, TAny* aThreadContext)=0;
	};

class CWorkerThread;
class CThreadDeathWatcher;

class CThreadPool : public CActive, public MTaskRunner
	{
public:
	static CThreadPool* NewL();
	~CThreadPool();
	void WorkerDied(CWorkerThread* aWorker);
	void WorkerFinished(CWorkerThread* aWorker);
	void CleanupAnyWorkersSharingAllocator(RAllocator* aAllocator);

public: // From MTaskRunner
	virtual MThreadedTask* NewTaskInSeparateThreadL(const TDesC& aThreadName, TBool aSharedHeap, TThreadFunctionL aThreadFunction, TAny* aThreadContext);

private:
	CThreadPool();
	void ConstructL();
	static TInt TimerCallback(TAny* aSelf);
	void PerformHouseKeeping();
	void RunL();
	void DoCancel();
	void SignalSelf();

	void Lock();
	void LockLC();
	void SwitchToThreadPoolHeap();
	void RestoreHeap();
	void Unlock();
	static void DoUnlock(TAny* aLock);

private:
	RFastLock iLock; // Thread pool is shared between eg fshell main thread and any source threads, so needs locking around WorkerFinished and NewTaskInSeparateThreadL etc
	RAllocator* iThreadPoolAllocator; // The thread pool can be modified from a thread that isn't using the main heap, so we need to track that and use User::SwitchAllocator as needed
	RAllocator* iTempThreadAllocator; // Only used while lock is held
	RPointerArray<CWorkerThread> iThreads;
	TInt iTaskCounter;
	RThread iMainThread;
	TInt iPendingCallbacks;
	CPeriodic* iIdleTimer;
	TInt iCountThreadsCreated; // This is for statistics gathering, not involved in the logic
	RArray<CThreadDeathWatcher*> iPendingThreadLogons; // Not owned
	RPointerArray<CThreadDeathWatcher> iThreadDeathWatchers;
	};

class CWorkerThread : public CBase, public MThreadedTask
	{
public:
	static CWorkerThread* NewLC(CThreadPool* aParentPool, RAllocator* aSharedAllocator);
	// For safety these 2 need to be called with the thread pool lock held
	TBool Busy() const;
	void SetBusy(TBool aBusy);

	TBool UsingSharedAllocator() const;
	RAllocator* SharedAllocator() const;
	TInt Setup(TInt aTaskId, const TDesC& aThreadName, MTaskRunner::TThreadFunctionL aThreadFunction, TAny* aThreadContext);
	void Shutdown();
	~CWorkerThread();

	TBool Running() const { return iWorkerThread.Handle() && iWorkerThread.ExitType() == EExitPending; }

public: // From MThreadedTask
	TThreadId GetThreadId() const;
	TInt ExecuteTask(TRequestStatus& aCompletionStatus);
	void AbortTask();

public: // For CThreadDeathWatcher to use
	void SignalClientThatThreadHasDied();

private:
	CWorkerThread(CThreadPool* aParentPool, RAllocator* aSharedAllocator);
	void ConstructL();
	void ThreadRun();
	static TInt ThreadFn(TAny* aSelf);
	void ThreadFnL();

private:
	CThreadPool* iParentPool;
	RThread iWorkerThread;
	RAllocator* iSharedAllocator;
	TRequestStatus* iDispatchStatus;
	CActiveSchedulerWait iAsWait; // Use a CActiveSchedulerWait to protect ourselves from whatever aThreadFunction may try and do
	CThreadDeathWatcher* iThreadDeathWatcher;
	TBool iBusy;

	// Things specific to the task currently being executed
	RThread iParentThread; // Worker needs this to signal completion to us
	const TDesC* iName;
	MTaskRunner::TThreadFunctionL iFn;
	TAny* iContext;
	TInt iTaskId;
	TRequestStatus* iCompletionStatus;

	friend class CWorkerThreadDispatcher;
	friend class CThreadDeathWatcher;
	};

#endif
