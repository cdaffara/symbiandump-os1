// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "creceivethreadmanagerTraces.h"
#endif

#include "creceivethreadmanager.h"
#include <ctsy/ltsy/ltsylogger.h>

CReceiveThreadManager* CReceiveThreadManager::NewL(MLtsyReceiveThreadEventStateHandler& aEventHandler, MLtsyReceiveThreadObserver& aLifeObserver)
	{
	CReceiveThreadManager* self = new(ELeave) CReceiveThreadManager();
	CleanupStack::PushL(self);
	self->ConstructL(aEventHandler, aLifeObserver);
	CleanupStack::Pop(self);
	return self;
	}

CReceiveThreadManager::~CReceiveThreadManager()
	{
	StopReceiveThread();
	delete iThreadLifeWatcher;
	delete iReceiveThreadWaitLoopSignaller;
	}

CReceiveThreadManager::CReceiveThreadManager()
	{	
	}

void CReceiveThreadManager::ConstructL(MLtsyReceiveThreadEventStateHandler& aEventHandler, MLtsyReceiveThreadObserver& aLifeObserver)
	{
	iThreadLifeWatcher = CReceiveThreadLifeWatcher::NewL(aLifeObserver);
	iReceiveThreadWaitLoopSignaller = CReceiveThreadWaitLoopSignaller::NewL(aEventHandler);
	}

void CReceiveThreadManager::StartReceiveThreadL()
	{
	SpawnReceiveThreadL();
	}

void CReceiveThreadManager::SpawnReceiveThreadL()
	{
	TSYLOGENTRYEXIT;
	
	//These may need chsanging for specific LTSY requirements
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;
		
	_LIT(KThreadName,"LtsyReceiveThread");

	User::LeaveIfError(iThread.Create(KThreadName,StartReceiveThreadFn,
					    KStackSize,KHeapSize,KMaxHeapSize,iReceiveThreadWaitLoopSignaller));
	
	iReceiveThreadWaitLoopSignaller->StartReceiver();	
	iThreadLifeWatcher->LogonL(iThread.Id());
	iThread.Resume();		
	//NOTE: We don't bother rendevous'ing and rely on the logon to complete #
	//if thread does not start correctly. 
	}
 
 
 TInt CReceiveThreadManager::StartReceiveThreadFn(TAny* aArg)
 	{
 	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVETHREADMANAGER_SPAWNRECEIVETHREADL_1, "StartReceiveThreadFn, thread id = %d", RThread().Id().Id());
 	
	// Create a cleanup stack object
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		return KErrNoMemory;
	
 	//start the wait loop
 	CReceiveThreadWaitLoopSignaller* signaller = static_cast<CReceiveThreadWaitLoopSignaller*>(aArg);
 	TInt err =  signaller->WaitLoop();
 	
 	delete cleanup;
 	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVETHREADMANAGER_SPAWNRECEIVETHREADL_2, "StartReceiveThreadFn, thread id = %d, Terminated with err = %d", RThread().Id().Id(), err);
 	return err;
 	}

 void CReceiveThreadManager::StopReceiveThread()
 	{
 	TSYLOGENTRYEXIT;
 	/*
 	 * There are two ways to stop a thread
 	 * 1. Use RThread::Kill
 	 * 2. Post a request that is handled the next time the thread runs.
 	 * 
 	 * Problem wth 2. is that it is indeterminate the time before the thread will run.  For us,
 	 * it will run the next time an event is received from the baseband.
 	 * 
 	 * Kill is less graceful but if StopReceiveThread has been called
 	 * chances are we are on our way to a reboot anyway!
 	 */
 	iThread.Kill(KErrNone);
 	iThread.Close();
 	}
 

//

 CReceiveThreadLifeWatcher* CReceiveThreadLifeWatcher::NewL(MLtsyReceiveThreadObserver& aReceiveThreadLifeObserver)
	{
	CReceiveThreadLifeWatcher* self = new(ELeave) CReceiveThreadLifeWatcher(aReceiveThreadLifeObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
 CReceiveThreadLifeWatcher::CReceiveThreadLifeWatcher(MLtsyReceiveThreadObserver& aReceiveThreadLifeObserver)
 : CActive(EPriorityHigh), iReceiveThreadLifeObserver(aReceiveThreadLifeObserver) //use high priority to get quicker scheduling of AO when event triggered
	 {
	 CActiveScheduler::Add(this);
	 }
 
 CReceiveThreadLifeWatcher::~CReceiveThreadLifeWatcher()
	 {
	 TSYLOGENTRYEXIT;
	 Cancel();
	 iThread.Close();
	 }

void CReceiveThreadLifeWatcher::ConstructL()
	{			
	}

void CReceiveThreadLifeWatcher::LogonL(TThreadId aID)
	{
	iThread.Close();
	User::LeaveIfError(iThread.Open(aID));
	iThread.Logon(iStatus);
	SetActive();
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVETHREADLIFEWATCHER_LOGONL_1, "CReceiveThreadLifeWatcher::LogonL.  Watching thread id = %d", iThread.Id().Id());
	}

void CReceiveThreadLifeWatcher::RunL()
	{
	TSYLOGENTRYEXIT;
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRECEIVETHREADLIFEWATCHER_RUNL_1, "CReceiveThreadLifeWatcher::RunL iThread = %d, iStatus=%d, exit reason = %d", iThread.Id().Id(), iStatus.Int(), iThread.ExitReason() );
	iReceiveThreadLifeObserver.HandleReceiveThreadLifeWatcherComplete(iStatus.Int());
	}

void CReceiveThreadLifeWatcher::DoCancel()
	{
	iThread.LogonCancel(iStatus);
	}

//

CReceiveThreadWaitLoopSignaller* CReceiveThreadWaitLoopSignaller::NewL(MLtsyReceiveThreadEventStateHandler& aEventHandler)
	{
	CReceiveThreadWaitLoopSignaller* self = new(ELeave) CReceiveThreadWaitLoopSignaller(aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CReceiveThreadWaitLoopSignaller::CReceiveThreadWaitLoopSignaller(MLtsyReceiveThreadEventStateHandler& aEventHandler)
 : CActive(EPriorityNormal), iEventHandler(aEventHandler)
	 {
	 CActiveScheduler::Add(this);
	 }
 
CReceiveThreadWaitLoopSignaller::~CReceiveThreadWaitLoopSignaller()
	 {
	 TSYLOGENTRYEXIT;
	 Cancel();
	 iEtelThread.Close();
	 iLtsyReceiveThread.Close();
	 }

void CReceiveThreadWaitLoopSignaller::ConstructL()
	{	
	// open a reference to the current thread - this will be the thread on which this object 
	// is created which will be the Etel thread. 
	User::LeaveIfError(iEtelThread.Open(RThread().Id()));
	}

void CReceiveThreadWaitLoopSignaller::StartReceiver()
	{
	//start listening for completions from the LtsyReceiveThread
	iStatus = KRequestPending;
	SetActive();
	}

void CReceiveThreadWaitLoopSignaller::RunL()
	{
	TSYLOGENTRYEXIT;
	//an event has been signalled from the LtsyReceiveThread - handle it
	TInt err = iEventHandler.HandleReceivedEvent();
	//re-start listening for next event
	StartReceiver();	
	//signal back to the LtsyReceiveThread that we have completed so it resumes
	RequestComplete(iLtsyReceiveThread, iLtsyReceiveThreadReqStatus, err);
	}

void CReceiveThreadWaitLoopSignaller::DoCancel()
	{
	TSYLOGENTRYEXIT;
	//only complete if still active otherwise get a stray signal
	if(iStatus == KRequestPending) //if request still pending then complete it otherwise event is awaiitng to be consumed by the User::WaitForRequest in the Cancel
		RequestComplete(iEtelThread, iStatus, KErrCancel);

	}

//This function will be run within the LtsyReceiveThread thread of execution
TInt CReceiveThreadWaitLoopSignaller::WaitLoop()
	{
	iLtsyReceiveThread.Open(RThread().Id());
	TInt err;
	//forever loop
	do
		{
		err = KErrNone;
		//wait for an event
		iEventHandler.DoRequestNextEvent();
		//signal event to the other (Etel) thread - this is synchronous.
		//We wait for event to be handled and the Etel thread to signal its 
		//completion before requesting next event.  This is because there is no queuing 
		//on Etel thread
		err = ExecSync();
		//cleanup - opportunity to free any meory associated with the event
		iEventHandler.DoReleaseEvent();
		//if ExecSync returned an error we break out the event loop
		//this will cause the thread to terminate
		if(err == MLtsyReceiveThreadEventStateHandler::KShutdownLtsyReceiveThread)
			{
			//graceful shutdown
			err = KErrNone;
			break;
			}
		else if(err == KErrNone)
			{
			continue;
			}
		else
			{
			//terminate thread 
			break;
			}
		}while(ETrue);
	return err;
	}

TInt CReceiveThreadWaitLoopSignaller::ExecSync()
	{
	//signal Etel thread
	iLtsyReceiveThreadReqStatus = KRequestPending;
	//check that ETEL thread is actually waiting for an event
	TInt err;
	if(IsActive())
		{
		RequestComplete(iEtelThread, iStatus, KErrNone);
		//block awaiting completion from Etel thread to say event has been processed
		User::WaitForRequest(iLtsyReceiveThreadReqStatus);
		err = iLtsyReceiveThreadReqStatus.Int();
		}
	else
		{
		//if ETEL thread not listening we just close as events will be lost otherwise
		err = KErrNotReady;
		}
	return err;
	}

void CReceiveThreadWaitLoopSignaller::RequestComplete(RThread& aThread, TRequestStatus& aStatus, TInt aError)
	{
	TRequestStatus* status = &aStatus;
	aThread.RequestComplete(status, aError);
	}


