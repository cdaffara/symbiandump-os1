// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BMALPHABLEND_H__
#define __BMALPHABLEND_H__

/**
MAlphaBlend interface provides only one method, which does an alpha blending using the 
supplied as arguments source and mask bitmap scanline data and writes the result to 
the screen. The content of the source and mask bitmap scanlines is preserved.
@internalComponent
*/
class MAlphaBlend
    {
public:
    /**
    TShadowing enum values are used in alpha blending implementations to specify when
    the shadowing/fading has to be done: before or after tha alpha blending.
    @internalComponent
    */
    enum TShadowing
        {
        EShdwBefore,
        EShdwAfter
        };

    /**
    The method performs an alpha blending of the source data - aRgbBuffer and the screen 
    pixels, using the data from aMaskBuffer buffer as an alpha blending factor.
    The formula used for that, is:
    (C1 * A + C2 * (255 - A)) / 255, where:
    - C1 - a pixel from aRgbBuffer1;
    - C2 - a pixel from screen;
    - A  - a pixel from aMaskBuffer;
    The content of source and mask buffers is preserved.
    The calculated alpha blended pixel is written to the destination - the screen or a bitmap.
    @param aX Logical X coordinate of the position in the target the result should be drawn to.
    @param aY Logical Y coordinate of the position in the target the result should be drawn to.
    @param aLength Source data - length in pixels.
    @param aRgbBuffer A pointer to a line of the source bitmap data.
    @param aMaskBuffer Buffer containing the data which should be used as an 
                       alpha blending factor.
    @param aShadowing It says when the shadowing/fading has to be done - before or after the
                      alpha blending transformation.
                      Before: A shadow/fade copy of the source bitmap will be used.
                      After: The result pixels will be shadowed/faded.
    @param aDrawMode	The mode for rendering the source image to the destination.
    */
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode) = 0;
    };


/**
MFastBlit provides optimised blitting for a number of special cases.  It is similar to
MAlphaBlend except that instead of taking generic buffers as parameters, it takes pointers
to scanlines in their native format.
@internalTechnology
*/
class MFastBlit
	{
public:
	/**
	Performs Alpha blending.Acceptable source formats are EColor64K and EColor16MU.
	Mask format must be EGray256.
	@param aX Logical X coordinate of the position in the target the result should be drawn to.
	@param aY Logical Y coordinate of the position in the target the result should be drawn to.
	@param aLength Source data - length in pixels.
	@param aSrcX X coordinate of the position of the first pixel in the source bitmap to use.
	@param aSrcPtr Pointer to the start of the current scanline of the source bitmap.
	@param aSrcFormat Pixel format of the source bitmap.
	@param aMaskX X coordinate of the position of the first pixel in the mask to use.
	@param aMaskPtr Pointer to the start of the current scanline of the mask bitmap.
	@param aShadowing It says when the shadowing/fading has to be done - before or after the
                      alpha blending transformation.
                      Before: A shadow/fade copy of the source bitmap will be used.
                      After: The result pixels will be shadowed/faded.
	*/
	virtual void WriteAlphaLineEx(	TInt aX,
									TInt aY,
									TInt aLength, 
									TInt aSrcX,
									const TUint32* aSrcPtr,
									TDisplayMode aSrcFormat,
									TInt aMaskX,
									const TUint32* aMaskPtr,
									MAlphaBlend::TShadowing aShadowing) = 0;

	/**
	Performs masked blitting.  Acceptable source formats are EColor64K and EColor16MU.
	Mask format must be EGray2.
	@param aX Logical X coordinate of the position in the target the result should be drawn to.
	@param aY Logical Y coordinate of the position in the target the result should be drawn to.
	@param aLength Source data - length in pixels.
	@param aSrcX X coordinate of the position of the first pixel in the source bitmap to use.
	@param aSrcPtr Pointer to the start of the current scanline of the source bitmap.
	@param aSrcFormat Pixel format of the source bitmap.
	@param aMaskX X coordinate of the position of the first pixel in the mask to use.
	@param aMaskPtr Pointer to the start of the current scanline of the mask bitmap.
	@param aInvertMask Specifies if the mask shuld be inverted.
	*/				
	virtual void WriteMaskLineEx(	TInt aX,
									TInt aY,
									TInt aLength, 
									TInt aSrcX,
									const TUint32* aSrcPtr,
									TDisplayMode aSrcFormat,
									TInt aMaskX,
									const TUint32* aMaskPtr,
									TBool aInvertMask) = 0;

	};



/**
MFastBlit2 provides optimised blitting for a number of special cases.
It is used for basic blitting where source and destination pixel formats match.
@internalTechnology
*/
class MFastBlit2
	{
public:
	/**
	Performs basic blitting.
	Source and destination pixel formats must match.
	Assumes that aSrcRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aDest           Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcDrawDevice  Draw device that will act as the source of the blit.
	@param aSrcRect        The rectangular region of the source to be blitted.
	@return                KErrNone unless an error occurs in which case no blitting occurs and a standard error code is returned.
	*/
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
	                              CFbsDrawDevice* aSrcDrawDevice,
	                              const TRect& aSrcRect) = 0;
	                              
	/**
	Performs basic blitting.
	Source and destination pixel formats must match.
	Assumes that aSrcRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aDest       Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcBase    Base address of the source bitmap.
	@param aSrcStride  Length in bytes between scanlines of the source bitmap.
	@param aSrcSize    Size of the source bitmap in pixels.
	@param aSrcRect    The rectangular region of the source to be blitted.
	@return            KErrNone unless an error occurs in which case no blitting occurs and a standard error code is returned.
	*/				
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
	                              const TUint32* aSrcBase,
	                              TInt aSrcStride,
	                              const TSize& aSrcSize,
	                              const TRect& aSrcRect) = 0;
	/**
	Returns a pointer to the first pixel.
	@return a pointer to the first pixel.
	*/				
	virtual const TUint32* Bits() const = 0;
	};

/**
MOutlineAndShadowBlend provides blending of outline pen, shadow, fill and the background colour.
It is used to draw the fonts with outline and shadow effects.
@internalTechnology
*/
class MOutlineAndShadowBlend
	{
	public:
		/**
		Performs blending of outline, shadow, fill and background colours and draws to the 
		screen. It uses pen colour as outline colour, brush colour as fill colour and pixel
		colour as background colour. Transparency is supported for modes higher than EColor256,
		alpha value of pen colour is used for same in these modes and other modes ignore this 
		value.
		@param aX Logical X coordinate of the start of the line.
		@param aY Logical Y coordinate of the line.
		@param aLength Length in pixels to modify.
		@param aOutlinePenColor	Outline pen colour of the font.
		@param aShadowColor Shadow colour of the font.
		@param aFillColor Fill colour of the font.
		@param aDataBuffer Buffer containing the data.
		@return	KErrNone if it is successful, otherwise a standard error code is returned.
		*/
		virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
		                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
		                                   TUint32 aFillColor, const TUint8* aDataBuffer) = 0;
	};

class MFastBlend
	{
public:
	/**
	Performs blended blitting.
	The interface can selectively supports various combinations of source and destination.
	Assumes that aSrcRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aDest           Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcDrawDevice  Draw device that will act as the source of the blit.
	@param aSrcRect        The rectangular region of the source to be blitted.
	@param aDrawMode	Current draw mode
	@param aShadowMode	Current shadow mode
	@return                if display modes supported KErrNone, else KErrNotSupported.
	*/
	virtual TInt FastBlendBitmap(const TPoint& aDest,
	                              CFbsDrawDevice* aSrcDrawDevice,
	                              const TRect& aSrcRect,
	                              CGraphicsContext::TDrawMode aDrawMode,
	                              TInt aShadowMode) = 0;
	/**
	Performs blended blitting.
	The interface can selectively supports various combinations of source and destination.
	Assumes that aSrcRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aDest		Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcBase		Base address of the source bitmap.
	@param aSrcStride	Length in bytes between scanlines of the source bitmap.
	@param aSrcSize		Size of the source bitmap in pixels.
	@param aSrcRect		The rectangular region of the source to be blitted.
	@param aSrcDisplayMode Display mode of the source bitmap
	@param aDrawMode	Current draw mode
	@param aShadowMode	Current shadow mode
	@return				if display modes supported KErrNone, else KErrNotSupported.
	*/
	virtual TInt FastBlendBitmap(const TPoint& aDest,const TUint32* aSrcBase,TInt aSrcStride,
	                              const TSize& aSrcSize,const TRect& aSrcRect,TDisplayMode aSrcDisplayMode,
	                              CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode) = 0;
	/**
	Performs blended blitting.
	The interface can selectively supports various combinations of source and destination.
	Assumes that aSrcRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aDest		Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcBase		Base address of the source bitmap.
	@param aSrcStride	Length in bytes between scanlines of the source bitmap.
	@param aSrcSize		Size of the source bitmap in pixels.
	@param aSrcRect		The rectangular region of the source to be blitted.
	@param aSrcDisplayMode Display mode of the source bitmap
	@param aMaskBase	Base address of the source bitmap.
	@param aMaskStride	Length in bytes between scanlines of the source bitmap.
	@param aMaskDisplayMode Display mode of the source bitmap
	@param aMaskSize	Size of the source bitmap in pixels.
	@param aInvertMask	If true invert the logic of an EGray2 mask
	@param aDrawMode	Current draw mode
	@param aShadowMode	Current shadow mode
	@return				if display modes supported KErrNone, else KErrNotSupported.
	*/
	virtual TInt FastBlendBitmapMasked(const TPoint& aDest, const TUint32* aSrcBase, TInt aSrcStride,
							const TSize& aSrcSize, const TRect& aSrcRect, TDisplayMode aSrcDisplayMode,
							const TUint32* aMaskBase, TInt aMaskStride, TDisplayMode aMaskDisplayMode, const TSize &aMaskSize,const TPoint &aMaskSrcPos, TBool aInvertMask,
							CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode)=0;
	/**
	Performs scaled blended blitting.
	The interface can selectively supports various combinations of source and destination.
	Assumes that aClipRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aClipRect	The target rectangle to clip drawing to
	@param aDest		Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcRect		Source rectangle
	@param aSrcBase		Base address of the source bitmap.
	@param aSrcStride	Length in bytes between scanlines of the source bitmap.
	@param aSrcDisplayMode Display mode of the source bitmap
	@param aSrcSize		Size of the source bitmap in pixels.
	@param aDrawMode	Current draw mode
	@param aShadowMode	Current shadow mode
	@return				if display modes supported KErrNone, else KErrNotSupported.
	*/
	virtual TInt FastBlendBitmapScaled(const TRect &aClipRect, const TRect& aDest,
							const TRect& aSrcRect, const TUint32 *aSrcBase, TInt aSrcStride,
							TDisplayMode aSrcDisplayMode, const TSize &aSrcSize,
							CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode) = 0;
	/**
	Performs scaled blended blitting.
	The interface can selectively supports various combinations of source and destination.
	Assumes that aClipRect is contained by the source device.
	Assumes that the resulting destination rectangle is contained by the destination device.
	@param aClipRect	The target rectangle to clip drawing to
	@param aDest		Logical coordinates of the position in the target that the result should be drawn to.
	@param aSrcRect		Source rectangle
	@param aSrcBase		Base address of the source bitmap.
	@param aSrcStride	Length in bytes between scanlines of the source bitmap.
	@param aSrcDisplayMode Display mode of the source bitmap
	@param aSrcSize		Size of the source bitmap in pixels.
	@param aMaskBase	Base address of the source bitmap.
	@param aMaskStride	Length in bytes between scanlines of the source bitmap.
	@param aMaskDisplayMode Display mode of the source bitmap
	@param aMaskSize	Size of the source bitmap in pixels.
	@param aInvertMask	If true invert the logic of an EGray2 mask
	@param aDrawMode	Current draw mode
	@param aShadowMode	Current shadow mode
	@return				if display modes supported KErrNone, else KErrNotSupported.
	*/
	virtual TInt FastBlendBitmapMaskedScaled(const TRect &aClipRect, const TRect& aDest,
							const TRect& aSrcRect, const TUint32 *aSrcBase, TInt aSrcStride,
							TDisplayMode aSrcDisplayMode, const TSize &aSrcSize,
							const TUint32* aMaskBase, TInt aMaskStride, TDisplayMode aMaskDisplayMode, const TSize &aMaskSize,TBool aInvertMask,
							CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode) = 0;
	};

#endif//__BMALPHABLEND_H__
