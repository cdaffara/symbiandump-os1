// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Remote Control server implementation.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <remcon/remcontargetselectorplugin.h>
#include <remcon/remcontargetselectorplugininterface.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/remconbearerbulkinterface.h>
#include "server.h"
#include "targetclientprocess.h"
#include "controllersession.h"
#include "targetsession.h"
#include "serversecuritypolicy.h"
#include "utils.h"
#include "bearermanager.h"
#include "messagequeue.h"
#include "convertermanager.h"
#include "remconmessage.h"
#include "connections.h"
#include "connectionhistory.h"
#include "messagerecipients.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

PANICCATEGORY("server");

#ifdef __FLOG_ACTIVE
#define LOGCONTROLLERSESSIONS				LogControllerSessions()
#define LOGTARGETSESSIONS					LogTargetSessions()
#define LOGREMOTES							LogRemotes()
#define LOGCONNECTIONHISTORYANDINTEREST		LogConnectionHistoryAndInterest()
#define LOGOUTGOINGCMDPENDINGTSP			LogOutgoingCmdPendingTsp()
#define LOGOUTGOINGNOTIFYCMDPENDINGTSP		LogOutgoingNotifyCmdPendingTsp()
#define LOGOUTGOINGRSPPENDINGTSP			LogOutgoingRspPendingTsp()
#define LOGOUTGOINGPENDINGSEND				LogOutgoingPendingSend()
#define LOGOUTGOINGSENT						LogOutgoingSent()
#define LOGINCOMINGCMDPENDINGADDRESS			LogIncomingCmdPendingAddress()
#define LOGINCOMINGNOTIFYCMDPENDINGADDRESS		LogIncomingNotifyCmdPendingAddress()
#define LOGINCOMINGNOTIFYCMDPENDINGREADDRESS	LogIncomingNotifyCmdPendingReAddress()
#define LOGINCOMINGPENDINGDELIVERY			LogIncomingPendingDelivery()
#define LOGINCOMINGDELIVERED				LogIncomingDelivered()
#else
#define LOGCONTROLLERSESSIONS
#define LOGTARGETSESSIONS
#define LOGREMOTES
#define LOGCONNECTIONHISTORYANDINTEREST
#define LOGOUTGOINGCMDPENDINGTSP
#define LOGOUTGOINGNOTIFYCMDPENDINGTSP
#define LOGOUTGOINGRSPPENDINGTSP
#define LOGOUTGOINGPENDINGSEND
#define LOGOUTGOINGSENT
#define LOGINCOMINGCMDPENDINGADDRESS
#define LOGINCOMINGNOTIFYCMDPENDINGADDRESS
#define LOGINCOMINGNOTIFYCMDPENDINGREADDRESS
#define LOGINCOMINGPENDINGDELIVERY
#define LOGINCOMINGDELIVERED
#endif // __FLOG_ACTIVE

TInt BulkMain(TAny* aParam);
TBool ControllerSessionCompare(const TUint* aSessionId, const CRemConControllerSession& aSession)
	{
	return *aSessionId == aSession.Id();
	}

TBool TargetClientCompareUsingSessionId(const TUint* aClientId, const CRemConTargetClientProcess& aClient)
	{
	return *aClientId == aClient.Id();
	}

TBool TargetClientCompareUsingProcessId(const TProcessId* aProcessId, const CRemConTargetClientProcess& aClient)
	{
	return *aProcessId == aClient.ClientInfo().ProcessId();
	}

CRemConServer* CRemConServer::NewLC()
	{
	LOG_STATIC_FUNC;
	CRemConServer* self = new(ELeave) CRemConServer();
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance 
	// of the same server running, so do it before ConstructL.
	self->StartL(KRemConServerName);
	self->ConstructL();
	return self;
	}

CRemConServer::~CRemConServer()
	{
	LOG_FUNC;

	delete iBearerManager;
	delete iShutdownTimer;
	
	// There should be no watcher as there should be no bulk thread running
	ASSERT_DEBUG(!iBulkThreadWatcher);

	iControllerSessions.Close();

	iTargetClientsLock.Wait();
	iTargetClients.Close();
	iTargetClientsLock.Close();

	// Destroy TSP before iIncomingPendingAddress in case the TSP is 
	// addressing a message on it at the time.
	// The TSP should not be handling outgoing commands or responses as all the sessions 
	// have gone, and they clean up their outgoing messages when they close.
	ASSERT_DEBUG(!iTspHandlingOutgoingCommand);
	ASSERT_DEBUG(!iTspHandlingOutgoingResponse);
	ASSERT_DEBUG(!iTspHandlingOutgoingNotifyCommand);	
	// We can't assert anything about iTspAddressingIncomingCommand- it isn't 
	// interesting.
	delete iTsp;

	delete iOutgoingCmdPendingTsp;
	delete iOutgoingNotifyCmdPendingTsp;
	delete iOutgoingRspPendingTsp;
	delete iOutgoingRspPendingSend;
	delete iOutgoingPendingSend;
	delete iOutgoingSent;
	delete iIncomingCmdPendingAddress;
	delete iIncomingNotifyCmdPendingAddress;
	delete iIncomingNotifyCmdPendingReAddress;
	delete iIncomingPendingDelivery;
	delete iIncomingDelivered;
	
	delete iMessageRecipientsList;

	iTspConnections.Reset();
	iTspIncomingCmdClients.Reset();
	iTspIncomingNotifyCmdClients.Reset();
	
	delete iConverterManager;

	// Clean up the connection information (must be done after the bearer 
	// manager is destroyed).
	LOGREMOTES;
	LOGCONNECTIONHISTORYANDINTEREST;
	delete iConnectionHistory;

	iSession2ConnHistory.Close();
	
	// This is the odd ECOM code for cleaning our session. NB This must be 
	// done AFTER destroying all the other things in this thread which use 
	// ECOM!
	if ( iEcom )
		{
		iEcom->Close();
		}
	REComSession::FinalClose();
	}

CRemConServer::CRemConServer()
 :	CPolicyServer(CActive::EPriorityHigh, KRemConServerPolicy),
 	iTspConnections(_FOFF(TRemConAddress, iLink)),
	iTspIncomingCmdClients(_FOFF(TClientInfo, iLink)),
	iTspIncomingNotifyCmdClients(_FOFF(TClientInfo, iLink2)),
	iTspIf4Stub(*this)
	{
	LOG_FUNC;
	// NB CRemConServer uses CActive::EPriorityHigh to help it get priority 
	// over other AOs in its thread. (The fact that it's added to the AS 
	// before anything else helps too.) This is so that client requests are 
	// not blocked by other AOs in the thread being very busy. This relies of 
	// course on the cooperation of the other AOs running in RemCon's thread 
	// over which we have no control (e.g. those in externally-supplied 
	// bearers or the Target Selector Plugin). In Symbian OS, it's the best we 
	// can do.
	
	// This is needed for BC reasons, as the TUint32 padding in TClientInfo is now replaced with
	// a second TSglQueLink. Therefore in order to maintain BC we need these two classes
	// to be the same size.
	__ASSERT_COMPILE(sizeof(TUint32) == sizeof(TSglQueLink));
	}

void CRemConServer::ConstructL()
	{
	LOG_FUNC;
	// Open ECOM session.
	iEcom = &(REComSession::OpenL());
	LEAVEIFERRORL(iTargetClientsLock.CreateLocal());

	iShutdownTimer = CPeriodic::NewL(CActive::EPriorityStandard);

	// Make the connection history holder before creating the bearer manager, 
	// as some bearers might call the bearer manager back synchronously with a 
	// new connection, and we need iConnectionHistory to be able to handle 
	// that.
	iConnectionHistory = CConnectionHistory::NewL();

	// Make the queues before making the bearer manager because otherwise a 
	// 'connection up' which is indicated synchronously with 
	// CBearerManager::NewL will blow us up (we address some of the queues 
	// when that happens).
	iOutgoingCmdPendingTsp = CMessageQueue::NewL();
	iOutgoingNotifyCmdPendingTsp = CMessageQueue::NewL();
	iOutgoingRspPendingTsp = CMessageQueue::NewL();
	iOutgoingRspPendingSend = CMessageQueue::NewL();
	iOutgoingPendingSend = CMessageQueue::NewL();
	iOutgoingSent = CMessageQueue::NewL();
	iIncomingCmdPendingAddress = CMessageQueue::NewL();
	iIncomingNotifyCmdPendingAddress = CMessageQueue::NewL();
	iIncomingNotifyCmdPendingReAddress = CMessageQueue::NewL();
	iIncomingPendingDelivery = CMessageQueue::NewL();
	iIncomingDelivered = CMessageQueue::NewL();

	iMessageRecipientsList = CMessageRecipientsList::NewL();
	
	// Make bearer manager. This makes the bearers, and connects them up to 
	// the event handler.
	iBearerManager = CBearerManager::NewL(*this);
	
	// We must load the bearers before the TSP as the TSP loader checks
	// whether there are any bearers with interface V2	
	ASSERT_ALWAYS(!iTspIf);
	
	iConverterManager = CConverterManager::NewL();

	LoadTspL();

	LOGREMOTES;
	LOGCONNECTIONHISTORYANDINTEREST;
	}

CSession2* CRemConServer::NewSessionL(const TVersion& aVersion, 
	const RMessage2& aMessage) const
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG3(_L("\taVersion = (%d,%d,%d)"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild);
		
	// Version number check...
	if ( aVersion.iMajor != KRemConSrvMajorVersionNumber || aVersion.iMinor != KRemConSrvMinorVersionNumber )
		{
		LEAVEIFERRORL(KErrNotSupported);
		}

	// We need a non-const copy of ourself so that:
	// (a) New target clients may be added to iTargetClients (controller sessions are added to 
	// iControllerSessions when ControllerClientOpened() is invoked).
	// (b) We may provide new sessions/clients with a non-const pointer to ourself.
	CRemConServer* ncThis = const_cast<CRemConServer*>(this);

	CRemConSession* sess = NULL;
	ASSERT_DEBUG(iBearerManager);

	TInt err = KErrNotSupported;	
	if (aVersion.iBuild == KRemConSrvControllerSession)
		{
		TRAP(err, sess = ncThis->CreateControllerSessionL(aMessage));
		}
	else if(aVersion.iBuild == KRemConSrvTargetSession)
		{
		TRAP(err, sess = ncThis->CreateTargetSessionL(aMessage));
		}
	
	if ( err != KErrNone)
		{
		// Session creation might have failed- if it has we need to check if 
		// we need to shut down again.
		const_cast<CRemConServer*>(this)->StartShutdownTimerIfNoClientsOrBulkThread();
		LEAVEIFERRORL(err);
		}

	LOG1(_L("\tsess = 0x%08x"), sess);
	return sess;
	}

CRemConControllerSession* CRemConServer::CreateControllerSessionL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	CRemConControllerSession* sess=NULL;

	// Obtain client process ID.
	TClientInfo clientInfo;
	ClientProcessAndSecureIdL(clientInfo, aMessage);

	// Create the session and return
	sess = CRemConControllerSession::NewL(*this, *iBearerManager, clientInfo, iSessionOrClientId++);
	return sess;
	}

CRemConTargetSession* CRemConServer::CreateTargetSessionL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	CRemConTargetSession* sess=NULL;
	
	// Obtain client process ID and search for clients server-side process representation.
	TClientInfo clientInfo;
	ClientProcessAndSecureIdL(clientInfo, aMessage);

	// We search for the client ourselves here as we need to know where it
	// is in the array if we end up needing to destroy it.
	iTargetClientsLock.Wait();	
	CleanupSignalPushL(iTargetClientsLock);

	TInt clientIndex = iTargetClients.Find(clientInfo.ProcessId(), TargetClientCompareUsingProcessId);

	if (clientIndex == KErrNotFound)
		{
		// Client is new, create process representation for client and add to iTargetClients 
		CRemConTargetClientProcess* newClient = CRemConTargetClientProcess::NewLC(clientInfo, iSessionOrClientId++, *this, *iBearerManager);

		iTargetClients.AppendL(newClient);

		// Set clientIndex to point to the new client.
		clientIndex = iTargetClients.Count()-1;

		CleanupStack::Pop(newClient);
		}

	// Create session on client and return.
	// On error, remove the clients process representation if client has no other sessions.
	TRAPD(err, sess = iTargetClients[clientIndex]->NewSessionL(iSessionOrClientId++));
	if (err)
		{
		TryToDropClientProcess(clientIndex);
		LEAVEL(err);
		}

	CleanupStack::PopAndDestroy(&iTargetClientsLock);

	return sess;
	}

void CRemConServer::ClientProcessAndSecureIdL(TClientInfo& aClientInfo, const RMessage2& aMessage) const
	{
	LOG_FUNC;

	RThread thread;
	LEAVEIFERRORL(aMessage.Client(thread));
	CleanupClosePushL(thread);
	RProcess process;
	LEAVEIFERRORL(thread.Process(process));
	aClientInfo.ProcessId() = process.Id();
	process.Close();
	aClientInfo.SecureId() = thread.SecureId();
	CleanupStack::PopAndDestroy(&thread);
	}

void CRemConServer::StartShutdownTimerIfNoClientsOrBulkThread()
	{
	LOG_FUNC;
	iTargetClientsLock.Wait();
	if ( iControllerSessions.Count() == 0 && iTargetClients.Count() == 0 && !iBulkThreadOpen)
		{
		LOG(_L("\tno remaining sessions- starting shutdown timer"));
		// Should have been created during our construction.
		ASSERT_DEBUG(iShutdownTimer);
		// Start the shutdown timer. It's actually a CPeriodic- the first 
		// event will be in KShutdownDelay microseconds' time.
		// NB The shutdown timer might already be active, in the following 
		// case: this function is being called by NewSessionL because there 
		// was a failure creating a new session, BUT this function had already 
		// been called by the session's destructor (i.e. the failure was in 
		// the session's ConstructL, NOT its new(ELeave)). To protect against 
		// KERN-EXEC 15 just check for if the timer is already active. 
		if ( !iShutdownTimer->IsActive() )
			{
			iShutdownTimer->Start(KShutdownDelay, 
				// Delay of subsequent firings (will not happen because we kill 
				// ourselves after the first).
				0, 
				TCallBack(CRemConServer::TimerFired, this)
				);
			}
		else
			{
			LOG(_L("\tshutdown timer was already active"));
			}
		}
	iTargetClientsLock.Signal();
	}

void CRemConServer::CancelShutdownTimer()
	{
	// Should have been created during our construction.		
	ASSERT_DEBUG(iShutdownTimer);
	iShutdownTimer->Cancel();
	}

void CRemConServer::TryToDropClientProcess(TUint aClientIndex)
	{
	ASSERT_DEBUG(iTargetClients.Count() > aClientIndex);
	
	iTargetClientsLock.Wait();
	
	CRemConTargetClientProcess* client = iTargetClients[aClientIndex]; 
	if (client->TargetSessionCount() == 0)
		{
		// No sessions held by this client process representation, OK to destroy.
		// The destructor of CRemConTargetClientProcess will call back to us, after which we will
		// handle the client close.
		delete client;
		}
	
	iTargetClientsLock.Signal();
	}

TInt CRemConServer::TimerFired(TAny* aThis)
	{
	LOG_STATIC_FUNC;
	static_cast<void>(aThis);
	
#if defined(__FLOG_ACTIVE) || defined(_DEBUG)
	CRemConServer* self = static_cast<CRemConServer*>(aThis);
	// We should have sent 'this' to this callback. 
	ASSERT_DEBUG(self);
	LOG1(_L8("\tauto shutdown- terminating the server [0x%08x]"), self);
#endif // __FLOG_ACTIVE || _DEBUG
	
	// Stop our Active Scheduler. This returns the flow of execution to after 
	// the CActiveScheduler::Start call in the server startup code, and 
	// terminates the server.
	CActiveScheduler::Stop();
	
	return KErrNone;
	}

void CRemConServer::InitialiseBulkServerThreadL()
	{
	LOG_FUNC;
	// Set up the configuration of the thread
	iBulkServerThread.SetPriority(EPriorityLess);
	
	iBulkThreadWatcher = new(ELeave) CBulkThreadWatcher(*this);
	CleanupDeleteAndNullPushL(iBulkThreadWatcher);
	
	// Create the communication between the servers.
	LEAVEIFERRORL(iBulkServerMsgQueue.CreateLocal(2)); // only ever two outstanding messages
	CleanupClosePushL(iBulkServerMsgQueue); // member variable, but closing again should be fine.
	
	// Load the server pointer (for TClientInfo info)
	TBulkServerMsg ctrlMsg;
	ctrlMsg.iType = EControlServer;
	ctrlMsg.iData = this;
	iBulkServerMsgQueue.SendBlocking(ctrlMsg); // this should not block as there is enough room in queue.
	
	// Load the bearer manager pointer
	TBulkServerMsg manMsg;
	manMsg.iType = EBearerManager;
	manMsg.iData = iBearerManager;
	iBulkServerMsgQueue.SendBlocking(manMsg); // this should not block as there is enough room in queue.
	
	TRequestStatus stat;
	iBulkServerThread.Rendezvous(stat);
	
	// Mark bulk server thread ready to run
	iBulkServerThread.Resume();
	User::WaitForRequest(stat); 	// wait for start or death
	
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	TInt err = (iBulkServerThread.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();
	LEAVEIFERRORL(err);
	
	// Everything appears to be running...so watch the thread until it dies...
	iBulkThreadWatcher->StartL();
	
	CleanupStack::Pop(2, &iBulkThreadWatcher); // iBulkServerMsgQueue, iBulkThreadWatcher
	}


TInt CRemConServer::BulkServerRequired()
	{
	LOG_FUNC;
	// If the bulk server is required then try to create it
	TThreadFunction bulkServerThreadFunction(BulkMain);
	_LIT(KBulkServerThreadName, "RemConBulkServerThread");
	const TInt KMaxBulkServerThreadHeapSize = 0x100000;
	
	TInt err = KErrNone;
	if(!iBulkThreadOpen)
		{
		err = iBulkServerThread.Create(KBulkServerThreadName, bulkServerThreadFunction, KDefaultStackSize, KMinHeapSize, KMaxBulkServerThreadHeapSize, &iBulkServerMsgQueue, EOwnerThread);
		if(err == KErrNone)
			{
			TRAP(err, InitialiseBulkServerThreadL());
			if(err == KErrNone)
				{
				iBulkThreadOpen = ETrue;
				}
			}
		}
	return err;
	}

TInt CRemConServer::ControllerClientOpened(CRemConControllerSession& aSession)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGCONTROLLERSESSIONS;

	// Register the session by appending it to our array, and also making an 
	// item for it in the record of which points in the connection history 
	// sessions are interested in.
	TInt ret = iControllerSessions.Append(&aSession);
	if ( ret == KErrNone )
		{
		TSessionPointerToConnectionHistory item;
		item.iSessionId = aSession.Id();
		item.iIndex = 0; // there is always at least one item in the connection history
		ret = iSession2ConnHistory.Append(item);
		if ( ret != KErrNone )
			{
			iControllerSessions.Remove(iControllerSessions.Count() - 1);
			}
		}

	if ( ret == KErrNone )
		{
		// Should have been created during our construction.
		ASSERT_DEBUG(iShutdownTimer);
		iShutdownTimer->Cancel();
		}

	LOGCONTROLLERSESSIONS;
	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TInt CRemConServer::RegisterTargetSessionPointerToConnHistory(const CRemConTargetSession& aSession)
	{
	LOG_FUNC;

	TSessionPointerToConnectionHistory item;
	item.iSessionId = aSession.Id();
	item.iIndex = 0; // there is always at least one item in the connection history
	TInt ret = iSession2ConnHistory.Append(item);

	return ret;
	}


// this function is called by the client when it has registered its features
void CRemConServer::TargetClientAvailable(const CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOG1(_L("\t&aClient = 0x%08x"), &aClient);
	LOGTARGETSESSIONS;

	ASSERT_DEBUG(iBearerManager);
	iBearerManager->TargetClientAvailable(aClient.Id(), aClient.PlayerType(), aClient.PlayerSubType(), aClient.Name());
	if(iTspIf5)
		{
		iTspIf5->TargetClientAvailable(aClient.ClientInfo());
		}

	LOGTARGETSESSIONS;
	}

void CRemConServer::TargetFeaturesUpdated(CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOG1(_L("\t&aClient = 0x%08x"), &aClient);
	LOGTARGETSESSIONS;

	ASSERT_DEBUG(iBearerManager);
	iBearerManager->TargetFeaturesUpdated(aClient.Id(), aClient.PlayerType(), aClient.PlayerSubType(), aClient.Name());

	LOGTARGETSESSIONS;
	}

// this function is called by the session when the client has registered its features
void CRemConServer::ControllerClientAvailable()
	{
	LOG_FUNC;
	LOGCONTROLLERSESSIONS;

	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ControllerClientAvailable();
	
	LOGCONTROLLERSESSIONS;
	}

// this function is called by the session when it goes connection oriented
void CRemConServer::ClientGoConnectionOriented(CRemConControllerSession& aSession, TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGCONTROLLERSESSIONS;

	(void)&aSession; // get rid of unused warning. We keep the param to enforce usage only
					 // by controller sessions

	/* now tell the bearer manager that someones went connection oriented
	   The bearer manager maintains controller and target counts for all bearers
	   and will tell bearers when they need to know things have changed */
	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ClientConnectionOriented(aUid);

	LOGCONTROLLERSESSIONS;
	}

// this is called by the session when the client goes connectionless
void CRemConServer::ClientGoConnectionless(CRemConControllerSession& aSession, TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGCONTROLLERSESSIONS;

	(void)&aSession; // get rid of unused warning. We keep the param to enforce usage only
					 // by controller sessions

	/* now tell the bearer manager that someones went connection less
	   The bearer manager maintains controller and target counts for all bearers
	   and will tell bearers when they need to know things have changed */
	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ClientConnectionless(aUid);

	LOGCONTROLLERSESSIONS;
	}

// called by controller session when closed.
void CRemConServer::ControllerClientClosed(CRemConControllerSession& aSession, TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGCONTROLLERSESSIONS;

	// Find this session in the array and remove it (if it's there).
	TInt index = iControllerSessions.Find(&aSession);
	if(index >= 0)
		{
		// We've found the session in our array.
		// 1. Remove the session from our array.
		iControllerSessions.Remove(index);
		
		// 2. Tell the bearers about the session going away, if it was the 
		// last controller.
		// If the session hasn't already set its type, then it doesn't 
		// count (we won't have told the bearers about it to begin with).
		// The bearer manager maintains controller and target counts for all bearers
		// and will tell bearers when they need to know things have changed 
		if (aSession.ClientAvailable())
			{
			ASSERT_DEBUG(iBearerManager);
			iBearerManager->ClientClosed(ETrue, aUid, aSession.Id());
			}

		// 3. Remove queued messages belonging to this session that: 
		// (a) are outgoing, awaiting access to the TSP 
		// (OutgoingPendingTsp, OutgoingNotifyPendingTsp), 
		// (b) are outgoing, awaiting a bearer connection 
		// (OutgoingPendingSend), 
		// (c) have been sent (OutgoingSent)
		// (d) are pending delivery to this session 
		// (IncomingPendingDelivery)

		// (3)(a) Outgoing, waiting access to the TSP:
		TSglQueIter<CRemConMessage>& cmdIter = OutgoingCmdPendingTsp().SetToFirst();
		CRemConMessage* msg;
		TBool first = ETrue;
		while ( ( msg = cmdIter++ ) != NULL )
			{
			if ( msg->SessionId() == aSession.Id() )
				{
				// If the message is currently being worked on by the 
				// TSP, cancel the TSP before destroying it.
				if ( iTspHandlingOutgoingCommand && first )
					{
					ASSERT_DEBUG(iTspIf);
					iTspIf->CancelOutgoingCommand();
					iTspHandlingOutgoingCommand = EFalse;
					}
				OutgoingCmdPendingTsp().RemoveAndDestroy(*msg);
				}
			first = EFalse;
			}
		
		cmdIter = OutgoingNotifyCmdPendingTsp().SetToFirst();
		first = ETrue;
		while ( ( msg = cmdIter++ ) != NULL )
			{
			if ( msg->SessionId() == aSession.Id() )
				{
				// If the message is currently being worked on by the 
				// TSP, cancel the TSP before destroying it.
				if ( iTspHandlingOutgoingNotifyCommand && first )
					{
					ASSERT_DEBUG(iTspIf3);
					iTspIf3->CancelOutgoingNotifyCommand();
					iTspHandlingOutgoingNotifyCommand = EFalse;
					}
				OutgoingNotifyCmdPendingTsp().RemoveAndDestroy(*msg);
				}
			first = EFalse;
			}

		// (3)(b) Outgoing, awaiting a bearer connection:
		TSglQueIter<CRemConMessage>& sendIter = OutgoingPendingSend().SetToFirst();
		while ( ( msg = sendIter++ ) != NULL )
			{
			if ( msg->SessionId() == aSession.Id() )
				{
				// Only commands are sent by controllers
				ASSERT_DEBUG(msg->MsgType() == ERemConCommand || msg->MsgType() == ERemConNotifyCommand);
				OutgoingPendingSend().RemoveAndDestroy(*msg);
				}
			}

		// (3)(c) Have been sent:
		OutgoingSent().RemoveAndDestroy(aSession.Id());
		
		// (3)(d) Are pending delivery to this session:
		TSglQueIter<CRemConMessage>& pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
		while ( ( msg = pendingDeliveryIter++ ) != NULL )
			{
			if ( msg->SessionId() == aSession.Id() )
				{
				// Only responses or rejects are received by controllers
				ASSERT_DEBUG(msg->MsgType() == ERemConResponse || msg->MsgType() == ERemConReject);
				IncomingPendingDelivery().RemoveAndDestroy(*msg);
				}
			}

		// (3)(e) Have been delivered to this session and are awaiting responses:
		TSglQueIter<CRemConMessage>& deliveredIter = IncomingDelivered().SetToFirst();
		while ( ( msg = deliveredIter++ ) != NULL )
			{
			if ( msg->SessionId() == aSession.Id() )
				{
				// Only responses or rejects are received by controllers
				ASSERT_DEBUG(msg->MsgType() == ERemConResponse || msg->MsgType() == ERemConReject);
				IncomingDelivered().RemoveAndDestroy(*msg);
				}
			}
		}

	// Also remove its record from the connection history record.
	RemoveSessionFromConnHistory(aSession);
	
	StartShutdownTimerIfNoClientsOrBulkThread();

	LOGCONTROLLERSESSIONS;
	}

/**
Called by CRemConTargetClientProcess when a session is closing.
We have some work to do here as we need to remove the messages pertaining to
that session.
**/
void CRemConServer::TargetSessionClosed(CRemConTargetClientProcess& aClient, CRemConTargetSession& aSession)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGTARGETSESSIONS;
	
	iTargetClientsLock.Wait();

	// Find the client in our array (required for later removal)
	TInt clientIndex = iTargetClients.Find(aClient.ClientInfo().ProcessId(), TargetClientCompareUsingProcessId);

	// We should always find the client.
	ASSERT_DEBUG(clientIndex > KErrNotFound);

	// 1. Remove queued messages belonging to this session that:
	// (a) are outgoing, awaiting access to the TSP 
	// (OutgoingRspPendingTsp)
	// (b) are outgoing, awaiting a bearer connection 
	// (OutgoingPendingSend), 
	// (c) have been sent (OutgoingSent)
	// (d) are pending delivery to this session 
	// (IncomingPendingDelivery)
	// (e) have been delivered to this session and are awaiting 
	// responses (IncomingDelivered).
		
	// (1)(a) Outgoing, awaiting access to the TSP
	// First remove the client pertaining to this session from the message recipients list.
	ASSERT_DEBUG(iMessageRecipientsList);
	TSglQueIter<CMessageRecipients>& messageRecipientsIter = iMessageRecipientsList->Iter();
	
	messageRecipientsIter.SetToFirst();
	CMessageRecipients* message;
	while ((message = messageRecipientsIter++) != NULL)
		{
		// First we need to find the message - it could be in
		// OutgoingRspPendingTsp, IncomingDelivered or IncomingPendingDelivery
		CRemConMessage* msg;
		msg = OutgoingRspPendingTsp().Message(message->TransactionId());
			
		if (!msg)
			{
			msg = IncomingDelivered().Message(message->TransactionId());
			}
		if (!msg)
			{
			msg = IncomingPendingDelivery().Message(message->TransactionId());
			}
			
		if(msg)
			{
			// Try to remove this client from the message (this does nothing if we were not a recipient).
			message->RemoveAndDestroyClient(aSession.ClientInfo());
			if (message->Clients().IsEmpty())
				{
				iMessageRecipientsList->Messages().Remove(*message);
				// Inform bearer that it won't be getting a response
				SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());

				delete message;
				}
			}
		}

	TSglQueIter<CRemConMessage>& rspIter = OutgoingRspPendingTsp().SetToFirst();
	CRemConMessage* msg;
	TBool first = ETrue;
	while ( ( msg = rspIter++ ) != NULL )
		{
		if ( aClient.Id() == msg->SessionId() && aSession.SupportedMessage(*msg) )
			{
			// If the message is currently being worked on by the 
			// TSP, cancel the TSP before destroying it.
			if (iTspIf2 && iTspHandlingOutgoingResponse && first )
				{
				iTspIf2->CancelOutgoingResponse();
				iTspHandlingOutgoingResponse = EFalse;
				}
			OutgoingRspPendingTsp().RemoveAndDestroy(*msg);
			}
		first = EFalse;
		}

	// (1)(b) Outgoing, awaiting a bearer connection:
	TSglQueIter<CRemConMessage>& sendIter = OutgoingPendingSend().SetToFirst();
	while ( ( msg = sendIter++ ) != NULL )
		{
		if ( aClient.Id() == msg->SessionId() && aSession.SupportedMessage(*msg) )
			{
			if (msg->MsgType() == ERemConResponse)
				{
				SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
				}
			OutgoingPendingSend().RemoveAndDestroy(*msg);
			}
		}

	// (1)(c) Have been sent:
	TSglQueIter<CRemConMessage>& haveSentIter = OutgoingSent().SetToFirst();
	while ( ( msg = haveSentIter++ ) != NULL)
		{
		if ( aClient.Id() == msg->SessionId() && aSession.SupportedMessage(*msg) )
			{
			OutgoingSent().RemoveAndDestroy(*msg);
			}
		}
	
	// (1)(d) Are pending delivery to this session:
	TSglQueIter<CRemConMessage>& pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
	while ( ( msg = pendingDeliveryIter++ ) != NULL )
		{
		if ( aClient.Id() == msg->SessionId() && aSession.SupportedMessage(*msg) )
			{
			if (msg->MsgType() == ERemConNotifyCommand)
				{
				SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
				}
			IncomingPendingDelivery().RemoveAndDestroy(*msg);
			}
		}

	// (1)(e) Have been delivered to this session and are awaiting responses:
	TSglQueIter<CRemConMessage>& deliveredIter = IncomingDelivered().SetToFirst();
	while ( ( msg = deliveredIter++ ) != NULL )
		{
		if ( aClient.Id() == msg->SessionId() && aSession.SupportedMessage(*msg) )
			{
			if (msg->MsgType() == ERemConNotifyCommand)
				{
				SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
				}
			IncomingDelivered().RemoveAndDestroy(*msg);
			}
		}

	// Remove the session's record from the connection history record.
	RemoveSessionFromConnHistory(aSession);

	// Finally, try to delete client process representation if it now has no sessions
	TryToDropClientProcess(clientIndex);

	iTargetClientsLock.Signal();
	
	LOGTARGETSESSIONS;
	}

void CRemConServer::RemoveSessionFromConnHistory(const CRemConSession& aSession)
	{
	LOG_FUNC;

	const TUint count = iSession2ConnHistory.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iSession2ConnHistory[ii].iSessionId == aSession.Id() )
			{
			iSession2ConnHistory.Remove(ii);
			UpdateConnectionHistoryAndPointers();
			break;
			}
		}
	}

// called by client process representation on close.
void CRemConServer::TargetClientClosed(CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOG1(_L("\t&aClient = 0x%08x"), &aClient);
	LOGTARGETSESSIONS;

	iTargetClientsLock.Wait();
	// Find this client in the array and remove it (if it's there).
	const TUint clientCount = iTargetClients.Count();
	for ( TUint ii = 0 ; ii < clientCount ; ++ii )
		{
		if ( iTargetClients[ii] == &aClient )
			{
			// We've found the client in our array.

			// 1. Remove the client from our array.
			iTargetClients.Remove(ii);

			// 2a. Tell the TSP the client has gone away 
			if(iTspIf5)
				{
				iTspIf5->TargetClientUnavailable(aClient.ClientInfo());
				}
			
			// 2b. Tell the bearers about the client going away, if it was the 
			// last target.
			// If the client hasn't already set its type, then it doesn't 
			// count (we won't have told the bearers about it to begin with).
			// The bearer manager maintains controller and target counts for all bearers
			// and will tell bearers when they need to know things have changed 
			if (aClient.ClientAvailable())
				{
				ASSERT_DEBUG(iBearerManager);
				iBearerManager->ClientClosed(EFalse, KNullUid, aClient.Id());
				}
					
			break;
			} // End found session in our array
		}
	iTargetClientsLock.Signal();

	StartShutdownTimerIfNoClientsOrBulkThread();

	LOGTARGETSESSIONS;
	}

#ifdef __FLOG_ACTIVE

void CRemConServer::LogControllerSessions() const 
	{
	const TUint count = iControllerSessions.Count();
	LOG1(_L("\tNumber of controller sessions = %d"), count);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const session = iControllerSessions[ii];
		ASSERT_DEBUG(session);
		LOG4(_L("\t\tsession %d [0x%08x], Id = %d, ProcessId = %d"), 
			ii, 
			session,
			session->Id(),
			static_cast<TUint>(session->ClientInfo().ProcessId())
			);
		}
	}

void CRemConServer::LogTargetSessions() const 
	{
	iTargetClientsLock.Wait();
	
	const TUint count = iTargetClients.Count();
	LOG1(_L("\tNumber of target clients = %d"), count);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConTargetClientProcess* const client = iTargetClients[ii];
		ASSERT_DEBUG(client);
		LOG5(_L("\t\tclient %d [0x%08x], Id = %d, ProcessId = %d, SessionCount = %d"), 
			ii, 
			client,
			client->Id(),
			static_cast<TUint>(client->ClientInfo().ProcessId()),
			client->TargetSessionCount()
			);
		}
	iTargetClientsLock.Signal();
	}

void CRemConServer::LogRemotes() const
	{
	// Called from dtor- this may not have been made yet.
	if ( iConnectionHistory )
		{
		CConnections& conns = iConnectionHistory->Last();
		conns.LogConnections();
		}
	}

void CRemConServer::LogConnectionHistoryAndInterest() const
	{
	LOG(_L("Logging connection history and interest in it"));
	if ( iConnectionHistory )
		{
		iConnectionHistory->LogConnectionHistory();

		const TUint count = iSession2ConnHistory.Count();
		LOG1(_L("\tNumber of sessions = %d"), count);
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			const TSessionPointerToConnectionHistory& interest = iSession2ConnHistory[ii];
			LOG3(_L("\t\tinterest %d, iSessionId = %d, iIndex = %d"), 
				ii, 
				interest.iSessionId,
				interest.iIndex
				);
			}
		}
	}
#endif // __FLOG_ACTIVE

void CRemConServer::MrctspoDoOutgoingNotifyCommandAddressed(TRemConAddress* aConnection, TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	LOGOUTGOINGNOTIFYCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingNotifyCommand);
	iTspHandlingOutgoingNotifyCommand = EFalse;

	CRemConMessage& msg = OutgoingNotifyCmdPendingTsp().First();
	ASSERT_DEBUG(msg.Addr().IsNull());
	MRemConMessageSendObserver* const observer = ControllerSession(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If observer is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(observer);
	
	if ( (aError != KErrNone) || !aConnection)
		{
		observer->MrcmsoMessageSendResult(msg, aError);
		}
	else
		{
		// Message addressed OK.
		if ( aConnection != NULL )
			{
			TBool sync = EFalse;
			TRAPD(err, SendCmdToRemoteL(msg, *aConnection, sync));
			if ( err || sync )
				{
				observer->MrcmsoMessageSendResult(msg, err);
				}
			
			delete aConnection;
			aConnection = NULL;
			}
		} 

	// We've now finished with the addressed message, so destroy it.
	OutgoingNotifyCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more notify commands to address.
	if ( !OutgoingNotifyCmdPendingTsp().IsEmpty() )
		{
		LOG(_L8("\tmore outgoing notify commands awaiting TSP..."));
		TspOutgoingNotifyCommand();
		}

	LOGOUTGOINGNOTIFYCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingCommandAddressed(TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingCommand);
	iTspHandlingOutgoingCommand = EFalse;

	// If aError is KErrNone,
	// Remove the addressed message from the iOutgoingPendingTsp queue.
	// Send the command to the requested bearer(s), putting items on the 
	// iOutgoingSent queue to await responses
	// Complete the controller client's message. 

	// Any error at any point during the above should roll back everything 
	// (apart from removing the original message from iOutgoingPendingTsp) 
	// and error the sending controller.
	
	// Finally, check iOutgoingPendingTsp for more commands to give to 
	// the TSP.

	// The head item, currently being dealt with, is always at index 0.
	// NB This msg will be destroyed by the end of the function, and copies 
	// taken to add to iOutgoingSent. 
	// Note that if the client went away while the address request was 
	// outstanding, this item will still be on this queue because we protect 
	// it (see ClientClosed).
	CRemConMessage& msg = OutgoingCmdPendingTsp().First();
	// Check that the message isn't addressed already. If this fails, it's 
	// possible that the TSP has called OutgoingCommandAddressed in response 
	// to a PermitOutgoingCommand request.
	ASSERT_DEBUG(msg.Addr().IsNull());
	// The observer is the session which generated the message.
	MRemConMessageSendObserver* const observer = ControllerSession(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If observer is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(observer);

	TInt numRemotesToTry = 0;

	if ( aError != KErrNone )
		{
		// Error prevented message send attempt from being made.
		observer->MrcmsoMessageSendOneOrMoreAttemptFailed(msg, aError);
		}
	else
		{
		// Message addressed OK.
		// Work out how many remotes the TSP said to send to.
		TSglQueIter<TRemConAddress> iter(iTspConnections);
		while ( iter++ )
			{
			++numRemotesToTry;
			}
		// Notify session of send attempt.
		observer->MrcmsoMessageSendOneOrMoreAttempt(msg, numRemotesToTry);
		iter.SetToFirst();
		// Try to connect and send a message to each specified remote.
		TRemConAddress* conn;
		while ( ( conn = iter++ ) != NULL )
			{
			LOG2(_L("\tsending message to remote [0x%08x] BearerUid = 0x%08x"), 
				conn, conn->BearerUid());

			// We send to as many of the remotes as we can. The observer remembers 
			// how many remotes got sent to successfully, and completes the 
			// client's request with either KErrNone or _one of_ the 
			// errors that were raised. 
			TBool sync = EFalse;
			TRAPD(err, SendCmdToRemoteL(msg, *conn, sync));
			if ( err || sync )
				{
				// We have finished trying to process this (copy of this) 
				// message, so we can adjust our 'remotes' counter / 
				// completion error.
				observer->MrcmsoMessageSendOneOrMoreResult(msg, err);
				}
			// else we didn't actually make a send attempt because conn was 
			// down. This particular message will undergo an actual 
			// (bearer-level) send attempt later on when the connection comes 
			// up. For now, however, we cannot legally complete the client's 
			// request.

			iTspConnections.Remove(*conn);
			delete conn;
			} // End while 
		} // End if TSP addressed command OK

	// We've now finished with the addressed message, so destroy it.
	OutgoingCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more commands to address.
	if ( !OutgoingCmdPendingTsp().IsEmpty() )
		{
		LOG(_L("\tmore outgoing commands awaiting TSP..."));
		TspOutgoingCommand();
		}

	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingCommandPermitted(TBool aIsPermitted)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taIsPermitted = %d"), aIsPermitted);
	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingCommand);
	iTspHandlingOutgoingCommand = EFalse;

	// If aIsPermitted is EFalse, complete the sending session with 
	// KErrPermissionDenied and destroy the message. Otherwise try to send the 
	// message and complete the sending session. If it sent OK, move the 
	// message to the 'outgoing sent' queue.
	// At the end, check the 'pending TSP' queue again.
	// The head item, currently being dealt with, is always at index 0.
	// Note that if the client went away while the address request was 
	// outstanding, this item will still be on this queue because we protect 
	// it (see ClientClosed).
	CRemConMessage& msg = OutgoingCmdPendingTsp().First();
	// Check that the message is addressed already. If this fails, it's 
	// possible that the TSP has called OutgoingCommandPermitted in response 
	// to a AddressOutgoingCommand request.
	ASSERT_DEBUG(!msg.Addr().IsNull());
	// The session is the observer
	MRemConMessageSendObserver* const observer = ControllerSession(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If observer is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(observer);
	TInt err = KErrPermissionDenied;
	if ( aIsPermitted )
		{
		TBool sync = EFalse;
		TRAP(err, SendCmdToRemoteL(msg, msg.Addr(), sync));
		if ( err || sync )
			{
			// We made a send attempt at the bearer level, notify observer.
			observer->MrcmsoMessageSendOneOrMoreResult(msg, err);
			}
		// else the message is waiting until a bearer-level connection 
		// comes up. Only then can we complete the client's message.
		}
	else
		{
		// The send wasn't permitted, notify observer.
		// This should complete the client's message, as we're connection oriented
		// (so only one remote to send to).
		observer->MrcmsoMessageSendOneOrMoreResult(msg, KErrPermissionDenied);
		}

	// We've now finished with the message, so destroy it.
	OutgoingCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more commands to give to the TSP.
	if ( !OutgoingCmdPendingTsp().IsEmpty() )
		{
		LOG(_L("\tmore outgoing commands awaiting TSP..."));
		TspOutgoingCommand();
		}

	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingNotifyCommandPermitted(TBool aIsPermitted)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taIsPermitted = %d"), aIsPermitted);
	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingNotifyCommand);
	iTspHandlingOutgoingNotifyCommand = EFalse;

	// If aIsPermitted is EFalse, complete the sending session with 
	// KErrPermissionDenied and destroy the message. Otherwise try to send the 
	// message and complete the sending session. If it sent OK, move the 
	// message to the 'outgoing sent' queue.
	// At the end, check the 'pending TSP' queue again.
	// The head item, currently being dealt with, is always at index 0.
	// Note that if the client went away while the address request was 
	// outstanding, this item will still be on this queue because we protect 
	// it (see ClientClosed).
	CRemConMessage& msg = OutgoingNotifyCmdPendingTsp().First();
	// Check that the message is addressed already. If this fails, it's 
	// possible that the TSP has called OutgoingCommandPermitted in response 
	// to a AddressOutgoingCommand request.
	ASSERT_DEBUG(!msg.Addr().IsNull());
	MRemConMessageSendObserver* const observer = ControllerSession(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If observer is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(observer);
	TInt err = KErrPermissionDenied;
	if ( aIsPermitted )
		{
		TBool sync = EFalse;
		TRAP(err, SendCmdToRemoteL(msg, msg.Addr(), sync));
		if ( err || sync )
			{
			observer->MrcmsoMessageSendResult(msg, err);
			}
		// else the message is waiting until a bearer-level connection 
		// comes up. Only then can we complete the client's message.
		}
	else
		{
		observer->MrcmsoMessageSendResult(msg, KErrPermissionDenied);
		}

	// We've now finished with the message, so destroy it.
	OutgoingNotifyCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more commands to give to the TSP.
	if ( !OutgoingNotifyCmdPendingTsp().IsEmpty() )
		{
		LOG(_L("\tmore outgoing commands awaiting TSP..."));
		TspOutgoingNotifyCommand();
		}

	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingResponsePermitted(TBool aIsPermitted)
	{
	LOG_FUNC;
	LOGOUTGOINGRSPPENDINGTSP;
	ASSERT_DEBUG(iTspHandlingOutgoingResponse);
	iTspHandlingOutgoingResponse = EFalse;
	ASSERT_DEBUG(iOutgoingRspPendingTsp);
	CRemConMessage& msg = iOutgoingRspPendingTsp->First();
	
	iOutgoingRspPendingTsp->Remove(msg);
	
	CRemConTargetClientProcess* const client = TargetClient(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If client is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(client);

	if (aIsPermitted)
		{
		ASSERT_DEBUG(iMessageRecipientsList);
		iMessageRecipientsList->RemoveAndDestroyMessage(msg.TransactionId());
		CompleteSendResponse(msg, *client); // Ownership of msg is always taken
		}
	else
		{
		CMessageRecipients* messageClients = iMessageRecipientsList->Message(msg.TransactionId());
		if (messageClients)
			{
			messageClients->RemoveAndDestroyClient(client->ClientInfo()); // Remove the current client info from the list
			if (messageClients->Clients().IsEmpty())
				{
				iMessageRecipientsList->RemoveAndDestroyMessage(msg.TransactionId());
				// The bearer won't be getting a response
				
				SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
				}
			}

		// Notify client that a send attempt to a remote was abandoned.
		client->MrcmsoMessageSendOneOrMoreAbandoned(msg);
		delete &msg;
		}
	if (!iOutgoingRspPendingTsp->IsEmpty())
		{
		PermitOutgoingResponse();
		}
	LOGOUTGOINGRSPPENDINGTSP;
	}

void CRemConServer::MrctspoDoIncomingNotifyAddressed(TClientInfo* aClientInfo, TInt aError)
	{
	LOG_FUNC;
	LOG(KNullDesC8());
	LOG1(_L("\taError = %d"), aError);
	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	LOGINCOMINGDELIVERED;

	// Send the command to the requested target client, and remove the 
	// addressed message from the iIncomingNotifyCmdPendingAddress queue. Any error at 
	// any point should be ignored- just complete as much as we can. 
	// Then check iIncomingPendingAddress for more commands to give to the 
	// TSP. 

	ASSERT_DEBUG(iTspAddressingIncomingNotifyCommand || iTspReAddressingIncomingNotifyCommands);
	
	if (iTspAddressingIncomingNotifyCommand)
		{
		iTspAddressingIncomingNotifyCommand = EFalse;
		if (!iTspDropIncomingNotifyCommand)
			{
			// We know that the queue is not empty because we put something on the 
			// queue to call AddressIncomingNotifyCommand, which results in one call to this 
			// function, which is the only place where messages are removed from the 
			// queue.
			CRemConMessage& msg = IncomingNotifyCmdPendingAddress().First();
			TBool cmdDelivered = EFalse;
		
			// If the TSP errored, can't complete to any clients.
			if ( aError == KErrNone && aClientInfo)
				{
				LOG1(_L("\t\tprocess ID %d"), static_cast<TUint>(aClientInfo->ProcessId()));
				// Get the corresponding client.
				CRemConTargetClientProcess* const client = TargetClient(aClientInfo->ProcessId());
				// NB The set of open clients may have changed while the request 
				// was out on the TSP. If the TSP indicates a client that has 
				// gone away, then ignore that client. 
				if ( client )
					{
					TRAPD(err, DeliverCmdToClientL(msg, *client));
					if (err == KErrNone)
						{
						cmdDelivered = ETrue;
						}
					// If we couldn't deliver an instance of the command to a 
					// target, there's not much we can do. 
					}
				}
			
			if (!cmdDelivered)
				{
				// The command wasn't delivered to any clients
				
				// The command wasn't delivered to any clients
				// Tell bearer it won't be getting a response		
		
				SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
				}

			
			// Destroy the message we've just dealt with.
			IncomingNotifyCmdPendingAddress().RemoveAndDestroy(msg);
			}
		iTspDropIncomingNotifyCommand = EFalse;
		if ( !IncomingNotifyCmdPendingAddress().IsEmpty() )
			{
			LOG(_L("\tmore incoming commands awaiting addressing..."));
			AddressIncomingNotifyCommand();
			}
		}
	else
		{
		if (!iTspDropIncomingNotifyCommand)
			{
			CRemConMessage& msg = IncomingNotifyCmdPendingReAddress().First();
			if(aError == KErrNone && aClientInfo)
				{
				LOG1(_L("\t\tprocess ID %d"), static_cast<TUint>(aClientInfo->ProcessId()));
				// Get the corresponding client.
				CRemConTargetClientProcess* const client = TargetClient(aClientInfo->ProcessId());
				if (client)
					{
					if (client->Id() == msg.SessionId())
						{
						// Don't do anything - it's already on IncomingDelivered
						}
					else
						{
						// Remove the original message from IncomingDelivered
						CRemConMessage* deliveredMsg = NULL;
						TSglQueIter<CRemConMessage> iter = IncomingDelivered().SetToFirst();
						while ((deliveredMsg = iter++) != NULL)
							{
							if (deliveredMsg->TransactionId() == msg.TransactionId())
								{
								// We need to update the subtype now, in case the client has sent an interim response while the notify
								// was being readdressed.
								msg.MsgSubType() = deliveredMsg->MsgSubType();
								// Deliver to the client
								TRAPD(err, DeliverCmdToClientL(msg, *client));
								if (err == KErrNone)
									{
									// Only remove the current message if the delivery to the new client suceeded.
									IncomingDelivered().RemoveAndDestroy(*deliveredMsg);
									}
								break;
								}
							}
						}
					}
				else
					{
					SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
					}
				}
			IncomingNotifyCmdPendingReAddress().RemoveAndDestroy(msg);
			}
			
		iTspDropIncomingNotifyCommand = EFalse;
	
		if ( !IncomingNotifyCmdPendingReAddress().IsEmpty() )
			{
			LOG(_L("\tmore incoming commands awaiting readdressing..."));
			ReAddressIncomingNotifyCommand();
			}
		else
			{
			iTspReAddressingIncomingNotifyCommands = EFalse;
			if ( !IncomingNotifyCmdPendingAddress().IsEmpty() )
				{
				LOG(_L("\tmore incoming commands awaiting addressing..."));
				AddressIncomingNotifyCommand();
				}
			}
		}
	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::MrctspoDoReAddressNotifies()
	{
	LOG_FUNC;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	LOGINCOMINGDELIVERED;
	ASSERT_DEBUG(iTspReAddressingIncomingNotifyCommands == EFalse && iTspAddressingIncomingNotifyCommand == EFalse);
	TSglQueIter<CRemConMessage> iter = IncomingDelivered().SetToFirst();
	CRemConMessage* deliveredMsg;
	while ((deliveredMsg = iter++) != NULL)
		{
		if (deliveredMsg->MsgType() == ERemConNotifyCommand)
			{
			CRemConMessage* newMsg = NULL;
			TRAPD(err, newMsg = CRemConMessage::CopyL(*deliveredMsg));
			if (err == KErrNone)
				{
				IncomingNotifyCmdPendingReAddress().Append(*newMsg);
				}
			// If we couldn't copy the message, there isn't much we can do now.
			}
		}
	if (!IncomingNotifyCmdPendingReAddress().IsEmpty())
		{
		iTspReAddressingIncomingNotifyCommands = ETrue;
		ReAddressIncomingNotifyCommand();
		}
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	}

void CRemConServer::MrctspoDoIncomingCommandPermitted(TBool aIsPermitted)
	{
	LOG_FUNC;
	
	MrctspoDoIncomingCommandAddressed(aIsPermitted ? KErrNone : KErrAccessDenied);
	}

void CRemConServer::MrctspoDoIncomingNotifyPermitted(TBool aIsPermitted)
	{
	LOG_FUNC;
	
	if(aIsPermitted)
		{
		TClientInfo* clientInfo = TargetClientIdToClientInfo(IncomingNotifyCmdPendingAddress().First().Client());
		MrctspoDoIncomingNotifyAddressed(clientInfo, KErrNone);
		}
	else
		{
		MrctspoDoIncomingNotifyAddressed(NULL, KErrAccessDenied);
		}
	}

void CRemConServer::TspOutgoingCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf);
	ASSERT_DEBUG(iTspHandlingOutgoingCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting access to the TSP).

	// Work out whether the next command to deal with is awaiting (a) 
	// addressing or (b) permission.
	// The head item is at index 0. This function should only be called if the 
	// queue is not empty.
	CRemConMessage& msg = OutgoingCmdPendingTsp().First();
	CRemConControllerSession* const sess = ControllerSession(msg.SessionId());
	// The session should exist- if it closed after asking to send this 
	// message, then the message should have been removed from the outgoing 
	// pending TSP queue at that time.
	ASSERT_DEBUG(sess);
	iTspHandlingOutgoingCommand = ETrue;
	if ( msg.Addr().IsNull() )
		{
		// Null address means it's awaiting an address.
		// Check the array of outgoing addresses is ready for this new request 
		// on the TSP.
		ASSERT_DEBUG(iTspConnections.IsEmpty());
		ASSERT_DEBUG(iBearerManager);
		iTspIf->AddressOutgoingCommand(
				msg.InterfaceUid(),
				msg.OperationId(), 
				sess->ClientInfo(),
				iTspConnections,
				iBearerManager->BearerSecurityPolicies());	
		}
	else
		{
		// Non-null address means it's awaiting permission to send.
		sess->MrcmsoMessageSendOneOrMoreAttempt(msg, 1);
		iTspIf->PermitOutgoingCommand(
			msg.InterfaceUid(),
			msg.OperationId(), 
			sess->ClientInfo(),
			msg.Addr());
		}
	}

void CRemConServer::TspOutgoingNotifyCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf3);
	ASSERT_DEBUG(iTspHandlingOutgoingNotifyCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting access to the TSP).

	// Work out whether the next command to deal with is awaiting (a) 
	// addressing or (b) permission.
	// The head item is at index 0. This function should only be called if the 
	// queue is not empty.
	CRemConMessage& msg = OutgoingNotifyCmdPendingTsp().First();
	CRemConControllerSession* const sess = ControllerSession(msg.SessionId());
	// The session should exist- if it closed after asking to send this 
	// message, then the message should have been removed from the outgoing 
	// pending TSP queue at that time.
	ASSERT_DEBUG(sess);
	iTspHandlingOutgoingNotifyCommand = ETrue;
	if ( msg.Addr().IsNull() )
		{
		ASSERT_DEBUG(iBearerManager);

		iTspIf3->AddressOutgoingNotify(
				msg.InterfaceUid(),
				msg.OperationId(), 
				sess->ClientInfo(),
				iBearerManager->BearerSecurityPolicies());
		}
	else
		{
		// Non-null address means it's awaiting permission to send.
		// As this is a notify command, we don't need to adjust NumRemotes() or 
		// NumRemotesToTry() on the session.
		iTspIf3->PermitOutgoingNotifyCommand(
			msg.InterfaceUid(),
			msg.OperationId(), 
			sess->ClientInfo(),
			msg.Addr());
		}
	}

void CRemConServer::AddressIncomingCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf);
	ASSERT_DEBUG(iTspAddressingIncomingCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting addressing).
	
	// There are two possibilities here, either the bearer has already provided
	// us with an address, in which case we just provide the TSP the command
	// for information, and to give it the opportunity to reject the command,
	// or we don't have an address, in which case we ask the TSP for one
	
	// This function should only be called if we know this queue is not 
	// empty.
	CRemConMessage& msg = IncomingCmdPendingAddress().First();
	iTspIncomingCmdClients.Reset();
	iTspAddressingIncomingCommand = ETrue;
	
	if(msg.Client() == KNullClientId)
		{
		// Prepare the array of target process IDs for the TSP.
		iTargetClientsLock.Wait();
		const TUint count = iTargetClients.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			CRemConTargetClientProcess* const client = iTargetClients[ii];
			ASSERT_DEBUG(client);
			iTspIncomingCmdClients.AddLast(client->ClientInfo());
			}
		iTargetClientsLock.Signal();
		
		iTspIf->AddressIncomingCommand(
			msg.InterfaceUid(),
			msg.OperationId(),
			iTspIncomingCmdClients);
		}
	else
		{
		iTspIncomingCmdClients.AddLast(*TargetClientIdToClientInfo(msg.Client()));
		ASSERT_DEBUG(iTspIf4);
		iTspIf4->PermitIncomingCommand(
			msg.InterfaceUid(),
			msg.OperationId(),
			*iTspIncomingCmdClients.First());
		}
	}

void CRemConServer::AddressIncomingNotifyCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf2);
	ASSERT_DEBUG(iTspAddressingIncomingNotifyCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting addressing).
	
	// There are two possibilities here, either the bearer has already provided
	// us with an address, in which case we just provide the TSP the command
	// for information, and to give it the opportunity to reject the command,
	// or we don't have an address, in which case we ask the TSP for one
	
	// This function should only be called if we know this queue is not 
	// empty
	CRemConMessage& msg = IncomingNotifyCmdPendingAddress().First();
	iTspIncomingNotifyCmdClients.Reset();
	iTspAddressingIncomingNotifyCommand = ETrue;
	
	if(!FindDuplicateNotify(msg))
		{
		if(msg.Client() == KNullClientId)
			{
			// Prepare the array of target process IDs for the TSP.
			iTargetClientsLock.Wait();
			const TUint count = iTargetClients.Count();
			for ( TUint ii = 0 ; ii < count ; ++ii )
				{
				CRemConTargetClientProcess* const client = iTargetClients[ii];
				ASSERT_DEBUG(client);
				iTspIncomingNotifyCmdClients.AddLast(client->ClientInfo());
				}
			iTargetClientsLock.Signal();
			
			// Only send the notify to the TSP if there isn't an identical one on either incomingpendingdelivery or incomingdelivered
			iTspIf2->AddressIncomingNotify(
					msg.InterfaceUid(),
					msg.OperationId(),
					iTspIncomingNotifyCmdClients);
			}
		else
			{
			iTspIncomingNotifyCmdClients.AddLast(*TargetClientIdToClientInfo(msg.Client()));
			ASSERT_DEBUG(iTspIf4);
			iTspIf4->PermitIncomingNotify(
				msg.InterfaceUid(),
				msg.OperationId(),
				*iTspIncomingNotifyCmdClients.First());
			}
		}
	else
		{
		// Otherwise, we can call IncomingNotifyAddressed with NULL, and it will be rejected back to the bearer
		MrctspoDoIncomingNotifyAddressed(NULL, KErrArgument);
		}
	}

void CRemConServer::ReAddressIncomingNotifyCommand()
	{
	LOG_FUNC;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	ASSERT_DEBUG(iTspIf2);
	ASSERT_DEBUG(iTspReAddressingIncomingNotifyCommands);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting addressing).
	// Prepare the array of target process IDs for the TSP.
	iTspIncomingNotifyCmdClients.Reset();
	iTargetClientsLock.Wait();
	const TUint count = iTargetClients.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConTargetClientProcess* const client = iTargetClients[ii];
		ASSERT_DEBUG(client);
		iTspIncomingNotifyCmdClients.AddLast(client->ClientInfo());
		}
	iTargetClientsLock.Signal();
	
	// This function should only be called if we know this queue is not 
	// empty.
	CRemConMessage& msg = IncomingNotifyCmdPendingReAddress().First();
	iTspIf2->AddressIncomingNotify(
		msg.InterfaceUid(),
		msg.OperationId(),
		iTspIncomingNotifyCmdClients);
	}


void CRemConServer::PermitOutgoingResponse()
	{
	LOG_FUNC;
	LOGOUTGOINGRSPPENDINGTSP;
	
	ASSERT_DEBUG(iTspHandlingOutgoingResponse == EFalse);
	ASSERT_DEBUG(!OutgoingRspPendingTsp().IsEmpty());
	while (!iTspHandlingOutgoingResponse && !OutgoingRspPendingTsp().IsEmpty())
		{
		CRemConMessage& msg = OutgoingRspPendingTsp().First();
		CRemConTargetClientProcess* client = TargetClient(msg.SessionId());
		// The client should exist- if it closed after asking to send this 
		// message, then the message should have been removed from the outgoing 
		// pending TSP queue at that time.		
		ASSERT_DEBUG(client);
		ASSERT_DEBUG(iMessageRecipientsList);
		CMessageRecipients* message = iMessageRecipientsList->Message(msg.TransactionId());

		if (message) // If we aren't returned a client list, this means that the message has been delivered elsewhere
			{
			iTspHandlingOutgoingResponse = ETrue;
			if (iTspIf2)
				{
				iTspIf2->PermitOutgoingResponse(
						msg.InterfaceUid(),
						msg.OperationId(),
						client->ClientInfo(),
						message->ConstIter()
						);
				}
			else
				{
				OutgoingResponsePermitted(ETrue);
				}
			}
		else
			{
			client->MrcmsoMessageSendOneOrMoreAbandoned(msg);
			OutgoingRspPendingTsp().RemoveAndDestroy(msg);
			}
		}
	LOGOUTGOINGRSPPENDINGTSP;
	}

void CRemConServer::SendCmdToRemoteL(const CRemConMessage& aMsg, const TRemConAddress& aConn, TBool& aSync)
	{
	LOG_FUNC;

	ASSERT_DEBUG(  
			       (aMsg.MsgType() == ERemConCommand)
			    || (aMsg.MsgType() == ERemConNotifyCommand)
			    );


	// Populate aSync immediately in case the function leaves.
	aSync = EFalse;
	TConnectionState conState = ConnectionState(aConn);
	if ( conState == EConnected)
		{
		aSync = ETrue;
		}

	// Take a copy of the message and set the right address.
	CRemConMessage* newMsg = CRemConMessage::CopyL(aMsg);
	newMsg->Addr() = aConn;
	CleanupStack::PushL(newMsg);
	ASSERT_DEBUG(iBearerManager);

	// If we're connected (and not mid-connect/disconnect) we can send immediately,
	// otherwise we queue and try to bring up the connection.
	switch ( conState )
		{
		case EConnected:
			{
			// We have a connection to send over. Try to send it.
			LEAVEIFERRORL(iBearerManager->Send(*newMsg));
			// If Send worked, then the bearer has taken ownership of the 
			// message's data.
			newMsg->OperationData().Assign(NULL); 
			CLEANUPSTACK_POP1(newMsg);
			OutgoingSent().Append(*newMsg); 
			break;
			}
		case EDisconnected:
			{
			// No connection. Try to bring one up. If we can't then destroy the 
			// new message and leave.
			CLEANUPSTACK_POP1(newMsg);
			// We need to put the message on the queue before trying to connect 
			// because the connect confirmation might come back synchronously.
			OutgoingPendingSend().Append(*newMsg);
			TInt err = iBearerManager->Connect(newMsg->Addr());
			if ( err != KErrNone )
				{
				OutgoingPendingSend().RemoveAndDestroy(*newMsg);
				aSync = ETrue;
				LEAVEIFERRORL(err); // will destroy newMsg
				}
			break;
			}
		case EConnecting:
		case EDisconnecting:
			{
			// Just queue if connection is coming up or going down.
			//
			// If connection is coming up, the message will be
			// sent when ConnectConfirm fires.
			//
			// If connection is going down: When CBearerManager gets
			// the DisconnectConfirm it calls us back at RemoveConnection,
			// whereat we will reconnect the bearer that was mid-disconnect;
			// the message will be sent when that completes.
			CLEANUPSTACK_POP1(newMsg);
			OutgoingPendingSend().Append(*newMsg);
			break;
			}
		default:
			{
			DEBUG_PANIC_LINENUM;
			break;
			}
		}

	LOG1(_L("\taSync = %d"), aSync);
	}

void CRemConServer::DeliverCmdToClientL(const CRemConMessage& aMsg, CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;

	ASSERT_DEBUG((aMsg.MsgType() == ERemConCommand) || (aMsg.MsgType() == ERemConNotifyCommand)); 
	// Take a copy of the message and set the right client ID (important to 
	// set the selected client's ID because this is how we match up the 
	// client's response, if aMsg is a command). 
	CRemConMessage* const newMsg = CRemConMessage::CopyL(aMsg);
	newMsg->SessionId() = aClient.Id();
	LEAVEIFERRORL(DeliverMessageToClient(*newMsg, aClient));
	}

TInt CRemConServer::DeliverMessageToClient(CRemConMessage& aMsg, CRemConControllerSession& aSess)
	{
	LOG_FUNC;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Controller clients only receive responses or rejects
	ASSERT_DEBUG(aMsg.MsgType() == ERemConResponse || aMsg.MsgType() == ERemConReject);

	TInt err = KErrNone;

	// First off check if the client supports this
	if(!aSess.SupportedMessage(aMsg))
		{
		err = KErrArgument;
        
		// 'Take ownership' of it by destroying it- it's finished with.
		delete &aMsg;
		}
	else if ( aSess.CurrentReceiveMessage().Handle() )
		{
		err = aSess.WriteMessageToClient (aMsg);

		// 'Take ownership' of it by destroying it- it's finished with.			
		delete &aMsg;
		}
	else
		{
		IncomingPendingDelivery().Append(aMsg);
		}
	
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	return err;
	}

TInt CRemConServer::DeliverMessageToClient(CRemConMessage& aMsg, CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	TInt err = KErrNone;

	// Target clients only receive commands
	ASSERT_DEBUG(aMsg.MsgType() == ERemConCommand || aMsg.MsgType() == ERemConNotifyCommand);

	// Pass message to client
	err = aClient.ReceiveMessage(aMsg);

	if (err == KErrArgument)
		{
		// Message not supported.
		// 'Take ownership' of it by destroying it- it's finished with.
		delete &aMsg;
		}
	else if (err == KErrNotReady)
		{
		err = KErrNone;
		// Client cannot receive this message at the moment.
		IncomingPendingDelivery().Append(aMsg);	
		}
	else 
		{
		// If the message was delivered with no error, 
		// then put it in the 'incoming delivered' log. Otherwise, delete it 
		// because it's finished with.
		if (err == KErrNone)
			{
			// We'll need to remember it for the response coming back.
			IncomingDelivered().Append(aMsg); 
			}
		else
			{
			// 'Take ownership' of it by destroying it- it's finished with.
			delete &aMsg;
			}
		}
	
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	return err;
	}
														  
void CRemConServer::MrctspoDoIncomingCommandAddressed(TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	LOGINCOMINGCMDPENDINGADDRESS;
	LOGINCOMINGDELIVERED;

	// Send the command to the requested target client(s), and remove the 
	// addressed message from the iIncomingPendingAddress queue. Any error at 
	// any point should be ignored- just complete as much as we can. 
	// Then check iIncomingPendingAddress for more commands to give to the 
	// TSP. 

	ASSERT_DEBUG(iTspAddressingIncomingCommand);
	
	iTspAddressingIncomingCommand = EFalse;

	if (!iTspDropIncomingCommand)
		{
		// If we did not call CommandExpired on the first item in the queue,
		// we know that the queue is not empty because we put something on the 
		// queue to call AddressIncomingCommand, which results in one call to this 
		// function, which is the only place where messages are removed from the 
		// queue.
		CRemConMessage& msg = IncomingCmdPendingAddress().First();
		TBool cmdDelivered = EFalse;
	
		// If the TSP errored, can't complete to any clients.
		if ( aError == KErrNone )
			{
			TSglQueIter<TClientInfo> iter(iTspIncomingCmdClients);
			
			TClientInfo* procId;
			CMessageRecipients* messageRecipients = NULL;
			TBool canDeliver = ETrue;
			TRAPD(err, messageRecipients = CMessageRecipients::NewL());
			if (err != KErrNone)
				{
				// If we didn't manage to create the list of clients we're delivering to,
				// we shouldn't deliver the message to the clients
				canDeliver = EFalse;
				}
			else
				{
				messageRecipients->TransactionId() = msg.TransactionId();
				}
			if (canDeliver)
				{
				while ( ( procId = iter++ ) != NULL )
					{
					LOG1(_L("\t\tprocess ID %d"), static_cast<TUint>(procId->ProcessId()));
					// Get the corresponding client.
					CRemConTargetClientProcess* const client = TargetClient(procId->ProcessId());
					// NB The set of open clients may have changed while the request 
					// was out on the TSP. If the TSP indicates a client that has 
					// gone away, then ignore that client. 
					if ( client )
						{
						TInt err = KErrNone;
						TClientInfo* clientInfo = NULL;
						TRAP(err, clientInfo = new (ELeave) TClientInfo);
						if (err == KErrNone)
							{
							// If we didn't manage to create the TClientInfo, we shouldn't deliver to the client
							TRAP(err, DeliverCmdToClientL(msg, *client));
							}
						if (err == KErrNone)
							{
							cmdDelivered = ETrue;
							// Add to the delivered information queue
							clientInfo->ProcessId() = procId->ProcessId();
							clientInfo->SecureId() = procId->SecureId();
							messageRecipients->Clients().AddLast(*clientInfo);
							}
						else
							{
							delete clientInfo;
							}
						// If we couldn't deliver an instance of the command to a 
						// target, there's not much we can do. 
						}
					}
				if ( messageRecipients->Clients().IsEmpty())
					{
					delete messageRecipients;
					}
				else
					{
					ASSERT_DEBUG(iMessageRecipientsList);
					iMessageRecipientsList->Messages().AddLast (*messageRecipients);
					}
				}
			}
	
		if ( !cmdDelivered)
			{
			// The command wasn't delivered to any clients
			// Tell bearer it won't be getting a response		
			
			SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
			}
		// Destroy the message we've just dealt with.
		IncomingCmdPendingAddress().RemoveAndDestroy(msg);
		
		}

	iTspDropIncomingCommand = EFalse;
	
	if ( !IncomingCmdPendingAddress().IsEmpty() )
		{
		LOG(_L("\tmore incoming commands awaiting addressing..."));
		AddressIncomingCommand();
		}
	
	LOGINCOMINGCMDPENDINGADDRESS;
	LOGINCOMINGDELIVERED;
	}

TInt CRemConServer::MrctspoDoGetConnections(TSglQue<TRemConAddress>& aConnections)
	{
	LOG_FUNC;

	ASSERT_DEBUG(aConnections.IsEmpty());

	// Get an owned copy of the current set of connections in the system.
	CConnections* conns = NULL;
	TRAPD(err, conns = CConnections::CopyL(Connections()));
	LOG1(_L("\terr = %d"), err);
	if ( err == KErrNone )
		{
		// Pass these into aConnections, taking ownership of them.
		TSglQueIter<TRemConAddress>& iter = conns->SetToFirst();
		TRemConAddress* addr;
		while ( ( addr = iter++ ) != NULL )
			{
			conns->Remove(*addr);
			aConnections.AddLast(*addr);
			};
		delete conns;
		}
  
	return err;
	}

TInt CRemConServer::MrctspoSetLocalAddressedClient(const TUid& aBearerUid, const TClientInfo& aClientInfo)
	{
	LOG_FUNC;
	
	TRemConClientId id = KNullClientId;
	iTargetClientsLock.Wait();
	const TUint count = iTargetClients.Count();
	for(TUint i=0; i<count; i++)
		{
		CRemConTargetClientProcess* const client = iTargetClients[i];
		ASSERT_DEBUG(client);
		if(client->ClientInfo().ProcessId() == aClientInfo.ProcessId())
			{
			id = client->Id();
			break;
			}
		}
	iTargetClientsLock.Signal();
	
	if(id != KNullClientId)
		{
		ASSERT_DEBUG(iBearerManager);
		return iBearerManager->SetLocalAddressedClient(aBearerUid, id);
		}
	else
		{
		return KErrNotFound;
		}
	}

void CRemConServer::LoadTspL()
	{
	LOG_FUNC;

	// Instantiate one-and-only implementation of the target selector plugin 
	// interface.
	const TUid KUidTargetSelectorInterface = TUid::Uid(KRemConTargetSelectorInterfaceUid);
	RImplInfoPtrArray implementations;
	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidTargetSelectorInterface, 
		noResolverParams, 
		KRomOnlyResolverUid,
		implementations);
	CleanupResetAndDestroyPushL(implementations);
	LOG1(_L("\tnumber of implementations of target selector plugin interface: %d"), implementations.Count());
	// We use ASSERT_ALWAYS here because this assumption is a crucial 
	// licensee-facing one- it's not a case of simply 'run the tests on a 
	// debug build and fix it if it's broken'.
	ASSERT_ALWAYS( implementations.Count() == 1 );
	CImplementationInformation* impl = implementations[0];
	ASSERT_DEBUG(impl);
	LOG(_L("\tloading TSP with:"));
	LOG1(_L("\t\timplementation uid 0x%08x"), impl->ImplementationUid());
	LOG1(_L("\t\tversion number %d"), impl->Version());
	TBuf8<KMaxName> buf8;
	buf8.Copy(impl->DisplayName());
	LOG1(_L8("\t\tdisplay name \"%S\""), &buf8);
	LOG1(_L("\t\tROM only %d"), impl->RomOnly());
	LOG1(_L("\t\tROM based %d"), impl->RomBased());
	iTsp = CRemConTargetSelectorPlugin::NewL(impl->ImplementationUid(), *this);
	CleanupStack::PopAndDestroy(&implementations);

	iTspIf = reinterpret_cast<MRemConTargetSelectorPluginInterface*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface1))
		);
	
	iTspIf2 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV2*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface2))
		);
	
	iTspIf3 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV3*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface3))
		);
	
	iTspIf4 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV4*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface4))
		);
	
	iTspIf5 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV5*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface5))
		);
	
	// If the TSP doesn't implement the required interface, panic server 
	// startup.
	ASSERT_ALWAYS(iTspIf);
	
	// We always need a V4 interface to allow simpler handling by the 
	// bearers of bearer addressing so if we don't have a V4 interface
	// from the TSP itself use a stub object to implement default
	// behaviour.
	if(!iTspIf4)
		{
		iTspIf4 = &iTspIf4Stub;
		}
	}

void CRemConServer::SendCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	if (aMsg.MsgType() == ERemConCommand)
		{
		LOGOUTGOINGCMDPENDINGTSP;
		
		OutgoingCmdPendingTsp().Append(aMsg);
		if ( !iTspHandlingOutgoingCommand )
			{				  
			TspOutgoingCommand();
			}
		
		LOGOUTGOINGCMDPENDINGTSP;
		}
	else
		{
		// Check the command is a notify command
		ASSERT_DEBUG(aMsg.MsgType() == ERemConNotifyCommand);
		
		if(iTspIf3)
			{
			OutgoingNotifyCmdPendingTsp().Append(aMsg);
			if ( !iTspHandlingOutgoingNotifyCommand )
				{				  
				TspOutgoingNotifyCommand();
				}
			}
		else
			{
			CRemConControllerSession* const sess = ControllerSession(aMsg.SessionId());
			delete &aMsg;
			ASSERT_DEBUG(sess);
			sess->SendError() = KErrNotSupported;
			sess->CompleteSend();
			}
		}
	
	}

void CRemConServer::SendResponse(CRemConMessage& aMsg, CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOGINCOMINGDELIVERED;
	LOGOUTGOINGRSPPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;

	CRemConMessage* response = &aMsg;
	CRemConMessage* newResponse = NULL;
	
	// Find the first command ('John') in the iIncomingDelivered queue with 
	// the same session ID, interface UID and operation ID as the response 
	// we're sending, and send the response to the same address that John came 
	// from.
	TSglQueIter<CRemConMessage>& iter = IncomingDelivered().SetToFirst();
	CRemConMessage* msg;
	TBool found = EFalse;
	
	while ( ( msg = iter++ ) != NULL )
		{
		if (	msg->SessionId() == aClient.Id()
			&&	msg->InterfaceUid() == response->InterfaceUid()
			&&	msg->OperationId() == response->OperationId()
			&&  (
					(msg->MsgSubType() == ERemConMessageDefault && response->MsgSubType() == ERemConMessageDefault)
				||  (msg->MsgSubType() == ERemConNotifyCommandAwaitingInterim && response->MsgSubType() == ERemConNotifyResponseInterim)
				||  (msg->MsgSubType() == ERemConNotifyCommandAwaitingChanged && response->MsgSubType() == ERemConNotifyResponseChanged)
				)
			)
			{
			LOG1(_L("\tfound a matching item in the incoming delivered commands log: [0x%08x]"), msg);
			found = ETrue;

			// Set the right address and transaction id in the outgoing message
			response->Addr() = msg->Addr();
			response->TransactionId() = msg->TransactionId();
			
			if(msg->MsgType() == ERemConCommand)
				{
				// Notify client (this shall go to one remote)
				aClient.MrcmsoMessageSendOneOrMoreAttempt(*response, 1);
			
				// Check the normal command and response have the default subtype set
				ASSERT_DEBUG(msg->MsgSubType() == ERemConMessageDefault && response->MsgSubType() == ERemConMessageDefault);

				// Remove the item from the 'incoming delivered' queue now we've 
				// addressed a response using it.
				IncomingDelivered().RemoveAndDestroy(*msg);
				
				// At this point we need to ask the TSP if we can deliver the outgoing response,
				// if there is a TSP supporting the V2 interface. If there isn't, we should just deliver
				// the message to the bearer
				OutgoingRspPendingTsp().Append(*response);
				if (!iTspHandlingOutgoingResponse)
					{
					PermitOutgoingResponse();
					}
				}
			else
				{
				// Notify client (this may be a series of messages to remotes)
				aClient.MrcmsoMessageSendOneOrMoreIncremental(*msg, 1);

				// Check the command is a notify command
				ASSERT_DEBUG(msg->MsgType() == ERemConNotifyCommand);
				
				// Check the command has a valid subtype for a notify command
				ASSERT_DEBUG(msg->MsgSubType() == ERemConNotifyCommandAwaitingInterim || msg->MsgSubType() == ERemConNotifyCommandAwaitingChanged);
				
				// Check the response has a valid subtype for a notify response
				ASSERT_DEBUG(response->MsgSubType() == ERemConNotifyResponseInterim || response->MsgSubType() == ERemConNotifyResponseChanged);

				switch(msg->MsgSubType())
					{
				case ERemConNotifyCommandAwaitingChanged:
					IncomingDelivered().RemoveAndDestroy(*msg);
					
					// As this is a changed notification, there could be several notifications outstanding
					// that should all be completed with this message.
					// We therefore need to take a copy of the message, but the response should not be
					// completed back to the client. We therefore set the Session ID of the new meessage to 0
					
					TRAPD(err, newResponse = CRemConMessage::CopyL(*response));
					if (err != KErrNone)
						{
						newResponse = NULL;
						}
					break;
				case ERemConNotifyCommandAwaitingInterim:
					msg->MsgSubType() = ERemConNotifyCommandAwaitingChanged;
					break;
					}
				OutgoingRspPendingSend().Append(*response);
				response = newResponse;
				}
				if (!newResponse)
					{
					break;
					}
			}
		}
	
	if (newResponse)
		{
		delete newResponse;
		}


	TSglQueIter<CRemConMessage>& rspIter = OutgoingRspPendingSend().SetToFirst();
	while ((msg = rspIter++) != NULL)
		{
		OutgoingRspPendingSend().Remove(*msg);
		CompleteSendResponse(*msg, aClient);
		}

	
	// If the command was not found, then the app has sent a response to a
	// non-existant command.  It may do this in good intention if the server 
	// has transparently died and been restarted between the app's reception
	// of the command and it sending its response, so we can't panic it.
	// Just drop the message.
	if ( !found )
		{
		// Inform client that message should be completed with KErrNone We 
		// have done all we can with it.  Any other error may encourage 
		// retries from the application, which would be useless in this situation.
		aClient.MrcmsoMessageSendOneOrMoreAttemptFailed(aMsg, KErrNone);
		delete &aMsg;
		}
	
	LOGOUTGOINGRSPPENDINGTSP;
	LOGINCOMINGDELIVERED;
	LOGOUTGOINGPENDINGSEND;
	}

void CRemConServer::CompleteSendResponse(CRemConMessage& aMsg, CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOGOUTGOINGPENDINGSEND;
	// If the bearer-level connection exists, then send the message. 
	// Otherwise, queue the message and request the connection to come 
	// up. The message will be sent when ConnectConfirm or 
	// ConnectIndicate is called (assuming no error).
	ASSERT_DEBUG(aMsg.MsgType() == ERemConResponse);
	ASSERT_DEBUG(iBearerManager);
	// If we're connected (and not mid-connect/disconnect) we can send immediately,
	// otherwise we queue and try to bring up the connection.
	switch ( ConnectionState(aMsg.Addr()) )
		{
		case EConnected:
			{
			// We're already connected
			// If the bearer couldn't send, we need to error the client.
			TInt err = iBearerManager->Send(aMsg);
			
			// Inform client that message should be completed with err. 
			// Bearer-level error means the response got sent to zero remotes- 
			// bearer-level success means it got sent to precisely 1.
			aClient.MrcmsoMessageSendOneOrMoreResult(aMsg, err);

			// We've now finished with the response.
			delete &aMsg;
			break;
			}
		case EDisconnected:
			{
			// We're not connected. 
			// Queue the response...
			OutgoingPendingSend().Append(aMsg);
			// ... and ask the bearer to establish a connection. If we 
			// couldn't then complete the client's message and clean up.
			TInt err = iBearerManager->Connect(aMsg.Addr());
			if ( err != KErrNone )
				{
				aClient.MrcmsoMessageSendOneOrMoreResult(aMsg, err);
				OutgoingPendingSend().RemoveAndDestroy(aMsg);
				}
			break;
			}
		case EConnecting:
		case EDisconnecting:
			{
			// Just queue if connection is coming up or going down.
			//
			// If connection is coming up, the message will be
			// sent when ConnectConfirm fires.
			//
			// If connection is going down: When CBearerManager gets
			// the DisconnectConfirm it calls us back at RemoveConnection,
			// whereat we will reconnect the bearer that was mid-disconnect;
			// the message will be sent when that completes.
			OutgoingPendingSend().Append(aMsg);
			break;
			}
		default:
			{
			DEBUG_PANIC_LINENUM;
			break;
			}
		}
	LOGOUTGOINGPENDINGSEND;
	}


void CRemConServer::SendReject(TRemConAddress aAddr, TUid aInterfaceUid, TUint aOperationId, TUint aTransactionId)
	{
	LOG_FUNC;
	LOGOUTGOINGPENDINGSEND;
	// If the bearer-level connection exists, then send the message. 
	// Otherwise, queue the message and request the connection to come 
	// up. The message will be sent when ConnectConfirm or 
	// ConnectIndicate is called (assuming no error).
	
	CRemConMessage* rejectMsg = NULL;
	RBuf8 data;
	data = KNullDesC8;
	TRAPD(err, rejectMsg = CRemConMessage::NewL(aAddr, KNullClientId, ERemConReject, ERemConMessageDefault, aInterfaceUid, aOperationId, data, 0, aTransactionId));

	if ( err == KErrNone)
		{
		// SendReject will always take ownership of rejectMsg
		
		ASSERT_DEBUG(iBearerManager);
		switch ( ConnectionState(rejectMsg->Addr()) )
			{
			case EConnected:
				{
				// We're already connected
				err = iBearerManager->Send(*rejectMsg);
				// We've now finished with the response.
				delete rejectMsg;
				break;
				}
			case EDisconnected:
				{
				// We're not connected. 
				// Queue the response...
				OutgoingPendingSend().Append(*rejectMsg);
				// ... and ask the bearer to establish a connection. If we 
				// couldn't then clean up.
				err = iBearerManager->Connect (rejectMsg->Addr ());
				if ( err != KErrNone)
					{
					OutgoingPendingSend().RemoveAndDestroy(*rejectMsg);
					}
				break;
				}
			case EConnecting:
			case EDisconnecting:
				{
				// Just queue if connection is coming up or going down.
				//
				// If connection is coming up, the message will be
				// sent when ConnectConfirm fires.
				//
				// If connection is going down: When CBearerManager gets
				// the DisconnectConfirm it calls us back at RemoveConnection,
				// whereat we will reconnect the bearer that was mid-disconnect;
				// the message will be sent when that completes.
				OutgoingPendingSend().Append(*rejectMsg);
				break;
				}
			default:
				{
				DEBUG_PANIC_LINENUM;
				break;
				}
			}
		}
	LOGOUTGOINGPENDINGSEND;
	}

void CRemConServer::SendCancel(CRemConControllerSession& aSess)
	{
	LOG_FUNC;
	LOGOUTGOINGCMDPENDINGTSP;
	
	TSglQueIter<CRemConMessage>& iter = OutgoingCmdPendingTsp().SetToFirst();
	CRemConMessage* msg;
	TBool first = ETrue;
	while ( ( msg = iter++ ) != NULL )
		{
		// A session can only have one send outstanding at once, so there can 
		// only be one message on the queue belonging to it.
		if ( msg->SessionId() == aSess.Id() )
			{
			ASSERT_DEBUG(msg->MsgType() == ERemConCommand);
			LOG1(_L("\tfound a command belonging to this client in the outgoing pending TSP queue: [0x%08x]"), msg);

			// If the TSP is currently handling this command, we must stop 
			// them! The one the TSP is handling, if any, is always the first 
			// in the queue.
			if ( iTspHandlingOutgoingCommand && first )
				{
				LOG(_L("\tTSP is processing this command- cancel it"));
				ASSERT_DEBUG(iTspIf);
				iTspIf->CancelOutgoingCommand();
				iTspHandlingOutgoingCommand = EFalse;
				iTspConnections.Reset();
				}
			// Can now destroy the message.
			OutgoingCmdPendingTsp().RemoveAndDestroy(*msg);
			}
		first = EFalse;
		}

	LOGOUTGOINGCMDPENDINGTSP;
	}

void CRemConServer::NewResponse(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Look through the 'outgoing sent' queue for items with the same address, 
	// interface UID, operation ID and transaction ID. We assume there will 
	// only be one such item.
	TBool sentCommandFound = EFalse;
	TSglQueIter<CRemConMessage>& iter = OutgoingSent().SetToFirst();
	CRemConMessage* cmd;
	while ( ( cmd = iter++ ) != NULL )
		{
		if (	cmd->Addr() == aMsg.Addr()
			&&	cmd->InterfaceUid() == aMsg.InterfaceUid()
			&&	cmd->OperationId() == aMsg.OperationId()
			&&	cmd->TransactionId() == aMsg.TransactionId()
			)
			{
			LOG1(_L("\tfound a matching item in the sent commands log: [0x%08x]"), cmd);
			sentCommandFound = ETrue;
			CRemConControllerSession* const session = ControllerSession(cmd->SessionId());
			// When sessions close, their messages are removed from the logs, 
			// so the session here _should_ exist.
			ASSERT_DEBUG(session);
			aMsg.SessionId() = cmd->SessionId();
			(void) DeliverMessageToClient(aMsg, *session);
			// Remove the found item from the sent message log.
			OutgoingSent().RemoveAndDestroy(*cmd);
			break;
			}
		}
	if ( !sentCommandFound )
		{
		// Either:
		// (a) the remote is buggy (sent a response when there wasn't an 
		// originating command), 
		// (b) the client closed their controller session before the response 
		// came back (this cleans up the 'sent commands' log)
		LOG(_L("\tno matching item found in sent commands log- response dropped"));
		delete &aMsg;
		}

	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::NewNotifyResponse(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Look through the 'outgoing sent' queue for items with the same address, 
	// interface UID, operation ID and transaction ID. We assume there will 
	// only be one such item.
	TBool sentCommandFound = EFalse;
	TSglQueIter<CRemConMessage>& iter = OutgoingSent().SetToFirst();
	CRemConMessage* cmd;
	TRemConMessageSubType submessagetype;
	submessagetype = aMsg.MsgSubType();
	
	while ( ( cmd = iter++ ) != NULL )
		{
		if (	cmd->Addr() == aMsg.Addr()
			&&	cmd->InterfaceUid() == aMsg.InterfaceUid()
			&&	cmd->OperationId() == aMsg.OperationId()
			&&	cmd->TransactionId() == aMsg.TransactionId()
			)
			{
			LOG1(_L("\tfound a matching item in the sent commands log: [0x%08x]"), cmd);
			sentCommandFound = ETrue;
			CRemConControllerSession* const session = ControllerSession(cmd->SessionId());
			// When sessions close, their messages are removed from the logs, 
			// so the session here _should_ exist.
			ASSERT_DEBUG(session);
			aMsg.SessionId() = cmd->SessionId();			
			(void) DeliverMessageToClient(aMsg, *session);
			
			//Do not remove the command if it is an interim response because
			//we need to wait for the changed response.
			//If the changed response received or error occurs, then remove
			//the command from the sent message log
			if (submessagetype != ERemConNotifyResponseInterim)
				{
				OutgoingSent().RemoveAndDestroy(*cmd);
				}

			break;
			}
		}
	if ( !sentCommandFound )
		{
		// Either:
		// (a) the remote is buggy (sent a response when there wasn't an 
		// originating command), 
		// (b) the client closed their controller session before the response 
		// came back (this cleans up the 'sent commands' log)
		LOG(_L("\tno matching item found in sent commands log- response dropped"));
		delete &aMsg;
		}

	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::NewCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGINCOMINGCMDPENDINGADDRESS;

	IncomingCmdPendingAddress().Append(aMsg);
	if ( !iTspAddressingIncomingCommand)
		{
		AddressIncomingCommand();
		}

	LOGINCOMINGCMDPENDINGADDRESS;
	}

void CRemConServer::NewNotifyCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	if (!iTspIf2)
		{
		SendReject(aMsg.Addr(), aMsg.InterfaceUid(), aMsg.OperationId(), aMsg.TransactionId());
		delete &aMsg;
		}
	else
		{
		IncomingNotifyCmdPendingAddress().Append(aMsg);
		if ( !iTspAddressingIncomingNotifyCommand && !iTspReAddressingIncomingNotifyCommands)
			{
			AddressIncomingNotifyCommand();
			}
		}

	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	}

#ifdef __FLOG_ACTIVE
void CRemConServer::LogOutgoingNotifyCmdPendingTsp() const
	{
	LOG(_L("Logging outgoing notify pending TSP commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingNotifyCmdPendingTsp().LogQueue();)
	}

void CRemConServer::LogOutgoingCmdPendingTsp() const
	{
	LOG(_L("Logging outgoing pending TSP commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingCmdPendingTsp().LogQueue();)
	}

void CRemConServer::LogOutgoingRspPendingTsp() const
	{
	LOG(_L("Logging outgoing pending TSP commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingRspPendingTsp().LogQueue();)
	}


void CRemConServer::LogOutgoingPendingSend() const
	{
	LOG(_L("Logging outgoing pending send commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingPendingSend().LogQueue();)
	}

void CRemConServer::LogOutgoingSent() const
	{
	LOG(_L("Logging outgoing sent commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingSent().LogQueue();)
	}

void CRemConServer::LogIncomingCmdPendingAddress() const
	{
	LOG(_L("Logging incoming pending address commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingCmdPendingAddress().LogQueue();)
	}

void CRemConServer::LogIncomingNotifyCmdPendingAddress() const
	{
	LOG(_L("Logging incoming pending address commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingNotifyCmdPendingAddress().LogQueue();)
	}

void CRemConServer::LogIncomingNotifyCmdPendingReAddress() const
	{
	LOG(_L("Logging incoming pending address commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingNotifyCmdPendingReAddress().LogQueue();)
	}

void CRemConServer::LogIncomingPendingDelivery() const
	{
	LOG(_L("Logging incoming pending delivery messages"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingPendingDelivery().LogQueue();)
	}

void CRemConServer::LogIncomingDelivered() const
	{
	LOG(_L("Logging incoming delivered commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingDelivered().LogQueue();)
	}
#endif // __FLOG_ACTIVE

CRemConControllerSession* CRemConServer::ControllerSession(TUint aSessionId) const
	{
	LOG_FUNC;

	CRemConControllerSession* sess = NULL;
	
	TInt index = iControllerSessions.Find(aSessionId, ControllerSessionCompare);
	
	if(index >= 0)
		{
		sess = iControllerSessions[index];
		}

	return sess;
	}

CRemConTargetClientProcess* CRemConServer::TargetClient(TUint aClientId) const
	{
	LOG_FUNC;

	CRemConTargetClientProcess* client = NULL;

	iTargetClientsLock.Wait();
	
	TInt index = iTargetClients.Find(aClientId, TargetClientCompareUsingSessionId);
	
	if(index >= 0)
		{
		client = iTargetClients[index];
		}

	iTargetClientsLock.Signal();

	return client;
	}

CRemConTargetClientProcess* CRemConServer::TargetClient(TProcessId aProcessId) const
	{
	LOG_FUNC;

	CRemConTargetClientProcess* client = NULL;

	iTargetClientsLock.Wait();
	
	TInt index = iTargetClients.Find(aProcessId, TargetClientCompareUsingProcessId);
	
	if(index >= 0)
		{
		client = iTargetClients[index];
		}
	
	iTargetClientsLock.Signal();

	return client;
	}

MRemConConverterInterface* CRemConServer::Converter(TUid aInterfaceUid, 
							   TUid aBearerUid) const
	{
	ASSERT_DEBUG(iConverterManager);
	return iConverterManager->Converter(aInterfaceUid, aBearerUid);
	}

MRemConConverterInterface* CRemConServer::Converter(const TDesC8& aInterfaceData, 
							   TUid aBearerUid) const
	{
	ASSERT_DEBUG(iConverterManager);
	return iConverterManager->Converter(aInterfaceData, aBearerUid);
	}

void CRemConServer::ReceiveRequest(CRemConControllerSession& aSession)
	{
	LOG_FUNC;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Find the first message in IncomingPendingDelivery for this session.
	TSglQueIter<CRemConMessage>& iter = IncomingPendingDelivery().SetToFirst();
	CRemConMessage* msg;
	while ( ( msg = iter++ ) != NULL )
		{
		if ( msg->SessionId() == aSession.Id() )
			{
			// Controllers receive responses or rejects only.
			ASSERT_DEBUG(msg->MsgType() == ERemConResponse || msg->MsgType() == ERemConReject);

			TInt err = aSession.WriteMessageToClient(*msg);
			IncomingPendingDelivery().Remove(*msg);
			
			// 'Take ownership' of it by destroying it- it's finished with.
			delete msg;				
			
			break;
			}
		}

	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::ReceiveRequest(CRemConTargetClientProcess& aClient)
	{
	LOG_FUNC;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Messages are addressed to the client. Ask client to deliver any pending
	// messages. For each delivered message, update ourselves accordingly.
	
	// Find the first message in IncomingPendingDelivery for this session.
	TSglQueIter<CRemConMessage>& iter = IncomingPendingDelivery().SetToFirst();
	CRemConMessage* msg;
	while ( ( msg = iter++ ) != NULL )
		{
		if (msg->SessionId() == aClient.Id())
			{
			// Targets receive commands only.
			ASSERT_DEBUG(msg->MsgType() == ERemConCommand || msg->MsgType() == ERemConNotifyCommand);

			TInt err = aClient.ReceiveMessage(*msg);

			if (err == KErrArgument)
				{
				// Message not supported by this client.
				// 'Take ownership' of it by destroying it- it's finished with.
				IncomingPendingDelivery().Remove(*msg);
				delete msg;
				}
			else if (err == KErrNotReady)
				{
				// Client cannot receive this message at the moment, skip for now
				// (message is already on the pemding queue).
				}
			else 
				{
				// Message delivered, remove from pending queue.
				IncomingPendingDelivery().Remove(*msg);
				
				if (err == KErrNone )
					{
					// We'll need to remember it for the response coming back.
					IncomingDelivered().Append(*msg); 
					}
				else
					{
					// Tell bearer it won't be getting a response
					CMessageRecipients* messageRecipients = iMessageRecipientsList->Message (msg->TransactionId ());

					// If we aren't returned a client list, this means that the message has been delivered elsewhere
					if (messageRecipients)
						{
						messageRecipients->RemoveAndDestroyClient (aClient.ClientInfo ());

						if ( messageRecipients->Clients().IsEmpty ())
							{
							iMessageRecipientsList->Messages().Remove (*messageRecipients);
							delete messageRecipients;

							SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
							}
						}

					// 'Take ownership' of it by destroying it- it's finished with.
					delete msg;
					}
				}
			}
		}
	
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

TBool CRemConServer::FindDuplicateNotify(CRemConMessage& aMsg)
	{
	ASSERT_DEBUG(aMsg.MsgType() == ERemConNotifyCommand);
	TSglQueIter<CRemConMessage>& deliveredIter = IncomingDelivered().SetToFirst();
	CRemConMessage* msg;
	while ((msg = deliveredIter++) != NULL)
		{
		if (msg->MsgType() == ERemConNotifyCommand
				&& msg->Addr() == aMsg.Addr()
				&& msg->InterfaceUid() == aMsg.InterfaceUid()
				&& msg->OperationId() == aMsg.OperationId()
				&& msg->Client() == aMsg.Client())
			{
			return ETrue;
			}
		}
	TSglQueIter<CRemConMessage>& pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
	while ((msg = pendingDeliveryIter++) != NULL)
		{
		if (msg->MsgType() == ERemConNotifyCommand
				&& msg->Addr() == aMsg.Addr()
				&& msg->InterfaceUid() == aMsg.InterfaceUid()
				&& msg->OperationId() == aMsg.OperationId()
				&& msg->Client() == aMsg.Client())
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt UidCompare(const TUid& aFirst, const TUid& aSecond)
	{
	if(aFirst.iUid < aSecond.iUid)
		{
		return -1;
		}
	else if(aFirst.iUid == aSecond.iUid)
		{
		return 0;
		}
	else
		{
		return 1;
		}
	}

/**
Collect all supported interfaces of controller clients.  

@param aSupportedInterfaces An empty RArray which will be populated with the current
		supported interfaces.  Ownership is retained by the caller.
@return KErrNone if any interfaces were able to be retrieved.  This does not 
		imply that every session's interfaces were able to be retrieved.
		KErrNoMemory if no interfaces could be retrived.
*/
TInt CRemConServer::ControllerSupportedInterfaces(RArray<TUid>& aSupportedInterfaces)
	{
	LOG_FUNC;
	ASSERT_DEBUG(aSupportedInterfaces.Count() == 0);
	
	TLinearOrder<TUid> uidCompare(&UidCompare);
	RArray<TUid> sessionFeatures;
	TInt err = KErrNone;
	for(TInt i=0; i<iControllerSessions.Count(); i++)
		{
		ASSERT_DEBUG(iControllerSessions[i]);
		err = iControllerSessions[i]->SupportedInterfaces(sessionFeatures);
		ASSERT_DEBUG(err == KErrNone || err == KErrNoMemory);
		
		if(!err)
			{
			for(TInt j=0; j<sessionFeatures.Count(); j++)
				{
				// Ignore failure here, we're trying this best effort
				// InsertInOrder is used rather than just bunging the
				// interface on the end as we want no duplicates 
				(void)aSupportedInterfaces.InsertInOrder(sessionFeatures[j], uidCompare);
				}
			}
		sessionFeatures.Reset();
		}
	
	if(aSupportedInterfaces.Count() > 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrNoMemory;
		}
	}

CMessageQueue& CRemConServer::OutgoingCmdPendingTsp()
	{
	ASSERT_DEBUG(iOutgoingCmdPendingTsp);
	return *iOutgoingCmdPendingTsp;
	}

CMessageQueue& CRemConServer::OutgoingNotifyCmdPendingTsp()
	{
	ASSERT_DEBUG(iOutgoingNotifyCmdPendingTsp);
	return *iOutgoingNotifyCmdPendingTsp;
	}

CMessageQueue& CRemConServer::OutgoingRspPendingTsp()
	{
	ASSERT_DEBUG(iOutgoingRspPendingTsp);
	return *iOutgoingRspPendingTsp;
	}

CMessageQueue& CRemConServer::OutgoingRspPendingSend()
	{
	ASSERT_DEBUG(iOutgoingRspPendingSend);
	return *iOutgoingRspPendingSend;
	}

CMessageQueue& CRemConServer::OutgoingPendingSend()
	{
	ASSERT_DEBUG(iOutgoingPendingSend);
	return *iOutgoingPendingSend;
	}

CMessageQueue& CRemConServer::OutgoingSent()
	{
	ASSERT_DEBUG(iOutgoingSent);
	return *iOutgoingSent;
	}

CMessageQueue& CRemConServer::IncomingCmdPendingAddress()
	{
	ASSERT_DEBUG(iIncomingCmdPendingAddress);
	return *iIncomingCmdPendingAddress;
	}

CMessageQueue& CRemConServer::IncomingNotifyCmdPendingAddress()
	{
	ASSERT_DEBUG(iIncomingNotifyCmdPendingAddress);
	return *iIncomingNotifyCmdPendingAddress;
	}

CMessageQueue& CRemConServer::IncomingPendingDelivery()
	{
	ASSERT_DEBUG(iIncomingPendingDelivery);
	return *iIncomingPendingDelivery;
	}

CMessageQueue& CRemConServer::IncomingDelivered()
	{
	ASSERT_DEBUG(iIncomingDelivered);
	return *iIncomingDelivered;
	}


CMessageQueue& CRemConServer::IncomingNotifyCmdPendingReAddress()
	{
	ASSERT_DEBUG(iIncomingNotifyCmdPendingReAddress);
	return *iIncomingNotifyCmdPendingReAddress;
	}

TBool CRemConServer::ConnectionHistoryPointerAtLatest(TUint aSessionId) const
	{
	LOG_FUNC;
	LOG1(_L("\taSessionId = %d"), aSessionId);

#ifdef _DEBUG
	TBool found = EFalse;
#endif
	TUint index = 0;
	const TUint count = iSession2ConnHistory.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iSession2ConnHistory[ii].iSessionId == aSessionId )
			{
			index = iSession2ConnHistory[ii].iIndex;
#ifdef _DEBUG
			found = ETrue;
#endif
			break;
			}
		}
	ASSERT_DEBUG(found);

	TBool ret = EFalse;
	ASSERT_DEBUG(iConnectionHistory);
	if ( index == iConnectionHistory->Count() - 1 )
		{
		ret = ETrue;
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

CConnections& CRemConServer::Connections()
	{
	ASSERT_DEBUG(iConnectionHistory);
	return iConnectionHistory->Last();
	}

TInt CRemConServer::HandleConnection(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());
	LOGREMOTES;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	// Try to update the connection history. If this fails (it involves memory 
	// allocation) we need to return the error so the connection will be 
	// dropped.
	if ( aError == KErrNone )
		{
		ASSERT_DEBUG(iConnectionHistory);
		aError = iConnectionHistory->NewConnection(aAddr);
		}

	// If we have a real new connection and we could handle it, aError is 
	// now KErrNone. In this case, sessions' notifications need completing.
	if ( aError == KErrNone )
		{
		TUint count = iControllerSessions.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			ASSERT_DEBUG(iControllerSessions[ii]);
			iControllerSessions[ii]->ConnectionsChanged();
			}
		
		iTargetClientsLock.Wait();
		count = iTargetClients.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			ASSERT_DEBUG(iTargetClients[ii]);
			iTargetClients[ii]->ConnectionsChanged();
			}
		iTargetClientsLock.Signal();
		}

	// Complete the specific client request(s) that caused a ConnectRequest on 
	// the bearer. Tell all sessions- they remember the address they wanted to 
	// connect to, and will filter on the address we give them. NB This 
	// function is called by ConnectIndicate as well as by ConnectConfirm, but 
	// the client doesn't care which end brought the connection up. 
	const TUint count = iControllerSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		ASSERT_DEBUG(iControllerSessions[ii]);
		iControllerSessions[ii]->CompleteConnect(aAddr, aError);
		}

	// Any messages waiting on OutgoingPendingSend for this connection need to 
	// be handled.
	TSglQueIter<CRemConMessage>& iter = OutgoingPendingSend().SetToFirst();
	CRemConMessage* msg;
	TBool moveToSent = EFalse;
	while ( ( msg = iter++ ) != NULL )
		{
		if ( msg->Addr() == aAddr )
			{
			MRemConMessageSendObserver* observer  = TargetClient(msg->SessionId());
			if(!observer)
				{
				observer = ControllerSession(msg->SessionId());
				}
#ifdef __DEBUG
			else
				{
				// Message has matched to a target session, so it should not also match
				// a controller session (we know the vice-versa is already true).
				ASSERT_DEBUG(!ControllerSession(msg->SessionId()));
				}
#endif

			// The session or client should exist- if it doesn't then this message 
			// wasn't cleaned from OutgoingPendingSend correctly when the 
			// session closed. The exceptions are Reject, which can be put
			// on the queue without a session, and notify changed responses when they are being
			// delivered to multiple controllers
			ASSERT_DEBUG(observer || msg->MsgType() == ERemConReject || msg->MsgSubType() == ERemConNotifyResponseChanged);

			if ( aError == KErrNone)
				{
				// We have a connection!
				ASSERT_DEBUG(iBearerManager);
				TInt err = iBearerManager->Send(*msg);
				if ( err == KErrNone 
				    && ((msg->MsgType() == ERemConCommand)||(msg->MsgType() == ERemConNotifyCommand)))
					{
					// If the send succeeded and it was a command, we move the 
					// message to the 'sent' log. Otherwise, it's simply 
					// deleted because we've finished with it.
					moveToSent = ETrue;
					}
				if ( observer && msg->MsgType() != ERemConReject )
					{
					observer->MrcmsoMessageSendOneOrMoreResult(*msg, err);
					}
				}
			else
				{
				// No connection, remember the error.
				if ( observer )
					{
					if (msg->MsgType() != ERemConReject)
						{
						observer->MrcmsoMessageSendOneOrMoreResult(*msg, aError);
						}
					}
				}

			if ( moveToSent)
				{
				OutgoingPendingSend().Remove (*msg);
				OutgoingSent().Append (*msg);
				}
			else
				{
				OutgoingPendingSend().RemoveAndDestroy (*msg);
				}
			}
		}

	LOGREMOTES;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	LOG1(_L("\taError = %d"), aError);
	return aError;
	}

void CRemConServer::RemoveConnection(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	LOGREMOTES;
	
	if(aError == KErrNone)
		{
		// The connection has gone away

		// We make a new item in the connection history and inform the sessions so 
		// they can complete outstanding connection status notifications.
	
		ASSERT_DEBUG(iConnectionHistory);
		iConnectionHistory->Disconnection(aAddr);

		TUint count = iControllerSessions.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			ASSERT_DEBUG(iControllerSessions[ii]);
			iControllerSessions[ii]->ConnectionsChanged();
			}
		
		iTargetClientsLock.Wait();
		count = iTargetClients.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			ASSERT_DEBUG(iTargetClients[ii]);
			iTargetClients[ii]->ConnectionsChanged();
			}
		iTargetClientsLock.Signal();
		
		// If there are any messages waiting on OutgoingPendingSend for this connection,
		// we re-connect it - they'll be picked up in HandleConnection above.
	
		TSglQueIter<CRemConMessage> iter = OutgoingPendingSend().SetToFirst();
		CRemConMessage* msg;
		TBool needToReconnect = false;
		while ( ( msg = iter++ ) != NULL )
			{
			if (msg->Addr() == aAddr)
				{
				needToReconnect = true;
				break;
				}
			}
		
		if (needToReconnect)
			{
			ASSERT_DEBUG(iBearerManager);
			TInt err = iBearerManager->Connect(aAddr);
			if ( err != KErrNone )
				{
				// This fails if:
				// 1. we're already connecting (in which case, we don't care)
				// 2. we can't add aAddr to the connecting list
				// The semantics of this observer don't let us return an error or leave, so
				// we can't do much about it here. Log it, and the next command will
				// invoke Connect from a better situation.
				LOG1(_L("\tFailed to re-connect bearer after connection removed: %d"), err);
				}
			}
		}
	
	// Complete the specific request(s) that caused a DisconnectRequest on the 
	// bearer. Tell all sessions- they remember the address they wanted to 
	// connect to, and will filter on the address we give them.
	TInt count = iControllerSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		ASSERT_DEBUG(iControllerSessions[ii]);
		iControllerSessions[ii]->CompleteDisconnect(aAddr, aError);
		}

	LOGREMOTES;
	}

void CRemConServer::SetConnectionHistoryPointer(TUint aSessionId)
	{
	LOG_FUNC;
	LOG1(_L("\taSessionId = %d"), aSessionId);
	LOGCONTROLLERSESSIONS;
	LOGTARGETSESSIONS;
	LOGCONNECTIONHISTORYANDINTEREST;

	// Update the record for this session.
	const TUint count = iSession2ConnHistory.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iSession2ConnHistory[ii].iSessionId == aSessionId )
			{
			ASSERT_DEBUG(iConnectionHistory);
			iSession2ConnHistory[ii].iIndex = iConnectionHistory->Count() - 1;
			break;
			}
		}

	// If the calling session was the last session pointing to that item in 
	// the history, and if it was the earliest item in the history, then we'll 
	// be able to clean up the history a bit.
	UpdateConnectionHistoryAndPointers();

	LOGCONNECTIONHISTORYANDINTEREST;
	}

const CConnections& CRemConServer::Connections(TUint aSessionId) const
	{
	LOG_FUNC;
	LOG1(_L("\taSessionId = %d"), aSessionId);

	// Get the connection history record for this session.
	const CConnections* conns = NULL; 
	const TUint count = iSession2ConnHistory.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iSession2ConnHistory[ii].iSessionId == aSessionId )
			{
			ASSERT_DEBUG(iConnectionHistory);
			conns = &(*iConnectionHistory)[iSession2ConnHistory[ii].iIndex];
			break;
			}
		}

	ASSERT_DEBUG(conns);

	return *conns;
	}

void CRemConServer::UpdateConnectionHistoryAndPointers()
	{
	LOG_FUNC;
	LOGCONNECTIONHISTORYANDINTEREST;
	
	// This function is called whenever a session finishes its interest in a 
	// connection history record, either by closing or by completing 
	// GetConnections. We remove uninteresting records in the history by 
	// removing the lowest-indexed item in the history until the 
	// lowest-indexed item has a session interested in it. As we do so, adjust 
	// the other sessions' pointers so they're still pointing at the right 
	// records.
	TUint lowestInterestingRecord = KMaxTUint;
	const TUint count = iSession2ConnHistory.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iSession2ConnHistory[ii].iIndex < lowestInterestingRecord )
			{
			lowestInterestingRecord = iSession2ConnHistory[ii].iIndex;
			}
		}

	// In theory, lowestInterestingRecord is now the number of connection 
	// history records we have to delete, starting with the 0th. This will not 
	// be the case (lowestInterestingRecord will still be KMaxTUint) if there 
	// are no sessions left. So adjust lowestInterestingRecord down to 
	// iConnectionHistory->Count() - 1 so that we remove all but the 'current' 
	// connection history record. This cleans up as much as possible in case 
	// server termination is interrupted.
	ASSERT_DEBUG(iConnectionHistory);
	if ( lowestInterestingRecord >= iConnectionHistory->Count() )
		{
		lowestInterestingRecord = iConnectionHistory->Count() - 1;
		}
	ASSERT_DEBUG(iConnectionHistory);
	for ( TUint ii = 0 ; ii < lowestInterestingRecord ; ++ii )
		{
		iConnectionHistory->DestroyFirst();
		}

	// We now have to go through iSession2ConnHistory and decrement each 
	// iIndex by lowestInterestingRecord, to keep _those_ records pointing 
	// at the right history records.
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		iSession2ConnHistory[ii].iIndex -= lowestInterestingRecord;
		}

	LOGCONNECTIONHISTORYANDINTEREST;
	}

TConnectionState CRemConServer::ConnectionState(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LOG1(_L("\taaAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	
	TConnectionState ret;

	// Because 'connection state' knowledge is spread across the system
	// we have asserts in following if statement to ensure that our state
	// is consistent

	// Check if connecting
	ASSERT_DEBUG(iBearerManager);
	if ( iBearerManager->IsConnecting(aAddr) )
		{
		ASSERT_DEBUG(!iBearerManager->IsDisconnecting(aAddr));
		ASSERT_DEBUG(!Connections().Find(aAddr)); 
		ret = EConnecting;
		}
	// Check if disconnecting
	else if ( iBearerManager->IsDisconnecting(aAddr) )
		{
		ASSERT_DEBUG(!iBearerManager->IsConnecting(aAddr));
		// NB Connection remains in connections list until we get DisconnectConfirm
		ASSERT_DEBUG(Connections().Find(aAddr));
		ret = EDisconnecting;	
		}
	// Check if connected
	else if ( Connections().Find(aAddr) )
		{
		ASSERT_DEBUG(!iBearerManager->IsConnecting(aAddr));
		ASSERT_DEBUG(!iBearerManager->IsDisconnecting(aAddr));
		ret = EConnected;
		}
	// otherwise it's disconnected
	else
		{
		ret = EDisconnected;
		}
		
	LOG1(_L("\tret(connection state) = %d"), ret);
	return ret;
	}

void CRemConServer::CommandExpired(TUint aTransactionId)
	{
	LOG_FUNC;

	CRemConMessage* msg;

	TBool first = ETrue;
	
	TSglQueIter<CRemConMessage> addressCommandIter = IncomingCmdPendingAddress().SetToFirst();
	
	while ((msg = addressCommandIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingCmdPendingAddress().RemoveAndDestroy(*msg);
			if (first && iTspAddressingIncomingCommand)
				{
				iTspDropIncomingCommand = ETrue;
				}
			}
		first = EFalse;
		}
	
	TSglQueIter<CRemConMessage> addressNotifyIter = IncomingNotifyCmdPendingAddress().SetToFirst();

	first = ETrue;
	
	while ((msg = addressNotifyIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingNotifyCmdPendingAddress().RemoveAndDestroy(*msg);
			if (first && iTspAddressingIncomingNotifyCommand)
				{
				iTspDropIncomingNotifyCommand = ETrue;
				}
			}
		first = EFalse;
		}

	TSglQueIter<CRemConMessage> reAddressNotifyIter = IncomingNotifyCmdPendingReAddress().SetToFirst();

	
	while ((msg = reAddressNotifyIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingNotifyCmdPendingReAddress().RemoveAndDestroy(*msg);
			if (first && iTspReAddressingIncomingNotifyCommands)
				{
				iTspDropIncomingNotifyCommand = ETrue;
				}

			}
		first = EFalse;
		}
	
	TSglQueIter<CRemConMessage> pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
	
	while ((msg = pendingDeliveryIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingPendingDelivery().RemoveAndDestroy(*msg);
			}
		}
	
	TSglQueIter<CRemConMessage> deliveredIter = IncomingDelivered().SetToFirst();
	
	while ((msg = deliveredIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingDelivered().RemoveAndDestroy(*msg);
			}
		}
	ASSERT_DEBUG(iMessageRecipientsList);
	iMessageRecipientsList->RemoveAndDestroyMessage(aTransactionId);
	}

TClientInfo* CRemConServer::TargetClientIdToClientInfo(TRemConClientId aId)
	{
	TClientInfo* clientInfo = NULL;
	
	iTargetClientsLock.Wait();
	const TUint count = iTargetClients.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConTargetClientProcess * const client = iTargetClients[ii];
		ASSERT_DEBUG(client);
		if (client->Id() == aId)
			{
			clientInfo = &client->ClientInfo();
			break;
			}
		}
	iTargetClientsLock.Signal();
	
	return clientInfo;
	}

TInt CRemConServer::SupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids)
	{
	TUint count = iControllerSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const sess = iControllerSessions[ii];
		ASSERT_DEBUG(sess);
		if (sess->Id() == aId)
			{
			return sess->SupportedInterfaces(aUids);
			}
		}
	
	iTargetClientsLock.Wait();
	count = iTargetClients.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConTargetClientProcess* const client = iTargetClients[ii];
		ASSERT_DEBUG(client);
		if (client->Id() == aId)
			{
			iTargetClientsLock.Signal();
			return client->SupportedInterfaces(aUids);
			}
		}
	iTargetClientsLock.Signal();
	
	return KErrNotFound;
	}

TInt CRemConServer::SupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	iTargetClientsLock.Wait();
	TUint count = iTargetClients.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConTargetClientProcess* const client = iTargetClients[ii];
		ASSERT_DEBUG(client);
		if (client->Id() == aId)
			{
			iTargetClientsLock.Signal();
			return client->SupportedOperations(aInterfaceUid, aOperations);
			}
		}
	iTargetClientsLock.Signal();

	count = iControllerSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConControllerSession* const sess = iControllerSessions[ii];
		ASSERT_DEBUG(sess);
		if (sess->Id() == aId)
			{
			return sess->SupportedOperations(aInterfaceUid, aOperations);
			}
		}
	
	return KErrNotFound;
	}

void CRemConServer::SetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId)
	{
	LOG_FUNC;
	
	TClientInfo* clientInfo = TargetClientIdToClientInfo(aId);
	// Bearer must supply valid client id
	ASSERT_DEBUG(clientInfo);

	ASSERT_DEBUG(iTspIf4);
	iTspIf4->SetRemoteAddressedClient(aBearerUid, *clientInfo);
	}

TInt CRemConServer::RegisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return iTspIf5 ? iTspIf5->RegisterLocalAddressedClientObserver(aBearerUid) : KErrNotSupported;
	}

TInt CRemConServer::UnregisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return iTspIf5 ? iTspIf5->UnregisterLocalAddressedClientObserver(aBearerUid) : KErrNotSupported;
	}

TRemConClientId CRemConServer::ClientIdByProcessId(TProcessId aProcessId)
	{
	LOG_FUNC;
	TRemConClientId ret = KNullClientId;
	iTargetClientsLock.Wait();
	CRemConTargetClientProcess* client = TargetClient(aProcessId);
	if(client)
		{
		ret = client->Id();
		}
	iTargetClientsLock.Signal();
	return ret;
	}

void CRemConServer::BulkInterfacesForClientL(TRemConClientId aId, RArray<TUid>& aUids)
	{
	LOG_FUNC;
	iTargetClientsLock.Wait();
	CleanupSignalPushL(iTargetClientsLock);
	CRemConTargetClientProcess* client = TargetClient(aId);
	if(!client)
		{
		LEAVEL(KErrNotFound);
		}
	LEAVEIFERRORL(client->SupportedBulkInterfaces(aUids));
	CleanupStack::PopAndDestroy(&iTargetClientsLock);
	}


// Helper Active Objects

CBulkThreadWatcher::CBulkThreadWatcher(CRemConServer& aServer)
	: CActive(CActive::EPriorityStandard)
	, iServer(aServer)
	{
	LOG_FUNC;
	}

CBulkThreadWatcher::~CBulkThreadWatcher()
	{
	LOG_FUNC;
	Cancel();
	}

void CBulkThreadWatcher::StartL()
	{
	LOG_FUNC;
	// Add to scheduler jit
	CActiveScheduler::Add(this);
	iServer.iBulkServerThread.Logon(iStatus);
	if(iStatus.Int() == KErrNoMemory)
		{
		User::WaitForRequest(iStatus); // swallow the signal...
		// if no memory then we have to fail now otherwise
		// we are in an odd state where we don't know if the
		// bulk thread is running or not.
		LEAVEL(KErrNoMemory);
		}
	else
		{
		// Otherwise the request is handled by the active scheduler.
		SetActive();
		}
	}

void CBulkThreadWatcher::RunL()
	{
	LOG_FUNC;
	LOG1(_L("\tiStatus.Int() = %d"), iStatus.Int());
	// Thread is dead so kill handle.
	iServer.iBulkServerThread.Close();
	iServer.iBulkThreadOpen = EFalse;
	iServer.StartShutdownTimerIfNoClientsOrBulkThread();
	iServer.iBulkThreadWatcher = NULL;
	delete this; // end...
	}

void CBulkThreadWatcher::DoCancel()
	{
	LOG_FUNC;
	iServer.iBulkServerThread.LogonCancel(iStatus);
	}

