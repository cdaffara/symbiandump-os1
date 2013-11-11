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
#include "connectionhistory.h"
#include "connections.h"
#include "utils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("connhist");
#endif

#ifdef __FLOG_ACTIVE
#define LOGCOLLECTIONPOOL	LogCollectionPool()
#define LOGADDRESSPOOL		LogAddressPool()
#else
#define LOGCOLLECTIONPOOL
#define LOGADDRESSPOOL
#endif // __FLOG_ACTIVE

CConnectionHistory* CConnectionHistory::NewL()
	{
	LOG_STATIC_FUNC
	CConnectionHistory* self = new(ELeave) CConnectionHistory();
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP1(self);
	return self;
	}

CConnectionHistory::CConnectionHistory()
 :	iHistory(_FOFF(CConnections, iLink)),
	iConnectionsPool(_FOFF(CConnections, iLink)),
	iAddressPool(_FOFF(TRemConAddress, iLink))
	{
	LOG_FUNC
	}

void CConnectionHistory::ConstructL()
	{
	LOG_FUNC;

	// Start the history off with the start-up state of the system- no 
	// connections.
	CConnections* const conns = CConnections::NewL();
	iHistory.AddLast(*conns);

	LogConnectionHistory();
	}

CConnectionHistory::~CConnectionHistory()
	{
	LOG_FUNC;
	LogConnectionHistory();
	LOGCOLLECTIONPOOL;
	LOGADDRESSPOOL;

	// Clean up all the collections of everything!

	LOG(_L("\tdestroying history..."));
		{
		TSglQueIter<CConnections> iter1(iHistory);
		CConnections* conns1;
		while ( ( conns1 = iter1++ ) != NULL )
			{
			iHistory.Remove(*conns1);
			delete conns1;
			}
		}

	LOG(_L("\tdestroying connections pool..."));
		{
		TSglQueIter<CConnections> iter2(iConnectionsPool);
		CConnections* conns2;
		while ( ( conns2 = iter2++ ) != NULL )
			{
			iConnectionsPool.Remove(*conns2);
			delete conns2;
			}
		}

	LOG(_L("\tdestroying address pool..."));
		{
		TSglQueIter<TRemConAddress> iter3(iAddressPool);
		TRemConAddress* addr;		 
		while ( ( addr = iter3++ ) != NULL )
			{
			iAddressPool.Remove(*addr);
			delete addr;
			}
		}
	
	// Can't LogConnectionHistory here because it breaks our invariant (we've 
	// deleted all the history items, so it's empty and Count == 0).
	LOGCOLLECTIONPOOL;
	LOGADDRESSPOOL;
	}

void CConnectionHistory::DestroyFirst()
	{
	LOG_FUNC;
	LogConnectionHistory();

	ASSERT_DEBUG(!iHistory.IsEmpty());
	CConnections* conn = iHistory.First();
	ASSERT_DEBUG(conn);
	iHistory.Remove(*conn);
	delete conn;
	ASSERT_DEBUG(!iHistory.IsEmpty());

	LogConnectionHistory();
	}

TUint CConnectionHistory::Count() const
	{
	TUint count = 0;

	TSglQueIter<CConnections> iter(const_cast<CConnectionHistory*>(this)->iHistory);
	while ( iter++ != NULL )
		{
		++count;
		}

	ASSERT_DEBUG(count >= 1);
	return count;
	}

const CConnections& CConnectionHistory::operator[](TUint aIndex)
	{
	TSglQueIter<CConnections> iter(iHistory);
	CConnections* conn;
	TUint count = 0;
	while ( ( conn = iter++ ) != NULL )
		{
		if ( count == aIndex )
			{
			break;
			}
		++count;
		}
	ASSERT_DEBUG(conn);
	return *conn;
	}

CConnections& CConnectionHistory::Last()
	{
	// The connection history should never be empty. It should always hold at 
	// least the current connection set.
	ASSERT_DEBUG(!iHistory.IsEmpty());

	CConnections* last = iHistory.Last();
	ASSERT_DEBUG(last);
	return *last;
	}

TInt CConnectionHistory::NewConnection(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LogConnectionHistory();
	LOGCOLLECTIONPOOL;
	LOGADDRESSPOOL;

	TRAPD(err, NewConnectionL(aAddr));
	LOG1(_L("\terr = %d"), err);
	
	LogConnectionHistory();
	LOGCOLLECTIONPOOL;
	LOGADDRESSPOOL;

	return err;
	}

void CConnectionHistory::NewConnectionL(const TRemConAddress& aAddr)
	{
	// Called to handle a new connection by updating the connection history.
	// Create a copy of the Last item (CConnections) in the history, add the 
	// new address to it, and add that (the new CConnections) to the end of 
	// the history.
	// Then, in order to guarantee that any subsequent disconnection will 
	// work, allocate a new CConnections and add it to the pool of 
	// CConnections iConnectionsPool. Also allocate N TRemConAddresses and add 
	// them to the pool iAddressPool, where N is the number of remotes in the 
	// old head CConnections. 
	// When a disconnection occurs, the new CConnections will be got from 
	// iConnectionsPool, and the required TRemConAddresses will be got from 
	// iAddressPool- without failing.
	// If any of this fails, roll us back to how we were at the beginning of 
	// the call and leave.
	// At the end the history will have a new Last, with one new item in it. 

	// Make a new item for the history...
	CConnections* const newSetOfConnections = CConnections::CopyL(Last());
	CleanupStack::PushL(newSetOfConnections);
	TRemConAddress* const addr = new(ELeave) TRemConAddress;
	*addr = aAddr;
	newSetOfConnections->Append(*addr);
	// Leave newSetOfConnections on the cleanup stack so if we leave it gets 
	// destroyed automatically.
	
	// ...and pre-allocate memory.
	CConnections* conn = CConnections::NewL();
	CleanupStack::PushL(conn); // leave this on the cleanup stack so it gets 
	// dealt with if we leave.

	// Get the number of TRemConAddresses we need to pre-allocate.
	const TUint count = Last().Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		TRemConAddress* preAllocAddr = new TRemConAddress;
		if ( preAllocAddr )
			{
			iAddressPool.AddLast(*preAllocAddr);
			}
		else
			{
			// We couldn't pre-allocate as much as we needed... destroy as 
			// many TRemConAddresses as we have already made and leave. 
			for ( TUint jj = 0 ; jj < ii ; ++jj )
				{
				ASSERT_DEBUG(!iAddressPool.IsEmpty());
				TRemConAddress* removeAddr = iAddressPool.Last();
				ASSERT_DEBUG(removeAddr);
				iAddressPool.Remove(*removeAddr);
				delete removeAddr;
				}
			// This leave will clean up the pre-allocated CConnections and the 
			// new 'real' CConnections.
			LEAVEIFERRORL(KErrNoMemory);
			}
		}

	// If we got to here, do some sanity checking, clean up the cleanup stack 
	// (!) and finally add the new history item.
	CLEANUPSTACK_POP1(conn);
	iConnectionsPool.AddLast(*conn);
	
	ASSERT_DEBUG(newSetOfConnections->Count() == Last().Count() + 1);
	
	CLEANUPSTACK_POP1(newSetOfConnections);
	iHistory.AddLast(*newSetOfConnections); 
	}

void CConnectionHistory::Disconnection(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LogConnectionHistory();
	LOGCOLLECTIONPOOL;
	LOGADDRESSPOOL;
	
	// Called to handle a disconnection by updating the connection history.
	// In order for this operation to be guaranteed to work, we must take a 
	// CConnections from the pool iConnectionsPool, and, for each 
	// TRemConAddress in Last, _except the one being disconnected in this 
	// call_, copy it into a TRemConAddress taken from the iAddressPool, and 
	// add it to the reclaimed CConnections. Finally add the CConnections to 
	// the history.
	// At the end the history will have a new Last with one less remote in it 
	// than the previous.

	// Get a CConnections from the pool.
	ASSERT_DEBUG(!iConnectionsPool.IsEmpty());
	CConnections* newSetOfConnections = iConnectionsPool.Last();
	ASSERT_DEBUG(newSetOfConnections);
	iConnectionsPool.Remove(*newSetOfConnections);

	// Copy addresses from Last into newSetOfConnections, except the one being 
	// disconnected.
	CConnections& last = Last();
	TSglQueIter<TRemConAddress>& iter = last.SetToFirst();
	TRemConAddress* conn = NULL;
	while ( ( conn = iter++ ) != NULL )
		{
		if ( !(*conn == aAddr) )
			{
			ASSERT_DEBUG(!iAddressPool.IsEmpty());
			TRemConAddress* const newAddr = iAddressPool.Last();
			ASSERT_DEBUG(newAddr);
			iAddressPool.Remove(*newAddr);
			*newAddr = *conn;
			newSetOfConnections->Append(*newAddr);
			}
		}

	// Sanity check and finally add the new CConnections to the history.
	ASSERT_DEBUG(newSetOfConnections->Count() == Last().Count() - 1);
	iHistory.AddLast(*newSetOfConnections);

	LogConnectionHistory();
	LOGCOLLECTIONPOOL;
	LOGADDRESSPOOL;
	}

void CConnectionHistory::LogConnectionHistory() const
	{
#ifdef __FLOG_ACTIVE

	const TUint count = Count();
	LOG1(_L("\tNumber of items in history = %d"), count);
	TSglQueIter<CConnections> iter(const_cast<CConnectionHistory*>(this)->iHistory);
	CConnections* conns;
	while ( ( conns = iter++ ) != NULL )
		{
		LOG1(_L("\t\titem [0x%08x]:"), conns);
		conns->LogConnections();
		}

#endif // __FLOG_ACTIVE
	}

#ifdef __FLOG_ACTIVE
void CConnectionHistory::LogCollectionPool() const
	{
	LOG(_L("\tLogging pre-allocated connections pool"));
	TSglQueIter<CConnections> iter(const_cast<CConnectionHistory*>(this)->iConnectionsPool);
	CConnections* conns;
	while ( ( conns = iter++ ) != NULL )
		{
		LOG1(_L("\t\titem [0x%08x]:"), conns);
		}
	}

void CConnectionHistory::LogAddressPool() const
	{
	LOG(_L("\tLogging pre-allocated address pool"));
	TSglQueIter<TRemConAddress> iter(const_cast<CConnectionHistory*>(this)->iAddressPool);
	TRemConAddress* addr;
	while ( ( addr = iter++ ) != NULL )
		{
		LOG1(_L("\t\titem [0x%08x]:"), addr);
		}
	}
#endif // __FLOG_ACTIVE
