// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ACCELSOFT.CPP
// 
//

#include <graphicsaccelerator.h>
#include <fbs.h>

/**
A wrapper around the screendriver implementation of a factory for creating 2D 
graphics accelerator objects whose graphics operations are implemented in software.

Do not use, link against scdv.lib, here for BC.

@internalComponent
@see CSoftwareGraphicsAccelerator
*/
class CSoftwareGraphicsAcceleratorBC : public CGraphicsAccelerator
	{
public:
	// Create a new CSoftwareGraphicsAccelerator for use with a given bitmap
	IMPORT_C static CSoftwareGraphicsAccelerator* NewL(CFbsBitmap* aBitmap);

	// Get the non-bitmap-specific capabilities of the hardware accelerator.
	IMPORT_C static const TGraphicsAcceleratorCaps* GenericCapabilities();
public:
	// From CGraphicsAccelerator
	virtual const TGraphicsAcceleratorCaps* Capabilities() = 0;
	virtual TInt Operation(const TGraphicsOperation& aOperation) = 0;
	virtual TInt Operation(const TGraphicsOperation& aOperation,TInt aNumClipRects,TRect* aClipRects) = 0;
	virtual TInt Operation(TDes8& aBuffer) = 0;
	virtual TInt Operation(TDes8& aBuffer,TInt aNumClipRects,TRect* aClipRects) = 0;
	// From CGraphicsAccelerator
	virtual void Reserved_1() = 0;
	virtual void Reserved_2() = 0;
	virtual void Reserved_3() = 0;
	virtual void Reserved_4() = 0;
	};

/** 
Allocates and constructs an instance of a derived class and initialises its 
capabilities.

Do not use, link against scdv.lib, here for BC. 

@param aBitmap The bitmap for the accelerator to draw to. This may be any 
type of bitmap, including a hardware bitmap.
@return Pointer to the initialised graphics accelerator object.
@see CSoftwareGraphicsAccelerator::NewL
*/
EXPORT_C CSoftwareGraphicsAccelerator* CSoftwareGraphicsAcceleratorBC::NewL(CFbsBitmap* aBitmap)
	{
	return CSoftwareGraphicsAccelerator::NewL(aBitmap);
	}

/** 
Gets the generic capabilities of the accelerator, including which display modes 
are supported for the bitmap passed to NewL().

Do not use, link against scdv.lib, here for BC. 

@return Generic capabilities for software graphics accelerators. 
@see CSoftwareGraphicsAccelerator::GenericCapabilities
*/
EXPORT_C const TGraphicsAcceleratorCaps* CSoftwareGraphicsAcceleratorBC::GenericCapabilities()
	{
	return CSoftwareGraphicsAccelerator::GenericCapabilities();
	}

