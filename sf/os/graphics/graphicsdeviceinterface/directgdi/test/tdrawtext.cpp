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

#include "tdrawtext.h"
#include <graphics/directgdiengine.h>
#include <graphics/directgdicontext.h>


LOCAL_C void CleanCache(TAny* aPtr)
	{
	MFontGlyphImageStorage* glyphImageStorage = reinterpret_cast<MFontGlyphImageStorage*> (aPtr);
	glyphImageStorage->CleanGlyphImageCache();
	}

/**
Default text drawing target size
*/
const TSize KDrawTextDefaultSize(640, 400);

/**
Test text
*/
_LIT16(KPangram, "The quick brown fox jumps over the lazy dog");
_LIT16(KAbcd,"Abcd");


CTDrawText::CTDrawText()
	{
	SetTestStepName(KTDirectGdiDrawTextStep);
	}

CTDrawText::~CTDrawText()
	{
	if(iFont)
		{
		ReleaseFont(iFont);		
		}
	}


/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTDrawText::doTestStepL()
	{
	// Test for each target pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
			
		SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, KDrawTextDefaultSize);

		iFont = GetFont();
		User::LeaveIfNull(iFont);
			
		RunTestsL();
		// only run OOM tests for one target pixel format to prevent duplication of tests
		if (targetPixelFormatIndex == 0)
			{
			RunOomTestsL();  //from base class
			}

		if(iFont)
			{
			ReleaseFont(iFont);
			iFont = NULL;
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDrawText::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0001
	
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
	Basic test for text drawing with basic and valid parameters.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented
	  
@SYMTestActions 	
	Test the DrawText() and DrawTextVertical() functions for text drawing using either BitGDI or DirectGDI gc.
	  
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
	
*/
void CTDrawText::TestBasicFunctionality()
	{
	_LIT(KTestName, "Text-Basic"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));
	
	//1.down
	
	iGc->DrawTextVertical(KPangram,NULL,TPoint(10,10),EFalse);
	
	iGc->DrawTextVertical(KAbcd, NULL, EFalse);
	
	TRect rect(30,10,50,400);
	TInt baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KPangram,NULL,rect,baseline,EFalse);
	
	rect.SetRect(50,10,80,400);
	baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KPangram,NULL,rect,baseline,10,EFalse);
	
	rect.SetRect(50,100,80,400);
	iGc->DrawTextVertical(KAbcd,NULL,rect,EFalse);
	
	//2.up
	
	iGc->DrawTextVertical(KPangram,NULL,TPoint(105,390),ETrue);
	
	iGc->DrawTextVertical(KAbcd,NULL,ETrue);
	
	rect.SetRect(110,10,140,390);
	baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KPangram,NULL,rect,baseline,ETrue);
	
	rect.SetRect(140,10,160,390);
	baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KPangram,NULL,rect,baseline,10,ETrue);
	
	rect.SetRect(140,10,160,200);
	iGc->DrawTextVertical(KAbcd,NULL,rect,ETrue);
	
	//horizontal
	iGc->DrawText(KPangram,NULL,TPoint(170, 25));
	
	iGc->DrawText(KAbcd,NULL);
	
	rect.SetRect(170,30,640,50);
	baseline = rect.Height() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawText(KPangram,NULL,rect,baseline);
		
	rect.SetRect(170,30,640,50);
	iGc->DrawText(KAbcd,NULL,rect);
	
	
	iGc->ResetFont();

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0002
	
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
	Test for text positioning and clipping in a rendering target. 
	Only horizontal drawing direction is taken into consideration.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented  
	
@SYMTestActions 	
	 Loop over all versions od DrawText() method.
	 Using each one draw a text "text" on screen at various positions.
	 Increment position from (-50,-50) up to target width+50 and target heigh+50
	 to test boundary cases as well. 
	 
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTDrawText::TestTextPositioning()
	{
	_LIT(KTestName, "Text-Positioning"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	_LIT(KText, "test");
	for(TInt funcNum=0; funcNum<4; funcNum++)
		{
		
		ResetGc();
		
		iGc->SetFont(iFont);
		TEST(iGc->HasFont());
		iGc->SetPenColor(TRgb(0, 0, 0));
		
		TPositionIterator posIterator(-50, KDrawTextDefaultSize.iWidth+50, 30, -50, KDrawTextDefaultSize.iHeight+50, 13);
		posIterator.Begin();
		
		do
			{
			switch(funcNum)
				{
				case 0:
					{
					// void DrawText(const TDesC& aText);
					iGc->Reset();
					iGc->SetFont(iFont);
					TEST(iGc->HasFont());
					iGc->SetPenColor(TRgb(0, 0, 0));
					iGc->SetOrigin(TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX));
					iGc->DrawText(KText,NULL);
					break;
					}

				case 1:
					{
					// void DrawText(const TDesC& aText, const TPoint& aPosition)
					iGc->DrawText(KText,NULL,TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX));
					break;
					}

				case 2:
					{
					// void DrawText(const TDesC& aText, const TRect& aBox)
					iGc->Reset();
					iGc->SetFont(iFont);
					TEST(iGc->HasFont());
					iGc->SetPenColor(TRgb(0, 0, 0));
					TPoint pos(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX);
					iGc->SetOrigin(pos);
					TRect box(-pos, KDrawTextDefaultSize);
					iGc->DrawText(KText,NULL,box);
					break;
					}
					
				case 3:
					{
					// void DrawText(const TDesC& aText, const TRect& aBox, TInt aBaselineOffset, 
					//		DirectGdi::TTextAlign aAlignment = DirectGdi::ELeft, TInt aMargin = 0)
					TRect rect(TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX), TSize(32, 13) );
					iGc->DrawText(KText, NULL, rect, 13, DirectGdi::ELeft, 1);
					break;
					}
				}
			}
		while(posIterator.Next());
		
		TESTNOERROR(iGc->GetError());
		TBuf<40> tname;
		tname.Format(_L("%S%d"), &KTestName, funcNum);
		TEST(KErrNone == WriteTargetOutput(iTestParams, tname));
		
		iGc->ResetFont();
		}
	}


/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0003
	
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
	Test for clipping text based on the box parameter.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented  
	
@SYMTestActions
	Set the origin to a non-zero position. 
	Draw a text "text" on screen at various positions using
	incremented box size.
	
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTDrawText::TestTextBox()
	{	
	_LIT(KTestName, "Text-BoxClipping"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	iGc->SetOrigin(TPoint(2, -2));
	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->SetBrushColor(TRgb(150, 150, 150));

	_LIT(KText, "test");

	TInt baseline = 2 + iFont->FontMaxAscent()/2;
	TPositionIterator posIterator(0, KDrawTextDefaultSize.iWidth, 50, 0, KDrawTextDefaultSize.iHeight, 13);
	posIterator.Begin();
	
	do
		{
		TRect rect(TPoint(posIterator.iPosX, posIterator.iPosY), TSize(posIterator.iIndexY, posIterator.iIndexX));
		if(!rect.IsEmpty())
			{
			iGc->DrawText(KText, NULL, rect, baseline);
			}
		}
	while(posIterator.Next());
	
	iGc->ResetFont();

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}


/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0004
	
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
	Test for positioning vertical text in a rendering target.
	Only vertical drawing direction is taken into consideration.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented  
	
@SYMTestActions
 	Loop over all versions od DrawTextVertical() method.
	Using each one draw a text "text" on screen at various positions.
	Increment position from (-50,-70) up to target width+50 and target heigh+70
	to test boundary cases as well. 
	
@SYMTestExpectedResults 
	Test should perform graphics operations successfully.
*/
void CTDrawText::TestTextPositioningVertical(TBool aUp)
	{	
	_LIT(KTestName, "Text-PositioningVertical"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	_LIT(KText, "test");

	for(TInt funcNum = 0; funcNum<5; funcNum++)
		{
		
		ResetGc();

		iGc->SetFont(iFont);
		TEST(iGc->HasFont());
		iGc->SetPenColor(TRgb(0, 0, 0));
		
		TPositionIterator posIterator(-50, KDrawTextDefaultSize.iWidth+50, 10, -70, KDrawTextDefaultSize.iHeight+70, 30);
		posIterator.Begin();
		
		do
			{
			switch(funcNum)
				{
				case 0:
					{
					// void DrawTextVertical(const TDesC& aText, TBool aUp)
					iGc->Reset();
					iGc->SetFont(iFont);
					TEST(iGc->HasFont());
					iGc->SetPenColor(TRgb(0, 0, 0));
					iGc->SetOrigin(TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX));
					iGc->DrawTextVertical(KText, NULL, aUp);
					break;
					}

				case 1:
					{
					// void DrawTextVertical(const TDesC& aText, const TPoint& aPosition, TBool aUp)
					iGc->DrawTextVertical(KText, NULL, TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX), aUp);
					break;
					}

				case 2:
					{
					// void DrawTextVertical(const TDesC& aText, const TRect& aBox, TBool aUp)
					iGc->Reset();
					iGc->SetFont(iFont);
					TEST(iGc->HasFont());
					iGc->SetPenColor(TRgb(0, 0, 0));
					TPoint pos(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX);
					iGc->SetOrigin(pos);
					TRect box(-pos, KDrawTextDefaultSize);
					iGc->DrawTextVertical(KText, NULL, box, aUp);
					break;
					}
					
				case 3:
					{
					// void DrawTextVertical(const TDesC& aText, const TRect& aBox, TInt aBaselineOffset, 
					//		TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0)
					TRect rect(TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX),TSize(13, 32));
					iGc->DrawTextVertical(KText, NULL, rect, 12, aUp,DirectGdi::ELeft, 0);
					break;
					}
					
				case 4:
					{
					// void DrawTextVertical(const TDesC& aText, const TRect& aBox, TInt aBaselineOffset, 
					//		TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0)
					TRect rect(TPoint(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX),TSize(13, 32));
					iGc->DrawTextVertical(KText, NULL, rect, 12, 4, aUp, DirectGdi::ELeft, 0);
					break;
					}
				}
			}
		while(posIterator.Next());
		
		TESTNOERROR(iGc->GetError());
	
		_LIT(KUp, "up");
		_LIT(KDown, "down");
	
		TBuf<40> tname;
		tname.Format(_L("%S%d%S"), &KTestName, funcNum, aUp ? &KUp : &KDown);
		TESTNOERROR(WriteTargetOutput(iTestParams, tname));
		
		iGc->ResetFont();
		}
	}


/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0005
	
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
	Test for clipping text based on the clipping rectangle parameter.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented  
	
@SYMTestActions
    Set the origin to a non-zero position.
 	Draw a text "text" on screen at various positions using
	incremented box size.
	
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTDrawText::TestTextBoxVertical()
	{	
	_LIT(KTestName, "Text-BoxClippingVertical"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->SetBrushColor(TRgb(150, 150, 150));
	iGc->SetOrigin(TPoint(2, -2));

	_LIT(KText, "test");

	TInt baseline = 2 + iFont->FontMaxAscent()/2;
	
	TPositionIterator posIterator(0, KDrawTextDefaultSize.iWidth, 14, 0, KDrawTextDefaultSize.iHeight, 40);
	posIterator.Begin();
	
	do
		{
		TRect rect(TPoint(posIterator.iPosX, posIterator.iPosY), TSize(posIterator.iIndexY, posIterator.iIndexX));
		if(!rect.IsEmpty())
			{
			iGc->DrawTextVertical(KText, NULL, rect, baseline, EFalse);
			}
		}
	while(posIterator.Next());
	
	iGc->ResetFont();

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}


/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0006
	
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
	Testing usage of pen colour, brush colour, style and pattern.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented 
	
@SYMTestActions 	
	Test pen and brush parameters with text drawing methods.
	Use color and brush style tables to generate various combinations
	of pen color, brush color and brush style. Text drawing on screen
	describes current color and style settings.  
	*NOTE extended for DEF133363-Zero length text causes no brush pattern to be drawn
	
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTDrawText::TestPenBrushL()
	{
	_LIT(KTestName, "Text-PenBrush"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	_LIT(KTxtPenBrushStyle,"PenColor-BrushColor-BrushStyle");
	_LIT(KTxtDash,"-");
	
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iTargetPixelFormat, (TSize(100, 100)));
	TESTL(bitmap != NULL);
	CleanupStack::PushL(bitmap);

	ResetGc();
	
	iGc->SetBrushPattern(*bitmap);
	
	iGc->SetFont(iFont);
	TEST(iGc->HasFont());

	TPoint point(22,10);
	TRect rect;
	TInt ii,baseline;
	
	//vertical
	for(ii = 0; ii<16; ii++)
		{
		iGc->SetPenColor(KColor16Table[ii%16]);
		point.iX =26 + ii*17;
		iGc->DrawTextVertical(KColor16TableNames[ii%16],NULL,point,EFalse);
		}

	point.SetXY(3,110);
	iGc->SetPenColor(KRgbBlack);
	iGc->DrawTextVertical(KTxtPenBrushStyle,NULL,point,EFalse);
	point.SetXY(20,110);
	for(ii = 0; ii<16; ii++)
		{
		point.iX =20+ii*17;
		TBuf<200> txtbuf;
		iGc->SetPenColor(KColor16Table[ii%16]);
		txtbuf.Append(KColor16TableNames[ii%16]);
		txtbuf.Append(KTxtDash);
		
		iGc->SetBrushColor(KColor16Table[(5*ii+1)%16]);
		txtbuf.Append(KColor16TableNames[(5*ii+1)%16]);
		txtbuf.Append(KTxtDash);
		
		iGc->SetBrushStyle(KBrushStyleTable[(2*ii+1)%9]);
		txtbuf.Append(KBrushStyleTableNames[(2*ii+1)%9]);
		
		if (ii == 6 || ii == 7 || ii == 8)
			{	//for DEF133363
			txtbuf.SetLength(0);
			}
		
		rect.SetRect(point.iX,point.iY,point.iX+17,point.iY+290);
		baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
		iGc->DrawTextVertical(txtbuf,NULL,rect,baseline,EFalse);
		}

	//horizontal
	point.SetXY(300,17);
	iGc->SetPenColor(KRgbBlack);
	iGc->DrawText(KTxtPenBrushStyle,NULL,point);
	for(ii=0; ii<16; ii++)
		{
		point.iY = 20+ii*17;
		TBuf<200> txtbuf;
		iGc->SetPenColor(KColor16Table[ii%16]);
		txtbuf.Append(KColor16TableNames[ii%16]);
		txtbuf.Append(KTxtDash);

		iGc->SetBrushColor(KColor16Table[(3*ii+5)%16]);
		txtbuf.Append(KColor16TableNames[(3*ii+5)%16]);
		txtbuf.Append(KTxtDash);

		iGc->SetBrushStyle(KBrushStyleTable[(5*ii+3)%9]);
		txtbuf.Append(KBrushStyleTableNames[(5*ii+3)%9]);
		
		if (ii == 9 || ii == 10 || ii == 11)
			{	//for DEF133363
			txtbuf.SetLength(0);
			}

		rect.SetRect(point.iX,point.iY,point.iX+290,point.iY+17);
		baseline = rect.Height() / 2 + iFont->AscentInPixels() / 2;
		iGc->DrawText(txtbuf,NULL,rect,baseline);	
		}

	for(ii=0; ii<16; ii++)
		{
		point.iX = 300 + (ii%4)*80;
		point.iY = 310 + (ii/4)*17;
		iGc->SetPenColor(KColor16Table[ii%16]);
		iGc->DrawText(KColor16TableNames[ii%16],NULL,point);
		}


	iGc->ResetFont();

	CleanupStack::PopAndDestroy(bitmap);

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0007
	
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
	Test for alignment of text inside drawing box.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented 
	
@SYMTestActions 	
	Call functions testing all combinations of parameters values given below:
	aAlignment:
		DGDI::ELeft,
		DGDI::ECenter,
		DGDI::ERight
	aMargin:
		-50,
		0,
		50
	aUp:
		ETrue,
		EFalse
	Use gray background box to show arguments impact. 

@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTDrawText::TestAlign()
	{
	_LIT(KTestName, "Text-Alignment");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	ResetGc();

	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->SetBrushColor(KRgbGray);

	_LIT(KMargin," Margin = ");
	_LIT(KAlign,"Align = ");
	TRect rect;
	TInt baseline;

	//vertical
	for(TInt kk=0; kk<2; kk++) //loop over up and down directions
		{
		for(TInt ii=0; ii<3; ii++) // negative, zero and positive margin value
			{
			TInt margin = (-ii+1)*50;
			for(TInt jj=0; jj<3; jj++) // ELeft, ECenter and ERight alignment values
				{
				rect.SetRect(kk*185+5+(3*ii+jj)*20 +1, 10, kk*185+25+(3*ii+jj)*20-1, 390);
				baseline = rect.Width()/2 + iFont->AscentInPixels()/2 ;
				TBuf<128> textbuf;
				textbuf.Copy(KAlign);
				textbuf.Append(KTextAlignTableNames[jj%3]);
				textbuf.Append(KMargin);
				textbuf.AppendNum(margin);
				iGc->DrawTextVertical(textbuf,NULL,rect,baseline,kk!=0,KTextAlignTable[jj%3],margin);
				}
			}
		}

	//horizontal
	for(TInt ii=0; ii<3; ii++) // negative, zero and positive margin value
		{
		TInt margin = (-ii+1)*40;
		for(TInt jj=0; jj<3; jj++) // ELeft, ECenter and ERight alignment values
			{
			rect.SetRect(373, 30+(3*ii+jj)*20+1, 635, 50+(3*ii+jj)*20-1);
			baseline = rect.Height()/2 + iFont->AscentInPixels()/2;
			TBuf<128> textbuf;
			textbuf.Copy(KAlign);
			textbuf.Append(KTextAlignTableNames[jj%3]);
			textbuf.Append(KMargin);
			textbuf.AppendNum(margin);
			iGc->DrawText(textbuf,NULL,rect,baseline,KTextAlignTable[jj%3],margin);
			}
		}

	iGc->ResetFont();

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0008
	
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
	Testing justification of a text.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Initial version 
	
@SYMTestActions 	
	Test the effect of the following functions:
	SetCharJustification()
	SetWordJustification()
	UpdateJustification()
	UpdateJustificationVertical()
	NoJustifyAutoUpdate() 
	
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.

*/
void CTDrawText::TestJustification()
	{
	_LIT(KTestName, "Text-Justification"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	ResetGc();

	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));

	//vertical
	TInt charExcessWidth = 30;
	TInt charNumGaps = 30;
	TInt wordExcessWidth = 100;
	TInt wordNumChars = 5;

	TRect rect;
	TInt baseline, kk, jj;
	for( kk=0; kk<2; kk++) //loop over up and down directions
		{
		for( jj=0; jj<4; jj++) 
			{
			iGc->SetCharJustification(charExcessWidth*(jj%2) , charNumGaps*(jj%2));
			iGc->SetWordJustification(wordExcessWidth*(jj>>1), wordNumChars*(jj>>1));
			rect.SetRect(10+(5*kk+jj)*20+1, 10, 30+(5*kk+jj)*20-1, 390);
			baseline = rect.Width()/2 + iFont->AscentInPixels()/2;
			iGc->DrawTextVertical(KPangram,NULL,rect,baseline,kk!=0);
			}
		}

	//horizontal
	for( jj=0; jj<4; jj++)
		{
		iGc->SetCharJustification(charExcessWidth*(jj%2) , charNumGaps*(jj%2));
		iGc->SetWordJustification(wordExcessWidth*(jj>>1), wordNumChars*(jj>>1));
		rect.SetRect(210, 10+jj*20+1, 640, 30+jj*20-1);
		baseline = rect.Height()/2 + iFont->AscentInPixels()/2;
		iGc->DrawText(KPangram,NULL,rect,baseline);
		}

	iGc->ResetFont();

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0009
	
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
	Testing usage of shadow colour, underline and strikethrough rendering using either BitGDI or DirectGDI gc.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented 
	
@SYMTestActions 	
	Test the shadow colour, underline and strikethrough parameters for text drawing. 
	
@SYMTestExpectedResults 
	Valid bitmap should be created. This bitmap should be the same as a reference bitmap.
*/
void CTDrawText::TestShadowUnderStrike()
	{
	_LIT(KTestName, "Text-ShadowUnderlineStrikethrough"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));

	//look-up tables, simplifies below loops
	const DirectGdi::TFontUnderline KFontUnderlineTable[]=
		{
		DirectGdi::EUnderlineOn,
		DirectGdi::EUnderlineOff,
		DirectGdi::EUnderlineOn,
		DirectGdi::EUnderlineOff,
		DirectGdi::EUnderlineOn,
		};

	const DirectGdi::TFontStrikethrough KFontStrikethroughTable[]=
		{
		DirectGdi::EStrikethroughOff,
		DirectGdi::EStrikethroughOn,
		DirectGdi::EStrikethroughOn,
		DirectGdi::EStrikethroughOff,
		DirectGdi::EStrikethroughOn,
		};
	
	//vertical
	TPoint position;
	for(TInt ii=0; ii<2; ii++ ) //loop over up and down directions
		{
		position.SetXY(10+ii*100,10+ii*380);
		for(TInt jj=0; jj<5; jj++)
			{
			iGc->SetUnderlineStyle(KFontUnderlineTable[jj%5]);
			iGc->SetStrikethroughStyle(KFontStrikethroughTable[jj%5]);
			if(jj==3)
				{
				iGc->SetTextShadowColor(TRgb(100,10,10));
				}
			iGc->DrawTextVertical(KPangram,NULL,position,ii!=0);
			position.iX+=20;
			}
		}

	//horizontal
	position.SetXY(210,30);
	for(TInt jj=0; jj<5; jj++)
		{
		iGc->SetUnderlineStyle(KFontUnderlineTable[jj%5]);
		iGc->SetStrikethroughStyle(KFontStrikethroughTable[jj%5]);
		if(jj==3)
			{
			iGc->SetTextShadowColor(TRgb(100,10,10));
			}
		iGc->DrawText(KPangram,NULL,position);
		position.iY+=20;
		}

	iGc->ResetFont();

	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-TEXT-0010
	
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
	Test for propper handling invalid arguments using either BitGDI or DirectGDI gc.
	
@SYMTestPriority	
	Critical
	
@SYMTestStatus 		
	Implemented 
	
@SYMTestActions 	
	Test for propper handling invalid arguments for text drawing using either BitGDI or DirectGDI gc.  
	
@SYMTestExpectedResults 
	Function shall detect invalid parameters and return. Nothing will be drawn on the target surface.
*/
void CTDrawText::TestInvalidParamsL()
	{
	_LIT(KTestName, "Text-InvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	iGc->SetFont(iFont);
	TEST(iGc->HasFont());
	iGc->SetPenColor(TRgb(0, 0, 0));
	
	//NullDescriptor - drawing an empty descriptor is not an error, DrawText()
	//and DrawTextVertical() should return silently in this case.
	//horizontal
	iGc->DrawText(KNullDesC, NULL, TPoint(10, 30));
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	iGc->DrawText(KNullDesC, NULL);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	
	TRect rect(10,50,300,70);
	TInt baseline = rect.Height() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawText(KNullDesC,NULL,rect,baseline);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	rect.SetRect(10,70,550,90);
	iGc->DrawText(KNullDesC,NULL,rect);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);

	//vertical
	iGc->DrawTextVertical(KNullDesC,NULL,TPoint(10,10),EFalse);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	iGc->DrawTextVertical(KNullDesC,NULL,EFalse);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	rect.SetRect(30,10,50,300);
	baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KNullDesC,NULL,rect,baseline,EFalse);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	rect.SetRect(50,10,80,300);
	baseline = rect.Width() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KNullDesC,NULL,rect,baseline,10,EFalse);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);
	rect.SetRect(50,100,80,400);
	iGc->DrawTextVertical(KNullDesC,NULL,rect,EFalse);
	CheckErrorsL(KErrNone, KErrNone, (TText8*)__FILE__, __LINE__);

	//invalid box
	for(TInt ii=1; ii<9; ii++)
		{
		rect.SetRect(220+(ii%3)*100, 100+(ii/3)*100, 320, 200);
		baseline = Abs(rect.Height())/2 + iFont->AscentInPixels()/2;
		iGc->DrawText(KPangram,NULL,rect,baseline);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
		iGc->DrawText(KPangram,NULL,rect);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		baseline = Abs(rect.Width())/2 + iFont->AscentInPixels() /2;
		iGc->DrawTextVertical(KPangram,NULL,rect,baseline,EFalse);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		iGc->DrawTextVertical(KPangram,NULL,rect,EFalse);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		iGc->DrawTextVertical(KPangram,NULL,rect,baseline,10,EFalse);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
		}
		
	//invalid aAlignment
	rect.SetRect(10,320,300,540);
	baseline = rect.Height() / 2 + iFont->AscentInPixels() / 2;
	iGc->DrawText(KPangram,NULL,rect,baseline, (DirectGdi::TTextAlign) 5 );
	CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

	rect.SetRect(540,10,560,350);
	baseline = rect.Width() /2 + iFont->AscentInPixels() / 2;
	iGc->DrawTextVertical(KPangram,NULL,rect,baseline,10, EFalse , (DirectGdi::TTextAlign) 5 );
	CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
	iGc->ResetFont();	
	}
	
/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDrawText::RunTestsL()
	{
	MFontGlyphImageStorage* glyphStorage = NULL;
	iGc->GetInterface(TUid::Uid(KDirectGdiGetGlyphStorageUid), (TAny*&) glyphStorage);
	if(glyphStorage)
		{
		glyphStorage->CleanGlyphImageCache();
		CleanupStack::PushL(TCleanupItem(CleanCache, glyphStorage));
		}
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0001"));
	TestBasicFunctionality();
	RecordTestResultL();
	if(!iRunningOomTests)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0002"));
		TestTextPositioning();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0003"));
		TestTextBox();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0004"));
		TestTextPositioningVertical(EFalse);
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0004"));
		TestTextPositioningVertical(ETrue);
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0005"));
		TestTextBoxVertical();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0008"));
		TestJustification();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0009"));
		TestShadowUnderStrike();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0007"));
		TestAlign();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0006"));
		TestPenBrushL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-TEXT-0010"));
		TestInvalidParamsL();
		RecordTestResultL();
		}

	if(glyphStorage)
		{
		glyphStorage->CleanGlyphImageCache();
		CleanupStack::Pop(glyphStorage);
		}
	}


	
