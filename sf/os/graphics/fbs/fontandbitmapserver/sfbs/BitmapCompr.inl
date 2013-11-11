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

/**
 
 The created class instance doesn't take the ownership on aBase parameter.
 @internalComponent
 @pre aBase != NULL
 @param aBase Points to the beginning of compressed bitmap data
 @param aComprDataBytes Total amount of compressed bitmap bytes
 @param aCanAdjustLineScanPos If not EFalse - scanline position can be adjusted calling
 AdjustLineScanningPosition function.
 @see AdjustLineScanningPosition(TLineScanningPosition& aLineScanPos, const TUint32* aBase, TInt aBitmapWidth, TInt aStartPos, TInt aCompressedDataBytes);
*/
template <TInt BPP, TInt BPP_DEST> 
TScanLineDecompressor<BPP, BPP_DEST>::TScanLineDecompressor(const TUint32* aBase, 
												  TInt aComprDataBytes, 
												  TBool aCanAdjustLineScanPos) :
	iBase(aBase),
	iComprDataBytes(aComprDataBytes),
	iCanAdjustLineScanPos(aCanAdjustLineScanPos)
	{
	}

/**
The method calculates the start byte position (as a index) in decompressed bitmap data .
@internalComponent
@param aPixel The decompression starts from this pixel
@param aByteWidth Scanline width in bytes
@return Calculated start position.
*/
template <TInt BPP, TInt BPP_DEST> 
TInt TScanLineDecompressor<BPP, BPP_DEST>::CalcStartPos(const TPoint& aPixel, TInt aByteWidth) const
	{
	return aPixel.iY * aByteWidth + aPixel.iX * BPP;
	}

/**
The method calculates the start byte position (as a index) in decompressed bitmap data .
@internalComponent
@param aPixel The decompression starts from this pixel
@param aByteWidth Scanline width in bytes
@return Calculated start position.
*/
template <TInt BPP, TInt BPP_DEST> 
TInt TScanLineDecompressor<BPP, BPP_DEST>::CalcDestStartPos(const TPoint& aPixel, TInt aByteWidth) const
	{
	return aPixel.iY * aByteWidth + aPixel.iX * BPP_DEST;
	}

/**
The method calculates the end byte position (as a index) in decompressed bitmap data .
@internalComponent
@param aLength Length in pixels of bitmap data we want to decompress.
@param aPixel The decompression starts from this pixel
@param aByteWidth Scanline width in bytes
@return Calculated end position.
*/
template <TInt BPP, TInt BPP_DEST> 
TInt TScanLineDecompressor<BPP, BPP_DEST>::CalcEndPos(TInt aLength, const TPoint& aPixel, 
											TInt aByteWidth) const
	{
	return aPixel.iY * aByteWidth + (aPixel.iX + aLength) * BPP_DEST;
	}

/**
The method calculates a pointer to a place in aDestBuffer where the 
copying of decompressed bitmap data starts.
@internalComponent
@param aDestBuffer Points to the start of the destination buffer.
@param aPixel The decompression starts from this pixel
@return A pointer to a place in aDestBuffer where the 
copying of decompressed bitmap data starts.
*/
template <TInt BPP, TInt BPP_DEST> 
TUint8* TScanLineDecompressor<BPP, BPP_DEST>::CalcDestPtr(const TUint8* aDestBuffer, 
												const TPoint& aPixel) const
	{
	return const_cast <TUint8*> (aDestBuffer + aPixel.iX * BPP_DEST);
	}

/**
The method calculates a pointer to a place in aDestBuffer where the copying
of the decompressed data stops.
@internalComponent
@param aDestPtr A pointer to a place in aDestBuffer where the 
copying of decompressed bitmap data starts - CalcDestPtr() return value.
@param aPixel The decompression starts from this pixel
@param aByteWidth Scanline width in bytes
@param aLength Length in pixels of bitmap data we want to decompress.
@param aStartPos CalcStartPos() method return value.
@return A pointer to a place in aDestBuffer where the 
copying of decompressed bitmap data stops.
@see CalcDestPtr
@see CalcStartPos
*/
template <TInt BPP, TInt BPP_DEST> 
TUint8* TScanLineDecompressor<BPP, BPP_DEST>::CalcDestPtrLimit(const TUint8* aDestPtr, 
													 const TPoint& aPixel, TInt aByteWidth, 
													 TInt aLength, TInt aStartPos) const
	{
	return const_cast <TUint8*> (aDestPtr + CalcEndPos(aLength, aPixel, aByteWidth) - aStartPos);
	}

/**
The method performs a quick find of the correct start position.
It uses the fact that aLineScanPos parameter should be recalculated after each positioning 
in the compresed bitmap data and might be used in the next call of decompression methods.
@internalComponent
@param aLineScanPos Saved information about the last used position in the compressed data
@param aByteWidth Scanline width in bytes
@param aStartPos CalcStartPos() method return value.
@return A pointer to a position in compressed bitmap data, where the decompression  
starts from
@see CalcStartPos
*/
template <TInt BPP, TInt BPP_DEST> 
TUint8* TScanLineDecompressor<BPP, BPP_DEST>::AdjustLineScanningPosition(TLineScanningPosition& aLineScanPos, 
															   TInt aByteWidth, 
															   TInt aStartPos) const
	{
	TUint8* srcPtr = NULL;
	// Fast find the correct position to start	
	if(iCanAdjustLineScanPos)
		{
		::AdjustLineScanningPosition(aLineScanPos, iBase, aByteWidth, aStartPos, iComprDataBytes);
		}
	srcPtr = aLineScanPos.iSrcDataPtr;
	//Unfortunatelly we can't move fast srcPtr if aLineScanPos.iCursorPos > aStartPos, 
	//because we have 1 byte for the compressed data length and the data itself after the "length"
	//byte. With this information we can move srcPtr forward, not backward.
	if(aLineScanPos.iCursorPos > aStartPos) 
		{
		srcPtr = (TUint8*)iBase;
		aLineScanPos.iCursorPos = 0;
		}
	TInt count = TInt8(*srcPtr);//pixels count
	TInt length = (count < 0 ? -count : count + 1) * BPP;//length in bytes
	//If count < 0 - we have a sequence of abs(count) pixels - abs(count) * BPP bytes
	//If count >= 0 - we have (count + 1) pixels with the same color - (count + 1) * BPP bytes
	while((aLineScanPos.iCursorPos + length) <= aStartPos)
		{
		srcPtr += count < 0 ? length + 1 : (BPP + 1);//(BPP + 1) - bytes for the pixel + 1 byte for the length
		aLineScanPos.iCursorPos += length;
		count = TInt8(*srcPtr);
		length = (count < 0 ? -count : count + 1) * BPP;
		}
	//After executing of the previous "if" operator and "while" loop, srcPtr points to 
	//the nearest "length" byte before the start position in compressed bitmap data.
	return srcPtr;
	}

/**
This overloaded "()" operator used for decompression of bitmap scan lines.
@internalComponent
@param aDestBuffer Points to the destination buffer. After the call it fill be filled
with the decomperssed data.
@param aPixel The decompression starts from this pixel
@param aLineScanPos Saved information about the last used position in the compressed data
@param aByteWidth Scanline width in bytes
@param aLength Length of requested decompressed data - in pixels
*/
template <TInt BPP, TInt BPP_DEST> 
void TScanLineDecompressor<BPP, BPP_DEST>::operator()(TUint8* aDestBuffer, const TPoint& aPixel, 
											TLineScanningPosition& aLineScanPos, 
											TInt aByteWidth, TInt aDestByteWidth, TInt aLength) const
	{
	TInt startPos = CalcStartPos(aPixel, aByteWidth);
	TInt destStartPos = CalcDestStartPos(aPixel, aDestByteWidth);
	TUint8* destPtr = CalcDestPtr(aDestBuffer, aPixel);
	const TUint8* destPtrLimit = CalcDestPtrLimit(destPtr, aPixel, aDestByteWidth, aLength, destStartPos);
	//Fast find the correct position to start	
	TUint8* srcPtr = AdjustLineScanningPosition(aLineScanPos, aByteWidth, startPos);
	//Decompress each slot of compressed data which has the following format:
	// ...|"length byte"| "pixel value(s)" |...
	while(destPtr < destPtrLimit)
		{
		TInt pixelsInTheSlot = TInt8(*srcPtr++);
		TInt availDestSpaceInBytes = destPtrLimit - destPtr;
		TInt skipBytes = startPos - aLineScanPos.iCursorPos;
		if(pixelsInTheSlot < 0)
			{//a sequence of abs(pixelsInTheSlot) pixels with different color values
			TInt bytesInTheSlot = -pixelsInTheSlot * BPP;
			TUint8* start_pos = srcPtr;
			TInt bytesToCopy = bytesInTheSlot;
			if(skipBytes > 0) //Skip the pixels before the start possition
				{
				bytesToCopy -= skipBytes;
				start_pos += skipBytes;
				}
			if(BPP == BPP_DEST)
				{
				destPtr = Mem::Copy(destPtr, start_pos, ::Min(bytesToCopy, availDestSpaceInBytes));
				}
			else
				{
				destPtr = CopyBlockPixel(destPtr, start_pos, ::Min(bytesToCopy / BPP, availDestSpaceInBytes / BPP_DEST));
				}	
			//Move to the next "length" byte only if everything, 
			//controlled by the current "length" byte, is copied
			if((bytesToCopy / BPP) <= (availDestSpaceInBytes / BPP_DEST))
				{
				srcPtr += bytesInTheSlot;
				aLineScanPos.iCursorPos += bytesInTheSlot;
				}
			else
				{
				srcPtr--;
				}
			}
		else//the "else" part of - if(pixelsInTheSlot < 0)
			{//(pixelsInTheSlot + 1) pixels with the same color
			TInt pixelsToCopy = pixelsInTheSlot + 1;
			TInt bytesInTheSlot = pixelsToCopy * BPP;
			if(skipBytes > 0) //Skip the pixels before the start possition
				{
				pixelsToCopy -= skipBytes / BPP;
				}
			destPtr = CopyPixel(destPtr, srcPtr, ::Min(pixelsToCopy, availDestSpaceInBytes / BPP_DEST));
			//Move to the next "length" byte only if everything, 
			//controlled by the current "length" byte, is copied
			if((pixelsToCopy * BPP) <= availDestSpaceInBytes)
				{
				srcPtr += BPP;// += BPP - we have (count + 1) pixels with the same color - only BPP bytes are used.
				aLineScanPos.iCursorPos += bytesInTheSlot;
				}
			else
				{
				srcPtr--;
				}
			}//end of - if(pixelsCnt < 0)
		}//end of - while(destPtr < destPtrLimit)
	aLineScanPos.iSrcDataPtr = srcPtr;//update aLineScanPos.iSrcDataPtr - it will be used 
									  //when the method is called again.
	}


