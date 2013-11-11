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

#ifndef PAIRINGSERVERSHARED_H
#define PAIRINGSERVERSHARED_H

#include <e32std.h>

const TInt KPairingServerMajorVersionNumber = 1;
const TInt KPairingServerMinorVersionNumber = 1;
const TInt KPairingServerBuildVersionNumber = 0;

_LIT(KPairingServerName, "!BTPairingSrv");

enum TPairingServerFunctions
	{
	// ==Session Functions==
	EPairingServerCloseSubSession,
	EPairingServerCreateOobDataSubSession,
	EPairingServerCreateDedicatedBondingSubSession,
	EPairingServerCreateSimplePairingResultSubSession,
	EPairingServerCreateAuthenticationResultSubSession,

	// ==OOB Subsession Functions==
	EPairingServerOobDataRefreshLocal,
	EPairingServerOobDataReadLocal,
	EPairingServerOobDataCancelReadLocal,
	EPairingServerOobDataProvideRemoteParsed,
	EPairingServerOobDataProvideRemoteRaw,
	EPairingServerOobDataClearRemote,

	// ==Dedicated Bonding Subsession Functions==
	EPairingServerStartDedicatedBond,

	// ==Simple Pairing Result Subsession Functions==
	EPairingServerSimplePairingResult,
	EPairingServerCancelSimplePairingResult,

	// ==Authentication Result Subsession Functions==
	EPairingServerAuthenticationResult,
	EPairingServerCancelAuthenticationResult,


	// !!!-----Add new server functions before this------!!!
	EPairingServerFunctionCount
	};

_LIT(KPairingServerFaultCat, "PairingSrvFault");
enum TPairingServerFaultReason
	{
	EPairingServerSessionContainerAndIndexMismatch,
	EPairingServerLinkManagerAlreadyProvided,
	EPairingServerNoMessageForObserverCompletion,
	EPairingServerUnexpectedSocketCallback,
	EPairingServerUnexpectedAccessCallback,
	EPairingServerBadShutdownState,
	EPairingServerMessageShouldBeNull,
	};

#endif // PAIRINGSERVERSHARED_H

