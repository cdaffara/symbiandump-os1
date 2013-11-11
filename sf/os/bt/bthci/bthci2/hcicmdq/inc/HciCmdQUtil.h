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

#ifndef HCICMDQUTIL_H
#define HCICMDQUTIL_H

_LIT(KHCICmdQPanic, "HCI Cmd Queue");

enum THCICmdQPanic
	{
	ECanSendBlockWhenEmpty = 1,
	ECanSendDeadlock = 2,
	ECommandTimeoutTooBig = 3,
	EInvalidCommandQueueItemId = 4,
	EIllegalBlock = 5,
	EIllegalRequestForQdpInterface = 6,
	EInitCmdAfterStart = 7,
	EHctlFlowControlError = 8,
	EUnknownDeadlock = 9,
	ERejectCommandWithNoError = 10,
	EResendDeadlock = 11,
	EStartError = 12,
	ETimedOutCommandError = 13,
	ETimerError = 14,
	ETryToSendWhilstSending = 15,
	EObjectNotInitialised = 16,
	EInvalidArgument = 17,
	EAttemptToAddNullCommand = 18,
	ENullTimerClient = 19,
	EInvalidStateTransition = 20,
	EUnknownActiveQueue = 21,
	EHctlInterfaceInitialisedTwice = 22,
	EHciCommandAllocatorInterfaceInitialisedTwice = 23,
	ELinkMuxNotifierInitialisedTwice = 24,
	EUnmatchedEventObserverInitialisedTwice = 25,
	EHardResetInitiatorInitialisedTwice = 26,
	EPossibleQdpDataLeak = 27,
	ECommandCreditsCountLessThanZero = 28,
	EInitCommandQNotEmptyInDestructor = 29,
	ENormalCommandQNotEmptyInDestructor = 30,
	ESentCommandQNotEmptyInDestructor = 31,
	EResendCommandQNotEmptyInDestructor = 32,
	EStartCalledWhenInitQNotEmpty = 33,
	EStarvationTimerFired = 34,
	EPriorityInsertPointNotNullWhenPendingQEmpty = 35,
	EInvalidCmdQueueItemType = 36,
	EQStarvationTimerNotGreaterThanMaxCmdTimer = 37,
	ETryingToDeleteCommandNotOwned = 38,
	ETryToSendWhileUnoperational = 39,
	ENullCmdQItemPtr = 40,
	EPriorityCommandQNotEmptyInDestructor = 41,
	EWorkaroundCommandQNotEmptyInDestructor = 42,
	ETryingToDeletePreChildCommand = 43,
	ETryingToDeletePostChildCommand = 44,
	ETryingToDeletePreChildCommandById = 45,
	ETryingToDeletePostChildCommandById = 46,
	ETooManyItemsOnWorkaroundQueue = 47,
	EQdpTryingToChangeEventCode = 48,
	};

#endif // HCICMDQUTIL_H
