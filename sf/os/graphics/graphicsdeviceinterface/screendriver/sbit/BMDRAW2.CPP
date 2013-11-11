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

GLREF_D const TUint32 wordlutab[256];

const TInt KBitsPerPixel = 2;
const TInt KPixelsPerByte = 4;
const TInt KPixelsPerWord = 16;

//Initializes iSize, iDrawRect, iLongWidth, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawTwoBppBitmap::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = (iSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1);
	iScanLineWords = iLongWidth / KPixelsPerWord;
	}
 
TInt CDrawTwoBppBitmap::Construct(TSize aSize)
	{
	return Construct(aSize, ((aSize.iWidth + (KPixelsPerWord - 1)) & ~(KPixelsPerWord - 1)) / KPixelsPerByte);
	}

TInt CDrawTwoBppBitmap::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	iDispMode = EGray4;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride & 3)
		return KErrArgument;
	iLongWidth = aStride * KPixelsPerByte;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = aStride >> 2;
	TInt size = 1 + (Max(aSize.iWidth,aSize.iHeight) >> 2);
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if (iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

void CDrawTwoBppBitmap::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		aColor = TRgb::_Gray256(FadeGray(aColor._Gray16() * 17));

	if (iShadowMode & EShadow)
		{
		TInt gray16 = aColor._Gray16();
		gray16 = Max(gray16 - 5,0);
		aColor = TRgb::_Gray16(gray16);
		}
	}

TUint32 CDrawTwoBppBitmap::ColorInt(TRgb aColor) const
	{
	TUint32 colorWord = aColor._Gray4();

	colorWord |= colorWord << 2;
	colorWord |= colorWord << 4;
	colorWord |= colorWord << 8;
	colorWord |= colorWord << 16;

	return colorWord;
	}

void CDrawTwoBppBitmap::HashInt(TUint32& aInt1,TUint32& aInt2,TRgb aColor,TInt aX,TInt aY) const
	{
	TUint32 int1 = Hash(aColor._Gray16(),aX,aY);
	TUint32 int2 = Hash(aColor._Gray16(),aX + 1,aY);
	aInt1 = (int1 >> 2) | (int2 & 0xc);

	aInt1 |= aInt1 << 4;
	aInt1 |= aInt1 << 8;
	aInt1 |= aInt1 << 16;

	int1 = Hash(aColor._Gray16(),aX,aY + 1);
	int2 = Hash(aColor._Gray16(),aX + 1,aY + 1);
	aInt2 = (int1 >> 2) | (int2 & 0xc);

	aInt2 |= aInt2 << 4;
	aInt2 |= aInt2 << 8;
	aInt2 |= aInt2 << 16;
	}

void CDrawTwoBppBitmap::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* const limit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	while (aBuffer < limit)
		*aBuffer++ ^= 0xffffffff;
	}

void CDrawTwoBppBitmap::MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs,TBool aMapForwards)
	{
	__ASSERT_DEBUG(aRect.iTl.iX>=0 && aRect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aRect.iTl.iY>=0 && aRect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aColors,Panic(EScreenDriverPanicNullPointer));
	__ASSERT_DEBUG(aNumPairs>0,Panic(EScreenDriverPanicZeroLength));

	const TInt offset = aMapForwards ? 0 : 1;
	TUint32* colorMap;
	TUint32 evenColorMap[4];
	TUint32 oddColorMap[4];

	for (TInt colorIndex = 0; colorIndex <= 3; colorIndex++)
		{
		evenColorMap[colorIndex] = ColorInt(TRgb::_Gray4(colorIndex));
		oddColorMap[colorIndex] = evenColorMap[colorIndex];
		}

	for (TInt index = 0; index < 4; index++)
		{
		for (TInt paircount = 0; paircount < aNumPairs; paircount++)
			{
			if (aColors[(paircount * 2) + offset]._Gray4() == index)
				{
				HashInt(evenColorMap[index],oddColorMap[index],aColors[(paircount * 2) + 1 - offset],0,0);
				break;
				}
			}
		}

	TInt x = aRect.iTl.iX;
	TInt y = aRect.iTl.iY;
	TInt startLong = (aRect.iTl.iX + KPixelsPerWord - 1) & (~0xf);
	TInt finishLong = aRect.iBr.iX & (~0xf);
	TInt startShift = (startLong - x) * KBitsPerPixel;
	TInt finishShift = (KPixelsPerWord - aRect.iBr.iX + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(y);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (y & 1)
		colorMap = oddColorMap;
	else
		colorMap = evenColorMap;

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
		const TUint32 invertedMask = ~mask;

		for (; y < aRect.iBr.iY; y++)
			{
			TUint32 newcolor = MapInt(pixelPtrLimit[0],colorMap) & mask;

			pixelPtrLimit[0] &= invertedMask;
			pixelPtrLimit[0] |= newcolor;
			pixelPtrLimit += iScanLineWords;

			if (colorMap == oddColorMap)
				colorMap = evenColorMap;
			else
				colorMap = oddColorMap;
			}

		return;
		}

	for (; y < aRect.iBr.iY; y++)
		{
		if (x < startLong)
			{
			TUint32 newcolor = MapInt(pixelPtr[-1],colorMap);
			pixelPtr[-1] = PasteInt(pixelPtr[-1],newcolor,startShift);
			}

		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] = MapInt(tempPixelPtr[0],colorMap);

		if (finishLong < aRect.iBr.iX)
			{
			TUint32 newcolor = MapInt(pixelPtrLimit[0],colorMap);
			pixelPtrLimit[0] = PasteInt(newcolor,pixelPtrLimit[0],finishShift);
			}

		base += iScanLineWords;
		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;

		if (colorMap == oddColorMap)
			colorMap = evenColorMap;
		else
			colorMap = oddColorMap;
		}
	}

TUint32 CDrawTwoBppBitmap::MapInt(TUint32 aInt,TUint32* aColorMap) const
	{
	TUint32 mask = 3;
	TUint32 newInt = 0;

	while (mask)
		{
		newInt |= mask & (aColorMap[aInt & 3]);
		aInt >>= 2;
		mask <<= 2;
		}

	return newInt;
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
void CDrawTwoBppBitmap::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
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


TRgb CDrawTwoBppBitmap::ReadRgbNormal(TInt aX,TInt aY) const
	{
	TUint32 colorWord = *(ScanLine(aY) + (aX / KPixelsPerWord));
	colorWord >>= ((aX & 0xf) * KBitsPerPixel);
	return TRgb::_Gray4(colorWord & 3);
	}

TUint32 CDrawTwoBppBitmap::ShadowWord(TUint32 aWord)
	{
	TUint32 decrement = (0xaaaaaaaa & aWord) >> 1;
	decrement |= (0x55555555 & aWord);  // decrement is OR of each pair of bits
	return aWord - decrement; // Only shadow pixels greater than zero (decrement == 1)
	}

TUint32 CDrawTwoBppBitmap::FadeWord(TUint32 aWord)
	{
	const TInt fadeArray[4] = { FadeGray(0) >> 6, FadeGray(85) >> 6, FadeGray(170) >> 6, FadeGray(255) >> 6 };

	TUint32 fadedWord = 0;

	for (TInt bitShift = 0; bitShift < 32; bitShift += 2)
		fadedWord |= fadeArray[(aWord >> bitShift) & 3] << bitShift;

	return fadedWord;
	}

void CDrawTwoBppBitmap::ShadowArea(const TRect& aRect)
	{
	__ASSERT_DEBUG(aRect.iTl.iX>=0 && aRect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aRect.iTl.iY>=0 && aRect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	const TInt startLong = (aRect.iTl.iX + KPixelsPerWord - 1) & ~0xf;
	const TInt finishLong = aRect.iBr.iX & ~0xf;
	const TInt startShift = (startLong - aRect.iTl.iX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aRect.iBr.iX + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aRect.iTl.iY);

	if (iShadowMode & EFade)
		{
		if (finishLong < startLong)
			{
			TUint32* pixelPtr = base + (finishLong / KPixelsPerWord);
			const TUint32* const pixelPtrRowLimit = pixelPtr + (aRect.Height() * iScanLineWords);

			const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
			const TUint32 inverseMask = ~mask;

			while (pixelPtr < pixelPtrRowLimit)
				{
				TUint32 shadowed = FadeWord(pixelPtr[0]) & mask;

				pixelPtr[0] &= inverseMask;
				pixelPtr[0] |= shadowed;

				pixelPtr += iScanLineWords;
				}
			}
		else
			{
			TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
			TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);
			const TUint32* const pixelPtrRowLimit = pixelPtr + (aRect.Height() * iScanLineWords);

			while (pixelPtr < pixelPtrRowLimit)
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
		if (finishLong < startLong)
			{
			TUint32* pixelPtr = base + (finishLong / KPixelsPerWord);
			const TUint32* const pixelPtrRowLimit = pixelPtr + (aRect.Height() * iScanLineWords);

			const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
			const TUint32 inverseMask = ~mask;

			while (pixelPtr < pixelPtrRowLimit)
				{
				TUint32 shadowed = ShadowWord(pixelPtr[0]) & mask;

				pixelPtr[0] &= inverseMask;
				pixelPtr[0] |= shadowed;

				pixelPtr += iScanLineWords;
				}
			}
		else
			{
			TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
			TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);
			const TUint32* const pixelPtrRowLimit = pixelPtr + (aRect.Height() * iScanLineWords);

			while (pixelPtr < pixelPtrRowLimit)
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

void CDrawTwoBppBitmap::ShadeBuffer(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* limit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	while (aBuffer < limit)
		{
		TUint32 secondbit = (0xaaaaaaaa & aBuffer[0]);
		*aBuffer++ = secondbit | (secondbit >> 1);
		}
	}

void CDrawTwoBppBitmap::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aBuffer != NULL,Panic(EScreenDriverPanicInvalidParameter));

	const TUint32* limit = aBuffer + ((aLength + KPixelsPerWord - 1) / KPixelsPerWord);

	if (iShadowMode & EFade)
		{
		for (TUint32* buffer = aBuffer; buffer < limit; buffer++)
			buffer[0] = FadeWord(buffer[0]);
		}

	if (iShadowMode & EShadow)
		{
		for (TUint32* buffer = aBuffer; buffer < limit; buffer++)
			buffer[0] = ShadowWord(buffer[0]);
		}
	}

void CDrawTwoBppBitmap::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	TInt col=0;

	if (iUserDispMode != EGray2)
		col = Hash(aColor._Gray16(),aX,aY) >> 2;
	else
		{
		if (aColor._Gray2())
			col = 3;
		}

	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TInt shift = (aX & 0xf) * KBitsPerPixel;
	pixelPtr[0] &= ~(3 << shift);
	pixelPtr[0] |= col << shift;
	}

void CDrawTwoBppBitmap::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TBool monoText = (aColor == KRgbBlack);

	const TInt previousLong = aX & (~0xf);
	const TInt bitShift = (aX - previousLong) * KBitsPerPixel;
	const TBool secondwordenable = (aX + aLength) > (previousLong + KPixelsPerWord);
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TInt mask = 0xffffffff >> (32 - aLength);
	TUint32* datalimit = aData + aHeight;

	if (monoText && !secondwordenable)
		{
		while (aData < datalimit)
			{
			TUint32 dataitem = *aData++;
			dataitem &= mask;
			TUint32 firsthalf = wordlutab[dataitem & 0xff];
			firsthalf |= wordlutab[(dataitem >> 8) & 0xff] << 16;
			*pixelPtr &= ~(firsthalf << bitShift);
			pixelPtr += iScanLineWords;
			}
		return;
		}

	const TInt reverseshift = 32 - bitShift;
	const TBool thirdwordenable = (aX + aLength) > (previousLong + 32);

	if (monoText)
		{
		while (aData < datalimit)
			{
			TUint32 dataitem = *aData++;
			dataitem &= mask;
			TUint32 firsthalf = wordlutab[dataitem & 0xff];
			dataitem >>= 8;
			firsthalf |= wordlutab[dataitem & 0xff] << 16;
			dataitem >>= 8;
			TUint32 secondhalf = wordlutab[dataitem & 0xff];
			dataitem >>= 8;
			secondhalf |= wordlutab[dataitem & 0xff] << 16;
			const TUint32 firstword = firsthalf << bitShift;
			TUint32 secondword = secondhalf << bitShift;
			TUint32 thirdword = 0;

			if (bitShift)
				{
				secondword |= firsthalf >> reverseshift;
				thirdword = secondhalf >> reverseshift;
				}

			pixelPtr[0] &= ~firstword;
			pixelPtr[1] &= ~secondword;
			pixelPtr[2] &= ~thirdword;
			pixelPtr += iScanLineWords;
			}
		}
	else
		{
		TUint32 colorWord1,colorWord2;
		HashInt(colorWord1,colorWord2,aColor,0,aY);
		TUint32 colorWord = colorWord1;

		while (aData < datalimit)
			{
			TUint32 dataitem = *aData++;
			dataitem &= mask;
			TUint32 firsthalf = wordlutab[dataitem & 0xff];
			dataitem >>= 8;
			firsthalf |= wordlutab[dataitem & 0xff] << 16;
			dataitem >>= 8;
			TUint32 secondhalf = wordlutab[dataitem & 0xff];
			dataitem >>= 8;
			secondhalf |= wordlutab[dataitem & 0xff] << 16;
			TUint32 firstword = firsthalf << bitShift;
			TUint32 secondword = secondhalf << bitShift;
			TUint32 thirdword = 0;

			if (bitShift)
				{
				secondword |= firsthalf >> reverseshift;
				thirdword = secondhalf >> reverseshift;
				}

			pixelPtr[0] &= ~firstword;
			if (secondwordenable)
				pixelPtr[1] &= ~secondword;
			if (thirdwordenable)
				pixelPtr[2] &= ~thirdword;

			if (colorWord)
				{
				pixelPtr[0] |= firstword & colorWord;
				if (secondwordenable)
					pixelPtr[1] |= secondword & colorWord;
				if (thirdwordenable)
					pixelPtr[2] |= thirdword & colorWord;
				}

			if (colorWord == colorWord1)
				colorWord = colorWord2;
			else
				colorWord = colorWord1;

			pixelPtr += iScanLineWords;
			}
		}
	}

void CDrawTwoBppBitmap::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	TUint32 colorWord1,colorWord2;
	HashInt(colorWord1,colorWord2,aColor,0,aY);
	TUint32 colorWord = colorWord1;
	TUint32* pixelPtr = ScanLine(aY) + (aX / KPixelsPerWord);
	const TUint32* pixelPtrLimit = ScanLine(aY + aHeight) + (aX / KPixelsPerWord);
	const TUint32 initialMask = (3 << ((aX & 0xf) * KBitsPerPixel));

	if (colorWord1 || colorWord2)
		{
		while (pixelPtr < pixelPtrLimit)
			{
			TUint32 dataMask = 1;
			TUint32 mask = initialMask;
			TUint32* tempPixelPtr = pixelPtr;

			for (TInt count = 0; count < aLength; count++,dataMask <<= 1,mask <<= 2)
				{
				if (!mask)
					{
					mask = 3;
					tempPixelPtr++;
					}

				if (aData[0] & dataMask)
					{
					if (aDrawMode == CGraphicsContext::EDrawModeXOR)
						tempPixelPtr[0] ^= mask & colorWord;
					else if (aDrawMode == CGraphicsContext::EDrawModeAND)
						tempPixelPtr[0] &= (mask & colorWord) | ~mask;
					else if (aDrawMode == CGraphicsContext::EDrawModeOR)
						tempPixelPtr[0] |= mask & colorWord;
					}
				}

			aData++;
			pixelPtr += iScanLineWords;
			colorWord = (colorWord == colorWord1) ? colorWord2 : colorWord1;
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

				for (TInt count = 0; count < aLength; count++,dataMask <<= 1,mask <<= 2)
					{
					if (!mask)
						{
						mask = 3;
						tempPixelPtr++;
						}

					if (aData[0] & dataMask)
						tempPixelPtr[0] &= ~mask;
					}

				aData++;
				pixelPtr += iScanLineWords;
				}
			}
		}
	}

void CDrawTwoBppBitmap::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aHeight,TRgb aColor,TBool aUp)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	TUint32 col1 = Hash(aColor._Gray16(),aX,aY) / 4;
	TUint32 col2 = Hash(aColor._Gray16(),aX,aY + 1) / 4;
	const TInt yLimit = aY + (aUp ? -aHeight : aHeight);
	const TInt scanlinediff = aUp ? -iScanLineWords : iScanLineWords;
	const TInt startWord = aX / KPixelsPerWord;
	const TInt startShift = (aX & 0xf) * KBitsPerPixel;
	TUint32* pixelPtr = ScanLine(aY) + startWord;
	TUint32* pixelPtrLimit = ScanLine(yLimit) + startWord;
	const TUint32 mask = ~(3 << startShift);
	TUint32 dataMask = 1;

	if (col1 || col2)
		{
		col1 <<= startShift;
		col2 <<= startShift;
		TUint32 col = col1;

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
				pixelPtr[0] |= col;
				}

			dataMask <<= 1;
			pixelPtr += scanlinediff;
			col = (col == col2) ? col1 : col2;
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
			pixelPtr += scanlinediff;
			}
		}
	}

void CDrawTwoBppBitmap::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,(TUint32*)aBuffer);

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	TUint32* base = ScanLine(aY);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
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
		Mem::Copy(pixelPtr,aBuffer,wordsToCopy * sizeof(TUint32));
		aBuffer += wordsToCopy;

		if (finishLong < aX + aLength)
			pixelPtrLimit[0] = PasteInt(aBuffer[0],pixelPtrLimit[0],finishShift);
		}
	}

void CDrawTwoBppBitmap::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,(TUint32*)aBuffer);

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	TUint32* base = ScanLine(aY);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
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

void CDrawTwoBppBitmap::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,(TUint32*)aBuffer);

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	TUint32* base = ScanLine(aY);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
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

void CDrawTwoBppBitmap::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		ShadeBuffer(aLength,(TUint32*)aBuffer);

	const TInt startLong = (aX + KPixelsPerWord - 1) & (~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	TUint32* base = ScanLine(aY);
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt startShiftExtra = 32 - startShift;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
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
void CDrawTwoBppBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                          const TUint8* aRgbBuffer,
                                          const TUint8* aMaskBuffer,
                                          MAlphaBlend::TShadowing aShadowing,
                                          CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 4);
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	TInt bitOffset = (aX & 3) * KBitsPerPixel;
	TRgb pixelClr;

	while (aMaskBuffer < maskBufferPtrLimit)
		{
        TRgb srcColor(aRgbBuffer[2],aRgbBuffer[1],aRgbBuffer[0]);
        if(aShadowing == MAlphaBlend::EShdwBefore)
            {
		    Shadow(srcColor);
            }
		TInt pixelValue = ((pixelPtr[0] >> bitOffset) & 0x03) * (255 - aMaskBuffer[0]);
		const TInt srceValue = (((srcColor.Red() << 1) + 
                                  srcColor.Green() + (srcColor.Green() << 2) + 
                                  srcColor.Blue()) >> 9) * aMaskBuffer[0];

		pixelValue += srceValue;
		pixelValue /= 255;

		pixelClr = TRgb::_Gray4(pixelValue);
        if(aShadowing == MAlphaBlend::EShdwAfter)
            {
		    Shadow(pixelClr);
            }
		MapColorToUserDisplayMode(pixelClr);

		pixelPtr[0] &= ~(3 << bitOffset);
		pixelPtr[0] |= TUint8(pixelClr._Gray4() << bitOffset);

		bitOffset += 2;
		if (bitOffset == 8)
			{
			bitOffset = 0;
			pixelPtr++;
			}

		aRgbBuffer += 4;
		aMaskBuffer++;
		}
	}

void CDrawTwoBppBitmap::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1)&(~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord1,colorWord2;

	if (aColor._Gray16() % 5 != 0)
		HashInt(colorWord1,colorWord2,aColor,startLong,aY);
	else
		colorWord1 = colorWord2 = ColorInt(aColor);

	TUint32 colorWord = colorWord1;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

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

void CDrawTwoBppBitmap::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1)&(~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord1,colorWord2;

	if (aColor._Gray16() % 5 != 0)
		HashInt(colorWord1,colorWord2,aColor,startLong,aY);
	else
		colorWord1 = colorWord2 = ColorInt(aColor);

	TUint32 colorWord = colorWord1;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

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

void CDrawTwoBppBitmap::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1)&(~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord1,colorWord2;

	if (aColor._Gray16() % 5 != 0)
		HashInt(colorWord1,colorWord2,aColor,startLong,aY);
	else
		colorWord1 = colorWord2 = ColorInt(aColor);

	TUint32 colorWord = colorWord1;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

	if (finishLong < startLong)
		{
		const TUint32 mask = (0xffffffff >> finishShift) & (0xffffffff << (32 - startShift));
		const TUint32 invertedMask = ~mask;
		colorWord &= mask;
		colorWord1 &= mask;
		colorWord2 &= mask;

		for (; aY < yLimit; aY++)
			{
			pixelPtrLimit[0] &= colorWord | invertedMask;

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

void CDrawTwoBppBitmap::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());

	const TInt startLong = (aX + KPixelsPerWord - 1)&(~0xf);
	const TInt finishLong = (aX + aLength) & (~0xf);
	const TInt yLimit = aY + aHeight;
	TUint32 colorWord1,colorWord2;

	if (aColor._Gray16() % 5 != 0)
		HashInt(colorWord1,colorWord2,aColor,startLong,aY);
	else
		colorWord1 = colorWord2 = ColorInt(aColor);

	TUint32 colorWord = colorWord1;
	const TInt startShift = (startLong - aX) * KBitsPerPixel;
	const TInt finishShift = (KPixelsPerWord - aX - aLength + finishLong) * KBitsPerPixel;
	TUint32* base = ScanLine(aY);
	TUint32* pixelPtr = base + (startLong / KPixelsPerWord);
	TUint32* pixelPtrLimit = base + (finishLong / KPixelsPerWord);

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

void CDrawTwoBppBitmap::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 4);
	TInt pixelOffset = (aX & 3) * 2;
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

	if (iShadowMode)
		Shadow(aColor);

	const TInt gray = aColor._Gray256();
	TRgb pixelColor;
	
	while (aMaskBuffer < maskBufferPtrLimit)
		{
		const TInt pixelGray256Value = ((pixelPtr[0] >> pixelOffset) & 3) * 85;
		pixelColor = TRgb::_Gray256(((gray * aMaskBuffer[0]) + ((255 - aMaskBuffer[0]) * pixelGray256Value)) / 255);
		pixelPtr[0] &= ~(3 << pixelOffset);
		pixelPtr[0] |= pixelColor._Gray4() << pixelOffset;

		pixelOffset += 2;
		if (pixelOffset >= 8)
			{
			pixelPtr++;
			pixelOffset = 0;
			}
		aMaskBuffer++;
		}
	}

void CDrawTwoBppBitmap::MapColorToUserDisplayMode(TRgb& aColor)
	{
	if (iUserDispMode == EGray2)
		aColor = TRgb::_Gray2(aColor._Gray2());
	}

void CDrawTwoBppBitmap::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	if (iUserDispMode == EGray2)
		{
		TUint8* bufferPtr = (TUint8*)aBuffer;
		const TUint8* bufferLimit = bufferPtr + ((aLength + 3) / 4);

		while (bufferPtr < bufferLimit)
			{
			TUint8 value = TUint8(*bufferPtr & 0xaa);
			*bufferPtr++ = TUint8(value | (value >> 1));
			}
		}
	}

TInt CDrawTwoBppBitmap::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
												TUint32 aOutlinePenColor, TUint32 aShadowColor,
												TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	//This is non-optimised since this screen mode is rarely used and is usually 
	//fast enough without optimisation.
	DeOrientate(aX,aY);
	TUint8* pixelPtr = REINTERPRET_CAST(TUint8*,ScanLine(aY)) + (aX / 4);
	TInt pixelOffset = (aX & 3) * 2;
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
			pixelOffset += 2;
			if (pixelOffset >= 8)
				{
				pixelPtr++;
				pixelOffset = 0;
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
			TRgb backgroundColor = TRgb::_Gray4((pixelPtr[0] >> pixelOffset) & 3);
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
		
		//Clear the relevant bits.
		pixelPtr[0] &= ~(3 << pixelOffset);
		pixelPtr[0] |= (finalColor._Gray4() << pixelOffset);
		pixelOffset += 2;
		
		if (pixelOffset >= 8)
			{
			pixelPtr++;
			pixelOffset = 0;
			}
		}
	return KErrNone;
	}
