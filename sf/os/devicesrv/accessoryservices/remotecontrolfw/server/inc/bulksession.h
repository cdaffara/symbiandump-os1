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

#ifndef REMCONBULKSESSION_H
#define REMCONBULKSESSION_H

#include <e32base.h>
#include <remconserverpanic.h>
#include <remcon/clientinfo.h>

class CRemConBulkServer;
class CRemConLogger;
class CBearerManager;
class CRemConMessage;
class CActiveHelper;
class CMessageQueue;

/**
Rem Con session.
*/
NONSHARABLE_CLASS(CRemConBulkSession) : public CSession2
	{
public:
	/**
	Factory method.
	@param aServer The server.
	@param aMessage Handle on the client message triggering this session creation.
	@return Ownership of a new session.
	*/
	static CRemConBulkSession* NewL(CRemConBulkServer& aServer,
		const RMessage2& aMessage);

	~CRemConBulkSession();

public: // called by the server
	/**
	@return This session's unique identifier.
	*/
	inline TUint Id() const;
	
	inline void SetId(TUint aId);
	
	inline RArray<TUid>& InterestedAPIs();
	
	/** 
	@return Information about the client using this session.
	*/
	inline const TClientInfo& ClientInfo() const;
	
	/**
	Writes aMsg to the client's Receive message (NB RMessage2::Write may fail) 
	and completes the client's Receive message with any error.
	@return Error.
	*/
	TInt WriteMessageToClient(const CRemConMessage& aMsg);

	/**
	Accessor for the currently outstanding client Receive message.
	@return Handle to client's receive message.
	*/
	inline const RMessage2& CurrentReceiveMessage() const;

	
private:
	/**
	@param aServer The server.
	@param aLogger The logger.
	@param aId The unique ID of the new session.
	*/
	CRemConBulkSession(CRemConBulkServer& aServer);

	/**
	2nd-phase construction.
	@param aMessage The message received from the client.
	*/
	void ConstructL(const RMessage2& aMessage);
		
private: // from CSession2
	/**
	Called when a message is received from the client.
	@param aMessage Message received from the client.
	*/
	void ServiceL(const RMessage2& aMessage);

private: // utility- IPC command handlers
	void Send(const RMessage2& aMessage);
	void SendUnreliable(const RMessage2& aMessage);
	void SendCancel(const RMessage2& aMessage);
	void Receive(const RMessage2& aMessage);
	void ReceiveCancel(const RMessage2& aMessage);
	
private: // utility
	void DoSendL(const RMessage2& aMessage);
	
	/**
	Utility to complete the given message with the given error code.
	@param aMessage Message to complete.
	@param aError Error to complete with.
	*/
	void CompleteClient(const RMessage2& aMessage, TInt aError);
	
	CRemConMessage* DoCreateMessageL(const RMessage2& aMessage, TBool aReliable);
	TInt SendToServer(CRemConMessage& aMsg);

	
private: // unowned
	CRemConBulkServer& iServer;

private: // message handles for asynchronous IPC requests
	RMessage2 iReceiveMsg;

private: // owned
	// The client's process ID, secure ID and caps.
	TClientInfo iClientInfo;

	// session identifier - mapped to the control session reference...
	TUint iId;
	
	RArray<TUid> iInterestedAPIs; // cached from control server.
	};

// Inlines

TUint CRemConBulkSession::Id() const
	{
	return iId;
	}

void CRemConBulkSession::SetId(TUint aId)
	{
	iId = aId;
	}

const RMessage2& CRemConBulkSession::CurrentReceiveMessage() const
	{
	return iReceiveMsg;
	}

RArray<TUid>& CRemConBulkSession::InterestedAPIs()
	{
	return iInterestedAPIs;
	}

const TClientInfo& CRemConBulkSession::ClientInfo() const
	{
	return iClientInfo;
	}

#endif // REMCONBULKSESSION_H

