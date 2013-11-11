/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

/*
 * owfdebug.h
 *
 */

#ifndef OWFDEBUG_H_
#define OWFDEBUG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Enable platform specific library cleanup */
int xatexit(void (*func)(void));

#ifdef _DEBUG

#undef DEBUG
#define DEBUG

#ifndef DEBUG_DONOT_LOG
#undef DEBUG_LOG
#define DEBUG_LOG
#endif

#endif

#ifdef DEBUG
#include <assert.h>
#define OWF_ASSERT(c)    assert(c);
#else
#define OWF_ASSERT(c)    ;
#endif

#ifdef DEBUG_LOG

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "owftypes.h"

/* Defining DEBUG_LOG enables all log file output. Do this in the command-line, make or .MMP file
 * The file output can be filtered by filename or by function name in the same way:
 *   Defining DEBUG_LOG_FUNCTION=fnname will filter for all functions STARTING with the name fnname
 *   Defining DEBUG_LOG_FILE=filename will filter for all functions ending with the name filename
 * These filters utilise the predefined macros __FILE__ and __FUNCTION__ which are defined in current standards,
 * but may contain platform-specific features such as path names, or linkage decoration,
 * so the filter-string may require some platform-specific tuning.
 * If the macros are not predefined in a useful way, or not defined at all, then you won't be able to filter, sorry.
 */
#if defined(DEBUG_LOG_FILE)
#define   DPRINT(x)         if (OWF_Debug_DoLog(__FILE__,sizeof(__FILE__)-1)) OWF_Debug_Print x
#elif defined(DEBUG_LOG_FUNCTION)
#define   DPRINT(x)         if (OWF_Debug_DoLog(__FUNCTION__,sizeof(__FUNCTION__)-1)) OWF_Debug_Print x
#else
#define DPRINT(x)           OWF_Debug_Print x
#endif

#define NFPRINT(x)       DPRINT(x)
#define ENTER(x)         DPRINT(("%s:", #x))
#define LEAVE(x)
#define TRACE(x)         OWF_Debug_Trace x
#define INDENT           OWF_Debug_TraceIndent()
#define UNDENT           OWF_Debug_TraceUndent()

int  OWF_Debug_DoLog(const char* symbol,int symlen);
void OWF_Debug_Print(const char* format, ...);
void OWF_Debug_Trace(const char* fmt, ...);
void OWF_Debug_TraceIndent();
void OWF_Debug_TraceUndent();

#else /* NOT DEBUG */

#define DPRINT(x)     /* do nothing */
#define NFPRINT(x)
#define ENTER(x)
#define LEAVE(x)
#define TRACE(x)
#define INDENT
#define UNDENT

#endif /* DEBUG */


#ifdef __cplusplus
}
#endif


#endif /* OWFDEBUG_H_ */
