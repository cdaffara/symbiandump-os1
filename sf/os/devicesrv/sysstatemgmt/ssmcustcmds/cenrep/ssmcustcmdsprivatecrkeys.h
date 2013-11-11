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
* Definitions of private CenRep keys of SsmCustCmds component.
*
*/

#ifndef SSMCUSTCMDSPRIVATECRKEYS_H
#define SSMCUSTCMDSPRIVATECRKEYS_H

#include <e32cmn.h>

// ===========================================================================
// SsmSystemCmds Stored Values API
// ===========================================================================
//
static const TUid KCRUidSsmSystemCmds = { 0x10205073 }; // SsmSystemCmds DLL UID

/**
* Used for storing the IMSI code of current SIM card.
* This is needed for determining if SIM card has changed from the previous boot.
* Key value is a descriptor containing the IMSI code.
*/
const TUint32 KSsmSystemCmdsStoredImsi = 0x1;

#endif // SSMCUSTCMDSPRIVATECRKEYS_H
