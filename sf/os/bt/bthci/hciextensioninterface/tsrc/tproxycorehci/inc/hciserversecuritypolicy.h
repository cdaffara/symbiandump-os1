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
//

/**
 @file 
 @internalComponent
*/
 
#ifndef HCISERVERSECURITYPOLICY_H
#define HCISERVERSECURITYPOLICY_H

#include <e32def.h>
#include <bluetooth/hci/hciipc.h>

/**
All the security checks are done on a per HCI server service basis and
therefore the policy defined here for the policy server allows all requests.
These can then be rejected by the HCI server if they do not meet the security
defined for the service that is being used.
*/

/** HCI Server Security Policy Definition */
const TUint KHCIServerRangeCount = 2;

const TInt KHCIServerRanges[KHCIServerRangeCount] = 
	{
	EConnectToService,				/** pass */
	ENumOfHciServerFunctions,		/** not supported */
	};

/** Index numbers into KHCIServerElements[] */
const TInt KPolicyPass = 0;

/** Mapping IPCs to policy element */
const TUint8 KHCIServerElementsIndex[KHCIServerRangeCount] = 
	{
	KPolicyPass,					/** All (valid) APIs */
	CPolicyServer::ENotSupported,	/** Undefined functions */
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KHCIServerElements[] = 
	{
		{ _INIT_SECURITY_POLICY_PASS },
	};

/** Main policy */
const CPolicyServer::TPolicy KHCIServerPolicy = 
	{
	CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
	KHCIServerRangeCount,
	KHCIServerRanges,
	KHCIServerElementsIndex,
	KHCIServerElements,
	};
	
#endif // HCISERVERSECURITYPOLICY_H
