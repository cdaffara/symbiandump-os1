// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

/**
@file
@internalTechnology
*/

#ifndef _RTXUTIL_H_
#define _RTXUTIL_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Utility functions */

EXTERNRT OSUINT32 rtxGetIdentByteCount (OSUINT32 ident);

EXTERNRT OSUINT32 rtxIntByteCount (OSINT32 val);

EXTERNRT OSUINT32 rtxOctetBitLen (OSOCTET w);

EXTERNRT OSUINT32 rtxUInt32BitLen (OSUINT32 w);

/**
 * This function retrieves the binary logarithm of the given value 
 * (by excess).
 *
 * @param w     Word value for which to get binary logarithm.
 * @return      The smallest x such as w <= 2^x
 */
EXTERNRT OSUINT32 rtxLog2Ceil (OSUINT32 w);

/**
 * This function retrieves the binary logarithm of the given value 
 * (by default).
 *
 * @param w     Word value for which to get binary logarithm.
 * @return      The smallest x such as 2^x <= w < 2^(x+1)
 */
EXTERNRT OSUINT32 rtxLog2Floor (OSUINT32 w);

#if !defined(_NO_INT64_SUPPORT)
EXTERNRT OSUINT32 rtxGetIdent64ByteCount (OSUINT64 ident);
EXTERNRT OSUINT32 rtxUInt64BitLen (OSUINT64 w);
#endif

#ifdef __cplusplus
}
#endif

#endif
