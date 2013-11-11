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
// WINS platform
// 
//

#include <hal.h>
#include "SCDRAW.H"
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
	TUint depths = 0;
	// As multiple screens are possible, the (optional) second parameter is necessary here
	// to find the color depth specific to this screen.
	EmulatorColorDepth(depths, aScreenNo); 
	ASSERT(depths);

	CFbsDrawDevice* drawDevice = NULL;

	switch(aDispMode)
		{
	case EGray2:
		if (!(depths & KEmulGray2))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawOneBppScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawOneBppScreenBitmap*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EGray4:
		if (!(depths & KEmulGray4))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawTwoBppScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawTwoBppScreenBitmap*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EGray16:
		if (!(depths & KEmulGray16))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawFourBppScreenBitmapGray;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawFourBppScreenBitmapGray*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EGray256:
		if (!(depths & KEmulGray256))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawEightBppScreenBitmapGray;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawEightBppScreenBitmapGray*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor16:
		if (!(depths & KEmulColor16))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawFourBppScreenBitmapColor;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawFourBppScreenBitmapColor*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor256:
		if (!(depths & KEmulColor256))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawEightBppScreenBitmapColor;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawEightBppScreenBitmapColor*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor4K:
		if (!(depths & KEmulColor4K))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawTwelveBppScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawTwelveBppScreenBitmap*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor64K:
		if (!(depths & KEmulColor64K))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawSixteenBppScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawSixteenBppScreenBitmap*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor16M:
		if (!(depths & KEmulColor16M))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawTwentyFourBppScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawTwentyFourBppScreenBitmap*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor16MU:
		if (!(depths & KEmulColor16M))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawUTwentyFourBppScreenBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawUTwentyFourBppScreenBitmap*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor16MA:
		if (!(depths & KEmulColor16M))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawThirtyTwoBppScreenBitmapAlpha;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawThirtyTwoBppScreenBitmapAlpha*> (drawDevice)) ->ConstructScreen(aScreenNo,
																			aScreenInfo.iAddress,
																			aScreenInfo.iSize));
		break;
	case EColor16MAP:
		if (!(depths & KEmulColor16M))
			User::Leave(KErrNotSupported);
		drawDevice = new(ELeave) CDrawThirtyTwoBppScreenBitmapAlphaPM;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError((static_cast<CDrawThirtyTwoBppScreenBitmapAlphaPM*> (drawDevice)) ->ConstructScreen(aScreenNo,
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
	__ASSERT_ALWAYS(aInfo.iScreenAddressValid && aInfo.iScreenAddress, Panic(EScreenDriverPanicInvalidWindowHandle));
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
	return EColor16MU;
	}
