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

#include "tdrawbitmapmasked.h"

CTDrawBitmapMasked::CTDrawBitmapMasked()
	{
	SetTestStepName(KTDirectGdiDrawBitmapMaskedStep);
	}

CTDrawBitmapMasked::~CTDrawBitmapMasked()
	{
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDrawBitmapMasked::RunTestsL()
	{
	if (iUseDirectGdi && !iUseSwDirectGdi)
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
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0003"));
		TestInvalidParametersL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0005"));
		TestSourceBitmapCloningL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0006"));
		TestMaskCloningL();
		RecordTestResultL();
		}
	do
		{
		if(!iLargeTarget)
			{
			iTestParams.iDoCompressed = ETrue;
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0001"));
			TestBasicL();
			RecordTestResultL();
			iTestParams.iDoCompressed = EFalse;
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0007"));
			TestDrawModeL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0008"));
			TestWithSameBitmapsL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0009"));	
			TestSetOriginL();
			RecordTestResultL();
			}
		else
			{
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0002"));
			TestPositioningL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0004"));
			TestScalingL();
			RecordTestResultL();
			
			if (iMaskType == EMaskL8)
				{
				iTestParams.iDoCompressed = ETrue;
				SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0004"));
				TestScalingL();
				RecordTestResultL();
				iTestParams.iDoCompressed = EFalse;
				}
			}
		}
	while(NextMaskIteration());
	if (iUseDirectGdi && !iUseSwDirectGdi)
		{
		CleanupStack::PopAndDestroy(iVgImageCache);
		}
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0001
	
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
	Bitmap drawing with masking basic functionality test.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test bitmap drawing with masking for simple and valid parameters.
	
@SYMTestExpectedResults 
	Source bitmap should be drawn masked in the target area. Valid bitmap 
	should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawBitmapMasked::TestBasicL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-Basic");		
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
	
	TSize bmpSize(bitmap->SizeInPixels());
	
	iGc->DrawBitmapMasked(TRect(TPoint(20, 20), bmpSize), *bitmap, TRect(TPoint(0, 0), bmpSize), *mask, iInvertMask);

	TESTNOERRORL(iGc->GetError());
	if(!iTestParams.iDoCompressed)
		WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0002
	
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
	Test regularity of bitmap with masking positioning.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test positioning functionality of DrawBitmapMasked() methods.
 	Test boundary conditions and clipping to surface area.
 	
@SYMTestExpectedResults 
*/
void CTDrawBitmapMasked::TestPositioningL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-Positioning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	iTestParams.iDoCompressed = ETrue;
	TestPositioningBaseL(KTestName(), EDrawBitmapMasked);
	iTestParams.iDoCompressed = EFalse;
	TestPositioningBaseL(KTestName(), EDrawBitmapMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0003
	
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
	Replace valid parameters with invalid ones:
		Invalid mask
			zero size mask
			not initialized mask
		see DrawBitmap-InvalidParameters for rest of invalid parameters.
		
@SYMTestExpectedResults 
	Function should detect invalid parameters and return. Nothing should be
 	drawn in  target area.
*/
void CTDrawBitmapMasked::TestInvalidParametersL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-InvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TestInvalidParametersBaseL(KTestName(), EDrawBitmapMasked);
	}

/**
@SYMTestCaseID 
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0004

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
	Test scaling down and up of a source bitmap. Test scaling down and tiling of a mask.

@SYMTestExpectedResults 
	Scaled bitmap should be drawn in a target area. Masking should be proceeded
	correctly. Valid bitmap should be created. The bitmap should be properly
	scaled. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawBitmapMasked::TestScalingL()
	{
	_LIT(KTestName, "DrawBitmapMasked-Scaling"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;

	CFbsBitmap* bitmap1 = NULL;
	CFbsBitmap* bitmap2 = NULL;
	CFbsBitmap* mask = NULL;
	if(iTestParams.iDoCompressed)
		{
		bitmap1 = iCompressedBitmapSmall;
		bitmap2 = iCompressedBitmap;
		mask = iCompressedMaskL8;
		}
	else
		{
		bitmap1 = iCheckedBoardBitmap1;
		bitmap2 = iCheckedBoardBitmap2;
		mask = iCurrentMask1;
		}
	TInt bmp1Width = bitmap1->SizeInPixels().iWidth;
	TInt bmp1Height = bitmap1->SizeInPixels().iHeight;
	
	TInt bmp2Width = bitmap2->SizeInPixels().iWidth;
	TInt bmp2Height = bitmap2->SizeInPixels().iHeight;

	ResetGc();

	// small bitmap - extreme magnification
	TReal32 scale = 1.0f;
	TReal32 scaleFactor = 1.55f;
	TInt posX = 1;
	for(TInt jj=0; jj<6; jj++)
		{
		TSize size(static_cast<TInt>(bmp1Width*scale), static_cast<TInt>(bmp1Height*scale));
		TPoint pos(posX, height/4-size.iHeight/2);

		iGc->DrawBitmapMasked(TRect(pos, size), *bitmap1,
				TRect(TPoint(0, 0), TSize(bmp1Width, bmp1Height)), *mask, iInvertMask);

		posX += size.iWidth+2;
		scale *= scaleFactor;
		}

	// large bitmap - extreme minification
	scale /= scaleFactor;
	posX = 1;
	for(TInt jj=5; jj>=0; jj--)
		{
		TSize size(static_cast<TInt>(bmp1Width*scale), static_cast<TInt>(bmp1Height*scale));
		TPoint pos(posX, 3*height/4-size.iHeight/2);

		iGc->DrawBitmapMasked(TRect(pos, size), *bitmap2,
				TRect(TPoint(0, 0), bitmap2->SizeInPixels()), *mask, iInvertMask);

		posX += size.iWidth+2;
		scale /= scaleFactor;
		}

	TESTNOERRORL(iGc->GetError());

	WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0005
	
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
	Deleting source bitmap object after issuing draw bitmap  with masking requests.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	1. Call method with valid parameters (see  DrawBitmapMasked-Basic). 
	2. Right after returning from method call, destroy the source bitmap.
	3. Call the method again with the same bitmap parameter.
	Repeat  p. 1-3 a few times.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. Memory leaks should not
	be created. Valid bitmap should be created. This bitmap should be the same
	as a reference bitmap.
*/
void CTDrawBitmapMasked::TestSourceBitmapCloningL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-SourceBitmapCloning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	iTestParams.iDoCompressed = ETrue;
	TestSourceBitmapCloningBaseL(KTestName(), EDrawBitmapMasked);
	iTestParams.iDoCompressed = EFalse;
	TestSourceBitmapCloningBaseL(KTestName(), EDrawBitmapMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0006
	
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
	Deleting mask objects after issuing draw bitmap with masking  request.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	1. Call DrawBitmapMasked() method with valid parameters (see DrawBitmapMasked-Basic). 
	2. Right after returning from DrawBitmapMasked() call, destroy the source mask (aMaskBitmap).
	3. Call the DrawBitmapMasked() method again with the same mask parameter.
	Repeat  p. 1-3 a few times.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area based on a mask data.
	Memory leaks should not be created. Valid bitmap should be created. This bitmap
	should be the same as a reference bitmap.
*/
void CTDrawBitmapMasked::TestMaskCloningL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-MaskCloning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	iTestParams.iDoCompressed = ETrue;
	TestMaskCloningBaseL(KTestName(), EDrawBitmapMasked);
	iTestParams.iDoCompressed = EFalse;
	TestMaskCloningBaseL(KTestName(), EDrawBitmapMasked);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0007
	
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
	Test correctness of bitmap drawing with mask in all possible draw modes.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Set valid parameters (see DrawBitmapMasked-Basic).
	Use the semi-transparent source bitmap (aBitmap).
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	Set drawmode to EDrawModePEN and call the methods.
	Set drawmode to EDrawModeWriteAlpha and call the methods.
	
@SYMTestExpectedResults 
	The image drawn in the center should be drawn masked and blended 
	(EDrawModePEN) and the images at the right and left should be drawn masked and 
	opaque (EDrawModeWriteAlpha). Valid bitmap should be created.
	This bitmap should be the same as a reference bitmap.
*/
void CTDrawBitmapMasked::TestDrawModeL()
	{
	_LIT(KTestName, "DrawBitmapMasked-DrawMode"); 
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
	
	// EDrawModeWriteAlpha is not supported in the following modes
	// This is tested under invalid parameter tests.
	if (!((iMaskPixelFormat == EUidPixelFormatL_8) && 
		  ((iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888) ||
		   (iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888_PRE))))
		{
		// Render column to left
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		iGc->DrawBitmapMasked(
			TRect (TPoint(0, 0), vertRectSize), 
			*iCheckedBoardWithAlphaBitmap, 
			TRect(TPoint(0, 0), vertRectSize), *iCurrentMask2, iInvertMask);
		}
	
	// Render row in center
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	iGc->DrawBitmapMasked(
		TRect (TPoint(0, y1), horizRectSize),
		*iCheckedBoardWithAlphaBitmap,
		TRect(TPoint(0, 0), horizRectSize), *iCurrentMask2, iInvertMask);

	// EDrawModeWriteAlpha is not supported in the following modes
	// This is tested under invalid parameter tests.
	if (!((iMaskPixelFormat == EUidPixelFormatL_8) && 
		  ((iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888) ||
		   (iTestParams.iSourcePixelFormat == EUidPixelFormatARGB_8888_PRE))))
		{
		// Render column to right
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		iGc->DrawBitmapMasked(
			TRect (TPoint(bmpSize.iWidth - quaterWidth, 0), vertRectSize), 
			*iCheckedBoardWithAlphaBitmap, 
			TRect(TPoint(0, 0), vertRectSize), *iCurrentMask2, iInvertMask);
		}
	
	TESTNOERRORL(iGc->GetError());
	WriteTargetOutput(KTestName());
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0008
	
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
	Test the basic functionality of bit blitting with masking.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Test bitmap drawing with masking for same source and mask bitmaps.
	
@SYMTestExpectedResults 
	Source bitmap should be copied into target area. A mask should be
	considered during coping. Valid bitmap should be created. This bitmap
	shall be compared to a reference bitmap.
*/
void CTDrawBitmapMasked::TestWithSameBitmapsL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-SameBitmaps"); 
	ResetGc();
	TSize bmpSize1(iBlackWhiteBitmap->SizeInPixels());
	iGc->DrawBitmapMasked(TRect(TPoint(0, 0), bmpSize1), *iBlackWhiteBitmap, TRect(TPoint(0, 0), bmpSize1), *iBlackWhiteBitmap, ETrue);
	iGc->DrawBitmapMasked(TRect(TPoint(0, 0), bmpSize1), *iBlackWhiteBitmap, TRect(TPoint(0, 0), bmpSize1), *iBlackWhiteBitmap, EFalse);
	TESTNOERROR(iGc->GetError());
	//Write target only if image is not empty. BitGDI is producing white images when Mask pixel format is EUidPixelFormatL_8 
	TBool pass = TestTargetL(KRgbWhite);
	if(!pass)
		{		
		WriteTargetOutput(KTestName());
		}	
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-DRAWBITMAPMASKED-0009
	
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
	Test that DrawBitmapMasked() performs correctly if it is called after SetOrigin().
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	
	Call SetOrigin().
	Call DrawBitmapMasked().
	
@SYMTestExpectedResults 
	Source bitmap should be drawn masked in the target area. Valid bitmap 
	should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawBitmapMasked::TestSetOriginL()
	{	
	_LIT(KTestName, "DrawBitmapMasked-SetOrigin");
	
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
	TSize bmpSize(bitmap->SizeInPixels());

	iGc->SetOrigin(TPoint(-35, 80));
	iGc->DrawBitmapMasked(TRect(TPoint(55, -60), bmpSize), *bitmap, TRect(TPoint(0, 0), bmpSize), *mask, iInvertMask);

	TESTNOERRORL(iGc->GetError());	
	WriteTargetOutput(KTestName());
	}
