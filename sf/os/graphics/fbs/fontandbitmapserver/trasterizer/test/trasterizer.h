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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TRASTERIZER_H
#define TRASTERIZER_H

#include "trasterizerbase.h"

_LIT(KTRasterizerStep, "TRasterizer");


/** Class for testing how the example rasterizer performs when passed
valid data.
 */
class CTRasterizer : public CTRasterizerBase
	{
public:
	CTRasterizer();	
	
	// From CStepStep	
	virtual TVerdict doTestStepL();	
	
private:	
	// Positive tests
	void TestLoadRasterizerDllL();
	void TestScanLineL();		
	void TestMultipleScanLinesL();
	void TestRegionOfInterestL();
	
	// Helper functions	
	TRgb ExtractRgb(const TUint8* aBuffer, TInt aPixelOffset, TDisplayMode aDispMode);
	CFbsBitmap* CreateExtendedBitmapLC(const TRgb* aColours, TBool aHorizontalStripe, TSize aSizeInPixels);
	
private:
	TDisplayMode iDisplayMode;	
	};

#endif // TRASTERIZER_H
