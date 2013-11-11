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
 * @file rtxDynBitSet.h - Implementation of a dynamic bit set similar to 
 * the Java BitSet class. 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXDYNBITSET_H_
#define _RTXDYNBITSET_H_

#include "rtxsrc/rtxBitString.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSRTBITSETSEGSIZE 16    /* Segment size in bytes                */

typedef struct {
   OSUINT16     nbytes;         /* Number of bytes in allocated array   */
   OSUINT16     maxbit;         /* Maximum bit number currently set     */
   OSUINT16     segsize;        /* Segment size for expansions          */
   OSUINT16     spare;          /* 32-bit word boundary padding         */
   OSOCTET*     data;           /* Dynamic array to hold bits           */
} OSRTDynBitSet;

/**
 * This function initializes a dynamic bit set structure.  Memory is 
 * allocated for the initial segment.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pbitset      Pointer to bit set structure to be initialized.
 * @param segNBytes    Number of bytes per segment expansion.  If zero, 
 *                       the default value is used.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynBitSetInit 
(OSCTXT* pctxt, OSRTDynBitSet* pbitset, OSUINT16 segNBytes);

/**
 * This function frees dynamic memory held by the bit set.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pbitset      Pointer to bit set structure to be freed.
 */
EXTERNRT void rtxDynBitSetFree (OSCTXT* pctxt, OSRTDynBitSet* pbitset);

/**
 * This function creates a deep copy of the given bit set.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pSrcBitSet   Pointer to bit set structure to be copied.
 * @param pDestBitSet  Pointer to bit set structure to recieve copied data.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynBitSetCopy 
(OSCTXT* pctxt, const OSRTDynBitSet* pSrcBitSet, OSRTDynBitSet* pDestBitSet);

/**
 * This function sets the bit at the given index.  The bit set will be 
 * expanded if the given index is outside the currently allocated range.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pbitset      Pointer to bit set structure.
 * @param index        Index of bit to be set.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynBitSetSetBit 
(OSCTXT* pctxt, OSRTDynBitSet* pbitset, OSUINT32 index);

/**
 * This function clears the bit at the given index.  The bit set will be 
 * not be expanded if the given index is outside the currently allocated 
 * range.  The bit will be assumed to already be clear since it is 
 * undefined.
 *
 * @param pbitset      Pointer to bit set structure.
 * @param index        Index of bit to be clear.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynBitSetClearBit (OSRTDynBitSet* pbitset, OSUINT32 index);

/**
 * This function tests the bit at the given index.  If the index is 
 * outside the range of the currently allocated set, the bit is 
 * assumed to be clear; otherwise, the state of the bit in the set
 * is tested.
 *
 * @param pbitset      Pointer to bit set structure.
 * @param index        Index of bit to be tested.
 * @return             Boolean result: true if bit set; false if clear.
 */
EXTERNRT OSBOOL rtxDynBitSetTestBit 
(const OSRTDynBitSet* pbitset, OSUINT32 index);

/**
 * This function sets the bit at the given index to the give value.  The 
 * bit set will be expanded if the given index is outside the currently 
 * allocated range.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pbitset      Pointer to bit set structure.
 * @param index        Index of bit to be set.
 * @param value        Boolean value to which bit is to be set.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynBitSetSetBitToValue 
(OSCTXT* pctxt, OSRTDynBitSet* pbitset, OSUINT32 index, OSBOOL value);

/**
 * This function inserts a bit with the given value at the given index.
 * All other bits are shifted to the right one position.  If the maximum 
 * set bit number is at the end of the allocated range, the set is 
 * expanded.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pbitset      Pointer to bit set structure.
 * @param index        Index of position where bit is to be inserted.
 * @param value        Boolean value of the bit.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynBitSetInsertBit 
(OSCTXT* pctxt, OSRTDynBitSet* pbitset, OSUINT32 index, OSBOOL value);

#ifdef _TRACE
/**
 * This function prints the contents of a bit set as a binary string.
 *
 * @param pbitset      Pointer to bit set structure.
 * @param varname      Variable name associated with bit set.
 */
EXTERNRT void rtxDynBitSetPrint 
(const OSRTDynBitSet* pbitset, const char* varname);

#endif

#ifdef __cplusplus
}
#endif

#endif
