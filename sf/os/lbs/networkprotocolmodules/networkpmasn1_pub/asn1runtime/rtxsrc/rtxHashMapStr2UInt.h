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
 * @file rtxHashMapStr2UInt.h
 * String-to-unsigned integer hash map interface.  This relates a string 
 * key structure (const OSUTF8CHAR*) to a 32-bit unsigned integer value 
 * (OSUINT32).  It uses the rtxHashMap .h/.c file as a template.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXHASHMAPSTR2INT_H_
#define _RTXHASHMAPSTR2INT_H_

#define HASHMAPTYPENAME  OSRTHashMapStr2UInt
#define HASHMAPENTRYTYPE OSRTHashMapStr2UIntEntry
#define HASHMAPKEYTYPE   const OSUTF8CHAR*
#define HASHMAPVALUETYPE OSUINT32
#define HASHMAPINITFUNC  rtxHashMapStr2UIntInit
#define HASHMAPNEWFUNC   rtxNewHashMapStr2UInt
#define HASHMAPCOPYFUNC  rtxHashMapStr2UIntCopy
#define HASHMAPFREEFUNC  rtxHashMapStr2UIntFree
#define HASHMAPINSERTFUNC rtxHashMapStr2UIntInsert
#define HASHMAPPUTFUNC   rtxHashMapStr2UIntPut
#define HASHMAPSEARCHFUNC rtxHashMapStr2UIntSearch
#define HASHMAPREMOVEFUNC rtxHashMapStr2UIntRemove
#define HASHMAPSORTFUNC  rtxHashMapStr2UIntSort

#include "rtxsrc/rtxHashMap.h"

#endif
