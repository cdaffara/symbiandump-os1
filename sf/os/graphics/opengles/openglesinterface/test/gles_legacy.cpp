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
// Test case code for checking legacy behaviour of graphics_openglesheaders
// The purpose of this source code is to throw an error during compilation
// if the contents of the #included header files has any symbol which is not
// part of the EGL 1.1 API specification even though later versions of EGL add new
// symbols to the namespace in a backwards-compatible way.
// This file must be compiled with the legacy setting
//

// #define __OPENGLESHEADERS_LEGACY_EGL_1_1
// Such a setting would normally be made in an OEM-specific system-wide
// included .hrh file.
//

/**
 @file
 @internalTechnology
 @released
*/
#include <e32base.h>

/*
 * Include the GLES/egl.h which we want to behave as a legacy EGL 1.1 API.
 * Note that this file is for legacy reasons is located under the GLES directory.
 * Note also, we don't include GLES/gl.h as we expect egl.h in EGL 1.1 to do this.
 */
#include <GLES/egl.h>

#ifndef __OPENGLESHEADERS_LEGACY_EGL_1_1
#error "openglesheaders testcase configuration error: macro __OPENGLESHEADERS_LEGACY_EGL_1_1 expected"
#endif

#ifndef GL_TRUE
#error "openglesheaders testcase failure: GLES/egl.h legacy behaviour requires egl.h to include gl.h"
#endif

#ifndef EGL_DEFAULT_DISPLAY
#error "openglesheaders testcase failure: GLES/egl.h legacy behaviour requires egl.h to include egltypes.h"
#endif

#ifndef EGL_VERSION_1_1
#error "openglesheaders testcase failure: GLES/egl.h legacy behaviour requires EGL 1.1 API to be seen"
#endif

#ifdef EGL_VERSION_1_2
#error "openglesheaders testcase failure: GLES/egl.h legacy behaviour requires no EGL API symbols beyond EGL API 1.1 to be seen"
#endif

/*
 * Make sure that the legacy egltypes.h file does exist in the GLES subdirectory.
 */
#include <GLES/egltypes.h>

#ifndef __LEGACY_EGL_1_1_EGLTYPES_H__
#error "openglesheaders testcase failure: GLES/egltypes.h should re-direct to the legacy egltypes.h, not the system-wide EGL/egltypes.h"
#endif

GLDEF_C TInt E32Main()
    {
    return 0;
    }
	
// Getting this far without a compilation error indicates success.
