// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file NetCon.cpp
 @internalComponent
*/

#include <e32std.h>
#include "NetConLog.h"
#include "CNetworkController.h"
#include "NetConPanic.h"

/**
First ordinal export

@return a pointer to class CNetworkController
*/
extern "C" EXPORT_C CNetworkController* InstallL()
	{	
	
	return CNetworkController::NewL();
	}

_LIT(KNetConPanic, "NetworkController"); //< Network Controller Panic

/**
Panic - programming error!

@param aPanic, NetworkController Panic.
@panic aPanic, NetworkController Panic.
*/
GLDEF_C void NetConPanic(NetworkController::TNetConPanic aPanic)
	{
	LOG( NetConLog::Printf(_L("NetworkController Panic %d"), aPanic); )
	User::Panic(KNetConPanic, aPanic);
	}


