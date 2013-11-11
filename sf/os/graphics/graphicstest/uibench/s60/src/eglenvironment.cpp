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


#include "eglenvironment.h"
#include "egldefs.h"
#include "eglerror.h"

#include <gdi.h>


_LIT(KErrEgl, "EGL error");
//_LIT(KErrDisplayMode, "Unsupported display mode");


/** 
 * Report any error that occurs.
 */
void ProcessEglError()
	{
	User::Panic(KErrEgl, eglGetError());
	}

CEglEnvironment* CEglEnvironment::NewL(RWindow& aWindow)
	{
	CEglEnvironment* self = new(ELeave) CEglEnvironment(aWindow);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CEglEnvironment::CEglEnvironment(RWindow& aWindow) : iWindow(aWindow)
	{
	// empty
	}

CEglEnvironment::~CEglEnvironment()
	{
	// Make sure that this egl status is active
    eglMakeCurrent(iDisplay, iSurface, iSurface, iContext);
    if (iContext != EGL_NO_CONTEXT)
        {
        eglDestroyContext(iDisplay,iContext);
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

void CEglEnvironment::ConstructL()
	{
	// Create display object
	iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (iDisplay == EGL_NO_DISPLAY)
		{
		ProcessEglError();
		}

	// Initialize display object
	if (!eglInitialize(iDisplay, NULL, NULL))
		{
		ProcessEglError();
		}

	// Query number of configs available
	EGLint numConfigs;
	if (!eglGetConfigs(iDisplay, iConfig, KMaxConfigs, &numConfigs))
		{
		ProcessEglError();
		}

	const TDisplayMode dispMode = iWindow.DisplayMode();
	const EGLint* attribList = NULL;
	// Select attribs based on current display mode
	switch (dispMode)
		{
    case EColor4K:	
        attribList = KColor4KAttribList;
        break;
    case EColor64K:	
         attribList = KColor64KAttribList;
        break;
    case EColor16M:	
        attribList = KColor16MAttribList;
        break;
    case EColor16MA: 
        attribList = KColor16MAAttribList;
        break;
    default:
        attribList = KColor16MAAttribList;
        //todo: check why this can panic (which displaymode is used)
        //User::Panic(KErrDisplayMode, KErrNotSupported);
		}
	// Choose the config to use
	if (!eglChooseConfig(iDisplay, attribList, iConfig, 1, &numConfigs))
		{
		ProcessEglError();
		}
	
	// Create window surface to draw direct to
	iSurface = eglCreateWindowSurface(iDisplay, iConfig[0], &iWindow, NULL);
	if (iSurface == EGL_NO_SURFACE)
		{
		ProcessEglError();
		}
	
	iContext = eglCreateContext(iDisplay, iConfig[0], EGL_NO_CONTEXT, NULL);
	if(!iContext)
        {
        ProcessEglError();
        }
	}

void CEglEnvironment::DrawToWindow() const
    {
    if(!eglMakeCurrent(iDisplay, iSurface, iSurface, iContext))
        {
        ProcessEglError();
        }
    // Flush colour buffer to the window surface
    if(!eglSwapBuffers(iDisplay, iSurface))
        {
        ProcessEglError();
        }
    }

EGLSurface CEglEnvironment::Surface() const
	{
	return iSurface;
	}

EGLDisplay CEglEnvironment::Display() const
	{
	return iDisplay;
	}
