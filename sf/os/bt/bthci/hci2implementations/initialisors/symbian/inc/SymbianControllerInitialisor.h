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


#ifndef SYMBIANCONTROLLERINITIALISOR_H
#define SYMBIANCONTROLLERINITIALISOR_H

#include <e32base.h>

/**
The panic category for the Symbian reference controller initialisor.
*/
_LIT(KSymbianControllerInitialisorPanic, "Symbian Ctlr Init");

/**
Panic codes for the Symbian reference controller initialisor
*/
enum TSymbianControllerInitialisorPanic
	{
	ENullInitialisationObserver				= 0,
	EInitialisationObserverOverwritten		= 1,
	EUnexpectedAbortInitialisation			= 2,
	};

#endif // SYMBIANCONTROLLERINITIALISOR_H

