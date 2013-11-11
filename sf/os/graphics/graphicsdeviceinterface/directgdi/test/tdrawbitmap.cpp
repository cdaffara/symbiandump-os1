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

#include "tdrawbitmap.h"

CTDrawBitmap::CTDrawBitmap()
	{
	SetTestStepName(KTDirectGdiDrawBitmapStep);
	}

CTDrawBitmap::~CTDrawBitmap()
	{
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDrawBitmap::RunTestsL()
	{
	if (iUseDirectGdi && !iUseSwDirectGdi)
		{
		// In the event that a test leaves after a BitBlt() or DrawBitmap() has occurred
		// the vgimage cache will need to be reset.
		// This needs to be the first item on the cleanupstack, 
		// as some tests perform pushes and pops of bitmaps.
		CleanupStack::PushL(TCleanupItem(ResetCache, iVgImageCache));
		}
	if(!iLargeTarget)
		{
		iTestParams.iDoCompressed = ETrue;
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0001"));
		TestBasicL();
		RecordTestResultL();
		iTestParams.iDoCompressed = EFalse;
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0003"));
		TestInvalidParametersL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0005"));
		TestSourceBitmapCloningL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0006"));
		TestDrawModeL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0007"));
		TestSetOriginL();
		RecordTestResultL();
		}
	else
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0002"));
		TestPositioningL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAP-0004"));
		TestScalingL();
		RecordTestResultL();
		}
	if (iUseDirectGdi && !iUseSwDirectGdi)
		{
		CleanupStack::PopAndDestroy(iVgImageCache);
		}
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0001
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Bitmap drawing basic functionality test.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test bitmap drawing for simple and valid parameters:
		- Set the origin at TPoint(-35,80)
		- Draw a stretched bitmap at TPoint(55,-60)
	
@SYMTestExpectedResults 
	Source bitmap should be copied into drawing target.
	Valid bitmap should be created. This bitmap shall be compared
	to a reference bitmap.
*/
void CTDrawBitmap::TestBasicL()
	{	
	_LIT(KTestName, "DrawBitmap-Basic"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	CFbsBitmap* bitmap;
	if(iTestParams.iDoCompressed)
		{
		bitmap = iCompressedBitmap;
		}
	else
		{
		bitmap = iCheckedBoardBitmap2;
		}
		
	iGc->DrawBitmap(TRect(TPoint(20, 20), bitmap->SizeInPixels()), *bitmap, TRect(10, 10, 130, 130));
	TESTNOERRORL(iGc->GetError());

	if(!iTestParams.iDoCompressed)
		WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0002
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test regularity of bitmap positioning.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test positioning functionality of DrawBitmap() methods. Test boundary
 	conditions and clipping to surface area.
 	Covers the whole target bitmap starting from TPoint(-30,-30) with staggered blits of a small 8x8 bitmap.
 	
@SYMTestExpectedResults 	
	Source bitmap should be drawn tiled in a staggered fashion across the whole target. Clipping should be applied.
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawBitmap::TestPositioningL()
	{	
	_LIT(KTestName, "DrawBitmap-Positioning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	iTestParams.iDoCompressed = ETrue;
	TestPositioningBaseL(KTestName(), EDrawBitmap);
	iTestParams.iDoCompressed = EFalse;
	TestPositioningBaseL(KTestName(), EDrawBitmap);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0003
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test functions behaviour after calling with invalid parameters.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid parameters (see DrawBitmap-BasicL). Replace individual parameters
	with Invalid ones:
		Invalid source rectangles
			aSourceRect : negative source rectangle starting point or size
				TRect( TPoint( -30, -30), TSize(-10, -10) ) 
			aSourceRect : source rectangle not compatible with bitmap
				TRect( TSize(aBitmap->SizeInPixel(). iWidth + 10, aBitmap->SizeInPixel(). IHeight + 10)) 
			aSourceRect : swapped points in source rectangle
				TRect( TPoint( 100, 100 ) , TPoint( 0, 0 ) )
		Invalid destination rectangle (aDestRect):
		 	negative size 
		Invalid source bitmap
			zero size bitmap
			not initialized bitmap
			
@SYMTestExpectedResults 
	Function should detect invalid parameters and return.
 	Nothing will be drawn on the target surface.
*/
void CTDrawBitmap::TestInvalidParametersL()
	{	
	_LIT(KTestName, "DrawBitmap-InvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TestInvalidParametersBaseL(KTestName(), EDrawBitmap);
	}

/**
@SYMTestCaseID 
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0004

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195 
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test regularity of bitmap scaling.

@SYMTestPriority
	Critical

@SYMTestStatus 
	Implemented

@SYMTestActions 
	Test scaling implementation of DrawBitmap() methods :  scaling down of a normal
	bitmap and scaling up of a bitmap fragment.

@SYMTestExpectedResults 
	Scaled bitmap should be drawn in a target area. Valid bitmap should be created.
 	The bitmap should be properly scaled. This bitmap shall be compared to a reference
 	bitmap.
*/
void CTDrawBitmap::TestScalingL()
	{
	_LIT(KTestName, "DrawBitmap-Scaling"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;

	CFbsBitmap* bitmap1 = iCheckedBoardBitmap1;
	TInt bmp1Width = bitmap1->SizeInPixels().iWidth;
	TInt bmp1Height = bitmap1->SizeInPixels().iHeight;
	CFbsBitmap* bitmap2 = iCheckedBoardBitmap2;
	TInt bmp2Width = bitmap2->SizeInPixels().iWidth;
	TInt bmp2Height = bitmap2->SizeInPixels().iHeight;

	ResetGc();

	// small bitmap - extrem magnification
	TReal32 scale = 1.0f;
	TReal32 scaleFactor = 1.55f;
	TInt posX = 1;
	for(TInt j=0; j<6; j++)
		{
		TSize size(static_cast<TInt>(bmp1Width*scale), static_cast<TInt>(bmp1Height*scale));
		TPoint pos(posX, height/4-size.iHeight/2);

		iGc->DrawBitmap(TRect(pos, size), *bitmap1);

		posX += size.iWidth+2;
		scale *= scaleFactor;
		}

	// large bitmap - extreme minification

	scale /= scaleFactor;
	posX = 1;
	for(TInt j=5; j>=0; j--)
		{
		TSize size(static_cast<TInt>(bmp1Width*scale), static_cast<TInt>(bmp1Height*scale));
		TPoint pos(posX, 3*height/4-size.iHeight/2);

		iGc->DrawBitmap(TRect(pos, size), *bitmap2);

		posX += size.iWidth+2;
		scale /= scaleFactor;
		}

	TESTNOERRORL(iGc->GetError());

	TBuf<30> testName;
	testName.Append(KTestName);
	WriteTargetOutput(testName);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0005
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Tests that a bitmap is still drawn successfully if it is deleted 
	right after the call to DrawBitmap() but before Finish() is called.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	For each method:
	1. Call method with valid parameters (see DrawBitmap-Basic). 
	2. Right after returning from method call, destroy the source bitmap.
	3. Call the method again with the same bitmap parameter.
	Repeat  p. 1-3 a few times.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Memory leaks should not be
	created. Valid bitmap should be created. This bitmap should be the same as
	a reference bitmap.
*/
void CTDrawBitmap::TestSourceBitmapCloningL()
	{
	_LIT(KTestName, "DrawBitmap-SourceBitmapCloning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	iTestParams.iDoCompressed = ETrue;
	TestSourceBitmapCloningBaseL(KTestName(), EDrawBitmap);
	iTestParams.iDoCompressed = EFalse;
	TestSourceBitmapCloningBaseL(KTestName(), EDrawBitmap);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0006
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test correctness of drawing bitmaps in all possible draw modes.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid parameters (see DrawBitmap-Basic).	
	Use the semi-transparent source bitmap (aBitmap).
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	Set drawmode to EDrawModePEN and call the methods.
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	
@SYMTestExpectedResults 
	The image drawn in the center of the test should be drawn blended (EDrawModePEN) 
	and the images drawn at the left and right sides should be drawn opaque (EDrawModeWriteAlpha).
	should be copied into rendering target. Valid bitmap should be created.
	This bitmap should be the same as a reference bitmap.
*/
void CTDrawBitmap::TestDrawModeL()
	{
	_LIT(KTestName, "DrawBitmap-DrawMode"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	TSize bmpSize(iCheckedBoardWithAlphaBitmap->SizeInPixels());

	TInt halfHeight = (bmpSize.iHeight  >> 1);	
	TInt quaterWidth  = (bmpSize.iWidth  >> 2);
	TInt y1 = halfHeight - (bmpSize.iHeight >> 2);
	
	TSize vertRectSize  (quaterWidth, bmpSize.iHeight);
	TSize horizRectSize (bmpSize.iWidth, halfHeight);
	
	// Render column to left
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
	iGc->DrawBitmap(
		TRect (TPoint(0, 0), vertRectSize),
		*iCheckedBoardWithAlphaBitmap);

	// Render row in center
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	iGc->DrawBitmap(
		TRect(TPoint(0, y1), horizRectSize),
		*iCheckedBoardWithAlphaBitmap);

	// Render column to right
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
	iGc->DrawBitmap(
		TRect (TPoint(bmpSize.iWidth - quaterWidth, 0), vertRectSize),
		*iCheckedBoardWithAlphaBitmap);
	
	TESTNOERRORL(iGc->GetError());

	WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAP-0007
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9199 
	REQ9204 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test that a bitmap is drawn at the correct position when SetOrigin() is called before
	the bitmap is drawn.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Call SetOrigin().
	Call DrawBitmap().
	
@SYMTestExpectedResults 
	Source bitmap should be drawn at the correct position (TPoint(20,20)). Valid bitmap 
	should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawBitmap::TestSetOriginL()
	{	
	_LIT(KTestName, "DrawBitmap-SetOrigin");
	
	// Only do test for one pixel format to cut down on the number of images produced
	// as this is just a positional test and we don't need to test for all pixel formats.
	if (!((iTestParams.iTargetPixelFormat == EUidPixelFormatRGB_565) && (iTestParams.iSourcePixelFormat == EUidPixelFormatRGB_565)))
		{
		return;
		}
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	CFbsBitmap* bitmap = iCheckedBoardBitmap2;	
	iGc->SetOrigin(TPoint(-35, 80));
	iGc->DrawBitmap(TRect(TPoint(55, -60), bitmap->SizeInPixels()), *bitmap, TRect(10, 10, 130, 130));
	TESTNOERRORL(iGc->GetError());
	
	WriteTargetOutput(KTestName());
	}
