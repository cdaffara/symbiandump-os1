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

#ifndef CONTROLLERSESSION_H
#define CONTROLLERSESSION_H

#include "session.h"
#include "messagesendobserver.h"

class CActiveHelper;
NONSHARABLE_CLASS(CRemConControllerSession) : public CRemConSession, public MRemConMessageSendObserver
	{
public:
	/**
	Factory method.
	@param aServer The server.
	@param aBearerManager The bearer manager.
	@param aClientInfo Information about the client using this session.
	@param aId The unique ID of the new session.
	@return Ownership of a new session.
	*/
	static CRemConControllerSession* NewL(CRemConServer& aServer,
		CBearerManager& aBearerManager,
		const TClientInfo& aClientInfo,
		TUint aId);

	~CRemConControllerSession();

	/** 
	Checks if this message is supported by the session
	@param aMsg The message to check
	@return ETrue if it is, EFalse otherwise
	*/
	TBool SupportedMessage(const CRemConMessage& aMsg) const;

	inline const TPlayerType& PlayerType() const;
	inline const TPlayerSubType& PlayerSubType() const;
	inline const TDesC8& Name() const;
	
public: // called by the bearer manager
	/**
	Indicates that a connect request has been completed. The request was not 
	necessarily from this session- the session must check that the connected 
	address is one it has asked to be connected. If it is, and we have a 
	connect request outstanding, the connect request should be completed.
	@param aAddr The connected address.
	@param aError The error with which the connection attempt was completed.
	*/
	void CompleteConnect(const TRemConAddress& aAddr, TInt aError);

	/**
	Indicates that a disconnect request has been completed. The request was 
	not necessarily from this session- the session must check that the 
	disconnected address is one it has asked to be disconnected. If it is, and 
	we have a disconnect request outstanding, the disconnect request should be 
	completed.
	@param aAddr The disconnected address.
	@param aError The error with which the disconnection attempt was 
	completed.
	*/
	void CompleteDisconnect(const TRemConAddress& aAddr, TInt aError);
	
public: // called by the active helper
	void ProcessPendingMsgL();
	
private:
	/**
	Constructor.
	@param aServer The server.
	@param aBearerManager The bearer manager.
	@param aId The unique ID of the new session.
	*/
	CRemConControllerSession(CRemConServer& aServer, 
		CBearerManager& aBearerManager,
		TUint aId);

	/**
	2nd-phase construction.
	@param aClientInfo Information about the client using this session.
	*/
	void ConstructL(const TClientInfo& aClientInfo);
	
private: // From CRemConSession: utility- IPC command handlers
	void SetPlayerType(const RMessage2& aMessage);
	void SendUnreliable(const RMessage2& aMessage);
	void RegisterInterestedAPIs(const RMessage2& aMessage);
	void GoConnectionOriented(const RMessage2& aMessage);
	void GoConnectionless(const RMessage2& aMessage);
	void ConnectBearer(const RMessage2& aMessage);
	void ConnectBearerCancel(const RMessage2& aMessage);
	void DisconnectBearer(const RMessage2& aMessage);
	void DisconnectBearerCancel(const RMessage2& aMessage);
	void SendNotify(const RMessage2& aMessage);

private: // From CRemConSession: utility
	CRemConMessage* DoPrepareSendMessageL(const RMessage2& aMessage);
	void DoSendCancel();
	void DoReceive();
	
private: // utility
	CRemConMessage* DoCreateUnreliableMessageL(const RMessage2& aMessage);
	void DoSendNotifyL(const RMessage2& aMessage);
	
	void SendToServer(CRemConMessage& aMsg);
	
	void CheckForPendingMsg() const; 
	void EmptySendQueue();

	void CompleteMessage(const CRemConMessage& aMessage);

public: //From MRemConMessageSendObserver
	void MrcmsoMessageSendResult(const CRemConMessage& aMessage, TInt aError);
	void MrcmsoMessageSendOneOrMoreAttempt(const CRemConMessage& aMessage, TUint aNumRemotes);
	void MrcmsoMessageSendOneOrMoreIncremental(const CRemConMessage& aMessage, TUint aNumRemotes);	// Not supported
	void MrcmsoMessageSendOneOrMoreAttemptFailed(const CRemConMessage& aMessage, TInt aError);
	void MrcmsoMessageSendOneOrMoreResult(const CRemConMessage& aMessage, TInt aError);
	void MrcmsoMessageSendOneOrMoreAbandoned(const CRemConMessage& aMessage);	// Not supported

private: // message handles for asynchronous IPC requests
	// Address of remote device associated with this session.
	// A null remote address indicates a connectionless 
	// controller; a non-null UID indicates a connection-oriented controller.
	TRemConAddress iRemoteAddress;

	RMessage2 iConnectBearerMsg;
	RMessage2 iDisconnectBearerMsg;
	// Stores pending connect/disconnect request
	// There can be only one pending request at any time
	RMessage2 iPendingMsg;
	
	// Helps with session's async connect/disconnect requests
	CActiveHelper *iPendingMsgProcessor;

	// the player type information
	TPlayerTypeInformation iPlayerType;
	//the player name 
	RBuf8 iPlayerName;
	};

#include "controllersession.inl"

#endif /* CONTROLLERSESSION_H */
