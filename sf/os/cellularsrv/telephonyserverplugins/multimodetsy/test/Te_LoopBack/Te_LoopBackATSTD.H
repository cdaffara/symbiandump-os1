// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains no class definitions.  It contains the panic information
// used in the Etel Regression Test Harness and a couple of globally useful
// string definitions.
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __ATSTD_H
#define __ATSTD_H

#include <e32std.h>

#define KOneSecondPause	1000				// in milliseconds

//	Expected modem responses
_LIT8(KOKString,"\nOK\r\n");
_LIT8(KErrorString,"\r\nERROR\r\n");

// Panics
enum THayesPanic
	{
	EATCommand_NotConstructed,
	EATCommand_IllegalWaitCompletion,
	EATCommand_IllegalCompletionWriteNotExpected,
	EATCommand_IllegalCompletionWriteExpected,
	EATCommand_IllegalCompletionReadNotExpected
	};

GLREF_C void HayesPanic(THayesPanic aPanicNumber);

#endif
