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
 @internalTechnology
 @released
*/

#ifndef REMCONBEARERBULKOBSERVER_H
#define REMCONBEARERBULKOBSERVER_H

#include <e32base.h>
#include <remcon/messagetype.h>
#include <remcon/clientid.h>

class TRemConAddress;

/**
Interface presented by RemCon down to bulk bearers.
The public methods are non-virtual and exported, so that they can be added to 
without breaking BC for existing (non-rebuilt) bearers.
*/
class MRemConBearerBulkObserver
	{
public:
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
	Called when an incoming command from a remote is ready to be picked up by 
	RemCon.  This overload is used when the command is for a stateless
	interface and must be delivered to a particular client.
	
	@param aAddr The address the command came from.
	@param aClientId The client to deliver the command to.
	@return Error. If KErrNone, RemCon is committing to collecting the message 
	using GetResponse. If non-KErrNone, the message will be dropped by the 
	bearer.
	*/
	IMPORT_C TInt NewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	
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
	Called by a bearer to select the client to which commands should be addressed.
	@param aAddr	The address for received commands to be delivered to a specific client.
	@param aClient	The client to which commands received from the given address should 
					be delievered to.
	*/
	IMPORT_C TInt SetAddressedClient(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	
	/**
	Called by a bearer to remove any addressing to particular clients for an address.
	@param aAddr	The address of received commands that are no longer to be delivered to
					specified clients.
	*/
	IMPORT_C TInt RemoveAddressing(const TRemConAddress& aAddr);
	
private:
	/** 
	@see NewCommand.
	*/
	virtual TInt MrcbboDoNewCommand(const TRemConAddress& aAddr) = 0;
	
	/** 
	@see NewCommand.
	*/
	virtual TInt MrcbboDoNewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient) = 0;
	
	/**
	@see TransactionId.
	*/
	virtual TUint MrcbboDoNewTransactionId() = 0;	
	
	/**
	@see CommandExpired.
	*/
	virtual void MrcbboDoCommandExpired(TUint aTransactionId) = 0;	
	
	/**
	@see SetAddressedClient.
	*/
	virtual TInt MrcbboDoSetAddressedClient(const TRemConAddress& aAddr, const TRemConClientId& aClient) = 0;
	
	/**
	@see RemoveAddressing.
	*/
	virtual TInt MrcbboDoRemoveAddressing(const TRemConAddress& aAddr) = 0;
	};

#endif // REMCONBEARERBULKOBSERVER_H
