// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
*/

#include "graphicsimagecomparison.h"

/**
Compares the contents of a rectangular region of one bitmap with a similarly sized
rectangular region of another bitmap.

@param aCompareSize, a const reference to a TSize object denoting the size of the
rectangular region for comparison.Negative and zero dimensions of this argument can
be passed in without returning a KErrArgument error.
@param aBitmap1Point, a const reference to a TPoint object denoting the top left
point of the rectangle in aBitmap1 used for comparison..
@param aBitmap2Point, a const reference to a TPoint object denoting the top left
point of the rectangle in aBitmap2 used for comparison.
@param aBitmap1, a const reference to the first CFbsBitmap for use in comparison.
@param aBitmap2, a const reference to the second CFbsBitmap for use in comparison.
@param aComparisonMask, a bit mask to be applied to the bitmap pixel data before
performing the comparison, in the form 0xAARRGGBB. Defaults to 0xFFFFFFFF 
@pre The rectanglular comparison region must reside wholly inside both of the bitmaps
@return KErrNone if the pixels contained in the bitmap rectangles are an exact match
or, otherwise, the count of the first unmatched pixel. Pixels are compared from TRUE
top-left to bottom-right in the horizontal direction (TRUE to cope with negative
dimensions of the aCompareSize object) An area of zero size will return KErrNone so
long as the pre-conditions are satisfied.
KErrArgument if the pre-conditions are not met.
*/
EXPORT_C TInt CTGraphicsImageComparison::CompareBitmaps(const TSize& aCompareSize,
                                                        const TPoint& aBitmap1Point,
                                                        const TPoint& aBitmap2Point,
                                                        const CFbsBitmap& aBitmap1,
                                                        const CFbsBitmap& aBitmap2,
                                                        const TUint32 aComparisonMask)
	{
	TInt err = CheckArguments(aCompareSize,
                              aBitmap1Point,
                              aBitmap2Point,
                              aBitmap1,
                              aBitmap2);

	if(err == KErrNone)
		{
		//Take local copies as cannot modify static arguments
		TSize localSize(aCompareSize);
		TPoint localPoint1(aBitmap1Point);
		TPoint localPoint2(aBitmap2Point);

		//Cope with negative aCompareSize dimensions
		if(aCompareSize.iWidth < 0)
			{
			localSize.iWidth = -localSize.iWidth;
			localPoint1.iX = localPoint1.iX - localSize.iWidth;
			localPoint2.iX = localPoint2.iX - localSize.iWidth;
			}

		if(aCompareSize.iHeight < 0)
			{
			localSize.iHeight = -localSize.iHeight;
			localPoint1.iY = localPoint1.iY - localSize.iHeight;
			localPoint2.iY = localPoint2.iY - localSize.iHeight;
			}

        // Set up buffers for obtaining scanlines
        TInt scanLineLength1 = aBitmap1.ScanLineLength(localSize.iWidth, ERgb);
		TUint8* buffer1 = new TUint8[scanLineLength1];
		if(!buffer1)
		    {
		    return KErrNoMemory;
		    }
        TPtr8 scanLine1(buffer1, scanLineLength1, scanLineLength1);
		TInt scanLineLength2 = aBitmap2.ScanLineLength(localSize.iWidth, ERgb);
        TUint8* buffer2 = new TUint8[scanLineLength2];
        if(!buffer2)
            {
            delete[] buffer1;
            return KErrNoMemory;
            }
        TPtr8 scanLine2(buffer2, scanLineLength2, scanLineLength2);

        //Perform scanline to scanline comparison without comparison mask
	    for(TInt y=0; y<localSize.iHeight; y++)
            {
            aBitmap1.GetScanLine(scanLine1, localPoint1+TPoint(0,y), localSize.iWidth, ERgb);
            aBitmap2.GetScanLine(scanLine2, localPoint2+TPoint(0,y), localSize.iWidth, ERgb);
            
            if(aComparisonMask!=0xFFFFFFFF || scanLine1.Compare(scanLine2)!=0)
                {
                //Comparison mask has been set, or scanlines are not equal
                //so perform pixel by pixel comparison using mask
                TRgb pixel1, pixel2;
                for(TInt x=0; x<localSize.iWidth; x++)
                    {
                    pixel1 = *(((TRgb*)buffer1) + x);
                    pixel2 = *(((TRgb*)buffer2) + x);

                    if( (pixel1.Internal()& aComparisonMask) != (pixel2.Internal()& aComparisonMask))
                        {
                        RDebug::Print(_L("x = %d y = %d  pixel1= %x pixel2 = %x"), x, y, pixel1.Internal(), pixel2.Internal());
                        delete[] buffer2;
                        delete[] buffer1;
                        return (y*localSize.iWidth + x) + 1;
                        }
                    }
                }
           }
	    delete[] buffer2;
	    delete[] buffer1;
        }
    return err;
	}

/**
Compares the contents of a rectangular region of a bitmap with a reference colour.

@param aCompareSize, a const reference to a TSize object denoting the size of the
rectangular region for comparison.Negative and zero dimensions of this argument can
be passed in without returning a KErrArgument error.
@param aBitmapPoint, a const reference to a TPoint object denoting the top left
point of the rectangle in aBitmap1 used for comparison..
@param aBitmap, a const reference to the CFbsBitmap for use in comparison.
@param aColor, the colour to test for
@param aComparisonMask, a bit mask to be applied to both the bitmap pixel data and the
colour data before performing the comparison, in the form 0xAARRGGBB. Defaults to 0xFFFFFFFF 
@pre The rectanglular comparison region must reside wholly inside the bitmap
@return KErrNone if the pixels contained in the bitmap rectangles are an exact match
to the reference colour or, otherwise, the count of the first unmatched pixel. Pixels
are compared from TRUE top-left to bottom-right in the horizontal direction (TRUE to
cope with negative dimensions of the aCompareSize object) An area of zero size will
return KErrNone so long as the pre-conditions are satisfied.
KErrArgument if the pre-conditions are not met.
*/
EXPORT_C TInt CTGraphicsImageComparison::CompareBitmaps(const TSize& aCompareSize,
                                                        const TPoint& aBitmapPoint,
                                                        const CFbsBitmap& aBitmap,
                                                        const TRgb& aColor,
                                                        const TUint32 aComparisonMask)
	{
	TInt err = CheckArgumentBitmap(aCompareSize,
                                   aBitmapPoint,
                                   aBitmap);

	if(err == KErrNone)
		{
		//Take local copies as cannot modify static arguments
		TSize localSize(aCompareSize);
		TPoint localPoint(aBitmapPoint);

		//Cope with negative aCompareSize dimensions
		if(aCompareSize.iWidth < 0)
			{
			localSize.iWidth = -localSize.iWidth;
			localPoint.iX = localPoint.iX - localSize.iWidth;
			}

		if(aCompareSize.iHeight < 0)
			{
			localSize.iHeight = -localSize.iHeight;
			localPoint.iY = localPoint.iY - localSize.iHeight;
			}

        // Set up buffers for obtaining scanlines
        TInt scanLineLength = aBitmap.ScanLineLength(localSize.iWidth, ERgb);
        TUint8* buffer = new TUint8[scanLineLength];
        if(!buffer)
            {
            return KErrNoMemory;
            }
        TPtr8 scanLine(buffer, scanLineLength, scanLineLength);

        //Perform the pixel by pixel comparison
        TRgb pixel;
		for(TInt y=0; y<localSize.iHeight; y++)
	 		{
            aBitmap.GetScanLine(scanLine, localPoint+TPoint(0,y), localSize.iWidth, ERgb);
	 		for(TInt x=0; x<localSize.iWidth; x++)
				{
                pixel = *(((TRgb*)buffer) + x);

                if( (pixel.Internal()& aComparisonMask) != (aColor.Internal()& aComparisonMask))
					{
					RDebug::Print(_L("x = %d y = %d  pixel= %x reference colour = %x"), x, y, pixel.Internal(), aColor.Internal());
                    delete[] buffer;
					return (y*localSize.iWidth + x) + 1;
					}
				}
	 		}
	    delete[] buffer;
		}
    return err;
	}

TInt CTGraphicsImageComparison::CheckArguments(const TSize& aCompareSize,
                                               const TPoint& aBitmap1Point,
                                               const TPoint& aBitmap2Point,
                                               const CFbsBitmap& aBitmap1,
                                               const CFbsBitmap& aBitmap2)
	{
	//Test that the arguments are valid for both bitmaps
	TInt err = CheckArgumentBitmap(aCompareSize, aBitmap1Point, aBitmap1);

	if(err == KErrNone)
		{
		err = CheckArgumentBitmap(aCompareSize, aBitmap2Point, aBitmap2);
		}

	return err;
	}

TInt CTGraphicsImageComparison::CheckArgumentBitmap(const TSize& aSize,
                                                    const TPoint& aPoint,
                                                    const CFbsBitmap& aBitmap)
	{
	//Top left-hand corner of the comparison rectangle is outside of the bitmap
	if( (aPoint.iX < 0) || (aPoint.iY < 0) )
		{
		return KErrArgument;
		}

	if(aSize.iWidth >= 0)
		{
		//Comparison rectangle is outside of the bitmap (rhs)
		if(aPoint.iX + aSize.iWidth > aBitmap.SizeInPixels().iWidth)
			{
			return KErrArgument;
			}
		}
	else
		{
		//Comparison rectangle is outside of the bitmap (lhs)
		if(aPoint.iX + aSize.iWidth < 0)
			{
			return KErrArgument;
			}
		}

	if(aSize.iHeight >= 0)
		{
		//Comparison rectangle is outside of the bitmap (bottom)
		if(aPoint.iY + aSize.iHeight > aBitmap.SizeInPixels().iHeight)
			{
			return KErrArgument;
			}
		}
	else
		{
		//Comparison rectangle is outside of the bitmap (top)
		if(aPoint.iY + aSize.iHeight < 0)
			{
			return KErrArgument;
			}
		}

	return KErrNone;
	}
