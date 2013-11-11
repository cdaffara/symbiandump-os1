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
//

#include <graphicsaccelerator.h>
#include <fbs.h>

// Dummy implementation

LOCAL_D const TGraphicsAcceleratorCaps SoftwareAcceleratorCapabilities = 
	{
	sizeof(TGraphicsAcceleratorCaps),	//TInt iStructureSize;	// The size of this class
	1,									//TInt iVersion;		// == 1 to specify current API
	{0},									//Tint iVendorUid;		// Optional ID
	0,									//iDisplayModes;
	0,									//iClipping;
	TGraphicsAcceleratorCaps::EMaskBitmapNone,	//iMaskType;	
	0,									//iTransparency;
	0,									//iAlphaChannel;
	0,									//iAlphaBitmap;
	0,									//iPatternSizes;
	0,									//iPattern;
	0,									//iPolygon;
	{0,0,0,0}							//iReserved[0] is used for supported orientations, iReserved[1]-iReserved[3] are reserved;
	};

/** 
Allocates and constructs an instance of a derived class and initialises its 
capabilities.

@param aBitmap The bitmap for the accelerator to draw to. This may be any 
type of bitmap, including a hardware bitmap.
@return Pointer to the initialised graphics accelerator object.
@see TGraphicsAcceleratorCaps::iDisplayModes 
@leave KErrNoMemory There is no memory to allocate the graphics accelerator
       KErrNotSupported There is no graphics accelerator supporting the display mode given by the bitmap parameter
*/
EXPORT_C CSoftwareGraphicsAccelerator* CSoftwareGraphicsAccelerator::NewL(CFbsBitmap* /*aBitmap*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/** 
Gets the generic capabilities of the accelerator, including which display modes 
are supported for the bitmap passed to NewL().

Generic capabilities apply to all software graphics accelerators on the device. 
The function is static, so it can be used to find out the capabilities of 
graphics accelerators before deciding on whether or not to create one.

The CGraphicsAccelerator::Capabilities() function provides information about 
the capabilities of a particular graphics accelerator.

As capabilities may vary depending on the display mode of a bitmap, this method 
should indicate as supported any features which are only available in some 
display modes.

@return Generic capabilities for software graphics accelerators. 
*/
EXPORT_C const TGraphicsAcceleratorCaps* CSoftwareGraphicsAccelerator::GenericCapabilities()
	{
	return &SoftwareAcceleratorCapabilities;
	}

