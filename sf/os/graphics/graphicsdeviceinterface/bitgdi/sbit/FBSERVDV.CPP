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

#include <bitstd.h>
#include <bitdev.h>
#include <bitdraw.h>
#include <bitdrawscaling.h>
#include <bitdraworigin.h>
#include <bitdrawinterfaceid.h>
#include "BITPANIC.H"

/**
Used by RectCompare()
@internalComponent
*/
enum {EScanBufSize=0x80}; 

CFbsDevice::CFbsDevice():
	CBitmapDevice()
	{
	iFbs = RFbsSession::GetSession();
	}


/** Frees all resources owned by the object prior to its destruction. */
EXPORT_C CFbsDevice::~CFbsDevice()
	{
	delete iDrawDevice;
	delete iTypefaceStore;
	delete iBitBltMaskedBuffer;
	delete iGraphicsAccelerator;
	}

/** Creates a font and bitmap server graphics context for the device and 
activates it.

It is the responsibility of the caller to delete the graphics context when 
it is no longer needed.

@param aGc On return, contains a pointer to the graphics context. 
@return KErrNone if successful, otherwise, another one of the system-wide error 
codes. */
EXPORT_C TInt CFbsDevice::CreateContext(CFbsBitGc*& aGc)
	{
	TRAPD(ret,aGc = CFbsBitGc::NewL());
	if (ret != KErrNone)
		return ret;

	if (iScreenDevice)
		aGc->ActivateNoJustAutoUpdate(this);
	else
		aGc->Activate(this);

	return KErrNone;
	}

/** Gets the device's display mode.

@return The display mode of the device. */
EXPORT_C TDisplayMode CFbsDevice::DisplayMode() const
	{
	return iDrawDevice->DisplayMode();
	}

/** Gets the size of the device, in pixels.

@return The width and height of the device, in pixels */
EXPORT_C TSize CFbsDevice::SizeInPixels() const
	{
	return iDrawDevice->SizeInPixels();
	}

/** Compares two rectangles, including their contents.

This function is intended for use by test code only.

@param aSourceRect The first rectangle to be compared (in this device).
@param aDevice The font and bitmap server device in which the second rectangle 
is found.
@param aDeviceRect The second rectangle to be compared.
@return ETrue if the rectangles are the same; EFalse otherwise. */
EXPORT_C TBool CFbsDevice::RectCompare(const TRect& aSourceRect,
									   const CFbsDevice& aDevice,
									   const TRect& aDeviceRect) const
	{
	TRect deviceRect1;
	iDrawDevice->GetDrawRect(deviceRect1);
	TRect deviceRect2;
	aDevice.iDrawDevice->GetDrawRect(deviceRect2);
	TRect area(deviceRect1);
	TRect devarea(deviceRect2);
	TRect tmp(aSourceRect);
	TRect tmp2(aDeviceRect);
	tmp.Intersection(area);
	tmp2.Intersection(devarea);
	if (tmp!=aSourceRect ||
		tmp2!=aDeviceRect)
		return(EFalse);
	TInt width=aSourceRect.Width();
	TInt height=aSourceRect.Height();
	if (width!=aDeviceRect.Width() ||
		height!=aDeviceRect.Height())
		return(EFalse);

	TBuf8<EScanBufSize> buf1;
	TBuf8<EScanBufSize> buf2;
	TDisplayMode displayMode = iDrawDevice->ScanLineDisplayMode();
	if (displayMode < EGray256)
		displayMode = EGray256;
	else if (displayMode == EColor16)
		displayMode = EColor256; // Increase the display mode so that each pixel takes at least one byte
	TInt colsPerBuf=(EScanBufSize*EScanBufSize)/
				CFbsBitmap::ScanLineLength(EScanBufSize, displayMode);
	TBool ret=EFalse;

	for(TInt row=0;row<height;row++)
		{
		TInt read=0;
		TInt len=colsPerBuf;
		do
			{
			if ((width-read)<len)
				len=width-read;
			GetScanLine(buf1,TPoint(aSourceRect.iTl.iX+read,aSourceRect.iTl.iY+row),len,displayMode);
			aDevice.GetScanLine(buf2,TPoint(aDeviceRect.iTl.iX+read,aDeviceRect.iTl.iY+row),len,displayMode);
			if (buf1!=buf2)
				goto failed;
			read+=len;
			} while(read<width);
		}
	ret=ETrue;
failed:
	return(ret);
	}

/** Adds a font file to the device's typeface store. The specified font
file must be accessible to any process, i.e. not located inside an
application's private directory.

@param aName The name of the font file. 
@param aId On return, the UID value of the font file.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes. */
EXPORT_C TInt CFbsDevice::AddFile(const TDesC& aName,TInt& aId)
	{
	return iTypefaceStore->AddFile(aName,aId);
	}

/** Decrements the reference count of a file which was added using 
AddFile(), and removes the file from the typeface store if the reference count reaches zero. 

If zero is passed as parameter, then an attempt is made to remove all font objects
from the device's typeface store provided none of the fonts in the store are 
currently accessed, otherwise it has no effect.

@param aId The UID value of the font file to be removed. The default is 0. */
EXPORT_C void CFbsDevice::RemoveFile(TInt aId)
	{
	iTypefaceStore->RemoveFile(aId);
	}

/** Provides access to a client-side font object in the device's typeface store that most closely 
matches a font specification.

When the font is no longer needed, call @c ReleaseFont().

Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInTwips() 
yielding (virtually) the same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of twips, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes.
@deprecated */
EXPORT_C TInt CFbsDevice::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Creates a client-side font from those available in the device's typeface 
store that most closely matches a font specification.

When the font is no longer needed, call @c ReleaseFont().

Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInPixels() 
yielding (virtually) the same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in pixels.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes.
@deprecated */
EXPORT_C TInt CFbsDevice::GetNearestFontInPixels(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

/** Creates a client-side font from those available in the device's typeface 
store that most closely matches a font specification. 

When the font is no longer needed, call @c ReleaseFont().

This new function replaces the deprecated @c GetNearestFontInTwips() yielding (virtually) the 
same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of twips, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes. */
EXPORT_C TInt  CFbsDevice::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return iTypefaceStore->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Creates a client-side font from those available in the device's typeface 
store that most closely matches a font specification. 

When the font is no longer needed, call @c ReleaseFont().

This new function replaces the deprecated @c GetNearestFontInPixels() yielding (virtually) the 
same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in pixels.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes. */
EXPORT_C TInt  CFbsDevice::GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return iTypefaceStore->GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

/** Creates a client-side font from those available in the device's typeface 
store that most closely matches a font specification. 

When the font is no longer needed, call @c ReleaseFont().

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

@param aFont On return, the pointer is set to point to the device font which 
most closely approximates to the required font specification.
@param aFontSpec An absolute font specification. 
@param aMaxHeight The maximum height in twips within which the font must
fit - this overrides the height specified in aFontSpec. If maximum height
is greater than 1024 pixels, the function returns KErrTooBig. And returns KErrArgument
if equals to 1 pixel.
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes. */
EXPORT_C TInt  CFbsDevice::GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return iTypefaceStore->GetNearestFontToMaxHeightInTwips(aFont, aFontSpec, aMaxHeight);
	}

/** Creates a client-side font from those available in the device's typeface 
store that most closely matches a font specification. 

When the font is no longer needed, call @c ReleaseFont().

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

@param aFont On return, the pointer is set to point to the device font which 
most closely approximates to the required font specification.
@param aFontSpec An absolute font specification. 
@param aMaxHeight The maximum height in pixels within which the font must
fit - this overrides the height specified in aFontSpec.  If maximum height
is greater than 1024 pixels, the function returns KErrTooBig. And returns KErrArgument
if equals to 1 pixel.
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes. */
EXPORT_C TInt  CFbsDevice::GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return iTypefaceStore->GetNearestFontToMaxHeightInPixels(aFont, aFontSpec, aMaxHeight );
	}

/** Gets a specific bitmap font, identified by its UID, from the device's typeface 
store.
	
When the font is no longer needed, call ReleaseFont().
	
@param aFont On return, set to point to the font.
@param aUId The UID identifying the bitmap font.
@param aStyle Algorithmic style for the font.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes. */
EXPORT_C TInt CFbsDevice::GetFontById(CFont*& aFont,TUid aUId,const TAlgStyle& aStyle)
	{
	return iTypefaceStore->GetFontById(aFont,aUId,aStyle);
	}

/** Marks the specified font as no longer needed by the user of the device.

As fonts can be shared between applications, this function does not delete 
the copy of the font from RAM unless the font is only being used by this device.

@param aFont A pointer to the font to be released. */
EXPORT_C void CFbsDevice::ReleaseFont(CFont* aFont)
	{
	iTypefaceStore->ReleaseFont(aFont);
	}

/** Gets the number of typefaces supported by the device.

@return The number of typefaces supported. */
EXPORT_C TInt CFbsDevice::NumTypefaces() const
	{
	return iTypefaceStore->NumTypefaces();
	}

/** Gets information about an indexed typeface.

@param aTypefaceSupport Provides information about the typeface, including 
its name and attributes.
@param aTypefaceIndex The index of the requested typeface in the device's 
typeface store; between zero and NumTypefaces() 1 inclusive.
@see CGraphicsDevice::TypefaceSupport() */
EXPORT_C void CFbsDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport,
										  TInt aTypefaceIndex) const
	{
	iTypefaceStore->TypefaceSupport(aTypefaceSupport,aTypefaceIndex);
	}

/** Gets the height of a font in twips.

This is an implementation of
CGraphicsDevice::FontHeightInTwips(). */
EXPORT_C TInt CFbsDevice::FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
    {
	return iTypefaceStore->FontHeightInTwips(aTypefaceIndex,aHeightIndex);
	}

/**  Gets the height, in pixels, of the specified typeface at one of its
defined heights.

This is an implementation of
CBitMapDevice::FontHeightInPixels(). */
EXPORT_C TInt CFbsDevice::FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const
	{
	return iTypefaceStore->FontHeightInPixels(aTypefaceIndex,aHeightIndex);
	}

	/**
	Function to add a CLinkedTypefaceSpecification to the font and bitmap server typeface store.
	@capability ECapabilityWriteDeviceData
	@publishedPartner
	@released
	@param aLinkedTypefaceSpec.  The typeface specification to be added.
	@param aId. A unique ID for the typeface
	@return an global error code
	@see CLinkedTypefaceSpecification
	*/
	EXPORT_C TInt CFbsDevice::RegisterLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec, TInt& aId)
	{
	return iTypefaceStore->RegisterLinkedTypeface(aLinkedTypefaceSpec, aId);
	}

/** Sets the variable 8 bits per pixel colour palette, replacing the system default 
one. Only the entries in the system default palette which have corresponding 
entries in aPalette are overwritten, i.e. if aPalette contains fewer than 
256 colours, some will remain unchanged. If aPalette has more than 256 entries, 
the additional entries are ignored.

@param aPalette The custom palette.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes. */
EXPORT_C TInt CFbsDevice::SetCustomPalette(const CPalette* aPalette)
	{
	TInt ret = iDrawDevice->SetCustomPalette(aPalette);
	if(ret==KErrNone)
		{
		// Graphics accelerator doesn't currently support changing palettes,
		// so delete it if the palette is successfully changed.
		delete iGraphicsAccelerator;
		iGraphicsAccelerator = NULL;
		}
	return ret;
	}

/**
The method gets a scanline and puts the scanline data into aBuf.
@internalComponent
@param aBuf The destination buffer. It should be with enough length to collect 
requested scanline data.
@param aPixel The first pixel of the requested scanline data
@param aLength The length in pixels of requested data
@param aDispMode requested scanline should be converted regarding aDispMode parameter
*/
void CFbsDevice::DoGetScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aLength,
							   TDisplayMode aDispMode)
	{
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
	if (aPixel.iY < deviceRect.iTl.iY || aPixel.iY > deviceRect.iBr.iY)
		return;

	TInt byteLength = 0;
	TInt bitsPerPixel = 0;

	switch(aDispMode)
		{
	case EGray2:
		bitsPerPixel = 1;
		byteLength = aLength / 8 + (aLength % 8 ? 1 : 0);
		break;
	case EGray4:
		bitsPerPixel = 2;
		byteLength = aLength / 4 + (aLength % 4 ? 1 : 0);
		break;
	case EGray16:
	case EColor16:
		bitsPerPixel = 4;
		byteLength = aLength / 2 + (aLength % 2 ? 1 : 0);
		break;
	case EGray256:
	case EColor256:
		bitsPerPixel = 8;
		byteLength = aLength;
		break;
	case EColor4K:
	case EColor64K:
		bitsPerPixel = 16;
		byteLength = aLength * 2;
		break;
	case EColor16M:
		bitsPerPixel = 24;
		byteLength = aLength * 3;
		break;
	case ERgb:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		bitsPerPixel = 32;
		byteLength = aLength * 4;
		break;
	case ENone:
	default:
		BG_PANIC_ALWAYS(EBitgdiPanicInvalidDisplayMode);
		}

	if (byteLength > aBuf.MaxLength())
		{
		TInt pixelsMaxCount = (aBuf.MaxLength() * 8) / bitsPerPixel;
		if(aLength > pixelsMaxCount)
			{
			aLength = pixelsMaxCount;
			}
		aBuf.SetMax();
		}
	else
		{
		aBuf.SetLength(byteLength);
		}

	TInt x1 = Max(aPixel.iX, deviceRect.iTl.iX);
	TInt x2 = Min(aPixel.iX + aLength, deviceRect.iBr.iX);
	if (x2 <= x1)
		return;

	TUint8* vals = (TUint8*)(aBuf.Ptr());
	iDrawDevice->ReadLine(x1,aPixel.iY,x2 - x1,vals,aDispMode);
	}

void CFbsDevice::TruncateRect(TRect& aRect)
	{
	TInt width = iDrawDevice->SizeInPixels().iWidth << 4;
	TInt height = iDrawDevice->SizeInPixels().iHeight << 4;

	aRect.iTl.iX = Min(aRect.iTl.iX,width);
	aRect.iTl.iY = Min(aRect.iTl.iY,height);
	aRect.iBr.iX = Min(aRect.iBr.iX,width);
	aRect.iBr.iY = Min(aRect.iBr.iY,height);

	width =- width;
	height =- height;

	aRect.iTl.iX = Max(aRect.iTl.iX,width);
	aRect.iTl.iY = Max(aRect.iTl.iY,height);
	aRect.iBr.iX = Max(aRect.iBr.iX,width);
	aRect.iBr.iY = Max(aRect.iBr.iY,height);
	}

TBool CFbsDevice::SetOrientation(CFbsBitGc::TGraphicsOrientation aOrientation)
	{
	TBool ret = iDrawDevice->SetOrientation((CFbsDrawDevice::TOrientation)aOrientation);
	if(ret)
		iOrientation = aOrientation;
	return ret;
	}

/**
Depending on the current graphics hardware this 
will return one of the 16M video modes defined in
TDisplayMode, or ENone if a 16M video mode is not supported.
@see TDisplayMode
@return	a 16M display mode or ENone.
*/
EXPORT_C TDisplayMode CFbsDevice::DisplayMode16M()
	{
	return CFbsDrawDevice::DisplayMode16M();
	}

/**
Sets scaling factor by which the drawing device should scale the drawing images.
If you want to un-scale the device, call SetScalingFactor() with Origin (0,0), 
factorX = 1, factorY = 1, divisorX = 1, divisorY = 1.

Note: The existing graphics acceleration interface does not have support for scaling.

Note: All graphics contexts, already created by the scaled device, should be 
re-activated calling CFbsBitGc::Activate().

@param aOrigin Specifies physical coordinates of the new scaling origin
of the drawing device. The drawing device maps the logical point [0, 0] to
the "aOrigin" physical point .
@param aFactorX Scaling factor for the X-axis of the screen device.
@param aFactorY Scaling factor for the y-axis of the screen device.
@param aDivisorX Not used. Should be set to 1.
@param aDivisorY Not used. Should be set to 1.
@return KErrNone If the method succeeds.
        KErrNotSupported The drawing device does not have scaling capabilities.
*/
EXPORT_C TInt CFbsDevice::SetScalingFactor(const TPoint& aOrigin,
										   TInt aFactorX, TInt aFactorY,
										   TInt aDivisorX, TInt aDivisorY)
	{
	//This class takes care about setting scaling and origin and restoring the original values
	//if some of the operations fails.
	class TInitializer
		{
	public:
		TInitializer(CFbsDrawDevice* aDrawDevice) :
			iScalingSettings(NULL), iOriginInterface(NULL),
			iError(KErrNone),
			iFx(1), iFy(1), iDivX(1), iDivY(1),
			iOrigin(0, 0)
			{//Acquire the interfaces and save the original settings.  They will be used if the initialization fails.
			iError = aDrawDevice->GetInterface(KScalingSettingsInterfaceID, 
											   reinterpret_cast <TAny*&> (iScalingSettings));
			if(iError == KErrNone)
				{
				BG_ASSERT_DEBUG_INVARIANT(iScalingSettings);
				iScalingSettings->Get(iFx, iFy, iDivX, iDivY);
				iError = aDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, 
												   reinterpret_cast <TAny*&> (iOriginInterface));
				if(iError == KErrNone)
					{
					BG_ASSERT_DEBUG_INVARIANT(iOriginInterface);
					iOriginInterface->Get(iOrigin);
					}
				}
			}
		~TInitializer()
			{//Restore the original settings if setting of the new ones had failed.
			if(iError != KErrNone)
				{
				if(iScalingSettings)
					{
					(void)iScalingSettings->Set(iFx, iFy, iDivX, iDivY);
					}
				if(iOriginInterface)
					{
					(void)iOriginInterface->Set(iOrigin);
					}
				}
			}
		void SetScalingFactor(TInt aFactorX, TInt aFactorY, TInt aDivisorX, TInt aDivisorY)
			{
			if(iError == KErrNone)
				{
				iError = iScalingSettings->Set(aFactorX, aFactorY, aDivisorX, aDivisorY);
				}
			}
		void SetOrigin(const TPoint& aOrigin)
			{
			if(iError == KErrNone)
				{
				iError = iOriginInterface->Set(aOrigin);
				}
			}
		TInt Error() const
			{
			return iError;
			}
	private:
		MScalingSettings* iScalingSettings;
		MDrawDeviceOrigin* iOriginInterface;
		TInt iError;
		TInt iFx, iFy, iDivX, iDivY;
		TPoint iOrigin;
		};
	TInitializer initializer(iDrawDevice);
	initializer.SetScalingFactor(aFactorX, aFactorY, aDivisorX, aDivisorY);
	initializer.SetOrigin(aOrigin);
	TInt err = initializer.Error();
	if(err == KErrNone && 
	  (aFactorX != aDivisorX || aFactorY != aDivisorY || aOrigin.iX != 0 || aOrigin.iY != 0))
		{
		//Graphics accelerator interface doesn't have support for scaling&origin.
		delete iGraphicsAccelerator;
		iGraphicsAccelerator = NULL;
		}
	return err;
	}

/**
Gets logical coordinates of the drawing rectangle.
If the device is not scaled, logocal coordinates of the drawing rectangle are the
same as its physical coordinates.
@param aRect Upon return aRect contains drawing rectangle logical coordinates.
*/
EXPORT_C void CFbsDevice::GetDrawRect(TRect& aRect) const
	{
	iDrawDevice->GetDrawRect(aRect);
	}

/**
An overloaded version of DrawingBegin(TBool). Similarly to that method, calls to
DrawingBegin(const CFbsBitmap*, TBool) must be paired with a subsequent call to
DrawingEnd(const CFbsBitmap*, TBool). Also, code must not leave between a
DrawingBegin(const CFbsBitmap*, TBool) - DrawingEnd(const CFbsBitmap*, TBool) pair.
@param aBitmap An additional parameter compared to the basic overload.
aBitmap is a pointer to a CFbsBitmap object to be frozen. If a null pointer
is given then the method panics.
@param aAlways Not used.

@see CFbsBitmapDevice::DrawingBegin()
*/
void CFbsDevice::DrawingBegin(const CFbsBitmap* aBitmap, TBool /*aAlways*/)
	{
	BG_ASSERT_DEBUG(aBitmap, EBitgdiPanicInvalidBitmap);
	if (aBitmap)
		aBitmap->BeginDataAccess();
	DrawingBegin();
	}

/**
This must always be called after DrawingBegin(const CFbsBitmap*, TBool).
Like DrawingBegin(const CFbsBitmap*, TBool) it will panic in debug builds
if passed a null pointer.
@param aBitmap A pointer to a CFbsBitmap object to be unfrozen.
@param aAlways Not used.

@see CFbsBitmapDevice::DrawingEnd()
*/
void CFbsDevice::DrawingEnd(const CFbsBitmap* aBitmap, TBool /*aAlways*/)
	{
	BG_ASSERT_DEBUG(aBitmap, EBitgdiPanicInvalidBitmap);
	if (aBitmap)
		aBitmap->EndDataAccess(ETrue);
	DrawingEnd();
	}

/**
Used to set an offset
@param aOrigin The offset to give
@return KErrNone on success, otherwise a system wide error code
*/
EXPORT_C TInt CFbsDevice::SetDrawDeviceOffset(const TPoint& aOrigin)
	{
	if(!iDrawDevice)
		{
		return KErrGeneral;
		}
	MDrawDeviceOrigin* drawDevice=NULL;
	if (	iDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID,reinterpret_cast<void*&>(drawDevice))<KErrNone
		||	drawDevice==NULL
			)
		{
		return KErrNotSupported;
		}
	drawDevice->Set(aOrigin);
	return KErrNone;
	}
