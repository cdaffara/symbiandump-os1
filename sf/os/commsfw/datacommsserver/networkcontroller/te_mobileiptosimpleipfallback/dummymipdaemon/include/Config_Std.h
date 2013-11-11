/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Provides constants common across the test configuration
* daemon.
* 
*
*/



/**
 @file
 @internalComponent
*/

#ifndef __CONFIG_STD_H__
#define __CONFIG_STD_H__

#include <comms-infras/commsdebugutility.h>
#include <es_sock.h>
#include <es_enum.h>
#include <in_sock.h>

IMPORT_C TInt E32Main();

/** The name of the server. */
_LIT(KConfigServerName,"!DummyMipDaemon"); 

/** Version info for the server. */
const TInt KConfigSrvMajorVersionNumber = 1;
const TInt KConfigSrvMinorVersionNumber = 0;
const TInt KConfigSrvBuildVersionNumber = 0;

/** Logging tags. */
__FLOG_STMT(_LIT8(KConfigLoggingTag1,"Netcon");)
__FLOG_STMT(_LIT8(KConfigLoggingTag2,"DummyMipDaemon");)

/** One second in microseconds. For convenience. */
const TUint	KOneSecond = 1000000; // 1000000 microseconds

#endif // __CONFIG_STD_H__


