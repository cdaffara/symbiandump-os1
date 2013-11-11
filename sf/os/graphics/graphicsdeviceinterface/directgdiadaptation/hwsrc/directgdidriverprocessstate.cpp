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

#include "directgdidriverprocessstate.h"


const TInt KMaxProcessStateHeapSize = 0x100000;


/** 
Reports any error that occurs. 
This is #defined so that we retain correct line information for the debug stream.
*/
#define ProcessEglError(err) \
	{ \
	const EGLint eglError = eglGetError(); \
	if (eglError == EGL_BAD_ALLOC) \
		err = KErrNoMemory; \
	else if (eglError == EGL_CONTEXT_LOST) \
		err = KErrDied; \
	else \
		err = KErrGeneral; \
	TBuf16<256> _message; \
	_message.Format(_L16("EGL Error: %x\n"), eglError); \
	GRAPHICS_LOGD_DEBUG(_message); \
	}
/**
Image source data constructor.
@param aSgImageId The unique identifier of the RSgImage used to create the EGLImage and VGImage.
@param aEglImage The EGLImage created from the RSgImage.
@param aVgImage The VGImage created from aEglImage.
@panic  DGDIAdapter 48, if invalid images are passed in.
*/
CImageSourceData::CImageSourceData(const TSgDrawableId& aSgImageId, EGLImageKHR aEglImage, VGImage aVgImage) :
	iSgImageId(aSgImageId),
	iEglImage(aEglImage),
	iVgImage(aVgImage)
	{
	GRAPHICS_ASSERT_DEBUG(iEglImage != EGL_NO_IMAGE_KHR, EDirectGdiPanicImageSourceDataConstructorError);
	GRAPHICS_ASSERT_DEBUG(iVgImage != VG_INVALID_HANDLE, EDirectGdiPanicImageSourceDataConstructorError);
	}

/**
Image source data destructor.
@panic  DGDIAdapter 49, if reference count is not zero when this funtion is called.
*/
CImageSourceData::~CImageSourceData()
	{
	GRAPHICS_ASSERT_ALWAYS(iRefCount == 0, EDirectGdiPanicImageSourceDataRefCountError);
	}

XDirectGdiDriverProcessState::XDirectGdiDriverProcessState()
	{
	iCreateMutexErr = iMutex.CreateLocal();
	}


/**
Creates a heap to store EGL and VG images. This heap will be shared between threads in 
the current process. This is necessary as EGL images are shared between threads, and an attempt
to create an EGL image from a RSgImage in two different threads would fail if the EGL image was not 
also shared. Also gets function pointers to image creation and destruction methods in EGL and saves 
them for use later. Note that Initialize() must be called before you call CreateVgImage() or
DestroyVgImage().

@pre The client has called MutexCreationStatus() and checked for KErrNone. 

@param aDisplay The current EGL display handle 

@return KErrNone if successful, KErrNoMemory if we fail to create the heap, KErrGeneral
if we encounter an EGL error, otherwise one of the system-wide error codes.
 */
TInt XDirectGdiDriverProcessState::Initialize(EGLDisplay aDisplay)
	{
	iMutex.Wait();
	
	// No need to re-initialize everything if we have already been initialized.
	TInt err = KErrNone;
	if (iLocalHeap == NULL)
		{
		// Create a heap that will be used to store EGLImages and VGImages that are created
		// when the driver creates a source image (in CDirectGdiDriverImpl::CreateDrawableSource())
		iLocalHeap = UserHeap::ChunkHeap(NULL, 0, KMaxProcessStateHeapSize);
		if (iLocalHeap == NULL)
			{
			err = KErrNoMemory;		
			}
		}
	
	// Check the availability of the required EGL/VG extensions
	if (err == KErrNone)
		{
		if (!iCheckedExtensionsAvailable)
			{
			err = CheckAllEglExtensions(aDisplay);				
			iCheckedExtensionsAvailable = ETrue;
			
#ifdef _DEBUG_DIRECTGDI
			RDebug::Printf("VG_VENDOR: %s\n", vgGetString(VG_VENDOR));
			RDebug::Printf("VG_RENDERER : %s\n", vgGetString(VG_RENDERER));
			RDebug::Printf("VG_VERSION : %s\n", vgGetString(VG_VERSION));
#endif	
			}
		}
					
	// Get function pointers for EGLImage creation and destruction and VGImage creation EGL functions.
	// These need to be refreshed every time this process state is initialised as it cannot be guaranteed
	// that the function pointers will remain the same after this process state was last closed
	// (and EGL was terminated).
	if (err == KErrNone)
		{
		iPfnEglCreateImageKHR = reinterpret_cast<TeglCreateImageKHRTypefPtr>(eglGetProcAddress("eglCreateImageKHR"));		
		if (iPfnEglCreateImageKHR == NULL)
			{
			ProcessEglError(err);
			err = KErrNotSupported;
			}		
		}
	
	if (err == KErrNone)
		{
		iPfnEglDestroyImageKHR = reinterpret_cast<TeglDestroyImageKHRTypefPtr>(eglGetProcAddress("eglDestroyImageKHR"));	
		if (iPfnEglDestroyImageKHR == NULL)
			{
			ProcessEglError(err);
			err = KErrNotSupported;
			}		
		}
	
	if (err == KErrNone)
		{
		iPfnVgCreateImageTargetKHR = reinterpret_cast<TvgCreateEGLImageTargetKHRTypefPtr>(eglGetProcAddress("vgCreateEGLImageTargetKHR"));		
		if (iPfnVgCreateImageTargetKHR == NULL)
			{
			ProcessEglError(err);
			err = KErrNotSupported;
			}		
		}
	iMutex.Signal();
		
	return err;
	}

/**
Destructor, asserts that the instance count is zero, and the count of images stored is zero.

@panic DGDIAdapter 50, if the instance count is not zero (debug-only).
@panic DGDIAdapter 51, if the image count is not zero (debug-only).
 */
XDirectGdiDriverProcessState::~XDirectGdiDriverProcessState()
	{
	if (iLocalHeap != NULL)
		{
		iLocalHeap->Close();
		}
	iMutex.Close();
		
	GRAPHICS_ASSERT_DEBUG(iDriverInstanceCount == 0, EDirectGdiPanicProcessStateInstanceCountError);	
	GRAPHICS_ASSERT_DEBUG(iImages.Count() == 0, EDirectGdiPanicProcessStateImageCountError);
	}

/**
Creates a VGImage from the passed RSgImage. This method first creates a EGLImageKHR from
the RSgImage, from which it can create a VGImage. The EGLImageKHR and the VGImage are saved
so that they can be shared if a user attempts to create another VGImage from the same RSgImage.
If a VGImage has already been created from the passed RSgImage, the existing image is returned
instead of a new one being created. This will increment the reference count on that stored image.

@param aDisplay The EGL display to associate the image with.
@param aVgImageRet A handle to the VGImage created is returned here.
@param aSgImage The VGImage will be created from this RSgImage.

@pre Initialize() has been called.

@return KErrNone if the image is created successfully, KErrGeneral if an EGL error occurs,
otherwise one of the system-wide error codes.

@panic DGDIAdapter 33, if one of the member EGL function pointers is NULL (debug-only).
If this happens it may be that Initialize() has not been called for this process state.
@panic DGDIAdapter 36, if aDisplay is EGL_NO_DISPLAY (debug-only).
 */
 TInt XDirectGdiDriverProcessState::CreateVgImage(EGLDisplay aDisplay, VGImage& aVgImageRet, const RSgImage& aSgImage)
	{
	VGImage vgImage = VG_INVALID_HANDLE;
	TInt err = KErrNone;	
	aVgImageRet = VG_INVALID_HANDLE;
	
	iMutex.Wait();	
	// If this image has already been created, just return a handle to the
	// previously created one.
	if (ImageExists(aSgImage, vgImage))
		{
		aVgImageRet = vgImage;
		iMutex.Signal();
		return KErrNone;
		}
	
	// If the function pointers are NULL, Initialize() may not have been called
	GRAPHICS_ASSERT_DEBUG(iPfnEglCreateImageKHR, EDirectGdiPanicProcessStateNotInitialized);	
	GRAPHICS_ASSERT_DEBUG(iPfnEglDestroyImageKHR, EDirectGdiPanicProcessStateNotInitialized);	
	GRAPHICS_ASSERT_DEBUG(iPfnVgCreateImageTargetKHR, EDirectGdiPanicProcessStateNotInitialized);
	GRAPHICS_ASSERT_DEBUG(aDisplay != EGL_NO_DISPLAY, EDirectGdiPanicNoDisplay);
	
	// Create an EGLImage from the passed RSgImage, then create a VGImage from that and 
	// assign the VGImage to the newly created source. (The EGL image creation functions
	// have already been retreived at this stage as function pointers using eglGetProcAddress()
	// in InitialiseEgl())	
	
	// Pass in the preserved attribute. Without this the existing image data could be trashed.
	EGLint createImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, 
								   		 EGL_NONE};
	
	EGLImageKHR eglImage = iPfnEglCreateImageKHR(aDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, reinterpret_cast<EGLClientBuffer>(&aSgImage), createImageAttribs);
	if (eglImage == EGL_NO_IMAGE_KHR)
		{
		ProcessEglError(err);
		}
		
	if (err == KErrNone)
		{
		vgImage = iPfnVgCreateImageTargetKHR(eglImage);	
		if (vgImage == VG_INVALID_HANDLE)
			{				
			ProcessEglError(err);
			}		
		}
		
	if (err == KErrNone)
		{
		// Add the image to the array of images on the process heap
		err = AddImage(aSgImage, eglImage, vgImage);
		}
	
	if (err == KErrNone)
		{		
		aVgImageRet = vgImage;
		}
	else
		{		
		// Clean up
		if (vgImage != VG_INVALID_HANDLE)
			{
			vgDestroyImage(vgImage);
			}
		iPfnEglDestroyImageKHR(aDisplay, eglImage);		
		}
	
	iMutex.Signal();
	
	return err;
	}
 
/**
Destroys the passed VGImage. Searches for the image in the image array and
decrements its reference count if found. Destroys the image when the reference count
reaches zero.

@param aDisplay The EGLDisplay the VGImage to be destroyed was created with.
@param aVgImage A handle to the VGImage to destroy.

@return KErrNone if the image is destroyed successfully, otherwise one of the system-wide
error codes.

@panic DGDIAdapter 33, if the EGL function pointer for destroying a VGImage is NULL (debug-only).
 */
TInt XDirectGdiDriverProcessState::DestroyVgImage(EGLDisplay aDisplay, VGImage aVgImage)
	{	
	GRAPHICS_ASSERT_DEBUG(iPfnEglDestroyImageKHR, EDirectGdiPanicProcessStateNotInitialized);
	
	iMutex.Wait();
	TInt err = RemoveImage(aDisplay, aVgImage);	
	iMutex.Signal();
	
	return err;
	}

/**
Increments the instance count.
*/
void XDirectGdiDriverProcessState::OpenDriver()
	{
	iMutex.Wait();
	++iDriverInstanceCount;
	iMutex.Signal();
	}

/**
Decrements the instance count.
If this is the last instance of a DirectGDI driver, the associated EGL display is terminated, and
there should be no images left in the array as they should all have been destroyed.

@param aDriverEglDisplay An EGLDisplay associated with the driver being closed. If this is the last
       driver being closed, the display is destroyed.
@panic DGDIAdapter 51, if the image count is not zero (debug-only).
 */
void XDirectGdiDriverProcessState::CloseDriver(EGLDisplay& aDriverEglDisplay)
	{
	iMutex.Wait();
	if (--iDriverInstanceCount == 0 && aDriverEglDisplay != EGL_NO_DISPLAY)
		{
		eglTerminate(aDriverEglDisplay);	
		aDriverEglDisplay = EGL_NO_DISPLAY;
		// Set the function pointers to NULL as cannot be guaranteed that they will be recreated at the same address
		// when EGL is next initialised.
		iPfnEglCreateImageKHR = NULL;
		iPfnEglDestroyImageKHR = NULL;
		iPfnVgCreateImageTargetKHR = NULL;
		GRAPHICS_ASSERT_DEBUG(iImages.Count() == 0, EDirectGdiPanicProcessStateImageCountError);
		}
	iMutex.Signal();
	}

/** 
Check that the following EGL extensions are available:
	EGL_SYMBIAN_PIXMAP_TYPE_RSGIMAGE
	EGL_KHR_image
	EGL_SYMBIAN_image_preserved
	
According to the spec, OpenVG extensions cannot be checked until a context has been made current, 
which has not happened at this point, so currently there is no check for VG_KHR_EGL_image extension. 
	
@pre eglInitialize() must have been called for the passed EGLDisplay.	
	
@param aDisplay The current EGL display handle. 	

@return KErrNone if all the extensions are available, KErrNotSupported if one or more
of the extensions are not supported by the EGL driver.	
 */
TInt XDirectGdiDriverProcessState::CheckAllEglExtensions(EGLDisplay aDisplay) const
	{
	_LIT8(KEglRSgImage, "EGL_SYMBIAN_PIXMAP_TYPE_RSGIMAGE");		
	_LIT8(KEglKhrImage, "EGL_KHR_image");
	_LIT8(KEglImagePreserved, "EGL_SYMBIAN_image_preserved");
		
	TInt err = CheckForEglExtension(aDisplay, KEglRSgImage);
	
	if (err == KErrNone)
		{
		err = CheckForEglExtension(aDisplay, KEglKhrImage);
		}
	
	if (err == KErrNone)
		{
		err = CheckForEglExtension(aDisplay, KEglImagePreserved);
		}
			
	return err;
	}

/**
Checks whether the passed EGL extension string is available with this EGL driver.

@param aDisplay The current EGL display handle.
@param aExtensionString The EGL extension string to search for.

@return KErrNotSupported is the extension string is not found, KErrNone if it is found successfully.
 */
TInt XDirectGdiDriverProcessState::CheckForEglExtension(EGLDisplay aDisplay,
															  const TDesC8& aExtensionString) const
{
	const TText8* extensions = reinterpret_cast<const TText8*>(eglQueryString(aDisplay, EGL_EXTENSIONS));
	
	TBool found = EFalse;	
	if (extensions)
		{		
		TPtrC8 ext(extensions);
		if (ext.Find(aExtensionString) != KErrNotFound)
			{
			found = ETrue;
			}
		}
	
	return (found) ? KErrNone : KErrNotSupported;
	}

/**
Checks to see that the passed RSgImage exists in the image array. Increases
the reference count on the image if it is found. The calling method should be holding the mutex
iMutex open before calling this method.

@param aSgImage The RSgImage to check for.
@param aVgImageRet The handle of the VGImage found is returned here.

@pre The mutex iMutex is held.  

@return ETrue if the image was found, EFalse otherwise.

@panic DGDIAdapter 52, if the mutex has not been opened before calling this method. 
	   This panic only happens when _DEBUG_DIRECTGDI is defined.
 */
TBool XDirectGdiDriverProcessState::ImageExists(const RSgImage& aSgImage, VGImage& aVgImageRet)
	{
#ifdef _DEBUG_DIRECTGDI
	GRAPHICS_ASSERT_ALWAYS(iMutex.IsHeld(), EDirectGdiPanicImageMutexError);
#endif	
	aVgImageRet = VG_INVALID_HANDLE;
	
	CImageSourceData* imageData = FindImage(aSgImage);
	if (imageData)
		{
		aVgImageRet = imageData->VgImage();
		imageData->Open();
		}

	return (aVgImageRet != VG_INVALID_HANDLE);
	}

/** 
Searches for an image in the array of images using the passed RSgImage id to find it.
The calling method should be holding the images mutex iMutex open before calling this method.

@param aSgImage The RSgImage to search for.

@pre The mutex iMutex is held.

@return Pointer to the CImageSourceData object held in the images array for the passed
RSgImage if it is found, NULL if the image is not found.

@panic DGDIAdapter 52, if the mutex has not been opened before calling this method. 
	   This panic only happens when _DEBUG_DIRECTGDI is defined.
 */
CImageSourceData* XDirectGdiDriverProcessState::FindImage(const RSgImage& aSgImage)
	{
#ifdef _DEBUG_DIRECTGDI
	GRAPHICS_ASSERT_ALWAYS(iMutex.IsHeld(), EDirectGdiPanicImageMutexError);
#endif
	CImageSourceData* imageDataRet = NULL;
	
	for (TInt i = 0; i < iImages.Count() && !imageDataRet; i++)
		{
		CImageSourceData* imageData = reinterpret_cast<CImageSourceData*>(iImages[i]);
		if (imageData->SgImageId() == aSgImage.Id())
			{
			imageDataRet = imageData;
			}
		}
	
	return imageDataRet;
	}
	
/**
Adds an image to the images array by saving its RSgImage id, its EGLImageKHR handle and its VGImage
handle. A new CImageSourceData object is created to hold this information. This CImageSourceData object 
is also used to maintain a reference count on the number of times this image has been shared.
The calling method should be holding the images mutex iMutex open before calling this method.

@see RemoveImage()

@param aSgImage The RSgImage to store, only the RSgImage id will be stored.
@param aEglImage The EGLImageKHR to store.
@param aVgImage The VGImage to stord.

@pre The mutex iMutex is held.
@return KErrNone if the details of the passed image are added to the image array successfully, KErrNoMemory
if memory allocation fails, otherwise one of the system-wide error codes.

@panic DGDIAdapter 52, if the mutex has not been opened before calling this method. 
	   This panic only happens when _DEBUG_DIRECTGDI is defined.
 */
TInt XDirectGdiDriverProcessState::AddImage(const RSgImage& aSgImage, EGLImageKHR aEglImage, VGImage aVgImage)
	{
#ifdef _DEBUG_DIRECTGDI
	GRAPHICS_ASSERT_ALWAYS(iMutex.IsHeld(), EDirectGdiPanicImageMutexError);
#endif
	TInt err = KErrNone;
	RHeap *prevHeap = User::SwitchHeap(iLocalHeap);
	
	CImageSourceData* imageData = new CImageSourceData(aSgImage.Id(), aEglImage, aVgImage);
	if (imageData == NULL)
		{
		err = KErrNoMemory;
		}	
	if (err == KErrNone)
		{				
		err = iImages.InsertInAddressOrder(imageData);
		}	
	if (err == KErrNone)
		{
		imageData->Open();
		}
	else
		{
		delete imageData;
		}
		
	User::SwitchHeap(prevHeap);	
	return err;
	}
	
/**
Removes the passed image from the image array by searching for it using its VGImage handle.
If the image is found in the image array, its reference count will be decremented. When the count reaches
zero the image will be removed from the image array and destroyed.

@see AddImage()

@param aDisplay The EGL display this image was created with.
@param aVgImage The hande to a VGImage to be removed from the image array.

@pre The mutex iMutex is held.

@return KErrNone if the image is removed from the image array successfully, KErrNotFound if the image
cannot be found in the image array, otherwise one of the system-wide error codes.

@panic DGDIAdapter 52, if the mutex has not been opened before calling this method. 
	   This panic only happens when _DEBUG_DIRECTGDI is defined.
 */
TInt XDirectGdiDriverProcessState::RemoveImage(EGLDisplay aDisplay, VGImage aVgImage)
	{
#ifdef _DEBUG_DIRECTGDI
	GRAPHICS_ASSERT_ALWAYS(iMutex.IsHeld(), EDirectGdiPanicImageMutexError);
#endif
	TInt err = KErrNone;
	TInt index = -1;
	CImageSourceData* removeImage = NULL;	
	
	for (TInt i = 0; i < iImages.Count() && (index == -1); i++)
		{
		if (CImageSourceData* imageData = reinterpret_cast<CImageSourceData*>(iImages[i]))
			{
			if (imageData->VgImage() == aVgImage)
				{
				// Only remove the image if the reference count is zero
				if (imageData->Close() == 0)					
					removeImage	= imageData;									
				index = i;
				}
			}
		}
			
	if (removeImage)
		{
		// Clean up the EGL and VG images
		vgDestroyImage(removeImage->VgImage());
		iPfnEglDestroyImageKHR(aDisplay, removeImage->EglImage());
		
		RHeap *prevHeap = User::SwitchHeap(iLocalHeap);
		iImages.Remove(index);
		iImages.GranularCompress();
		delete removeImage;
		User::SwitchHeap(prevHeap);
		}
	
	if (index == -1)
		{
		err = KErrNotFound;
		}
	
	return err;
	}
