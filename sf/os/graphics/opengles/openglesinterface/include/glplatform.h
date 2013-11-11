#ifndef __glplatform_h_
#define __glplatform_h_

/**
Portions Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
@publishedAll
@released
*/

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
 * Definition of GL_API and GL_APIENTRY
 *-----------------------------------------------------------------------*/

#if defined(AEE_SIMULATOR)
#define __GL_EXPORTS
#endif

#if defined(_WIN32) && !defined(__WINS__)
#   ifdef __GL_EXPORTS
#       define GL_API __declspec(dllexport)
#   else
#       define GL_API __declspec(dllimport)
#   endif
#else
#   if (__ARMCC_VERSION >= 220000)
#   	ifdef __GL_EXPORTS
#	    	define GL_API __declspec(dllexport)
#   	else
#	    	define GL_API __declspec(dllimport)
#   	endif
#   else
#   	ifdef __GL_EXPORTS
#           define GL_API
#   	else
#           define GL_API extern
#		endif
#   endif
#endif

#define GL_APIENTRY 

#ifndef APIENTRY
#   define APIENTRY GL_APIENTRY
#endif
 
#ifndef GLAPI
#	define GLAPI GL_API
#endif

#ifdef __cplusplus
}
#endif

#endif /* __glplatform_h_ */
