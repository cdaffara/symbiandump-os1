// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pixelutil.h"
#include "directgdiadapter.h"
#include "directgditypes.h"

TBool PixelFormatUtil::HasAlpha(TUidPixelFormat aPixelFormat)
	{
	const TBool hasAlpha = 
		// short circuit format equivalent to legacy TDisplayMode with alpha channel
		aPixelFormat == EUidPixelFormatARGB_8888 ||
		aPixelFormat == EUidPixelFormatARGB_8888_PRE ||

		aPixelFormat == EUidPixelFormatBGRA_8888 ||
		aPixelFormat == EUidPixelFormatABGR_8888 ||
		aPixelFormat == EUidPixelFormatABGR_8888_PRE ||
		aPixelFormat == EUidPixelFormatBGRA_8888_PRE ||
		aPixelFormat == EUidPixelFormatARGB_2101010 ||
		aPixelFormat == EUidPixelFormatABGR_2101010 ||
		aPixelFormat == EUidPixelFormatARGB_1555 ||
		aPixelFormat == EUidPixelFormatARGB_4444 ||
		aPixelFormat == EUidPixelFormatARGB_8332 ||
		aPixelFormat == EUidPixelFormatBGRA_5551 ||
		aPixelFormat == EUidPixelFormatBGRA_4444 ||
		aPixelFormat == EUidPixelFormatAP_88 ||
		aPixelFormat == EUidPixelFormatA_8;
	
	return hasAlpha;
	}

TUidPixelFormat PixelFormatUtil::ConvertToPixelFormat(TDisplayMode aDisplayMode)
	{
	switch (aDisplayMode)
		{
		case EGray2:
			return EUidPixelFormatL_1;
		case EGray4:
			return EUidPixelFormatL_2;
		case EGray16:
			return EUidPixelFormatL_4;
		case EGray256:
			return EUidPixelFormatL_8;
		case EColor16:
			return EUidPixelFormatP_4;
		case EColor256:
			return EUidPixelFormatP_8;
		case EColor4K:
			return EUidPixelFormatXRGB_4444;
		case EColor64K:
			return EUidPixelFormatRGB_565;
		case EColor16M:
			return EUidPixelFormatRGB_888;		
		case EColor16MU:
			return EUidPixelFormatXRGB_8888;
		case EColor16MA:
			return EUidPixelFormatARGB_8888;
		case EColor16MAP:
			return EUidPixelFormatARGB_8888_PRE;
		default:
			return EUidPixelFormatUnknown;
		};	
	}

TDisplayMode PixelFormatUtil::ConvertToDisplayMode(TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)	
		{
		case EUidPixelFormatL_1:
			return EGray2;
		case EUidPixelFormatL_2:
			return EGray4;
		case EUidPixelFormatL_4:
			return EGray16;
		case EUidPixelFormatL_8:
			return EGray256;
		case EUidPixelFormatP_4:
			return EColor16;
		case EUidPixelFormatP_8:
			return EColor256;
		case EUidPixelFormatXRGB_4444:
			return EColor4K;
		case EUidPixelFormatRGB_565:
			return EColor64K;
		case EUidPixelFormatRGB_888:
			return EColor16M;
		case EUidPixelFormatXRGB_8888:
			return EColor16MU;
		case EUidPixelFormatARGB_8888:
			return EColor16MA;
		case EUidPixelFormatARGB_8888_PRE:
			return EColor16MAP;
		default:
			return ENone;
		}
	}

TInt PixelFormatUtil::BitsPerPixel(TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)
		{
		case EUidPixelFormatP_1:
		case EUidPixelFormatL_1:
			return 1;
		case EUidPixelFormatP_2:
		case EUidPixelFormatL_2:
			return 2;
		case EUidPixelFormatP_4:
		case EUidPixelFormatL_4:
			return 4;
		case EUidPixelFormatRGB_332:
		case EUidPixelFormatA_8:
		case EUidPixelFormatBGR_332:
		case EUidPixelFormatP_8:
		case EUidPixelFormatL_8:
			return 8;
		case EUidPixelFormatRGB_565:
		case EUidPixelFormatBGR_565:
		case EUidPixelFormatARGB_1555:
		case EUidPixelFormatXRGB_1555:
		case EUidPixelFormatARGB_4444:
		case EUidPixelFormatARGB_8332:
		case EUidPixelFormatBGRX_5551:
		case EUidPixelFormatBGRA_5551:
		case EUidPixelFormatBGRA_4444:
		case EUidPixelFormatBGRX_4444:
		case EUidPixelFormatAP_88:
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatXBGR_4444:
			return 16;
		case EUidPixelFormatBGR_888:
		case EUidPixelFormatRGB_888:
			return 24;
		case EUidPixelFormatXRGB_8888:
		case EUidPixelFormatBGRX_8888:
		case EUidPixelFormatXBGR_8888:
		case EUidPixelFormatBGRA_8888:
		case EUidPixelFormatARGB_8888:
		case EUidPixelFormatABGR_8888:
		case EUidPixelFormatARGB_8888_PRE:
		case EUidPixelFormatABGR_8888_PRE:
		case EUidPixelFormatBGRA_8888_PRE:
		case EUidPixelFormatARGB_2101010:
		case EUidPixelFormatABGR_2101010:
			return 32;
		default:
			GRAPHICS_ASSERT_DEBUG(EFalse, EDirectGdiPanicInvalidDisplayMode);
			return 0;
		};	
	}

/**
Create pixel buffer reader from a given pixel buffer by specifying its buffer addres and properties.
Supported format are:
-EUidPixelFormatRGB_565
-EUidPixelFormatXRGB_8888
-EUidPixelFormatARGB_8888
-EUidPixelFormatARGB_8888_PRE
*/
TPixelBufferReader::TPixelBufferReader(const TUint32* aPixelBuffer, const TSize& aSize, TInt aStride, TUidPixelFormat aFormat):
	iBuffer(aPixelBuffer),
	iSize(aSize),
	iStride(aStride),
	iFormat(aFormat)
	{
	GRAPHICS_ASSERT_DEBUG(iBuffer && iStride!=0, EDirectGdiPanicInvalidBitmap);
	GRAPHICS_ASSERT_DEBUG(iSize.iWidth!=0 && iSize.iHeight!=0, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(
		iFormat==EUidPixelFormatRGB_565 ||
		iFormat==EUidPixelFormatXRGB_8888 ||
		iFormat==EUidPixelFormatARGB_8888 ||
		iFormat==EUidPixelFormatARGB_8888_PRE,
		EDirectGdiPanicInvalidDisplayMode);
	}

/**
Copies pixels into user buffer starting and ending based on the given read position and 
read length (in pixels). Convert data into user pixel format if requested.
Copying will be done forward or backward (from a given read position) based on read direction parameter.

@pre	TPixelBufferReader object was constructed with valid pixel buffer and its properties.
		Starting and ending read position is within the pixel buffer area.
		Supported read format:
			RGB_565
			XRGB_8888, ARGB_8888 or ARGB_8888_PRE.
@post	Pixels copied into user buffer.
*/
void TPixelBufferReader::GetScanLine(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen,
		TUidPixelFormat aReadFormat, TReadDirection aReadDir) const
	{
	GRAPHICS_ASSERT_DEBUG(aReadPos.iX>=0 && aReadPos.iX<iSize.iWidth &&
		aReadPos.iY>=0 && aReadPos.iY<iSize.iHeight, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(
		aReadFormat==EUidPixelFormatRGB_565 ||
		aReadFormat==EUidPixelFormatXRGB_8888 ||
		aReadFormat==EUidPixelFormatARGB_8888 ||
		aReadFormat==EUidPixelFormatARGB_8888_PRE,
		EDirectGdiPanicInvalidDisplayMode);

#ifdef _DEBUG
	switch (aReadDir)
		{
		case EReadHorizontal:
		GRAPHICS_ASSERT_DEBUG(aReadPos.iX+aReadLen<=iSize.iWidth, EDirectGdiPanicOutOfBounds);
		break;

		case EReadHorizontalReverse:
		GRAPHICS_ASSERT_DEBUG(aReadPos.iX-aReadLen+1>=0, EDirectGdiPanicOutOfBounds);
		break;
		
		case EReadVertical:
  		GRAPHICS_ASSERT_DEBUG(aReadPos.iY+aReadLen<=iSize.iHeight, EDirectGdiPanicOutOfBounds);
		break;
		
		case EReadVerticalReverse:
  		GRAPHICS_ASSERT_DEBUG(aReadPos.iY-aReadLen+1>=0, EDirectGdiPanicOutOfBounds);
		break;
		}
#endif

	switch(aReadFormat)
		{
		case EUidPixelFormatRGB_565:
			GetScanLineRGB_565(aReadBuf, aReadPos, aReadLen,  aReadDir);
			break;
		case EUidPixelFormatXRGB_8888:
			GetScanLineXRGB_8888(aReadBuf, aReadPos, aReadLen,  aReadDir);
			break;
		case EUidPixelFormatARGB_8888:
			GetScanLineARGB_8888(aReadBuf, aReadPos, aReadLen,  aReadDir);
			break;
		case EUidPixelFormatARGB_8888_PRE:
			GetScanLineARGB_8888_PRE(aReadBuf, aReadPos, aReadLen,  aReadDir);
			break;
		default:
			aReadBuf.SetLength(0);
			break;
		};
	}

/**
Copies pixels into user buffer starting and ending based on the given read position and 
read length (in pixels). Converts data into user pixel format and scales up or down depending
on the specified parameters. Copying will be done forward or backward (from a given read position)
based on read direction parameter.

@pre	TPixelBufferReader object was constructed with valid pixel buffer and its properties.
		Starting and ending read position is within the pixel buffer area.
		Supported read format:
			RGB_565
			XRGB_8888, ARGB_8888 or ARGB_8888_PRE.
@post	Pixels copied into user buffer.
*/
void TPixelBufferReader::GetScaledScanLine(TDes8& aReadBuf, const TPoint& aReadPos, TInt aClipPos,
		TInt aClipLen, TInt aDestLen, TInt aSrcLen, TUidPixelFormat aReadFormat,
		TReadDirection aReadDir) const
	{
	GRAPHICS_ASSERT_DEBUG(aReadPos.iX>=0 && aReadPos.iX<iSize.iWidth &&
		aReadPos.iY>=0 && aReadPos.iY<iSize.iHeight, EDirectGdiPanicOutOfBounds);

	GRAPHICS_ASSERT_DEBUG(
		aReadFormat==EUidPixelFormatRGB_565 ||
		aReadFormat==EUidPixelFormatXRGB_8888 ||
		aReadFormat==EUidPixelFormatARGB_8888 ||
		aReadFormat==EUidPixelFormatARGB_8888_PRE,
		EDirectGdiPanicInvalidDisplayMode);

	if (aReadDir == EReadHorizontal || aReadDir == EReadHorizontalReverse)
		{
		GetScaledScanLineH(aReadBuf, aReadPos, aClipPos, aClipLen, aDestLen, aSrcLen, aReadFormat, aReadDir);
		}
	else
		{
		GetScaledScanLineV(aReadBuf, aReadPos, aClipPos, aClipLen, aDestLen, aSrcLen, aReadFormat, aReadDir);
		}
	}

/**
Gets pixel address in arbitrary position within the buffer
*/
const TUint32* TPixelBufferReader::GetPixelAddr(const TPoint& aPos) const
	{
	const TUint32* slptr = GetScanLineAddr(aPos.iY);
	return PixelFormatUtil::BitsPerPixel(iFormat)==32? slptr+aPos.iX : (TUint32*)((TUint16*)slptr+aPos.iX);
	}

/**
Gets scanline address for a give row poisition
*/
const TUint32* TPixelBufferReader::GetScanLineAddr(TInt aRow) const
	{
	return (iBuffer + (aRow * iStride >> 2));
	}

/**
Copies from 16-bit src to 32-bit dest
*/
void TPixelBufferReader::CopyFromRGB_565(TUint32* aDstPtr, const TUint16* aSrcPtr,
		TInt aNumOfPixels, TInt aAdvance) const
	{
	const TUint16* lowAdd = Convert16to32bppLow();
	const TUint32* highAdd = Convert16to32bppHigh();

	while (aNumOfPixels--)
		{
		const TUint8 low = *aSrcPtr & 0xff;
		const TUint8 high = *aSrcPtr >> 8;
		*aDstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
		
		aSrcPtr += aAdvance;
		}
	}

/**
Calculates pixel position increment based on a given read direction and buffer pixel format.
*/
TInt TPixelBufferReader::GetAdvance(TReadDirection aReadDir) const
	{
	TInt advance = 0;
	// supported pixel buffer is either 16-bit or 32-bit
	//
	switch (aReadDir)
		{
		case EReadHorizontal:
			advance = 1;
			break;
		case EReadHorizontalReverse:
			advance = -1;
			break;
		case EReadVertical:
			advance = PixelFormatUtil::BitsPerPixel(iFormat)==16? iStride >> 1 : iStride >> 2;
			break;
		case EReadVerticalReverse:
			advance = PixelFormatUtil::BitsPerPixel(iFormat)==16? -(iStride >> 1) : -(iStride >> 2);
			break;
		}

	return advance;
	}

/**
Reads and converts scanline into RGB_565.
*/
void TPixelBufferReader::GetScanLineRGB_565(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen,
		TReadDirection aReadDir) const
	{
	// read as much as buffer can hold
	aReadLen = Min(aReadLen, aReadBuf.MaxLength() >> 1);
	aReadBuf.SetLength(aReadLen << 1);

	TUint16* dstPtr = (TUint16*)aReadBuf.Ptr();
	const TUint32* scanLinePtr = GetScanLineAddr(aReadPos.iY);
	TInt posX = aReadPos.iX;
	const TInt advance = GetAdvance(aReadDir);

	// supported pixel buffer:
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE
	//
	switch (iFormat)		
		{
		case EUidPixelFormatRGB_565:
			{
			const TUint16* srcPtr = (TUint16*)scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = *srcPtr;
				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatXRGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = TUint16(TRgb::Color16MU(*srcPtr).Color64K());
				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatARGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = TUint16(TRgb::Color16MA(*srcPtr).Color64K());
				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatARGB_8888_PRE:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = TUint16(TRgb::Color16MAP(*srcPtr).Color64K());
				srcPtr += advance;
				}
			}
			break;
		}
	}

/**
Reads and converts scanline into XRGB_8888.
*/
void TPixelBufferReader::GetScanLineXRGB_8888(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen,
		TReadDirection aReadDir) const
	{
	// read as much as buffer can hold
	aReadLen = Min(aReadLen, aReadBuf.MaxLength() >> 2);
	aReadBuf.SetLength(aReadLen << 2);

	TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
	const TUint32* scanLinePtr = GetScanLineAddr(aReadPos.iY);
	TInt posX = aReadPos.iX;
	const TInt advance = GetAdvance(aReadDir);

	// supported pixel buffer:
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE
	//
	switch (iFormat)
		{
		case EUidPixelFormatRGB_565:
			{
			const TUint16* srcPtr = (TUint16*)scanLinePtr + posX;
			CopyFromRGB_565(dstPtr, srcPtr, aReadLen, advance);
			}
			break;

		case EUidPixelFormatXRGB_8888:
		case EUidPixelFormatARGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = *srcPtr;
				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatARGB_8888_PRE:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			const TUint16* normTable = PtrTo16BitNormalisationTable();
			while(aReadLen--)
				{
				*dstPtr++ = PMA2NonPMAPixel(*srcPtr, normTable);
				srcPtr += advance;
				}			
			}
			break;
		}
	}

/**
Reads and converts scanline into ARGB_8888.
*/
void TPixelBufferReader::GetScanLineARGB_8888(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen,
		TReadDirection aReadDir) const
	{
	// read as much as buffer can hold
	aReadLen = Min(aReadLen, aReadBuf.MaxLength() >> 2);
	aReadBuf.SetLength(aReadLen << 2);

	TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
	const TUint32* scanLinePtr = GetScanLineAddr(aReadPos.iY);
	TInt posX = aReadPos.iX;
	const TInt advance = GetAdvance(aReadDir);

	// supported pixel buffer:
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE
	//
	switch (iFormat)
		{
		case EUidPixelFormatRGB_565:
			{
			const TUint16* srcPtr = (TUint16*)scanLinePtr + posX;
			CopyFromRGB_565(dstPtr, srcPtr, aReadLen, advance);
			}
			break;

		case EUidPixelFormatXRGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = 0xff000000 | *srcPtr;
				srcPtr += advance;
				}
			}
			break;
		
		case EUidPixelFormatARGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = *srcPtr;
				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatARGB_8888_PRE:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			const TUint16* normTable = PtrTo16BitNormalisationTable();
			while(aReadLen--)
				{
				*dstPtr++ = PMA2NonPMAPixel(*srcPtr, normTable);
				srcPtr += advance;
				}			
			}
			break;
		}
	}

/**
Read and convert scanline into ARGB_8888_PRE.
*/
void TPixelBufferReader::GetScanLineARGB_8888_PRE(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen,
		TReadDirection aReadDir) const
	{
	// read as much as buffer can hold
	aReadLen = Min(aReadLen, aReadBuf.MaxLength() >> 2);
	aReadBuf.SetLength(aReadLen << 2);

	TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
	const TUint32* scanLinePtr = GetScanLineAddr(aReadPos.iY);
	TInt posX = aReadPos.iX;
	const TInt advance = GetAdvance(aReadDir);

	// supported pixel buffer:
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE
	//
	switch (iFormat)
		{
		case EUidPixelFormatRGB_565:
			{
			const TUint16* srcPtr = (TUint16*)scanLinePtr + posX;
			CopyFromRGB_565(dstPtr, srcPtr, aReadLen, advance);
			}
			break;

		case EUidPixelFormatXRGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = 0xff000000 | *srcPtr;
				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatARGB_8888:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				TUint32 argb = *srcPtr;
				Convert2PMA(argb);
				*dstPtr++ = argb;

				srcPtr += advance;
				}
			}
			break;

		case EUidPixelFormatARGB_8888_PRE:
			{
			const TUint32* srcPtr = scanLinePtr + posX;
			while (aReadLen--)
				{
				*dstPtr++ = *srcPtr;
				srcPtr += advance;
				}
			}
			break;
		}
	}

/**
Reads and scales pixels horizontally from either left or right. Converts to other pixel format if
requested.
*/
void TPixelBufferReader::GetScaledScanLineH(TDes8& aReadBuf, const TPoint& aReadPos, TInt aClipDestPos,
		TInt aClipDestLen, TInt aDestLen, TInt aSrcLen, TUidPixelFormat aReadFormat,
		TReadDirection aReadDir) const
	{
	// setup DDA for scaling in X direction, use read pos as starting point and move right or left
	// depending on the read direction
	//
	TLinearDDA xScaler;
	TPoint xPos(aReadPos.iX, 0);
	const TPoint delta = aReadDir==EReadHorizontal? TPoint(aSrcLen, aDestLen) : TPoint(-aSrcLen, aDestLen);
	xScaler.Construct(xPos, xPos + delta, TLinearDDA::ELeft);

	// jump to dest X position and return the corresponding src X position
	xPos.iY = aClipDestPos;
	if (aClipDestPos > 0)
		{
		xScaler.JumpToYCoord(xPos.iX, xPos.iY);
		}
	else
		{
		xScaler.NextStep(xPos);
		}

	const TUint32* scanLinePtr = GetScanLineAddr(aReadPos.iY);

	// supported pixel buffer:
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE

	// supported read format
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE

	switch(aReadFormat)
		{
		case EUidPixelFormatRGB_565:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 1);
			aReadBuf.SetLength(aClipDestLen << 1);

			TUint16* dstPtr = (TUint16*)aReadBuf.Ptr();
			TUint16* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*) scanLinePtr;					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + xPos.iX);
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatXRGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while (dstPtr < dstLimit)
						{
						*dstPtr++ = TUint16(TRgb::Color16MU(*(srcPtr + xPos.iX)).Color64K());
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while (dstPtr < dstLimit)
						{
						*dstPtr++ = TUint16(TRgb::Color16MA(*(srcPtr + xPos.iX)).Color64K());
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = scanLinePtr;
					while (dstPtr < dstLimit)
						{
						*dstPtr++ = TUint16(TRgb::Color16MAP(*(srcPtr + xPos.iX)).Color64K());
						xScaler.NextStep(xPos);
						}
					}
					break;
				}
			}
			break;

		case EUidPixelFormatXRGB_8888:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 2);
			aReadBuf.SetLength(aClipDestLen << 2);

			TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
			TUint32* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*)scanLinePtr;

					const TUint16* lowAdd = Convert16to32bppLow();
					const TUint32* highAdd = Convert16to32bppHigh();

					while (dstPtr < dstLimit)
						{
						TUint16 c = *(srcPtr + xPos.iX);
						const TUint8 low = c & 0xff;
						const TUint8 high = c >> 8;
						*dstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
						
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatXRGB_8888:
				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + xPos.iX);
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = scanLinePtr;
					const TUint16* normTable = PtrTo16BitNormalisationTable();
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = PMA2NonPMAPixel(*(srcPtr + xPos.iX), normTable);
						xScaler.NextStep(xPos);
						}			
					}
					break;
				}
			}
			break;
		
		case EUidPixelFormatARGB_8888:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 2);
			aReadBuf.SetLength(aClipDestLen << 2);

			TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
			TUint32* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*)scanLinePtr;

					const TUint16* lowAdd = Convert16to32bppLow();
					const TUint32* highAdd = Convert16to32bppHigh();

					while (dstPtr < dstLimit)
						{
						TUint16 c = *(srcPtr + xPos.iX);
						const TUint8 low = c & 0xff;
						const TUint8 high = c >> 8;
						*dstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
						
						xScaler.NextStep(xPos);
						}
					}
					break;
					
				case EUidPixelFormatXRGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = 0xff000000 | *(srcPtr + xPos.iX);
						xScaler.NextStep(xPos);
						}
					}
					break;
				
				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + xPos.iX);
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = scanLinePtr;
					const TUint16* normTable = PtrTo16BitNormalisationTable();
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = PMA2NonPMAPixel(*(srcPtr + xPos.iX), normTable);
						xScaler.NextStep(xPos);
						}			
					}
					break;
				}
			}
			break;

		case EUidPixelFormatARGB_8888_PRE:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 2);
			aReadBuf.SetLength(aClipDestLen << 2);

			TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
			TUint32* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*)scanLinePtr;

					const TUint16* lowAdd = Convert16to32bppLow();
					const TUint32* highAdd = Convert16to32bppHigh();

					while (dstPtr < dstLimit)
						{
						TUint16 c = *(srcPtr + xPos.iX);
						const TUint8 low = c & 0xff;
						const TUint8 high = c >> 8;
						*dstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
						
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatXRGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = 0xff000000 | *(srcPtr + xPos.iX);
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = scanLinePtr;
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + xPos.iX);
						xScaler.NextStep(xPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = scanLinePtr;
					while (dstPtr < dstLimit)
						{
						TUint32 argb = *(srcPtr + xPos.iX);
						Convert2PMA(argb);
						*dstPtr++ = argb;

						xScaler.NextStep(xPos);
						}
					}
					break;
				}
			}
		break;
		}
	}

/**
Reads and scales pixels vertically from either top or bottom. Converts to other pixel format 
if requested.
*/
void TPixelBufferReader::GetScaledScanLineV(TDes8& aReadBuf, const TPoint& aReadPos, TInt aClipDestPos,
		TInt aClipDestLen, TInt aDestLen, TInt aSrcLen, TUidPixelFormat aReadFormat,
		TReadDirection aReadDir) const
	{
	// setup DDA for scaling in Y direction, use read pos as starting point and move up or down
	// depending on the read direction
	//
	TLinearDDA yScaler;
	TPoint yPos(aReadPos.iY, 0);
	const TPoint delta = aReadDir==EReadVertical? TPoint(aSrcLen, aDestLen) : TPoint(-aSrcLen, aDestLen);
	yScaler.Construct(yPos, yPos + delta, TLinearDDA::ELeft);

	// jump to dest Y position and return the corresponding src Y position
	yPos.iY = aClipDestPos;
	if (aClipDestPos > 0)
		{
		yScaler.JumpToYCoord(yPos.iX, yPos.iY);
		}
	else
		{
		yScaler.NextStep(yPos);
		}

	// supported pixel buffer:
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE

	// supported read format
	// RGB_565
	// XRGB_8888
	// ARGB_8888
	// ARGB_8888_PRE

	switch(aReadFormat)
		{
		case EUidPixelFormatRGB_565:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 1);
			aReadBuf.SetLength(aClipDestLen << 1);

			TUint16* dstPtr = (TUint16*)aReadBuf.Ptr();
			TUint16* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*) iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 1;
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + yPos.iX * offset);
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatXRGB_8888:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while (dstPtr < dstLimit)
						{
						*dstPtr++ = TUint16(TRgb::Color16MU(*(srcPtr + yPos.iX * offset)).Color64K());
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while (dstPtr < dstLimit)
						{
						*dstPtr++ = TUint16(TRgb::Color16MA(*(srcPtr + yPos.iX * offset)).Color64K());
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while (dstPtr < dstLimit)
						{
						*dstPtr++ = TUint16(TRgb::Color16MAP(*(srcPtr + yPos.iX * offset)).Color64K());
						yScaler.NextStep(yPos);
						}
					}
					break;
				}
			}
			break;

		case EUidPixelFormatXRGB_8888:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 2);
			aReadBuf.SetLength(aClipDestLen << 2);

			TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
			TUint32* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*)iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 1;

					const TUint16* lowAdd = Convert16to32bppLow();
					const TUint32* highAdd = Convert16to32bppHigh();

					while (dstPtr < dstLimit)
						{
						TUint16 c = *(srcPtr + yPos.iX * offset);
						const TUint8 low = c & 0xff;
						const TUint8 high = c >> 8;
						*dstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
						
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatXRGB_8888:
				case EUidPixelFormatARGB_8888:				
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + yPos.iX * offset);
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					const TUint16* normTable = PtrTo16BitNormalisationTable();
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = PMA2NonPMAPixel(*(srcPtr + yPos.iX * offset), normTable);
						yScaler.NextStep(yPos);
						}			
					}
					break;
				}
			}
			break;
		
		case EUidPixelFormatARGB_8888:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 2);
			aReadBuf.SetLength(aClipDestLen << 2);

			TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
			TUint32* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*)iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 1;

					const TUint16* lowAdd = Convert16to32bppLow();
					const TUint32* highAdd = Convert16to32bppHigh();

					while (dstPtr < dstLimit)
						{
						TUint16 c = *(srcPtr + yPos.iX * offset);
						const TUint8 low = c & 0xff;
						const TUint8 high = c >> 8;
						*dstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
						
						yScaler.NextStep(yPos);
						}
					}
					break;
					
				case EUidPixelFormatXRGB_8888:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = 0xff000000 | *(srcPtr + yPos.iX * offset);
						yScaler.NextStep(yPos);
						}
					}
					break;
				
				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + yPos.iX * offset);
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					const TUint16* normTable = PtrTo16BitNormalisationTable();
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = PMA2NonPMAPixel(*(srcPtr + yPos.iX * offset), normTable);
						yScaler.NextStep(yPos);
						}			
					}
					break;
				}
			}
			break;

		case EUidPixelFormatARGB_8888_PRE:
			{
			aClipDestLen = Min(aClipDestLen, aReadBuf.MaxLength() >> 2);
			aReadBuf.SetLength(aClipDestLen << 2);

			TUint32* dstPtr = (TUint32*)aReadBuf.Ptr();
			TUint32* dstLimit = dstPtr + aClipDestLen;
			
			switch (iFormat)
				{
				case EUidPixelFormatRGB_565:
					{
					const TUint16* srcPtr = (TUint16*)iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 1;

					const TUint16* lowAdd = Convert16to32bppLow();
					const TUint32* highAdd = Convert16to32bppHigh();

					while (dstPtr < dstLimit)
						{
						TUint16 c = *(srcPtr + yPos.iX * offset);
						const TUint8 low = c & 0xff;
						const TUint8 high = c >> 8;
						*dstPtr++ = (*(highAdd+high)) | (*(lowAdd+low));
						
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatXRGB_8888:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = 0xff000000 | *(srcPtr + yPos.iX * offset);
						yScaler.NextStep(yPos);
						}
					}
					break;
				
				case EUidPixelFormatARGB_8888_PRE:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while(dstPtr < dstLimit)
						{
						*dstPtr++ = *(srcPtr + yPos.iX * offset);
						yScaler.NextStep(yPos);
						}
					}
					break;

				case EUidPixelFormatARGB_8888:
					{
					const TUint32* srcPtr = iBuffer + aReadPos.iX;
					const TInt offset = iStride >> 2;
					
					while (dstPtr < dstLimit)
						{
						TUint32 argb = *(srcPtr + yPos.iX * offset);
						Convert2PMA(argb);
						*dstPtr++ = argb;

						yScaler.NextStep(yPos);
						}
					}
					break;
				}
			}
		break;
		}
	}
