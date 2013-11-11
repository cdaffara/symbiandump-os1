// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GLES/EGLTYPES.H
// 
//

#ifndef __GLES_EGLTYPES_H_
#define __GLES_EGLTYPES_H_


/**
@publishedAll
@released


__OPENGLESHEADERS_LEGACY_EGL_1_1
    
The purpose of this define is to allow the staged migration to
EGL 1.3 from EGL 1.1 taken from the perspective of OpenGL ES 1.X

If __OPENGLESHEADERS_LEGACY_EGL_1_1 is defined, then GLES/egl.h and
GLES/egltypes.h will only provide the EGL 1.1 API; in particular no
symbols from a later version of EGL will be seen at a source code level.
This is considered to be legacy behaviour.  If
__OPENGLESHEADERS_LEGACY_EGL_1_1 is not defined, the GLES/egl.h
and GLES/egltypes.h header files will re-direct to the system-wide
EGL/egl.h. and EGL/egltypes.h respectively.

The use of GLES/egl.h or GLES/egltypes.h by applications is considered
legacy behaviour because applications should instead use EGL/egl.h
or EGL/egltypes.h respectively.

When this legacy behaviour is desired, the define should be placed in an
OEM-specific system-wide .hrh file.  Symbian product configurations
never set this legacy behaviour.
*/
#ifdef __OPENGLESHEADERS_LEGACY_EGL_1_1
#include <GLES/legacy_egl_1_1/egltypes.h>
#else
#include <EGL/egltypes.h>
#endif

#endif /* __GLES_EGLTYPES_H_ */

