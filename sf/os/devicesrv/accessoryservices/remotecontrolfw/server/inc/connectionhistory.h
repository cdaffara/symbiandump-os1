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

#ifndef CONNECTIONHISTORY_H
#define CONNECTIONHISTORY_H

#include <e32base.h>

class CConnections;
class TRemConAddress;

/**
Encapsulates the history of the connection statuses. 
Each item in the history is a CConnections. So, at any point in time, the 
CConnections given by Last will represent the currently extant connections in 
the system. 
Note that this class allows the client to remove older items from the history 
as they become uninteresting.
Provides APIs to extend or grow the history when connections or disconnections 
occur. In each case a new CConnections is made, based on Last, reflecting the 
new change. 
As an implementation detail, the disconnection API uses memory pre-allocated 
at connection time to make sure that it cannot fail, as required (ultimately) 
by the Bearer API.
*/
NONSHARABLE_CLASS(CConnectionHistory) : public CBase
	{
public:
	/**
	Makes a new connection history- a holder for the connection statuses as 
	they evolve over time. Seeds itself with the initial state of the system 
	(no connections).
	@return Ownership of a new CConnectionHistory object.
	*/
	static CConnectionHistory* NewL();

	/** Destructor. */
	~CConnectionHistory();

public:
	/**
	Destroys the first item in the connection history. Used when no session is 
	interested in it any more. Debug-asserts that the history is empty neither 
	before nor after the call.
	*/
	void DestroyFirst();

	/**
	Gets the number of items in the history. This is always >= 1.
	*/
	TUint Count() const;

	/**
	Accessor for a point in the connection history. If aIndex is out of range, 
	panics.
	@param aIndex Index of the desired point in the connection history.
	@return A specific set of connections.
	*/
	const CConnections& operator[](TUint aIndex);

	/**
	Used to access the last item in the history, i.e. the current set of 
	connections.
	*/
	CConnections& Last();

	/**
	Used when a connection is made. Pre-allocates memory so that any 
	subsequent disconnection is guaranteed to work, and makes a new 'last' 
	item on the history expressing the current state of the system (i.e. the 
	previous state of the system, plus the change expressed by this call).
	@param aAddr The new remote.
	*/
	TInt NewConnection(const TRemConAddress& aAddr);

	/**
	Used when a connection comes down. Uses the pre-allocated memory to make a 
	new latest item in the history expressing the current state of the system 
	(i.e. the previous state of the system, minus the connection which just 
	went away).
	@param aAddr The disappearing remote.
	*/
	void Disconnection(const TRemConAddress& aAddr);

	/**
	Logs the connection history. Note that this might not be the complete 
	connection history, as earlier items are dropped as they become 
	uninteresting. */
	void LogConnectionHistory() const;

private: // utility
	void NewConnectionL(const TRemConAddress& aAddr);
#ifdef __FLOG_ACTIVE
	void LogCollectionPool() const;
	void LogAddressPool() const;
#endif // __FLOG_ACTIVE

private:
	/** Constructor. */
	CConnectionHistory();

	/** 2nd-phase construction. */
	void ConstructL();
		
private: // owned
	/** The history. */
	TSglQue<CConnections> iHistory;

	/** Pre-allocated pool of CConnections, to be used when a disconnection 
	occurs. */
	TSglQue<CConnections> iConnectionsPool;

	/** Pre-allocated pool of TRemConAddresses, to be used when a 
	disconnection occurs. */
	TSglQue<TRemConAddress> iAddressPool;
	};

#endif // CONNECTIONSTATUSHISTORY_H
