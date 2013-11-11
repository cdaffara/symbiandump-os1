// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "NetConLog.h"
#include "NetConPanic.h"

//
//                           //
//  Network Controller Panic //
//                           //
//

_LIT(KNetConPanic, "NetworkController");

GLDEF_C void NetConPanic(NetworkController::TNetConPanic aPanic)
/**
 * Panic - programming error!
 *
 */
	{
	LOG( NetConLog::Printf(_L("NetworkController Panic %d"), aPanic); )
	User::Panic(KNetConPanic, aPanic);
	}

