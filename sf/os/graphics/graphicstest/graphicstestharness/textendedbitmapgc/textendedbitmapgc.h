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
// textendedbitmap.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef TEXTENDEDBITMAPGC_H
#define TEXTENDEDBITMAPGC_H

#include <test/testexecutestepbase.h>
#include <test/testharnesscallback.h>
#include <w32std.h>

/** Test class for testing the operation of extended bitmaps with different graphics
contexts. Creators of this test class must provide a pointer to a MTestHarnessCallBack object
and a CBitmapContext. The MTestHarnessCallBack callback object allows CTExtendedBitmapGc to 
pass logging and test status operations back to the creator. All drawing operations that this
test class performs are drawn to the supplied CBitmapContext object.
 */
class CTExtendedBitmapGc : public CBase
	{
public:
	/** Stripe styles that are supported by the example bitmap example rasterizer, note that only
	vertical and horizontal stripes are supported
	*/
	enum TStripeStyle
		{
		EVerticalStripe = 0,
		EHorizontalStripe = 1
		};
	
	/** Defines whether this class should draw extended bitmaps as flags, or as plain white
	rectangles. EDrawFlag should be used when testing with BitGdi where the example rasterizer
	is available, or when testing with WServ where the example extended bitmap render stage is
	available. Otherwise EDrawWhite should be used. It is up to the creator of CTExtendedBitmapGc
	to decide which draw mode should be used.
	 */
	enum TDrawMode
		{
		EDrawFlag,
		EDrawWhite
		};

public:
	IMPORT_C static CTExtendedBitmapGc* NewL(MTestHarnessCallBack* aCallBack, CBitmapContext& aGc, TDrawMode aDrawMode, TDisplayMode aDisplayMode);	
	IMPORT_C ~CTExtendedBitmapGc();
	IMPORT_C void RunTestCaseL(TInt aCurTestCase);

	// Helper functions
	IMPORT_C static void WriteExtendedBitmapInfoL(TUint8* aData,
										   TInt& aDataSize,
										   const TRgb* aColorArray, 
										   TStripeStyle aStripeStyle);

private:	
	// Construction
	CTExtendedBitmapGc(MTestHarnessCallBack* aCallBack, CBitmapContext& aGc, TDrawMode aDrawMode);
	void ConstructL(TDisplayMode aDisplayMode);
	
	// Test cases
	void TestBitmapDrawingL(TBool aTestRegionOfInterest, TStripeStyle aStripeStyle, TInt aCaseNumber);
	void TestBrushPatternL(TInt aCaseNumber);
	
	// Helper functions	
	void CreateTestBitmapLC(CFbsBitmap*& aBmpRet,
						const TSize& aSize,
						TDisplayMode aDisplayMode, 
						const TRgb& aColor1, 
						const TRgb& aColor2, 
						const TRgb& aColor3,
						TStripeStyle aStripeStyle);

	CTestExecuteLogger&	Logger() {return iCallBack->Logger();}
	
private:
	MTestHarnessCallBack* iCallBack;
	TDrawMode iDrawMode;
	TDisplayMode iDisplayMode;
	CBitmapContext& iGc;
	TBool iIsFbs;
	CFont* iFont;
	};

#endif // TEXTENDEDBITMAPGC_H
