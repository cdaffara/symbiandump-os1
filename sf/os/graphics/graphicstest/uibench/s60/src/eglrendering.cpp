 // Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "eglrendering.h"
#include "engine.h"
#include "openvgengine.h"

#include <hal.h>
#include <string.h>


_LIT(KErrEgl, "EGL error");
_LIT(KErrOpenVg, "OpenVG error");
_LIT(KErrEglReturn, "EGL return error");


/** Attributes to be passed into eglChooseConfig */
const EGLint	KColorRGB565AttribList[] =
		{
		EGL_RED_SIZE,			5,
		EGL_GREEN_SIZE,			6,
		EGL_BLUE_SIZE,			5,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, 	EGL_OPENVG_BIT,
		EGL_NONE
		};


CEGLRendering* CEGLRendering::NewL(RWindow& aWindow)
	{
	CEGLRendering* self = CEGLRendering::NewLC(aWindow);
	CleanupStack::Pop(self);
	return self;
	}

CEGLRendering* CEGLRendering::NewLC(RWindow& aWindow)
	{
	CEGLRendering* self = new(ELeave) CEGLRendering(aWindow);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CEGLRendering::CEGLRendering(RWindow& aWindow) : iWindow(aWindow),iCount(0)
    {
    // empty
    }

void CEGLRendering::ConstructL()
    {
    const TDisplayMode dispMode = iWindow.DisplayMode();
    const TSize windowSize(iWindow.Size());

    // Create display object
    iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLCheckError();

    // Initialize display object
    EGLCheckReturnError(eglInitialize(iDisplay, NULL, NULL));
    
    // Check that EGL provides the capabilities for this app.
    if (NULL == strstr(eglQueryString(iDisplay, EGL_CLIENT_APIS), "OpenVG")) 
        {
        RDebug::Printf("OpenVG not listed in supported client APIs %s", eglQueryString(iDisplay, EGL_CLIENT_APIS));
        User::Leave(KErrNotSupported);
        }
    if (NULL == strstr(eglQueryString(iDisplay, EGL_EXTENSIONS), "EGL_SYMBIAN_COMPOSITION")) 
        {
        RDebug::Printf("EGL_SYMBIAN_COMPOSITION not listed in extension string %s", eglQueryString(iDisplay, EGL_EXTENSIONS));
        User::Leave(KErrNotSupported);
        }
    
    EGLint numConfigs;
    EGLConfig chosenConfig = 0;

    // Choose the config to use
    EGLCheckReturnError(eglChooseConfig(iDisplay, KColorRGB565AttribList, &chosenConfig, 1, &numConfigs));
    if (numConfigs == 0)
        {
        RDebug::Printf("No matching configs found", eglQueryString(iDisplay, EGL_EXTENSIONS));
        User::Leave(KErrNotSupported);
        }
    
    // Create window surface to draw direct to.
    EGLCheckReturnError(eglBindAPI(EGL_OPENVG_API));
    iSurface = eglCreateWindowSurface(iDisplay, chosenConfig, &iWindow, NULL);
    EGLCheckError();

    TInt redSize, greenSize, blueSize, alphaSize;
    EGLCheckReturnError(eglGetConfigAttrib(iDisplay, chosenConfig, EGL_ALPHA_SIZE, &alphaSize));
    EGLCheckReturnError(eglGetConfigAttrib(iDisplay, chosenConfig, EGL_RED_SIZE, &redSize));
    EGLCheckReturnError(eglGetConfigAttrib(iDisplay, chosenConfig, EGL_GREEN_SIZE, &greenSize));
    EGLCheckReturnError(eglGetConfigAttrib(iDisplay, chosenConfig, EGL_BLUE_SIZE, &blueSize));

    // Create context to store surface settings
    iContextVG = eglCreateContext(iDisplay, chosenConfig, EGL_NO_CONTEXT, NULL);
    EGLCheckError();
    
    iCurrentDemo = COpenVGEngine::NewL(iWindow,iDisplay,iSurface,iContextVG);
    iCurrentDemo->ActivateL();
    }

CEGLRendering::~CEGLRendering()
	{
	delete iCurrentDemo;
	CEGLRendering::EGLCheckReturnError(eglMakeCurrent(iDisplay, iSurface, iSurface, iContextVG));
	if (iContextVG != EGL_NO_CONTEXT)
		{
		EGLCheckReturnError(eglDestroyContext(iDisplay,iContextVG));
		}
	if (iSurface != EGL_NO_SURFACE)
		{
		EGLCheckReturnError(eglDestroySurface(iDisplay,iSurface));
		}
	// Call eglMakeCurrent() to ensure the surfaces and contexts are truly destroyed. 
	EGLCheckReturnError(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
	//EGLCheckReturnError(eglTerminate(iDisplay));
	eglReleaseThread();
	}

void CEGLRendering::EGLCheckError()
	{
	EGLint error = eglGetError();
	if(error != EGL_SUCCESS)
		{
		User::Panic(KErrEgl, error);
		}
	}

void CEGLRendering::VGCheckError()
	{
	VGint error = vgGetError();
	if(error != VG_NO_ERROR)
		{
		User::Panic(KErrOpenVg, error);
		}
	}

void CEGLRendering::EGLCheckReturnError(EGLBoolean aBool)
	{
	if (!aBool)
		{
		User::Panic(KErrEglReturn, eglGetError());
 		}
	}

/**
 * Update the display.
 */
void CEGLRendering::UpdateDisplay()
	{
	// Guard against unexpected re-entrant problem. Use this flag until the issue is resolved.
	if (iBusySwapping)
		{
		return;
		}

	if (iCurrentDemo->IsPending() || (!iCurrentDemo->IsPending() && iShowMirrorToggled))
		{	
		// needed to make sure the correct status is active
		CEGLRendering::EGLCheckReturnError(eglMakeCurrent(iDisplay, iSurface, iSurface, iContextVG));
		iShowMirrorToggled = EFalse;
		iCurrentDemo->Step();
		iBusySwapping = ETrue;
		CEGLRendering::EGLCheckReturnError(eglSwapBuffers(iDisplay, iSurface));
		iBusySwapping = EFalse;
		}	
	}
