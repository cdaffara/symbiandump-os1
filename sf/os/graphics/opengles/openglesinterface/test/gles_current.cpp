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
// Test case code for checking current behaviour of graphics_openglesheaders
// The purpose of this source code is to thrown an error during compilation
// if the GLES/egl.h header does not re-direct to the system-wide EGL/egl.h
// The use of GLES/egl.h is a legacy, and kept to ease the transition of
// applications to using EGL/egl.h instead.
// This code must be compiled without the legacy setting
// __OPENGLESHEADERS_LEGACY_EGL_1_1 to ensure that GLES/egl.h re-directs
// to the system-wide EGL/egl.h
// 
//

/**
 @file
 @internalTechnology
 @released
*/
#include <e32base.h>

/*
 * Include the GLES/egl.h which we want to behave as the system-wide EGL API.
 * Note that this file is for legacy reasons is located under the GLES directory.
 * Note also, we don't include GLES/gl.h as we expect GLES egl.h in to do this.
 */
#include <GLES/egl.h>

#ifdef __OPENGLESHEADERS_LEGACY_EGL_1_1
/*
 * If the above macro has been set unexpectedly, then it has most probably
 * arisen because Symbian_OS.hrh defined it.  This is an error because Symbian
 * never defines this macro in a Symbian product configuration.
 */
#error "openglesheaders testcase configuration error: macro __OPENGLESHEADERS_LEGACY_EGL_1_1 should not be defined"
#endif

#ifndef GL_TRUE
#error "openglesheaders testcase failure: GLES/egl.h behaviour requires egl.h to include gl.h"
#endif

#ifndef EGL_DEFAULT_DISPLAY
#error "openglesheaders testcase failure: GLES/egl.h behaviour requires egl.h to include egltypes.h"
#endif

#ifndef EGL_VERSION_1_1
#error "openglesheaders testcase failure: GLES/egl.h behaviour requires EGL 1.1 API to be seen"
#endif

#ifndef EGL_VERSION_1_2
#error "openglesheaders testcase failure: GLES/egl.h behaviour requires symbols from all EGL versions to be seen"
#endif

/*
 * Make sure that the current egltypes.h file does exist in the GLES subdirectory.
 */
#include <GLES/egltypes.h>

#ifdef __LEGACY_EGL_1_1_EGLTYPES_H__
#error "openglesheaders testcase failure: GLES/egltypes.h should re-direct to the system-wide egltypes.h, not the legacy GLES egltypes.h"
#endif

GLDEF_C TInt E32Main()
    {
    return 0;
    }
	
// Getting this far without a compilation error indicates success.
