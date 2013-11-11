/*
 * regexp.h : describes the basic API for libxml regular expressions handling
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */
/*
NOTE: the copyright notice below applies only to source and header files 
that include this notice.  It does not apply to other Objective Systems 
software with different attached notices.

Except where otherwise noted in the source code (e.g. the files hash.c,
list.c and the trio files, which are covered by a similar licence but
with different Copyright notices) all the files are:

 Copyright (C) 1998-2003 Daniel Veillard.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is fur-
nished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
DANIEL VEILLARD BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON-
NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Daniel Veillard shall not
be used in advertising or otherwise to promote the sale, use or other deal-
ings in this Software without prior written authorization from him.
*/
/**
 * @file rtxRegExp.h
 */
/**
 * Changes made to original libxml source file (xmlregexp.h) by 
 * Objective Systems, Inc are as follows:
 *
 * 1. Changed to fit Objective Systems run-time environment including 
 * common type name changes and use of OSys mem mgmt.
 *
 * 2. Name change from xmlregexp to rtxRegExp to reflect fact that the 
 * code will be used in other non-XML environments.
 */

/**
@file
@internalTechnology
*/

#ifndef __RTXREGEXP_H__
#define __RTXREGEXP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * rtxRegexpPtr:
 *
 * A libxml regular expression, they can actually be far more complex
 * than POSIX regex expressions.
 */
typedef struct _rtxRegexp rtxRegexp;
typedef rtxRegexp *rtxRegexpPtr;

/**
 * rtxRegExecCtxtPtr:
 *
 * A libxml progressive regular expression evaluation context
 */
typedef struct _rtxRegExecCtxt rtxRegExecCtxt;
typedef rtxRegExecCtxt *rtxRegExecCtxtPtr;

#ifdef __cplusplus
}
#endif 
#include <stdio.h>
#include "rtxsrc/rtxContext.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
 * The POSIX like API
 */
EXTERNRT rtxRegexpPtr rtxRegexpCompile 
(OSCTXT* pOSCTXT, const OSUTF8CHAR* regexp);

EXTERNRT void rtxRegFreeRegexp (OSCTXT* pOSCTXT, rtxRegexpPtr regexp);

EXTERNRT int rtxRegexpExec 
(OSCTXT* pOSCTXT, rtxRegexpPtr comp, const OSUTF8CHAR *value);

EXTERNRT void rtxRegexpPrint (FILE *output, rtxRegexpPtr regexp);

EXTERNRT int rtxRegexpIsDeterminist (OSCTXT* pOSCTXT, rtxRegexpPtr comp);

EXTERNRT int
rtxRegexpFill(OSCTXT* pOSCTXT, rtxRegexpPtr regexp, const OSUTF8CHAR** ppDest);

/*
 * Callback function when doing a transition in the automata
 */
typedef void (*rtxRegExecCallbacks) (rtxRegExecCtxtPtr exec,
	                             const OSUTF8CHAR *token,
				     void *transdata,
				     void *inputdata);

/*
 * The progressive API
 */
rtxRegExecCtxtPtr	rtxRegNewExecCtxt	(OSCTXT* pOSCTXT, 
                                                 rtxRegexpPtr comp,
						 rtxRegExecCallbacks callback,
						 void *data);
void			rtxRegFreeExecCtxt	(rtxRegExecCtxtPtr exec);
int			rtxRegExecPushString	(rtxRegExecCtxtPtr exec,
						 const OSUTF8CHAR *value,
						 void *data);

#ifdef __cplusplus
}
#endif 

#endif /*__RTXREGEXP_H__ */
