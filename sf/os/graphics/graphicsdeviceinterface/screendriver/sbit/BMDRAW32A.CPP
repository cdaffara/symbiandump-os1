// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
Performs a blend based on the PD method, with 2* 16 bit in one 32 bit operation optimisation.
Note that this method assumes both input s are NOT alpha pre multiplied, but the output IS pre multiplied.
The mechanism of this method is non-standard and non-intuitive, and is being addressed in 9.3 
by developing distinct classes for the premultiplied and nonmultiplied cases.
The mechanism remains hybrid to maintain compatibility and colour channel overflows caused by rounding
overflows have been addressed.
PD is basically CDest=CSrc*MulSrc+CDest*MulDest
//A second optimisation is that multiplier value must be scaled from 0..255 to 0.0..1.0, this is 1/255 = 0.00392156.
A second optimisation is that multiplier value must be scaled from 257..65535 to 0.0..1.0, this is 1/65535 = 0.0000152590.
This used to be done by simply adding 1, giving 1..256/256 
but this allows overflows to occur when the fractional parts are added together. 
We want to keep adding the fractional parts as that gives a better result.
Instead I am now using an approximation of 0.00392151, based on 255*257/256.
To describe it imagine a "decimal" colour mode, where the channels count from 0..9, and 10-based operations are "efficient".
We need 9 to generate 1.0 or 10/10 meaning "all".
If we add 1, then we get 1..10 ==> 0.1 .. 1.0
If we multiply by 1.1 then we get 0.0 .. 9.9. 
And finally add a 0.5 rounding to the result.
I do this multiply after the CSrc*MulSrc, while I still have a 16bit intermediate result.
It is possible to generate a faster, less accurate result by exhaustively finding
the highest value that can be added instead of rb = rb+((rb>>8)&0x00ff00ff)+0x00800080; without causing an overflow.
@param aBeneath non-premultiplied color with alpha of the destination 
@param aSrcColor non-premultiplied color with alpha of the source
@param aMaskBuffer mask
@return pre multiplied color resulting from the blending operation
*/
FORCEINLINE TUint32 OptimizedBlend32A(TUint32 aBeneath,TUint32 aSrcColor,TUint8 aMaskBuffer)
 	{
	if(aMaskBuffer)
		{
		if(aMaskBuffer == 0xff) // opaque, so unchanged
			{
			//Still need to convert source to destination from non-multiplied to pre-multiplied
			//But this code resolves to a copy. The ARM optimiser prefers shifts over big constants.
			return (aSrcColor|(aMaskBuffer<<24));
			}
		else
			{
			//0, 1, 2, 3 
			//b, g, r, alpha  

			const TUint32 srcMult = aMaskBuffer;
			TUint32 destMult = ((255 - aMaskBuffer) * ((aBeneath >> 24)));
			//This gives a slightly more accurate result than ((aBeneath >> 24)+1)
			destMult=destMult+(destMult>>8);	 
			destMult+= 0x0080;	  
			destMult >>= 8;

			TUint32 rb =(((aSrcColor&0x00ff00ff)*srcMult)) + (((aBeneath&0x00ff00ff)*destMult));
			rb = rb+((rb>>8)&0x00ff00ff);
			rb+=0x00800080;
			rb>>=8;
			TUint32 ag = (((aSrcColor&0x0000ff00)*srcMult)) + (((aBeneath&0x0000ff00)*destMult));
			ag>>=8;	 //Note that if alpha is processed here, this shift must be performed before the multiplies
			ag = ag+((ag>>8)&0x00ff00ff);
			ag+=0x00800080;
			TUint32 aa = srcMult+destMult;
			return (rb&0x00ff00ff) | (ag&0x0000ff00) | (aa << 24);
			
			}
		}
 	else // completely transparent
		{
		return aBeneath;
 		}

 	}

TInt CDrawThirtyTwoBppBitmapAlpha::Construct(TSize aSize)
	{
	return Construct(aSize, aSize.iWidth << 2);
	}

TInt CDrawThirtyTwoBppBitmapAlpha::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EColor16MA;
	return CDrawThirtyTwoBppBitmapCommon::Construct(aSize, aStride);
	}

void CDrawThirtyTwoBppBitmapAlpha::Shadow(TUint32& aColor)
	{
	TUint32 value = aColor & 0x00ffffff;
	if (iShadowMode & EFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		value = ((value >> 1) & ~0x00808080) + (SYMBIAN_USE_FAST_FADING);
#else		
		const TInt wordFadeMapOffset = ((iFadeMapOffset & 0xff) << 16) | (iFadeMapOffset & 0xff);
		const TInt rb = ((((value & 0x00ff00ff) * iFadeMapFactor) >> 8) + wordFadeMapOffset) & 0x00ff00ff;
	  	const TInt g = ((((value & 0x0000ff00) * iFadeMapFactor) >> 16) + iFadeMapOffset) << 8;
		value = rb | g;
#endif		
		}

	if (iShadowMode & EShadow)
		{
		const TInt r = (value & 0x00c00000) ? ((value & 0x00ff0000)-0x00400000) : 0;
		const TInt g = (value & 0x0000c000) ? ((value & 0x0000ff00)-0x00004000) : 0;
		const TInt b = (value & 0x000000c0) ? ((value & 0x000000ff)-0x00000040) : 0;
		value = r | g | b;
		}
	// alpha is unchanged.
	aColor = (aColor & 0xff000000) | value;
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawThirtyTwoBppBitmapAlpha::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                  const TUint8* aRgbBuffer,
                                                  const TUint8* aMaskBuffer,
                                                  MAlphaBlend::TShadowing aShadowing,
                                                  CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
    // precondition for this function is that the aRgbBuffer lies on a word boundary
    // Assert checks that the pointer is at a word boundary
    __ASSERT_DEBUG(!(((TUint)aRgbBuffer) & 0x3), Panic(EScreenDriverPanicInvalidPointer));
    
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	
	// The purpose of this conditional is to remove if statements from within the loop 
	// if shadow mode is not enabled and the UserDispMode is none or EColor16MA
	if(!(iShadowMode & (EFade | EShadow)) && (iUserDispMode ==EColor16MA || iUserDispMode == ENone))
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			if(*aMaskBuffer)
				{
				*pixelPtr = OptimizedBlend32A(*pixelPtr, *((TUint32*)(aRgbBuffer)), *aMaskBuffer);
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
			if(*aMaskBuffer)
				{
				TUint32 srcColor = *((TUint32*)(aRgbBuffer));
				if(aShadowing == MAlphaBlend::EShdwBefore)
					{
					Shadow(srcColor);
					}
				
				TUint32 pixelClr = 0x0;
				
				pixelClr = OptimizedBlend32A(*pixelPtr, srcColor, *aMaskBuffer);

				if(aShadowing == MAlphaBlend::EShdwAfter)
					{
					Shadow(pixelClr);
					}
					
				if(iUserDispMode !=EColor16MA && iUserDispMode != ENone)
					{
					TInt red = TUint8(pixelClr >> 16);
					TInt green = TUint8(pixelClr >> 8);
					TInt blue = TUint8(pixelClr);
					CDrawBitmap::MapColorToUserDisplayMode(red,green,blue);
					pixelClr = (pixelClr&0xff000000) | (red << 16) | (green << 8) | blue;
					}
				*pixelPtr = pixelClr;
				}
			pixelPtr += pixelPtrInc;
			aRgbBuffer += 4;
			aMaskBuffer++;
			}
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	TUint32 srcColor = aColor.Internal();
	if (iShadowMode)
		Shadow(srcColor);

	const TInt red   = (srcColor & 0x00ff0000) >> 16;
	const TInt green = (srcColor & 0x0000ff00) >> 8;
	const TInt blue  = srcColor & 0x000000ff;
	const TInt alpha = srcColor >> 24;

	if (alpha == 255)
		{
		// the most common case
		// source is opaque, so we simply blend it using the mask
		while (aMaskBuffer < maskBufferPtrLimit)
			{

			*pixelPtr = OptimizedBlend32A(*pixelPtr, srcColor, *aMaskBuffer);

			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			}
		}
	else
		{
		// pen is semi-transparent, so we must blend using both the mask and pen alpha
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			TUint8* componentPtr = reinterpret_cast <TUint8*> (pixelPtr);
			const TUint32 srcAlpha = alpha * aMaskBuffer[0];
			const TUint32 srcMult = srcAlpha * 255;
			const TUint32 destMult = (255*255 - srcAlpha) * componentPtr[3];
			componentPtr[0] = TUint8(((blue * srcMult) + (componentPtr[0] * destMult)) / (255*255*255));
			componentPtr[1] = TUint8(((green * srcMult) + (componentPtr[1] * destMult)) / (255*255*255));
			componentPtr[2] = TUint8(((red * srcMult) + (componentPtr[2] * destMult)) / (255*255*255));
			componentPtr[3] = TUint8((srcMult + destMult) / (255*255));
			// ie alpha' = srcAlpha + (1-srcAlpha)*destAlpha 

			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			}
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();

	if(sourceAlpha==255)
		{
		TUint32* componentPtr = PixelAddress(aX,aY);
		*componentPtr=aColor.Internal();
		}
	else if (sourceAlpha==0)
		{
		return;
		}
	else
		{
		TUint8* componentPtr = reinterpret_cast <TUint8*> (PixelAddress(aX,aY));
		const TUint32 srcMult = sourceAlpha * 255;
		const TUint32 destMult = (255 - sourceAlpha) * componentPtr[3];
		componentPtr[0] = TUint8(((aColor.Blue() * srcMult) + (componentPtr[0] * destMult)) / (255*255));
		componentPtr[1] = TUint8(((aColor.Green() * srcMult) + (componentPtr[1] * destMult)) / (255*255));
		componentPtr[2] = TUint8(((aColor.Red() * srcMult) + (componentPtr[2] * destMult)) / (255*255));
		componentPtr[3] = TUint8((srcMult + destMult) / 255);
		// ie alpha' = srcAlpha + (1-srcAlpha)*destAlpha 
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)
		{
		CDrawThirtyTwoBppBitmapCommon::WriteBinary(aX,aY,aBuffer,aLength,aHeight,aColor);
		return;
		}
	if (sourceAlpha==0)
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

	const TInt sourceRed = aColor.Red();
	const TInt sourceGreen = aColor.Green();
	const TInt sourceBlue = aColor.Blue();

	while (aBuffer < dataLimit)
		{
		TUint32 dataWord = *aBuffer++;
		TUint32 dataMask = 1;
		TUint32* tempPixelPtr = pixelPtr;

		while (dataMask != dataMaskLimit)
			{
			if(dataWord & dataMask)
				{
				TUint8* componentPtr = reinterpret_cast <TUint8*> (tempPixelPtr);
				const TUint32 srcMult = sourceAlpha * 255;
				const TUint32 destMult = (255 - sourceAlpha) * componentPtr[3];
				componentPtr[0] = TUint8(((sourceBlue * srcMult) + (componentPtr[0] * destMult)) / (255*255));
				componentPtr[1] = TUint8(((sourceGreen * srcMult) + (componentPtr[1] * destMult)) / (255*255));
				componentPtr[2] = TUint8(((sourceRed * srcMult) + (componentPtr[2] * destMult)) / (255*255));
				componentPtr[3] = TUint8((srcMult + destMult) / 255);
				// ie alpha' = srcAlpha + (1-srcAlpha)*destAlpha 
				}

			tempPixelPtr += pixelInc;
			dataMask <<= 1;
			}

		pixelPtr += rowInc;
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aHeight,TRgb aColor,TBool aUp)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)
		{
		CDrawThirtyTwoBppBitmapCommon::WriteBinaryLineVertical(aX,aY,aBuffer,aHeight,aColor,aUp);
		return;
		}
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

	const TInt sourceRed = aColor.Red();
	const TInt sourceGreen = aColor.Green();
	const TInt sourceBlue = aColor.Blue();

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
			TUint8* componentPtr = reinterpret_cast <TUint8*> (pixelPtr);
			const TUint32 srcMult = sourceAlpha * 255;
			const TUint32 destMult = (255 - sourceAlpha) * componentPtr[3];
			componentPtr[0] = TUint8(((sourceBlue * srcMult) + (componentPtr[0] * destMult)) / (255*255));
			componentPtr[1] = TUint8(((sourceGreen * srcMult) + (componentPtr[1] * destMult)) / (255*255));
			componentPtr[2] = TUint8(((sourceRed * srcMult) + (componentPtr[2] * destMult)) / (255*255));
			componentPtr[3] = TUint8((srcMult + destMult) / 255);
			// ie alpha' = srcAlpha + (1-srcAlpha)*destAlpha 
			}

		dataMask <<= 1;
		pixelPtr += scanlineWordLength;
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* bufferLimit = aBuffer + aLength;
	TRgb color;
	
	switch (iUserDispMode)
		{
	case EGray2:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::_Gray2(color._Gray2());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EGray4:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::_Gray4(color._Gray4());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EGray16:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::_Gray16(color._Gray16());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EGray256:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::_Gray256(color._Gray256());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EColor16:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::Color16(color.Color16());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EColor256:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::Color256(color.Color256());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EColor4K:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::_Color4K(color._Color4K());
			*aBuffer++ = color._Color16MA();
			}
		break;
	case EColor64K:
		while (aBuffer < bufferLimit)
			{
			color = TRgb::_Color16MA(*aBuffer);
			color = TRgb::_Color64K(color._Color64K());
			*aBuffer++ = color._Color16MA();
			}
		break;
	default:
		break;
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
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

	const TInt sourceRed = aColor.Red();
	const TInt sourceGreen = aColor.Green();
	const TInt sourceBlue = aColor.Blue();
	TUint srcValue = aColor._Color16MA();
	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			// check that the alpha value is not 0xFF
			if((*tempPixelPtr & 0xFF000000) ^ 0xFF000000)
				{	
				TUint8* componentPtr = reinterpret_cast <TUint8*> (tempPixelPtr);
				const TUint32 srcMult = sourceAlpha * 255;
				const TUint32 destMult = (255 - sourceAlpha) * componentPtr[3];
				componentPtr[0] = TUint8(((sourceBlue * srcMult) + (componentPtr[0] * destMult)) / (255*255));
				componentPtr[1] = TUint8(((sourceGreen * srcMult) + (componentPtr[1] * destMult)) / (255*255));
				componentPtr[2] = TUint8(((sourceRed * srcMult) + (componentPtr[2] * destMult)) / (255*255));
				componentPtr[3] = TUint8((srcMult + destMult) / 255);
				}
			else
				{	
				AlphaBlendPixelToDest(srcValue, (TUint8)sourceAlpha, tempPixelPtr);
				}

			// ie alpha' = srcAlpha + (1-srcAlpha)*destAlpha 
			}

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawThirtyTwoBppBitmapAlpha::BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);

	const TUint32* bufferPtrLimit = aBuffer + aLength;
	const TInt pixelPtrInc = (iOrientation == EOrientationNormal) ? 1 : PixelAddressIncrement();
				
	while (aBuffer < bufferPtrLimit)
		{
		*pixelPtr = OptimizedBlend32A(*pixelPtr,  *aBuffer, (*aBuffer)>>24);
		
		aBuffer++;
		pixelPtr += pixelPtrInc;
		}
	}

TRgb CDrawThirtyTwoBppBitmapAlpha::RgbColor(TUint32 aColor) const
	{
	return TRgb::_Color16MA(aColor);
	}

TUint32 CDrawThirtyTwoBppBitmapAlpha::Color(const TRgb& aColor)
	{
	return aColor._Color16MA();	
	}

void CDrawThirtyTwoBppBitmapAlpha::ShadowArea(const TRect& aRect)
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
#if !defined(SYMBIAN_USE_FAST_FADING)
		const TInt wordFadeMapOffset = ((iFadeMapOffset & 0xff) << 16) | (iFadeMapOffset & 0xff);
#endif
		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint32* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				const TUint32 color = *tempPixelPtr;
#if defined(SYMBIAN_USE_FAST_FADING)
				*tempPixelPtr++ = (color & 0xff000000) | ((((color & 0x00ffffff) >> 1) & ~0x00808080) + (SYMBIAN_USE_FAST_FADING));
#else
				const TInt rb = ((((color & 0x00ff00ff) * iFadeMapFactor) >> 8) + wordFadeMapOffset) & 0x00ff00ff;
			  	const TInt g = ((((color & 0x0000ff00) * iFadeMapFactor) >> 16) + iFadeMapOffset) << 8;
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
				const TInt r = (color & 0x00c00000) ? ((color & 0x00ff0000)-0x00400000) : 0;
				const TInt g = (color & 0x0000c000) ? ((color & 0x0000ff00)-0x00004000) : 0;
				const TInt b = (color & 0x000000c0) ? ((color & 0x000000ff)-0x00000040) : 0;
				*tempPixelPtr++	= (color & 0xff000000) | r | g | b;
				}

			pixelRowPtr += iScanLineWords;
			pixelPtrLimit += iScanLineWords;
			}
		}
	}

TInt CDrawThirtyTwoBppBitmapAlpha::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
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
	const TUint16* normTable = PtrTo16BitNormalisationTable();

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

	while (aDataBuffer < dataBufferPtrLimit)
		{
		index = *aDataBuffer++;
		if(255 == FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			//background colour
			//No drawing required
			}
		else if (255 == FourColorBlendLookup[index][KFillColorIndex])
			{
			//Use fill colour to draw
			finalColor = OptimizedBlend32A(*pixelPtr, aFillColor, alphaFillColor);
			*pixelPtr = PMA2NonPMAPixel(finalColor, normTable);
			}
		else if (255 == FourColorBlendLookup[index][KShadowColorIndex])
			{
			//Use shadow colour to draw
			finalColor = OptimizedBlend32A(*pixelPtr, aShadowColor, alphaShadowColor);
			*pixelPtr = PMA2NonPMAPixel(finalColor, normTable);
			}
		else if (255 == FourColorBlendLookup[index][KOutlineColorIndex])
			{
			//Use outline colour to draw
			finalColor = OptimizedBlend32A(*pixelPtr, aOutlinePenColor, alphaOutlinePenColor);
			*pixelPtr = PMA2NonPMAPixel(finalColor, normTable);
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

			// The blended colours have been alpha multiplied, hence the resulting colour is 16MAP
			// Before doing the OptimizedBlend with the destination, note the following
			//	- The source alpha is set as fully opaque so that the blend is just with the mask
			//	- Input parameters for OptimizedBlend are NON-PRE, hence conversion from PRE to NON-PRE beforehand
			//	- output parameter for OptimizedBlend is PRE, hence conversion from PRE to NON-PRE afterwards
			finalColor = PMA2NonPMAPixel((blendedAlpha << 24) | (blendedRedColor << 16) | (blendedGreenColor << 8) | blendedBlueColor, normTable);
			finalColor = OptimizedBlend32A(*pixelPtr, finalColor | 0xff000000, blendedAlpha);
			*pixelPtr = PMA2NonPMAPixel(finalColor, normTable);
			}
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}
