// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "openwfcthreadmanager.h"
#include "openwfcjobmanager.h"


COpenWfcMonitorThread::COpenWfcMonitorThread(COpenWfcJobManger& aManager):
iEndThread(EFalse),
iManager(aManager)
	{
	
	}

COpenWfcMonitorThread::~COpenWfcMonitorThread()
	{
	EndThread();
	TRequestStatus threadDies = KErrNone;
	iThread.Resume();
	iThread.Logon(threadDies);
	if (iThread.ExitType() == EExitPending || 
		threadDies != KRequestPending)
		{
		User::WaitForRequest(threadDies);
		}
	iThread.Close();
	iSemaphore.Close();
	}

void COpenWfcMonitorThread::ConstructL(TInt aScreenNumber)
	{
	TBuf <255> threadManagerName;
	/*
	 * At a given instant in time, one screen can have at most one monitor
	 * thread.  Test code will, for a given screen, destroy and re-create
	 * the monitor thread.
	 * 
	 * At appears that the thread, once killed off, still lingers in
	 * the OS for a short time.  The newly created thread fails to create
	 * if it shares the same name as the one that was killed but still is
	 * lingering around.
	 * 
	 * A unique name is needed.  This is guaranteed by using a thread name
	 * which includes a microsecond counter.  This counter wraps every hour,
	 * roughly.  This is comfortably outside the race condition above.
	 * 
	 * To make debugging easier, we also include the screen number in
	 * the thread name.
	 * 
	 * Thus, the thread name is:
	 * WFCMonitorThread_<ScreenNumber>_<MicrosecondCounter>
	 */
	_LIT(KThreadName, "WFCMonitorThread_%d_%d");
	TTime now;
	now.UniversalTime();
	TInt microseconds = I64LOW(now.Int64());
	threadManagerName.Format(KThreadName, aScreenNumber, microseconds);

	User::LeaveIfError(iThread.Create(threadManagerName, 
					   static_cast<TThreadFunction>(&COpenWfcMonitorThread::Main), 
			           KDefaultStackSize, 
			           NULL, 
			           this,
			           EOwnerProcess));	
	iThread.SetPriority(EPriorityNormal);
	User::LeaveIfError(iSemaphore.CreateLocal(0));
  Start();
	}

COpenWfcMonitorThread* COpenWfcMonitorThread::NewLC(TInt aNumber, COpenWfcJobManger& aManager)
	{
	COpenWfcMonitorThread* self = new(ELeave) COpenWfcMonitorThread(aManager);
	CleanupStack::PushL(self);
	self->ConstructL(aNumber);
	return(self);
	}

COpenWfcMonitorThread* COpenWfcMonitorThread::NewL(TInt aNumber, COpenWfcJobManger& aManager)
	{
	COpenWfcMonitorThread* self = COpenWfcMonitorThread::NewLC(aNumber, aManager);
	CleanupStack::Pop(self);
	return self;
	}

TInt COpenWfcMonitorThread::Main(TAny *aSelf)
	{
	COpenWfcMonitorThread* self = static_cast<COpenWfcMonitorThread*>(aSelf);
    return self->Run();
	}

void COpenWfcMonitorThread::Start()
	{
	iThread.Rendezvous(iThreadStatus);
	iThread.Resume();
	User::WaitForRequest(iThreadStatus);
	}

void COpenWfcMonitorThread::Resume()
	{
	iThread.Resume();
	}

void COpenWfcMonitorThread::EndThread()
	{
    iSemaphore.Signal();
	iEndThread = ETrue;
	}

void COpenWfcMonitorThread::Suspend()
	{
	iThread.Suspend();
	}

void COpenWfcMonitorThread::Signal()
    {
    iSemaphore.Signal();
    }

TInt COpenWfcMonitorThread::Run()
	{
	RThread::Rendezvous(KErrNone);
    JQLOG(("** START * COpenWfcMonitorThread::Run()"));
    while(!iEndThread)
        {
        JQLOG(("** WAIT FOR SIGNAL * CCOpenwfcMonitorThread::Run()"));
        iSemaphore.Wait();
        
        if (!iEndThread)
            {
            iManager.DoExecuteJob();
            }
        }

    /* Release any use of EGL by this thread. */
    eglReleaseThread();

    JQLOG(("** EXIT * COpenWfcMonitorThread::Run()"));
	return KErrNone;
	}
