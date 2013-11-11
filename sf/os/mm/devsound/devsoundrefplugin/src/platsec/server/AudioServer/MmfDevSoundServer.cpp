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
#include "MmfDevSoundServer.h"
#include "MmfDevSoundServerStart.h"
#include "MmfDevSoundSession.h"

const TInt KDevSoundShutDownDelay = 0; //0 sec
_LIT(KMMFDevSoundServerName, "MMFDevSoundServer-");



EXPORT_C CMMFDevSoundServer* CMMFDevSoundServer::NewL(CMMFAudioServer* aAudioServer, TProcessId& aClientPID)
	{
	CMMFDevSoundServer* s = new(ELeave) CMMFDevSoundServer(aAudioServer, aClientPID);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFDevSoundServer::CMMFDevSoundServer(CMMFAudioServer* aAudioServer, TProcessId& aClientPID) :
	CMmfIpcServer(EPriorityStandard, EGlobalSharableSessions), iClientPID(aClientPID)
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
	StartL(KNullDesC);
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
	
void CMMFDevSoundServer::RenamePrioritizeServer()
	{
	//Rename devsound server name
	RThread devsoundServerThread;
	TThreadId threadId;
	TName name;
	name.Append(KMMFDevSoundServerName); 
	threadId = devsoundServerThread.Id();
	name.AppendNum(threadId.Id(),EHex);
	//We are ignoring the error code returned from User::RenameThread
	//as it is not important here, may be for profiling
	User::RenameThread(name);
	
	//Set the devsound priority
	TThreadPriority devsoundServerPriority = static_cast<TThreadPriority>(KDevsoundServerPriority);
	devsoundServerThread.SetPriority(devsoundServerPriority);
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
  
/**
@internalTechnology

This function raises a panic on the client of the devsoundserver

@param	aError
		one of the several panic codes that may be raised by this dll on the client

@panic	EMMFDevSoundPlayDataWithoutInitialize is raised when playdata is called without initialization
@panic	EMMFDevSoundRecordDataWithoutInitialize is raised when recorddata is called without initialization
*/
GLDEF_C void PanicClient(const RMmfIpcMessage& aMessage, TMMFDevSoundClientPanicCodes aPanicCode)
	{
	aMessage.Panic(KMMFDevSoundClientPanicCategory, aPanicCode);
	}
