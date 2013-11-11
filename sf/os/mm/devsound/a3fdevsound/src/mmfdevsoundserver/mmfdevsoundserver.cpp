// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "mmfdevsoundserver.h"
#include "mmfdevsoundserverstart.h"
#include "mmfdevsoundsession.h"
#include "mmfdevsoundserverconsts.h"

const TInt KDevSoundShutDownDelay = 0; //no delay
_LIT(KMMFDevSoundServerName, "MMFDevSoundServer-");

// ======== LOCAL FUNCTIONS ========
void Panic(TInt aPanicCode)
	{
	_LIT(KDevSoundServerPanicCategory, "mmfdevsoundserver");
	User::Panic(KDevSoundServerPanicCategory, aPanicCode);
	}

/**
	CMMFDevSoundServer* CMMFDevSoundServer::NewL
	Two-phased constructor.
 */
EXPORT_C CMMFDevSoundServer* CMMFDevSoundServer::NewL(
	MGlobalProperties* aGlobalProperties,
	TProcessId& aClientPID)
	{
	CMMFDevSoundServer* self = new(ELeave) CMMFDevSoundServer(aGlobalProperties,
															aClientPID);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 	CMMFDevSoundServer::CMMFDevSoundServer
	C++ default constructor can NOT contain any code, that might leave.
 */
CMMFDevSoundServer::CMMFDevSoundServer(
	MGlobalProperties* aGlobalProperties,
	TProcessId& aClientPID) :
	CMmfIpcServer(EPriorityStandard, EGlobalSharableSessions),
	iClientPID(aClientPID),
	iActualClientPID(aClientPID)
	{
	iGlobalProperties = aGlobalProperties;
	}

/**
 	CMMFDevSoundServer::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CMMFDevSoundServer::ConstructL()
	{
	RProcess client;
	User::LeaveIfError(client.Open(iClientPID));


	client.Close();

	iDelayDevSoundShutDown = CDelayDevSoundShutDown::NewL();

	// Call base class to Start server
	StartL(KNullDesC);
	}

/**
	CMMFDevSoundServer::~CMMFDevSoundServer
	Destructor
*/
CMMFDevSoundServer::~CMMFDevSoundServer()
	{
	if (iDelayDevSoundShutDown)
		{
		iDelayDevSoundShutDown->Cancel();
		delete iDelayDevSoundShutDown;
		}
	}

/**
 	CMMFDevSoundServer::NewSessionL
 	Creates new session
 	(other items were commented in a header).
 */
CMmfIpcSession* CMMFDevSoundServer::NewSessionL(const TVersion& aVersion) const
	{
	CMmfIpcSession* devSoundSession = NULL;
	
	TRAPD(error, devSoundSession = CreateNewSessionL(aVersion));
	
	if (error!=KErrNone)
		{
		// session create failed, so flag for subsequent destruction of server
		CMMFDevSoundServer* mutableThis = const_cast<CMMFDevSoundServer*>(this);
		mutableThis->CheckForNoSessionsLeft();
		User::Leave(error);
		}
	return devSoundSession;
	}

/**
 	CMMFDevSoundServer::CreateNewSessionL
 	Guts of what would otherwise be in NewSessionL() itself
 */
CMmfIpcSession* CMMFDevSoundServer::CreateNewSessionL(const TVersion& aVersion) const
	{
	TVersion version(KMMFDevSoundServerVersion,
			KMMFDevSoundServerMinorVersionNumber,
			KMMFDevSoundServerBuildVersionNumber);

	if(!User::QueryVersionSupported(version, aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	CMMFDevSoundSession* devSoundSession =
					CMMFDevSoundSession::NewL(*iGlobalProperties);
	return devSoundSession;
	}

/**
 	CMMFDevSoundServer::IncrementSessionId
 	Increments session count
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::IncrementSessionId()
	{
	iDevSoundSessionId++;
	if (iDevSoundSessionId)
		{
		iDelayDevSoundShutDown->Cancel();
		}
	}

/**
 	CMMFDevSoundServer::DecrementSessionId
 	Decrements session count
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::DecrementSessionId()
	{
	iDevSoundSessionId--;
	CheckForNoSessionsLeft();
	}

/**
 	CMMFDevSoundServer::CheckForNoSessionsLeft
 	If no sessions are left, we kick off timer, which closes down server after timeout
 	really meaning the server closes asynchronously
 */
void CMMFDevSoundServer::CheckForNoSessionsLeft()
	{
	if (iDevSoundSessionId == 0)
		{
		iDelayDevSoundShutDown->SetDelay(
					TTimeIntervalMicroSeconds32(KDevSoundShutDownDelay));
		}
	}

/**
 	CMMFDevSoundServer::SendEventToClient
 	For the session requested, send event to client
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::SendEventToClient(TInt aSessionToAlert,
									TInt /*aSessionToBeLaunched*/)
	{
	iSessionIter.SetToFirst();
	CMMFDevSoundSession* session =
						static_cast<CMMFDevSoundSession*>(iSessionIter++);

	while (session != NULL)
		{
		if (session->DevSoundSessionId() == aSessionToAlert)
			{
			break;  // Finished
			}
		session = static_cast<CMMFDevSoundSession*>(iSessionIter++);
		}
	}

/**
 	CMMFDevSoundServer::LaunchRequest
 	Launches the DevSound servers waiting for Audio Policy.
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::LaunchRequest(TInt aSessionId)
	{
	iSessionIter.SetToFirst();
	CMMFDevSoundSession* session =
					static_cast<CMMFDevSoundSession*>(iSessionIter++);

	while (session != NULL)
		{
		if (session->DevSoundSessionId() == aSessionId)
			{
			break;  // Finished
			}
		session = static_cast<CMMFDevSoundSession*>(iSessionIter++);
		}
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

/**
 	CMMFDevSoundServer* CMMFDevSoundServer::NewL
 	Two-phased constructor.
 */
CMMFDevSoundServer::CDelayDevSoundShutDown*
CMMFDevSoundServer::CDelayDevSoundShutDown::NewL()
	{
	CDelayDevSoundShutDown* self = new(ELeave) CDelayDevSoundShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
	CMMFDevSoundServer::CDelayDevSoundShutDown::CDelayDevSoundShutDown
	C++ constructor
	(other items were commented in a header).
 */
CMMFDevSoundServer::CDelayDevSoundShutDown::CDelayDevSoundShutDown()
	: CActive(0)
	{
	}

/**
 	CMMFDevSoundServer::CDelayDevSoundShutDown::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CMMFDevSoundServer::CDelayDevSoundShutDown::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}

/**
 	CMMFDevSoundServer::CDelayDevSoundShutDown::~CDelayDevSoundShutDown
 	Destructor
 */
CMMFDevSoundServer::CDelayDevSoundShutDown::~CDelayDevSoundShutDown()
	{
	Cancel();
	iTimer.Close();
	}

/**
 	CMMFDevSoundServer::CDelayDevSoundShutDown::SetDelay
 	Request a timeout after aDelay
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::CDelayDevSoundShutDown::SetDelay(
		TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EMMFDevSoundServerIsActive));
	iTimer.After(iStatus, aDelay);
	SetActive();
	}

/**
 	CMMFDevSoundServer::CDelayDevSoundShutDown::RunL
 	Called by Active object framework after timer expires
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::CDelayDevSoundShutDown::RunL()
	{
	CActiveScheduler::Stop();
	}

/**
 	CMMFDevSoundServer::CDelayDevSoundShutDown::DoCancel
 	Called by Active object framework when user cancels active object.
 	(other items were commented in a header).
 */
void CMMFDevSoundServer::CDelayDevSoundShutDown::DoCancel()
	{
	iTimer.Cancel();
	}

/**
 	CMMFDevSoundServer::SetClientProcessIdL
 */
void CMMFDevSoundServer::SetClientProcessIdL(TThreadId aTid)
	{
	
	RThread clientThread;
	User::LeaveIfError(clientThread.Open(aTid));
	CleanupClosePushL(clientThread);
	
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));
	CleanupClosePushL(clientProcess);
	
	iActualClientPID = clientProcess.Id();
	CleanupStack::PopAndDestroy(2, &clientThread);	// clientProcess, clientThread		
	}
//GLOBAL FUNCTIONS

/**
 	This function raises a panic on the client of the DevSound Server
 	
 	@param  aError
			one of the several panic codes that may be raised by this dll on
			the client
 	
 	@panic  EMMFDevSoundPlayDataWithoutInitialize is raised when playdata is
			called without initialization
 	@panic  EMMFDevSoundRecordDataWithoutInitialize is raised when recorddata
			is called without initialization
 */
GLDEF_C void PanicClient(const RMmfIpcMessage& aMessage,
					TMMFDevSoundClientPanicCodes aPanicCode)
	{
	aMessage.Panic(KMMFDevSoundClientPanicCategory, aPanicCode);
	}

//  End of File
