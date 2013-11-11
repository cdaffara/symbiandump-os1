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

#include "swdirectgdiimagetargetimpl.h"
#include "swdirectgdidriverimpl.h"
#include <bitdraw.h>

/**
Constructs a CSwDirectGdiImageTargetImpl.
@param	aDriver		The driver implementation which created this target.
@param	aDrawDevice A pointer to the draw device associated with this target. 
				   This class takes ownership of aDrawDevice.
*/
CSwDirectGdiImageTargetImpl::CSwDirectGdiImageTargetImpl(CSwDirectGdiDriverImpl& aDriver, CFbsDrawDevice* aDrawDevice):
	iDriver(aDriver),
	iDrawDevice(aDrawDevice)	
	{	
	}

/**
Destroys a CSwDirectGdiImageTargetImpl.
@post This image target is removed from the associated driver's list of target images.
*/
CSwDirectGdiImageTargetImpl::~CSwDirectGdiImageTargetImpl()
	{
	iDriver.UnregisterTargetImage(*this);
	delete iDrawDevice;
	}

/**
Two-phase construction of a CSwDirectGdiImageTarget object.

@param aImage On success, holds a pointer to the newly-created image.
@param aDriver The driver to register this image with.
@param aDrawDevice The draw device to associate with this target. This target will take ownership
of the draw device if this method returns without an error, if the method returns with an error
then the caller still owns the draw device.
@param aSgImage The RSgImage which this image will be based upon.

@return KErrNone if successful, KErrNoMemory if not enough memory could be allocated, otherwise a return
        value from Construct().
*/
TInt CSwDirectGdiImageTargetImpl::New(CSwDirectGdiImageTargetImpl*& aImage, CSwDirectGdiDriverImpl& aDriver, CFbsDrawDevice* aDrawDevice, const RSgImage& aSgImage)
	{
	CSwDirectGdiImageTargetImpl* image = new CSwDirectGdiImageTargetImpl(aDriver, aDrawDevice);
	if (!image)
		return KErrNoMemory;
	TInt err = image->Construct(aSgImage);
	if (err == KErrNone)
		aImage = image;
	else
		{
		image->iDrawDevice = NULL;
		delete image;
		}
	return err;
	}

/**
Obtains a draw device pointer that is associated with this target
@return A pointer to the draw device.
*/
CFbsDrawDevice* CSwDirectGdiImageTargetImpl::DrawDevice() const
	{
	return iDrawDevice;
	}

/**
Gets the supplied image structure and sets the internal data. If it fails for any reason, it will
delete itself.

@param aSgImage The RSgImage to create the source image from.
@return KErrNone if successful, KErrNotSupported if the RSgImage doesn't have the required interface,
        otherwise an error from CDirectGdiImageRef::Construct().
*/ 
TInt CSwDirectGdiImageTargetImpl::Construct(const RSgImage& aSgImage)
	{	
	TInt err = CDirectGdiImageRef::Construct(aSgImage);
	
	if (err == KErrNone)
		{		
		err = iDriver.RegisterTargetImage(*this);
		}
	
	if (err == KErrNone)
		{
		Open();
		}
	
	return err;	
	}
