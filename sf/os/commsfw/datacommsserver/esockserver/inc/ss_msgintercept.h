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
// SS_MSGNTERCEPT.H
// 
//

/**
 @file
 @internalComponent 
*/

#if !defined(__SS_MSGINTERCEPT_H__)
#define __SS_MSGINTERCEPT_H__

#include <elements/sd_mintercept.h>

#if defined (__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)

// these macros are here to minimise impact on esock source code
#define ESOCK_DEBUG_MESSAGE_INTERCEPT(SENDER, MESSAGE, RECEIVER) ELEMENTS_DEBUG_MESSAGE_INTERCEPT(SENDER, MESSAGE, RECEIVER)
#define ESOCK_DEBUG_REGISTER_PROVIDER_NODE(UID, COMMSID, PROVIDERINFO) ELEMENTS_DEBUG_REGISTER_PROVIDER_NODE(UID, COMMSID, PROVIDERINFO)
#define ESOCK_DEBUG_REGISTER_GENERAL_NODE(UID, COMMSID) ELEMENTS_DEBUG_REGISTER_GENERAL_NODE(UID, COMMSID)

#else

#define ESOCK_DEBUG_MESSAGE_INTERCEPT(SENDER, MESSAGE, RECEIVER)
#define ESOCK_DEBUG_REGISTER_PROVIDER_NODE(UID, COMMSID, PROVIDERINFO)
#define ESOCK_DEBUG_REGISTER_GENERAL_NODE(UID, COMMSID)

#endif //(__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)

// UIDs for node types without factories (with their own UIDs)
namespace ESockDebug
	{
	const TInt KSocketNodeUid = 0x10000001; // TODO_CDG we should get some genuine ones!
	const TInt KConnectionNodeUid = 0x10000002; // TODO_CDG we should get some genuine ones!
	const TInt KSubConnectionNodeUid = 0x10000003; // TODO_CDG we should get some genuine ones!
	const TInt KHostResolverNodeUid = 0x10000004; // TODO_CDG we should get some genuine ones!
	}

#endif // !(__SS_MSGINTERCEPT_H__)

