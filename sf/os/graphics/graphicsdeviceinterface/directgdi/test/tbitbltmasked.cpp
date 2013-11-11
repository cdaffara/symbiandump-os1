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

#include "tbitbltmasked.h"

CTBitBltMasked::CTBitBltMasked()
	{
	SetTestStepName(KTDirectGdiBitBltMaskedStep);
	}

CTBitBltMasked::~CTBitBltMasked()
	{
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTBitBltMasked::RunTestsL()
	{
	if(iUseDirectGdi && !iUseSwDirectGdi)
		{
		// In the event that a test leaves after a BitBlt() or DrawBitmap() has occurred
		// the vgimage cache will need to be reset.
		// This needs to be the first item on the cleanupstack, 
		// as some tests perform pushes and pops of bitmaps.
		CleanupStack::PushL(TCleanupItem(ResetCache, iVgImageCache));
		}

	BeginMaskIteration();
	// to minimize tests call number these tests are processed only for one mask
	if(!iLargeTarget)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0003"));
		TestInvalidParametersL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0004"));
		TestSourceBitmapCloningL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0005"));
		TestMaskCloningL();
		RecordTestResultL();
		}
	do
		{
		if(!iLargeTarget)
			{
			iTestParams.iDoCompressed = ETrue;
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0001"));
			TestBasicL();
			RecordTestResultL();
			iTestParams.iDoCompressed = EFalse;
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0006"));
			TestDrawModeL();			
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0007"));
			TestWithSameBitmapsL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0008"));
			TestSetOriginL();
			RecordTestResultL();
			}
		else
			{
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-BITBLTMASKED-0002"));
			TestPositioningL();
			RecordTestResultL();
			}
		}
	while(NextMaskIteration());

	if(iUseDirectGdi && !iUseSwDirectGdi)
		{
		CleanupStack::PopAndDestroy(iVgImageCache);
		}
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0001
	
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
	Bit blitting with masking basic functionality.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test bit blitting with masking for basic and valid parameters.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Basic or inverted
	masking should be considered. Valid bitmap should be created. This
	bitmap should be the same as a reference bitmap.
*/
void CTBitBltMasked::TestBasicL()
	{	
	_LIT(KTestName, "BitBltMasked-Basic"); 
	_LIT(KTestNameNegative, "BitBltMasked-Basic-NegativeMaskPosition");	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	ResetGc();
	CFbsBitmap* bitmap;
	CFbsBitmap* mask;
	if(iTestParams.iDoCompressed)
		{
		bitmap = iCompressedBitmap;
		mask = iCompressedMaskL8;
		}
	else
		{
		bitmap = iCheckedBoardBitmap2;
		mask = iCurrentMask1;
		}
	
	TRect bmpRect(TPoint(0, 0), bitmap->SizeInPixels());	
	if(iMaskPixelFormat == EUidPixelFormatL_1)
		{
		iGc->BitBltMasked(TPoint(20, 20), *bitmap, bmpRect, *mask, iInvertMask);
		iGc->BitBltMasked(TPoint(20, 20), *bitmap, bmpRect, *mask, ETrue);
		}
	else
		{
		iGc->BitBltMasked(TPoint(20, 20), *bitmap, bmpRect, *mask, TPoint(0, 0));
		}
	TESTNOERROR(iGc->GetError());
	if(!iTestParams.iDoCompressed)
		WriteTargetOutput(KTestName());
	
	// negative mask position
	iGc->BitBltMasked(TPoint(-35, 180), *bitmap, bmpRect, *iMask1L8, TPoint(-1, -1));
	TESTNOERROR(iGc->GetError());	
	
	// mask position greater than mask size
	iGc->BitBltMasked(TPoint(-35, 200), *bitmap, bmpRect, *iMask1L8, iMask1L8->SizeInPixels().AsPoint());
	CheckErrorsL(KErrNone, KErrArgument, (TText8*)__FILE__, __LINE__);
	
	if(!iTestParams.iDoCompressed)
		TESTNOERRORL(WriteTargetOutput(KTestNameNegative()));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0002
	
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
 	of a bitmap are tested. Test also covers mask alignment, tiling and boundary
 	parameters.
 	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions
	Covers the whole target bitmap starting from TPoint(-30,-30) with staggered blits of a small 8x8 bitmap. 	
	Test positioning functionality of BitBltMasked() methods. Test boundary
 	conditions, tiling and clipping to surface area.
 	
@SYMTestExpectedResults 
*/
void CTBitBltMasked::TestPositioningL()
	{	
	_LIT(KTestName, "BitBltMasked-Positioning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	iTestParams.iDoCompressed = ETrue;
	TestPositioningBaseL(KTestName(), EBitBltMasked);
	iTestParams.iDoCompressed = EFalse;
	TestPositioningBaseL(KTestName(), EBitBltMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0003
	
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
	Calling BitBltMasked() method with invalid parameters
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Invalid source rect:
		- TRect(-30, -30, -10, -10);
		- TRect(bmpWidth+10, bmpHeight+10, bmpWidth+20, bmpHeight+20);
		- TRect(bmpWidth, bmpHeight, 0, 0);
		- TRect(-10, -10, -30, -30);
		- TRect(0, 0, 0, 0);
	Invalid mask (aMaskBitmap):
		- zero size mask
		- not initialized mask
	Mask position greater than mask size: aMaskPt = TPoint(mask_width, mask_height)
    Invalid source bitmap:
		- zero size bitmap
        - not initialised bitmap

@SYMTestExpectedResults 
	Function should detect invalid parameters and return. Nothing should be drawn
 	in a target area.
*/
void CTBitBltMasked::TestInvalidParametersL()
	{	
	_LIT(KTestName, "BitBltMasked-InvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	TestInvalidParametersBaseL(KTestName(), EBitBltMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0004
	
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
	Tests that masked bitmaps are drawn correctly if they 
	are deleted immediately after BitBltMasked() is called, but before Finish() is called.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid mask parameters (see BitBltMasked-Basic).
	For test description see BitBlt-SourceBitmapCloning.
	 
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Memory leaks should not
 	be created. Valid bitmap should be created. This bitmap should be
 	the same as a reference bitmap.
*/
void CTBitBltMasked::TestSourceBitmapCloningL()
	{	
	_LIT(KTestName, "BitBltMasked-SourceBitmapCloning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	iTestParams.iDoCompressed = ETrue;
	TestSourceBitmapCloningBaseL(KTestName(), EBitBltMasked);
	iTestParams.iDoCompressed = EFalse;	
	TestSourceBitmapCloningBaseL(KTestName(), EBitBltMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0005
	
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
	Tests that masked bitmaps are drawn correctly if the mask 
	is deleted immediately after BitBltMasked() is called, but before Finish() is called.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented

@SYMTestActions 	
	1. Invoke the BitBltMasked() method with valid parameters (see BitBltMasked-Basic). 
	2. Right after returning from BitBltMasked() call, destroy the source mask (aMaskBitmap).
	3. Call the BitBltMasked() method again with the same mask parameter.
	Repeat  p. 1-3 a few times.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area and should be correctly masked.
 	Memory leaks should not be created. Valid bitmap should be created. This bitmap
 	should be the same as a reference bitmap.
*/	
void CTBitBltMasked::TestMaskCloningL()
	{	
	_LIT(KTestName, "BitBltMasked-MaskCloning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	iTestParams.iDoCompressed = ETrue;
	TestMaskCloningBaseL(KTestName(), EBitBltMasked);
	iTestParams.iDoCompressed = EFalse;
	TestMaskCloningBaseL(KTestName(), EBitBltMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0006
	
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
	Test correctness of bit blitting with mask in all possible draw modes.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid parameters (see BitBltMasked-Basic).
	Use the semi-transparent source bitmap (iCheckedBoardWithAlphaBitmap).
	Set drawmode to EDrawModePEN and call the methods.
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	
@SYMTestExpectedResults 
	Semi-transparent (for  EDrawModePEN) and opaque (for  EDrawModeWriteAlpha)
 	should be copied into rendering target. Valid bitmap should be created.
 	This bitmap should be the same as a reference bitmap.
*/
void CTBitBltMasked::TestDrawModeL()
	{
	_LIT(KTestName, "BitBltMasked-DrawMode"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	ResetGc();

	TSize bmpSize(iCheckedBoardWithAlphaBitmap->SizeInPixels());

	TInt halfHeight = (bmpSize.iHeight  >> 1);	
	TInt quarterWidth  = (bmpSize.iWidth  >> 2);
	TInt y1 = halfHeight - (bmpSize.iHeight >> 2);	
	TSize vertRectSize  (quarterWidth, bmpSize.iHeight);
	TSize horizRectSize (bmpSize.iWidth, halfHeight);

	// EDrawModeWriteAlpha is not supported in the following modes
	// This is tested under invalid parameter tests.
	if (!((iMaskPixelFormat == EUidPixelFormatL_8) && 
		  ((iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888) ||
		   (iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888_PRE))))
		{
		// Render column to left
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		iGc->BitBltMasked(
			TPoint(0, 0),
			*iCheckedBoardWithAlphaBitmap,
			TRect(TPoint(0, 0), vertRectSize), 
			*iCurrentMask2, 
			TPoint(0, 0));
		}

	// Render row in center
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	iGc->BitBltMasked(
		TPoint(0, y1), 
		*iCheckedBoardWithAlphaBitmap, 
		TRect(TPoint(0, 0), horizRectSize),
		*iCurrentMask2, 
		TPoint(0, 0));
	
	// EDrawModeWriteAlpha is not supported in the following modes
	// This is tested under invalid parameter tests.
	if (!((iMaskPixelFormat == EUidPixelFormatL_8) && 
		  ((iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888) ||
		   (iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888_PRE))))
		{
		// Render column to right
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		iGc->BitBltMasked(
			TPoint(bmpSize.iWidth - quarterWidth, 0),
			*iCheckedBoardWithAlphaBitmap,
			TRect(TPoint(0, 0), vertRectSize), 
			*iCurrentMask2, 
			TPoint(0, 0));
		}
	
	TESTNOERRORL(iGc->GetError());

	TESTNOERRORL(WriteTargetOutput(KTestName()));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0007
	
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
	Test the basic functionality of bit blitting with masking.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test bit blitting with masking for same source and mask bitmaps.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Basic or inverted
 	masking should be considered. Valid bitmap should be created. This
 	bitmap should be the same as a reference bitmap.
*/
void CTBitBltMasked::TestWithSameBitmapsL()
	{	
	_LIT(KTestName, "BitBltMasked-SameBitmaps"); 
	ResetGc();
	TRect bmpRect1(TPoint(0, 0), iBlackWhiteBitmap->SizeInPixels());
	iGc->SetOrigin(TPoint(30, 30));
	iGc->BitBltMasked(TPoint(0, 0), *iBlackWhiteBitmap, bmpRect1, *iBlackWhiteBitmap, ETrue);
	iGc->BitBltMasked(TPoint(0, 0), *iBlackWhiteBitmap, bmpRect1, *iBlackWhiteBitmap, EFalse);
	TESTNOERRORL(iGc->GetError());
	//Write target only if image is not empty. BitGDI is producing white images when Mask pixel format is EUidPixelFormatL_8 
	TBool pass = TestTargetL(KRgbWhite);
	if(!pass)
		{		
		TESTNOERRORL(WriteTargetOutput(KTestName()));
		}	
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-BITBLTMASKED-0008
	
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
	Test that BitBltMasked() performs correctly when it is called after SetOrigin().
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Call SetOrigin().
	Call BitBltMasked().
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Basic or inverted
	masking should be considered. Valid bitmap should be created. This
	bitmap should be the same as a reference bitmap.
*/
void CTBitBltMasked::TestSetOriginL()
	{	
	_LIT(KTestName, "BitBltMasked-SetOrigin"); 
	_LIT(KTestNameNegative, "BitBltMasked-SetOrigin-NegativeMaskPosition");
	
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
	CFbsBitmap* mask = iCurrentMask1;	
	
	TRect bmpRect(TPoint(0, 0), bitmap->SizeInPixels());
	iGc->SetOrigin(TPoint(-35, 80));
	if(iMaskPixelFormat == EUidPixelFormatL_1)
		{
		iGc->BitBltMasked(TPoint(55, -60), *bitmap, bmpRect, *mask, iInvertMask);
		iGc->BitBltMasked(TPoint(55, -60), *bitmap, bmpRect, *mask, ETrue);
		}
	else
		{
		iGc->BitBltMasked(TPoint(55, -60), *bitmap, bmpRect, *mask, TPoint(0, 0));
		}
	TESTNOERROR(iGc->GetError());	
	TESTNOERRORL(WriteTargetOutput(KTestName()));
	
	// negative mask position
	iGc->BitBltMasked(TPoint(0, 100), *bitmap, bmpRect, *iMask1L8, TPoint(-1, -1));
	TESTNOERROR(iGc->GetError());	
	
	// mask position greater than mask size
	iGc->BitBltMasked(TPoint(0, 120), *bitmap, bmpRect, *iMask1L8, iMask1L8->SizeInPixels().AsPoint());
	CheckErrorsL(KErrNone, KErrArgument, (TText8*)__FILE__, __LINE__);
		
	TESTNOERRORL(WriteTargetOutput(KTestNameNegative()));
	}


