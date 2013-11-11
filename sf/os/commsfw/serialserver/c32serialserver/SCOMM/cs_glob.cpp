// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cs_glob.h"
#include <e32std.h>
#include "C32LOG.H"
#include "cs_roles.h"
#include "cs_thread.h"


CC32Data* CC32Data::NewL()
		{
		return new(ELeave) CC32Data();
		}


	
CC32Data::CC32Data()
	{
	}


CC32WorkerThread* CC32Data::SelfWorker() const
	{ 
	return iWorkerThread;
	}

void C32GlobalUtil::InitC32GlobalsL(CC32WorkerThread* aWorker)
	{
	CTLSRedirector* tls = CRedirectorInTls();
	if(tls)
		{
		Fault(EInitTwice);
		}

	// Allocate global storage to be held in tls.
	// only transfer ownership to tls at end since if we fail, the thread is never started
	CC32Data *globals=CC32Data::NewL();

	CleanupStack::PushL(globals);

	// Pre allocate the shutdown timer so that we can always shut down.
	globals->iShutdownWatchDog=CC32ShutdownWatchDog::NewL();
#ifdef _DEBUG
	globals->iShutdownWatchDog->SetWorkerId(aWorker->WorkerId());
#endif
	CleanupStack::PushL(globals->iShutdownWatchDog);

	globals->iWorkerThread = aWorker;
	
	tls = CTLSRedirector::NewL();
	
	CleanupStack::PushL(tls);
	
	CDeltaTimer* timer = CDeltaTimer::NewL(CActive::EPriorityHigh, KCommTimerGranularity);

	tls->SetDeltaTimer(timer); // ownership	transferred
		
	tls->SetC32GlobalData(globals); //ownership transferred

	
	Dll::SetTls(tls);  // owned now indirectly by the worker thread which must now remove it when shutting down

	CleanupStack::Pop(3); //watchdog, globals, tls

	C32LOG1(KC32Bootup, _L("C32GlobalUtil::InitL() Done!"));
	}

void C32GlobalUtil::ShutDown()
/**
free data structures
*/
	{
	// Mark ourself as on the way out
	CC32Data* globals = CC32DataInTls();
	CTLSRedirector* tls = CRedirectorInTls();
	// The CC32Data destructor does not own so does not delete the data it points
	// This is since don't want to delete the Watchdog - it is left to delete itself.
	// So we delete everything else in CC32Data explicitly.

	C32LOG2(KC32Shutdown, _L("--------------- C32Server W%d starting shutdown ---------------"), globals->iWorkerThread->WorkerId());

	delete globals->iWorkerThread;
	delete tls;
	Dll::SetTls(NULL); // so that logger sees it is gone.
	}

void C32GlobalUtil::NewSession()
/**
Increment the session counter - if we were about to shutdown, stop.
*/
	{
	CC32Data* globals = CC32DataInTls();
	globals->iNumSessions++;
	C32LOG2(KC32Detail, _L8("C32GlobalUtil::NewSession() iNumSessions= %d "), globals->iNumSessions );
	}

void C32GlobalUtil::SessionClosing()
/**
A session is closing, if it's the last one take our stumps home.
*/
	{
	CC32Data* globals = CC32DataInTls();
	globals->iNumSessions--;
	C32LOG2(KC32Detail, _L8("C32GlobalUtil::SessionClosing() iNumSessions= %d "), globals->iNumSessions );

	CC32WorkerThread* selfWorker = globals->SelfWorker();
	if (selfWorker->ShuttingDown())
		{
		C32LOG2(KC32Shutdown, _L8("Shutdown requested: %d sessions"), globals->iNumSessions );
		if (globals->iNumSessions <= 0)
			{
			if(selfWorker->IsMainThread())
				{
				selfWorker->DealerByRef().SessionShutdownComplete();
				}
			else
				{
				selfWorker->MaybeTriggerThreadShutdownCallback();
				}
			}
		}
	}
	
	
	
CTLSRedirector* CTLSRedirector::NewL()
		{
		return new(ELeave) CTLSRedirector();
		}


CTLSRedirector::~CTLSRedirector()
	{
	delete iTimer;
	delete iC32Data;
	}


CC32ShutdownWatchDog::CC32ShutdownWatchDog()
	:CTimer(EPriorityLow)
/**
Constructor
*/
	{
	}

void CC32ShutdownWatchDog::Shutdown()
/**
Que ourselves to shut the C32 server down after a suitable delay.
*/
	{
	C32LOG1(KC32Shutdown, _L("CC32ShutdownWatchDog::Shutdown()"));

	if (!IsAdded())
		{
		CActiveScheduler::Add(this);
		}

	if(!IsActive())
		{
		After(KCommServerShutdownLatencyMicroSeconds);
		}
	}

void CC32ShutdownWatchDog::RunL()
	{
	C32LOG1(KC32Shutdown, _L("CC32ShutdownWatchDog::RunL()"));	
	ASSERT(CC32DataInTls()->iNumSessions == 0);
	// End of the line
	C32GlobalUtil::ShutDown();
	CActiveScheduler::Stop();
	// tls is now deleted, so we mimmick our workerId description on the log line:
    C32LOG2(KC32Shutdown, _L("W%d: CC32ShutdownWatchDog::RunL() - about to delete self, which is last step of shutdown"),iWorkerId);
	delete this;
	}

CC32ShutdownWatchDog* CC32ShutdownWatchDog::NewL()
	{
	CC32ShutdownWatchDog* t=new (ELeave) CC32ShutdownWatchDog();
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop(t);
	return t;
	}

#ifdef _DEBUG
void CC32ShutdownWatchDog::SetWorkerId(TWorkerId aWorkerId)
	{
	iWorkerId = aWorkerId;
	}
#endif


// EOF - CS_GLOB.CPP


