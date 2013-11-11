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
 @internalComponent
*/

#include <bluetooth/logger.h>
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <e32base.h>

class CRemConMessage;

/**
A queue for RemCon messages (commands and responses). 
Owns the messages.
*/
NONSHARABLE_CLASS(CMessageQueue) : public CBase 
	{
public:
	/**
	@return Ownership of a new CMessageQueue object.
	*/
	static CMessageQueue* NewL();

	/**
	Destructor.
	*/
	~CMessageQueue();

public:
	/**
	Accessor for a member iterator over the queue. The iterator is provided 
	set to the first item in the queue. The iterator is 'safe' (will not be 
	otherwise interfered with) until SetToFirst or the destructor is called. 
	@return Iterator.
	*/
	TSglQueIter<CRemConMessage>& SetToFirst();

	/**
	Appends the given item at the end of the queue.
	@param aMsg New message.
	*/
	void Append(CRemConMessage& aMsg); 

	/**
	Removes the given item from the queue and destroys it.
	@param aMsg Message to be destroyed.
	*/
	void RemoveAndDestroy(CRemConMessage& aMsg);

	/**
	Removes and destroys all messages belonging to the given session.
	@param aSessionId ID of session.
	*/
	void RemoveAndDestroy(TUint aSessionId);

	/**
	Removes the given item from the queue without destroying it.
	@param aMsg Message to be removed.
	*/
	void Remove(CRemConMessage& aMsg);

	/**
	Returns the first item in the queue. The queue must not be empty. 
	@return First item in the queue.
	*/
	CRemConMessage& First();

	/**
	Returns a message with a specified transaction ID
	@param Transaction ID of the required message
	@return Pointer to the message, or NULL if it does not exist. Ownership is retained by the queue
	*/
	
	CRemConMessage* Message(TUint aTransactionId);
	
	/** 
	@return Whether the queue is empty or not.
	*/
	inline TBool IsEmpty() const;

	inline TSglQue<CRemConMessage>& Queue();

	void LogQueue() const;
	
private:
	CMessageQueue();

private: // owned
	TSglQue<CRemConMessage> iQueue;
	TSglQueIter<CRemConMessage> iIter;
	// Used for logging purposes.
	TUint iCount;
	};

// Inlines

TBool CMessageQueue::IsEmpty() const
	{
#ifdef __FLOG_ACTIVE
	// Needs to be defined locally to prevent redefinition
	_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

	LOG_FUNC;
	
	TBool ret = iQueue.IsEmpty();
	LOG1(_L("\tret = %d"), ret);

	return ret;
	}

TSglQue<CRemConMessage>& CMessageQueue::Queue()
	{
	return iQueue;
	}

#endif // MESSAGEQUEUE_H
