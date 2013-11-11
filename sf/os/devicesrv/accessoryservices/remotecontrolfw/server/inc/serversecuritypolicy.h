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
// Server Security Policy definitions.
// 
//

/**
 @file 
 @internalComponent
*/
 
#ifndef REMCONSERVERSECURITYPOLICY_H
#define REMCONSERVERSECURITYPOLICY_H

#include <e32base.h>
#include "remconserver.h"

/** RemCon Server Security Policy Definition */
const TUint KRemConServerRangeCount = 2;

const TInt KRemConServerRanges[KRemConServerRangeCount] = 
	{
	ERemConDbgMarkHeap, 			/** pass */
	ERemConNumberOfServerFunctions,	/** fail (to KMaxTInt) */
	};

/** Index numbers into KRemConServerElements[] */
const TInt KPolicyPass = 0;

/** Mapping IPCs to policy element */
const TUint8 KRemConServerElementsIndex[KRemConServerRangeCount] = 
	{
	KPolicyPass,					/** All (valid) APIs */
	CPolicyServer::ENotSupported,	/** remainder of possible IPCs */
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KRemConServerElements[] = 
	{
		{ _INIT_SECURITY_POLICY_PASS },
	};

/** Main policy */
const CPolicyServer::TPolicy KRemConServerPolicy = 
	{
	CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
	KRemConServerRangeCount,
	KRemConServerRanges,
	KRemConServerElementsIndex,
	KRemConServerElements,
	};
	
#endif // REMCONMANSERVERSECURITYPOLICY_H
