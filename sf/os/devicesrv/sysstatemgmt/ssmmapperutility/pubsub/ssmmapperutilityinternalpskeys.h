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
* Definitions of internal PS keys of SsmMapperUtility component.
*
*/

#ifndef SSMMAPPERUTILITYINTERNALPSKEYS_H
#define SSMMAPPERUTILITYINTERNALPSKEYS_H

#include <e32cmn.h>

/**
 * PS UID to use with KStarterSecurityPhase
 */
static const TUid KPSStarterUid = { 0x100059C9 };

/**
 * Initialized at boot-time to indicate RTC validity.
 *
 * Used with PS UID KPSStarterUid
 */
const TUint32 KRTCPropertyKey = 0x00000003;

#endif // SSMPOLICYPLUGINSPRIVATECRKEYS_H
