// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef _EIRMANSERVERSECURITYPOLICY_H
#define _EIRMANSERVERSECURITYPOLICY_H

#include <e32def.h>
#include <bluetooth/eirmanshared.h>


_LIT_SECURE_ID(KSDPServerID, 0x10009220); 
_LIT_SECURE_ID(KStackID, 0x101f7989); 

/** EIR Manager Server Security Policy Definition */
const TUint KEirManServerRangeCount = 3;

const TInt KEirManServerRanges[KEirManServerRangeCount] = 
	{
	EEirManRegisterTag,					/** check comes from SDP */
	EEirManSpaceAvailableNotification,	/** check has local services **/
	EEirManServerMessageCount,			/** not supported */
	};

/** Index numbers into KEirManServerElements[] */
const TInt KPolicyCapabilityCheck = 0;

/** Mapping IPCs to policy element */
const TUint8 KEirManServerElementsIndex[KEirManServerRangeCount] = 
	{
	CPolicyServer::ECustomCheck,
	KPolicyCapabilityCheck, 
	CPolicyServer::ENotSupported,	/** Undefined functions */
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KEirManServerElements[] = 
	{
		{ _INIT_SECURITY_POLICY_C1(ECapabilityLocalServices) }
	};

/** Main policy */
const CPolicyServer::TPolicy KEirManServerPolicy = 
	{
	KPolicyCapabilityCheck , /** Connect attempts are only successful if the caller has ECapabilityLocalServices */
	KEirManServerRangeCount,
	KEirManServerRanges,
	KEirManServerElementsIndex,
	KEirManServerElements,
	};
	
#endif // _EIRMANSERVERSECURITYPOLICY_H
