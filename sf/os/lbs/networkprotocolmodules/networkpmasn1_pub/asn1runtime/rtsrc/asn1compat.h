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
 * @file asn1compat.h 
 * This header file contains deprecated definitions of ASN.1 runtime 
 * constants, data structures, and other defintions.  It is included 
 * with generated code when the '-compat' command-line switch is used.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1COMPAT_H_
#define _ASN1COMPAT_H_

#include "rtxsrc/osSysTypes.h"

#define ASN_OK          0

/* ASN.1 Primitive Type Definitions */

typedef char            ASN1CHAR;
typedef OSUINT8         ASN1OCTET;
typedef OSBOOL          ASN1BOOL;
typedef OSINT8          ASN1INT8;
typedef OSUINT8         ASN1UINT8;
typedef OSINT16         ASN1SINT;
typedef OSUINT16        ASN1USINT;
typedef OSINT32         ASN1INT;
typedef OSUINT32        ASN1UINT;
typedef OSINT32         ASN1ENUM;
typedef OSREAL          ASN1REAL;

typedef OSUNICHAR       ASN116BITCHAR;
typedef OS32BITCHAR     ASN132BITCHAR;

#define ASN1UINT_MAX    OSUINT32_MAX
#define ASN1INT_MAX     ((OSINT32)OSINT32_MAX)
#define ASN1INT_MIN     ((OSINT32)OSINT32_MIN)

/* 64-bit long integer type */

#ifndef ASN1INT64
#define ASN1INT64       OSINT64
#define ASN1UINT64      OSUINT64
#define ASN1INT64FMT    OSINT64FMT
#define ASN1UINT64FMT   OSUINT64FMT
#define ASN1INT64FMTX   OSINT64FMTX
#define ASN1INT64MAX    OSINT64MAX
#define ASN1UINT64MAX   OSUINT64MAX
#define ASN1INT64MIN    OSINT64MIN
#define ASN1I64CONST(val)  OSI64CONST(val)
#define ASN1UI64CONST(val) OSUI64CONST(val)
#endif

typedef const char* ASN1ConstCharPtr;
typedef const ASN1OCTET* ASN1ConstOctetPtr;
typedef const void* ASN1ConstVoidPtr;
typedef const ASN116BITCHAR* ASN1Const16BitCharPtr;
#define ASN1Const const

#define ASN1CTXT OSCTXT

#define rtInitContext rtAsn1InitContext
#define rtFreeContext rtAsn1FreeContext

#define xu_error rtxErrSetData
#define xu_addErrParm rtxErrAddStrParm
#define xu_addIntErrParm rtxErrAddIntParm
#define xu_addUnsignedErrParm rtxErrAddUIntParm
#define xu_log_error rtxErrLogUsingCB
#define xu_fmtErrMsg rtxErrGetText
#define xu_freeErrParms rtxErrFreeParms
#define xu_perror rtxErrPrint

#define xu_malloc rtxMemAlloc
#define xu_freeall rtxMemFree
#define ALLOC_ASN1ELEM rtxMemAllocType

#endif
