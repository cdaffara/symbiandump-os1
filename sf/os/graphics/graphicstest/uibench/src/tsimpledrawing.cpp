// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsimpledrawing.h"

const TInt KIterationsToTest = 100;

const TInt KTextDrawingIterationsToTest = 2000;

_LIT(KTestBitmap, "z:\\system\\data\\uibench_24bit.mbm");
_LIT(KTextPhrase, "abcdefABCDEF0123456789");
const TInt KTextPhraseLength = KTextPhrase().Length();
_LIT(KFontTypeface, "DejaVu Sans Condensed");
_LIT(KLongTextPhrase, "The quick brown fox jumps over the lazy dog.");
const TInt KFontSize = 14;

const TInt KTopLeftX = 1;
const TInt KTopLeftY = 1;
const TInt KBottomRightX = 639;
const TInt KBottomRightY = 239;
const TInt KWidth = KBottomRightX - KTopLeftX;

CTSimpleDrawing::CTSimpleDrawing()
	{
	SetTestStepName(KTSimpleDrawingPerfName);
	}
	
CTSimpleDrawing::~CTSimpleDrawing()	
	{	
	delete iBitmap;
	delete iBidiText;
	iTs->ReleaseFont(iFont);
	iTs->ReleaseFont(iLargeFont);
	delete iTs;
	}

TVerdict CTSimpleDrawing::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	iTs = CFbsTypefaceStore::NewL(NULL);
	
	TInt fontError = iTs->GetNearestFontToDesignHeightInPixels(iFont, TFontSpec(KFontTypeface, KFontSize));
	TInt largeFontError = iTs->GetNearestFontToDesignHeightInPixels(iLargeFont,TFontSpec(KFontTypeface, 40));	
	iBitmap=new(ELeave) CFbsBitmap;

	TInt ret = iBitmap->Load(KTestBitmap, EMbmTbmpTbmp, EFalse);
	User::LeaveIfError(ret);
	
	iScreenRect.SetRect(KTopLeftX,KTopLeftY,KBottomRightX,KBottomRightY);
	iBidiText = TBidiText::NewL(KTextPhrase, 1);
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTSimpleDrawing::doTestStepL()
	{
	for (TInt dispModeIndex = KNumValidDisplayModes-1; dispModeIndex >= 0; --dispModeIndex)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);
		iDevice = &iScreenDevice->BitmapDevice();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0027"));
		ClearingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0040"));
		AndClearingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0030"));
		LineDrawingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0043"));
		VerticalLinesL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0094"));
		HorizontalLinesL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0034"));
		TextDrawingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0169"));
		TextDrawingAntiAliasedL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0170"));
		MeasureTextL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0033"));
		TextDrawingWithOverheadL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0035"));
		LargeTextDrawingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0036"));
		BidiTextDrawingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0037"));
		LargeBidiTextDrawingL(); 
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0032"));
		CopyRectL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0041"));
		AndCopyRectL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0042"));
		MaskBlittingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0028"));
		ShadowingL();
		RecordTestResultL();
		DrawRectL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0031"));
		ThickLinesL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0029"));
		ColorMappingL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0044"));
		BltBmpL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0045"));
		BltGcL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0038"));
		BitmapDrawingL();
		RecordTestResultL();
		}	
	return TestStepResult();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0027

@SYMTestCaseDesc
Clear screen

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::ClearingL()
	{
	_LIT(KTestName, "Clear Screen");
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{
		iGc->Clear(iScreenRect);
		}
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.Size().iWidth * iScreenRect.Size().iHeight);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0028

@SYMTestCaseDesc
Set shadow area

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::ShadowingL()
	{
	_LIT(KTestName, "Shadowing");
	iProfiler->InitResults();
	TRegionFix<1> area(iScreenRect);
	TInt reps=KIterationsToTest;
	for(TInt count=0;count<reps;count++)
		{
		iGc->ShadowArea(&area);
		}	
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.Size().iWidth * iScreenRect.Size().iHeight);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0029

@SYMTestCaseDesc
Maps pixels in the specified rectangle

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::ColorMappingL()
	{
	_LIT(KTestName, "Colour Mapping");
	iProfiler->InitResults();
	TRgb colortable[4];
	colortable[0]=TRgb::Gray2(0);
	colortable[1]=TRgb::Gray2(1);
	colortable[2]=TRgb::Gray2(1);
	colortable[3]=TRgb::Gray2(0);
	TInt reps=KIterationsToTest;
	for(TInt count=0;count<reps;count++)
		{
		iGc->MapColors(iScreenRect,colortable);
		}		
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.Size().iWidth * iScreenRect.Size().iHeight);
	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0030

@SYMTestCaseDesc
Draws a diagonal one pixel line

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::LineDrawingL()
	{
	_LIT(KTestName, "Line Drawing");
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{
		iGc->DrawLine(iScreenRect.iTl,iScreenRect.iBr);
		}
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	TReal pixels;
	Math::Sqrt(pixels, (iScreenRect.iBr.iX-iScreenRect.iTl.iX)*(iScreenRect.iBr.iX-iScreenRect.iTl.iX)+(iScreenRect.iBr.iY-iScreenRect.iTl.iY)*(iScreenRect.iBr.iY-iScreenRect.iTl.iY));
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, static_cast<TInt>(pixels));
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0031

@SYMTestCaseDesc
Draws a diagonal thick line

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::ThickLinesL()
	{
	_LIT(KTestName, "Draw Thick Lines");
	iGc->SetPenSize(TSize(10,10));
	TInt reps=KIterationsToTest;
	iProfiler->InitResults();
	for(TInt count=0;count<reps;count++)
		{
		iGc->DrawLine(iScreenRect.iTl,iScreenRect.iBr);
		}
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	TReal pixels;
	Math::Sqrt(pixels, (iScreenRect.iBr.iX-iScreenRect.iTl.iX)*(iScreenRect.iBr.iX-iScreenRect.iTl.iX)+(iScreenRect.iBr.iY-iScreenRect.iTl.iY)*(iScreenRect.iBr.iY-iScreenRect.iTl.iY));
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, 10*static_cast<TInt>(pixels));
	iGc->SetPenSize(TSize(1,1));
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0032

@SYMTestCaseDesc
Copy a rectangle from one part of the screen to another

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::CopyRectL()
	{
	_LIT(KTestName, "CopyRect");		
	iProfiler->InitResults();
	TInt reps=KIterationsToTest;
	for(TInt count=0;count<reps;count++)
		{
		iGc->CopyRect(TPoint(2,2),iScreenRect);
		}		
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.Size().iWidth * iScreenRect.Size().iHeight);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0034

@SYMTestCaseDesc
Draw text.

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::TextDrawingL()
	{
	_LIT(KTestName, "Text Drawing");	
	iGc->UseFont(iFont);
	// Call once outside of timing loop to stop glyph-caching from affecting measurements.
	iGc->DrawText(KTextPhrase,TPoint(10,100));
	
	iProfiler->InitResults();
	for(TInt count=0;count<KTextDrawingIterationsToTest;count++)
		{
		iGc->DrawText(KTextPhrase,TPoint(10,100));		
		}
	iScreenDevice->Update();	
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, KTextPhraseLength);
	iGc->DiscardFont();	
	}

/**
@SYMTestCaseID 
GRAPHICS-UI-BENCH-0169

@SYMTestCaseDesc
Draw anti-aliased text.

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::TextDrawingAntiAliasedL()
	{
	_LIT(KTestName, "Anti Aliased Text Drawing");		
	CFont* font = NULL;
	TFontSpec fs(KFontTypeface, KFontSize);
	fs.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	TESTL(iTs->GetNearestFontToDesignHeightInPixels(font, fs) == KErrNone);
	iGc->UseFont(font);
	
	// Call once outside of timing loop to stop addition of glyphs to the glyph-cache
	// from affecting measurements.	
	TPoint pos(10,100);
	iGc->DrawText(KTextPhrase, pos);
	
	iProfiler->InitResults();
	for(TInt count=KTextDrawingIterationsToTest; count>=0; count--)
		{
		iGc->DrawText(KTextPhrase, pos);		
		}
	iScreenDevice->Update();	
	
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, KTextPhrase().Length());
	
	iGc->DiscardFont();	
	iTs->ReleaseFont(font);
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0033

@SYMTestCaseDesc
Draw text, with additional overhead.

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::TextDrawingWithOverheadL()
	{
	_LIT(KTestName, "Text Drawing With Overhead");	
	iGc->UseFont(iFont);	
	iProfiler->InitResults();
	for(TInt count=0;count<KTextDrawingIterationsToTest;count++)
		{
		(void)iFont->TextWidthInPixels(KTextPhrase); //do nothing with this, just need the overhead
		(void)iFont->AscentInPixels(); //more typical overhead in a drawtext operation
		iGc->DrawText(KTextPhrase,TPoint(10,100));		
		}
	iScreenDevice->Update();	
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, KTextPhraseLength);
	iGc->DiscardFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0170

@SYMTestCaseDesc
Call CFont::MeasureText() a number of times.

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::MeasureTextL()
	{	
	_LIT(KTestName, "Measure Text");			
	iGc->UseFont(iFont);
	
	// set up the input params for MeasureText()
	TInt textLength = KLongTextPhrase().Length();
	CFont::TMeasureTextInput measureTextInput;
	measureTextInput.iCharJustNum = 0;
	measureTextInput.iCharJustExcess = 0;
	measureTextInput.iWordJustNum = 0;
	measureTextInput.iWordJustExcess = 0;
	measureTextInput.iFlags |= CFont::TMeasureTextInput::EFVisualOrder; // left-to-right	
	measureTextInput.iStartInputChar = 0;
	measureTextInput.iEndInputChar = textLength - 1;	
	CFont::TMeasureTextOutput measureTextOutput;
	
	// Call once outside of timing loop to stop addition of glyphs to the glyph-cache
	// from affecting measurements.
	iFont->MeasureText(KLongTextPhrase, &measureTextInput, &measureTextOutput);
	
	iProfiler->InitResults();
	for(TInt count=KTextDrawingIterationsToTest; count>=0; count--)
		{
		iFont->MeasureText(KLongTextPhrase, &measureTextInput, &measureTextOutput);		
		}
	iScreenDevice->Update();
	
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, textLength);
	
	iGc->DiscardFont();		
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0035

@SYMTestCaseDesc
Draw large text

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::LargeTextDrawingL()
	{
	_LIT(KTestName, "Large Text Drawing");	
	iGc->UseFont(iLargeFont);
	// Call once outside of timing loop to stop glyph-caching from affecting measurements.   
	iGc->DrawText(KTextPhrase,TPoint(10,100));
	
	iProfiler->InitResults();
	for(TInt count=0;count<KTextDrawingIterationsToTest;count++)
		{
		(void)iLargeFont->TextWidthInPixels(KTextPhrase); //do nothing with this, just need the overhead
		(void)iLargeFont->AscentInPixels(); //more typical overhead in a drawtext operation
		iGc->DrawText(KTextPhrase,TPoint(10,100));
		}
	iScreenDevice->Update();	
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, KTextPhraseLength);
	iGc->DiscardFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0036

@SYMTestCaseDesc
Bidi draw text

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::BidiTextDrawingL()
	{
	_LIT(KTestName, "Bidi Text Drawing");	
	iGc->UseFont(iFont);			
	iProfiler->InitResults();
	for(TInt count=0;count<KTextDrawingIterationsToTest;count++)
		{
		iBidiText->WrapText(KWidth, *iFont, 0);
		iBidiText->DrawText(*iGc,TPoint(10,100));
		}
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, KTextPhraseLength);
	iGc->DiscardFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0037

@SYMTestCaseDesc
Bidi large draw text

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::LargeBidiTextDrawingL()
	{
	_LIT(KTestName, "Large Bidi Text Drawing");	
	iGc->UseFont(iLargeFont);	
	iProfiler->InitResults();
	for(TInt count=0;count<KTextDrawingIterationsToTest;count++)
		{
		iBidiText->WrapText(KWidth, *iFont, 0);
		iBidiText->DrawText(*iGc,TPoint(10,100));
		}
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisCharacterRate(KTestName, 0, 0, iDevice->DisplayMode(), KTextDrawingIterationsToTest, KTextPhraseLength);
	iGc->DiscardFont();	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0038

@SYMTestCaseDesc
Drawbitmap

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::BitmapDrawingL()
	{
	_LIT(KTestName, "Bitmap Drawing");
	TEST(iBitmap->Handle() != 0);		

	TInt reps=KIterationsToTest;
	TRect bmprect(iBitmap->SizeInPixels());			
	iProfiler->InitResults();
	for(TInt count=0;count<reps;count++)
		{		
		iGc->DrawBitmap(iScreenRect,iBitmap,bmprect);
		}
	iScreenDevice->Update();		
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, bmprect.Size().iWidth * bmprect.Size().iHeight);	
	}


/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0039

@SYMTestCaseDesc
Paint

@SYMTestActions
Set bitmap as brush and draw rectangle using this brush

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::DrawRectL()
	{
	_LIT(KTestName, "DrawRect");
	TEST(iBitmap->Handle() != 0);

	iGc->UseBrushPattern(iBitmap);
	iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);	
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{
		iGc->DrawRect(iScreenRect);
		}		
	iScreenDevice->Update();		
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.Size().iWidth * iScreenRect.Size().iHeight);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0040

@SYMTestCaseDesc
Clear screen with drawmode AND

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::AndClearingL()
	{
	_LIT(KTestName, "And Clearing");
	iGc->SetDrawMode(CGraphicsContext::EDrawModeAND);			
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{
		iGc->Clear(iScreenRect);
		}		
	iScreenDevice->Update();	
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.Size().iWidth * iScreenRect.Size().iHeight);
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0041

@SYMTestCaseDesc
CopyRect with drawmode AND

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::AndCopyRectL()
	{
	_LIT(KTestName, "And CopyRect");
	iGc->SetDrawMode(CGraphicsContext::EDrawModeAND);
	TRect r(iScreenRect);
	r.Grow(1,1);	
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{
		iGc->CopyRect(TPoint(32,32),r);
		}		
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, r.Size().iWidth * r.Size().iHeight);
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0042

@SYMTestCaseDesc
BitbltMasked

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::MaskBlittingL()
	{
	_LIT(KTestName, "Mask Blitting");
	TEST(iBitmap->Handle() != 0);	

	TRect bmprect(iBitmap->SizeInPixels());	
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{
		iGc->BitBltMasked(TPoint(0,0),iBitmap,bmprect,iBitmap,ETrue);
		}
	iScreenDevice->Update();		
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, bmprect.Size().iWidth * bmprect.Size().iHeight);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0043

@SYMTestCaseDesc
Draw vertical line

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::VerticalLinesL()
	{
	_LIT(KTestName, "Vertical Lines");	
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{		
		iGc->DrawLine(TPoint(iScreenRect.iTl.iX+count, iScreenRect.iTl.iY),TPoint(iScreenRect.iTl.iX+count,iScreenRect.iBr.iY));	
		}	
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.iBr.iY-iScreenRect.iTl.iY);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0044

@SYMTestCaseDesc
BltBlt

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::BltBmpL()
	{
	_LIT(KTestName, "Bitblt");
	TEST(iBitmap->Handle() != 0);	

	TInt reps=KIterationsToTest;
	TPoint zero(0,0);	
	iProfiler->InitResults();
	for(TInt count=0;count<reps;count++)
		{
		iGc->BitBlt(zero,iBitmap);
		}		
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, iBitmap->DisplayMode(), iDevice->DisplayMode(), KIterationsToTest, iBitmap->SizeInPixels().iWidth * iBitmap->SizeInPixels().iHeight);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0045

@SYMTestCaseDesc
Bitblt from another GC

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::BltGcL()
	{
	_LIT(KTestName, "Blt Gc");
	TEST(iBitmap->Handle() != 0);

	TInt reps=KIterationsToTest;
	TPoint zero(0,0);
	CFbsBitmapDevice* device=NULL;
	TRAP_IGNORE(device=CFbsBitmapDevice::NewL(iBitmap));
	CFbsBitGc* gc=NULL;
	device->CreateContext(gc);	
	iProfiler->InitResults();
	for(TInt count=0;count<reps;count++)
		{
		iGc->BitBlt(zero,*gc);
		}		
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, iBitmap->DisplayMode(), iDevice->DisplayMode(), KIterationsToTest, iBitmap->SizeInPixels().iWidth * iBitmap->SizeInPixels().iHeight);
	delete gc;
	delete device;	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0094

@SYMTestCaseDesc
Draw horizontal line

@SYMTestActions
Compare the results over time

@SYMTestExpectedResults
Measure average time to complete one cycle
*/
void CTSimpleDrawing::HorizontalLinesL()
	{
	_LIT(KTestName, "Horizontal Lines");	
	iProfiler->InitResults();
	for(TInt count=0;count<KIterationsToTest;count++)
		{		
		iGc->DrawLine(TPoint(iScreenRect.iTl.iX, iScreenRect.iTl.iY+count), TPoint(iScreenRect.iBr.iX, iScreenRect.iTl.iY+count));
		}	
	iScreenDevice->Update();
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(KTestName, 0, 0, iDevice->DisplayMode(), KIterationsToTest, iScreenRect.iBr.iX-iScreenRect.iTl.iX);	
	}
