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
#include "MmfBtAudioServer.h"
#include "MmfBtAudioServerStart.h"
#include "MmfBtDevSoundSession.h"
#include "MmfBtAudioServerSession.h"
#include "MmfBtDevSoundServer.h"
#include "MmfBtDevSoundServerStart.h"
#include "../../../../inc/common/mmfBtBase.hrh"


static const TUid KUidDevSoundServer = {KUidMmfBtDevSoundServerDllUnicodeDefine};

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
	iDelayAudioServerShutDown = CDelayAudioServerShutDown::NewL();
	// Call base class to Start server
	StartL(KAudioServerName);
	}

CMMFAudioServer::~CMMFAudioServer()
	{
	if (iDelayAudioServerShutDown)
		{
		iDelayAudioServerShutDown->Cancel();
		delete iDelayAudioServerShutDown;
		}
	iDevSoundServList.Close();
	iPolicyServerHandle.Close();
	}

CMmfIpcSession* CMMFAudioServer::NewSessionL(const TVersion& aVersion) const
	{
	TVersion v(KMMFAudioServerVersion,KMMFAudioServerMinorVersionNumber,KMMFAudioServerBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);

	for(TInt i=0; i<iDevSoundServList.Count(); i++)
		{
		CStartAndMonitorDevSoundThread* devSoundMonitorThread = iDevSoundServList[i];
		if(!devSoundMonitorThread->IsActive())
			{
			iDevSoundServList.Remove(i);
			delete devSoundMonitorThread;
			}
		iDevSoundServList.Compress();
		}

	TName devSoundServerName;
	User::LeaveIfError(StartDevSoundServerL(devSoundServerName));
	CMMFAudioServerSession* audioServerSession = CMMFAudioServerSession::NewL(devSoundServerName);
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

TInt CMMFAudioServer::StartDevSoundServerL(TName& aDevSoundName) const
	{
	RMessage2 message(Message());
	CStartAndMonitorDevSoundThread* monitorDevSound = NULL;

	TRAPD(err, monitorDevSound = CStartAndMonitorDevSoundThread::NewL(const_cast<CMMFAudioServer*>(this)));
	if(err != KErrNone)
		{
		delete monitorDevSound;
		return err;
		}

	iDevSoundServList.Append(monitorDevSound);
	
	CMMFAudioServer* mutableThis = const_cast<CMMFAudioServer*>(this);
	mutableThis->iGlobalNum+=1;

	return monitorDevSound->StartDevSoundServerL(message, aDevSoundName, iGlobalNum);

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
	
TInt CStartAndMonitorDevSoundThread::StartDevSoundServerL(RMessage2& aMessage, TName& aDevSoundName, TInt aUniqueNum)
	{
	RThread clientThread;
	aMessage.ClientL(clientThread);
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));
	TProcessId clientProcessId(clientProcess.Id());

	clientThread.Close();
	clientProcess.Close();

	TDevSoundServerStart start(iAudioServer, clientProcessId);//Pass to DevSoundServer the clien't process ID 

	const TUidType serverUid(KNullUid,KNullUid,KUidDevSoundServer);
	
	TThreadFunction serverFunc=CMMFDevSoundServer::StartThread;
	//
	// To deal with the unique thread (+semaphore!) naming in EPOC, and that we may
	// be trying to restart a server that has just exited we attempt to create a
	// unique thread name for the server.
	//

	TInt err = KErrNone;
	TName name(KDevSoundServerName);
	name.AppendNum(TInt(this),EHex);
	name.Append(TChar('.'));
	name.AppendNum(aUniqueNum,EHex);
	err = iServer.Create(name, serverFunc, KDevSoundServerStackSize,
	                	KDevSoundServerInitHeapSize, KDevSoundServerMaxHeapSize,
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
		return reqStatus.Int();
		}
		
	aDevSoundName = iServer.Name();

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



