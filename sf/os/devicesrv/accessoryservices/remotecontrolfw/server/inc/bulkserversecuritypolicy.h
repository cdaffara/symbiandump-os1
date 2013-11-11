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
// Server Security Policy definitions.
//



/**
 @file 
 @internalComponent
*/
 
#ifndef REMCONBULKSERVERSECURITYPOLICY_H
#define REMCONBULKSERVERSECURITYPOLICY_H

#include <e32base.h>
#include "remconserver.h"

/** RemCon Bulk Server Security Policy Definition */
const TUint KRemConBulkServerRangeCount = 2;

const TInt KRemConBulkServerRanges[KRemConBulkServerRangeCount] = 
	{
	ERemConBulkDbgMarkHeap,						/** pass */
	ERemConBulkNumberOfServerFunctions,			/** fail (to KMaxTInt) */
	};

/** Index numbers into KRemConServerElements[] */
const TInt KPolicyPass = 0;

/** Mapping IPCs to policy element */
const TUint8 KRemConBulkServerElementsIndex[KRemConBulkServerRangeCount] = 
	{
	KPolicyPass,					/** All (valid) APIs */
	CPolicyServer::ENotSupported,	/** remainder of possible IPCs */
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KRemConBulkServerElements[] = 
	{
		{ _INIT_SECURITY_POLICY_PASS },
	};

/** Main policy */
const CPolicyServer::TPolicy KRemConBulkServerPolicy = 
	{
	CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
	KRemConBulkServerRangeCount,
	KRemConBulkServerRanges,
	KRemConBulkServerElementsIndex,
	KRemConBulkServerElements,
	};
	
#endif // REMCONBULKSERVERSECURITYPOLICY_H
