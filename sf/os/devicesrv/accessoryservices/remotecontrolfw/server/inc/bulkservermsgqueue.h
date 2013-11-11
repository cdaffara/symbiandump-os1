// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BULKSERVERMSGQUEUE_H
#define BULKSERVERMSGQUEUE_H

#include <remcon/clientid.h>

NONSHARABLE_STRUCT(TBulkClientInfo)
	{
	// Process Id is used to match bulk session with target session
	TProcessId iProcessId;
	
	// Use to uniquely identify this client to allow bearers to address
	TRemConClientId iRemConId;
	
	// This contains only the bulk APIs
	RArray<TUid> iInterestedAPIs;
	
	// This is used by the receiver to queue.  Only the owner of this
	// object may queue it.
	TSglQueLink iClientInfoLink;
	};

enum TBulkServerMsgType
	{
	// Sent upon bulk server creation to provide the control server which
	// is used to map RemCon Client IDs between bulk and control sessions.
	EControlServer = 0,
	// Sent upon bulk server creation to provide the bearer manager which
	// can be used to extract (or create) the bulk interfaces from the bearer.
	EBearerManager = 1,
	};

NONSHARABLE_STRUCT(TBulkServerMsg)
	{
	TBulkServerMsgType	iType;
	TAny*				iData;
	};

#endif // BULKSERVERMSGQUEUE_H

