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

#ifndef TARGETSELECTORPLUGININTERFACE_H
#define TARGETSELECTORPLUGININTERFACE_H

#include <e32base.h>

class MRemConTargetSelectorPluginObserver;
class TRemConAddress;
class TClientInfo;
class TBearerSecurity;
class TClientInfoConstIter;

/**
The UID of this version of the Target Selector Plugin interface.
*/
const TInt KRemConTargetSelectorInterface1 = 0x10208BCD;


/**
Abstract base class for target selector plugins.
*/
class MRemConTargetSelectorPluginInterface
	{
public:
	/**
	Called by RemCon to get the TSP to address an outgoing command (from a 
	connectionless controller client) to zero or more remotes. The implementor 
	should add zero or more items to aConnections and then call 
	OutgoingCommandAddressed on the observer with an appropriate error.
	Note that only one of AddressOutgoingCommand and PermitOutgoingCommand is 
	outstanding at once.
	The implementor is responsible for the capability check. For this reason, 
	aSender contains the client's current send message, and aBearerSecurity 
	contains all the bearer security policies. To reiterate, RemCon does no 
	security check on the client's send request either before calling 
	AddressOutgoingCommand.
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aSender The TClientInfo of the sending session.
	@param aConnections An empty collection of connections. NB On completion, 
	an actual (bearer-level) connection does not need to exist for each item 
	in the collection- RemCon passes responsibility to the bearer(s) for 
	creating the specified connections. On successful completion, RemCon takes 
	ownership of any items in the collection- new TRemConAddresses must be 
	made on the heap.
	@param aBearerSecurity Contains all the bearer security policies.
	*/
	virtual void AddressOutgoingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aSender,
		TSglQue<TRemConAddress>& aConnections,
		TSglQue<TBearerSecurity>& aBearerSecurity) = 0;

	/**
	Called by RemCon to find out from the TSP whether the given 
	connection-oriented controller client is permitted to send the given 
	command to the given remote at this time. The implementor should call 
	PermitOutgoingCommand with either ETrue, if the send is permitted, or 
	EFalse, if the send is not permitted.
	Note that only one of AddressOutgoingCommand and PermitOutgoingCommand is 
	outstanding at once.
	Note that a capability check will have been done by RemCon before 
	PermitOutgoingCommand is called- actually at GoConnectionOriented time. 
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aSender The TClientInfo of the sending session.
	@param aConnection The remote the command will be sent over if permission 
	is granted.
	*/
	virtual void PermitOutgoingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aSender,
		const TRemConAddress& aConnection) = 0;

	/** 
	Called by RemCon to cancel the current AddressOutgoingCommand or 
	PermitOutgoingCommand command.
	On receipt, the TSP must stop dereferencing any data given in the 
	AddressOutgoingCommand or PermitOutgoingCommand request. The TSP should 
	not subsequently call OutgoingCommandAddressed or 
	OutgoingCommandPermitted, except in response to a subsequent new 
	AddressOutgoingCommand or PermitOutgoingCommand command.
	If an AddressOutgoingCommand request is currently being processed, the TSP 
	is responsible for deleting any TRemConAddresses it has already created.
	*/
	virtual void CancelOutgoingCommand() = 0;

	/**
	Called by RemCon to get the TSP to address an incoming command (from a 
	remote to zero or more target clients). The implementor should add or 
	remove zero or more items from aClients and then call 
	IncomingCommandAddressed on the observer with a suitable error. 
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aClients A collection of existing target clients. The implementor 
	may wish to start further target client(s) and add their process ID(s) to 
	this collection. New TClientInfo items must be made on the stack. Note 
	that when adding a TClientInfo to aClients, only the process ID needs to 
	be correctly populated.
	There is no 'cancel' method for AddressIncomingCommand. RemCon will only 
	want to 'cancel' this when the server is terminating, at which point it 
	will destroy the TSP anyway.
	*/
	virtual void AddressIncomingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		TSglQue<TClientInfo>& aClients) = 0;
			
	};
	
/**
The UID of this version of the Target Selector Plugin interface.
*/
const TInt KRemConTargetSelectorInterface2 = 0x102858CF;

/**
Additional functions for TSP Interface V2
*/
class MRemConTargetSelectorPluginInterfaceV2: public MRemConTargetSelectorPluginInterface
	{
public:
	/**
	Called by RemCon to get the TSP to decide which client should be
	allowed to respond to a command.
	This function is called as soon as each client returns a response, so
	the order in which clients are offered to the TSP is not predetermined.
	Since AV/C expects only a single response, the first response allowed by 
	the TSP is the one which will be sent on-air.
	The initial list of clients will be populated from the TSP's response 
	to AddressIncomingCommand().
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aClient The client which is trying to send a response
	@param aClients A list of clients which are still expected to respond,
	including the one specified in aClient
	*/
	 
	virtual void PermitOutgoingResponse(
			TUid aInterfaceUid,
			TUint aOperationId,
			const TClientInfo& aClient,
			TClientInfoConstIter& aClients) = 0;
	
	/**
	Called by RemCon to get the TSP to address an incoming notify (from a 
	remote to zero or more target clients). The implementor should call 
	IncomingNotifyAddressed on the observer with a pointer to the chosen
	client (or NULL if no client is to be addressed) from the list, and a
	suitable error.
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aClients A collection of existing target clients. The implementor 
	may wish to start a target client and call IncomingNotifyAddressed
	with a new TClientInfo. The new TClientInfo item must be made on the stack. 
	Note when creating the new TClientInfo, only the process ID needs to 
	be correctly populated.
	*/
	virtual void AddressIncomingNotify(
		TUid aInterfaceUid,
		TUint aOperationId, 
		TSglQue<TClientInfo>& aClients) = 0;
	
	/** 
	Called by RemCon to cancel the current PermitOutgoingResponse request.
	On receipt, the TSP must stop dereferencing any data given in the 
	PermitOutgoingResponse request. The TSP should not subsequently call
	OutgoingResponsePermitted, except in response to a subsequent new 
	PermitOutgoingResponse request.
	*/
	virtual void CancelOutgoingResponse() = 0;
	};

/**
The UID of this version of the Target Selector Plugin interface.
*/
const TInt KRemConTargetSelectorInterface3 = 0x102858D3;

/**
Additional functions for TSP Interface V3

This interface must be implemented if the TSP wishes to support outgoing
Notify commands (local role controller).
*/
class MRemConTargetSelectorPluginInterfaceV3
	{
public:
	/**
	Called by RemCon to get the TSP to address an outgoing notify command (from a 
	connectionless controller client) to zero or one remote. The implementor 
	need to get the connection list from which to chose the only one target device and then call 
	OutgoingNotifyCommandAddressed on the observer with an appropriate error.
	Note that only one of AddressOutgoingNotifyCommand and PermitOutgoingNotifyCommand is 
	outstanding at once.
	The implementor is responsible for the capability check. For this reason, 
	aSender contains the client's current send message, and aBearerSecurity 
	contains all the bearer security policies. To reiterate, RemCon does no 
	security check on the client's send request either before calling 
	AddressOutgoingNotify.
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aSender The TClientInfo of the sending session.
	@param aBearerSecurity Contains all the bearer security policies.
	*/
	virtual void AddressOutgoingNotify(
			TUid aInterfaceUid,
			TUint aOperationId, 
			const TClientInfo& aSender,
			TSglQue<TBearerSecurity>& aBearerSecurity) = 0;
	
	/** 
	Called by RemCon to cancel the current AddressOutgoingNotify or 
	PermitOutgoingNotifyCommand command.
	On receipt, the TSP must stop dereferencing any data given in the 
	AddressOutgoingNotify or PermitOutgoingNotifyCommand request. The TSP should 
	not subsequently call OutgoingNotifyCommandAddressed or 
	OutgoingNotifyCommandPermitted, except in response to a subsequent new 
	AddressOutgoingNotify or PermitOutgoingNotifyCommand command.
	If an AddressOutgoingNotify request is currently being processed, the TSP 
	is responsible for deleting any TRemConAddresses it has already created.
	*/
	virtual void CancelOutgoingNotifyCommand() = 0;
	
	/**
	Called by RemCon to find out from the TSP whether the given 
	connection-oriented controller client is permitted to send the given notify
	command to the given remote at this time. The implementor should call 
	PermitOutgoingNotifyCommand with either ETrue, if the send is permitted, or 
	EFalse, if the send is not permitted.
	Note that only one of AddressOutgoingNotify and PermitOutgoingNotifyCommand is 
	outstanding at once.
	Note that a capability check will have been done by RemCon before 
	PermitOutgoingNotifyCommand is called- actually at GoConnectionOriented time. 
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aSender The TClientInfo of the sending session.
	@param aConnection The remote the command will be sent over if permission 
	is granted.
	*/
	virtual void PermitOutgoingNotifyCommand(
			TUid aInterfaceUid,
			TUint aOperationId, 
			const TClientInfo& aSender,
			const TRemConAddress& aConnection) = 0;
	};

/**
The UID of this version of the Target Selector Plugin interface.
*/
const TInt KRemConTargetSelectorInterface4 = 0x10286A7;

/**
Additional functions for TSP Interface V4

This interface must be implemented if the TSP wishes to support addressing
by the bearer.  Bearer addressing will always be used if supported by the
bearer.  When bearer addressing is used the TSP will not be given any
option over which client receives a command, however if it implements this
interface it has visibility of where each command goes and can reject the 
command if it is not acceptable.

If this interface is not implemented default implementations of these functions 
will be used which will allow the command to be addressed to the bearer's 
selected client.  This means that if the TSP does not implement this interface
it will not see any commands which have been addressed by the bearer.

Regardless of whether this interface is implemented commands which are not 
addressed by the bearer will continue to be addressed by the TSP via earlier 
versions of MRemConTargetSelectorPluginInterface.
*/
class MRemConTargetSelectorPluginInterfaceV4
	{
public:
	/**
	Called by RemCon to get the TSP to permit an incoming command. This is called
	if the bearer has provided a target client for the command.  
	
	The implementor should decide if they wish to allow this command and then call
	IncomingCommandPermitted on the observer with a suitable error. 
	
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the command.
	@param aClient a TClientInfo referring to the selected client
	*/
	virtual void PermitIncomingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aClient) = 0;
	
	/**
	Called by RemCon to get the TSP to permit an incoming Notify. This is called
	if the bearer has provided a target client for the Notify.  
	
	The implementor should decide if they wish to allow this Notify and then call
	IncomingNotifyPermitted on the observer with a suitable error. 
	
	@param aInterfaceUid The UID of the client interface.
	@param aOperationId The operation ID of the Notify.
	@param aClient a TClientInfo referring to the selected client
	*/
	virtual void PermitIncomingNotify(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aClient) = 0;
	
	/** 
	Called by RemCon when a bearer that can address commands wishes to
	inform the system that there has been a remote user action to 
	select a different addressed client.
	
	The bearer will then route addressed commands to this client until
	such time as SetRemoteAddressedClient is called again or the TSP
	calls SetLocalAddressedClient.
	
	@param aBearerUid The bearer that has changed its addressed client
	@param aClient The RemCon client that is now selected by the bearer
	*/
	virtual void SetRemoteAddressedClient(const TUid& aBearerUid, 
			const TClientInfo& aClient) = 0;
	};

/**
The UID of this version of the Target Selector Plugin interface.
*/
const TInt KRemConTargetSelectorInterface5 = 0x2002e6e1;

class MRemConTargetSelectorPluginInterfaceV5
	{
public:
	/** Called by RemCon when a new target client has connected.
	 
	 @aClientInfo The information about the new client.
	 */
	virtual void TargetClientAvailable(const TClientInfo& aClientInfo) = 0;
	
	/** Called by RemCon when a target client has disconnected. 
	 
	 @aClientInfo The information about the client that has disconnected.
	 */
	virtual void TargetClientUnavailable(const TClientInfo& aClientInfo) = 0;
	
	/** Called by RemCon when a bearer wishes to begin being informed when
	the locally addressed player changes.  Once this function has been called
	the TSP should inform RemCon via SetLocalAddressedPlayer each time the
	player to which incoming commands from aBearer would be routed changes.
	This might occur for example if a new application is launched, or if the
	foreground application changes, depending on what the TSP's rules are
	for deciding the target of the incoming message.  These updates should
	occur until UnregisterLocalAddressedClientObserver is called.
	
	@param aBearerUid The bearer that wishes to be informed of updates
	*/
	virtual TInt RegisterLocalAddressedClientObserver(const TUid& aBearerUid) = 0;

	/** Called by RemCon when a bearer wishes to stop being informed of 
	changes to the local addresse client.
	
	@param aBearerUid The bearer that no longer wishes to be informed of updates
	*/
	virtual TInt UnregisterLocalAddressedClientObserver(const TUid& aBearerUid) = 0;
	};


#endif // TARGETSELECTORPLUGININTERFACE_H
