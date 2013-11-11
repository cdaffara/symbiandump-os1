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
// Remote Control bulk server implementation.
//



/**
 @file
 @internalComponent
*/
#include <bluetooth/logger.h>

#include "bulkbearerinterface.h"
#include "bulkserver.h"
#include "bulkserversecuritypolicy.h"
#include "bulksession.h"
#include "messagequeue.h"
#include "remconmessage.h"
#include "utils.h"

#include "server.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("bulkserver");
#endif // _DEBUG

#ifdef __FLOG_ACTIVE
#define LOGSESSIONS							LogSessions()
#define LOGINCOMINGPENDINGDELIVERY			LogIncomingPendingDelivery()
#define LOGINCOMINGDELIVERED				LogIncomingDelivered()
#else
#define LOGSESSIONS
#define LOGINCOMINGPENDINGDELIVERY
#define LOGINCOMINGDELIVERED
#endif // __FLOG_ACTIVE

CRemConBulkServer* CRemConBulkServer::NewLC(RMsgQueue<TBulkServerMsg>& aMsgQueue)
	{
	LOG_STATIC_FUNC;
	CRemConBulkServer* self = new(ELeave) CRemConBulkServer(aMsgQueue);
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance 
	// of the same server running, so do it before ConstructL.
	self->StartL(KRemConBulkServerName);
	self->ConstructL();
	return self;
	}

CRemConBulkServer::~CRemConBulkServer()
	{
	LOG_FUNC;

	delete iShutdownTimer;

	iSessions.Close();
	
	delete iIncomingPendingDelivery;
	delete iIncomingDelivered;
	
	delete iBulkBearerInterface;
	}

CRemConBulkServer::CRemConBulkServer(RMsgQueue<TBulkServerMsg>& aMsgQueue)
 :	CPolicyServer(CActive::EPriorityStandard, KRemConBulkServerPolicy, ESharableSessions),
 	iRemConMsgQueue(aMsgQueue)
	{
	LOG_FUNC;
	}

void CRemConBulkServer::ConstructL()
	{
	LOG_FUNC;
	iShutdownTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	
	iIncomingPendingDelivery = CMessageQueue::NewL();
	iIncomingDelivered = CMessageQueue::NewL();
	
	// First message is the control server for client ID binding
	TBulkServerMsg ctrlMsg;
	iRemConMsgQueue.ReceiveBlocking(ctrlMsg);
	ASSERT_DEBUG(ctrlMsg.iType == EControlServer);
	ASSERT_DEBUG(ctrlMsg.iData);
	iControlServer = reinterpret_cast<CRemConServer*>(ctrlMsg.iData);
	
	// Second message is bearer manager information for interface loading.
	TBulkServerMsg msg;
	iRemConMsgQueue.ReceiveBlocking(msg);
	ASSERT_DEBUG(msg.iType == EBearerManager);
	ASSERT_DEBUG(msg.iData);
	CBearerManager* bearerManager = reinterpret_cast<CBearerManager*>(msg.iData);
	iBulkBearerInterface = CBulkBearerInterface::NewL(*this, *bearerManager);
	}

CSession2* CRemConBulkServer::NewSessionL(const TVersion& aVersion, 
	const RMessage2& aMessage) const
	{
	LOG_FUNC;
	LOG3(_L8("\taVersion = (%d,%d,%d)"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild);
		
	// Version number check...
	TVersion v(KRemConBulkSrvMajorVersionNumber,
		KRemConBulkSrvMinorVersionNumber,
		KRemConBulkSrvBuildNumber);

	if ( !User::QueryVersionSupported(v, aVersion) )
		{
		LEAVEIFERRORL(KErrNotSupported);
		}

	CRemConBulkServer* ncThis = const_cast<CRemConBulkServer*>(this);
	
	CRemConBulkSession* sess = NULL;
	TRAPD(err, sess = CRemConBulkSession::NewL(*ncThis, 
				aMessage)
			);
	if ( err != KErrNone )
		{
		// Session creation might have failed- if it has we need to check if 
		// we need to shut down again.
		const_cast<CRemConBulkServer*>(this)->StartShutdownTimerIfNoSessions();
		LEAVEIFERRORL(err);
		}

	LOG1(_L8("\tsess = 0x%08x"), sess);
	return sess;
	}

void CRemConBulkServer::StartShutdownTimerIfNoSessions()
	{
	LOG_FUNC;

	if ( iSessions.Count() == 0 )
		{
		LOG(_L8("\tno remaining sessions- starting shutdown timer"));
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
				TCallBack(CRemConBulkServer::TimerFired, this)
				);
			}
		else
			{
			LOG(_L8("\tshutdown timer was already active"));
			}
		}
	}

TInt CRemConBulkServer::TimerFired(TAny* aThis)
	{
	LOG_STATIC_FUNC
	static_cast<void>(aThis);
	
#if defined(__FLOG_ACTIVE) || defined(_DEBUG)
	CRemConBulkServer* self = static_cast<CRemConBulkServer*>(aThis);
	// We should have sent 'this' to this callback. 
	ASSERT_DEBUG(self);
	LOG1(_L8("\tauto shutdown- terminating the bulk server [0x%08x]"), self);
#endif // __FLOG_ACTIVE || _DEBUG
	
	// Stop our Active Scheduler. This returns the flow of execution to after 
	// the CActiveScheduler::Start call in the server startup code, and 
	// terminates the server.
	CActiveScheduler::Stop();
	
	return KErrNone;
	}

TInt CRemConBulkServer::ClientOpened(CRemConBulkSession& aSession, TProcessId aProcessId)
	{
	LOG_FUNC;
	LOG1(_L8("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;
	
	ASSERT_DEBUG(iControlServer);
	TRemConClientId id = iControlServer->ClientIdByProcessId(aProcessId);
	TInt ret = KErrNotFound; // If the control session has already been destroyed then fail...
	if(id != KNullClientId)
		{
		TRAP(ret, iControlServer->BulkInterfacesForClientL(id, aSession.InterestedAPIs()));
		if(ret == KErrNone)
			{
			aSession.SetId(id);
		
			// Register the session by appending it to our array, and also making an 
			// item for it in the record of which points in the connection history 
			// sessions are interested in.
			ret = iSessions.Append(&aSession);
			
			if ( ret == KErrNone )
				{
				ASSERT_DEBUG(iBulkBearerInterface);
				iBulkBearerInterface->BulkClientAvailable(id);
				
				// Should have been created during our construction.
				ASSERT_DEBUG(iShutdownTimer);
				iShutdownTimer->Cancel();
				}
			}
		}

	LOGSESSIONS;
	LOG1(_L8("\tret = %d"), ret);
	return ret;
	}

// called by session when session is closed.
void CRemConBulkServer::ClientClosed(CRemConBulkSession& aSession)
	{
	LOG_FUNC;
	LOG1(_L8("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	// Find this session in the array and remove it (if it's there).
	const TUint sessCount = iSessions.Count();
	for(TUint ix = 0; ix < sessCount; ++ix)
		{
		if(iSessions[ix] == &aSession)
			{
			// We've found the session in our array.
			ASSERT_DEBUG(aSession.Id() != KNullClientId);
			ASSERT_DEBUG(iBulkBearerInterface);
			iBulkBearerInterface->BulkClientRemoved(aSession.Id());
			// Remove the session from our array.
			iSessions.Remove(ix);
			break;
			}
		}

	StartShutdownTimerIfNoSessions();

	LOGSESSIONS;
	}

#ifdef __FLOG_ACTIVE
void CRemConBulkServer::LogSessions() const
	{
	const TUint count = iSessions.Count();
	LOG1(_L8("\tNumber of sessions = %d"), count);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConBulkSession* const session = iSessions[ii];
		ASSERT_DEBUG(session);
		LOG3(_L8("\t\tsession %d [0x%08x], Id = %d"), 
			ii, 
			session,
			session->Id()
			);
		}
	}

void CRemConBulkServer::LogIncomingPendingDelivery() const
	{
	LOG(_L8("Logging incoming pending delivery messages"));
	__FLOG_STMT(const_cast<CRemConBulkServer*>(this)->IncomingPendingDelivery().LogQueue();)
	}

void CRemConBulkServer::LogIncomingDelivered() const
	{
	LOG(_L8("Logging incoming delivered commands"));
	__FLOG_STMT(const_cast<CRemConBulkServer*>(this)->IncomingDelivered().LogQueue();)
	}
#endif // __FLOG_ACTIVE


void CRemConBulkServer::ReceiveRequest(CRemConBulkSession& aSession)
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
			TInt err = aSession.WriteMessageToClient(*msg);
			IncomingPendingDelivery().Remove(*msg);
			
			if (err == KErrNone )
				{
				// We'll need to remember it for the response coming back.
				IncomingDelivered().Append(*msg); 
				}
			else
				{
				// Tell bearer it won't be getting a response
				SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());

				// 'Take ownership' of it by destroying it- it's finished with.
				delete msg;
				}
			
			break;
			}
		}

	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

void CRemConBulkServer::NewCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	CRemConBulkSession* const sess = Session(aMsg.Client());
	if(!sess)
		{
        // Tell bearer it won't be getting a response
        SendReject(aMsg.Addr(), aMsg.InterfaceUid(), aMsg.OperationId(), aMsg.TransactionId());

        // 'Take ownership' of it by destroying it- it's finished with.
        delete &aMsg;
		}
	else
		{
		DeliverCmdToClient(aMsg, *sess);
		}
	//now wait for the actual response to come from the client (success!)
	}

void CRemConBulkServer::DeliverCmdToClient(const CRemConMessage& aMsg, CRemConBulkSession& aSess)
	{
	LOG_FUNC;

	ASSERT_DEBUG(aMsg.MsgType() == ERemConCommand); 
	// Take a reference of the message and set the right session ID (important 
	// to set the selected session's ID because this is how we match up the 
	// client's response).
	CRemConMessage& msg = const_cast<CRemConMessage&>(aMsg);
	msg.SessionId() = aSess.Id();
	DeliverMessageToClient(msg, aSess);
	}

void CRemConBulkServer::DeliverMessageToClient(CRemConMessage& aMsg, CRemConBulkSession& aSess)
	{
	LOG_FUNC;

	TInt err = KErrNone;
	// See if the client can take the message now and put it on the right 
	// queue.
	if ( aSess.CurrentReceiveMessage().Handle() )
		{
		err = aSess.WriteMessageToClient(aMsg);
		// If the message was a command, and it was delivered with no error, 
		// then put it in the 'incoming delivered' log. Otherwise, delete it 
		// because it's finished with.
		if (err == KErrNone )
			{
			// We'll need to remember it for the response coming back.
			IncomingDelivered().Append(aMsg); 
			}
		else
			{
			// Tell bearer it won't be getting a response
			SendReject(aMsg.Addr(), aMsg.InterfaceUid(), aMsg.OperationId(), aMsg.TransactionId());

			// 'Take ownership' of it by destroying it- it's finished with.
			delete &aMsg;
			}
		}
	else
		{
		IncomingPendingDelivery().Append(aMsg);
		}
	
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

TInt CRemConBulkServer::SendResponse(CRemConMessage& aMsg, CRemConBulkSession& aSess)
	{
	LOG_FUNC;
	LOGINCOMINGDELIVERED;
	TInt ret = KErrNone;

	CRemConMessage* response = &aMsg;
	
	// Find the first command ('space badger') in the iIncomingDelivered queue with 
	// the same session ID, interface UID and operation ID as the response 
	// we're sending, and send the response to the same address that space badger came 
	// from.
	TSglQueIter<CRemConMessage>& iter = IncomingDelivered().SetToFirst();
	CRemConMessage* msg;
	TBool found = EFalse;
	while ( ( msg = iter++ ) != NULL )
		{
		if (	msg->SessionId() == aSess.Id()
			&&	msg->InterfaceUid() == response->InterfaceUid()
			&&	msg->OperationId() == response->OperationId())
			{
			LOG1(_L8("\tfound a matching item in the incoming delivered commands log: [0x%08x]"), msg);
			found = ETrue;
			
			// Set the right address and transaction id in the outgoing message
			response->Addr() = msg->Addr();
			response->TransactionId() = msg->TransactionId();
			
			// Remove the item from the 'incoming delivered' queue now we've 
			// addressed a response using it.
			IncomingDelivered().RemoveAndDestroy(*msg);
			
			ASSERT_DEBUG(iBulkBearerInterface);
			ret = iBulkBearerInterface->Send(aMsg);

			break;
			}
		}

	// If the command was not found, then the app has sent a response to a
	// non-existant command.  It may do this in good intention if the server 
	// has transparently died and been restarted between the app's reception
	// of the command and it sending its response, so we can't panic it.
	// Just drop the message.
	if ( !found )
		{
		// Complete the message with KErrNone.  We have done all we can with
		// it.  Any other error may encourage retries from the application,
		// which would be useless in this situation.
		ret = KErrNone;
		}

	// We've now finished with the response.
	delete response;
		
	LOGINCOMINGDELIVERED;
	return ret;
	}

void CRemConBulkServer::CommandExpired(TUint aTransactionId)
	{
	LOG_FUNC;

	CRemConMessage* msg;
	
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
	}

void CRemConBulkServer::SendReject(TRemConAddress aAddr, TUid aInterfaceUid, TUint aOperationId, TUint aTransactionId)
	{
	LOG_FUNC;
	
	CRemConMessage* rejectMsg = NULL;
	RBuf8 data;
	data = KNullDesC8;
	TRAPD(err, rejectMsg = CRemConMessage::NewL(aAddr, KNullClientId, ERemConReject, ERemConMessageDefault, aInterfaceUid, aOperationId, data, 0, aTransactionId));
	if ( err == KErrNone)
		{
		static_cast<void>(iBulkBearerInterface->Send(*rejectMsg));
		// We've now finished with the response.
		delete rejectMsg;
		}
	}

CMessageQueue& CRemConBulkServer::IncomingPendingDelivery()
	{
	ASSERT_DEBUG(iIncomingPendingDelivery);
	return *iIncomingPendingDelivery;
	}

CMessageQueue& CRemConBulkServer::IncomingDelivered()
	{
	ASSERT_DEBUG(iIncomingDelivered);
	return *iIncomingDelivered;
	}

CRemConBulkSession* CRemConBulkServer::Session(TUint aSessionId) const
	{
	CRemConBulkSession* sess = NULL;

	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConBulkSession* const temp = iSessions[ii];
		ASSERT_DEBUG(temp);
		if ( temp->Id() == aSessionId )
			{
			sess = temp;
			break;
			}
		}

	return sess;
	}

