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
 * @file rtxArrayList.h
 * ArrayList functions.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXARRAYLIST_H_
#define _RTXARRAYLIST_H_

#include "rtxsrc/rtxContext.h"

#define OSRT_ARRAYLIST_CAPACITY 10

typedef struct _OSRTArrayList {
   OSRTDList segments;
   size_t    segmentSize; 
   size_t    size;
   size_t    dataSize;
   OSBOOL    (*equalsFunc)(void*,void*,size_t);
} OSRTArrayList;

typedef struct _OSRTArrayListIter {
   OSRTArrayList* pArrayList;
   OSRTDListNode* pSegment;
   OSUINT32       index;
} OSRTArrayListIter;

/* Iteration is done as follows:
 * rtxArrayListInitIter (&iter, &arrayList, 0);
 * while (rtxArrayListHasNextItem (&iter)) {
 *    pdata = rtxArrayListNextItem (&iter);
 *    ...
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function initializes an array list structure.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param capacity      Initial capacity of the array or zero to use default.
 */
EXTERNRT void rtxArrayListInit 
(OSRTArrayList* pArrayList, size_t capacity);

/**
 * This function creates a new array list to hold the initial capacity of 
 * elements. 
 *
 * @param pctxt         Pointer to a context structure.
 * @param capacity      Initial capacity of the array or zero to use default.
 * @return              Allocated array list structure or NULL if insufficient 
 *                        dynamic memory is available to hold the structure.
 */
EXTERNRT OSRTArrayList* rtxNewArrayList (OSCTXT* pctxt, size_t capacity);

/**
 * This function frees all dynamic memory held by the array list.  It does
 * not free the array list structure itself, just the internal data array.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure.
 */
EXTERNRT void rtxFreeArrayList (OSCTXT* pctxt, OSRTArrayList* pArrayList);

/**
 * This function adds an element to an array list. 
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to add.
 * @param pindex        Pointer to index variable to receive index at 
 *                        which entry was added.
 * @return              Zero if item was successfully added; a negative 
 *                        status code if error.
 */
EXTERNRT int rtxArrayListAdd 
(OSCTXT* pctxt, OSRTArrayList* pArrayList, void* pdata, OSUINT32* pindex);

/**
 * This function removes an element from an array list. 
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to remove.
 */
EXTERNRT void rtxArrayListRemove 
(OSCTXT* pctxt, OSRTArrayList* pArrayList, void* pdata);

/**
 * This function removes the element at the given index from the array list. 
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param index         Index of item to remove.  -1 indicates tail item
 *                        should be removed.
 */
EXTERNRT void rtxArrayListRemoveIndexed 
(OSCTXT* pctxt, OSRTArrayList* pArrayList, int index);

/**
 * This function inserts an element at the given position in the array list. 
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to insert.
 * @param index         Index of location where item should be inserted.
 * @return              Zero if item was successfully added; a negative 
 *                        status code if error.
 */
EXTERNRT int rtxArrayListInsert 
(OSCTXT* pctxt, OSRTArrayList* pArrayList, void* pdata, OSUINT32 index);

/**
 * This function replaces (overwrites) the element at the given position in 
 * the array list with the new element.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to insert.
 * @param index         Index of location where item should be inserted.
 * @return              Zero if item was successfully added; a negative 
 *                        status code if error.
 */
EXTERNRT int rtxArrayListReplace 
(OSRTArrayList* pArrayList, void* pdata, OSUINT32 index);

/**
 * This function gets the indexed data item from the array list.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param index         Index of location where item should be inserted.
 * @return              Pointer to indexed data item or NULL if index
 *                        is greater than max index in list.
 */
EXTERNRT void* rtxArrayListGetIndexed 
(const OSRTArrayList* pArrayList, OSUINT32 index);

/**
 * This function returns the index of the given data item in the list. 
 * The 'equals' callback function is used to do comparisons.
 * 
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to find in list.
 * @return              Index of item in list or -1 if not found.
 */
EXTERNRT int rtxArrayListIndexOf (OSRTArrayList* pArrayList, void* pdata);

/* Iteration */

/**
 * This function initializes an array list iterator with the given start
 * index.
 *
 * @param piter         Pointer to array list iterator structure.
 * @param pArrayList    Pointer to array list structure.
 * @param startIndex    Index from which iteration is to start.
 * @return              Zero if successfully initialized or RTERR_OUTOFBND
 *                        if start index is beyond the current size of 
 *                        the array list.
 */
EXTERNRT int rtxArrayListInitIter 
(OSRTArrayListIter* piter, OSRTArrayList* pArrayList, OSUINT32 startIndex);

/**
 * This function determines if another element exists at the next 
 * sequential position in the array list.
 *
 * @param piter         Pointer to array list iterator structure.
 * @return              True if another element exists; false otherwise.
 */
EXTERNRT OSBOOL rtxArrayListHasNextItem (OSRTArrayListIter* piter);

/**
 * This function gets the next item from the array list.
 *
 * @param piter         Pointer to array list iterator structure.
 * @return              Pointer to next item or null if beyond the 
 *                        end of the array.
 */
EXTERNRT void* rtxArrayListNextItem (OSRTArrayListIter* piter);

#ifdef __cplusplus
}
#endif

#endif
