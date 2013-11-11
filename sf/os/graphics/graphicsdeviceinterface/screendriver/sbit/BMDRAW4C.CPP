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

const TInt color16ShadowTable[16] = {
	0,	0,	0,	0,	0,	2,	3,	4,
	11,	12,	13,	0,	0,	0,	1,	14
	};

const TInt KPixelsPerWord = 8;
const TInt KPixelsPerByte = 2;
const TInt KBitsPerPixel = 4;

// CDrawFourBppBitmapColor

//Initializes iSize, iDrawRect, iLongWidth, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawFourBppBitmapColor::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = (iSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1);
	iScanLineWords = iLongWidth / KPixelsPerWord;
	}
 
TInt CDrawFourBppBitmapColor::Construct(TSize aSize)
	{
	return Construct(aSize, ((aSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1)) / KPixelsPerByte);
	}

TInt CDrawFourBppBitmapColor::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	iDispMode = EColor16;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride & 3)
		return KErrArgument;
	iLongWidth = aStride * KPixelsPerByte;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = aStride >> 2;
	TInt size = 1 + (Max(aSize.iWidth,aSize.iHeight) >> 1);
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if (iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

void CDrawFourBppBitmapColor::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = FadeRgb(TRgb::Color16(aColor.Color16()));

	if (iShadowMode & EShadow)
		aColor = TRgb::Color16(color16ShadowTable[aColor.Color16()]);
	}

TUint32 CDrawFourBppBitmapColor::ShadowWord(TUint32 aWord)
	{
	TUint32 shadowWord = color16ShadowTable[aWord & 0xf];

	shadowWord |= color16ShadowTable[(aWord >> 4) & 0xf] << 4;
	shadowWord |= color16ShadowTable[(aWord >> 8) & 0xf] << 8;
	shadowWord |= color16ShadowTable[(aWord >> 12) & 0xf] << 12;
	shadowWord |= color16ShadowTable[(aWord >> 16) & 0xf] << 16;
	shadowWord |= color16ShadowTable[(aWord >> 20) & 0xf] << 20;
	shadowWord |= color16ShadowTable[(aWord >> 24) & 0xf] << 24;
	shadowWord |= color16ShadowTable[(aWord >> 28) & 0xf] << 28;

	return shadowWord;
	}

TUint32 CDrawFourBppBitmapColor::FadeWord(TUint32 aWord)
	{
	TUint32 fadeWord = FadeRgb(TRgb::Color16(aWord & 0xf)).Color16();

	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 4) & 0xf)).Color16() << 4;
	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 8) & 0xf)).Color16() << 8;
	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 12) & 0xf)).Color16() << 12;
	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 16) & 0xf)).Color16() << 16;
	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 20) & 0xf)).Color16() << 20;
	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 24) & 0xf)).Color16() << 24;
	fadeWord |= FadeRgb(TRgb::Color16((aWord >> 28) & 0xf)).Color16() << 28;

	return fadeWord;
	}

TUint32 CDrawFourBppBitmapColor::ColorInt(TRgb aColor) const
	{
	TUint32 colorWord = aColor.Color16();

	colorWord |= colorWord << 4;
	colorWord |= colorWord << 8;
	colorWord |= colorWord << 16;

	return colorWord;
	}

void CDrawFourBppBitmapColor::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* bufferLimit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	while (aBuffer < bufferLimit)
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
void CDrawFourBppBitmapColor::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
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
	ReadLineCommon(pixelPtr,bufferPtr,wordsCnt,restPixels,bytesCnt,shiftBits*KBitsPerPixel);
	}


TRgb CDrawFourBppBitmapColor::ReadRgbNormal(TInt aX,TInt aY) const
	{
	TUint32 col = *(ScanLine(aY) + (aX / KPixelsPerWord));
	col >>= ((aX & 7) * KBitsPerPixel);
	return TRgb::Color16(col & 0xf);
	}

void CDrawFourBppBitmapColor::ShadowArea(const TRect& aRect)
	{
	__ASSERT_DEBUG(aRect.iTl.iX>=0 && aRect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aRect.iTl.iY>=0 && aRect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	const TInt startLong = (aRect.iTl.iX + KPixelsPerWord - 1) & ~7;
	const TInt finishLong = aRect.iBr.iX & ~7;
	const TInt startShift = (startLong - aRect.iTl.iX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aRect.iBr.iX + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aRect.iTl.iY);

	if (iShadowMode & EFade)
		{
		TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
		TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

		if (finishLong < startLong)
			{
			const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
			const TUint32 inverseMask = ~mask;

			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				const TUint32 faded = FadeWord(pixelPtrLimit[0]) & mask;
				pixelPtrLimit[0] &= inverseMask;
				pixelPtrLimit[0] |= faded;
				pixelPtrLimit += iScanLineWords;
				}
			}
		else
			{
			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				if (aRect.iTl.iX < startLong)
					pixelPtr[-1] = PasteInt(pixelPtr[-1],FadeWord(pixelPtr[-1]),startShift);

				for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
					tempPixelPtr[0] = FadeWord(tempPixelPtr[0]);

				if (finishLong < aRect.iBr.iX)
					pixelPtrLimit[0] = PasteInt(FadeWord(pixelPtrLimit[0]),pixelPtrLimit[0],finishShift);

				pixelPtr += iScanLineWords;
				pixelPtrLimit += iScanLineWords;
				}
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
		TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

		if (finishLong < startLong)
			{
			const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
			const TUint32 inverseMask = ~mask;

			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				TUint32 shadowed = ShadowWord(pixelPtrLimit[0]) & mask;
				pixelPtrLimit[0] &= inverseMask;
				pixelPtrLimit[0] |= shadowed;
				pixelPtrLimit += iScanLineWords;
				}
			}
		else
			{
			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				if (aRect.iTl.iX < startLong)
					pixelPtr[-1] = PasteInt(pixelPtr[-1],ShadowWord(pixelPtr[-1]),startShift);

				for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
					tempPixelPtr[0] = ShadowWord(tempPixelPtr[0]);

				if (finishLong < aRect.iBr.iX)
					pixelPtrLimit[0] = PasteInt(ShadowWord(pixelPtrLimit[0]),pixelPtrLimit[0],finishShift);

				pixelPtr += iScanLineWords;
				pixelPtrLimit += iScanLineWords;
				}
			}
		}
	}

void CDrawFourBppBitmapColor::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aBuffer != NULL,Panic(EScreenDriverPanicInvalidParameter));

	const TUint32* bufferLimit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	if (iShadowMode & EFade)
		{
		for (TUint32* buffer = aBuffer; buffer < bufferLimit; buffer++)
			buffer[0] = FadeWord(buffer[0]);
		}

	if (iShadowMode & EShadow)
		{
		for (TUint32* buffer = aBuffer; buffer < bufferLimit; buffer++)
			buffer[0] = ShadowWord(buffer[0]);
		}
	}

void CDrawFourBppBitmapColor::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TInt shift = (aX & 7) * KBitsPerPixel;
	pixelPtr[0] &= ~(0xf << shift);
	pixelPtr[0] |= aColor.Color16() << shift;
	}

void CDrawFourBppBitmapColor::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt xLimit = aX + aLength;
	const TInt yLimit = aY + aHeight;
	const TUint8 color16 = TUint8(aColor.Color16());

	for (; aY < yLimit; aY++,aData++)
		{
		TUint32 dataMask = 1;
		TUint8* pixelPtr = (TUint8*)ScanLine(aY);
		pixelPtr += aX / 2;

		if (color16)
			{
			for (TInt x = aX; x < xLimit; x++,dataMask <<= 1)
				{
				if (aData[0] & dataMask)
					{
					if (x & 1)
						pixelPtr[0] = TUint8((color16 << 4) | (pixelPtr[0] & 0x0f));
					else
						pixelPtr[0] = TUint8((pixelPtr[0] & 0xf0) | color16);
					}

				if (x & 1)
					pixelPtr++;
				}
			}
		else
			{
			TUint8 bytemask = TUint8((aX & 1) ? 0x0f : 0xf0);

			for (TInt x = aX; x < xLimit; x++,dataMask <<= 1,bytemask = (TUint8)~bytemask)
				{
				if (aData[0] & dataMask)
					pixelPtr[0] &= bytemask;

				if (x & 1)
					pixelPtr++;
				}
			}
		}
	}

void CDrawFourBppBitmapColor::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	const TInt xLimit = aX + aLength;
	const TInt yLimit = aY + aHeight;
	const TInt color16 = aColor.Color16();

	for (; aY < yLimit; aY++)
		{
		TUint32 dataMask = 1;
		TUint8* pixelPtr = ((TUint8*)ScanLine(aY)) + (aX / 2);
		TUint8 bytemask = TUint8((aX & 1) ? 0x0f : 0xf0);

		if (color16)
			{
			for (TInt x = aX; x < xLimit; x++,dataMask <<= 1,bytemask = (TUint8)~bytemask)
				{
				if (aData[0] & dataMask)
					{
					TUint8 pixelColorIndex = TUint8((x & 1) ? color16 << 4 : color16);
					if (aDrawMode == CGraphicsContext::EDrawModeXOR)
						pixelPtr[0] = (TUint8)(pixelColorIndex ^ pixelPtr[0]);
					else if (aDrawMode == CGraphicsContext::EDrawModeAND)
						pixelPtr[0] = (TUint8)((pixelColorIndex | bytemask) & pixelPtr[0]);
					else if (aDrawMode == CGraphicsContext::EDrawModeOR)
						pixelPtr[0] = (TUint8)(pixelColorIndex | pixelPtr[0]);
					}

				if (x & 1)
					pixelPtr++;
				}
			}
		else
			{
			if (aDrawMode == CGraphicsContext::EDrawModeAND)
				{
				for (TInt x = aX; x < xLimit; x++,dataMask <<= 1,bytemask = (TUint8)~bytemask)
					{
					if (aData[0] & dataMask)
						pixelPtr[0] &= bytemask;

					if (x & 1)
						pixelPtr++;
					}
				}
			}

		aData++;
		}
	}

void CDrawFourBppBitmapColor::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aLength,TRgb aColor,TBool aUp)
	{
	const TInt yLimit = aY + (aUp ? -aLength : aLength);
	TUint32 color16 = aColor.Color16();
	const TInt scanLineWords = aUp ? -iScanLineWords : iScanLineWords;
	const TInt startWord = aX / KPixelsPerWord;
	const TInt startShift = (aX & 7) * KBitsPerPixel;
	TUint32* pixelPtr = ScanLine(aY) + startWord;
	const TUint32* pixelPtrLimit = ScanLine(yLimit) + startWord;
	const TUint32 mask = ~(0xf << startShift);
	TUint32 dataMask = 1;

	if (color16)
		{
		color16 <<= startShift;
		while (pixelPtr != pixelPtrLimit)
			{
			if (!dataMask)
				{
				dataMask = 1;
				aData++;
				}

			if (aData[0] & dataMask)
				{
				pixelPtr[0] &= mask;
				pixelPtr[0] |= color16;
				}

			dataMask <<= 1;
			pixelPtr += scanLineWords;
			}
		}
	else
		{
		while (pixelPtr != pixelPtrLimit)
			{
			if (!dataMask)
				{
				dataMask = 1;
				aData++;
				}

			if (aData[0] & dataMask)
				pixelPtr[0] &= mask;

			dataMask <<= 1;
			pixelPtr += scanLineWords;
			}
		}
	}

void CDrawFourBppBitmapColor::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << startShiftExtra);

		pixelPtrLimit[0] &= ~mask;
		pixelPtrLimit[0] |= (aBuffer[0] << startShiftExtra) & mask;
		pixelPtrLimit += iScanLineWords;
		return;
		}

	const TInt wordsToCopy = pixelPtrLimit - pixelPtr;

	if (startShift > 0)
		{
		pixelPtr[-1] = PasteInt(pixelPtr[-1],aBuffer[0] << startShiftExtra,startShift);

		CopyOffset(pixelPtr,aBuffer,wordsToCopy,startShift);

		aBuffer += wordsToCopy;

		if (finishLong < aX + aLength)
			{
			TUint32 first = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] = PasteInt(first,pixelPtrLimit[0],finishShift);
			}
		}
	else
		{
		Mem::Copy(pixelPtr,aBuffer,wordsToCopy * sizeof(TUint32));

		aBuffer += wordsToCopy;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] = PasteInt(aBuffer[0],pixelPtrLimit[0],finishShift);
		}
	}

void CDrawFourBppBitmapColor::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		pixelPtrLimit[0] ^= (aBuffer[0] << startShiftExtra) & (0xffffffff >> finishShift);
		pixelPtrLimit += iScanLineWords;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] ^= PasteInt(0,aBuffer[0] << startShiftExtra,startShift);

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] ^= (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);

			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < aX + aLength)
			{
			TUint32 first = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] ^= PasteInt(first,0,finishShift);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ ^= *aBuffer++;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] ^= PasteInt(aBuffer[0],0,finishShift);
		}
	}

void CDrawFourBppBitmapColor::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << startShiftExtra);

		pixelPtrLimit[0] &= (aBuffer[0] << startShiftExtra) | ~mask;
		pixelPtrLimit += iScanLineWords;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] &= PasteInt(0xffffffff,aBuffer[0] << startShiftExtra,startShift);

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] &= (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);

			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < aX + aLength)
			{
			TUint32 first = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] &= PasteInt(first,0xffffffff,finishShift);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ &= *aBuffer++;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] &= PasteInt(aBuffer[0],0xffffffff,finishShift);
		}
	}

void CDrawFourBppBitmapColor::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		pixelPtrLimit[0] |= (aBuffer[0] << startShiftExtra) & (0xffffffff >> finishShift);
		pixelPtrLimit += iScanLineWords;
		return;
		}

	if (startShift > 0)
		{
		pixelPtr[-1] |= PasteInt(0,aBuffer[0] << startShiftExtra,startShift);

		while (pixelPtr < pixelPtrLimit)
			{
			pixelPtr[0] |= (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);

			pixelPtr++;
			aBuffer++;
			}

		if (finishLong < aX + aLength)
			{
			TUint32 first = (aBuffer[0] >> startShift) | (aBuffer[1] << startShiftExtra);
			pixelPtrLimit[0] |= PasteInt(first,0,finishShift);
			}
		}
	else
		{
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ |= *aBuffer++;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] |= PasteInt(aBuffer[0],0,finishShift);
		}
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawFourBppBitmapColor::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                                const TUint8* aRgbBuffer,
                                                const TUint8* aMaskBuffer,
                                                MAlphaBlend::TShadowing aShadowing,
                                                CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 2);
	TRgb pixelColor;

	if (aX & 1)
		{
        TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor);
            }
        pixelColor = ::AlphaBlend(srcColor,TRgb::Color16(pixelPtr[0] >> 4),aMaskBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelColor);
            }
		MapColorToUserDisplayMode(pixelColor);
		pixelPtr[0] &= 0x0f;
		pixelPtr[0] |= TUint8(pixelColor.Color16() << 4);

		pixelPtr++;
		aRgbBuffer += 4;
		aMaskBuffer++;
		aLength--;
		}

	const TUint8* pixelPtrLimit = pixelPtr + (aLength / 2);
	TRgb pixelColor1, pixelColor2;
	
	while (pixelPtr < pixelPtrLimit)
		{
        TRgb srcColor1(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor1);
            }
        pixelColor1 = ::AlphaBlend(srcColor1,TRgb::Color16(pixelPtr[0] & 0x0f),aMaskBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelColor1);
            }
		MapColorToUserDisplayMode(pixelColor1);

        TRgb srcColor2(aRgbBuffer[6],aRgbBuffer[5],aRgbBuffer[4]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor2);
            }
        pixelColor2 = ::AlphaBlend(srcColor2,TRgb::Color16(pixelPtr[0] >> 4),aMaskBuffer[1]);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelColor2);
            }
		MapColorToUserDisplayMode(pixelColor2);

		pixelPtr[0] = TUint8(pixelColor1.Color16());
		pixelPtr[0] |= TUint8(pixelColor2.Color16() << 4);

		pixelPtr++;
		aRgbBuffer += 8;
		aMaskBuffer += 2;
		}

	if (aLength & 1)
		{
        TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor);
            }
        pixelColor = ::AlphaBlend(srcColor,TRgb::Color16(pixelPtr[0] & 0x0f),aMaskBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelColor);
            }
		MapColorToUserDisplayMode(pixelColor);
		pixelPtr[0] &= 0xf0;
		pixelPtr[0] |= TUint8(pixelColor.Color16());
		}
	}

void CDrawFourBppBitmapColor::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord = ColorInt(aColor);
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << startShiftExtra);
		colorWord &= mask;
		mask = ~mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= mask;
			pixelPtrLimit[0] |= colorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] = PasteInt(pixelPtr[-1],colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] = colorWord;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] = PasteInt(colorWord,pixelPtrLimit[0],finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawFourBppBitmapColor::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord = ColorInt(aColor);
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << startShiftExtra);
		colorWord &= mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] ^= colorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] ^= PasteInt(0,colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] ^= colorWord;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] ^= PasteInt(colorWord,0,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawFourBppBitmapColor::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord = ColorInt(aColor);
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << startShiftExtra);
		colorWord &= mask;
		colorWord |= ~mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= colorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] &= PasteInt(0xffffffff,colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] &= colorWord;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] &= PasteInt(colorWord,0xffffffff,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawFourBppBitmapColor::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord = ColorInt(aColor);
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << startShiftExtra);
		colorWord &= mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] |= colorWord;
			pixelPtrLimit += iScanLineWords;
			}
		return;
		}

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] |= PasteInt(0,colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] |= colorWord;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] |= PasteInt(colorWord,0,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawFourBppBitmapColor::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 2);
	const TBool oddEndCoord = (aX + aLength) & 1;
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength - (oddEndCoord ? 1 : 0);

	if (iShadowMode)
		Shadow(aColor);

	const TInt red = aColor.Red();
	const TInt green = aColor.Green();
	const TInt blue = aColor.Blue();
	
	TRgb pixelColor;
	
	if (aX & 1)
		{
		pixelColor = AlphaBlend(red,green,blue,TRgb::Color16(pixelPtr[0] >> 4),aMaskBuffer[0]);
		pixelPtr[0] &= 0x0f;
		pixelPtr[0] |= TUint8(pixelColor.Color16() << 4);

		pixelPtr++;
		aMaskBuffer++;
		}

	TRgb lowerPixelColor, upperPixelColor;
	
	while (aMaskBuffer < maskBufferPtrLimit)
		{
		lowerPixelColor = AlphaBlend(red,green,blue,TRgb::Color16(pixelPtr[0] & 0x0f),aMaskBuffer[0]);
		upperPixelColor = AlphaBlend(red,green,blue,TRgb::Color16(pixelPtr[0] >> 4),aMaskBuffer[1]);
		pixelPtr[0] = TUint8(lowerPixelColor.Color16());
		pixelPtr[0] |= TUint8(upperPixelColor.Color16() << 4);

		pixelPtr++;
		aMaskBuffer += 2;
		}

	if (oddEndCoord)
		{
		pixelColor = AlphaBlend(red,green,blue,TRgb::Color16(pixelPtr[0] & 0x0f),aMaskBuffer[0]);
		pixelPtr[0] &= 0xf0;
		pixelPtr[0] |= TUint8(pixelColor.Color16());
		}
	}

void CDrawFourBppBitmapColor::MapColorToUserDisplayMode(TRgb& aColor)
	{
	switch (iUserDispMode)
		{
	case EGray2:
		aColor = TRgb::_Gray2(aColor._Gray2());
		break;
	case EGray4:
	case EGray16:// EGray16 can't be done - nearest is EGray4
	case EGray256:// EGray256 can't be done - nearest is EGray4
		aColor = TRgb::_Gray4(aColor._Gray4());
		break;
	default:
		break;
		}
	}

void CDrawFourBppBitmapColor::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint8* bufferPtr = (TUint8*)aBuffer;
	const TUint8* bufferLimit = bufferPtr + ((aLength + 1) / 2);
	TRgb color1, color2;
	
	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			color1 = TRgb::Color16(*bufferPtr & 0xf);
			color2 = TRgb::Color16(*bufferPtr >> 4);
			color1 = TRgb::_Gray2(color1._Gray2());
			color2 = TRgb::_Gray2(color2._Gray2());
			*bufferPtr++ = TUint8(color1.Color16() | (color2.Color16() << 4));
			}
		break;
	case EGray4:
	case EGray16:// EGray256 can't be done - nearest is EGray4
	case EGray256:// EGray256 can't be done - nearest is EGray4
		while (bufferPtr < bufferLimit)
			{
			color1 = TRgb::Color16(*bufferPtr & 0xf);
			color2 = TRgb::Color16(*bufferPtr >> 4);
			color1 = TRgb::_Gray4(color1._Gray4());
			color2 = TRgb::_Gray4(color2._Gray4());
			*bufferPtr++ = TUint8(color1.Color16() | (color2.Color16() << 4));
			}
		break;
	default:
		break;
		}
	}

TInt CDrawFourBppBitmapColor::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
														TUint32 aOutlinePenColor, TUint32 aShadowColor,
														TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	//This is non-optimised since this screen mode is rarely used and is usually 
	//fast enough without optimisation.
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 2);
	const TBool oddEndCoord = (aX + aLength) & 1;
	const TUint8* dataBufferPtrLimit = aDataBuffer + aLength - (oddEndCoord ? 1 : 0);

	TUint8 index = 0;
	TRgb finalColor;
	TRgb lowerPixelColor;
	TRgb upperPixelColor;

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

	if (aX & 1)
		{
		index = *aDataBuffer;
		if (255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			TRgb backgroundColor = TRgb::Color16(pixelPtr[0] >> 4);
			finalColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
										redOutlinePenColor, redShadowColor, redFillColor,
										greenOutlinePenColor, greenShadowColor, greenFillColor,
										blueOutlinePenColor, blueShadowColor, blueFillColor,
										backgroundColor, index);
			pixelPtr[0] &= 0x0f;
			pixelPtr[0] |= TUint8(finalColor.Color16() << 4);
			}
		pixelPtr++;
		aDataBuffer++;
		}

	while (aDataBuffer < dataBufferPtrLimit)
		{
		index = aDataBuffer[0];
		TRgb upperPixelBackgroundColor = TRgb::Color16(pixelPtr[0] >> 4);
		if (255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			TRgb lowerPixelBackgroundColor = TRgb::Color16(pixelPtr[0] & 0x0f );
			lowerPixelColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
											redOutlinePenColor, redShadowColor, redFillColor,
											greenOutlinePenColor, greenShadowColor, greenFillColor,
											blueOutlinePenColor, blueShadowColor, blueFillColor,
											lowerPixelBackgroundColor, index);
			pixelPtr[0] = TUint8(lowerPixelColor.Color16());
			}
		
		index = aDataBuffer[1];
		if (255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			upperPixelColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
											redOutlinePenColor, redShadowColor, redFillColor,
											greenOutlinePenColor, greenShadowColor, greenFillColor,
											blueOutlinePenColor, blueShadowColor, blueFillColor,
											upperPixelBackgroundColor, index);
			pixelPtr[0] |= TUint8(upperPixelColor.Color16() << 4);
			}
		pixelPtr++;
		aDataBuffer += 2;
		}

	if (oddEndCoord)
		{
		index = aDataBuffer[0];
		if(255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			TRgb backgroundColor = TRgb::Color16(pixelPtr[0] & 0x0f );
			finalColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
										redOutlinePenColor, redShadowColor, redFillColor,
										greenOutlinePenColor, greenShadowColor, greenFillColor,
										blueOutlinePenColor, blueShadowColor, blueFillColor,
										backgroundColor, index);
			pixelPtr[0] &= 0xf0;
			pixelPtr[0] |= TUint8(finalColor.Color16());
			}
		}
	return KErrNone;
	}

TRgb CDrawFourBppBitmapColor::BlendFourColors(TUint32 aOutlinePenColor, TUint32 aShadowColor, TUint32 aFillColor,
											TInt aRedOutlinePenColor, TInt aRedShadowColor,TInt aRedFillColor,
											TInt aGreenOutlinePenColor, TInt aGreenShadowColor, TInt aGreenFillColor,
											TInt aBlueOutlinePenColor,	TInt aBlueShadowColor,	TInt aBlueFillColor,
											TRgb aBackgroundColor, TUint8 aIndex) const
	{
	TRgb finalColor;
	if (255 == FourColorBlendLookup[aIndex][KFillColorIndex])
		{
		//fill colour
		finalColor.SetInternal(aFillColor);
		}
	else if (255 == FourColorBlendLookup[aIndex][KShadowColorIndex])
		{
		//Shadow colour
		finalColor.SetInternal(aShadowColor);
		}
	else if (255 == FourColorBlendLookup[aIndex][KOutlineColorIndex])
		{
		//Outline colour
		finalColor.SetInternal(aOutlinePenColor);
		}
	else
		{
		TInt blendedRedColor = (aRedOutlinePenColor * FourColorBlendLookup[aIndex][KOutlineColorIndex] + 
						   		aRedShadowColor * FourColorBlendLookup[aIndex][KShadowColorIndex] +
						  		aRedFillColor * FourColorBlendLookup[aIndex][KFillColorIndex] + 
						  		aBackgroundColor.Red() * FourColorBlendLookup[aIndex][KBackgroundColorIndex]) >> 8;

		TInt blendedGreenColor = (aGreenOutlinePenColor * FourColorBlendLookup[aIndex][KOutlineColorIndex] + 
								aGreenShadowColor * FourColorBlendLookup[aIndex][KShadowColorIndex] +
								aGreenFillColor * FourColorBlendLookup[aIndex][KFillColorIndex] + 
								aBackgroundColor.Green() * FourColorBlendLookup[aIndex][KBackgroundColorIndex]) >> 8;

		TInt blendedBlueColor = (aBlueOutlinePenColor * FourColorBlendLookup[aIndex][KOutlineColorIndex] + 
								aBlueShadowColor * FourColorBlendLookup[aIndex][KShadowColorIndex] +
								aBlueFillColor * FourColorBlendLookup[aIndex][KFillColorIndex] + 
								aBackgroundColor.Blue() * FourColorBlendLookup[aIndex][KBackgroundColorIndex]) >> 8;
		finalColor = TRgb(blendedRedColor, blendedGreenColor, blendedBlueColor);
		}
	return finalColor;
	}
