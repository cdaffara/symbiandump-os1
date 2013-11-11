//mmfaudioserver.cpp

// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Portions Copyright Nokia-Symbian * Nokia Core OS *
// INCLUDE FILES
//

#include <e32math.h>
#include <mmf/common/mmfbase.hrh>

#include "mmfaudioserver.h"
#include "mmfaudioserverdecs.h"
#include "mmfdevsoundsession.h"
#include "mmfaudioserversession.h"
#include "mmfdevsoundserver.h"
#include "mmfdevsoundserverstart.h"
#include "mmfaudioserverfactory.h"
#include "devsoundsvrthreadpriorityconsts.h"

#ifdef _DEBUG
#include "e32debug.h"
#define SYMBIAN_DEBPRN0(str)    RDebug::Print(str, this)
#else
#define SYMBIAN_DEBPRN0(str)
#endif //_DEBUG


// Time delay that the audio server wait before shutdown itself when is detected that the last DevSoundSession is closed
const TInt KAudioServerShutDownDelay = 50000000; //50 sec

// ======== LOCAL FUNCTIONS ========

void Panic(TInt aPanicCode)
	{
	_LIT(KAudioServerPanicCategory, "mmfaudioserver");
	User::Panic(KAudioServerPanicCategory, aPanicCode);
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMMFAudioServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMMFAudioServer* CMMFAudioServer::NewL()
	{
	CMMFAudioServer* self = new(ELeave) CMMFAudioServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CMMFAudioServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMMFAudioServer::CMMFAudioServer()
	: CMmfIpcServer(EPriorityStandard)
	{
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::ConstructL()
	{
	SYMBIAN_DEBPRN0(_L("CMMFAudioServer[0x%x]::ConstructL - enter"));

	SetPinClientDescriptors(ETrue);
	// Call base class to Start server
	StartL(KAudioServerName);

	iFourCCConvertor = CFourCCConvertor::NewL();
	iDelayAudioServerShutDown = CDelayAudioServerShutDown::NewL();

	iFactory = CMMFAudioServerFactory::NewL();
	iFactory->StartL(*this);	

	SYMBIAN_DEBPRN0(_L("CMMFAudioServer[0x%x]::ConstructL - exit"));
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::~CMMFAudioServer
// Destructor
// -----------------------------------------------------------------------------
//
CMMFAudioServer::~CMMFAudioServer()
	{
	if (iDelayAudioServerShutDown)
		{
		iDelayAudioServerShutDown->Cancel();
		delete iDelayAudioServerShutDown;
		iDelayAudioServerShutDown = NULL;
		}

	for(TInt i=(iDevSoundServList.Count()-1); i >=0 ; i--)
		{
		CStartAndMonitorDevSoundThread* devSoundMonitorThread = iDevSoundServList[i];
		iDevSoundServList.Remove(i);
		delete devSoundMonitorThread;
		}
	iDevSoundServList.Close();
	delete iFourCCConvertor;
	if (iFactory)
		{
		iFactory->Stop(*this);
		delete iFactory;
		}
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::NewSessionL
// Creates a new session to handle requests by the client and returns it to the
// client server framework.
// -----------------------------------------------------------------------------
//
CMmfIpcSession* CMMFAudioServer::NewSessionL(const TVersion& aVersion) const
	{
	SYMBIAN_DEBPRN0(_L("CMMFAudioServer[0x%x]::NewSessionL - enter"));
	
	TVersion v(KMMFAudioServerVersion,
			KMMFAudioServerMinorVersionNumber,
			KMMFAudioServerBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		{
		User::Leave(KErrNotSupported);
		}
	
	for(TInt i=(iDevSoundServList.Count()-1); i >=0 ; i--)
		{
		CStartAndMonitorDevSoundThread* devSoundMonitorThread = iDevSoundServList[i];

		if(!devSoundMonitorThread->IsActive())
			{
			iDevSoundServList.Remove(i);
			delete devSoundMonitorThread;
			}
		iDevSoundServList.Compress();
		}

	RMMFDevSoundServerProxy devSoundSessionHandle;
	User::LeaveIfError(StartDevSoundServer(devSoundSessionHandle));
	CMMFAudioServerSession* audioServerSession = CMMFAudioServerSession::NewL(devSoundSessionHandle);
	SYMBIAN_DEBPRN0(_L("CMMFAudioServer[0x%x]::NewSessionL - session created"));
	return audioServerSession;
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::IncrementSessionId
// Increment the session Id.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::IncrementSessionId()
	{
	iAudioServerSessionId++;
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::DecrementSessionId
// Decrement the session Id.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::DecrementSessionId()
	{
	iAudioServerSessionId--;
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::IncrementDevSoundCount
// Increment the DevSound server counter. If there is atleast one DevSound
// server, it will cancel its shutdown timer.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::IncrementDevSoundCount()
	{
	iDevSoundCount++;
	//in the case we started the shutdown due to no more DevSound
	if(iDevSoundCount)
		{
		ASSERT(iDelayAudioServerShutDown);
		if (iDelayAudioServerShutDown)
			{
			iDelayAudioServerShutDown->Cancel();
			}
		}
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::DecrementDevSoundCount
// Decrement the DevSound server counter. Once the number of DevSound server's
// instances becomes zero, Audio Server will start its shutdown routine.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::DecrementDevSoundCount()
	{
	iDevSoundCount--;
	if (iDevSoundCount == 0)
		{
		ASSERT(iDelayAudioServerShutDown);
		if (iDelayAudioServerShutDown)
			{
			iDelayAudioServerShutDown->SetDelay(TTimeIntervalMicroSeconds32(KAudioServerShutDownDelay));
			}
		}
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::SendEventToClient
// Sends Event to DevSound client.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::SendEventToClient(TInt aSessionToAlert, TInt /*aSessionToBeLaunched*/)
	{
	// For the session requested, send event to client
	iSessionIter.SetToFirst();
	CMMFAudioServerSession* session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
	while (session)
		{
		if (session->AudioServerSessionId() == aSessionToAlert)
			{
			break;  // Finished
			}

		session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
		}
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::LaunchRequest
// Launches the DevSound servers waiting for Audio Policy.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::LaunchRequest(TInt aSessionId)
	{
	iSessionIter.SetToFirst();
	CMMFAudioServerSession* session = static_cast<CMMFAudioServerSession*>(iSessionIter++);

	while (session)
		{
		if (session->AudioServerSessionId() == aSessionId)
			{
			break;  // Finished
			}
		session = static_cast<CMMFAudioServerSession*>(iSessionIter++);
		}
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::StartDevSoundServer
// Starts a new DevSound server instance.
// -----------------------------------------------------------------------------
//
TInt CMMFAudioServer::StartDevSoundServer(RMMFDevSoundServerProxy& aDevSoundSessionHandle) const
	{
	SYMBIAN_DEBPRN0(_L("CMMFAudioServer[0x%x]::StartDevSoundServer - enter"));

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
	
	SYMBIAN_DEBPRN0(_L("CMMFAudioServer[0x%x]::StartDevSoundServer - exit"));
	return monitorDevSound->StartDevSoundServer(message, aDevSoundSessionHandle);

	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CDelayAudioServerShutDown::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMMFAudioServer::CDelayAudioServerShutDown*
CMMFAudioServer::CDelayAudioServerShutDown::NewL()
	{
	CDelayAudioServerShutDown* self = new(ELeave) CDelayAudioServerShutDown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CMMFAudioServer::CDelayAudioServerShutDown::CDelayAudioServerShutDown()
	: CActive(0)
	{
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CDelayAudioServerShutDown::ConstructL
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::CDelayAudioServerShutDown::ConstructL()
	{
	User::LeaveIfError(iShutDownTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CDelayAudioServerShutDown::~CDelayAudioServerShutDown
// -----------------------------------------------------------------------------
//
CMMFAudioServer::CDelayAudioServerShutDown::~CDelayAudioServerShutDown()
	{
	Cancel();
	iShutDownTimer.Close();
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CDelayAudioServerShutDown::SetDelay
// Request a timeout after aDelay
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::CDelayAudioServerShutDown::SetDelay(
	TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EMMFAudioServerIsActive));
	iShutDownTimer.After(iStatus, aDelay);
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CDelayAudioServerShutDown::RunL
// Called by Active object framework when timer times out.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::CDelayAudioServerShutDown::RunL()
	{
	CActiveScheduler::Stop();
	}

// -----------------------------------------------------------------------------
// CMMFAudioServer::CDelayAudioServerShutDown::DoCancel
// Called by the Active object framework when user cancels active object.
// -----------------------------------------------------------------------------
//
void CMMFAudioServer::CDelayAudioServerShutDown::DoCancel()
	{
	iShutDownTimer.Cancel();
	}

// -----------------------------------------------------------------------------
// CStartAndMonitorDevSoundThread::NewL
// -----------------------------------------------------------------------------
//
CStartAndMonitorDevSoundThread* CStartAndMonitorDevSoundThread::NewL(
	CMMFAudioServer* aAudioServer)
	{
	CStartAndMonitorDevSoundThread* self = new(ELeave) CStartAndMonitorDevSoundThread(aAudioServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CStartAndMonitorDevSoundThread::ConstructL
// -----------------------------------------------------------------------------
//
void CStartAndMonitorDevSoundThread::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CStartAndMonitorDevSoundThread::StartDevSoundServer
// Starts a new DevSound server.
// -----------------------------------------------------------------------------
//
TInt CStartAndMonitorDevSoundThread::StartDevSoundServer(
	RMessage2& aMessage,
	RMMFDevSoundServerProxy& aDevSoundSessionHandle)
	{
	SYMBIAN_DEBPRN0(_L("CStartAndMonitorDevSoundThread[0x%x]::StartDevSoundServer - enter"));
	
	RThread clientThread;
	TInt err(KErrNone);
	
	// Open handle to client thread
	err = aMessage.Client(clientThread);
	if (err != KErrNone)
		{
		clientThread.Close();
		return err;
		}
	// Open a process-relative handle to the process which owns clientThread.
	RProcess clientProcess;
	err = clientThread.Process(clientProcess);
	
	if (err != KErrNone)
		{
		// Close client thread handle and return error
		clientThread.Close();
		return err;
		}
	
	// Get the client process id
	TProcessId clientProcessId(clientProcess.Id());
	
	// Close client thread and client process handles
	clientThread.Close();
	clientProcess.Close();
	
	RServer2 devSoundServerHandle;
	devSoundServerHandle.SetReturnedHandle(0);
	
	//Pass to DevSoundServer the clien't process ID
	TDevSoundServerStart start(iAudioServer,
								clientProcessId,
								devSoundServerHandle);
	
	TThreadFunction serverFunc=CMMFDevSoundServer::StartThread;
	
	// To deal with the unique thread (+semaphore!) naming in EPOC,
	// and that we may be trying to restart a server that has just
	// exited we attempt to create a unique thread name for the server.
#ifdef SYMBIAN_USE_SEPARATE_HEAPS
	err = iServer.Create(KNullDesC,serverFunc,KDevSoundServerStackSize*2,
						KDevSoundServerInitHeapSize,
						KDevSoundServerMaxHeapSize,
						&start,
						EOwnerProcess);
#else
	err = iServer.Create(KNullDesC,
						serverFunc,
						(KDevSoundServerStackSize*2),
						NULL,
						&start,
						EOwnerProcess);
#endif // SYMBIAN_USE_SEPARATE_HEAPS
	if(err != KErrNone)
		{
		return err;
		}
	iServer.SetPriority(KDevsoundSvrPriority);

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

	// note devSoundServerHandle really "owned" by client thread - just copied here

	User::WaitForRequest(reqStatus); // wait for start or death
	if(reqStatus.Int() != KErrNone)
		{
		// close our handle to the thread. If we reach here, the thread should have
		// closed anyway as continue Rendezvous is always KErrNone() 
		iServer.Close();
		return reqStatus.Int();
		}

	err = aDevSoundSessionHandle.Open(devSoundServerHandle);

	if(err != KErrNone)
		{
		// close our handle to the thread. If we reach here, this means that NewSessionL() failed
		// on server, and that action itself will asynchronously stop the thread
		iServer.Close();
		return err;
		}

	aDevSoundSessionHandle.ShareProtected();
	iServer.Logon(iStatus);
	SetActive();

	iAudioServer->IncrementDevSoundCount();

	SYMBIAN_DEBPRN0(_L("CStartAndMonitorDevSoundThread[0x%x]::StartDevSoundServer - exit"));
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CStartAndMonitorDevSoundThread::CStartAndMonitorDevSoundThread
// Constructor
// -----------------------------------------------------------------------------
//
CStartAndMonitorDevSoundThread::CStartAndMonitorDevSoundThread(
	CMMFAudioServer* aAudioServer)
	: CActive(EPriorityStandard),iAudioServer(aAudioServer)
	{

	}

// Desctructor
CStartAndMonitorDevSoundThread::~CStartAndMonitorDevSoundThread()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
// CStartAndMonitorDevSoundThread::RunL
// Called by Active Object framework when DevSound server is destroyed.
// -----------------------------------------------------------------------------
//
void CStartAndMonitorDevSoundThread::RunL()
	{
	iServer.Close();
	iAudioServer->DecrementDevSoundCount();
	}

// -----------------------------------------------------------------------------
// CStartAndMonitorDevSoundThread::DoCancel
// Called by Active Object framework when client cancels active object.
// -----------------------------------------------------------------------------
//
void CStartAndMonitorDevSoundThread::DoCancel()
	{
	iServer.LogonCancel(iStatus);
	}


// -----------------------------------------------------------------------------
// RMMFDevSoundServerProxy::Open
// Creates new audio server session for the devsound server.
// -----------------------------------------------------------------------------
//
TInt RMMFDevSoundServerProxy::Open(RServer2& aDevSoundServerHandle)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundServerProxy[0x%x]::Open - enter"));
	TInt err = CreateSession(aDevSoundServerHandle, TVersion(KMMFDevSoundServerVersion,
							KMMFDevSoundServerMinorVersionNumber,
							KMMFDevSoundServerBuildVersionNumber),
							-1, EIpcSession_GlobalSharable);

	SYMBIAN_DEBPRN0(_L("RMMFDevSoundServerProxy[0x%x]::Open - exit"));
	return err;
	}

//  End of File
