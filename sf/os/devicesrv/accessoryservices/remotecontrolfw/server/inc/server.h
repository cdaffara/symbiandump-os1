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

#ifndef SERVER_H
#define SERVER_H

#include <e32base.h>
#include <e32msgqueue.h>
#include <remcon/remcontargetselectorpluginobserver.h>
#include <remcon/clientid.h>

#include "bulkservermsgqueue.h"
#include "connectionstate.h"
#include "tspv4stub.h"
#include "utils.h"

class REComSession;
class CBearerManager;	
class CConverterManager;
class CRemConTargetClientProcess;
class CRemConSession;
class CRemConTargetSession;
class CRemConControllerSession;
class CMessageQueue;
class CRemConTargetSelectorPlugin;
class CRemConMessage;
class CRemConConverterPlugin;
class CMessageRecipientsList;
class TClientInfo;
class TRemConAddress;
class CConnectionHistory;
class CConnections;
class MRemConTargetSelectorPluginInterface;
class MRemConTargetSelectorPluginInterfaceV2;
class MRemConTargetSelectorPluginInterfaceV3;
class MRemConTargetSelectorPluginInterfaceV4;
class MRemConConverterInterface;

// helper classes
class CBulkThreadWatcher;

/**
The server class for Rem Con.
*/
NONSHARABLE_CLASS(CRemConServer) : public CPolicyServer, public MRemConTargetSelectorPluginObserver
	{
friend class CBulkThreadWatcher; // helper active object
public:
	/**
	RemCon server construction.
	@return Ownership of a new RemCon server object
	*/
	static CRemConServer* NewLC();

	~CRemConServer();

public: // called by controller session objects 

	/** Called by a controller session when created. Cancels the 
	shutdown timer in accordance with transient server design. */
	TInt ControllerClientOpened(CRemConControllerSession& aSession);
		
	/** Called by a controller session when it has had its features registered. */
	void ControllerClientAvailable();

	/** Called by a controller session when it has had its client go connection oriented
	Uses the bearer manager to inform bearers as appropriate
	aUid is the uid of the bearer we are going connection oriented on.
	*/
	void ClientGoConnectionOriented(CRemConControllerSession& aSession, TUid aUid);
	
	/** Called by a controller session when it has had its client go connection less.
	 Uses the bearer manager to inform bearers as appropriate.
	 aUid is the uid of the bearer we were connection oriented on,
	*/
	void ClientGoConnectionless(CRemConControllerSession& aSession, TUid aUid);

	/** Called by a controller session it is destroyed. Starts the 
	shutdown timer if necessary in accordance with transient server design. 
	Does not assume that the session successfully registered itself with the 
	server to begin with.
	aUid is the bearer uid the session was on for connection oriented sessions,
	if KNullUid the session was connectionless, 
	*/
	void ControllerClientClosed(CRemConControllerSession& aSession, TUid aUid);

	/**
	Sends a command.
	Puts the command on the 'pending TSP' queue so the TSP can either address 
	it or give it permission to send.
	Always takes ownership of aMsg. 
	*/
	void SendCommand(CRemConMessage& aMsg);

	/**
	Removes any current message from this controller session from the 'outgoing pending 
	TSP' queue. If the message is that currently being dealt with by the TSP, 
	then cancels the TSP's operation.
	*/
	void SendCancel(CRemConControllerSession& aSess);

	/**
	Tries to complete a Receive request for a controller session.
	Called by controller sessions when a Receive request is posted. The 'incoming pending 
	delivery' queue is checked for commands waiting to be delivered to 
	aSession. The controller session's request is completed with the first such found.
	*/
	void ReceiveRequest(CRemConControllerSession& aSession);
	
public:	// Called by target client objects (either CRemConTargetClientProcess or CRemConTargetSession)

	/** 
	Called by CRemConTargetClientProcess to cancel the servers shutdown timer when a new
	target session has been opened.
	*/
	void CancelShutdownTimer();

	/** Called by CRemConTargetClientProcess to notify the server that it is available.
	This is done when the client has registered one or more interfaces. */
	void TargetClientAvailable(const CRemConTargetClientProcess& aClient);

	/** Called by CRemConTargetClientProcess to notify the server that it has registered
	additional intefaces. */
	void TargetFeaturesUpdated(CRemConTargetClientProcess& aClient);

	/** Called by CRemConTargetClientProcess to notify the server that a target session
	has closed. */
	void TargetSessionClosed(CRemConTargetClientProcess& aClient, CRemConTargetSession& aSession);

	/** Called by CRemConTargetClientProcess to notify the server that a client session is closing.
	Starts the shutdown timer if necessary in accordance with transient server design. 
	Does not assume that the session successfully registered itself with the 
	server to begin with.
	*/
	void TargetClientClosed(CRemConTargetClientProcess& aClient);

	/** Called by a client instance when a session is created. Makes an item 
	for the session in the record of which points in the connection history 
	sessions are interested in. */
	TInt RegisterTargetSessionPointerToConnHistory(const CRemConTargetSession& aSession);

	/**
	Starts the process of sending a response, via the TSP
	Completes the client's send message with a bearer-level error.
	Always takes ownership of aMsg. 
	*/
	void SendResponse(CRemConMessage& aMsg, CRemConTargetClientProcess& aClient);

	/** Finishes the process of sending a response, after the TSP
	has permitted the response
	Always takes onwership of aMsg.
	*/
	void CompleteSendResponse(CRemConMessage& aMsg, CRemConTargetClientProcess& aClient);

	/**
	Sends a reject back to the bearer.
	*/
	void SendReject (TRemConAddress aAddr, TUid aInterfaceUid, TUint aOperationId, TUint aTransactionId);

	/**
	Tries to complete a Receive request for a target client.
	Called by clients when a Receive request is posted. The 'incoming pending 
	delivery' queue is checked for commands waiting to be delivered to 
	the client. A delivery attempt is made for each pending message.
	*/
	void ReceiveRequest(CRemConTargetClientProcess& aClient);

public:	// called by controller and target sessions

	/** Returns the current bearer-level connection state of the system. */
	CConnections& Connections();

	/** Each session has a pointer into the connection history- pointing at 
	the record in the history in which the session is interested. This 
	function moves the pointer (for the given session) to the last item in the 
	history. */
	void SetConnectionHistoryPointer(TUint aSessionId);

	/** Gets the record in the connection history which the session is 
	pointing to. */
	const CConnections& Connections(TUint aSessionId) const;

	/** Returns ETrue if the given session's currently flagged connection 
	history record is the 'Last' connection history record, i.e. the current 
	connection set. Returns EFalse otherwise. */
	TBool ConnectionHistoryPointerAtLatest(TUint aSessionId) const;

	/**
	Determines a state of a connection to the given remote address.
	@param - aAddr, remote address of a connection
	@return - connection state 
	*/
	TConnectionState ConnectionState(const TRemConAddress& aAddr);

	/**
	Informs RemCon server that one of the interfaces being used by the calling
	session requires the use of the bulk server.
	*/
	TInt BulkServerRequired();	
	
public: // called by the bearer manager 

	/** This function is called when a ConnectIndicate is handled by the 
	bearer manager (in which case aError will be KErrNone) and when 
	ConnectConfirm is handled by the bearer manager (in which case aError will 
	be the connection error). 
	@return Error. If we cannot handle the new connection (if there is one) 
	then the bearer must drop the connection. Note that if aError is not 
	KErrNone the return value will be meaningless.
	*/
	TInt HandleConnection(const TRemConAddress& aAddr, TInt aError);

	/** This function is called when a connection goes away, either by 
	indication (from the remote end) or confirmation (from our end). */
	void RemoveConnection(const TRemConAddress& aAddr, TInt aError);

	/** 
	Handles a new incoming response. 
	Finds the (assumed single) command on the 'outgoing sent' queue matching 
	it, to find the session which sent _that_. Gives the new response to that 
	client session. 
	Always takes ownership of aMsg. 
	*/
	void NewResponse(CRemConMessage& aMsg);
	
	/** 
	Handles a new incoming response for a notify command. 
	Finds the (assumed single) command on the 'outgoing sent' queue matching 
	it, to find the session which sent _that_. Gives the new response to that 
	client session. 
	Always takes ownership of aMsg. 
	*/
	void NewNotifyResponse(CRemConMessage& aMsg);

	/** 
	Handles a new incoming command.
	Puts the message on the 'incoming response pending' queue. If the TSP is 
	not busy, asks it to address the message to target client(s). If the TSP 
	is busy, then the queue will be checked (and the next message handled) 
	when the TSP calls IncomingCommandAddressed. 
	Always takes ownership of aMsg.
	*/
	void NewCommand(CRemConMessage& aMsg);
	
	/** 
	Handles a new incoming notify command.
	Puts the message on the 'incoming response pending' queue. If the TSP is 
	not busy, asks it to address the message to target client. If the TSP 
	is busy, then the queue will be checked (and the next message handled) 
	when the TSP calls IncomingNotifyCommandAddressed. 
	Always takes ownership of aMsg.
	*/
	void NewNotifyCommand(CRemConMessage& aMsg);

	/** Returns the converter interface which supports the given outer-layer 
	API and bearer UIDs, or NULL if it does not exist. */
	MRemConConverterInterface* Converter(TUid aInterfaceUid, TUid aBearerUid) const;
	
	/** Returns the converter interface which supports the given outer-layer 
	API and bearer UIDs, or NULL if it does not exist. */
	MRemConConverterInterface* Converter(const TDesC8& aInterfaceData, TUid aBearerUid) const;

	void CommandExpired(TUint aTransactionId);
	
	TInt SupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids);
	TInt SupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations);
	TInt ControllerSupportedInterfaces(RArray<TUid>& aSupportedInterfaces);
	
	void SetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId);
	
	TInt RegisterLocalAddressedClientObserver(const TUid& aBearerUid);
	TInt UnregisterLocalAddressedClientObserver(const TUid& aBearerUid);

public: // called by bulk server
	TRemConClientId ClientIdByProcessId(TProcessId aProcessId);
	
	void BulkInterfacesForClientL(TRemConClientId aId, RArray<TUid>& aUids);

private:
	/** Constructor. */
	CRemConServer();

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

private: // from MRemConTargetSelectorPluginObserver
	void MrctspoDoOutgoingCommandAddressed(TInt aError);
	void MrctspoDoOutgoingCommandPermitted(TBool aIsPermitted);
	void MrctspoDoOutgoingNotifyCommandPermitted(TBool aIsPermitted);
	void MrctspoDoIncomingCommandAddressed(TInt aError);
	TInt MrctspoDoGetConnections(TSglQue<TRemConAddress>& aConnections);
	void MrctspoDoOutgoingResponsePermitted(TBool aIsPermitted);
	void MrctspoDoIncomingNotifyAddressed(TClientInfo* aClientInfo, TInt aError);
	void MrctspoDoReAddressNotifies();
	void MrctspoDoOutgoingNotifyCommandAddressed(TRemConAddress* aConnection, TInt aError);
	void MrctspoDoIncomingCommandPermitted(TBool aIsPermitted);
	void MrctspoDoIncomingNotifyPermitted(TBool aIsPermitted);
	TInt MrctspoSetLocalAddressedClient(const TUid& aBearerUid, const TClientInfo& aClientInfo);

private: // utility
	CRemConControllerSession* CreateControllerSessionL(const RMessage2& aMessage);
	CRemConTargetSession* CreateTargetSessionL(const RMessage2& aMessage);

	/** Extracts a client's process ID and secure ID from a given RMessage2 and stores
	in a given TClientInfo. */
	void ClientProcessAndSecureIdL(TClientInfo& aClientInfo, const RMessage2& aMessage) const;

	void RemoveSessionFromConnHistory(const CRemConSession& aSession);

	/** Removes connection history records which are no longer interesting and 
	updates the indices in iSession2ConnHistory accordingly. */
	void UpdateConnectionHistoryAndPointers();

	CRemConControllerSession* ControllerSession(TUint aSessionId) const;
	CRemConTargetClientProcess* TargetClient(TUint aClientId) const;
	CRemConTargetClientProcess* TargetClient(TProcessId aProcessId) const;
	
	void StartShutdownTimerIfNoClientsOrBulkThread();
	void TryToDropClientProcess(TUint aClientIndex);
	void LoadTspL();
	
	TBool FindDuplicateNotify(CRemConMessage& aMsg);
	
#ifdef __FLOG_ACTIVE
	void LogControllerSessions() const;
	void LogTargetSessions() const;
	void LogRemotes() const;
	void LogConnectionHistoryAndInterest() const;
	void LogOutgoingCmdPendingTsp() const;
	void LogOutgoingNotifyCmdPendingTsp() const;
	void LogOutgoingRspPendingTsp() const;
	void LogOutgoingPendingSend() const;
	void LogOutgoingSent() const;
	void LogIncomingCmdPendingAddress() const;
	void LogIncomingNotifyCmdPendingAddress() const;
	void LogIncomingNotifyCmdPendingReAddress() const;
	void LogIncomingPendingDelivery() const;
	void LogIncomingDelivered() const;
#endif // __FLOG_ACTIVE
	
	CMessageQueue& OutgoingCmdPendingTsp();
	CMessageQueue& OutgoingNotifyCmdPendingTsp();
	CMessageQueue& OutgoingRspPendingTsp();
	CMessageQueue& OutgoingRspPendingSend();
	CMessageQueue& OutgoingPendingSend();
	CMessageQueue& OutgoingSent();
	
	CMessageQueue& IncomingCmdPendingAddress();
	CMessageQueue& IncomingNotifyCmdPendingAddress();
	CMessageQueue& IncomingNotifyCmdPendingReAddress();
	CMessageQueue& IncomingPendingDelivery();
	CMessageQueue& IncomingDelivered();

	/** 
	Utility for sending a single outgoing command to a remote.
	Takes a copy of aMsg and sets its Addr to aConn. If aConn exists as a 
	bearer-level connection then it tries to send the message. If the send 
	fails, the new message is destroyed and the function leaves. If the send 
	succeeds, the new message is put on OutgoingSent. 
	If aConn does not exist as a bearer-level connection, the new message is 
	put on OutgoingPendingSend and the bearer is requested to bring up the 
	connection.
	Whatever happens, ownership of the new message is retained. Ownership of 
	aMsg stays with the caller.
	@param aSync If aConn exists as a bearer-level connection, this is set 
	to ETrue, otherwise it is set to EFalse. It effectively indicates whether 
	a send attempt was made synchronously (ETrue) or if we'll have to wait for 
	a bearer-level connection to come up (EFalse). aSync is guaranteed to be 
	set correctly whether the function leaves or not.
	*/
	void SendCmdToRemoteL(const CRemConMessage& aMsg, const TRemConAddress& aConn, TBool& aSync);

	/** 
	Utility for delivering a single incoming command to a client session.
	Takes a copy of aMsg, sets its SessionId, and delivers it to aSess, 
	putting it on the correct queue. 
	Does not take ownership of aMsg.
	*/
	void DeliverCmdToClientL(const CRemConMessage& aMsg, CRemConTargetClientProcess& aClient);

	/** 
	Utility for delivering a single message to a controller client session.
	If the session aSess has an outstanding Receive request, completes the 
	request with aMsg. Otherwise, puts aMsg in the 'incoming pending delivery' 
	queue. 
	Always takes ownership of aMsg.
	@return KErrNone if the message was successfully delivered or put on the
	incoming pending delivered queue, otherwise one of the system wide error codes
	Ownership of aMsg will be taken regardless of the error.
	*/
	TInt DeliverMessageToClient(CRemConMessage& aMsg, CRemConControllerSession& aSess);

	/** 
	Utility for delivering a single message to a target client.
	If the client aClient has an outstanding Receive request, completes the 
	request with aMsg and puts aMsg the 'incoming delivered' queue (since it
	shall be a command). Otherwise, puts aMsg in the 'incoming pending delivery' 
	queue. 
	Always takes ownership of aMsg.
	@return KErrNone if the message was successfully delivered or put on the
	incoming pending delivered queue, otherwise one of the system wide error codes
	Ownership of aMsg will be taken regardless of the error.
	*/
	TInt DeliverMessageToClient(CRemConMessage& aMsg, CRemConTargetClientProcess& aClient);

	/** Gives the head outgoing command to the TSP for (a) addressing to 
	remote target(s), if its address is null, or (b) permission to send, if it 
	has a remote address. */
	void TspOutgoingCommand();
	
	void TspOutgoingNotifyCommand();
	
	/** Gives the head incoming command to the TSP for addressing to target 
	client(s). */
	void AddressIncomingCommand();
	
	/** Gives the head incoming notify command to the TSP for addressing to target 
	client. */
	void AddressIncomingNotifyCommand();

	/** Gives the head incoming notify command to the TSP for readdressing to target 
	client. */
	void ReAddressIncomingNotifyCommand();

	
	/** Gives the head outgoing response to the TSP for permission to send */
	void PermitOutgoingResponse();
	
	TClientInfo* TargetClientIdToClientInfo(TRemConClientId aId);
	
	void InitialiseBulkServerThreadL();

private: // called by the shutdown timer
	/** 
	Called by the shutdown timer when it fires. Terminates the server thread. 
	@param aThis This. 
	@return KErrNone.
	*/
	static TInt TimerFired(TAny* aThis);

private: // owned
	// Holds the history of the connection states of the system.
	CConnectionHistory* iConnectionHistory;

	// NB This isn't really being used as a CPeriodic, but (a) it will do the 
	// job, and (b) it saves us writing our own concrete timer (there aren't 
	// any other more suitable concrete timer classes).
	CPeriodic* iShutdownTimer;

	// Shutdown delay (when there are no sessions) = 1 second- there's no 
	// point hanging around.
	static const TUint KShutdownDelay = 1 * 1000 * 1000;
	
	// Not to be deleted, only closed (see ECOM API documentation).
	REComSession* iEcom; 

	CBearerManager* iBearerManager;
	CConverterManager* iConverterManager;

	// Unique identifier seed for sessions and clients.
	// Controller sessions, target clients and target sessions all need a unique ID.
	TUint iSessionOrClientId;

	RPointerArray<CRemConControllerSession> iControllerSessions;
	RPointerArray<CRemConTargetClientProcess> iTargetClients;
	mutable RNestableLock iTargetClientsLock; 

	// In the following discussion, mark carefully the difference between a 
	// QUEUE of items awaiting serialised access to a resource, and a LOG of 
	// things which have happened which we need to remember.
	// There are nine collections of messages in the server:
	// 1/ OutgoingCmdPendingTsp
	// This is a QUEUE of commands (a) from our connectionless controller 
	// clients, waiting to be addressed by the TSP, and (b) from our 
	// connection-oriented controller clients, waiting to be 'permitted' by 
	// the TSP. On sending, commands move off this queue to OutgoingPendingSend
	// 2/ OutgoingRspPendingTsp
	// This is a QUEUE of responses (a) from our clients waiting to be 
	// 'permitted' by the TSP. On sending, commands move off this queue to...
	// 3/ OutgoingPendingSend
	// This is a QUEUE of messages, waiting for the relevant 
	// bearer connection to exist so they can be sent. On sending, commands 
	// move off this queue to...
	// 4/ OutgoingSent
	// This is a LOG of commands sent by our controller clients. Items here 
	// have been given to bearers and are awaiting responses from remotes. We 
	// remember these commands in order to match responses to the controller 
	// session which sent the originating command.
	// 5/ IncomingCmdPendingAddress
	// This is a QUEUE of incoming commands (i.e. from remotes). Items here 
	// are waiting to be given to the TSP for addressing. On being addressed 
	// they move to IncomingPendingDelivery.
	// 6/ IncomingNotifyCmdPendingAddress
	// This is a QUEUE of incoming Notify commands (i.e. from remotes). Items here 
	// are waiting to be given to the TSP for addressing. On being addressed 
	// they move to Incoming PendingDelivery
	// 7/ IncomingNotifyCmdPendingReAddress
	// This is a QUEUE of incmoing Notify commands (i.e. from remotes). Items here 
	// are waiting to be given to the TSP for readdressing, after the TSP called
	// ReAddressNotifies. They are copied from IncomingDelivered, then the equivalent
	// command is removed from IncomingDelivered after the TSP has readdressed, and
	// moved to IncomingPendingDelivery
	// 8/ IncomingPendingDelivery
	// This is a QUEUE of incoming messages (commands and responses) waiting 
	// to be given to our clients, i.e. for the client to post a Receive 
	// request. (This is so that messages don't get dropped just because a 
	// client hasn't reposted Receive quickly enough.) On being delivered, 
	// responses are destroyed, but commands move to...
	// 9/ IncomingDelivered
	// This is a LOG of delivered commands. We remember them in order to 
	// address responses to the correct remote.
	CMessageQueue* iOutgoingCmdPendingTsp;
	CMessageQueue* iOutgoingNotifyCmdPendingTsp;
	CMessageQueue* iOutgoingRspPendingTsp;
	CMessageQueue* iOutgoingRspPendingSend;
	CMessageQueue* iOutgoingPendingSend;
	CMessageQueue* iOutgoingSent;
	
	CMessageQueue* iIncomingCmdPendingAddress;
	CMessageQueue* iIncomingNotifyCmdPendingAddress;
	CMessageQueue* iIncomingNotifyCmdPendingReAddress;
	CMessageQueue* iIncomingPendingDelivery;
	CMessageQueue* iIncomingDelivered;

	// NB IncomingPendingDelivery and IncomingDelivered would be more logical 
	// as members of the session class, as we know which session these 
	// messages are associated with. They are here in the server with the 
	// other queues in order to make the flow of messages through the whole 
	// system clearer and therefore easier to maintain.

	// The target selector plugin, which controls the flow of commands through 
	// the server.
	CRemConTargetSelectorPlugin* iTsp;
	// This is the object supplied by the TSP which implements the TSP API. 
	// This is the mandatory base value and is guaranteed to exist once the 
	// TSP has been load.
	MRemConTargetSelectorPluginInterface* iTspIf;

	// This is the object supplied by the TSP which implements the TSP V2 API. 
	// The TSP may not support this interface so RemCon must handle this value
	// being NULL.
	MRemConTargetSelectorPluginInterfaceV2* iTspIf2;
	
	// This is the object supplied by the TSP which implements the TSP V3 API.
	// The TSP may not support this interface, so RemCon must handle this 
	// value being NULL
	MRemConTargetSelectorPluginInterfaceV3* iTspIf3;

	// This is the object supplied by the TSP which implements the TSP V4 API.
	// If the TSP does not implement it itself this points to a stub object
	// implementing default behaviour, so it is guaranteed to exist once the TSP
	// has been loaded.
	MRemConTargetSelectorPluginInterfaceV4* iTspIf4;
	
	// This is the object supplied by the TSP which implements the TSP V5 API.
	// The TSP may not support this interface, so RemCon must handle this 
	// value being NULL
	MRemConTargetSelectorPluginInterfaceV5* iTspIf5;
	
	// Collections we pass to the TSP for it to indicate addressing 
	// information back to us.
	TSglQue<TRemConAddress> iTspConnections;
	TSglQue<TClientInfo> iTspIncomingCmdClients;
	TSglQue<TClientInfo> iTspIncomingNotifyCmdClients;

	// List of which clients each message was delivered to	
	CMessageRecipientsList* iMessageRecipientsList;
	
	// Flags to control serialised access to the three parts of the TSP API.
	TBool iTspHandlingOutgoingCommand;
	TBool iTspHandlingOutgoingNotifyCommand;
	TBool iTspAddressingIncomingCommand;
	TBool iTspAddressingIncomingNotifyCommand;
	TBool iTspReAddressingIncomingNotifyCommands;
	TBool iTspHandlingOutgoingResponse;

	// Flags to control when a command is expired while being handled in TSP, so RemCon
	// knows to remove them when the TSP is finished
	TBool iTspDropIncomingCommand;
	TBool iTspDropIncomingNotifyCommand;
	
	/** We have an array of these records, one for each session. At any point 
	in time, each session's record's iIndex is the index of the connection 
	history record in which that session is currently interested. */
	struct TSessionPointerToConnectionHistory
		{
		/** The session ID. */
		TUint iSessionId;

		/** The index of the connection history record. */
		TUint iIndex;
		};
	RArray<TSessionPointerToConnectionHistory> iSession2ConnHistory;
	
	TTspV4Stub iTspIf4Stub;

	TBool iBulkThreadOpen;
	RThread iBulkServerThread;
	RMsgQueue<TBulkServerMsg> iBulkServerMsgQueue;
	CBulkThreadWatcher*	iBulkThreadWatcher;
	};

// Inlines

NONSHARABLE_CLASS(CBulkThreadWatcher)
	: public CActive
	{
public:
	CBulkThreadWatcher(CRemConServer& aServer);
	~CBulkThreadWatcher();
	
	void StartL();
	
private: // from CActive
	void RunL();
	void DoCancel();
	
private: // unowned
	CRemConServer&	iServer;
	};

#endif // SERVER_H
