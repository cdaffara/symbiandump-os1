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

#include "directgdidriverimpl.h"
#include "directgdiimagetargetimpl.h"
#include "directgdiimagesourceimpl.h"
#include "directgdiadapter.h"
#include "directgdiimageref.h"
#include "vgengine.h"
#include "confighelper.h"
#include <graphics/directgdiimagetarget.h>
#include <graphics/sgresource.h>
#include <fbs.h>
#include <bitdev.h>

#ifndef __WINS__
// Static member variable
XDirectGdiDriverProcessState CDirectGdiDriverImpl::gProcessState;
#endif

/**
Handles an EGL failure. It may either set the passed-in TInt to a Symbian OS-mapped error code,
or panic the specified panic code, depending on the egl failure. Done as a macro so the line number is reported.
*/
#define HANDLE_EGL_ERROR(err, panic) \
	{ \
	const EGLint eglError = eglGetError(); \
	if (eglError == EGL_BAD_ALLOC) \
		err = KErrNoMemory; \
	else if (eglError == EGL_CONTEXT_LOST) \
		err = KErrDied; \
	else if (eglError != EGL_SUCCESS) \
		{ \
		TBuf16<256> _message; \
		_message.Format(_L16("EGL Error: %x\n"), eglError); \
		GRAPHICS_LOGD_DEBUG(_message); \
		GRAPHICS_ASSERT_DEBUG(eglError == EGL_SUCCESS, panic);	\
		} \
	}

/**
Populates the config hash map with matching EGL configs, based on the supported pixel types.

@param aDisplay A valid EGL display to retrieve EGL configs for.

@panic DGDIAdapter 24, if no suitable EGL configs are present (debug-only).
@panic DGDIAdapter 25, if no exact matching EGL config can be found.
@panic DGDIAdapter 29, if RSgImage::GetPixelFormats returns pixel count <= 0.

@pre  EGL has been initialised by eglInitialize().
@post The supported EGL configs are stored in the hash map.

@return KErrNoError if successful, KErrNotReady if EGL was not initialised, otherwise one of the 
system-wide error codes.
*/
TInt CDirectGdiDriverImpl::InitializeEglConfigs(EGLDisplay aDisplay)
	{
#ifdef _DEBUG_DIRECTGDI
	GRAPHICS_LOG_DEBUG("InitializeEglConfigs");
#endif
	TInt err = KErrNone;	
	
	// Get EGL configs upfront and store them in a hash map	
	EGLint configSize = 0;
	EGLConfig *configs = NULL;
	
	// Query RsgImage for the number of supported target pixel formats. Then we know how much memory should be 
	// allocated to the pixelformats array.
	TInt count = 0;
	TSgImageInfo imageInfo;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	imageInfo.iSizeInPixels = TSize(1,1);
	TUidPixelFormat* pixelFormats = NULL;		
	err = RSgImage::GetPixelFormats(imageInfo, NULL, count);
	GRAPHICS_ASSERT_DEBUG(count > 0, EDirectGdiPanicNoValidPixelFormats);
	
	if (err == KErrNone)
		{
		pixelFormats = new TUidPixelFormat[count];			
		if (pixelFormats == NULL)
			{
			err = KErrNoMemory;
			}
		}
	
	if (err == KErrNone)
		{
		err = RSgImage::GetPixelFormats(imageInfo, pixelFormats, count);		
		if (!eglGetConfigs(aDisplay, configs, 0, &configSize) && configSize)
			{
			HANDLE_EGL_ERROR(err, EDirectGdiPanicNoAvailableConfigs);
			}
		}
	
	if (err == KErrNone)
		{
		configs = new EGLConfig[configSize];
		if (configs == NULL)
			{
			err = KErrNoMemory;
			}
		}
	
	for (TInt i = 0; (i < count) && (err == KErrNone); ++i) 
		{
		// Finds a suitable config to map to this bitmap.
		TInt selPixmapAttribs = TConfigHelper::MatchPixelType(pixelFormats[i]);
		EGLint numConfigs = 0;
		if (!eglChooseConfig(aDisplay, TConfigHelper::KSurfaceAttribs[selPixmapAttribs], configs, configSize, &numConfigs))
			{
			HANDLE_EGL_ERROR(err, EDirectGdiPanicNoMatchingConfig);
			break;
			}
		GRAPHICS_ASSERT_ALWAYS(numConfigs >= 1, EDirectGdiPanicNoAvailableConfigs);
		// Find an exact matching config from the available configs.
		TInt match = TConfigHelper::GetSuitablePixmapConfigIndex(aDisplay, configs, numConfigs, selPixmapAttribs);
		GRAPHICS_ASSERT_ALWAYS(match >= 0, EDirectGdiPanicNoMatchingConfig);
		EGLConfig currentConfig = configs[match];

		// Store this matching config and the pixel type in the hash map.
		err = iPixelConfigMap.Insert(selPixmapAttribs, currentConfig);					
		}		
	
	delete [] configs;		
	delete [] pixelFormats;
	
	return err;
	}

/**
Constructor to allocate memory to CDirectGdiDriverImpl object.

@param aInternal On success, will store a pointer to the newly created CDirectGdiDriverImpl.
@param aLibrary
@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
TInt CDirectGdiDriverImpl::New(CDirectGdiDriverInternal*& aInternal, RLibrary aLibrary)
	{
	CDirectGdiDriverImpl* self = new CDirectGdiDriverImpl(aLibrary);
	if(!self)
		return KErrNoMemory;
	TInt err = self->Construct();
	if(err != KErrNone)
		{
		aInternal = NULL;
		delete self;
		return err;
		}
	aInternal = self;
	return err;		
	}

/**
Destructor - frees the memory for the hashmaps and the source and target arrays.

@panic DGDIAdapter 45, if images are still in the source or target image array (debug-only).
*/
CDirectGdiDriverImpl::~CDirectGdiDriverImpl()
	{
	// Check that there are no drawbles left in the drawable array
	GRAPHICS_ASSERT_DEBUG(iSourceArray.Count() == 0, EDirectGdiPanicItemsLeftInImageArray);
	GRAPHICS_ASSERT_DEBUG(iTargetArray.Count() == 0, EDirectGdiPanicItemsLeftInImageArray);
	
	iSourceArray.Close();
	iTargetArray.Close();

	delete iVgImageCache;
	
	THashMapIter<TInt, EGLContext> contextIter(iPixelContextMap);
	const EGLContext* context = contextIter.NextValue();
	while (context)
		{
		eglDestroyContext(iDisplay, *context);
		context = contextIter.NextValue();		
		}
	iPixelContextMap.Close();
	iPixelConfigMap.Close();
	delete iGlyphImageStorage;

	eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	// This will terminate the display if it is the last driver.
	ProcessState().CloseDriver(iDisplay); 
	eglReleaseThread();
	
	SgDriver::Close();
	}

/**
@see CDirectGdiDriver::Flush()
*/
void CDirectGdiDriverImpl::Flush()
	{
	vgFlush();
	}

/**
 @see CDirectGdiDriver::Finish()
 */
void CDirectGdiDriverImpl::Finish()
	{
	// Call eglWaitClient() instead of vgFinish() because MBX EGL implementation currently 
	// does nothing with a vgFinish().
	if (eglWaitClient() == EGL_FALSE)
		{
		TInt err = KErrNone;
		HANDLE_EGL_ERROR(err, EDirectGdiPanicFinish);
		SetError(err);
		}

	}

/**
@see CDirectGdiDriver::GetError()

If no error has been reported explicitly, or SetError() has not been called, it will instead return
the first (if any) OpenVG error to occur since GetError() was last called.
*/
TInt CDirectGdiDriverImpl::GetError()
	{
	// Call vgGetError() even when iErrorCode is already set, so that any OpenVG error state is 
	// cleared and not reported in future.
	const TInt vgErrorCode = GetVgError();
	
	if (iErrorCode == KErrNone)
		{
		iErrorCode = vgErrorCode;
		}
	TInt error = iErrorCode;
	iErrorCode = KErrNone;
	return error;
	}

/**
@see CDirectGdiDriver::CreateDrawableSource()

For image resource creates a DirectGDI adaptation-specific resource from the given RSgDrawable resource so 
it can be drawn using the DirectGDI rendering API. Creates an EGLImage, then a VGImage from the 
passed RSgImage and associates the new source with these images. Sources that are successfully created 
are added to an array of sources so that they can be closed and deleted correctly at a later stage.
Note that an image source cannot be created before an image target has been created and activated.
This is a limitation of OpenVG as a VG context must exist before an image source can be created.

@param aHandleRet Handle of newly created drawable source, must be KNullHandle (is checked by generic driver).
@param aSgDrawable The RSgDrawable object to use when creating the drawable source. An attempt is made to 
match the pixel format of the new EGL surface with the pixel format of this RSgImage.

@pre CDirectGdiDriver object has been initialised from the calling thread and a target has been activated.
     aHandleRet is KNullHandle.
@post The DirectGDI adaptation-specific resource that is bound to the given 
drawable resource is created and this handle is now associated with it. The reference 
counter on the drawable resource is incremented. The CDirectGdiDriver for this thread 
is now aware of and owns the adaptation-specific resource.

@return KErrNone if successful, KErrArgument if the drawable resource is not valid, 
KErrNotSupported if the drawable resource is not created with the type (KSgImageTypeUid), 
otherwise one of the system-wide error codes.
*/
TInt CDirectGdiDriverImpl::CreateDrawableSource(TInt& aHandleRet, const RSgDrawable& aSgDrawable)
	{
	TUid typeUid = aSgDrawable.DrawableType();
	if(typeUid != KSgImageTypeUid)
		{
		return KErrNotSupported;
		}
	const RSgImage* image = reinterpret_cast<const RSgImage*>(&aSgDrawable); 
	CDirectGdiImageSourceImpl* imageSource = NULL;
	TInt err = CDirectGdiImageSourceImpl::New(imageSource, *this, *image);

	if (err == KErrNone)
		{
		aHandleRet = reinterpret_cast<TInt>(imageSource);			
		}
	
	return err;
	}
	
/**
@see CDirectGdiDriver::CreateDrawableSource()

Destroys the DirectGDI adaptation-specific resource associated with this handle.
Calling this method on a handle that is not associated with any DirectGDI specific 
resource will do nothing. Once Close() is called, this handle can be reused. The handle
is set to KNullHandle.

@param 	aHandle A reference to a valid handle to a RDirectGdiDrawableSource object which is
        to be closed.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post 	The DirectGDI adaptation-specific resource associated with this handle will
be destroyed (at any time preferred by the adaptation). This handle is no longer 
associated with a DirectGDI adaptation-specific resource. The reference counter of 
the underlying drawable resource is decremented. aHandle is set to KNullHandle.

@panic DGDIAdapter 37, if an image source cannot be created from the passed handle.
*/
void CDirectGdiDriverImpl::CloseDrawableSource(TInt& aHandle)
	{	
	CDirectGdiImageSourceImpl* imageSource = GetImageSourceFromHandle(aHandle);
	GRAPHICS_ASSERT_ALWAYS(imageSource, EDirectGdiPanicCloseDrawableHandleFailure);		
	
	// Decrement the reference count on this image source. If it reaches zero, it will
	// remove itself from the source array and delete itself.
	imageSource->Close();
	aHandle = KNullHandle;
	}
	

/**
@see CDirectGdiDriver::CreateImageTarget()

Allocates and constructs a CDirectGdiImageTargetImpl object, and associates it with aHandleRet.

@param  aHandleRet Handle of newly created Image Target, must be KNullHandle (is checked by generic driver).
@param aSgImage The RSgImage to associate with the created image target. An attempt is made to match 
the pixel format of the new EGL surface with the pixel format of this RSgImage.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
The image resource has been fully constructed and created with the correct usage 
that allows it to be used as a DirectGDI target. aHandleRet is KNullHandle.

@post 	The DirectGDI adaptation-specific resource that is bound to the given image 
resource is created and this handle is now associated with it. The reference counter
on the image resource is incremented. 

@return KErrNone if successful, KErrArgument if the image resource is not valid, 
KErrNotSupported if the image resource is not created with the correct 
usage, otherwise one of the system-wide error codes.
*/
TInt CDirectGdiDriverImpl::CreateImageTarget(TInt& aHandleRet, const RSgImage& aSgImage)
	{		
	CDirectGdiImageTargetImpl* imageTarget = NULL;
	TInt err = CDirectGdiImageTargetImpl::New(imageTarget, *this, aSgImage, iPixelContextMap, iPixelConfigMap, iSharedContext);
		
	if (err == KErrNone)
		{
		// Return the handle to the newly created image target.
		aHandleRet = reinterpret_cast<TInt>(imageTarget);					
		}

	return err;	
	}

/**
Destroys the DirectGDI adaptation-specific resource associated with this handle. 
Calling this method on a handle that is not associated with any DirectGDI adaptation-
specific resource will do nothing. Once Close() is called, this handle can be reused.

@param 	aHandle A handle to a RDirectGdiImageTarget object to be closed.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.

@post 	The DirectGDI specific resource associated with this handle will be destroyed
if the reference count falls to zero. This handle is no longer associated with 
a DirectGDI specific resource. The reference counter of the underlying image 
resource is decremented. aHandle is set to KNullHandle.

@panic DGDIAdapter 37, if the image source associated with aHandle is NULL.
*/
void CDirectGdiDriverImpl::CloseImageTarget(TInt& aHandle)
	{	
	CDirectGdiImageTargetImpl* imageTarget = GetImageTargetFromHandle(aHandle);
	GRAPHICS_ASSERT_ALWAYS(imageTarget, EDirectGdiPanicCloseDrawableHandleFailure);	
	
	// Decrement the reference count on this image target. If the reference count reaches zero, 
	// it is deleted and removed from the targets array.
	imageTarget->Close();
	aHandle = KNullHandle;
	}

/**
Instantiates an adaptation-specific rendering engine.

@param	aDirectGdiEngine Reference to instantiated engine.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.

@return	KErrNone if the engine is constructed, else KErrNoMemory.
*/
TInt CDirectGdiDriverImpl::CreateEngine(MDirectGdiEngine*& aDirectGdiEngine)
	{
	TInt result = KErrNone;
	aDirectGdiEngine = new CVgEngine(*this);
	if (aDirectGdiEngine == NULL)
		{
		result = KErrNoMemory;
		}
	return result;
	}

/**
Unbinds target from rendering engine and marks it for deletion.
NOTE: Currently this means just deleting the engine.

@param	aDirectGdiEngine Engine to operate on.

@pre CDirectGdiDriver object has been initialised from the calling thread.

@panic DGDIAdapter 38, if the engine is NULL.
*/
void CDirectGdiDriverImpl::DestroyEngine(MDirectGdiEngine* aDirectGdiEngine)
	{
	GRAPHICS_ASSERT_ALWAYS(aDirectGdiEngine, EDirectGdiPanicDestroyNullEngine);	
	CVgEngine* actualEngine = static_cast<CVgEngine*>(aDirectGdiEngine);
	delete actualEngine;
	}

/**
The only extension interface supported by this implementation is MDirectGdiDriverCacheSize.

@see CDirectGdiDriver::GetInterface()
 */
TInt CDirectGdiDriverImpl::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt err = KErrNone;
	switch (aInterfaceId.iUid)
		{
		case KDirectGdiDriverCacheSizeUid:
			{
			aInterface = static_cast<MDirectGdiDriverCacheSize*>(this);
			break;	
			}		
		default:
			err = KErrExtensionNotSupported;
			break;
		}
	return err;
	}

CDirectGdiDriverImpl::CDirectGdiDriverImpl(RLibrary aLibrary)
	: CDirectGdiDriverInternal(aLibrary),
	iDisplay(EGL_NO_DISPLAY)
	{
	}
/**
Performs the second phase of construction.

@pre CDirectGdiDriver object has been initialised from the calling thread.
@post EGL has been initialised. SgDriver has been opened.

@panic DGDIAdapter 36, if eglGetDisplay() fails.
@panic DGDIAdapter 35, if eglInitialize() fails.
@panic DGDIAdapter 23, if eglBindAPI() fails.
@return KErrNone if successful, KErrNotFound if no implementation of the Graphics Resource API is found,
        KErrNotSupported if the version of EGL is less than required (1.2), otherwise one of the 
        system-wide error codes.
*/
TInt CDirectGdiDriverImpl::Construct()
	{
	TInt err = KErrNone;
	iSharedContext = EGL_NO_CONTEXT;
	
	// First thing is to check whether the mutex for the Process State was successfully created.
	// If not, it's a fatal error. If it is, we must increment the reference count on the mutex as 
	// early as possible to make the creation/destruction of the display thread-safe. 
	err = ProcessState().MutexCreationStatus();
	if (err == KErrNone)
		{
		ProcessState().OpenDriver();
		}

	if (err == KErrNone)
		{
		err = SgDriver::Open();
		}

	if (err == KErrNone)
		{
		// Create display object
		iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		GRAPHICS_ASSERT_ALWAYS(iDisplay != EGL_NO_DISPLAY, EDirectGdiPanicNoDisplay);			
	
		// Initialise the display.  If it is already initialised, EGL will return quickly.
		EGLint major, minor;
		if (!eglInitialize(iDisplay, &major, &minor))
			{
			iDisplay = EGL_NO_DISPLAY;
			HANDLE_EGL_ERROR(err, EDirectGdiPanicInitializeDisplay);
			}
		else if (major < 1 || (major == 1 && minor < 2))
			{
			// Atleast EGL 1.2 needs to be available.
			err = KErrNotSupported;
			}
		}

	if (err == KErrNone)
		{
		// Let EGL know we will use it for OpenVG
		if(!eglBindAPI(EGL_OPENVG_API))
			{
			HANDLE_EGL_ERROR(err, EDirectGdiPanicBindApi);
			}
		}

	if (err == KErrNone)
		{
		iGlyphImageStorage = new CFontGlyphImageStorage(KDirectGdiAdapterDefaultMaxGlyphImageCacheSize);
		if(!iGlyphImageStorage)
			{
			err = KErrNoMemory;
			}
		}

	if (err == KErrNone)
		{
		iVgImageCache = new CVgImageCache(KDirectGdiAdapterDefaultMaxImageCacheSize);
		if(!iVgImageCache)
			{
			err = KErrNoMemory;
			}
		}

	if (err == KErrNone)
		{
		err = ProcessState().Initialize(iDisplay);
		}

	if (err == KErrNone)
		{
		err = InitializeEglConfigs(iDisplay);
		}
	
	return err;
	}

/**
@see CDirectGdiDriver::SetError()

In the event that OpenVG's error state has been set, it will be used as the driver's error state,
overriding aErr.  

@param aErr The error code to be set.
@pre  CDirectGdiDriver object has been initialised from the calling thread.
@post The error state is set to the current OpenVG error state, otherwise aErr, if the current error state
      is empty.
*/
void CDirectGdiDriverImpl::SetError(TInt aErr)
	{
	if(	iErrorCode == KErrNone )
		{
		iErrorCode = GetVgError();
		if (iErrorCode == KErrNone)
			{
			iErrorCode = aErr;
			}
		}
	}

/**
Static function. Converts the current OpenVG error state (if any) to a standard Symbian error code.

@panic DGDIAdapter 64, if the error code received from OpenVG is due to misuse of OpenVG rather
       than a runtime error.
@return KErrGeneral if the OpenVG error code not recognised, otherwise a system-wide error 
code converted from the OpenVG error code.
*/
TInt CDirectGdiDriverImpl::GetVgError()
	{
	TInt error = KErrGeneral;
	switch(vgGetError())
		{
		case VG_NO_ERROR:
			error = KErrNone;
			break;
		case VG_BAD_HANDLE_ERROR:
			error = KErrBadHandle;
			break;
		case VG_ILLEGAL_ARGUMENT_ERROR:
			error = KErrArgument;
			break;
		case VG_OUT_OF_MEMORY_ERROR:
			error = KErrNoMemory;
			break;
		default:
			// Any other error is due to DirectGDI using OpenVG incorrectly somewhere.
			GRAPHICS_PANIC_DEBUG(EDirectGdiPanicVgError);
		}	
	return error;
	}

/**
Sets the current engine as the most recent engine which has been activated.

@param 	aCurrentEngine The pointer to the most recent engine.
*/
void CDirectGdiDriverImpl::SetCurrentEngine(CVgEngine* aCurrentEngine)
	{
	iCurrentEngine = aCurrentEngine;
	}

/**
Checks whether the given engine is current or not, so that the OpenVG state can be reset.

@param 	aCurrentEngine The pointer to an engine.
@return ETrue if it is the current engine, else EFalse.
*/
TBool CDirectGdiDriverImpl::IsCurrentEngine(const CVgEngine* aCurrentEngine) const
	{
	return (iCurrentEngine == aCurrentEngine);
	}

/**
Checks whether this target is current or not, so that the EGL state can be reset.

@param 	aCurrentTarget The pointer to a target.
@return ETrue if it is the current target, else EFalse.
*/
TBool CDirectGdiDriverImpl::IsCurrentTarget(const CDirectGdiImageTargetImpl* aCurrentTarget) const
	{
	return (iCurrentTarget == aCurrentTarget);
	}

/**
Sets the current target as the most recent target which has been activated.

@param 	aCurrentTarget The pointer to the most recent target.
*/
void CDirectGdiDriverImpl::SetCurrentTarget(CDirectGdiImageTargetImpl* aCurrentTarget)
	{
	iCurrentTarget = aCurrentTarget;
	}

/**
Activates the specified target and increments its reference count as it can be shared across many 
DirectGDI contexts.

@param aRenderingTarget The target object which is to be activated.
*/
void CDirectGdiDriverImpl::Activate(CDirectGdiImageTargetImpl* aRenderingTarget)
	{
	// Reactivate() does the same activation
	Reactivate(aRenderingTarget);
	aRenderingTarget->Open();
	}

/**
Reactivates the specified target, without affecting its reference count.

@param aRenderingTarget The target to be reactivated.

@panic DGDIAdapter 2, if eglGetError() returns an error we can't handle.
*/
void CDirectGdiDriverImpl::Reactivate(CDirectGdiImageTargetImpl* aRenderingTarget)
	{
	if(!aRenderingTarget->Activate())
		{
		TInt error = KErrNone;
		HANDLE_EGL_ERROR(error, EDirectGdiPanicUnexpectedError);
		if(error == KErrNoMemory)
			{
			SetError(error);
			}
		// Can check here whether error == KErrDied, which signifies an EGL_CONTEXT_LOST error.
		// due to a power management event.  Then need to destroy and recreate all EGLContexts used.
		// This is not implemented as the EGL used with this reference adaptation does not produce this error.
		// Following pseudocode will achieve this:
		// 1. For each context stored in iPixelMapContext:
		// 1a. Destroy context.
		// 1b. Create replacement context with same parameters.
		// 1c. Search through all rendering targets replacing the context stored (if it matches with the one just destroyed) with the new one.
		// 2. Try activating the rendering target again.
		}
	}

/**
Deactivates the target if it was the current target, and decrements its reference count as it can 
be shared across many DirectGDI contexts.

@param 	aRenderingTarget The target object which you want to deactivate.
*/
void CDirectGdiDriverImpl::Deactivate(CDirectGdiImageTargetImpl* aRenderingTarget)
	{			
	aRenderingTarget->Close();
	}

/**
Helper function for removing a pointer to a CDirectGdiImageRef object from the source image array.
@param aImage A pointer to a CDirectGdiImageRef source object.
@return KErrNone if successful, KErrNotFound if no suitable object pointer can be found, 
otherwise one of the other system wide error codes.
 */
TInt CDirectGdiDriverImpl::UnregisterSourceImage(const CDirectGdiImageSourceImpl& aImage)
	{
	TInt index = -1;
	TInt err = iSourceArray.FindInAddressOrder(&aImage, index);
	if (err == KErrNone)
		{
		iSourceArray.Remove(index);		
		iSourceArray.GranularCompress();
		}
	
	return err;
	}

/**
Helper function for removing a pointer to a CDirectGdiImageRef object from the target image array.
@param aImage A pointer to a CDirectGdiImageRef source object.
@return KErrNone if successful, KErrNotFound if no suitable object pointer can be found, 
otherwise one of the other system wide error codes.
 */
TInt CDirectGdiDriverImpl::UnregisterTargetImage(const CDirectGdiImageTargetImpl& aImage)
	{
	TInt index = -1;
	TInt err = iTargetArray.FindInAddressOrder(&aImage, index);
	if (err == KErrNone)
		{
		iTargetArray.Remove(index);		
		iTargetArray.GranularCompress();
		}	
	
	return err;
	}

/** 
Helper function to convert a handle for a RDirectGdiDrawableSource object to a CDirectGdiImageSourceImpl 
object. In debug builds, it also checks that the image source was actually created by this driver and 
exists in the array of image sources.

@param aHandle A valid handle to a RDirectGdiDrawableSource object.
@return A pointer to a CDirectGdiImageSourceImpl object that the handle represents.
*/
CDirectGdiImageSourceImpl* CDirectGdiDriverImpl::GetImageSourceFromHandle(TInt aHandle) const
	{
#ifdef _DEBUG
	CheckSourceIsValid(aHandle);
#endif // _DEBUG
	
	CDirectGdiImageSourceImpl* source = reinterpret_cast<CDirectGdiImageSourceImpl*>(aHandle);
	return source;
	}

/** 
Helper function to convert a handle for a RDirectGdiImageTarget object to a CDirectGdiImageTargetImpl 
object. In debug builds, it also checks that the image target was actually created by this driver and 
exists in the array of image target.

@param aHandle A valid handle to a RDirectGdiImageTarget object.
@return A pointer to a CDirectGdiImageTargetImpl object that the handle represents.
*/
CDirectGdiImageTargetImpl* CDirectGdiDriverImpl::GetImageTargetFromHandle(TInt aHandle) const
	{
#ifdef _DEBUG
	CheckTargetIsValid(aHandle);
#endif // _DEBUG
	
	CDirectGdiImageTargetImpl* target = reinterpret_cast<CDirectGdiImageTargetImpl*>(aHandle);
	return target;
	}

/**
Allows the engine to check whether a resource is an image source or not, given its handle. 
If the image is an image source, it will be in the array of image sources.

@param aHandle The handle to check for being an image source.
@return ETrue if the passed handle is for an image source, EFalse otherwise.
 */
TBool CDirectGdiDriverImpl::IsImageSource(TInt aHandle) const
	{
	CDirectGdiImageRef* source = reinterpret_cast<CDirectGdiImageRef*>(aHandle);
	TInt index = -1;	
	return (iSourceArray.FindInAddressOrder(source, index) == KErrNone);
	}

#ifdef _DEBUG
/**
Debug-only helper function to check that a source object has been created by this driver.
If it has been created by this driver it will exist in the source array.

@panic DGDIAdapter 32, if the source can not be found in the source array.
*/
void CDirectGdiDriverImpl::CheckSourceIsValid(TInt aHandle) const
	{
	// Debug only check to make sure the handle exists in the array of source objects 	
	// that have been created by this driver
	CDirectGdiImageRef* source = reinterpret_cast<CDirectGdiImageRef*>(aHandle);
	TInt index = -1;	
	TInt err = iSourceArray.FindInAddressOrder(source, index);
	GRAPHICS_ASSERT_ALWAYS(err == KErrNone, EDirectGdiPanicResourceHandleNotFound);
	}

/**
Debug-only helper function to check that a target object has been created by this driver.
If it has been created by this driver it will exist in the target array.

@panic DGDIAdapter 32, if the target is not found in the target array.
 */
void CDirectGdiDriverImpl::CheckTargetIsValid(TInt aHandle) const
	{
	// Debug only check to make sure the handle exists in the array of target objects 	
	// that have been created by this driver
	CDirectGdiImageRef* target = reinterpret_cast<CDirectGdiImageRef*>(aHandle);
	TInt index = -1;	
	TInt err = iTargetArray.FindInAddressOrder(target, index);
	GRAPHICS_ASSERT_ALWAYS(err == KErrNone, EDirectGdiPanicResourceHandleNotFound);
	}
#endif // _DEBUG

// VGImage cache APIs

/**
@see CVgImageCache::GetVgImageFromCache()
 */
VGImage CDirectGdiDriverImpl::GetVgImageFromCache(const CFbsBitmap& aBitmap, const TPoint& aOrigin) const
	{
	return iVgImageCache->GetVgImageFromBitmap(aBitmap, aOrigin);
	}

/**
@see CVgImageCache::AddImage()
 */
TBool CDirectGdiDriverImpl::AddVgImageToCache(const CFbsBitmap& aBitmap, VGImage& aImage, const TPoint& aOrigin)
	{
	return iVgImageCache->AddImage(aBitmap, aImage, aOrigin);
	}

/**
Gets the VGImage cache.
@return The VGImage cache.
*/
CVgImageCache* CDirectGdiDriverImpl::VgImageCache() const
	{
	return iVgImageCache;
	}

/**
Gets the Glyph image storage.
@return The Glyph image storage.
*/
CFontGlyphImageStorage* CDirectGdiDriverImpl::FontGlyphImageStorage() const
	{
	return iGlyphImageStorage;
	}

/**
Creates VG images which will be used to draw glyphs if the system runs out of memory
*/
TInt CDirectGdiDriverImpl::PreAllocateFontGlyphImages()
	{
	TInt res = KErrNotReady;
	if(iGlyphImageStorage)
		{
		res = iGlyphImageStorage->PreAllocateImages();
		if(res == KErrNoMemory)
			{//try to clean bitmap cache
			iVgImageCache->ResetCache();
			res = iGlyphImageStorage->PreAllocateImages();
			if(res == KErrNoMemory)
				{//try to clean glyph image cache
				iGlyphImageStorage->CleanGlyphImageCache();
				res = iGlyphImageStorage->PreAllocateImages();
				}
			}
		}
	return res;	
	}

/** 
Registers the image source with this driver, by adding it to a list of internal image sources.
@param aImage The image to register.
@return KErrNone if successful, otherwise on of the system-wide error codes.
*/ 
TInt CDirectGdiDriverImpl::RegisterSourceImage(const CDirectGdiImageSourceImpl& aImage)
	{
	return iSourceArray.InsertInAddressOrder(&aImage);
	}

/** 
Registers the image target with this driver, by adding it to a list of internal image targets.
@param aImage The image to register.
@return KErrNone if successful, otherwise on of the system-wide error codes.
*/ 
TInt CDirectGdiDriverImpl::RegisterTargetImage(const CDirectGdiImageTargetImpl& aImage)
	{
	return iTargetArray.InsertInAddressOrder(&aImage);
	}

/** 
@see MDirectGdiDriverCacheSize::SetMaxImageCacheSize()
 */
TInt CDirectGdiDriverImpl::SetMaxImageCacheSize(TInt aSize)
	{
	return iVgImageCache->SetMaxCacheSize(aSize);
	}

/** 
@see MDirectGdiDriverCacheSize::MaxImageCacheSize()
 */
TInt CDirectGdiDriverImpl::MaxImageCacheSize() const
	{
	return iVgImageCache->MaxCacheSize();
	}

/** 
@see MDirectGdiDriverCacheSize::SetMaxGlyphCacheSize()
 */
TInt CDirectGdiDriverImpl::SetMaxGlyphCacheSize(TInt aSize)
	{
	return iGlyphImageStorage->SetMaxGlyphCacheSize(aSize);
	}

/** 
@see MDirectGdiDriverCacheSize::MaxGlyphCacheSize()
 */
TInt CDirectGdiDriverImpl::MaxGlyphCacheSize() const
	{
	return iGlyphImageStorage->MaxGlyphCacheSize();
	}
