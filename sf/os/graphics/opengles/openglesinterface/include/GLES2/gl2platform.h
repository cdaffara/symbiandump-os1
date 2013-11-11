#ifndef __gl2platform_h_
#define __gl2platform_h_

/*
 * Portions Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Differences from the actual sample implementation provided by Khronos:
 * 1. This comment text.
 * 2. Doxygen comment with tag 'publishedPartner', and tag 'released'
 * 3. Modified GL_APICALL.  The symbian specific check has been moved to the
 *    top because with a clean epoc/build/* starting point, makmake.pl wincsw
 *    would pick up the Win32 defines section.  For vendor implementations,
 *    use in your .MMP file
 *    MACRO SYMBIAN_OGLES_DLL_EXPORTS.
 *    The GCC32 case has been removed because this does not fit with the symbian
 *    build system which uses GCCE instead.
 * 4. A vendor implementation is allowed to overwrite/modify this file;
 *    see Khronos API Implementers' Guidelines, Revision 0.91.0.
 */

/**
@publishedAll
@released
*/

/* $Id: gl2platform.h 4532 2007-11-26 11:12:44Z markc $ */

#ifdef __cplusplus
extern "C" {
#endif

/*
** SGI FREE SOFTWARE LICENSE B (Version 2.0, Sept. 18, 2008)
** Copyright (C) 2008 Silicon Graphics, Inc. All Rights Reserved.
** 
** Permission is hereby granted, free of charge, to any person obtaining 
** a copy of this software and associated documentation files 
** (the "Software"), to deal in the Software without restriction, 
** including without limitation the rights to use, copy, modify, merge, 
** publish, distribute, sublicense, and/or sell copies of the Software, 
** and to permit persons to whom the Software is furnished to do so, 
** subject to the following conditions:
**  
** The above copyright notice including the dates of first 
** publication and either this permission notice or a reference to 
** http://oss.sgi.com/projects/FreeB/ shall be included in all copies 
** or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
** MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
** IN NO EVENT SHALL SILICON GRAPHICS, INC. BE LIABLE FOR ANY CLAIM, 
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
** Except as contained in this notice, the name of Silicon Graphics, Inc. 
** shall not be used in advertising or otherwise to promote the sale, use 
** or other dealings in this Software without prior written authorization 
** from Silicon Graphics, Inc. 
*/

/*
 * If the pre-included toolchain header does not specify the __SOFTFP macro,
 * this include defines it.
 */
#include <e32def.h>

/*-------------------------------------------------------------------------
 * Definition of GL_APICALL and GL_APIENTRY
 *-----------------------------------------------------------------------*/

#if defined (__SYMBIAN32__)
#   if defined (SYMBIAN_OGLES_DLL_EXPORTS)
#       define GL_APICALL __declspec(dllexport)
#   else
#       define GL_APICALL __declspec(dllimport)
#   endif
#elif defined(_WIN32) || defined(__VC32__)       /* Win32 */
#   if defined (_DLL_EXPORTS)
#       define GL_APICALL __declspec(dllexport)
#   else
#       define GL_APICALL __declspec(dllimport)
#   endif
#elif defined (__ARMCC_VERSION)                  /* ADS */
#   define GL_APICALL
#elif defined (__GNUC__)                         /* GCC dependencies (kludge) */
#   define GL_APICALL
#endif

#if !defined (GL_APICALL)
#   error Unsupported platform!
#endif

#define GL_APIENTRY

#ifdef __cplusplus
}
#endif

#endif /* __gl2platform_h_ */
