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

#ifndef COREHCIUTIL_H
#define COREHCIUTIL_H

#include <e32def.h>

/**
The panic category for code associated with the HCI Server.
*/
_LIT(KHciServerPanicCat, "HCIServer");

/**
Panic codes for Core HCI Server.
*/
enum TCoreHciServerPanic
	{
	EMessageAlreadyCompleted = 0,
	ENoServiceProvidedOnConnect = 1,
	ENoRequestManagerToProvide = 2,
	EPwrCtrlReleasedWhenNotLocked = 3,
	EHciDaReleasedWhenNotLocked = 4,
	EUnknownServiceReleased = 5,
	ENonVendorOpcodeCompleted = 6,
	};

/**
The panic category for code associated with the rest of the Core HCI.
*/
_LIT(KCoreHciPanicCat, "CoreHCI");

/**
Panic codes for Core HCI.
*/
enum TCoreHciPanic
	{
	EUninitialisedInterface = 0,
	EInterfaceAlreadyInitialised = 1,
	EReceivedUnmatchedEvent = 2,
	EReceivedUnknownEvent = 3,
	};

/**
Panic the HCI server client, see hciipc.h for client panic codes.
*/
void PanicClient(const RMessage2& aMessage, TInt aPanic);
	
#endif // COREHCIUTIL_H
