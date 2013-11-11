// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#include "t_pseudoappopenglanimation.h"

/** Vertex co-ordinates for a cube */
const GLfloat KVertexArray[] = {	-10.0f,  10.0f,  10.0f,
									 10.0f,  10.0f,  10.0f,
									-10.0f, -10.0f,  10.0f,
									 10.0f, -10.0f,  10.0f,
									-10.0f,  10.0f, -10.0f,
									 10.0f,  10.0f, -10.0f,
									-10.0f, -10.0f, -10.0f,
									 10.0f, -10.0f, -10.0f };

/** Left, near, top co-ordinate index */
const GLubyte LNT = 0;
/** Right, near, top co-ordinate index */
const GLubyte RNT = 1;
/** Left, far, top co-ordinate index */
const GLubyte LFT = 2;
/** Right, far, top co-ordinate index */
const GLubyte RFT = 3;
/** Left, near, bottom co-ordinate index */
const GLubyte LNB = 4;
/** Right, near, bottom co-ordinate index */
const GLubyte RNB = 5;
/** Left, far, bottom co-ordinate index */
const GLubyte LFB = 6;
/** Right, far, bottom co-ordinate index */
const GLubyte RFB = 7;
/** Number of indicies per face */
const TInt KNumFaceIndices = 6;

/** Array of indicies into vertex array to define a cube */
const GLubyte KIndexArray[] = {	RNB, LNB, LFB, RNB, LFB, RFB,
								RNB, RFB, RFT, RNB, RFT, RNT,
								LNB, RNB, RNT, LNB, RNT, LNT,
								LFB, LNB, LNT, LFB, LNT, LFT,
								RFB, LFB, LFT, RFB, LFT, RFT,
								LNT, RNT, RFT, LNT, RFT, LFT };


COpenGLAnimation* COpenGLAnimation::NewL(RWindow* aWin, const TDisplayMode& aMode, const TSize& aSurfaceSize,
																		  const TInt aHorizontalRate,
																		  const TInt aVerticalRate)
	{
	RDebug::Print(_L("Creating COpenGLAnimation class\n"));
    COpenGLAnimation* self = new (ELeave) COpenGLAnimation(aMode,aSurfaceSize,aHorizontalRate,aVerticalRate);
    CleanupStack::PushL(self);
    self->ConstructL(aWin);
    CleanupStack::Pop(); // self;
    return self;
	}

COpenGLAnimation::COpenGLAnimation(const TDisplayMode& aMode, const TSize& aSurfaceSize, const TInt aHorizontalRate, const TInt aVerticalRate)
: CEglBase(aMode,aSurfaceSize,aHorizontalRate,aVerticalRate)
	{
	}

void COpenGLAnimation::ConstructL(RWindow* aWin)
	{
	BaseConstructL(aWin);

	// eglBindAPI
	TInt ret = eglBindAPI(EGL_OPENGL_ES_API);
	TestL(ret, _L("eglBindAPI failed"));

	__ASSERT_DEBUG(iUseConfig>0, User::Panic(_L("COpenGLAnimation"), KErrArgument));

	// eglCreateContext
	iContext = eglCreateContext(iDisplay, iUseConfig, EGL_NO_CONTEXT, NULL);
	TestL(iContext!=EGL_NO_CONTEXT, _L("eglCreateContext failed"));

	// Make surface and context current on the display object
	ret = eglMakeCurrent(iDisplay, iSurface, iSurface, iContext);
	TestL(ret, _L("eglMakeCurrent failed"));

	CheckGlErrorL(_L("GL_Problems creating second surface"));
	}

COpenGLAnimation::~COpenGLAnimation()
	{
	}

/** Draws a cube using OpenGLES commands */
void COpenGLAnimation::DrawToEglL()
	{
	// Increment rotation variable
	iRotAngle += 15;

	// Set half-transparent drawing value
	const GLfloat KAlphaValue = 0.5f;

	// Remove hidden faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Use a flat shading model
	glShadeModel(GL_FLAT);

	// Set the projection parameters
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustumf(-0.154f, 0.154f, -0.0577f, 0.0577f, 0.1f, 100.0f);

	// Set the cube position
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Move position away from viewpoitn
	glTranslatef(iX+=iHorizontalRate, 0.0f, -50.0f);

	// Rotate 45 degrees about the z-axis
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
	// Rotate iX degrees about the y- and z-axes
	glRotatef(iRotAngle, 0.0f, 1.0f, 1.0f);

	// Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable vertex processing
	glEnableClientState(GL_VERTEX_ARRAY);

	// Set the vertex array to use
	glVertexPointer(3, GL_FLOAT, 0, KVertexArray);

	const GLubyte* faceIndex = KIndexArray;

	// Process cube faces
	for (TInt face = 1; face <= 6; face++)
		{
		const GLfloat red = (face < 2 || face > 4) ? 1.0f : 0;
		const GLfloat green = (face > 0 && face < 4) ? 1.0f : 0;
		const GLfloat blue = (face > 2) ? 1.0f : 0;
		// Set colour to draw in
		glColor4f(red, green, blue, KAlphaValue);
		// Draw the vertices that make up each face as two triangles
		glDrawElements(GL_TRIANGLES, KNumFaceIndices, GL_UNSIGNED_BYTE, faceIndex);
		faceIndex += KNumFaceIndices;
		}

	// Flush colour buffer to the window surface
	eglSwapBuffers(iDisplay, iSurface);
	}

void COpenGLAnimation::DrawL(CTestSurfaceRasterizer*  /*aSurfaceRasterizer*/, SurfaceDetails& /*aSurfDetails*/)
	{
	DrawToEglL();
	}

void COpenGLAnimation::CheckGlErrorL(const TDesC& aMessage)
	{
	GLenum err=glGetError();
	if ( err != GL_NO_ERROR )
		{
		RDebug::Print(_L("Error:0x%x - %S"),err,&aMessage);
		User::Leave(KErrUnknown);
		}
	}
