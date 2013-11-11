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
#include "directgdiimagesourceimpl.h"
#include "directgdidriverimpl.h"
#include "directgdidriverprocessstate.h"
#include <graphics/sgimage.h>

/**
Constructs a CDirectGdiImageSourceImpl.
@param	aDriver		The driver implementation which created this target.
*/
CDirectGdiImageSourceImpl::CDirectGdiImageSourceImpl(CDirectGdiDriverImpl& aDriver) :
	iDriver(aDriver),
	iVgImage(VG_INVALID_HANDLE)
	{
	}

/**
Destructor. 

@post The associated VgImage (if any) is destroyed by the process state.
	  This image is removed from the associated driver's list of source images.
*/
CDirectGdiImageSourceImpl::~CDirectGdiImageSourceImpl()
{	
	GRAPHICS_LOG_DEBUG("Destroying CDirectGdiImageSourceImpl");
	if (iVgImage != VG_INVALID_HANDLE)
		{
		iDriver.ProcessState().DestroyVgImage(iDriver.EglDisplay(), iVgImage);
		}
	iDriver.UnregisterSourceImage(*this);		
}

/**
Two-phase construction of a CDirectGdiImageSourceImpl object.
@param aImage On success, holds a pointer to the newly-created image.
@param aDriver The driver to register this image with.
@param aSgImage The RSgImage which this image will be based upon.
@return KErrNone if successful, KErrNoMemory if not enough memory could be allocated, otherwise a return
        value from Construct().
*/
TInt CDirectGdiImageSourceImpl::New(CDirectGdiImageSourceImpl*& aImage, CDirectGdiDriverImpl& aDriver, const RSgImage& aSgImage)
	{
	CDirectGdiImageSourceImpl* image = new CDirectGdiImageSourceImpl(aDriver);
	if (!image)
		return KErrNoMemory;
	TInt err = image->Construct(aSgImage);
	if (err == KErrNone)
		aImage = image;
	else
		delete image;
	return err;
	}

/**
Gets the supplied image structure and sets the internal data.
Registers itself with the driver.
 
@param aSgImage The RSgImage to create the source image from.
@return KErrNone if successful, otherwise an error from CDirectGdiImageRef::Construct().
*/
TInt CDirectGdiImageSourceImpl::Construct(const RSgImage& aSgImage)
	{
	TInt err = CDirectGdiImageRef::Construct(aSgImage);
	
	if (err == KErrNone)
		{
		// Let the process state create the VGImage as EGLImages and their associated VGImages
		// are shared across all threads in this process. (Only one EGLImage can be created per RSgImage
		// in a process).
		err = iDriver.ProcessState().CreateVgImage(iDriver.EglDisplay(), iVgImage, iSgImage);
		}
	
	if (err == KErrNone)
		{
		err = iDriver.RegisterSourceImage(*this);				
		}
	
	if (err == KErrNone)
		{
		Open();
		}	
	
	return err;
	}

