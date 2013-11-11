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

#include "mmrcserver.h"
#include "mmrcserversession.h"
#include "mmrcservercontroller.h"
#include "mmrcserverstart.h"
#include <a3f/a3ffourcclookup.h>
#include <a3f/a3f_trace_utils.h>


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 * Constructor
 */
CMMRCServer::CMMRCServer()
:  CServer2(EPriorityStandard)
	{ 
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCServer::CMMRCServer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	Cancel();
	
	DP_OUT();
	}

/**
 * Destructor.
 */
CMMRCServer::~CMMRCServer()
	{
	DP_CONTEXT(----> CMMRCServer::~CMMRCServer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	if(iMMRCServerController)
		{
		delete iMMRCServerController;
		}
	delete iFourCCConvertor;
	DP_OUT();
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 * Constructs, and returns a pointer to, a new CMMRCServer object.
 * Leaves on failure.
 * @return CMMRCServer* A pointer to newly created utlitly object.
 */		
CMMRCServer* CMMRCServer::NewL()
	{
	DP_STATIC_CONTEXT(----> CMMRCServer::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServer* self = NewLC();
	CleanupStack::Pop(self);
	
	DP0_RET(self, "0x%x");
	} 

/**
 * Constructs, leaves object on the cleanup stack, and returns a pointer
 * to, a new CMMRCServer object.
 * Leaves on failure.
 * @return CMMRCServer* A pointer to newly created utlitly object.
 */		
CMMRCServer* CMMRCServer::NewLC()
	{
	DP_STATIC_CONTEXT(----> CMMRCServer::Open *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServer* self = new(ELeave) CMMRCServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	DP0_RET(self, "self = 0x%x");
	}

/**
 	CMMRCServer::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CMMRCServer::ConstructL()
	{
	DP_CONTEXT(----> CMMRCServer::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerController = CMMRCServerController::NewL();
	
	iFourCCConvertor = CFourCCConvertor::NewL();
	DP_OUT();
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 * From CServer2. Creates a server-side client session object.
 * Creates a new session.  Only one session may be created with the ControllerProxyServer.
 * This function may leave with one of the system-wide error codes.
 * @param const TVersion& aVersion	The version number of the session.
 * @param const RMessage2& aMessage
 * @return A pointer to the new session.
 */
CSession2* CMMRCServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	DP_CONTEXT(----> CMMRCServer::NewSessionL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TVersion version(KMMRCServerVersion,KMMRCServerMinorVersionNumber,KMMRCServerBuildVersionNumber);
	if(!User::QueryVersionSupported(version, aVersion))
		{
		User::Leave(KErrNotSupported);	   
		}
	CMMRCServerSession* mmrcSession = CMMRCServerSession::NewL(*iMMRCServerController, *iFourCCConvertor);
	
	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/**
A utility function to panic the server.
*/
void CMMRCServer::PanicServer(TInt aPanic)
	{
	_LIT(KMMRCServerPanic,"MMRC Server panic");
	User::Panic(KMMRCServerPanic, aPanic);
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
TInt CMMRCServer::ThreadFunctionL(TAny* /**aStarted*/)
	{
	DP_STATIC_CONTEXT(----> CMMRCServer::ThreadFunctionL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	// create an active scheduler and server
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	
	//Install the active scheduler
	CActiveScheduler::Install(sched);
	
	CMMRCServer *server = CMMRCServer::NewLC();
		

	// Start the server
	TInt err = server->Start(KMMRCServerName);
	if (err != KErrNone)
		{
		CMMRCServer::PanicServer(KErrNoMemory);
		}

	// Let everyone know that we are ready to
	// deal with requests.
	RThread::Rendezvous(KErrNone);

	// And start fielding requests from client(s).
	CActiveScheduler::Start();

	// Tidy up...
	CleanupStack::PopAndDestroy(2, sched);

	DP0_RET(KErrNone, "error = %d");
	}

/**
Create the thread that will act as the server.
This function is exported from the DLL and called by the client.

Note that a server can also be implemented as a separate
executable (i.e. as a separate process).
*/
TInt CMMRCServer::ThreadFunction(TAny* aStarted)
	{
	DP_STATIC_CONTEXT(----> CMMRCServer::ThreadFunction *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup == NULL)
		{
		CMMRCServer::PanicServer(KErrNoMemory);
		}
	
	TRAPD( err, ThreadFunctionL(aStarted) );

	delete cleanup; 

	DP0_RET(err, "error = %d");
	}

/**
Create the thread that will act as the server.
This function is exported from the DLL and called by the client.

Note that a server can also be implemented as a separate
executable (i.e. as a separate process).
*/
EXPORT_C TInt StartMMRCServer(RThread& aServerThread)
	{
	TInt res = KErrNone;
	
	// Create the server, if one with this name does not already exist.
	TFindServer findCountServer(KMMRCServerName);
	TFullName   name;
	
	// Need to check that the server exists.
	if ( findCountServer.Next(name) != KErrNone )
		{
		  // Create the thread for the server.
		res = aServerThread.Create(KMMRCServerName,
			CMMRCServer::ThreadFunction,
			KMMRCServerStackSize,
			KMMRCServerInitHeapSize,
			KMMRCServerMaxHeapSize,
			NULL
			);
			
		// The thread has been created OK so get it started - however
		// we need to make sure that it has started before we continue.
		if (res==KErrNone)
			{
			TRequestStatus rendezvousStatus;
			
			aServerThread.SetPriority(EPriorityNormal);
			aServerThread.Rendezvous(rendezvousStatus);
			aServerThread.Resume();
			User::WaitForRequest(rendezvousStatus);
			}
			
		  // The thread has not been created - clearly there's been a problem.
		else
			{
			aServerThread.Close();
			}
		}	
	return res;
	}

//EOF
