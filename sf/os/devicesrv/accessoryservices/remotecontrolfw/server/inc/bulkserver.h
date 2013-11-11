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
//



/**
 @file
 @internalComponent
*/

#ifndef BULKSERVER_H
#define BULKSERVER_H

#include <e32base.h>
#include <e32msgqueue.h>
#include "bulkservermsgqueue.h"

class CBulkBearerInterface;
class CMessageQueue;
class CRemConBulkSession;
class CRemConMessage;
class TRemConAddress;

class CRemConServer;


/**
The bulk server class for Rem Con.
*/
NONSHARABLE_CLASS(CRemConBulkServer) : public CPolicyServer
	{
public:
	/**
	RemCon bulk server construction.
	@return Ownership of a new RemCon bulk server object
	*/
	static CRemConBulkServer* NewLC(RMsgQueue<TBulkServerMsg>& aMsgQueue);

	/** Destructor. */
	~CRemConBulkServer();

public: // called by session objects 
	/** Called by a session when a client session is created. Cancels the 
	shutdown timer in accordance with transient server design. */
	TInt ClientOpened(CRemConBulkSession& aSession, TProcessId aProcessId);

	/** Called by a session when a client session is destroyed. Starts the 
	shutdown timer if necessary in accordance with transient server design. 
	Does not assume that the session successfully registered itself with the 
	server to begin with.
	*/
	void ClientClosed(CRemConBulkSession& aSession);
	
	/** Called by a session when a client session has issued a receive.
	*/
	void ReceiveRequest(CRemConBulkSession& aSession);
	
	/**
	Starts the process of sending a response.
	Always takes ownership of aMsg.
	@return error
	*/
	TInt SendResponse(CRemConMessage& aMsg, CRemConBulkSession& aSess);
	
	/**
	Sends a reject back to the bearer.
	*/
	void SendReject(TRemConAddress aAddr, TUid aInterfaceUid, TUint aOperationId, TUint aTransactionId);
	
	inline CBulkBearerInterface& BulkBearerInterface();

public: // called by the bearer manager 
	inline RPointerArray<CRemConBulkSession>& Sessions();
	
	/** 
	Handles a new incoming command.
	Always takes ownership of aMsg.
	*/
	void NewCommand(CRemConMessage& aMsg);
	
	void CommandExpired(TUint aTransactionId);

private:
	/** Constructor. */
	CRemConBulkServer(RMsgQueue<TBulkServerMsg>& aMsgQueue);

	/** 2nd-phase construction. */
	void ConstructL();
	
private: // from CPolicyServer
	/**
	Called by the base class to create a new session.
	@param aVersion Version of client
	@param aMessage Client's IPC message
	@return A new session to be used for the client. If this could not be made, 
	this function should leave.
	*/
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

private: // utility	
	CRemConBulkSession* Session(TUint aSessionId) const;
	
	void StartShutdownTimerIfNoSessions();
	
#ifdef __FLOG_ACTIVE
	void LogSessions() const;
	void LogIncomingPendingDelivery() const;
	void LogIncomingDelivered() const;
#endif // __FLOG_ACTIVE
	
	CMessageQueue& IncomingPendingDelivery();
	CMessageQueue& IncomingDelivered();
	
	void DeliverCmdToClient(const CRemConMessage& aMsg, CRemConBulkSession& aSess);
	void DeliverMessageToClient(CRemConMessage& aMsg, CRemConBulkSession& aSess);
		
private: // called by the shutdown timer
	/** 
	Called by the shutdown timer when it fires. Terminates the server thread. 
	@param aThis This. 
	@return KErrNone.
	*/
	static TInt TimerFired(TAny* aThis);

	
private: // owned

	// NB This isn't really being used as a CPeriodic, but (a) it will do the 
	// job, and (b) it saves us writing our own concrete timer (there aren't 
	// any other more suitable concrete timer classes).
	CPeriodic* iShutdownTimer;

	// Shutdown delay (when there are no sessions) = 1 second- there's no 
	// point hanging around.
	static const TUint KShutdownDelay = 1 * 1000 * 1000;

	RPointerArray<CRemConBulkSession> iSessions;
	
	CMessageQueue* iIncomingPendingDelivery;
	CMessageQueue* iIncomingDelivered;
	
	CBulkBearerInterface* iBulkBearerInterface;
	
	RMsgQueue<TBulkServerMsg> iRemConMsgQueue;
	
	CRemConServer* iControlServer; //unowned.
	};

// Inlines

RPointerArray<CRemConBulkSession>& CRemConBulkServer::Sessions()
	{
	return iSessions;
	}

CBulkBearerInterface& CRemConBulkServer::BulkBearerInterface()
	{
	return *iBulkBearerInterface;
	}

#endif // BULKSERVER_H
