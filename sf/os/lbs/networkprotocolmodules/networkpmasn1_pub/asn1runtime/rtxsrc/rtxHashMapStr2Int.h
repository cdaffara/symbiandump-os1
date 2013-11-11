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
 * @file rtxHashMapStr2Int.h
 * String-to-integer hash map interface.  This relates a STRING key structure 
 * (const OSUTF8CHAR*) to a 32-bit signed integer value (OSINT32).  It uses 
 * the rtxHashMap .h/.c file as a template.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXHASHMAPSTR2INT_H_
#define _RTXHASHMAPSTR2INT_H_

#define HASHMAPTYPENAME  OSRTHashMapStr2Int
#define HASHMAPENTRYTYPE OSRTHashMapStr2IntEntry
#define HASHMAPKEYTYPE   const OSUTF8CHAR*
#define HASHMAPVALUETYPE OSINT32
#define HASHMAPINITFUNC  rtxHashMapStr2IntInit
#define HASHMAPNEWFUNC   rtxNewHashMapStr2Int
#define HASHMAPCOPYFUNC  rtxHashMapStr2IntCopy
#define HASHMAPFREEFUNC  rtxHashMapStr2IntFree
#define HASHMAPINSERTFUNC rtxHashMapStr2IntInsert
#define HASHMAPPUTFUNC   rtxHashMapStr2IntPut
#define HASHMAPSEARCHFUNC rtxHashMapStr2IntSearch
#define HASHMAPREMOVEFUNC rtxHashMapStr2IntRemove
#define HASHMAPSORTFUNC  rtxHashMapStr2IntSort

#include "rtxsrc/rtxHashMap.h"

#endif
