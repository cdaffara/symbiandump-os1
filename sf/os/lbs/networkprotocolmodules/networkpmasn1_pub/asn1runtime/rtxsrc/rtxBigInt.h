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
 * @file rtxBigInt.h 
 */


/**
@file
@internalTechnology
*/

#ifndef _RTXBIGINT_H_
#define _RTXBIGINT_H_

#include "rtxsrc/rtxContext.h"

/* Big Integer */

typedef struct OSBigInt {
   int       numocts;           /* number of used octets in mag         */
   OSOCTET   *mag;              /* magnitude                            */
   int       sign;              /* sign, can be -1, 0, 1                */

   int       allocated;         /* number of allocated octets for mag   */
   OSBOOL    dynamic;           /* is buffer dynamic?                   */
} OSBigInt;


#ifdef __cplusplus
extern "C" {
#endif

/* BigInt functions */

EXTERNRT void rtxBigIntInit (OSBigInt* pInt);
EXTERNRT int  rtxBigIntSetStr (OSCTXT* pCtxt, OSBigInt* pInt, 
                                const char* value, int radix);
EXTERNRT int  rtxBigIntSetInt64 (OSCTXT* pCtxt, OSBigInt* pInt, 
                                  OSINT64 value);
EXTERNRT int  rtxBigIntSetUInt64 (OSCTXT* pCtxt, OSBigInt* pInt, 
                                   OSUINT64 value);
EXTERNRT int  rtxBigIntSetBytes (OSCTXT* pCtxt, OSBigInt* pInt, 
                                  OSOCTET* value, int vallen);
EXTERNRT int  rtxBigIntGetDataLen (const OSBigInt* pInt);
EXTERNRT int  rtxBigIntGetData (OSCTXT* pCtxt, const OSBigInt* pInt, 
                                 OSOCTET* buffer, int bufSize);
EXTERNRT int  rtxBigIntDigitsNum (const OSBigInt* pInt, int radix);
EXTERNRT int  rtxBigIntCopy (OSCTXT* pCtxt, const OSBigInt* pSrc, OSBigInt* pDst);
EXTERNRT int  rtxBigIntFastCopy (OSCTXT* pCtxt, const OSBigInt* pSrc, OSBigInt* pDst);
EXTERNRT int  rtxBigIntToString (OSCTXT* pCtxt, const OSBigInt* pInt, int radix, 
                                  char* str, int strSize);
EXTERNRT int  rtxBigIntPrint (const OSUTF8CHAR* name, const OSBigInt* bigint, int radix);
EXTERNRT int  rtxBigIntCompare (const OSBigInt* arg1, const OSBigInt* arg2);
EXTERNRT int  rtxBigIntStrCompare (OSCTXT* pCtxt, const char* arg1, 
                                    const char* arg2);
EXTERNRT void rtxBigIntFree (OSCTXT* pCtxt, OSBigInt* pInt);

/* Arithmetic */
EXTERNRT int rtxBigIntAdd 
   (OSCTXT* pctxt, 
    OSBigInt* result, const OSBigInt* arg1, const OSBigInt* arg2);

EXTERNRT int rtxBigIntSubtract 
   (OSCTXT* pctxt, 
    OSBigInt* result, const OSBigInt* arg1, const OSBigInt* arg2);

EXTERNRT int rtxBigIntMultiply 
   (OSCTXT* pctxt, 
    OSBigInt* result, const OSBigInt* arg1, const OSBigInt* arg2);

/* Common */
EXTERNRT unsigned short rtxBigIntBitsPerDigit (int radix);
EXTERNRT short rtxBigIntDigitsPerByte (int halfRadix);
EXTERNRT short rtxBigIntByteRadix (int halfRadix);

#ifdef __cplusplus
}
#endif

#endif /* RTXBIGINT */

