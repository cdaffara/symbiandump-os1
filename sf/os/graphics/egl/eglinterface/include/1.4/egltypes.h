// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// EGL/EGLTYPES.H
// 
//

#ifndef __EGL_EGLTYPES_H__
#define __EGL_EGLTYPES_H__

#include <e32def.h>


#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif

#ifndef EGLAPIENTRYP
#define EGLAPIENTRYP EGLAPIENTRY*
#endif

#ifndef EGL_APIENTRY
#define EGL_APIENTRY EGLAPIENTRY
#endif

#ifndef EGLAPI
#define EGLAPI IMPORT_C
#endif


/**
@publishedAll
@released
*/

/*
** egltypes.h is platform dependent. It defines:
**
**     - EGL types and resources
**     - Native types
**     - EGL and native handle values
**
** EGL types and resources are to be typedef'ed with appropriate platform
** dependent resource handle types. EGLint must be an integer of at least
** 32-bit.
**
** NativeDisplayType, NativeWindowType and NativePixmapType are to be
** replaced with corresponding types of the native window system in egl.h.
**
** EGL and native handle values must match their types.
**
*/


typedef int EGLBoolean;
typedef int EGLint;
typedef int EGLenum;

typedef int EGLDisplay;
typedef int EGLConfig;
typedef int EGLSurface;
typedef int EGLContext;
typedef int EGLClientBuffer;

typedef int    NativeDisplayType;
typedef void*  NativeWindowType;
typedef void*  NativePixmapType;

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeWindowType EGLNativeWindowType;

#define EGL_DEFAULT_DISPLAY 0
#define EGL_NO_CONTEXT 0
#define EGL_NO_DISPLAY 0
#define EGL_NO_SURFACE 0

#endif // __EGL_EGLTYPES_H__
