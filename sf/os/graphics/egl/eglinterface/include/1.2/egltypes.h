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

#if !defined(EGLAPI)
#   define EGLAPI IMPORT_C
#endif

#if !defined(EGL_APIENTRY)
#   define EGL_APIENTRY
#endif

/**
@publishedAll
@released
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

#define EGL_DEFAULT_DISPLAY 0
#define EGL_NO_CONTEXT 0
#define EGL_NO_DISPLAY 0
#define EGL_NO_SURFACE 0

#endif // __EGL_EGLTYPES_H__

