// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef MESSAGERECIPIENTS_H
#define MESSAGERECIPIENTS_H

#include <e32base.h>
#include <remcon/clientinfo.h>

/**
A list of clients a message with the given transaction ID was delivered to,
that hasn't yet been responded to
*/
NONSHARABLE_CLASS(CMessageRecipients): public CBase
	{
public:
	/**
	@return Ownership of a new CMessageRecipients object.
	*/
	static CMessageRecipients* NewL();
	
	/**
	Destructor.
	*/
	~CMessageRecipients();
	
	/**
	@return List of clients to which the message was delivered
	*/
	TSglQue<TClientInfo>& Clients();
	
	/**
	@return Iterator over a list of clients to which the message was delivered.
	@note Pointers returned by the iterator will be const TClientInfo*
	*/
	TClientInfoConstIter& ConstIter();
	
	/**
	@return The transaction ID of the message that was delivered
	*/
	TUint& TransactionId();
	
	/**
 	Removes and deletes the specified client from the list of clients this message was delivered to.
 	Called when the client responds to the message
	*/
	void RemoveAndDestroyClient(const TClientInfo& aClientInfo);
public:
	TSglQueLink iLink;
private:

	/**
	Constructor
	*/
	CMessageRecipients();

	TSglQue<TClientInfo> iClientInfo;
	TSglQueIter<TClientInfo> iIter;
	TClientInfoConstIter iConstIter;
	TUint iTransactionId;
	};

/**
A list of CMessageRecipients, for storing the list of messages that haven't yet
had a response sent to the bearer and which clients each was delivered to
*/
NONSHARABLE_CLASS(CMessageRecipientsList): public CBase
	{
public:
	/**
	@return Ownership of a new CMessageRecipientsList object.
	*/
	static CMessageRecipientsList* NewL();
	
	/**
	Destructor.
	*/
	~CMessageRecipientsList();
	
	/**
	@return The list of messages and which client each was delivered to.
	*/
	TSglQue<CMessageRecipients>& Messages();
	
	/**
	@return Iterator over the list of messages
	*/
	TSglQueIter<CMessageRecipients>& Iter();
	
	/**
	Removes and deletes a message from the message recipients list with a specified transaction ID
	Called when the response is sent to the bearer, or the last client has responded. 
	*/
	void RemoveAndDestroyMessage(const TUint aTransactionId);
	
	/**
	@return pointer to a CMessageRecipients object with a specified transaction ID, or NULL
	if one was not found
	*/
	CMessageRecipients* Message(TUint aTransactionId);
	
private:

	/**
	Constructor
	*/
	CMessageRecipientsList();

	TSglQue<CMessageRecipients> iMessages;
	TSglQueIter<CMessageRecipients> iIter;
	};
	
#endif // MESSAGERECIPIENTS_H
