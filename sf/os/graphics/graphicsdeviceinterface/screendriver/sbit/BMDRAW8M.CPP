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

// CDrawEightBppBitmapGray

TInt CDrawEightBppBitmapGray::Construct(TSize aSize)
	{
	return Construct(aSize, (aSize.iWidth + 3) & ~3);
	}

TInt CDrawEightBppBitmapGray::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EGray256;
	return CDrawEightBppBitmapCommon::Construct(aSize, aStride);
	}

void CDrawEightBppBitmapGray::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = TRgb::_Gray256(FadeGray(aColor._Gray256()));

	if (iShadowMode & EShadow)
		{
		TInt gray256 = Max(aColor._Gray256() - 85,0);
		aColor = TRgb::_Gray256(gray256);
		}
	}

TUint8 CDrawEightBppBitmapGray::ShadowAndFade(TInt aGray256)
	{
	if (iShadowMode & EFade)
		aGray256 = FadeGray(aGray256);

	if (iShadowMode & EShadow)
		aGray256 = Max(aGray256 - 85,0);

	return TUint8(aGray256);
	}

TRgb CDrawEightBppBitmapGray::ReadRgbNormal(TInt aX,TInt aY) const
	{
	return TRgb::_Gray256(*PixelAddress(aX,aY));
	}

void CDrawEightBppBitmapGray::ShadowArea(const TRect& aRect)
	{
	const TRect rect(DeOrientate(aRect));

	__ASSERT_DEBUG(rect.iTl.iX>=0 && rect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(rect.iTl.iY>=0 && rect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	const TInt longWidth = iLongWidth;
	TUint8* pixelPtr = PixelAddress(rect.iTl.iX,rect.iTl.iY);
	const TUint8* pixelRowPtrLimit = pixelPtr + (rect.Height() * longWidth);

	if (iShadowMode & EFade)
		{
		TUint8* pixelRowPtr = pixelPtr;
		TUint8* pixelPtrLimit = pixelPtr + rect.Width();

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint8* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				*tempPixelPtr = FadeGray(*tempPixelPtr);
				++tempPixelPtr;
				}

			pixelRowPtr += longWidth;
			pixelPtrLimit += longWidth;
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint8* pixelRowPtr = pixelPtr;
		TUint8* pixelPtrLimit = pixelPtr + rect.Width();

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint8* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				*tempPixelPtr = TUint8(Max(*tempPixelPtr - 85,0));
				++tempPixelPtr;
				}

			pixelRowPtr += longWidth;
			pixelPtrLimit += longWidth;
			}
		}
	}

void CDrawEightBppBitmapGray::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicInvalidParameter));
	__ASSERT_DEBUG(aBuffer!=NULL,Panic(EScreenDriverPanicInvalidParameter));

	TUint8* limit = ((TUint8*)aBuffer) + aLength;

	if (iShadowMode & EFade)
		{
		TUint8* buffer = (TUint8*)aBuffer;

		while(buffer < limit)
			{
			*buffer = FadeGray(*buffer);
			++buffer;
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint8* buffer = (TUint8*)aBuffer;

		while(buffer < limit)
			{
			*buffer = TUint8(Max(*buffer - 85,0));
			++buffer;
			}
		}
	}

void CDrawEightBppBitmapGray::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgb(aX,aY,TUint8(aColor._Gray256()));
	}

void CDrawEightBppBitmapGray::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteBinary(aX,aY,aData,aLength,aHeight,TUint8(aColor._Gray256()));
	}

void CDrawEightBppBitmapGray::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	CDrawEightBppBitmapCommon::WriteBinaryOp(aX,aY,aData,aLength,aHeight,TUint8(aColor._Gray256()),aDrawMode);
	}

void CDrawEightBppBitmapGray::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aLength,TRgb aColor,TBool aUp)
	{
	CDrawEightBppBitmapCommon::WriteBinaryLineVertical(aX,aY,aData,aLength,TUint8(aColor._Gray256()),aUp);
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawEightBppBitmapGray::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                const TUint8* aRgbBuffer,
                                                const TUint8* aMaskBuffer,
                                                MAlphaBlend::TShadowing aShadowing,
                                                CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	TRgb pixelClr;

	while (aMaskBuffer < maskBufferPtrLimit)
		{
        TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor);
            }
		TInt pixelValue = pixelPtr[0] * (255 - aMaskBuffer[0]);
		TInt srceValue = (((srcColor.Red() << 1) + 
                            srcColor.Green() + (srcColor.Green() << 2) + 
                            srcColor.Blue()) >> 3) * aMaskBuffer[0];

		pixelValue += srceValue;
		pixelClr =TRgb::_Gray256(pixelValue / 255);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelClr);
            }
		MapColorToUserDisplayMode(pixelClr);
		pixelPtr[0] = TUint8(pixelClr._Gray256());

		pixelPtr += pixelPtrInc;
		aRgbBuffer += 4;
		aMaskBuffer++;
		}
	}

void CDrawEightBppBitmapGray::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMulti(aX,aY,aLength,aRows,TUint8(aColor._Gray256()));
	}

void CDrawEightBppBitmapGray::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMultiXOR(aX,aY,aLength,aRows,TUint8(aColor._Gray256()));
	}

void CDrawEightBppBitmapGray::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMultiAND(aX,aY,aLength,aRows,TUint8(aColor._Gray256()));
	}

void CDrawEightBppBitmapGray::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMultiOR(aX,aY,aLength,aRows,TUint8(aColor._Gray256()));
	}

void CDrawEightBppBitmapGray::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);
	
	const TInt gray = aColor._Gray256();
	while (aMaskBuffer < maskBufferPtrLimit)
		{
		pixelPtr[0] = TUint8(((gray * aMaskBuffer[0]) + ((255 - aMaskBuffer[0]) * pixelPtr[0])) / 255);

		pixelPtr += pixelPtrInc;
		aMaskBuffer++;
		}
	}

void CDrawEightBppBitmapGray::MapColorToUserDisplayMode(TRgb& aColor)
	{
	switch (iUserDispMode)
		{
	case EGray2:
		aColor = TRgb::_Gray2(aColor._Gray2());
		break;
	case EGray4:
	case EColor16:
		aColor = TRgb::_Gray4(aColor._Gray4());
		break;
	case EGray16:
	case EColor256:
		aColor = TRgb::_Gray16(aColor._Gray16());
		break;
	default:
		break;
		}
	}

void CDrawEightBppBitmapGray::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint8* bufferPtr = (TUint8*)aBuffer;
	const TUint8* bufferLimit = bufferPtr + aLength;

	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			if (*bufferPtr & 0x80)
				*bufferPtr++ = 0xff;
			else
				*bufferPtr++ = 0;
			}
		break;
	case EGray4:
	case EColor16:
		while (bufferPtr < bufferLimit)
			{
			TUint8 gray4 = TUint8(*bufferPtr >> 6);
			gray4 |= (gray4 << 2);
			*bufferPtr++ = TUint8(gray4 | (gray4 << 4));
			}
		break;
	case EGray16:
	case EColor256:
		while (bufferPtr < bufferLimit)
			{
			TUint8 gray16 = TUint8(*bufferPtr >> 4);
			*bufferPtr++ = TUint8(gray16 | (gray16 << 4));
			}
		break;
	default:
		break;
		}
	}

TInt CDrawEightBppBitmapGray::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor,
	                                   TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	//This is non-optimised since this screen mode is rarely used and is usually 
	//fast enough without optimisation.
	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* dataBufferPtrLimit = aDataBuffer + aLength;
	TInt blendedRedColor;
	TInt blendedGreenColor;
	TInt blendedBlueColor;
	TUint8 index = 0;
	TRgb finalColor;

	TRgb outlinePenColor;
	outlinePenColor.SetInternal(aOutlinePenColor);
	TRgb shadowColor;
	shadowColor.SetInternal(aShadowColor);
	TRgb fillColor;
	fillColor.SetInternal(aFillColor);

	const TInt redOutlinePenColor = outlinePenColor.Red();
	const TInt redShadowColor = shadowColor.Red();
	const TInt redFillColor = fillColor.Red();

	const TInt greenOutlinePenColor = outlinePenColor.Green();
	const TInt greenShadowColor = shadowColor.Green();
	const TInt greenFillColor = fillColor.Green();

	const TInt blueOutlinePenColor = outlinePenColor.Blue();
	const TInt blueShadowColor = shadowColor.Blue();
	const TInt blueFillColor = fillColor.Blue();
	
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
			//fill colour
			finalColor.SetInternal(aFillColor);
			}
		else if (255 == FourColorBlendLookup[index][KShadowColorIndex])
			{
			//Shadow colour
			finalColor.SetInternal(aShadowColor);
			}
		else if (255 == FourColorBlendLookup[index][KOutlineColorIndex])
			{
			//Outline colour
			finalColor.SetInternal(aOutlinePenColor);
			}
		else
			{
			TRgb backgroundColor = TRgb::_Gray256(*pixelPtr);
	
			blendedRedColor = (redOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
						   		redShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
						  		redFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
						  		backgroundColor.Red() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			blendedGreenColor = (greenOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								greenShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
								greenFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
								backgroundColor.Green() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			blendedBlueColor = (blueOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								blueShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
								blueFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
								backgroundColor.Blue() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			finalColor = TRgb(blendedRedColor, blendedGreenColor, blendedBlueColor);
			}

		*pixelPtr = TUint8(finalColor._Gray256());
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}
