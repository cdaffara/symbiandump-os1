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

#include "swdirectgdiimagesourceimpl.h"
#include "swdirectgdidriverimpl.h"
#include <graphics/sgimage_sw.h>

/**
Constructs a CSwDirectGdiImageTargetImpl
@param	aDriver		The driver implementation which created this target.
*/
CSwDirectGdiImageSourceImpl::CSwDirectGdiImageSourceImpl(CSwDirectGdiDriverImpl& aDriver) :
	iDriver(aDriver)
	{	
	}

/**
Destroys a CSwDirectGdiImageSourceImpl.

@post This image source is removed from the associated driver's list of source images.
*/
CSwDirectGdiImageSourceImpl::~CSwDirectGdiImageSourceImpl()
	{
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
TInt CSwDirectGdiImageSourceImpl::New(CSwDirectGdiImageSourceImpl*& aImage, CSwDirectGdiDriverImpl& aDriver, const RSgImage& aSgImage)
	{
	CSwDirectGdiImageSourceImpl* image = new CSwDirectGdiImageSourceImpl(aDriver);
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
@param aSgImage The RSgImage to create the source image from.
@return KErrNone if successful, KErrNotSupported if the RSgImage doesn't have the required interface,
        otherwise an error from CDirectGdiImageRef::Construct().
*/ 
TInt CSwDirectGdiImageSourceImpl::Construct(const RSgImage& aSgImage)
	{	
	TInt err = CDirectGdiImageRef::Construct(aSgImage);
	
	MSgImage_Sw* pImage = NULL;
	if (err == KErrNone)
		{		
		RSgImage image = Image();
		err = image.GetInterface(pImage);
		}
	
	if (err == KErrNone)
		{
		err = iDriver.RegisterSourceImage(*this);
		}
	
	if (err == KErrNone)
		{
		iDataBuffer = pImage->DataAddress();
		iStride = pImage->DataStride();
		Open();
		}
	
	return err;	
	}
