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


#ifndef UARTORIGINALUTILS_H
#define UARTORIGINALUTILS_H

#include <e32base.h>

/**
The panic category for the UART ORIGINAL HCTL.
*/
_LIT(KUartOriginalPanic, "UART Orig. Panic");

/**
Panic codes for UART_ORIGINAL HCTL.
*/
enum TUartOriginalPanic
	{
	EHctlReceiverBufferOverflow				= 0,
	EIllegalState							= 1,
	EPortNotOpen							= 2,
	EUnexpectedCtrlMgrPowerState			= 3,
	EUnexpectedReset						= 4,
	EPortReadAttemptWhenReadOutstanding 	= 5,
	EAttemptToReadZeroBytes					= 6,
	EReadCompletedWithInsufficientBytes 	= 7,
	EStateObserverNotAvailable				= 8,
	EStartCalledWhenReadOutstanding			= 9,
	ENoControllerManager					= 10,
	};

#endif // UARTORIGINALUTILS_H

