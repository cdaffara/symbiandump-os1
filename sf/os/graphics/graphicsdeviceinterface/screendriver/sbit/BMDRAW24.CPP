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

//Initializes iSize, iDrawRect, iLongWidth, iScanLineBytes, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawTwentyFourBppBitmap::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iScanLineBytes = (((iSize.iWidth * 3) + 11) / 12) * 12;
	iLongWidth = iScanLineBytes / 3;
	iScanLineWords = iScanLineBytes / 4;
	}
 
TInt CDrawTwentyFourBppBitmap::Construct(TSize aSize)
	{
	return Construct(aSize, (((aSize.iWidth * 3) + 11) / 12) * 12);
	}

TInt CDrawTwentyFourBppBitmap::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	iDispMode = EColor16M;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride % 12)
		return KErrArgument;
	iScanLineBytes = aStride;
	iLongWidth = aStride / 3;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = aStride >> 2;
	TInt size = (((Max(aSize.iWidth,aSize.iHeight) * 3) + 11) / 12) * 12;
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if (iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

inline TInt CDrawTwentyFourBppBitmap::PixelAddressIncrement() const
	{
	switch (iOrientation)
		{
		case EOrientationNormal:
			return 3;
		case EOrientationRotated90:
			return iScanLineBytes;
		case EOrientationRotated180:
			return -3;
		case EOrientationRotated270:
			return -iScanLineBytes;
		default:
			return 1;
		}
	}

inline void  CDrawTwentyFourBppBitmap::PixelAddressIncrement(TInt& aPixelInc,TInt& aRowInc) const
	{
	switch (iOrientation)
		{
		case EOrientationNormal:
			aPixelInc = 3;
			aRowInc = iScanLineBytes;
			break;
		case EOrientationRotated90:
			aPixelInc = iScanLineBytes;
			aRowInc = -3;
			break;
		case EOrientationRotated180:
			aPixelInc = -3;
			aRowInc = -iScanLineBytes;
			break;
		case EOrientationRotated270:
			aPixelInc = -iScanLineBytes;
			aRowInc = 3;
			break;
		default:
			aPixelInc = 1;
			aRowInc = 1;
		}
	}

void CDrawTwentyFourBppBitmap::FadeRgb(TInt& red,TInt& green,TInt& blue)
	{
  	blue = ((blue * iFadeMapFactor) >> 8)  + iFadeMapOffset;
  	green = ((green * iFadeMapFactor) >> 16) + iFadeMapOffset;
  	red = ((red * iFadeMapFactor) >> 8) + iFadeMapOffset; 
	}

void CDrawTwentyFourBppBitmap::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = CDrawBitmap::FadeRgb(aColor);

	if (iShadowMode & EShadow)
		{
		TInt r = ShadowComponentInl(aColor.Red());
		TInt g = ShadowComponentInl(aColor.Green());
		TInt b = ShadowComponentInl(aColor.Blue());
		aColor = TRgb(r,g,b);
		}
	}

void CDrawTwentyFourBppBitmap::Shadow(TInt& red,TInt& green,TInt& blue)
  	{
  	if (iShadowMode & EFade)
  		FadeRgb(red,green,blue);
   
  	if (iShadowMode & EShadow)
		{
  		red = ShadowComponentInl(red);
  		green = ShadowComponentInl(green);
  		blue = ShadowComponentInl(blue);
  		}
	}

TUint8 CDrawTwentyFourBppBitmap::ShadowAndFade(TInt aComponent)
	{
	if (iShadowMode & EFade)
		aComponent = FadeGray(aComponent);

	if (iShadowMode & EShadow)
		aComponent = ShadowComponentInl(aComponent);

	return TUint8(aComponent);
	}

TUint8 CDrawTwentyFourBppBitmap::ShadowComponentInl(TInt aRgbComponent)
	{
	return TUint8(Max(0,aRgbComponent-0x40));
	}

TUint8 CDrawTwentyFourBppBitmap::ShadowComponent(TInt aRgbComponent)
	{
	return ShadowComponentInl(aRgbComponent);
	}

void CDrawTwentyFourBppBitmap::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	TUint8* buffer = (TUint8*)aBuffer;
	TUint8* limit = buffer + (aLength * 3);

	while (buffer < limit)
		*buffer++ ^= 0xff;
	}

void CDrawTwentyFourBppBitmap::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
	{
	const TUint8* pixelPtr = PixelAddress(aX,aY);

	if (iOrientation == EOrientationNormal)
		Mem::Copy(aBuffer,pixelPtr,aLength * 3);
	else
		{
		const TInt pixelPtrInc = PixelAddressIncrement();

		TUint8* bufferPtr = STATIC_CAST(TUint8*,aBuffer);
		const TUint8* bufferPtrLimit = bufferPtr + (aLength * 3);

		while (bufferPtr < bufferPtrLimit)
			{
			*bufferPtr++ = pixelPtr[0];
			*bufferPtr++ = pixelPtr[1];
			*bufferPtr++ = pixelPtr[2];
			pixelPtr += pixelPtrInc;
			}
		}
	}

TRgb CDrawTwentyFourBppBitmap::ReadRgbNormal(TInt aX,TInt aY) const
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	return TRgb(pixelPtr[2],pixelPtr[1],pixelPtr[0]);
	}

void CDrawTwentyFourBppBitmap::ShadowArea(const TRect& aRect)
	{
	const TRect rect(DeOrientate(aRect));

	__ASSERT_DEBUG(rect.iTl.iX>=0 && rect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(rect.iTl.iY>=0 && rect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	TUint8* pixelPtr = PixelAddress(rect.iTl.iX,rect.iTl.iY);
	TUint8* pixelRowPtrLimit = pixelPtr + (rect.Height() * iScanLineBytes);

	if (iShadowMode & EFade)
		{
		TUint8* pixelRowPtr = pixelPtr;
		TUint8* pixelPtrLimit = pixelPtr + (rect.Width() * 3);

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint8* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				*tempPixelPtr = FadeGray(*tempPixelPtr);
				++tempPixelPtr;
				}

			pixelRowPtr += iScanLineBytes;
			pixelPtrLimit += iScanLineBytes;
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint8* pixelRowPtr = pixelPtr;
		TUint8* pixelPtrLimit = pixelPtr + (rect.Width() * 3);

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			TUint8* tempPixelPtr = pixelRowPtr;

			while (tempPixelPtr < pixelPtrLimit)
				{
				*tempPixelPtr = ShadowComponent(*tempPixelPtr);
				++tempPixelPtr;
				}

			pixelRowPtr += iScanLineBytes;
			pixelPtrLimit += iScanLineBytes;
			}
		}
	}

void CDrawTwentyFourBppBitmap::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	TUint8* limit = ((TUint8*)aBuffer) + (aLength * 3);

	if (iShadowMode & EFade)
		{
		TUint8* buffer = (TUint8*)aBuffer;

		while (buffer < limit)
			{
			*buffer = FadeGray(*buffer);
			++buffer;
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint8* buffer = (TUint8*)aBuffer;

		while (buffer < limit)
			{
			*buffer = ShadowComponent(*buffer);
			++buffer;
			}
		}
	}

void CDrawTwentyFourBppBitmap::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	pixelPtr[0] = TUint8(aColor.Blue());
	pixelPtr[1] = TUint8(aColor.Green());
	pixelPtr[2] = TUint8(aColor.Red());
	}

void CDrawTwentyFourBppBitmap::WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor)
	{
	DeOrientate(aX,aY);

	TInt pixelInc;
	TInt rowInc;

	PixelAddressIncrement(pixelInc,rowInc);

	const TUint32* dataLimit = aBuffer + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;

	TUint8* pixelPtr = PixelAddress(aX,aY);

	const TUint8 r = (TUint8)aColor.Red();
	const TUint8 g = (TUint8)aColor.Green();
	const TUint8 b = (TUint8)aColor.Blue();

	while (aBuffer < dataLimit)
		{
		TUint32 dataWord = *aBuffer++;
		TUint32 dataMask = 1;
		TUint8* tempPixelPtr = pixelPtr;

		while (dataMask != dataMaskLimit)
			{
			if(dataWord & dataMask)
				{
				tempPixelPtr[0] = b;
				tempPixelPtr[1] = g;
				tempPixelPtr[2] = r;
				}

			tempPixelPtr += pixelInc;
			dataMask <<= 1;
			}

		pixelPtr += rowInc;
		}
	}

void CDrawTwentyFourBppBitmap::WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	if (aLength <= 0)
		return;

	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);

	const TUint32* dataPtrLimit = aBuffer + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;

	TInt pixelInc;
	TInt rowInc;

	PixelAddressIncrement(pixelInc,rowInc);

	const TUint8 r = (TUint8)aColor.Red();
	const TUint8 g = (TUint8)aColor.Green();
	const TUint8 b = (TUint8)aColor.Blue();

	if (r || g || b)
		{
		while (aBuffer < dataPtrLimit)
			{
			TUint32 dataWord = *aBuffer++;
			TUint32 dataMask = 1;
			TUint8* tempPixelPtr = pixelPtr;

			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					switch (aDrawMode)
						{
						case CGraphicsContext::EDrawModeXOR:
							tempPixelPtr[0] ^= b;
							tempPixelPtr[1] ^= g;
							tempPixelPtr[2] ^= r;
							break;
						case CGraphicsContext::EDrawModeAND:
							tempPixelPtr[0] &= b;
							tempPixelPtr[1] &= g;
							tempPixelPtr[2] &= r;
							break;
						case CGraphicsContext::EDrawModeOR:
							tempPixelPtr[0] |= b;
							tempPixelPtr[1] |= g;
							tempPixelPtr[2] |= r;
							break;
						default:
							break;
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
			TUint8* tempPixelPtr = pixelPtr;

			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					tempPixelPtr[0] = 0;
					tempPixelPtr[1] = 0;
					tempPixelPtr[2] = 0;
					}

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}

			pixelPtr += rowInc;
			}
		}
	}

void CDrawTwentyFourBppBitmap::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aHeight,TRgb aColor,TBool aUp)
	{
	DeOrientate(aX,aY);

	TInt scanlineByteLength;

	switch (iOrientation)
		{
		case EOrientationNormal:
			scanlineByteLength = iScanLineBytes;
			break;
		case EOrientationRotated90:
			scanlineByteLength = -3;
			break;
		case EOrientationRotated180:
			scanlineByteLength = -iScanLineBytes;
			break;
		default: // EOrientationRotated270
			scanlineByteLength = 3;
			break;
		}

	if (aUp)
		scanlineByteLength = -scanlineByteLength;

	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TUint8* pixelPtrLimit = pixelPtr + (aHeight * scanlineByteLength);
	TUint32 dataWord = *aBuffer;
	TUint32 dataMask = 1;

	const TUint8 r = (TUint8)aColor.Red();
	const TUint8 g = (TUint8)aColor.Green();
	const TUint8 b = (TUint8)aColor.Blue();

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
			pixelPtr[0] = b;
			pixelPtr[1] = g;
			pixelPtr[2] = r;
			}

		dataMask <<= 1;
		pixelPtr += scanlineByteLength;
		}
	}

void CDrawTwentyFourBppBitmap::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);

	if (iOrientation == EOrientationNormal)
		Mem::Copy(pixelPtr,aBuffer,aLength * 3);
	else
		{
		const TInt pixelPtrInc = PixelAddressIncrement();

		TUint8* bufferPtr = REINTERPRET_CAST(TUint8*,aBuffer);
		TUint8* bufferPtrLimit = bufferPtr + (aLength * 3);

		while (bufferPtr < bufferPtrLimit)
			{
			pixelPtr[0] = *bufferPtr++;
			pixelPtr[1] = *bufferPtr++;
			pixelPtr[2] = *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	}

void CDrawTwentyFourBppBitmap::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();

	TUint8* bufferPtr = REINTERPRET_CAST(TUint8*,aBuffer);
	TUint8* bufferPtrLimit = bufferPtr + (aLength * 3);

	while (bufferPtr < bufferPtrLimit)
		{
		pixelPtr[0] ^= *bufferPtr++;
		pixelPtr[1] ^= *bufferPtr++;
		pixelPtr[2] ^= *bufferPtr++;
		pixelPtr += pixelPtrInc;
		}
	}

void CDrawTwentyFourBppBitmap::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();

	TUint8* bufferPtr = REINTERPRET_CAST(TUint8*,aBuffer);
	TUint8* bufferPtrLimit = bufferPtr + (aLength * 3);

	while (bufferPtr < bufferPtrLimit)
		{
		pixelPtr[0] &= *bufferPtr++;
		pixelPtr[1] &= *bufferPtr++;
		pixelPtr[2] &= *bufferPtr++;
		pixelPtr += pixelPtrInc;
		}
	}

void CDrawTwentyFourBppBitmap::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();

	TUint8* bufferPtr = REINTERPRET_CAST(TUint8*,aBuffer);
	TUint8* bufferPtrLimit = bufferPtr + (aLength * 3);

	while (bufferPtr < bufferPtrLimit)
		{
		pixelPtr[0] |= *bufferPtr++;
		pixelPtr[1] |= *bufferPtr++;
		pixelPtr[2] |= *bufferPtr++;
		pixelPtr += pixelPtrInc;
		}
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawTwentyFourBppBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                 const TUint8* aRgbBuffer,
                                                 const TUint8* aMaskBuffer,
                                                 MAlphaBlend::TShadowing aShadowing,
                                                 CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	while (aMaskBuffer < maskBufferPtrLimit)
		{
  		TInt mask = aMaskBuffer[0];
          
  		if(mask)
  			{
  			TInt red = aRgbBuffer[2];
          	TInt green = aRgbBuffer[1];
          	TInt blue = aRgbBuffer[0];
          
  			if(aShadowing == MAlphaBlend::EShdwBefore)
  				{
  				Shadow(red,green,blue);
  				}
  		
  			if(aMaskBuffer[0] != 0xff) // Blend 24bpp
  				{
  				mask = mask * 257;
  				red = (((red   - pixelPtr[2]) * mask) >> 16) + pixelPtr[2];
  				green = (((green - pixelPtr[1]) * mask) >> 16) + pixelPtr[1];
  				blue = (((blue  - pixelPtr[0]) * mask) >> 16) + pixelPtr[0];
  				}
  
  			if(aShadowing == MAlphaBlend::EShdwAfter)
  				{
  				Shadow(red,green,blue);
  				}
  			CDrawBitmap::MapColorToUserDisplayMode(red,green,blue);
  			pixelPtr[0] = TUint8(blue); 
  			pixelPtr[1] = TUint8(green); 
  			pixelPtr[2] = TUint8(red);
  			}
  		pixelPtr += pixelPtrInc;
  		aRgbBuffer += 4;
  		aMaskBuffer++;
		}
    }

const TUint32 KWordAlignedCount = 4;
const TUint32 KWordAlignedMask = 3;
const TUint32 KFinishEarlyByThree = 3;

void WriteTwentyFourBppColourAsWords(TUint8& r, TUint8& g, TUint8& b, TUint8* pixelPtr, const TInt byteLength, TUint8* pixelRowPtrLimit, const TInt scanLineBytes)
	{
	TUint8 bgr[3] = 
		{
		b,g,r
		};
	TUint32 bgrb = b+(g<<8)+(r<<16)+(b<<24);
	TUint32 grbg = g+(r<<8)+(b<<16)+(g<<24);
	TUint32 rbgr = r+(b<<8)+(g<<16)+(r<<24);

	TUint8* pixelPtrLimit = pixelPtr + byteLength;

	TInt leadingPixels = KWordAlignedCount-((TUint32)pixelPtr)&KWordAlignedMask;
	if (leadingPixels == KWordAlignedCount)
		leadingPixels = 0;
	const TInt trailingPixels = ((TUint32)pixelPtrLimit & KWordAlignedMask);

	while (pixelPtr < pixelRowPtrLimit)
		{
		TUint8* tempPixelPtr;
		TUint32* tempWordPtr;
		TInt nextOfBgr = 0;
		TUint8* leadingPixelPtrLimit = pixelPtr+leadingPixels;
		for (tempPixelPtr = pixelPtr; tempPixelPtr < leadingPixelPtrLimit; tempPixelPtr++)
			{
			tempPixelPtr[0] = bgr[nextOfBgr++];
			}

		if (nextOfBgr == (KWordAlignedCount-1))
			nextOfBgr = 0;

		TUint32* wordPtrLimit = ((TUint32*)(pixelPtrLimit-trailingPixels))-KFinishEarlyByThree;

		tempWordPtr = (TUint32*)tempPixelPtr;
		switch (nextOfBgr)
			{
		case 1:
			if (tempWordPtr < wordPtrLimit)
				{
				*tempWordPtr++ = grbg;
				nextOfBgr = 2;
				}
			//no break
		case 2:
			if (tempWordPtr < wordPtrLimit)
				{
				*tempWordPtr++ = rbgr;
				nextOfBgr = 0;
				}
			}

		while (tempWordPtr < wordPtrLimit)
			{
			*tempWordPtr++ = bgrb;
			*tempWordPtr++ = grbg;
			*tempWordPtr++ = rbgr;
			}

		for (tempPixelPtr = (TUint8*)tempWordPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			tempPixelPtr[0] = bgr[nextOfBgr++];
			if (nextOfBgr > 2)
				nextOfBgr = 0;
			}

		pixelPtr += scanLineBytes;
		pixelPtrLimit += scanLineBytes;
		}
	}

/**
Writes a specific colour to the screen, optimised to use mem fill if the colour is shade of grey
and word aligned access the three possible combinations of the colour bytes.
*/
void CDrawTwentyFourBppBitmap::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt scanLineBytes = iScanLineBytes;
	const TInt byteLength = aLength * 3;

	TUint8* pixelPtr = PixelAddress(aX,aY);
	TUint8* pixelRowPtrLimit = pixelPtr + (aHeight * scanLineBytes);

	TUint8 r = TUint8(aColor.Red());
	TUint8 g = TUint8(aColor.Green());
	TUint8 b = TUint8(aColor.Blue());

	if ((r == g) && (g == b))
		{
		while (pixelPtr < pixelRowPtrLimit)
			{
			Mem::Fill(pixelPtr,byteLength,r);
			pixelPtr += scanLineBytes;
			}
		}
	else
		{
		WriteTwentyFourBppColourAsWords(r, g, b, pixelPtr, byteLength, pixelRowPtrLimit, scanLineBytes);
		}
	}

void CDrawTwentyFourBppBitmap::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	TUint8* pixelPtrLimit = pixelPtr + (aLength * 3);
	TUint8* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineBytes);

	TUint8 r = TUint8(aColor.Red());
	TUint8 g = TUint8(aColor.Green());
	TUint8 b = TUint8(aColor.Blue());

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint8* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr += 3)
			{
			tempPixelPtr[0] ^= b;
			tempPixelPtr[1] ^= g;
			tempPixelPtr[2] ^= r;
			}

		pixelPtr += iScanLineBytes;
		pixelPtrLimit += iScanLineBytes;
		}
	}

void CDrawTwentyFourBppBitmap::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	TUint8* pixelPtrLimit = pixelPtr + (aLength * 3);
	TUint8* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineBytes);
	TUint8 r = TUint8(aColor.Red());
	TUint8 g = TUint8(aColor.Green());
	TUint8 b = TUint8(aColor.Blue());

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint8* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr += 3)
			{
			tempPixelPtr[0] &= b;
			tempPixelPtr[1] &= g;
			tempPixelPtr[2] &= r;
			}

		pixelPtr += iScanLineBytes;
		pixelPtrLimit += iScanLineBytes;
		}
	}

void CDrawTwentyFourBppBitmap::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	TUint8* pixelPtrLimit = pixelPtr + (aLength * 3);
	TUint8* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineBytes);
	TUint8 r = TUint8(aColor.Red());
	TUint8 g = TUint8(aColor.Green());
	TUint8 b = TUint8(aColor.Blue());

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint8* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr += 3)
			{
			tempPixelPtr[0] |= b;
			tempPixelPtr[1] |= g;
			tempPixelPtr[2] |= r;
			}

		pixelPtr += iScanLineBytes;
		pixelPtrLimit += iScanLineBytes;
		}
	}

void CDrawTwentyFourBppBitmap::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);

	const TInt red = aColor.Red();
	const TInt green = aColor.Green();
	const TInt blue = aColor.Blue();
	while (aMaskBuffer < maskBufferPtrLimit)
		{
		if(aMaskBuffer[0])
			{
			TRgb pixelClr;
	
			if(aMaskBuffer[0] != 0xff)
				{
				pixelClr = AlphaBlend(red, green, blue, TRgb(pixelPtr[2], pixelPtr[1], pixelPtr[0]), aMaskBuffer[0]);
				}
			else
				{
				pixelClr = aColor;	
				}
			
			pixelPtr[0] = TUint8(pixelClr.Blue());
			pixelPtr[1] = TUint8(pixelClr.Green());
			pixelPtr[2] = TUint8(pixelClr.Red());
			}
		pixelPtr += pixelPtrInc;
		aMaskBuffer++;
		}
	}

void CDrawTwentyFourBppBitmap::MapColorToUserDisplayMode(TRgb& aColor)
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
	case EColor4K:
		aColor = TRgb::_Color4K(aColor._Color4K());
		break;
	case EColor64K:
		aColor = TRgb::_Color64K(aColor._Color64K());
		break;
	default:
		break;
		}
	}

void CDrawTwentyFourBppBitmap::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint8* bufferPtr = (TUint8*)aBuffer;
	const TUint8* bufferLimit = bufferPtr + (aLength * 3);

	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::_Gray2(color._Gray2());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EGray4:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::_Gray4(color._Gray4());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EGray16:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::_Gray16(color._Gray16());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EGray256:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::_Gray256(color._Gray256());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EColor16:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::Color16(color.Color16());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EColor256:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::Color256(color.Color256());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EColor4K:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::_Color4K(color._Color4K());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	case EColor64K:
		while (bufferPtr < bufferLimit)
			{
			TInt blue = bufferPtr[0];
			TInt green = bufferPtr[1];
			TInt red = bufferPtr[2];
			TRgb color(red,green,blue);
			color = TRgb::_Color64K(color._Color64K());
			bufferPtr[0] = TUint8(color.Blue());
			bufferPtr[1] = TUint8(color.Green());
			bufferPtr[2] = TUint8(color.Red());
			bufferPtr += 3;
			}
		break;
	default:
		break;
		}
	}

TInt CDrawTwentyFourBppBitmap::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
														TUint32 aOutlinePenColor, TUint32 aShadowColor,
														TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
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
	const TInt alpha = aOutlinePenColor >> 24;

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
			TRgb backgroundColor = TRgb::_Color16M(TRgb(pixelPtr[2], pixelPtr[1], pixelPtr[0])._Color16M());

			blendedRedColor = (redOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
						   		redShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
						  		redFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
						  		backgroundColor.Red() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			blendedGreenColor = (greenOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								greenShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
								greenFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
								backgroundColor.Green() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			blendedBlueColor = 	(blueOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
								blueShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
								blueFillColor * FourColorBlendLookup[index][KFillColorIndex] + 
								backgroundColor.Blue() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;

			if (alpha != 0xff)
				{
				TRgb alphablend = AlphaBlend(blendedRedColor, blendedGreenColor, blendedBlueColor, TRgb(pixelPtr[2], pixelPtr[1], pixelPtr[0]), alpha);
				pixelPtr[0] = TUint8(alphablend.Blue());
				pixelPtr[1] = TUint8(alphablend.Green());
				pixelPtr[2] = TUint8(alphablend.Red());
				}
			else
				{
				//opaque
				pixelPtr[0] = TUint8(blendedBlueColor);
				pixelPtr[1] = TUint8(blendedGreenColor);
				pixelPtr[2] = TUint8(blendedRedColor);
				}
			pixelPtr += pixelPtrInc;
			continue;
			}

		if (alpha != 0xff)
			{
			TRgb alphablend = AlphaBlend(finalColor, TRgb(pixelPtr[2], pixelPtr[1], pixelPtr[0]), alpha);
			pixelPtr[0] = TUint8(alphablend.Blue());
			pixelPtr[1] = TUint8(alphablend.Green());
			pixelPtr[2] = TUint8(alphablend.Red());
			}
		else
			{
			pixelPtr[0] = TUint8(finalColor.Blue());
			pixelPtr[1] = TUint8(finalColor.Green());
			pixelPtr[2] = TUint8(finalColor.Red());
			}
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}

void CDrawTwentyFourBppBitmap::BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	
	if (sourceAlpha == 0xFF) //Fully opaque
		{
		WriteRgbMulti(aX,aY,aLength,aHeight,aColor);
		return;
		}
	else if (sourceAlpha == 0x00) //Fully transparent
		{
		return;
		}
	else //Perform alpha blending
		{
		TUint8* pixelPtr = PixelAddress(aX,aY);
		const TInt pixelPtrInc = PixelAddressIncrement();
		TUint8* pixelPtrEnd = pixelPtr + (aLength * pixelPtrInc);
		
		TUint8 dr;
		TUint8 dg;
		TUint8 db;
		
		//Perform pre-multiplication on values from aColor
		const TUint8 pmsr = (sourceAlpha * aColor.Red()) / 255;
		const TUint8 pmsg = (sourceAlpha * aColor.Green()) / 255;
		const TUint8 pmsb = (sourceAlpha * aColor.Blue()) / 255;
		
		for (TInt ii = 0 ; ii <= aHeight; ii++)
			{
			while (pixelPtr != pixelPtrEnd)
				{
				dr = pixelPtr[2];
				dg = pixelPtr[1];
				db = pixelPtr[0];
				
				//Target has no alpha channel so assume to be 0xFF (opaque)
				pixelPtr[0] = pmsb + ((0xFF-sourceAlpha) * db)/255;
				pixelPtr[1] = pmsg + ((0xFF-sourceAlpha) * dg)/255;
				pixelPtr[2] = pmsr + ((0xFF-sourceAlpha) * dr)/255;
				
				pixelPtr+=pixelPtrInc;
				}	
			pixelPtr = PixelAddress(aX, ii+aY);
			pixelPtrEnd = pixelPtr + (aLength * pixelPtrInc);
			}
		}
	}
