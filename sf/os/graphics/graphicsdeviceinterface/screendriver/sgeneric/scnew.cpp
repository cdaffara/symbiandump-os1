// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "BITDRAW.H"
#include <hal.h>
#include "ScreenInfo.h"
#include "scdraw.h"
#include "scdraw.inl"
#include <graphics/gdi/gdiconsts.h>
#include <graphics/suerror.h>
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

	TInt modeCount;
	TInt matchedMode=-1;
	//there is some "ambiguity" about 24 and 32 bit modes... 
	//They are both byte per color component, and both actually have 32 bits per pixel memory use.
	//This ambiguity does not exist between 12 and 16 bit modes,
	//because they are distinct color component patterns (x444, 565)
	//but for now 24 and 32 bit modes are considered equivalent here.

	if (HAL::Get(aScreenNo, HALData::EDisplayNumModes,modeCount)== KErrNone && modeCount>=1)
		{	//If multiple modes are supported then the highest bpp must be found
		
		TInt reqBpp= TDisplayModeUtils::NumDisplayModeBitsPerPixel(aDispMode);
		TInt reqBpp2=reqBpp;
		if ( reqBpp==24 || reqBpp==32 ) //Best to be specific here. Who knows how likely is 30 or 64 bpp support?
			{
			reqBpp2=32+24 - reqBpp;   //reflect 24<==>32
			//Important compile-time decision embedded here: Only one 32-bit mode is supported
			if(CFbsDrawDevice::DisplayMode16M() != aDispMode)
				{
				User::Leave(KErrNotSupported);
				}
			}
		for (TInt mode=0; mode<modeCount; mode++)
			{
			TInt modeBpp=mode;
			if(HAL::Get(aScreenNo, HALData::EDisplayBitsPerPixel, modeBpp) == KErrNone)
				{
				if (modeBpp==reqBpp || modeBpp==reqBpp2)
					{
					matchedMode=mode;
					break;
					}
				}
			}
		}
	if (matchedMode==-1)
		{	//This is the expected error code
		User::Leave(KErrNotSupported);
		}
	//Switch the display mode, call the constructor of the class defined
	switch(aDispMode)
		{
	/** Monochrome display mode (1 bpp) */
	case EGray2:
		{
		CDrawOneBppScreenBitmap* drawDeviceX = new (ELeave) CDrawOneBppScreenBitmap;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	/** Four grayscales display mode (2 bpp) */
	case EGray4:
		{
		CDrawTwoBppScreenBitmap* drawDeviceX = new (ELeave) CDrawTwoBppScreenBitmap;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	/** 16 grayscales display mode (4 bpp) */
	case EGray16:
		{
		CDrawFourBppScreenBitmapGray* drawDeviceX = new (ELeave) CDrawFourBppScreenBitmapGray;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	/** 256 grayscales display mode (8 bpp) */
	case EGray256:
		{
		CDrawEightBppScreenBitmapGray* drawDeviceX = new (ELeave) CDrawEightBppScreenBitmapGray;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	/** Low colour EGA 16 colour display mode (4 bpp) */
	case EColor16:
		{
		CDrawFourBppScreenBitmapColor* drawDeviceX = new (ELeave) CDrawFourBppScreenBitmapColor;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	/** 256 colour display mode (8 bpp) */
	case EColor256:
		{
		CDrawEightBppScreenBitmapColor* drawDeviceX = new (ELeave) CDrawEightBppScreenBitmapColor;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	/** 4,000 colour display mode (16 bpp) */
	case EColor4K: // 12 Bpp color mode
		{
		CDrawTwelveBppScreenBitmapColor* drawDeviceX = new (ELeave) CDrawTwelveBppScreenBitmapColor;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;

	case EColor64K: // 16 Bpp color mode
		{
		CDrawSixteenBppScreenBitmap* drawDeviceX = new (ELeave) CDrawSixteenBppScreenBitmap;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	case EColor16MU:
		{
		CDrawUTwentyFourBppScreenBitmap* drawDeviceX = new (ELeave) CDrawUTwentyFourBppScreenBitmap;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	case EColor16MA:
		{
		CDrawThirtyTwoBppScreenBitmapAlpha* drawDeviceX = new (ELeave) CDrawThirtyTwoBppScreenBitmapAlpha;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
		}
		break;
	case EColor16MAP:
		{
		CDrawThirtyTwoBppScreenBitmapAlphaPM* drawDeviceX = new (ELeave) CDrawThirtyTwoBppScreenBitmapAlphaPM;
		drawDevice=drawDeviceX;
		CleanupStack::PushL(drawDevice) ;
		User::LeaveIfError(drawDeviceX->ConstructScreen(
	                                                    aScreenNo,
														aScreenInfo.iAddress, 
														aScreenInfo.iSize,matchedMode));
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
	TInt width = 0, height = 0;
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayXPixels, width));
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayYPixels, height));
	__ASSERT_ALWAYS(width > 0 && height > 0, Panic(EScreenDriverPanicInvalidHalValue));
	
	TUint8* address = 0;
	
	TScreenInfo screenInfo(address, TSize(width, height));
	return ::CreateInstanceL(aScreenNo, aDispMode, screenInfo);
	}

/**
 Depending on the current graphics hardware this 
 will return one of the 16M video modes defined in
 TDisplayMode, or ENone if a 16M video mode is not supported.
 The method has to be implemented on all hardware platforms.
 @return a 16M display mode or ENone.
 ENone - it means that current hardware doesn't have 16M color mode.
*/
EXPORT_C TDisplayMode CFbsDrawDevice::DisplayMode16M()
	{
	return EColor16MA;
	}


/**
Complete construction of the helper object.
@param aScreenNo	The screen number, starting from 0.
@param aPixelFormat	Pixel format UID or 0 for default based on bpp
@return KErrNone or a system wide error value.
*/
TInt CScreenDeviceHelper::Construct(TInt aScreenNo, TUidPixelFormat aPixelFormat, TUint aHalMode)
	{
	iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField] = aScreenNo;		// Screen number
	iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField] = aHalMode;			// Rotation and hal mode index
	iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::ETypeGuidField] = aPixelFormat;	//May be zero for non-GCE modes
	iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::ETypeClassField] 
			= ((TUint32)(TSurfaceId::EScreenSurface) << TSurfaceId::TScreenSurfaceUsage::ETypeClassShift);	// Type
	iAssignedOrientation = EDeviceOrientationNormal; // Actual rotation is held seperately from surface ID

	TInt val = 0;
	iHasChunk = EFalse;
	TInt ret = HAL::Get(aScreenNo,HALData::EDisplayMemoryHandle,val);
	if (ret == KErrNone)
		{
		__ASSERT_DEBUG(val != 0, Panic(EScreenDriverPanicInvalidHalValue));
		RChunk chunk;
		ret = chunk.SetReturnedHandle(val);
		if (ret != KErrNone)
			{
			return ret;
			}
		iChunk = chunk;
		ret = iChunk.Duplicate(RThread(), EOwnerProcess);
		// Close before checking for errors, as we don't want to leave the
		// temporary chunk handle floating about. 
		chunk.Close();
		if (ret != KErrNone)
			{
			return ret;
			}
		iHasChunk = ETrue;
		}
	// KErrNotSupported is returned if we can't get the Handle because it's not a driver
	// that supports the concept. We don't return that error, since it's perfectly valid
	// to not support this. 
	else if (ret != KErrNotSupported)   
		{
		return ret;
		}
	return iSurfaceUpdateSession.Connect();
	}

CScreenDeviceHelper::~CScreenDeviceHelper()
	{
	
	iSurfaceUpdateSession.Close();
	iChunk.Close();
	}

void CScreenDeviceHelper::NotifyWhenAvailable(TRequestStatus& aStatus)
	{
	iSurfaceUpdateSession.NotifyWhenAvailable(aStatus);
	}

void CScreenDeviceHelper::CancelUpdateNotification()
	{
	iSurfaceUpdateSession.CancelAllUpdateNotifications();
	}

/**
Implementation of corresponding function in CDrawDevice, utilizing a tracked
update region. Updates the screen from the surface, if the update region is
not empty.
*/
void CScreenDeviceHelper::Update()
	{
	TRequestStatus updateComplete = KRequestPending;
	Update(updateComplete);
	User::WaitForRequest(updateComplete);
	}

void CScreenDeviceHelper::Update(TRequestStatus& aStatus)
	{
	if (!iUpdateRegion.IsEmpty())
		{
		iSurfaceUpdateSession.NotifyWhenAvailable(aStatus);
		iSurfaceUpdateSession.SubmitUpdate(KAllScreens, iSurface, 0, &iUpdateRegion);
		iUpdateRegion.Clear();
		}
	else
		{
		TRequestStatus* pComplete=&aStatus;
		User::RequestComplete(pComplete,KErrNone);	    
		}
	}


/**
Implementation of corresponding function in CDrawDevice, utilizing a tracked
update region. Adds the given rectangle to the update region.
@param aRect	Rectangle to be added to the update region.
*/
void CScreenDeviceHelper::UpdateRegion(const TRect& aRect)
	{
	if (aRect.IsEmpty())
		{
		// Adding an empty rectangle should have no effect.
		return;
		}

	if (iUpdateRegion.CheckError())
		{
		// Try to ensure the region doesn't keep an error forever.
		iUpdateRegion.Clear();
		}

	TRect bounds(iUpdateRegion.BoundingRect());
	iUpdateRegion.AddRect(aRect);

	// If the region fills up, start again with the old bounding box plus this
	// rectangle.
	if (iUpdateRegion.CheckError())
		{
		iUpdateRegion.Clear();
		iUpdateRegion.AddRect(bounds);
		iUpdateRegion.AddRect(aRect);
		}
	}

/**
Reset the update region to be empty without submitting any outstanding updates.
*/
void CScreenDeviceHelper::ResetUpdateRegion()
	{
	iUpdateRegion.Clear();
	}

/**
This function returns the current surface in use for this screen.
@param aSurface	The identifier to be updated with the current screen surface.
*/
void CScreenDeviceHelper::GetSurface(TSurfaceId& aSid) const
	{
	aSid = iSurface;
	}

/**
This function is used to request the device orientations supported by the
screen device.
@return A bitwise combination of one or more TDeviceOrientation enumerated
values indicating the device orientations that are supported by this device.
*/
TUint CScreenDeviceHelper::DeviceOrientationsAvailable(const TSize& aScreenSize) const
	{
	//All that can be reported here is what the CScreenDevice can support via HAL
	//With generic driver, the runtime can be further restricted by the GCE
	if (	aScreenSize.iWidth	&&	aScreenSize.iWidth==aScreenSize.iHeight	)
		{
		return EDeviceOrientationNormal | EDeviceOrientation90CW |
			EDeviceOrientation180 | EDeviceOrientation270CW;
		}
	//Query base HAL for rotated view support
	TInt	offset1=iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField]|TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
	if (	HAL::Get(ScreenNumber(), HALData::EDisplayOffsetBetweenLines, offset1)==KErrNone
		&&	offset1!=0)
		{
		return EDeviceOrientationNormal | EDeviceOrientation90CW |
			EDeviceOrientation180 | EDeviceOrientation270CW;
		}
	else
		return EDeviceOrientationNormal | EDeviceOrientation180;
	}

/**
This function selects the surface and device buffer to use in the screen
driver for this screen. Normal and 180° rotations will generally use the same
surface, while 90° and 270° will use another. The surfaces may have different
width, height, stride and surface ID, so functions that make use of any of
these may be affected after a change in surface orientation, and the return
value should be checked for this reason.

This call does not change the way rendering is performed, but may operate on
the underlying memory using a new shape. The call does not change the display
controller's settings, as this is handled via the GCE. All this changes are the
internal attributes of the screen device and driver objects. A CFbsBitGc object
activated on the device should be reactivated, to update its own attributes, or
drawing may be corrupted.

Note: while TDeviceOrientation values do not directly correspond to 
CFbsBitGc::TGraphicsOrientation values, and cannot be used interchangeably, it
is simple to generate the former from the latter using the left-shift operator
(i.e. device == (1 &lt;&lt; graphics)). In particular a device orientation of
90 degrees clockwise is equivalent to a content orientation of 90 degrees anti-
clockwise, which is what TGraphicsOrientation refers to for the equivalent
setting. The letters &quot;CW&quot; in the TDeviceOrientation enumeration refer
to a clockwise device rotation, so EDeviceOrientation90CW is a 90 degree
clockwise rotation of the device.

@param aOrientation	The new device orientation, relative to the normal physical
screen orientation.
@param aNewSize The new pixel dimensions of the surface to be used.
@return ETrue is returned if any of the surface, width, height or stride
attributes of the screen device have changed as a result of the call or EFalse
if none of the attributes have changed.
*/
TBool CScreenDeviceHelper::SetDeviceOrientation(TDeviceOrientation aOrientation, TSize& aNewSize)
	{
	// Check only one orientation bit is set
	if (((TInt)aOrientation - 1) & aOrientation)
		{
		Panic(EScreenDriverPanicInvalidParameter);
		}

	// Check the orientation is supported
	if ((DeviceOrientationsAvailable(aNewSize) & aOrientation) == 0)
		{
		Panic(EScreenDriverPanicInvalidParameter);
		}

	iAssignedOrientation=aOrientation;
	
	return SetDeviceFlipMode(ConvertFlip(aOrientation),aNewSize);
	}
/** Sets or clears the flipped flag indicating that the width and height have been swapped for a +/-90 deg rotation
 *  Rotation is not required for square displays unless the Hal wants one.
 **/
TBool CScreenDeviceHelper::SetDeviceFlipMode(TBool aFlip, TSize& aNewSize)
	{
	//This is now a private method that doesn't validate aFlip
	TInt newFlipMode= iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
	if (aFlip)
		{
		newFlipMode|=TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
		}
	else
		{
		newFlipMode&=~TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
		}
	if (newFlipMode == iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField])
		{
		// No change to mode requested.
		return EFalse;
		}
	TInt err=0;
	err|=HAL::Get(ScreenNumber(), HALData::EDisplayXPixels, aNewSize.iWidth);
	err|=HAL::Get(ScreenNumber(), HALData::EDisplayYPixels, aNewSize.iHeight);
	__ASSERT_ALWAYS(err==KErrNone,Panic(EScreenDriverPanicInvalidHalValue));
	 if (aNewSize.iWidth==aNewSize.iHeight)
		{	//Attempt optimisation to not flip if the screen is square, so avoid recomposition.
		TInt	stride1=iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
		TInt	stride2=stride1^TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
		TInt	offset1=stride1;
		TInt	offset2=stride2;
		//Does the rotated mode have any other attributes that differ?
		//It is just about possible to imagine that the rotated display 
		//wants to use a different setting for the flipped legacy buffer for optimisation purposes.
		err|=HAL::Get(ScreenNumber(), HALData::EDisplayOffsetToFirstPixel, offset1);
		err|=HAL::Get(ScreenNumber(), HALData::EDisplayOffsetBetweenLines, stride1);
		//The existing mode settings should not fail... we are already in this mode!
		__ASSERT_ALWAYS(err==KErrNone,Panic(EScreenDriverPanicInvalidHalValue));
		
		TInt rotatedErr =	HAL::Get(ScreenNumber(), HALData::EDisplayOffsetToFirstPixel, offset2);
		rotatedErr |=		HAL::Get(ScreenNumber(), HALData::EDisplayOffsetBetweenLines, stride2);
		//The HAL may indicate rotation is not required by failing to return data or returning the same data
		if ( rotatedErr!=KErrNone || stride2==0 ) //Offset can legitimately be zero.
			{	
			// No change to mode supported.
			return EFalse;
			}
		if ( stride1==stride2 && offset1==offset2 )
			{
			// No change to mode needed.
			return EFalse;
			}
		}

	iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField] = newFlipMode;
	if (aFlip)
		{
		// Swap width and height in the alternate orientation.
		aNewSize.SetSize(aNewSize.iHeight, aNewSize.iWidth);
		}
	return ETrue;
	}
/**	Returns the stride for the given mode.
 *  This method must not panic if it should fail!
 **/
TUint CScreenDeviceHelper::BytesPerScanline() const
	{
	TInt linepitchInBytes = iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
	TInt ret = HAL::Get(ScreenNumber(),HALData::EDisplayOffsetBetweenLines,linepitchInBytes);
	if (ret!=KErrNone)
		{
		return 0;
		}
	return linepitchInBytes ;
	}
/** Returns the address for the image data
 *  This method must not panic if it should fail!
 **/
void* CScreenDeviceHelper::AddressFirstPixel() const
	{
	TInt bufferStartAddress = iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
	TInt ret = KErrNone;
	if (iHasChunk)
		{
		// The "chunk" way to do this is to get the handle of the chunk, and then the base address of the 
		// chunk itself. 
		bufferStartAddress = (TInt)iChunk.Base();
		}	 
	else 
		{
		// Chunk not supported, use older HAL call to get the buffer address. 
		ret = HAL::Get(ScreenNumber(),HALData::EDisplayMemoryAddress,bufferStartAddress);	
		if (ret!=KErrNone)
			{
			return 0;
			}
		}
	TInt bufferOffsetFirstPixel = iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
	ret = HAL::Get(ScreenNumber(),HALData::EDisplayOffsetToFirstPixel,bufferOffsetFirstPixel);
	if (ret!=KErrNone)
		{
		return 0;
		}
	return (void*)(bufferStartAddress+bufferOffsetFirstPixel);
	}

/**
Returns the current device width/height flip state of this surface, representing a +/-90 deg rotation.
**/
TBool	CScreenDeviceHelper::DeviceFlipped() const
	{
	return  (iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField] & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) != 0;	//!=0 forces true --> 1
	}


/**
Returns the current device orientation.
*/
TDeviceOrientation CScreenDeviceHelper::DeviceOrientation() const
	{
	return iAssignedOrientation;
	}
/**	Returns an accurate scaling factor between twips and pixels in width.

 **/
TInt	CScreenDeviceHelper::HorzTwipsPerThousandPixels(const TSize& aPixels)const
	{
	__ASSERT_DEBUG(aPixels.iWidth, Panic(EScreenDriverPanicInvalidSize));
	
	TInt width = 0;
	TInt r = HAL::Get(ScreenNumber(), SecondIfFlipped(HAL::EDisplayXTwips,HAL::EDisplayYTwips), width);
	__ASSERT_DEBUG(r==KErrNone && width!=0, Panic(EScreenDriverPanicInvalidHalValue));
   
	return (width * 1000) / aPixels.iWidth;
	}
/**	Returns an accurate scaling factor between twips and pixels in height.
 **/
TInt	CScreenDeviceHelper::VertTwipsPerThousandPixels(const TSize& aPixels)const
	{
	__ASSERT_DEBUG(aPixels.iHeight, Panic(EScreenDriverPanicInvalidSize));

	TInt height = 0;
	TInt r = HAL::Get(ScreenNumber(), SecondIfFlipped(HAL::EDisplayYTwips,HAL::EDisplayXTwips), height);
	__ASSERT_DEBUG(r==KErrNone && height!=0, Panic(EScreenDriverPanicInvalidHalValue));

	return (height * 1000) / aPixels.iHeight;
	} 


