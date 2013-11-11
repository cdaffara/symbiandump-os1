// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <mmf/common/mmfcontrollerframework.h>
#include "MmfDrmPluginServer.h"
#include "MmfDrmPluginServerStart.h"
#include "MmfDrmPluginServerSession.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfcontrollerframeworkclasses.h>
#endif

CMMFDRMPluginServer* CMMFDRMPluginServer::NewL()
	{
	CMMFDRMPluginServer* s = new(ELeave) CMMFDRMPluginServer();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFDRMPluginServer::CMMFDRMPluginServer() :
	CMmfIpcServer(EPriorityStandard)
	{
	}

void CMMFDRMPluginServer::ConstructL()
	{
	iDelayServerShutDown = CDelayServerShutDown::NewL();
	// Call base class to Start server
	StartL(KDrmPluginServerName);
	}

CMMFDRMPluginServer::~CMMFDRMPluginServer()
	{
	iControllerServList.ResetAndDestroy();
	iControllerServList.Close();
	delete iDelayServerShutDown;
	}

CMmfIpcSession* CMMFDRMPluginServer::NewSessionL(const TVersion& aVersion) const
	{
	TVersion v(KMMFDRMPluginServerVersion,
				KMMFDRMPluginServerMinorVersionNumber,
				KMMFDRMPluginServerBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		{
		User::Leave(KErrNotSupported);
		}
		
	for(TInt i = 0; i < iControllerServList.Count(); i++)
		{
		CStartAndMonitorControllerThread* controllerMonThread 
			= iControllerServList[i];
		if(!controllerMonThread->IsActive())
			{
			iControllerServList.Remove(i);
			delete controllerMonThread;
			}
		}
	iControllerServList.Compress();
	
	CMMFDRMPluginServerSession* session = CMMFDRMPluginServerSession::NewL();
	return session;
	}

void CMMFDRMPluginServer::IncrementSessionId()
	{
	iSessionCount++;
	}

void CMMFDRMPluginServer::DecrementSessionId()
	{
	iSessionCount--;
	if(iSessionCount == 0 && iControllerCount == 0)
		{
		// Guarantee that server will be closed, after a period of time, even
		// a session has been created without a controller thread initialized 
		if(!iDelayServerShutDown->IsActive())
			{
			// If shut down timer has been started previously, that setup has the piority
			iDelayServerShutDown->SetDelay(TTimeIntervalMicroSeconds32(iServerTimeout));
			}
		}
	}

void CMMFDRMPluginServer::IncrementControllerCount()
	{
	iControllerCount++;
	if(iControllerCount)
		{
		//In the case we started the shutdown due to no more controllers
		iDelayServerShutDown->Cancel(); 
		}
	}

void CMMFDRMPluginServer::DecrementControllerCount()
	{
	iControllerCount--;
    if (iControllerCount == 0)
		{
		iDelayServerShutDown->SetDelay(TTimeIntervalMicroSeconds32(iServerTimeout));
 		}
	}
	
TInt CMMFDRMPluginServer::StartControllerServer(const RThread& aClientThread, TUint aMaxHeapSize, TBool aUseSharedHeap,
											RMMFControllerServerProxy& aControllerSessionHandle, TThreadId& aControllerTID, TUint aStackSize) const
	{
	CStartAndMonitorControllerThread* monitor = NULL;

	TRAPD(err, monitor = 
		CStartAndMonitorControllerThread::NewL(const_cast<CMMFDRMPluginServer*>(this)));
	if(err != KErrNone)
		{
		delete monitor;
		return err;
		}

	err = iControllerServList.Append(monitor);
	if(err != KErrNone)
		{
		delete monitor;
		return err;
		}
	
	return monitor->StartControllerServer(aClientThread, aMaxHeapSize, aUseSharedHeap, 
										aControllerSessionHandle, aControllerTID, aStackSize);
	}

void CMMFDRMPluginServer::PanicControllerThread(TThreadId aTid, const TDesC& aCategory,TInt aReason)
	{
	for(TInt i = 0; i < iControllerServList.Count(); i++)
		{
		if(iControllerServList[i]->Thread().Id() == aTid)
			{
			iControllerServList[i]->Thread().Panic(aCategory, aReason);
			return;
			}
		}
	}

void CMMFDRMPluginServer::KillControllerThread(TThreadId aTid, TInt aReason)
	{
	for(TInt i = 0; i < iControllerServList.Count(); i++)
		{
		if(iControllerServList[i]->Thread().Id() == aTid)
			{
			iControllerServList[i]->Thread().Kill(aReason);
			return;
			}
		}
	}

TInt CMMFDRMPluginServer::SetThreadPriority(TThreadId aTid, TThreadPriority aPriority)
	{
	TBool threadFound = EFalse;
	for(TInt i = 0; i < iControllerServList.Count(); i++)
		{
		const RThread& controllerThread = iControllerServList[i]->Thread();
		if (controllerThread.Id() == aTid)
			{
			if (controllerThread.Priority() != aPriority)
				{
				controllerThread.Suspend();
				controllerThread.SetPriority(aPriority);
				controllerThread.Resume();
				}
			threadFound = ETrue;
			break;
			}
		}
	return threadFound? KErrNone: KErrNotFound;
	}

CMMFDRMPluginServer::CDelayServerShutDown* CMMFDRMPluginServer::CDelayServerShutDown::NewL()
	{
	CDelayServerShutDown* self = new(ELeave) CDelayServerShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFDRMPluginServer::CDelayServerShutDown::CDelayServerShutDown() : CActive(0)
	{
	}
        
void CMMFDRMPluginServer::CDelayServerShutDown::ConstructL()
	{
	User::LeaveIfError(iShutDownTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}

CMMFDRMPluginServer::CDelayServerShutDown::~CDelayServerShutDown()
	{
	Cancel();
	iShutDownTimer.Close();
	}

void CMMFDRMPluginServer::CDelayServerShutDown::SetDelay(TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(_L("CMMFDRMPluginServer"), 1));
	iShutDownTimer.After(iStatus, aDelay);
	SetActive();
	}

void CMMFDRMPluginServer::CDelayServerShutDown::RunL()
	{
	CActiveScheduler::Stop();
	}


void CMMFDRMPluginServer::CDelayServerShutDown::DoCancel()
	{
	iShutDownTimer.Cancel();
	}

CStartAndMonitorControllerThread* CStartAndMonitorControllerThread::NewL(CMMFDRMPluginServer* aPluginServer)
	{
	CStartAndMonitorControllerThread* self = new(ELeave) CStartAndMonitorControllerThread(aPluginServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CStartAndMonitorControllerThread::ConstructL()
	{
	CActiveScheduler::Add(this);
	}
	
TInt CStartAndMonitorControllerThread::StartControllerServer(const RThread& /*aClientThread*/, TUint aMaxHeapSize, TBool aUseSharedHeap,
													RMMFControllerServerProxy& aControllerSessionHandle, TThreadId& aControllerTID,TUint aStackSize)
	{
	Cancel();
	TInt err = KErrNone;

	RServer2 controllerServer;	
	TControllerProxyServerParams params;
	params.iServer = &controllerServer;
	params.iUsingSharedHeap = aUseSharedHeap;
	
	TName name;
	name.Append(KMMFControllerProxyServerName);
	
	// Threads create own heap (default behaviour)
	if(aMaxHeapSize < static_cast<TUint>(KMinHeapSize))
		{
		aMaxHeapSize = KMinHeapSize; //else raises a USER 111 panic
		}
	else if(aMaxHeapSize >  static_cast<TUint>(KMMFControllerProxyMaxHeapSize))
		{
		aMaxHeapSize = KMMFControllerProxyMaxHeapSize;
		}
	
	err = iServerThread.Create(name, &CMMFControllerProxyServer::StartThread,
		aStackSize, KMinHeapSize, aMaxHeapSize, 
		&params, EOwnerThread);
	if (err)
		{
		return err;
		}
		
	// Synchronise with the server
	TRequestStatus reqStatus;
	iServerThread.Rendezvous(reqStatus);
	
	if (reqStatus!=KRequestPending)
		{
		iServerThread.Kill(0);
		}
	else
		{
		iServerThread.SetPriority(EPriorityNormal);
		iServerThread.Resume();
		// Server will call the reciprocal static synchronise call	
		}

	User::WaitForRequest(reqStatus); // wait for start or death
	if(reqStatus.Int() != KErrNone)
		{
		iServerThread.Close();
		controllerServer.Close();
		return reqStatus.Int();
		}
	
	err = aControllerSessionHandle.Open(controllerServer);
	if(err != KErrNone)
		{
		iServerThread.Close();
		controllerServer.Close();
		return err;
		}
	
	iStatus = KRequestPending;
	iServerThread.Logon(iStatus);
	if (iStatus != KRequestPending)
		{
		// Failed to logon
		aControllerSessionHandle.Close();
		iServerThread.Close();
		controllerServer.Close();
		return iStatus.Int();
		}
	else
		{
		SetActive();
		}
		
	aControllerSessionHandle.ShareProtected();
	aControllerTID = iServerThread.Id();
	iDrmPluginServer->IncrementControllerCount();
	return KErrNone;
	}


CStartAndMonitorControllerThread::CStartAndMonitorControllerThread(CMMFDRMPluginServer* aPluginServer)
	: CActive(EPriorityStandard)
	{
	iDrmPluginServer = aPluginServer;
	}

CStartAndMonitorControllerThread::~CStartAndMonitorControllerThread()
	{
	Cancel();
	}
    
void CStartAndMonitorControllerThread::RunL()
	{
	iServerThread.Close();
	iDrmPluginServer->DecrementControllerCount();
	}

void CStartAndMonitorControllerThread::DoCancel()
	{
	iServerThread.LogonCancel(iStatus);
	iServerThread.Kill(0);
	iServerThread.Close();
	iDrmPluginServer->DecrementControllerCount();
	}

TInt RMMFControllerServerProxy::Open(RServer2& aControllerServerHandle)
	{
	TInt err = CreateSession(aControllerServerHandle, KMMFControllerProxyVersion, 
							-1, EIpcSession_GlobalSharable);
	return err;
	}
