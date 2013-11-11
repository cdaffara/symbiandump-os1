// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <s32file.h>
#include <fbs.h>
#include <bitmap.h>
#include "UTILS.H"
#include <e32hashtab.h>

GLREF_C void Panic(TFbsPanic aPanic);

void CBitwiseBitmap::DoInternalizeCompressedDataL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase,TBitmapfileCompression aCompression)
	{
	TInt destSizeInBytes = iHeader.iSizeInPixels.iHeight * iByteWidth;
	TUint8* destPtr = (TUint8*)aBase;

	// try to allocate memory for the compressed bitmap
	TUint8* decompressionBuffer = (TUint8*)User::Alloc(aSrceSizeInBytes);

	if (decompressionBuffer)
		{
		// read the compressed bitmap, then decompress
		CleanupStack::PushL(decompressionBuffer);
		aStream.ReadL(decompressionBuffer,aSrceSizeInBytes);
		switch (aCompression)
			{
		case EByteRLECompression:
			DoDecompressByteData(destPtr,destSizeInBytes,decompressionBuffer,aSrceSizeInBytes);
			break;
		case ETwelveBitRLECompression:
			DoDecompressTwelveBitData(destPtr,destSizeInBytes,decompressionBuffer,aSrceSizeInBytes);
			break;
		case ESixteenBitRLECompression:
			DoDecompressSixteenBitData(destPtr,destSizeInBytes,decompressionBuffer,aSrceSizeInBytes);
			break;
		case ETwentyFourBitRLECompression:
			DoDecompressTwentyFourBitData(destPtr,destSizeInBytes,decompressionBuffer,aSrceSizeInBytes);
			break;
		case EThirtyTwoUBitRLECompression:
			DoDecompressThirtyTwoUBitData(destPtr,destSizeInBytes,decompressionBuffer,aSrceSizeInBytes);
			break;
		case EThirtyTwoABitRLECompression:
			DoDecompressThirtyTwoABitData(destPtr,destSizeInBytes,decompressionBuffer,aSrceSizeInBytes);
			break;
		default:
			break;
			}
		CleanupStack::PopAndDestroy(); // decompressionBuffer
		return;
		}

	// not enough heap to pre-load the source bitmap
	switch (aCompression)
		{
	case EByteRLECompression:
		DoDecompressByteDataAltL(aStream,aSrceSizeInBytes,aBase);
		break;
	case ETwelveBitRLECompression:
		DoDecompressTwelveBitDataAltL(aStream,aSrceSizeInBytes,aBase);
		break;
	case ESixteenBitRLECompression:
		DoDecompressSixteenBitDataAltL(aStream,aSrceSizeInBytes,aBase);
		break;
	case ETwentyFourBitRLECompression:
		DoDecompressTwentyFourBitDataAltL(aStream,aSrceSizeInBytes,aBase);
		break;
	case EThirtyTwoUBitRLECompression:
		DoDecompressThirtyTwoUBitDataAltL(aStream,aSrceSizeInBytes,aBase);
		break;
	case EThirtyTwoABitRLECompression:
		DoDecompressThirtyTwoABitDataAltL(aStream,aSrceSizeInBytes,aBase);
		break;
	default:
		break;
		}
	}

void CBitwiseBitmap::DoDecompressByteData(TUint8* aDestBuffer,TInt aDestSize,TUint8* aSrceBuffer,TInt aSrceSize)
	{
	TUint8* srcePtr = aSrceBuffer;
	TUint8* destPtr = aDestBuffer;
	TUint8* srcePtrLimit = aSrceBuffer + aSrceSize;
	TUint8* destPtrLimit = aDestBuffer + aDestSize;

	while (srcePtr < srcePtrLimit && destPtr < destPtrLimit)
		{
		TInt8 count = *srcePtr++;

		if (count >= 0)
			{
			const TInt numBytes = Min(count + 1, destPtrLimit - destPtr);
			Mem::Fill(destPtr,numBytes,*srcePtr++);
			destPtr += numBytes;
			}
		else
			{
			const TInt numBytes = Min(-count, destPtrLimit - destPtr);
			Mem::Copy(destPtr,srcePtr,numBytes);
			srcePtr += numBytes;
			destPtr += numBytes;
			}
		}
	__ASSERT_DEBUG(srcePtr == srcePtrLimit && destPtr == destPtrLimit,Panic(EFbsBitmapDecompressionError));
	}

void CBitwiseBitmap::DoDecompressByteDataAltL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase)
	{
	TInt destSizeInBytes = iHeader.iSizeInPixels.iHeight * iByteWidth;
	TUint8* destPtr = (TUint8*)aBase;
	TUint8* destPtrLimit = destPtr + destSizeInBytes;

	while(aSrceSizeInBytes > 0 && destPtr < destPtrLimit)
		{
		TInt8 count = aStream.ReadInt8L();
		aSrceSizeInBytes--;

		if (count >= 0)
			{
			const TInt numBytes = Min(count + 1, destPtrLimit - destPtr);
			TUint8 value = aStream.ReadUint8L();
			aSrceSizeInBytes--;
			Mem::Fill(destPtr,numBytes,value);
			destPtr += numBytes;
			}
		else
			{
			const TInt numBytes = Min(-count, destPtrLimit - destPtr);
			aStream.ReadL(destPtr,numBytes);
			aSrceSizeInBytes -= numBytes;
			destPtr += numBytes;
			}
		}
	__ASSERT_DEBUG(aSrceSizeInBytes == 0 && destPtr == destPtrLimit,Panic(EFbsBitmapDecompressionError));
	}

/** Fills the 32bit pixels into the destination pointer
This method uses the concept of Duff's Device 
@param aDestPtr32 pointer to 32bit destination buffer.
@param aCount Number of 32bit pixels to be copied.
@param aValue32 32bit pixel data. */
void CBitwiseBitmap::BitmapFill32(TUint32* aDestPtr32, TInt aCount, TUint32 aValue32)
	{
	__ASSERT_DEBUG(aCount > 0, Panic(EFbsBitmapDecompressionError));
	if (aCount > 0)
		{ // for performance loop is unrolled, using "Duff's Device" pattern
		TInt blocksOf16 = aCount / 16;	// number of blocks of 16 full words to write
		// the first iteration writes 1 to 15 words

		switch (aCount & 0x0f)
			{ // note that case statements intentionally cascade
		case 0:
write16dblPixels:	// second and subsequent iterations always write 16 words
			--blocksOf16;
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 15:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 14:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 13:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 12:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 11:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 10:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 9:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 8:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 7:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 6:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 5:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 4:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 3:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 2:
			*aDestPtr32++ = aValue32;
			//coverity [fallthrough]
		case 1:
			*aDestPtr32++ = aValue32;
			}

		if (blocksOf16 > 0)
			{
			goto write16dblPixels;
			}
		}
	}

/** Fills the 16bit pixels into the destination pointer
This method uses the concept of Duff's Device 
@param aDestPtr16 pointer to 16bit destination buffer.
@param aCount Number of 16bit pixels to be copied.
@param aValue16 16bit pixel data. */
inline void CBitwiseBitmap::BitmapFill16(TUint16* aDestPtr16, TInt aCount, TUint16 aValue16)
	{
	// Call the 32-bit fill method if there at least 8 pixels to fill
	if (aCount >= 8)
		{
		// first pixel is on half word address?
		if (reinterpret_cast<TUint32>(aDestPtr16) & 2) 
			{
			*aDestPtr16++ = aValue16; 
			--aCount; 
			} 

		// destPtr16 is now full Word aligned
		const TInt countDoublePixels = aCount / 2;
		TUint32* destPtr32 = (TUint32*) aDestPtr16;
		BitmapFill32(destPtr32, countDoublePixels, (aValue16 << 16) | aValue16);

		aCount -= countDoublePixels * 2;
		if (aCount == 1)
			{ // last pixel is on a half-word
			aDestPtr16 += countDoublePixels * 2;
			*aDestPtr16++ = aValue16; 
			}
		}
	else
		{ // less than 8 pixels to fill
		switch (aCount)
			{ // note that case statements intentionally cascade
		case 7:
			*aDestPtr16++ = aValue16; 
		case 6:
			*aDestPtr16++ = aValue16; 
		case 5:
			*aDestPtr16++ = aValue16; 
		case 4:
			*aDestPtr16++ = aValue16; 
		case 3:
			*aDestPtr16++ = aValue16; 
		case 2:
			*aDestPtr16++ = aValue16; 
		case 1:
			*aDestPtr16++ = aValue16; 
		case 0: // nothing to do
			;
			}
		}
	}

void CBitwiseBitmap::DoDecompressTwelveBitData(TUint8* aDestBuffer,TInt aDestSize,TUint8* aSrceBuffer,TInt aSrceSize)
	{
	TUint16* srcePtr = (TUint16*)aSrceBuffer;
	TUint16* destPtr = (TUint16*)aDestBuffer;
	TUint16* srcePtrLimit = srcePtr + (aSrceSize / 2);
	TUint16* destPtrLimit = destPtr + (aDestSize / 2);

	while (srcePtr < srcePtrLimit && destPtr < destPtrLimit)
		{
		TUint16 value = *srcePtr++;
		TInt count = Min(value >> 12, destPtrLimit - destPtr);
		value &= 0x0fff;

		while (count >= 0)
			{
			*destPtr++ = value;
			count--;
			}
		}

	__ASSERT_DEBUG(srcePtr == srcePtrLimit && destPtr == destPtrLimit,Panic(EFbsBitmapDecompressionError));
	}

void CBitwiseBitmap::DoDecompressTwelveBitDataAltL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase)
	{
	TInt destSizeInBytes = iHeader.iSizeInPixels.iHeight * iByteWidth;
	TUint16* destPtr = (TUint16*)aBase;
	TUint16* destPtrLimit = destPtr + (destSizeInBytes / 2);

	while(aSrceSizeInBytes > 0 && destPtr < destPtrLimit)
		{
		TUint16 value = aStream.ReadUint16L();
		TInt count = Min(value >> 12, destPtrLimit - destPtr);
		value &= 0x0fff;
		aSrceSizeInBytes -= 2;

		while (count >= 0)
			{
			*destPtr++ = value;
			count--;
			}
		}
	__ASSERT_DEBUG(aSrceSizeInBytes == 0 && destPtr == destPtrLimit,Panic(EFbsBitmapDecompressionError));
	}

/** The function decodes 24-bit compressed buffer to the 16-bit stream with unused top bytes by using RLE compression algorithm*/
void CBitwiseBitmap::DoDecompressSixteenBitData(TUint8* aDestBuffer,TInt aDestSize,TUint8* aSrceBuffer,TInt aSrceSize)
	{
	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aDestBuffer) & 0x2)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is half-word aligned
	__ASSERT_DEBUG((aDestSize & 0x2)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is half-word aligned

	TUint8* srcePtr8 = aSrceBuffer; // pointer to compressed source Byte(8-bit) stream 
	TUint16* destPtr16 = reinterpret_cast<TUint16*>(aDestBuffer); //pointer to uncompressed destination HalfWord(16-bit) pixel stream
	TUint8* srcePtrLimit8 = aSrceBuffer + aSrceSize; //maximum number of compressed source bytes
	TUint16* destPtrLimit16 = reinterpret_cast<TUint16*>(aDestBuffer + aDestSize); //maximum number of uncompressed destination pixel stream
	const TInt KPixelSize = 2; //number of bytes of the source stream that is to be considered as one Pixel
	
	while (srcePtr8 < srcePtrLimit8 && destPtr16 < destPtrLimit16)
		{
		TInt8 count = *srcePtr8++; //number of pixels to be retrieved from the source stream

		if (count >= 0) //repeating number of pixels
			{
			const TInt numPixels = 1 + Min(count, (destPtrLimit16 - destPtr16));
			const TUint8 lowByte = *srcePtr8++;
			const TUint8 highByte = *srcePtr8++;
			const TUint16 pixel = highByte<<8 | lowByte; //Pixel buffer which needs to be stored in destPtr16

			BitmapFill16(destPtr16, numPixels, pixel);
			destPtr16 += numPixels;
			}
		else
			{
			const TInt numPixels = Min(-count, destPtrLimit16 - destPtr16); //number of pixels to be copied
			const TInt numBytes = numPixels * KPixelSize; //number of bytes needs to be retrieved from the srcePtr
			Mem::Copy(destPtr16, srcePtr8, numBytes); // copy bytes
			srcePtr8 += numBytes; //incrementing the srcePtr by number of bytes
			destPtr16 += numPixels; //incrementing the destPtr16 by number of pixels
			}
		}
	__ASSERT_DEBUG(srcePtr8 == srcePtrLimit8 && destPtr16 == destPtrLimit16, Panic(EFbsBitmapDecompressionError));
	}

/** The alternative decoding function which decompresses 24-bit buffer to the 16-bit stream with unused top bytes 
 	by using RLE compression algorithm. The function is used under low memory conditions. */
void CBitwiseBitmap::DoDecompressSixteenBitDataAltL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase)
	{
	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aBase) & 0x2)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is half-word aligned

	TInt destSizeInPixels = (iHeader.iSizeInPixels.iHeight * iByteWidth); //size of destination Byte pixel buffers
	destSizeInPixels >>= 1; //dividing by two, to get size of destination interms of Halfword pixel buffers
	TUint16* destPtr16 = reinterpret_cast<TUint16*>(aBase); //pointer to uncompressed destination HalfWord(16-bit) pixel stream
	TUint16* destPtrLimit16 = destPtr16 + destSizeInPixels; //maximum number of compressed source bytes
	const TInt KPixelSize = 2; //number of bytes of the source stream that is to be considered as one Pixel

	while(aSrceSizeInBytes > 0 && destPtr16 < destPtrLimit16)
		{
		TInt8 count = aStream.ReadInt8L(); //number of pixels to be retrieved from the source stream
		aSrceSizeInBytes--; //One byte from source stream read, hence reduce size by one Byte

		if (count >= 0)
			{
			const TInt numPixels = 1 + Min(count, (destPtrLimit16 - destPtr16));
			const TUint16 pixel = aStream.ReadUint16L(); //Pixel buffer which needs to be stored in destPtr16
			aSrceSizeInBytes -= KPixelSize; //Two bytes (Halfword pixel) read, reduce size by two Bytes
			
			BitmapFill16(destPtr16, numPixels, pixel);
			destPtr16 += numPixels;
			}
		else
			{
			const TInt numPixels = Min(-count, destPtrLimit16 - destPtr16); //number of pixels to be stored into destPtr16
			const TInt numBytes = numPixels * KPixelSize; //number of bytes needs to be retrieved from the srcePtr
			aStream.ReadL(destPtr16, numPixels); //read TUint16's
			aSrceSizeInBytes -= numBytes; //two-byte (halfword) pixel buffers read, reduce by number of bytes
			destPtr16 += numPixels; //incrementing the destPtr16 by number of pixels
			}
		}
	__ASSERT_DEBUG(aSrceSizeInBytes == 0 && destPtr16 == destPtrLimit16, Panic(EFbsBitmapDecompressionError));
	}

void CBitwiseBitmap::DoDecompressTwentyFourBitData(TUint8* aDestBuffer,TInt aDestSize,TUint8* aSrceBuffer,TInt aSrceSize)
	{
	TUint8* srcePtr = aSrceBuffer;
	TUint8* destPtr = aDestBuffer;
	TUint8* srcePtrLimit = aSrceBuffer + aSrceSize;
	TUint8* destPtrLimit = aDestBuffer + aDestSize;

	while (srcePtr < srcePtrLimit && destPtr < destPtrLimit)
		{
		TInt8 count = *srcePtr++;

		if (count >= 0)
			{
			count = Min(count, (destPtrLimit - destPtr) / 3);
			TUint8 component1 = *srcePtr++;
			TUint8 component2 = *srcePtr++;
			TUint8 component3 = *srcePtr++;

			while (count >= 0)
				{
				*destPtr++ = component1;
				*destPtr++ = component2;
				*destPtr++ = component3;
				count--;
				}
			}
		else
			{
			const TInt numBytes = Min(count * -3, destPtrLimit - destPtr);
			Mem::Copy(destPtr,srcePtr,numBytes);
			srcePtr += numBytes;
			destPtr += numBytes;
			}
		}
	__ASSERT_DEBUG(srcePtr == srcePtrLimit && destPtr == destPtrLimit,Panic(EFbsBitmapDecompressionError));
	}

void CBitwiseBitmap::DoDecompressTwentyFourBitDataAltL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase)
	{
	TInt destSizeInBytes = iHeader.iSizeInPixels.iHeight * iByteWidth;
	TUint8* destPtr = (TUint8*)aBase;
	TUint8* destPtrLimit = destPtr + destSizeInBytes;

	while(aSrceSizeInBytes > 0 && destPtr < destPtrLimit)
		{
		TInt8 count = aStream.ReadInt8L();
		aSrceSizeInBytes--;

		if (count >= 0)
			{
			count = Min(count, (destPtrLimit - destPtr) / 3);
			TUint8 component1 = aStream.ReadUint8L();
			TUint8 component2 = aStream.ReadUint8L();
			TUint8 component3 = aStream.ReadUint8L();
			aSrceSizeInBytes -= 3;

			while (count >= 0)
				{
				*destPtr++ = component1;
				*destPtr++ = component2;
				*destPtr++ = component3;
				count--;
				}
			}
		else
			{
			const TInt numBytes = Min(count * -3, destPtrLimit - destPtr);
			aStream.ReadL(destPtr, numBytes);
			aSrceSizeInBytes -= numBytes;
			destPtr += numBytes;
			}
		}
	__ASSERT_DEBUG(aSrceSizeInBytes == 0 && destPtr == destPtrLimit,Panic(EFbsBitmapDecompressionError));
	}

/** The function decodes 24-bit compressed buffer to the 32-bit stream with unused top bytes by using RLE compression algorithm*/
void CBitwiseBitmap::DoDecompressThirtyTwoUBitData(TUint8* aDestBuffer,TInt aDestSize,TUint8* aSrceBuffer,TInt aSrceSize)
	{
	
	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aDestBuffer) & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is word aligned
	__ASSERT_DEBUG((aDestSize & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the size is whole no of pixels
	
	TUint8* srcePtr8 = aSrceBuffer;
	TUint8* srcePtrLimit8 = aSrceBuffer + aSrceSize;

	TUint32* destPtr32 = reinterpret_cast<TUint32*>(aDestBuffer);
	TUint32* destPtrLimit32 = reinterpret_cast<TUint32*>(aDestBuffer + aDestSize);

	while (srcePtr8 < srcePtrLimit8 && destPtr32 < destPtrLimit32)
		{
		
		TInt8 count = *srcePtr8++; // important to read into a byte variable

		if (count >= 0) //repeating pixel buffer
			{
			count  = Min(count, (destPtrLimit32 - destPtr32));
			const TUint8 component1 = *srcePtr8++;
			const TUint8 component2 = *srcePtr8++;
			const TUint8 component3 = *srcePtr8++;

			const TUint32 pixel = 0xff000000 | (component3<<16) | (component2<<8) | component1;

			// for performance loop is unrolled, using "Duff's Device" pattern
			TInt blocksOf16 = (count >> 4);	// number of blocks of 16 full words to write
			// the first iteration writes 1 to 15 words
			switch (count & 0x0f)
				{ // note that case statements intentionally cascade
			case 15:
				do {	// second and subsequent iterations always write 16 words
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
			case 0:
				*destPtr32++ = pixel;
 
				} while(0 <= --blocksOf16);
				}
			}
		else // negative value corresponds non repeating pixel buffer
			{
			const TInt numPixel = Min(-count, destPtrLimit32 - destPtr32) ;
			for(TInt ii = 0; ii < numPixel; ii++)
				{
				TUint8 component1 = *srcePtr8++;
				TUint8 component2 = *srcePtr8++;
				TUint8 component3 = *srcePtr8++;

				*destPtr32++ =  0xff000000 | (component3<<16) | (component2<<8) | component1;
				}
			}
		}

	__ASSERT_DEBUG(srcePtr8 == srcePtrLimit8 && destPtr32 == destPtrLimit32,Panic(EFbsBitmapDecompressionError));
	}

/** The alternative decoding function which decompresses 24-bit buffer to the 32-bit stream with unused top bytes 
	by using RLE compression algorithm. The function is using in case of memory shortage. */
void CBitwiseBitmap::DoDecompressThirtyTwoUBitDataAltL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase)
	{
	const TInt destSizeInDwords = iHeader.iSizeInPixels.iHeight * (iByteWidth>>2);
	TUint32* destPtr32 = aBase;
	TUint32* destPtrLimit32 = destPtr32 + destSizeInDwords;
	
	const TInt KMaxRLENonRepeatingPixelBufferSize = 128 * 3;
	TUint8 dataBuffer[KMaxRLENonRepeatingPixelBufferSize];

	while(aSrceSizeInBytes > 0 && destPtr32 < destPtrLimit32)
		{
		TInt8 count = aStream.ReadInt8L();
		aSrceSizeInBytes--;

		if (count >= 0) //repeating pixel buffer
			{
			count = Min(count, (destPtrLimit32 - destPtr32));
			const TUint8 component1 = aStream.ReadUint8L();
			const TUint8 component2 = aStream.ReadUint8L();
			const TUint8 component3 = aStream.ReadUint8L();
			aSrceSizeInBytes -= 3;

			const TUint32 pixel = 0xff000000 | (component3<<16) | (component2<<8) | component1; 
			
			TInt blocksOf16 = (count >> 4);	// number of blocks of 16 full words to write
			// the first iteration writes 1 to 15 words
			switch (count & 0x0f)
				{ // note that case statements intentionally cascade
			case 15:
				do {	// second and subsequent iterations always write 16 words
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
			case 0:
 				*destPtr32++ = pixel;

				} while(0 <= --blocksOf16);
				}
			}
		else // negative value corresponds non repeating pixel buffer
			{
			const TInt numDestDwords = Min(-count, destPtrLimit32 - destPtr32);
			const TInt numSrcBytes = numDestDwords * 3;
			aStream.ReadL(&dataBuffer[0], numSrcBytes);
			aSrceSizeInBytes -= numSrcBytes;
			TUint8* srcPtr = dataBuffer;
			const TInt& numPixel = numDestDwords;
			for(TInt ii = 0; ii < numPixel; ii++)
				{
				const TUint8 component1 = *srcPtr++;
				const TUint8 component2 = *srcPtr++;
				const TUint8 component3 = *srcPtr++;

				*destPtr32++ = 0xff000000 | (component3<<16) | (component2<<8) | component1; 
				}
			}
		}

	__ASSERT_DEBUG(aSrceSizeInBytes == 0 && destPtr32 == destPtrLimit32,Panic(EFbsBitmapDecompressionError));
	}

/** The function decodes 32-bit compressed buffer (where top bytes are used as alpha channel) to the 32-bit stream by using RLE compression algorithm*/
void CBitwiseBitmap::DoDecompressThirtyTwoABitData(TUint8* aDestBuffer,TInt aDestSize,TUint8* aSrceBuffer,TInt aSrceSize)
	{
	TUint8* srcePtr = aSrceBuffer;
	TUint8* srcePtrLimit = aSrceBuffer + aSrceSize;

	TUint32* destPtr32 = reinterpret_cast<TUint32*>(aDestBuffer);
	TUint32* destPtrLimit32 = reinterpret_cast<TUint32*>(aDestBuffer + aDestSize);

	while (srcePtr < srcePtrLimit && destPtr32 < destPtrLimit32)
		{
		TInt8 count = *srcePtr++;

		if (count >= 0) //repeating pixel buffer
			{
			count = Min(count, (destPtrLimit32 - destPtr32));
			TUint8 component1 = *srcePtr++;
			TUint8 component2 = *srcePtr++;
			TUint8 component3 = *srcePtr++;
			TUint8 component4 = *srcePtr++;
			const TUint32 pixel = (component4<<24) | (component3<<16) | (component2<<8) | component1;

			// for performance loop is unrolled, using "Duff's Device" pattern
			TInt blocksOf16 = (count >> 4);	// number of blocks of 16 full words to write
			// the first iteration writes 1 to 15 words
			switch (count & 0x0f)
				{ // note that case statements intentionally cascade
			case 15:
				do {	// second and subsequent iterations always write 16 words
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
			case 0:
 				*destPtr32++ = pixel;

				} while(0 <= --blocksOf16);
				}
			}
		else // negative value corresponds non repeating pixel buffer
			{
			const TInt numPixel = Min(-count, destPtrLimit32 - destPtr32);

			Mem::Copy(destPtr32, srcePtr, numPixel*4); 
			destPtr32 += numPixel;
			srcePtr += numPixel*4;

			}
		}

	__ASSERT_DEBUG(srcePtr == srcePtrLimit && destPtr32 == destPtrLimit32,Panic(EFbsBitmapDecompressionError));
	}

/** The alternative decoding function which decompresses 32-bit buffer (where top bytes are used as alpha channel)
	to the 32-bit stream by using RLE compression algorithm. The function is using in case of memory shortage. */
void CBitwiseBitmap::DoDecompressThirtyTwoABitDataAltL(RReadStream& aStream,TInt aSrceSizeInBytes,TUint32* aBase)
	{
	const TInt destSizeInDwords = iHeader.iSizeInPixels.iHeight * (iByteWidth>>2);
	TUint32* destPtr32 = aBase;
	TUint32* destPtrLimit32 = destPtr32 + destSizeInDwords;

	while(aSrceSizeInBytes > 0 && destPtr32 < destPtrLimit32)
		{
		TInt8 count = aStream.ReadInt8L();
		aSrceSizeInBytes--;

		if (count >= 0) //repeating pixel buffer
			{
			count = Min(count, (destPtrLimit32 - destPtr32));
			const TUint32 pixel = aStream.ReadUint32L();
			aSrceSizeInBytes -= 4;

			// for performance loop is unrolled, using "Duff's Device" pattern
			TInt blocksOf16 = (count >> 4);	// number of blocks of 16 full words to write
			// the first iteration writes 1 to 15 words
			switch (count & 0x0f)
				{ // note that case statements intentionally cascade
			case 15:
				do {	// second and subsequent iterations always write 16 words
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
			case 0:
 				*destPtr32++ = pixel;

				} while(0 <= --blocksOf16);
				}
			}
		else // negative value corresponds non repeating pixel buffer
			{
			const TInt numPixels = Min(-count, destPtrLimit32 - destPtr32);
			aStream.ReadL((TUint16*)destPtr32, numPixels * 2); // read TUint16's
			aSrceSizeInBytes -= numPixels * 4;
			destPtr32 += numPixels;
			}
		}

	__ASSERT_DEBUG(aSrceSizeInBytes == 0 && destPtr32 == destPtrLimit32,Panic(EFbsBitmapDecompressionError));
	}

void CBitwiseBitmap::DoExternalizeDataCompressedL(RWriteStream& aStream,TUint8* aData,TInt aSizeInBytes) const
	{
	switch (iHeader.iBitsPerPixel)
		{
	case 1:
	case 2:
	case 4:
	case 8:
		DoExternalizeByteDataCompressedL(aStream,aData,aSizeInBytes);
		break;
	case 12:
		DoExternalizeTwelveBitDataCompressedL(aStream,aData,aSizeInBytes);
		break;
	case 16:
		DoExternalizeSixteenBitDataCompressedL(aStream,aData,aSizeInBytes);
		break;
	case 24:
		DoExternalizeTwentyFourBitDataCompressedL(aStream,aData,aSizeInBytes);
		break;
	case 32:
		__ASSERT_DEBUG(iHeader.iColor == SEpocBitmapHeader::EColor||
						iHeader.iColor == SEpocBitmapHeader::EColorAlpha||
						iHeader.iColor == SEpocBitmapHeader::EColorAlphaPM,
						Panic(EFbsBitmapInvalidCompression));
		if(iHeader.iColor == SEpocBitmapHeader::EColor)
			{
			DoExternalizeThirtyTwoUBitDataCompressedL(aStream,aData,aSizeInBytes);
			}
		else
			{
			DoExternalizeThirtyTwoABitDataCompressedL(aStream,aData,aSizeInBytes);
			}
		break;
	default:
		break;
		}
	}

void CBitwiseBitmap::DoExternalizeByteDataCompressedL(RWriteStream& aStream,TUint8* aData,TInt aSizeInBytes) const
	{
	TUint8* dataLimit=aData+aSizeInBytes-2;
	TUint8 runPair[2];
	while(aData<dataLimit)
		{
		TUint8 value=*aData;
		TUint8* runStartPtr = aData;

		if (*(aData+1)==value && *(aData+2)==value)
			{
			aData+=3;
			while(aData<dataLimit && *aData==value)
				aData++;
			TInt runLength=aData-runStartPtr;
			runPair[0]=127;
			runPair[1]=value;
			while(runLength>128)
				{
				aStream.WriteL(&runPair[0],2);
				runLength-=128;
				}
			runPair[0]=TUint8(runLength-1);
			aStream.WriteL(&runPair[0],2);
			}
		else
			{
			while(aData<dataLimit && (*(aData+1)!=value || *(aData+2)!=value))
				{
				aData++;
				value=*aData;
				}
			TInt runLength = aData - runStartPtr;
			while (runLength > 128)
				{
				aStream.WriteInt8L(-128);
				aStream.WriteL(runStartPtr,128);
				runLength-=128;
				runStartPtr+=128;
				}
			aStream.WriteInt8L(-runLength);
			aStream.WriteL(runStartPtr,runLength);
			}
		}
	dataLimit+=2;
	if (aData<dataLimit)
		{
		TInt runLength=dataLimit-aData;
		aStream.WriteInt8L(-runLength);
		aStream.WriteL(aData,runLength);
		}
	}

void CBitwiseBitmap::DoExternalizeTwelveBitDataCompressedL(RWriteStream& aStream,TUint8* aData,TInt aSizeInBytes) const
	{
	TUint16* srcePtr = (TUint16*)aData;
	TUint16* srcePtrLimit = srcePtr + (aSizeInBytes / 2);

	while (srcePtr < srcePtrLimit)
		{
		TUint16* runStartPtr = srcePtr;
		TUint16 value = TUint16(*srcePtr & 0x0fff);
		do
			{
			srcePtr++;
			}
		while (srcePtr < srcePtrLimit && *srcePtr == value);

		TInt pixelLength = srcePtr - runStartPtr;
		TUint16 maxLengthData = TUint16(value | 0xf000);

		while (pixelLength > 16)
			{
			aStream.WriteUint16L(maxLengthData);
			pixelLength -= 16;
			}

		if (pixelLength > 0)
			aStream.WriteUint16L(value | TUint16((pixelLength - 1) << 12));
		}
	}

void CBitwiseBitmap::DoExternalizeSixteenBitDataCompressedL(RWriteStream& aStream,TUint8* aData,TInt aSizeInBytes) const
	{
	TUint16* srcePtr = (TUint16*)aData;
	TUint16* srceLimitPtr = srcePtr + (aSizeInBytes / 2);
	TUint16* srceLimitPtrMinusOne = srceLimitPtr - 1;

	while (srcePtr < srceLimitPtrMinusOne)
		{
		TUint16 value = *srcePtr;
		TUint16* runStartPtr = srcePtr++;

		if(*srcePtr == value)
			{
			do
				{
				srcePtr++;
				}
			while(srcePtr < srceLimitPtr && *srcePtr == value);

			TInt pixelLength = srcePtr-runStartPtr;
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(127);
				aStream.WriteUint16L(value);
				pixelLength -= 128;
				}

			aStream.WriteUint8L(pixelLength - 1);
			aStream.WriteUint16L(value);
			}
		else
			{
			value = *srcePtr;
			while (srcePtr < srceLimitPtrMinusOne && *(srcePtr + 1) != value)
				{
				srcePtr++;
				value = *srcePtr;
				}

			TInt pixelLength = srcePtr - runStartPtr;
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(-128);
				aStream.WriteL(runStartPtr,128);
				runStartPtr += 128;
				pixelLength -= 128;
				}

			aStream.WriteInt8L(-pixelLength);
			aStream.WriteL(runStartPtr,pixelLength);
			}
		}

	TInt remainingPixels = srceLimitPtr - srcePtr;
	if (remainingPixels > 0)
		{
		aStream.WriteInt8L(-remainingPixels);
		aStream.WriteL(srcePtr,remainingPixels);
		}
	}

void CBitwiseBitmap::DoExternalizeTwentyFourBitDataCompressedL(RWriteStream& aStream,TUint8* aData,TInt aSizeInBytes) const
	{
	TUint8* srceLimitPtr = aData + aSizeInBytes;
	TUint8* srceLimitPtrMinusThree = srceLimitPtr - 3; // three bytes == one pixel

	while (aData < srceLimitPtrMinusThree)
		{
		TUint8* runStartPtr = aData;
		TUint8 component1 = *aData++;
		TUint8 component2 = *aData++;
		TUint8 component3 = *aData++;

		if (TrueColorPointerCompare(aData,component1,component2,component3))
			{
			do
				{
				aData += 3;
				}
			while (aData < srceLimitPtr && TrueColorPointerCompare(aData,component1,component2,component3));

			TInt pixelLength = (aData - runStartPtr) / 3;
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(127);
				aStream.WriteUint8L(component1);
				aStream.WriteUint8L(component2);
				aStream.WriteUint8L(component3);
				pixelLength -= 128;
				}

			aStream.WriteInt8L(pixelLength - 1);
			aStream.WriteUint8L(component1);
			aStream.WriteUint8L(component2);
			aStream.WriteUint8L(component3);
			}
		else
			{
			TBool more  = ETrue;
			TBool eqRun = EFalse;
			do
				{
				component1 = *aData++;
				component2 = *aData++;
				component3 = *aData++;
				more = (aData < srceLimitPtr);
				eqRun = more && TrueColorPointerCompare(aData,component1,component2,component3);
				}
			while (more && !eqRun);
			if (eqRun)
				aData -= 3;
			TInt pixelLength = (aData - runStartPtr) / 3;
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(-128);
				aStream.WriteL(runStartPtr,384);
				runStartPtr += 384;
				pixelLength -= 128;
				}

			aStream.WriteInt8L(-pixelLength);
			aStream.WriteL(runStartPtr,pixelLength * 3);
			}
		}

	TInt remainingPixels = srceLimitPtr - aData;
	if (remainingPixels > 0)
		{
		TInt pixelLength = remainingPixels / 3;
		aStream.WriteInt8L(-pixelLength);
		aStream.WriteL(aData,remainingPixels);
		}
	}
	
/** The function externalizes 32-bit buffer with unused top bytes to the 24-bit compressed stream by using RLE compression algorithm*/
void CBitwiseBitmap::DoExternalizeThirtyTwoUBitDataCompressedL(RWriteStream& aStream,TUint8* aData8,TInt aSizeInBytes) const
	{

	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aData8) & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is word aligned
	__ASSERT_DEBUG((aSizeInBytes & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the size is whole no of pixels

	TUint32* ptr = reinterpret_cast<TUint32*>(aData8);
	TUint32* srceLimitPtr = reinterpret_cast<TUint32*>(aData8 + aSizeInBytes);
	TUint32* srceLimitPtr2ndLast = srceLimitPtr - 1; 

	while (ptr < srceLimitPtr2ndLast)
		{
		TUint32* runStartPtr = ptr;
		TUint32 pixel = *ptr++ & 0x00ffffff;

		if ( (((*ptr)&0x00ffffff)==pixel) )
			{
			do
				{
				ptr++;
				}
			while (ptr< srceLimitPtr && (((*ptr)&0x00ffffff)==pixel) );

			TInt pixelLength = (ptr - runStartPtr);
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(127);
				aStream.WriteUint8L(pixel&0x000000ff);
				aStream.WriteUint8L((pixel>>8)&0x000000ff);
				aStream.WriteUint8L((pixel>>16)&0x000000ff);
				pixelLength -= 128;
				}

			aStream.WriteInt8L(pixelLength - 1);
			aStream.WriteUint8L(pixel&0x000000ff);
			aStream.WriteUint8L((pixel>>8)&0x000000ff);
			aStream.WriteUint8L((pixel>>16)&0x000000ff);
			}
		else
			{
			TBool more  = ETrue;
			TBool eqRun = EFalse;
			do
				{
				pixel = *ptr++ & 0x00ffffff;
				more = (ptr < srceLimitPtr);
				eqRun = more && (((*ptr)&0x00ffffff)==pixel);
				} while (more && !eqRun);
			if (eqRun)
				ptr--;
			TInt pixelLength = (ptr - runStartPtr);
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(-128);
				for(TInt ii = 0; ii < 128; ii++)
					{
					aStream.WriteL(reinterpret_cast<TUint8*>(runStartPtr), 3);
					runStartPtr++;
					}
				pixelLength -= 128;
				}

			aStream.WriteInt8L(-pixelLength);
			for(TInt kk = 0; kk < pixelLength; kk++)
				{
				aStream.WriteL(reinterpret_cast<TUint8*>(runStartPtr), 3);
				runStartPtr++;
				}
			}
		}

	const TInt remainingPixels = srceLimitPtr - ptr;
	if (remainingPixels > 0)
		{
		__ASSERT_DEBUG(remainingPixels == 1, Panic(EFbsBitmapDecompressionError));

		aStream.WriteInt8L(-remainingPixels);
		for(TInt ii = 0; ii < remainingPixels; ii++)
			{
			aStream.WriteL(reinterpret_cast<TUint8*>(ptr), 3);
			ptr++;
			}
		}
	}

/** The function externalizes 32-bit buffer with alpha channel in top byte to the 32-bit compressed stream by using RLE compression algorithm*/
void CBitwiseBitmap::DoExternalizeThirtyTwoABitDataCompressedL(RWriteStream& aStream,TUint8* aData8,TInt aSizeInBytes) const
	{

	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aData8) & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is word aligned
	__ASSERT_DEBUG((aSizeInBytes & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the size is whole no of pixels

	TUint32* ptr = reinterpret_cast<TUint32*>(aData8);
	TUint32* srceLimitPtr = reinterpret_cast<TUint32*>(aData8 + aSizeInBytes);
	TUint32* srceLimitPtr2ndLast = srceLimitPtr - 1; 

	while (ptr < srceLimitPtr2ndLast)
		{
		TUint32* runStartPtr = ptr;
		TUint32 pixel = *ptr++;

		if (*ptr==pixel)
			{
			do
				{
				ptr++;
				}
			while (ptr < srceLimitPtr && *ptr==pixel);

			TInt pixelLength = (ptr - runStartPtr);
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(127);
				aStream.WriteUint32L(pixel);
				pixelLength -= 128;
				}

			aStream.WriteInt8L(pixelLength - 1);
			aStream.WriteUint32L(pixel);
			}
		else
			{
			TBool more  = ETrue;
			TBool eqRun = EFalse;
			do
				{
				pixel = *ptr++;
				more = (ptr < srceLimitPtr);
				eqRun = more && *ptr==pixel;
				} while (more && !eqRun);
			
			if (eqRun)
				ptr--;
			
			TInt pixelLength = (ptr - runStartPtr);
			while (pixelLength > 128)
				{
				aStream.WriteInt8L(-128);
				aStream.WriteL(reinterpret_cast<TUint8*>(runStartPtr), 128*4);
				runStartPtr += 128;
				pixelLength -= 128;
				}

			aStream.WriteInt8L(-pixelLength);
			aStream.WriteL(reinterpret_cast<TUint8*>(runStartPtr), pixelLength*4);
			}
		}

	const TInt remainingPixels = srceLimitPtr - ptr;
	if (remainingPixels > 0)
		{
		__ASSERT_DEBUG(remainingPixels == 1, Panic(EFbsBitmapDecompressionError));

		aStream.WriteInt8L(-remainingPixels);
		aStream.WriteL(reinterpret_cast<TUint8*>(ptr), remainingPixels*4);
		}
	}

TInt CBitwiseBitmap::SizeOfDataCompressed(TUint8* aData,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	switch (iHeader.iBitsPerPixel)
		{
	case 1:
	case 2:
	case 4:
	case 8:
		return SizeOfByteDataCompressed(aData,aSizeInBytes);
	case 12:
		return SizeOfTwelveBitDataCompressed(aData,aSizeInBytes);
	case 16:
		return SizeOfSixteenBitDataCompressed(aData,aSizeInBytes);
	case 24:
		return SizeOfTwentyFourBitDataCompressed(aData,aSizeInBytes);
	case 32:
		__ASSERT_DEBUG(iHeader.iColor == SEpocBitmapHeader::EColor||
						iHeader.iColor == SEpocBitmapHeader::EColorAlpha||
						iHeader.iColor == SEpocBitmapHeader::EColorAlphaPM,
						Panic(EFbsBitmapInvalidCompression));
		if(iHeader.iColor == SEpocBitmapHeader::EColor)
			{
			return SizeOfThirtyTwoUBitDataCompressed(aData,aSizeInBytes);
			}
		else
			{
			return SizeOfThirtyTwoABitDataCompressed(aData,aSizeInBytes);
			}
	default:
		break;
		}

	return 0;
	}

TInt CBitwiseBitmap::SizeOfByteDataCompressed(TUint8* aData,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	TInt compressedSize=0;
	TUint8* dataLimit=aData+aSizeInBytes-2;
	
	while(aData<dataLimit)
		{
		TUint8 value=*aData;
		if (*(aData+1)==value && *(aData+2)==value)
			{
			TUint8* runStartPtr=aData;
			aData+=3;
			while(aData<dataLimit && *aData==value)
				aData++;
			TInt runLength=aData-runStartPtr;

			compressedSize+= 2*(((runLength-1)>>7) + 1) ;
			}
		else
			{
			TUint8* runStartPtr=aData;
			while(aData<dataLimit && (*(aData+1)!=value || *(aData+2)!=value))
				{
				aData++;
				value=*aData;
				}
			TInt runLength=aData-runStartPtr;
			
			compressedSize+= runLength + ((runLength-1)>>7) + 1;
			}
		}
	dataLimit+=2;
	if (aData<dataLimit)
		compressedSize+=dataLimit-aData+1;
	return(compressedSize);
	}

TInt CBitwiseBitmap::SizeOfTwelveBitDataCompressed(TUint8* aData,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	TInt compressedSize = 0;
	TUint16* srcePtr = (TUint16*)aData;
	TUint16* srcePtrLimit = srcePtr + (aSizeInBytes / 2);

	while (srcePtr < srcePtrLimit)
		{
		TUint16* runStartPtr = srcePtr;
		TUint16 value = TUint16(*srcePtr & 0x0fff);
		do
			{
			srcePtr++;
			}
		while (srcePtr < srcePtrLimit && *srcePtr == value);

		TInt pixelLength = srcePtr - runStartPtr;

		compressedSize += 2*( ((pixelLength-1)>>4) + 1);
		}
	return compressedSize;
	}

TInt CBitwiseBitmap::SizeOfSixteenBitDataCompressed(TUint8* aData,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	TInt compressedSize = 0;
	TUint16* srcePtr = (TUint16*)aData;
	TUint16* srceLimitPtr = srcePtr + (aSizeInBytes / 2);
	TUint16* srceLimitPtrMinusOne = srceLimitPtr - 1;

	while (srcePtr < srceLimitPtrMinusOne)
		{
		TUint16 value = *srcePtr;
		TUint16* runStartPtr = srcePtr++;

		if(*srcePtr == value)
			{
			do
				{
				srcePtr++;
				}
			while(srcePtr < srceLimitPtr && *srcePtr == value);

			TInt pixelLength = srcePtr-runStartPtr;

			compressedSize += 3*( ((pixelLength-1)>>7) + 1);
			}
		else
			{
			value = *srcePtr;
			while (srcePtr < srceLimitPtrMinusOne && *(srcePtr + 1) != value)
				{
				srcePtr++;
				value = *srcePtr;
				}

			TInt pixelLength = srcePtr-runStartPtr;

			compressedSize += (pixelLength * 2) + ((pixelLength-1)>>7) + 1;
			}
		}
	if (srcePtr < srceLimitPtr)
		compressedSize += ((srceLimitPtr - srcePtr) * 2) + 1;
	return compressedSize;
	}

TInt CBitwiseBitmap::SizeOfTwentyFourBitDataCompressed(TUint8* aData,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	TInt compressedSize = 0;
	TUint8* srceLimitPtr = aData + aSizeInBytes;
	TUint8* srceLimitPtrMinusThree = srceLimitPtr - 3; // three bytes == one pixel

	while (aData < srceLimitPtrMinusThree)
		{
		TUint8* runStartPtr = aData;
		TUint8 component1 = *aData++;
		TUint8 component2 = *aData++;
		TUint8 component3 = *aData++;

		if (TrueColorPointerCompare(aData,component1,component2,component3))
			{
			do
				{
				aData += 3;
				}
			while (aData < srceLimitPtr && TrueColorPointerCompare(aData,component1,component2,component3));

			TInt pixelLength = (aData - runStartPtr) / 3;
			
			compressedSize += 4*( ((pixelLength-1)>>7) + 1); 
			}
		else
			{
			TBool more  = ETrue;
			TBool eqRun = EFalse;
			do
				{
				component1 = *aData++;
				component2 = *aData++;
				component3 = *aData++;
				more = (aData < srceLimitPtr);
				eqRun = more && TrueColorPointerCompare(aData,component1,component2,component3);
				}
			while (more && !eqRun);
			if (eqRun)
				aData -= 3;
			TInt pixelLength = (aData - runStartPtr) / 3;

			compressedSize += (pixelLength * 3) + ((pixelLength-1)>>7) + 1;
			}
		}
	
	if (aData < srceLimitPtr)
		compressedSize += srceLimitPtr - aData + 1;

	return compressedSize;
	}

/** The function calculates the size of 24-bit RLE compression stream which could be obtain from given 32-bit buffer, 
	where the top bytes are unused*/
TInt CBitwiseBitmap::SizeOfThirtyTwoUBitDataCompressed(TUint8* aData8,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aData8) & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is word aligned
	__ASSERT_DEBUG((aSizeInBytes & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the size is whole no of pixels


	TInt compressedSize = 0;
	
	
	TUint32* ptr = reinterpret_cast<TUint32*>(aData8);
	TUint32* srceLimitPtr = reinterpret_cast<TUint32*>(aData8 + aSizeInBytes);
	TUint32* srceLimitPtr2ndLast = srceLimitPtr - 1; 
	while (ptr < srceLimitPtr2ndLast)
		{
		TUint32* runStartPtr = ptr;
		TUint32 pixel = *ptr++ & 0x00ffffff;

		if ((((*ptr)&0x00ffffff)==pixel))
			{
			do
				{
				ptr++;
				}
			while (ptr < srceLimitPtr && (((*ptr)&0x00ffffff)==pixel));

			TInt pixelLength = (ptr - runStartPtr);
			compressedSize += 4*( ((pixelLength-1)>>7) + 1); 
			}	
		else
			{
			TBool more  = ETrue;
			TBool eqRun = EFalse;
			do
				{
				pixel = *ptr++ & 0x00ffffff;
				more = (ptr < srceLimitPtr);
				eqRun = more && (((*ptr)&0x00ffffff)==pixel);
				} while (more && !eqRun);

			if (eqRun)
				ptr--;

			TInt pixelLength = (ptr - runStartPtr) ;
			compressedSize += 3*pixelLength + ((pixelLength-1)>>7) + 1; 
			}
		}

	if (ptr < srceLimitPtr)
		compressedSize += (srceLimitPtr - ptr)*3 + 1;

	return compressedSize;
	}


TBool CBitwiseBitmap::TrueColorPointerCompare(TUint8* aColorPointer,TUint8 aComponent1,TUint8 aComponent2,TUint8 aComponent3) const
	{
	return (*aColorPointer == aComponent1 && *(aColorPointer + 1) == aComponent2 && *(aColorPointer + 2) == aComponent3);
	}

/** This function calculates the size of 32-bit RLE compression stream which is obtained from a given 32-bit buffer, 
	where the top 8 bits are used to represent the alpha channel*/
TInt CBitwiseBitmap::SizeOfThirtyTwoABitDataCompressed(TUint8* aData8,TInt aSizeInBytes) const
	{
	if(aSizeInBytes<=0)
		return 0;

	__ASSERT_DEBUG((reinterpret_cast<TUint32>(aData8) & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the start address is word aligned
	__ASSERT_DEBUG((aSizeInBytes & 0x3)==0,Panic(EFbsBitmapDecompressionError)); // make sure the size is whole no of pixels


	TInt compressedSize = 0;
	
	
	TUint32* ptr = reinterpret_cast<TUint32*>(aData8);
	TUint32* srceLimitPtr = reinterpret_cast<TUint32*>(aData8 + aSizeInBytes);
	TUint32* srceLimitPtr2ndLast = srceLimitPtr - 1; 
	while (ptr < srceLimitPtr2ndLast)
		{
		TUint32* runStartPtr = ptr;
		TUint32 pixel = *ptr++;

		if (*ptr==pixel)
			{
			do
				{
				ptr++;
				}
			while (ptr < srceLimitPtr && *ptr==pixel);

			TInt pixelLength = (ptr - runStartPtr);
			compressedSize += 5*( ((pixelLength-1)>>7) + 1); 
			}	
		else
			{
			TBool more  = ETrue;
			TBool eqRun = EFalse;
			do
				{
				pixel = *ptr++;
				more = (ptr < srceLimitPtr);
				eqRun = more && *ptr==pixel;
				} while (more && !eqRun);

			if (eqRun)
				ptr--;

			TInt pixelLength = (ptr - runStartPtr) ;
			compressedSize += 4*pixelLength + ((pixelLength-1)>>7) + 1; 
			}
		}

	if (ptr < srceLimitPtr)
		compressedSize += (srceLimitPtr - ptr)*4 + 1;

	return compressedSize;
	}


/*	Here's a BNF description of the RLE encoding in use for all but the 12-bit compression:

	encoding := run*
	run := equal-run | unequal-run
	equal-run := <runlength - 1> pixel-value
	unequal-run := <-runlength> pixel-value+    (i.e. pixel-value repeated <runlength> times)
	runlength := uint8 in 1..128
	pixelvalue := byte-pixel | 16bit-pixel | 24bit-pixel | 32bit-u-pixel | 32bit-a-pixel
	byte-pixel := octet
	16bit-pixel := octet octet
	24bit-pixel := rr gg bb
	32bit-u-pixel := rr gg bb
	32bit-a-pixel := rr gg bb aa
	rr := octet  (red component)
	gg := octet  (green component)
	bb := octet  (blue component)
	aa := octet  (alpha channel)

	This scheme models runs of 2-128 equal pixels or of upto 127 unequal pixels.
	Obviously a run of 0 is meaningless.  A run of 1 is considered to be an 'unequal' run containing
	a single pixel.

	For 12-bit compression a different encoding scheme is used. Only equal-runs are written, and the
	run	length is encoded into the top 4 bits of a 16 bit word.  Here's a BNF for 12-bit compression:
	
	12-bit-encoding := run*
	run := equal-run
	equal-run := high-octet low-octet
	high-octet := [<runlength - 1> rr]
	low-octet := [gg bb]
	runlength := 1..16
	rr := quartet  (red component)
	gg := quartet  (green component)
	bb := quartet  (blue component)

*/

//Palette compression additions

/**
Attempts to compress a bitmap by reducing it to a palette + data.
If the compression fails, for any of the reasons detailed below, RLE compression will be attempted instead.
Prerequisites:
- Bitmap must not already be compressed.
- Bitmap must contain no more than 255 colors - If bitmap contains >255 colors then palette compression is unlikely to be effective.
- Bitmap must be 16, 24 or 32 (non alpha) bpp.  Other modes could be implemented, but smaller bit depths will yield less compression
Small bitmaps (under 1000 pixels) will be unlikely to compress well if at all
Structure of compressed bitmap will be as follows;
size of palette[4 bytes] | palette[size * 4 bytes per entry] | data[pixels * upto 1 byte per pixel]
Bitmap data is packed into memory as efficiently as possible, according to the number of bits required.
Each line of the compressed bitmap will start on a byte boundary, but not necessarily on a word boundary.
*/
TInt CBitwiseBitmap::PaletteCompress()
	{
	//Compression algorithm
	//1.Iterate bitmap data, building hash of unique colours found
	//2.Iterate the hash, and build linear array of the unique colours (this is the palette)
	//3.Iterate the array, and set the array index of each colour back into the hash
	//4.Iterate the bitmap data again, for each pixel replace with palette index found from hash
	
	//Bitmap must be uncompressed
	if (iHeader.iCompression != ENoBitmapCompression)
		return KErrNone;
	
	//There must be some data in the bitmap
	TUint8* base = REINTERPRET_CAST(TUint8*, DataAddress());
	if (!base)
		return KErrNone;
	
	//Get the bytes per pixel, and make sure it is a supported configuration
	TUint sourceBytesPerPixel = PaletteBytesPerPixel(iHeader.iBitsPerPixel);
	if (sourceBytesPerPixel == 0)
		return KErrNotSupported;
	
	//Create a hash table and give it a decent amount of RAM to start with
	RHashMap<TInt,TInt> colors;
	if (colors.Reserve(256) != KErrNone)
		return KErrNoMemory;

	//Loop the data area of the bitmap, one pixel (sourceBytesPerPixel bytes) at a time
	TInt numPixels = (iHeader.iSizeInPixels.iWidth * iHeader.iSizeInPixels.iHeight);
	//we need some pixels!
	if (numPixels == 0)
		{
		colors.Close();
		return KErrNone;
		}
	
	TUint8* top = base + (iHeader.iSizeInPixels.iHeight * iByteWidth) ; //address of lastmost byte
	TInt32 lineLengthInBytes = iHeader.iSizeInPixels.iWidth * sourceBytesPerPixel;  //The actual useful data on each line
	TInt32 lineLengthInBytesPadded = iByteWidth ; //the actual (aligned) length of each line
	TUint8* pixelAddress;
	TUint8* lineAddress;
	//Loop each line of the bitmap data.  Note that each line ends on a 32bit align in the source MBM data.
	TUint8* lineEnd=0;

	//Separate looping for 16 & 24+ bpp.  Adds a chunk of duplicated code but saves checking bpp for  every pixel
	if (iHeader.iBitsPerPixel < 24)
		{
		for (lineAddress=base;lineAddress<top;lineAddress+=iByteWidth)
			{
			//Loop each pixel within the line
			lineEnd = lineAddress + lineLengthInBytes;
			for (pixelAddress = lineAddress; pixelAddress < lineEnd; pixelAddress += sourceBytesPerPixel)
				{
				//Extract colour value
				TUint color = *pixelAddress;
				color |= (*(pixelAddress+1)) << 8;
				//Insert colour value into hash
				//Quicker to use Find() to eliminate duplicates Insert()
				if (colors.Find(color) == NULL)
					{
					if (colors.Insert(color,0) != KErrNone)
						{
						colors.Close();
						return KErrNoMemory;
						}
					}
				}
			//Test each line whether it's worth continuing to the end	
			//We aren't interested if there's more than 256 colours (RLE compression will probably do a better job in this case)
			if (colors.Count() > 256)
				{
				colors.Close();
				return KErrNotSupported ;
				}
			}
		}
	else //>=24 bpp
		{
		for (lineAddress = base; lineAddress<top; lineAddress += iByteWidth)
			{
			//Loop each pixel within the line
			lineEnd = lineAddress + lineLengthInBytes;
			for (pixelAddress = lineAddress; pixelAddress < lineEnd; pixelAddress += sourceBytesPerPixel)
				{
				//Extract colour value
				TUint color = *pixelAddress;
				color |= (*(pixelAddress+1)) << 8;
				color |= (*(pixelAddress+2)) << 16;
				//Insert colour value into hash
				//Quicker to use Find() to eliminate duplicates Insert()

				if (colors.Find(color) == NULL)
					{
					if (colors.Insert(color,0) != KErrNone)
						{
						colors.Close();
						return KErrNoMemory;
						}
					}
				}
			//Test each line whether it's worth continuing to the end
			//We aren't interested if there's more than 256 colours (RLE compression will probably do a better job in this case)
			if (colors.Count() > 256)
				{
				colors.Close();
				return KErrNotSupported;
				}
			} //for
		}//end if
	
	
	//Now we have enough data to build the palette by iterating all the entries in the colors hash
	RArray<TUint> palette(colors.Count());
	THashMapIter<TInt,TInt> colorIterator(colors);
	const TInt* color = colorIterator.NextKey();
	while (color)
		{
		if (palette.Append(*color) != KErrNone)
			{
			palette.Close();
			colors.Close();
			return KErrNoMemory;
			}
		//set the index of each entry back into the color hash for lookup later
		//const cast needed as CurrentValue returns a const pointer; for no good reason?
		TInt* index = CONST_CAST(TInt*, colorIterator.CurrentValue());
		*index = palette.Count() - 1;
		color = colorIterator.NextKey();
		}
		
	//Set up some new memory for the palettised bitmap
	//size is (4 bytes for palette size) + (4 bytes per palette entry) + (upto 1 byte per pixel)
	//pixels are packed according to number of colors
	TUint compressedBitsPerPixel = PaletteBitsPerPixel(colors.Count());
	TUint compressedPixelsPerByte = 8 / compressedBitsPerPixel;
	TUint compressedLineLengthInBytesPadded = (iHeader.iSizeInPixels.iWidth + compressedPixelsPerByte - 1)/ compressedPixelsPerByte;
	TInt compressedDataBytes = 4 + 4 * colors.Count() + compressedLineLengthInBytesPadded * iHeader.iSizeInPixels.iHeight;

	//add two extra ints for storing function pointers (8 bytes )
	compressedDataBytes += 8 ;
	
	TUint8* compressedBase = NULL;
	compressedBase = iPile->Alloc(compressedDataBytes);
	if (!compressedBase)
		{
		palette.Close();
		colors.Close();
		return KErrNoMemory;
		}
	iDataOffset = compressedBase - iPile->ChunkBase();

	iHeader.iBitmapSize = sizeof(SEpocBitmapHeader) + compressedDataBytes;
	iHeader.iCompression = EGenericPaletteCompression;
	
	//copy the palette length into the data area...
	*(REINTERPRET_CAST(TInt*, compressedBase)) = palette.Count(); //let's hope we're 4 byte aligned...
	compressedBase+=4;
	//...then the palette itself...
	for (TUint loop = 0; loop < palette.Count(); loop++)
		{
		*(REINTERPRET_CAST(TInt*, compressedBase)) = palette[loop];
		compressedBase +=4;
		}
		
	//Work out, then store, the decoding functions required for packing density and colour depth.
	//This saves having to do it for every scanline during decompression.

	TDecodeFunction decodeFunction = NULL;
	switch (compressedPixelsPerByte)
		{
		case 1:
			decodeFunction=CBitwiseBitmap::PaletteDecode1PixelPerByte;
			break;			
		case 2:
			decodeFunction=CBitwiseBitmap::PaletteDecode2PixelPerByte;
			break;			
		case 4:
			decodeFunction=CBitwiseBitmap::PaletteDecode4PixelPerByte;
			break;
		case 8:
			decodeFunction=CBitwiseBitmap::PaletteDecode8PixelPerByte;
			break;			
		default:
			::Panic(EFbsNotSupportedForCompression);
		}
	*(REINTERPRET_CAST(TDecodeFunction*, compressedBase))  = decodeFunction ;
	compressedBase += 4 ;

	//Select the appropriate assignment function based on the bits-per-pixel of the target
	TAssignFunction assignFunction = NULL;
	switch (iHeader.iBitsPerPixel)
		{
		case 16:
			assignFunction=CBitwiseBitmap::PaletteAssign16BitColor;
			break;
		case 24:
			assignFunction=CBitwiseBitmap::PaletteAssign24BitColor;
			break;
		case 32:
			assignFunction=CBitwiseBitmap::PaletteAssign32BitColor;
			break;
		default:
			::Panic(EFbsNotSupportedForCompression);
		}
	*(REINTERPRET_CAST(TAssignFunction*, compressedBase)) = assignFunction ;
	compressedBase += 4 ;
		
 	//...and finally the data
	pixelAddress = base;

	//separate loops for 16 & 24+ bpp again

	if ( iHeader.iBitsPerPixel < 24 )
		{
		for (lineAddress = base; lineAddress < top; lineAddress += lineLengthInBytesPadded)
			{
			//Loop each pixel within the line
			pixelAddress = lineAddress;
			while (pixelAddress < lineAddress + lineLengthInBytes)
				{
				TUint8 pack = 0;
				//loop for each pixel to pack into the byte.  If we run out of pixels in the line, we write out the pack byte and continue on the next line
				for (TInt ii = 0; ii < compressedPixelsPerByte && pixelAddress < lineAddress + lineLengthInBytes; ii++)
					{
					pack <<= compressedBitsPerPixel;
					//extract the color
					TUint color = *pixelAddress;
					color |= (*(pixelAddress+1)) << 8;
					
					//lookup the palette index for the color
					TInt* paletteIndex = colors.Find(color);
					//pack the palette index into the target byte
					pack |= *paletteIndex;
					//next pixel
					pixelAddress += sourceBytesPerPixel;
					}
				//store the packed pixel data into the new compressed data area
				*compressedBase = pack;
				compressedBase++;				
				}
			}
		}
	else //>= 24bpp
		{
		for (lineAddress = base; lineAddress < top; lineAddress += lineLengthInBytesPadded)
			{
			//Loop each pixel within the line
			pixelAddress = lineAddress;
			while (pixelAddress < lineAddress + lineLengthInBytes)
				{
				TUint8 pack = 0;
				//loop for each pixel to pack into the byte.  If we run out of pixels in the line, we write out the pack byte and continue on the next line
				for (TInt ii = 0; ii < compressedPixelsPerByte && pixelAddress < lineAddress + lineLengthInBytes; ii++)
					{
					pack <<= compressedBitsPerPixel;
					//extract the color
				TUint color = *pixelAddress;
					color |= (*(pixelAddress+1)) << 8;
					color |= (*(pixelAddress+2)) << 16;
					//if 32 bit, just ignore the 4th byte as it is unused for color data

					//lookup the palette index for the color
					TInt* paletteIndex = colors.Find(color);
					//pack the palette index into the target byte
					pack |= *paletteIndex;
					//next pixel
					pixelAddress += sourceBytesPerPixel;
					}
				//store the packed pixel data into the new compressed data area
				*compressedBase = pack;
				compressedBase++;				
				}
			}
		}//if


	//Set the RAM compression flag
	iIsCompressedInRAM = ETrue;

	//Free the old data.
	iPile->Free(base);
	//Clean up	
	palette.Close();
	colors.Close();
	return KErrNone;
	}
	
/**
Create a scan line from a palette compressed bitmap.
Starting from aPixel in the bitmap pointed to be aBase, populate aDestBuffer with aLength pixels looked up in the palette.
Note this function assumes 16, 24 or 32 non alpha bit uncompressed bitmaps, compressed into 8 bit palettes (ie <256 colors)
Structure of bitmap is (4 bytes for palette size) + (4 bytes per palette entry) + (1 byte per pixel)
@param aDestBuffer Points to the destination buffer. After the call it will be filled
with the decompressed data.
@param aPixel The decompression starts from this pixel
@param aLength Length of requested decompressed data - in pixels
@param aBase Points to the beginning of compressed bitmap data
@param aLineScannngPosition Saved information about the last used position in the compressed data
*/
void CBitwiseBitmap::GenerateLineFromPaletteCompressedData(
									 TUint8* aDestBuffer, 
									 const TPoint& aPixel,
									 TInt aLength, 
									 TUint32* aBase, 
									 TLineScanningPosition& /*aLineScanningPosition*/) const
	{
	//At entry, aBase will point to the start of the compressed data, ie the palette size
	//Each line in the bitmap will start at a byte boundary in the compressed data.
	
	TUint32* srcPalette = aBase + 1; //Address of the palette in the source data
	TUint srcNumPaletteEntries = *aBase;	//Number of entries in the palette (will be <=255)
	TUint compressedBitsPerPixel = PaletteBitsPerPixel(srcNumPaletteEntries);
	__ASSERT_DEBUG(compressedBitsPerPixel <= 8, Panic( EFbsBitmapDecompressionError )) ;
	const TUint8 lookup[] = {0, 8, 4, 0, 2, 0, 0, 0, 1};
	//TUint compressedPixelsPerByte = 8 / compressedBitsPerPixel;
	TUint compressedPixelsPerByte = lookup[compressedBitsPerPixel];
	__ASSERT_DEBUG(compressedPixelsPerByte>0, ::Panic(EFbsNotSupportedForCompression));	

	TUint8* srcData = REINTERPRET_CAST(TUint8*, srcPalette + srcNumPaletteEntries);	//Address of the pixel data in the source data

	//Extract the function pointers for decoding functions. Set up decode & assign functions.
	TDecodeFunction decodeFunction = NULL ;
	decodeFunction = reinterpret_cast<TDecodeFunction>(*((TUint32*)srcData )) ;
	srcData += 4 ;
	TAssignFunction assignFunction = NULL;
	assignFunction = reinterpret_cast<TAssignFunction>(*((TUint32*)srcData )) ;
	srcData += 4 ;

	//Note: The following lines have been optimised to avoid divisions.
	//By way of explanation the original lines have been left as comments

	//TUint srcBytesPerLinePadded = (iHeader.iSizeInPixels.iWidth + compressedPixelsPerByte - 1) / compressedPixelsPerByte;	//number of bytes occupied by each line in the compressed bitmap.
	TUint srcBytesPerLinePadded = ((iHeader.iSizeInPixels.iWidth + compressedPixelsPerByte - 1)  * compressedBitsPerPixel) >> 3;	//number of bytes occupied by each line in the compressed bitmap.
	//TUint srcStartBytesFromBase = aPixel.iY * srcBytesPerLinePadded + aPixel.iX / compressedPixelsPerByte;	//Starting bytes from the start of the bitmap
	TUint srcStartBytesFromBase = aPixel.iY * srcBytesPerLinePadded + ((aPixel.iX * compressedBitsPerPixel) >> 3);	//Starting bytes from the start of the bitmap
	//TUint srcStartPixelInByte = aPixel.iX  % compressedPixelsPerByte;		//starting pixel position in byte (lines start on byte boundary)
	TUint srcStartPixelInByte = aPixel.iX & ((compressedPixelsPerByte)- 1);		//starting pixel position in byte (lines start on byte boundary)
	//TUint srcEndBytesFromBase = srcStartBytesFromBase + (aLength + compressedPixelsPerByte - 1) / compressedPixelsPerByte;		//Ending bytes from the start of the bitmap
	TUint srcEndBytesFromBase = srcStartBytesFromBase + (((aLength + compressedPixelsPerByte - 1) * compressedBitsPerPixel) >> 3);		//Ending bytes from the start of the bitmap
	//TUint srcEndPixelInByte = (aPixel.iX + aLength) % compressedPixelsPerByte;		//Ending pixel position in byte
	TUint srcEndPixelInByte = (aPixel.iX + aLength) & ((compressedPixelsPerByte)-1);		//Ending pixel position in byte
	TUint8* srcStartData = srcData + srcStartBytesFromBase;	//Address of the first byte of packed pixels in the source
	TUint8* srcEndData = srcData + srcEndBytesFromBase;	//Address of the last+1 byte of packed pixels in the source
	TUint8* destStartData = aDestBuffer + ((aPixel.iX*iHeader.iBitsPerPixel) >> 3); //Address of the first pixel in the destination
	
	//3 stages to the decompression:
	//1. Decompress any pixels which are a subset of the first byte
	//2. Loop whole bytes extracting all pixels at once
	//3. Decompress any pixels which are a subset of the last byte

	TUint8* srcDataPtr = srcStartData;
	
	//Stage 1: Decompress any pixels which are a subset of the first byte
	if (srcStartPixelInByte > 0)
		PaletteDecodeAndAssignGeneric(srcDataPtr++, srcPalette, destStartData, srcStartPixelInByte, compressedPixelsPerByte-1, compressedPixelsPerByte, compressedBitsPerPixel);

	//If the last byte is only partly packed with pixels from this line, stop one byte short in the main loop
	if (srcEndPixelInByte > 0)
		srcEndData--;
	
	//Stage 2: Loop all the required pixels and call the appropriate functions		
	while (srcDataPtr < srcEndData)
		{
		__ASSERT_DEBUG(srcDataPtr <= srcEndData, ::Panic(EFbsNotSupportedForCompression));	
		__ASSERT_DEBUG(destStartData <= aDestBuffer + ((aPixel.iX*iHeader.iBitsPerPixel) >> 3) + aLength * PaletteBytesPerPixel(iHeader.iBitsPerPixel), ::Panic(EFbsNotSupportedForCompression));	
		(*decodeFunction)(srcDataPtr++, srcPalette, destStartData, assignFunction);
		}
	
	//Stage 3: Decompress any pixels which are a subset of the last byte
	if (srcEndPixelInByte > 0)
		PaletteDecodeAndAssignGeneric(srcDataPtr++, srcPalette, destStartData, 0, srcEndPixelInByte-1, compressedPixelsPerByte, compressedBitsPerPixel);
	}

/** 
This function deals with all different bit depths & color counts dynamically - smaller but slower
@param aDataPtr Address in compressed data to read from
@param aPalettePtr Address of the start of the palette in the compressed data
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aStartPixel Zero based position within the compressed byte of the first pixel to decompress
@param aEndPixel Zero based position within the compressed byte of the last pixel to decompress
@param aCompressedPixelsPerByte Number of pixels packed into each byte of the compressed data
@param aCompressedBitsPerPixel Number of bits used to express each pixel in the compressed data.  Nothing to do with the color depth of the image.  
*/
void CBitwiseBitmap::PaletteDecodeAndAssignGeneric(	TUint8* aDataPtr, 
												TUint32* aPalettePtr, 
												TUint8*& aDestPtr,
												TUint aStartPixel, 
												TUint aEndPixel, 
												TUint aCompressedPixelsPerByte,
												TUint aCompressedBitsPerPixel) const
	{
	__ASSERT_DEBUG(aStartPixel<aCompressedPixelsPerByte, ::Panic(EFbsNotSupportedForCompression));
	__ASSERT_DEBUG(aEndPixel<aCompressedPixelsPerByte, ::Panic(EFbsNotSupportedForCompression));

	//create a mask for the appropriate number of bits
	TUint8 mask = 0xFF;
	mask <<= 8 - aCompressedBitsPerPixel;

	//Adjust the mask in case we've been asked to start at an intermediate pixel
	mask >>= aStartPixel * aCompressedBitsPerPixel;
	
	TUint8 pack = *aDataPtr; //max of 8 bits for palette entry
	
	//Loop the pixel data from the requested start to the requested end
	for (TInt ii = aStartPixel;  ii <= aEndPixel;  ii++)
		{	
		//extract the bits corresponding to the required color index from the pack using the mask
		TUint8 index = pack&mask;
		//shift the index to the right to get true value
		index >>= (aCompressedPixelsPerByte- ii - 1) * aCompressedBitsPerPixel;
		//get the address of the required color value from the palette
		TUint32 color = *(aPalettePtr + index);
		//and copy the actual color value into the scanline buffer
		*aDestPtr ++= color;
		*aDestPtr ++= color >> 8;
		if (iHeader.iBitsPerPixel >= 24)
			*aDestPtr ++= color >> 16;
		if (iHeader.iBitsPerPixel == 32)
			*aDestPtr ++= 0xFF;  //use 0xFF rather than 0x00 as it is more alpha friendly
		
		//shift the mask to get the next required bits
		mask >>= aCompressedBitsPerPixel;
		}
	}

/** 
Specialised function for decoding pixels from a palette compressed bitmap with 1 pixel packed in each byte.
Implemented for speed, not size
@param aDataPtr Address in compressed data to read from
@param aPalettePtr Address of the start of the palette in the compressed data
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aAssignFunction Function pointer to assigment function to use to write actual pixel data to uncompressed scanline
*/
void CBitwiseBitmap::PaletteDecode1PixelPerByte(TUint8* aDataPtr, TUint32* aPalettePtr, TUint8*& aDestPtr, TAssignFunction aAssignFunction)
	{
	(*aAssignFunction)(aDestPtr, *(aPalettePtr + *aDataPtr));
	}
	
/** 
Specialised function for decoding pixels from a palette compressed bitmap with 2 pixels packed in each byte.
Implemented for speed, not size
@param aDataPtr Address in compressed data to read from
@param aPalettePtr Address of the start of the palette in the compressed data
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aAssignFunction Function pointer to assigment function to use to write actual pixel data to uncompressed scanline
*/
void CBitwiseBitmap::PaletteDecode2PixelPerByte(TUint8* aDataPtr, TUint32* aPalettePtr, TUint8*& aDestPtr, TAssignFunction aAssignFunction)
	{
	TUint8 mask = 0xF0; //binary 11110000
	TUint8 pack = *aDataPtr;
	//Pixel 1
	TUint8 index = pack&mask;
	index >>= 4;
	TUint32 color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color);
	mask >>= 4;
 	//Pixel 2
 	index = pack&mask;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	}

/** 
Specialised function for decoding pixels from a palette compressed bitmap with 4 pixels packed in each byte.
Implemented for speed, not size
@param aDataPtr Address in compressed data to read from
@param aPalettePtr Address of the start of the palette in the compressed data
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aAssignFunction Function pointer to assigment function to use to write actual pixel data to uncompressed scanline
*/
void CBitwiseBitmap::PaletteDecode4PixelPerByte(TUint8* aDataPtr, TUint32* aPalettePtr, TUint8*& aDestPtr, TAssignFunction aAssignFunction)
	{
	TUint8 mask = 0xC0; //binary 11000000
	TUint8 pack = *aDataPtr;
	//Pixel 1
	TUint8 index = pack&mask;
	index >>= 6;
	TUint32 color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color);
	mask >>= 2;
 	//Pixel 2
 	index = pack&mask;
	index >>= 4;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 2;
 	//Pixel 3
 	index = pack&mask;
	index >>= 2;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 2;
 	//Pixel 4
 	index = pack&mask;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	}

/** 
Specialised function for decoding pixels from a palette compressed bitmap with 8 pixels packed in each byte.
Implemented for speed, not size
@param aDataPtr Address in compressed data to read from
@param aPalettePtr Address of the start of the palette in the compressed data
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aAssignFunction Function pointer to assigment function to use to write actual pixel data to uncompressed scanline
*/
void CBitwiseBitmap::PaletteDecode8PixelPerByte(TUint8* aDataPtr, TUint32* aPalettePtr, TUint8*& aDestPtr, TAssignFunction aAssignFunction)
	{
	TUint8 mask = 0x80; //binary 10000000
	TUint8 pack = *aDataPtr;
	//Pixel 1
	TUint8 index = pack&mask;
	index >>= 7;
	TUint32 color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color);
	mask >>= 1;
 	//Pixel 2
 	index = pack&mask;
	index >>= 6;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask>>=1;
 	//Pixel 3
 	index = pack&mask;
	index >>= 5;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 1;
  	//Pixel 4
 	index = pack&mask;
	index >>= 4;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 1;
  	//Pixel 5
 	index = pack&mask;
	index >>= 3;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 1;
  	//Pixel 6
 	index = pack&mask;
	index >>= 2;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 1;
  	//Pixel 7
 	index = pack&mask;
	index >>= 1;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	mask >>= 1;
 	//Pixel 8
 	index = pack&mask;
 	color = *(aPalettePtr + index);
	(*aAssignFunction)(aDestPtr, color); 			
	}

/** 
Specialised function for assigning pixels into an uncompressed scanline of 16 bit color depth.
Implemented for speed, not size
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aColor Color info to write.
*/
void CBitwiseBitmap::PaletteAssign16BitColor(TUint8*& aDestPtr, TUint32 aColor)
	{
	*aDestPtr ++= aColor;
	*aDestPtr ++= aColor >> 8;
	}

/** 
Specialised function for assigning pixels into an uncompressed scanline of 24 bit color depth.
Implemented for speed, not size
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aColor Color info to write.
*/
void CBitwiseBitmap::PaletteAssign24BitColor(TUint8*& aDestPtr, TUint32 aColor)
	{
	*aDestPtr ++= aColor;
	*aDestPtr ++= aColor >> 8;
	*aDestPtr ++= aColor >> 16;
	}

/** 
Specialised function for assigning pixels into an uncompressed scanline of 32 bit color depth.
Implemented for speed, not size
@param aDestPtr Address to write uncompressed data to.  Will be incremented on return from function.
@param aColor Color info to write.
*/
void CBitwiseBitmap::PaletteAssign32BitColor(TUint8*& aDestPtr, TUint32 aColor)
	{
	*aDestPtr ++= aColor;
	*aDestPtr ++= aColor >> 8;
	*aDestPtr ++= aColor >> 16;
	*aDestPtr ++= 0xFF;  //use 0xFF rather than 0x00 as it is more alpha friendly
	}

/**
Get the bits used per pixel when packing multiple pixels in palette compression.
The value returned is a power of 2, not always the most efficient pack, for alignment reasons, Eg
 65537 -> KMaxTInt : 32 bpp
 257 -> 65536 colors : 16 bpp
 17 -> 256 colors : 8 bpp
 5  -> 16  colors : 4 bpp
 3  -> 4   colors : 2 bpp
 0  -> 2   colors : 1 bpp
@param aNumColors The number of colors in the bitmap.  This governs the size of the palette and thus
the number of bits needed to represent an index into it.
*/
TUint CBitwiseBitmap::PaletteBitsPerPixel(TInt aNumColors) const
	{
	if (aNumColors <= 2)
		return 1;
	else if (aNumColors <= 4)
		return 2;
	else if (aNumColors <= 16)
		return 4;
	else if (aNumColors <= 256)
		return 8;
	else if (aNumColors <= 65536)
		return 16;
	else
		return 32;
	}
	
/**
Gets the bytes used per pixel according to the bits per pixel of a bitmap.
Also used to find which bit per pixel values are supported by palette compression, hence this is not
a case of simple division by 8.
If return value is zero, the supplied bit per pixel value is not supported by palette compression.
@param aBitsPerPixel The bits per pixel value to transform into bytes
*/
TUint CBitwiseBitmap::PaletteBytesPerPixel(TInt aBitsPerPixel) const
	{
	if (aBitsPerPixel == 16)
		return 2;  //16 bit
	else if (aBitsPerPixel == 24)
		return 3;   //24 bit
	else if (aBitsPerPixel == 32 && iHeader.iColor == SEpocBitmapHeader::EColor)
		return 4;	//32 bit with no alpha
	else 
		return 0;
	}

