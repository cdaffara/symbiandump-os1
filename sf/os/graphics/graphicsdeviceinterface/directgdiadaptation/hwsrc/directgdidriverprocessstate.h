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
// directhdidriverprocessstate.h
// 
//

#ifndef _DIRECTGDIDRIVERPROCESSSTATE_H_
#define _DIRECTGDIDRIVERPROCESSSTATE_H_

/**
@file
@internalComponent
*/

#include "directgdiadapter.h"
#include <graphics/sgimage.h>
#include <EGL/egl.h>
#include <VG/openvg.h>
#include <e32base.h>


/**
Function pointers for use with eglGetProcAddress for creation and deletion of EGLImages and creation of VGImages
 */
typedef EGLImageKHR (*TeglCreateImageKHRTypefPtr) (EGLDisplay dpy, EGLContext ctx, EGLenum aTarget, EGLClientBuffer buffer, EGLint*attrib_list);
typedef EGLBoolean (*TeglDestroyImageKHRTypefPtr) (EGLDisplay dpy, EGLImageKHR image);
typedef EGLBoolean (*TvgCreateEGLImageTargetKHRTypefPtr) (VGeglImageKHR image);

/**
Helper class used by CDirectGdiDriverProcessState to group together related EGL and VG images.
 */
NONSHARABLE_CLASS(CImageSourceData): public CBase
	{
public:
	CImageSourceData(const TSgDrawableId& aSgImageId, EGLImageKHR aEglImage, VGImage aVgImage);
	virtual ~CImageSourceData();
	
	inline const TSgDrawableId& SgImageId() const; 
	inline EGLImageKHR EglImage() const;
	inline VGImage VgImage() const;
	inline TInt Open();
	inline TInt Close();	
	
private:
	TSgDrawableId iSgImageId;
	EGLImageKHR iEglImage;
	VGImage iVgImage;
	TInt iRefCount;	
	};



/**
Class for storing the state of CDirectGdiDriver objects within the current process. This class is 
responsible for maintaining an instance count of the number of drivers open in this process. There may 
be more than one thread that creates and opens a driver in this process. This class initializes EGL and  
terminates it using an instance count, as EGL terminate should not be called until all drivers in this 
process have been closed, not when each thread closes as another thread may be using EGL in the same
process.

This class is also responsible for storing a list of all EGLImageKHR objects and associated RSgImage IDs
created in this process, as there is a restriction that an EGLImageKHR can only be created
from a particular RSgImage once per process. This allows EGL images that have already been created
from RSgImages to be shared among threads. The list of EGL images is created on a shared process heap
so that it can be shared between all threads.

Users of this class should call CreateVGImageFromSgImage
 */
NONSHARABLE_CLASS(XDirectGdiDriverProcessState): public CBase
	{
public:
	XDirectGdiDriverProcessState();	
	virtual ~XDirectGdiDriverProcessState();
		
	TInt Initialize(EGLDisplay aDisplay);
	TInt CreateVgImage(EGLDisplay aDisplay, VGImage& aVgImageRet, const RSgImage& aSgImage);
	TInt DestroyVgImage(EGLDisplay aDisplay, VGImage aVgImage);
	
	void OpenDriver();
	void CloseDriver(EGLDisplay& aDriverEglDisplay);
	
	inline TInt MutexCreationStatus() const;
	
private:	
	TInt CheckAllEglExtensions(EGLDisplay aDisplay) const ;
	TInt CheckForEglExtension(EGLDisplay aDisplay, const TDesC8& aExtensionString) const;
	
	// Always use the mutex iImagesMutex around the following methods
	TBool ImageExists(const RSgImage& aSgImage, VGImage& aVgImageRet);
	CImageSourceData* FindImage(const RSgImage& aSgImage);
	TInt AddImage(const RSgImage& aSgImage, EGLImageKHR aEglImage, VGImage aVgImage);
	TInt RemoveImage(EGLDisplay aDisplay, VGImage aVgImage);
		
	TInt iDriverInstanceCount;
	RHeap* iLocalHeap;		
	RPointerArray<CImageSourceData> iImages;
	RMutex iMutex;
	// Holds the error state for creating the mutex in the constructor, so that it can be reported
	// outside of the constructor.
	TInt iCreateMutexErr;	
	TBool iCheckedExtensionsAvailable;
	
	// Function pointers for creation and destruction of EGLImageKHR and creation of VGImage
	TeglCreateImageKHRTypefPtr iPfnEglCreateImageKHR;
	TeglDestroyImageKHRTypefPtr iPfnEglDestroyImageKHR;
	TvgCreateEGLImageTargetKHRTypefPtr iPfnVgCreateImageTargetKHR;
	};


#include "directgdidriverprocessstate.inl"

#endif // _DIRECTGDIDRIVERPROCESSSTATE_H_
