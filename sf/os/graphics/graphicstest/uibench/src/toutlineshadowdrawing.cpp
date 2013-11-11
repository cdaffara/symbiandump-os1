// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "toutlineshadowdrawing.h"
#include <hal.h>
// When enabled allows testing with a variable windows size rather than fixed size
#define _TESTWITHVARIABLEWINDOWSIZE

_LIT(KTextPhrase, "$%() ShAdOW OLine 1234");
_LIT(KFontTypeface, "fontTypeFace");

const TInt KIterationsToTest = 50; 		// Number of iterations to run tests
const TInt KHeight = 60;
const TInt KTempLength = 60; // Used for temp buffer in side function
const TInt KLengthOfTestCase = 256;
_LIT(KTestCaseFormat, "%S_%S_ORIENT_%d");

/**
Auxilary function called to generate test case name based on input parameters
@param aTestDesc holds the test case description (eg. GET_FONT_MAX_HEIGHT)
@param aIsShadowOn holds bool value of shadow effects (eg. ETrue or EFalse)
@param aIsOutlineOn holds bool value of outline effects (eg. ETrue or EFalse)
@param aOrientation holds int value of orientation (eg. 0, 1, 2)
@param aPenColor holds pen color used to extract alpha value.
*/
HBufC* CTOutlineShadowDrawing::GenerateTestCaseLC(const TDesC& aTestDesc, TBool aIsShadowOn, TBool aIsOutlineOn, TInt aOrientation, const TRgb& aPenColor)
	{
	TBuf<KLengthOfTestCase> tempBuffer;
	TBuf<KTempLength> temp;
	if(aIsShadowOn && aIsOutlineOn)
		{
		temp.Format(_L("WITH BOTH OUTLINE & SHADOW ON_"));
		}
	else if(aIsOutlineOn)
		{
		temp.Format(_L("WITH OUTLINE ONLY ON_"));
		}
	else if(aIsShadowOn)
		{
		temp.Format(_L("WITH SHADOW ONLY ON_"));
		}
	else 
		{
		temp.Format(_L("WITH BOTH OUTLINE & SHADOW OFF_"));
		}
		temp.AppendNum(aPenColor.Alpha());
	
	tempBuffer.Format(KTestCaseFormat, &aTestDesc, &temp, aOrientation);
	return tempBuffer.AllocLC();
	}

/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0147

	@SYMPREQ PREQ1543

	@SYMTestCaseDesc Tests how long it takes to run GetNearestFontToDesignHeightInTwips API 
	and calculate time with different screen modes with outline and shadow effects.
	Also with Screen Rotation

	@param aFontFace is the fontface which is used to set to fontspec
	@param aRotation is the rotation index which is passed to ResultAnalysis
	@param aIsShadowOn holds shadow status (True or False)
	@param aIsOutlineOn holds outline status (True or False)

	@SYMTestPriority High

	@SYMTestStatus Implemented

	@SYMTestActions Compare the results over time, GetNearestFontToDesignHeightInPixels() code.
	Time Calculated with different Screen Modes and with Screen Rotation.
	API Calls:
	CFbsBitGc::GetNearestFontToDesignHeightInPixels()
	TFontStyle::SetEffects()

	@SYMTestExpectedResults Test should pass and display total test time and time per display mode
*/
void CTOutlineShadowDrawing::DoGetAPIsOutlineandShadowFontsL(const TDesC& aFontFace, TInt aRotation, TBool aIsShadowOn, TBool aIsOutlineOn)
	{
	TFontSpec fsp;
	iGc->Clear();
	iGc->Reset();
	fsp.iFontStyle.SetEffects(FontEffect::EDropShadow, aIsShadowOn);
	fsp.iFontStyle.SetEffects(FontEffect::EOutline, aIsOutlineOn);
	fsp.iTypeface.iName = aFontFace;
	fsp.iHeight = KHeight;
	TRgb outlinePenColor = iGc->PenColor();
	
	_LIT(KTestName1, "GET_FONT_MAX_HEIGHT");
	HBufC* testCase1 = GenerateTestCaseLC(KTestName1, aIsShadowOn, aIsOutlineOn, aRotation, outlinePenColor);
	CFbsFont* font = NULL;
	iProfiler->InitResults();
	TInt iteration = 0;
	for(iteration=0; iteration<=KIterationsToTest; iteration++)
		{
		TEST(iDev->GetNearestFontToMaxHeightInPixels((CFont*&)font,fsp,fsp.iHeight) == KErrNone);
		iProfiler->MarkResultSetL();
		iDev->ReleaseFont(font);
		font = NULL;
		}
	iProfiler->ResultsAnalysis(*testCase1, aRotation, 0, iDev->DisplayMode(), KIterationsToTest);
	CleanupStack::PopAndDestroy(testCase1);

	_LIT(KTestName2, "GET_FONT");
	HBufC* testCase2 = GenerateTestCaseLC(KTestName2, aIsShadowOn, aIsOutlineOn, aRotation, outlinePenColor);
	// Tests performance for GetNearestFontInPixels API
	iProfiler->InitResults();
	for(iteration=0; iteration<=KIterationsToTest; iteration++)
		{
		TEST(iDev->GetNearestFontInPixels((CFont*&)font,fsp) == KErrNone);
		iProfiler->MarkResultSetL();
		iDev->ReleaseFont(font);
		font = NULL;
		}
	iProfiler->ResultsAnalysis(*testCase2, aRotation, 0, iDev->DisplayMode(), KIterationsToTest);
	CleanupStack::PopAndDestroy(testCase2);

	_LIT(KTestName3, "GET_FONT_DESIGN_HEIGHT");
	HBufC* testCase3 = GenerateTestCaseLC(KTestName3, aIsShadowOn, aIsOutlineOn, aRotation, outlinePenColor);
	// Tests performance for GetNearestFontToDesignHeightInPixels API
	iProfiler->InitResults();
	for(iteration=0; iteration<=KIterationsToTest; iteration++)
		{
		TEST(iDev->GetNearestFontToDesignHeightInPixels((CFont*&)font,fsp) == KErrNone);
		iProfiler->MarkResultSetL();
		iDev->ReleaseFont(font);
		font = NULL;
		}
	iProfiler->ResultsAnalysis(*testCase3, aRotation, 0, iDev->DisplayMode(), KIterationsToTest);
	CleanupStack::PopAndDestroy(testCase3);
	}

/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0148

	@SYMPREQ PREQ1543

	@SYMTestCaseDesc Tests how long it takes to run DrawText API
	and calculate time with different screen modes with and outline and shadow effects

	@param aFontFace is the fontface which is used to set to fontspec
	@param aRotation is the rotation index which is passed to ResultAnalysis
	@param aBitmapType is the bitmap type (Here it is AntiAliased)
	@param aOutlinePenColor is the pen color
	@param aIsShadowOn holds shadow status (True or False)
	@param aIsOutlineOn holds outline status (True or False)
	@param aDispModeIndex is the display mode index used for displaying in log

	@SYMTestPriority High 

	@SYMTestStatus Implemented

	@SYMTestActions Compare the results over time, DrawText with and without Outline & Shadow and
	Time calculated with different Screen Modes and with Screen Rotation DrawText().
	API Calls:
	CFbsBitGc::DrawText()
	TFontStyle::SetEffects()

	@SYMTestExpectedResults Test should pass and display total test time and time per display mode
*/	
void CTOutlineShadowDrawing::DoDrawTextOutlineShadowL(const TDesC& aFontFace, TInt aRotation, const TRgb& aOutlinePenColor, TBool aIsShadowOn, TBool aIsOutlineOn, TInt aDispModeIndex)
	{
	TFontSpec fsp;
	iGc->Clear();
	iGc->Reset();
	fsp.iTypeface.iName = aFontFace;
	fsp.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);

	_LIT(KTestName1, "DRAWTEXT_DISPLAYINDEX_%d");

	TBuf<KTempLength> temp;
	fsp.iHeight = KHeight;
	fsp.iFontStyle.SetEffects(FontEffect::EDropShadow, aIsShadowOn);
	fsp.iFontStyle.SetEffects(FontEffect::EOutline, aIsOutlineOn);
	iGc->SetPenColor(aOutlinePenColor);
	CFont* font = NULL;
	User::LeaveIfError(iDev->GetNearestFontToDesignHeightInPixels((CFont*&)font,fsp));
	iGc->UseFont(font);
	
	temp.Format(KTestName1, aDispModeIndex);
	HBufC* testCase1 = GenerateTestCaseLC(temp, aIsShadowOn, aIsOutlineOn, aRotation, aOutlinePenColor);
	iProfiler->InitResults();
	for(TInt i = 0; i <= KIterationsToTest; i++)
		{
		iGc->DrawText(KTextPhrase,TPoint(10,100));
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(*testCase1, aRotation, 0, iDev->DisplayMode(), KIterationsToTest);
	CleanupStack::PopAndDestroy(testCase1);
	iGc->DiscardFont();
	iDev->ReleaseFont(font);
	}

/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0149
	
	@SYMPREQ PREQ1543

	@SYMTestCaseDesc Tests how long it takes to run DrawTextVertical API
	and calculate time with different screen modes with and outline and shadow effects

	@param aFontFace is the fontface which is used to set to fontspec
	@param aRotation is the rotation index which is passed to ResultAnalysis
	@param aBitmapType is the bitmap type (Here it is AntiAliased)
	@param aOutlinePenColor is the pen color
	@param aIsShadowOn holds shadow status (True or False)
	@param aIsOutlineOn holds outline status (True or False)
	@param aDispModeIndex is the display mode index used for displaying in log

	@SYMTestPriority High 

	@SYMTestStatus Implemented

	@SYMTestActions Compare the results over time, with DrawTextVertical with and without Outline & Shadow and
	Time calculated with different Screen Modes and with Screen Rotation with DrawTextVertical
	API Calls:
	CFbsBitGc::DrawTextVertical()
	TFontStyle::SetEffects()

	@SYMTestExpectedResults Test should pass and display total test time and time per display mode
*/
void CTOutlineShadowDrawing::DoDrawVerticalTextOutlineShadowL(const TDesC& aFontFace, TInt aRotation, const TRgb& aOutlinePenColor, TBool aIsShadowOn, TBool aIsOutlineOn, TInt aDispModeIndex)
	{
	TFontSpec fsp;
	CFont* font = NULL;
	iGc->Clear();
	iGc->Reset();
	TInt alpha = aOutlinePenColor.Alpha();
	fsp.iTypeface.iName = aFontFace;
	fsp.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);

	_LIT(KTestName1, "DRAWTEXTVERTICAL_DISPLAYINDEX_%d");

	TBuf<KTempLength> temp;	
	temp.Format(KTestName1, aDispModeIndex);
	HBufC* testCase1 = GenerateTestCaseLC(temp, aIsShadowOn, aIsOutlineOn, aRotation, aOutlinePenColor);
	fsp.iHeight = KHeight;
	fsp.iFontStyle.SetEffects(FontEffect::EDropShadow, aIsShadowOn);
	fsp.iFontStyle.SetEffects(FontEffect::EOutline, aIsOutlineOn);

	iGc->SetPenColor(aOutlinePenColor);
	User::LeaveIfError(iDev->GetNearestFontToDesignHeightInPixels((CFont*&)font,fsp));
	iGc->UseFont(font);
	iProfiler->InitResults();
	for(TInt i=0; i<=KIterationsToTest; i++)
		{
		iGc->DrawTextVertical(KTextPhrase,TPoint(10,10),EFalse);
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(*testCase1, aRotation, 0, iDev->DisplayMode(), KIterationsToTest);
	CleanupStack::PopAndDestroy(testCase1);
	iGc->DiscardFont();
	iDev->ReleaseFont(font);
	}

CTOutlineShadowDrawing::~CTOutlineShadowDrawing()
	{
	}

CTOutlineShadowDrawing::CTOutlineShadowDrawing()
	{
	SetTestStepName(KTOutlineShadowDrawing);
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

Takes Font Face from uibench.ini file and pass that value to functions.
It also Rotates the screen before calling test functions.

@return TVerdict code
*/	
TVerdict CTOutlineShadowDrawing::doTestStepL()
	{	
	TPtrC fontface;

	TBool returnValue1 = GetStringFromConfig(ConfigSection(), KFontTypeface, fontface);
	if(returnValue1)
		{
		INFO_PRINTF1(_L("OutlineShadowDrawing"));
		iGc->Reset();
		TBool orientation[4];
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0147"));
		DoGetAPIsOutlineandShadowFontsL(fontface, 0, ETrue, ETrue);
		DoGetAPIsOutlineandShadowFontsL(fontface, 0, ETrue, EFalse);
		DoGetAPIsOutlineandShadowFontsL(fontface, 0, EFalse, ETrue);
		DoGetAPIsOutlineandShadowFontsL(fontface, 0, EFalse, EFalse);
		RecordTestResultL();

		for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
			{
			SetScreenModeL(KValidDisplayModes[dispModeIndex]);
			iDev = &iScreenDevice->BitmapDevice();
			iGc->OrientationsAvailable(orientation);
			for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
				{
				iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
				// Draw Text - without transperency
				SetTestStepID(_L("GRAPHICS-UI-BENCH-0148"));
				DoDrawTextOutlineShadowL(fontface, orient, KRgbGreen, ETrue, ETrue, dispModeIndex);
				DoDrawTextOutlineShadowL(fontface, orient, KRgbGreen, EFalse, ETrue, dispModeIndex);
				DoDrawTextOutlineShadowL(fontface, orient, KRgbGreen, ETrue, EFalse, dispModeIndex);
				DoDrawTextOutlineShadowL(fontface, orient, KRgbGreen, EFalse, EFalse, dispModeIndex);
				RecordTestResultL();
				// Draw Vertical Text - without transperency
				SetTestStepID(_L("GRAPHICS-UI-BENCH-0149"));
				DoDrawVerticalTextOutlineShadowL(fontface, orient, KRgbRed, ETrue, ETrue, dispModeIndex);
				DoDrawVerticalTextOutlineShadowL(fontface, orient, KRgbRed, ETrue, EFalse, dispModeIndex);
				DoDrawVerticalTextOutlineShadowL(fontface, orient, KRgbRed, EFalse, ETrue, dispModeIndex);
				DoDrawVerticalTextOutlineShadowL(fontface, orient, KRgbRed, EFalse, EFalse, dispModeIndex);
				RecordTestResultL();
				// Transperency tests - Draw Text
				SetTestStepID(_L("GRAPHICS-UI-BENCH-0148"));
				DoDrawTextOutlineShadowL(fontface, orient, TRgb(128, 128, 128, 120), ETrue, ETrue, dispModeIndex);
				DoDrawTextOutlineShadowL(fontface, orient, TRgb(128, 128, 128, 120), EFalse, ETrue, dispModeIndex);
				DoDrawTextOutlineShadowL(fontface, orient, TRgb(128, 128, 128, 120), ETrue, EFalse, dispModeIndex);
				DoDrawTextOutlineShadowL(fontface, orient, TRgb(128, 128, 128, 120), EFalse, EFalse, dispModeIndex);
				RecordTestResultL();
				// Transperency tests - Draw Vertical Text
				SetTestStepID(_L("GRAPHICS-UI-BENCH-0149"));
				DoDrawVerticalTextOutlineShadowL(fontface, orient, TRgb(128, 128, 0, 20), ETrue, ETrue, dispModeIndex);
				DoDrawVerticalTextOutlineShadowL(fontface, orient, TRgb(128, 128, 0, 20), ETrue, EFalse, dispModeIndex);
				DoDrawVerticalTextOutlineShadowL(fontface, orient, TRgb(128, 128, 0, 20), EFalse, ETrue, dispModeIndex);
				DoDrawVerticalTextOutlineShadowL(fontface, orient, TRgb(128, 128, 0, 20), EFalse, EFalse, dispModeIndex);					
				RecordTestResultL();
				}
			iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
			}
		}
	else
		{
		INFO_PRINTF1(_L("ERROR WHILE READING CONFIG FILE"));
		User::Leave(returnValue1);
		}
	return TestStepResult();
	}

/**
Override of base class pure virtual

@return - TVerdict code
*/
TVerdict CTOutlineShadowDrawing::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	SetScreenModeL(EColor16MU);
	iDev = &iScreenDevice->BitmapDevice();
	return TestStepResult();
	}
