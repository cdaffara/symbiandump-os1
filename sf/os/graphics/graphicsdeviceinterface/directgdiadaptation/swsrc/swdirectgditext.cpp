// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "swdirectgdiengine.h"
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>

/**
@see MDirectGdiEngine::SetFont()
*/
void CSwDirectGdiEngine::SetFont(TUint32 /*aFontId*/)
	{
	// Do nothing.
	// The SW adapter doesn't need the font ID to index a separate font cache because it uses the standard one.
	}


/**
@see MDirectGdiEngine::ResetFont()
*/
void CSwDirectGdiEngine::ResetFont()
	{
	}


/**
@see MDirectGdiEngine::SetTextShadowColor()
*/
void CSwDirectGdiEngine::SetTextShadowColor(const TRgb& aColor)
	{	
	iTextShadowColor = aColor;
	}

/**
@see MDirectGdiEngine::BeginDrawGlyph()
*/
void CSwDirectGdiEngine::BeginDrawGlyph()
	{
	}

/**
@see MDirectGdiEngine::DrawGlyph()
@panic DGDIAdapter 56, if an invalid glyph bitmap type is passed in.
*/
void CSwDirectGdiEngine::DrawGlyph(const TPoint& aScrPos, const TChar /*aChar*/, const TUint8* aGlyphImage,
								   const TGlyphBitmapType aGlyphBitmapType, const TSize& aGlyphImageSize,
								   const TRect& aScrClipRect, const DirectGdi::TGraphicsRotation aRotation)
	{
	TPoint pos=aScrPos;
	pos+=iDrawOrigin;
	TRect clipRect=aScrClipRect;
	clipRect.iTl+=iDrawOrigin;
	clipRect.iBr+=iDrawOrigin;
	TRect regionRect(0, 0, 0, 0);
	TInt numRects = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < numRects; count++)
		{
		// Do the clip rects intersect?
		regionRect = (*iDefaultRegionPtr)[count];
		if (!regionRect.Intersects(clipRect))
			{
			// Nothing to draw
			continue;
			}
		// Clip to intersection of two clip rects
		regionRect.Intersection(clipRect);
		
		if (aRotation == DirectGdi::EGraphicsRotationNone)	// Horizontal text
			{
			// Do the glyph and the clip rect intersect?
			TRect glyphRect(pos, aGlyphImageSize);
			if (!regionRect.Intersects(glyphRect))
				{
				// Nothing to draw
				continue;
				}
			// Clip to intersection with glyph bitmap 
			regionRect.Intersection(glyphRect);
			
			switch (aGlyphBitmapType)
				{
				case EMonochromeGlyphBitmap:
					{
					DrawBitmapGlyph(pos, aGlyphImage, aGlyphImageSize, regionRect);
					break;
					}
				case EAntiAliasedGlyphBitmap:
					{
					DrawAntiAliasedGlyph(pos, aGlyphImage, aGlyphImageSize, regionRect);
					break;
					}
				case EFourColourBlendGlyphBitmap:
					{
					DrawFourColourGlyph(pos, aGlyphImage, aGlyphImageSize, regionRect);
					break;
					}
				default:
					GRAPHICS_PANIC_ALWAYS(EDirectGdiPanicInvalidGlyphBitmapType);
				}
			}
		else	// Vertical text
			{
			/*
			// Do the glyph and the clip rect intersect?
			TRect glyphRect(aPos, aGlyphImageSize);
			if (!regionRect.Intersects(glyphRect))
				{
				// Nothing to draw
				continue;
				}
			// Clip to intersection with glyph bitmap 
			regionRect.Intersection(glyphRect);
			*/
			
			switch (aGlyphBitmapType)
				{
				case EMonochromeGlyphBitmap:
					{
					DrawRotatedBitmapGlyph(pos, aGlyphImage, aGlyphImageSize, regionRect, aRotation);
					break;
					}
				case EAntiAliasedGlyphBitmap:
					{
					DrawRotatedAntiAliasedGlyph(pos, aGlyphImage, aGlyphImageSize, regionRect, aRotation);
					break;
					}
				case EFourColourBlendGlyphBitmap:
					{
					DrawRotatedFourColourGlyph(pos, aGlyphImage, aGlyphImageSize, regionRect, aRotation);
					break;
					}
				default:
					GRAPHICS_PANIC_ALWAYS(EDirectGdiPanicInvalidGlyphBitmapType);
				}
			}
		// Now display it
		iDrawDevice->UpdateRegion(regionRect);
		}
	}

/**
@see MDirectGdiEngine::EndDrawGlyph()
*/
void CSwDirectGdiEngine::EndDrawGlyph()
	{
	}

/**
Draw a bitmap glyph.
	
@param	aPos			Position to start drawing gyph.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
*/
void CSwDirectGdiEngine::DrawBitmapGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
										 const TSize& aGlyphImageSize, const TRect& aClipRect)
	{
	// aChar parameter not needed because SW implementation uses the default glyph cache
	// therefore does not need aChar to index its own local cache
	
	/*
	Divert if the character is large.
	Large is defined as wider than 30 bits (a scan line won't fit in a TInt32)
	or greater than 32 bits high (because that's the current array size - could be changed).
	*/
	TInt dataHeight = aGlyphImageSize.iHeight;
	TInt dataLength = aGlyphImageSize.iWidth;
	if (dataLength > 30 || dataHeight > 32)
		{
		DrawLargeBitmapGlyph(aPos, aGlyphImage, aGlyphImageSize, aClipRect);
		return;
		}
	
	TInt bitIndex = 0;
	TInt16 repeatCount = 0;
	TUint32 binaryData[32];
	TUint32* binaryDataPtr = binaryData;
	TUint32* binaryDataPtrLimit;
	for (TInt glyphLine = 0; glyphLine < dataHeight; glyphLine += repeatCount) // for lines in the glyph bitmap
		{
		repeatCount = Load16(aGlyphImage + (bitIndex >> 3));
		repeatCount >>= bitIndex & 7;
		TInt multiLineFlag = repeatCount & 1;
		repeatCount >>= 1;
		repeatCount &= 0xf;
		bitIndex += 5;
		binaryDataPtrLimit = binaryData + glyphLine + repeatCount;
		if (multiLineFlag)
			{
			while (binaryDataPtr < binaryDataPtrLimit)
				{
				TInt glyphDataOffsetPtr = TInt(aGlyphImage) + (bitIndex >> 3);
				TUint32* glyphDataWord = (TUint32*)(glyphDataOffsetPtr & ~3);
				TInt bitShift = bitIndex & 7;
				bitShift += (glyphDataOffsetPtr & 3) << 3;
				*binaryDataPtr = (*glyphDataWord++) >> bitShift;
				if (bitShift)
					{
					*binaryDataPtr |= (*glyphDataWord << (32 - bitShift));
					}
				bitIndex += dataLength;
				binaryDataPtr++;
				}
			}
		else
			{
			TInt glyphDataOffsetPtr = TInt(aGlyphImage) + (bitIndex >> 3);
			TUint32* glyphDataWord = (TUint32*)(glyphDataOffsetPtr & ~3);
			TInt bitShift = bitIndex & 7;
			bitShift += (glyphDataOffsetPtr & 3) << 3;
			TUint32 data = (*glyphDataWord++) >> bitShift;
			if (bitShift)
				{
				data |= (*glyphDataWord << (32 - bitShift));
				}
			while (binaryDataPtr < binaryDataPtrLimit)
				{
				*binaryDataPtr++ = data;
				}
			bitIndex += dataLength;
			}
		}
	TPoint topLeft(aPos);
	binaryDataPtr = ClipBinaryArray(binaryData, binaryData + dataHeight, dataLength, dataHeight, topLeft, aClipRect);
	if ((dataLength > 0) && (dataHeight > 0))
		{
		iDrawDevice->WriteBinary(topLeft.iX, topLeft.iY, binaryDataPtr, dataLength, dataHeight, iPenColor, GcDrawMode(iDrawMode) );
		}
	}
	
	
/**
Draw a large bitmap glyph.
	
@param	aPos			Position to start drawing gyph.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
*/
void CSwDirectGdiEngine::DrawLargeBitmapGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
											 const TSize& aGlyphImageSize, const TRect& aClipRect)
	{
	TPoint printPos(aPos);
	const TInt dataLength = aGlyphImageSize.iWidth;
	const TInt dataHeight = aGlyphImageSize.iHeight;
	TInt bitIndex = 0;
	TInt16 repeatCount = 0;
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	const TInt scanLineWords = (iDrawDevice->ScanLineBytes()) << 3;
	for (TInt glyphLine = 0; glyphLine < dataHeight; glyphLine += repeatCount) // for lines in the glyph bitmap
		{
		repeatCount = Load16(aGlyphImage + (bitIndex >> 3));
		repeatCount >>= bitIndex & 7;
		const TInt multiLineFlag = repeatCount & 1;
		repeatCount >>= 1;
		repeatCount &= 0xf;
		bitIndex += 5;
		if (multiLineFlag)
			{
			for (TInt currentLine = 0; currentLine < repeatCount; currentLine++)
				{
				CopyCharLine(scanLineBuffer, scanLineWords, aGlyphImage + (bitIndex >> 3), bitIndex & 7, dataLength);
				OutputCharLineMultiplied(printPos, scanLineBuffer, dataLength, 1, aClipRect);
				bitIndex += dataLength;
				printPos.iY++;
				}
			}
		else
			{
			CopyCharLine(scanLineBuffer, scanLineWords, aGlyphImage + (bitIndex >> 3), bitIndex & 7, dataLength);
			OutputCharLineMultiplied(printPos, scanLineBuffer, dataLength, repeatCount, aClipRect);
			printPos.iY += repeatCount;
			bitIndex += dataLength;
			}
		}
	}


/**

*/
void CSwDirectGdiEngine::CopyCharLine(TUint32* aBinaryDataPtr, TInt aBufferWords, const TUint8* aData, TInt aBitShift, TInt aCharWidth)
	{
	aBitShift &= 7;
	TInt wordsToCopy = (aCharWidth + 31) >> 5;
	if (wordsToCopy > aBufferWords)
		{
		wordsToCopy = aBufferWords;
		}
	TUint32* ptrLimit = aBinaryDataPtr + wordsToCopy;
	TUint32* dataWord = (TUint32*)(TInt(aData) & ~3);
	aBitShift += (TInt(aData) - TInt(dataWord)) << 3;
	while (aBinaryDataPtr < ptrLimit)
		{
		*aBinaryDataPtr = *dataWord++;
		*aBinaryDataPtr >>= aBitShift;
		if (aBitShift)
			{
			*aBinaryDataPtr |= (*dataWord << (32 - aBitShift));
			}
		aBinaryDataPtr++;
		}
	}


/**

*/
void CSwDirectGdiEngine::OutputCharLineMultiplied(TPoint aPrintPos, TUint32* aBuffer, TInt aDataLength, TInt aNum, const TRect& aClipRect)
	{
	if (aDataLength <= 0)
		{
		return;
		}
	TInt bufferWords = (aDataLength + 31) >> 5;
	TUint32* bufferLimit = aBuffer + bufferWords;
	if (aPrintPos.iX < aClipRect.iTl.iX)
		{
		TInt pixelExcess = aClipRect.iTl.iX - aPrintPos.iX;
		while (pixelExcess >= 32)
			{
			aBuffer++;
			bufferWords--;
			aDataLength -= 32;
			pixelExcess -= 32;
			}
		if (aDataLength <= 0)
			{
			return;
			}
		if (pixelExcess > 0)
			{
			TInt shiftUp = 32 - pixelExcess;
			TUint32* bufferPtr = aBuffer;
			while (bufferPtr < bufferLimit)
				{
				*bufferPtr >>= pixelExcess;
				if (bufferPtr < bufferLimit - 1)
					{
					*bufferPtr |= (*(bufferPtr + 1) << shiftUp);
					}
				bufferPtr++;
				}
			aDataLength -= pixelExcess;
			if (aDataLength <= 0)
				{
				return;
				}
			}
		aPrintPos.iX = aClipRect.iTl.iX;
		}
	if (aPrintPos.iX + aDataLength > aClipRect.iBr.iX)
		{
		TInt pixelExcess = aPrintPos.iX + aDataLength - aClipRect.iBr.iX;
		aDataLength -= pixelExcess;
		if (aDataLength <= 0)
			{
			return;
			}
		}
	while (aNum > 0)
		{
		if ((aPrintPos.iY >= aClipRect.iTl.iY) && (aPrintPos.iY < aClipRect.iBr.iY))
			{
			iDrawDevice->WriteBinaryLine(aPrintPos.iX, aPrintPos.iY, aBuffer, aDataLength, iPenColor, GcDrawMode(iDrawMode));
			}
		aPrintPos.iY++;
		aNum--;
		}
	}


/**
Draw a rotated bitmap glyph.
	
@param	aPos			Position to start drawing glyph after rotation has been applied.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
@param	aRotation		Rotation specifying how the glyph will be drawn.
*/
void CSwDirectGdiEngine::DrawRotatedBitmapGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
												const TSize& aGlyphImageSize, const TRect& aClipRect, const DirectGdi::TGraphicsRotation aRotation)
	{
	TPoint printPos(aPos);
	TInt dataLength = aGlyphImageSize.iWidth;
	TInt dataHeight = aGlyphImageSize.iHeight;
	TInt bitIndex = 0;
	TInt16 repeatCount = 0;
	TInt direction = (aRotation == DirectGdi::EGraphicsRotation270) ? 1 : -1;
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	TInt scanLineWords = (iDrawDevice->ScanLineBytes()) << 3;
	for (TInt glyphLine = 0; glyphLine < dataHeight; glyphLine += repeatCount) // for lines in the glyph bitmap...
		{
		repeatCount = Load16(aGlyphImage + (bitIndex >> 3));
		repeatCount >>= bitIndex & 7;
		TInt multiLineFlag = repeatCount & 1;
		repeatCount >>= 1;
		repeatCount &= 0xf;
		bitIndex += 5;
		TInt signedRepeatCount = repeatCount * direction;
		if (multiLineFlag)
			{
			for (TInt currentLine = 0; currentLine < repeatCount; currentLine++)
				{
				CopyCharLine(scanLineBuffer, scanLineWords, aGlyphImage + (bitIndex >> 3), bitIndex & 7, dataLength);
				OutputCharLineVertical(printPos, scanLineBuffer, dataLength, 1, direction, aClipRect);
				bitIndex += dataLength;
				printPos.iX += direction;
				}
			}
		else
			{
			CopyCharLine(scanLineBuffer,scanLineWords, aGlyphImage + (bitIndex >> 3), bitIndex & 7, dataLength);
			OutputCharLineVertical(printPos, scanLineBuffer, dataLength, repeatCount, direction, aClipRect);
			printPos.iX += signedRepeatCount;
			bitIndex += dataLength;
			}
		}
	}


/**

*/
void CSwDirectGdiEngine::OutputCharLineVertical(TPoint aPrintPos, TUint32* aBuffer, TInt aDataLength,
												TInt aNum, TInt aDirection, const TRect& aClipRect)
	{
//	const DirectGdi::TGraphicsRotation rotation = aUp ? DirectGdi::EGraphicsRotation270 : DirectGdi::EGraphicsRotation90;
//	TInt direction = (aRotation == DirectGdi::EGraphicsRotation270) ? 1 : -1;
	if (aDataLength <= 0)
		{
		return;
		}
	TInt bufferWords = (aDataLength + 31) >> 5;
	TUint32* bufferLimit = aBuffer + bufferWords;
	if (aDirection == 1)
		{
		if (aPrintPos.iY >= aClipRect.iBr.iY)
			{
			TInt pixelExcess = aPrintPos.iY - aClipRect.iBr.iY + 1;
			while (pixelExcess >= 32)
				{
				aBuffer++;
				aDataLength -= 32;
				pixelExcess -= 32;
				}
			if (aDataLength <= 0)
				{
				return;
				}
			if (pixelExcess > 0)
				{
				TInt shiftUp = 32 - pixelExcess;
				TUint32* bufferPtr = aBuffer;
				while (bufferPtr < bufferLimit)
					{
					*bufferPtr >>= pixelExcess;
					if (bufferPtr < bufferLimit - 1)
						*bufferPtr |= (*(bufferPtr + 1) << shiftUp);
					bufferPtr++;
					}
				aDataLength -= pixelExcess;
				if (aDataLength <= 0)
					{
					return;
					}
				}
			aPrintPos.iY = aClipRect.iBr.iY - 1;
			}
		if ((aPrintPos.iY - aDataLength) < (aClipRect.iTl.iY - 1))
			{
			TInt pixelExcess = aClipRect.iTl.iY - 1 - aPrintPos.iY + aDataLength;
			aDataLength -= pixelExcess;
			if (aDataLength <= 0)
				{
				return;
				}
			}
		}
	else
		{
		if (aPrintPos.iY < aClipRect.iTl.iY)
			{
			TInt pixelExcess = aClipRect.iTl.iY - aPrintPos.iY;
			while (pixelExcess >= 32)
				{
				aBuffer++;
				aDataLength -= 32;
				pixelExcess -= 32;
				}
			if (aDataLength <= 0)
				{
				return;
				}
			if (pixelExcess > 0)
				{
				TInt shiftup = 32 - pixelExcess;
				TUint32* bufferptr = aBuffer;
				while (bufferptr < bufferLimit)
					{
					*bufferptr >>= pixelExcess;
					if (bufferptr < bufferLimit - 1)
						*bufferptr |= (*(bufferptr + 1) << shiftup);
					bufferptr++;
					}
				aDataLength -= pixelExcess;
				if (aDataLength <= 0)
					{
					return;
					}
				}
			aPrintPos.iY = aClipRect.iTl.iY;
			}
		if (aPrintPos.iY + aDataLength > aClipRect.iBr.iY)
			{
			TInt pixelExcess = aPrintPos.iY + aDataLength - aClipRect.iBr.iY;
			aDataLength -= pixelExcess;
			if (aDataLength <= 0)
				{
				return;
				}
			}
		}
	CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);
	while (aNum > 0)
		{
		if ((aPrintPos.iX >= aClipRect.iTl.iX) && (aPrintPos.iX < aClipRect.iBr.iX))
			iDrawDevice->WriteBinaryLineVertical(aPrintPos.iX, aPrintPos.iY, aBuffer, aDataLength, iPenColor, drawMode, (aDirection == 1));
		aPrintPos.iX += aDirection;
		aNum--;
		}
	}


/**
Draw an antialiased glyph.
	
@param	aPos			Position to start drawing gyph.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
*/
void CSwDirectGdiEngine::DrawAntiAliasedGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
											  const TSize& aGlyphImageSize, const TRect& aClipRect)
	{
	const TInt topRow = Max(0, aClipRect.iTl.iY - aPos.iY);
	const TInt bottomRow = Min(aGlyphImageSize.iHeight, aClipRect.iBr.iY - aPos.iY);
	const TInt leftCol = Max(0, aClipRect.iTl.iX - aPos.iX);
	const TInt rightCol = Min(aGlyphImageSize.iWidth, aClipRect.iBr.iX - aPos.iX);
	const TUint8* p = aGlyphImage + (topRow * aGlyphImageSize.iWidth) + leftCol;
	const TInt x = aPos.iX + leftCol;
	TInt y = aPos.iY + topRow;
	const TInt cols = rightCol - leftCol;

	for (TInt row = topRow; row < bottomRow; row++, p += aGlyphImageSize.iWidth, y++)
		{
		iDrawDevice->WriteRgbAlphaMulti(x, y, cols, iPenColor, p);
		}
	}


/**
Draw a rotated antialiased glyph.
	
@param	aPos			Position to start drawing gyph after rotation has been applied.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
@param	aRotation		Rotation specifying how the glyph will be drawn.
*/
void CSwDirectGdiEngine::DrawRotatedAntiAliasedGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
													 const TSize& aGlyphImageSize, const TRect& aClipRect, const DirectGdi::TGraphicsRotation aRotation)
	{
	const int KBufferSize = 32;
	TUint8 maskBuffer[KBufferSize];
	int topRow = 0;
	int bottomRow = 0;
	int leftCol = 0;
	int rightCol = 0;
	const TUint32 penColor = iPenColor.Internal();
	const TUint32 brushColor = iBrushColor.Internal();
	
	if (aRotation == DirectGdi::EGraphicsRotation270)
		{
		topRow = Max(0, aClipRect.iTl.iX - aPos.iX);
		bottomRow = Min(aGlyphImageSize.iHeight, aClipRect.iBr.iX - aPos.iX);
		leftCol = Max(0, aPos.iY - aClipRect.iBr.iY + 1);
		rightCol = Min(aGlyphImageSize.iWidth, aPos.iY - aClipRect.iTl.iY + 1);
		TInt y = aPos.iY - (rightCol - 1);
		for (TInt col = rightCol - 1; col >= leftCol; col--, y++)
			{
			TInt x = aPos.iX + topRow;
			for (TInt row = topRow; row < bottomRow; row += KBufferSize, x += KBufferSize)
				{
				TInt length = KBufferSize;
				if (length > bottomRow - row)
					{
					length = bottomRow - row;
					}
				const TUint8* p = aGlyphImage + row * aGlyphImageSize.iWidth + col;
				for (TInt i = 0; i < length; i++, p += aGlyphImageSize.iWidth)
					{
					maskBuffer[i] = *p;
					}
				iDrawDevice->WriteRgbAlphaMulti(x, y, length, iPenColor, maskBuffer);
				}
			}
		}
	else
		{
		topRow = Max(0, aPos.iX - aClipRect.iBr.iX + 1);
		bottomRow = Min(aGlyphImageSize.iHeight, aPos.iX - aClipRect.iTl.iX + 1);
		leftCol = Max(0, aClipRect.iTl.iY - aPos.iY);
		rightCol = Min(aGlyphImageSize.iWidth, aClipRect.iBr.iY - aPos.iY);
		int y = aPos.iY + leftCol;
		for (TInt col = leftCol; col < rightCol; col++, y++)
			{
			TInt x = aPos.iX - (bottomRow - 1);
			for (TInt row = bottomRow; row > topRow; row -= KBufferSize, x += KBufferSize)
				{
				int length = KBufferSize;
				if (length > row - topRow)
					length = row - topRow;
				const TUint8* p = aGlyphImage + (row - 1) * aGlyphImageSize.iWidth + col;
				for (TInt i = 0; i < length; i++, p -= aGlyphImageSize.iWidth)
					{
					maskBuffer[i] = *p;
					}
				iDrawDevice->WriteRgbAlphaMulti(x, y, length, iPenColor, maskBuffer);
				}
			}
		}
	}


/**
Draw a four colour glyph.
	
@param	aPos			Position to start drawing gyph.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
*/
void CSwDirectGdiEngine::DrawFourColourGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
											 const TSize& aGlyphImageSize, const TRect& aClipRect)
	{
	const TInt topRow = Max(0, aClipRect.iTl.iY - aPos.iY);
	const TInt bottomRow = Min(aGlyphImageSize.iHeight, aClipRect.iBr.iY - aPos.iY);
	const TInt leftCol = Max(0, aClipRect.iTl.iX - aPos.iX);
	const TInt rightCol = Min(aGlyphImageSize.iWidth, aClipRect.iBr.iX - aPos.iX);
	const TUint8* p = aGlyphImage + (topRow * aGlyphImageSize.iWidth) + leftCol;
	const TInt x = aPos.iX + leftCol;
	TInt y = aPos.iY + topRow;
	const TInt cols = rightCol - leftCol;
	const TUint32 penColor = iPenColor.Internal();
	const TUint32 shadowColor = iTextShadowColor.Internal();
	const TUint32 brushColor = iBrushColor.Internal();

	MOutlineAndShadowBlend* outlineAndShadow = NULL;
	const TInt err = iDrawDevice->GetInterface(KOutlineAndShadowInterfaceID, reinterpret_cast <TAny*&> (outlineAndShadow));
	if (err == KErrNone) 
		{
		//There is a support for the interface with KOutlineAndShadowInterface id.
		for (TInt row = topRow; row < bottomRow; row++, p += aGlyphImageSize.iWidth, y++)
			{
			outlineAndShadow->WriteRgbOutlineAndShadow(x, y, cols, penColor, shadowColor, brushColor, p);
			}
		}
	else
		{
		// Assert if MOutlineAndShadowBlend interface is not implemented
		GRAPHICS_ASSERT_DEBUG(outlineAndShadow, EDirectGdiPanicInvalidInterfaceHandle);
		}
	}


/**
Draw a rotated four colour glyph.
	
@param	aPos			Position to start drawing gyph after rotation has been applied.
@param	aGlyphImage		Pointer to the glyph image data.
@param	aGlyphImageSize	Glyph image size.
@param	aClipRect		Clipping rect.
@param	aRotation		Rotation specifying how the glyph will be drawn.
*/
void CSwDirectGdiEngine::DrawRotatedFourColourGlyph(const TPoint& aPos, const TUint8* aGlyphImage, const TSize& aGlyphImageSize,
													const TRect& aClipRect, const DirectGdi::TGraphicsRotation aRotation)
	{
	const int KBufferSize = 32;
	TUint8 maskBuffer[KBufferSize];
	int topRow = 0;
	int bottomRow = 0;
	int leftCol = 0;
	int rightCol = 0;
	const TUint32 penColor = iPenColor.Internal();
	const TUint32 shadowColor = iTextShadowColor.Internal();
	const TUint32 brushColor = iBrushColor.Internal();
	
	MOutlineAndShadowBlend* outlineAndShadow = NULL;
	TInt err = iDrawDevice->GetInterface(KOutlineAndShadowInterfaceID, reinterpret_cast <TAny*&> (outlineAndShadow));
	if(err != KErrNone)
		{
		// Assert if MOutlineAndShadowBlend interface is not implemented
		GRAPHICS_ASSERT_DEBUG(outlineAndShadow, EDirectGdiPanicInvalidInterfaceHandle);
		}
	
	if (aRotation == DirectGdi::EGraphicsRotation270)
		{
		topRow = Max(0, aClipRect.iTl.iX - aPos.iX);
		bottomRow = Min(aGlyphImageSize.iHeight, aClipRect.iBr.iX - aPos.iX);
		leftCol = Max(0, aPos.iY - aClipRect.iBr.iY + 1);
		rightCol = Min(aGlyphImageSize.iWidth, aPos.iY - aClipRect.iTl.iY + 1);
		TInt y = aPos.iY - (rightCol - 1);
		for (TInt col = rightCol - 1; col >= leftCol; col--, y++)
			{
			TInt x = aPos.iX + topRow;
			for (TInt row = topRow; row < bottomRow; row += KBufferSize, x += KBufferSize)
				{
				TInt length = KBufferSize;
				if (length > bottomRow - row)
					{
					length = bottomRow - row;
					}
				const TUint8* p = aGlyphImage + row * aGlyphImageSize.iWidth + col;
				for (TInt i = 0; i < length; i++, p += aGlyphImageSize.iWidth)
					{
					maskBuffer[i] = *p;
					}
				//There is a support for the interface with KOutlineAndShadowInterface id.
				outlineAndShadow->WriteRgbOutlineAndShadow(x, y, length, penColor, shadowColor,	brushColor, maskBuffer);
				}
			}
		}
	else
		{
		topRow = Max(0, aPos.iX - aClipRect.iBr.iX + 1);
		bottomRow = Min(aGlyphImageSize.iHeight, aPos.iX - aClipRect.iTl.iX + 1);
		leftCol = Max(0, aClipRect.iTl.iY - aPos.iY);
		rightCol = Min(aGlyphImageSize.iWidth, aClipRect.iBr.iY - aPos.iY);
		int y = aPos.iY + leftCol;
		for (TInt col = leftCol; col < rightCol; col++, y++)
			{
			TInt x = aPos.iX - (bottomRow - 1);
			for (TInt row = bottomRow; row > topRow; row -= KBufferSize, x += KBufferSize)
				{
				int length = KBufferSize;
				if (length > row - topRow)
					length = row - topRow;
				const TUint8* p = aGlyphImage + (row - 1) * aGlyphImageSize.iWidth + col;
				for (TInt i = 0; i < length; i++, p -= aGlyphImageSize.iWidth)
					{
					maskBuffer[i] = *p;
					}
				//There is a support for the interface with KOutlineAndShadowInterface id.
				outlineAndShadow->WriteRgbOutlineAndShadow(x, y, length, penColor, shadowColor,	brushColor, maskBuffer);
				}
			}
		}
	}


/**
Helper function to clip the array to the clip rect.

@param	aArray		Start of array of data to be clipped.
@param	aArrayLimit	End of array of data to be clipped.
@param	aDataWd 	Length of data.
@param	aDataHt		Height of data.
@param	aPos		Position to start drawing from.
@param	aClipRect	Rectangle to clip data array to.

@return Pointer to array of clipped data.
*/
TUint32* CSwDirectGdiEngine::ClipBinaryArray(TUint32* aArray, TUint32* aArrayLimit, TInt& aDataWd, TInt& aDataHt, TPoint& aPos, const TRect& aClipRect)
	{
	TUint32* arrayPtr = aArray;
	TInt clipDiff = aClipRect.iTl.iX - aPos.iX;
	if (clipDiff > 0)
		{
		while (arrayPtr < aArrayLimit)
			{
			*arrayPtr++ >>= clipDiff;
			}
		aDataWd -= clipDiff;
		aPos.iX = aClipRect.iTl.iX;
		arrayPtr = aArray;
		}
	if ((aPos.iX + aDataWd > aClipRect.iBr.iX) && (aDataWd > 0))
		{
		aDataWd = aClipRect.iBr.iX - aPos.iX;
		}
	clipDiff = aClipRect.iTl.iY - aPos.iY;
	if (clipDiff > 0)
		{
		aDataHt -= clipDiff;
		arrayPtr += clipDiff;
		aPos.iY = aClipRect.iTl.iY;
		}
	if (((aPos.iY + aDataHt) > (aClipRect.iBr.iY)) && (aDataHt > 0))
		{
		aDataHt = aClipRect.iBr.iY - aPos.iY;
		}
	return arrayPtr;
	}
