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

const TInt KPixelsPerByte = 8;
const TInt KPixelsPerWord = 32;

//Initializes iSize, iDrawRect, iLongWidth, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawOneBppBitmap::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = (iSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1);
	iScanLineWords = iLongWidth / KPixelsPerWord;
	}
 
TInt CDrawOneBppBitmap::Construct(TSize aSize)
	{
	return Construct(aSize, ((aSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1)) / KPixelsPerByte);
	}

TInt CDrawOneBppBitmap::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	iDispMode = EGray2;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride & 3)
		return KErrArgument;
	iLongWidth = aStride * KPixelsPerByte;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = aStride >> 2;
	TInt size = 1 + (Max(aSize.iWidth,aSize.iHeight) >> 3);
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if (iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

void CDrawOneBppBitmap::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = FadeRgb(TRgb::_Gray2(aColor._Gray2()));

	if (iShadowMode & EShadow)
		aColor = KRgbBlack;
	}

void CDrawOneBppBitmap::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength > 0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	const TUint32* const limit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	while (aBuffer < limit)
		*aBuffer++ ^= 0xffffffff;
	}

/**	Copies a number of pixels into a word-aligned buffer without format translation.
	Note that the byte length to the target buffer is honoured, 
 	but the end contents of the last byte are generally overwritten with extra pixel data (or garbage)  
 	Note that I am assuming the compiler optimiser will convert all these divides and multiplies into shifts!  
@param	aX		x coordinate to start copy from (need not be aligned at all)
@param	aY		y coordinate to copy line from	
@param	aLength	number of pixels to copy  
@param	aBuffer	target word-aligned buffer (but may or may not be word length) 
 **/
void CDrawOneBppBitmap::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
	{
	TUint32* pixelPtr = ScanLine(aY);
	TInt startLongPix = aX & -KPixelsPerWord;
	pixelPtr += startLongPix / KPixelsPerWord;
	TUint32* bufferPtr = (TUint32*)aBuffer;
	TInt wordsCnt = (aLength+KPixelsPerByte-1) / KPixelsPerWord;		//how many words to write to target
	TInt restPixels = aLength - wordsCnt * KPixelsPerWord;				//how many pixels left to copy
	TInt bytesCnt = (restPixels+KPixelsPerByte-1) / KPixelsPerByte ;	//how many target bytes to copy
	TInt shiftBits = aX - startLongPix;
	restPixels=shiftBits+restPixels;	//How many pixels are read from the second word by the final word copy
	if (bytesCnt==0 && shiftBits && restPixels<=0)
		{
		// This correction is required because although a last whole word will be written to the target buffer,
		// this special test indicates that the required significant data bits plus the shift 
		// add up to one word (or less) to be read. 
		// The copy words optimisation used to copy the main body of the line 
		// will read from the next location after the copy, 
		// but this may not always be accessable memory (on the last scanline)
		// The correction is not required if the second word would need to be read anyway.
		//eg we want to copy 7 nibbles with a 1 nibble shift (16 color), restpixels would be 0
		bytesCnt=4;
		wordsCnt--;
		}
	//How many pixels are read from the second word in the final byte copy?
	//If zero (or less) then the second word should not be read in the copy bytes phase
	//really this should be an else of the if above, but this gives the same end condition.
	//eg we want to copy 5 nibbles with a 2 nibble shift (16 color), restpixels would be -1.
	restPixels-=KPixelsPerWord;	
	ReadLineCommon(pixelPtr,bufferPtr,wordsCnt,restPixels,bytesCnt,shiftBits);
	}


TRgb CDrawOneBppBitmap::ReadRgbNormal(TInt aX,TInt aY) const
	{
	TUint32 colorWord = *(ScanLine(aY) + (aX / KPixelsPerWord));

	if (colorWord & (1 << (aX & 0x1f)))
		return KRgbWhite;

	return KRgbBlack;
	}

void CDrawOneBppBitmap::ShadowArea(const TRect& aRect)
	{
	__ASSERT_DEBUG(aRect.iTl.iX>=0 && aRect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aRect.iTl.iY>=0 && aRect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	if (iShadowMode & EFade)
		{
		TInt fadedWhite = FadeRgb(KRgbWhite)._Gray2();
		TInt fadedBlack = FadeRgb(KRgbBlack)._Gray2();

		if (fadedWhite)
			{
			if (fadedBlack) // Everything fades to white
				WriteRgbMulti(aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height(),KRgbWhite);
			// else Nothing changes
			}
		else
			{
			if (fadedBlack) // Everything inverted
				WriteRgbMultiXOR(aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height(),KRgbWhite);
			else // Everything fades to black
				WriteRgbMulti(aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height(),KRgbBlack);
			}
		}

	if (iShadowMode & EShadow)
		{
		const TInt x = aRect.iTl.iX;
		TInt y = aRect.iTl.iY;
		const TInt startLong = (x + KPixelsPerWord - 1) & (~0x1f);
		const TInt finishLong = (aRect.iBr.iX) & (~0x1f);
		const TInt startShift = startLong - x;
		TUint32* base = ScanLine(y);
		TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
		TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

		if (finishLong < startLong)
			{
			TUint32 mask = (0xffffffff >> (startLong - aRect.iBr.iX)) & (0xffffffff << (x - finishLong));
			mask = ~mask;

			for (; y < aRect.iBr.iY; y++)
				{
				pixelPtrLimit[0] &= mask;
				pixelPtrLimit += iScanLineWords;
				}

			return;
			}

		const TInt bytesToFill = (pixelPtrLimit - pixelPtr) * sizeof(TUint32);
		const TInt finishShift = 32 - aRect.iBr.iX + finishLong;

		for (;y<aRect.iBr.iY;y++)
			{
			if (x < startLong)
				pixelPtr[-1] = PasteInt(pixelPtr[-1],0,startShift);

			Mem::FillZ(pixelPtr,bytesToFill);

			if (finishLong < aRect.iBr.iX)
				pixelPtrLimit[0] = PasteInt(0,pixelPtrLimit[0],finishShift);

			pixelPtr += iScanLineWords;
			pixelPtrLimit += iScanLineWords;
			}
		}
	}

void CDrawOneBppBitmap::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	const TInt byteLength = (aLength + 7) / 8;

	if (iShadowMode & EFade)
		{
		TInt fadedWhite = FadeRgb(KRgbWhite)._Gray2();
		TInt fadedBlack = FadeRgb(KRgbBlack)._Gray2();

		if (fadedWhite)
			{
			if (fadedBlack) // Everything fades to white
				Mem::Fill(aBuffer,byteLength,0xff);
			// else Nothing changes
			}
		else
			{
			if (fadedBlack) // Everything inverted
				{
				TUint8* bufferPtr = REINTERPRET_CAST(TUint8*,aBuffer);
				const TUint8* bufferPtrLimit = bufferPtr + byteLength;

				while (bufferPtr < bufferPtrLimit)
					*bufferPtr ^= 0xff;
				}
			else // Everything fades to black
				Mem::FillZ(aBuffer,byteLength);
			}
		}

	if (iShadowMode & EShadow)
		Mem::FillZ(aBuffer,byteLength);
	}

void CDrawOneBppBitmap::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TUint32 mask = 1 << (aX & 0x1f);

	if (aColor._Gray2())
		pixelPtr[0] |= mask;
	else
		pixelPtr[0] &= ~mask;
	}

void CDrawOneBppBitmap::WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TBool white = (aColor._Gray2() == 1);
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TUint32* pixelPtrLimit = pixelPtr + (aHeight * iScanLineWords);

	while (pixelPtr < pixelPtrLimit)
		{
		TUint32 dataMask = 1;
		TUint32 mask = 1 << (aX & 0x1f);
		TUint32* tempPixelPtr = pixelPtr;

		for (TInt count = 0; count < aLength; count++,dataMask <<= 1,mask <<= 1)
			{
			if (!mask)
				{
				mask = 1;
				tempPixelPtr++;
				}

			if (aBuffer[0] & dataMask)
				{
				if (white)
					tempPixelPtr[0] |= mask;
				else
					tempPixelPtr[0] &= ~mask;
				}
			}

		aBuffer++;
		pixelPtr += iScanLineWords;
		}
	}

void CDrawOneBppBitmap::WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	const TBool white = (aColor._Gray2() == 1);
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TUint32* pixelPtrLimit = pixelPtr + (aHeight * iScanLineWords);
	TUint32 initialMask = 1 << (aX & 0x1f);

	if (white)
		{
		if (aDrawMode == CGraphicsContext::EDrawModeXOR || aDrawMode == CGraphicsContext::EDrawModeOR)
			{
			while (pixelPtr < pixelPtrLimit)
				{
				TUint32 dataMask = 1;
				TUint32 mask = initialMask;
				TUint32* tempPixelPtr = pixelPtr;

				for (TInt count = 0; count < aLength;count++,dataMask <<= 1,mask <<= 1)
					{
					if (!mask)
						{
						mask = 1;
						tempPixelPtr++;
						}

					if (aBuffer[0] & dataMask)
						{
						if (aDrawMode == CGraphicsContext::EDrawModeXOR)
							tempPixelPtr[0] ^= mask;
						else if (aDrawMode == CGraphicsContext::EDrawModeOR)
							tempPixelPtr[0] |= mask;
						}
					}

				aBuffer++;
				pixelPtr += iScanLineWords;
				}
			}
		}
	else
		{
		if (aDrawMode == CGraphicsContext::EDrawModeAND)
			{
			while (pixelPtr < pixelPtrLimit)
				{
				TUint32 dataMask = 1;
				TUint32 mask = initialMask;
				TUint32* tempPixelPtr = pixelPtr;

				for (TInt count = 0; count < aLength;count++,dataMask <<= 1,mask <<= 1)
					{
					if (!mask)
						{
						mask = 1;
						tempPixelPtr++;
						}

					if (*aBuffer & dataMask)
						*tempPixelPtr &= ~mask;
					}

				aBuffer++;
				pixelPtr += iScanLineWords;
				}
			}
		}
	}

void CDrawOneBppBitmap::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aHeight,TRgb aColor,TBool aUp)
	{
	const TBool white = (aColor._Gray2() == 1);
	const TInt yLimit = aY + ((aUp) ? -aHeight : aHeight);
	const TInt scanLineWords = (aUp) ? -iScanLineWords : iScanLineWords;
	const TInt startWord = aX / KPixelsPerWord;
	TUint32* pixelPtr = ScanLine(aY) + startWord;
	TUint32* pixelPtrLimit = ScanLine(yLimit) + startWord;
	TUint32 mask = 1 << (aX & 0x1f);
	TUint32 dataMask = 1;

	if (white)
		{
		while (pixelPtr != pixelPtrLimit)
			{
			if (!dataMask)
				{
				dataMask = 1;
				aBuffer++;
				}

			if (aBuffer[0] & dataMask)
				pixelPtr[0] |= mask;

			dataMask <<= 1;
			pixelPtr += scanLineWords;
			}
		}
	else
		{
		mask = ~mask;
		while (pixelPtr != pixelPtrLimit)
			{
			if (!dataMask)
				{
				dataMask = 1;
				aBuffer++;
				}

			if (aBuffer[0] & dataMask)
				pixelPtr[0] &= mask;

			dataMask <<= 1;
			pixelPtr += scanLineWords;
			}
		}
	}

void CDrawOneBppBitmap::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishX = aX + aLength;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << startShiftExtra) & (0xffffffff >> (startLong - finishX));
		pixelPtrLimit[0] &= ~mask;
		pixelPtrLimit[0] |= (aBuffer[0] << startShiftExtra) & mask;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] &= 0xffffffff >> startShift;
		pixelPtr[-1] |= aBuffer[0] << startShiftExtra;

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < finishX)
			{
			TUint32 value = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] = PasteInt(value,pixelPtrLimit[0],32 - finishX + finishLong);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ = *aBuffer++;

		if (finishLong < finishX)
			pixelPtrLimit[0] = PasteInt(aBuffer[0],pixelPtrLimit[0],32 - finishX + finishLong);
		}
	}

void CDrawOneBppBitmap::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishX = aX + aLength;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << startShiftExtra) & (0xffffffff >> (startLong - finishX));
		pixelPtrLimit[0] ^= (aBuffer[0] << startShiftExtra) & mask;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] ^= aBuffer[0] << startShiftExtra;

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] ^= (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < finishX)
			{
			TUint32 value = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] ^= PasteInt(value,0,32 - finishX + finishLong);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ ^= *aBuffer++;

		if (finishLong < finishX)
			pixelPtrLimit[0] ^= PasteInt(aBuffer[0],0,32 - finishX + finishLong);
		}
	}

void CDrawOneBppBitmap::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishX = aX + aLength;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << startShiftExtra) & (0xffffffff >> (startLong - finishX));
		pixelPtrLimit[0] &= (aBuffer[0] << startShiftExtra) | ~mask;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] &= (aBuffer[0] << startShiftExtra) | (0xffffffff >> startShift);

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] &= (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < finishX)
			{
			TUint32 value = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] &= PasteInt(value,0xffffffff,32 - finishX + finishLong);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ &= *aBuffer++;

		if (finishLong < finishX)
			pixelPtrLimit[0] &= PasteInt(aBuffer[0],0xffffffff,32 - finishX + finishLong);
		}
	}

void CDrawOneBppBitmap::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishX = aX + aLength;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << startShiftExtra) & (0xffffffff >> (startLong - finishX));
		pixelPtrLimit[0] |= (aBuffer[0] << startShiftExtra) & mask;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] |= aBuffer[0] << startShiftExtra;

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] |= (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < finishX)
			{
			TUint32 value = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] |= PasteInt(value,0,32 - finishX + finishLong);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ |= *aBuffer++;

		if (finishLong < finishX)
			pixelPtrLimit[0] |= PasteInt(aBuffer[0],0,32 - finishX + finishLong);
		}
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawOneBppBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                          const TUint8* aRgbBuffer,
                                          const TUint8* aMaskBuffer,
                                          MAlphaBlend::TShadowing,
                                          CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	TUint32 bitMask = 1 << (aX & 0x1f);

	while (aMaskBuffer < maskBufferPtrLimit)
		{
		if (*aMaskBuffer++ & 0x80)
			{
			if (((aRgbBuffer[2] << 1) + aRgbBuffer[1] + (aRgbBuffer[1] << 2) + aRgbBuffer[0]) > 1016)
				pixelPtr[0] |= bitMask;
			else
				pixelPtr[0] &= ~bitMask;
			}

		bitMask <<= 1;

		if (!bitMask)
			{
			bitMask = 1;
			pixelPtr++;
			}

		aRgbBuffer += 4;
		}
	}

void CDrawOneBppBitmap::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TUint32 colorWord = (aColor._Gray2() == 1) ? 0xffffffff : 0;
	const TInt yLimit = aY + aHeight;
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt finishShift = 32 - aX - aLength + finishLong;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << (32 - startShift)) & (0xffffffff >> (startShift - aLength));
		const TUint32 maskedColorWord = colorWord & mask;
		mask = ~mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= mask;
			pixelPtrLimit[0] |= maskedColorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		TUint32* bmpbitstmp = pixelPtr;

		if (startShift > 0)
			bmpbitstmp[-1] = PasteInt(bmpbitstmp[-1],colorWord,startShift);

		while (bmpbitstmp < pixelPtrLimit)
			*bmpbitstmp++ = colorWord;

		if (extra)
			pixelPtrLimit[0] = PasteInt(colorWord,pixelPtrLimit[0],finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawOneBppBitmap::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TUint32 colorWord = (aColor._Gray2() == 1) ? 0xffffffff : 0;
	const TInt yLimit = aY + aHeight;
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt finishShift = 32 - aX - aLength + finishLong;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << (32 - startShift)) & (0xffffffff >> (startShift - aLength));
		const TUint32 maskedColorWord = colorWord & mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] ^= maskedColorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		TUint32* bmpbitstmp = pixelPtr;

		if (startShift > 0)
			bmpbitstmp[-1] ^= PasteInt(0,colorWord,startShift);

		while (bmpbitstmp < pixelPtrLimit)
			*bmpbitstmp++ ^= colorWord;

		if (extra)
			pixelPtrLimit[0] ^= PasteInt(colorWord,0,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawOneBppBitmap::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TUint32 colorWord = (aColor._Gray2() == 1) ? 0xffffffff : 0;
	const TInt yLimit = aY + aHeight;
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt finishShift = 32 - aX - aLength + finishLong;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << (32 - startShift)) & (0xffffffff >> (startShift - aLength));
		const TUint32 maskedColorWord = colorWord | ~mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= maskedColorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		TUint32* bmpbitstmp = pixelPtr;

		if (startShift > 0)
			bmpbitstmp[-1] &= PasteInt(0xffffffff,colorWord,startShift);

		while (bmpbitstmp < pixelPtrLimit)
			*bmpbitstmp++ &= colorWord;

		if (extra)
			pixelPtrLimit[0] &= PasteInt(colorWord,0xffffffff,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawOneBppBitmap::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TUint32 colorWord = (aColor._Gray2() == 1) ? 0xffffffff : 0;
	const TInt yLimit = aY + aHeight;
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0x1f);
	const TInt finishLong = (aX + aLength) & (~0x1f);
	const TInt startShift = startLong - aX;
	const TInt finishShift = 32 - aX - aLength + finishLong;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff << (32 - startShift)) & (0xffffffff >> (startShift - aLength));
		const TUint32 maskedColorWord = colorWord & mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] |= maskedColorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		TUint32* bmpbitstmp = pixelPtr;

		if (startShift > 0)
			bmpbitstmp[-1] |= PasteInt(bmpbitstmp[-1],colorWord,startShift);

		while (bmpbitstmp < pixelPtrLimit)
			*bmpbitstmp++ |= colorWord;

		if (extra)
			pixelPtrLimit[0] |= PasteInt(colorWord,pixelPtrLimit[0],finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawOneBppBitmap::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 8);
	TInt pixelMask = 1 << (aX & 7);
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);

	const TInt gray = aColor._Gray256();
	TRgb pixelColor;
	
	while (aMaskBuffer < maskBufferPtrLimit)
		{
		const TInt pixelGray256Value = (pixelPtr[0] & pixelMask) ? 255 : 0;
		pixelColor = TRgb::_Gray256(((gray * aMaskBuffer[0]) + ((255 - aMaskBuffer[0]) * pixelGray256Value)) / 255);
		if (pixelColor._Gray2())
			pixelPtr[0] |= pixelMask;
		else
			pixelPtr[0] &= ~pixelMask;

		pixelMask <<= 1;
		if (pixelMask > 128)
			{
			pixelPtr++;
			pixelMask = 1;
			}
		aMaskBuffer++;
		}
	}

TInt CDrawOneBppBitmap::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
												TUint32 aOutlinePenColor, TUint32 aShadowColor,
												TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	//This is non-optimised since this screen mode is rarely used and is usually 
	//fast enough without optimisation.
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 8);
	TInt pixelMask = 1 << (aX & 7);
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
			pixelMask <<= 1;
			if (pixelMask > 0x80)
				{
				pixelPtr++;
				pixelMask = 1;
				}
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
			TRgb backgroundColor = TRgb::_Gray2((pixelPtr[0] & pixelMask) ? 255 : 0);
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
		
		if (finalColor._Gray2())
			{
			pixelPtr[0] |= pixelMask;
			}
		else
			{
			pixelPtr[0] &= ~pixelMask;
			}

		pixelMask <<= 1;
		
		if (pixelMask > 0x80)
			{
			pixelPtr++;
			pixelMask = 1;
			}
		}
	return KErrNone;
	}
