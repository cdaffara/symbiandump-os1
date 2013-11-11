// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This module implements the functions that create the screen class depending
// on the screen type.
// Include files                                                   
// 
//

/**
 @file
*/
/********************************************************************/
#include <bitdraw.h>
#include "scdraw.h"
#include <hal.h>
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

	TInt bpp;
	if(HAL::Get(aScreenNo, HALData::EDisplayMode, bpp) != KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
	if(HAL::Get(aScreenNo, HALData::EDisplayBitsPerPixel, bpp) != KErrNone)
		{
		User::Leave(KErrNotSupported);
		}
   
    //Switch the display mode, call the construtor of the class defined
    //in the ScmonXX.cpp or SccolXX.cpp file.
	switch(aDispMode)
		{
	case EColor256: // 8 Bpp color mode
		if(bpp == 8)
			{
			drawDevice = new (ELeave) CDrawEightBppScreenBitmapColor;
			CleanupStack::PushL(drawDevice) ;
			User::LeaveIfError(((CDrawEightBppScreenBitmapColor*)drawDevice)->ConstructScreenL(
                                                            aScreenNo,
															aScreenInfo.iAddress, 
															aScreenInfo.iSize));
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		break;

	case EColor64K: // 16 Bpp color mode
		if(bpp == 16)
			{
			drawDevice = new (ELeave) CDrawSixteenBppScreenBitmap;
			CleanupStack::PushL(drawDevice);
			User::LeaveIfError(((CDrawSixteenBppScreenBitmap*)drawDevice)->ConstructScreenL(
                                                            aScreenNo,
															aScreenInfo.iAddress, 
															aScreenInfo.iSize));
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		break;
   
   	case EColor16MU: // 24 unpacked Bpp color mode
		if((bpp == 24 || bpp == 32) && aDispMode == CFbsDrawDevice::DisplayMode16M()) 	//there is some "ambiguity" about 24 and 32 bit modes... 
	//They are both byte per color component, and both actually have 32 bits per pixel memory use.
			{
			drawDevice = new (ELeave) CDrawUTwentyFourBppScreenBitmap;
			CleanupStack::PushL(drawDevice);
			User::LeaveIfError(((CDrawUTwentyFourBppScreenBitmap*)drawDevice)->ConstructScreenL(
                                                            aScreenNo,
															aScreenInfo.iAddress, 
															aScreenInfo.iSize));
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		break;
	
	 case EColor16MA: // 32 Bpp color mode
		if((bpp == 24 || bpp == 32) && aDispMode == CFbsDrawDevice::DisplayMode16M())  	//there is some "ambiguity" about 24 and 32 bit modes... 
	//They are both byte per color component, and both actually have 32 bits per pixel memory use.
			{
			drawDevice = new (ELeave) CDrawThirtyTwoBppScreenBitmapAlpha;
			CleanupStack::PushL(drawDevice);
			User::LeaveIfError(((CDrawThirtyTwoBppScreenBitmapAlpha*)drawDevice)->ConstructScreenL(
                                                            aScreenNo,
															aScreenInfo.iAddress, 
															aScreenInfo.iSize));
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		break;
		
	case EColor16MAP: // 32 Bpp color mode
		if((bpp == 24 || bpp == 32) && aDispMode == CFbsDrawDevice::DisplayMode16M()) 	//there is some "ambiguity" about 24 and 32 bit modes... 
	//They are both byte per color component, and both actually have 32 bits per pixel memory use.
			{
			drawDevice = new (ELeave) CDrawThirtyTwoBppScreenBitmapAlphaPM;
			CleanupStack::PushL(drawDevice);
			User::LeaveIfError(((CDrawThirtyTwoBppScreenBitmapAlphaPM*)drawDevice)->ConstructScreenL(
                                                            aScreenNo,
															aScreenInfo.iAddress, 
															aScreenInfo.iSize));
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		break;

	default:
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(drawDevice);
    return drawDevice;
	}


/********************************************************************/
/*  Implementation of CFbsDrawDevice class                          */
/********************************************************************/

/**
This function calls the correct constructor in function of the display mode.
@param aInfo, Structure of the LCD info
@param aDispMode, display mode   
@return A pointer to just created screen device, which implements CFbsDrawDevice interface
@deprecated Use CFbsDrawDevice::NewScreenDeviceL(TInt aScreenNo, TDisplayMode aDispMode)
*/
EXPORT_C CFbsDrawDevice* CFbsDrawDevice::NewScreenDeviceL(TScreenInfoV01 aInfo, TDisplayMode aDispMode)
	{
    __ASSERT_ALWAYS(aInfo.iScreenAddressValid, Panic(EScreenDriverPanicInvalidWindowHandle));
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
//	return ENone;
	return EColor16MA;
	}
