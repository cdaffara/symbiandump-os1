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

#ifndef __GRAPHICSSCREENCOMPARISON_H__
#define __GRAPHICSSCREENCOMPARISON_H__

#include <e32std.h>
#include <w32std.h>
#include <graphics/testscreencapture.h>

_LIT(KDir,"c:\\logs\\testexecute\\screencomparison\\");
_LIT(KMbmSuffix, ".mbm");
const TInt KFixedNumWidth = 3;

class CTGraphicsScreenComparison : public CBase
	{
public:
    IMPORT_C static CTGraphicsScreenComparison* NewL(CWsScreenDevice& aScreenDevice);
	IMPORT_C TInt CompareScreenImageL(const TSize& aCompareSize,
                                      const TPoint& aScreenPoint,
                                      const TPoint& aBitmapPoint,
                                      const CFbsBitmap& aRefBitmap,
                                      const TDesC& aTestStepName = KNullDesC);
	IMPORT_C TInt CompareScreenImageL(const TSize& aCompareSize,
                                      const TPoint& aScreenPoint,
                                      const TRgb& aColor,
                                      const TDesC& aTestStepName = KNullDesC);
	IMPORT_C MTestScreenCapture* GetMTestScreenCapture()  const;
	IMPORT_C ~CTGraphicsScreenComparison();

private:
	CTGraphicsScreenComparison();
	void ConstructL(CWsScreenDevice& aScreenDevice);
	void SaveBitmapL(CFbsBitmap& aBitmap, const TDesC& aTestStepName);

private:
	TSize iScreenSize;
	CFbsBitmap* iBmp;
	MTestScreenCapture* iCsc;
    TInt iSavedBitmapCounter;
	};

#endif //__GRAPHICSSCREENCOMPARISON_H___
