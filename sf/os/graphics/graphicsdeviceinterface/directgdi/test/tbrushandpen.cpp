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

#include "tbrushandpen.h"
#include <e32math.h>

/**
default brush and pen drawing target size
*/
const TSize KDrawBrushPenDefaultSize(640, 450);

CTBrushAndPen::CTBrushAndPen()
	{
	SetTestStepName(KTDirectGdiBrushAndPenStep);
	}

CTBrushAndPen::~CTBrushAndPen()
	{
	}


/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-PENBRUSH-0001

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
	Test drawing with brush.

@SYMTestPriority 
	High

@SYMTestStatus
	Implemented

@SYMTestActions 
	Basic test for pen and brush methods.
	Test the following brush settings:
		- colour
		- style
		Call drawing functions with the following parameters:
			colours from the COLOR_TABLE.
			aBrushStyle
				ENullBrush
				ESolidBrush
				EPatternedBrush 
				EVerticalHatchBrush 
				EForwardDiagonalHatchBrush 
				EHorizontalHatchBrush 
				ERearwardDiagonalHatchBrush 
				ESquareCrossHatchBrush 
				EDiamondCrossHatchBrush 
	Issue shape drawing operations: DrawRect, DrawRoundRect, DrawEllipse, DrawPie, DrawPolygon.
	Draw overlapping shapes to test correctness of brush transparency. 
	Repeat the drawing in write alpha mode (DrawModeWriteAlpha bitmap).	

@SYMTestExpectedResults
	Brush colours and styles should be correctly drawn for each shape and each draw mode.
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.

@param	aDrawMode	the draw mode to run this test in, only DirectGdi::EDrawModePEN and DirectGdi::EDrawModeWriteAlpha
		are supported
*/
void CTBrushAndPen::TestBrushColorAndStyleL(const DirectGdi::TDrawMode aDrawMode)
	{	
	_LIT(KTestName, "PenBrush-BrushColourStyle_%S"); //test case name		
	_LIT(KDrawModePEN, "DrawModePen"); 
	_LIT(KDrawModeWriteAlpha, "DrawModeWriteAlpha"); 
	TBuf<50> testCaseName;	
	
	switch (aDrawMode)
		{
		case DirectGdi::EDrawModePEN:
			testCaseName.Format(KTestName, &KDrawModePEN);
			break;		
		case DirectGdi::EDrawModeWriteAlpha:
			testCaseName.Format(KTestName, &KDrawModeWriteAlpha);
			break;		
		default:
			// This method should only be called aDrawMode set to DirectGdi::EDrawModePEN or DirectGdi::EDrawModeWriteAlpha
			TESTL(EFalse);
			break;
		}
		
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testCaseName);	
		}
	
	ResetGc();
	
	// Set the draw mode
	iGc->SetDrawMode(aDrawMode);
	TESTNOERROR(iGc->GetError());
	
	iGc->SetPenStyle(DirectGdi::ESolidPen);

	// print two row of shapes
	// the second one has overlapping shapes to test alpha blending
	for(TInt j =0; j<2; j++) 
		{
		// draw 8 columns of different shapes with various pen colours
		// brush colours, and brush styles 
		for(TInt i = 0; i<8; i++) 
			{			
			iGc->SetBrushStyle(KBrushStyleTable[i==1?1:i+1]);
		
			TInt startx = 10+i*70 - j*i*10;
			TInt endx = 70+i*70;
			TInt starty = j*263;
			
			//this is setter helper function, see below method definition
			SetHashColors(i,j,1);
			iGc->DrawRect(TRect(startx,starty+10,endx,starty+30));
		
			SetHashColors(i,j,2);
			iGc->DrawRoundRect(TRect(startx,starty+40,endx,starty+60), TSize(10,10));
			
			SetHashColors(i,j,3);
			iGc->DrawPie(TRect(startx,starty+65,endx,starty+125), TPoint(startx,starty+65) , TPoint(endx,starty+65));
			
			// below shapes are not necessary to test alpha blending,
			// so there are drawn only three above shapes with increasing overlap
			if(j == 0) 
				{
				SetHashColors(i,j,4);
				iGc->DrawEllipse(TRect(startx,starty+130 ,endx,starty+190 ));
	
				SetHashColors(i,j,5);
				CArrayFixFlat<TPoint>* pointList = new (ELeave) CArrayFixFlat<TPoint>(6);
				CleanupStack::PushL(pointList);
				pointList->AppendL(TPoint(startx,starty+192));
				pointList->AppendL(TPoint(startx + 50,starty+242));
				pointList->AppendL(TPoint(startx + 10,starty+242));
				pointList->AppendL(TPoint(endx,starty+192));
				pointList->AppendL(TPoint(endx,starty+262));
				pointList->AppendL(TPoint(startx,starty+262));
			
				DirectGdi::TFillRule fillrule = i%2==0 ? DirectGdi::EAlternate : DirectGdi::EWinding;
			
				iGc->DrawPolygon(*pointList ,fillrule);
				CleanupStack::PopAndDestroy(pointList);
				}
			}
		}
	
	// For coverage - test the BrushColor() method works correctly
	iGc->SetBrushColor(KRgbRed);
	TESTL(iGc->BrushColor() == KRgbRed);	

	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(testCaseName)));
	}
	
/**
Helper method that sets the current pen and brush colours from a color table. The color table used contains a 
palette of 16 standard colors ranging from white to black.
@param aGeneralIndex Used when indexing into the color table for the pen and brush colors, and for setting their alpha values.
@param aAlphaStep Used when setting an alpha value for the pen and the brush, the alpha value is set to (255-aAlphaStep*aGeneralIndex*32).
@param aPenBrushIndex Used when indexing into the color table for the pen and brush colors. The pen color is set to 
KColor16Table[(3*aGeneralIndex+aPenBrushIndex)%16] and the brush color is set to KColor16Table[(5*aGeneralIndex+aPenBrushIndex)%16]. 
 */
void CTBrushAndPen::SetHashColors(TInt aGeneralIndex, TInt aAlphaStep, TInt aPenBrushIndex)
	{
	TRgb penColor;
	TRgb brushColor;
	penColor = KColor16Table[(3*aGeneralIndex+aPenBrushIndex)%16];
	penColor.SetAlpha(255-aAlphaStep*aGeneralIndex*32);
	iGc->SetPenColor(penColor);
	brushColor = KColor16Table[(5*aGeneralIndex+aPenBrushIndex)%16];
	brushColor.SetAlpha(255-aAlphaStep*aGeneralIndex*32);
	iGc->SetBrushColor(brushColor);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-PENBRUSH-0002

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
	Test for brush pattern functionality: set a brush pattern for a CDirectGdiContext object,
	then check it has a brush pattern. Test that the SetBrushOrigin() method works as expected.

@SYMTestPriority 
	High

@SYMTestStatus
	Implemented

@SYMTestActions
	Test various brush pattern usage scenarios:
	Set it's brush pattern, then check it has a brush pattern.
	Check setting a brush pattern both from a CFbsBitmap and from a handle.
	Test resetting of brush pattern.
	Call functions that use brush during shapes drawing.
	Create a brush pattern image and set it as the brush pattern. Draw a rectangle, move 
	the brush origin to various positions and draw more shapes.

@SYMTestExpectedResults
	The rectangle is filled correctly for each different brush origins.
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/ 
void CTBrushAndPen::TestBrushPatternOriginL()
	{	
	_LIT(KTestName, "PenBrush-BrushPatternOrigin"); //test case name
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();		
	
	// create patterns programatically so we can test EPatternedBrush		
	TUidPixelFormat pixelFormat = iTestParams.iTargetPixelFormat;
	TSize patternSize(56,56);
	CFbsBitmap* pattern = CreateCheckedBoardBitmapL(pixelFormat, patternSize);
	CleanupStack::PushL(pattern);	
	
	CFbsBitmap* pattern2 = CreateCheckedBoardBitmapL(pixelFormat, patternSize);
	CleanupStack::PushL(pattern2);	

	//draw nine columns of different shapes with various settings 
	// of brush style and brush origin
	for(TInt i = 0; i<9; i++)
		{
		switch(i)
			{
			case 0:
				{
				// (for coverage)
				iGc->ResetBrushPattern();
				
				// Set a brush pattern using CFbsBitmap
				iGc->SetBrushPattern(*pattern);
				TESTNOERRORL(iGc->GetError());
				TEST(iGc->HasBrushPattern());
				
				iGc->SetBrushStyle(DirectGdi::EPatternedBrush);
				break;
				}
			case 1:
				{
				// Check that ResetBrushPattern works
				iGc->ResetBrushPattern();
				TEST(iGc->HasBrushPattern() == EFalse);
				
				// Set a brush pattern using a handle
				iGc->SetBrushPattern(pattern2->Handle());
				TEST(iGc->HasBrushPattern());
			
				iGc->SetBrushStyle(DirectGdi::EPatternedBrush);
				break;
				}
			//change brush origin, and use last settings of brush style	
			case 2:
				{
				// Set a NULL brush pattern, then set the pattern using the handle  
				// for pattern2 (for coverage)
				if(iUseDirectGdi)
					{
					iGc->SetBrushPattern(KNullHandle);				
					TESTL(iGc->GetError() == KErrBadHandle);
					}
				iGc->SetBrushPattern(pattern2->Handle());				
				iGc->SetBrushOrigin(TPoint(10,10));
				break;
				}
			//change brush style and set brush origin
			// in case 4 change only brush origin
			case 3:
				{
				iGc->SetBrushStyle(DirectGdi::EForwardDiagonalHatchBrush);
				iGc->SetPenColor(KRgbGreen);
				iGc->SetBrushColor(KRgbRed);
				iGc->SetBrushOrigin(TPoint(i,i));
				break;
				}
			//change brush style and set brush origin
			// in case 6 change only brush origin
			case 5:
				{
				iGc->SetBrushStyle(DirectGdi::ESquareCrossHatchBrush);
				iGc->SetPenColor(KRgbGreen);
				iGc->SetBrushColor(KRgbRed);
				iGc->SetBrushOrigin(TPoint(i,i));
				break;
				}
			//change brush style and set brush origin
			// in case 8 change only brush origin
			case 7:
				{
				iGc->SetBrushStyle(DirectGdi::EDiamondCrossHatchBrush);
				iGc->SetPenColor(KRgbGreen);
				iGc->SetBrushColor(KRgbRed);
				iGc->SetBrushOrigin(TPoint(i,i));
				break;
				}
			case 4:
			case 6:
			case 8:
				{
				iGc->SetBrushOrigin(TPoint(i,i));
				break;
				}
			}
			
		//draw a set of shapes using current brush
		TInt startx = 10+i*70;
		TInt endx = 70+i*70;
		TInt starty = 0;
		
		iGc->DrawRect(TRect(startx,starty+10,endx,starty+30));
		iGc->DrawRoundRect(TRect(startx,starty+40,endx,starty+60), TSize(10,10));
		iGc->DrawPie(TRect(startx,starty+70,endx,starty+130), TPoint(startx,starty+70) , TPoint(endx,starty+70));
		iGc->DrawEllipse(TRect(startx,starty+130 ,endx,starty+190 ));
		
		CArrayFixFlat<TPoint>* pointList = new (ELeave) CArrayFixFlat<TPoint>(6);
		CleanupStack::PushL(pointList);
		pointList->AppendL(TPoint(startx,starty+200));
		pointList->AppendL(TPoint(startx + 50,starty+250));
		pointList->AppendL(TPoint(startx + 10,starty+250));
		pointList->AppendL(TPoint(endx,starty+200));
		pointList->AppendL(TPoint(endx,starty+270));
		pointList->AppendL(TPoint(startx,starty+270));
	
		iGc->DrawPolygon(*pointList ,DirectGdi::EAlternate);
		CleanupStack::PopAndDestroy(pointList);
		}
	
	// Check that ResetBrushPattern() works
	iGc->ResetBrushPattern();
	TEST(iGc->HasBrushPattern() == EFalse);				
	
	pattern2->Reset();
	TESTNOERROR(iGc->GetError());

	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));

	CleanupStack::PopAndDestroy(2, pattern);		
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-PENBRUSH-0003

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
	Test pen drawing functionality.

@SYMTestPriority 
	High

@SYMTestStatus
	Implemented.

@SYMTestActions
	Test setting pen colour and size.
	Change pen settings:
		colours from the COLOR_TABLE.
		Iterate over various pen sizes.
	Perform drawing operations that use the pen.
	Use also overlapping shapes to test alpha blending.

@SYMTestExpectedResults
	Valid bitmap should be created. Shapes will be drawn with correct pen colour and size.
	This bitmap shall be compared to a reference bitmap.
*/ 
void CTBrushAndPen::TestPenColorSizeL(const DirectGdi::TDrawMode aDrawMode)
	{
	_LIT(KTestName, "PenBrush-PenColorSize_%S"); //test case name
	_LIT(KDrawModePEN, "DrawModePen");
	_LIT(KDrawModeWriteAlpha, "DrawModeWriteAlpha");
	TBuf<50> testCaseName;

	switch (aDrawMode)
		{
		case DirectGdi::EDrawModePEN:
			testCaseName.Format(KTestName, &KDrawModePEN);
			break;
		
		case DirectGdi::EDrawModeWriteAlpha:
			testCaseName.Format(KTestName, &KDrawModeWriteAlpha);
			break;
		
		default:
			// This method should only be called aDrawMode set to DirectGdi::EDrawModePEN or DirectGdi::EDrawModeWriteAlpha
			TESTL(EFalse);
			break;
		}
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testCaseName);	
		}
	
	ResetGc();

	//draw nine columns of different shapes
	// the last four overlaps to test alpha blending
	for(TInt i = 0 ; i < 9; i++)
		{
		if(i==0)
			{
			iGc->SetBrushStyle(DirectGdi::ESolidBrush);
			iGc->SetBrushColor(KRgbDarkRed);
			}
		else
			{
			iGc->SetBrushStyle(DirectGdi::ENullBrush);
			}
		
		
		TInt stfact = (i<5 ? 0 : i);
		TInt startx = 10+i*70 - stfact*10;
		TInt endx = 70+i*70;
		
		TRgb penColor;
				
		iGc->SetPenSize(TSize(i,i));
		// The sw version allocates memory when you set a pen size and this memory is not freed
		// until after the __UHEAP_MARKEND macro when running OOM tests, this causes a memory 
		// leak when this test leaves. The following TCleanupItem has been added to reset the pen
		// size to 1,1 on account of a leave as settting the pen size to 1,1 deletes the allocated
		// pen memory.
		CleanupStack::PushL(TCleanupItem(ResetPenSize, iGc));

		penColor = KColor16Table[(5*i+1)%15+1];
		penColor.SetAlpha(255-stfact*30);
		iGc->SetPenColor(penColor);	
		iGc->DrawRect(TRect(startx,10,endx,30));

		penColor = KColor16Table[(5*i+2)%15+1];
		penColor.SetAlpha(255-stfact*30);
		iGc->SetPenColor(penColor);
		iGc->DrawRoundRect(TRect(startx,40,endx,60), TSize(10,10));
			
		penColor = KColor16Table[(5*i+3)%15+1];
		penColor.SetAlpha(255-stfact*30);
		iGc->SetPenColor(penColor);
		iGc->DrawPie(TRect(startx,70,endx,130), TPoint(startx,70), TPoint(endx,70));
			
		penColor = KColor16Table[(5*i+4)%15+1];
		penColor.SetAlpha(255-stfact*30);
		iGc->SetPenColor(penColor);
		iGc->DrawEllipse(TRect(startx,135 ,endx,195 ));
			
		penColor = KColor16Table[(5*i+5)%15+1];
		penColor.SetAlpha(255-stfact*30);
		CArrayFixFlat<TPoint>* pointList = new (ELeave) CArrayFixFlat<TPoint>(6);
		CleanupStack::PushL(pointList);
		pointList->AppendL(TPoint(startx,200));
		pointList->AppendL(TPoint(startx+50,250));
		pointList->AppendL(TPoint(startx+10,250));
		pointList->AppendL(TPoint(endx,200));
		pointList->AppendL(TPoint(endx,270));
		pointList->AppendL(TPoint(startx,270));
		iGc->SetPenColor(penColor);
		iGc->DrawPolygon(*pointList ,DirectGdi::EAlternate);
		CleanupStack::PopAndDestroy(pointList);
			
		penColor = KColor16Table[(5*i+6)%15+1];
		penColor.SetAlpha(255-stfact*30);
		CArrayFixFlat<TPoint>* pointListPolyLine = new (ELeave) CArrayFixFlat<TPoint>(4);
		CleanupStack::PushL(pointListPolyLine);
	    pointListPolyLine->AppendL(TPoint(startx,280));
	    pointListPolyLine->AppendL(TPoint(endx,320));
	    pointListPolyLine->AppendL(TPoint(startx,320));
	    pointListPolyLine->AppendL(TPoint(endx,280));
		iGc->SetPenColor(penColor);
	    iGc->DrawPolyLine(*pointListPolyLine);
	    CleanupStack::PopAndDestroy(pointListPolyLine);

	    penColor = KColor16Table[(5*i+7)%15+1];
		penColor.SetAlpha(255-stfact*30);
		iGc->SetPenColor(penColor);
		iGc->DrawArc(TRect(startx,330 ,endx, 360),TPoint(endx,330),TPoint(startx,330));

	    penColor = KColor16Table[(5*i+8)%15+1];
		penColor.SetAlpha(255-stfact*30);
		iGc->SetPenColor(penColor);
	    iGc->DrawLine(TPoint(startx,355),TPoint(endx,375));
	    
	    penColor = KColor16Table[(5*i+8)%15+1];
		penColor.SetAlpha(255-stfact*30);
		pointListPolyLine = new (ELeave) CArrayFixFlat<TPoint>(4);
		CleanupStack::PushL(pointListPolyLine);
	    pointListPolyLine->AppendL(TPoint(startx,400));
	    pointListPolyLine->AppendL(TPoint(endx,440));
	    pointListPolyLine->AppendL(TPoint(startx,440));
	    pointListPolyLine->AppendL(TPoint(endx,400));
		iGc->SetPenColor(penColor);		
	    iGc->DrawPolyLineNoEndPoint(*pointListPolyLine);
	    CleanupStack::PopAndDestroy(pointListPolyLine);
	    
	    CleanupStack::PopAndDestroy(1);
		}

	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(testCaseName)));
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-PENBRUSH-0004

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
	Test drawing with various brush styles.

@SYMTestPriority 
	High

@SYMTestStatus
	Implemented

@SYMTestActions
	Test setting pen style.
	Set pen style:
	Call functions that use pen during shapes drawing and are affected by the pen
	style (DrawLine(), DrawPolyLine(), DrawRect(), DrawPolygon();.

@SYMTestExpectedResults
	Valid bitmap should be created. Shapes will be drawn with correct pen style. This
	bitmap shall be compared to a reference bitmap.
*/
void CTBrushAndPen::TestPenStyleL()
	{
	_LIT(KTestName, "PenBrush-PenStyle"); //test case name		
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();	

	//draw nine columns of different shapes with 
	// various pen style settigns
	for(TInt i = 0 ; i < 9; i++)
		{
		TInt colindx = ( i < 6 ? i : i+1 );
		iGc->SetPenStyle(KPenStyleTable[colindx%6]);
		
		iGc->SetPenColor(KColor16Table[(i+6)%16]);	
		iGc->DrawRect(TRect(10+i*70,10,70+i*70,30));
		
		iGc->DrawLine(TPoint(10+i*70,40),TPoint(70+i*70,190));
		iGc->DrawLine(TPoint(70+i*70,40),TPoint(10+i*70,190));
			
		CArrayFixFlat<TPoint>* pointList = new (ELeave) CArrayFixFlat<TPoint>(6);
		CleanupStack::PushL(pointList);
		pointList->AppendL(TPoint(10+i*70,200));
		pointList->AppendL(TPoint(60+i*70,250));
		pointList->AppendL(TPoint(20+i*70,250));
		pointList->AppendL(TPoint(70+i*70,200));
		pointList->AppendL(TPoint(70+i*70,270));
		pointList->AppendL(TPoint(10+i*70,270));
		
		iGc->DrawPolygon(*pointList ,DirectGdi::EAlternate);
		CleanupStack::PopAndDestroy(pointList);
			
		CArrayFixFlat<TPoint>* pointListPolyLine = new (ELeave) CArrayFixFlat<TPoint>(4);
		CleanupStack::PushL(pointListPolyLine);
	    pointListPolyLine->AppendL(TPoint(10+i*70,280));
	    pointListPolyLine->AppendL(TPoint(70+i*70,320));
	    pointListPolyLine->AppendL(TPoint(10+i*70,320));
	    pointListPolyLine->AppendL(TPoint(70+i*70,280));

	    iGc->DrawPolyLine(*pointListPolyLine);
		    
	    CleanupStack::PopAndDestroy(pointListPolyLine);
	    		    
	    iGc->DrawLine(TPoint(10+i*70,370),TPoint(70+i*70,370));
		}

	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-PENBRUSH-0005

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
	High

@SYMTestStatus
	Implemented

@SYMTestCaseDesc
	Test shape drawing functions invalid pen and brush parameters handling.

@SYMTestActions		
	Draw a rectangle with pen style not from DirectGdi::TPenStyle enum.
	Draw a ellipse with brush style not from DirectGdi::TBrushStyle enum.
	Use negative pen size.

@SYMTestExpectedResults
	Shapes should be drawn with default brush and pen settings when invalid paramaters are used.
*/
void CTBrushAndPen::TestBrushAndPenInvalidParametersL()
	{	
	//there is no way in BitGDI to check if KErrArgument error occured,
	//so this is test only in DirectGDI
	if(!iUseDirectGdi)
		return;
		
	_LIT(KTestName, "PenBrush-InvalidParameters"); //test case name				
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();				
	
	TInt i;
	for(i = 0; i<6; i++)
		{
		switch (i)
			{
			case 0:
				{
				iGc->SetPenStyle(static_cast<DirectGdi::TPenStyle>(-1));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				break;
				}
			case 1:
				{
				iGc->SetPenStyle(static_cast<DirectGdi::TPenStyle>(10));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				iGc->SetPenSize(TSize(3,3));
				break;
				}
			case 2:
				{
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				iGc->SetPenSize(TSize(-1,-1));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				break;
				}
			case 3:
				{
				iGc->SetPenStyle(static_cast<DirectGdi::TPenStyle>(-1));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				iGc->SetPenSize(TSize(-3,-3));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				break;
				}
			case 4:
				{
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				iGc->SetPenSize(TSize(1,1));
				iGc->SetBrushStyle(static_cast<DirectGdi::TBrushStyle>(-1));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				break;
				}
			case 5:
				{
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				iGc->SetPenSize(TSize(1,1));
				iGc->SetBrushStyle(static_cast<DirectGdi::TBrushStyle>(20));
				CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
				break;
				}
			}

		TInt startx = 5+i*70;
		TInt endx = 65+i*70;
		iGc->DrawRect(TRect(startx,10,endx,30));
		TESTNOERROR(iGc->GetError());
		iGc->DrawRoundRect(TRect(startx,40,endx,60), TSize(10,10));
		TESTNOERROR(iGc->GetError());
		iGc->DrawPie(TRect(startx,70,endx,130), TPoint(30,0), TPoint(150,0));
		TESTNOERROR(iGc->GetError());
		iGc->DrawEllipse(TRect(startx,130,endx,190));
		TESTNOERROR(iGc->GetError());
		iGc->DrawArc(TRect(startx,190,endx,250),TPoint(endx,250),TPoint(startx,190));
		TESTNOERROR(iGc->GetError());
		iGc->DrawLine(TPoint(startx,270), TPoint(endx,270));
		TESTNOERROR(iGc->GetError());
		}
	
	TEST(KErrNone == WriteTargetOutput(iTestParams, KTestName()));
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTBrushAndPen::doTestStepPreambleL()
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
TVerdict CTBrushAndPen::doTestStepL()
	{		
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, KDrawBrushPenDefaultSize);
		
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
void CTBrushAndPen::RunTestsL()
	{	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-PENBRUSH-0001"));
	TestBrushColorAndStyleL(DirectGdi::EDrawModePEN);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-PENBRUSH-0005"));
	TestBrushAndPenInvalidParametersL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-PENBRUSH-0002"));
	TestBrushPatternOriginL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-PENBRUSH-0004"));
	TestPenStyleL();
	RecordTestResultL();
	if(!iRunningOomTests || iUseDirectGdi) // disable pen color and size tests for BitGdi Oom
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-PENBRUSH-0003"));
		TestPenColorSizeL(DirectGdi::EDrawModePEN);
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-PENBRUSH-0003"));
		TestPenColorSizeL(DirectGdi::EDrawModeWriteAlpha);
		RecordTestResultL();
		}
	}
