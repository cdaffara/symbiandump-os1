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

#ifndef REMCONBEARERBULKINTERFACE_H
#define REMCONBEARERBULKINTERFACE_H

#include <e32base.h>
#include <remcon/clientid.h>

class MRemConBearerBulkObserver;
class TBearerParams;
class TRemConAddress;

/** 
The UID of the bearer API. If the bearer API ever has to change, a new UID and 
associated M class will be created. New implementations of CRemConBearerPlugin 
may implement the new API. Old (non-updated) bearers will still work as long 
as RemCon supports the old API.
*/
const TInt KRemConBearerBulkInterface1 = 0x102863fc;

/**
Mixin for the bearer API.
*/
class MRemConBearerBulkInterface
	{
public:
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
	virtual TInt MrcbbiGetCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr) = 0;

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
	virtual TInt MrcbbiSendResponse(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint aTransactionId, 
		RBuf8& aData, 
		const TRemConAddress& aAddr) = 0;
	
	/**
	Called by RemCon when all appropriate clients disconnect from remcon before the 
	response is sent.
	@param aInterfaceUid The UID of the outer-layer client API that the command
	was sent to
	@param aOperationId The ID of the command operation sent to remcon
	@param aId The command identifier used as a cookie for command/response 
	matching.
	@param aAddr The connection.
	*/
	virtual void MrcbbiSendReject(TUid aInterfaceUid, 
			TUint aOperationId, 
			TUint aTransactionId, 
			const TRemConAddress& aAddr) = 0;
	
	/**
	Called by RemCon to initialise the bulk service.  This call will be
	made in the context of the bulk server thread.
	@param aObserver The bulk bearer observer.
	@return KErrNone if the bulk service was successfully intialised.
			System wide error code otherwise.
	*/
	virtual TInt MrcbbiStartBulk(MRemConBearerBulkObserver& aObserver) = 0;
	
	/**
	Called by RemCon to disable the bulk service.  This call will be made
	in the context of the bulk server thread.
	The bearer should use this opportunity to cancel any outstanding bulk
	services. 
	*/
	virtual void MrcbbiStopBulk() = 0;
	
	/**
	Called by RemCon when a bulk client becomes available.  The provided 
	client ID will be shared with the associated control client.
	@param aId The ID of the bulk client that has become available.
	*/
	virtual void MrcbbiBulkClientAvailable(const TRemConClientId& aId) = 0;
	
	/**
	Called by RemCon when a bulk client is no longer available.
	@param aId The ID of the bulk client that is no longer available.
	*/
	virtual void MrcbbiBulkClientNotAvailable(const TRemConClientId& aId) = 0;
	};
	
#endif // REMCONBEARERBULKINTERFACE_H
