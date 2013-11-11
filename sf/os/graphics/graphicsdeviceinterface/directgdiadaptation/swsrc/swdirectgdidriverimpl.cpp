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
#include "swdirectgdidriverimpl.h"
#include "swdirectgdiimagesourceimpl.h"
#include "swdirectgdiimagetargetimpl.h"
#include "pixelutil.h"
#include <e32cmn.h>
#include <pixelformats.h>
#include <graphics/sgimage_sw.h>

/**
@panic DGDIAdapter 6, if the reference count to DirectGDI driver is invalid.
@panic DGDIAdapter 45, if objects are still in the source or target image array.
*/
CSwDirectGdiDriverImpl::~CSwDirectGdiDriverImpl()
	{
	GRAPHICS_ASSERT_DEBUG(iEngines.Count() == 0, EDirectGdiPanicDriverInvalidRefCount);
	GRAPHICS_ASSERT_DEBUG(iTargets.Count() == 0, EDirectGdiPanicItemsLeftInImageArray);
	GRAPHICS_ASSERT_DEBUG(iImageSources.Count() == 0, EDirectGdiPanicItemsLeftInImageArray);
	
	// Delete all the engines owned by this driver
	iEngines.ResetAndDestroy();
	iTargets.ResetAndDestroy();
	iImageSources.ResetAndDestroy();
	}

/**
Constructor, which initialises base class.
*/
CSwDirectGdiDriverImpl::CSwDirectGdiDriverImpl(RLibrary aLibrary)
	: CDirectGdiDriverInternal(aLibrary)
	{
	}

/**
Empty function as this adaptation is synchronous.
@see CDirectGdiDriverInternal::Flush()
*/
void CSwDirectGdiDriverImpl::Flush()
	{
	}

/**
Empty function as this adaptation is synchronous.
@see CDirectGdiDriverInternal::Finish()
*/
void CSwDirectGdiDriverImpl::Finish()
	{
	}

/**
@see CDirectGdiDriver::GetError()
*/
TInt CSwDirectGdiDriverImpl::GetError()
	{
	TInt err = iErrorCode;
	iErrorCode = KErrNone;
	return err;
	}

/**
@see CDirectGdiDriverInternal::CloseDrawableSource()
@panic DGDIAdapter 57, if aHandleRet is not KNullHandle (debug only).
*/
TInt CSwDirectGdiDriverImpl::CreateDrawableSource(TInt& aHandleRet, const RSgDrawable& aSgDrawable)
	{
	TInt err = KErrNotSupported;
	const TUid typeUid = aSgDrawable.DrawableType(); 
	if(KSgImageTypeUid == typeUid)
		{
		CSwDirectGdiImageSourceImpl* imageSource = NULL;
		RSgImage* image =  (RSgImage*) (&aSgDrawable);
		err = CSwDirectGdiImageSourceImpl::New(imageSource, *this, *image);
	
		if (err == KErrNone)
			{
			aHandleRet = reinterpret_cast<TInt>(imageSource);
			}
		}
	return err;
	}
	
/**
@see CDirectGdiDriverInternal::CreateDrawableSource()
@panic DGDIAdapter 47, if the drawable source belonging to aHandle could not be found in the internal 
	   list of sources (debug only).
*/
void CSwDirectGdiDriverImpl::CloseDrawableSource(TInt& aHandle)
	{
	// Graphics Resource and DirectGDI reference implementation only support pixel based resource
	CSwDirectGdiImageSourceImpl* source = reinterpret_cast<CSwDirectGdiImageSourceImpl*>(aHandle);
	if (source)
		source->Close();
	aHandle = KNullHandle;
	}
	
/**
@see CDirectGdiDriverInternal::CreateImageTarget()
@panic DGDIAdapter 58, if aHandleRet is not KNullHandle (debug only).
@panic DGDIAdapter 1023, if the attempt to get an RSgImage interface MSgImage_Sw failed.
*/
TInt CSwDirectGdiDriverImpl::CreateImageTarget(TInt& aHandleRet, const RSgImage& aImage)
	{
	TSgImageInfo info;
	TInt ret = aImage.GetInfo(info);
	if (ret != KErrNone)
		{
		return ret;
		}
	TDisplayMode displayMode = PixelFormatUtil::ConvertToDisplayMode(info.iPixelFormat);
	TSize size = info.iSizeInPixels;
	CFbsDrawDevice* device = NULL;
	const MSgImage_Sw* pImage;
	TInt err = aImage.GetInterface(pImage);
	GRAPHICS_ASSERT_DEBUG((err == KErrNone) && (pImage != NULL), EDirectGdiPanicInvalidInterfaceHandle);
	TRAP(err, device = CFbsDrawDevice::NewBitmapDeviceL(size, displayMode, pImage->DataStride()));
	if (err != KErrNone)
		{		
		return err;
		}	
		
	// Setup the new device with the memory from the image
	device->SetBits(pImage->DataAddress());
		
	// Create a new image
	if (err == KErrNone)
		{
		CSwDirectGdiImageTargetImpl* target = NULL;
		err = CSwDirectGdiImageTargetImpl::New(target, *this, device, aImage);
		if (err == KErrNone)
			{
			// Make sure we aren't overwriting an exisiting valid handle.
			GRAPHICS_ASSERT_DEBUG(aHandleRet == KNullHandle, EDirectGdiPanicTargetHandleNotNull);
			aHandleRet = reinterpret_cast<TInt>(target);
			}		
		}
	
	if (err != KErrNone)
		{
		delete device;
		}
		
	return err;
	}
	
/**
@see CDirectGdiDriverInternal::CloseImageTarget()
@panic DGDIAdapter 40, if the image target associated with aHandle could not be found in the 
       internal list of targets (debug only).	
*/
void CSwDirectGdiDriverImpl::CloseImageTarget(TInt& aHandle)
	{
	CSwDirectGdiImageTargetImpl* target = reinterpret_cast<CSwDirectGdiImageTargetImpl*>(aHandle);
	if (target)
		target->Close();
	aHandle = KNullHandle;
	}
	
		
/**
@see CDirectGdiDriverInteral::CreateEngine()
*/
TInt CSwDirectGdiDriverImpl::CreateEngine(MDirectGdiEngine*& aEngine)
	{
	TInt err = KErrNoMemory; 
	CSwDirectGdiEngine* engine = new CSwDirectGdiEngine(this);
	aEngine = engine;	
	if(engine != NULL)	
		{		
		// Put this in an array so we can destroy it on close
		err = iEngines.InsertInAddressOrder(engine);
		if (KErrNone != err)
			{
			delete engine;
			aEngine = NULL;
			}
		}
	return err;
	}
		
/**
@see CDirectGdiDriverInternal::DestroyEngine()
@panic DGDIAdapter 60, if aEngine could not be found in the internal list of engines (debug only).
*/
void CSwDirectGdiDriverImpl::DestroyEngine(MDirectGdiEngine* aEngine)
	{
	TInt index = iEngines.FindInAddressOrder(aEngine);
	GRAPHICS_ASSERT_DEBUG(index != KErrNotFound, EDirectGdiPanicEngineNotFound);
	iEngines.Remove(index);
	delete (static_cast<CSwDirectGdiEngine*>(aEngine));
	}

/**
@see CDirectGdiDriverInternal::SetError()
*/
void CSwDirectGdiDriverImpl::SetError(TInt aErr)
	{
	if (aErr != KErrNone && iErrorCode == KErrNone)
		{
		iErrorCode = aErr;
		}
	}

/**
No extensions interfaces are supported.

@see CDirectGdiDriver::GetInterface()
 */
TInt CSwDirectGdiDriverImpl::GetInterface(TUid /*aInterfaceId*/, TAny*& aInterface)
	{
	aInterface = NULL;	
	return KErrExtensionNotSupported;			
	}

/**
Deactivate the target if it was the current target, decrement the reference count of the target as it can be shared 
across many DirectGDI contexts.

@param 	aRenderingTarget The target object which you want to deactivate.

@panic DGDIAdapter 55, if aRengeringTarget is NULL.
*/
void CSwDirectGdiDriverImpl::Deactivate(CSwDirectGdiImageTargetImpl* aRenderingTarget)
	{
	GRAPHICS_ASSERT_ALWAYS(aRenderingTarget, EDirectGdiPanicNullTargetDeactivate);
	aRenderingTarget->Close();
	}

/**
Checks whether the image source handle is valid. 

@param aHandle The handle to an Image Source that should be found.
@return ETrue if aHandle is a valid handle to an image registered with the driver, otherwise EFalse.
*/
TBool CSwDirectGdiDriverImpl::ValidImageSource(TInt aHandle) const
	{
	CSwDirectGdiImageSourceImpl* impl = reinterpret_cast<CSwDirectGdiImageSourceImpl*>(aHandle);
	return iImageSources.FindInAddressOrder(impl) != KErrNotFound;
	}

/**
Checks whether the target image handle is valid. 

@param aHandle The handle to an Image Target that should be found.
@return ETrue if aHandle is a valid handle to an image registered with the driver, otherwise EFalse.
*/
TBool CSwDirectGdiDriverImpl::ValidImageTarget(TInt aHandle) const
	{	
	CSwDirectGdiImageTargetImpl* impl = reinterpret_cast<CSwDirectGdiImageTargetImpl*>(aHandle);
	return iTargets.FindInAddressOrder(impl) != KErrNotFound;
	}

/**
Removes the specified image target from the internal list of targets.

@param aImageTarget The image to remove.
@return KErrNotFound if it could not be found in the internal list, otherwise KErrNone.
*/
TInt CSwDirectGdiDriverImpl::UnregisterTargetImage(const CSwDirectGdiImageTargetImpl& aImageTarget)
	{
	TInt err = KErrNone;
	TInt index = iTargets.FindInAddressOrder(&aImageTarget);
	if (index == KErrNotFound)
		{
		err = KErrNotFound;
		}
	else
		{
		iTargets.Remove(index);
		iTargets.GranularCompress();
		}
	return err;
	}

/**
Removes the specified image source from the internal list of sources.

@param aImageSource The image to remove.
@return KErrNotFound if it could not be found in the internal list, otherwise KErrNone.
*/
TInt CSwDirectGdiDriverImpl::UnregisterSourceImage(const CSwDirectGdiImageSourceImpl& aImageSource)
	{
	TInt err = KErrNone;
	TInt index = iImageSources.FindInAddressOrder(&aImageSource);
	if (index == KErrNotFound)
		{		
		err = KErrNotFound;
		}
	else
		{	
		iImageSources.Remove(index);
		iImageSources.GranularCompress();
		}
	return err;
	}

/**
Helper function. When an image source is constructed, it registers itself on the driver's internal 
list of image sources.
@param aImageSource The image to add to the array.
@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
TInt CSwDirectGdiDriverImpl::RegisterSourceImage(const CSwDirectGdiImageSourceImpl& aImageSource)
	{
	return iImageSources.InsertInAddressOrder(&aImageSource);
	}

/**
Helper function. When an image target is constructed, it registers itself on the driver's internal 
list of image targets.
@param aImageTarget The image to add to the array.
@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
TInt CSwDirectGdiDriverImpl::RegisterTargetImage(const CSwDirectGdiImageTargetImpl& aImageTarget)
	{
	return iTargets.InsertInAddressOrder(&aImageTarget);
	}
	
