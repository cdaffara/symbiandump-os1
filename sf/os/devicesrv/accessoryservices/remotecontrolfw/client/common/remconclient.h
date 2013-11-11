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
// Remote Control client side.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef REMCONCLIENT_H
#define REMCONCLIENT_H

#include <remcon/clienttype.h>
#include <remcon/messagetype.h>
#include <remcon/playertype.h>
#include "operationinformation.h"
#include "remconserver.h"

class TRemConAddress;

/**
The abstract base class for RemCon session handles.
*/
NONSHARABLE_CLASS(RRemCon) : public RSessionBase
	{
public:
	/**
	Connect the handle to the server.
	Must be called before all other methods (except Version and Close).
	@return Error.
	*/
	IMPORT_C TInt Connect();

	/**
	Connect the handle to the server.
	Must be called before all other methods (except Version and Close).
	@param aPlayerType  The type of client player
	@param aPlayerSubType The sub-type of the client player
	@param aName The name of client player
	@return Error.
	*/
	IMPORT_C TInt Connect(const TPlayerType& aPlayerType, const TPlayerSubType& aPlayerSubType, const TDesC8& aName);
	/**
	Getter for the version of the server.
	@return Version of the server.
	*/
	virtual TVersion Version() const = 0;

	/**
	Sends a message (command or response) to the remote device.
	@param aStatus TRequestStatus for asynchronous completion.
	@param aInterfaceUid The UID of the interface to which the message 
	belongs.
	@param aOperationId The ID of the message. RemCon needs to know this, 
	separately from the arbitrary data, so it can (a) match up any incoming 
	response to this client (if the message is a command), and (b) match this 
	message up to the target (if this message is a response).
	@param aNumRemotes On success only, the number of remotes the message was 
	successfully sent to (at the bearer level). If the message is a command 
	from a connection-oriented controller, then on success aNumRemotes will be 
	1. [For consistency, this pattern holds if the message is a response, even 
	though the information is redundant.] If the message is a command from a 
	connectionless controller, then aNumRemotes will be zero or more, 
	depending on what the TSP said should be done with the message and how 
	many of the TSP-nominated bearers successfully sent the message.
	@param aData Data associated with the message.
	*/
	IMPORT_C void Send(TRequestStatus& aStatus, 
		TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint& aNumRemotes,
		TRemConMessageSubType aSubType,
		const TDesC8& aData = KNullDesC8());
	
	IMPORT_C void SendNotify(TRequestStatus& aStatus, 
			TUid aInterfaceUid, 
			TUint aOperationId, 
			TRemConMessageSubType aSubType,
			const TDesC8& aData = KNullDesC8());
	
	IMPORT_C TInt SendUnreliable(	TUid aInterfaceUid, 
			TUint aOperationId,
			TRemConMessageSubType aSubType,
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
	@param aReceivePackage A struct containing the received message.
	@param aData Data associated with the message.
	*/
	IMPORT_C void Receive(TRequestStatus& aStatus, 
		TRemConClientReceivePackage& aReceivePackage,
		TDes8& aData);

	/**
	Cancels interest in the completion of an outstanding Receive operation.
	@return KErrNone.
	*/
	IMPORT_C TInt ReceiveCancel();

	/**
	Getter for the current set of connections in the system (not just those 
	associated with this session). The client is responsible for cleaning up 
	the collection- the addresses are on the client's heap.
	@param aConnections A collection of remote addresses, representing all the 
	currently extant connections. Must be empty when this function is called.
	@return Error.
	*/
	IMPORT_C TInt GetConnections(TSglQue<TRemConAddress>& aConnections);

	/**
	Notification for changes in the set of connections.
	This completes whenever the set of connections changes in some way. 
	If they wish to know what specifically changed, the client must call 
	GetConnections and do their own analysis of the results from that.
	Changes to the connection history of the system are logged internally so 
	that the client will not 'miss' any changes by not reposting the 
	notification quickly enough. However, if more than one bearer-level 
	connection change occurs in the server before the notification is reposted 
	by the client, then the following notification completion may 'cover' more 
	than one actual state change. 
	@param aStatus TRequestStatus for asynchronous completion.
	*/
	IMPORT_C void NotifyConnectionsChange(TRequestStatus& aStatus);

	/**
	Cancels interest in the completion of an outstanding 
	NotifyConnectionsChange operation.
	@return KErrNone.
	*/
	IMPORT_C TInt NotifyConnectionsChangeCancel();

	/** 
	 Tells the server in which APIs the client is interested. The server 
	 will only deliver incoming commands of these APIs to the client.
	 @param aNumAPIs The number of APIs to be registered
	 @param aAPIs A concatenated string of UIDs for the interfaces present. 
	 @return The error code returned from the server.
	 */
	IMPORT_C TInt RegisterInterestedAPIs(const TDesC8& aAPIs);
	
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

protected:
	/**
	@param aType The type of the session.
	*/
	RRemCon(TRemConClientType aType);

private: // utility
	TInt DoConnect();
	TInt SetPlayerType(const TPlayerType& aPlayerType, const TPlayerSubType& aPlayerSubType, const TDesC8& aName);

private: // owned
	const TRemConClientType iClientType;

	/** 
	Used by Send.
	*/
	TPckg<TUint> iNumRemotesPckg;
	TPckgBuf<TOperationInformation> iOpInfoPckg;
	
	/** 
	Used by Receive.
	*/
	TPckg<TRemConClientReceivePackage> iReceivePckg;
	TPckgBuf<TPlayerTypeInformation> iPlayerTypePckg;
	};

/**
The concrete session class for RemCon controllers.
Controller sessions are connectionless when opened. This means that addressing 
of commands is done by the Target Selector Plugin (TSP). A controller may 
alternatively be connection-oriented, which means that addressing of commands 
is done using a member of the server-side session which specifies a connection 
to a remote device. [NB Just because a session 'points to' a connection in 
this way does not means that the connection necessarily exists at the bearer 
level or at any other level.]
To make a controller session connection-oriented, call GoConnectionOriented. 
On success, the session's remote address member will have been set to the 
requested remote address.
To make a session connectionless again, use GoConnectionless. On success, the 
remote address member will be null, indicating that the TSP will be used to 
address our commands.
To control bearer-level connections, use ConnectBearer and DisconnectBearer. 
Note that real connections may, depending on the bearer, be torn down by the 
remote end outside of our control. Use GetConnections (and the associated 
notification) to get information about the current state of the real 
connections. Note however that the client is not _required_ to be interested 
in this level of control as RemCon is responsible for making sure the required 
connection exists at the bearer level before sending the message. The level of 
control mentioned is provided to the client so that it can, for instance, 
ensure adequate responsiveness of the first command sent.
ConnectBearerCancel and DisconnectBearerCancel merely cancel interest in the 
corresponding request. They do not change the state of the system, bearers, 
connections, or member data in any other way. They operate as pure Symbian OS 
asynchronous cancel methods.
*/
NONSHARABLE_CLASS(RRemConController) : public RRemCon
	{
public:
	IMPORT_C RRemConController();

	/**
	Makes the session connection-oriented. On success, the given connection 
	data will be used for sending commands.
	@param aBearerUid The UID of the bearer to use.
	@param aData Optional bearer-specific connection data.
	@return Error.
	*/
	IMPORT_C TInt GoConnectionOriented(const TRemConAddress& aConnection);

	/**
	Makes the session connectionless. On success, the TSP will be used for 
	sending commands.
	@return Error.
	*/
	IMPORT_C TInt GoConnectionless();

	/** 
	Establish a bearer-level connection using the information supplied in 
	GoConnectionOriented.
	@param aStatus Used by the server to indicate completion of the request.
	*/	
	IMPORT_C void ConnectBearer(TRequestStatus& aStatus);

	/**
	Cancels interest in the completion of an outstanding ConnectBearer 
	request. Does not affect the state of the bearer-level connection.
	@return KErrNone.
	*/
	IMPORT_C TInt ConnectBearerCancel();

	/** 
	Triggers bearer-level disconnection of the connection specified in 
	GoConnectionOriented.
	@param aStatus Used by the server to indicate completion of the request.
	*/	
	IMPORT_C void DisconnectBearer(TRequestStatus& aStatus);

	/**
	Cancels interest in the completion of an outstanding DisconnectBearer 
	request. Does not affect the state of the bearer-level connection.
	@return KErrNone.
	*/
	IMPORT_C TInt DisconnectBearerCancel();

	/** Returns the version of the RemCon server interface this session supports.
	@return Supported version
	*/
	TVersion Version() const;
	};

/**
The concrete session class for RemCon targets.
*/
NONSHARABLE_CLASS(RRemConTarget) : public RRemCon
	{
public:
	IMPORT_C RRemConTarget();
	
	/** Returns the version of the RemCon server interface this session supports.
	@return Supported version
	*/
	TVersion Version() const;
	};

#endif // REMCONCLIENT_H
