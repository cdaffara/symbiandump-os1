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

#include "tclear.h"

#include <e32math.h>

CTClear::CTClear()
	{
	SetTestStepName(KTDirectGdiClearStep);
	}

CTClear::~CTClear()
	{
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-CLEAR-0001

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
	Test that Clear clears the whole target successfully

@SYMTestPriority  
	Critical

@SYMTestStatus
	Implemented

@SYMTestActions
	Set the brush colour to black and pen colour to green.
	Draw a rectangle in the centre of the target.
	Set the brush colour to red (and therefore clear colour to red).
	Clear the screen.

@SYMTestExpectedResults
	The target should be filled with red.
*/ 
void CTClear::TestClear()
	{
	_LIT(KTestName, "Clear-EntireTarget"); 		
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	iGc->SetPenSize(TSize(3,3));
	iGc->SetBrushColor(TRgb(0,0,0));
	iGc->SetPenColor(TRgb(0,255,0));
	iGc->DrawRect(TRect(70,70,140,140));
	iGc->SetBrushColor(TRgb(255,0,0));
	iGc->Clear();
	
	iGc->SetOrigin(TPoint(0, 1));
	iGc->Clear();
	
	iGc->SetOrigin(TPoint(1, 1));
	iGc->Clear();
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	TESTNOERROR(iGc->GetError());
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-CLEAR-0002

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
	Test that Clear clears a specified rectangle successfully.

@SYMTestPriority  
	Critical

@SYMTestStatus
	Implemented

@SYMTestActions
	Sets brush to red, and clears a rectangle at the top left of the target.
	Sets brush to green, and clears a thin rectangle across the entire target, using negative and large rectangle parameters.
	Sets brush to black, calls Clear using an empty rect.
	Calls Clear using a TRect with all corners at the same point.
	Calls Clear using a TRect with no width.
	Calls Clear using a TRect with no height.
	Calls Clear using a TRect with the bottom-right point defined first.

@SYMTestExpectedResults
	The first rectangle to be drawn correctly to the target for the regular clear.
	A green rectangle across the target when testing large/negative rectangle values.
	All other rectangle clears should clear nothing, no error should be set.
*/ 
void CTClear::TestClearRect()
	{
	_LIT(KTestName, "Clear-Rect"); 		
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();	
	
	// Regular rectangle clear.
	iGc->SetBrushColor(KRgbRed);
	iGc->Clear(TRect(20,20, 130,130));

	// Test negative points and large points.	
	iGc->SetBrushColor(KRgbGreen);
	iGc->Clear(TRect(-10,60, 1000,90));
	TESTNOERROR(iGc->GetError());
	
	// Test clearing an empty rectangle
	iGc->SetBrushColor(KRgbBlack);
	iGc->Clear(TRect(0,0,0,0));
	TEST(!iUseDirectGdi || iGc->GetError() == KErrNone);
	
	// Test clearing a rectangle with all corners matching.
	iGc->Clear(TRect(1,1,1,1));
	TEST(!iUseDirectGdi || iGc->GetError() == KErrNone);
	
	// Test clearing a horizontal line.
	iGc->Clear(TRect(0,70,150,70));
	TEST(!iUseDirectGdi || iGc->GetError() == KErrNone);
	
	// Test clearing a vertical line.
	iGc->Clear(TRect(70,0,70,150));	
	TEST(!iUseDirectGdi || iGc->GetError() == KErrNone);
	
	// Test TRect defined bottom-right first.
	iGc->SetBrushColor(KRgbCyan);
	iGc->Clear(TRect(50,50,20,20));
	TEST(!iUseDirectGdi || iGc->GetError() == KErrNone);
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-CLEAR-0003

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
	Test that when clipping is enabled, Clear clears the defined clipping regions only

@SYMTestPriority  
	Critical

@SYMTestStatus
	Implemented

@SYMTestActions
	Set brush to blue, and clear target.
	Set the brush to orange, and set a clipping region.
	Call Clear to clear the whole target.

@SYMTestExpectedResults
	The target should be orange, with a large 'I' shape in the centre in blue.
 */ 
void CTClear::TestClearWithClipping()
	{
	_LIT(KTestName, "Clear-Clipping"); 	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	// Clear screen in blue
	iGc->SetBrushColor(TRgb(0,0,255));
	iGc->Clear();
	
	// Define a clipping region.
	RRegion clippingRegion(3);
	clippingRegion.AddRect(TRect(10,10, 140, 30));
	clippingRegion.AddRect(TRect(60, 10, 90, 140));
	clippingRegion.AddRect(TRect(10,120, 140, 140));
	
	// Clear screen in orange, with clipping region set.
	iGc->SetBrushColor(TRgb(255,128,0));
	iGc->SetClippingRegion(clippingRegion);
	iGc->Clear();
	iGc->ResetClippingRegion();
	clippingRegion.Close();
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	TESTNOERROR(iGc->GetError());
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-CLEAR-0004

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
	Test the effects of the different drawmodes on Clear.

@SYMTestPriority  
	Critical

@SYMTestStatus
	Implemented

@SYMTestActions
	Clear the entire target in red.
	Set drawmode to EDrawModePEN and clear a rectangular area of the screen in white, with opacity 0.5
	Set drawmode to EDrawModeWriteAlpha and clear a different rectangular area of the screen in white.

@SYMTestExpectedResults
	A pink rectangle on the left (pink because it is white blended with the red underneath) 
	and, if the display-mode has an alpha channel there will be a grey rectangle of equal size to the 
	right (as it is not blending but using write-alpha), or if the display-mode does not have alpha it will
	appear as white. with a darker red surrounding the rectangles. The rectangles are surrounded by a 
	red border.
*/ 
void CTClear::TestClearDrawModes()
	{
	_LIT(KTestName, "Clear-DrawModes"); 		
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	// Clear screen in red
	iGc->SetBrushColor(TRgb(255,0,0));
	iGc->Clear();
	// Left rectangle in white, using alpha-blend 50% opacity. 
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);
	iGc->SetBrushColor(TRgb(255,255,255,128));
	iGc->Clear(TRect(10,10,80,140));
	// Right rectangle in white, using write-alpha 50% opacity. 
	iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
	iGc->SetBrushColor(TRgb(255,255,255,128));
	iGc->Clear(TRect(80,10,140,140));
	TESTNOERROR(iGc->GetError());
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}


/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTClear::doTestStepPreambleL()
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
TVerdict CTClear::doTestStepL()
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
void CTClear::RunTestsL()
	{	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLEAR-0001"));
	TestClear();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLEAR-0002"));
	TestClearRect();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLEAR-0003"));
	TestClearWithClipping();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CLEAR-0004"));
	TestClearDrawModes();
	RecordTestResultL();
	}
