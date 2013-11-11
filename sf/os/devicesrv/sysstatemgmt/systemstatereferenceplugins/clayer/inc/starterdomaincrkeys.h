/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, Startup Status API.
* Domain Central Repository definitions of System Startup subsystem.
*
*/



#ifndef STARTERDOMAINCRKEYS_H
#define STARTERDOMAINCRKEYS_H

#include <centralrepository.h>
#include <ssm/startupreason.h>

/*
* @publishedPartner
* @released
*/
// =============================================================================
// Startup Status API
// =============================================================================
const TUid KCRUidStartup = { 0x101F8762 };

/**
* Used by Starter to store the additional startup reason (language switch, RFS,
* etc.) over boot.
*
* Contains one of the values of TStartupReason enumeration defined in
* startupreason.h.
*/
const TUint32 KStartupReason = 0x00000002;

/**
* Used by Starter to check whether this is the first boot. The value is set by
* Startup Application.
*/
const TUint32 KStartupFirstBoot = 0x00000003;
enum TStartupFirstBoot
    {
    EStartupIsFirstBoot = 0,
    EStartupNotFirstBoot
    };

/**
* Used by Starter to store the device lock status
*/
const TUint32 KStartupDevLockStatus = 0x00000004;
enum TStartupDevLockSuccess
    {
    EStartupDevLockNotSucess = 100,
    EStartupDevLockSucess
    };

#endif // STARTERDOMAINCRKEYS_H

// End of File

