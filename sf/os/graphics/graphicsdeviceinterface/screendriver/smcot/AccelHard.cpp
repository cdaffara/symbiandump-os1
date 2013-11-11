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

#include "GraphicsAccelerator.h"

LOCAL_D const TGraphicsAcceleratorCaps HardwareAcceleratorCapabilities =
	{
	sizeof(TGraphicsAcceleratorCaps),	//TInt iStructureSize;	// The size of this class
	1,									//TInt iVersion;		// == 1 to specify current API
	{0}									//Tint iVendorUid;		// Optional ID
	// Default values of zero...
	};

EXPORT_C CHardwareGraphicsAccelerator* CHardwareGraphicsAccelerator::NewL(RHardwareBitmap /*aBitmap*/)
/** 
Allocates and constructs an instance of a derived class and initialises its 
capabilities.

@param aBitmap The hardware bitmap for the accelerator to draw to.
@return Pointer to the initialised graphics accelerator object.
@see TGraphicsAcceleratorCaps::iDisplayModes 
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C const TGraphicsAcceleratorCaps* CHardwareGraphicsAccelerator::GenericCapabilities()
/** 
Gets the generic capabilities of the accelerator, including which display modes 
are supported for the bitmap passed to NewL().

Generic capabilities apply to all hardware graphics accelerators on the device. 
The function is static, so it can be used to find out the capabilities of 
graphics accelerators before deciding on whether or not to create one.

The CGraphicsAccelerator::Capabilities() function provides information about 
the capabilities of a particular graphics accelerator.

As capabilities may vary depending on the display mode of a bitmap, this method 
should indicate as supported any features which are only available in some 
display modes.

@return Generic capabilities for hardware graphics accelerators. 
*/
	{
	return &HardwareAcceleratorCapabilities;
	}

