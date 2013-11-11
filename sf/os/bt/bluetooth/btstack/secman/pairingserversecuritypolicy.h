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

#ifndef PAIRINGSERVERSECURITYPOLICY_H
#define PAIRINGSERVERSECURITYPOLICY_H

#include <e32def.h>

const TUint KPairingServerRangeCount = 5;

const TInt KPairingServerRanges[KPairingServerRangeCount] =
	{
	// Local Services
	EPairingServerCloseSubSession,
//	EPairingServerCreateOobDataSubSession,
//	EPairingServerCreateDedicatedBondingSubSession,

	// Local Services & Read Device Data
	EPairingServerCreateSimplePairingResultSubSession,
//	EPairingServerCreateAuthenticationResultSubSession,

	// Local Services
	EPairingServerOobDataRefreshLocal,
//	EPairingServerOobDataReadLocal,
//	EPairingServerOobDataCancelReadLocal,
//	EPairingServerOobDataProvideRemoteParsed,
//	EPairingServerOobDataProvideRemoteRaw,
//	EPairingServerOobDataClearRemote,
//	EPairingServerStartDedicatedBond,

	// Local Services & Read Device Data
	EPairingServerSimplePairingResult,
//	EPairingServerCancelSimplePairingResult,
//	EPairingServerAuthenticationResult,
//	EPairingServerCancelAuthenticationResult,

	// Not Supported
	EPairingServerFunctionCount,
	};

/** Index numbers into KPairingServerElements[] */
const TInt KPolicyLocalServices						= 0;
const TInt KPolicyLocalServicesReadDeviceData		= 1;
const TInt KPolicyLocalServicesPanic				= 2;
const TInt KPolicyLocalServiceReadDeviceDataPanic	= 3;

/** Mapping IPCs to policy element */
const TUint8 KPairingServerElementsIndex[KPairingServerRangeCount] =
	{
	KPolicyLocalServices,
	KPolicyLocalServicesReadDeviceData,
	KPolicyLocalServicesPanic,
	KPolicyLocalServiceReadDeviceDataPanic,
	CPolicyServer::ENotSupported,	/** Undefined functions */
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KPairingServerElements[] =
	{
		{ _INIT_SECURITY_POLICY_C1(ECapabilityLocalServices), CPolicyServer::EFailClient },
		{ _INIT_SECURITY_POLICY_C2(ECapabilityLocalServices, ECapabilityReadDeviceData), CPolicyServer::EFailClient },
		{ _INIT_SECURITY_POLICY_C1(ECapabilityLocalServices), CPolicyServer::EPanicClient },
		{ _INIT_SECURITY_POLICY_C2(ECapabilityLocalServices, ECapabilityReadDeviceData), CPolicyServer::EPanicClient }
	};

/** Main policy */
const CPolicyServer::TPolicy KPairingServerPolicy =
	{
	CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
	KPairingServerRangeCount,
	KPairingServerRanges,
	KPairingServerElementsIndex,
	KPairingServerElements,
	};

#endif // __PAIRINGSERVERSECURITYPOLICY_H__
