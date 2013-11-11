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
* Definitions of private P&S keys of SsmCustCmds component.
*
*/

#ifndef SSMCUSTCMDSPRIVATEPSKEYS_H
#define SSMCUSTCMDSPRIVATEPSKEYS_H

#include <e32cmn.h>

/**
 * Initialized at boot-time to zero. Once startup ends, key is set.
 * Once key set, ssmdiskreserver frees reserved disk space.
 * Alternatively some other normal state indicating mechanism could
 * have been used in ssmdiskreserver to free reserved disk space,
 * but this approach makes space reservation more controllable.
 *
 * Used with PS UID KPSStarterUid
 */
const TUint32 KDiskReserveKey = 0x00000002;

#endif // SSMCUSTCMDSPRIVATEPSKEYS_H
