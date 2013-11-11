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
 * @file rtxPrintStream.h 
 * Functions that allow printing diagnostic message to a stream using a 
 * callback function.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXPRINTSTREAM_H_
#define _RTXPRINTSTREAM_H_

#include <stdarg.h>
#include "rtxsrc/rtxContext.h"
#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @addtogroup ccfDiag
 * @{
 */
/**
 * Callback function definition for print stream.  
 */
typedef void (*rtxPrintCallback)
(void* pPrntStrmInfo, const char* fmtspec, va_list arglist);

/**
 * Structure to hold information about a global PrintStream.
 */
typedef struct OSRTPrintStream {
   rtxPrintCallback pfPrintFunc;
   void * pPrntStrmInfo;
} OSRTPrintStream;

#ifndef __SYMBIAN32__
/**
 * Global PrintStream
 */
extern OSRTPrintStream g_PrintStream;
#endif

/**
 * This function is for setting the callback function for a PrintStream. Once a
 * callback function is set, then all print and debug output ia sent to 
 * the defined callback function.
 *
 * @param pctxt      Pointer to a context in which callback print function 
 *                    will be set
 * @param myCallback Pointer to a callback print function.
 * @param pStrmInfo  Pointer to user defined PrintInfo structure where users
 *                    can store information required by the callback function
 *                    across calls. Ex. An open File handle for callbak 
 *                    function which directs stream to a file.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxSetPrintStream
(OSCTXT *pctxt, rtxPrintCallback myCallback, void* pStrmInfo);

#ifndef __SYMBIAN32__
/**
 * This function is for setting the callback function for a PrintStream. 
 * This version of the function sets a callback at the global level.
 *
 * @param myCallback Pointer to a callback print function.
 * @param pStrmInfo  Pointer to user defined PrintInfo structure where users
 *                    can store information required by the callback function
 *                    across calls. Ex. An open File handle for callbak 
 *                    function which directs stream to a file.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxSetGlobalPrintStream
(rtxPrintCallback myCallback, void* pStrmInfo);
#endif

/**
 * Print-to-stream function which in turn calls the user registered callback
 * function of the context for printing. If no callback function is registered
 * it prints to standard output by default.
 *
 * @param pctxt      Pointer to context to be used.
 * @param fmtspec    A printf-like format specification string describing the
 *                    message to be printed (for example, "string %s, ivalue
 *                    %d\n").
 * @param             ... A variable list of arguments.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */ 
EXTERNRT int rtxPrintToStream (OSCTXT* pctxt, const char* fmtspec, ...);

/**
 * Diagnostics print-to-stream function.  This is the same as the 
 * \c rtxPrintToStream function except that it checks if diagnostic tracing 
 * is enabled before invoking the callback function.
 * 
 * @param pctxt      Pointer to context to be used.
 * @param fmtspec    A printf-like format specification string describing the
 *                    message to be printed (for example, "string %s, ivalue
 *                    %d\n").
 * @param arglist    A variable list of arguments passed as va_list
 *
 * @return           Completion status, 0 on success, negative value on failure
 */ 
EXTERNRT int rtxDiagToStream
(OSCTXT *pctxt, const char* fmtspec, va_list arglist);

/**
 * This function releases the memory held by PrintStream in the context
 * 
 * @param pctxt      Pointer to a context for which the memory has to be
 *                    released.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxPrintStreamRelease (OSCTXT* pctxt);

#ifdef __cplusplus
}
#endif
/**
 * @} ccfDiag
 */
#endif /* _RTXPRINTSTREAM_H_ */

