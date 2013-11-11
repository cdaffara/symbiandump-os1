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
 * @file rtxHashMap.h
 * Generic hash map interface.  This relates a generic key structure (void*) 
 * to a generic value (void*).  Based on "C Hash Table" public domain code
 * (http://www.cl.cam.ac.uk/~cwc22/hashtable/).
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXHASHMAP_H_
#define _RTXHASHMAP_H_

#include "rtxsrc/rtxContext.h"

#ifndef HASHMAPTYPENAME
#define HASHMAPTYPENAME   OSRTHashMap
#define HASHMAPENTRYTYPE  OSRTHashMapEntry
#define HASHMAPKEYTYPE    void*
#define HASHMAPVALUETYPE  void*
#define HASHMAPINITFUNC   rtxHashMapInit
#define HASHMAPNEWFUNC    rtxNewHashMap
#define HASHMAPCOPYFUNC   rtxHashMapCopy
#define HASHMAPFREEFUNC   rtxHashMapFree
#define HASHMAPINSERTFUNC rtxHashMapInsert
#define HASHMAPPUTFUNC    rtxHashMapPut
#define HASHMAPREMOVEFUNC rtxHashMapRemove
#define HASHMAPSEARCHFUNC rtxHashMapSearch
#define HASHMAPSORTFUNC   rtxHashMapSort
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HASHMAPENTRYTYPE {
   HASHMAPKEYTYPE key;
   HASHMAPVALUETYPE value;
   OSUINT32 hashCode;
   struct HASHMAPENTRYTYPE* next;
} HASHMAPENTRYTYPE;

typedef struct {
   OSUINT32 tableLength;
   HASHMAPENTRYTYPE** table;
   OSUINT32 entryCount;
   OSUINT32 loadLimit;
   OSUINT32 primeIndex;
   OSUINT32 (*hashFunc)(HASHMAPKEYTYPE key);
   OSBOOL (*equalsFunc)(HASHMAPKEYTYPE key1, HASHMAPKEYTYPE key2);
} HASHMAPTYPENAME;

/**
 * This function initializes the hash map.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure.
 * @param capacity      Capacity of the hash map or zero to use default.
 * @param hashFunc      Hash callback function.
 * @param keyEqualsFunc Key equals callback function.
 */
EXTERNRT void HASHMAPINITFUNC
(OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap, size_t capacity, 
 OSUINT32 (*hashFunc)(HASHMAPKEYTYPE), 
 OSBOOL (*keyEqualsFunc)(HASHMAPKEYTYPE,HASHMAPKEYTYPE));

/**
 * This function creates a new hash map.
 *
 * @param pctxt         Pointer to a context structure.
 * @param capacity      Capacity of the map or zero to use default.
 * @param hashFunc      Hash callback function.
 * @param keyEqualsFunc Key equals callback function.
 * @return              Allocated hash map structure or NULL if insufficient 
 *                        dynamic memory is available to hold the structure.
 */
EXTERNRT HASHMAPTYPENAME* HASHMAPNEWFUNC 
(OSCTXT* pctxt, size_t capacity, 
 OSUINT32 (*hashFunc)(HASHMAPKEYTYPE), 
 OSBOOL (*keyEqualsFunc)(HASHMAPKEYTYPE,HASHMAPKEYTYPE));

/**
 * This function creates a copy of an existing hash map.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure to copy.
 * @return              Allocated and copied hash map structure or NULL 
 *                        if insufficient dynamic memory is available to 
 *                        hold the structure.
 */
EXTERNRT HASHMAPTYPENAME* HASHMAPCOPYFUNC
(OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap);

/**
 * This function frees all entries within an existing hash map.  It does 
 * not free the structure itself.  
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure to free.
 */
EXTERNRT void HASHMAPFREEFUNC (OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap);

/**
 * This function inserts an entry into the hash map.  The table will be 
 * expanded if the insertion would take the ratio of entries to table size 
 * over the maximum load factor.
 *
 * This function does not check for repeated insertions with a duplicate key.
 * The value returned when using a duplicate key is undefined -- when
 * the hashtable changes size, the order of retrieval of duplicate key
 * entries is reversed.  If in doubt, remove before insert.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure.
 * @param key           Key value. Memory is owned by caller. 
 * @param value         Value to insert. Memory is owned by caller.
 * @return              Zero if insertion was successful, a negative 
 *                        status code otherwise.
 */
EXTERNRT int HASHMAPINSERTFUNC 
(OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap, HASHMAPKEYTYPE key, 
 HASHMAPVALUETYPE value);

/**
 * This function searches for an entry in the hash map.
 *
 * @param pHashMap      Pointer to hash map structure.
 * @param key           Key value. Memory is owned by caller. 
 * @param pvalue        Pointer to value to receive search result value.
 * @return              Boolean search result: true if found; false if not.
 */
EXTERNRT OSBOOL HASHMAPSEARCHFUNC
(HASHMAPTYPENAME* pHashMap, HASHMAPKEYTYPE key, HASHMAPVALUETYPE* pvalue);

/**
 * This function removes an entry from the hash map.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure.
 * @param key           Key value. Memory is owned by caller. 
 * @param pvalue        Pointer to value to receive search result value.
 * @return              Boolean result: true if found and removed.
 */
EXTERNRT OSBOOL HASHMAPREMOVEFUNC
(OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap, HASHMAPKEYTYPE key, 
 HASHMAPVALUETYPE* pvalue);

/**
 * This function inserts/replaces an entry into the hash map.  If the key
 * already exists in the map, its value is updated.  Otherwise, the 
 * key/value pair is inserted.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure.
 * @param key           Key value. Memory is owned by caller. 
 * @param value         Value to insert/replace. Memory is owned by caller.
 * @return              Zero if operation was successful, a negative 
 *                        status code otherwise.
 */
EXTERNRT int HASHMAPPUTFUNC 
(OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap, HASHMAPKEYTYPE key, 
 HASHMAPVALUETYPE value);

/**
 * This function sorts the hash map in ascending order using the 
 * given key compare function.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pHashMap      Pointer to hash map structure.
 * @param compareFunc   Comparison function for key values.
 * @param pSortedList   Pointer to linked list structure to receive sorted 
 *                        values.  Entries within the list are items 
 *                        from the hash map themselves, not copies. 
 *                        List memory may be freed by calling 
 *                        rtxDListFreeNodes.
 * @return              Status of operation: 0 = success or negative 
 *                        status code.
 *                        
 */
EXTERNRT int HASHMAPSORTFUNC 
(OSCTXT* pctxt, HASHMAPTYPENAME* pHashMap, OSRTDList* pSortedList, 
 int (*compareFunc)(HASHMAPKEYTYPE key1, HASHMAPKEYTYPE key2));

#ifdef __cplusplus
}
#endif

#endif
