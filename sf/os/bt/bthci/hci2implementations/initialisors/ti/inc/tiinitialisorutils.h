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


#ifndef TICONTROLLERINITIALISORUTILS_H
#define TICONTROLLERINITIALISORUTILS_H

#include <e32base.h>

/**
The panic category for the TI controller initialisor.
*/
_LIT(KTiControllerInitialisorPanic, "TI Ctlr Init");

/**
Panic codes for TI controller initialisor.
*/
enum TTiControllerInitialisorPanic
	{
	EConfigInterfaceNull					= 0,
	EInvalidInitialisorState				= 1,
	ENullInitialisationObserver				= 2,
	EHciCommandQueueOverwritten				= 3,
	EInitialisationObserverOverwritten		= 4,
	ECoreHciOverwritten						= 5,
	};

#endif // TICONTROLLERINITIALISORUTILS_H
