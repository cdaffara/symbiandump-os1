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

#include <gdi.h>
#include "talphablend.h"

const TInt KIterationsToTest = 100;
const TInt KDrawRectIterationsToTest = KIterationsToTest*5;	// More as faster simpler drawing than bitblits
const TInt KDrawVertLineIterationsToTest = KIterationsToTest*5;
const TInt KDrawTextIterationsToTest = KIterationsToTest*2;// More as drawing fewer pixels than blits.
const TInt KDrawVertTextIterationsToTest = KIterationsToTest;// Slow, less iterations.

    _LIT(KFontTypeface,"DejaVu Sans Condensed");
//
// We have two overloads of this test, the normal version that runs all tests except...
//

CAlphaBlendTestNormal::CAlphaBlendTestNormal()
	{
	SetTestStepName(KAlphaBlendTest);
	}

TVerdict CAlphaBlendTestNormal::doTestStepL()
	{	
	return(CAlphaBlendTest::doTestStepL(EFalse));
	}

/*
...the iType font variant, this needs the rom configured differently so can not be run with the other tests
to configure the rom for iType fonts you need to use:

..\iType\binaries\ityperast install

and to remove them and swich back to normal

 ..\iType\binaries\ityperast uninstall
*/

CAlphaBlendTestIType::CAlphaBlendTestIType()
	{
	SetTestStepName(KAlphaBlendTestIType);
	}

TVerdict CAlphaBlendTestIType::doTestStepL()
	{	
	return(CAlphaBlendTest::doTestStepL(ETrue));
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CAlphaBlendTest::doTestStepL(TBool aIType)
	{	
	RWsSession wsSession;
	User::LeaveIfError(wsSession.Connect());
	CleanupClosePushL(wsSession);

	//
	// Uncomment to debug WSERV redraw problems.
	// wsSession.SetAutoFlush(ETrue); 
	
	CWsScreenDevice* windowDevice = new (ELeave) CWsScreenDevice(wsSession);
	CleanupStack::PushL(windowDevice);
	User::LeaveIfError(windowDevice->Construct());
	CWindowGc* windowGc=NULL;	
	User::LeaveIfError(windowDevice->CreateContext(windowGc)); // create graphics context	
	CleanupStack::PushL(windowGc);	
	
	RWindowGroup group;	
	group  = RWindowGroup(wsSession);
	CleanupClosePushL(group);
	User::LeaveIfError(group.Construct(1, EFalse));
	
	RWindow background;
	background = RWindow(wsSession);
	CleanupClosePushL(background);
	User::LeaveIfError(background.Construct(group, 2));
	
	RWindow window;
	window = RWindow(wsSession);
	CleanupClosePushL(window);
	User::LeaveIfError(window.Construct(group, 3));

	TSize        windowSize = windowDevice->SizeInPixels();
	TDisplayMode windowMode = windowDevice->DisplayMode();

	background.Activate();
	background.Invalidate();

	ClearWindow(wsSession, background, windowGc, BLACK_SEMI_TRANSPARENT);	
	ClearWindow(wsSession, window, windowGc, BLACK_SEMI_TRANSPARENT);
	
	window.SetTransparencyAlphaChannel();	
	window.Activate();
	window.Invalidate();
	if (aIType)
		{
		if (CheckMonoTypeInstalledL())
			DoDrawBlendedTestsL(EBlendTestDrawTextIType,wsSession, KDrawVertTextIterationsToTest);
		else
			{
			INFO_PRINTF1(_L("Monotype fonts not installed, skipping test"));
			}
		}
	else
		{
/*
Test speed of blended draw rects on all modes supporting alpha blending
*/	
		DoDrawBlendedTestsL(EBlendTestDrawRect,wsSession, KDrawRectIterationsToTest);
/*
Test speed of blended draw vertical line calls on all modes supporting alpha blending
*/	
		DoDrawBlendedTestsL(EBlendTestVerticalLine,wsSession, KDrawVertLineIterationsToTest);
/*
Test speed of blended draw text calls on all modes supporting alpha blending
*/	
		DoDrawBlendedTestsL(EBlendTestDrawText,wsSession, KDrawTextIterationsToTest);
		DoDrawBlendedTestsL(EBlendTestDrawTextAntiAliased,wsSession, KDrawVertTextIterationsToTest);
		DoDrawBlendedTestsL(EBlendTestDrawVerticalText,wsSession, KDrawVertTextIterationsToTest);
/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0022

   @SYMTestCaseDesc
   Alphablend test BITBLT with EColor16MA source and EColor16MU destination.

   @SYMTestActions
   Compare the results over time

   @SYMTestExpectedResults
*/
		RDebug::Printf("Alpha Tests: EColor16MU, EColor16MA");
		RDebug::Printf("Alpha Blend");	
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0022"));
		DoAlphaBlendBitmapsBitmapTestL(EColor16MA, EColor16MU, wsSession, window, windowGc, KIterationsToTest);	
		RecordTestResultL();

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0023

   @SYMTestCaseDesc
   Test BITBLT with EColor16MA source and EColor16MU destination.

   @SYMTestActions
   Compare the results over time

   @SYMTestExpectedResults
*/
		RDebug::Printf("BitBlt Alpha");
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0023"));
		DoBitBltAlphaBitmapTestL(EColor16MA, EColor16MU, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0024

   @SYMTestCaseDesc
   Alpha blend test BITBLT with EColor16MA source and EColor16MA destination.

   @SYMTestActions
   Compare the results over time

   @SYMTestExpectedResults
*/
		RDebug::Printf("Alpha Tests: EColor16MA, EColor16MA");
		RDebug::Printf("Alpha Blend");	
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0024"));
		DoAlphaBlendBitmapsBitmapTestL(EColor16MA,EColor16MA, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0025

   @SYMTestCaseDesc
   Test BITBLT with EColor16MA source and EColor16MA destination.

   @SYMTestActions
   Compare the results over time

   @SYMTestExpectedResults
*/
		RDebug::Printf("BitBlt Alpha");
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0025"));
		DoBitBltAlphaBitmapTestL(EColor16MA, EColor16MA, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0063
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MAP source and EColor16MAP destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0063"));
		DoNormalBitBltL(EFalse, EColor16MAP,EColor16MAP, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0064
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MA source and EColor16MA destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0064"));
		DoNormalBitBltL(EFalse, EColor16MA,EColor16MA, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0065
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MAP source and EColor16MA destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0065"));
		DoNormalBitBltL(EFalse, EColor16MAP,EColor16MA, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0066
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MA source and EColor16MAP destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0066"));
		DoNormalBitBltL(EFalse, EColor16MA,EColor16MAP, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0067
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MU source and EColor16MU destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0067"));
		DoNormalBitBltL(EFalse, EColor16MU,EColor16MU, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0068
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MAP source and EColor16MU destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0068"));
		DoNormalBitBltL(EFalse, EColor16MAP,EColor16MU, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0069
	
	@SYMTestCaseDesc
	Test BITBLT with EColor16MU source and EColor16MAP destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0069"));
		DoNormalBitBltL(EFalse, EColor16MU,EColor16MAP, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0070
	
	@SYMTestCaseDesc
	Test BITBLT with EColor64K source and EColor16MU destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0070"));
		DoNormalBitBltL(ETrue, EColor64K,EColor16MU, wsSession, window, windowGc, KIterationsToTest);
		
		RecordTestResultL();
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0071
	
	@SYMTestCaseDesc
	Test DrawBitmap with different sizes (STRETCHED) with EColor16MAP source and EColor16MAP destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0071"));
		DoDrawBitmapL(EFalse, EColor16MAP, EColor16MAP, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0072
	
	@SYMTestCaseDesc
	Test DrawBitmap with different sizes (STRETCHED) with EColor16MA source and EColor16MA destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
	
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0072"));
		DoDrawBitmapL(EFalse, EColor16MA, EColor16MA, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0073
	
	@SYMTestCaseDesc
	Test DrawBitmap (using alpha mask) with different sizes (STRETCHED) with EColor16MU source and EColor16MU destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/	
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0073"));
		DoDrawBitmapL(ETrue, EColor16MU, EColor16MU, wsSession, window, windowGc, KIterationsToTest);
		RecordTestResultL();
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0057
	
	@SYMTestCaseDesc
	Test DrawBitmap (using alpha mask) with different sizes (STRETCHED) with EColor16MAP source and EColor16MAP destination.
	
	@SYMTestActions
	Compare the results over several iterations over time.
	
	@SYMTestExpectedResults
*/
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0057"));
		DoDrawBitmapL(ETrue, EColor16MAP, EColor16MAP, wsSession, window, windowGc, KIterationsToTest);	
		RecordTestResultL();
		}
	CleanupStack::PopAndDestroy(6, &wsSession);
	return TestStepResult();
	}

void CAlphaBlendTest::DoDrawBlendedTestsL(TBlendTestFunc aTestFunc, RWsSession& aSession, TInt aNumIterations)
	{
	TDisplayMode blendModes[]={EColor16MAP,EColor16MA,EColor16MU,EColor64K};
	const TInt KNumBlendTestModes=sizeof(blendModes)/sizeof(TDisplayMode);
	for(TInt modeIndex=0;modeIndex<KNumBlendTestModes;modeIndex++)
		{
		DoDrawBlendedTestL(aTestFunc, 0x40, blendModes[modeIndex], aSession, aNumIterations);
		DoDrawBlendedTestL(aTestFunc, 0xFF, blendModes[modeIndex], aSession, aNumIterations);
		DoDrawBlendedTestL(aTestFunc, 0, blendModes[modeIndex], aSession, aNumIterations);
		}
	}

void CAlphaBlendTest::DoDrawBlendedTestL(TBlendTestFunc aTestFunc, TInt aAlpha, TDisplayMode aDisplayMode, RWsSession& aSession, TInt aNumIterations)
	{
	SetScreenModeL(aDisplayMode);
	const TSize devSize = iScreenDevice->SizeInPixels();
	// For fair back to back tests of rotated mode we need graphics clipped to a square test area.
	// Although if we aren't going to be comparing rotated results against each other it might be better
	// to allow the tests to use the full available area.
	TSize testSize;
	testSize.iWidth=Min(devSize.iWidth,devSize.iHeight);
	testSize.iHeight=testSize.iWidth;

	TFontSpec testFontSpec; 
	if (aTestFunc==EBlendTestDrawTextAntiAliased || aTestFunc==EBlendTestDrawTextIType)
		{
		testFontSpec.iTypeface.iName=KFontTypeface;
		if (aTestFunc==EBlendTestDrawTextAntiAliased)
			{
			testFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
			}
		else
			{
			testFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
			testFontSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
			testFontSpec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
			}
		}
	testFontSpec.iHeight=12;
	// Set bold because sets more pixels, so tests blending code more
	testFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	_LIT(KTestBlendTextString,"BLENDTESTWITHBIGSOLIDCHARACTERS0123456789HAVEWERUNOUTOFSCREENYET?OKAFEWMORECHARACTERSJUSTTOMAKESURE");
	const TInt maxTextLen=KTestBlendTextString().Length();
//
	TBool orientations[4];
	iGc->OrientationsAvailable(orientations);
// Use for loop to test across multiple rotations, however the implementation hardly varies at the moment
// so very little point in testing the differences, if in future an optimised route is added for normal
// rotation this test would be worth switching back to looping upto EGraphicsOrientationRotated270.
//
//	const TInt KMaxOrientation=CFbsBitGc::EGraphicsOrientationRotated270;
	const TInt KMaxOrientation=CFbsBitGc::EGraphicsOrientationNormal;
	for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= KMaxOrientation; orient++)
		{
		if (orientations[orient])
			{
			iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
			User::After(100000);	// Let rotate sort itself out
			iGc->Reset();
			iGc->SetClippingRect(TRect(testSize));
			iGc->SetPenStyle(CGraphicsContext::ESolidPen);
			iGc->SetBrushColor(TRgb(0x80,0x80,0xFF));
			iGc->Clear();
			iGc->SetBrushColor(TRgb(10,128,240,aAlpha));
			iGc->SetPenColor(TRgb(240,128,10,aAlpha));
			iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			aSession.Flush();

			CFbsFont* font=NULL;
			TInt fontHeight=0;
			TInt testParam=0;
			TSize orientatedTestSize=testSize;
			TBuf <32> testName;	
			TGlyphBitmapType checkGlyphBitmapType=EDefaultGlyphBitmap;
			TGlyphBitmapType actualGlyphBitmapType=EDefaultGlyphBitmap;
			TPtrC textPtr;
			switch(aTestFunc)
				{
			case EBlendTestVerticalLine:
				_LIT(KDrawRectVerticalLine,"DrawRectVertLine");
				testName=KDrawRectVerticalLine;
				break;
			case EBlendTestDrawRect:
				_LIT(KDrawRectAlpha,"DrawRectAlpha");
				testName=KDrawRectAlpha;
				iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
				iGc->SetPenStyle(CGraphicsContext::ENullPen);
				break;
			case EBlendTestDrawTextIType:
				_LIT(KDrawTextIType,"DrawTextIT");
				testName=KDrawTextIType;
				testParam=testSize.iHeight;
				checkGlyphBitmapType=EFourColourBlendGlyphBitmap;
				goto drawTextTest;
			case EBlendTestDrawTextAntiAliased:
				_LIT(KDrawTextAA,"DrawTextAA");
				testName=KDrawTextAA;
				testParam=testSize.iHeight;
				checkGlyphBitmapType=EAntiAliasedGlyphBitmap;
				goto drawTextTest;
			case EBlendTestDrawVerticalText:
				_LIT(KDrawTextVert,"DrawTextVert");
				orientatedTestSize.SetSize(orientatedTestSize.iHeight,orientatedTestSize.iWidth);
				testName=KDrawTextVert;
				testParam=testSize.iWidth;
				checkGlyphBitmapType=EMonochromeGlyphBitmap;
				goto drawTextTest;
			case EBlendTestDrawText:
				_LIT(KDrawText,"DrawText");
				testParam=testSize.iHeight;
				testName=KDrawText;
				checkGlyphBitmapType=EMonochromeGlyphBitmap;
drawTextTest:	User::LeaveIfError(iScreenDevice->BitmapDevice().GetNearestFontInPixels((CFont* &)font,testFontSpec));
				fontHeight=font->HeightInPixels();
				iGc->UseFont(font);
				actualGlyphBitmapType=font->FontSpecInTwips().iFontStyle.BitmapType();
				TInt displayLen=Min(font->TextCount(KTestBlendTextString,orientatedTestSize.iWidth)+1,maxTextLen);
				textPtr.Set(KTestBlendTextString().Left(displayLen));
				iScreenDevice->BitmapDevice().ReleaseFont(font);
				break;
				}
			if (checkGlyphBitmapType!=EDefaultGlyphBitmap)
				{
				if (actualGlyphBitmapType!=checkGlyphBitmapType)
					{
					INFO_PRINTF3(_L("Failed to load correct glyph type font, wanted %d, got %d"),checkGlyphBitmapType, actualGlyphBitmapType);
					break;
					}
				}
			_LIT(KAppendAlphaTxt,"[A=0x%02x]");
			testName.AppendFormat(KAppendAlphaTxt,aAlpha);
			iProfiler->InitResults();	
			for(TInt iter=0; iter<aNumIterations; iter++)
				{
				switch(aTestFunc)
					{
				case EBlendTestVerticalLine:
					for(TInt xpos=0; xpos<testSize.iWidth; xpos++)
						iGc->DrawLine(TPoint(xpos,0),TPoint(xpos,testSize.iHeight));
					break;
				case EBlendTestDrawRect:
					iGc->DrawRect(TRect(testSize));
					break;
				case EBlendTestDrawText:
				case EBlendTestDrawVerticalText:
				case EBlendTestDrawTextAntiAliased:
				case EBlendTestDrawTextIType:
					{
					for(TInt pos=0;pos<testParam;pos+=fontHeight)
						{
						if (aTestFunc==EBlendTestDrawVerticalText)
							iGc->DrawTextVertical(textPtr,TPoint(pos,0),EFalse);
						else
							iGc->DrawText(textPtr,TPoint(0,pos));
						}
					break;
					}
					}
				iProfiler->MarkResultSetL();
				}
			INFO_PRINTF3(_L("%S %S"), &testName, &ColorModeName(aDisplayMode));
			iProfiler->ResultsAnalysis(testName, orient, aDisplayMode, aDisplayMode, aNumIterations);
			}
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	}

TBool CAlphaBlendTest::CheckMonoTypeInstalledL()
	{
	SetScreenModeL(EColor16MU);
//
	TFontSpec fontSpec;
	fontSpec.iTypeface.iName = KFontTypeface;
	fontSpec.iHeight = 20;
	fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fontSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
	fontSpec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
	CFont* font;
	User::LeaveIfError(iScreenDevice->BitmapDevice().GetNearestFontToDesignHeightInPixels((CFont*&)font,fontSpec));
	TBool monoTypeInstalled=font->FontSpecInTwips().iFontStyle.BitmapType()==EFourColourBlendGlyphBitmap;
	iScreenDevice->BitmapDevice().ReleaseFont(font);
	return(monoTypeInstalled);
	}

/**
Bitblt test

@param aSrcMode is the source display mode
@param aDstMode is the destination display mode
@param aSession is the windows server session
@param aWindow is a reference to the window
@param aGc is the graphics context of the window
@param aNumIterations is the number of iterations to run the test
*/		
void CAlphaBlendTest::DoBitBltAlphaBitmapTestL(TDisplayMode aSrcMode,TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations)
	{	
	const TSize bitmapSize = aWindow.Size();
	
	CFbsBitmap* bitmapTarget = CreateSoftwareBitmapLC(bitmapSize, aDstMode);
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmapTarget);
	CleanupStack::PushL(bitmapDevice);
	CFbsBitGc* bitmapGc = NULL;	
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapGc));	
	CleanupStack::PushL(bitmapGc);
	
	CFbsBitmap* source = CreateSoftwareBitmapLC(bitmapSize, aSrcMode);
	CFbsBitmap* sourceAlpha = CreateSoftwareBitmapLC(bitmapSize, EGray256);	// match size to src
	VerticalGradientAlphaL(sourceAlpha, TRgb(0x01010101), TRgb(0xfefefefe));
	VerticalGradientAlphaL(source, TRgb(0x00000000), TRgb(0xffffffff));

	TPoint point(0,0);
	
	bitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	bitmapGc->SetBrushColor(TRANSPARENT_BLACK);

	bitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	bitmapGc->Clear();
	bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	bitmapGc->BitBlt(point, source);

	aGc->Activate(aWindow);
	aGc->BitBlt(point, bitmapTarget);
	aGc->Deactivate();
	aSession.Flush();

	iProfiler->InitResults();		
	for(TInt i=0; i<aNumIterations; i++)
	{
		bitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		bitmapGc->Clear();
		bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		bitmapGc->BitBlt(point, source);
		iProfiler->MarkResultSetL();
	}
	
	iProfiler->ResultsAnalysis(_L("DoBitBltAlphaBitmapTestL"), 0, aSrcMode, aDstMode, aNumIterations);
	
	// copy up to screen for sanity check
	aGc->Activate(aWindow);
	aGc->BitBlt(TPoint(), bitmapTarget);
	aGc->Deactivate();
	CleanupStack::PopAndDestroy(5, bitmapTarget); //sourceAlpha, source, bitmapGc, bitmapDevice, bitmapTarget
	}


/**
Clears the window to a colour

@param aSession is the windows server session
@param aWindow is a reference to the window
@param aGc is the graphics context of the window
@param aColor is the colour to clear the window with
*/
void CAlphaBlendTest::ClearWindow(RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TRgb aColor)
{
	// clear so we can see bitmap version has completed
	aWindow.Invalidate();
	aWindow.BeginRedraw();
	aGc->Activate(aWindow);
	aGc->SetBrushColor(aColor);
	aGc->Clear();
	aGc->Deactivate();
	aWindow.EndRedraw();
	aSession.Flush();
}
		
/**
BitBlts a bitmap to the windows GC.

@param aSession is the windows server session
@param aWindow is a reference to the window
@param aGc is the graphics context of the window
@param aImage is the bitmap to bitblt
*/		
void CAlphaBlendTest::BitBlt(RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, CFbsBitmap& aImage)
	{
	aWindow.Invalidate();
	aWindow.BeginRedraw();
	aGc->Activate(aWindow);
	aGc->BitBlt(TPoint(0,0), &aImage);
	aGc->Deactivate();
	aWindow.EndRedraw();	
	aSession.Flush();	
	}

/**
Alpha blends two bitmaps together

@param aDisplayMode1 is the source display mode
@param aDisplayMode2 is the destination display mode
@param aSession is the windows server session
@param aWindow is a reference to the window
@param aGc is the graphics context of the window
@param aNumIterations is the number of iterations to run the test
*/	
void CAlphaBlendTest::DoAlphaBlendBitmapsBitmapTestL(TDisplayMode aSrcMode, TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations)
	{
	const TSize bitmapSize = aWindow.Size();	

	CFbsBitmap* bitmapTarget = CreateSoftwareBitmapLC(bitmapSize, aDstMode);
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmapTarget);
	CleanupStack::PushL(bitmapDevice);
	
	CFbsBitGc* bitmapGc=NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapGc));	
	CleanupStack::PushL(bitmapGc);
		
	CFbsBitmap* sourceUnder  = CreateSoftwareBitmapLC(bitmapSize, aDstMode);
	CFbsBitmap* sourceOver   = CreateSoftwareBitmapLC(bitmapSize, aSrcMode);
	CFbsBitmap* sourceAlpha  = CreateSoftwareBitmapLC(bitmapSize, EGray256);

	VerticalGradientAlphaL(sourceAlpha, TRgb(0x01010101), TRgb(0xfefefefe));
	VerticalGradientAlphaL(sourceUnder, TRgb(0xff000000), TRgb(0x00ffffff));
	VerticalGradientAlphaL(sourceOver, TRgb(0x00ffffff), TRgb(0xff000000));
	RDebug::Printf("DABBBT 2");

	TPoint point(0,0);
	TRect  rect(bitmapSize);
	
	bitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	bitmapGc->SetBrushColor(TRANSPARENT_BLACK);

	bitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	bitmapGc->Clear();
	bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	bitmapGc->AlphaBlendBitmaps(point, sourceUnder, sourceOver, rect, point, sourceAlpha, point);		

	aGc->Activate(aWindow);
	aGc->BitBlt(point, bitmapTarget);
	aGc->Deactivate();
	aSession.Flush();

	iProfiler->InitResults();
	// blend sourceUnder with sourceOver using alpha mask
	for(TInt i=0; i<aNumIterations; i++)
	{
		bitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		bitmapGc->Clear();
		bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		bitmapGc->AlphaBlendBitmaps(point, sourceUnder, sourceOver, rect, point, sourceAlpha, point);
		iProfiler->MarkResultSetL();
	}
	iProfiler->ResultsAnalysis(_L("DoAlphaBlendBitmapsBitmapTestL"), 0, aSrcMode, aDstMode, aNumIterations);
	
	// copy up to screen for sanity check
	BitBlt(aSession, aWindow, aGc, *bitmapTarget);	
	CleanupStack::PopAndDestroy(6, bitmapTarget); // sourceAlpha, sourceOver, sourceUnder, bitmapGc, bitmapDevice, bitmapTarget
	}	
	
/**
Draws a stretched bitmap with or without a mask.

@param aUseMask set to ETrue to use a alpha mask. Normally used for 16MU display modes that do not store the alpha.
@param aSrcMode is the source display mode
@param aDstMode is the destination display mode
@param aSession is the windows server session
@param aWindow is a reference to the window
@param aGc is the graphics context of the window
@param aNumIterations is the number of iterations to run the test
*/
void CAlphaBlendTest::DoDrawBitmapL(TBool aUseMask, TDisplayMode aSrcMode, TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations)
	{		
	const TSize bitmapSize = aWindow.Size();
	
	// Construct target bitmap.
	CFbsBitmap* bitmapTarget = CreateSoftwareBitmapLC(bitmapSize, aDstMode);
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmapTarget);
	CleanupStack::PushL(bitmapDevice);
	
	// Construct GC.
	CFbsBitGc* bitmapGc = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapGc));
	CleanupStack::PushL(bitmapGc);
	
	// Construct source bitmap.	
	TSize smallerSize(bitmapSize.iWidth/2,  bitmapSize.iHeight/2);
	CFbsBitmap* source = CreateSoftwareBitmapLC(smallerSize, aSrcMode);
	VerticalGradientAlphaL(source, TRgb(0x00000000), TRgb(0xffffffff));	
	CFbsBitmap* sourceAlpha = CreateSoftwareBitmapLC(smallerSize, EGray256);	// match size to src
	VerticalGradientAlphaL(sourceAlpha, TRgb(0x01010101), TRgb(0xfefefefe));
		
	bitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	bitmapGc->SetBrushColor(TRANSPARENT_BLACK);
	bitmapGc->Clear();
	bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	aGc->Activate(aWindow);
	TPoint point(0,0);
	bitmapGc->BitBlt(point, bitmapTarget);
	aGc->Deactivate();
	aSession.Flush();

	TBuf <20> testName;
	if (!aUseMask)
		{
		testName=_L("DrawBitmap");
		iProfiler->InitResults();
		for(int i=0; i<aNumIterations; i++)
			{
			bitmapGc->DrawBitmap(TRect(point, bitmapSize), source);
			iProfiler->MarkResultSetL();
			}
		}
	else
		{
		testName=_L("DrawBitmapMasked");
		iProfiler->InitResults();
		for(int i=0; i<aNumIterations; i++)
			{
			bitmapGc->DrawBitmapMasked(TRect(point, bitmapSize), source,TRect(point, smallerSize), sourceAlpha, EFalse);
			iProfiler->MarkResultSetL();
			}
		}
	INFO_PRINTF4(_L("%S(Stretched) with src = %S, dst = %S"), &testName, &ColorModeName(aSrcMode), &ColorModeName(aDstMode));
	iProfiler->ResultsAnalysis(testName, 0, aSrcMode, aDstMode, aNumIterations);
	// copy up to screen for sanity check
	BitBlt(aSession, aWindow, aGc, *bitmapTarget);
	CleanupStack::PopAndDestroy(5, bitmapTarget);
	}

/**
Performs a BitBlt with or without a mask

@param aUseMask set to ETrue to use a alpha mask. Normally used for 16MU display modes that do not store the alpha.
@param aSrcMode is the source display mode
@param aDstMode is the destination display mode
@param aSession is the windows server session
@param aWindow is a reference to the window
@param aGc is the graphics context of the window
@param aNumIterations is the number of iterations to run the test
*/
void CAlphaBlendTest::DoNormalBitBltL(TBool aUseMask, TDisplayMode aSrcMode, TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations)
	{	
	const TSize bitmapSize = aWindow.Size();
	
	// Construct target bitmap
	CFbsBitmap* bitmapTarget = CreateSoftwareBitmapLC(bitmapSize, aDstMode);
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmapTarget);
	CleanupStack::PushL(bitmapDevice);
	
	// Construct GC
	CFbsBitGc* bitmapGc = NULL;	
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapGc));
	CleanupStack::PushL(bitmapGc);
	
	// Construct source bitmap
	CFbsBitmap* source = CreateSoftwareBitmapLC(bitmapSize, aSrcMode);
	VerticalGradientAlphaL(source, TRgb(0x00000000), TRgb(0xffffffff));	
	CFbsBitmap* sourceAlpha = CreateSoftwareBitmapLC(bitmapSize, EGray256);	// match size to src
	VerticalGradientAlphaL(sourceAlpha, TRgb(0x01010101), TRgb(0xfefefefe));
		
	bitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	bitmapGc->SetBrushColor(TRANSPARENT_BLACK);
	bitmapGc->Clear();
	bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	TPoint point(0,0);
	TRect rect(bitmapSize);
	if (aUseMask)		
		bitmapGc->AlphaBlendBitmaps(point, source, rect, sourceAlpha, point);		
	else
		bitmapGc->BitBlt(point, source);	// BitBlt source to target bitmap		
	
	aGc->Activate(aWindow);
	aGc->BitBlt(point, bitmapTarget);	// BitBlt bitmapTarget to screen
	aGc->Deactivate();
	aSession.Flush();	
	
	TBuf <20> testName;	
	if (aUseMask)
		{
		testName=_L("AlphaBlendBitmaps");
		// blend sourceUnder with sourceOver using alpha mask
		iProfiler->InitResults();		
		for(TInt i=0; i<aNumIterations; i++)
			{
			bitmapGc->Clear();
			bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			bitmapGc->AlphaBlendBitmaps(point, source, rect, sourceAlpha, point);
			iProfiler->MarkResultSetL();
			}
		}
	else
		{
		testName=_L("Normal BitBlt");
		iProfiler->InitResults();
		for(TInt i=0; i<aNumIterations; i++)
			{
			bitmapGc->BitBlt(point, source);
			iProfiler->MarkResultSetL();
			}
		}

	INFO_PRINTF4(_L("%S with src = %S, dst = %S"), &testName, &ColorModeName(aSrcMode), &ColorModeName(aDstMode));
	iProfiler->ResultsAnalysis(testName, 0, aSrcMode, aDstMode, aNumIterations);
	// copy up to screen for sanity check
	BitBlt(aSession, aWindow, aGc, *bitmapTarget);
	CleanupStack::PopAndDestroy(5, bitmapTarget);
	}
