/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Domain Central Repository key definitions for
*                CommonEngine subsystem
*
*/


#ifndef COMMONENGINEDOMAINCRKEYS_H
#define COMMONENGINEDOMAINCRKEYS_H


// INCLUDES
#include <e32std.h>

// =============================================================================
// General CR API for publishing CR keys needed in base services
// =============================================================================

const TUid KCRUidCommonEngineKeys = {0x1020503B};

/**
* Display Text Language, integer value
*
* Possible values are:
* 0 (automatic)
* Valid Symbian OS language code
*
* Default value: 0
**/
const TUint32  KGSDisplayTxtLang = 0x00000001;

/**
* Collation code, integer value
*
* Possible values are:
* 0 (automatic)
* Valid Symbian OS collation(language) code
*
* Default value: 0
**/
const TUint32  KGSCollation = 0x00000002;

/**
* Region code, integer value
*
* Possible values are:
* 0 (automatic)
* Valid Symbian OS region code
*
* Default value: 0
**/
const TUint32  KGSRegion = 0x00000003;

#endif // COMMONENGINEDOMAINCRKEYS_H
