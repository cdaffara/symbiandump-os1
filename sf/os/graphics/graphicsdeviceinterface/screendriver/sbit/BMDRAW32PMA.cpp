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
//

#include "BMDRAW.H"
#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>

/*
Does the same conversion as NonPMA2PMAPixel, but leaves the values in the RB and G uint32 values
ready for use in the blending algorithm. Also assumes alphas of 0 or 0xFF have already been
stripped out as special cases.
*/
FORCEINLINE void NonPMA2PMAPixelRBG(TUint32 aPixel, TUint32 &aSrcRB, TUint32 &aSrcG, TUint32 &aMask)
	{
	TUint32 alpha=(aPixel >> 24);
	aMask = 0x0100 - alpha;	// for use in PMABlend_fromRBandG
	TUint32 tap1=alpha+1;
	aSrcRB = (aPixel & KRBMask) * tap1;
	aSrcRB >>= 8;
	aSrcRB &= KRBMask;
	aSrcG = (aPixel&0xFF000000) | (((aPixel & KGMask)>>8) * tap1) & KAGMask;
	}

/*
Uses same algorithm as PMABlend_noChecksInplace, but optimised for use with NonPMA2PMAPixelRBG where
source values are pre-split into RB and G components ready for use in blend
*/
FORCEINLINE void PMABlend_fromRBandG(TUint32& aDest_io, const TUint32 aSrcRB, const TUint32 aSrcG, const TUint8 aMask)
	{
	TUint32 dst_ag = (aDest_io & KAGMask) >> 8;
	aDest_io = aDest_io  & KRBMask;
	aDest_io = (aSrcRB +  ((aMask * aDest_io) >> 8)) & KRBMask;
	aDest_io |= (aSrcG +  (aMask * dst_ag)) & KAGMask;
	}

/*
Takes a non-PM source colour and an alpha value
First pre-multiplies aSrcPixel with aAlpha
Then blends it into aDestIo using the same alpha value.
*/
FORCEINLINE void PMABlend_FromNonPmAndAlpha(TUint32& aDest_io, const TUint32& aSrcPixel, TUint32 aAlpha)
	{
	TUint32 tap1=aAlpha+1;
	TUint32 srcRB = (aSrcPixel & KRBMask) * tap1;
	srcRB=(srcRB>>8) & KRBMask;
	TUint32 srcAG = (aSrcPixel & KGMask ) * tap1;
	srcAG=(srcAG>>8) & KGMask;
	srcAG|=aAlpha<<24;

	TUint32 dst_ag = (aDest_io & KAGMask) >> 8;
	TUint32 dst_rb = aDest_io & KRBMask;
	const TUint32 mask = 0x0100 - aAlpha;
	aDest_io = (srcRB +  ((mask*dst_rb) >> 8)) & KRBMask;
	aDest_io |= (srcAG + mask*dst_ag) & KAGMask;
	}

// Same as NonPMA2PMAPixel, but for use where checks for alpha of 0 or 0xFF have already been done
FORCEINLINE TUint32 NonPMA2PMAPixelNoCheck(TUint32 aPixel)
	{
	TUint32 tap1=(aPixel>>24)+1;
	TUint32 scaledRB = (aPixel & KRBMask) * tap1;
	TUint32 scaledG = (aPixel & KGMask ) * tap1;
	return (aPixel & 0xff000000) | ((scaledRB>>8) & KRBMask) | ((scaledG>>8)& KGMask);
	}

/**
Composite mask application
where AlphaComposite = AlphaColor * AlphaMask / 255.
@param aColor - non-premltiplied color with alpha 
@param aMask  - the mask value. (0 to 255).
@return the color with alpha combined from color and mask (combination
operator used is scaled multiplication).
*/
FORCEINLINE TUint32 CompMask(TUint32 aColor, TUint8 aMask)
	{
	// if the mask is FF the colour will be fully opaque and can be immediatly returned
	if (aMask == 0xff)
		return aColor;
	
	TUint32 cAlpha = (aMask+1) * (aColor >> 24);
	return ((aColor & 0x00ffffff) | ((cAlpha <<16) & 0xff000000));
	}

TInt CDrawThirtyTwoBppBitmapAlphaPM::Construct(TSize aSize)
	{
	return Construct(aSize, aSize.iWidth << 2);
	}

TInt CDrawThirtyTwoBppBitmapAlphaPM::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EColor16MAP;
	return CDrawThirtyTwoBppBitmapCommon::Construct(aSize, aStride);
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
Blends the supplied buffer of TRgb format data (ARGB), and the mask, to the destination
which is specified by the pixel positions aX, aY and length.
@param aX	The starting position on the x-axis for the destination pixel.
@param aY	The starting position on the y-axis for the destination pixel.
@param aRgbBuffer	The RGB source data buffer. This is assumed to be bit aligned 32 bit non-premultiplied data
@param aMaskBuffer	The masking data. The mask buffer and the source alpha values are multiplied together to
					generate the alpha used for blending to the destination.
@param aShadowing	The shadowing flag. Indicates when and how to apply shadowing.
@param aDrawMode	The mode for rendering the source image to the destination. 
					Currently supports EWriteAlpha and EPenmode.

@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawThirtyTwoBppBitmapAlphaPM::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                  const TUint8* aRgbBuffer,
                                                  const TUint8* aMaskBuffer,
                                                  MAlphaBlend::TShadowing aShadowing,
                                                  CGraphicsContext::TDrawMode aDrawMode)
    {
    // precondition for this function is that the aRgbBuffer lies on a word boundary
    // Assert checks that the pointer is at a word boundary
    __ASSERT_DEBUG(!(((TUint)aRgbBuffer) & 0x3), Panic(EScreenDriverPanicInvalidPointer));

	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint16* normTable = PtrTo16BitNormalisationTable();
	TRgb tmp;
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	// The purpose of this conditional is to remove if statements from within the loop
	// if shadow mode is not enabled and the UserDispMode is none or EColor16MAP
	if(!(iShadowMode & (EFade | EShadow)) && (iUserDispMode ==EColor16MAP || iUserDispMode == ENone))
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			if (*aMaskBuffer)
				{
				TUint32 srcData = *((TUint32*)aRgbBuffer);
				if ((aDrawMode == CGraphicsContext::EDrawModeWriteAlpha) && (*aMaskBuffer == 0xFF))
					{
					// Write the source straight through to the target, otherwise
					// blend. This relies on the fact that EDrawModeWriteAlpha can only
					// be used with masks that contain only black or white.
					Convert2PMA(srcData);
					*pixelPtr = srcData;
					}
				else
					{
					//blend alpha value in source with the mask value.
					srcData = CompMask(srcData, *aMaskBuffer);
					//pre-multiply, inplace.
					Convert2PMA(srcData);
					PMAInplaceBlend(*pixelPtr, srcData);
					}
				}
			pixelPtr += pixelPtrInc;
			aRgbBuffer += 4;
			aMaskBuffer++;
			}
		}
	else
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			TUint32 srcColor = *((TUint32*)(aRgbBuffer));
			if(*aMaskBuffer)
				{
				if(aShadowing == MAlphaBlend::EShdwBefore)
					{
					// src color in non-premultiplied.
					Shadow(*(TRgb*)&srcColor);
					}
				TUint32 srcData;
				if ((aDrawMode == CGraphicsContext::EDrawModeWriteAlpha) && (*aMaskBuffer == 0xFF))
					{
					Convert2PMA(srcColor);
					srcData = srcColor;
					}
				else
					{
					//blend alpha value in source with the mask value.
					srcData = CompMask(srcColor, *aMaskBuffer);
					//pre-multiply, inplace.
					Convert2PMA(srcData);
					srcData = PMAPixelBlend(*pixelPtr, srcData);
					}

				if(aShadowing == MAlphaBlend::EShdwAfter)
					{
					// shadow is done in PMA mode.
					Shadow(srcData);
					}
				if(iUserDispMode !=EColor16MAP && iUserDispMode != ENone)
					{
					tmp.SetInternal(PMA2NonPMAPixel(srcData, normTable));
					CDrawThirtyTwoBppBitmapCommon::MapColorToUserDisplayMode(tmp);
					srcData = tmp.Internal();
					Convert2PMA(srcData);
					}
				*pixelPtr = srcData;
				}
			pixelPtr += pixelPtrInc;
			aRgbBuffer += 4;
			aMaskBuffer++;
			}
		}
	}

/**
This assumes that the pen color (aColor) alpha information is to be blended with
the alpha provided by the mask buffer.
If pen is opaque, the mask is used as transparency information to be used.
If pen is semi-transparent, the mask is contructed by calculating the resulting alpha
by multiplying the mask with the alpha information of the pen color.
@see CFbsDrawDevice::WriteRgbAlphaMulti
*/
void CDrawThirtyTwoBppBitmapAlphaPM::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	TUint32 srcAlpha = aColor.Alpha();
	if (srcAlpha==0 || aLength<=0)
		return;
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	if (iShadowMode)
		CDrawThirtyTwoBppBitmapCommon::Shadow(aColor);
	// pre-multiply the color.
	const TUint32 srcColor = aColor.Internal();
	if (srcAlpha == 255)
		{ // source (pen) is opaque, so we simply blend it using the mask
		// little trick to make main loop more optimised, having 'pixelPtr += pixelPtrInc' at the top 
		// of the loop makes the compiler generate better code without extra jumps to reach the
		// 'pixelPtr += pixelPtrInc' line.
		// We also need to do the FOREVER loop with a break when aMaskBuffer==maskBufferPtrLimit or
		// else the compiler puts the test at the end and extra jumps are required to reach it.
		pixelPtr -= pixelPtrInc;
		FOREVER
			{
			pixelPtr += pixelPtrInc;
			if (aMaskBuffer==maskBufferPtrLimit)
				break;
			const TUint maskAlpha=*aMaskBuffer++;
			if (maskAlpha==0)
				{	// Most pixels in text are blank, so this is the most important route to optimise
				continue;
				}
			else if (maskAlpha==0xFF)
				{	// Second most important are fully opaque pixels
				*pixelPtr=srcColor;
				continue;
				}
			TUint32 maskColor = NonPMA2PMAPixelNoCheck((srcColor&~0xFF000000) | ((TUint32)maskAlpha << 24));
			PMAInplaceBlend(*pixelPtr, maskColor);
			}
		}
	else if(srcAlpha > 0)
		{
		// pen is semi-transparent, so we must blend using both the mask and pen alpha
		// pre calculate ffMaskColor optimised for common case where aMaskBuffer contains 0xFF
		const TUint32 ffMaskColor=NonPMA2PMAPixelNoCheck(srcColor);
		const TUint32 noAlphaSrcColor=srcColor&0xFFFFFF;
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			const TUint maskAlpha=*aMaskBuffer;
			if (maskAlpha)
				{
				if (maskAlpha==0xFF)
					PMABlend_noChecksInplace(*pixelPtr, ffMaskColor, srcAlpha);
				else
					{
					TUint32 mixedAlpha = ((maskAlpha+1) * srcAlpha)>>8;
					PMABlend_FromNonPmAndAlpha(*pixelPtr, noAlphaSrcColor, mixedAlpha);
					}
				}
			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			}
		}
	}

/**
@see CFbsDrawDevice::WriteRgb
*/
void CDrawThirtyTwoBppBitmapAlphaPM::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TUint32 srcColor = NonPMA2PMAPixel(aColor.Internal());
	TUint32* pixelPtr = PixelAddress(aX,aY);
	PMAInplaceBlend(*pixelPtr, srcColor);
	}

/**
@see CFbsDrawDevice::WriteBinary
*/
void CDrawThirtyTwoBppBitmapAlphaPM::WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TUint32 colorInternal=aColor.Internal();
	const TUint8  srcAlpha = colorInternal >> 24;
	if (srcAlpha==0)
		return;
	DeOrientate(aX,aY);

	TInt pixelInc;
	TInt rowInc;

	switch(iOrientation)
		{
		case EOrientationNormal:
			{
			pixelInc = 1;
			rowInc = iScanLineWords;
			break;
			}
		case EOrientationRotated90:
			{
			pixelInc = iScanLineWords;
			rowInc = -1;
			break;
			}
		case EOrientationRotated180:
			{
			pixelInc = -1;
			rowInc = -iScanLineWords;
			break;
			}
		default: // EOrientationRotated270
			{
			pixelInc = -iScanLineWords;
			rowInc = 1;
			}
		}

	const TUint32* dataLimit = aBuffer + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;

	TUint32* pixelPtr = PixelAddress(aX,aY);
	if (srcAlpha==0xFF)
		{
		while (aBuffer < dataLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint32* tempPixelPtr = pixelPtr;
			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					*tempPixelPtr=colorInternal;
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	else
		{
		TUint32 src_rb;
		TUint32 src_g;
		TUint32 mask;
	//pre-multiply the src color, before blending.
		NonPMA2PMAPixelRBG(colorInternal,src_rb,src_g,mask);
		while (aBuffer < dataLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint32* tempPixelPtr = pixelPtr;
			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					PMABlend_fromRBandG(*tempPixelPtr, src_rb, src_g, mask);
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	}

/**
@see CFbsDrawDevice::WriteBinaryLineVertical
*/
void CDrawThirtyTwoBppBitmapAlphaPM::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aHeight,TRgb aColor,TBool aUp)
	{
	const TUint32 colorInternal=aColor.Internal();
	const TInt sourceAlpha = colorInternal>>24;
	if (sourceAlpha==0)
		return;

	DeOrientate(aX,aY);

	TInt scanlineWordLength;

	switch(iOrientation)
		{
		case EOrientationNormal:
			scanlineWordLength = iScanLineWords;
			break;
		case EOrientationRotated90:
			scanlineWordLength = -1;
			break;
		case EOrientationRotated180:
			scanlineWordLength = -iScanLineWords;
			break;
		default: // EOrientationRotated270
			scanlineWordLength = 1;
		}

	if (aUp)
		scanlineWordLength = -scanlineWordLength;

	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TUint32* pixelPtrLimit = pixelPtr + (aHeight * scanlineWordLength);
	TUint32 dataWord = *aBuffer;
	TUint32 dataMask = 1;

	if (sourceAlpha==0xFF)
		{
		while(pixelPtr != pixelPtrLimit)
			{
			if(!dataMask)
				{
				dataMask = 1;
				aBuffer++;
				dataWord = *aBuffer;
				}

			if(dataWord & dataMask)
				{
				*pixelPtr = colorInternal;
				}
			dataMask <<= 1;
			pixelPtr += scanlineWordLength;
			}
		}
	else
		{
		TUint32 src_rb;
		TUint32 src_g;
		TUint32 mask;
	//pre-multiply the src color, before blending.
		NonPMA2PMAPixelRBG(colorInternal,src_rb,src_g,mask);
		while(pixelPtr != pixelPtrLimit)
			{
			if(!dataMask)
				{
				dataMask = 1;
				aBuffer++;
				dataWord = *aBuffer;
				}

			if(dataWord & dataMask)
				{
				PMABlend_fromRBandG(*pixelPtr, src_rb, src_g, mask);
				}
			dataMask <<= 1;
			pixelPtr += scanlineWordLength;
			}
		}
	}

/**
@see CFbsDrawDevice::BlendRgbMulti
*/
void CDrawThirtyTwoBppBitmapAlphaPM::BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)// opaque
		{
		WriteRgbMulti(aX,aY,aLength,aHeight,aColor);
		return;
		}
	if (sourceAlpha==0)// transparent
		return;
	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineWords);
	TUint32* pixelPtrLimit = pixelPtr + aLength;

	TUint32 srcValue = NonPMA2PMAPixel(aColor.Internal());
	TUint32 src_c = srcValue & KRBMask;
	TUint32 src_g = (srcValue & KAGMask)>>8;
	const TUint32 mask = 0x0100 - sourceAlpha;
	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			const TUint32 dst = *tempPixelPtr;
			TUint32 dst_c = dst  & KRBMask;
			dst_c = (src_c +  ((mask * dst_c)>>8)) & KRBMask;
			const TUint32 dst_ag = (dst & KAGMask)>>8;
			dst_c |= ((src_g +  ((mask * dst_ag)>>8)) & KRBMask)<<8;
			*tempPixelPtr=dst_c;
			}
		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

/**
@see CFbsDrawDevice::BlendLine
*/
void CDrawThirtyTwoBppBitmapAlphaPM::BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);

	const TUint32* bufferPtrLimit = aBuffer + aLength;
	const TInt pixelPtrInc = (iOrientation == EOrientationNormal) ? 1 : PixelAddressIncrement();

	while (aBuffer < bufferPtrLimit)
		{
		PMAInplaceBlend(*pixelPtr,  *aBuffer);
		aBuffer++;
		pixelPtr += pixelPtrInc;
		}
	}
/**
@see CFbsDrawDevice::ShadowArea
*/
void CDrawThirtyTwoBppBitmapAlphaPM::ShadowArea(const TRect& aRect)
	{
	const TRect rect(DeOrientate(aRect));
	__ASSERT_DEBUG(rect.iTl.iX>=0 && rect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(rect.iTl.iY>=0 && rect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	TUint32* pixelPtr = PixelAddress(rect.iTl.iX,rect.iTl.iY);
	TUint32* pixelRowPtrLimit = pixelPtr + (rect.Height() * iScanLineWords);

	TUint32* pixelRowPtr = pixelPtr;
	TUint32* pixelPtrLimit = pixelPtr + rect.Width();

	if (iShadowMode & EFade)
		{
		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint32* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				const TUint32 color = *tempPixelPtr;
#if defined(SYMBIAN_USE_FAST_FADING)
				const TUint32 fast_fade_offset = NonPMA2PMAPixel((color & 0xff000000) | SYMBIAN_USE_FAST_FADING) & 0x00ffffff;
				*tempPixelPtr++ = (color & 0xff000000) | ((((color & 0x00ffffff) >> 1) & ~0x00808080) + fast_fade_offset);
#else
				const TInt alpha = (color >> 24) + 1;
				const TUint32 fadeMapOffset = ((alpha * iFadeMapOffset) >> 8) & 0xff;
				const TUint32 wordFadeMapOffset = ((fadeMapOffset) << 16) | (fadeMapOffset);
				const TUint32 rb = ((((color & 0x00ff00ff) * iFadeMapFactor) >> 8) + wordFadeMapOffset) & 0x00ff00ff;
		  		const TUint32 g = ((((color & 0x0000ff00) * iFadeMapFactor) >> 16) + fadeMapOffset) << 8;
		  		*tempPixelPtr++ = (color & 0xff000000) | rb | g;
#endif
				}
			pixelRowPtr += iScanLineWords;
			pixelPtrLimit += iScanLineWords;
			}
		}

	if (iShadowMode & EShadow)
		{
		pixelRowPtr = pixelPtr;
		pixelPtrLimit = pixelPtr + rect.Width();

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint32* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				const TUint32 color = *tempPixelPtr;
				const TInt alpha = (color >> 24) + 1;
				const TInt uLimit = ((0x40) * alpha) >> 8;
				TInt r = (color >> 16) & 0xff;
				r = (r > uLimit) ? (r-uLimit) : 0;
				TInt g = (color >> 8) & 0xff;
				g = (g > uLimit) ? (g - uLimit) : 0;
				TInt b = color & 0xff;
				b = (b > uLimit) ? (b - uLimit) : 0;
				*tempPixelPtr++	=  (color & 0xff000000) | (r << 16) | (g << 8) | b;
				}
			pixelRowPtr += iScanLineWords;
			pixelPtrLimit += iScanLineWords;
			}
		}
	}

/**
@see CFbsDrawDevice::WriteLine
*/
void CDrawThirtyTwoBppBitmapAlphaPM::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer,
		CGraphicsContext::TDrawMode aDrawMode)
	{
	const TPoint originalPoint(aX,aY);
	DeOrientate(aX,aY);//aX and aY - physical coordinates

	__ASSERT_DEBUG(aX >= 0,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= 0,Panic(EScreenDriverPanicOutOfBounds));
#if defined(_DEBUG)
	switch (iOrientation)
		{
	case EOrientationNormal:
		__ASSERT_DEBUG(aX + aLength <= iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
		break;
	case EOrientationRotated90:
		__ASSERT_DEBUG(aY + aLength <= iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));
		break;
	case EOrientationRotated180:
		__ASSERT_DEBUG(aX - aLength >= -1,Panic(EScreenDriverPanicOutOfBounds));
		break;
	default: // EOrientationRotated270
		__ASSERT_DEBUG(aY - aLength >= -1,Panic(EScreenDriverPanicOutOfBounds));
		break;
		}
#endif
	__ASSERT_DEBUG(aLength > 0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	MapBufferToUserDisplayMode(aLength,aBuffer);
	if(iShadowMode)
		{
		ShadowBuffer(aLength,aBuffer);
		}
	if(aDrawMode&CGraphicsContext::EInvertPen)
		{
#if defined(_DEBUG)
		// code in CFbsBitGc::DoBitBltMasked() does not require logical operator pen modes in premultiplied alpha screen driver for blitting of icons with masks
//		RDebug::Printf("Premultiplied alpha mode invert pen behaviour not defined / implemented");
#endif
		// no-operation
		}
	if(aDrawMode&CGraphicsContext::EPenmode)
		{
		BlendLine(aX,aY,aLength,aBuffer);
		return;
		}
	if(aDrawMode&CGraphicsContext::EWriteAlpha)
		{
		CDrawThirtyTwoBppBitmapCommon::WriteLine(aX,aY,aLength,aBuffer);
		return;
		}
	if(aDrawMode&CGraphicsContext::EInvertScreen)
		{
#if defined(_DEBUG)
//		RDebug::Printf("Premultiplied alpha mode invert screen behaviour not defined / implemented");
#endif
		return; //no-operation
		}
	if(aDrawMode&CGraphicsContext::EXor)
		{
#if defined(_DEBUG)
		// code in CFbsBitGc::DoBitBltMasked() does not require logical operator pen modes in premultiplied alpha screen driver for blitting of icons with masks
//		RDebug::Printf("Premultiplied alpha mode XOR operation not defined / implemented");
#endif
		return; //no-operation
		}
else if(aDrawMode&CGraphicsContext::EAnd)
		{
#if defined(_DEBUG)
		// code in CFbsBitGc::DoBitBltMasked() does not require logical operator pen modes in premultiplied alpha screen driver for blitting of icons with masks
//		RDebug::Printf("Premultiplied alpha mode AND operation not defined / implemented");
#endif
		return; //no-operation
		}
	else if(aDrawMode&CGraphicsContext::EOr)
		{
#if defined(_DEBUG)
//		RDebug::Printf("Premultiplied alpha mode OR operation not defined / implemented");
#endif
		return; //no-operation
		}
	}

TRgb CDrawThirtyTwoBppBitmapAlphaPM::RgbColor(TUint32 aColor) const
	{
	return TRgb::_Color16MAP(aColor);
	}

TUint32 CDrawThirtyTwoBppBitmapAlphaPM::Color(const TRgb& aColor)
	{
	return aColor._Color16MAP();
	}
	
TInt CDrawThirtyTwoBppBitmapAlphaPM::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
															TUint32 aOutlinePenColor, TUint32 aShadowColor,
															TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* dataBufferPtrLimit = aDataBuffer + aLength;
	TInt blendedRedColor;
	TInt blendedGreenColor;
	TInt blendedBlueColor;
	TInt blendedAlpha;
	TUint8 index = 0;
	TUint32 finalColor;

	//Get red color. Equivalent to TRgb::Red()
	const TInt redOutlinePenColor = (aOutlinePenColor & 0xff0000) >> 16;
	const TInt redShadowColor = (aShadowColor & 0xff0000) >> 16;
	const TInt redFillColor = (aFillColor & 0xff0000) >> 16;

	//Get green color. Equivalent to TRgb::Green()
	const TInt greenOutlinePenColor = (aOutlinePenColor & 0xff00) >> 8;
	const TInt greenShadowColor = (aShadowColor & 0xff00) >> 8;
	const TInt greenFillColor = (aFillColor & 0xff00) >> 8;

	//Get blue color. Equivalent to TRgb::Blue()
	const TInt blueOutlinePenColor = aOutlinePenColor & 0xff;
	const TInt blueShadowColor = aShadowColor & 0xff;
	const TInt blueFillColor = aFillColor & 0xff;

	//Get alpha color. Equivalent to TRgb::Alpha()
	const TInt alphaOutlinePenColor = aOutlinePenColor >> 24;
	const TInt alphaShadowColor = aShadowColor >> 24;
	const TInt alphaFillColor = aFillColor >> 24;

	// Calculate PMA values for aFillColor & aOutlinePenColor that we can use for fast blending in the simple cases
	// Don't pre calculate PMA version of aShadowColor as it is presumed not to be used enough to make this worthwhile
	const TUint32 pmaFillColor = NonPMA2PMAPixel(aFillColor);
	const TUint32 pmaOutlineColor = NonPMA2PMAPixel(aOutlinePenColor);

	while (aDataBuffer < dataBufferPtrLimit)
		{
		index = *aDataBuffer++;
		if (255 == FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			//background colour
			//No drawing required
			}
		else if (255 == FourColorBlendLookup[index][KFillColorIndex])
			{
			//Use fill colour to draw
			finalColor = pmaFillColor;
			PMAInplaceBlend(*pixelPtr, finalColor);
			}
		else if (255 == FourColorBlendLookup[index][KOutlineColorIndex])
			{
			//Use outline colour to draw
			finalColor = pmaOutlineColor;
			PMAInplaceBlend(*pixelPtr, finalColor);
			}
		else if (255 == FourColorBlendLookup[index][KShadowColorIndex])
			{
			//Use shadow colour to draw
			finalColor = NonPMA2PMAPixel(aShadowColor);
			PMAInplaceBlend(*pixelPtr, finalColor);
			}
		else
			{
			blendedRedColor = (redOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] * alphaOutlinePenColor + 
								redShadowColor * FourColorBlendLookup[index][KShadowColorIndex] * alphaShadowColor +
								redFillColor * FourColorBlendLookup[index][KFillColorIndex] * alphaFillColor) >> 16;
	
			blendedGreenColor = (greenOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] * alphaOutlinePenColor  + 
								greenShadowColor * FourColorBlendLookup[index][KShadowColorIndex] * alphaShadowColor +
								greenFillColor * FourColorBlendLookup[index][KFillColorIndex] * alphaFillColor) >> 16;
	
			blendedBlueColor = (blueOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] * alphaOutlinePenColor  + 
								blueShadowColor * FourColorBlendLookup[index][KShadowColorIndex] * alphaShadowColor +
								blueFillColor * FourColorBlendLookup[index][KFillColorIndex] * alphaFillColor) >> 16;
	
			blendedAlpha = (alphaOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
							alphaShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
							alphaFillColor * FourColorBlendLookup[index][KFillColorIndex]) >> 8;

			finalColor = (blendedAlpha << 24) | (blendedRedColor << 16) | (blendedGreenColor  << 8 )| (blendedBlueColor);
			PMAInplaceBlend(*pixelPtr, finalColor);
			}
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}
