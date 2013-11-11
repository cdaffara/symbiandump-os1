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
// The fix enables the fading effect with alpha-blending, which was not applied bofore.
// The test will load a bitmap and two different masks: on/off and alpha-blend.
// The bitmap will be masked with these masks and displayed before and after
// setting the fading effect.
// All different colour modes are being tested for both mask types.
// The test will check the colour of a specific pixel in the scene before and after the
// fading. The higher values in the After circle means that it has been highlighted.
// The result will be printed in wstest log file.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TFADINGBITMAP.H"

//===================================================
// CBaseWin Declaration
//===================================================

CBaseWin::CBaseWin(): CTWin()
	{
	}

CBaseWin::~CBaseWin()
	{
	delete iTempBitmap;
	delete iMaskGray256;
	delete iMaskGray2;
	delete iTempMask;
	delete iBitmap;
	}

void CBaseWin::ConstructWinL(TPoint aPos, TSize aSize, TBool aVisible)
	{	
	/*** Setting up the window ***/
	iSize = aSize;
	SetUpL(aPos, aSize, TheClient->iGroup, *TheClient->iGc, aVisible);
	Win()->SetBackgroundColor(TRgb(20, 80, 20));	// dark green background
	BaseWin()->SetRequiredDisplayMode(EColor64K);

	/*** 24 bit bitmap ***/
	// the original 24b bitmap to mask
	iTempBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmap->Load(_L("Z:\\WSTEST\\WSAUTOTEST.MBM"), EMbmWsautotestCircles24b));
	iBitmap = new (ELeave) CFbsBitmap();

	/*** on/off mask ***/
	iMaskGray2 = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iMaskGray2->Load(_L("Z:\\WSTEST\\WSAUTOTEST.MBM"), EMbmWsautotestCircles_mask2b));

	/*** alpha-blend mask ***/
	// holds the 24bit copy of the alpha blend mask which will be 
	// copied into the proper Gray256 mask, iMaskGray256.
	iTempMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempMask->Load(_L("Z:\\WSTEST\\WSAUTOTEST.MBM"), EMbmWsautotestCircles_mask256));	
	// alpha blend mask; copying its data from iTempMask
	iMaskGray256 = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iMaskGray256->Create(iTempBitmap->SizeInPixels(),EGray256));
	CFbsBitmapDevice *dev = CFbsBitmapDevice::NewL(iMaskGray256);
	CleanupStack::PushL(dev);
	CFbsBitGc *gc;
	User::LeaveIfError(dev->CreateContext(gc));
	// performing the copying here
	gc->BitBlt(TPoint(0,0), iTempMask);
	// cleaning up
	CleanupStack::Pop();
	delete gc;
	gc = NULL;	
	delete dev;
	dev = NULL;	
	}

void CBaseWin::Draw()
	{
	iGc->Clear();

	// Font intialization
	CFont* myFont;    	
	_LIT(KMyFontName,"Swiss");
	TFontSpec myFontSpec = TFontSpec(KMyFontName,16); // to get smallest Swiss font
	TFontStyle style = TFontStyle (EPostureUpright, EStrokeWeightBold,   EPrintPosNormal);
	myFontSpec.iFontStyle = style;
	User::LeaveIfError(TheClient->iScreen->GetNearestFontInPixels(myFont, myFontSpec));
	iGc->UseFont(myFont);
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetPenColor(TRgb(255, 255, 255));
	
	// drawing text
	iGc->DrawText(_L("Fading = OFF"), TPoint(130,15));
	iGc->DrawText(_L("Fading = ON"),  TPoint(275,15));
	iGc->DrawText(_L("Alpha blend"), TPoint(15,90));
	iGc->DrawText(_L("on/off mask"),  TPoint(15,190));
	TBuf <30> displayMode(_L("Display Mode = "));
	displayMode.Append(iMode);	
	iGc->DrawText(displayMode, TPoint(385,100));		
	
	/*** drawing bitmap with its on/off mask and alpha-blending 
		 before and after fading ***/
	iGc->BitBltMasked(TPoint(140,25), iBitmap, 
						   TRect(0,0,100,100), iMaskGray256, EFalse);	
	// Save the pixel colour of a pixel on the outer ring of the circle 
	// before fading enabled.
	TheClient->iScreen->GetPixel(iNonFadedPixel, TPoint(190,30));	

	iGc->SetFaded(ETrue);
	iGc->BitBltMasked(TPoint(270,25), iBitmap, 
						   TRect(0,0,100,100), iMaskGray256, EFalse);
	// Save the pixel colour of a pixel on the outer ring of the circle 
	// after fading enabled.
	TheClient->iScreen->GetPixel(iFadedPixel, TPoint(320,30));	

	iGc->SetFaded(EFalse);
	
	iGc->BitBltMasked(TPoint(140,125), iBitmap, 
						   TRect(0,0,100,100), iMaskGray2, EFalse);
	iGc->SetFaded(ETrue);
	iGc->BitBltMasked(TPoint(270,125), iBitmap, 
						   TRect(0,0,100,100), iMaskGray2, EFalse);
	iGc->SetFaded(EFalse);

	iGc->DiscardFont();
	TheClient->iScreen->ReleaseFont(myFont);
	}


//===================================================
// CTFadingBitmap Definition
//===================================================

CTFadingBitmap::CTFadingBitmap(CTestStep* aStep):
	CTWsGraphicsBase(aStep), iTestResult(ETrue)
	{
	}

CTFadingBitmap::~CTFadingBitmap()
	{
	delete iBgWin;
	}

void CTFadingBitmap::TestFadingL()
	{
	// Modes to test
	TDisplayMode modes[] = 
		{
		EGray2, EGray4, EGray16, EGray256, 
		EColor16, EColor256, EColor4K, EColor64K, 
		EColor16M, EColor16MU, EColor16MA, EColor16MAP   
		};
	
	TBuf <12> modesTxt []= 
		{
		_L("EGray2"), _L("EGray4"), _L("EGray16"), _L("EGray256"), 
		_L("EColor16"), _L("EColor256"), _L("EColor4K"), _L("EColor64K"), 
		_L("EColor16M"), _L("EColor16MU"), _L("EColor16MA"), _L("EColor16MAP")
		};
	
	TBuf <100> testTxt;		 
	for( int i = 0; i < 12; i++)
		{
		testTxt.Format(modesTxt[i]);
		INFO_PRINTF1(testTxt);
		// Here we copy the content of the temp bitmap, which holds the test bitmap,
		// into the bitmap created with alternating color depths.
		User::LeaveIfError(iBgWin->iBitmap->Create(iBgWin->iTempBitmap->SizeInPixels(), modes[i]));
		CFbsBitmapDevice *dev = CFbsBitmapDevice::NewL(iBgWin->iBitmap);
		CleanupStack::PushL(dev);
		CFbsBitGc *gc;
		User::LeaveIfError(dev->CreateContext(gc));
		// performing the copying here
		gc->BitBlt(TPoint(0,0), iBgWin->iTempBitmap);
		// setting the mode text to display it
		iBgWin->iMode = modesTxt[i];
		// draws the bitmap on screen 
		iBgWin->DrawNow();
		TheClient->Flush();
		User::After(5000);
		// cleaning up		
		CleanupStack::Pop();
		delete gc;
		gc = NULL;	
		delete dev;
		dev = NULL;		
		
		// Here the colours of pixels before and after fading are printed in wstest log
		testTxt.Format(_L("Nonfaded circle - color of the outside ring: R=%d G=%d B=%d"), iBgWin->iNonFadedPixel.Red(), iBgWin->iNonFadedPixel.Green(), iBgWin->iNonFadedPixel.Blue());
		INFO_PRINTF1(testTxt);
		testTxt.Format(_L("Faded circle  - color of the outside ring: R=%d G=%d B=%d"), iBgWin->iFadedPixel.Red(), iBgWin->iFadedPixel.Green(), iBgWin->iFadedPixel.Blue());
		INFO_PRINTF1(testTxt);
		
		// Checks if the colors are the same before and after the fading.
		// The color will be the same only in EGray2 and EGray4 as there are no enough 
		// color variations to represent the fading and nonfading effects.
		if(iTestResult &&
		   iBgWin->iNonFadedPixel.Red()   == iBgWin->iFadedPixel.Red() &&
		   iBgWin->iNonFadedPixel.Green() == iBgWin->iFadedPixel.Green() &&
		   iBgWin->iNonFadedPixel.Blue()  == iBgWin->iFadedPixel.Blue() &&
		   modes[i] != EGray2 && modes[i] != EGray4)
			iTestResult = EFalse;
		}
	}

void CTFadingBitmap::ConstructL()
	{
	// construct the base window of the test in the background
	TSize scrSize = TSize(TheClient->iScreen->SizeInPixels());
	iBgWin = new (ELeave) CBaseWin();
	iBgWin->ConstructWinL(TPoint(0,0), scrSize, ETrue);	
	}

void CTFadingBitmap::RunTestCaseL(TInt aCurTestCase)
	{
    	((CTFadingBitmapStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		case 1:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0566
*/
            		((CTFadingBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0566"));
			TestFadingL();
			// Fails or passes the test
			if(!iTestResult)
				TEST(EFalse);
			break;
		default:
            		((CTFadingBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTFadingBitmapStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTFadingBitmapStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(FadingBitmap)
