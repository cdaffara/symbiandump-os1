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
 * @file rtxBitString.h - Contains utility functions for setting, clearing, 
 * and testing bits at any position in an arbitrarily sized array of bytes.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXBITSTRING_H_
#define _RTXBITSTRING_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bitstrhelpers Bit String Functions 
 * @{
 * Bit string functions allow bits to be set, cleared, or tested in 
 * arbitrarily sized byte arrays.
 */
/**
 * This macro is used to calculate the byte array size required to hold 
 * the given number of bits.
 */
#define OSRTBYTEARRAYSIZE(numbits) (((numbits-1)/8)+1)

/**
 * This function sets the specified bit in the bit string.
 *
 * @param pBits        Pointer to octets of bit string.
 * @param numbits      Number of bits in the bit string.
 * @param bitIndex     Index of bit to be set. The bit with index 0 is a most
 *                       significant bit in the octet with index 0.
 * @return             If successful, returns the previous state of bit. If bit
 *                       was set the return value is positive, if bit was not
 *                       set the return value is zero. Otherwise, return value
 *                       is an error code:
 *                       - RTERR_OUTOFBND = bitIndex is out of bounds
 */
EXTERNRT int rtxSetBit (OSOCTET* pBits, OSUINT32 numbits, OSUINT32 bitIndex);

/**
 * This function sets one or more bits to TRUE or FALSE in a 32-bit unsigned 
 * bit flag set.
 *
 * @param flags        Flags to which mask will be applied.
 * @param mask         Mask with one or more bits set that will be applied 
 *                       to pBitMask.
 * @param action       Boolean action indicating if bits in flags should be 
 *                       set (TRUE) or cleared (FALSE).
 * @return             Updated flags after mask is applied.
 */
EXTERNRT OSUINT32 rtxSetBitFlags 
(OSUINT32 flags, OSUINT32 mask, OSBOOL action);

/**
 * This function clears the specified bit in the bit string.
 *
 * @param pBits        Pointer to octets of bit string.
 * @param numbits      Number of bits in the bit string.
 * @param bitIndex     Index of bit to be cleared. The bit with index 0 is a
 *                       most significant bit in the octet with index 0.
 * @return             If successful, returns the previous state of bit. If bit
 *                       was set the return value is positive, if bit was not
 *                       set the return value is zero. Otherwise, return value
 *                       is an error code:
 *                       - RTERR_OUTOFBND = bitIndex is out of bounds
 */
EXTERNRT int rtxClearBit 
(OSOCTET* pBits, OSUINT32 numbits, OSUINT32 bitIndex);

/**
 * This function tests the specified bit in the bit string.
 *
 * @param pBits        Pointer to octets of bit string.
 * @param numbits      Number of bits in the bit string.
 * @param bitIndex     Index of bit to be tested. The bit with index 0 is a
 *                       most significant bit in the octet with index 0.
 * @return             True if bit set or false if not set or array index is
 *                       beyond range of number of bits in the string.
 */
EXTERNRT OSBOOL rtxTestBit 
(const OSOCTET* pBits, OSUINT32 numbits, OSUINT32 bitIndex);

/**
 * @}bitstrhelpers
 */
#ifdef __cplusplus
}
#endif

#endif
