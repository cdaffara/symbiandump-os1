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





#ifndef HWRMFMTXDOMAINPSKEYS_H
#define HWRMFMTXDOMAINPSKEYS_H

// INCLUDES
#include <e32std.h>


// =============================================================================
// HWRM FM Tx API
// =============================================================================
/**
*
* @publishedPartner
* @released
*/
const TUint32 KHWRMSid = 0x101F7A02; // SID of HWRM server
/**
*
* @publishedPartner
* @released
*/
static const TUid KPSUidHWRMFmTx = {KHWRMSid};

/**
* FM Transmitter status.
* This data is interpreted as TFmTxState
*
* @publishedPartner
* @released
*/
const TUint32 KHWRMFmTxStatus  = 0x00000001;

#endif      // HWRMFMTXDOMAINPSKEYS_H

// End of File

