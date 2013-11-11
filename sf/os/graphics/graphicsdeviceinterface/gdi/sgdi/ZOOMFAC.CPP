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
//

#include <gdi.h>
#include "GDIPANIC.h"

_LIT(KGdiTZoomFactorPanicCategory,"TZoomFactor");

//
// TZoomFactor
//

 
EXPORT_C TZoomFactor::TZoomFactor():
	iZoomFactor(EZoomOneToOne),
	iDevice(NULL)
/** Constructs a default zoom factor object.

Note that a TZoomFactor object cannot be used until a CGraphicsDevice to which 
it is associated is specified (by SetGraphicsDeviceMap()). Therefore the other 
constructor is normally used for constructing TZoomFactors. The default constructor 
function is provided for use in TZoomFactor-derived classes. */
	{}

 
EXPORT_C TZoomFactor::~TZoomFactor()
/** Destructor.

Frees resources owned by the object, prior to its destruction. */
	{}

 
EXPORT_C TInt TZoomFactor::ZoomFactor() const
/** Gets the zoom factor.

@return The zoom factor */
	{
	return iZoomFactor;
	}

 
EXPORT_C void TZoomFactor::SetZoomFactor(TInt aZoomFactor)
/** Sets the zoom factor.

@param aZoomFactor The desired zoom factor. */
	{
	iZoomFactor=aZoomFactor;
	}

 
EXPORT_C void TZoomFactor::SetTwipToPixelMapping(const TSize& aSizeInPixels,const TSize& aSizeInTwips)
// Ensure that aSizeInTwips fits inside aSizeInPixels when zoomed
/** Sets the twips to pixels mapping for the graphics device with which the zoom 
factor is associated.

This setting is used by all the twips to pixels and pixels to twips conversion 
functions.

@param aSizeInPixels The size of the graphics device area in pixels. 
@param aSizeInTwips The size of the graphics device area in twips. */
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice!=NULL,User::Panic(KGdiTZoomFactorPanicCategory,KErrNotFound));
	TInt devicepixelwidth=iDevice->HorizontalTwipsToPixels(aSizeInTwips.iWidth);
	TInt devicepixelheight=iDevice->VerticalTwipsToPixels(aSizeInTwips.iHeight);
	if(devicepixelwidth==0 || devicepixelheight==0)
		{
		iZoomFactor=0;
		return;
		}
	iZoomFactor = (1000*aSizeInPixels.iWidth+(devicepixelwidth/2))/devicepixelwidth;
	TInt temp = (1000*aSizeInPixels.iHeight+(devicepixelheight/2))/devicepixelheight;
	if (temp<iZoomFactor)
		iZoomFactor = temp;
	}

EXPORT_C TInt TZoomFactor::HorizontalTwipsToPixels(TInt aTwipWidth) const
/** Converts a horizontal dimension from twips to pixels on the graphics
device.

This function implements the pure virtual function defined in
MGraphicsDeviceMap::HorizontalTwipsToPixels() */
	{
	// iZoomFactor expressed in units of a thousandth
	// delegated conversion is done 8 times multiplied for 3 guard bits
	// hence result divided by 8
	GDI_ASSERT_ALWAYS_GENERAL(iDevice!=NULL,User::Panic(KGdiTZoomFactorPanicCategory,KErrNotFound));
	return (iDevice->HorizontalTwipsToPixels(((aTwipWidth*iZoomFactor)/125))+((aTwipWidth<0)?-4:4))/8;
	}


EXPORT_C TInt TZoomFactor::VerticalTwipsToPixels(TInt aTwipHeight) const
/** Converts a vertical dimension from twips to pixels on the graphics
device.

This function implements the pure virtual function defined in
MGraphicsDeviceMap::VerticalTwipsToPixels() */	
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice!=NULL,User::Panic(KGdiTZoomFactorPanicCategory,KErrNotFound));
	return (iDevice->VerticalTwipsToPixels(((aTwipHeight*iZoomFactor)/125))+((aTwipHeight<0)?-4:4))/8;
	}


EXPORT_C TInt TZoomFactor::HorizontalPixelsToTwips(TInt aPixelWidth) const 
/** Converts a horizontal dimension from pixels to twips on the graphics
device.

This function implements the pure virtual function defined in
MGraphicsDeviceMap::HorizontalPixelsToTwips() */	
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice!=NULL,User::Panic(KGdiTZoomFactorPanicCategory,KErrNotFound));
	TInt temp = iDevice->HorizontalPixelsToTwips(aPixelWidth*8)*1000;
	temp+=(temp<0)?-(iZoomFactor*4):iZoomFactor*4;
	TInt eightTimesZoom=iZoomFactor*8;
	if(eightTimesZoom)
		return temp/eightTimesZoom;
	return(0);
	}

EXPORT_C TInt TZoomFactor::VerticalPixelsToTwips(TInt aPixelHeight) const
/**  Converts a vertical dimension from pixels to twips on the graphics
device.

This function implements the pure virtual function defined in
MGraphicsDeviceMap::VerticalPixelsToTwips() */	
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice!=NULL,User::Panic(KGdiTZoomFactorPanicCategory,KErrNotFound));
	TInt temp = iDevice->VerticalPixelsToTwips(aPixelHeight*8)*1000;
	temp+=(temp<0)?-(iZoomFactor*4):iZoomFactor*4;
	TInt eightTimesZoom=iZoomFactor*8;
	if(eightTimesZoom)
		return temp/eightTimesZoom;
	return(0);
	}

/**
@publishedAll
*/
EXPORT_C TInt TZoomFactor::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.
Matching to design height gives no guarantees on the actual physical size of the font.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt TZoomFactor::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice, User::Panic(KGdiTZoomFactorPanicCategory, KErrNotFound));
	TFontSpec fontSpec = aFontSpec;
	fontSpec.iHeight = (fontSpec.iHeight * iZoomFactor + EZoomOneToOne / 2) / EZoomOneToOne;
	return (const_cast<MGraphicsDeviceMap*>(iDevice))->GetNearestFontToDesignHeightInTwips(aFont, fontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.
Matching to maximum height returns a font that will fit within the height specified.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@param aMaxHeight The maximum height within which the font must fit.
This overrides the height specified in aFontSpec. If maximum height
is greater than 1024 pixels, the function returns KErrTooBig. And 
returns KErrArgument if equals to 1 pixel.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt TZoomFactor::GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice, User::Panic(KGdiTZoomFactorPanicCategory, KErrNotFound));
	TFontSpec fontSpec = aFontSpec;
	fontSpec.iHeight = (fontSpec.iHeight * iZoomFactor + EZoomOneToOne / 2) / EZoomOneToOne;
	aMaxHeight = (aMaxHeight * iZoomFactor + EZoomOneToOne / 2) / EZoomOneToOne;
	return (const_cast<MGraphicsDeviceMap*>(iDevice))->GetNearestFontToMaxHeightInTwips(aFont, fontSpec, aMaxHeight);
	}

/** Releases the specified font.

This function implements the pure virtual function defined in
MGraphicsDeviceMap::ReleaseFont() 
@param aFont A pointer to the font to be released. */	
EXPORT_C void TZoomFactor::ReleaseFont(CFont* aFont)
	{
	GDI_ASSERT_ALWAYS_GENERAL(iDevice != NULL, User::Panic(KGdiTZoomFactorPanicCategory, KErrNotFound));
	((MGraphicsDeviceMap*)iDevice)->ReleaseFont(aFont);
	}
