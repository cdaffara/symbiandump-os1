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

#include "tbitblt.h"
#include <e32const.h>

CTBitBlt::CTBitBlt()
	{
	SetTestStepName(KTDirectGdiBitBltStep);
	}

CTBitBlt::~CTBitBlt()
	{
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTBitBlt::RunTestsL()
	{
	if(iUseDirectGdi && !iUseSwDirectGdi)
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
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLT-0001"));
		TestBasicL();
		RecordTestResultL();
		iTestParams.iDoCompressed = EFalse;
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLT-0003"));
		TestInvalidParametersL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLT-0004"));
		TestSourceBitmapCloningL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLT-0005"));
		TestDrawModeL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLT-0006"));
		TestSetOriginL();
		RecordTestResultL();
		}
	else
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLT-0002"));
		TestPositioningL();
		RecordTestResultL();
		}

	if(iUseDirectGdi && !iUseSwDirectGdi)
		{
		CleanupStack::PopAndDestroy(iVgImageCache);
		}
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLT-0001
	
@SYMPREQ 			
	PREQ39
	
@SYMREQ 			
	REQ9197
	REQ9204 
	REQ9195 
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237 
	
@SYMTestCaseDesc	
	Bit blitting basic functionality.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test bit blitting with basic and valid parameters. The test sets an origin that is not 0,0
	and does a single BitBlt() to a position offscreen that will be moved back onscreen due to the previously
	set origin. The image should appear at 20,20.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Valid bitmap should be created. 
	This bitmap should be the same as a reference bitmap.
*/
void CTBitBlt::TestBasicL()
	{	
	_LIT(KTestName, "BitBlt-Basic"); 	
	
	// Only do the compressed version of this test for one pixel format
	// to cut down on the number of images produced.
	TBool writeOutCompressed = (iTestParams.iDoCompressed && (iTestParams.iTargetPixelFormat == EUidPixelFormatRGB_565));	
					
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
	
	iGc->BitBlt(TPoint(20, 20), *bitmap);
	TESTNOERRORL(iGc->GetError());	
	
	if (!iTestParams.iDoCompressed || writeOutCompressed)
		WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLT-0002
	
@SYMPREQ 			
	PREQ39

@SYMREQ 			
	REQ9197
	REQ9198
	REQ9204 
	REQ9195 
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237	
	
@SYMTestCaseDesc	
	Various combinations of destination positions, source position and size 
	of a bitmap are tested. This test also covers boundary position and size.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Bitmap positioning in a target area, for boundary position, and for clipping 
	to target area.
	
@SYMTestExpectedResults 
	Various parts of source bitmap should be copied into target area at
	various positions. Parts of source bitmap that go beyond a target area
	should be clipped. Valid bitmap should be created. This bitmap should
	be the same as a reference bitmap.
*/
void CTBitBlt::TestPositioningL()
	{	
	_LIT(KTestName, "BitBlt-Positioning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	iTestParams.iDoCompressed = ETrue;
	TestPositioningBaseL(KTestName(), EBitBlt);
	iTestParams.iDoCompressed = EFalse;
	TestPositioningBaseL(KTestName(), EBitBlt);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLT-0003
	
@SYMPREQ 			
	PREQ39

@SYMREQ 			
	REQ9197
	REQ9198
	REQ9204 
	REQ9195 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Calling BitBlt() method with invalid parameters
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid parameters (see BitBlt-Basic). Invoke the BitBlt() method replacing valid parameters with invalid equivalent. Create separate tests for horizontal and vertical coordinates.
	Invalid parameters:
	    Invalid source rectangles:
			- TRect(-30, -30, -10, -10);
			- TRect(bmpWidth+10, bmpHeight+10, bmpWidth+20, bmpHeight+20));
			- TRect(bmpWidth, bmpHeight, 0, 0));
			- TRect(-10, -10, -30, -30);
			- TRect(0, 0, 0, 0));    
	    Invalid source bitmaps:
			- zero size bitmap
			- not initialized bitmap

			
@SYMTestExpectedResults 
	Function should detect invalid parameters and return.
	Nothing should be drawn in a target area.
*/
void CTBitBlt::TestInvalidParametersL()
	{	
	_LIT(KTestName, "BitBlt-InvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	TestInvalidParametersBaseL(KTestName(), EBitBlt);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLT-0004
	
@SYMPREQ 			
	PREQ39

@SYMREQ 			
	REQ9197
	REQ9204 
	REQ9195 
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Tests that the source bitmap is drawn correctly if it is deleted in the 
	test code right after BitBlt() is called, but before Finish() is called and 
	the image is actually drawn by the engine.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	1. Invoke the BitBlt() method with valid parameters (see BitBlt-Basic). 
	2. Right after returning from BitBlt() call, destroy the source Bitmap (aBitmap).
	3. Call the BitBlt() method again with the same bitmap parameter.
	Repeat p. 1-3 a few times.#
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Memory leaks should not be
	created. Valid bitmap should be created. This bitmap should be the same as
	a reference bitmap.
*/
void CTBitBlt::TestSourceBitmapCloningL()
	{
	_LIT(KTestName, "BitBlt-SourceBitmapCloning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	iTestParams.iDoCompressed = ETrue;
	TestSourceBitmapCloningBaseL(KTestName(), EBitBlt);
	iTestParams.iDoCompressed = EFalse;
	TestSourceBitmapCloningBaseL(KTestName(), EBitBlt);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLT-0005
	
@SYMPREQ 			
	PREQ39

@SYMREQ 			
	REQ9198
	REQ9204 
	REQ9195 
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test correctness of bit blitting in all possible draw modes.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid parameters (see BitBlt-Basic).
	Use the semi-transparent source bitmap (aBitmap).
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	Set drawmode to EDrawModePEN and call the methods.
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	
@SYMTestExpectedResults 
	Semi-transparent (for EDrawModePEN) and opaque (for EDrawModeWriteAlpha)
	should be copied into rendering target, that is, the left and the right 
	bitmaps in the test should be drawn opaque, and the center bitmap in the 
	test should be draw blended. Valid bitmap should be created.
	This bitmap should be the same as a reference bitmap.
*/
void CTBitBlt::TestDrawModeL()
	{
	_LIT(KTestName, "BitBlt-DrawMode"); 
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
	iGc->BitBlt(
		TPoint(0, 0), 
		*iCheckedBoardWithAlphaBitmap, 
		TRect(TPoint(0, 0), vertRectSize));

	// Render row in center
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	iGc->BitBlt(
		TPoint(0, y1),
		*iCheckedBoardWithAlphaBitmap,
		TRect(TPoint(0, 0), horizRectSize));

	// Render column to right
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
	iGc->BitBlt(
		TPoint(bmpSize.iWidth - quaterWidth, 0),  
		*iCheckedBoardWithAlphaBitmap, 
		TRect(TPoint(0, 0), vertRectSize));
	
	TESTNOERRORL(iGc->GetError());

	WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLT-0006
	
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
	the bitmap is blitted.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Call SetOrigin().
	Call BitBlt().
	
@SYMTestExpectedResults 
	Source bitmap should be drawn at the correct position (TPoint(20,20)). Valid bitmap 
	should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTBitBlt::TestSetOriginL()
	{	
	_LIT(KTestName, "BitBlt-SetOrigin");
	
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
	iGc->BitBlt(TPoint(55, -60), *bitmap);
	TESTNOERRORL(iGc->GetError());	
		
	WriteTargetOutput(KTestName());
	}
