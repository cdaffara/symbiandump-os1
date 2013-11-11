/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Internal definitions file
*
*/


#ifndef PSMDEFINES_H
#define PSMDEFINES_H

#include <e32base.h>
#include "psmtrace.h"

/**
* PSM server process name
*/
_LIT(KPsmProcessName, "!PsmServer");

/**
* PSM server main thread name
*/
_LIT(KPsmMainThreadName, "PsmMain");

/**
* PSM server executable name
*/
_LIT( KPsmExeName, "psmserver.exe" );

/**
* Drive identifier of the drive where PSM server executable is.
*/
_LIT( KPsmExeDrive, "z:" );

/**
* Number of times PSM server client should try reconneting if session is not found.
*/
const TInt KPsmClientTryCount(2);

/**
* Major version number
*/
const TInt KPsmVersionMajor(0);

/**
* Minor version number
*/
const TInt KPsmVersionMinor(0);

/**
* Build number
*/
const TInt KPsmVersionBuild(1);

/**
* Async message slots to be used in client server comms. 
*/
const TInt KPsmServerDefaultAsyncSlots(4); 

/**
* Separate TInt needed or DLL's won't compile because of static data if UID is used in some policy lit
*/
const TInt KPsmServerUid2Int(0x2000B187);  

/**
* PSM Server UID
*/
const TUid KPsmServerUid2 = { KPsmServerUid2Int }; 

#endif // PSMDEFINES_H
