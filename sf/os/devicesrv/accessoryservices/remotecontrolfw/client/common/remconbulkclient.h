// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Remote Control bulk client side.
//



/**
 @file
 @internalComponent
*/

#ifndef REMCONBULKCLIENT_H
#define REMCONBULKCLIENT_H

#include <e32base.h>
#include <remcon/messagetype.h>
#include "remconserver.h"

/**
The abstract base class for RemCon session handles.
*/
NONSHARABLE_CLASS(RRemConBulk) : public RSessionBase
	{
public:
	IMPORT_C RRemConBulk();
	
	/**
	Connect the handle to the server.
	Must be called before all other methods (except Version and Close).
	@return Error.
	*/
	IMPORT_C TInt Connect();

	/**
	Getter for the version of the server.
	@return Version of the server.
	*/
	IMPORT_C TVersion Version() const;

	/**
	Sends a message (command or response) to the remote device.
	Note that currently only responses are supported, but the API
	is generic enough for both types of message.
	@param aStatus TRequestStatus for asynchronous completion.
	@param aInterfaceUid The UID of the interface to which the message 
	belongs.
	@param aOperationId The ID of the message. RemCon needs to know this, 
	separately from the arbitrary data, so it can (a) match up any incoming 
	response to this client (if the message is a command), and (b) match this 
	message up to the target (if this message is a response).
	@param aData Data associated with the message.
	*/
	IMPORT_C void Send(TRequestStatus& aStatus, 
		TUid aInterfaceUid, 
		TUint aOperationId, 
		const TDesC8& aData = KNullDesC8());
	
	/**
	Sends a message (command or response) unreliably to the remote device.
	Note that currently only reponses are supported, by the API is 
	generic enough for both types of message.
	@param aInterfaceUid The UID of the interface to which the message 
	belongs.
	@param aOperationId The ID of the message. RemCon needs to know this, 
	separately from the arbitrary data, so it can (a) match up any incoming 
	response to this client (if the message is a command), and (b) match this 
	message up to the target (if this message is a response).
	@param aData Data associated with the message.
	@return Error - this is the first point of error.  A message may error
	after this function returns (that error will not be reported).
	*/
	IMPORT_C TInt SendUnreliable(TUid aInterfaceUid, 
			TUint aOperationId,
			const TDesC8& aData = KNullDesC8());

	/**
	Cancels interest in the completion of an outstanding Send operation.
	@return KErrNone.
	*/
	IMPORT_C TInt SendCancel();

	/**
	Receive a message (command or response) from the remote device. Note that 
	RemCon server queues both commands and responses so that none are ever 
	thrown away just because the client didn't have a Receive outstanding when 
	they arrived.
	@param aStatus TRequestStatus for asynchronous completion.
	@param aInterfaceUid The UID of the interface to which the message 
	belongs.
	@param aOperationId The ID of the message.
	@param aData Data associated with the message.
	*/
	IMPORT_C void Receive(TRequestStatus& aStatus, 
		TUid& aInterfaceUid,
		TUint& aOperationId,
		TDes8& aData);

	/**
	Cancels interest in the completion of an outstanding Receive operation.
	@return KErrNone.
	*/
	IMPORT_C TInt ReceiveCancel();

	/**
	Marks the start of heap cell checking in the server's heap. In release 
	builds, just returns KErrNone.
	@return Error.
	*/
	IMPORT_C TInt __DbgMarkHeap();

	/**
	Checks that the number of allocated cells on the server's heap is correct. 
	The server is panicked if not. In release builds, just returns KErrNone.
	@param aCount The expected number of allocated heap cells.
	@return Error.
	*/
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);

	/**
	Marks the end of heap cell checking. Checks that the number of heap cells 
	allocated since the last __DbgMarkHeap() is aCount; the most common value 
	to pass here is zero. In release builds, just returns KErrNone.
	@param aCount The expected number of allocated heap cells.
	@return Error.
	*/
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);

	/**
	Simulates memory allocation failure in the server. In release builds, just 
	returns KErrNone.
	@param aCount The number of allocations after which memory allocation 
	should fail.
	@return Error.
	*/
	IMPORT_C TInt __DbgFailNext(TInt aCount);

private: // owned
	/** 
	Used by Send.
	*/
	TPckgBuf<TOperationInformation> iOpInfoPckg;
	
	/** 
	Used by Receive.
	*/
	TPckg<TUint> iUidPckg;
	TPckg<TUint> iOpIdPckg;
	};

#endif // REMCONBULKCLIENT_H
