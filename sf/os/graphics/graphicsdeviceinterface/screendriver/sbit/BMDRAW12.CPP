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

#include "BMDRAW.H"

// CDrawTwelveBppBitmap

TInt CDrawTwelveBppBitmap::Construct(TSize aSize)
	{
	return Construct(aSize, ((aSize.iWidth + 1) & ~1) << 1);
	}

TInt CDrawTwelveBppBitmap::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EColor4K;
	return CDrawSixteenBppBitmapCommon::Construct(aSize, aStride);
	}

void CDrawTwelveBppBitmap::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = FadeRgb(TRgb::_Color4K(aColor._Color4K()));

	if (iShadowMode & EShadow)
		aColor = TRgb::_Color4K(ShadowIndex(TUint16(aColor._Color4K())));
	}

TUint16 CDrawTwelveBppBitmap::ShadowIndex(TUint16 aColor4KIndex)
	{
	TInt red = (aColor4KIndex & 0xf00) >> 8;
	TInt green = (aColor4KIndex & 0x0f0) >> 4;
	TInt blue = aColor4KIndex & 0x00f;

	red = Max(0,red-5);
	green = Max(0,green-5);
	blue = Max(0,blue-5);

	return TUint16((red << 8) | (green << 4) | blue);
	}

TUint16 CDrawTwelveBppBitmap::FadeIndex(TUint16 aColor4KIndex)
	{
	return TUint16(FadeRgb(TRgb::_Color4K(aColor4KIndex))._Color4K());
	}

TRgb CDrawTwelveBppBitmap::ReadRgbNormal(TInt aX,TInt aY) const
	{
	return TRgb::_Color4K(*PixelAddress(aX,aY));
	}

void CDrawTwelveBppBitmap::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	*PixelAddress(aX,aY) = TUint16(aColor._Color4K());
	}

void CDrawTwelveBppBitmap::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteBinary(aX,aY,aData,aLength,aHeight,(TUint16)aColor._Color4K());
	}

void CDrawTwelveBppBitmap::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	CDrawSixteenBppBitmapCommon::WriteBinaryOp(aX,aY,aData,aLength,aHeight,(TUint16)aColor._Color4K(),aDrawMode);
	}

void CDrawTwelveBppBitmap::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aHeight,TRgb aColor,TBool aUp)
	{
	CDrawSixteenBppBitmapCommon::WriteBinaryLineVertical(aX,aY,aData,aHeight,(TUint16)aColor._Color4K(),aUp);
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawTwelveBppBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                             const TUint8* aRgbBuffer,
                                             const TUint8* aMaskBuffer,
                                             MAlphaBlend::TShadowing aShadowing,
                                             CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	TRgb pixelColor;
	
	while (aMaskBuffer < maskBufferPtrLimit)
		{
   		TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor);
            }
        pixelColor = ::AlphaBlend(srcColor,TRgb::_Color4K(pixelPtr[0]),aMaskBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelColor);
            }
		MapColorToUserDisplayMode(pixelColor);
		pixelPtr[0] = TUint16(pixelColor._Color4K());

		pixelPtr += pixelPtrInc;
		aRgbBuffer += 4;
		aMaskBuffer++;
		}
	}

void CDrawTwelveBppBitmap::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMulti(aX,aY,aLength,aHeight,(TUint16)aColor._Color4K());
	}

void CDrawTwelveBppBitmap::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMultiXOR(aX,aY,aLength,aHeight,(TUint16)aColor._Color4K());
	}

void CDrawTwelveBppBitmap::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMultiAND(aX,aY,aLength,aHeight,(TUint16)aColor._Color4K());
	}

void CDrawTwelveBppBitmap::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMultiOR(aX,aY,aLength,aHeight,(TUint16)aColor._Color4K());
	}

void CDrawTwelveBppBitmap::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);

	const TInt red = aColor.Red();
	const TInt green = aColor.Green();
	const TInt blue = aColor.Blue();
	while (aMaskBuffer < maskBufferPtrLimit)
		{
		pixelPtr[0] = TUint16(AlphaBlend(red,green,blue,TRgb::_Color4K(pixelPtr[0]),aMaskBuffer[0])._Color4K());

		pixelPtr += pixelPtrInc;
		aMaskBuffer++;
		}
	}

void CDrawTwelveBppBitmap::MapColorToUserDisplayMode(TRgb& aColor)
	{
	switch (iUserDispMode)
		{
	case EGray2:
		aColor = TRgb::_Gray2(aColor._Gray2());
		break;
	case EGray4:
		aColor = TRgb::_Gray4(aColor._Gray4());
		break;
	case EGray16:
		aColor = TRgb::_Gray16(aColor._Gray16());
		break;
	case EGray256:
		aColor = TRgb::_Gray256(aColor._Gray256());
		break;
	case EColor16:
		aColor = TRgb::Color16(aColor.Color16());
		break;
	case EColor256:
		aColor = TRgb::Color256(aColor.Color256());
		break;
	default:
		break;
		}
	}

void CDrawTwelveBppBitmap::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint16* bufferPtr = (TUint16*)aBuffer;
	const TUint16* bufferLimit = bufferPtr + aLength;
	TRgb color;
	
	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color4K(*bufferPtr);
			color = TRgb::_Gray2(color._Gray2());
			*bufferPtr++ = TUint16(color._Color4K());
			}
		break;
	case EGray4:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color4K(*bufferPtr);
			color = TRgb::_Gray4(color._Gray4());
			*bufferPtr++ = TUint16(color._Color4K());
			}
		break;
	case EGray16:
	case EGray256: // EGray256 can't be done - nearest is EGray16
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color4K(*bufferPtr);
			color = TRgb::_Gray16(color._Gray16());
			*bufferPtr++ = TUint16(color._Color4K());
			}
		break;
	case EColor16:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color4K(*bufferPtr);
			color = TRgb::Color16(color.Color16());
			*bufferPtr++ = TUint16(color._Color4K());
			}
		break;
	case EColor256:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color4K(*bufferPtr);
			color = TRgb::Color256(color.Color256());
			*bufferPtr++ = TUint16(color._Color4K());
			}
		break;
	default:
		break;
		}
	}
	
TInt CDrawTwelveBppBitmap::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
													TUint32 aOutlinePenColor, TUint32 aShadowColor,
													TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* dataBufferPtrLimit = aDataBuffer + aLength;
	TInt blendedRedColor;
	TInt blendedGreenColor;
	TInt blendedBlueColor;
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

	while (aDataBuffer < dataBufferPtrLimit)
		{
		index = *aDataBuffer++;
		
		if (255 == FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			//background colour
			//No drawing required so move on to next pixel.
			pixelPtr += pixelPtrInc;
			continue;
			}
		else if (255 == FourColorBlendLookup[index][KFillColorIndex])
			{
			//Use fill colour to draw
			finalColor = aFillColor;
			}
		else if (255 == FourColorBlendLookup[index][KShadowColorIndex])
			{
			//Use shadow colour to draw
			finalColor = aShadowColor;
			}
		else if (255 == FourColorBlendLookup[index][KOutlineColorIndex])
			{
			//Use outline colour to draw
			finalColor = aOutlinePenColor;
			}
		else
			{
			//Get the background pixel colour. Equivalent to TRgb::_Color4K(TInt)
			TUint32 color = (*pixelPtr & 0xf00) << 8;
			color |= (*pixelPtr & 0x0f0) << 4;
			color |= (*pixelPtr & 0x00f);
			
			//Equivalent to TRgb::TRgb(TUint32, TInt) except that alpha is not set in background colour 
			//as it is not used for calculating final colour
			TUint32 backgroundColor = (((color | (color << 4)) & 0x00ffffff));

			blendedRedColor = (redOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								redShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
						  		redFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
						  		((backgroundColor & 0xff0000) >> 16) * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			blendedGreenColor = (greenOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								greenShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
								greenFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
								((backgroundColor & 0xff00) >> 8) * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			blendedBlueColor = (blueOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								blueShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
								blueFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
								(backgroundColor & 0xff) * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			//Equivalent to TRgb::TRgb(TUint32)
			finalColor = (blendedRedColor << 16) | (blendedGreenColor << 8) | blendedBlueColor | 0xff000000;
			}
		
		//Convert 32 bit to 4K colour. Equivalent to TRgb::_Color4K(TInt)
		TInt finalColor4K = (finalColor & 0x0000f0) >> 4;
		finalColor4K |= (finalColor & 0x00f000) >> 8;
		finalColor4K |= (finalColor & 0xf00000) >> 12;

		//Draw the final colour
		*pixelPtr = finalColor4K;
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}
