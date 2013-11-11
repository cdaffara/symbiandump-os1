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

#include "tsimpledrawing_directgdi.h" 

#include <graphics/directgdidriver.h>

// The number of iterations to perform for each test. All but the line drawing tests
// will use this figure. Line tests use the figure below as more repetition is needed.
const TInt KIterationsToTest = 200;

// This value must not be greater than the extents of the pixmap, otherwise
// line-drawing tests will get clipped and results will be distorted.
const TInt KLineDrawingIterationsToTest = 750;

// Number of iterations for all text-rendering tests.
const TInt KTextDrawingIterationsToTest = 2000;

_LIT(KTestBitmap, "z:\\system\\data\\uibench_24bit.mbm");

// The following constants should match those in tsimpledrawing.cpp for a like-for-like 
// comparison.
//
_LIT(KTextPhrase, "abcdefABCDEF0123456789");
const TInt KTextPhraseLength = KTextPhrase().Length();

CTSimpleDrawingDirectGdi::CTSimpleDrawingDirectGdi()
	{
	SetTestStepName(KTSimpleDrawingPerfDirectGdi);
	}
	
CTSimpleDrawingDirectGdi::~CTSimpleDrawingDirectGdi()	
	{		
	}

TVerdict CTSimpleDrawingDirectGdi::doTestStepPreambleL()
	{	
	CTDirectGdiTestBase::doTestStepPreambleL();
		
	iBitmap = new(ELeave) CFbsBitmap;
	TInt ret = iBitmap->Load(KTestBitmap, 0, EFalse);
	TESTL(ret == KErrNone);
	INFO_PRINTF3(_L("Size of bitmap: %d x %d"), iBitmap->SizeInPixels().iWidth, iBitmap->SizeInPixels().iHeight);
	
	iScreenRect = TRect(1,1,GetPixmapSizeInPixels().iWidth, GetPixmapSizeInPixels().iHeight);

	return TestStepResult();
	}


/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTSimpleDrawingDirectGdi::doTestStepL()
	{
	// Perform all tests for each display mode.
	for(TInt dispModeIndex = 0; dispModeIndex < iTargetPixelFormatArray.Count(); ++dispModeIndex)
		{
		iDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iTargetPixelFormatArray[dispModeIndex]);
		if(SetTargetPixelFormatL(iDisplayMode))
			{
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0098"));
			ClearingL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0099"));
			LineDrawingL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0103"));
			VerticalLinesL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0104"));
			HorizontalLinesL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0101"));
			CopyRectL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0102"));
			DrawRectL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0100"));
			ThickLinesL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0105"));
			TextDrawingL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0171"));
			TextDrawingAntiAliasedL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0106"));
			TextDrawingWithOverheadL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0107"));
			LargeTextDrawingL();
			RecordTestResultL();
			}
		}
	
    CloseTMSGraphicsStep();
    return TestStepResult();
	}

TVerdict CTSimpleDrawingDirectGdi::doTestStepPostambleL()
	{	
	delete iBitmap;
	iBitmap = NULL;
	return CTDirectGdiTestBase::doTestStepPostambleL();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0098

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measures the performance of Clear().

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
For KIterationsToTest, clear the entire contents of the screen, and Finish().
Calculate and log the results as a pixel rate.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::ClearingL()
	{
	_LIT (KTestName, "DirectGdi Clear Screen");
	
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();

	iProfiler->InitResults ();
	for (TInt count = 0; count < KIterationsToTest; count++)
		{
		iContext->Clear (iScreenRect);
		iDGdiDriver->Finish ();
		}
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDisplayMode, KIterationsToTest, iScreenRect.Width()*iScreenRect.Height() );
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0099

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measures the performance of DrawLine().
Draws a diagonal one pixel line. With each iteration, the left edge of the line is moved down 
whilst the right edge is moved up, so that varying diagonals are tested.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
For KLineDrawingIterationsToTest, a line is drawn from from the left edge to the right edge of the 
target. With each iteration, the left edge is moved down and the right edge is moved up to cover 
varying angles of line. The lines are drawn with a pen size 1,1.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::LineDrawingL()
	{
	_LIT(KTestName, "DirectGdi Line Drawing");
	
	DoLineDrawingL(KTestName(), TSize(1,1));
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0100

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measures the performance of DrawLine().
Draws a diagonal thick line. With each iteration, the left edge is moved down and the right edge is
moved up so that varying angles are tested.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
For KLineDrawingIterationsToTest, a line is drawn from from the left edge to the right edge of the 
target. With each iteration, the left edge is moved down and the right edge is moved up to cover 
varying angles of line. The lines are drawn with a pen size 10,10.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::ThickLinesL()
	{
	_LIT(KTestName, "DirectGdi Draw Thick Lines");
	
	DoLineDrawingL(KTestName(), TSize(10,10));

	// Reset the pensize.
	iContext->SetPenSize(TSize(1, 1));	
	}

/**
Helper method. Used by diagonal line drawing tests.
@param aName The test name.
@param aPenSize The pen size.
*/
void CTSimpleDrawingDirectGdi::DoLineDrawingL(TPtrC aName, const TSize& aPenSize)
	{
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();
	
	TRect rect(iScreenRect);
	iContext->SetPenSize(aPenSize);	
	iProfiler->InitResults();
	for (TInt count = KLineDrawingIterationsToTest; count > 0 ; --count)
		{
		iContext->DrawLine(rect.iTl, rect.iBr);
		++rect.iTl.iY;
		--rect.iBr.iY;
		}		
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	WriteTargetOutput(aName);
	
	iProfiler->ResultsAnalysisPixelRate(aName, 0, 0, iDisplayMode, KLineDrawingIterationsToTest, rect.Width()*aPenSize.iHeight);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0101

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of CopyRect(), by copying a rectangle from one part of the screen to another.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
For KIterationsToTest, the entire screen contents to a location 2 pixels away in X/Y.
Call Finish() to force the drawing to complete, before logging the time.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::CopyRectL()
	{	
	_LIT(KTestName, "DirectGdi CopyRect");
	
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();
		
	iProfiler->InitResults();
	for (TInt count = 0; count < KIterationsToTest; count++)
		{	
		iContext->CopyRect(TPoint(2,2), iScreenRect);
		}												
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	WriteTargetOutput(KTestName());
	
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDisplayMode, KIterationsToTest, iScreenRect.Width()*iScreenRect.Height());		
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0102

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawRect(), with a patterned fill.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
Set bitmap as brush and draw rectangle using this brush for KIterationsToTest.
Call Finish() to force the drawing to complete, before logging the time.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::DrawRectL()
	{			
	_LIT(KTestName, "DirectGdi DrawRect");
	TEST(iBitmap->Handle() != 0);
	
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();
	
	iContext->SetBrushPattern(*iBitmap);
	iContext->SetBrushStyle(DirectGdi::EPatternedBrush);	
	iProfiler->InitResults();
	for (TInt count = 0; count < KIterationsToTest; count++)
		{
			iContext->DrawRect(iScreenRect);
		}		
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	WriteTargetOutput(KTestName());
	
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDisplayMode, KIterationsToTest, iScreenRect.Width() * iScreenRect.Height());
	iContext->SetBrushStyle(DirectGdi::ENullBrush);			
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0103

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measures the performance of DrawLine(), drawing vertical lines.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
For KLineDrawingIterationsToTest, draw a vertical line from the far left of the target to the right.
Call Finish() to force the drawing to complete, before logging the time.
The pen size is 1,1.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::VerticalLinesL()
	{
	_LIT(KTestName, "DirectGdi Vertical Lines");
	
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();
	
	iContext->SetPenSize(TSize(1, 1));	
	iProfiler->InitResults();
	for (TInt count = 0; count < KLineDrawingIterationsToTest; ++count)
		{
		iContext->DrawLine(TPoint(iScreenRect.iTl.iX+count, iScreenRect.iTl.iY), TPoint(iScreenRect.iTl.iX+count, iScreenRect.iBr.iY));
		}		
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	WriteTargetOutput(KTestName());
	
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDisplayMode, KLineDrawingIterationsToTest, iScreenRect.Height());
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0104

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measures the performance of DrawLine(), drawing horizontal lines.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
For KLineDrawingIterationsToTest, draw a horizontal line from the top of the target to the bottom.
Call Finish() to force the drawing to complete, before logging the time. The pen size is 1,1.

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::HorizontalLinesL()
	{
	_LIT(KTestName, "DirectGdi Horizontal Lines");	
	
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();
	
	iContext->SetPenSize(TSize(1, 1));
	iProfiler->InitResults();
	for (TInt count = 0; count < KLineDrawingIterationsToTest; ++count)
		{
		iContext->DrawLine(TPoint(iScreenRect.iTl.iX, iScreenRect.iTl.iY+count), TPoint(iScreenRect.iBr.iX, iScreenRect.iTl.iY+count));
		}		
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	WriteTargetOutput(KTestName());
	
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDisplayMode, KLineDrawingIterationsToTest, iScreenRect.Width());	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0105

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawText() when rendering monochrome text.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
Set the font.
Call DrawText() once to force the glyphs into the cache.
For KTextDrawingIterationsToTest, draw the same string of text.
Call Finish() to force the drawing to complete, before logging the time. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::TextDrawingL()
	{
	_LIT(KTestName, "DirectGdi Text Drawing");
	// Erase the target
	iContext->Clear();
	iContext->SetFont(iFont);
	iDGdiDriver->Finish();
	
	// Draw the text once so that we aren't timing the time to load it into the cache.
	iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
		
	iProfiler->InitResults();
	for(TInt count = KTextDrawingIterationsToTest; count > 0; --count)
		{
		iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
		}
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDisplayMode, KTextDrawingIterationsToTest, KTextPhraseLength);
	
	WriteTargetOutput(KTestName());
	iContext->ResetFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0171

@SYMDEF 135375

@SYMTestCaseDesc
Measure the performance of DrawText() when rendering anti-aliased text.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
Set the font.
Call DrawText() once to force the glyphs into the cache.
For KTextDrawingIterationsToTest, draw the same string of text.
Call Finish() to force the drawing to complete, before logging the time. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::TextDrawingAntiAliasedL()
	{
	_LIT(KTestName, "DirectGdi Text Drawing Antialiased");
	// Erase the target
	iContext->Clear();
	CFont* font = NULL;
	TFontSpec fs(KFontTypeface, KFontSize);	
	fs.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	TESTL(iTs->GetNearestFontToDesignHeightInPixels(font, fs) == KErrNone);
	iContext->SetFont(font);
	iDGdiDriver->Finish();
	
	// Draw the text once so that we aren't timing the time to load it into the cache.
	iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
	
	iProfiler->InitResults();
	for(TInt count = KTextDrawingIterationsToTest; count > 0; --count)
		{
		iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
		}
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();

	TEST(iDGdiDriver->GetError() == KErrNone);
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDisplayMode, KTextDrawingIterationsToTest, KTextPhrase().Length());
	
	WriteTargetOutput(KTestName());
	iContext->ResetFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0106

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawText(), with other overhead.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
Set the font.
Call DrawText() once to force the glyphs into the cache.
For KTextDrawingIterationsToTest, calculate the text width in pixels, the ascent in pixels, and finally draw the text.
Call Finish() to force the drawing to complete, before logging the time. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::TextDrawingWithOverheadL()
	{
	_LIT(KTestName, "DirectGdi Text Drawing With Overhead");
	// Erase the target
	iContext->Clear();
	iContext->SetFont(iFont);
	iDGdiDriver->Finish();
	
	// Draw the text once so that we aren't timing the time to load it into the cache.
	iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
	
	iProfiler->InitResults();
	for(TInt count = KTextDrawingIterationsToTest; count > 0; --count)
		{
		(void)iFont->TextWidthInPixels(KTextPhrase); //do nothing with this, just need the overhead
		(void)iFont->AscentInPixels(); //more typical overhead in a drawtext operation
		iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
		}
	iDGdiDriver->Finish();
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDisplayMode, KTextDrawingIterationsToTest, KTextPhraseLength);
	
	WriteTargetOutput(KTestName());
	iContext->ResetFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0107

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawText() when using a large font.

@SYMTestActions
Clear the target, call Finish() so that when the timer begins all outstanding drawing has finished.
Set the large font.
For KTextDrawingIterationsToTest, draw the same string of text.
Call Finish() to force the drawing to complete, before logging the time. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate.
*/
void CTSimpleDrawingDirectGdi::LargeTextDrawingL()
	{
	_LIT(KTestName, "DirectGdi Large Text Drawing");
	// Erase the target
	iContext->Clear();
	iDGdiDriver->Finish();
	iContext->SetFont(iLargeFont);	
	// Draw the text once so that we aren't timing the time to load it into the cache.
	iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
	
	iProfiler->InitResults();
	for(TInt count=0;count<KTextDrawingIterationsToTest;count++)
		{
		iContext->DrawText(KTextPhrase,NULL,TPoint(10,100));
		}
	iDGdiDriver->Finish();	
	iProfiler->MarkResultSetL();
	TEST(iDGdiDriver->GetError() == KErrNone);
	
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDisplayMode, KTextDrawingIterationsToTest, KTextPhraseLength);
	WriteTargetOutput(KTestName());
	iContext->ResetFont();	
	}
