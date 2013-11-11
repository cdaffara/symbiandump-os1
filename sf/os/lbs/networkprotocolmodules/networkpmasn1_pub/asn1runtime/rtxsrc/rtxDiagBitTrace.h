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
 * @file rtxDiagBitTrace.h 
 * Common runtime functions for tracing bit patterns written to or read 
 * from a stream.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXDIAGBITTRACE_H_
#define _RTXDIAGBITTRACE_H_

#include <stdarg.h>
#include "rtxsrc/rtxMemBuf.h"
#include "rtxsrc/rtxSList.h"
#include "rtxsrc/rtxPrintToStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ccfDiag
 */
typedef struct {
   const char*  elemName;
   const char*  nameSuffix;
   size_t       bitOffset;
   size_t       numbits;
} OSRTDiagBitField;

typedef struct {
   OSUINT16  disabledCount;
   OSRTSList fieldList; /* list of OSRTDiagBitField */
   OSRTMEMBUF* pCaptureBuf; /* capture buffer for streams */
} OSRTDiagBitFieldList;

/* Macros */

#ifdef _TRACE
#define RTDIAG_INSBITFLDLEN(pctxt)        rtxDiagInsBitFieldLen(pctxt)
#define RTDIAG_NEWBITFIELD(pctxt,suffix)  rtxDiagNewBitField(pctxt,suffix)
#define RTDIAG_SETBITFLDOFFSET(pctxt)     rtxDiagSetBitFldOffset(pctxt)
#define RTDIAG_SETBITFLDCOUNT(pctxt)      rtxDiagSetBitFldCount(pctxt)
#else
#define RTDIAG_INSBITFLDLEN(pctxt)
#define RTDIAG_NEWBITFIELD(pctxt,suffix)
#define RTDIAG_SETBITFLDOFFSET(pctxt)
#define RTDIAG_SETBITFLDCOUNT(pctxt)
#endif

/** 
 * This macro calculates the relative bit offset to the current 
 * buffer position.
 */
#define RTDIAG_GETCTXTBITOFFSET(pctxt) \
(((pctxt)->buffer.byteIndex * 8) + (8 - (pctxt)->buffer.bitOffset))

/**
 * This function enables bit field tracing.
 *
 * @param pctxt       Pointer to a context structure.
 * @param value       Boolean value to turn trace on or off.
 * @return            Previous state of bit trace enabled setting.
 */
EXTERNRT OSBOOL rtxDiagSetBitTraceEnabled (OSCTXT* pctxt, OSBOOL value);

/**
 * This function check to see if bit field tracing is enabled.
 *
 * @param pctxt       Pointer to a context structure.
 * @return            True if enabled; false otherwise.
 */
EXTERNRT OSBOOL rtxDiagBitTraceEnabled (OSCTXT* pctxt);

/**
 * This function initializes a bit field list structure.
 *
 * @param pctxt       Pointer to a context structure.
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagBitFieldListInit
(OSCTXT* pctxt, OSRTDiagBitFieldList* pBFList);

/**
 * This function inserts a special length field before the current record
 * in the bit field list.
 *
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagInsBitFieldLen (OSRTDiagBitFieldList* pBFList);

/**
 * This function allocates a new bit field structure and adds it to 
 * the bit field list.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param nameSuffix  Suffix to append to the bit field name.
 * @return            Allocated bit field structure.
 */
EXTERNRT OSRTDiagBitField* rtxDiagNewBitField 
(OSRTDiagBitFieldList* pBFList, const char* nameSuffix);

/**
 * This function is used to set the bit offset in the current bit field
 * structure.
 *
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagSetBitFldOffset (OSRTDiagBitFieldList* pBFList);

/**
 * This function is used to set the bit count in the current bit field
 * structure.
 *
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagSetBitFldCount (OSRTDiagBitFieldList* pBFList);

/**
 * This function is used to set the name suffix in the current bit field
 * structure.  This text is printed after the element name when the 
 * field is displayed.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param nameSuffix  Suffix to append to the bit field name.
 */
EXTERNRT void rtxDiagSetBitFldNameSuffix 
(OSRTDiagBitFieldList* pBFList, const char* nameSuffix);

/**
 * This function increments or decrements the disabled count.  This 
 * allows the list to be temporaily disabled to allow collection of 
 * more bits to form larger, aggregate fields.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param value       Indicates if disabled count should be incremented
 *                      (TRUE) or decremented (FALSE).
 * @return            TRUE if field operations are still disabled.
 */
EXTERNRT OSBOOL rtxDiagSetBitFldDisabled 
(OSRTDiagBitFieldList* pBFList, OSBOOL value);

/**
 * This function prints the bit field list to a an output stream.  By 
 * default, the output goes to stdout; but this can be changed by 
 * creating a print output stream within the context (see rtxPrintStream).
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param varname     A variable name that is prepended to each field.
 */
EXTERNRT void rtxDiagBitTracePrint 
(OSRTDiagBitFieldList* pBFList, const char* varname);

/**
 * This function prints the bit field list to a an HTML document.
 *
 * @param filename    Name of HTML file to be written.
 * @param pBFList     Pointer to bit field list structure.
 * @param varname     A variable name that is prepended to each field.
 */
EXTERNRT void rtxDiagBitTracePrintHTML 
(const char* filename, OSRTDiagBitFieldList* pBFList, const char* varname);

/**
 * This function appends the given name part to the element name 
 * in the bit field.  A dot (.) separator character is added 
 * after the existing name and before the name part.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param namePart    A name part that is appended to the field.
 */
EXTERNRT void rtxDiagBitFldAppendNamePart
(OSRTDiagBitFieldList* pBFList, const char* namePart);

#ifdef __cplusplus
}
#endif

#endif
