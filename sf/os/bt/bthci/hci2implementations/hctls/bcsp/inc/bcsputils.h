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

#ifndef BCSPUTILS_H
#define BCSPUTILS_H

#include <e32def.h>

/**
The panic category for the BCSP HCTL.
*/
_LIT(KBcspPanicCat, "BCSP Panic");

/**
Panic codes for BCSP.
*/
enum TBcspPanic
	{
	EUnexpectedLinkEvent		    = 0,
	EBadFramePointer				= 1,
	EBadBcspFrame					= 2,
	EBadLinkPointer					= 3,
	EPortNotOpen					= 4,
	EReliableQueueFull				= 5,
	EBcspIllegalProtocolId			= 6,
	EBcspInvalidAckValue			= 7,
	EBcspInvalidSequenceValue		= 8,
	EUnexpectedPowerState			= 9,
	EUnexpectedReset				= 10,
	EUnexpectedControllerMgrState	= 11,
	EInvalidCurrentTask				= 12,
	EBCSPLinkStateOutOfBounds		= 13
	};

#endif // BCSPUTILS_H

