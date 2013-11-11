// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONBEAREROBSERVER_H
#define REMCONBEAREROBSERVER_H

#include <e32base.h>
#include <remcon/messagetype.h>
#include <remcon/clientid.h>

class CRemConConverterPlugin;
class TRemConAddress;

/**
Interface presented by RemCon down to bearers.
The public methods are non-virtual and exported, so that they can be added to 
without breaking BC for existing (non-rebuilt) bearers.
*/
class MRemConBearerObserver
	{
public:
	/** 
	Called when an incoming response from a remote is ready to be picked up by 
	RemCon.
	@param aAddr The address the response came from.
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewResponse(const TRemConAddress& aAddr);	
	
	/** 
	Called when an incoming notify response from a remote is ready to be picked up by 
	RemCon.
	@param aAddr The address the response came from.
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetNotifyResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewNotifyResponse(const TRemConAddress& aAddr);	
	
	/** 
	Called when an incoming command from a remote is ready to be picked up by 
	RemCon.
	@param aAddr The address the command came from.
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewCommand(const TRemConAddress& aAddr);

	/** 
	Called when an incoming notify command from a remote is ready to be picked up by 
	RemCon.  A Notify command is different to a normal command in that it doesn't
	follow the standard request / response sequence.  Instead a notify is registered
	to retrieve the current state value in an interim response, then when the state
	has changed relative the interim response a second response is sent containing the
	new, changed, state.
	
	Depending on the semantics of the bearer protocol it may be able to map its 
	state observation commands to plain RemCon commands, however for a reliable 
	mechanism without polling this NotifyCommand is provided.  
	
	@param aAddr The address the command came from.
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewNotifyCommand(const TRemConAddress& aAddr);
	
	/** 
	Called by a bearer when an incoming connection has been established. 
	@param aAddr The address of the connection.
	@return Error. If RemCon cannot add the connection to its internal data, 
	then the bearer must logically drop the connection.
	*/
	IMPORT_C TInt ConnectIndicate(const TRemConAddress& aAddr);
	
	/**
	Called by a bearer when a connection has been disconnected from the remote 
	end. 
	The bearer should only call this function if either (a) ConnectIndicate 
	has already been called for the connection aAddr (and KErrNone returned by 
	RemCon), or (b) ConnectConfirm has been called with KErrNone for the 
	connection aAddr (and KErrNone has been returned by RemCon).
	@param aAddr The RemCon address of the connection.
	*/
	IMPORT_C void DisconnectIndicate(const TRemConAddress& aAddr);
	
	/**
	Called by a bearer to indicate completion of an outgoing connection 
	request (CRemConBearerPlugin::ConnectRequest).
	@param aAddr The address of the connection in question.
	@param aError The success status of the connection establishment. If 
	KErrNone, the connection was established. If non-KErrNone, the connection 
	was not established.
	@return Error. If RemCon cannot add the connection to its internal data, 
	then the bearer must logically drop the connection. If aError is not 
	KErrNone, then this return value is irrelevant.
	*/
	IMPORT_C TInt ConnectConfirm(const TRemConAddress& aAddr, TInt aError);
	
	/**
	Called by a bearer to indicate completion of a disconnection request 
	(CRemConBearerPlugin::Disconnect).
	@param aAddr The address of the connection in question.
	@param aError The success status of the disconnection. If KErrNone, the 
	disconnection occurred. If non-KErrNone, the connection still exists.
	*/
	IMPORT_C void DisconnectConfirm(const TRemConAddress& aAddr, TInt aError);

	/**
	Called by a bearer to convert a message from an outer-layer API format to 
	the bearer's format. The bearer observer finds a converter to perform 
	this. 
	@param aBearerUid The UID of the bearer.
	@param aInterfaceUid The UID of the outer layer API to which the message 
	belongs.
	@param aOperationId The operation ID of the message.
	@param aData The operation-specific data of the message.
	@param aMsgType The type of the message.
	@param aBearerData On success, the message encoded in the bearer's format.
	@return Error. If a converter could not be found, KErrNotSupported, 
	indicating that the message/bearer combination is not supported by the 
	system.
	*/
	IMPORT_C TInt InterfaceToBearer(TUid aBearerUid,
		TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const;
	
	/**
	Called by a bearer to convert a message from the bearer's format to that 
	of an outer-layer API. The bearer observer finds a converter to perform 
	this. 
	@param aBearerUid The UID of the bearer.
	@param aInterfaceData Data identifying the interface in bearer-specific 
	format.
	@param aBearerData The message encoded in the bearer's format.
	@param aInterfaceUid On success, the UID of the outer layer API to which 
	the message belongs.
	@param aOperationId On success, the operation ID of the message.
	@param aData On success, the operation-specific data of the message.
	@param aMsgType On success, the type of the message.
	@return Error. If a converter could not be found, KErrNotSupported, 
	indicating that the message/bearer combination is not supported by the 
	system.
	*/
	IMPORT_C TInt BearerToInterface(TUid aBearerUid,
		const TDesC8& aInterfaceData, 
		const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const;

	/**
	Called by a bearer when a new command has come in. RemCon returns a cookie 
	(a transaction id), guaranteed to be unique, which the bearer may use for 
	its own identification purposes.
	@return A new transaction ID.
	*/
	IMPORT_C TUint NewTransactionId();
	
	/**
	Called by a bearer when a command is no longer valid and should be removed
	from RemCon's queues
	@param aTransactionId The transaction ID of the expired command
	*/
	IMPORT_C void CommandExpired(TUint aTransactionId);
	
	/** 
	Called when an incoming command from a remote is ready to be picked up by 
	RemCon.
	
	This overload is provided for the case where the bearer supports command 
	addressing.
	
	@param aAddr The address the command came from.
	@param aTarget The application to which the command is targeted
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient);

	/** 
	Called when an incoming notify command from a remote is ready to be picked up by 
	RemCon.  A Notify command is different to a normal command in that it doesn't
	follow the standard request / response sequence.  Instead a notify is registered
	to retrieve the current state value in an interim response, then when the state
	has changed relative the interim response a second response is sent containing the
	new, changed, state.
	
	Depending on the semantics of the bearer protocol it may be able to map its 
	state observation commands to plain RemCon commands, however for a reliable 
	mechanism without polling this NotifyCommand is provided.  
	
	This overload is provided for the case where the bearer supports command
	addressing.
	
	@param aAddr The address the command came from.
	@param aTarget The application to which the command is targeted
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewNotifyCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient);

	/**
	Called by the bearer when it want to retrieve which interfaces are supported by a 
	specific client.
	@param aId A unique identifier for this client
	@param aUids An RArray to be populated with the supported UIDs.  On return with 
			KErrNone this array is populated with the UIDs of the interfaces which 
			this client supports.  Ownership of the RArray remains with the caller.
			Any existing entries in the array will be removed.
	@return KErrNone on successful retrieval of supported interfaces.  System wide
			error code otherwise.
	*/
	IMPORT_C TInt SupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids) ;

	/**
	Called by the bearer when it wants to retrieve which operations within the interface
	are supported by this client.  Note that this information is not available for all
	interface and client combinations.
	
	@param aId A unique identifier for this client
	@param aInterfaceUid The interface to return the supported operations for
	@param aOperations An RArray to be populated with the supported operations.  
			On return with KErrNone this array is populated with the operation
			ids of each supported operations within the requested interface.  
			Ownership of the RArray remains with the caller.  Any existing 
			entries in the array will be removed.
	@return KErrNone if the supported operations could be successfully retrieved.
			KErrNotSupported if the operations could not be retrieved for this 
			particular client/interface combination.  System wide error code
			otherwise.
	*/
	IMPORT_C TInt SupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations);

	/** 
	Called by the bearer to inform RemCon that remote user action means that
	this bearer will now route addressed commands to the specified client.
	This is valid until either the bearer calls SetRemoteAddressedPlayer again
	or RemCon calls SetLocalAddressedPlayer.
	
	@param aBearerUid The Uid of this bearer
	@param aId The client to which this bearer will route addressed commands.
	*/
	IMPORT_C void SetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId);
	
	/**
	Called by the bearer to indicate it would like to be informed when the 
	locally addressed client changes.
	*/
	IMPORT_C TInt RegisterLocalAddressedClientObserver(const TUid& aBearerUid);
	
	/**
	Called by the bearer to indicate it would no longer like to be informed when 
	the locally addressed client changes.
	*/
	IMPORT_C TInt UnregisterLocalAddressedClientObserver(const TUid& aBearerUid);
	
private:
	/** 
	@see NewResponse.
	*/
	virtual TInt MrcboDoNewResponse(const TRemConAddress& aAddr) = 0;	
	
	/** 
	@see NewNotifyResponse.
	*/
	virtual TInt MrcboDoNewNotifyResponse(const TRemConAddress& aAddr) = 0;
	
	/** 
	@see NewCommand.
	*/
	virtual TInt MrcboDoNewCommand(const TRemConAddress& aAddr) = 0;
	
	/** 
	@see NewNotifyCommand.
	*/
	virtual TInt MrcboDoNewNotifyCommand(const TRemConAddress& aAddr) = 0;

	/** 
	@see ConnectIndicate.
	*/
	virtual TInt MrcboDoConnectIndicate(const TRemConAddress& aAddr) = 0;
	
	/**
	@see DisconnectIndicate.
	*/
	virtual void MrcboDoDisconnectIndicate(const TRemConAddress& aAddr) = 0;
	
	/**
	@see ConnectConfirm.
	*/
	virtual TInt MrcboDoConnectConfirm(const TRemConAddress& aAddr, TInt aError) = 0;
	
	/**
	@see DisconnectConfirm.
	*/
	virtual void MrcboDoDisconnectConfirm(const TRemConAddress& aAddr, TInt aError) = 0;
	
	/**
	@see InterfaceToBearer.
	*/
	virtual TInt MrcboDoInterfaceToBearer(TUid aBearerUid,
		TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const = 0;
	
	/**
	@see BearerToInterface.
	*/
	virtual TInt MrcboDoBearerToInterface(TUid aBearerUid,
		const TDesC8& aInterfaceData, 
		const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const = 0;
	
	/**
	@see TransactionId.
	*/
	virtual TUint MrcboDoNewTransactionId() = 0;	
	
	/**
	@see CommandExpired.
	*/
	virtual void MrcboDoCommandExpired(TUint aTransactionId) = 0;	
	
	/**
	@see NewCommand
	*/
	virtual TInt MrcboDoNewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient) = 0;

	/** 
	@see NewNotifyCommand
	*/
	virtual TInt MrcboDoNewNotifyCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient) = 0;

	/**
	@see SupportedInterfaces
	*/
	virtual TInt MrcboDoSupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids) = 0;

	/**
	@see SupportedOperations
	*/
	virtual TInt MrcboDoSupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations) = 0;

	/**
	@see SetRemoteAddressedClient
	*/
	virtual void MrcboDoSetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId) = 0;
	
	/**
	@see RegisterLocalAddressedClientObserver
	*/
	virtual TInt MrcboDoRegisterLocalAddressedClientObserver(const TUid& aBearerUid) = 0;
	
	/**
	@see UnregisterLocalAddressedClientObserver
	*/
	virtual TInt MrcboDoUnregisterLocalAddressedClientObserver(const TUid& aBearerUid) = 0;
	};

#endif // REMCONBEAREROBSERVER_H
