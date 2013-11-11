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

#include "directgdiadapter.h"
#include "directgdiimagetargetimpl.h"
#include "directgdidriverimpl.h"
#include "confighelper.h"

/**
Constructs a CDirectGdiImageTargetImpl.
@param	aDriver		The driver implementation which created this target.
*/
CDirectGdiImageTargetImpl::CDirectGdiImageTargetImpl(CDirectGdiDriverImpl& aDriver) :
	iDriver(aDriver),
	iSurface(EGL_NO_SURFACE),
	iContext(EGL_NO_CONTEXT)
	{
	}

/** 
Destructor for CDirectGdiImageTargetImpl - Destroys the EGL surface.
It will remove itself from the driver's image target array, before deleting itself.
*/
CDirectGdiImageTargetImpl::~CDirectGdiImageTargetImpl()
{	
#ifdef _DEBUG_DIRECTGDI
	GRAPHICS_LOG_DEBUG("Destroying CDirectGdiImageTargetImpl");
#endif
	if (iDriver.IsCurrentTarget(this))
		{
		iDriver.SetCurrentTarget(NULL);
		}
	
	if (iSurface != EGL_NO_SURFACE)
		{
		// If the surface being deleted is current to EGL, it will defer the destruction of the surface 
		// until something else is made current. Destruction would rather be immediate so that the 
		// associated native pixmap can be used again. In that case, the surface is dereferenced by making 
		// something else current.
		if (eglGetCurrentSurface(EGL_DRAW) == iSurface)
			{
			eglMakeCurrent(iDriver.EglDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);			
			}
		
		eglDestroySurface(iDriver.EglDisplay(), iSurface);	
		}
	iDriver.UnregisterTargetImage(*this);	
}

/**
Two-phase construction of a CDirectGdiImageTargetImpl object.
@param aImage On success, holds a pointer to the newly-created image.
@param aDriver The driver to register this image with.
@param aSgImage The RSgImage which this image will be based upon.
@param aContexts A hash map of already-created EGLContext/Config pairs. 
@param aConfigs A hash map of EGLConfig/Attribute pairs.
@param aSharedContext A reference to the current shared context.
@return KErrNone if successful, KErrNoMemory if not enough memory could be allocated, otherwise a return
        value from Construct().
*/
TInt CDirectGdiImageTargetImpl::New(CDirectGdiImageTargetImpl*& aImage, 
		CDirectGdiDriverImpl& aDriver, 
		const RSgImage& aSgImage, 
		RHashMap<TInt, EGLContext>& aContexts, 
		RHashMap<TInt, EGLConfig>& aConfigs, 
		EGLContext& aSharedContext)
	{
	CDirectGdiImageTargetImpl* image = new CDirectGdiImageTargetImpl(aDriver);
	if (!image)
		return KErrNoMemory;
	TInt err = image->Construct(aSgImage, aContexts, aConfigs, aSharedContext);
	if (err == KErrNone)
		aImage = image;
	else 
		delete image;
	return err;
	}

/**
Gets the supplied image structure and sets the internal data. It creates a new image target by creating 
an EGL context corresponding to a pixel type and an EGL config (if the EGLContext is not already created). 
If it was already there, it fetches the EGL context from the hash map. Creates an EGL surface that is then 
set for use by the new target object. Targets that are successfully created are added to an array of targets 
so that they can be closed and deleted correctly at a later stage.

@param aSgImage The RSgImage to create the source image from.
@param aContexts A hash map of already-created EGLContext/Config pairs. 
@param aConfigs A hash map of EGLConfig/Attribute pairs.
@param aSharedContext A reference to the current shared context.
@return KErrNone if successful, otherwise an error from CDirectGdiImageRef::Construct().

@panic DGDIAdapter 41, if a matching EGL config is not found for this RSgImage.
@panic DGDIAdapter 42, if an EGLSurface cannot be created.
@panic DGDIAdapter 43, if an EGLContext cannot be created.
*/ 
TInt CDirectGdiImageTargetImpl::Construct(const RSgImage& aSgImage, RHashMap<TInt, EGLContext>& aContexts, RHashMap<TInt, EGLConfig>& aConfigs, EGLContext& aSharedContext)
	{
	TSgImageInfo info;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext* context = NULL;
	EGLContext newContext = EGL_NO_CONTEXT;
	TBool createdNewContext = EFalse;
	TInt pixmapAttribs = 0;
	const EGLDisplay display = iDriver.EglDisplay();
	
	TInt err = CDirectGdiImageRef::Construct(aSgImage);
			
	if (err == KErrNone)
		{
		err = iSgImage.GetInfo(info);
		}
	
	if (err == KErrNone)
		{
		// Create an EGL target surface.
		// Get the right Config index for this RSgImage format.
		pixmapAttribs = TConfigHelper::MatchPixelType(info.iPixelFormat);
		EGLConfig* currentConfig = aConfigs.Find(pixmapAttribs);
		GRAPHICS_ASSERT_ALWAYS(currentConfig, EDirectGdiPanicNoConfigFound);	
			
		// Set up the surface attribs.
		const EGLint eglAlphaFormat = (info.iPixelFormat == EUidPixelFormatARGB_8888_PRE) ? EGL_VG_ALPHA_FORMAT_PRE : EGL_VG_ALPHA_FORMAT_NONPRE;
		EGLint attributeList[] = {EGL_VG_ALPHA_FORMAT, eglAlphaFormat,
								  EGL_VG_COLORSPACE,   EGL_VG_COLORSPACE_sRGB,
								  EGL_NONE};
		
		// Create an EGL target surface.
		surface = eglCreatePixmapSurface(display, *currentConfig, (RSgImage*)&iSgImage, attributeList);
		GRAPHICS_ASSERT_ALWAYS(surface != EGL_NO_SURFACE, EDirectGdiPanicNoDrawingSurface);
		
		// See if we have already created a context for this pixel format, create
		// one if we haven't and save it for reuse later.
		context = aContexts.Find(pixmapAttribs);	
		if (context == NULL)
			{
			newContext = eglCreateContext(display, *currentConfig, aSharedContext, NULL);	
			GRAPHICS_ASSERT_ALWAYS(newContext != EGL_NO_CONTEXT, EDirectGdiPanicNoContext);	
			err = aContexts.Insert(pixmapAttribs, newContext);	
			if (err == KErrNone)
				context = &newContext;
			createdNewContext = ETrue;
			}
		}
	
	if (err == KErrNone)
		{
		err = iDriver.RegisterTargetImage(*this);				
		}
	
	if (err == KErrNone)
		{
		// Everything went well.
		iContext = *context;
		iSurface = surface;
		if (createdNewContext)
			aSharedContext = iContext;
		Open();
		}
	else
		{
		// Cleanup.
		if (surface != EGL_NO_SURFACE)
			{
			eglDestroySurface(display, surface);
			}
		if (createdNewContext)
			{
			if (context)
				{
				eglDestroyContext(display, *context);
				}
			if (aContexts.Find(pixmapAttribs))
				{
				aContexts.Remove(pixmapAttribs);
				}
			}
		}
	
	return err;
	}

/**
Sets the passed EGLSurface as the current rendering target for EGL, sets the passed 
EGLContext as the current context. The surface will be used for both read and write 
operations. The previously current context will be flushed at this point.

@pre The target has been successfully constructed.

@return ETrue if activation was successful, otherwise EFalse.
*/
TBool CDirectGdiImageTargetImpl::Activate()
	{
	return (EGL_TRUE == eglMakeCurrent(iDriver.EglDisplay(), iSurface, iSurface, iContext));
	}
