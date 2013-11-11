// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms Database and Table inline functions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef COMMDB_INL
#define	COMMDB_INL

#include <commdb.h>
#include <cdbpreftable.h>


inline RDbNamedDatabase* CCommsDatabaseBase::Database()
/**
Returns the interface for creating and opening a database identified by name and format.

@return A pointer to the interface for creating and opening a database identified by name and format.
*/
	{
	return &iDatabase;
	}

inline void CCommsDbTableView::GetTableName(TDes& aTableName) const
/**
Gets the name of the table associated with this view and copies it into the
descriptor supplied by the caller.

@param aTableName A reference to a descriptor passed by the caller. On return
from this function it contains the name of the table.
*/
	{
	aTableName=iTableName;
	}

inline TDbColNo CCommsDbConnectionPrefTableView::ColNum(const TDesC& /*aColumn*/) const
/**
Returns the column number of the Column aColumn in the table

Coloumn concept doesn't exist in CommsDat
depNot supported from v9.1

@param aColumn A reference to a descriptor containing the name of a column in the current record.
@return The ordinal number of the column else KDbNullColNo if no such column exists.
*/
	{
	return KErrNone;
	}


#endif

