// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MCOT platform
// 
//

#include <hal.h>
#include "scdraw.h"
#include "ScreenInfo.h"
#include <graphics/gdi/gdiconsts.h>

/**
Creates an instance of CFbsDrawDevice class.
@param aScreenNo Screen number
@param aDispMode Display mode
@param aScreenInfo Screen parameters: video memory address and screen size
@return A pointer to the created CFbsDrawDevice object
@leave System-wide error code including KErrNoMemory
@internalComponent
*/
static CFbsDrawDevice* CreateInstanceL(TInt aScreenNo,
									   TDisplayMode aDispMode,
									   const TScreenInfo& aScreenInfo)
	{
	CFbsDrawDevice* drawDevice = NULL;

	switch (aDispMode)
		{
	case SCREEN_LAYOUT_DISPLAY_MODE:
		drawDevice = new(ELeave) CDrawScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawScreenBitmap*)drawDevice)->ConstructScreen(aScreenNo,
																aScreenInfo.iAddress, 
																aScreenInfo.iSize));
		break;
	default:
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(); // drawDevice
	return drawDevice;
	}

/**
@deprecated Use NewScreenDeviceL(TInt aScreenNo, TDisplayMode aDispMode)
*/
EXPORT_C CFbsDrawDevice* CFbsDrawDevice::NewScreenDeviceL(TScreenInfoV01 aInfo,
														  TDisplayMode aDispMode)
	{
	TScreenInfo screenInfo(aInfo.iScreenAddress, aInfo.iScreenSize);
	return ::CreateInstanceL(KDefaultScreenNo, aDispMode, screenInfo);
	}

/**
Creates a new screen device instance, which implements CFbsDrawDevice interface.
The method has to be implemented for each type of supported video hardware.
@param aScreenNo Screen number
@param aDispMode Requested display mode
@return A pointer to just created screen device, which implements CFbsDrawDevice interface
@leave KErrNoMemory Not enough memory
	   KErrNotSupported The requested screen device type is not supported
*/
EXPORT_C CFbsDrawDevice* CFbsDrawDevice::NewScreenDeviceL(TInt aScreenNo,
														  TDisplayMode aDispMode)
	{
	TInt address = 0, width = 0, height = 0;
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayMemoryAddress, address));
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayXPixels, width));
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayYPixels, height));
	__ASSERT_ALWAYS(width > 0 && height > 0 && address != 0, Panic(EScreenDriverPanicInvalidHalValue));
	TScreenInfo screenInfo(reinterpret_cast <TAny*> (address), TSize(width, height));
	return ::CreateInstanceL(aScreenNo, aDispMode, screenInfo);
	}

/**
Depending on the current graphics hardware this 
will return one of the 16M video modes defined in
TDisplayMode, or ENone if a 16M video mode is not supported.
@see TDisplayMode
@return	a 16M display mode or ENone.
*/
EXPORT_C TDisplayMode CFbsDrawDevice::DisplayMode16M()
	{
	return ENone;
	}
