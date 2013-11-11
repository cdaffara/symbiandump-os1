// undertaker.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/iocli.h>
#include "undertaker.h"

TInt CCmdUndertaker::Queue()
	{
	TInt err = iUndertaker.Logon(iStatus, iDeadThreadHandle);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

void CCmdUndertaker::DoCancel()
	{
	iUndertaker.LogonCancel();
	}

void CCmdUndertaker::RunL()
	{
	TInt handle = iDeadThreadHandle;
	Queue(); // Queue early, to try and avoid missing notifications
	// We don't use the RunL if we're in paranoid mode - the undertaker notifications are serviced directly in DoWork() without an active scheduler
	ProcessHandle(handle);
	}
	
void CCmdUndertaker::ProcessHandle(TInt aDeadThreadHandle)
	{
	RThread deadThread;
	deadThread.SetHandle(aDeadThreadHandle);
	TFullName name(deadThread.FullName());
	TExitType type = deadThread.ExitType();
	if (type != EExitKill || deadThread.ExitReason() != 0 || iAll)
		{
		Write(_L("Thread "));
		Write(name);
		Printf(_L(" (tid=%d) "), (TUint)deadThread.Id());
		}

	if (type == EExitPanic)
		{
		TExitCategoryName cat = deadThread.ExitCategory();
		Printf(_L("panicked with %S %d\r\n"), &cat, deadThread.ExitReason());
		}
	else if (type == EExitTerminate)
		{
		Printf(_L("terminated with reason %d\r\n"), deadThread.ExitReason());
		}
	else if (deadThread.ExitReason() != 0)
		{
		// We'll consider a kill with non-zero exit code as counting as abnormal
		Printf(_L("killed with reason %d\r\n"), deadThread.ExitReason());
		}
	else if (iAll)
		{
		Printf(_L("exited cleanly\r\n"));
		}
		
	if (!iLeakThreads)
		{
		deadThread.Close();
		}
	}

CCommandBase* CCmdUndertaker::NewLC()
	{
	CCmdUndertaker* self = new(ELeave) CCmdUndertaker();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCmdUndertaker::~CCmdUndertaker()
	{
	Cancel();
	if (iWorkerThread.Id() != RThread().Id()) iWorkerThread.Kill(KErrAbort);
	iHandles.Close();
	CloseProcessOwnedHandles();
	}

CCmdUndertaker::CCmdUndertaker()
	: CMemoryAccessCommandBase(EManualComplete)
	{
	}

void CCmdUndertaker::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CCmdUndertaker::Name() const
	{
	_LIT(KName, "undertaker");
	return KName;
	}

void CCmdUndertaker::DoRunL()
	{	
	User::LeaveIfError(iUndertaker.Create());

	if (iParanoid)
		{
		User::LeaveIfError(iMainThread.Open(RThread().Id()));
		User::LeaveIfError(iLock.CreateLocal());
		iHandles.ReserveL(10);
		TFullName threadName(RThread().Name());
		threadName.Append(_L("_worker"));
		LeaveIfErr(iWorkerThread.Create(threadName, &WorkerThreadFn, 8192, NULL, this), _L("Couldn't create worker thread"));
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
		LoadMemoryAccessL();
		LeaveIfErr(iMemAccess.SetThreadPriority(iWorkerThread, 31), _L("Couldn't set worker thread priority with memoryaccess"));
#else
		iWorkerThread.SetPriority(EPriorityMuchMore); // Best we can do
#endif
		iWorkerThread.Resume();
		}
	else
		{
		User::LeaveIfError(Queue());
		}	
		
	if (Stdin().IsForeground() > 0)
		{
		Write(iAll ? _L("Waiting for any thread exit...\r\n") : _L("Waiting for panicked thread exit...\r\n"));
		}
		
	if (iParanoid)
		{
		// We've spawned off our worker thread, which is the client of the RUndertaker. We now wait for it to signal us back again.
		// It's easier to balance the requests if we don't go through the active scheduler (so long as nothing else signals this thread!)
		for (;;)
			{
			User::WaitForRequest(iMainThreadStat);
			if (iMainThreadStat.Int() != KErrNone) User::Leave(iMainThreadStat.Int());
			iLock.Wait();
			TInt handle = iHandles[0];
			iHandles.Remove(0);
			iLock.Signal();
			ProcessHandle(handle);
			}
		}
	}

void CCmdUndertaker::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptAll, "all");
	_LIT(KOptNoClose, "noclose");
	_LIT(KOptParanoid, "paranoid");
	aOptions.AppendBoolL(iAll, KOptAll);
	aOptions.AppendBoolL(iLeakThreads, KOptNoClose);
	aOptions.AppendBoolL(iParanoid, KOptParanoid);
	}

TInt CCmdUndertaker::WorkerThreadFn(TAny* aSelf)
	{
	CCmdUndertaker* self = static_cast<CCmdUndertaker*>(aSelf);
	return self->DoWork();
	}
	
TInt CCmdUndertaker::DoWork()
	{
	TRequestStatus mainThreadDeadStat;
	iMainThread.Logon(mainThreadDeadStat);
	
	TRequestStatus stat;
	TInt deadThreadHandle;
	TInt err = KErrNone;
	for (;;)
		{
		err = iUndertaker.Logon(stat, deadThreadHandle);
		TRequestStatus* s = &iMainThreadStat;
		if (err)
			{
			iMainThread.RequestComplete(s, err);
			break;
			}
		else
			{
			User::WaitForRequest(stat, mainThreadDeadStat);
			if (mainThreadDeadStat.Int() != KRequestPending)
				{
				// We're dead
				err = mainThreadDeadStat.Int();
				// Have to clean up our process-owned handles here...
				CloseProcessOwnedHandles();
				break;
				}
			// Have to duplicate the thread handle so the main thread can see it. Can't seem to persuade RUndertaker to give us a process-owned handle in the first place
			RThread origHandle;
			origHandle.SetHandle(deadThreadHandle);
			RThread newHandle(origHandle);
			err = newHandle.Duplicate(RThread(), EOwnerProcess);
			origHandle.Close();
			if (!err)
				{
				iLock.Wait();
				err = iHandles.Append(newHandle.Handle());
				iLock.Signal();
				}
			iMainThread.RequestComplete(s, err);
			}
		}
	return err;
	}

void CCmdUndertaker::CloseProcessOwnedHandles()
	{
	iMainThread.Close();
	iWorkerThread.Close();
	iUndertaker.Close();
	iLock.Close();
	}

#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdUndertaker)
#endif

