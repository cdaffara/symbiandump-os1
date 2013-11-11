// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>
#include "BMDRAW.H"
#include "BitDrawInterfaceId.h"

FORCEINLINE void BlendFromRBandG(TUint32 *aPixelPtr, TUint32 aSrcRB, TUint32 aSrcG, TUint32 aSrcAlpha, TUint32 aMaskX2)
	{
	const TUint32 d = *aPixelPtr;
	const TUint32 d_rb = d & 0x00FF00FF;
	const TUint32 rb = ((((aSrcAlpha * ((0x01000100 + aSrcRB) - d_rb)) >> 8) + d_rb) - aMaskX2) & 0x00FF00FF;

	const TInt d_g = (d & 0xFF00) >> 8;
	const TInt g = ((aSrcAlpha * (aSrcG - d_g)) >> 8) + d_g;

	*aPixelPtr = rb | (g<<8) | 0xff000000;
	}

/**Initializes iSize, iDrawRect, iLongWidth, iScanLineBytes, iScanlineWords data members.
 It should be called every time when iSize is going to be changed - from Construct().

 @param aSize Physical screen size in pixels.
 @panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the
 device is scaled and the scaling origin goes outside physical drawing rectangle. */
void CDrawThirtyTwoBppBitmapCommon::SetSize(const TSize& aSize)
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = iSize.iWidth;
	iScanLineWords = iSize.iWidth;
	}

TInt CDrawThirtyTwoBppBitmapCommon::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride & 3)
		return KErrArgument;
	iLongWidth = aStride >> 2;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = iLongWidth;
	TInt size = Max(aSize.iWidth,aSize.iHeight) << 2;
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if (iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

void CDrawThirtyTwoBppBitmapCommon::Shadow(TRgb& aColor)
	{
	TUint32 value = aColor.Internal();
	const TInt alpha = value >> 24;

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

	aColor = TRgb(value,alpha);
	}

void CDrawThirtyTwoBppBitmapCommon::Shadow(TUint32& aColor)
	{
	// aColor is in the format indicated by ScanLineDisplayMode(), which we
	// assume is EColor16MAP here. If not, this function must be overridden.
	const TInt alpha = (aColor >> 24) + 1;
	TUint32 value = aColor & 0x00ffffff;
	if (iShadowMode & EFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		const TUint32 fast_fade_offset = NonPMA2PMAPixel((aColor & 0xff000000) | SYMBIAN_USE_FAST_FADING) & 0x00ffffff;
		value = ((value >> 1) & ~0x00808080) + (fast_fade_offset);
#else
		const TInt fadeMapOffset = ((alpha * iFadeMapOffset) >> 8) & 0xff;
		const TInt wordFadeMapOffset = ((fadeMapOffset) << 16) | (fadeMapOffset);
		const TInt rb = ((((value & 0x00ff00ff) * iFadeMapFactor) >> 8) + wordFadeMapOffset) & 0x00ff00ff;
	  	const TInt g = ((((value & 0x0000ff00) * iFadeMapFactor) >> 16) + fadeMapOffset) << 8;
		value = rb | g;
#endif
		}

	if (iShadowMode & EShadow)
		{
		const TInt uLimit = ((0x40) * alpha) >> 8;
		TInt r = (value >> 16) & 0xff;
		r = (r > uLimit) ? (r-uLimit) : 0;
		TInt g = (value >> 8) & 0xff;
		g = (g > uLimit) ? (g - uLimit) : 0;
		TInt b = value & 0xff;
		b = (b > uLimit) ? (b - uLimit) : 0;
		value = (r << 16) | (g << 8) | b;
		}
	// alpha is unchanged.
	aColor = (aColor & 0xff000000) | value;
	}

TUint8 CDrawThirtyTwoBppBitmapCommon::ShadowAndFade(TInt aComponent)
	{
	if (iShadowMode & EFade)
		aComponent = FadeGray(aComponent);

	if (iShadowMode & EShadow)
		aComponent = ShadowComponent(aComponent);

	return TUint8(aComponent);
	}

TUint8 CDrawThirtyTwoBppBitmapCommon::ShadowComponent(TInt aRgbComponent)
	{
	return TUint8(Max(0,aRgbComponent-0x40));
	}

void CDrawThirtyTwoBppBitmapCommon::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	TUint32* limit = aBuffer + aLength;

	while (aBuffer < limit)
		{
		*aBuffer++ ^= 0x00ffffff;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
	{
	const TUint32* pixelPtr = PixelAddress(aX,aY);

	if (iOrientation == EOrientationNormal)
		Mem::Copy(aBuffer,pixelPtr,aLength << 2);
	else
		{
		const TInt pixelPtrInc = PixelAddressIncrement();

		TUint32* bufferPtr = static_cast <TUint32*> (aBuffer);
		const TUint32* bufferPtrLimit = bufferPtr + aLength;

		while (bufferPtr < bufferPtrLimit)
			{
			*bufferPtr++ = *pixelPtr;
			pixelPtr += pixelPtrInc;
			}
		}
	}

TRgb CDrawThirtyTwoBppBitmapCommon::ReadRgbNormal(TInt aX,TInt aY) const
	{
	return RgbColor(*PixelAddress(aX,aY));
	}

void CDrawThirtyTwoBppBitmapCommon::ShadowArea(const TRect& aRect)
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
#if defined(SYMBIAN_USE_FAST_FADING)
				*tempPixelPtr++ = 0xff000000 | ((((*tempPixelPtr) >> 1) & ~0x00808080) + (SYMBIAN_USE_FAST_FADING));
#else
				const TUint32 color = *tempPixelPtr;
				const TInt rb = ((((color & 0x00ff00ff) * iFadeMapFactor) >> 8) + wordFadeMapOffset) & 0x00ff00ff;
	  			const TInt g = ((((color & 0x0000ff00) * iFadeMapFactor) >> 16) + iFadeMapOffset) << 8;
				*tempPixelPtr++ = 0xff000000 | rb | g;
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
				*tempPixelPtr++	= 0xff000000 | r | g | b;
				}

			pixelRowPtr += iScanLineWords;
			pixelPtrLimit += iScanLineWords;
			}
		}
	}

void CDrawThirtyTwoBppBitmapCommon::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	TUint32* limit = aBuffer + aLength;

	while (aBuffer < limit)
		Shadow(*aBuffer++);
	}

void CDrawThirtyTwoBppBitmapCommon::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TUint8* componentPtr = reinterpret_cast <TUint8*> (PixelAddress(aX,aY));
	const TInt sourceAlpha = aColor.Alpha();

	if (sourceAlpha==0)
		return;
	if(sourceAlpha != 0xff)
		{
		aColor = AlphaBlend(aColor.Red(), aColor.Green(), aColor.Blue(), TRgb(componentPtr[2], componentPtr[1], componentPtr[0]), sourceAlpha);
		}

	componentPtr[0] = TUint8(aColor.Blue());
	componentPtr[1] = TUint8(aColor.Green());
	componentPtr[2] = TUint8(aColor.Red());
	}

void CDrawThirtyTwoBppBitmapCommon::WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if(sourceAlpha == 0)
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

	if(sourceAlpha == 255) //we split code on two parts because of performance reasons
		{
		TInt color = Color(aColor);
		while (aBuffer < dataLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint32* tempPixelPtr = pixelPtr;

			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					*tempPixelPtr = color;
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	else //sourceAlpha != 255
		{
		const TUint32 sourceInternal=aColor.Internal();
		const TUint32 s_rb = sourceInternal & 0x00FF00FF;
		const TUint32 s_g = (sourceInternal & 0xFF00) >> 8;
		const TUint32 mask2 = sourceAlpha | (sourceAlpha << 16);
		while (aBuffer < dataLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint32* tempPixelPtr = pixelPtr;

			while (dataMask != dataMaskLimit)
				{
				if (dataWord & dataMask)
					{
					BlendFromRBandG(tempPixelPtr,s_rb,s_g,sourceAlpha,mask2);
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	if (aLength <= 0)
		return;

	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);

	const TUint32* dataPtrLimit = aBuffer + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;

	TInt pixelInc;
	TInt rowInc;

	if (iOrientation == EOrientationNormal)
		{
		pixelInc = 1;
		rowInc = iScanLineWords;
		}
	else if (iOrientation == EOrientationRotated90)
		{
		pixelInc = iScanLineWords;
		rowInc = -1;
		}
	else if (iOrientation == EOrientationRotated180)
		{
		pixelInc = -1;
		rowInc = -iScanLineWords;
		}
	else // EOrientationRotated270
		{
		pixelInc = -iScanLineWords;
		rowInc = 1;
		}

	TInt color = Color(aColor);// & 0x00FFFFFF;

	if (color != 0)
		{
		while (aBuffer < dataPtrLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint32* tempPixelPtr = pixelPtr;

			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					if(aDrawMode==CGraphicsContext::EDrawModeXOR)
						{
						*tempPixelPtr ^= color;
						}
					else if(aDrawMode==CGraphicsContext::EDrawModeAND)
						{
						*tempPixelPtr &= color;
						}
					else if(aDrawMode==CGraphicsContext::EDrawModeOR)
						{
						*tempPixelPtr |= color;
						}
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	else if (aDrawMode == CGraphicsContext::EDrawModeAND)
		{
		while (aBuffer < dataPtrLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint32* tempPixelPtr = pixelPtr;

			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					*tempPixelPtr = 0;
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aHeight,TRgb aColor,TBool aUp)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if(sourceAlpha == 0)
		return;
	DeOrientate(aX,aY);

	TInt scanlineWordLength;

	if (iOrientation == EOrientationNormal)
		scanlineWordLength = iScanLineWords;
	else if (iOrientation == EOrientationRotated90)
		scanlineWordLength = -1;
	else if (iOrientation == EOrientationRotated180)
		scanlineWordLength = -iScanLineWords;
	else // EOrientationRotated270
		scanlineWordLength = 1;

	if (aUp)
		scanlineWordLength = -scanlineWordLength;

	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TUint32* pixelPtrLimit = pixelPtr + (aHeight * scanlineWordLength);
	TUint32 dataWord = *aBuffer;
	TUint32 dataMask = 1;

	if(sourceAlpha == 255) //we split code on two parts because of performance reasons
		{
		TInt color = Color(aColor);
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
				*pixelPtr = color;
				}

			dataMask <<= 1;
			pixelPtr += scanlineWordLength;
			}
		}
	else //sourceAlpha != 255
		{
		const TUint32 sourceInternal=aColor.Internal();
		const TUint32 s_rb = sourceInternal & 0x00FF00FF;
		const TUint32 s_g = (sourceInternal & 0xFF00) >> 8;
		const TUint32 mask2 = sourceAlpha | (sourceAlpha << 16);
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
				BlendFromRBandG(pixelPtr, s_rb, s_g, sourceAlpha, mask2);
				}

			dataMask <<= 1;
			pixelPtr += scanlineWordLength;
			}
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);

	if (iOrientation == EOrientationNormal)
		Mem::Copy(pixelPtr,aBuffer,aLength << 2);
	else
		{
		const TInt pixelPtrInc = PixelAddressIncrement();

		TUint32* bufferPtrLimit = aBuffer + aLength;

		while (aBuffer < bufferPtrLimit)
			{
			*pixelPtr = *aBuffer++;
			pixelPtr += pixelPtrInc;
			}
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();

	TUint32* bufferPtrLimit = aBuffer + aLength;

	while (aBuffer < bufferPtrLimit)
		{
		*pixelPtr ^= *aBuffer++;
		pixelPtr += pixelPtrInc;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();

	TUint32* bufferPtrLimit = aBuffer + aLength;

	while (aBuffer < bufferPtrLimit)
		{
		*pixelPtr &= *aBuffer++;
		pixelPtr += pixelPtrInc;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();

	TUint32* bufferPtrLimit = aBuffer + aLength;

	while (aBuffer < bufferPtrLimit)
		{
		*pixelPtr |= *aBuffer++;
		pixelPtr += pixelPtrInc;
		}
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawThirtyTwoBppBitmapCommon::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                  const TUint8* aRgbBuffer,
                                                  const TUint8* aMaskBuffer,
                                                  MAlphaBlend::TShadowing aShadowing,
                                                  CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	while (aMaskBuffer < maskBufferPtrLimit)
		{
		TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);// !! we don't have any alpha information for the source, so assume opaque
		if(aMaskBuffer[0])
			{
			if(aShadowing == MAlphaBlend::EShdwBefore)
				{
				Shadow(srcColor);
				}
			TUint8* componentPtr = reinterpret_cast <TUint8*> (pixelPtr);
			if(aMaskBuffer[0] != 0xff)
				{
				srcColor = AlphaBlend(srcColor.Red(), srcColor.Green(), srcColor.Blue(), TRgb(componentPtr[2], componentPtr[1], componentPtr[0]), aMaskBuffer[0]);
				}
			if(aShadowing == MAlphaBlend::EShdwAfter)
				{
				Shadow(srcColor);
				}
			MapColorToUserDisplayMode(srcColor);
			componentPtr[0] = TUint8(srcColor.Blue());
			componentPtr[1] = TUint8(srcColor.Green());
			componentPtr[2] = TUint8(srcColor.Red());
			}
		pixelPtr += pixelPtrInc;
		aRgbBuffer += 4;
		aMaskBuffer++;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineWords);

	TInt color = Color(aColor);

	while (pixelPtr < pixelRowPtrLimit)
		{
		MemFillTUint32(pixelPtr, aLength, color);
		pixelPtr += iScanLineWords;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* pixelPtrLimit = pixelPtr + aLength;
	TUint32* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineWords);
	TInt color = Color(aColor);

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			*tempPixelPtr ^= color;
			}

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* pixelPtrLimit = pixelPtr + aLength;
	TUint32* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineWords);
	TInt color = Color(aColor);

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			*tempPixelPtr &= color;
			}

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawThirtyTwoBppBitmapCommon::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* pixelPtrLimit = pixelPtr + aLength;
	TUint32* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineWords);
	TInt color = Color(aColor);

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			*tempPixelPtr |= color;
			}

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

inline TUint32 OptimizedBlend32(TInt aPrimaryRed,TInt aPrimaryGreen,TInt aPrimaryBlue,TUint32 aSecondary,TUint8 aAlphaValue)
	{
 	__ASSERT_DEBUG(!(aPrimaryRed>>8) && !(aPrimaryGreen>>8) && !(aPrimaryBlue>>8) && !(aAlphaValue>>8),
					Panic(EScreenDriverPanicAlphaBlendInvariant));

 	if(aAlphaValue == 0xff)
		{
		return (aPrimaryBlue + (aPrimaryGreen<<8) + (aPrimaryRed<<16)) | 0xff000000;
 		}
 	else
 		{
 		const TUint32 alphaValue = (aAlphaValue << 8) + aAlphaValue;

 		const TInt r2 = (aSecondary & 0x00ff0000) >> 16;
 		const TInt g2 = (aSecondary & 0x0000ff00) >> 8;
 		const TInt b2 = aSecondary & 0x000000ff;

 		const TInt r3 = ((alphaValue * (aPrimaryRed   - r2)) >> 16) + r2;
		const TInt g3 = ((alphaValue * (aPrimaryGreen - g2)) >> 16) + g2;
		const TInt b3 = ((alphaValue * (aPrimaryBlue  - b2)) >> 16) + b2;

		return (b3 & 0xFF) | ((g3<<8) & 0xFF00) | ((r3<<16) & 0xFF0000) | 0xFF000000;
 		}
 	}

void CDrawThirtyTwoBppBitmapCommon::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	const TUint32 sourceAlpha = aColor.Alpha();
	if (sourceAlpha==0 || aLength<=0)
		return;
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);

	const TUint32 sourceInternal=aColor.Internal();
	const TUint32 s_rb = sourceInternal & 0x00FF00FF;
	const TUint32 s_g = (sourceInternal & 0xFF00) >> 8;
	if (sourceAlpha==0xFF)
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			const TUint32 maskAlpha=*aMaskBuffer;
			if (maskAlpha)
				{
				if (maskAlpha==0xFF)
					*pixelPtr = sourceInternal;
				else
					BlendFromRBandG(pixelPtr,s_rb,s_g,maskAlpha,maskAlpha|(maskAlpha<<16));
				}
			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			}
		}
	else
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			const TUint32 maskAlpha=*aMaskBuffer;
			if (maskAlpha)
				{
				TUint blendAlpha = sourceAlpha;
				if (maskAlpha!=0xFF)
					blendAlpha=((maskAlpha+1) * sourceAlpha)>>8;
				BlendFromRBandG(pixelPtr,s_rb,s_g,blendAlpha,blendAlpha|(blendAlpha<<16));
				}
			pixelPtr += pixelPtrInc;
			aMaskBuffer++;
			}
		}
	}

void CDrawThirtyTwoBppBitmapCommon::MapColorToUserDisplayMode(TRgb& aColor)
	{
	const TInt alpha = aColor.Alpha();
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
	case EColor4K:
		aColor = TRgb::_Color4K(aColor._Color4K());
		break;
	case EColor64K:
		aColor = TRgb::_Color64K(aColor._Color64K());
		break;
	default:
		break;
		}
	aColor.SetAlpha(alpha);
	}

void CDrawThirtyTwoBppBitmapCommon::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* bufferLimit = aBuffer + aLength;
	const TUint16* nTable = PtrTo16BitNormalisationTable();
	TRgb color;

	switch (iUserDispMode)
		{
	case EGray2:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::_Gray2(color._Gray2());
			*aBuffer++ = color.Internal();
			}
		break;
	case EGray4:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::_Gray4(color._Gray4());
			*aBuffer++ = color.Internal();
			}
		break;
	case EGray16:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::_Gray16(color._Gray16());
			*aBuffer++ = color.Internal();
			}
		break;
	case EGray256:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::_Gray256(color._Gray256());
			*aBuffer++ = color.Internal();
			}
		break;
	case EColor16:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::Color16(color.Color16());
			*aBuffer++ = color.Internal();
			}
		break;
	case EColor256:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::Color256(color.Color256());
			*aBuffer++ = color.Internal();
			}
		break;
	case EColor4K:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::_Color4K(color._Color4K());
			*aBuffer++ = color.Internal();
			}
		break;
	case EColor64K:
		while (aBuffer < bufferLimit)
			{
			color.SetInternal(PMA2NonPMAPixel(*aBuffer, nTable));
			color = TRgb::_Color64K(color._Color64K());
			*aBuffer++ = color.Internal();
			}
		break;
	default:
		break;
		}
	}

/**
Implementation for CFbsDrawDevice::GetInterface().
Retrieves a pointer to a specified interface of CFbsDrawDevice implementation.
@param aInterfaceId Interface identifier of the interface to be retrieved.
@param aInterface Address of variable that retrieves the specified interface.
@return KErrNone If the interface is supported, KErrNotSupported otherwise.
*/
TInt CDrawThirtyTwoBppBitmapCommon::GetInterface(TInt aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt ret = KErrNotSupported;
	
	if (aInterfaceId == KFastBlit2InterfaceID)
		{
		aInterface = static_cast<MFastBlit2*>(this);
		ret = KErrNone;
		}
	else
		return CDrawBitmap::GetInterface(aInterfaceId, aInterface);
		
	return ret;
	}

/**
CDrawThirtyTwoBppBitmapCommon::WriteBitmapBlock() implementation.
@internalTechnology
@see MFastBlit2::WriteBitmapBlock()
*/
TInt CDrawThirtyTwoBppBitmapCommon::WriteBitmapBlock(const TPoint& aDest,
									CFbsDrawDevice* aSrcDrawDevice,
									const TRect& aSrcRect)
	{
	__ASSERT_DEBUG(aSrcDrawDevice && ((aSrcDrawDevice->DisplayMode()==EColor16MU) || (aSrcDrawDevice->DisplayMode()==EColor16MA) ||(aSrcDrawDevice->DisplayMode()==EColor16MAP)), Panic(EScreenDriverPanicInvalidParameter));
	
	TAny* interface=NULL;
	TInt ret = aSrcDrawDevice->GetInterface(KFastBlit2InterfaceID, interface);
	if (ret != KErrNone)
		{
		return KErrNotSupported;
		}

	TAny* interface1=NULL;
	ret = aSrcDrawDevice->GetInterface(KScalingSettingsInterfaceID, interface1);
	if(ret != KErrNone || (interface1 && !reinterpret_cast<MScalingSettings*>(interface1)->IsScalingOff()))
		{
		return KErrNotSupported;
		}

	ret = aSrcDrawDevice->GetInterface(KOrientationInterfaceID, interface1);
	if(ret != KErrNone || (interface1 && reinterpret_cast<MDrawDeviceOrientation*>(interface1)->Orientation() != 0))
		{
		return KErrNotSupported;
		}

	ret = aSrcDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, interface1);
	if(ret != KErrNone)
		{
		return KErrNotSupported;
		}
	
	if(interface1)
		{
	 	TPoint pt;
	 	reinterpret_cast<MDrawDeviceOrigin*>(interface1)->Get(pt);
	 	if(pt.iX != 0 || pt.iY != 0)
	 		{
			return KErrNotSupported;
	 		}
		}

	const TUint32* srcBase = reinterpret_cast<MFastBlit2*>(interface)->Bits();
	__ASSERT_DEBUG(srcBase!=NULL, Panic(EScreenDriverPanicInvalidParameter));
	TInt srcStride = aSrcDrawDevice->ScanLineBytes();  
	__ASSERT_DEBUG((srcStride&3)==0, Panic(EScreenDriverPanicInvalidParameter));  // stride is assumed to be a multiple of 4
	TSize srcSize = aSrcDrawDevice->SizeInPixels();

	return WriteBitmapBlock(aDest, srcBase, srcStride, srcSize, aSrcRect);
	}
									
/**
CDrawThirtyTwoBppBitmapCommon::WriteBitmapBlock() implementation.
@internalTechnology
@see MFastBlit2::WriteBitmapBlock()
*/													
TInt CDrawThirtyTwoBppBitmapCommon::WriteBitmapBlock(const TPoint& aDest,
									const TUint32* aSrcBase,
									TInt aSrcStride,
									const TSize& aSrcSize,
									const TRect& aSrcRect)
	{
	__ASSERT_DEBUG(aSrcBase, Panic(EScreenDriverPanicInvalidParameter));
	__ASSERT_DEBUG((aSrcStride&3)==0, Panic(EScreenDriverPanicInvalidParameter));
	__ASSERT_DEBUG(iBits, Panic(EScreenDriverPanicInvalidPointer));

	if (iShadowMode!=NULL ||
    	(iUserDispMode!=NULL && iUserDispMode!=iDispMode) ||
    	iOrientation!=EOrientationNormal ||
		!IsScalingOff() ||
		!iOriginIsZero)
		{
		return KErrNotSupported;
		}
	
	__ASSERT_DEBUG(aSrcRect.iTl.iX >= 0, Panic(EScreenDriverPanicOutOfBounds)); 
	__ASSERT_DEBUG(aSrcRect.iTl.iY >= 0, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aSrcRect.iBr.iX <= aSrcSize.iWidth,  Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aSrcRect.iBr.iY <= aSrcSize.iHeight, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aDest.iX >= 0, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aDest.iY >= 0, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG((aDest.iX + aSrcRect.Width())  <= SizeInPixels().iWidth,  Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG((aDest.iY + aSrcRect.Height()) <= SizeInPixels().iHeight, Panic(EScreenDriverPanicOutOfBounds));
	
	const TInt srcStrideWords=aSrcStride >> 2;
	const TInt dstStrideWords=iScanLineWords;
	
	if (aSrcSize.iWidth == aSrcRect.Width() &&
		aSrcSize.iWidth == SizeInPixels().iWidth &&
		srcStrideWords == dstStrideWords)
		{
		// Optimum case - one memcpy
		__ASSERT_DEBUG(aSrcRect.iTl.iX==0 && aDest.iX==0, Panic(EScreenDriverPanicInvalidParameter));  // this is implied by the above conditions
		const TUint32* srcPtr = aSrcBase + (iScanLineWords * aSrcRect.iTl.iY);
		TUint32* dstPtr       = iBits    + (iScanLineWords * aDest.iY);
		const TInt length = aSrcStride * aSrcRect.Height();
		Mem::Move(dstPtr, srcPtr, length);
		return KErrNone;
		}
		
	// Sub-optimal case - one memcpy per line
	const TUint32* srcPtr = aSrcBase + (srcStrideWords * aSrcRect.iTl.iY) + aSrcRect.iTl.iX;
	TUint32* dstPtr       = iBits    + (dstStrideWords * aDest.iY		) + aDest.iX;
	const TInt length = aSrcRect.Width() << 2;
	TInt lines = aSrcRect.Height();
	while (lines--)
		{
		Mem::Move(dstPtr, srcPtr, length);
		srcPtr+=srcStrideWords;
		dstPtr+=dstStrideWords;
		}
	return KErrNone;
	}

/**
CDrawThirtyTwoBppBitmapCommon::Bits() implementation.
@internalTechnology
@see MFastBlit2::Bits()
*/
const TUint32* CDrawThirtyTwoBppBitmapCommon::Bits() const
	{
	return iBits;
	}
