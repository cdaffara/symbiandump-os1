// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/server/mmfsubthreadbase.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/server/mmfsubthreadbaseimpl.h>
#endif


EXPORT_C void RMMFSubThreadBase::ReceiveEvents(TMMFEventPckg& aEventPckg, TRequestStatus& aStatus)
	{
	SendReceiveResult(EMMFSubThreadReceiveEvents, aEventPckg, aStatus);
	}

EXPORT_C TInt RMMFSubThreadBase::CancelReceiveEvents()
	{
	return SendReceive(EMMFSubThreadCancelReceiveEvents);
	}

EXPORT_C void RMMFSubThreadBase::Shutdown()
	{
	if (iSubThread.Handle() != KNullHandle)
		{
		// check that the subthread has yet to terminate
		if (iLogonStatus == KRequestPending)
			{	
			//close the subthread and wait for its exit.
			//signal to the subthread to close down.			
			TInt shutdownError = SendReceive(EMMFSubThreadShutdown);
			if (shutdownError != KErrNone)
				{
				iSubThread.LogonCancel(iLogonStatus);
				}
			else
				{
				RTimer timer;
				TInt err = timer.CreateLocal();
				//if we managed to create the timer, wait for both the death and the timeout to minimise the risk of deadlock
				if (!err)
					{
					TRequestStatus timeout;
					timer.After(timeout, iShutdownTimeout);
					User::WaitForRequest(iLogonStatus, timeout);
					if (iLogonStatus == KRequestPending)
						{
						//we have timed out.  Panic the thread in debug mode, kill it in release mode.
						iSubThread.LogonCancel(iLogonStatus);
						User::WaitForRequest(iLogonStatus);
					#ifdef _DEBUG	
						Panic(EMMFSubThreadPanicTimedOut);
					#else
						iSubThread.Kill(KErrDied);
					#endif
						}
					else
						{
						//subthread has exited. Cancel the timer.
						timer.Cancel();
						User::WaitForRequest(timeout);
						}
					}
				else
					{
					//we have no timer so we'll just have to risk the deadlock
					User::WaitForRequest(iLogonStatus);
					}
				timer.Close();
				}
			}
		//close the handle to the subthread
		iSubThread.Close();
		}
	// close our parent
	RHandleBase::Close(); 
	}
	
void RMMFSubThreadBase::Panic(TMMFSubThreadPanicCode aPanicCode)
	{
	_LIT(KMMFSubThreadPanicCategory, "MMFSubThread");
	iSubThread.Panic(KMMFSubThreadPanicCategory, aPanicCode);
	}

TInt RMMFSubThreadBase::DoCreateSubThread(const TDesC& aName, TThreadFunction aFunction, TBool aUseNewHeap)
	{
	TInt error = KErrNone;

	if (aUseNewHeap)
		{
		error = iSubThread.Create(aName, aFunction,
			KDefaultStackSize, KMinHeapSize, KMMFSubThreadMaxHeapSize, 
			NULL, EOwnerThread);
		}
	else
		{
		error = iSubThread.Create(aName, aFunction,
			KDefaultStackSize, &User::Heap(), 
			NULL, EOwnerThread);
		}

	if (error)
		{
		return error;
		}
	
	TRequestStatus rendezvous;
	iSubThread.Rendezvous(rendezvous);
	
	if (rendezvous != KRequestPending)
		{
		iSubThread.Kill(0);
		}
	else
		{
		iLogonStatus = KRequestPending;
		iSubThread.Logon(iLogonStatus);
		if (iLogonStatus != KRequestPending)
			{
			// Failed to log on
			iSubThread.RendezvousCancel(rendezvous);
			User::WaitForRequest(rendezvous);
			iSubThread.Kill(0);
			iSubThread.Close();
			return iLogonStatus.Int();
			}
		else
			{
			iSubThread.Resume();		
			}
		}
	User::WaitForRequest(rendezvous); // wait for start or death
	
	if (rendezvous != KErrNone)
		{
		iSubThread.LogonCancel(iLogonStatus);
		User::WaitForRequest(iLogonStatus);
		iSubThread.Close();
		return rendezvous.Int();
		}
		
	return KErrNone;	
	}

CMMFSubThreadShutdown* CMMFSubThreadShutdown::NewL() 
	{
	CMMFSubThreadShutdown* s = new(ELeave) CMMFSubThreadShutdown();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFSubThreadShutdown::CMMFSubThreadShutdown() : 
	CTimer(EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}

void CMMFSubThreadShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

void CMMFSubThreadShutdown::Start()
	{
	After(EMMFSubThreadShutdownDelay);
	}

void CMMFSubThreadShutdown::RunL()
	{
	ShutdownNow();
	}

void CMMFSubThreadShutdown::ShutdownNow()
	{
	CActiveScheduler::Stop();
	}




CMMFSubThreadServer::CMMFSubThreadServer(TInt aPriority) :
	CMmfIpcServer(aPriority)
	{
	}

void CMMFSubThreadServer::ConstructL()
	{
	StartL(RThread().Name());
	iShutdownTimer = CMMFSubThreadShutdown::NewL();
	iShutdownTimer->Start();
	}

CMMFSubThreadServer::~CMMFSubThreadServer()
	{
	delete iShutdownTimer;
	}

void CMMFSubThreadServer::SessionCreated()
	{
	//stop the shutdown timer
	iShutdownTimer->Cancel();
	}

void CMMFSubThreadServer::ShutdownNow()
	{
	iShutdownTimer->ShutdownNow();
	}

TInt CMMFSubThreadServer::RunError(TInt aError)
	{
	//signal the client
	Message().Complete(aError);
	ReStart();
	return KErrNone;
	}





CMMFSubThreadEventReceiver* CMMFSubThreadEventReceiver::NewL(const RMmfIpcMessage& aMessage)
	{
	return new(ELeave) CMMFSubThreadEventReceiver(aMessage);
	}

CMMFSubThreadEventReceiver::~CMMFSubThreadEventReceiver()
	{
	if (iNeedToCompleteMessage)
		iMessage.Complete(KErrDied);
	}

void CMMFSubThreadEventReceiver::SendEvent(const TMMFEvent& aEvent)
	{
	TMMFEventPckg eventpckg(aEvent);
	TRAPD(err, MmfMessageUtil::WriteL(iMessage, 0, eventpckg));
	iMessage.Complete(err);
	iNeedToCompleteMessage = EFalse;
	}

CMMFSubThreadEventReceiver::CMMFSubThreadEventReceiver(const RMmfIpcMessage& aMessage) : 
	iMessage(aMessage)
	{
	iNeedToCompleteMessage = ETrue;
	}






void CMMFSubThreadSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	iServer = STATIC_CAST(CMMFSubThreadServer*, (CONST_CAST(CMmfIpcServer*, &aServer)));
	iServer->SessionCreated();
	}


CMMFSubThreadSession::~CMMFSubThreadSession()
	{
	delete iEventReceiver;
	iEvents.Close();
	}


TBool CMMFSubThreadSession::ReceiveEventsL(const RMmfIpcMessage& aMessage)
	{
	if (iEventReceiver)
		User::Leave(KErrAlreadyExists);
	iEventReceiver = CMMFSubThreadEventReceiver::NewL(aMessage);
	//send the next cached event (if any) to the client
	if (iEvents.Count() > 0)
		{
		TMMFEvent& event = iEvents[0];
		iEventReceiver->SendEvent(event);
		delete iEventReceiver;
		iEventReceiver=NULL;
		iEvents.Remove(0);
		}
	return EFalse;
	}

TBool CMMFSubThreadSession::CancelReceiveEvents()
	{
	delete iEventReceiver;
	iEventReceiver = NULL;
	return ETrue;
	}

TBool CMMFSubThreadSession::ShutDown()
	{
	iServer->ShutdownNow();
	return ETrue;
	}

TInt CMMFSubThreadSession::SendEventToClient(const TMMFEvent& aEvent)
	{
	TInt error = KErrNone;
	if (iEventReceiver)
		{
		//send event to client now
		iEventReceiver->SendEvent(aEvent);
		delete iEventReceiver;
		iEventReceiver=NULL;
		error = KErrNone;
		}
	else
		{
		//queue the request for later
		TMMFEvent event(aEvent);
		//if we've exceeded the max number of cached messages, delete the first and append this one to the end
		if (iEvents.Count() >= KMMFSubThreadMaxCachedMessages)
			iEvents.Remove(0);
		error = iEvents.Append(event);
		}
	return error;
	}


