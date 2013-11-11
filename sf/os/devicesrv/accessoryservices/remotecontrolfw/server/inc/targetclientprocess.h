// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Server-side representation of a target client, which may have multiple 
// sessions associated with it.
// 

/**
 @file
 @internalComponent
*/

#ifndef TARGETCLIENTPROCESS_H
#define TARGETCLIENTPROCESS_H

#include <e32base.h>
#include <remcon/clientinfo.h>
#include <remcon/clientid.h>
#include <remcon/playertype.h>

#include "messagesendobserver.h"


/**
The maximum number of target sessions permitted for a single client process
*/
const TUint KMaxNumberTargetSessions = 2;

class CRemConServer;
class CBearerManager;
class CRemConTargetSession;
class CRemConInterfaceDetails;
class CRemConMessage;

/**
Represents a target client process and manages that clients sessions.
*/
NONSHARABLE_CLASS(CRemConTargetClientProcess) : public CBase, public MRemConMessageSendObserver
	{
public:
	static CRemConTargetClientProcess* NewLC(TClientInfo& aClientInfo, TRemConClientId aClientId, CRemConServer& aServer, CBearerManager& aBearerManager);
	~CRemConTargetClientProcess();

	/**
	Returns the unique client ID for this client.
	@return The client ID.
	*/
	inline TRemConClientId Id() const;

	/**
	Returns a description of the client represented by this CRemConTargetClientProcess instance.
	@return A TClientInfo instance describing the client.
	*/
	inline TClientInfo& ClientInfo();
	inline const TClientInfo& ClientInfo() const;

	/**
	Indicates whether or not this CRemConTargetClientProcess represents a client described by a given TClientInfo instance.
	@param aClientInfo The TClientInfo instance describing the client.
	@return ETrue if the client described by aClientInfo is that represented by this CRemConTargetClientProcess instance.
	*/
	inline TBool IsClient(const TClientInfo& aClientInfo) const;

	/**
	Indicates whether or not this client has been registered as available with the server.
	@return ETrue if this client has been marked as available.
	*/
	inline TBool ClientAvailable() const;

	/**
	Creates a new target session for the client with a given unique session ID.
	If the client already has KMaxNumberTargetSessions sessions open, then this will leave with KErrOverflow.
	@param aSessionId The session ID for the new session.
	*/
	CRemConTargetSession* NewSessionL(TUint aSessionId);

	/**
	Called by CRemConTargetSession to provide notification of the opening of a new session.
	@param aSession The CRemConTargetSession that is opening.
	@return KErrNone on success, otherwise one of the other system-wide error codes.
	*/
	TInt TargetSessionOpened(CRemConTargetSession& aSession);

	/**
	Called by CRemConTargetSession to provide notificaton of session closure.
	@param aSession The CRemConTargetSession that is closing.
	*/
	void TargetSessionClosed(CRemConTargetSession& aSession);

	/**
	Returns the number of target sessions currently open on this client.
	@return The number of open target sessions.
	*/
	inline TInt TargetSessionCount() const;

	/**
	Sets the player information for this client.
	This information can only be set once for the client.
	@param aPlayerType The player type.
	@param aPlayerName The player name.
	*/
	void SetPlayerInformationL(const TPlayerTypeInformation& aPlayerType, const TDesC8& aPlayerName);

	/**
	Indicates whether or not player information has been set for this client.
	@return ETrue if player information has been set.
	*/
	inline TBool HasPlayerInformation() const;

	/**
	Returns the player type set for this client.
	This is only valid if HasPlayerInformation() returns ETrue.
	@return The player type.
	*/
	inline TPlayerType PlayerType() const;

	/**
	Returns the player subtype set for this client.
	This is only valid if HasPlayerInformation() returns ETrue.
	@return The player subtype.
	*/
	inline TPlayerSubType PlayerSubType() const;

	/**
	Returns the player name set for this client.
	This is only valid if HasPlayerInformation() returns ETrue.
	@return The player name.
	*/
	inline const TDesC8& Name() const;

	/**
	Indicates whether or not the given player information matches that set for this client.
	This is only valid if HasPlayerInformation() returns ETrue.
	@return ETrue if the player information matches.
	*/
	inline TBool PlayerInformationMatches(const TPlayerTypeInformation& aPlayerType, const TDesC8& aPlayerName) const;

	/**
	Called by CRemConTargetSession to provide notification that the session has registered a new interface.
	@param aSession The CRemConTargetSession that has registered the new interface.
	*/
	void InterfacesRegistered();

	/**
	Determines whether or not an interface of the same type as that given has already been registered
	in a session owned by this client, other than the given session.
	@param aSession The target session to exclude from the interface search.
	@param aInterface The interface type to test for.
	@return Whether or not a registered interface of this type exists for the client.
	**/
	TBool IsInterfaceTypeRegisteredByAnotherSession(CRemConTargetSession& aSession, TUid aInterfaceUid) const;
	
	/**
	Called by CRemConServer when a message has been received for this client.
	The client will attempt to deliver the message to the appropriate session. If no session
	supports the message, KErrArgument is returned. Otherwise, if the supporting session is
	not ready to receive the message, KErrNotReady is returned. Any other error code is returned
	by the session as it processes the message.
	@param aMessage The message for this client.
	@return KErrArgument if no session supports this message, KErrNotReady if the message cannot be handled yet, or an error
	code returned by the session as it processes the message.
	*/
	TInt ReceiveMessage(CRemConMessage& aMessage);

	/**
	Indicates that a connection has come up or down.
	This notifies each session held by this client.
	*/
	void ConnectionsChanged();

	/**
	Provides a list of interfaces supported by this client.
	@param aUids An RArray to hold the UIDs of the supported interfaces.
	@return KErrNone on success, otherwise one of the system-wide error codes.
	*/
	TInt SupportedInterfaces(RArray<TUid>& aUids);

	/**
	Provides a list of interfaces supported by this client that require the bulk server.
	@param aUids An RArray to hold the UIDs of the interfaces.
	@return KErrNone on success, otherwise one of the system-wide error codes.
	*/
	TInt SupportedBulkInterfaces(RArray<TUid>& aUids);

	/**
	Provides a list of operations supported by a particular interface.
	@param aInterfaceUid The UID of the interface of interest.
	@param aOperations An RArray to hold the supported operations.
	@return KErrNone on success, otherwise one of the system-wide error codes.
	*/
	TInt SupportedOperations(TUid aInterfaceUid, RArray<TUint>& aOperations);

public:	// From MRenConMessageSendObserver
	void MrcmsoMessageSendResult(const CRemConMessage& aMessage, TInt aError);	// Not supported
	void MrcmsoMessageSendOneOrMoreAttempt(const CRemConMessage& aMessage, TUint aNumRemotes);
	void MrcmsoMessageSendOneOrMoreIncremental(const CRemConMessage& aMessage, TUint aNumRemotes);
	void MrcmsoMessageSendOneOrMoreAttemptFailed(const CRemConMessage& aMessage, TInt aError);
	void MrcmsoMessageSendOneOrMoreResult(const CRemConMessage& aMessage, TInt aError);
	void MrcmsoMessageSendOneOrMoreAbandoned(const CRemConMessage& aMessage);

private:
	CRemConTargetClientProcess(TClientInfo& aClientInfo, TRemConClientId aClientId, CRemConServer& aServer, CBearerManager& aBearerManager);

	/**
	Returns the target session that supports a given RemCon message.
	Since no two sessions may register the same interface, there is a maximum of one possible session that could support the message.
	@param aMessage The RemCon message.
	@return The supporting CRemConTargetSession instance or NULL if no supporting session could be found.
	*/
	CRemConTargetSession* FindSessionForMessage(const CRemConMessage& aMessage);

	/**
	Returns an index to a target session held by this client that supports a particular interface.
	@param aInterface The UID of the interface of interest.
	@return If found, the index of the relevent target session. Otherwise, KErrNotFound or one of the other system-wide error codes.
	*/
	TInt FindSessionForInterface(TUid aInterface) const;

	/**
	Utility method to complete a given RemCon message for a particular target session.
	@param aMessage The RemCon message
	@param aSession The target session
	*/
	void CompleteMessageForSession(const CRemConMessage& aMessage, CRemConTargetSession& aSession);

private:
	// Client information
	TClientInfo iClientInfo;
	TRemConClientId iClientId;
	TBool iClientAvailable;

	CRemConServer& iServer;
	CBearerManager& iBearerManager;

	// Player information
	TPlayerTypeInformation iPlayerType;
	RBuf8 iPlayerName;
	TBool iPlayerInfoSet;

	// Target sessions for this client
	RPointerArray<CRemConTargetSession> iTargetSessions;
	
	};

#include "targetclientprocess.inl"

#endif // TARGETCLIENTPROCESS_H
