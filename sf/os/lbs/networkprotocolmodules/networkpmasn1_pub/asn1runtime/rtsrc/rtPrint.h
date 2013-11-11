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
 * @file rtPrint.h  Functions to print ASN.1 data in various formats.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTPRINT_H_
#define _RTPRINT_H_
#include <stdio.h>
#include "rtsrc/asn1type.h"
#include "rtxsrc/rtxPrint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup valsToStdout Print Values to Standard Output 
 * @ingroup cruntime 
 * @{
 * These functions print the output in a "name=value" format. The
 * value format is obtained by calling one of the ToString functions with the
 * given value. 
 */
#define rtPrintBoolean rtxPrintBoolean
#define rtPrintInteger rtxPrintInteger
#define rtPrintInt64   rtxPrintInt64
#define rtPrintUnsigned rtxPrintUnsigned
#define rtPrintUInt64  rtxPrintUInt64
#define rtPrintReal    rtxPrintReal
#define rtPrintCharStr rtxPrintCharStr
#define rtPrintHexStr  rtxPrintHexStr

/** 
 * This function prints the value of a bit string to stdout.
 *
 * @param name         The name of the variable to print.
 * @param numbits      The number of bitsto be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 */
EXTERNRT void rtPrintBitStr (const char* name, OSUINT32 numbits, 
                              const OSOCTET* data, const char* conn);

/** 
 * This function prints the value of a bit string to stdout in brace text 
 * format.
 *
 * @param name         The name of the variable to print.
 * @param numbits      The number of bits to be printed.
 * @param data         A pointer to the data to be printed.
 */
EXTERNRT void rtPrintBitStrBraceText 
(const char* name, OSUINT32 numbits, const OSOCTET* data);

/** 
 * This function prints the value of an octet string to stdout.
 *
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 */
EXTERNRT void rtPrintOctStr (const char* name, OSUINT32 numocts, 
                              const OSOCTET* data, const char* conn);

/** 
 * This function prints the value of a 16-bit character string to stdout.
 *
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 */
EXTERNRT void rtPrint16BitCharStr (const char* name, 
                                    Asn116BitCharString* bstring);


/** 
 * This function prints the value of a 32-bit character string to stdout.
 *
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 */
EXTERNRT void rtPrint32BitCharStr (const char* name, 
                                    Asn132BitCharString* bstring,
                                    const char* conn);

/**
 * This function prints a Universal string to standard output.  Characters 
 * in the string that are within the normal Ascii range are printed as 
 * single characters.  Characters outside the Ascii range are printed 
 * as 8-byte hex codes (0xnnnnnnnn).
 *
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 */
EXTERNRT void rtPrintUnivCharStr (const char* name, 
                                   Asn132BitCharString* bstring);

/** 
 * This function prints the value of an object identifier to stdout.
 *
 * @param name         The name of the variable to print.
 * @param pOID         A pointer to the OID to be printed.
 */
EXTERNRT void rtPrintOID (const char* name, ASN1OBJID* pOID);

/** 
 * This function prints the value of an object identifier 
 * to stdout.  Only the value is printed, not the name.
 *
 * @param pOID         A pointer to the OID to be printed.
 */
EXTERNRT void rtPrintOIDValue (ASN1OBJID* pOID);

/** 
 * This function prints the value of an object identifier with 64-bit arc
 * values to stdout.
 *
 * @param name         The name of the variable to print.
 * @param pOID         A pointer to the OID to be printed.
 */
EXTERNRT void rtPrintOID64 (const char* name, ASN1OID64* pOID); 

/** 
 * This function prints the value of an object identifier with 64-bit arc
 * values to stdout.  Only the value is printed, not the name.
 *
 * @param pOID         A pointer to the OID to be printed.
 */
EXTERNRT void rtPrintOID64Value (ASN1OID64* pOID);

/** 
 * This function prints the value of an open type to stdout.
 *
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         The pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 */
EXTERNRT void rtPrintOpenType (const char* name, OSUINT32 numocts, 
                                const OSOCTET* data, const char* conn);

/**
 * This function prints the value of an open type extension to stdout.
 *
 * @param name         The name of the variable to print.
 * @param pElemList    A pointer to an element of a list.
 */
EXTERNRT void rtPrintOpenTypeExt (const char* name, OSRTDList* pElemList);

/**
 * This function prints the value of an open type extension in brace 
 * text format to stdout.
 *
 * @param name         The name of the variable to print.
 * @param pElemList    A pointer to an element of a list.
 */
EXTERNRT void rtPrintOpenTypeExtBraceText 
(const char* name, OSRTDList* pElemList);

#define rtPrintIndent rtxPrintIndent
#define rtPrintIncrIndent rtxPrintIncrIndent
#define rtPrintDecrIndent rtxPrintDecrIndent
#define rtPrintCloseBrace rtxPrintCloseBrace
#define rtPrintOpenBrace  rtxPrintOpenBrace

/**
 * @} valsToStdout
 */
#ifdef __cplusplus
}
#endif
#endif 
