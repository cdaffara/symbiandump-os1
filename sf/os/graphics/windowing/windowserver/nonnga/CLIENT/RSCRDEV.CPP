// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Shells for window server screen device
// 
//

#include <e32std.h>
#include <e32hal.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"
#include "w32comm.h"
#include "scrdevextension.h"

const TInt KDefaultScreenNumber = 0 ;

EXPORT_C CWsScreenDevice::CWsScreenDevice()
/** Default constructor. Developers should use the other constructor overload. */
	{
	}

EXPORT_C CWsScreenDevice::CWsScreenDevice(RWsSession &aWs) : MWsClientClass(aWs.iBuffer)
/** Constructs a new screen device attached to a particular window server session.

@param aWs The window server session this screen should be attached to. */
	{
	}

EXPORT_C TInt CWsScreenDevice::CreateContext(CGraphicsContext *&aGc)
/** Creates a graphics context for this device.

This function always causes a flush of the window server buffer.

@param aGc On successful return, contains a new graphics context referring 
to this screen device. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see CGraphicsDevice::CreateContext() */
	{
	if ((aGc=new CWindowGc(this))==NULL)
		return(KErrNoMemory);
	TInt err=((CWindowGc *)aGc)->Construct();
	if (err!=KErrNone)
		{
		delete aGc;
		aGc=NULL;
		}
	return(err);
	}

EXPORT_C CWsScreenDevice::~CWsScreenDevice()
/** Destructor. */
	{
	if (iBuffer)
		{
		if (iWsHandle)
			Write(EWsSdOpFree);
		}
	if (iExtension)
		{
		delete TypeFaceStore();
		delete iExtension;
		}
	}

#pragma warning(disable : 4710)
/** 
Completes construction of the object.

This method invokes Construct(TInt aDefaultScreenNumber) with default Screen number.
@return KErrNone if successful, otherwise another of the system-wide error codes. 
*/
EXPORT_C TInt CWsScreenDevice::Construct()
	{
	return Construct( KDefaultScreenNumber ) ;
	}


EXPORT_C TInt CWsScreenDevice::Construct(TInt aDefaultScreenNumber)
/** Completes construction of the object.

This function always causes a flush of the window server buffer.
@param aDefaultScreenNumber - This is the screen on which an application will start
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.*/
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TInt ret;
	TWsClCmdCreateScreenDevice createScreenDevice;
	createScreenDevice.screenNumber = aDefaultScreenNumber;
	createScreenDevice.clientScreenDevicePointer = (TUint)this;
	if ( ( ret=iBuffer->WriteReplyWs(&createScreenDevice,sizeof(createScreenDevice),EWsClOpCreateScreenDevice ) ) < 0 )
		{
		iBuffer=NULL;
		}
	else
		{
		iWsHandle=ret;
		//If the extension fails to allocate then clients will be refused access to the extension interface.
		TRAP(ret,iExtension=new(ELeave) CScrDevExtension(iBuffer,iWsHandle));
		if (ret>=KErrNone)
			{
			TRAP(ret,iExtension->SetTypefaceStore(CFbsTypefaceStore::NewL(this)));
			}
		iDisplaySizeInPixels=SizeInPixels();
		iPhysicalScreenSizeInTwips=SizeInTwips();
		if (iDisplaySizeInPixels.iWidth==0)
			{
			TMachineInfoV1Buf macInfo;
			UserHal::MachineInfo(macInfo);
			iPhysicalScreenSizeInTwips=macInfo().iPhysicalScreenSize;
			iDisplaySizeInPixels=macInfo().iDisplaySizeInPixels;
			}
		}
	return(ret);
	}
#pragma warning(default : 4710)

EXPORT_C TDisplayMode CWsScreenDevice::DisplayMode() const
/** Gets the device's display mode.

This function always causes a flush of the window server buffer.

@return The device's display mode. 
@see CGraphicsDevice::DisplayMode() */
	{
	return((TDisplayMode)WriteReply(EWsSdOpDisplayMode));
	}

EXPORT_C TRect CWsScreenDevice::PointerRect() const
/** Gets the active area for the pointing device. 

This is a device-dependent parameter, and will typically depend on the screen 
size and other factors.

This function always causes a flush of the window server buffer.

@return The active area, measured in pixels. */
	{
	TPckgBuf<TRect> rectPkg;
  	WriteReplyP(&rectPkg,EWsSdOpPointerRect);
	return(rectPkg());
	}

EXPORT_C TSize CWsScreenDevice::SizeInPixels() const
/** Gets the size of the screen device area in pixels.

This function always causes a flush of the window server buffer.

@return The x and y dimensions of the screen device area, in pixels. 
@see CGraphicsDevice::SizeInPixels() */
	{
	TPckgBuf<TSize> sizePkg;
  	WriteReplyP(&sizePkg,EWsSdOpPixelSize);
	return(sizePkg());
	}

EXPORT_C TSize CWsScreenDevice::SizeInTwips() const
/** Gets the size of the screen device area in twips.

This function always causes a flush of the window server buffer.

@return The x and y dimensions of the screen device area, in twips. 
@see CGraphicsDevice::SizeInTwips() */
	{
	TPckgBuf<TSize> sizePkg;
  	WriteReplyP(&sizePkg,EWsSdOpTwipsSize);
	return(sizePkg());
	}

EXPORT_C TInt CWsScreenDevice::HorizontalTwipsToPixels(TInt aTwips) const
/** Translates a twips to a pixel value.

@param aTwips The value in twips. 
@return The equivalent number of pixels. 
@see MGraphicsDeviceMap::HorizontalTwipsToPixels() */
	{
	TInt64 twips=aTwips;
	twips=(twips*iDisplaySizeInPixels.iWidth+(iPhysicalScreenSizeInTwips.iWidth/2))/iPhysicalScreenSizeInTwips.iWidth;
	return I64INT(twips);
	}

EXPORT_C TInt CWsScreenDevice::VerticalTwipsToPixels(TInt aTwips) const
/** Translates a vertical dimension of a screen device in twips into pixels.

@param aTwips A vertical dimension of a device in twips. 
@return The vertical dimension in pixels. */
	{
	TInt64 twips=aTwips;
	twips=(twips*iDisplaySizeInPixels.iHeight+(iPhysicalScreenSizeInTwips.iHeight/2))/iPhysicalScreenSizeInTwips.iHeight;
	return I64INT(twips);
	}

EXPORT_C TInt CWsScreenDevice::HorizontalPixelsToTwips(TInt aPixels) const
/** Translates a specified pixel value to a twips value. 

@param aPixels The value in pixels to be translated. 
@return The equivalent number of twips. */
	{
	TInt64 pixels=aPixels;
	pixels=(pixels*iPhysicalScreenSizeInTwips.iWidth+(iDisplaySizeInPixels.iWidth/2))/iDisplaySizeInPixels.iWidth;
	return I64INT(pixels);
	}

EXPORT_C TInt CWsScreenDevice::VerticalPixelsToTwips(TInt aPixels) const
/** Translates a vertical dimension of a screen device in pixels into twips.

@param aPixels A vertical dimension of a device in pixels. 
@return The vertical dimension in twips. 
@see MGraphicsDeviceMap::VerticalPixelsToTwips() */
	{
	TInt64 pixels=aPixels;
	pixels=(pixels*iPhysicalScreenSizeInTwips.iHeight+(iDisplaySizeInPixels.iHeight/2))/iDisplaySizeInPixels.iHeight;
	return I64INT(pixels);
	}

EXPORT_C void CWsScreenDevice::GetPixel(TRgb &aColor,const TPoint &aPixel) const
/** Gets the RGB colour of an individual pixel on a screen device.

This function always causes a flush of the window server buffer.

@param aColor On return, contains the RGB colour of the pixel. 
@param aPixel The x,y co-ordinates of the pixel. The top left pixel is (0,0).
@see CBitmapDevice::GetPixel() */
	{
	aColor.SetInternal(((TUint32)WriteReply(&aPixel,sizeof(aPixel),EWsSdOpPixel)));
	}

EXPORT_C void CWsScreenDevice::GetScanLine(TDes8 &aScanLine,const TPoint &aStartPixel,TInt aPixelLength, TDisplayMode aDispMode) const
/** Gets a scanline into a buffer. 

The pixels are converted from the current screen display mode format 
to the format of the specified device display mode.

By specifying the start pixel and number of pixels either the whole or a portion 
of a row of screen pixels may be copied.

This function always causes a flush of the window server buffer.

@param aScanLine A buffer into which pixels are copied, it must be sufficiently 
large to store all the scanline pixels. 
@param aStartPixel The (x,y) co-ordinates of the first pixel of the bitmap scanline 
to be put into the buffer. 
@param aPixelLength The number of pixels to put into the buffer.
@param aDispMode The display mode into which to convert the pixels. 
@see CBitmapDevice::GetScanLine() */
	{
	TWsSdCmdGetScanLine getScanLine(aStartPixel,aPixelLength,aDispMode);
	WriteReplyP(&getScanLine,sizeof(getScanLine),&aScanLine,EWsSdOpGetScanLine);
	}

EXPORT_C TBool CWsScreenDevice::RectCompare(const TRect &aRect1,const TRect &aRect2) const
/** Compares two areas of the screen to see if they have the same content. 

If there are any sprites on the screen they are not included in the comparison.

This function always causes a flush of the window server buffer.

@param aRect1 A rectangle. 
@param aRect2 Another rectangle. 
@return ETrue if the two screen areas are identical. */
 	{
	return RectCompare(aRect1,aRect2,ERemoveSprite);
	}

EXPORT_C TBool CWsScreenDevice::RectCompare(const TRect &aRect1,const TRect &aRect2,TUint aFlags) const
/** Compares two areas of the screen to see if they have the same content.
 
This function always causes a flush of the window server buffer.

@param aRect1 A rectangle. 
@param aRect2 Another rectangle.
@param aFlags EIncludeSprite to include the sprite in the compare or ERemoveSprite to remove the sprite.
@return ETrue if the two screen areas are identical. */
	{
	TWsSdCmdRectCompare rectCompare(aRect1,aRect2,aFlags);
 	return(WriteReply(&rectCompare,sizeof(rectCompare),EWsSdOpRectCompare));
 	}

EXPORT_C TInt CWsScreenDevice::CopyScreenToBitmap(const CFbsBitmap *aBitmap) const
/** Saves the entire screen to a bitmap.

This function always causes a flush of the window server buffer.

@param aBitmap Bitmap to be filled with the screen image. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	AddToBitmapArray(aBitmap->Handle());
	TWsSdCmdCopyScreenToBitmap rectCompare(aBitmap->Handle());
	return(WriteReply(&rectCompare,sizeof(rectCompare),EWsSdOpCopyScreenToBitmap));
	}

EXPORT_C TInt CWsScreenDevice::CopyScreenToBitmap(const CFbsBitmap *aBitmap, const TRect &aRect) const
/** Saves a region of the screen to a bitmap.

This function always causes a flush of the window server buffer.

@param aBitmap Bitmap to be filled with the screen region image. 
@param aRect Screen region to be saved. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	AddToBitmapArray(aBitmap->Handle());
	TWsSdCmdCopyScreenToBitmap2 rectCompare(aRect, aBitmap->Handle());
	return(WriteReply(&rectCompare,sizeof(rectCompare),EWsSdOpCopyScreenToBitmap2));
	}

EXPORT_C TInt CWsScreenDevice::GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec)
/** Gets the nearest font, in twips, to that in the specified font specification. 

This function is replaced by GetNearestFontToDesignHeightInTwips()

The font and bitmap server returns a pointer to the nearest matching font 
from those available, in aFont.

@param aFont On return, this is set to point to the device font closest to the font 
specification passed in the second argument.
@param aFontSpec An absolute font specification. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see MGraphicsDeviceMap::GetNearestFontInTwips()
@deprecated */
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}
CFbsTypefaceStore* CWsScreenDevice::TypeFaceStore()const
/** Helper member fn to access the movable typeface store.
 **/
	{
	return iExtension->TypefaceStore();
	}

EXPORT_C TInt CWsScreenDevice::GetNearestFontToDesignHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec)
/** Gets the nearest font in twips to that specified.

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to design height of font - this gives no
guarantees on the actual physical size of the font.

This function replaces GetNearestFontInTwips

@param aFont On return, the pointer is set to point to the device font which 
most closely approximates to the required font specification.
@param aFontSpec An absolute font specification. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes.
@see MGraphicsDeviceMap::GetNearestFontToDesignHeightInTwips() */
	{
	return TypeFaceStore()->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Gets the nearest font, in twips, to that specified. 

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
@param aMaxHeight The maximum height within which the font must fit - this
overrides the height specified in the TFontSpec. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes.
@see MGraphicsDeviceMap::GetNearestFontToMaxHeightInTwips() */
EXPORT_C TInt CWsScreenDevice::GetNearestFontToMaxHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec,TInt aMaxHeight)
	{
	return TypeFaceStore()->GetNearestFontToMaxHeightInTwips(aFont, aFontSpec, aMaxHeight);
	}

EXPORT_C TInt CWsScreenDevice::GetNearestFontInPixels(CFont*& aFont,const TFontSpec& aFontSpec)
/** Gets the nearest font to that specified for use by a bitmapped graphics device. 

This function is replaced by GetNearestFontToDesignHeightInPixels()

The font and bitmap server returns a pointer to the nearest matching font 
from those available, in aFont.

@param aFont On return, this is set to point to the device font that is  
closest to the font specification passed in the second argument 
@param aFontSpec An absolute font specification 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see CBitmapDevice::GetNearestFontInPixels()
@deprecated */
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

EXPORT_C TInt CWsScreenDevice::GetNearestFontToDesignHeightInPixels(CFont*& aFont,const TFontSpec& aFontSpec)
/** Gets the nearest font in pixels to that specified. 

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to design height of font - this gives no
guarantees on the actual physical size of the font.

This function replaces GetNearestFontInTwips

@param aFont On return, the pointer is set to point to the device font which 
most closely approximates to the required font specification.
@param aFontSpec An absolute font specification. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes.
@see CBitmapDevice::GetNearestFontToDesignHeightInPixels() */
	{
	return TypeFaceStore()->GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

/** Gets the nearest font in pixels to that specified. 

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
@param aMaxHeight The maximum height within which the font must fit - this
overrides the height specified in the TFontSpec. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes.
@see CBitmapDevice::GetNearestFontToMaxHeightInPixels() */
EXPORT_C TInt CWsScreenDevice::GetNearestFontToMaxHeightInPixels(CFont*& aFont,const TFontSpec& aFontSpec,TInt aMaxHeight)
	{
	return TypeFaceStore()->GetNearestFontToMaxHeightInPixels(aFont, aFontSpec, aMaxHeight);
	}

EXPORT_C TInt CWsScreenDevice::NumTypefaces() const
/** Gets the number of typefaces supported by the screen device.

@return The number of typefaces supported. 
@see CGraphicsDevice::NumTypefaces() */
	{
	return(TypeFaceStore()->NumTypefaces());
	}

EXPORT_C void CWsScreenDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const
/** Gets typeface information for a particular typeface index number.

This information is returned in aTypefaceSupport, and includes: the typeface 
name and typeface attributes (in a TTypeface object), the number of font heights, 
the maximum and minimum font heights and whether it is a scalable typeface.

@param aTypefaceSupport On return, if the function executed successfully, 
this contains the typeface information.
@param aTypefaceIndex A typeface index number, in the range zero to (NumTypefaces() 
- 1). 
@see CGraphicsDevice::TypefaceSupport() */
	{
	TypeFaceStore()->TypefaceSupport(aTypefaceSupport,aTypefaceIndex);
	}

EXPORT_C TInt CWsScreenDevice::FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
/** Gets the height in twips of the specified font. 

The value returned is rounded up or down to the nearest font height in twips.

The specified font is the one with height index number aHeightIndex of the 
typeface with index number aTypefaceIndex.

@param aTypefaceIndex A typeface index number, in the range: 0 to (NumTypefaces() - 1). 
@param aHeightIndex A font height index number, in the range: 0 to (iNumHeights - 1). 
@return The height of the font in twips. 
@see CGraphicsDevice::FontHeightInTwips() */
	{
	return(TypeFaceStore()->FontHeightInTwips(aTypefaceIndex,aHeightIndex));
	}

EXPORT_C TInt CWsScreenDevice::FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const
/** Gets the height of the specified font in pixels.

The value returned is rounded up or down to the nearest font height in pixels.

The specified font is the one with height index number aHeightIndex of the 
typeface with index number aTypefaceIndex.

@param aTypefaceIndex A typeface index number, in the range: 0 to (NumTypefaces() - 1). 
@param aHeightIndex A font height index number, in the range: 0 to (iNumHeights - 1).
@return The height of the font in pixels. 
@see CBitmapDevice::FontHeightInPixels() */
	{
	return(TypeFaceStore()->FontHeightInPixels(aTypefaceIndex,aHeightIndex));
	}

EXPORT_C TInt CWsScreenDevice::GetFontById(CFont *&aFont,TUid aUid,const TAlgStyle& aAlgStyle)
/** Gets a font by its bitmap UID. 

Within a font file each font has its own UID. An algorithmic style is not 
part of the actual font description, but is applied to it. For example algorithmic 
bolding applies an algorithm to increase the apparent weight of each character 
in the font. Note that the algorithm is applied blindly, and that a typeface 
may already have a style e.g. it may already be bold or italic. Thus a bold 
face will appear extra-bold if algorithmic bolding is applied to it. Algorithmic 
effects are not necessarily a substitute for typeface design and should be 
used with care.

@param aFont On a successful return, contains a pointer to the new CFont. 
@param aUid UID of the bitmap font. 
@param aAlgStyle The algorithmic style to apply. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	return(TypeFaceStore()->GetFontById(aFont,aUid,aAlgStyle));
	}

EXPORT_C TInt CWsScreenDevice::AddFile(const TDesC& aName,TInt& aId)
/** Adds a font file to the device's typeface store. The specified font
file must be accessible to any process, i.e. not located inside an
application's private directory.

@param aName Name of the font file. 
@param aId ID for the font file. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see CBitmapDevice::AddFile() */
	{
	return(TypeFaceStore()->AddFile(aName, aId));
	}

EXPORT_C void CWsScreenDevice::RemoveFile(TInt aId)
/** Removes a font file from the font store.

@param aId The ID of the font file to be removed, default 0. 
@see CBitmapDevice::RemoveFile() */
	{
	TypeFaceStore()->RemoveFile(aId);
	}

EXPORT_C void CWsScreenDevice::ReleaseFont(CFont* aFont)
/** Releases a specified font.

This function is used to indicate that the specified font is no longer needed 
for use by the screen device. As fonts can be shared between applications, 
this function does not delete the copy of the font from RAM, unless the font 
was only being used by this device.

@param aFont A pointer to the font to be released. 
@see MGraphicsDeviceMap::ReleaseFont() */
	{
	TypeFaceStore()->ReleaseFont(aFont);
	}

EXPORT_C void CWsScreenDevice::PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
/** Gets the attributes of the device's palette.
 
This function always causes a flush of the window server buffer.

@param aModifiable On return, indicates whether or not the device's palette 
is modifiable (true) or fixed (false).
@param aNumEntries On return, holds the number of entries in the device's 
palette. 
@see CFbsScreenDevice::PaletteAttributes() */
	{
	TInt ret=WriteReply(EWsSdOpPaletteAttributes);
	aModifiable=ret&EWsSdSetableBitFlag;
	aNumEntries=ret&(~EWsSdSetableBitFlag);
	}

EXPORT_C void CWsScreenDevice::SetPalette(CPalette* aPalette)
/** Sets the screen device's palette.

This function always causes a flush of the window server buffer.

Use of this function is deprecated. SetCustomPalette() should be used instead.

@param aPalette The screen device's new palette. */
	{
#if defined(__WINS__)
	__ASSERT_DEBUG(SetCustomPalette(aPalette)==KErrNone,Panic(EW32PanicSilentFail));
#endif
	SetCustomPalette(aPalette);
	}

EXPORT_C TInt CWsScreenDevice::SetCustomPalette(const CPalette* aPalette)
/** Sets the custom palette.
 
This function always causes a flush of the window server buffer.

@param aPalette The custom palette.
@return KErrNone if sucessful, or one of the system error codes.
@panic W32 6 aPalette is NULL.
@capability WriteDeviceData */
	{
	__ASSERT_ALWAYS(aPalette,Panic(EW32PanicNullPalette));
	TPtr8 palette(NULL,0);
	CONST_CAST(CPalette*,aPalette)->GetDataPtr(0,aPalette->Entries(),palette);
	TPtr8 empty(NULL,0);
	return WriteReplyByProvidingRemoteReadAccess(&empty, sizeof(empty), &palette, EWsSdOpSetPalette);
	}

EXPORT_C TInt CWsScreenDevice::GetPalette(CPalette*& aPalette) const
/** Gets the screen device's palette.
 
This function always causes a flush of the window server buffer.

@param aPalette On return, contains the screen device's palette. The caller 
takes responsibility for discarding the palette.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see CFbsScreenDevice::GetPalette() */
	{
	TBool modifiable;		//Dummy parameter
	TInt numEntries;
	TInt ret;
	PaletteAttributes(modifiable,numEntries);
	aPalette=NULL;
Retry:
	TRAP(ret,aPalette=CPalette::NewL(numEntries));
	if (ret==KErrNone)
		{
		TPtr8 palette(NULL,0);
		aPalette->GetDataPtr(0,numEntries,palette);
		ret=WriteReplyIntP(numEntries,&palette,EWsSdOpGetPalette);
		if (ret!=KErrNone)
			{
			delete aPalette;
			aPalette=NULL;
			if (ret>0)		//The mode of the screen display changed
				{
				numEntries=ret;
				goto Retry;
				}
			}
		}
	return ret;
	}

EXPORT_C void CWsScreenDevice::SetScreenSizeAndRotation(const TPixelsTwipsAndRotation &aSizeAndRotation)
/** Sets the current screen size in twips and pixels, and the rotation for the 
screen device.

@param aSizeAndRotation The new rotation and the screen size in both pixels 
and twips. */
	{
	// Need to reset the cache if the ratio of twip/pixels changes
	// Using multiply as this is much quicker than divide on ARM
	// No need to flush cache if this is a width/height swap
	if((iPhysicalScreenSizeInTwips.iWidth * aSizeAndRotation.iPixelSize.iWidth != aSizeAndRotation.iTwipsSize.iWidth * iDisplaySizeInPixels.iWidth
		|| iPhysicalScreenSizeInTwips.iHeight * aSizeAndRotation.iPixelSize.iHeight != aSizeAndRotation.iTwipsSize.iHeight * iDisplaySizeInPixels.iHeight)
		&& !(aSizeAndRotation.iPixelSize.iHeight == iDisplaySizeInPixels.iWidth
		&& aSizeAndRotation.iPixelSize.iWidth == iDisplaySizeInPixels.iHeight
		&& iPhysicalScreenSizeInTwips.iHeight == aSizeAndRotation.iTwipsSize.iWidth
		&& iPhysicalScreenSizeInTwips.iWidth == aSizeAndRotation.iTwipsSize.iHeight))
		{
		TypeFaceStore()->ReleaseTwipsCache();
		}
	Write(&aSizeAndRotation,sizeof(aSizeAndRotation),EWsSdOpSetScreenSizeAndRotation);
	iDisplaySizeInPixels=aSizeAndRotation.iPixelSize;
	iPhysicalScreenSizeInTwips=aSizeAndRotation.iTwipsSize;
	}

EXPORT_C void CWsScreenDevice::GetDefaultScreenSizeAndRotation(TPixelsTwipsAndRotation &aSizeAndRotation) const
/** Gets the current screen size (in both pixels and twips) and rotation.

This function always causes a flush of the window server buffer.

@param aSizeAndRotation The default screen size and rotation, defining screen 
size in both pixels and twips. */
	{
	TPckgBuf<TPixelsTwipsAndRotation> sarPkg;
  	WriteReplyP(&sarPkg,EWsSdOpGetDefaultScreenSizeAndRotation);
	aSizeAndRotation=sarPkg();
	}

EXPORT_C void CWsScreenDevice::SetScreenSizeAndRotation(const TPixelsAndRotation &aSizeAndRotation)
/** Sets the current screen size in pixels, and the rotation for the screen device.

This function always causes a flush of the window server buffer.

@param aSizeAndRotation The new rotation and the screen size in pixels. */
	{
	if(iDisplaySizeInPixels != aSizeAndRotation.iPixelSize
		&& !(aSizeAndRotation.iPixelSize.iHeight == iDisplaySizeInPixels.iWidth
		&& aSizeAndRotation.iPixelSize.iWidth == iDisplaySizeInPixels.iHeight))
		{
		// Reset the twips cache.
		TypeFaceStore()->ReleaseTwipsCache();
		}
	Write(&aSizeAndRotation,sizeof(aSizeAndRotation),EWsSdOpSetScreenSizeAndRotation2);
	iDisplaySizeInPixels=aSizeAndRotation.iPixelSize;
	iPhysicalScreenSizeInTwips=SizeInTwips();
	}

EXPORT_C void CWsScreenDevice::GetDefaultScreenSizeAndRotation(TPixelsAndRotation &aSizeAndRotation) const
/** Gets the current screen size (in pixels) and the rotation.

This function always causes a flush of the window server buffer.

@param aSizeAndRotation The default screen size in pixels and the rotation. */
	{
	TPckgBuf<TPixelsAndRotation> sarPkg;
  	WriteReplyP(&sarPkg,EWsSdOpGetDefaultScreenSizeAndRotation2);
	aSizeAndRotation=sarPkg();
	}

EXPORT_C TDisplayMode CWsScreenDevice::GetScreenModeDisplayMode(const TInt &aMode) const
/** Gets the display mode of the screen for the specified screen mode

This function always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the display mode is required
@return The display mode for the specified screen mode. */
	{
	return STATIC_CAST(TDisplayMode,WriteReplyInt(aMode,EWsSdOpGetScreenModeDisplayMode));
	}
 
EXPORT_C TPoint CWsScreenDevice::GetDefaultScreenModeOrigin() const
/** Gets the origin for the current screen mode

This function always causes a flush of the window server buffer.

@param aOrigin The default offset of the current 
screen mode from the physical screen. */
	{
	TPckgBuf<TPoint> pntPkg;
  	WriteReplyP(&pntPkg,EWsSdOpGetDefaultScreenModeOrigin);
	return pntPkg();
	}

EXPORT_C TPoint CWsScreenDevice::GetScreenModeOrigin(TInt aMode) const
/** Get the origin of the screen for the specified screen mode.

This function always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the screen origin is required. 
@param aOrigin The origin of the specified screen mode. */
	{
	TPckgBuf<TPoint> pntPkg;
	WriteReplyP(&aMode,sizeof(aMode),&pntPkg,EWsSdOpGetScreenModeOrigin);
	return pntPkg();
	}

EXPORT_C TSize CWsScreenDevice::GetScreenModeScale(TInt aMode) const
/** Gets the scale for the specified screen mode.

This function always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the screen scale is required. 
@return The scale for the specified screen mode. */
	{
	TPckgBuf<TSize> pntPkg;
	WriteReplyP(&aMode,sizeof(aMode),&pntPkg,EWsSdOpGetScreenModeScale);
	return pntPkg();
	}

EXPORT_C TSize CWsScreenDevice::GetCurrentScreenModeScale() const
/** Gets the scale for the current screen mode.

This function always causes a flush of the window server buffer.

@return The scale for the current screen mode. */
	{
	TPckgBuf<TSize> pntPkg;
	WriteReplyP(&pntPkg,EWsSdOpGetCurrentScreenModeScale);
	return pntPkg();
	}

EXPORT_C TPoint CWsScreenDevice::GetCurrentScreenModeScaledOrigin() const
/** Gets the current screen mode's scaled origin.

The formula used is (A+B-1)/B

where:

- A is the screen mode origin in physical coordinates,
- B is the screen mode scale width.

The result obtained is the scaled origin of the present screen mode.

@return The scaled origin for the current screen mode. */
	{
	TPckgBuf<TPoint> pntPkg;
	WriteReplyP(&pntPkg,EWsSdOpGetCurrentScreenModeScaledOrigin);
	return pntPkg();
	}

EXPORT_C TPoint CWsScreenDevice::GetScreenModeScaledOrigin(TInt aMode) const
/** Gets the specfied screen mode's scaled origin.

The functionality is same as CWsScreenDevice::GetCurrentScreenModeScaledOrigin().

It always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the scaled origin is required. 
@return The scaled origin for the specified screen mode. */
	{
	TPckgBuf<TPoint> pntPkg;
	WriteReplyP(&aMode,sizeof(aMode),&pntPkg,EWsSdOpGetScreenModeScaledOrigin);
	return pntPkg();
	}

/**
@internalComponent
@released

Used for testing purposes only.

@return The present screen mode.
*/
EXPORT_C TSizeMode CWsScreenDevice::GetCurrentScreenModeAttributes() const
	{
	TPckgBuf<TSizeMode> pntPkg;
	WriteReplyP(&pntPkg,EWsSdOpGetCurrentScreenModeAttributes);
	return pntPkg();
	}

/**
@internalComponent
@released

Used for testing purposes only.

@param aModeAtt Screen size mode to be set.
*/
EXPORT_C void CWsScreenDevice::SetCurrentScreenModeAttributes(const TSizeMode &aModeAtt)
	{
	Write(&aModeAtt,sizeof(aModeAtt),EWsSdOpSetCurrentScreenModeAttributes);
	}

EXPORT_C void CWsScreenDevice::SetAppScreenMode(TInt aMode)
/** Sets the application's screen mode; this also sets all the attributes
of the screen mode.

Note: although this API was added in Symbian OS v8.0, the functionality is 
only available from Symbian OS v8.1 onwards.

@param aMode The index of the application's new screen mode.*/
	{
	WriteInt(aMode,EWsSdOpSetAppScreenMode);
	}

EXPORT_C TInt CWsScreenDevice::NumScreenModes() const
/** Gets the number of available screen modes. 

Each mode has a different size, and one or more possible rotations/orientations.

This function always causes a flush of the window server buffer.

@return The number of screen modes. */
	{
	return(WriteReply(EWsSdOpGetNumScreenModes));
	}

EXPORT_C TInt CWsScreenDevice::CurrentScreenMode() const
/** Gets the current screen mode index.

This function always causes a flush of the window server buffer.

@return The index into the list of available screen modes of the current screen 
mode. */
	{
	return WriteReply(EWsSdOpGetScreenMode);
	}

EXPORT_C void CWsScreenDevice::SetScreenMode(TInt aMode)
/**
@publishedPartner
@released

Sets the current screen mode.

Note that this function is only useful for testing. This is because the screen mode 
normally reflects the state of real hardware, e.g. whether the cover is open 
or closed on a phone that supports screen flipping.
 
This function always causes a flush of the window server buffer.

@param aMode The screen mode index, starting from zero.
@capability WriteDeviceData */
	{
	WriteInt(aMode,EWsSdOpSetScreenMode);
	iDisplaySizeInPixels=SizeInPixels();
	iPhysicalScreenSizeInTwips=SizeInTwips();
	}

EXPORT_C void CWsScreenDevice::GetScreenModeSizeAndRotation(TInt aMode, TPixelsTwipsAndRotation &aSizeAndRotation) const
/** Get the screen rotation and size, in both pixels and twips, for the specified 
screen mode.
 
This function always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the screen size and rotation 
are required. 
@param aSizeAndRotation The orientation of the specified screen mode, and its 
size in both pixels and twips. */
	{
	TPckgBuf<TPixelsTwipsAndRotation> sarPkg;
  	WriteReplyP(&aMode,sizeof(aMode),&sarPkg,EWsSdOpGetScreenModeSizeAndRotation);
	aSizeAndRotation=sarPkg();
	}

EXPORT_C void CWsScreenDevice::GetScreenModeSizeAndRotation(TInt aMode, TPixelsAndRotation &aSizeAndRotation) const
/** Get the screen rotation and size (in pixels) for the specified screen mode.
 
This function always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the screen size and rotation 
are required. 
@param aSizeAndRotation The orientation of the specified screen mode, and its 
size in pixels. */
	{
	TPckgBuf<TPixelsAndRotation> sarPkg;
  	WriteReplyP(&aMode,sizeof(aMode),&sarPkg,EWsSdOpGetScreenModeSizeAndRotation2);
	aSizeAndRotation=sarPkg();
	}

EXPORT_C void CWsScreenDevice::SetCurrentRotations(TInt aMode, CFbsBitGc::TGraphicsOrientation aRotation) const
/** 
@publishedPartner
@released

Sets the screen rotation that should be used with a particular screen size. 

After calling this function, whenever you change into the screen size specified 
by aMode you will have the rotation aRotation. The setting remains in force 
until it is explicitly changed using this function. 

Panics if the specified rotation is not allowed by the given screen mode.

@param aMode The index of the screen mode the rotation applies to. 
@param aRotation The new screen orientation.
@see GetRotationsList()
@capability WriteDeviceData */
	{
	TWsSdCmdSetScreenRotation screenRotation(aMode,aRotation);
	Write(&screenRotation,sizeof(screenRotation),EWsSdOpSetModeRotation);
	}

EXPORT_C TInt CWsScreenDevice::GetRotationsList(TInt aMode, CArrayFixFlat<TInt> *aRotationList) const
/** Gets the list of valid rotations for a particular screen size.

The list is initialised in wsini.ini.
 
This function always causes a flush of the window server buffer.

@param aMode The index of the screen mode for which the rotation list 
is required. 
@param aRotationList The list of valid screen orientations. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see SetCurrentRotations() */
	{
	TUint modeList=(TUint)WriteReplyInt(aMode,EWsSdOpGetRotationList);
	TUint modeBit=1<<CFbsBitGc::EGraphicsOrientationNormal;
	TBool rots[4];
	TInt rotations=0;
	TInt ii;
	for (ii=0;ii<4;ii++)
		{
		rots[ii]=(modeList&modeBit);
		if (rots[ii])
			++rotations;
		modeBit=modeBit<<1;
		}
	if (!aRotationList)
		return rotations;
	TRAPD(err,aRotationList->ResizeL(rotations));
	if (err!=KErrNone)
		return(err);
	rotations=0;
	for (ii=0;ii<4;ii++)
		{
		if (rots[ii])
			(*aRotationList)[rotations++]=REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,ii);
		}
	return KErrNone;
	}

EXPORT_C TScreenModeEnforcement CWsScreenDevice::ScreenModeEnforcement() const
/** Gets the current screen mode enforcement settings.

The global screen mode enforcement setting defines the requirements that a 
group window must meet to be displayed. The requirements may have been set 
in wsini.ini, or using SetScreenModeEnforcement(). 
 
This function always causes a flush of the window server buffer.

@return The screen mode enforcement requirements. */
	{
	return((TScreenModeEnforcement)WriteReply(EWsSdOpScreenModeEnforcement));
	}

EXPORT_C void CWsScreenDevice::SetScreenModeEnforcement(TScreenModeEnforcement aMode) const
/** 
@publishedPartner
@released

Sets the screen mode enforcement requirements.

This global setting defines the requirements that a group window must meet 
to be displayed. The value may be set using this function, but is more likely 
to be defined in wsini.ini. 

@param aMode The screen mode enforcement requirements.
@capability WriteDeviceData */
	{
	WriteInt(aMode,EWsSdOpSetScreenModeEnforcement);
	}

EXPORT_C TInt CWsScreenDevice::GetScreenNumber() const
/** Get device's screen number 

@return The device's screen number
@see CWsScreenDevice::Construct( TInt aDefaultScreenNumber ) */
	{
	return WriteReply(EWsSdOpGetScreenNumber);
	}

/** Gets the available screen size modes.

This function retrieves all available screen size modes which are supported by
the server.

@param aModeList On return, the list of available screen size modes.
@return The number of supported screen size modes if successful otherwise returns KErrNoMemory if 
there is insufficient memory to create the array. */
EXPORT_C TInt CWsScreenDevice::GetScreenSizeModeList(RArray<TInt>* aModeList) const
	{
	__ASSERT_ALWAYS(aModeList, Panic(EW32PanicNullArray));
	aModeList->Reset();
	TInt count=WriteReply(EWsSdOpGetNumScreenModes);
	TInt totSize=count*sizeof(TInt);
	TInt* allocMem=static_cast<TInt*>(User::Alloc(totSize));
	if(allocMem==NULL)
		{
		return KErrNoMemory;
		}
	TPtr8 listPtr(reinterpret_cast<TUint8*>(allocMem), totSize);
	count=WriteReplyP(&listPtr, EWsSdOpGetScreenSizeModeList);
	new(aModeList) RArray<TInt>(allocMem, count);
	return count;	
	}	

EXPORT_C TInt CWsScreenDevice::SetBackLight(TBool aBackLight)
/** Set back light. 
@param aBackLight, ETrue Set the backlight on, EFlase set the backlight off.
@capability EikServ SID */
	{
	return(WriteReplyInt(aBackLight,EWsClOpSetBackLight));
	}

/** Interface Extension capability
	Use of this interface going forward will allow the published client interface to be dynamically extended.
	Note that the pointer returned is only good for the lifetime of the called CBase derived object.

	@param  aInterfaceId	uniqueid or well known id of interface
	@return	pointer to interface object matching this ID or NULL if no match.
*/
EXPORT_C TAny* CWsScreenDevice::GetInterface(TUint aInterfaceId)
	{
	return iExtension->GetInterface(aInterfaceId);
	}

/** Returns whether the given screen size mode is dynamic or not.
	Dynamic screen size modes may change their size in pixels and/or twips 
	and other attributes at run time, so they must not be cached. Static size 
	mode attributes will not change at run time, but may not make full use of the display.
	Invalid size modes shall return EFalse.

	@param aSizeMode The screen size mode to check.
	@return ETrue if the given screen size mode is dynamic, EFalse otherwise.
*/
EXPORT_C TBool CWsScreenDevice::IsModeDynamic(TInt /*aSizeMode*/) const
	{
	return EFalse;
	}

/** Returns whether the current screen size mode is dynamic or not.

	@return ETrue if current screen size mode is dynamic, EFalse otherwise.
	@see IsModeDynamic
*/
EXPORT_C TBool CWsScreenDevice::IsCurrentModeDynamic() const
	{
	return EFalse;
	}
