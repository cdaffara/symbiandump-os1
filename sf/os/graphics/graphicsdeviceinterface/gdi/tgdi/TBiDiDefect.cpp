// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <biditext.h>
#include <bidi.h>
#include <bidivisual.h>
#include "TGraphicsContext.h"
#include "TBiDiDefect.h"

class CTBiDiDefect : public CTGraphicsBase
	{

public:
	CTBiDiDefect(CTestStep* aStep);
	~CTBiDiDefect();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	void CreateTestGraphicsContextL();

	void INC016328L(const TDesC& aTestText);
	void INC016665L();
	void INC017825L();
	void INC017974L();
	void INC017991L();
	void DEF021227();
	void INC023337();
	void INC023917();
	void DEF037928();
	void DEF021347L();
	void INC037549L();
	void INC042422L();
	void DEF059214L();

	void TestDEF021347L(TUint16 aControlCharacter);
	void TestDEF043720L();
	
	void TestPDEF117110L();

private:
	TBidirectionalState::TRunInfo* iRunInfoArray;
	CTestFont* iTestFont;
	TBidiText* iBidiText;
	CTestGraphicsDevice* iTestGraphicsDevice;
	CTestGraphicsContext* iTestGraphicsContext;
	};


CTBiDiDefect::CTBiDiDefect(CTestStep* aStep) :
	CTGraphicsBase(aStep),
	iRunInfoArray(NULL),
	iTestFont(NULL),
	iBidiText(NULL),
	iTestGraphicsDevice(NULL),
	iTestGraphicsContext(NULL)
	{
	}

CTBiDiDefect::~CTBiDiDefect()
	{
	delete iTestGraphicsContext;
	delete iTestGraphicsDevice;
	delete iBidiText;
	delete iTestFont;
	delete [] iRunInfoArray;
	}

void CTBiDiDefect::RunTestCaseL(TInt aCurTestCase)
	{
    ((CTBiDiDefectStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		{
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0001
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0001"));
		TBuf<32> testText(_L(".Test"));
		INFO_PRINTF1(_L("INC016328"));
		INC016328L(testText);
		}
		break;
	case 2:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0002
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0002"));
		INFO_PRINTF1(_L("INC016665"));
		INC016665L();
		break;
	case 3:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0003
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0003"));
		INFO_PRINTF1(_L("INC017825"));
		INC017825L();
		break;
	case 4:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0004
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0004"));
		INFO_PRINTF1(_L("INC017974"));
		INC017974L();
		break;
	case 5:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0005
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0005"));
		INFO_PRINTF1(_L("INC017991"));
		INC017991L();
		break;
	case 6:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0006
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0006"));
		INFO_PRINTF1(_L("DEF021227"));
		DEF021227();
		break;
	case 7:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0007
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0007"));
		INFO_PRINTF1(_L("DEF021347"));
		DEF021347L();
		break;
	case 8:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0008
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0008"));
		INFO_PRINTF1(_L("INC023337"));
		INC023337();
		break;
	case 9:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0009
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0009"));
		INFO_PRINTF1(_L("INC023917"));
		INC023917();
		break;
	case 10:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0010
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0010"));
		INFO_PRINTF1(_L("DEF037928"));
		DEF037928();
		break;
	case 11:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0011
*/
	//INC037549 TBidiText crashes if  <CR><LF> appear at the end of the text
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0011"));
		INFO_PRINTF1(_L("INC037549L"));
		INC037549L();
		break;
	case 12:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0012
*/
	//INC042422 when calculated length in BidiCompact.cpp TRunInfoCompact::Reorder < 0
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0012"));
		INFO_PRINTF1(_L("INC042422L"));
		INC042422L();
		break;
	case 13:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0013
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0013"));
		INFO_PRINTF1(_L("DEF043720"));
		TestDEF043720L();
		break;
	case 14:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0014
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0014"));
		INFO_PRINTF1(_L("DEF059214: Erroneous caret cursor movements"));
		DEF059214L();
		break;
	case 15:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-BiDiDefect-0015
*/
		((CTBiDiDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-BiDiDefect-0015"));
		INFO_PRINTF1(_L("PDEF117110"));
		TestPDEF117110L();
		break;
	case 16:
		((CTBiDiDefectStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTBiDiDefectStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
        ((CTBiDiDefectStep*)iStep)->RecordTestResultL();
	}

void CTBiDiDefect::CreateTestGraphicsContextL()
	{
	delete iTestGraphicsContext;
	iTestGraphicsContext = NULL;
	delete iTestGraphicsDevice;
	iTestGraphicsDevice = NULL;
	
	TSize windowSize(400, 400);
	iTestGraphicsDevice = CTestGraphicsDevice::NewL(windowSize);
	CGraphicsContext* tContext = NULL;	
	TEST(iTestGraphicsDevice->CreateContext(tContext) == KErrNone);
	iTestGraphicsContext = static_cast<CTestGraphicsContext*>(tContext);
	}

//TBidiText crashes if the first character of the text is '.'.
void CTBiDiDefect::INC016328L(const TDesC& aTestText)
	{
	TInt arraySize = TBidirectionalState::GenerateBdRunArray(aTestText.Ptr(), aTestText.Length(), 0, 0);
	iRunInfoArray = new (ELeave) TBidirectionalState::TRunInfo[arraySize];
	TBidirectionalState::GenerateBdRunArray(aTestText.Ptr(), aTestText.Length(), iRunInfoArray, arraySize);
	TBidirectionalState state;
	state.ReorderLine(iRunInfoArray, arraySize, ETrue, ETrue, EFalse, TChar::EOtherNeutral, TChar::EOtherNeutral);
	TEST(iRunInfoArray->iLength > 0);
	}

//Truncation after space character in Bidirectional text classes.
//Actually it doesn't prove missing space character but proves 
//truncation at the middle of "text" word - yet another defect.
void CTBiDiDefect::INC016665L()
	{
	iTestFont = new (ELeave) CTestFont;
	iBidiText = TBidiText::NewL(_L("Truncated text"), 1);
	iBidiText->WrapText(110, *iTestFont, NULL);//"Truncated " and "text"
	TEST(iBidiText->DisplayText().Length() == 10);
	TEST(iBidiText->DisplayText()[9] == 0x2026);
	delete (iBidiText);

	iBidiText = TBidiText::NewL(_L("Truncated     text"), 1);
	iBidiText->WrapText(110, *iTestFont, NULL);//"Truncated " and "text"
	TEST(iBidiText->DisplayText().Length() == 10);
	TEST(iBidiText->DisplayText()[9] == 0x2026);
	}

//TBidiText crashes with length 0 descriptor.
void CTBiDiDefect::INC017825L()
	{
	delete iTestGraphicsContext;
	iTestGraphicsContext = NULL;
	CreateTestGraphicsContextL();

	delete iTestFont;
	iTestFont = NULL;
	iTestFont = new (ELeave) CTestFont;

	delete iBidiText;
	iBidiText = NULL;
	iBidiText = TBidiText::NewL(KNullDesC(), 1);
	iBidiText->WrapText(100, *iTestFont, NULL);

	iBidiText->DrawText(*iTestGraphicsContext, TPoint(0, 20), 20, CGraphicsContext::ERight);
	}

//BidiText does not show 0x062A character correctly if it is a first character.
void CTBiDiDefect::INC017974L()
	{
	_LIT(KTextWithLatinLettersFirstArabicLetter, "\x062A abcdef");
	
	delete iTestFont;
	iTestFont = NULL;
	iTestFont = new (ELeave) CTestFont;

	delete iBidiText;
	iBidiText = NULL;
	iBidiText = TBidiText::NewL(KTextWithLatinLettersFirstArabicLetter, 1);
	iBidiText->WrapText(100, *iTestFont, NULL);
	const TText* text = iBidiText->DisplayText().Ptr();
	TEST(text[7] == 0x062A);//the arabic character should be the first at the end.
	}

//TBidiText::DrawText draws out of the area if alignment is right.
void CTBiDiDefect::INC017991L()
	{
	delete iTestGraphicsContext;
	iTestGraphicsContext = NULL;
	CreateTestGraphicsContextL();

	_LIT(KTextWithLatinLetters, "abc def ghijk lmnop qrstuvw xyz 12.34.");
	const TInt KTextWidth = 200;

	delete iTestFont;
	iTestFont = NULL;
	iTestFont = new (ELeave) CTestFont;

	delete iBidiText;
	iBidiText = NULL;
	iBidiText = TBidiText::NewL(KTextWithLatinLetters, 2);
	iBidiText->WrapText(KTextWidth, *iTestFont, NULL);

	iBidiText->DrawText(*iTestGraphicsContext, TPoint(0, 20), 20, CGraphicsContext::ERight);
	const TTestGCDisplayLine& line1 = iTestGraphicsContext->DisplayLine(0);
	const TDesC& text1 = line1.iLineData;
	TInt width1 = iTestFont->TextWidthInPixels(text1);
	TEST((width1 + line1.iLinePos.iX) <= KTextWidth);
	const TTestGCDisplayLine& line2 = iTestGraphicsContext->DisplayLine(1);
	const TDesC& text2 = line2.iLineData;
	TInt width2 = iTestFont->TextWidthInPixels(text2);
	TEST((width2 + line2.iLinePos.iX) <= KTextWidth);
	}

//TBidiLogicalToVisual seems to lose characters. 
void CTBiDiDefect::DEF021227()
	{
	_LIT(KTestText, "Waitnote + D prompt ");
// 	_LIT(KTestText, "Waitnote H D prompt ");
	TBuf<100> visualText;
	visualText.Fill(0xCDCD);
	TBidirectionalState::TRunInfo runArray[6];

	TBidiLogicalToVisual bidiConverter(KTestText, EFalse, runArray,
		sizeof(runArray)/sizeof(runArray[0]));
	bidiConverter.Reorder();
	TDesC des = KTestText;
	bidiConverter.GetVisualLine(visualText, 0, des.Length() - 1, 0xFFFF);
	TInt len = visualText.Length();
	TEST((des.Length() - 1) == len);
	}

//DrawText should not draw control codes (particularly Bidirectional ones)
void CTBiDiDefect::DEF021347L()
	{
	TestDEF021347L(0x200C);
	TestDEF021347L(0x200D);
	TestDEF021347L(0x200E);
	TestDEF021347L(0x200F);
	TestDEF021347L(0x202A);
	TestDEF021347L(0x202B);
	TestDEF021347L(0x202C);
	TestDEF021347L(0x202D);
	TestDEF021347L(0x202E);
	// Tested in TGLYPHSEL now following rewrite.
	//TestDEF021347L(0xFFFF);
	}

void CTBiDiDefect::TestDEF021347L(TUint16 aControlCharacter)
	{
	const TInt KTestTextLen = 5;
	TBuf<100> testText(KTestTextLen);
	testText[0] = 'a';
	testText[1] = 'b';
	testText[2] = aControlCharacter;
	testText[3] = 'c';
	testText[4] = 'd';

	delete iTestFont;
	iTestFont = NULL;
	iTestFont = new (ELeave) CTestFont;

	CFont::TPositionParam param;
	param.iDirection = 0;
	param.iFlags = 1;
	param.iText.Set(testText);
	param.iPosInText = 0;
	param.iPen.iX = 0;
	param.iPen.iY = 0;
	param.iPosInText = 2; //testText[2] = aControlCharacter
	TBool r = iTestFont->GetCharacterPosition(param);

	TEST(r && param.iPen.iX == 0 && param.iOutputGlyphs == 0);
	}

//TBidiLogicalToVisual::GetVisualLine() panics when aEnd == 0 
void CTBiDiDefect::INC023337()
	{
	_LIT(KTestText, "");
	TBuf<100> visualText;
	visualText.Fill(0xCDCD);
	TBidirectionalState::TRunInfo runArray[6];
	TBidiLogicalToVisual bidiConverter(KTestText, EFalse, runArray, sizeof(runArray)/sizeof(runArray[0]));
	bidiConverter.Reorder();

	TDesC des = KTestText;
	const TUint KEmptyChar = TUint(0xFFFF);
	//The next statement will panics, if INC023337 is not fixed.
	bidiConverter.GetVisualLine(visualText, 0, des.Length(), KEmptyChar);
	}

//TBidiLogicalToVisual indexes over the given text.
void CTBiDiDefect::INC023917()
	{
	const TInt dest_buf_len = 10;
	TBidirectionalState::TRunInfo runInfoArray[dest_buf_len];
	const TInt src_buf_len = 4;
    TBuf<src_buf_len> logical;
    logical.Fill(0x0639, src_buf_len);

    _LIT(KArabicWord, "\x0631\x0641\x0636");
    logical = KArabicWord;

    TBidiLogicalToVisual bidiConverter(logical, runInfoArray, sizeof(runInfoArray)/sizeof(runInfoArray[0]));
    bidiConverter.Reorder();

    TBuf<dest_buf_len> visual;
    bidiConverter.GetVisualLine(visual, 0, logical.Length(), 0xFFFF);

	TEST(visual[0] == 0x0636);
	}

// DEF037928 - TBidiLogicalToVisual::GetVisualLine does not work correctly for 0 length strings
void CTBiDiDefect::DEF037928()
	{
	// Zero length string for testing
	_LIT(KTestText, "");
	TBuf<100> visualText;
	visualText.Fill(0xCDCD);
	visualText.SetMax();
	
	TBidirectionalState::TRunInfo runArray[6];
	TBidiLogicalToVisual bidiConverter(KTestText, EFalse, runArray,
		sizeof(runArray)/sizeof(runArray[0]));
	bidiConverter.Reorder();

	bidiConverter.GetVisualLine(visualText, 0, 1, 0xFFFF);
	TEST( visualText.Length() == 0);
	}
	
	
// INC037549 - TBidiText crashes if  <CR><LF> appear at the end of the text
void CTBiDiDefect::INC037549L()
	{
	// This crash occured when the final character is <cr>
	// and following memory location is <lf>
	// To test this testString has a length of 5, but sPtr has length 4
	_LIT( testString, "123\r\n" ) ;
	TBuf<5> string( testString ) ;
	TPtrC sPtr( string.Ptr(), 4 ) ;
	TBidiText* bText = TBidiText::NewL( sPtr, 1, TBidiText::ELeftToRight ) ;

	delete bText;
	}

// INC042422 - when calculated length in BidiCompact.cpp TRunInfoCompact::Reorder < 0
//
// This defect causes an extra blank line to be added when text contains \r\n
// This was caused by a miscalculation in Biditext.cpp SizeLineBreak().
// this test case is modified due to change of TBidiText::DoWrapText for fixing PDEF117110
void CTBiDiDefect::INC042422L()
	{
	_LIT( testString, "123\r\n" ) ;
	TBidiText* bText = TBidiText::NewL( testString, 1, TBidiText::ELeftToRight ) ;
	bText->WrapText(100,*iTestFont,0);
	TPtrC text = bText->DisplayText(); 

	// The defect caused text to contain an extra blank line
	// So the length was 5 ( text contained:1,2,3, 0x2028, 0x2026)
	// when the correct value is 4 (text should contain: 1,2,3, 0x2026)
	// Note: Due to fix for PDEF117109, the correct value is 3 now (text contains: 1,2,3)
	TEST(text.Length() == 3);
	TEST(text[0] == '1');
	TEST(text[1] == '2');
	TEST(text[2] == '3');

	delete bText;
	}

//DEF043720 - Assert fails in TBiDiText wrapping when more than one \n is present in the text 
void CTBiDiDefect::TestDEF043720L()
	{
	delete iTestFont;
	iTestFont = NULL;
	iTestFont = new (ELeave) CTestFont;
	
	delete iBidiText;
	iBidiText = NULL;
	iBidiText = TBidiText::NewL(_L("\n\nAB\n\n\nCD\n"),10);
	iBidiText->WrapText(10,*iTestFont,0);

	//After wrapping text should look like "\x2028\x2028A\x2028B\x2028\x2028\x2028C\x2028D"
	//where \x2028 is the line separator
	TPtrC afterwrap(iBidiText->DisplayText());
	TEST(afterwrap.Length()==11);
	TEST(afterwrap[0]==0x2028);
	TEST(afterwrap[1]==0x2028);
	TEST(afterwrap[3]==0x2028);
	TEST(afterwrap[5]==0x2028);
	TEST(afterwrap[6]==0x2028);
	TEST(afterwrap[7]==0x2028);
	TEST(afterwrap[9]==0x2028);

	INFO_PRINTF1(iBidiText->DisplayText());

	delete iBidiText;
	iBidiText = NULL;
	delete iTestFont;
	iTestFont = NULL;
	}

void CTBiDiDefect::DEF059214L()
	{
	TBidirectionalState::TRunInfo runs[4];
	TBidirectionalState bs;
	TBool ambig;

	// entirely left to right paragraph is not ambiguous
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ELeftToRight;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, EFalse, TChar::ELeftToRight,
		TChar::ELeftToRight, ambig);

	// entirely right to left paragraph is not ambiguous
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ERightToLeft;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, ETrue, TChar::ERightToLeft,
		TChar::ERightToLeft, ambig);
	TEST(!ambig);

	// right to left portion within left to right paragraph is ambiguous
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ERightToLeft;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, EFalse, TChar::ERightToLeft,
		TChar::ERightToLeft, ambig);
	TEST(ambig && ambig != 3000);
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ERightToLeft;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, EFalse, TChar::EOtherNeutral,
		TChar::ERightToLeft, ambig);
	TEST(ambig && ambig != 3000);

	// left to right portion within right to left paragraph is ambiguous
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ELeftToRight;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, ETrue, TChar::ELeftToRight,
		TChar::ELeftToRight, ambig);
	TEST(ambig && ambig != 3000);
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ELeftToRight;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, ETrue, TChar::EOtherNeutral,
		TChar::ELeftToRight, ambig);
	TEST(ambig && ambig != 3000);

	// right to left following on from left to right paragraph is ambiguous
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ELeftToRight;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, EFalse, TChar::ERightToLeft,
		TChar::ERightToLeft, ambig);
	TEST(ambig && ambig != 3000);
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ELeftToRight;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, EFalse, TChar::EOtherNeutral,
		TChar::ERightToLeft, ambig);
	TEST(ambig && ambig != 3000);

	// left to right following on from right to left paragraph is ambiguous
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ERightToLeft;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, ETrue, TChar::ELeftToRight,
		TChar::ELeftToRight, ambig);
	TEST(ambig && ambig != 3000);
	ambig = 3000;
	bs.Reset();
	runs[0].iCategory = TChar::ERightToLeft;
	runs[0].iStart = 0;
	runs[0].iLength = 10;
	bs.ReorderLine(runs, 1, ETrue, EFalse, ETrue, TChar::EOtherNeutral,
		TChar::ELeftToRight, ambig);
	TEST(ambig && ambig != 3000);
	}

void CTBiDiDefect::TestPDEF117110L()
	{
	TPtrC wrappedText;
	TPtrC expectedText;
		
	delete iTestFont;
	iTestFont = new (ELeave) CTestFont;
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test Text\n"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 1);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test Text\r"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 1);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test Text\r\n"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 1);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test Text\x2028"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 1);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test Text\n\x2028"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 2);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test Text\x2028"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test\nText\n"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 2);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test\x2028Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
	
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test\nText\n"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 1);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test\x2026"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("TestText\n"), 5);
	iBidiText->WrapText(45, *iTestFont, NULL, 2);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test\x2028Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = TBidiText::NewL(_L("Test Text"), 5);
	iBidiText->WrapText(KMaxTInt, *iTestFont, NULL, 1);
	wrappedText.Set(iBidiText->DisplayText());
	expectedText.Set(_L("Test Text"));
	TEST(expectedText.Compare(wrappedText) == 0);
		
	delete iBidiText;
	iBidiText = NULL;
	delete iTestFont;
	iTestFont = NULL;
	}

//--------------
CTBiDiDefectStep::CTBiDiDefectStep()
	{
	SetTestStepName(KTBiDiDefectStep);
	}

CTGraphicsBase* CTBiDiDefectStep::CreateTestL()
	{
	return new (ELeave) CTBiDiDefect(this);
	}
