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
 * @file rtxBitEncode.h
 * Bit encode functions.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXBITENCODE_H_
#define _RTXBITENCODE_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function will set the bit at the current encode bit cursor 
 * position to 1 or 0 and advance the cursor pointer.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        The value to be encoded.
 */
EXTERNRT int rtxEncBit (OSCTXT* pctxt, OSBOOL value);

/**
 * This function will encode a series of bits (up to 32) from an unsigned
 * integer value/
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value       The value to be encoded.
 * @param nbits       Number of bits from the value to encode.
 * @return            Status of the operation.  Zero if successful; a 
 *                       negative status code if failed.
 */
EXTERNRT int rtxEncBits (OSCTXT* pctxt, OSUINT32 value, OSUINT32 nbits);

/**
 * This macro will byte-align the encode buffer in the context.
 */
#define rtxByteAlign(pctxt) \
if ((pctxt)->buffer.bitOffset != 8) { \
(pctxt)->buffer.byteIndex++; (pctxt)->buffer.bitOffset = 8; }

#ifdef __cplusplus
}
#endif

#endif
