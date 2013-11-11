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

GLREF_D const TUint8 ditherlutab[16][4];
GLREF_D const TUint8 shadowlutab[256];

const TInt KPixelsPerWord = 8;
const TInt KPixelsPerByte = 2;
const TInt KBitsPerPixel = 4;

// CDrawFourBppBitmapGray

//Initializes iSize, iDrawRect, iLongWidth, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawFourBppBitmapGray::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = (iSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1);
	iScanLineWords = iLongWidth / KPixelsPerWord;
	}
 
TInt CDrawFourBppBitmapGray::Construct(TSize aSize)
	{
	return Construct(aSize, ((aSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1)) / KPixelsPerByte);
	}

TInt CDrawFourBppBitmapGray::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	iDispMode = EGray16;
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

void CDrawFourBppBitmapGray::Shadow(TRgb& aColor)
	{
	aColor = TRgb::_Gray16(ShadowAndFadeGray16(aColor._Gray16()));
	}

TUint8 CDrawFourBppBitmapGray::ShadowAndFadeGray16(TInt aGray16)
	{
	if (iShadowMode & EFade)
		aGray16 = FadeGray(aGray16 * 17) >> 4;

	if (iShadowMode & EShadow)
		aGray16 = Max(aGray16 - 5,0);

	return (TUint8)aGray16;
	}

TUint32 CDrawFourBppBitmapGray::FadeWord(TUint32 aWord)
	{
	TUint32 fadedWord = 0;

	for (TInt bitShift = 0; bitShift < 32; bitShift += 4)
		{
		TInt gray = (aWord >> bitShift) & 0xf;
		gray = FadeGray(gray * 17) >> 4;
		fadedWord |= gray << bitShift;
		}

	return fadedWord;
	}

TUint32 CDrawFourBppBitmapGray::ColorInt(TRgb aColor) const
	{
	TUint32 colorWord = aColor._Gray16();

	colorWord |= colorWord << 4;
	colorWord |= colorWord << 8;
	colorWord |= colorWord << 16;

	return colorWord;
	}

void CDrawFourBppBitmapGray::DitherBuffer(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	aX += iDitherOrigin.iX;
	aY += iDitherOrigin.iY;
	aX &= 1;
	aY &= 1;
	aY <<= 1;

	const TInt first = aX + aY;
	aX ^= 1;
	const TInt second = aX + aY;

	TUint8* bufferPtr = (TUint8*)aBuffer;
	const TUint8* bufferLimit = bufferPtr + ((aLength + 1) / 2);

	for (; bufferPtr < bufferLimit; bufferPtr++)
		{
		TUint8 index1 = bufferPtr[0];
		TUint8 index2 = TUint8(index1 >> 4);
		index1 &= 0xf;

		const TUint8 value1 = ditherlutab[index1][first];
		const TUint8 value2 = ditherlutab[index2][second];
		bufferPtr[0] = TUint8(value1 | (value2 << 4));
		}
	}

TUint32 CDrawFourBppBitmapGray::HashInt(TRgb aColor,TInt aX,TInt aY) const
	{
	const TUint32 gray = aColor._Gray16();
	const TUint32 int1 = Hash(gray,aX,aY);
	const TUint32 int2 = Hash(gray,aX + 1,aY);

	TUint32 colorWord = int1 | (int2 << 4);

	colorWord |= colorWord << 8;
	colorWord |= colorWord << 16;

	return colorWord;
	}

void CDrawFourBppBitmapGray::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* bufferLimit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	while (aBuffer < bufferLimit)
		*aBuffer++ ^= 0xffffffff;
	}

/**	Copies a number of pixels into a word-aligned buffer without format translation.
	Note that the byte length to the target buffer is honoured, 
 	but the end contents of the last byte are generally overwritten with extra pixel data (or garbage).
 	Note that I am assuming the compiler optimiser will convert all these divides and multiplies into shifts!  
@param	aX		x coordinate to start copy from (need not be aligned at all)
@param	aY		y coordinate to copy line from	
@param	aLength	number of pixels to copy  
@param	aBuffer	target word-aligned buffer (but may or may not be word length) 
 **/
void CDrawFourBppBitmapGray::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
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


TRgb CDrawFourBppBitmapGray::ReadRgbNormal(TInt aX,TInt aY) const
	{
	TUint32 colorWord = *(ScanLine(aY) + (aX / KPixelsPerWord));
	colorWord >>= ((aX & 7) * KBitsPerPixel);
	return TRgb::_Gray16(colorWord & 0xf);
	}

void CDrawFourBppBitmapGray::ShadowArea(const TRect& aRect)
	{
	__ASSERT_DEBUG(aRect.iTl.iX>=0 && aRect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aRect.iTl.iY>=0 && aRect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	TInt startLong = (aRect.iTl.iX + KPixelsPerWord - 1) & ~7;
	TInt finishLong = aRect.iBr.iX & ~7;
	TInt startShift = (startLong - aRect.iTl.iX) * KBitsPerPixel;
	TInt finishShift = (KPixelsPerWord - aRect.iBr.iX + finishLong) * KBitsPerPixel;
	TInt startLongAdjust = startLong / KPixelsPerWord;
	TInt finishLongAdjust = finishLong / KPixelsPerWord;
	TUint32* base = ScanLine(aRect.iTl.iY);

	if (iShadowMode & EFade)
		{
		TUint32* pixelPtr = base + startLongAdjust;
		TUint32* pixelPtrLimit = base + finishLongAdjust;

		if (finishLong < startLong)
			{
			const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
			const TUint32 invertedMask = ~mask;

			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				TUint32 shadowed = FadeWord(pixelPtrLimit[0]) & mask;
				pixelPtrLimit[0] &= invertedMask;
				pixelPtrLimit[0] |= shadowed;
				pixelPtrLimit += iScanLineWords;
				}
			}
		else
			{
			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				if (aRect.iTl.iX < startLong)
					{
					TUint32 shadowed = FadeWord(pixelPtr[-1]);
					pixelPtr[-1] = PasteInt(pixelPtr[-1],shadowed,startShift);
					}

				for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
					tempPixelPtr[0] = FadeWord(tempPixelPtr[0]);

				if (finishLong < aRect.iBr.iX)
					{
					TUint32 shadowed = FadeWord(pixelPtrLimit[0]);
					pixelPtrLimit[0] = PasteInt(shadowed,pixelPtrLimit[0],finishShift);
					}

				pixelPtr += iScanLineWords;
				pixelPtrLimit += iScanLineWords;
				}
			}
		}

	if (iShadowMode & EShadow)
		{
		if (iUserDispMode == EGray2)
			{
			WriteRgbMulti(aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height(),KRgbBlack);
			return;
			}

		TUint32* pixelPtr = base + startLongAdjust;
		TUint32* pixelPtrLimit = base + finishLongAdjust;

		if (finishLong < startLong)
			{
			const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
			const TUint32 invertedMask = ~mask;

			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				TUint32 shadowed = shadowlutab[pixelPtrLimit[0] & 0xff];
				shadowed |= (shadowlutab[(pixelPtrLimit[0] >> 8) & 0xff] << 8);
				shadowed |= (shadowlutab[(pixelPtrLimit[0] >> 16) & 0xff] << 16);
				shadowed |= (shadowlutab[(pixelPtrLimit[0] >> 24) & 0xff] << 24);

				pixelPtrLimit[0] &= invertedMask;
				pixelPtrLimit[0] |= (shadowed & mask);
				pixelPtrLimit += iScanLineWords;
				}
			}
		else
			{
			for (TInt y = aRect.iTl.iY; y < aRect.iBr.iY; y++)
				{
				if (aRect.iTl.iX < startLong)
					{
					TUint32 shadowed = shadowlutab[pixelPtr[-1] & 0xff];
					shadowed |= (shadowlutab[(pixelPtr[-1] >> 8) & 0xff] << 8);
					shadowed |= (shadowlutab[(pixelPtr[-1] >> 16) & 0xff] << 16);
					shadowed |= (shadowlutab[(pixelPtr[-1] >> 24) & 0xff] << 24);
					pixelPtr[-1] = PasteInt(pixelPtr[-1],shadowed,startShift);
					}

				for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
					{
					TUint32 shadowed = shadowlutab[tempPixelPtr[0] & 0xff];
					shadowed |= (shadowlutab[(tempPixelPtr[0] >> 8) & 0xff] << 8);
					shadowed |= (shadowlutab[(tempPixelPtr[0] >> 16) & 0xff] << 16);
					shadowed |= (shadowlutab[(tempPixelPtr[0] >> 24) & 0xff] << 24);
					tempPixelPtr[0] = shadowed;
					}

				if (finishLong < aRect.iBr.iX)
					{
					TUint32 shadowed = shadowlutab[pixelPtrLimit[0] & 0xff];
					shadowed |= (shadowlutab[(pixelPtrLimit[0] >> 8) & 0xff] << 8);
					shadowed |= (shadowlutab[(pixelPtrLimit[0] >> 16) & 0xff] << 16);
					shadowed |= (shadowlutab[(pixelPtrLimit[0] >> 24) & 0xff] << 24);
					pixelPtrLimit[0] = PasteInt(shadowed,pixelPtrLimit[0],finishShift);
					}

				pixelPtr += iScanLineWords;
				pixelPtrLimit += iScanLineWords;
				}
			}
		}
	}

void CDrawFourBppBitmapGray::ShadeBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aBuffer != NULL,Panic(EScreenDriverPanicInvalidParameter));

	const TUint32* bufferLimit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	while (aBuffer < bufferLimit)
		{
		TUint32 fourthbit = (0x88888888 & aBuffer[0]);
		aBuffer[0] = fourthbit | (fourthbit >> 1);
		aBuffer[0] |= aBuffer[0] >> 2;
		aBuffer++;
		}
	}

void CDrawFourBppBitmapGray::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aBuffer != NULL,Panic(EScreenDriverPanicInvalidParameter));

	const TUint32* bufferLimit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	if (iShadowMode & EFade)
		{
		for (TUint32* bufferPtr = aBuffer; bufferPtr < bufferLimit; bufferPtr++)
			bufferPtr[0] = FadeWord(bufferPtr[0]);
		}

	if (iShadowMode & EShadow)
		{
		for (TUint32* bufferPtr = aBuffer; bufferPtr < bufferLimit; bufferPtr++)
			{
			TUint32 bufferWord = bufferPtr[0];

			TUint firstbyte = shadowlutab[bufferWord & 0xff];
			bufferWord >>= 8;
			firstbyte |= (shadowlutab[bufferWord & 0xff] << 8);
			bufferWord >>= 8;
			firstbyte |= (shadowlutab[bufferWord & 0xff] << 16);
			bufferWord >>= 8;
			firstbyte |= (shadowlutab[bufferWord & 0xff] << 24);

			bufferPtr[0] = firstbyte;
			}
		}
	}

void CDrawFourBppBitmapGray::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TUint32 colorIndex = aColor._Gray16();

	if (iUserDispMode == EGray2)
		colorIndex = (colorIndex > 7) ? 15 : 0;
	else if (iUserDispMode == EGray4)
		colorIndex = Hash(colorIndex,aX,aY);

	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	TInt shift = (aX & 7) * KBitsPerPixel;

	pixelPtr[0] &= ~(0xf << shift);
	pixelPtr[0] |= colorIndex << shift;
	}

void CDrawFourBppBitmapGray::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	TUint32 color1 = 0;
	TUint32 color2 = 0;
	const TInt yLimit = aY + aHeight;
	const TUint32 gray16 = aColor._Gray16();

	if (iUserDispMode == EGray2)
		{
		if (gray16 > 7)
			{
			color1 = 15;
			color2 = 15;
			}
		}
	else if (iUserDispMode != EGray4)
		{
		color1 = gray16;
		color2 = gray16;
		}

	for (; aY < yLimit; aY++,aData++)
		{
		if (iUserDispMode == EGray4)
			{
			color1 = Hash(gray16,aX,aY);
			color2 = Hash(gray16,aX + 1,aY);
			}

		TUint32 color = color1;
		TUint32 dataMask = 1;
		const TInt xLimit = aX + aLength;
		TUint8* pixelPtr = ((TUint8*)ScanLine(aY)) + (aX / 2);

		if (color1 || color2)
			{
			for (TInt x = aX; x < xLimit; x++,dataMask <<= 1)
				{
				if (aData[0] & dataMask)
					{
					if(x & 1)
						pixelPtr[0] = (TUint8)(color << 4 | (pixelPtr[0] & 0x0f));
					else
						pixelPtr[0] = (TUint8)(color | (pixelPtr[0] & 0xf0));
					}

				color = (color == color2) ? color1 : color2;
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

void CDrawFourBppBitmapGray::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	TUint32 color1 = 0;
	TUint32 color2 = 0;
	const TUint32 gray16 = aColor._Gray16();
	const TInt yLimit = aY + aHeight;

	if (iUserDispMode == EGray2)
		{
		if (gray16 > 7)
			{
			color1 = 15;
			color2 = 15;
			}
		}
	else if(iUserDispMode != EGray4)
		{
		color1 = gray16;
		color2 = gray16;
		}

	for (; aY < yLimit; aY++)
		{
		if (iUserDispMode == EGray4)
			{
			color1 = Hash(gray16,aX,aY);
			color2 = Hash(gray16,aX + 1,aY);
			}

		TUint32 color = color1;
		TUint32 dataMask = 1;
		const TInt xLimit = aX + aLength;
		TUint8* pixelPtr = ((TUint8*)ScanLine(aY)) + (aX / 2);

		if(color1 || color2)
			{
			for (TInt x = aX; x < xLimit; x++,dataMask <<= 1)
				{
				if (aData[0] & dataMask)
					{
					if (x & 1)
						{
						if (aDrawMode == CGraphicsContext::EDrawModeXOR)
							pixelPtr[0] = (TUint8)((color << 4) ^ pixelPtr[0]);
						else if (aDrawMode == CGraphicsContext::EDrawModeAND)
							pixelPtr[0] = (TUint8)(((color << 4) | 0x0f) & pixelPtr[0]);
						else if (aDrawMode==CGraphicsContext::EDrawModeOR)
							pixelPtr[0] = (TUint8)((color << 4) | pixelPtr[0]);
						}
					else
						{
						if (aDrawMode == CGraphicsContext::EDrawModeXOR)
							pixelPtr[0] = (TUint8)(color ^ pixelPtr[0]);
						else if (aDrawMode == CGraphicsContext::EDrawModeAND)
							pixelPtr[0] = (TUint8)((color | 0xf0) & pixelPtr[0]);
						else if (aDrawMode==CGraphicsContext::EDrawModeOR)
							pixelPtr[0] = (TUint8)(color | pixelPtr[0]);
						}
					}

				color = (color == color2) ? color1 : color2;
				if (x & 1)
					pixelPtr++;
				}
			}
		else
			{
			TUint8 bytemask = TUint8((aX & 1) ? 0x0f : 0xf0);

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

void CDrawFourBppBitmapGray::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aLength,TRgb aColor,TBool aUp)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	TUint32 color1 = 0;
	TUint32 color2 = 0;
	const TUint32 gray16 = aColor._Gray16();

	if (iUserDispMode == EGray4 && gray16 % 5 != 0)
		{
		color1 = Hash(gray16,aX,aY);
		color2 = Hash(gray16,aX,aY + 1);
		}
	else
		color1 = color2 = gray16;

	const TInt yLimit = aY + (aUp ? -aLength : aLength);
	const TInt scanLineWords = aUp ? -iScanLineWords : iScanLineWords;
	const TInt startword = aX / KPixelsPerWord;
	const TInt startShift = (aX & 7) * KBitsPerPixel;
	TUint32* pixelPtr = ScanLine(aY) + startword;
	const TUint32* pixelPtrLimit = ScanLine(yLimit) + startword;
	const TUint32 mask = ~(0xf << startShift);
	TUint32 dataMask = 1;

	if (color1 || color2)
		{
		color1 <<= startShift;
		color2 <<= startShift;
		TUint32 color = color1;

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
				pixelPtr[0] |= color;
				}

			dataMask <<= 1;
			pixelPtr += scanLineWords;
			color = (color == color2) ? color1 : color2;
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

void CDrawFourBppBitmapGray::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,aBuffer);
	else if (iUserDispMode == EGray4)
		DitherBuffer(aX,aY,aLength,aBuffer);

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
		return;
		}

	const TInt wordsToCopy = pixelPtrLimit - pixelPtr;

	if (startShift > 0)
		{
		pixelPtr[-1] &= 0xffffffff >> startShift;
		pixelPtr[-1] |= aBuffer[0] << startShiftExtra;

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
		while (pixelPtr < pixelPtrLimit)
			*pixelPtr++ = *aBuffer++;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] = PasteInt(aBuffer[0],pixelPtrLimit[0],finishShift);
		}
	}

void CDrawFourBppBitmapGray::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,aBuffer);
	else if (iUserDispMode == EGray4)
		DitherBuffer(aX,aY,aLength,aBuffer);

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

void CDrawFourBppBitmapGray::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,aBuffer);
	else if (iUserDispMode == EGray4)
		DitherBuffer(aX,aY,aLength,aBuffer);

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

void CDrawFourBppBitmapGray::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,aBuffer);
	else if (iUserDispMode == EGray4)
		DitherBuffer(aX,aY,aLength,aBuffer);

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
void CDrawFourBppBitmapGray::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                               const TUint8* aRgbBuffer,
                                               const TUint8* aMaskBuffer,
                                               MAlphaBlend::TShadowing aShadowing,
                                               CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 2);
	TRgb pixelClr;

	if (aX & 1)
		{
        TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor);
            }
		TInt pixelValue = (pixelPtr[0] >> 4) * (255 - aMaskBuffer[0]);
		TInt srceValue = (((srcColor.Red() << 1) + 
                            srcColor.Green() + (srcColor.Green() << 2) + 
                            srcColor.Blue()) >> 7) * aMaskBuffer[0];

		pixelValue += srceValue;
		pixelValue /= 255;

		pixelClr = TRgb::_Gray16(pixelValue);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
	    	Shadow(pixelClr);
            }
		MapColorToUserDisplayMode(pixelClr);

		pixelPtr[0] &= 0x0f;
		pixelPtr[0] |= TUint8(pixelClr._Gray16() << 4);

		pixelPtr++;
		aRgbBuffer += 4;
		aMaskBuffer++;
		aLength--;
		}

	const TUint8* pixelPtrLimit = pixelPtr + (aLength / 2);

	while (pixelPtr < pixelPtrLimit)
		{
        TRgb srcColor1(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor1);
            }
		TInt pixelValue = (pixelPtr[0] & 0x0f) * (255 - aMaskBuffer[0]);
		TInt srceValue = (((srcColor1.Red() << 1) + 
                            srcColor1.Green() + (srcColor1.Green() << 2) + 
                            srcColor1.Blue()) >> 7) * aMaskBuffer[0];

		pixelValue += srceValue;
		pixelValue /= 255;

		TInt nextValue = (pixelPtr[0] >> 4) * (255 - aMaskBuffer[1]);

		pixelClr = TRgb::_Gray16(pixelValue);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
	    	Shadow(pixelClr);
            }
		MapColorToUserDisplayMode(pixelClr);

		pixelPtr[0] = TUint8(pixelClr._Gray16());

        TRgb srcColor2(aRgbBuffer[6],aRgbBuffer[5],aRgbBuffer[4]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor2);
            }
		srceValue = (((srcColor2.Red() << 1) + 
                       srcColor2.Green() + (srcColor2.Green() << 2) + 
                       srcColor2.Blue()) >> 7) * aMaskBuffer[1];

		nextValue += srceValue;
		nextValue /= 255;

		pixelClr = TRgb::_Gray16(nextValue);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
	    	Shadow(pixelClr);
            }
		MapColorToUserDisplayMode(pixelClr);

		pixelPtr[0] |= TUint8(pixelClr._Gray16() << 4);

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
		TInt pixelValue = (pixelPtr[0] & 0x0f) * (255 - aMaskBuffer[0]);
		TInt srceValue = (((srcColor.Red() << 1) + 
                            srcColor.Green() + (srcColor.Green() << 2) + 
                            srcColor.Blue()) >> 7) * aMaskBuffer[0];

		pixelValue += srceValue;
		pixelValue /= 255;

		pixelClr = TRgb::_Gray16(pixelValue);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
	    	Shadow(pixelClr);
            }
		MapColorToUserDisplayMode(pixelClr);

		pixelPtr[0] &= 0xf0;
		pixelPtr[0] |= TUint8(pixelClr._Gray16());
		}
	}

void CDrawFourBppBitmapGray::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt yLimit = aY + aHeight;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	TUint32 colorWord1,colorWord2;
	if (iUserDispMode == EGray4 && aColor._Gray16() % 5 != 0)
		{
		colorWord1 = HashInt(aColor,startLong,aY);
		colorWord2 = HashInt(aColor,startLong,aY + 1);
		}
	else
		colorWord1 = colorWord2 = ColorInt(aColor);
	TUint32 colorWord = colorWord1;

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
		const TUint32 invertedMask = ~mask;
		colorWord &= mask;
		colorWord1 &= mask;
		colorWord2 &= mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= invertedMask;
			pixelPtrLimit[0] |= colorWord;
			pixelPtrLimit += iScanLineWords;
			colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] = PasteInt(pixelPtr[-1],colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] = colorWord;

		if (extra)
			pixelPtrLimit[0] = PasteInt(colorWord,pixelPtrLimit[0],finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
		}
	}

void CDrawFourBppBitmapGray::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt yLimit = aY + aHeight;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	TUint32 colorWord1,colorWord2;
	if (iUserDispMode == EGray4 && aColor._Gray16() % 5 != 0)
		{
		colorWord1 = HashInt(aColor,startLong,aY);
		colorWord2 = HashInt(aColor,startLong,aY + 1);
		}
	else
		colorWord1 = colorWord2 = ColorInt(aColor);
	TUint32 colorWord = colorWord1;

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
		colorWord &= mask;
		colorWord1 &= mask;
		colorWord2 &= mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] ^= colorWord;
			pixelPtrLimit += iScanLineWords;
			colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] ^= PasteInt(0,colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] ^= colorWord;

		if (extra)
			pixelPtrLimit[0] ^= PasteInt(colorWord,0,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
		}
	}

void CDrawFourBppBitmapGray::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt yLimit = aY + aHeight;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	TUint32 colorWord1,colorWord2;
	if (iUserDispMode == EGray4 && aColor._Gray16() % 5 != 0)
		{
		colorWord1 = HashInt(aColor,startLong,aY);
		colorWord2 = HashInt(aColor,startLong,aY + 1);
		}
	else
		colorWord1 = colorWord2 = ColorInt(aColor);
	TUint32 colorWord = colorWord1;

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
		const TUint32 invertedMask = ~mask;
		colorWord &= mask;
		colorWord1 &= mask;
		colorWord2 &= mask;
		colorWord |= invertedMask;
		colorWord1 |= invertedMask;
		colorWord2 |= invertedMask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= colorWord;
			pixelPtrLimit += iScanLineWords;
			colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] &= PasteInt(0xffffffff,colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] &= colorWord;

		if (extra)
			pixelPtrLimit[0] &= PasteInt(colorWord,0xffffffff,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
		}
	}

void CDrawFourBppBitmapGray::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~7);
	const TInt finishLong = (aX + aLength) & (~7);
	const TInt yLimit = aY + aHeight;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	TUint32 colorWord1,colorWord2;
	if (iUserDispMode == EGray4 && aColor._Gray16() % 5 != 0)
		{
		colorWord1 = HashInt(aColor,startLong,aY);
		colorWord2 = HashInt(aColor,startLong,aY + 1);
		}
	else
		colorWord1 = colorWord2 = ColorInt(aColor);
	TUint32 colorWord = colorWord1;

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
		colorWord &= mask;
		colorWord1 &= mask;
		colorWord2 &= mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] |= colorWord;
			pixelPtrLimit += iScanLineWords;
			colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
			}
		return;
		}

	const TBool extra = (finishLong < aX + aLength);

	for (; aY < yLimit; aY++)
		{
		if (startShift > 0)
			pixelPtr[-1] |= PasteInt(0,colorWord,startShift);

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] |= colorWord;

		if (extra)
			pixelPtrLimit[0] |= PasteInt(colorWord,0,finishShift);

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		colorWord = (colorWord == colorWord2) ? colorWord1 : colorWord2;
		}
	}

void CDrawFourBppBitmapGray::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 2);
	const TBool oddEndCoord = (aX + aLength) & 1;
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength - (oddEndCoord ? 1 : 0);

	if (iShadowMode)
		Shadow(aColor);
	
	const TInt gray = aColor._Gray256();
	if (aX & 1)
		{
		const TInt upper = ((gray * aMaskBuffer[0]) + ((255 - aMaskBuffer[0]) * (pixelPtr[0] >> 4) * 17)) / (255 * 17);
		pixelPtr[0] &= 0x0f;
		pixelPtr[0] |= TUint8(upper << 4);

		pixelPtr++;
		aMaskBuffer++;
		}

	while (aMaskBuffer < maskBufferPtrLimit)
		{
		const TInt lower = ((gray * aMaskBuffer[0]) + ((255 - aMaskBuffer[0]) * (pixelPtr[0] & 0x0f) * 17)) / (255 * 17);
		const TInt upper = ((gray * aMaskBuffer[1]) + ((255 - aMaskBuffer[1]) * (pixelPtr[0] >> 4) * 17)) / (255 * 17);
		pixelPtr[0] = TUint8(lower);
		pixelPtr[0] |= TUint8(upper << 4);

		pixelPtr++;
		aMaskBuffer += 2;
		}

	if (oddEndCoord)
		{
		const TInt lower = ((gray * aMaskBuffer[0]) + ((255 - aMaskBuffer[0]) * (pixelPtr[0] & 0x0f) * 17)) / (255 * 17);
		pixelPtr[0] &= 0xf0;
		pixelPtr[0] |= TUint8(lower);
		}
	}

void CDrawFourBppBitmapGray::MapColorToUserDisplayMode(TRgb& aColor)
	{
	switch (iUserDispMode)
		{
	case EGray2:
		aColor = TRgb::_Gray2(aColor._Gray2());
		break;
	case EColor16:
		aColor = TRgb::_Gray4(aColor._Gray4());
		break;
	default:
		break;
		}
	}

void CDrawFourBppBitmapGray::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint8* bufferPtr = (TUint8*)aBuffer;
	const TUint8* bufferLimit = bufferPtr + ((aLength + 1) / 2);

	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			TUint8 value = TUint8(*bufferPtr & 0x88);
			value |= value >> 1;
			*bufferPtr++ = TUint8(value | (value >> 2));
			}
		break;
	case EColor16:
		while (bufferPtr < bufferLimit)
			{
			TUint8 value = TUint8(*bufferPtr & 0xcc);
			*bufferPtr++ = TUint8(value | (value >> 2));
			}
		break;
	default:
		break;
		}
	}

TInt CDrawFourBppBitmapGray::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
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
			TRgb backgroundColor = TRgb::_Gray16(pixelPtr[0] >> 4);
			finalColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
										redOutlinePenColor, redShadowColor, redFillColor,
										greenOutlinePenColor, greenShadowColor, greenFillColor,
										blueOutlinePenColor, blueShadowColor, blueFillColor,
										backgroundColor, index);
			pixelPtr[0] &= 0x0f;
			pixelPtr[0] |= TUint8(finalColor._Gray16() << 4);
			}
			
		pixelPtr++;
		aDataBuffer++;
		}

	while (aDataBuffer < dataBufferPtrLimit)
		{
		index = aDataBuffer[0];
		TRgb upperPixelBackgroundColor = TRgb::_Gray16(pixelPtr[0] >> 4);
		if (255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			TRgb lowerPixelBackgroundColor = TRgb::_Gray16(pixelPtr[0] & 0x0f);
			lowerPixelColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
											redOutlinePenColor, redShadowColor, redFillColor,
											greenOutlinePenColor, greenShadowColor, greenFillColor,
											blueOutlinePenColor, blueShadowColor, blueFillColor,
											lowerPixelBackgroundColor, index);
			pixelPtr[0] = TUint8(lowerPixelColor._Gray16());
			}
		
		index = aDataBuffer[1];
		if (255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			upperPixelColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
											redOutlinePenColor, redShadowColor, redFillColor,
											greenOutlinePenColor, greenShadowColor, greenFillColor,
											blueOutlinePenColor, blueShadowColor, blueFillColor,
											upperPixelBackgroundColor, index);
			pixelPtr[0] |= TUint8(upperPixelColor._Gray16() << 4);
			}
		pixelPtr++;
		aDataBuffer += 2;
		}

	if (oddEndCoord)
		{
		index = aDataBuffer[0];
		if (255 != FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			TRgb backgroundColor = TRgb::_Gray16(pixelPtr[0] & 0x0f);
			finalColor = BlendFourColors(aOutlinePenColor, aShadowColor, aFillColor,
										redOutlinePenColor, redShadowColor, redFillColor,
										greenOutlinePenColor, greenShadowColor, greenFillColor,
										blueOutlinePenColor, blueShadowColor, blueFillColor,
										backgroundColor, index);
			pixelPtr[0] &= 0xf0;
			pixelPtr[0] |= TUint8(finalColor._Gray16());
			}
		}
	return KErrNone;
	}

TRgb CDrawFourBppBitmapGray::BlendFourColors(TUint32 aOutlinePenColor, TUint32 aShadowColor, TUint32 aFillColor,
											TInt aRedOutlinePenColor, TInt aRedShadowColor, TInt aRedFillColor,
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
