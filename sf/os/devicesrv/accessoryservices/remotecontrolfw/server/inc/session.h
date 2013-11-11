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
//

/**
 @file
 @internalComponent
*/

#ifndef REMCONSESSION_H
#define REMCONSESSION_H

#include <e32base.h>
#include <remconserverpanic.h>
#include <remconaddress.h>
#include <remcon/clientinfo.h>
#include <remcon/clienttype.h>
#include <remcon/messagetype.h>
#include <remcon/playertype.h>
#include <remcon/clientid.h>

class CRemConServer;
class CBearerManager;
class CRemConMessage;
class CMessageQueue;
class CRemConInterfaceDetailsArray;
class CRemConInterfaceDetails;

/**
Rem Con session.
*/
NONSHARABLE_CLASS(CRemConSession) : public CSession2
	{
public:
	~CRemConSession();

public: // called by the server
	/** 
	@return The number of remotes the current 'send' has gone to successfully.
	*/
	inline TUint& NumRemotes();

	/** 
	@return The number of remotes the current 'send' is supposed to be going 
	to. NB This is decremented as the sends actually occur.
	*/
	inline TInt& NumRemotesToTry();

	/** 
	@return The error the current 'send' will be completed with.
	*/
	inline TInt& SendError();

	/** 
	@return Information about the client using this session.
	*/
	inline TClientInfo& ClientInfo();

	/** 
	@return Information about the client using this session.
	*/
	inline const TClientInfo& ClientInfo() const;

	/**
	@return This session's unique identifier.
	*/
	inline TUint Id() const;

	/**
	Used by the server to complete a client send request. 
	@param aError Error.					
	*/
	void CompleteSend();
	
	/**
	Used by the server to complete a client send notify request.
	*/
	void CompleteSendNotify();
	
	/** 
	Checks if this message is supported by the session
	@return ETrue if it is, EFalse otherwise
	*/
	virtual TBool SupportedMessage(const CRemConMessage& aMsg) const = 0;

	/**
	Writes aMsg to the client's Receive message (NB RMessage2::Write may fail) 
	and completes the client's Receive message with any error.
	@return Error.
	*/
	TInt WriteMessageToClient(const CRemConMessage& aMsg);

	/**
	Accessor for the currently outstanding client Send message. This is used 
	to check the client's capabilities to send over a particular bearer.
	@return Handle to client's send message.
	*/
	inline const RMessage2& CurrentSendMessage() const;

	/**
	Accessor for the currently outstanding client Receive message.
	@return Handle to client's receive message.
	*/
	inline const RMessage2& CurrentReceiveMessage() const;
	
	TInt SupportedInterfaces(RArray<TUid>& aUids);
	TInt AppendSupportedInterfaces(RArray<TUid>& aUids);
	TInt SupportedBulkInterfaces(RArray<TUid>& aUids);
	TInt AppendSupportedBulkInterfaces(RArray<TUid>& aUids);
	TInt SupportedOperations(TUid aInterfaceUid, RArray<TUint>& aOperations);

	/**
	Panics the client's current Send message with the given code.
	*/
	void PanicSend(TRemConClientPanic aCode);
	
	/**
	Checks whether this session is fully initialised and available for use
	by bearers.
	@return ETrue if session can be used by bearers, EFalse otherwise
	*/
	inline TBool ClientAvailable() const;

	/**
	Indicates that a connection has come up or down. If the session has a 
	'connections notification' outstanding, it should be completed. Note that, 
	when a connection comes up or down, ConnectionsChanged is called before 
	CompleteConnect or DisconnectComplete. These functions are called 
	synchronously by the connection-handling code.
	*/
	void ConnectionsChanged();
	
protected:
    /**
    Constructor.
    @param aServer The server.
    @param aBearerManager The bearer manager.
    @param aId The unique ID of the new session.
    */
    CRemConSession(CRemConServer& aServer, 
        CBearerManager& aBearerManager,
        TUint aId);
    
    /**
    2nd-phase construction.
    @param aMessage The message received from the client.
    */
    void BaseConstructL(const TClientInfo& aClientInfo);

	
protected: 		

	/**
	Utility to complete the given message with the given error code.
	@param aMessage Message to complete.
	@param aError Error to complete with.
	*/
	void CompleteClient(const RMessage2& aMessage, TInt aError);

	CRemConInterfaceDetailsArray* ExtractInterestedAPIsL(const RMessage2& aMessage);
	
	/**
	Gets the information needed to send from the RMessage.
	
	@param aMessage The message containing the info.
	@param aInterfaceUid The uid of the interface to send on
	@param aOperationId The operation id to send
	@param aMessageSubType The sub type of hte message
	@param aSendDes The operation data to send.  Ownership is passed
					to the caller and aSendDes is placed on the cleanup
					stack when this function completes successfully.
	@leave If the data was not successfully read. Note: if the message is invalid
	       this function will not leave, but rather panic the message and return
	       EFalse. This function will only leave if there's a system error, such
	       as out of memory.
	@return ETrue if the message information was successfully retrieved,
			or EFalse if the message is invalid, in which case it got panicked.
			If this function returns EFalse, aSendDes is not left on the cleanup stack.
	 */
	TBool DoGetSendInfoLC(const RMessage2& aMessage, 
			TUid& aInterfaceUid,
			TUint& aOperationId,
			TRemConMessageSubType& aMessageSubType,
			RBuf8& aSendDes);
			
	
protected: // from CSession2
	/**
	Called when a message is received from the client.
	
	Also used from the session, where a session has stored
	a message for later	processing.
	
	@param aMessage Message received from the client.
	*/
	void ServiceL(const RMessage2& aMessage);

private: // utility- IPC command handlers
	virtual void SetPlayerType(const RMessage2& aMessage)=0;
	virtual void SendUnreliable(const RMessage2& aMessage) = 0;
	virtual void SendCancel(const RMessage2& aMessage);
	virtual void Receive(const RMessage2& aMessage);
	virtual void ReceiveCancel(const RMessage2& aMessage);
	virtual void GetConnectionCount(const RMessage2& aMessage);
	virtual void GetConnections(const RMessage2& aMessage);
	virtual void NotifyConnectionsChange(const RMessage2& aMessage);
	virtual void NotifyConnectionsChangeCancel(const RMessage2& aMessage);
	virtual void RegisterInterestedAPIs(const RMessage2& aMessage) = 0;
	virtual void GoConnectionOriented(const RMessage2& aMessage);
	virtual void GoConnectionless(const RMessage2& aMessage);
	virtual void ConnectBearer(const RMessage2& aMessage);
	virtual void ConnectBearerCancel(const RMessage2& aMessage);
	virtual void DisconnectBearer(const RMessage2& aMessage);
	virtual void DisconnectBearerCancel(const RMessage2& aMessage);
	virtual void SendNotify(const RMessage2& aMessage);
	void Send(const RMessage2& aMessage);

private: // utility
	virtual CRemConMessage* DoPrepareSendMessageL(const RMessage2& aMessage)=0;
	virtual void DoSendCancel()=0;
	virtual void DoReceive()=0;
	void WriteMessageToClientL(const CRemConMessage& aMsg);
	virtual void SendToServer(CRemConMessage& aMsg)=0;
	static TInt SendNextCb(TAny* aThis);
	void DoSendNext();

protected:
	void DoSendL(const RMessage2& aMessage);
	void GetPlayerTypeAndNameL(const RMessage2& aMessage, TPlayerTypeInformation& aPlayerType, RBuf8& aPlayerName);
	CRemConInterfaceDetails* FindInterfaceByUid(TUid aUid) const;
	
protected: // unowned
	CRemConServer& iServer;
	CBearerManager& iBearerManager;

protected: // message handles for asynchronous IPC requests
	RMessage2 iSendMsg;
	RMessage2 iReceiveMsg;
	RMessage2 iNotifyConnectionsChangeMsg;

	CMessageQueue* iSendQueue;
	
protected: // owned
	// The client's process ID, secure ID and caps.
	TClientInfo iClientInfo;

	// Unique session identifier.
	const TUint iId;

	// Used to control the completion of the client's send request until we 
	// know the correct values of iNumSuccessfulRemotes and iSendError to use.
	// Generally 0. 
	// 1 when sending a connection-oriented command or a response.
	// N when sending a connectionless command (where N is the number of 
	// remotes the TSP addressed it to). 
	// While processing outgoing commands to multiple remotes, 
	// iNumRemotesToTry is decremented each time we finish trying to address a 
	// remote. This may be at the connection stage or the actual send stage.
	// On return to 0, the client is completed with either the result of the send
	// or KErrCancel.
	TInt iNumRemotesToTry;
	// For completion of the current send request. NB A send may be completed 
	// only after numerous asynchronous stages.
	TUint iNumRemotes;
	TInt iSendError;
	
	CRemConInterfaceDetailsArray* iInterestedAPIs;
	
	CAsyncCallBack* iSendNextCallBack;
	
	enum TRemConSessionSending
		{
		ENotSending,
		ESendingReliable,
		ESendingUnreliable
		};
	
	TRemConSessionSending iSending;
	};

// Inlines

TInt& CRemConSession::NumRemotesToTry()
	{
	return iNumRemotesToTry;
	}

TUint& CRemConSession::NumRemotes()
	{
	return iNumRemotes;
	}

TInt& CRemConSession::SendError()
	{
	return iSendError;
	}

TClientInfo& CRemConSession::ClientInfo()
	{
	return iClientInfo;
	}

const TClientInfo& CRemConSession::ClientInfo() const
	{
	return iClientInfo;
	}

TUint CRemConSession::Id() const
	{
	return iId;
	}

const RMessage2& CRemConSession::CurrentSendMessage() const
	{
	return iSendMsg;
	}

const RMessage2& CRemConSession::CurrentReceiveMessage() const
	{
	return iReceiveMsg;
	}

TBool CRemConSession::ClientAvailable() const
	{
	// Client is available as soon as it has registered the APIs
	// it is interested in.
	return iInterestedAPIs ? ETrue : EFalse;
	}

#endif // REMCONSESSION_H

