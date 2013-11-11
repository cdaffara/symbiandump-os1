/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include <e32math.h>
#include "egltest_threadmonitor.h"


//CThreadMonitor creates a new monitor thread and instanciates a CThreadMonitorBackend object.
//The thread monitor backend is constructed on the monitor thread's heap and only 
//runs in that context. It creates a CThread instance for each thread that it needs
//to monitor. The CThread instance reports back to the monitor backend when the
//thread that it wraps has exitted. The backend is then responsible for deciding 
//how to respond: If the exit type is a panic, it forwards that panic to all the 
//other threads, then exits itself. Note: The controller thread MUST be at position
//zero in the passed in array.
class CThreadMonitorBackend : public CActive
    {
private:
    class CThread : public CActive
        {
    public:
        static CThread* NewL(const TThreadId& aThread, CThreadMonitorBackend& aMonitor);
        ~CThread();
        void Panic(TInt aExitReason, const TExitCategoryName& aExitCategory);
        
    private:
        CThread(CThreadMonitorBackend& aMonitor);
        void ConstructL(const TThreadId& aThread);
        void RunL();
        void DoCancel();
    
    private:
        CThreadMonitorBackend& iMonitor;
        RThread iThread;
        };

public:
    static CThreadMonitorBackend* NewL(const RArray<TThreadId>& aThreadsToMonitor, TRequestStatus*& aNotifyCancel);
    ~CThreadMonitorBackend();
    void StartMonitoring();
    void ThreadExitted(CThread* aThread, TExitType aExitType, TInt aExitReason, const TExitCategoryName& aExitCategory);
    
private:
    CThreadMonitorBackend(TRequestStatus*& aNotifyCancel);
    void ConstructL(const RArray<TThreadId>& aThreadsToMonitor);
    TBool ThreadIsController(CThread* aThread) const;
    void RunL();
    void DoCancel();
    
private:
    RPointerArray<CThread> iThreads;
    };


//CThreadMonitor---------------------------------------------------------------

CThreadMonitor* CThreadMonitor::NewL(const RArray<TThreadId>& aThreadsToMonitor)
    {
    CThreadMonitor* self = new (ELeave) CThreadMonitor(aThreadsToMonitor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CThreadMonitor::~CThreadMonitor()
    {
    //Tell the backend to stop monitoring.
    iMonitor.RequestComplete(iNotifyCancel, KErrNone);
    iMonitor.Close();
    }


CThreadMonitor::CThreadMonitor(const RArray<TThreadId>& aThreadsToMonitor) :
    iThreadsToMonitor(aThreadsToMonitor)
    {
    }


void CThreadMonitor::ConstructL()
    {
    const TInt KStackSize = 12000;
    const TInt KHeapMinSize = 16000;
    const TInt KHeapMaxSize = 1000000;
    
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");
    _LIT(KMonitorName, "EpThreadMonitor");
    threadName.Format(KThreadNameFormat, &KMonitorName, random);
    
    User::LeaveIfError(iMonitor.Create(threadName, MonitorThreadEntry, KStackSize, KHeapMinSize, KHeapMaxSize, this, EOwnerThread));
    TRequestStatus rendezvous;
    iMonitor.Rendezvous(rendezvous);
    iMonitor.Resume();
    User::WaitForRequest(rendezvous);
    User::LeaveIfError(rendezvous.Int());
    ASSERT(iNotifyCancel);
    }


TInt CThreadMonitor::MonitorThreadEntry(TAny* aSelf)
    {
    CThreadMonitor* self = static_cast<CThreadMonitor*>(aSelf);
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TRAPD(err, MonitorThreadEntryL(self->iThreadsToMonitor, self->iNotifyCancel));
    __ASSERT_ALWAYS(err == KErrNone, User::Invariant());

    delete cleanup;
    return KErrNone;
    }


void CThreadMonitor::MonitorThreadEntryL(const RArray<TThreadId>& aThreadsToMonitor, TRequestStatus*& aNotifyCancel)
    {
    //Create active scheduler.
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    //Create the monitor and start monitoring.
    CThreadMonitorBackend* monitor = CThreadMonitorBackend::NewL(aThreadsToMonitor, aNotifyCancel);
    RThread().Rendezvous(KErrNone);
    monitor->StartMonitoring();
    delete monitor;

    //Clean up.
    CleanupStack::PopAndDestroy(scheduler);
    }

//-----------------------------------------------------------------------------


//CThreadMonitorBackend--------------------------------------------------------

CThreadMonitorBackend* CThreadMonitorBackend::NewL(const RArray<TThreadId>& aThreadsToMonitor, TRequestStatus*& aNotifyCancel)
    {
    CThreadMonitorBackend* self = new (ELeave) CThreadMonitorBackend(aNotifyCancel);
    CleanupStack::PushL(self);
    self->ConstructL(aThreadsToMonitor);
    CleanupStack::Pop(self);
    return self;
    }


CThreadMonitorBackend::CThreadMonitorBackend(TRequestStatus*& aNotifyCancel) :
    CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
    
    //Pass the cancel TRequestStatus back to the controller thread.
    aNotifyCancel = &iStatus;
    }


void CThreadMonitorBackend::ConstructL(const RArray<TThreadId>& aThreadsToMonitor)
    {
    //Reserve the space up front so we can gaurantee that the append will not fail.
    //This way we don't need to use the cleanup stack to hold the new CThread while
    //we attempt to append.
    iThreads.ReserveL(aThreadsToMonitor.Count());
    for(TInt i=0; i < aThreadsToMonitor.Count(); i++)
        {
        iThreads.Append(CThread::NewL(aThreadsToMonitor[i], *this));
        }
    }


CThreadMonitorBackend::~CThreadMonitorBackend()
    {
    Cancel();
    iThreads.ResetAndDestroy();
    }


void CThreadMonitorBackend::StartMonitoring()
    {
    CActiveScheduler::Start();
    }


void CThreadMonitorBackend::ThreadExitted(CThread* aThread, TExitType aExitType, TInt aExitReason, const TExitCategoryName& aExitCategory)
    {
    //If a worker thread exits normally, do nothing.
    //If a worker thread panics, forward the panic to all other threads and stop active scheduler.
    //If the controller thread exits normally, stop active scheduler.
    //If the controller thread panics, forward the panic to all other threads and stop active scheduler.
    
    //Stop monitoring according to above.
    if(ThreadIsController(aThread) || aExitType == EExitPanic)
        {
        CActiveScheduler::Stop();
        }

    //Forward panic according to above. Second condition is for when controller times out.
    if(aExitType == EExitPanic || (ThreadIsController(aThread) && aExitType == EExitKill && aExitReason == KErrTimedOut))
        {
        for(TInt i=0; i < iThreads.Count(); i++)
            {
            iThreads[i]->Panic(aExitReason, aExitCategory);
            }
        }
    }


TBool CThreadMonitorBackend::ThreadIsController(CThread* aThread) const
    {
    //The controller thread must be at index zero in the passed in array.
    //Due to way we construct, we gaurantee that it is also at index zero in iThread.
    return (iThreads.Count() > 0) && (iThreads[0] == aThread);
    }


void CThreadMonitorBackend::RunL()
    {
    //The client has destructed the CThreadMonitor object, 
    //so stop the active scheduler so we exit the thread.
    CActiveScheduler::Stop();
    }


void CThreadMonitorBackend::DoCancel()
    {
    //Not ideal, but we should only get here if the thread that created 
    //the original ConitorThread panics, so it should be safe.
    TRequestStatus* status =&iStatus;
    User::RequestComplete(status, KErrCancel);
    }

//-----------------------------------------------------------------------------


//CThreadMonitorBackend::CThread-----------------------------------------------

CThreadMonitorBackend::CThread* CThreadMonitorBackend::CThread::NewL(const TThreadId& aThread, CThreadMonitorBackend& aMonitor)
    {
    CThread* self = new (ELeave) CThread(aMonitor);
    CleanupStack::PushL(self);
    self->ConstructL(aThread);
    CleanupStack::Pop(self);
    return self;
    }


CThreadMonitorBackend::CThread::CThread(CThreadMonitorBackend& aMonitor) :
    CActive(CActive::EPriorityStandard),
    iMonitor(aMonitor)
    {
    CActiveScheduler::Add(this);
    }


void CThreadMonitorBackend::CThread::ConstructL(const TThreadId& aThread)
    {
    User::LeaveIfError(iThread.Open(aThread, EOwnerThread));
    iThread.Logon(iStatus);
    SetActive();
    }


CThreadMonitorBackend::CThread::~CThread()
    {
    Cancel();
    iThread.Close();
    }


void CThreadMonitorBackend::CThread::Panic(TInt aExitReason, const TExitCategoryName& aExitCategory)
    {
    iThread.Panic(aExitCategory, aExitReason);
    }


void CThreadMonitorBackend::CThread::RunL()
    {
    //Inform the monitor backend that the thread exitted.
    TExitCategoryName category = iThread.ExitCategory();
    TInt reason = iThread.ExitReason();
    TExitType type = iThread.ExitType();
    iMonitor.ThreadExitted(this, type, reason, category);
    }


void CThreadMonitorBackend::CThread::DoCancel()
    {
    iThread.LogonCancel(iStatus);
    }

//-----------------------------------------------------------------------------
