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

#include "tgeneraldrawing.h"

#include <e32math.h>

CTGeneralDrawing::CTGeneralDrawing()
	{
	SetTestStepName(KTDirectGdiGeneralDrawingStep);
	}

CTGeneralDrawing::~CTGeneralDrawing()
	{
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0001

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

@SYMTestPriority
	Critical

@SYMTestCaseDesc
	Check the results of drawing to a context which has been reset, the default context settings 
	should be used rather than any previously set settings.

@SYMTestActions		
	Set a pen color
	origin
	pen style
	pen size
	brush color
	brush style
	brush pattern
	brush origin
	draw mode
	line pos
	and draw a rect, then reset the context and draw the another rect.
	The second rect should be drawn with the default settings and not 
	any of the previously set settings.
	Draw a third rect with a brush pattern in draw mode write alpha,
	reset again and draw a fourth rect. The fourth rect should also be 
	drawn with default settings.

@SYMTestExpectedResults
	The second rectangle should be drawn with the default settings rather then any of the previously set settings.
	The fourth rectangle should also be drawn with default settings.
	Valid bitmap should be created. This bitmap  should be the same as appropriate reference bitmap.

@SYMTestStatus
	Implemented
*/
void CTGeneralDrawing::TestDrawToResetContextL()
	{
	_LIT(KTestName, "Drawing-DrawToResetContext"); //test case name
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	// create a brush pattern programatically so we can test EPatternedBrush	
	TUidPixelFormat pixelFormat = iTestParams.iTargetPixelFormat;
	TSize patternSize(50,50);
	CFbsBitmap* pattern = CreateCheckedBoardBitmapL(pixelFormat, patternSize);
	CleanupStack::PushL(pattern);
	TEST(pattern != NULL);	
	
	ResetGc();
	
	// Draw a rectangle and line with lots of settings set
	iGc->SetOrigin(TPoint(2,2));
	iGc->SetPenColor(KRgbRed);
	iGc->SetPenStyle(DirectGdi::EDotDashPen);
	iGc->SetPenSize(TSize(3,3));
	iGc->SetBrushColor(KRgbYellow);
	iGc->SetBrushStyle(DirectGdi::EDiamondCrossHatchBrush);
	iGc->SetBrushOrigin(TPoint(1,1));	
	iGc->DrawRect(TRect(10,10,140,30));
	iGc->MoveTo(TPoint(10,20));
	iGc->DrawLineBy(TPoint(150,0));	
	
	iGc->Reset();
	
	TESTNOERROR(iGc->GetError());		
	
	// Draw a rectangle and line with no settings set
	iGc->DrawRect(TRect(10,40,140,60));
	iGc->DrawLine(TPoint(10,50), TPoint(150,50));				
	
	// Draw a rectangle and line with a brush pattern in draw mode write alpha
	TRgb redWithAlpha(255,0,0,125);
	iGc->SetBrushPattern(*pattern);
	iGc->SetBrushStyle(DirectGdi::EPatternedBrush);
	iGc->SetPenColor(redWithAlpha);
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);	
	iGc->DrawRect(TRect(10,80,140,100));
	iGc->DrawLine(TPoint(10,90), TPoint(150,90));
	
	TESTNOERROR(iGc->GetError());		
	
	iGc->Reset();		
	
	// Draw a rectangle and line with just the previous pen color set (to check draw mode has been reset properly)
	iGc->SetPenColor(redWithAlpha);
	iGc->DrawRect(TRect(10,120,140,140));	
	iGc->MoveBy(TPoint(10,130)); // should move from 0,0 if the previous position was reset properly
	iGc->DrawLineBy(TPoint(140,0));			
	
	TESTNOERROR(iGc->GetError());		
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	
	CleanupStack::PopAndDestroy(pattern);
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0002

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
	Check the results of drawing to a context with only the default pen, brush etc. set.	

@SYMTestPriority
	Critical

@SYMTestActions		
	Draw some shapes to a context without setting pen and brush colors etc. and make sure
	we get the same result when drawing with bitgdi and directgdi

@SYMTestExpectedResults
	The BitGdi and DirectGdi created bitmaps should be the same

@SYMTestStatus
	Implemented
*/
void CTGeneralDrawing::TestDrawToDefaultContextL()
	{
	_LIT(KTestName, "Drawing-DrawToDefaultContext"); //test case name
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(5);
	CleanupStack::PushL(array);
	array->AppendL(TPoint(110, 60));
	array->AppendL(TPoint(120, 93));
	array->AppendL(TPoint(130, 93));
	array->AppendL(TPoint(142, 67));
	array->AppendL(TPoint(142, 110));			
	
	iGc->DrawRect(TRect(10,10,140,30));
	iGc->DrawLine(TPoint(5,7), TPoint(50,130));
	iGc->DrawArc(TRect(50,60,140,140), TPoint(0,0), TPoint(150,150));
	iGc->DrawPolyLine(*array);
	
	TESTNOERROR(iGc->GetError());		
	iGc->Reset();
	CleanupStack::PopAndDestroy(array);
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0003

@SYMTestPriority
	Critical

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

@SYMTestStatus
	Complete

@SYMTestCaseDesc
	Test the case where a target is closed after it has been activated and then it is drawn to.

@SYMTestActions	
	Test the use case where we:
	Create a target.
	Activate the target.
	Close the target.
	Draw to the target.
	If the reference counting fails this test will panic with error DGDIAdapter 28 EglMakeCurrentError.
	
@SYMTestStatus
	Implemented
 */
void CTGeneralDrawing::DrawToClosedActiveTargetL()
	{
	_LIT(KTestName, "Drawing-DrawToClosedActiveTarget"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	// Close the target before drawing to it to check that the reference count increment
	// that is applied when the target is activated is working correctly
	iGdiTarget->Close();
	
	iGc->SetPenColor(TRgb(100,100,100));
	TEST(iGc->PenColor() == TRgb(100,100,100));
	iGc->DrawEllipse(TRect(0,0,30,30));
	TESTNOERROR(iGc->GetError());
	
	TESTL(KErrNone == WriteTargetOutput(iTestParams, KTestName()));	
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0004

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
	Tests the draw modes DirectGdi::EDrawModePEN and DirectGdi::EDrawModeWriteAlpha are set correctly
	when drawing rectangles.

@SYMTestPriority 
	Critical

@SYMTestActions 
	Set the draw mode to DirectGdi::EDrawModePEN.
	Draw several red rectangles with green borders, some with half alpha pen and brush set.
	Set the draw mode to DirectGdi::EDrawModeWriteAlpa.
	Draw the same rectangles.

@SYMTestExpectedResults
	The rectangles drawn in DirectGdi::EDrawModePEN should be drawn blended when they are drawn with half alpha 
	(the top two red rectangles)).
	The rectangles drawn in DirectGdi::EDrawModeWriteAlpha should not blend, they should write over whatever was
	drawn underneath them (the rest of the red rectangles).
	
@SYMTestStatus
	Implemented
*/
void CTGeneralDrawing::TestSetDrawMode()
	{
	_LIT(KTestName, "Drawing-SetDrawMode"); //test case name	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	const TRgb greenWithAlpha(0,255,0,125);
	const TRgb redWithAlpha(255,0,0,125);
	const TRect rectBack(50,10,100,200);
	const TRect rectA(10,30,70,60);
	const TRect rectB(80,30,140,60);
	const TRect rectC(10,90,70,120);
	const TRect rectD(80,90,140,120);
	const TRect rectE(10,150,70,180);
	const TRect rectF(80,150,140,180);
					
	// Test EDrawModePEN
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	
	iGc->SetPenColor(TRgb(KRgbBlack));
	iGc->SetPenSize(TSize(3,3));

	// Draw a large rect background rect
	iGc->SetBrushColor(KRgbBlue);
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->DrawRect(rectBack);
	TESTNOERROR(iGc->GetError());

	// Draw two rects in draw mode EDrawModePEN	
	iGc->SetPenColor(KRgbGreen);
	iGc->SetBrushColor(KRgbRed);
	iGc->DrawRect(rectA);
	TESTNOERROR(iGc->GetError());
	iGc->SetPenColor(greenWithAlpha);
	iGc->SetBrushColor(redWithAlpha);
	iGc->DrawRect(rectB);
	TESTNOERROR(iGc->GetError());

	// Test EDrawModeWriteAlpha
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);

	// Draw two rects in draw mode EDrawModeWriteAlpha
	iGc->SetPenColor(KRgbGreen);
	iGc->SetBrushColor(KRgbRed);
	iGc->DrawRect(rectC);
	TESTNOERROR(iGc->GetError());
	iGc->SetPenColor(greenWithAlpha);
	iGc->SetBrushColor(redWithAlpha);
	iGc->DrawRect(rectD);
	TESTNOERROR(iGc->GetError());

	// Draw two rects in draw mode EDrawModeWriteAlpha with PenSize set to 1
	iGc->SetPenSize(TSize(1,1));
	iGc->SetPenColor(KRgbGreen);
	iGc->SetBrushColor(KRgbRed);
	iGc->DrawRect(rectE);
	TESTNOERROR(iGc->GetError());
	iGc->SetPenColor(greenWithAlpha);
	iGc->SetBrushColor(redWithAlpha);
	iGc->DrawRect(rectF);
	TESTNOERROR(iGc->GetError());

	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0005

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test functions behaviour after calling with invalid parameters.

@SYMTestPriority
	Critical

@SYMTestActions
	Call SetDrawMode with invalid parameters.

@SYMTestExpectedResults
	KErrArgument should be set as error.

@SYMTestStatus
	Implemented
*/
void CTGeneralDrawing::TestSetDrawModeInvalidParametersL()
	{
	_LIT(KTestName, "Drawing-SetAttributes-InvalidParameters");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	iGc->SetDrawMode((DirectGdi::TDrawMode)1234);
	if(iUseDirectGdi)
		{
		TESTL(iGc->GetError() == KErrArgument);
		}
	else
		{
		TESTNOERRORL(iGc->GetError());
		}	
	ResetGc();
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0006

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
	Test the MoveBy method performs correctly.

@SYMTestPriority
	Critical

@SYMTestActions
	Draw 10 rectangles, each shifted by 5,5 pixels from the previous one

@SYMTestExpectedResults
	Test should successfully 10 rectangles each shifted by 5,5 pixels from the previous one
	
@SYMTestStatus
	Implemented
*/
void CTGeneralDrawing::TestMoveBy()
	{	
	_LIT(KTestName, "Drawing-MoveBy"); //test case name
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TPoint move(5,5);
	
	ResetGc();
	iGc->MoveTo(TPoint(25,25));
	for (TInt i = 0; i < 10; i++)
		{
		iGc->DrawLineBy(TPoint(50,0));
		iGc->DrawLineBy(TPoint(0,50));
		iGc->DrawLineBy(TPoint(-50,0));
		iGc->DrawLineBy(TPoint(0,-50));
				
		iGc->MoveBy(move);
		TESTNOERROR(iGc->GetError());
		}
	
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0007

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
	Check that calling DrawRect() does not affect the internal drawing position.

@SYMTestPriority
	Critical

@SYMTestActions		
	Call MoveTo() to set new internal drawing position.
	Call DrawRect().
	Call DrawLineTo() to draw a line from the internal drawing position.

@SYMTestExpectedResults
	DrawRect() should not affect the internal drawing position.
	A black rectangle should be drawn, with a red line going from the top-left to the bottom-right of
	the rectangle.
	
@SYMTestStatus
	Implemented
*/
void CTGeneralDrawing::TestInternalDrawingPositionDrawRect()
	{
	_LIT(KTestName, "Drawing-InternalDrawingPositionDrawRect"); //test case name
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	const TRect rectTop(10,10,iGdiTarget->SizeInPixels().iWidth - 10, iGdiTarget->SizeInPixels().iHeight/2 - 10);
	const TRect rectBottom(10,iGdiTarget->SizeInPixels().iHeight/2 + 10,iGdiTarget->SizeInPixels().iWidth - 10, iGdiTarget->SizeInPixels().iHeight - 10);
	
	ResetGc();
	
	iGc->MoveTo(TPoint(rectTop.iBr.iX, rectTop.iTl.iY));
	iGc->SetPenSize(TSize(1,1));
	iGc->DrawRect(rectTop);
	iGc->SetPenColor(KRgbRed);
	iGc->SetPenSize(TSize(3,3));
	iGc->DrawLineTo(rectTop.iBr);
	
	iGc->SetPenColor(KRgbBlack);
	iGc->MoveTo(TPoint(rectBottom.iBr.iX, rectBottom.iTl.iY));
	iGc->SetPenSize(TSize(3,3));
	iGc->DrawRect(rectBottom);
	iGc->SetPenColor(KRgbRed);
	iGc->SetPenSize(TSize(3,3));
	iGc->DrawLineTo(rectBottom.iBr);	
	TESTNOERROR(iGc->GetError());	
	iGc->Reset();
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0008

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
	Draws various shapes with irregular pen sizes, that is with pens where the width and 
	height of the pen are different.

@SYMTestPriority
	Critical

@SYMTestActions		
	Set a irregular pen size.
	Draw various shapes

@SYMTestExpectedResults
	The following shapes should be drawn with a black outline, filled with a checked brush pattern
	ellipse, rect, plot, polygon, round rect, pie, another polygon, crossed lines.
	
@SYMTestStatus
	Implemented
 */
void CTGeneralDrawing::TestDrawShapesIrregularPenSizeL(const TSize& aPenSize)
	{
	_LIT(KTestName, "Drawing-IrregularPenSize"); //test case name
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TBuf<70> testName;
	TBuf<10> testCaseName;	
	_LIT(KPenSize, "%d_%d");
	testCaseName.Format(KPenSize, aPenSize.iWidth, aPenSize.iHeight);
	testName.Append(KTestName);
	testName.Append(KSeparator);	
	testName.Append(testCaseName);
	
	// create a brush pattern programatically so we can test EPatternedBrush	
	TUidPixelFormat pixelFormat = iTestParams.iTargetPixelFormat;
	TSize patternSize(50,50);
	CFbsBitmap* pattern = CreateCheckedBoardBitmapL(pixelFormat, patternSize);
	CleanupStack::PushL(pattern);	
	
	ResetGc();	
	
	iGc->SetPenSize(aPenSize);
	// The sw version allocates memory when you set a pen size and this memory is not freed
	// until after the __UHEAP_MARKEND macro when running OOM tests, this causes a memory 
	// leak when this test leaves. The following TCleanupItem has been added to reset the pen
	// size to 1,1 on account of a leave as settting the pen size to 1,1 deletes the allocated
	// pen memory.
	CleanupStack::PushL(TCleanupItem(ResetPenSize, iGc));
	
	iGc->SetPenColor(KRgbBlack);
	iGc->SetBrushPattern(*pattern);
	iGc->SetBrushStyle(DirectGdi::EPatternedBrush);
	
	// Draw an ellipse
	iGc->DrawEllipse(TRect(10,10,100,50));
	
	// Draw a round rect
	iGc->DrawRect(TRect(110,10,190,50));
	
	CArrayFixFlat<TPoint>* array = new (ELeave)CArrayFixFlat<TPoint>(10);
	TESTL(array != NULL);
	CleanupStack::PushL(array);
		
	// Draw a zigzag line
	for (TInt i = 0; i < 9; i++)
		{
		if (i%2)
			array->AppendL(TPoint(35+(i*15), 60));
		else
			array->AppendL(TPoint(10+(i*20), 90));
		}	
	iGc->DrawPolygon(*array, DirectGdi::EWinding);
	
	// Draw a rect
	TSize corner(30,15);
	iGc->DrawRoundRect(TRect(10,100,90,140), corner);
	
	// Draw a diamond shape
	array->Reset();
	array->AppendL(TPoint(10,170));
	array->AppendL(TPoint(50,150));
	array->AppendL(TPoint(90,170));
	array->AppendL(TPoint(50,190));
	iGc->DrawPolygon(*array, DirectGdi::EWinding);
	
	// Draw a pie
	iGc->DrawPie(TRect(110,100,190,170), TPoint(0,0), TPoint(200,0));
	
	// Plot
	iGc->Plot(TPoint(10,60));
	
	// Line
	iGc->DrawLine(TPoint(120,170), TPoint(190,190));
	iGc->DrawLine(TPoint(120,190), TPoint(190,170));
	
	TESTNOERROR(iGc->GetError());	
	
	CleanupStack::PopAndDestroy(3, pattern);
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, testName));
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-GENERALDRAWING-0009

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
	Test that shape drawing happens at the correct positions when a combination of DrawLineTo, DrawLineBy,
	MoveTo, MoveBy, SetOrigin and Reset are called in conjunction with various shape drawing methods.

@SYMTestPriority
	Critical

@SYMTestActions
	Draw a blue arc and two lines with a combination of MoveTo() and MoveBy().
	Draw a red arc and two lines with a combination of MoveTo() and MoveBy().
	Draw an two orange ellipses with different SetOrigin() calls, only one ellipse should be seen.
	Draw a magenta rect, round rect and plot after a SetOrigin() call.
	Draw a cyan polygon after a SetOrigin() call.
	Draw a green polyline after a SetOrigin() call.
	Draw three grey lines after a SetOrigin() and two MoveBy() calls.		

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap should be the same as corresponding reference bitmap
	and should contain the shapes mentioned in the SYMTestActions section above.
	
@SYMTestStatus
	Implemented
 */
void CTGeneralDrawing::TestDrawShapesChangingOriginL()
	{
	_LIT(KTestName, "Drawing-DrawShapesChangingOrigin");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();		
	
	// draw a blue arc with a line either side
	iGc->SetPenColor(KRgbBlue);
	iGc->MoveTo(TPoint(10,30));
	iGc->DrawLineTo(TPoint(50,40));
	iGc->DrawArc(TRect(50,20,150,60), TPoint(200,40), TPoint(0,40));
	iGc->MoveBy(TPoint(100,0));
	iGc->DrawLineBy(TPoint(40,-10));
	
	// draw a red arc with a line either side
	iGc->SetPenColor(KRgbRed);		
	iGc->DrawArc(TRect(50,30,150,70), TPoint(0,50), TPoint(200,50));
	iGc->MoveTo(TPoint(50,50));
	iGc->DrawLineBy(TPoint(-40,-10));
	iGc->MoveBy(TPoint(180,0));
	iGc->DrawLineBy(TPoint(-40,10));
			
	iGc->Reset();
	
	// draw an orange ellipse
	TRgb orange(255,128,64);
	iGc->SetPenColor(orange);	
	iGc->SetOrigin(TPoint(-100,-100));
	iGc->DrawEllipse(TRect(50,30,150,70));
	iGc->SetOrigin(TPoint(0,0));
	iGc->DrawEllipse(TRect(50,40,150,80));	
	
	// draw a magenta rect, round rect and plot
	iGc->SetPenColor(KRgbMagenta);	
	iGc->SetOrigin(TPoint(100,110));
	iGc->DrawRect(TRect(-75,-20,75,20));
	iGc->DrawRoundRect(TRect(-65,-10,65,30), TSize(10,10));
	iGc->Plot(TPoint(0,0));
	
	// draw a cyan polygon
	iGc->SetPenColor(KRgbCyan);	
	iGc->SetOrigin(TPoint(0,160));
	TInt numPoints = 5;
	CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(numPoints);	
	TESTL(array != NULL);
	CleanupStack::PushL(array);
	array->AppendL(TPoint(0,0));
	array->AppendL(TPoint(50,-50));
	array->AppendL(TPoint(100,0));
	array->AppendL(TPoint(150,-50));
	array->AppendL(TPoint(200,0));
	iGc->DrawPolygon(*array, DirectGdi::EAlternate);
	
	// draw a green polyline
	iGc->SetPenColor(KRgbGreen);
	iGc->SetOrigin(TPoint(0,170));
	iGc->DrawPolyLine(*array);
	
	// draw 3 grey lines
	iGc->SetPenColor(KRgbDarkGray);
	iGc->SetOrigin(TPoint(10,180));	
	iGc->DrawLine(TPoint(0,0), TPoint(180,0));
	iGc->MoveBy(TPoint(0,5));
	iGc->DrawLineTo(TPoint(0,5));
	iGc->MoveBy(TPoint(0,5));
	iGc->DrawLineBy(TPoint(180,0));
	
	// the following DrawLineTo() calls have been added for coverage, they don't draw anything, they just return
	iGc->SetPenSize(TSize(10,0));
	iGc->DrawLineTo(TPoint(100,100));
	iGc->SetPenSize(TSize(0,0));
	iGc->DrawLineTo(TPoint(100,100));
	iGc->SetPenStyle(DirectGdi::ENullPen);
	iGc->DrawLineTo(TPoint(100,100));
	
	CleanupStack::PopAndDestroy(1, array);
		
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTGeneralDrawing::doTestStepPreambleL()
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
TVerdict CTGeneralDrawing::doTestStepL()
	{		
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat);
		RunTestsL();
		RunOomTestsL();
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTGeneralDrawing::RunTestsL()
	{	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0002"));
	TestDrawToDefaultContextL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0001"));
	TestDrawToResetContextL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0004"));
	TestSetDrawMode();	
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0005"));
	TestSetDrawModeInvalidParametersL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0006"));
	TestMoveBy();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0007"));
	TestInternalDrawingPositionDrawRect();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0008"));
	TestDrawShapesIrregularPenSizeL(TSize(2,5));	
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0008"));
	TestDrawShapesIrregularPenSizeL(TSize(6,3));
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0009"));
	TestDrawShapesChangingOriginL();
	RecordTestResultL();
	if (iUseDirectGdi)
		{
		// test for reference counting of targets, only has meaning on DirectGdi
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-GENERALDRAWING-0003"));
		DrawToClosedActiveTargetL();
		RecordTestResultL();
		}
	}
