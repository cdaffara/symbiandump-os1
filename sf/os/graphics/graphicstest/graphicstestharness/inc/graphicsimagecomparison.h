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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __GRAPHICSIMAGECOMPARISON_H__
#define __GRAPHICSIMAGECOMPARISON_H__

#include <e32std.h>
#include <w32std.h>

class CTGraphicsImageComparison : public CBase
	{
public:
	IMPORT_C static TInt CompareBitmaps(const TSize& aCompareSize,
                                        const TPoint& aBitmap1Point,
                                        const TPoint& aBitmap2Point,
                                        const CFbsBitmap& aBitmap1,
                                        const CFbsBitmap& aBitmap2,
                                        const TUint32 aComparisonMask = 0xFFFFFFFF);

	IMPORT_C static TInt CompareBitmaps(const TSize& aCompareSize,
                                        const TPoint& aBitmapPoint,
                                        const CFbsBitmap& aBitmap,
                                        const TRgb& aColour,
                                        const TUint32 aComparisonMask = 0xFFFFFFFF);
private:
	static TInt CheckArguments(const TSize& aCompareSize,
                        const TPoint& aBitmap1Point,
                        const TPoint& aBitmap2Point,
                        const CFbsBitmap& aBitmap1,
                        const CFbsBitmap& aBitmap2);

	static TInt CheckArgumentBitmap(const TSize& aCompareSize,
                             const TPoint& aPoint,
                             const CFbsBitmap& aBitmap);
	};

#endif //__GRAPHICSIMAGECOMPARISON_H__
