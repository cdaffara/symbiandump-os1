// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/



#if !defined(__ES_PANIC_PARTNER_H__)
#define __ES_PANIC_PARTNER_H__


#include <e32base.h>

/** The panic category for panics caused by protocol modules (was "eSock protocol" in previous versions)
@publishedPartner
@released
@see TProtocolPanic
*/
_LIT(KESockProtocolPanic, "ESock_protocol");

/**	The panic numbers for panics caused by protocol modules
@publishedPartner
@released
@see KESockProtocolPanic
*/
enum TProtocolPanic
	{
	ENothingToComplete = 0,
	EUnexpectedConnect = 1,
	EUnexpectedClose = 2,
	EUnexpectedConnectData = 3,
	EUnexpectedDisconnectData = 4,
	ERemoveRefused = 5,
	EBadStateUpCall = 6,
	EBadDataCount = 7,
	EBadErrorCall = 8,
	EBadDisconnect = 9,
	EBadByteCount = 10,
	EBadProtocolDescription = 11,
	EErrorCallNotHandled = 12,
	ECantProcessDescriptors = 13,
	ECantProcessMbufs = 14,
	ECantSendDescriptors = 15,
	ECantSendMBufs = 16,
	EDoesNotBindAbove = 17,
	EDoesNotBindBelow = 18,
	ENotListeningSocket = 19,
	EAcceptQueFull = 20,
	EProviderIncomplete = 21,
	ENullSap = 22,
	EBadResolverState = 23,
	EMisconfigured = 24,
	ECorruptIniData = 25,
	EDuplicateWorkerId = 26,
	ENonESockWorkerThread = 27,
	EBadSubSessionUniqueId = 28,
	EAmbigiousRoles = 29,
	EBadDisconnectFromListener = 30
	};


#endif // __ES_PANIC_PARTNER_H__
