// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "MmfAudioServer.h"
#include "MmfAudioServerStart.h"
#include "MmfDevSoundSession.h"
#include "MmfAudioServerSession.h"
#include "MmfDevSoundServer.h"
#include "MmfDevSoundServerStart.h"
#include "mmfbase.hrh"


static const TUid KUidDevSoundServer = {KUidMmfDevSoundServerDllUnicodeDefine};

const TInt KAudioServerShutDownDelay = 50000000; //50 sec


CMMFAudioServer* CMMFAudioServer::NewL()
	{
	CMMFAudioServer* s = new(ELeave) CMMFAudioServer();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFAudioServer::CMMFAudioServer() :
	CMmfIpcServer(EPriorityStandard)
	{
	}

void CMMFAudioServer::ConstructL()
	{
	// Call base class to Start server
	StartL(KAudioServerName);
	iDelayAudioServerShutDown = CDelayAudioServerShutDown::NewL();
	// Create an audio policy server
	iAudioPolicyProxy = new (ELeave) RMMFAudioPolicyProxy();	
	User::LeaveIfError(iAudioPolicyProxy->CreateServer(iPolicyServerHandle));
	}

CMMFAudioServer::~CMMFAudioServer()
	{
	if (iDelayAudioServerShutDown)
		{
		iDelayAudioServerShutDown->Cancel();
		delete iDelayAudioServerShutDown;
		}
	iDevSoundServList.ResetAndDestroy();
	iDevSoundServList.Close();
	if( iAudioPolicyProxy != NULL)
		{
		iAudioPolicyProxy->Close(); 
		delete iAudioPolicyProxy;
		}	
	iPolicyServerHandle.Close();
	}

CMmfIpcSession* CMMFAudioServer::NewSessionL(const TVersion& aVersion) const
	{
	TVersion v(KMMFAudioServerVersion,KMMFAudioServerMinorVersionNumber,KMMFAudioServerBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);

	TInt i=0;
	while(i<iDevSoundServList.Count())
		{
		CStartAndMonitorDevSoundThread* devSoundMonitorThread = iDevSoundServList[i];
		if(!devSoundMonitorThread->IsActive())
			{
			iDevSoundServList.Remove(i);
			delete devSoundMonitorThread;
			}
		else
			{
			i++;
			}
		}
	iDevSoundServList.Compress();

	RMMFDevSoundServerProxy devSoundSessionHandle;
	User::LeaveIfError(StartDevSoundServer(devSoundSessionHandle));
	CMMFAudioServerSession* audioServerSession = CMMFAudioServerSession::NewL(devSoundSessionHandle);
	return audioServerSession;
	}

void CMMFAudioServer::IncrementSessionId()
	{
	iAudioServerSessionId++;
	}

void CMMFAudioServer::DecrementSessionId()
	{
	iAudioServerSessionId--;
	}

void CMMFAudioServer::IncrementDevSoundCount()
	{
	iDevSoundCount++;
	if(iDevSoundCount)
		iDelayAudioServerShutDown->Cancel(); //in the case we started the shutdown due to no more DevSound
	}

void CMMFAudioServer::DecrementDevSoundCount()
	{
	iDevSoundCount--;
    if ( iDevSoundCount == 0 )
		{
		iDelayAudioServerShutDown->SetDelay(TTimeIntervalMicroSeconds32(KAudioServerShutDownDelay));
 		}
	}

void CMMFAudioServer::SendEventToClient(TInt aSessionToAlert, TInt /*aSessionToBeLaunched*/)
	{
	// For the session requested, send event to client
	iSessionIter.SetToFirst();
	CMMFAudioServerSession* session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
	while (session != NULL)
		{
		if (session->AudioServerSessionId() == aSessionToAlert)
			{
			break;  // Finished
			}
		session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
		}
	}

void CMMFAudioServer::LaunchRequest(TInt aSessionId)
	{
	iSessionIter.SetToFirst();
	CMMFAudioServerSession* session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
	while (session != NULL)
		{
		if (session->AudioServerSessionId() == aSessionId)
			{
			break;  // Finished
			}
		session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
		}
	}

TInt CMMFAudioServer::StartDevSoundServer(RMMFDevSoundServerProxy& aDevSoundSessionHandle) const
	{
	RMessage2 message(Message());
	CStartAndMonitorDevSoundThread* monitorDevSound = NULL;

	TRAPD(err, monitorDevSound = CStartAndMonitorDevSoundThread::NewL(const_cast<CMMFAudioServer*>(this)));
	if(err != KErrNone)
		{
		delete monitorDevSound;
		return err;
		}

	err = iDevSoundServList.Append(monitorDevSound);
	if(err != KErrNone)
		{
		delete monitorDevSound;
		return err;
		}
	
	return monitorDevSound->StartDevSoundServer(message, aDevSoundSessionHandle);

	}


CMMFAudioServer::CDelayAudioServerShutDown* CMMFAudioServer::CDelayAudioServerShutDown::NewL()
	{
	CDelayAudioServerShutDown* self = new(ELeave) CDelayAudioServerShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFAudioServer::CDelayAudioServerShutDown::CDelayAudioServerShutDown() : CActive(0)
	{
	}
        
void CMMFAudioServer::CDelayAudioServerShutDown::ConstructL()
	{
	User::LeaveIfError(iShutDownTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}

CMMFAudioServer::CDelayAudioServerShutDown::~CDelayAudioServerShutDown()
	{
	Cancel();
	iShutDownTimer.Close();
	}


void CMMFAudioServer::CDelayAudioServerShutDown::SetDelay(TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(_L("CDelayedShutDown"), 1));
	iShutDownTimer.After(iStatus, aDelay);
	SetActive();
	}

void CMMFAudioServer::CDelayAudioServerShutDown::RunL()
	{
	CActiveScheduler::Stop();
	}


void CMMFAudioServer::CDelayAudioServerShutDown::DoCancel()
	{
	iShutDownTimer.Cancel();
	}


CStartAndMonitorDevSoundThread* CStartAndMonitorDevSoundThread::NewL(CMMFAudioServer* aAudioServer)
	{
	CStartAndMonitorDevSoundThread* self = new(ELeave) CStartAndMonitorDevSoundThread(aAudioServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CStartAndMonitorDevSoundThread::ConstructL()
	{
	CActiveScheduler::Add(this);
	}
	
TInt CStartAndMonitorDevSoundThread::StartDevSoundServer(RMessage2& aMessage, RMMFDevSoundServerProxy& aDevSoundSessionHandle)
	{
	RThread clientThread;
	TInt err = aMessage.Client(clientThread);
	if (err != KErrNone)
		{
		return err;
		}
	RProcess clientProcess;
    err = clientThread.Process(clientProcess);
	if (err != KErrNone)
		{
		clientThread.Close();	
		return err;
		}
	TProcessId clientProcessId(clientProcess.Id());

	clientThread.Close();
	clientProcess.Close();

	RServer2 devSoundServerHandle;
	TDevSoundServerStart start(iAudioServer, clientProcessId, devSoundServerHandle);//Pass to DevSoundServer the clien't process ID 

	const TUidType serverUid(KNullUid,KNullUid,KUidDevSoundServer);
	
	TThreadFunction serverFunc=CMMFDevSoundServer::StartThread;
	
	err = iServer.Create(_L(""), serverFunc, KDevSoundServerStackSize,
#ifdef SYMBIAN_USE_SEPARATE_HEAPS
	                	KDevSoundServerInitHeapSize, KDevSoundServerMaxHeapSize,
#else
						&User::Heap(), //shared heap is now default
#endif
	                	&start, EOwnerProcess);
				

	if(err != KErrNone)
		return err;
	// Synchronise with the server
	TRequestStatus reqStatus;
	iServer.Rendezvous(reqStatus);
	
	if (reqStatus!=KRequestPending)
		{
		iServer.Kill(0);
		}
	else
		{
		// Start the test harness
		iServer.Resume();
		// Server will call the reciprocal static synchronise call	
		}

	User::WaitForRequest(reqStatus); // wait for start or death
	if(reqStatus.Int() != KErrNone)
		{
		iServer.Close();
		devSoundServerHandle.Close();
		return reqStatus.Int();
		}
	err = aDevSoundSessionHandle.Open(devSoundServerHandle);
	
	if(err != KErrNone)
		{
		iServer.Close();
		devSoundServerHandle.Close();
		return err;
		}
	aDevSoundSessionHandle.ShareProtected();
	iServer.Logon(iStatus);
	SetActive();

	iAudioServer->IncrementDevSoundCount();
	return KErrNone;
	}


CStartAndMonitorDevSoundThread::CStartAndMonitorDevSoundThread(CMMFAudioServer* aAudioServer): CActive(0)
	{
	iAudioServer = aAudioServer;
	}

CStartAndMonitorDevSoundThread::~CStartAndMonitorDevSoundThread()
	{
	Cancel();
	}
    
void CStartAndMonitorDevSoundThread::RunL()
	{
	iServer.Close();
	iAudioServer->DecrementDevSoundCount();
	}

void CStartAndMonitorDevSoundThread::DoCancel()
	{
	iServer.LogonCancel(iStatus);
	}

TInt RMMFDevSoundServerProxy::Open(RServer2& aDevSoundServerHandle)
	{
	TInt err = CreateSession(aDevSoundServerHandle, TVersion(KMMFDevSoundServerVersion,
														KMMFDevSoundServerMinorVersionNumber,
														KMMFDevSoundServerBuildVersionNumber), -1, EIpcSession_GlobalSharable);
	return err;
	}

