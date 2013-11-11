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

#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <e32base.h>

/**
Contains information (of interest to the TSP) about a client of RemCon.
Includes the client's process ID and the client's current send message 
(RMessage2) which is triggering a request (AddressOutgoingCommand or 
PermitOutgoingCommand) on the TSP. Via the RMessage2, the TSP can access the 
client's secure ID and also do capability checks on the client.
The process ID is also used by the server to police creation of target 
sessions (only 1 is allowed per client process).
*/
class TClientInfo
	{
public:
	/** Link between elements of this type in a TSglQue. If this member 
	changes offset, remconserver must be rebuilt. */
	TSglQueLink iLink;
	TSglQueLink iLink2;

public:
	/** Constructor. */
	IMPORT_C TClientInfo();

	/** Destructor. */
	IMPORT_C ~TClientInfo();

public:
	/** 
	Accessor for process ID.
	@return The process ID.
	*/
	IMPORT_C TProcessId& ProcessId();

	/** 
	Accessor for process ID.
	@return The process ID.
	*/
	IMPORT_C TProcessId ProcessId() const;

	/**
	Accessor for message.
	@return The message.
	*/
	IMPORT_C RMessage2& Message();

	/**
	Accessor for the client's current Send message. 
	This is provided for the TSP to access the client's current Send message 
	for capability checks only. The TSP must not Complete this message. Note 
	that this message is only valid if AddressOutgoingCommand or 
	PermitOutgoingCommand are being used (i.e. the client currently has a Send 
	outstanding). 
	For AddressIncomingCommand, the message returned will be invalid and must 
	not be used by the TSP.
	@return The message.
	*/
	IMPORT_C const RMessage2& Message() const;

	/**
	Accessor for the secure ID.
	@return The secure ID.
	*/
	IMPORT_C TSecureId SecureId() const;

	/**
	Accessor for the secure ID.
	@return The secure ID.
	*/
	IMPORT_C TSecureId& SecureId();

private:
	/** The process ID. */
	TProcessId iProcessId;

	/** The client's current send message. */
	RMessage2 iMessage;

	/** The secure ID. */
	TSecureId iSecureId;
	
	// The previous BC pad has been used for iLink2. Further extensions will need to be implemented by moving data out
	// into an extension class
	
	};

class TClientInfoConstIter: TSglQueIter<TClientInfo>
	{
public:
	inline TClientInfoConstIter(TSglQue<TClientInfo>& aQue);
	inline void SetToFirst();
	inline void Set(TClientInfo& aLink);
	inline operator const TClientInfo*();	
	inline const TClientInfo* operator++(TInt);
	};

inline TClientInfoConstIter::TClientInfoConstIter(TSglQue<TClientInfo>& aQue): TSglQueIter<TClientInfo>(aQue)
	{ }

inline void TClientInfoConstIter::SetToFirst()
	{
	TSglQueIter<TClientInfo>::SetToFirst();
	}

inline void TClientInfoConstIter::Set(TClientInfo& aLink)
	{ DoSet(&aLink); }

inline TClientInfoConstIter::operator const TClientInfo*()
	{ return((const TClientInfo*)DoCurrent()); }

inline const TClientInfo* TClientInfoConstIter::operator ++(TInt)
	{ return((const TClientInfo *)DoPostInc()); }

#endif // CLIENTINFO_H
