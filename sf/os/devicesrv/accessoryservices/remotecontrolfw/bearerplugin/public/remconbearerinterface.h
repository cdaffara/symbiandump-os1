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
 @publishedPartner
 @released
*/

#ifndef REMCONBEARERINTERFACE_H
#define REMCONBEARERINTERFACE_H

#include <e32base.h>
#include <remcon/clientid.h>
#include <remcon/messagetype.h>
#include <remcon/playertype.h>

class MRemConBearerObserver;
class TBearerParams;
class TRemConAddress;

/** 
The UID of the bearer API. If the bearer API ever has to change, a new UID and 
associated M class will be created. New implementations of CRemConBearerPlugin 
may implement the new API. Old (non-updated) bearers will still work as long 
as RemCon supports the old API.
*/
const TInt KRemConBearerInterface1 = 0x10208A78;

/**
Mixin for the bearer API.
*/
class MRemConBearerInterface
	{
public:
	/** 
	Called by RemCon to retrieve a response on a connection. Must only be 
	called as a result of a NewResponse up-call.
	@param aInterfaceUid The UID of the outer-layer client API specifying the 
	response.
	@param aTransactionId The command identifier used as a cookie for command/response 
	matching.
	@param aOperationId The ID of the response operation in the outer-layer 
	client API.
	@param aData API-specific message data. On success, ownership is 
	returned.
	@param aAddr The connection.
	@return Error.
	*/
	virtual TInt GetResponse(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr) = 0;
	
	/** 
	Called by RemCon to retrieve a command on a connection. Must only be 
	called as a result of a NewCommand up-call.
	@param aInterfaceUid The UID of the outer-layer client API specifying the 
	command.
	@param aTransactionId The command identifier used as a cookie for command/response 
	matching.
	@param aOperationId The ID of the command operation in the outer-layer 
	client API.
	@param aData API-specific message data. On success, ownership is 
	returned.
	@param aAddr The connection.
	@return Error.
	*/
	virtual TInt GetCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr) = 0;

	/** 
	Called by RemCon to send a command on a connection. The connection is not 
	assumed to exist- the bearer is responsible for bringing up the requested 
	connection if necessary.
	@param aInterfaceUid The UID of the outer-layer client API specifying the 
	command.
	@param aOperationId The ID of the command operation in the outer-layer 
	client API.
	@param aTransactionId The command identifier used as a cookie for command/response 
	matching.
	@param aData API-specific message data. On success, ownership is passed. 
	@param aAddr The connection.
	@return Error. This request is synchronous. It should be completed by the 
	bearer when it has taken responsibility for sending the message. This will 
	involve checking that the message is well-formed, and possibly actually 
	trying to send it, or adding it to a queue.
	*/
	virtual TInt SendCommand(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint aTransactionId,  
		RBuf8& aData, 
		const TRemConAddress& aAddr) = 0;
	
	/** 
	Called by RemCon to send a response on a connection. The connection is not 
	assumed to exist- the bearer is responsible for bringing up the requested 
	connection if necessary.
	@param aInterfaceUid The UID of the outer-layer client API specifying the 
	response.
	@param aOperationId The ID of the response operation in the outer-layer 
	client API.
	@param aTransactionId The command identifier used as a cookie for command/response 
	matching
	@param aData API-specific message data. On success, ownership is passed.
	@param aAddr The connection.
	@return Error. This request is synchronous. It should be completed by the 
	bearer when it has taken responsibility for sending the message. This will 
	involve checking that the message is well-formed, and possibly actually 
	trying to send it, or adding it to a queue.
	*/
	virtual TInt SendResponse(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint aTransactionId, 
		RBuf8& aData, 
		const TRemConAddress& aAddr) = 0;
	
	/** 
	Called by RemCon to establish a bearer-level connection to another party. 
	Completion is signalled back in ConnectConfirm.
	@param aAddr The RemCon address to connect to.
	*/
	virtual void ConnectRequest(const TRemConAddress& aAddr) = 0;
	
	/** 
	Called by RemCon to destroy a bearer-level connection to another party. 
	Completion is signalled back in DisconnectConfirm.
	@param aAddr The RemCon address to disconnect from.
	*/
	virtual void DisconnectRequest(const TRemConAddress& aAddr) = 0;

	/**
	Called by RemCon when either (a) the number of controller clients changes 
	from 0 to 1 or from 1 to 0, or (b) the number of target clients changes 
	from 0 to 1 or from 1 to 0.
	@param aControllerPresent Is true if any controllers are present, EFalse otherwise.
	@param aTargetPresent Is true if any targets are present, EFalse otherwise.
	*/
	virtual void ClientStatus(TBool aControllerPresent, TBool aTargetPresent) = 0;

	/**
	Called by RemCon to get the capabilities required to make/destroy 
	connections over the bearer, and to send and receive messages over the 
	bearer.
	@return The bearer's security policy.
	*/
	virtual TSecurityPolicy SecurityPolicy() const = 0;
	};
	
	
/** 
The UID of the bearer API. If the bearer API ever has to change, a new UID and 
associated M class will be created. New implementations of CRemConBearerPlugin 
may implement the new API. Old (non-updated) bearers will still work as long 
as RemCon supports the old API.
*/
const TInt KRemConBearerInterface2 = 0x10285AD9;

	
class MRemConBearerInterfaceV2 : public MRemConBearerInterface
	{
public:
	/** 
	Called by RemCon to retrieve a notify command on a connection. Must only be 
	called as a result of a NewNotify up-call.
	@param aInterfaceUid The UID of the outer-layer client API specifying the 
	notify command.
	@param aTransactionId The command identifier used as a cookie for command/response 
	matching.
	@param aOperationId The ID of the command operation in the outer-layer 
	client API.
	@param aData API-specific message data. On success, ownership is 
	returned.
	@param aAddr The connection.
	*/
	virtual TInt GetNotifyCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr) = 0;
	
	/**
	Called by RemCon when either
	a) The TSP does not address a command to any clients
	b) The TSP does not permit the response from any commands
	c) All the clients disconnect from remcon before the response is sent,
	to send a reject on a connection. RemCon will call this function after bringing
	up the connection.
	@param aInterfaceUid The UID of the outer-layer client API that the command
	was sent to
	@param aOperationId The ID of the command operation sent to remcon
	@param aTransactionId The command identifier used as a cookie for command/response 
	matching.
	@param aAddr The connection.
	*/
	virtual void SendReject(TUid aInterfaceUid, 
			TUint aOperationId, 
			TUint aTransactionId, 
			const TRemConAddress& aAddr) = 0; 
	};	

/** 
The UID of the bearer API. If the bearer API ever has to change, a new UID and 
associated M class will be created. New implementations of CRemConBearerPlugin 
may implement the new API. Old (non-updated) bearers will still work as long 
as RemCon supports the old API.
*/
const TInt KRemConBearerInterface3 = 0x10285ADB;

class MRemConBearerInterfaceV3 : public MRemConBearerInterfaceV2
	{
public:
	/** 
	Called by RemCon to send a notify command on a connection. The connection is not 
	assumed to exist- the bearer is responsible for bringing up the requested 
	connection if necessary.
	@param aInterfaceUid The UID of the outer-layer client API specifying the command.
	@param aOperationId The ID of the command operation in the outer-layer client API.
	@param aTransactionId The command identifier used as a cookie for command/response matching.
	@param aData API-specific message data. On success, ownership is passed. 
	@param aAddr The connection.
	@return Error. This request is synchronous. It returns KErrNone when the 
	bear has taken responsibility for sending the message. This will 
	involve checking that the message is well-formed, and possibly actually 
	trying to send it, or adding it to a queue.
	*/
	virtual TInt SendNotifyCommand(TUid aInterfaceUid, 
			TUint aOperationId, 
			TUint aTransactionId,  
			RBuf8& aData, 
			const TRemConAddress& aAddr) = 0;
	
	/** 
	Called by RemCon to retrieve a notify response on a connection. Must only be 
	called as a result of a NewNotifyResponseL up-call.
	@param aInterfaceUid The UID of the outer-layer client API specifying the response.
	@param aTransactionId The command identifier used as a cookie for command/response matching.
	@param aOperationId The ID of the response operation in the outer-layer client API.
	@param aData API-specific message data. On success, ownership is returned.
	@param aAddr The connection.
	@return Error.
	*/
	virtual TInt GetNotifyResponse(TUid& aInterfaceUid, 
			TUint& aId, 
			TUint& aOperationId, 
			RBuf8& aCommandData, 
			TRemConAddress& aAddr,
			TRemConMessageSubType& aSubMessageType)=0;
	
	/**
	Called by RemCon when a client has become available for addressing.  Once this
	call has been made the bearer may use the provided TRemConClientId to address
	incoming commands and notifys to this client until RemCon calls ClientNotAvailable
	with this TRemConClientId.
	
	@param aId A unique identifier for this client, that can be used when addressing 
	incoming commands.
	@param aClientType The basic type of this client
	@param aClientSubType More detailed type information on this client
	@param aName The name of this client in UTF-8.  This remains valid until ClientNotAvailable 
				 is called for this player.
	*/
	virtual void ClientAvailable(TRemConClientId& aId, TPlayerType aClientType, TPlayerSubType aClientSubType, const TDesC8& aName) = 0;
	
	/**
	Called by RemCon when a client is no longer available for addressing.  Once this
	call has been made the bearer shall not use this client id when addressing incoming
	commands and notifys until informed that the client is available again via
	ClientAvailable.
	
	@param aId The client that has ceased to be available.
	*/
	virtual void ClientNotAvailable(TRemConClientId& aId) = 0;
	
	/** 
	Called by RemCon when the TSP has requested this bearer use a different addressed
	client.
	
	@param aId The client to which this bearer should route addressed commands.
	*/
	virtual TInt SetLocalAddressedClient(TRemConClientId& aId) = 0;
	
	/**
	Called by RemCon when a controller client is opened or closed to provide the 
	current set of interfaces supported by controller sessions.  This is not
	guaranteed to have changed since the last time the function was called.
	
	@param aSupportedInterfaces An RArray of interface UIDs.  Ownership is retained
				by RemCon and its lifetime is not guaranteed to last beyond the scope
				of this function.  Each supported interface appears once in the array
				irrespective of how many controller sessions support that interface.
	*/
	virtual void ControllerFeaturesUpdated(RArray<TUid>& aSupportedInterfaces) = 0;
	
	/**
	Called by RemCon when a target client registers new interfaces after being made available.

	@param aId The target client that has regitered new interfaces.
	@param aPlayerType The basic type of this client after registration of new interfaces.
	@param aPlayerSubType More detailed type information on this client after registration of new interfaces.
	@param aName The name of this client in UTF-8. This remains valid until ClientNotAvailable 
				 is called for this player.
	*/
	virtual void TargetFeaturesUpdated(const TRemConClientId& aId, TPlayerType aPlayerType, TPlayerSubType aPlayerSubType, const TDesC8& aName) = 0;
	};

#endif // REMCONBEARERINTERFACE_H
