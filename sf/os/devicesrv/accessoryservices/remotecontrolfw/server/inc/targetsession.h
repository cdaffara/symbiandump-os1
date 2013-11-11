// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef TARGETSESSION_H
#define TARGETSESSION_H

#include "targetclientprocess.h"
#include "session.h"

NONSHARABLE_CLASS(CRemConTargetSession) : public CRemConSession
	{
public:
	/**
	Factory method.
	@param aClientProcess The client for which this session is being created.
	@param aServer The server.
	@param aBearerManager The bearer manager.
	@param aId The unique ID of the new session.
	@return Ownership of a new session.
	*/
	static CRemConTargetSession* NewL(CRemConTargetClientProcess& aClientProcess,
		CRemConServer& aServer,
		CBearerManager& aBearerManager,
		TUint aId);

	~CRemConTargetSession();

	/** 
	From CRemConSession.
	Checks if this message is supported by the session
	@param aMsg The message to check
	@return ETrue if it is, EFalse otherwise
	*/
	TBool SupportedMessage(const CRemConMessage& aMsg) const;

	/**
	 Indicates whether or not a given interface is supported by this
	 target session.
	 @param aInterfaceUid The UID of the interface to check for.
	 @return ETrue if the interface is supported.
	 */
	TBool InterfaceSupported(TUid aInterfaceUid) const;

private: // From CRemConSession: utility- IPC command handlers
	void SetPlayerType(const RMessage2& aMessage);
	void SendUnreliable(const RMessage2& aMessage);
	void RegisterInterestedAPIs(const RMessage2& aMessage);

private: // From CRemConSession: utility
	CRemConMessage* DoPrepareSendMessageL(const RMessage2& aMessage);
	void DoSendCancel();
	void DoReceive();
	void SendToServer(CRemConMessage& aMsg);
	
private: // utility
	CRemConMessage* DoCreateUnreliableMessageL(const RMessage2& aMessage);
	
	
private:
	/**
	Constructor.
	@param aServer The server.
	@param aBearerManager The bearer manager.
	@param aId The unique ID of the new session.
	*/
	CRemConTargetSession(CRemConTargetClientProcess& aClientProcess,
		CRemConServer& aServer, 
		CBearerManager& aBearerManager,
		TUint aId);

	/**
	2nd-phase construction.
	*/
	void ConstructL();

private:
	CRemConTargetClientProcess&	iClientProcess;
	};

#endif /* TARGETSESSION_H */
