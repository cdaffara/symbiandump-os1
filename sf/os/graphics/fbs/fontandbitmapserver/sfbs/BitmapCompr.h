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

#ifndef __BITMAPCOMPR_H__
#define __BITMAPCOMPR_H__

//Forward declaration
class TLineScanningPosition;
//Global function used by 12, 16, 24 bpp scan line decompression methods
void AdjustLineScanningPosition(TLineScanningPosition& aLineScanPos, 
								const TUint32* aBase, 
								TInt aBitmapWidth, 
								TInt aStartPos,
								TInt aCompressedDataBytes);

/**
Possible values for the template argument of TScanLineDecompressor class.
@internalComponent
*/
typedef enum
	{
	E2bpp = 2,
	E3bpp = 3,
	E4bpp = 4
	} TScanLineDecompressorType;

/**
This functor class is used for bitmap scanline decompression.
Possible values for the template argument of TScanLineDecompressor class are enumerated
in TScanLineDecompressorType enum.
Note: "BPP" means "bytes per pixel for source buffer".
Note: "BPP_DEST" means "bytes per pixel for destination buffer".
Note: Template class design was preffered to base-derived class design (with virtual 
CopyPixel() method), because performance is more important than code size for bitmap
scanline decompression.
@internalComponent
*/
template <TInt BPP, TInt BPP_DEST> 
class TScanLineDecompressor
	{
public:
	TScanLineDecompressor(const TUint32* aBase, 
							TInt aComprDataBytes, 
							TBool aCanAdjustLineScanPos);
	void operator()(TUint8* aDestBuffer, const TPoint& aPixel, 
					TLineScanningPosition& aLineScanPos, TInt aByteWidth, TInt aDestByteWidth, TInt aLength) const;

private:
	TInt CalcStartPos(const TPoint& aPixel, TInt aByteWidth) const;
	TInt CalcDestStartPos(const TPoint& aPixel, TInt aByteWidth) const;
	TInt CalcEndPos(TInt aLength, const TPoint& aPixel, TInt aByteWidth) const;
	TUint8* CalcDestPtr(const TUint8* aDestBuffer, const TPoint& aPixel) const;
	TUint8* CalcDestPtrLimit(const TUint8* aDestPtr, const TPoint& aPixel, TInt aByteWidth, 
								TInt aLength, TInt aStartPos) const;
	TInt CalcAvailDestSpace(const TUint8* aDestPtr, const TUint8* aDestPtrLimit) const;
	TUint8* AdjustLineScanningPosition(TLineScanningPosition& aLineScanPos, TInt aByteWidth, 
										TInt aStartPos) const;
	TUint8* CopyPixel(TUint8* aDestPtr, const TUint8* aSrcPtr, TInt aCount) const;
	TUint8* CopyBlockPixel(TUint8* aDestPtr, const TUint8* aSrcPtr, TInt aCount) const;

	TScanLineDecompressor(const TScanLineDecompressor&);//prevent default copy constructor
	TScanLineDecompressor& operator=(const TScanLineDecompressor&);//prevent default "=" operator

private:
	const TUint32* iBase;
	const TInt iComprDataBytes;
	const TBool iCanAdjustLineScanPos;

	};

#include "BitmapCompr.inl"

#endif//__BITMAPCOMPR_H__

