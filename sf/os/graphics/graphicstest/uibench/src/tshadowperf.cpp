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
 
#include "tshadowperf.h"

const TInt KIterationsToTest = 250; 		// Number of iterations to run tests

//The 16 bit mbm file
_LIT(Kmbm16bpp, "z:\\system\\data\\uibench_16bit.mbm");

CTShadowPerf::~CTShadowPerf()
	{
	}

CTShadowPerf::CTShadowPerf()
	{
	SetTestStepName(KTShadowPerfName);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTShadowPerf::doTestStepPreambleL()
	{	
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
		
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTShadowPerf::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0046"));
	ShadowAreaL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0047"));
	FadeAreaL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0077"));
	FadedShadowTestL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0078"));
	AntialiasedTextTestL();
	RecordTestResultL();

	return TestStepResult();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0046

@SYMTestCaseDesc
Tests how long it takes to calculate and display a shadow area with different screen modes

@SYMTestActions
Compare the results over time, and before and after changes to CFbsBitGc::ShadowArea() code.
In particular this function calls ShadowArea() from derived classes of CFbsDrawDevice 
e.g. CDrawThirtyTwoBppBitmapCommon with TShadowMode::EShadow

@SYMTestExpectedResults
Test should pass and display total test time and time per display mode
*/
void CTShadowPerf::ShadowAreaL()
	{
	INFO_PRINTF1(_L("CTShadowPerf::ShadowArea"));
	_LIT(KTestName, "ShadowArea");

	for (TInt dispModeIndex = KNumValidDisplayModes-1; dispModeIndex >= 0; --dispModeIndex)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);			
		AreaTestL(EShadow, KTestName, dispModeIndex);		
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0047

@SYMTestCaseDesc
Tests how long it takes to calculate and display a faded area with different screen modes

@SYMTestActions
Compare the results over time, and before and after changes to CFbsBitGc::FadeArea() code.
In particular this function calls FadeArea() from derived classes of CFbsDrawDevice 
e.g. CDrawThirtyTwoBppBitmapCommon with TShadowMode::EFade

@SYMTestExpectedResults
Test should pass and display total test time and time per display mode
*/
void CTShadowPerf::FadeAreaL()
	{
	INFO_PRINTF1(_L("CTShadowPerf::FadeArea"));
	_LIT(KTestName, "FadeArea");

	for (TInt dispModeIndex = KNumValidDisplayModes-1; dispModeIndex >= 0; --dispModeIndex)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);			
		AreaTestL(EFade, KTestName, dispModeIndex);		
		}
	}

/**
Helper function to test the performance of shadowing and fading

@param aShadowTest the shadow test to carry out
@param aTestName the name of the test
@param aDisplayModeIndex the display mode index
*/
void CTShadowPerf::AreaTestL(TShadowTest aShadowTest, const TDesC& aTestName, TInt aDisplayModeIndex)
	{
	const TRect shadowRect(25,25,50,50);
	RRegion shadowRegion(shadowRect);

	iGc->Clear();
	switch (aShadowTest)
		{
		case EShadow:
		    iProfiler->InitResults();
			for (TInt count=KIterationsToTest; count>=0; --count)
				{
				iGc->ShadowArea(&shadowRegion);
				iProfiler->MarkResultSetL();
				}	
			break;
		
		case EFade:
		    iProfiler->InitResults();
			for (TInt count=KIterationsToTest; count>=0; --count)
				{
				iGc->FadeArea(&shadowRegion);
				iProfiler->MarkResultSetL();
				}	
			break;					
		}
	iProfiler->ResultsAnalysis(aTestName, aShadowTest, KValidDisplayModes[aDisplayModeIndex], KValidDisplayModes[aDisplayModeIndex], KIterationsToTest);
	shadowRegion.Close();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0077

@SYMTestCaseDesc
Tests the performance of CDrawSixteenBppBitmap::WriteRgbAlphaLine function.

@SYMTestActions
Compare the results over time, and before and after changes to 
CDrawSixteenBppBitmap::WriteRgbAlphaLine and  CDrawSixteenBppBitmap::Shadow code.

@SYMTestExpectedResults
Test should pass and display total test time.
*/
void CTShadowPerf::FadedShadowTestL()
	{
	_LIT(KTestName, "FadedShadow");
	SetScreenModeL(EColor64K);
	CFbsBitmap* bitmap16bit = LoadBitmapL(Kmbm16bpp, 0);
	CleanupStack::PushL(bitmap16bit);
	CFbsBitmap* alpha8bit = CopyIntoNewBitmapL(bitmap16bit, EGray256);
	CleanupStack::PushL(alpha8bit);
	TRect cropTo(0,0,200,200);
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iGc->Clear();
		iGc->SetFaded(ETrue);
		iProfiler->StartTimer();	
		iGc->BitBltMasked(TPoint(0,0), bitmap16bit, cropTo, alpha8bit, EFalse);
		iProfiler->MarkResultSetL();
		}	
	iGc->SetFaded(EFalse);
	iGc->Clear();
	CleanupStack::PopAndDestroy(2, bitmap16bit);
	iProfiler->ResultsAnalysis(KTestName, 0, EColor64K, EColor64K, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0078

@SYMTestCaseDesc
Tests the performance of CDrawSixteenBppBitmap::WriteRgbAlphaMulti function.

@SYMTestActions
Compare the results over time, and before and after changes to 
CDrawSixteenBppBitmap::WriteRgbAlphaMulti and CDrawSixteenBppBitmap:: Shadow code.

@SYMTestExpectedResults
Test should pass and display total test time
*/
void CTShadowPerf::AntialiasedTextTestL()
	{
	SetScreenModeL(EColor64K);
	if(iScreenDevice->isScreenDevice())
		{
		_LIT(KTestName, "AntialiasedText");
		_LIT(KText,"Drawing Antialiased Text");
		CFont* font = NULL;
		CFbsScreenDevice* screenDevice = reinterpret_cast<CFbsScreenDevice*>(&(iScreenDevice->BitmapDevice()));
		TInt typefaces = screenDevice->NumTypefaces();
		iProfiler->InitResults();
		for (TInt count=KIterationsToTest; count>=0; --count)
			{
			for (TInt i = 0; i < typefaces; i++)
				{
				TTypefaceSupport typeface_support;
				screenDevice->TypefaceSupport(typeface_support,i);
				if (typeface_support.iIsScalable)
					{
					iGc->Clear();
					iGc->SetFaded(ETrue);
					TPoint pos(100,30);
					TFontSpec fs;
					fs.iTypeface = typeface_support.iTypeface;
					fs.iHeight = 14;
					fs.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
			 		User::LeaveIfError(screenDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font,fs));
					iGc->UseFont(font);
					iProfiler->StartTimer();
					iGc->DrawText(KText,pos);
					iProfiler->MarkResultSetL();
					screenDevice->ReleaseFont(font);
					iGc->DiscardFont();
					}
				}
			}
		iProfiler->ResultsAnalysis(KTestName, 0, EColor64K, EColor64K, KIterationsToTest);
		}
	}
