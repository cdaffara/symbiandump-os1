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

#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <e32base.h>
#include <remconaddress.h>

/**
Encapsulates a snapshot of all the connections (remotes) in RemCon at one 
point in time.
*/
NONSHARABLE_CLASS(CConnections) : public CBase
	{
public:
	/** Link between elements of this type in a TSglQue. */
	TSglQueLink iLink;

public:
	/**
	@return Ownership of a new CConnections object.
	*/
	static CConnections* NewL();
	
	/**
	@return Ownership of a new CConnections object, with its own (owned) 
	copies of the data in aItem.
	*/
	static CConnections* CopyL(CConnections& aItem);
	
	/**
	Destructor.
	*/
	~CConnections();

public:
	/**
	Accessor for a member iterator over the queue. The iterator is provided 
	set to the first item in the queue. The iterator is 'safe' (will not be 
	otherwise interfered with) until SetToFirst or the destructor is called. 
	@return Iterator.
	*/
	TSglQueIter<TRemConAddress>& SetToFirst() const;

	/**
	@return The number of remote connections.
	*/
	inline TUint Count() const;

	/**
	Adds a remote connection to the current set. Panics if a duplicate is 
	already present.
	@param aAddr The new connection.
	*/
	void Append(TRemConAddress& aAddr);

	/**
	Removes a remote connection to the current set. Panics if the connection 
	is not already present.
	Does not delete the address. This is a very special utility function- the 
	client is expected to be assuming ownership of the address. This function 
	effectively removes ownership of the address from this CConnections.
	@param aAddr The connection to remove.
	*/
	void Remove(TRemConAddress& aAddr);

	/**
	@return ETrue if the given address is present in this set of connections, 
	EFalse otherwise.
	*/
	TBool Find(const TRemConAddress& aAddr) const;

	/** Logs the connections. */
	void LogConnections() const;

private:
	/** Constructor. */
	CConnections();
		
private: // owned
	TSglQue<TRemConAddress> iConnections;
	TSglQueIter<TRemConAddress> iIter;
	};

// Inlines

TUint CConnections::Count() const
	{
	TUint count = 0;

	TSglQueIter<TRemConAddress> iter(const_cast<CConnections*>(this)->iConnections);
	while ( iter++ )
		{
		++count;
		}

	return count;
	}

#endif // CONNECTIONS_H
