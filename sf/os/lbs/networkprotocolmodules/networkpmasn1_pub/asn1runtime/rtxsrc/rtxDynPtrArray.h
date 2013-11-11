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
 * @file rtxDynPtrArray.h - Implementation of a dynamic pointer array.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXDYNPTRARRAY_H_
#define _RTXDYNPTRARRAY_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
   OSUINT16     capacity;       /* Current size of the structure        */
   OSUINT16     count;          /* Number of pointers in array          */
   void**       data;           /* The actual pointer values            */
} OSRTDynPtrArray;

/**
 * This function initializes a new dynamic pointer array structure.  
 * Memory is allocated for the initial capacity of pointers.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pArray       Pointer to dynamic pointer array structure.
 * @param initialSize  Initial capacilty of the array.  The size will 
 *                       doubled on each expansion.  If zero is provided, 
 *                       a default size will be used.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynPtrArrayInit
(OSCTXT* pctxt, OSRTDynPtrArray* pArray, OSUINT16 initialSize);

/**
 * This function adds a pointer to the end of the array and expands the 
 * array if necessary.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pArray       Pointer to dynamic pointer array structure.
 * @param ptr          Pointer to be added to the array.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynPtrArrayAppend 
(OSCTXT* pctxt, OSRTDynPtrArray* pArray, void* ptr);

#ifdef __cplusplus
}
#endif

#endif
