/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definitions of private CenRep keys of SsmPolicyPlugins component.
*
*/

#ifndef SSMPOLICYPLUGINSPRIVATECRKEYS_H
#define SSMPOLICYPLUGINSPRIVATECRKEYS_H

#include <e32cmn.h>

// ===========================================================================
// SsmStartupPolicy Configuration API
// ===========================================================================
//
static const TUid KCRUidSsmStartupPolicy = { 0x20021346 }; // SsmStartupPolicy DLL UID

// Key range for this API is 0x1 - 0xFFFF

/**
* Indicates the maximum amount of subsequent resets allowed.
*/
const TUint32 KSsmStartupErrorResetLimit = 0x1;

// ===========================================================================
// SsmStartupPolicy Stored Values API
// ===========================================================================
//
// Use KCRUidSsmStartupPolicy

// Key range for this API is 0x10000 ->

/**
* Indicates the amount of subsequent resets.
*/
const TUint32 KSsmStartupErrorResetCounter = 0x10000;

#endif // SSMPOLICYPLUGINSPRIVATECRKEYS_H
