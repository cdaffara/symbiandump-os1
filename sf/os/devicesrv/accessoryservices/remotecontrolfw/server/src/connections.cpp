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
#include "connections.h"
#include "utils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("connectns");
#endif

CConnections* CConnections::NewL()
	{
	LOG_STATIC_FUNC
	return new(ELeave) CConnections();
	}

CConnections::CConnections()
 :	iConnections(_FOFF(TRemConAddress, iLink)),
	iIter(iConnections)
	{
	LOG_FUNC
	}

CConnections::~CConnections()
	{
	LOG_FUNC;
	LogConnections();

	iIter.SetToFirst();
	TRemConAddress* addr;
	while ( ( addr = iIter++ ) != NULL )
		{
		iConnections.Remove(*addr);
		delete addr;
		}

	LogConnections();
	}

CConnections* CConnections::CopyL(CConnections& aItem)
	{
	CConnections* const self = NewL();
	CleanupStack::PushL(self);

	TSglQueIter<TRemConAddress> iter(aItem.iConnections);
	TRemConAddress* orig;
	while ( ( orig = iter++ ) != NULL )
		{
		TRemConAddress* const newAddr = new(ELeave) TRemConAddress;
		newAddr->BearerUid() = orig->BearerUid();
		newAddr->Addr() = orig->Addr();
		self->Append(*newAddr);
		}

	CLEANUPSTACK_POP1(self);
	return self;
	}

TSglQueIter<TRemConAddress>& CConnections::SetToFirst() const
	{
	LOG_FUNC;

	TSglQueIter<TRemConAddress>& ncIter = const_cast<CConnections*>(this)->iIter;

	ncIter.SetToFirst();

	return ncIter;
	}

void CConnections::Append(TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LogConnections();

	// Add the connection to the array of remotes. In debug, check it isn't 
	// already known. This is a programming error somewhere, possibly the 
	// bearer informing us of a new connection twice.
#ifdef _DEBUG
	TSglQueIter<TRemConAddress> iter(iConnections);
	TRemConAddress* addr;
	while ( ( addr = iter++ ) != NULL )
		{
		ASSERT_DEBUG( !(*addr == aAddr) );
		}
#endif // _DEBUG			 

	iConnections.AddLast(aAddr);

	LogConnections();
	}


void CConnections::Remove(TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LogConnections();

	// Remove the connection from the array of remotes. In debug, check it's 
	// already known.
#ifdef _DEBUG
	TBool found = EFalse;
	TSglQueIter<TRemConAddress> iter(iConnections);
	TRemConAddress* addr;
	while ( ( addr = iter++ ) != NULL )
		{
		if ( *addr == aAddr )
			{
			found = ETrue;
			}
		}
	ASSERT_DEBUG(found);
#endif // _DEBUG			 

	iConnections.Remove(aAddr);

	LogConnections();
	}

TBool CConnections::Find(const TRemConAddress& aAddr) const
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid() = 0x%08x"), aAddr.BearerUid());
	LogConnections();

	TBool found = EFalse;
	TSglQueIter<TRemConAddress> iter(const_cast<CConnections*>(this)->iConnections);
	TRemConAddress* addr;
	while ( ( addr = iter++ ) != NULL )
		{
		if ( *addr == aAddr )
			{
			found = ETrue;
			break;
			}
		}

	LOG1(_L("\tfound = %d"), found);
	return found;
	}

void CConnections::LogConnections() const
	{
#ifdef __FLOG_ACTIVE

	const TUint count = Count();
	LOG1(_L("\tNumber of remotes = %d"), count);
	TSglQueIter<TRemConAddress> iter(const_cast<CConnections*>(this)->iConnections);
	TRemConAddress* addr;
	while ( ( addr = iter++ ) != NULL )
		{
		LOG3(_L("\t\tremote [0x%08x], BearerUid = 0x%08x, Addr.Length = %d"), 
			addr,
			addr->BearerUid(),
			addr->Addr().Length()
			);
		}

#endif // __FLOG_ACTIVE
	}
