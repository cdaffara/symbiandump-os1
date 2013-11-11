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

#ifndef REMCONTARGETSELECTORPLUGINOBSERVER_H
#define REMCONTARGETSELECTORPLUGINOBSERVER_H

#include <e32base.h>

class TRemConAddress;
class TClientInfo;

/**
Interface presented by RemCon down to the concrete Target Selector Plugin. 
This class is intended for derivation inside RemCon only.
*/
class MRemConTargetSelectorPluginObserver
	{
public:
	/**
	Called by the TSP to indicate that it has addressed the outgoing command 
	from the previous AddressOutgoingCommand request.
	@param aError The error with which the command was addressed. If other 
	than KErrNone, the command will not be sent by RemCon.
	*/
	IMPORT_C void OutgoingCommandAddressed(TInt aError);

	/**
	Called by the TSP to indicate that it has decided whether the outgoing 
	command (from the previous AllowOutgoingCommand request) is permitted or 
	not.
	@param aIsPermitted ETrue- the send is permitted, EFalse- the send is not 
	permitted.
	*/
	IMPORT_C void OutgoingCommandPermitted(TBool aIsPermitted);

	/**
	Called by the TSP to indicate that it has addressed the incoming command 
	from the previous AddressIncomingCommand request.
	@param aError The error with which the command was addressed. If other 
	than KErrNone, the command will be dropped.
	*/
	IMPORT_C void IncomingCommandAddressed(TInt aError);

	/**
	Called by the TSP to indicate that it has decided whether the outgoing 
	response (from the previous AllowOutgoingResponse request) is permitted or 
	not.
	@param aIsPermitted ETrue- the send is permitted, EFalse- the send is not 
	permitted.
	*/
	IMPORT_C void OutgoingResponsePermitted(TBool aIsPermitted);

	/** Called by the TSP to address the incoming notify
	from the previous AddressIncomingNotify request.
	@param aClientInfo Pointer to the chosen client from the list of clients
	passed to the TSP in the previous AddressIncomingNotify request, or a new
	TClientInfo, with the correct process ID (the other fields will be ignored).
	In the case of the TClientInfo being chosen from the list, the
	ownership of the TClientInfo will be retained by RemCon server. In the case
	of the TClientInfo being newly created because of the TSP starting and application,
	RemCon Server will immediately take a copy of the TClientInfo, and the TSP will
	retain ownership. aClientInfo should be NULL if no client is to be addressed.
	@param aError The error with which the command was addressed. If other 
	than KErrNone, the command will be dropped.
	*/
	
	IMPORT_C void IncomingNotifyAddressed(TClientInfo* aClientInfo, TInt aError);
	
	/** Called by the TSP when the outstanding notifies need to be readdressed to
	other clients. Calling this will result in AddressIncomingNotify requests for
	each currently outstanding notify.
	@note RemCon will do this automatically if a client with outstanding notifies
	exits
	*/
	
	IMPORT_C void ReAddressNotifies();
	
	/**
	Utility for TSPs. Retrieves a list of currently extant bearer-level 
	connections in RemCon server. May be used at any time between the end of 
	TSP construction and the beginning of TSP destruction.
	The TSP is responsible for cleaning up aConnections- the addresses will be 
	on the heap.
	@param aConnections A collection of remote addresses, representing all the 
	currently extant connections. On entry this must be empty otherwise the 
	server will panic.
	@return Error.
	*/
	IMPORT_C TInt GetConnections(TSglQue<TRemConAddress>& aConnections);
	
	/**
	Called by the TSP to indicate that it has addressed the outgoing notify command 
	from the previous AddressOutgoingNotify request.
	@param aConnection Pointer to the address of the chosen remote target device which is chose through
	the use-defined way, e.g. choose from the user-defined ini file or any other file the TSP can 
	read and make a choice from it.	For notify command addressing, the 	command will be addressed 
	to the only one target device, so the TSP should only choose one target address. The ownership 
	of TRemConAddress is retained by remcon server.The aConnection should be NULL if no target 
	address is chose.
	@param aError The error with which the command was addressed. If other 
	than KErrNone, the command will not be sent by RemCon.
	*/
	IMPORT_C void OutgoingNotifyCommandAddressed(TRemConAddress* aConnection, TInt aError);
	
	/**
	Called by the TSP to indicate that it has decided whether the outgoing notify
	command (from the previous AllowOutgoingNotify request) is permitted or 
	not.
	@param aIsPermitted ETrue- the send is permitted, EFalse- the send is not 
	permitted.
	*/
	IMPORT_C void OutgoingNotifyCommandPermitted(TBool aIsPermitted);
	
	/**
	Called by the TSP to indicate that it has decided whether the incoming 
	command (from the previous PermitIncomingCommand request) is permitted or 
	not.
	
	@param aIsPermitted ETrue- the send is permitted, EFalse- the send is not 
	permitted.
	*/
	IMPORT_C void IncomingCommandPermitted(TBool aIsPermitted);
	
	/**
	Called by the TSP to indicate that it has decided whether the incoming notify
	command (from the previous PermitIncomingNotify request) is permitted or 
	not.
	
	@param aIsPermitted ETrue- the send is permitted, EFalse- the send is not 
	permitted.
	*/
	IMPORT_C void IncomingNotifyPermitted(TBool aIsPermitted);
	
	/**
	Called by the TSP to indicate that a user action has occurred which means
	that the specified bearer should address commands to the specified client.
	Addressed commands from that bearer will then be routed to this client until
	either the TSP calls SetLocalAddressedClient again or RemCon calls
	SetRemoteAddressedClient.
	
	@param aBearerUid The bearer that should change its addressed client
	@param aClient The client which the bearer should route addressed commands to.
	@return KErrNone if the request has been accepted.
			KErrNotFound if the supplied client info does not refer to a target client
			Other system wide error code if the request was unable to be handled for 
			another reason.
	*/
	IMPORT_C TInt SetLocalAddressedClient(const TUid& aBearerUid, const TClientInfo& aClientInfo);
	
private:
	/**
	@see OutgoingCommandAddressed.
	*/
	virtual void MrctspoDoOutgoingCommandAddressed(TInt aError) = 0;

	/**
	@see OutgoingCommandPermitted.
	*/
	virtual void MrctspoDoOutgoingCommandPermitted(TBool aIsPermitted) = 0;

	/**
	@see IncomingCommandAddressed.
	*/
	virtual void MrctspoDoIncomingCommandAddressed(TInt aError) = 0;

	/**
	@see GetConnections.
	*/
	virtual TInt MrctspoDoGetConnections(TSglQue<TRemConAddress>& aConnections) = 0;
	
	/**
	@see OutgoingResponsePermitted.
	*/
	virtual void MrctspoDoOutgoingResponsePermitted(TBool aIsPermitted) = 0;
	
	/**
	@see IncomingNotifyAddressed.
	*/
	
	virtual void MrctspoDoIncomingNotifyAddressed(TClientInfo* aClientInfo, TInt aError) = 0;
	
	/**
	@see ReAddressNotifies
	*/
	
	virtual void MrctspoDoReAddressNotifies() = 0;
	
	/**
	@see OutgoingNotifyCommandAddressed
	*/
	virtual void MrctspoDoOutgoingNotifyCommandAddressed(TRemConAddress* aConnection, TInt aError) = 0;
	
	/**
	@see OutgoingNotifyCommandPermitted.
	*/
	virtual void MrctspoDoOutgoingNotifyCommandPermitted(TBool aIsPermitted) = 0;
	
	/**
	@see IncomingCommandPermitted.
	*/
	virtual void MrctspoDoIncomingCommandPermitted(TBool aIsPermitted) = 0;
	
	/**
	@see IncomingNotifyPermitted.
	*/
	virtual void MrctspoDoIncomingNotifyPermitted(TBool aIsPermitted) = 0;
	
	/**
	@see SetLocalAddressedClient.
	*/
	virtual TInt MrctspoSetLocalAddressedClient(const TUid& aBearerUid, const TClientInfo& aClientInfo) = 0;
	};

#endif // REMCONTARGETSELECTORPLUGINOBSERVER_H
