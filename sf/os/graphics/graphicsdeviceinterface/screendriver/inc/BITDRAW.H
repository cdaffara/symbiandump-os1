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

#ifndef __BITDRAW_H__
#define __BITDRAW_H__

#include <gdi.h>
#include <e32event_private.h>

/**
@publishedPartner
@released
*/
enum TScreenDriverPanic
	{
	EScreenDriverPanicNoDevicePresent=1,
	EScreenDriverPanicInvalidParameter,
	EScreenDriverPanicInvalidDisplayMode,
	EScreenDriverPanicInvalidWindowHandle,
	EScreenDriverPanicOutOfBounds,
	EScreenDriverPanicZeroLength,
	EScreenDriverPanicNullPointer,
	EScreenDriverPanicInvalidPointer,
	EScreenDriverPanicAlphaBlendInvariant,
	EScreenDriverPanicIvalidMethodCall,
	EScreenDriverPanicInvalidSize,
	EScreenDriverPanicInvalidHalValue,
	EScreenDriverPanicInvalidScreenNo,
	EScreenDriverPanicIncompatiblePreviousDevice		//<The previous device in SetDisplayMode was not compatible
	};

/**
@publishedPartner
@released
*/
GLREF_C void Panic(TScreenDriverPanic aPanicCode);


/**
@publishedPartner
@released
*/
class CFbsDrawDevice : public CBase
	{
public:
	/**
	Defines possible Shadowmode values
	*/
	enum TShadowMode
		{
		/** No Shadow mode is supported.
		*/
		ENoShadow	= 0x0,
		/** Shadow mode is supported. 
		*/
		EShadow		= 0x1,
		/** Faded mode is supported.
		*/
		EFade		= 0x2,
		/** Shadow and faded mode is supported. 
		*/
		EShadowFade	= 0x3
		};
	/**
	Defines possible rotation values
	*/
	enum TOrientation
		{
		/** Normal orientation is supported. 
		*/
		EOrientationNormal,
		/** A 90 degree rotation is supported. 
		*/
		EOrientationRotated90,
		/** A 180 degree rotation is supported. 
		*/
		EOrientationRotated180,
		/** A 270 degree rotation is supported. 
		*/
		EOrientationRotated270
		};
public:
   	/** 
   	DEPRECATED: Use NewScreenDeviceL(TInt aScreenNo, TDisplayMode aDispMode)
   	
	@deprecated
	*/
	IMPORT_C static CFbsDrawDevice* NewScreenDeviceL(TScreenInfoV01 aInfo,TDisplayMode aDispMode);
	
	/**
	Creates a new screen device instance, which implements CFbsDrawDevice interface.
	The method has to be implemented for each type of supported video hardware.
	@param aScreenNo    Screen number
	@param aDispMode    Requested display mode
	@return A pointer to just created screen device, which implements CFbsDrawDevice interface
	@leave 	KErrNoMemory Not enough memory
	KErrNotSupported The requested screen device type is not supported
	*/
	IMPORT_C static CFbsDrawDevice* NewScreenDeviceL(TInt aScreenNo, TDisplayMode aDispMode);
	
	/** 
	DEPRECATED: Use NewBitmapDeviceL(const TSize& aSize, TDisplayMode aDispMode, TInt aDataStride)
	
	@deprecated 
	*/
	IMPORT_C static CFbsDrawDevice* NewBitmapDeviceL(TScreenInfoV01 aInfo, TDisplayMode aDispMode, TInt aDataStride);
	
	/**	
	Creates a new bitmap device instance, which implements CFbsDrawDevice interface.
	
	@param  aSize       Bitmap device size
	@param  aDispMode   Bitmap display mode
	@param  aDataStride Bitmap data stride 
	@return A pointer to just created bitmap device, which implements CFbsDrawDevice interface
	@leave	KErrNoMemory Not enough memory
	KErrArgument Invalid aSize value
	*/
	IMPORT_C static CFbsDrawDevice* NewBitmapDeviceL(const TSize& aSize, TDisplayMode aDispMode, TInt aDataStride);
	
	/**	
	Depending on current graphics hardware the method will return one of 
	two possible 16M video modes: 
	EColor16M or EColor16MU. If the hardware doesn't support EColor16M 
	or EColor16MU mode, the return value will be ENone.
	
	@return	EColor16M, EColor16MU or ENone.	
	ENone - it means that current hardware doesn't support 16M color mode.
	*/
	IMPORT_C static TDisplayMode DisplayMode16M();

	/**
	Return the display mode of the device.
	@return Display mode
	*/
	virtual TDisplayMode DisplayMode() const = 0;

	/**
	Scanline width in pixels. 
	The return value can be greater or equal than iSize.iWidth, because
	the scan line memory is allocated in 32-bit words and can be rounded up, if
	the display mode allows more than 1 pixel to be stored in a single byte.
	@return The (adjusted) width in pixels.
	*/
	virtual TInt LongWidth() const = 0;
	
	/**
	Alter the colours within a defined rectangle according to the supplied mapping.
	The colour map is supplied as pairs of TRgb objects. One member of the pair defines a colour value
	to match, the other defines a colour to replace that value by.
	Note that, for comparison purposes, smaller gamuts (e.g. 64K colour) are mapped to TRgb form before
	comparison, so a colour that is not representable in the current display mode will not be matched.
	Pixels in the original that do not match an entry in the colour map are unchanged.
	@param aRect 			Area of the device/bitmap to be mapped in logical coordinates
	@param aColors 			Colour map. This should be provided as a set of TRgb pairs, one to be matched
							and the other to provide a replacement. 
	@param aNumPairs 		Number of pairs of colours in aColors.
	@param aMapForwards 	If ETrue, match the first colour of a pair and replace by the second, otherwise
	                    	match the second and replace by the first.
	@panic EScreenDriverPanicOutOfBounds 	if aRect transforms back to illegal physical coordinates.
	@panic EScreenDriverPanicNullPointer 	if aColors == NULL
	@panic EScreenDriverPanicZeroLength 	if aNumPairs == 0
	*/
	virtual void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs,TBool aMapForwards) = 0;
	
	/**
	Reads a line of aLength pixels starting at [aX, aY] (in logical coordinates). The direction of reading
	is affected by the current orientation, and the "stride" taken in sampling pixels is affected by
	any scaling currently in force. Pixels are converted (if necessary) to the form specified by
	aDispMode before being written to the buffer. The entire line to be read must be within the
	physical bounds of the bitmap or device
	@param aX			X coordinate of the pixel to start reading from (logical coordinates)
	@param aY			Y coordinate of the pixel to start reading from(logical coordinates)
	@param aLength		Number of pixels to read
	@param aBuffer		Buffer to receive the pixel data (must be large enough)
	@param aDispMode	Format to use for writing pixels into the buffer.
	@panic EScreenDriverPanicOutOfBounds 	if either end of the line is out of bounds
	@panic EScreenDriverPanicNullPointer 	if aBuffer == NULL
	@panic EScreenDriverPanicZeroLength 	if aLength == 0
	*/
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer,TDisplayMode aDispMode) const = 0;
	
	/**
	Get the colour of the pixel at the logical position [aX,aY]
	@param aX	X-coordinate of pixel to read (logical coordinates)
	@param aY	Y-coordinate of pixel to read (logical coordinates)
	@return The colour pf the pixel at [aX,aY]
	@panic EScreenDriverPanicOutOfBounds 	if [aX,aY] transforms back to illegal physical coordinates.
	*/
	virtual TRgb ReadPixel(TInt aX,TInt aY) const = 0;

	/**
	Returns a pointer to a buffer large enough to read a line of pixels of maximum length. Ownership of
	the buffer is retained by the bitmap or device object. Repeated calls to this function will return
	the same buffer.
	@return A pointer to a scan line buffer
	*/
	virtual TUint32* ScanLineBuffer() const = 0;
	
	/**
	Scanline width in bytes. The return value may be greater than the actual number of bytes needed
	to store the pixels, since scan line memory is allocated in 32-bit words and can be rounded up.
	@return The (adjusted) width in bytes.
	*/
	virtual TInt ScanLineBytes() const = 0;
	
	/**
	Return the display mode to be used when passing pixel data to the device using the scanline buffer.
	@return Display mode
	@see CFbsDrawDevice::DisplayMode
	*/
	virtual TDisplayMode ScanLineDisplayMode() const = 0;
	
	/**
	Returns the screen size in pixels, taking the orientation into account.
	Always prefer GetDrawRect() to SizeInPixels() call. 
	GetDrawRect() will take into account possible non-[0,0] top-left corner of the drawing
	rectangle if the device is scaled.
	@return TSize Screen size in pixels
	*/
	virtual TSize SizeInPixels() const = 0;

	/**
	Allows the caller to calculate the physical size of a bitmap on the current hardware
	by calculating how many twips a 1000-pixel wide bitmap would be. Only applicable to
	screen-based devices; bitmap devices return 0.
	@return Twips per 1000 pixels
	*/
	virtual TInt HorzTwipsPerThousandPixels() const = 0;
	
	/**
	Allows the caller to calculate the physical size of a bitmap on the current hardware
	by calculating how many twips a 1000-pixel high bitmap would be. Only applicable to
	screen-based devices; bitmap devices return 0.
	@return Twips per 1000 pixels
	*/
	virtual TInt VertTwipsPerThousandPixels() const = 0;
	
	/**
	Reports on which orientations are available on the device/bitmap.
	@param aOrientation	Array to receive the capability (use CFbsDrawDevice::TOrientation as an index)
	*/
	virtual void OrientationsAvailable(TBool aOrientation[4]) = 0;
	
	/**
	Set the orientation of the device
	@param aOrientation	Orientation to set
	@return ETrue if the new orientation was set successfully, EFalse otherwise
	*/
	virtual TBool SetOrientation(TOrientation aOrientation) = 0;

	/**
	Combines the current content of the device/bitmap with a supplied colour, based on a bitmask and
	a draw mode. The bitmask is supplied as an array of TUint32s, one for each line (so there should
	be aHeight entries in the array). Since one word is used per line, aLength must be <= 32. Each bit
	of the mask controls one pixel - if the bit is 1, the pixel is combined with aColor, if the bit
	is 0, the pixel is left unchanged. Note that bits in the bitmask are examined in low-high order,
	so bit0 affects the pixel at aX, bit1 affects the pixel at aX+1 and so forth.
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	@param aX			Left edge of the rectangle (logical coordinates)
	@param aY			Top edge of the rectangle (logical coordinates)
	@param aBuffer		Array of bitmasks - one per line
	@param aLength		Width of the rectangle (must be >0 and <= 32)
	@param aHeight		Height of the rectangle (== number of entries in aBuffer)
	@param aColor		Colour to combine with the existing pixel data
	@param aDrawMode	Combination function for source and destination pixels.			
	@panic EScreenDriverPanicOutOfBounds	If any part of the rectangle maps to an illegal physical coordinate
											or if aLength > 32.
	@panic EScreenDriverPanicNullPointer	If aBuffer == NULL
	@panic EScreenDriverPanicZeroLength		If aLength <= 0
	*/
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode) = 0;
	
	/**
	Combines the current content of the device/bitmap with a supplied colour, based on a bitmask and
	a draw mode. This function differs from WriteBinary, in that aLength can be greater than 32, and
	the height is implicitly 1. aBuffer must contain sufficient elements to hold aLength bits
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	@param aX			Starting X coordinate (logical coordinates)
	@param aY			Starting Y coordinate (logical coordinates)
	@param aBuffer		Array of bitmasks
	@param aLength		Number of pixels
	@param aColor		Colour to combine with existing pixels
	@param aDrawMode	Combination function for source and destination pixels.			
	@panic EScreenDriverPanicOutOfBounds	If any part of the line maps to an illegal physical coordinate
	@panic EScreenDriverPanicNullPointer	If aBuffer == NULL
	@panic EScreenDriverPanicZeroLength		If aLength <= 0
	@see WriteBinary
	*/
	virtual void WriteBinaryLine(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode) = 0;

	/**
	Similar to WriteBinaryLine, but writes a vertical, rather than a horizontal line. LIne is drawn upward
	(decreasing Y) if aUp == ETrue.
	@see WriteBinaryLine 
	*/	
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode,TBool aUp) = 0;

	/**
	Write the given colour to the location [aX,aY], combining it with the existing pixel
	using aDrawMode
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	@param aX			X coordinate (logical coordinates)
	@param aY			Y coordinate (logical coordinates)
	@param aColor		Colour to write
	@param aDrawMode	Combination function for source and destination pixels.			
	@panic EScreenDriverPanicOutOfBounds	If [aX,aY] maps to an illegal physical address
	*/
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode) = 0;

	/**
	Write the given colour to the rectangle [aX,aY] - [aX+aLength,aY+aHeight], combining it
	with the exiasting pixels using aDrawMode
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	@param aX			X coordinate (logical coordinates)
	@param aY			Y coordinate (logical coordinates)
	@param aLength		Width of the rectangle (logical coordinates)
	@param aHeight		Height of the rectangle (logical coordinates)
	@param aColor		Colour to write
	@param aDrawMode	Combination function for source and destination pixels.			
	@panic EScreenDriverPanicOutOfBounds	If any part of the rectangle maps to an illegal physical address
	*/
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode) = 0;
	
	/**
	The method performs an alpha blending of the source data - aRgbBuffer and screen pixels, using
	the data from aMaskBuffer buffer as an alpha blending factor.
	The formula used for that, is:
		(C1 * A + C2 * (255 - A)) / 255, where:
			- C1 - a pixel from aRgbBuffer;
			- C2 - a pixel from the sceen;
			- A  - a pixel from aMaskBuffer;
	The content of source and mask buffers is preserved.
	The calculated alpha blended pixel is written to the destination - the screen or a bitmap.
	If the shadowing/fading flag is set, a shadow/fade copy of the source bitmap will be used.
	@param aX 			Logical X coordinate of the start of the line.
	@param aY 			Logical Y coordinate of the line.
	@param aLength 		Source data - length in pixels.
	@param aRgbBuffer 	A pointer to a line of the source bitmap data.
	                    Must be in ERgb format.
	@param aMaskBuffer 	Buffer containing the data which should be used as an 
	                   	alpha blending factor. Must be in EGray256 format.
	@param aDrawMode	Combination function for source and destination pixels.
	@panic EScreenDriverPanicOutOfBounds	If any part of the line maps to an illegal physical address
	*/
	virtual void WriteRgbAlphaLine(TInt aX,TInt aY,TInt aLength,TUint8* aRgbBuffer,TUint8* aMaskBuffer, CGraphicsContext::TDrawMode aDrawMode) = 0;

	/**
	Combine the data in aBuffer with existing pixels along the line [aX,aY]-[aX+aLength,aY],
	using aDrawMode as the combining function.
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	@param aX 			Logical X coordinate of the start of the line.
	@param aY 			Logical Y coordinate of the line.
	@param aLength 		Source data - length in pixels.
	@param aBuffer		Source data. Must be in the format returned by ScanLineDisplayMode().
	@param aDrawMode	Combination function for source and destination pixels.			
	@panic EScreenDriverPanicOutOfBounds	If any part of the line maps to an illegal physical address
	*/
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer,CGraphicsContext::TDrawMode aDrawMode) = 0;
public:

	/**
	Do any necessary initialisation on screen devices (default applies to bitmaps)
	@return Success/failure code
	*/
	virtual TInt InitScreen() {return(KErrNone);}
	
	/** Sets or unsets auto-update for the screen.

	@param aValue ETrue, if the screen is set to auto-update; EFalse, otherwise. 
	*/
	virtual void SetAutoUpdate(TBool aValue) {(void) aValue;}	// To avoid compiler warning for unused parameter
	
	/**
	Set the internal data buffer to point to the supplied buffer. No checks are made that the buffer
	is appropriate for the purpose. Ownership is not taken.
	@param aBits	A buffer of sufficient size.
	*/
	virtual void SetBits(TAny*) {}
	
	/**
	Set up a custom palette for those bit formats that support palettised colour
	@param aPalette		Palette to use
	@return Success/failure code
	*/
	virtual TInt SetCustomPalette(const CPalette*) { return KErrNotSupported; }

	/**
	Retrieve a custom palette, if the bit format supports it.
	@param aPalette	Pointer to a CPalette. The palette will be created by this function.
	@return Success/failure code
	*/
	virtual TInt GetCustomPalette(CPalette*&) { return KErrNotSupported; }

	/**
	Copies all settings (scaling, orientation, etc.) from the supplied device
	and reinitialised the current device.
	@param aDrawDevice	Device to get settings from
	*/
	virtual void SetDisplayMode(CFbsDrawDevice*) {}

	/**
	Set the origin point of the dither matrix (if appropriate)
	@param aPoint	Dither origin (logical coordiantes)
	*/
	virtual void SetDitherOrigin(const TPoint&) {}

	/**
	Sets the user display mode - used for certain colour mapping functions
	*/
	virtual void SetUserDisplayMode(TDisplayMode) {}
	
	/**
	Sets the current shadowing mode
	@see TShadowMode
	*/
	virtual void SetShadowMode(TShadowMode) {}

	/**
	Set the current fading parameters
	@param aBlackMap	Black point
	@param aWhiteMap	White point
	*/
	virtual void SetFadingParameters(TUint8 /*aBlackMap*/,TUint8 /*aWhiteMap*/) {}

	/**
	Apply shadow processing to all the pixels in the supplied rectangle
	@param aRect	Area to apply shadow processing to (logical coordinates)
	@panic EScreenDriverPanicOutOfBounds	If any part of the rectangle maps to an illegal physical address
	*/
	virtual void ShadowArea(const TRect&) {} // Obeys shadow mode

	/**
	Applies shadow processing to the supplied buffer
	@param aLength		Length of buffer in pixels
	@param aBuffer		Buffer to process in the format returned by ScanLineDisplayMode()
	@panic EScreenDriverPanicZeroLength If aLength <= 0
	@panic EScreenDriverPanicNullPointer If aBuffer == NULL
	*/
	virtual void ShadowBuffer(TInt,TUint32*) {} // Obeys shadow mode

	/**
	Update the screen with the update region.
	*/
	virtual void Update() {}

	/**
	Update the screen with the union of the update and specified regions.
	@param aRegion		Region to update (logical coordinates)
	*/
	virtual void Update(const TRegion&) {}

	/**
	Update the update region.
	@param aRegion		Region to update (logical coordinates)
	*/
	virtual void UpdateRegion(const TRect&) {}
public:

	/**
	Blend aColor with the pixels along the line [aX,aY]-[aX+aLength,aY], using aMaskBuffer
	as alpha data
	If the shadowing/fading flag is set, a shadow/fade copy of the source bitmap will be used.
	@param aX			Logical X coordinate of the start of the line
	@param aY			Logical Y coordinate of the line.
	@param aLength		Length of line to modify
	@param aColor		Colour to blend with existing pixels
	@param aMaskBuffer	Buffer containing the data which should be used as an 
	                   	alpha blending factor.
	@panic EScreenDriverPanicOutOfBounds	If any part of the line maps to an illegal physical address
	*/
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer) = 0;

	/**
	The method performs an alpha blending of the source data - aRgbBuffer1 and aBuffer2, using
	the data from aMaskBuffer buffer as an alpha blending factor.
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	The formula used for that, is:
		(C1 * A + C2 * (255 - A)) / 255, where:
			- C1 - a pixel from aRgbBuffer1;
			- C2 - a pixel from aBuffer2;
			- A  - a pixel from aMaskBuffer;
	The content of source and mask buffers is preserved.
	The calculated alpha blended pixel is written to the destination - the screen or a bitmap.
	If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
	@param aX Logical X coordinate of the position in the target the result should be drawn to.
	@param aY Logical Y coordinate of the position in the target the result should be drawn to.
	@param aLength Source data - length in pixels.
	@param aRgbBuffer1 A pointer to a line of the source bitmap data 1.
	                   Must be in ERgb format.
	@param aBuffer2 A pointer to a line of the source bitmap data 2.
	                Must be in the format returned by ScanLineDisplayMode().
	@param aMaskBuffer Buffer containing the data which should be used as an 
	                   alpha blending factor. Must be in EGray256 format.
	@param aDrawMode Drawing mode
	*/
	virtual void WriteRgbAlphaLine(TInt aX,TInt aY,TInt aLength,
								   const TUint8* aRgbBuffer1,
								   const TUint8* aBuffer2,
								   const TUint8* aMaskBuffer,
								   CGraphicsContext::TDrawMode aDrawMode) = 0;
	/**
	Retrieves a pointer to the specified CFbsDrawDevice interface extension.
	@param aInterfaceId Interface identifier of the interface to be retrieved.
	@param aInterface Address of pointer variable that retrieves the specified interface.
	@return KErrNone If the interface is supported, KErrNotSupported otherwise.
	@see BitDrawInterfaceId.h file for the IDs of supported interfaces
	*/
	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface) = 0;
	
	/**
	Gets logical coordinates of the drawing rectangle.
	If the device is not scaled and with zero origin, logocal coordinates of 
	the drawing rectangle are the same as its physical coordinates.
	If the device is rotated, drawing rectangle width and height are swapped.
	Always prefer GetDrawRect() to SizeInPixels() call. SizeInPixels() will return
	drawing rectangle width and height. But if the device is scaled or with nonzero origin,
	GetDrawRect() will take into account and the top-left corner of the drawing rectangle too,
	which may not be [0, 0].
	@param aDrawRect Upon return aRect contains drawing rectangle logical coordinates.
	*/
	virtual void GetDrawRect(TRect& aDrawRect) const = 0;
	
	/**
	The method swaps bitmap device's width and height.
	For example: if the size is (40, 20), the swapped size will be (20, 40).
	The device's content is not preserved.
	The method leaves CDrawBitmap object in a consistent state - 
	scaling settings will be set with their default values (the scaling is switched off),
	iDitherOrigin will be set to (0,0), iOrigin to (0,0).
	Note: This method is used internally by BITGDI component. Do not call it!
	*/
	virtual void SwapWidthAndHeight() = 0;
	};

#endif

