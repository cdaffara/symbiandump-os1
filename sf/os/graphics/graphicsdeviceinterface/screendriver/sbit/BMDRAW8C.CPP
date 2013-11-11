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

const TInt KEightBppPaletteSize = 256;
const TInt KEightBppInversePaletteSize = 4096;

GLREF_D const TUint8 color256shadowlutab[256];

// CDrawEightBppBitmapColor

TInt CDrawEightBppBitmapColor::Construct(TSize aSize)
	{
	return Construct(aSize, (aSize.iWidth + 3) & ~3);
	}

TInt CDrawEightBppBitmapColor::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EColor256;
	iShadowIndex = (TUint8*)color256shadowlutab;

	return CDrawEightBppBitmapCommon::Construct(aSize, aStride);
	}

CDrawEightBppBitmapColor::~CDrawEightBppBitmapColor()
	{
	delete[] iPalette;
	delete[] iColor4KIndex;
	if (iShadowIndex != color256shadowlutab)
		delete[] iShadowIndex;
	}

TInt CDrawEightBppBitmapColor::SetCustomPalette(const CPalette* aPalette)
	{
	if (aPalette)
		{
		if (!iPalette)
			iPalette = new TRgb[KEightBppPaletteSize];
		if (!iColor4KIndex)
			iColor4KIndex = new TUint8[KEightBppInversePaletteSize];
		if (iShadowIndex == color256shadowlutab)
			iShadowIndex = new TUint8[KEightBppPaletteSize];

		if (iPalette && iColor4KIndex && iShadowIndex)
			{
			TInt index = 0;
			TRgb* palettePtr = iPalette;
			const TRgb* const palettePtrLimit = iPalette + Min(aPalette->Entries(),KEightBppPaletteSize);

			while (palettePtr < palettePtrLimit)
				*palettePtr++ = aPalette->GetEntry(index++);

			index = 0;
			TUint8* inversePtr = iColor4KIndex;
			const TUint8* const inversePtrLimit = iColor4KIndex + KEightBppInversePaletteSize;

			while (inversePtr < inversePtrLimit)
				*inversePtr++ = TUint8(aPalette->NearestIndex(TRgb::_Color4K(index++)));

			index = 0;
			TUint8* shadowPtr = iShadowIndex;
			const TUint8* const shadowPtrLimit = iShadowIndex + KEightBppPaletteSize;
			TRgb color;

			while (shadowPtr < shadowPtrLimit)
				{
				color = iPalette[index++];
				const TInt red = Max(color.Red() - 0x33,0) >> 4;
				const TInt green = Max(color.Green() - 0x33,0) >> 4;
				const TInt blue = Max(color.Blue() - 0x33,0) >> 4;
				*shadowPtr++ = iColor4KIndex[(red << 8) | (green << 4) | blue];
				}

			return KErrNone;
			}

		// Fall through to cleanup
		}

	delete[] iPalette;
	iPalette = NULL;
	delete[] iColor4KIndex;
	iColor4KIndex = NULL;
	if (iShadowIndex != color256shadowlutab)
		{
		delete[] iShadowIndex;
		iShadowIndex = (TUint8*)color256shadowlutab;
		}

	return aPalette ? KErrNoMemory : KErrNone;
	}

TInt CDrawEightBppBitmapColor::GetCustomPalette(CPalette*& aPalette)
	{
	TRAPD(err, aPalette = CPalette::NewDefaultL(EColor256));

	if (err == KErrNone && iPalette)
		{
		for (TInt index = 0; index < KEightBppPaletteSize; index++)
			aPalette->SetEntry(index, iPalette[index]);
		}

	return err;
	}

TUint8 CDrawEightBppBitmapColor::ColorToIndex(TRgb aColor) const
	{
	if (iColor4KIndex)
		return iColor4KIndex[aColor._Color4K()];

	return TUint8(aColor.Color256());
	}

TRgb CDrawEightBppBitmapColor::IndexToColor(TInt aIndex) const
	{
	if (iPalette)
		return iPalette[aIndex];

	return TRgb::Color256(aIndex);
	}

void CDrawEightBppBitmapColor::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = FadeRgb(IndexToColor(ColorToIndex(aColor)));

	if (iShadowMode & EShadow)
		aColor = IndexToColor(iShadowIndex[ColorToIndex(aColor)]);
	}

//aX, aY - physical coordinates
TRgb CDrawEightBppBitmapColor::ReadRgbNormal(TInt aX,TInt aY) const
	{
	return IndexToColor(*PixelAddress(aX,aY));
	}

//aRect - logical coordinates
void CDrawEightBppBitmapColor::ShadowArea(const TRect& aRect)
	{
	const TRect rect(DeOrientate(aRect));//rect - physical coordinates

	__ASSERT_DEBUG(rect.iTl.iX>=0 && rect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(rect.iTl.iY>=0 && rect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	const TInt longWidth = iLongWidth;
	TUint8* pixelPtr = PixelAddress(rect.iTl.iX,rect.iTl.iY);
	const TUint8* pixelRowPtrLimit = pixelPtr + (rect.Height() * longWidth);
	TRgb color;
	register const TUint8* bitsEnd = 
		reinterpret_cast <const TUint8*> (iBits) + iLongWidth * iSize.iHeight;

	if(pixelRowPtrLimit >= bitsEnd)
		{
		pixelRowPtrLimit = bitsEnd;
		}
	
	if (iShadowMode & EFade)
		{
		TUint8* pixelRowPtr = pixelPtr;
		TUint8* pixelPtrLimit = pixelPtr + rect.Width();

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint8* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				color = IndexToColor(*tempPixelPtr);
				color = FadeRgb(color);
				*tempPixelPtr++ = ColorToIndex(color);
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
				*tempPixelPtr = iShadowIndex[*tempPixelPtr];
				++tempPixelPtr;
				}

			pixelRowPtr += longWidth;
			pixelPtrLimit += longWidth;
			}
		}
	}

void CDrawEightBppBitmapColor::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicInvalidParameter));
	__ASSERT_DEBUG(aBuffer!=NULL,Panic(EScreenDriverPanicInvalidParameter));

	const TUint8* limit = ((TUint8*)aBuffer) + aLength;

	if (iShadowMode & EFade)
		{
		TUint8* buffer = (TUint8*)aBuffer;
		TRgb color;
		
		while (buffer < limit)
			{
			color = FadeRgb(IndexToColor(*buffer));
			*buffer++ = ColorToIndex(color);
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint8* buffer = (TUint8*)aBuffer;

		while (buffer < limit)
			{
			*buffer = iShadowIndex[*buffer];
			++buffer;
			}
		}
	}

//aX, aY - physical coordinates
void CDrawEightBppBitmapColor::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgb(aX,aY,ColorToIndex(aColor));
	}

//aX, aY - physical coordinates
void CDrawEightBppBitmapColor::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteBinary(aX,aY,aData,aLength,aHeight,ColorToIndex(aColor));
	}

//aX, aY - physical coordinates
void CDrawEightBppBitmapColor::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	CDrawEightBppBitmapCommon::WriteBinaryOp(aX,aY,aData,aLength,aHeight,ColorToIndex(aColor),aDrawMode);
	}

//aX, aY - physical coordinates
void CDrawEightBppBitmapColor::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aLength,TRgb aColor,TBool aUp)
	{
	CDrawEightBppBitmapCommon::WriteBinaryLineVertical(aX,aY,aData,aLength,ColorToIndex(aColor),aUp);
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawEightBppBitmapColor::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                 const TUint8* aRgbBuffer,
                                                 const TUint8* aMaskBuffer,
                                                 MAlphaBlend::TShadowing aShadowing,
                                                 CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	DeOrientate(aX,aY);//aX, aY - physical coordinates
	TUint8* pixelPtr = PixelAddress(aX,aY);
	register TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	TRgb pixelColor;

	if(iScalingOff)
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
            TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
            if(aShadowing == MAlphaBlend::EShdwBefore)
                {
		        Shadow(srcColor);
                }
            pixelColor = ::AlphaBlend(srcColor,IndexToColor(pixelPtr[0]),aMaskBuffer[0]);
            if(aShadowing == MAlphaBlend::EShdwAfter)
                {
		        Shadow(pixelColor);
                }
			MapColorToUserDisplayMode(pixelColor);
			pixelPtr[0] = ColorToIndex(pixelColor);
			pixelPtr += pixelPtrInc;
			aRgbBuffer += 4;
			aMaskBuffer++;
			}
		}
	else
		{
		const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
		const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while (aMaskBuffer < maskBufferPtrLimit)
			{
            TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
            if(aShadowing == MAlphaBlend::EShdwBefore)
                {
		        Shadow(srcColor);
                }
            pixelColor = ::AlphaBlend(srcColor,IndexToColor(pixelPtr[0]),aMaskBuffer[0]);
            if(aShadowing == MAlphaBlend::EShdwAfter)
                {
		        Shadow(pixelColor);
                }
			MapColorToUserDisplayMode(pixelColor);
			const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			SetPixels(pixelPtr, ColorToIndex(pixelColor), pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			aRgbBuffer += 4;
			aMaskBuffer++;
			IncScaledY(aY);
			}
		}
	}

void CDrawEightBppBitmapColor::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMulti(aX,aY,aLength,aRows,ColorToIndex(aColor));
	}

void CDrawEightBppBitmapColor::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMultiXOR(aX,aY,aLength,aRows,ColorToIndex(aColor));
	}

void CDrawEightBppBitmapColor::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMultiAND(aX,aY,aLength,aRows,ColorToIndex(aColor));
	}

void CDrawEightBppBitmapColor::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aRows,TRgb aColor)
	{
	CDrawEightBppBitmapCommon::WriteRgbMultiOR(aX,aY,aLength,aRows,ColorToIndex(aColor));
	}

//aX, aY - not deorientated
//aLength - not scaled
void CDrawEightBppBitmapColor::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,
												  TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);//aX, aY - scaled and deorientated
	register TUint8* pixelPtr = PixelAddress(aX,aY);
	register TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);

	register TInt red = aColor.Red();
	register TInt green = aColor.Green();
	register TInt blue = aColor.Blue();
	TRgb pixelColor;

	if(iScalingOff)
		{
		while(aMaskBuffer < maskBufferPtrLimit)
			{
			pixelColor = AlphaBlend(red, green, blue, IndexToColor(pixelPtr[0]), aMaskBuffer[0]);
			pixelPtr[0] = ColorToIndex(pixelColor);
			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			}
		}
	else
		{
		const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
		const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(aMaskBuffer < maskBufferPtrLimit)
			{
			pixelColor = AlphaBlend(red, green, blue, IndexToColor(pixelPtr[0]), aMaskBuffer[0]);
			const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			SetPixels(pixelPtr, ColorToIndex(pixelColor), pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			IncScaledY(aY);
			}
		}
	}


void CDrawEightBppBitmapColor::MapColorToUserDisplayMode(TRgb& aColor)
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
	case EGray256:// EGray256 can't be done - nearest is EGray16
		aColor = TRgb::_Gray16(aColor._Gray16());
		break;
	case EColor16:
		aColor = TRgb::Color16(aColor.Color16());
		break;
	default:
		break;
		}
	}

void CDrawEightBppBitmapColor::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint8* bufferPtr = (TUint8*)aBuffer;
	const TUint8* bufferLimit = bufferPtr + aLength;
	TRgb color;
	
	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			color = IndexToColor(*bufferPtr);
			color = TRgb::_Gray2(color._Gray2());
			*bufferPtr++ = ColorToIndex(color);
			}
		break;
	case EGray4:
		while (bufferPtr < bufferLimit)
			{
			color = IndexToColor(*bufferPtr);
			color = TRgb::_Gray4(color._Gray4());
			*bufferPtr++ = ColorToIndex(color);
			}
		break;
	case EGray16:
	case EGray256:// EGray256 can't be done - nearest is EGray16
		while (bufferPtr < bufferLimit)
			{
			color = IndexToColor(*bufferPtr);
			color = TRgb::_Gray16(color._Gray16());
			*bufferPtr++ = ColorToIndex(color);
			}
		break;
	case EColor16:
		while (bufferPtr < bufferLimit)
			{
			color = IndexToColor(*bufferPtr);
			color = TRgb::Color16(color.Color16());
			*bufferPtr++ = ColorToIndex(color);
			}
		break;
	default:
		break;
		}
	}

TInt CDrawEightBppBitmapColor::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
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
			TRgb backgroundColor = TRgb::Color256(*pixelPtr);
	
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

		*pixelPtr = TUint8(finalColor.Color256());
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}

