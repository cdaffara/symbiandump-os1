/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/





#ifndef HWRMFMTXINTERNALPSKEYS_H
#define HWRMFMTXINTERNALPSKEYS_H

// INCLUDES
#include <e32std.h>
#include <hwrm/hwrmfmtxdomainpskeys.h>

// =============================================================================
// HWRM FM Tx API
// =============================================================================

/**
* RDS PTY (integer)
*
* @internalComponent
* @released
*/
const TUint32 KHWRMFmTxRdsPty = 0x00000010; 

/**
* RDS PTYN (string)
*
* @internalComponent
* @released
*/
const TUint32 KHWRMFmTxRdsPtyn = 0x00000011; 

/**
* RDS music/speech flag
*
* @internalComponent
* @released
*/
const TUint32 KHWRMFmTxRdsMs = 0x00000012; 

/**
* RDS language ID
*
* @internalComponent
* @released
*/
const TUint32 KHWRMFmTxRdsLangId = 0x00000013; 

#endif      // HWRMFMTXINTERNALPSKEYS_H

// End of File

