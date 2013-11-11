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
 * @file rtxDiag.h 
 * Common runtime functions for diagnostic tracing and debugging.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXDIAG_H_
#define _RTXDIAG_H_

#include <stdarg.h>
#include "rtxsrc/rtxContext.h"
#include "rtxsrc/rtxPrintToStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Diagnostic trace functions */
/**
 * @defgroup ccfDiag Diagnostic trace functions 
 * @{
 *
 * These functions add diagnostic tracing to the generated code to assist in
 * finding where a problem might occur. Calls to these macros and functions are
 * added when the \c -trace command-line argument is used. The diagnostic
 * message can be turned on and off at both C compile and run-time. To C
 * compile the diagnostics in, the _TRACE macro must be defined (-D_TRACE). To
 * turn the diagnostics on at run-time, the \c rtxSetDiag function must be
 * invoked with the \c value argument set to TRUE.
 */
#ifdef _TRACE
#define RTDIAG1(pctxt,msg)              rtxDiagPrint(pctxt,msg)
#define RTDIAG2(pctxt,msg,a)            rtxDiagPrint(pctxt,msg,a)
#define RTDIAG3(pctxt,msg,a,b)          rtxDiagPrint(pctxt,msg,a,b)
#define RTDIAG4(pctxt,msg,a,b,c)        rtxDiagPrint(pctxt,msg,a,b,c)
#define RTDIAG5(pctxt,msg,a,b,c,d)      rtxDiagPrint(pctxt,msg,a,b,c,d)
#define RTDIAGU(pctxt,ucstr)            rtxDiagPrintUCS(pctxt,ucstr)
#define RTHEXDUMP(pctxt,buffer,numocts) rtxDiagHexDump(pctxt,buffer,numocts)
#define RTDIAGCHARS(pctxt,buf,nchars)   rtxDiagPrintChars(pctxt,buf,nchars)
#define RTDIAGSTRM2(pctxt,msg)          rtxDiagStream(pctxt,msg)
#define RTDIAGSTRM3(pctxt,msg,a)        rtxDiagStream(pctxt,msg,a)
#define RTDIAGSTRM4(pctxt,msg,a,b)      rtxDiagStream(pctxt,msg,a,b)
#define RTDIAGSTRM5(pctxt,msg,a,b,c)    rtxDiagStream(pctxt,msg,a,b,c)
#define RTHEXDUMPSTRM(pctxt,buffer,numocts) \
rtxDiagStreamHexDump(pctxt,buffer,numocts)
#define RTDIAGSCHARS(pctxt,buf,nchars) \
rtxDiagStreamPrintChars(pctxt,buf,nchars)
#else
#define RTDIAG1(pctxt,msg)
#define RTDIAG2(pctxt,msg,a)
#define RTDIAG3(pctxt,msg,a,b)
#define RTDIAG4(pctxt,msg,a,b,c)
#define RTDIAG5(pctxt,msg,a,b,c,d)
#define RTDIAGU(pctxt,ucstr)
#define RTHEXDUMP(pctxt,buffer,numocts)
#define RTDIAGCHARS(pctxt,buf,nchars)
#define RTDIAGSTRM2(pctxt,msg)
#define RTDIAGSTRM3(pctxt,msg,a)
#define RTDIAGSTRM4(pctxt,msg,a,b)
#define RTDIAGSTRM5(pctxt,msg,a,b,c)
#define RTHEXDUMPSTRM(pctxt,buffer,numocts)
#define RTDIAGSCHARS(pctxt,buf,nchars)
#endif

typedef enum {
   OSRTDiagError, 
   OSRTDiagWarning, 
   OSRTDiagInfo,
   OSRTDiagDebug
} OSRTDiagTraceLevel;

/**
 * This function is used to determine if diagnostic tracing is currently
 * enabled for the specified context. It returns true if enabled, false
 * otherwise.
 *
 * @param pctxt        Pointer to context structure.
 * @return             Boolean result.
 */
EXTERNRT OSBOOL rtxDiagEnabled (OSCTXT* pctxt);

/**
 * This function is used to turn diagnostic tracing on or off at run-time on 
 * a per-context basis. Code generated using ASN1C or XBinder or a similar 
 * code generator must use the -trace command line option to enable 
 * diagnostic messages.  The generated code must then be C compiled with 
 * _TRACE defined for the code to be present.
 *
 * @param pctxt        Pointer to context structure.
 * @param value        Boolean switch: TRUE turns tracing on, FALSE off.
 * @return             Prior setting of the diagnostic trace switch in the
 *                       context.
 */
EXTERNRT OSBOOL rtxSetDiag (OSCTXT* pctxt, OSBOOL value);

#ifndef __SYMBIAN32__
/**
 * This function is used to turn diagnostic tracing on or off at run-time on 
 * a global basis. It is similar to rtxSetDiag except tracing is enabled 
 * within all contexts.
 *
 * @param value        Boolean switch: TRUE turns tracing on, FALSE off.
 * @return             Prior setting of the diagnostic trace switch in the
 *                       context.
 */
EXTERNRT OSBOOL rtxSetGlobalDiag (OSBOOL value);
#endif

/**
 * This function is used to print a diagnostics message to \c stdout. Its
 * parameter specification is similar to that of the C runtime \c printf
 * method. The \c fmtspec argument may contain % style modifiers into which
 * variable arguments are substituted. This function is called in the generated
 * code vai the RTDIAG macros to allow diagnostic trace call to easily be
 * compiled out of the object code.
 *
 * @param pctxt        Pointer to context structure.
 * @param fmtspec      A printf-like format specification string describing the
 *                       message to be printed (for example, "string %s, ivalue
 *                       %d\n").  A special character sequence (~L) may be 
 *                       used at the beginning of the string to select a 
 *                       trace level (L would be replaced with E for Error, 
 *                       W for warning, I for info, or D for debug).
 * @param             ... Variable list of parameters to be substituted into
 *                       the format string.
 */
EXTERNRT void rtxDiagPrint (OSCTXT* pctxt, const char* fmtspec, ...);

/**
 * This function conditionally outputs diagnostic trace messages to an output 
 * stream defined within the context. A code generator embeds calls to this 
 * function into the generated source code when the -trace option is specified
 * on the command line (note: it may embed the macro version of these calls - 
 * RTDIAGSTREAMx - so that these calls can be compiled out of the final code.
 *
 * @see rtxDiagPrint
 */
EXTERNRT void rtxDiagStream (OSCTXT* pctxt, const char* fmtspec, ...);

/**
 * This function is used to print a diagnostics hex dump of a section of
 * memory.
 *
 * @param pctxt        Pointer to context structure.
 * @param data         Start address of memory to dump.
 * @param numocts      Number of bytes to dump.
 */
EXTERNRT void rtxDiagHexDump 
(OSCTXT* pctxt, const OSOCTET* data, OSUINT32 numocts);

/**
 * This function is used to print a diagnostics hex dump of a section of
 * memory to a print stream.
 *
 * @param pctxt        Pointer to context structure.
 * @param data         Start address of memory to dump.
 * @param numocts      Number of bytes to dump.
 */
EXTERNRT void rtxDiagStreamHexDump 
(OSCTXT* pctxt, const OSOCTET* data, OSUINT32 numocts);

/**
 * This function is used to print a given number of characters to 
 * standard output.  The buffer containing the characters does not 
 * need to be null-terminated.
 *
 * @param pctxt        Pointer to context structure.
 * @param data         Start address of character string.
 * @param nchars       Number of characters to dump 
 *                       (this assumes 1-byte chars).
 */
EXTERNRT void rtxDiagPrintChars
(OSCTXT* pctxt, const char* data, OSUINT32 nchars);

/**
 * This function is used to print a given number of characters to 
 * a print stream.  The buffer containing the characters does not 
 * need to be null-terminated.
 *
 * @param pctxt        Pointer to context structure.
 * @param data         Start address of character string.
 * @param nchars       Number of characters to dump 
 *                       (this assumes 1-byte chars).
 */
EXTERNRT void rtxDiagStreamPrintChars
(OSCTXT* pctxt, const char* data, OSUINT32 nchars);

/**
 * This function is used to set the maximum trace level for diagnostic 
 * trace messages.  Values are ERROR, WARNING, INFO, or DEBUG.  The 
 * special string start sequence (~L) described in rtxDiagPrint 
 * function documentation is used to set a message level to be compared 
 * with the trace level.
 *
 * @param pctxt        Pointer to context structure.
 * @param level        Trace level to be set.
 */
EXTERNRT void rtxDiagSetTraceLevel (OSCTXT* pctxt, OSRTDiagTraceLevel level);

/**
 * @} ccfDiag
 */

#ifdef __cplusplus
}
#endif

#endif
