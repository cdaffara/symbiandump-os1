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

#include "tcopyrect.h"

CTCopyRect::CTCopyRect()
	{
	SetTestStepName(KTDirectGdiCopyRectStep);
	}

CTCopyRect::~CTCopyRect()
	{
	}
			
/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-COPYRECT-0001
	
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
	Tests copying the content of a rectangular area on the target to another location.
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions 
	Test various combinations of method parameters:
		target location is out of bounds or partially out of bounds
		target destination doesn't intersect source destination
		target destination is clipped
		source destination intersects clipping region
		source destination is partially out of bounds
		target destination overlaps source destination
		
@SYMTestExpectedResults 
	Content should be copied to specified location.
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTCopyRect::TestCopyRectL()
	{
	_LIT(KTestName, "CopyRect-Basic"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	TSize size(30,30);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iTargetPixelFormat, size);

	ResetGc();
	TPoint point(15,15);
	TRect rectToCopy(point, size);
	iGc->BitBlt(point, *bitmap);
	
	// copy rect where target destination is out of bounds
	iGc->CopyRect(TPoint(-100,0), rectToCopy);
	
	// copy rect where target destination is partially out of bounds
	iGc->CopyRect(TPoint(40,-34), rectToCopy);
	
	// copy rect where target destination doesn't intersect source destination
	iGc->CopyRect(TPoint(17,44), rectToCopy);

	// copy rect where target destination is clipped
	RRegion clippingRegion(4);
	clippingRegion.AddRect(TRect(60,90,100,130));
	clippingRegion.SubRect(TRect(80,105,90,115));
	iGc->SetClippingRegion(clippingRegion);
	iGc->CopyRect(TPoint(55,85), rectToCopy);
	clippingRegion.Clear();
	
	// copy rect where source destination intersects clipping region (shouldn't actually be clipped)
	clippingRegion.AddRect(TRect(25,25,150,150));
	iGc->SetClippingRegion(clippingRegion);
	iGc->CopyRect(TPoint(60,30), rectToCopy);
	iGc->ResetClippingRegion();	
	clippingRegion.Close();
	
	// copy rect where source destination is partially out of bounds
	iGc->CopyRect(TPoint(0,95), TRect(TPoint(-2,15),size));
	
	// copy rect where target destination overlaps source destination
	iGc->CopyRect(TPoint(12,12), rectToCopy);
	delete bitmap;

	TESTNOERROR(iGc->GetError());
	if (iUseDirectGdi && !iUseSwDirectGdi)
		{
		iVgImageCache->ResetCache();
		}
	TInt err = WriteTargetOutput(iTestParams, TPtrC(KTestName));
	TEST(KErrNone == err);
	}


/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-COPYRECT-0002
	
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
	Tests calling CopyRect() with invalid parameters.
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions 
	Blit a bitmap.
	Test CopyRect() with invalid rects ( e.g. iTl.iX>iBr.iX and other ).
	The offset is set to out of blited bitmap area.
	Clear blited bitmap area.
	
@SYMTestExpectedResults 
	There should be no effect of using CopyRect(). So, after clearing 
	blited bitmap area, the whole target should be clear and
	no bitmap will be created.
*/
void CTCopyRect::TestCopyRectInvalidParametersL()
	{
	_LIT(KTestName, "CopyRect-InvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	//blit a bitmap
	TSize size(40,40);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iTargetPixelFormat, size);
	iGc->BitBlt(TPoint(0,0), *bitmap);
	
	//test invalid rectangles but on bitmap area drawn above
	for(TInt i =1; i<9; i++)
		{
		TRect rect( (i%3)*20 , (i/3)*20 , 20 , 20  );
		iGc->CopyRect(TPoint(40,40),rect);
		TESTNOERROR(iGc->GetError());
		}
	//test with offset (0,0), this should just return and do nothing
	iGc->CopyRect(TPoint(0,0),TRect(0,0,100,100));
	//clear blited bitmap area
	iGc->Clear(TRect(0,0,40,40));
	TESTNOERROR(iGc->GetError());
	
	delete bitmap;
	if (iUseDirectGdi && !iUseSwDirectGdi)
		{
		iVgImageCache->ResetCache();
		}
	// test if target is still clear
	TBool pass = TestTargetL(KRgbWhite);
	if(!pass)
		{
		// write target only if test failed
		TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
		}
	TEST(pass);
	}

/**
@SYMTestCaseID GRAPHICS-DIRECTGDI-COPYRECT-0003

@SYMDEF PDEF138111

@SYMTestCaseDesc Test CDirectGdiContext::CopyRect() with non-trivial alpha channel
 
@SYMTestPriority Normal

@SYMTestStatus Implemented

@SYMTestActions Fill the upper half of the bitmap with semi-transparent gray. 
				Call CDirectGdiContext::CopyRect() to copy the upper half to the lower half.
 				Sample one pixel from both the upper and lower half of the bitmap.

@SYMTestExpectedResults The two pixels should be the same. Without the fix the lower part
						would show a lighter shade of the gray.
*/
void CTCopyRect::CopyRectAlphaL()
	{	
	ResetGc();
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->Clear();
	TRgb color = TRgb(128, 128, 128, 128);
	iGc->SetBrushColor(color);
	iGc->SetPenColor(color);
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
	TSize size = iGdiTarget->SizeInPixels();
	iGc->DrawRect(TRect(0, 0, size.iWidth, size.iHeight/2));
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	iGc->CopyRect(TPoint(0, size.iHeight/2), TRect(0, 0, size.iWidth, size.iHeight/2));
	
	//sample a pixel from the lower part of the bitmap and it should be the same as the upper half 
	TRgb pixelColorUpper;
	iGdiTarget->GetTargetFbsBitmapL()->GetPixel(pixelColorUpper, TPoint(size.iWidth/2, size.iHeight/4));
	TRgb pixelColorLower;
	iGdiTarget->GetTargetFbsBitmapL()->GetPixel(pixelColorLower, TPoint(size.iWidth/2, size.iHeight*3/4));
	TEST(pixelColorUpper == pixelColorLower);
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTCopyRect::doTestStepPreambleL()
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
TVerdict CTCopyRect::doTestStepL()
	{
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat);	
		RunTestsL();
		RunOomTestsL();  //from base class
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTCopyRect::RunTestsL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-COPYRECT-0001"));
	TestCopyRectL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-COPYRECT-0002"));
	TestCopyRectInvalidParametersL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-COPYRECT-0003"));
	CopyRectAlphaL();
	RecordTestResultL();
	}
