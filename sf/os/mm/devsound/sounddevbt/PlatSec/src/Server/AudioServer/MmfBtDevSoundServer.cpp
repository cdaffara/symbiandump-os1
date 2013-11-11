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
#include "MmfBtDevSoundServer.h"
#include "MmfBtDevSoundServerStart.h"
#include "MmfBtDevSoundSession.h"

const TInt KDevSoundShutDownDelay = 0; //0 sec

EXPORT_C CMMFDevSoundServer* CMMFDevSoundServer::NewL(CMMFAudioServer* aAudioServer, TProcessId& aClientPID)
	{
	CMMFDevSoundServer* s = new(ELeave) CMMFDevSoundServer(aAudioServer, aClientPID);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFDevSoundServer::CMMFDevSoundServer(CMMFAudioServer* aAudioServer, TProcessId& aClientPID) :
	CMmfIpcServer(EPriorityStandard), iClientPID(aClientPID)
	{
	iAudioServer = aAudioServer;
	}

void CMMFDevSoundServer::ConstructL()
	{
	RProcess client;
	User::LeaveIfError(client.Open(iClientPID));
	iClientHasCaps = client.HasCapability(ECapabilityMultimediaDD, KSuppressPlatSecDiagnostic);
	client.Close();

	iDelayDevSoundShutDown = CDelayDevSoundShutDown::NewL();

	// Call base class to Start server
	TName name(RThread().Name());
	StartL(name);
	}

CMMFDevSoundServer::~CMMFDevSoundServer()
	{
	if (iDelayDevSoundShutDown)
		{
		iDelayDevSoundShutDown->Cancel();
		delete iDelayDevSoundShutDown;
		}
	}

CMmfIpcSession* CMMFDevSoundServer::NewSessionL(const TVersion& aVersion) const
	{
	RMessage2 message(Message());
	RThread clientThread;
	message.ClientL(clientThread);
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));

	TProcessId clientProcessID(clientProcess.Id());
	clientThread.Close();
	clientProcess.Close();

	if(iClientPID != clientProcessID)
		{
		User::Leave(KErrAccessDenied);
		}


	TVersion v(KMMFDevSoundServerVersion,KMMFDevSoundServerMinorVersionNumber,KMMFDevSoundServerBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);
	
	CMMFDevSoundSession* devSoundSession = CMMFDevSoundSession::NewL(iAudioServer->PolicyServerHandle());
	return devSoundSession;
	}

void CMMFDevSoundServer::IncrementSessionId()
	{
	iDevSoundSessionId++;
	if(iDevSoundSessionId)
		iDelayDevSoundShutDown->Cancel();

	}

void CMMFDevSoundServer::DecrementSessionId()
	{
	iDevSoundSessionId--;
    if ( iDevSoundSessionId == 0 )
		{
		iDelayDevSoundShutDown->SetDelay(TTimeIntervalMicroSeconds32(KDevSoundShutDownDelay));
 		}
	}

void CMMFDevSoundServer::SendEventToClient(TInt aSessionToAlert, TInt /*aSessionToBeLaunched*/)
	{
	// For the session requested, send event to client
	iSessionIter.SetToFirst();
	CMMFDevSoundSession* session = static_cast<CMMFDevSoundSession*>(iSessionIter++);
	while (session != NULL)
		{
		if (session->DevSoundSessionId() == aSessionToAlert)
			{
			break;  // Finished
			}
		session = static_cast<CMMFDevSoundSession*>(iSessionIter++);
		}
	}

void CMMFDevSoundServer::LaunchRequest(TInt aSessionId)
	{
	iSessionIter.SetToFirst();
	CMMFDevSoundSession* session = static_cast<CMMFDevSoundSession*>(iSessionIter++);
	while (session != NULL)
		{
		if (session->DevSoundSessionId() == aSessionId)
			{
			break;  // Finished
			}
		session = static_cast<CMMFDevSoundSession*>(iSessionIter++);
		}
	}

TBool CMMFDevSoundServer::CheckClientCapabilities()
	{
	return iClientHasCaps;
	}

void CMMFDevSoundServer::SetClientCapabilitiesL(TThreadId aTid)
	{
	RThread clientThread;
	User::LeaveIfError(clientThread.Open(aTid));
	CleanupClosePushL(clientThread);
	
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));
	CleanupClosePushL(clientProcess);
	
	iClientHasCaps = clientProcess.HasCapability(ECapabilityMultimediaDD, KSuppressPlatSecDiagnostic);
	
	CleanupStack::PopAndDestroy(2, &clientThread);	// clientProcess, clientThread
	}

CMMFDevSoundServer::CDelayDevSoundShutDown* CMMFDevSoundServer::CDelayDevSoundShutDown::NewL()
	{
	CDelayDevSoundShutDown* self = new(ELeave) CDelayDevSoundShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFDevSoundServer::CDelayDevSoundShutDown::CDelayDevSoundShutDown() : CActive(0)
	{
	}
        
void CMMFDevSoundServer::CDelayDevSoundShutDown::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}

CMMFDevSoundServer::CDelayDevSoundShutDown::~CDelayDevSoundShutDown()
	{
	Cancel();
	iTimer.Close();
	}


void CMMFDevSoundServer::CDelayDevSoundShutDown::SetDelay(TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(!IsActive(),
	User::Panic(_L("CDelayedShutDown"), 1));
	iTimer.After(iStatus, aDelay);
	SetActive();
	}

void CMMFDevSoundServer::CDelayDevSoundShutDown::RunL()
	{
	CActiveScheduler::Stop();
	}

void CMMFDevSoundServer::CDelayDevSoundShutDown::DoCancel()
	{
	iTimer.Cancel();
	}
  

        

