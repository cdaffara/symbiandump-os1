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
 * @file rtxBitDecode.h
 * Bit decode functions.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXBITDECODE_H_
#define _RTXBITDECODE_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This function decodes up to sizeof(unsigned) bits
 *  and returns the result in an unsigned integer value.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pvalue      Pointer to value to be receive decoded result.
 * @param nbits       Number of bits to read from decode buffer.
 * @return            Status of the operation.  Zero if successful; a 
 *                       negative status code if failed.
 */
EXTERNRT int rtxDecBits (OSCTXT* pctxt, OSUINT32* pvalue, OSUINT32 nbits);

#ifdef __cplusplus
}
#endif

#endif
