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

#include "tclipping.h"

CTClipping::CTClipping()
	{
	SetTestStepName(KTDirectGdiClippingStep);
	}

CTClipping::~CTClipping()
	{
	}
			
/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-CLIPPING-0001
	
@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc  
	Tests setting clipping regions.
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions 
	Set the clipping region consisting of a number of rectangles. Blit a bitmap. 
	Reset the clipping region.
	
@SYMTestExpectedResults 
	Bitmap should be drawn only within the clipped regions successfully.
*/
void CTClipping::TestClippingBasicL()
	{	
	_LIT(KTestName, "Clipping-Basic"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iTargetPixelFormat, TSize(150,150));
	TESTL(bitmap != NULL);
	RRegion clippingRegion(8);
	clippingRegion.AddRect(TRect(10,10,40,40)); // normal square clipping rect
	clippingRegion.AddRect(TRect(60,70,67,132));	// normal rectangular clipping rect
	clippingRegion.AddRect(TRect(50,75,110,85));	// clipping rect crosses another clipping rect
	clippingRegion.AddRect(TRect(125,125,125,125));	// clipping rect with zero size
	clippingRegion.AddRect(TRect(50,30,45,25));	// clipping rect with negative size
	
	ResetGc();
	iGc->SetClippingRegion(clippingRegion);
	TESTNOERROR(iGc->GetError());
	iGc->BitBlt(TPoint(0, 0), *bitmap);
	TESTNOERROR(iGc->GetError());
	iGc->ResetClippingRegion();
	TESTNOERROR(iGc->GetError());
	clippingRegion.Close();
	delete bitmap;
	if (iUseDirectGdi && !iUseSwDirectGdi)
		{
		iVgImageCache->ResetCache();
		}

	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}



/**
@SYMTestCaseID 
	GRAPHICS-DIRECTGDI-CLIPPING-0002
	
@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc  
	Tests the argument checking within SetClippingRegion(TRegion&).
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented  
	
@SYMTestActions 
	1. Test a region that is outside the bounds of the target.  
	2. Test a region which is partially outside the bounds of the target.
	3. Test using a region which has its error flag set.
	
@SYMTestExpectedResults 
	Test should return KErrArgument.
*/
void CTClipping::TestClippingErrors()
	{	
	_LIT(KTestName, "Clipping-Errors"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	RRegion outOfBoundsRegion(TRect(200,200,220,220));	// clipping rect wholly outside target area
	RRegion partOutOfBoundsRegion(TRect(80,-10,100,25));	// clipping rect partially outside target area
	TRegionFix<1> regionWithError(TRect(50,75,110,85));
	//add more rects than region can hold to set error flag
	regionWithError.AddRect(TRect(0,0,5,5));

	ResetGc();
	
	// Test whether setting clipping region returns  error when passed invalid clipping regions
	// 1: Pass region wholly outside of target.
	iGc->SetClippingRegion(outOfBoundsRegion);
	TEST(KErrArgument == iGc->GetError());
	iGc->ResetClippingRegion();
	TESTNOERROR(iGc->GetError());
	// 2: Pass region partially outside of target.
	iGc->SetClippingRegion(partOutOfBoundsRegion);
	TEST(KErrArgument == iGc->GetError());
	iGc->ResetClippingRegion();
	TESTNOERROR(iGc->GetError());
	// 3: Pass region with error flag set.
	iGc->SetClippingRegion(regionWithError);
	TEST(KErrArgument == iGc->GetError());
	iGc->ResetClippingRegion();
	TESTNOERROR(iGc->GetError());
	outOfBoundsRegion.Close();
	partOutOfBoundsRegion.Close();
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-CLIPPING-0003
	
@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc  
	Tests using clipping regions while using various APIs to draw images, using a non-zero origin.
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions 
	Create a bitmap.
	Set a clipping region rectangle (50,50) from the top-left of the target.
	Set the origin to 50,50.
	Draw an image using BitBlt() at (0,0).
	Reset the clipping.
	Set a clipping region rectangle (150,50) from the top-left of the target.
	Set the origin to 150,50.
	Draw an image using DrawBitmap() at (0,0).
	Reset the clipping.
	Set a clipping region rectangle (50,150) from the top-left of the target.
	Set the origin to 50,150.
	Draw an image using BitBltMasked() at (0,0).
	Reset the clipping.
	Set a clipping region rectangle (150,150) from the top-left of the target.
	Set the origin to 150,150.
	Draw an image using DrawBitmapMasked() at (0,0).
	
@SYMTestExpectedResults 
	The first bitmap should be drawn, 50,50 from the top-left of the target. 
	The second bitmap should be drawn, 100,50 from the top-left of the target.
	The third bitmap should be drawn, 50,100 from the top-left of the target.
	The fourth bitmap should be drawn, 100,100 from the top-left of the target.
	None of them should be clipped because although it is always being drawn at (0,0), the
	drawing and the clipping should take into account the Origin offset.
*/
void CTClipping::TestClippingWithOriginL()
	{	
	_LIT(KTestName, "Clipping-WithOrigin"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	const TSize bitmapSrcSize = TSize(40,40);
	
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iTargetPixelFormat, bitmapSrcSize);
	CleanupStack::PushL(bitmap);
	CFbsBitmap* mask = CreateBlackWhiteBitmapL(iTestParams.iTargetPixelFormat, bitmapSrcSize, TSize(2,2));
	CleanupStack::PushL(mask);
	
	// Test with BitBlt().
	iGc->SetOrigin(TPoint(50,50));
	RRegion clippingRegion(1);
	clippingRegion.AddRect(TRect(TPoint(50,50), TSize(50,50)));
	iGc->SetClippingRegion(clippingRegion);
	iGc->BitBlt(TPoint(0,0), *bitmap);
	iGc->ResetClippingRegion();
	clippingRegion.Close();
	TESTNOERROR(iGc->GetError());
	
	// Test with DrawBitmap() with scaling.
	iGc->SetOrigin(TPoint(150, 50));
	clippingRegion.AddRect(TRect(TPoint(150,50), TSize(50,50)));
	iGc->SetClippingRegion(clippingRegion);
	iGc->DrawBitmap(TRect(TPoint(0,0), TSize(50,50)), *bitmap, TRect(TPoint(0,0), bitmapSrcSize));
	iGc->ResetClippingRegion();
	clippingRegion.Close();
	TESTNOERROR(iGc->GetError());
	
	// Test with BitBltMasked().
	iGc->SetOrigin(TPoint(50, 150));
	clippingRegion.AddRect(TRect(TPoint(50,150), TSize(50,50)));
	iGc->SetClippingRegion(clippingRegion);
	iGc->BitBltMasked(TPoint(0,0), *bitmap, TRect(TPoint(0,0), bitmapSrcSize), *mask, EFalse);
	iGc->ResetClippingRegion();
	clippingRegion.Close();	
	TESTNOERROR(iGc->GetError());
	
	// Test with DrawBitmapMasked().
	iGc->SetOrigin(TPoint(150, 150));
	clippingRegion.AddRect(TRect(TPoint(150,150), TSize(50,50)));
	iGc->SetClippingRegion(clippingRegion);
	iGc->DrawBitmapMasked(TRect(TPoint(0,0), TSize(50,50)), *bitmap, TRect(TPoint(0,0), bitmapSrcSize), *mask, EFalse);
	iGc->ResetClippingRegion();
	clippingRegion.Close();	
	TESTNOERROR(iGc->GetError());
	
	CleanupStack::PopAndDestroy(2, bitmap);
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTClipping::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}
	
/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTClipping::doTestStepL()
	{
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat);	
		RunTestsL();
		// only run OOM tests for one target pixel format to prevent duplication of tests
		if (targetPixelFormatIndex == 0)
			{
			RunOomTestsL();  //from base class
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTClipping::RunTestsL()
	{
	if(iUseDirectGdi && !iUseSwDirectGdi)
		{
		// At least one clipping test uses BitBlt() or DrawBitmap().
		// In the event that a test leaves after a BitBlt() or DrawBitmap() has occurred
		// the vgimage cache will need to be reset.
		// This needs to be the first item on the cleanupstack, 
		// as some tests perform pushes and pops of bitmaps.
		CleanupStack::PushL(TCleanupItem(ResetCache, iVgImageCache));
		}
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLIPPING-0001"));
	TestClippingBasicL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLIPPING-0002"));
	TestClippingErrors();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLIPPING-0003"));
	TestClippingWithOriginL();
	RecordTestResultL();
	if(iUseDirectGdi && !iUseSwDirectGdi)
		{
		CleanupStack::PopAndDestroy(iVgImageCache);
		}
	}
