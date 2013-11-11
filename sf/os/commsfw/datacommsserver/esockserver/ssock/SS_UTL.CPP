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
//

/**
 @file
 @internalComponent 
*/

#include <ss_std.h>
#include <c32comm.h>
#include <f32file.h>

/**
Panic the program.

*/
GLDEF_C void Fault(TESockFault aFault)
	{
	User::Panic(KESockFaultPanic,aFault);
	}


/**
Panic the protocol
*/
EXPORT_C void Panic(TProtocolPanic aPanic)
	{
	User::Panic(KESockProtocolPanic, aPanic);
	}

EXPORT_C TInt StartESOCK(TAny * /*AnArg*/)
    {
    // We only support StartESOCK on WINC
    return KErrNotSupported;
    }

