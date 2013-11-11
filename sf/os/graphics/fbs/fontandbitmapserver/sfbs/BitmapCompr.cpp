// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <bitmap.h>
#include "CompressionBookmark.h"
#include "BitmapCompr.h"
#include "UTILS.H"
#include "bitmapconst.h"

GLREF_C void Panic(TFbsPanic aPanic);

/**
The method recalculates aLineScanPos which is used for quick search of the
nearest "length" byte before aStartPos.
Note: The method should be used only for compressed in RAM bitmaps for which
iPile != NULL.
@internalComponent
@post aLineScanPos might be updated
*/
GLDEF_C void AdjustLineScanningPosition(TLineScanningPosition& aLineScanPos, 
										const TUint32* aBase, 
										TInt aBitmapWidth, 
										TInt aStartPos,
										TInt aCompressedDataBytes)
	{
	TInt difference = ::Abs(aStartPos - aLineScanPos.iCursorPos);
	if(difference > aBitmapWidth && aCompressedDataBytes > KCompressionBookMarkThreshold)
		{
		const TCompressionBookMark* bookMarkPtr = 
			(const TCompressionBookMark*)((const TUint8*)aBase + 
										  (aCompressedDataBytes + 3) / 4 * 4 + 4);
		if(bookMarkPtr->iSrcDataOffset && bookMarkPtr->IsCheckSumOk())
			{
			TInt difference2 = ::Abs(aStartPos - bookMarkPtr->iCursorPos);
			if(difference2 < difference)
				{
				aLineScanPos.iCursorPos = bookMarkPtr->iCursorPos;
				aLineScanPos.iSrcDataPtr = ((TUint8*)aBase) + bookMarkPtr->iSrcDataOffset;
				}
			}
		}
	}

/**
The method gets the first 16 bpp pixel value pointed by aSrcPtr and copies it aCount times
to aDestPtr buffer.
@internalComponent
@pre aCount > 0
@param aSrcPtr It points to the source pixel
@param aDestPtr It points to the destination buffer where the pixel should be copied aCount times
@param aCount How many times the pixel value should be copied
@return The updated aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E2bpp, E2bpp>::CopyPixel(TUint8* aDestPtr, 
												 const TUint8* aSrcPtr, 
												 TInt aCount) const
	{
	//We can't access directly the 16 bits data chunk, because it may be located at an odd address and
	//THUMB build will fail with "KERNEL EXEC-3".
	TUint8 lsb = *aSrcPtr;
	TUint8 msb = *(aSrcPtr + 1);
	TUint16 data = TUint16(lsb | (TUint16(msb) << 8));
	TUint32 data32 = TUint32(data | (TUint32(data) << 16));
	if((TUint32(aDestPtr) & 0x2) && aCount)
		{
		*(TUint16*)aDestPtr = data;
		aDestPtr += 2;//+= 2 - the pixel color is coded into 16 bits - 2 bytes
		aCount--;
		}
	while(aCount > 1)
		{
		*(TUint32*)aDestPtr = data32;
		aDestPtr += 4;//+= 4 - the pixel color is coded into 16 bits - 2 x 2 bytes
		aCount -= 2;
		}
	if(aCount)
		{
		*(TUint16*)aDestPtr = data;
		aDestPtr += 2;//+= 2 - the pixel color is coded into 16 bits - 2 bytes
		}
	return aDestPtr;
	}

/**
The method gets the first 24 bpp pixel value pointed by aSrcPtr and copies it aCount times
to aDestPtr buffer.
@internalComponent
@pre aCount > 0
@param aSrcPtr It points to the source pixel
@param aDestPtr It points to the destination buffer where the pixel should be copied aCount times
@param aCount How many times the pixel value should be copied
@return The updated aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E3bpp, E3bpp>::CopyPixel(TUint8* aDestPtr, 
												 const TUint8* aSrcPtr, 
												 TInt aCount) const
	{
	const TUint8 b1 = *aSrcPtr;
	const TUint8 b2 = *(aSrcPtr + 1);
	const TUint8 b3 = *(aSrcPtr + 2);
	while(aCount--)
		{
		*aDestPtr++ = b1;
		*aDestPtr++ = b2;
		*aDestPtr++ = b3;
		}
	return aDestPtr;
	}

/**
The method gets the first 32 bpp pixel value pointed by aSrcPtr and copies it aCount times
to aDestPtr buffer.
@internalComponent
@pre aCount > 0
@param aSrcPtr It points to the source pixel
@param aDestPtr It points to the destination buffer where the pixel should be copied aCount times
@param aCount How many times the pixel value should be copied
@return The updated aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E4bpp, E4bpp>::CopyPixel(TUint8* aDestPtr, 
												 const TUint8* aSrcPtr, 
												 TInt aCount) const
	{
	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aDestPtr) & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is word aligned

	const TUint8 b1 = *aSrcPtr;
	const TUint8 b2 = *(aSrcPtr + 1);
	const TUint8 b3 = *(aSrcPtr + 2);
	const TUint8 b4 = *(aSrcPtr + 3);

	if((reinterpret_cast<TUint32>(aDestPtr) & 0x3)== 0)	// the start address is word aligned
		{
		const TUint32 pixel = (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
		TUint32* destPtr32 = reinterpret_cast <TUint32*>(aDestPtr);
		TInt blocksOf16 = ((aCount + 0x0f) >> 4);	// number of blocks of upto 16 words to write
		// the first iteration writes 1 to 15 words
		switch (aCount & 0x0f)
			{ // note that case statements intentionally cascade
		case 0:
			do {	// second and subsequent iterations always write 16 words
			*destPtr32++ = pixel;
		case 15:
			*destPtr32++ = pixel;
		case 14:
			*destPtr32++ = pixel;
		case 13:
			*destPtr32++ = pixel;
		case 12:
			*destPtr32++ = pixel;
		case 11:
			*destPtr32++ = pixel;
		case 10:
			*destPtr32++ = pixel;
		case 9:
			*destPtr32++ = pixel;
		case 8:
			*destPtr32++ = pixel;
		case 7:
			*destPtr32++ = pixel;
		case 6:
			*destPtr32++ = pixel;
		case 5:
			*destPtr32++ = pixel;
		case 4:
			*destPtr32++ = pixel;
		case 3:
			*destPtr32++ = pixel;
		case 2:
			*destPtr32++ = pixel;
		case 1:
			*destPtr32++ = pixel;
			} while(--blocksOf16 > 0);
			}

		return reinterpret_cast <TUint8*>(destPtr32);
		}

	while(aCount--)
		{
		*aDestPtr++ = b1;
		*aDestPtr++ = b2;
		*aDestPtr++ = b3;
		*aDestPtr++ = b4;
		}
	return aDestPtr;
	}

/**
The method gets the first 24 bpp pixel value pointed by aSrcPtr and copies it aCount times
to aDestPtr 32 bpp buffer.
@internalComponent
@pre aCount > 0
@param aSrcPtr It points to the source pixel
@param aDestPtr It points to the destination buffer where the pixel should be copied aCount times
@param aCount How many times the pixel value should be copied
@return The updated aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E3bpp, E4bpp>::CopyPixel(TUint8* aDestPtr, 
												 const TUint8* aSrcPtr, 
												 TInt aCount) const
	{
	const TUint8 b1 = *aSrcPtr;
	const TUint8 b2 = *(aSrcPtr + 1);
	const TUint8 b3 = *(aSrcPtr + 2);

	if((reinterpret_cast<TUint32>(aDestPtr) & 0x3)== 0)	// the start address is word aligned
		{
		const TUint32 pixel = 0xFF000000 | (b3 << 16) | (b2 << 8) | b1; //it is more accurate to equalize to 0xff, 
																		 //as top byte could correspond an alpha channel
		TUint32* destPtr32 = reinterpret_cast <TUint32*>(aDestPtr);
		TInt blocksOf16 = ((aCount + 0x0f) >> 4);	// number of blocks of upto 16 words to write
		// the first iteration writes 1 to 15 words
		switch (aCount & 0x0f)
			{ // note that case statements intentionally cascade
		case 0:
			do {	// second and subsequent iterations always write 16 words
			*destPtr32++ = pixel;
		case 15:
			*destPtr32++ = pixel;
		case 14:
			*destPtr32++ = pixel;
		case 13:
			*destPtr32++ = pixel;
		case 12:
			*destPtr32++ = pixel;
		case 11:
			*destPtr32++ = pixel;
		case 10:
			*destPtr32++ = pixel;
		case 9:
			*destPtr32++ = pixel;
		case 8:
			*destPtr32++ = pixel;
		case 7:
			*destPtr32++ = pixel;
		case 6:
			*destPtr32++ = pixel;
		case 5:
			*destPtr32++ = pixel;
		case 4:
			*destPtr32++ = pixel;
		case 3:
			*destPtr32++ = pixel;
		case 2:
			*destPtr32++ = pixel;
		case 1:
			*destPtr32++ = pixel;
			} while(--blocksOf16 > 0);
			}

		return reinterpret_cast <TUint8*>(destPtr32);
		}

	while(aCount--)
		{
		*aDestPtr++ = b1;
		*aDestPtr++ = b2;
		*aDestPtr++ = b3;
		*aDestPtr++ = 0xff; //it is more accurate to equalize to 0xff, as top byte 
									//could correspond an alpha channel
		}
	return aDestPtr;
	}

/**
The method gets the 24 bpp pixel value pointed by aSrcPtr and copies it aCount times
to aDestPtr 32 bpp buffer.
@internalComponent
@pre aCount > 0
@param aSrcPtr It points to the source pixel
@param aDestPtr It points to the destination buffer where the pixel should be copied aCount times
@param aCount How many times the pixel value should be copied
@return The updated aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E3bpp, E4bpp>::CopyBlockPixel(TUint8* aDestPtr, 
												 const TUint8* aSrcPtr, 
												 TInt aCount) const
	{
	while(aCount--)
		{
		*aDestPtr++ = *aSrcPtr++;
		*aDestPtr++ = *aSrcPtr++;
		*aDestPtr++ = *aSrcPtr++;
		*aDestPtr++ = 0xff; //it is more accurate to equalize to 0xff, as top byte 
									//could correspond an alpha channel
		}
	return aDestPtr;
	}

/**
The method should never be executed.
Introduced to prevent CCover compiler from failing.
@internalComponent
@return The unchanged aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E2bpp, E2bpp>::CopyBlockPixel(TUint8* aDestPtr, 
												 const TUint8* /*aSrcPtr*/, 
												 TInt /*aCount*/) const
	{
	return aDestPtr;
	}

/**
The method should never be executed.
Introduced to prevent CCover compiler from failing.
@internalComponent
@return The unchanged aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E3bpp, E3bpp>::CopyBlockPixel(TUint8* aDestPtr, 
												 const TUint8* /*aSrcPtr*/, 
												 TInt /*aCount*/) const
	{
	return aDestPtr;
	}

/**
The method should never be executed.
Introduced to prevent CCover compiler from failing.
@internalComponent
@return The unchanged aDestPtr
*/
template <> 
TUint8* TScanLineDecompressor<E4bpp, E4bpp>::CopyBlockPixel(TUint8* aDestPtr, 
												 const TUint8* /*aSrcPtr*/, 
												 TInt /*aCount*/) const
	{
	return aDestPtr;
	}
