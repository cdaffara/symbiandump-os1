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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "tsmallwindowopenvg.h"


// todo: is it the right config
const EGLint KColorRGB565AttribList[] =
    {
    EGL_RED_SIZE,			5,
    EGL_GREEN_SIZE,			6,
    EGL_BLUE_SIZE,			5,
    EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, 	EGL_OPENVG_BIT,
    EGL_NONE
    };


CTWindow* CTSmallWindowOpenVG::NewL(RWsSession &aWs, 
        const RWindowTreeNode &aParent, 
        const TPoint& aStartingPoint,
        const TSize& aWindowSize)
    {
    CTSmallWindowOpenVG* self = new (ELeave) CTSmallWindowOpenVG(aStartingPoint, aWindowSize);
    CleanupStack::PushL(self);
    self->ConstructL(aWs, aParent);
    CleanupStack::Pop(self);
    return self;
    }

CTSmallWindowOpenVG::CTSmallWindowOpenVG(const TPoint& aStartingPoint, const TSize& aWindowSize):
        CTWindow(aStartingPoint, aWindowSize)
	{
	// empty
	}

CTSmallWindowOpenVG::~CTSmallWindowOpenVG()
	{
	// Make sure that this egl status is active
	eglMakeCurrent(iDisplay, iSurface, iSurface, iContextVG);
    vgDestroyPaint(iFillPaint);
    vgDestroyPaint(iStrokePaint);
    vgDestroyPath(iPath);
	if (iContextVG != EGL_NO_CONTEXT)
		{
		eglDestroyContext(iDisplay,iContextVG);
		}
	if (iSurface != EGL_NO_SURFACE)
		{
		eglDestroySurface(iDisplay,iSurface);
		}	
	// Call eglMakeCurrent() to ensure the surfaces and contexts are truly destroyed. 
	eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	//eglTerminate(iDisplay);
	eglReleaseThread();
	}



void CTSmallWindowOpenVG::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
	{
	CTWindow::ConstructL(aWs, aParent);
	
	iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(iDisplay, NULL, NULL);
	
	EGLint numConfigs;
	EGLConfig chosenConfig = 0;

	// Choose the config to use
	eglChooseConfig(iDisplay, KColorRGB565AttribList, &chosenConfig, 1, &numConfigs);
	if (numConfigs == 0)
		{
		RDebug::Printf("No matching configs found", eglQueryString(iDisplay, EGL_EXTENSIONS));
		User::Leave(KErrNotSupported);
		}
	
	// Create window surface to draw direct to.
	eglBindAPI(EGL_OPENVG_API);
	iSurface = eglCreateWindowSurface(iDisplay, chosenConfig, &iWindow, NULL);

	// Create context to store surface settings
	iContextVG = eglCreateContext(iDisplay, chosenConfig, EGL_NO_CONTEXT, NULL);
	
	eglMakeCurrent(iDisplay, iSurface, iSurface, iContextVG);	
	
    VGfloat strokeColor[4]  = {1.f, 0.f, 0.f, 1.f};
    VGfloat fillColor[4]    = {0.f, 0.f, 1.f, 1.f};

    VGubyte pathSegments[6] =
        {
        VG_LINE_TO | (int)VG_ABSOLUTE,
        VG_LINE_TO | (int)VG_ABSOLUTE,
        VG_LINE_TO | (int)VG_ABSOLUTE,
        VG_LINE_TO | (int)VG_ABSOLUTE,
        VG_CLOSE_PATH
        };

    VGfloat pathData[10] =
        {
          0.f, 100.f,
        100.f, 100.f,
        100.f,   0.f,
          0.f,   0.f
        };

    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);

    iCurrentRotation = 0.f;
    iStrokePaint = vgCreatePaint();
    iFillPaint   = vgCreatePaint();

    vgSetParameteri(iStrokePaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv(iStrokePaint, VG_PAINT_COLOR, 4, strokeColor);

    vgSetParameteri(iFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv(iFillPaint, VG_PAINT_COLOR, 4, fillColor);

    iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 4, 4, (unsigned int)VG_PATH_CAPABILITY_ALL);

    vgAppendPathData(iPath, 4, pathSegments, pathData);
	}

void CTSmallWindowOpenVG::RenderL()
	{
	CTWindow::RenderL();

	// Make sure that this egl status is active
	eglMakeCurrent(iDisplay, iSurface, iSurface, iContextVG);

    VGfloat clearColor[4] = {0.1f, 0.2f, 0.4f, 1.f};
    VGfloat scaleFactor = Size().iWidth/200.f;
    if (Size().iHeight/200.f < scaleFactor)
        {
        scaleFactor = Size().iHeight/200.f;
        }        

    iCurrentRotation = iTime;

    if (iCurrentRotation >= 360.f)
        {
        iCurrentRotation -= 360.f;
        }

    vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
    vgClear(0, 0, Size().iWidth, Size().iHeight);

    vgLoadIdentity();
    vgTranslate((float)Size().iHeight / 2, (float)Size().iHeight / 2);
    vgScale(scaleFactor, scaleFactor);
    vgRotate(iCurrentRotation);
    vgTranslate(-50.f, -50.f);

    vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
    vgSeti(VG_FILL_RULE, VG_EVEN_ODD);

    vgSetPaint(iFillPaint, VG_FILL_PATH);

    vgSetf(VG_STROKE_LINE_WIDTH, 10.f);
    vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
    vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);
    vgSetf(VG_STROKE_MITER_LIMIT, 0.f);
    vgSetPaint(iStrokePaint, VG_STROKE_PATH);

    vgDrawPath(iPath, VG_FILL_PATH | VG_STROKE_PATH);

	iTime++;
	eglSwapBuffers(iDisplay, iSurface);
	}

