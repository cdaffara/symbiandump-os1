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

/**
 Creates a font and bitmap server graphics context for the device and activates 
 it.
 
 It is the responsibility of the caller to delete the graphics context when 
 it is no longer needed.
 
 @param aGc On return, contains a pointer to the graphics context. 
 @return KErrNone if successful, otherwise, another one of the system-wide error 
 codes. 
*/
TInt CFbsDevice::CreateContext(CGraphicsContext*& aGc)
	{
	return CreateContext((CFbsBitGc*&)aGc);// relies on CFbsDevice deriving _only_ from CBitmapDevice
	} 

/** Creates a client-side FBSERV font from those available in the device's typeface store 
that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function is replaced by GetNearestFontToDesignHeightInTwips().

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is interpreted 
as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes.
@deprecated */
TInt CFbsDevice::GetNearestFontInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Creates a client-side FBSERV font from those available in the device's typeface store 
that most closely matches a font specification.

When the font is no longer needed, call ReleaseFont().

This function is replaced by GetNearestFontToDesignHeightInPixels().

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is interpreted 
as being in pixels.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes.
@deprecated */
TInt CFbsDevice::GetNearestFontInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	} 

/** Creates a client-side FBSERV font from those available in the device's typeface store 
that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function replaces GetNearestFontInTwips().

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is interpreted 
as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes. */
TInt CFbsDevice::GetNearestFontToDesignHeightInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips((CFont*&)aFont, aFontSpec); // relies on CFbsFont deriving _only_ from CFont
	}
	
/** Creates a client-side FBSERV font from those available in the device's typeface store 
that most closely matches a font specification.

When the font is no longer needed, call ReleaseFont().

This function replaces GetNearestFontInPixels().

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is interpreted 
as being in pixels.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes. */
TInt CFbsDevice::GetNearestFontToDesignHeightInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels((CFont*&)aFont, aFontSpec); // relies on CFbsFont deriving _only_ from CFont
	} 
	
/** Creates a client-side FBSERV font from those available in the device's typeface store 
that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

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
fit - this overrides the height specified in aFontSpec. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes. */
TInt CFbsDevice::GetNearestFontToMaxHeightInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return GetNearestFontToMaxHeightInTwips((CFont*&)aFont, aFontSpec, aMaxHeight); // relies on CFbsFont deriving _only_ from CFont
	}

/** Creates a client-side FBSERV font from those available in the device's typeface store 
that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

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
fit - this overrides the height specified in aFontSpec. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes. */
TInt CFbsDevice::GetNearestFontToMaxHeightInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return GetNearestFontToMaxHeightInPixels((CFont*&)aFont, aFontSpec, aMaxHeight); // relies on CFbsFont deriving _only_ from CFont
	} 

/** Gets a specific bitmap font, identified by its UID, from the device's typeface store.

When the font is no longer needed, call ReleaseFont().

@param aFont On return, set to point to the font.
@param aFileId The UID identifying the bitmap font.
@param aStyle Algorithmic style for the font.
@return KErrNone if successful; otherwise, another of the system-wide error 
codes. */
TInt CFbsDevice::GetFontById(CFbsFont*& aFont,TUid aFileId,const TAlgStyle& aStyle)
	{
	return GetFontById((CFont*&)aFont,aFileId,aStyle);// relies on CFbsFont deriving _only_ from CFont
	} 

/** Gets a pointer to the 2D graphics accelerator owned by the device. If one is 
available, it is used to accelerate various CFbsBitGc graphics operations.

@return Pointer to the graphics accelerator or NULL if not supported.
@see CFbsBitmapDevice::NewL() */
CGraphicsAccelerator* CFbsDevice::GraphicsAccelerator() const
	{
	return iGraphicsAccelerator;
	}


/** Gets the device's orientation. The orientation can be set using CFbsBitGc::SetOrientation().

@return The device's orientation. */
CFbsBitGc::TGraphicsOrientation CFbsDevice::Orientation() const
	{
	return iOrientation;
	}

