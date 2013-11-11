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
//

#include <hal.h>
#include <fbs.h>
#include <bitbase.h>
#include <bitdev.h>
#include <bitdraw.h>
#include <bitdrawscaling.h>
#include <bitdraworigin.h>
#include <bitdrawinterfaceid.h>
#include "BITPANIC.H"
#include <bmalphablend.h>
#include <graphics/gdi/gdiconsts.h>


#if defined(SYMBIAN_GRAPHICS_GCE)
#include <bitdrawsurface.h>
#endif

/**
@param aScreenNo The screen number. If the device has a support for only one screen, its 
                 number is 0.
*/
CFbsScreenDevice::CFbsScreenDevice(TInt aScreenNo):
	CFbsDevice(),
    iScreenNo(aScreenNo)
	{
    }


/** Creates a new CFbsScreenDevice object.

@param aLibname Not used.
@param aDispMode The display mode of the device.
@return The newly created FBSERV screen device. */ 
EXPORT_C CFbsScreenDevice* CFbsScreenDevice::NewL(const TDesC& /*aLibname*/,
												  TDisplayMode aDispMode)
	{
	return CFbsScreenDevice::NewL(KDefaultScreenNo, aDispMode);
	}

/** Creates a new CFbsScreenDevice object.
	
@param aLibname Not used.
@param aDispMode The display mode of the device.
@param aWhite Not used. 
@return The newly created font and bitmap server screen device.
@deprecated Use two param version instead. */
EXPORT_C CFbsScreenDevice* CFbsScreenDevice::NewL(const TDesC& /*aLibname*/,
												  TDisplayMode aDispMode,
												  TRgb /*aWhite*/)
	{
	return CFbsScreenDevice::NewL(KDefaultScreenNo, aDispMode);
	}

/** 
Creates a new CFbsScreenDevice object.
@param aScreenNo The screen number. If the device has a support for only one screen, its 
                 number is 0.
@param aDispMode The display mode of the device.
@return The newly created FBSERV screen device. 
*/ 
EXPORT_C CFbsScreenDevice* CFbsScreenDevice::NewL(TInt aScreenNo, TDisplayMode aDispMode)
	{
	CFbsScreenDevice* self = new (ELeave) CFbsScreenDevice(aScreenNo);
	CleanupStack::PushL(self);
	self->ConstructL(aScreenNo, aDispMode);
	CleanupStack::Pop(); // self
	return self;
	}

/** Frees all resources owned by the object prior to its destruction. */
EXPORT_C CFbsScreenDevice::~CFbsScreenDevice()
	{
    }

void CFbsScreenDevice::ConstructL(TInt aScreenNo, TDisplayMode aDispMode)
	{
	iDrawDevice = CFbsDrawDevice::NewScreenDeviceL(aScreenNo, aDispMode);
	iScreenDevice = ETrue;
	iTypefaceStore = CFbsTypefaceStore::NewL(this);
	if(aDispMode != EGray4) // Flicker-free blitting not enabled under EGray4 because it messes up the EGray16-emulating dithering
		{
		TInt scanLineBytes = iDrawDevice->ScanLineBytes();
		const TSize screenSize = SizeInPixels();
		if(screenSize.iHeight > screenSize.iWidth)
			{
			scanLineBytes = scanLineBytes * screenSize.iHeight / screenSize.iWidth;
			}
		iBitBltMaskedBuffer = new(ELeave) TUint8[scanLineBytes * 2];
		}
	}


/** Copies a scanline into a buffer.

This implements the pure virtual function
CBitmapDevice::GetScanLine(). */
EXPORT_C void CFbsScreenDevice::GetScanLine(TDes8& aBuf,const TPoint& aStartPixel,
											TInt aLength,TDisplayMode aDispMode) const
    {
	if (!iDrawDevice)
		return;

	((CFbsScreenDevice*)this)->DoGetScanLine(aBuf,aStartPixel,aLength,aDispMode);
	}


/** Gets the RGB colour of an individual pixel on a bitmapped graphics
device.

This implements the pure virtual function
CBitmapDevice::GetPixel(). */ 
EXPORT_C void CFbsScreenDevice::GetPixel(TRgb& aColor,const TPoint& aPoint) const
    {
	if (!iDrawDevice)
		return;

	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
	if (!deviceRect.Contains(aPoint))
		return;

	aColor = iDrawDevice->ReadPixel(aPoint.iX,aPoint.iY);
	}


/** Converts a horizontal dimension from pixels to twips.

This implements the pure virtual function
MGraphicsDeviceMap::HorizontalPixelsToTwips(). */
EXPORT_C TInt CFbsScreenDevice::HorizontalPixelsToTwips(TInt aPixels) const
    {
	TInt64 pixels=aPixels;
	pixels=(iDrawDevice->HorzTwipsPerThousandPixels() * pixels + 500) / 1000;
	return I64INT(pixels);
	}


/** Converts a vertical dimension from pixels to twips.

This implements the pure virtual function
MGraphicsDeviceMap::VerticalPixelsToTwips(). */	
EXPORT_C TInt CFbsScreenDevice::VerticalPixelsToTwips(TInt aPixels) const
   {
	TInt64 pixels=aPixels;
	pixels=(iDrawDevice->VertTwipsPerThousandPixels() * pixels + 500) / 1000;
	return I64INT(pixels);
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.
@deprecated
*/
EXPORT_C void CFbsScreenDevice::DrawSpriteBegin()
	{
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.
@deprecated
*/
EXPORT_C void CFbsScreenDevice::DrawSpriteEnd()
	{
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.

@return NULL.
@deprecated
*/
EXPORT_C TSpriteBase* CFbsScreenDevice::HideSprite() const
	{
	return NULL;
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.

@param aRect Ignored.
@param aClippingRegion Ignored.
@return NULL.
@deprecated
*/
EXPORT_C TSpriteBase* CFbsScreenDevice::HideSprite(const TRect& /*aRect*/,
												   const TRegion* /*aClippingRegion*/) const
	{
	return NULL;
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.

@param aSprite Ignored.
@deprecated
*/
EXPORT_C void CFbsScreenDevice::ShowSprite(TSpriteBase* /*aSprite*/) const
	{
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.

@param aSprite Ignored.
@param aRect Ignored.
@param aClippingRegion Ignored.
@deprecated
*/
EXPORT_C void CFbsScreenDevice::ShowSprite(TSpriteBase* /*aSprite*/,const TRect& /*aRect*/,
										   const TRegion* /*aClippingRegion*/) const
	{
	}


/** Changes the screen device.

@param aOldDevice A pointer to the old screen device. */
EXPORT_C void CFbsScreenDevice::ChangeScreenDevice(CFbsScreenDevice* aOldDevice)
	{
	if (aOldDevice == NULL)
		{
		TInt ret = iDrawDevice->InitScreen();
		BG_ASSERT_ALWAYS(ret == KErrNone,EBitgdiPanicInvalidWindowHandle);
		}
	else
		{
		delete aOldDevice->iGraphicsAccelerator;
		aOldDevice->iGraphicsAccelerator = NULL;

		if (aOldDevice != this)
			{
			iDrawDevice->SetDisplayMode(aOldDevice->iDrawDevice);
			iOrientation = aOldDevice->iOrientation;
			}
		// else we're re-initialising the existing CFbsScreenDevice's graphics accelerator only
		}

	//Check if the screen device is scaled or the origin is moved. 
	//If it is, then do not create graphics accelerator
	//instance, because it does not have support for scaling&origin.
	TBool scalingOff = ETrue;
	TBool originZero = ETrue;
	MScalingSettings* scalingSettings = NULL;
	if(iDrawDevice->GetInterface(KScalingSettingsInterfaceID, 
							reinterpret_cast <TAny*&> (scalingSettings)) == KErrNone)
		{
		BG_ASSERT_DEBUG_INVARIANT(scalingSettings);
		scalingOff = scalingSettings->IsScalingOff();
		}
	MDrawDeviceOrigin* originInterface = NULL;
	if(iDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, 
							reinterpret_cast <TAny*&> (originInterface)) == KErrNone)
		{
		BG_ASSERT_DEBUG_INVARIANT(originInterface);
		TPoint origin;
		originInterface->Get(origin);
		originZero = origin.iX == 0 && origin.iY == 0;
		}
	if(scalingOff && originZero)
		{
		RHardwareBitmap screen;
        //Some explanations about "-1 - iScreenNo" expression.
        //SetAsScreenReference() is a public, exported method with a default argument'value -1.
        //In SetAsScreenReference()'s implementation before "Multiple screens" source code update,
        //"-1" value meant - this is a screen hardware bitmap. Positive value meant - 
        //in-memory hardware bitmap.
        //After "Multiple screens" update, the meaning of SetAsScreenReference()'s argument is:
        // - Positive value - in-memory hardware bitmap;
        // - Negative value - screen number: "-1" - screen 0, "-2" - screen 1, "-3" - screen 2, ...;
		if(screen.SetAsScreenReference(-1 - iScreenNo)==KErrNone)
			{
			TRAP_IGNORE(iGraphicsAccelerator = CHardwareGraphicsAccelerator::NewL(screen));
			}
		}
	}


/** Gets the palette attributes of the device.

@param aModifiable On return, holds information on whether or not the device 
palette is modifiable (ETrue) or fixed (EFalse). 
@param aNumEntries On return, holds the number of entries in the device 
palette. */
EXPORT_C void CFbsScreenDevice::PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
	{
	aModifiable = (iDrawDevice->DisplayMode() == EColor256);
	aNumEntries = TDisplayModeUtils::NumDisplayModeColors(iDrawDevice->DisplayMode());
	}


/** Sets the device's palette to the specified palette.

Setting the palette is only possible if the device has a modifiable palette, 
which can be determined by calling PaletteAttributes().

@param aPalette The new palette for the device. */
EXPORT_C void CFbsScreenDevice::SetPalette(CPalette* aPalette)
	{
	SetCustomPalette(aPalette); // Have to ignore error for compatibility
	}


/** Gets the device's current palette.

This function is only supported if the device has a modifiable palette, 
which can be determined by calling PaletteAttributes().

@param aPalette On return, holds the devices current palette. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes. */
EXPORT_C TInt CFbsScreenDevice::GetPalette(CPalette*& aPalette) const
	{
	return iDrawDevice->GetCustomPalette(aPalette);
	}


/**
This method has been deprecated.  Sprites are no longer supported in BitGDI.
Calling this method has no effect.
@deprecated
*/
EXPORT_C void CFbsScreenDevice::CancelSprite() const
	{
	}


/** Sets or unsets auto-update for the screen.

@param aValue ETrue, if the screen is set to auto-update; EFalse, otherwise. */
EXPORT_C void CFbsScreenDevice::SetAutoUpdate(TBool aValue)
	{
	iDrawDevice->SetAutoUpdate(aValue);
	}


/** Forces any out of date region of the screen to update. */
EXPORT_C void CFbsScreenDevice::Update()
	{
	iDrawDevice->Update();
	}


/** Forces any out of date region of the screen to update, 
and additionally forces the specified region to update.

@param aRegion The region of the screen to update, in addition 
to any out of date region. */
EXPORT_C void CFbsScreenDevice::Update(const TRegion& aRegion)
	{
	iDrawDevice->Update(aRegion);
	}


/** Gets the size of the device area, in twips.

This implements the pure virtual function CGraphicsDevice::SizeInTwips().

@see CGraphicsDevice::SizeInTwips() */
EXPORT_C TSize CFbsScreenDevice::SizeInTwips() const
	{
	TSize twipssize;
	twipssize.iWidth = HorizontalPixelsToTwips(iDrawDevice->SizeInPixels().iWidth);
	twipssize.iHeight = VerticalPixelsToTwips(iDrawDevice->SizeInPixels().iHeight);
	return twipssize;
	}


/** Converts a horizontal dimension from twips to pixels.

This implements the pure virtual function
MGraphicsDeviceMap::HorizontalTwipsToPixels(). */
EXPORT_C TInt CFbsScreenDevice::HorizontalTwipsToPixels(TInt aTwips) const
    {
	const TInt htptp = iDrawDevice->HorzTwipsPerThousandPixels();
	TInt64 twips = aTwips;
	twips = (1000 * twips + (htptp >> 1)) / htptp;
	return I64INT(twips);
	}


/** Converts a vertical dimension from twips to pixels.

This implements the pure virtual function
MGraphicsDeviceMap::VerticalTwipsToPixels(). */	
EXPORT_C TInt CFbsScreenDevice::VerticalTwipsToPixels(TInt aTwips) const
    {
	const TInt vtptp = iDrawDevice->VertTwipsPerThousandPixels();
	TInt64 twips = aTwips;
	twips = (1000 * twips + (vtptp >> 1)) / vtptp;
	return I64INT(twips);
	}


/** Creates and returns a hardware bitmap (a bitmap which can be drawn to by a 
graphics accelerator whose operations may be implemented in hardware or software), 
whose handle is to the screen.

This allows the caller to draw to the screen like any other hardware bitmap.

This function may not be supported on all hardware. If unsupported, it returns 
an RHardwareBitmap with a handle of zero.

The hardware bitmap can be used to draw directly to the screen. Use it to 
create a TAcceleratedBitmapSpec object, which can either be used to get a 
TAcceleratedBitmapInfo, or can be passed to a graphics operation (an instance 
of a class derived from class TGraphicsOperation) e.g. a bitblt to copy one 
part of the screen to another.

Direct screen access must only be carried out in combination with the Window 
Server's direct screen access classes; i.e. only use the hardware bitmap on 
the CFbsScreenDevice which you get from CDirectScreenAccess, and not from 
your own CFbsScreenDevice.

@return A hardware bitmap whose handle is to the screen.
@see TAcceleratedBitmapSpec
@see TGraphicsOperation
@see CGraphicsAccelerator::Operation()
@see CDirectScreenAccess */
EXPORT_C RHardwareBitmap CFbsScreenDevice::HardwareBitmap()
	{
	RHardwareBitmap hwb;
    //Some explanations about "-1 - iScreenNo" expression.
    //SetAsScreenReference() is a public, exported method with a default argument'value -1.
    //In SetAsScreenReference()'s implementation before "Multiple screens" source code update,
    //"-1" value meant - this is a screen hardware bitmap. Positive value meant - 
    //in-memory hardware bitmap.
    //After "Multiple screens" update, the meaning of SetAsScreenReference()'s argument is:
    // - Positive value - in-memory hardware bitmap;
    // - Negative value - screen number: "-1" - screen 0, "-2" - screen 1, "-3" - screen 2, ...;
	hwb.SetAsScreenReference(-1 - iScreenNo);
	return hwb;
	}

/** Returns pointer to the location of first pixel in frame buffer. Not necessarily the same as pointer
to frame buffer.

@return Pointer to the location of first pixel or NULL if it is not accessible.
@internalComponent
*/
EXPORT_C const TUint32* CFbsScreenDevice::Bits() const
	{
	TAny* interface = NULL;
	TInt ret = iDrawDevice->GetInterface(KFastBlit2InterfaceID, interface);
	if (ret != KErrNone)
		return NULL;
	
	return reinterpret_cast<MFastBlit2*>(interface)->Bits();	
	}

/** Returns frame buffer line pitch or stride.

@return Frame buffer stride.
@internalComponent
*/
EXPORT_C TInt CFbsScreenDevice::Stride() const
	{
	return iDrawDevice->ScanLineBytes();	
	}

/** Query the screen number to which the object refers.

@return Screen number as passed in aScreenNo to NewL.
@publishedAll
@released
*/
EXPORT_C TInt CFbsScreenDevice::ScreenNo() const
	{
	return iScreenNo;	
	}

/** Get the surface identifier for the current device orientation of the screen.
If screen device doesn't support providing a surface, the caller is panicked.

@param aSurface	Set to the surface identifier for the screen buffer.
@internalTechnology
@prototype
*/
EXPORT_C void CFbsScreenDevice::GetSurface(TSurfaceId& aSurface) const
	{
#if defined(SYMBIAN_GRAPHICS_GCE)
	TAny* interface = NULL;
	TInt ret = iDrawDevice->GetInterface(KSurfaceInterfaceID, interface);
	if (ret == KErrNone)
	    {
	    reinterpret_cast<MSurfaceId*>(interface)->GetSurface(aSurface);
	    return;
	    }
#endif
	(void)aSurface;		// Satisfies the compiler, if its a Non-GCE build.
	Panic(EBitgdiPanicInvalidScreenDeviceLibrary);
	}

/** This function is used to request the device orientations supported by the
screen device.

@return A bitwise combination of one or more TDeviceOrientation enumerated
values indicating the device orientations that are supported by this device.
@internalTechnology
@prototype
*/
EXPORT_C TUint CFbsScreenDevice::DeviceOrientationsAvailable() const
	{
#if defined(SYMBIAN_GRAPHICS_GCE)
	TAny* interface = NULL;
	TInt ret = iDrawDevice->GetInterface(KSurfaceInterfaceID, interface);
	if (ret == KErrNone)
	    return reinterpret_cast<MSurfaceId*>(interface)->DeviceOrientationsAvailable();
#endif
	Panic(EBitgdiPanicInvalidScreenDeviceLibrary);
	return 0; // Make the compiler happy.
	}

/** This function selects the surface and device buffer to use in the screen
driver for this screen. Normal and 180° rotations will generally use the same
surface, while 90° and 270° will use another. The surfaces may have different
width, height, stride and surface, so functions that make use of any of these
may be affected after a change in surface orientation, and the return value
should be checked for this reason.

This call does not change the way rendering is performed, but may operate on
the underlying memory using a new shape. The call does not change the display
controller’s settings, as this is handled via the GCE. All this changes are the
internal attributes of the screen device and driver objects. A CFbsBitGc object
activated on the device should be reactivated, to update its own attributes, or
drawing may be corrupted.

Note: while TDeviceOrientation values do not directly correspond to
CFbsBitGc::TGraphicsOrientation values, and cannot be used interchangeably, it
is simple to generate the former from the latter using the left-shift operator
(i.e. device == (1 << graphics)). In particular a device orientation of 90
degrees clockwise is equivalent to a content orientation of 90 degrees anti-
clockwise, which is what TGraphicsOrientation refers to for the equivalent
setting. The letters "CW" in the TDeviceOrientation enumeration refer to a
clockwise device rotation, so EDeviceOrientation90CW is a 90 degree clockwise
rotation of the device.

@param aOrientation	The new device orientation, relative to the normal physical
screen orientation.
@return ETrue is returned if any of the surface, width, height or stride
attributes of the screen device have changed as a result of the call or EFalse
if none of the attributes have changed.
@internalTechnology
@prototype
*/
EXPORT_C TBool CFbsScreenDevice::SetDeviceOrientation(TDeviceOrientation aOrientation) const
	{
#if defined(SYMBIAN_GRAPHICS_GCE)
	TAny* interface = NULL;
	TInt ret = iDrawDevice->GetInterface(KSurfaceInterfaceID, interface);
	if (ret == KErrNone)
	    return reinterpret_cast<MSurfaceId*>(interface)->SetDeviceOrientation(aOrientation);
#endif
	(void)aOrientation; // Satisfies the compiler, if its a Non-GCE build.
	Panic(EBitgdiPanicInvalidScreenDeviceLibrary);
	return EFalse; // Make the compiler happy.
	}

/** This function is used to request the current device orientation.

@return One of the TDeviceOrientation enumerated values.
@internalTechnology
@prototype
*/
EXPORT_C TDeviceOrientation CFbsScreenDevice::DeviceOrientation() const
	{
#if defined(SYMBIAN_GRAPHICS_GCE)
	TAny* interface = NULL;
	TInt ret = iDrawDevice->GetInterface(KSurfaceInterfaceID, interface);
	if (ret == KErrNone)
	    return reinterpret_cast<MSurfaceId*>(interface)->DeviceOrientation();
#endif
	Panic(EBitgdiPanicInvalidScreenDeviceLibrary);
	return EDeviceOrientationNormal; // Make the compiler happy.
	}

