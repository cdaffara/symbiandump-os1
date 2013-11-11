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

#include <bitdev.h>
#include <bitdraw.h>
#include <bitdrawscaling.h>
#include <bitdrawinterfaceid.h>
#include "BITPANIC.H"

const TInt KMaxPixelSize = KMaxTInt / 4; // Maximum pixel size to avoid some overflow problems

CFbsBitmapDevice::CFbsBitmapDevice():
	CFbsDevice()
	{}


/** Frees all resources owned by the object prior to its destruction. */
EXPORT_C CFbsBitmapDevice::~CFbsBitmapDevice()
	{
	delete iFbsBmp;
	}


/** Constructs the object from the specified Font and Bitmap server
managed bitmap.

@param aFbsBitmap A pointer to a Font and Bitmap server managed bitmap.
@param aLibname Name of the library to create the low-level CFbsDrawDevice 
object from.
@return A pointer to the newly constructed graphics device. */
EXPORT_C CFbsBitmapDevice* CFbsBitmapDevice::NewL(CFbsBitmap* aFbsBitmap,
												  const TDesC& /*aLibname*/)
    {
	return CFbsBitmapDevice::NewL(aFbsBitmap);
	}

/** Allocates and constructs the device with the bitmap. Also creates a 2D graphics 
accelerator which is owned and used by the device.

@param aFbsBitmap A pointer to the font and bitmap server managed bitmap.
@leave KErrArgument The bitmap's handle is zero.
@leave KErrAccessDenied The bitmap is in the ROM.
@return A pointer to the newly constructed device.
@panic EBitgdiPanicInvalidBitmap aFbsBitmap is NULL. */
EXPORT_C CFbsBitmapDevice* CFbsBitmapDevice::NewL(CFbsBitmap* aFbsBitmap)
	{
	BG_ASSERT_ALWAYS(aFbsBitmap != NULL,EBitgdiPanicInvalidBitmap);

	CFbsBitmapDevice* self = new(ELeave) CFbsBitmapDevice;
	CleanupStack::PushL(self);
	self->ConstructL(aFbsBitmap);
	CleanupStack::Pop(); // self
	return self;
	}

void CFbsBitmapDevice::ConstructL(CFbsBitmap* aFbsBitmap)
	{
	if(!aFbsBitmap->Handle())
		User::Leave(KErrArgument);
	if(aFbsBitmap->IsRomBitmap())
		User::Leave(KErrAccessDenied);

	iTypefaceStore = CFbsTypefaceStore::NewL(this);
	iFbsBmp = new(ELeave) CFbsBitGcBitmap;
	User::LeaveIfError(iFbsBmp->Duplicate(aFbsBitmap->Handle()));
	const TDisplayMode dispMode = iFbsBmp->DisplayMode();

	iDrawDevice = CFbsDrawDevice::NewBitmapDeviceL(iFbsBmp->SizeInPixels(), dispMode, iFbsBmp->DataStride());

	TInt hwHandle = aFbsBitmap->HardwareBitmapHandle();
	if(hwHandle)
		{
		TRAP_IGNORE(iGraphicsAccelerator = CHardwareGraphicsAccelerator::NewL(RHardwareBitmap(hwHandle)));
		}
	if (iGraphicsAccelerator==NULL)
		{
		TRAP_IGNORE(iGraphicsAccelerator = CSoftwareGraphicsAccelerator::NewL(iFbsBmp));
		}
	}

/** Resizes the device.

@param aSize The new size in pixels. 
@return KErrNone, if successful; otherwise another of the system-wide error 
codes. */
EXPORT_C TInt CFbsBitmapDevice::Resize(const TSize& aSize)
	{
	if(!iDrawDevice || !iFbsBmp)
		return(KErrGeneral);

	if(aSize.iWidth < 0 || aSize.iHeight < 0)
		return KErrArgument;

	if (aSize.iWidth > KMaxPixelSize || aSize.iHeight > KMaxPixelSize)
		return KErrTooBig;

    if (iFbsBmp->HardwareBitmapHandle())
        {
        return KErrNotSupported;
        }

	const TDisplayMode dispMode = iFbsBmp->DisplayMode();
	CFbsDrawDevice* drawDevice = NULL;
	TRAPD(err, drawDevice = CFbsDrawDevice::NewBitmapDeviceL(aSize, dispMode, CFbsBitmap::ScanLineLength(aSize.iWidth, dispMode)));
	if (err != KErrNone)
		return err;

	TInt ret = iFbsBmp->Resize(aSize);
	if (ret != KErrNone)
		{
		delete drawDevice;
		return ret;
		}

	delete iDrawDevice;
	drawDevice->SetBits(NULL);
	iDrawDevice = drawDevice;
	iOrientation = CFbsBitGc::EGraphicsOrientationNormal;

	// Now get a new GraphicsAccelerator but it doesn't matter if we fail, we can work without one
	delete iGraphicsAccelerator;
	iGraphicsAccelerator = NULL;
	TRAP_IGNORE(iGraphicsAccelerator = CSoftwareGraphicsAccelerator::NewL(iFbsBmp));
	return KErrNone;
	}

/**
This method is called when you are about to start direct drawing to the bitmap memory.
Calls to DrawingBegin() must be paired with a subsequent call to DrawingEnd().
Also, code must not leave between a DrawingBegin() - DrawingEnd() pair.
@param aAlways Not used.

@see CFbsBitmapDevice::DrawingEnd()
*/
EXPORT_C void CFbsBitmapDevice::DrawingBegin(TBool /*aAlways*/)
	{
	iFbsBmp->BeginDataAccess();
	SetBits();
	}

/**
This method is called when you have finished direct drawing to the bitmap memory.
Calls to DrawingEnd() must correspond to a prior call to DrawingBegin().
@param aAlways Not used.

@see CFbsBitmapDevice::DrawingBegin()
*/
EXPORT_C void CFbsBitmapDevice::DrawingEnd(TBool /*aAlways*/)
	{
	iDrawDevice->SetBits(NULL);
	iFbsBmp->EndDataAccess(EFalse);
	}

void CFbsBitmapDevice::SetBits()
	{
#ifdef _DEBUG
	// Bitmap devices only support normal orientation
	TInt devHeight = iDrawDevice->SizeInPixels().iHeight;
	MScalingSettings* scaling;
	if (iDrawDevice->GetInterface(KScalingSettingsInterfaceID, (TAny*&)scaling) == KErrNone)
		{
		TInt factorX, factorY, divisorX, divisorY;
		scaling->Get(factorX, factorY, divisorX, divisorY);
		// Both divisorX and divisorY should be 1
		if (factorY > 1)
			{
			devHeight = (devHeight - 1) * factorY + 1;
			}
		}
#endif
	BG_ASSERT_DEBUG(iFbsBmp->DataStride() == iDrawDevice->ScanLineBytes(), EBitgdiPanicInvalidBitmap);
	BG_ASSERT_DEBUG(iFbsBmp->SizeInPixels().iHeight >= devHeight, EBitgdiPanicInvalidBitmap);
	TUint32* data = iFbsBmp->DataAddress();
	BG_ASSERT_ALWAYS(data, EBitgdiPanicInvalidBitmap);
	iDrawDevice->SetBits(data);
	}


/** Copies a scanline into a buffer.

The function provides a concrete implementation of the pure virtual
function CBitmapDevice::GetScanLine(). */
EXPORT_C void CFbsBitmapDevice::GetScanLine(TDes8& aBuf,
											const TPoint& aPixel,
											TInt aLength,
											TDisplayMode aDispMode) const
 	{
	iFbsBmp->BeginDataAccess();
	CONST_CAST(CFbsBitmapDevice*,this)->SetBits();
	CONST_CAST(CFbsBitmapDevice*,this)->DoGetScanLine(aBuf,aPixel,aLength,aDispMode);
	iFbsBmp->EndDataAccess(ETrue);
	}

/** Gets the RGB colour of an individual pixel on a bitmapped graphics
device.

The function provides a concrete implementation of the pure virtual
function CBitmapDevice::GetPixel(). */
EXPORT_C void CFbsBitmapDevice::GetPixel(TRgb& aColor,const TPoint& aPoint) const
	{
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
	if (!deviceRect.Contains(aPoint))
		return;

	iFbsBmp->BeginDataAccess();
	((CFbsBitmapDevice*)this)->SetBits();
	aColor = iDrawDevice->ReadPixel(aPoint.iX,aPoint.iY);
	iFbsBmp->EndDataAccess(ETrue);
	}


/** Converts a horizontal dimension of a device in pixels to a horizontal
dimension in twips.

The function provides a concrete implementation of the pure virtual
function MGraphicsDeviceMap::HorizontalPixelsToTwips(). */	 
EXPORT_C TInt CFbsBitmapDevice::HorizontalPixelsToTwips(TInt aPixels) const
    {
	return iFbsBmp->HorizontalPixelsToTwips(aPixels);
	}


/** Converts a vertical dimension of a device in pixels to a vertical
dimension in twips.

The function provides a concrete implementation of the pure virtual
function MGraphicsDeviceMap::VerticalPixelsToTwips(). */
EXPORT_C TInt CFbsBitmapDevice::VerticalPixelsToTwips(TInt aPixels) const
  	{
	return iFbsBmp->VerticalPixelsToTwips(aPixels);
	}


/** Gets the size of the device, in twips.

@return The size of the device. */
EXPORT_C TSize CFbsBitmapDevice::SizeInTwips() const
	{
	return iFbsBmp->SizeInTwips();
	}

/**  Converts a horizontal dimension of a device in twips to a horizontal
dimension in pixels.

The function provides a concrete implementation of the pure virtual
function MGraphicsDeviceMap::HorizontalTwipsToPixels(). */
EXPORT_C TInt CFbsBitmapDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return iFbsBmp->HorizontalTwipsToPixels(aTwips);
	}


/** Converts a vertical dimension of a device in twips to a vertical
dimension in pixels.

The function provides a concrete implementation of the pure virtual
function MGraphicsDeviceMap::VerticalTwipsToPixels(). */	
EXPORT_C TInt CFbsBitmapDevice::VerticalTwipsToPixels(TInt aTwips) const
    {
	return iFbsBmp->VerticalTwipsToPixels(aTwips);
	}


/** Gets the palette attributes of the device.

The function provides a concrete implementation of the pure virtual
function CGraphicsDevice::PaletteAttributes(). */	
EXPORT_C void CFbsBitmapDevice::PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
    {
	aModifiable = (iDrawDevice->DisplayMode() == EColor256);
	aNumEntries = TDisplayModeUtils::NumDisplayModeColors(iDrawDevice->DisplayMode());
	}

/** Sets the device's palette to the specified palette.

Setting the palette is only possible if the device has a modifiable palette, 
which can be determined by calling PaletteAttributes().

The function provides a concrete implementation of the pure virtual
function CGraphicsDevice::SetPalette(). */
EXPORT_C void CFbsBitmapDevice::SetPalette(CPalette* aPalette)
	{
	SetCustomPalette(aPalette); // Have to ignore error for compatibility
	}


/** Gets the device's current palette.

This function is only supported if the device has a modifiable palette, 
which can be determined by calling PaletteAttributes().

The function provides a concrete implementation of the pure virtual
function CGraphicsDevice::GetPalette(). */	
EXPORT_C TInt CFbsBitmapDevice::GetPalette(CPalette*& aPalette) const
    {
	return iDrawDevice->GetCustomPalette(aPalette);
	}

/**
The method swaps bitmap device's width and height.
For example: if the size is (40, 20), the swapped size will be (20, 40).
The device's content is not preserved.
The method leaves CFbsBitmapDevice object in a consistent state - 
scaling settings will be set with their default values (the scaling is switched off),
the device's dither origin will be set to (0,0), scaling origin to (0,0).

Note: If the device was scaled or its dither origin was set with a non-default value,
it has to be rescaled again, respectivelly the dither origin has to be set again.

Note: All graphics contexts, already created by the device, should be 
re-activated calling CFbsBitGc::Activate(). 

Note: Do not call SwapWidthAndHeight() between DrawingBegin() and DrawingEnd() calls!

@return KErrNone The call was successfull.
@return KErrAccessDenied ROM bitmap size can't be swapped.
@return KErrNotSupported Hardware bitmap size can't be swapped.
@return KErrGeneral iDrawDevice or iFbsBmp is NULL.
*/
EXPORT_C TInt CFbsBitmapDevice::SwapWidthAndHeight()
	{
	if(!iDrawDevice || !iFbsBmp)
		{
		return KErrGeneral;
		}
	TInt err = iFbsBmp->SwapWidthAndHeight();
	if(err == KErrNone)
		{
		iDrawDevice->SwapWidthAndHeight();
		}
	return err;
	}


/**
 Required to ensure BC between NGage and 7.0S platforms.  
 Functions are exported at ordinal corresponding to where NGage platform
 has extended this library and must not be moved.
 */
EXPORT_C void DummyReserved1()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved2()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved3()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved4()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved5()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved6()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved7()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved8()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved9()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
EXPORT_C void DummyReserved10()
	{
	User::Panic(_L("Dummy Function"), 0);
	}
