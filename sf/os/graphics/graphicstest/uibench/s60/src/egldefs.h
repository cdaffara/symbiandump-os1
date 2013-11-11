// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef EGLDEFS_H_
#define EGLDEFS_H_

#include <e32base.h>
#include <GLES/egl.h>

/** Attributes for an EColor4K window. */
const GLint	KColor4KAttribList[] =
		{
		EGL_BUFFER_SIZE,	12,
		EGL_RED_SIZE,		4,
		EGL_GREEN_SIZE,		4,
		EGL_BLUE_SIZE,		4,
		EGL_ALPHA_SIZE,		0,
		EGL_DEPTH_SIZE,		16,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,EGL_OPENGL_ES_BIT,
		EGL_NONE
		};

/** Attributes for an EColor64K window. */
const GLint	KColor64KAttribList[] =
		{
		EGL_BUFFER_SIZE,	16,
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE,		6,
		EGL_BLUE_SIZE,		5,
		EGL_ALPHA_SIZE,		0,
		EGL_DEPTH_SIZE,		16,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,EGL_OPENGL_ES_BIT,
		EGL_NONE
		};

/** Attributes for an EColor16M window. */
const GLint	KColor16MAttribList[] =
		{
		EGL_BUFFER_SIZE,	24,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE,		8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE,		0,
		EGL_DEPTH_SIZE,		16,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,EGL_OPENGL_ES_BIT,
		EGL_NONE
		};

/** Attributes for an EColor16MA window. */
const GLint KColor16MAAttribList[] =
        {
        EGL_BUFFER_SIZE,    32,
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_DEPTH_SIZE,     16,
        EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,EGL_OPENGL_ES_BIT,
        EGL_NONE
        };

/** 
 *  Values for the perspective matrix used by glFrustumf.
 *  Width is calculated based on the surface aspect ratio.
 */
const GLfloat KFrustumHeight = 1.0f;
const GLfloat KFrustumNear = 1.0f;
const GLfloat KFrustumFar = 1000000.0f;

/** Values to translate the model view. */
const GLfloat KModelViewX = 0.0f;
const GLfloat KModelViewY = 0.0f;
const GLfloat KModelViewZ = -40.0f;

#endif /* EGLDEFS_H_ */
