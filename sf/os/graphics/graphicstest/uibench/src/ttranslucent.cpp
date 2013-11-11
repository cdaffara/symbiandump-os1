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

#include "ttranslucent.h"

const TInt KIterationsToTest = 100;

//
// CTTranslucent
//
CTTranslucent::CTTranslucent()
	{
	SetTestStepName(KTTranslucent);
	}

CTTranslucent::~CTTranslucent()
	{
	iWindowArray.ResetAndDestroy();
	iWindowArray.Close();
    delete iTopWindow;
    delete iBottomWindow;
    delete iBitmap; 

    delete iGc;
    delete iScreen;
	iWindowGroup.Close();
	iWsSession.Close();
	}	

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTTranslucent::doTestStepPreambleL()
	{	
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();	
	SetScreenModeL(EColor16MU);
	return TestStepResult();
	}
	
TVerdict CTTranslucent::doTestStepL()
	{
    User::LeaveIfError(iWsSession.Connect());
    
    iWindowGroup=RWindowGroup(iWsSession);
    User::LeaveIfError(iWindowGroup.Construct(2, ETrue)); // meaningless handle; enable focus

    // construct screen device and graphics context
    iScreen=new (ELeave) CWsScreenDevice(iWsSession); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction
    User::LeaveIfError(iScreen->CreateContext(iGc)); // create graphics context

    iScreenRect = TRect(0,0,iScreen->SizeInPixels().iWidth, iScreen->SizeInPixels().iHeight);

    // Construct a gradient bitmap for use by bitmap test
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(80,80), EColor16MA));
	CTe_graphicsperformanceSuiteStepBase::VerticalGradientAlphaL(bitmap, TRgb(0x000000ff), TRgb(0xff000000));
	delete iBitmap;
	iBitmap = bitmap;
	CleanupStack::Pop(bitmap);

	/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0132
	@SYMPREQ PREQ1841
	@SYMTestCaseDesc Measures time taken to redraw 2, 8, 32 and 144 blank translucent
					 partially overlapping windows.
	@SYMTestActions
	1. Create blank translucent partially overlapping windows.
	2. Cover them all with an "ontop" window.
	3. Hide the "ontop" window and measure time taken to redraw all overlapping windows underneath.
	4. Repeat KIterationsToTest times and calculate average time per iteration.

	@SYMTestExpectedResults
	As the number of windows increases, so should the time taken to redraw them.
	*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0132"));
	RunTestCaseL(_L("Redraw-BlankWindows2"), EBlankWindows, 2, 1);
	RunTestCaseL(_L("Redraw-BlankWindows8"), EBlankWindows, 2, 4);
	RunTestCaseL(_L("Redraw-BlankWindows32"), EBlankWindows, 4, 8);
	RunTestCaseL(_L("Redraw-BlankWindows144"), EBlankWindows, 12, 12);
	RecordTestResultL();

	/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0133
	@SYMPREQ PREQ1841
	@SYMTestCaseDesc Measures time taken to redraw 2, 8, 32 and 144 translucent
					 partially overlapping windows containing drawn text.
	@SYMTestActions
	1. Create translucent partially overlapping windows.
	2. Cover them all with an "ontop" window.
	3. Hide the "ontop" window and measure time taken to redraw all overlapping windows underneath.
	4. Repeat KIterationsToTest times and calculate average time per iteration.

	@SYMTestExpectedResults
	As the number of windows increases, so should the time taken to redraw them.
	*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0133"));
	RunTestCaseL(_L("Redraw-TextWindows2"), ETextWindows, 2, 1);
	RunTestCaseL(_L("Redraw-TextWindows8"), ETextWindows, 2, 4);
	RunTestCaseL(_L("Redraw-TextWindows32"), ETextWindows, 4, 8);
	RunTestCaseL(_L("Redraw-TextWindows144"), ETextWindows, 12, 12);
	RecordTestResultL();

	/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0134
	@SYMPREQ PREQ1841
	@SYMTestCaseDesc Measures time taken to redraw 2, 8, 32 and 144 translucent
					 partially overlapping windows containing drawn ellipsis.
	@SYMTestActions
	1. Create translucent partially overlapping windows.
	2. Cover them all with an "ontop" window.
	3. Hide the "ontop" window and measure time taken to redraw all overlapping windows underneath.
	4. Repeat KIterationsToTest times and calculate average time per iteration.

	@SYMTestExpectedResults
	As the number of windows increases, so should the time taken to redraw them.
	*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0134"));
	RunTestCaseL(_L("Redraw-EllipseWindows2"), EEllipseWindows, 2, 1);
	RunTestCaseL(_L("Redraw-EllipseWindows8"), EEllipseWindows, 2, 4);
	RunTestCaseL(_L("Redraw-EllipseWindows32"), EEllipseWindows, 4, 8);
	RunTestCaseL(_L("Redraw-EllipseWindows144"), EEllipseWindows, 12, 12);
	RecordTestResultL();

	/**
	@SYMTestCaseID GRAPHICS-UI-BENCH-0135
	@SYMPREQ PREQ1841
	@SYMTestCaseDesc Measures time taken to redraw 2, 8, 32 and 144 translucent
					 partially overlapping windows containing bitmaps.
	@SYMTestActions
	1. Create translucent partially overlapping windows.
	2. Cover them all with an "ontop" window.
	3. Hide the "ontop" window and measure time taken to redraw all overlapping windows underneath.
	4. Repeat KIterationsToTest times and calculate average time per iteration.

	@SYMTestExpectedResults
	As the number of windows increases, so should the time taken to redraw them.
	*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0135"));
	RunTestCaseL(_L("Redraw-BitmapWindows2"), EBitmapWindows, 2, 1);
	RunTestCaseL(_L("Redraw-BitmapWindows8"), EBitmapWindows, 2, 4);
	RunTestCaseL(_L("Redraw-BitmapWindows32"), EBitmapWindows, 4, 8);
	RunTestCaseL(_L("Redraw-BitmapWindows144"), EBitmapWindows, 12, 12);
	RecordTestResultL();
		
	return TestStepResult();
	}

void CTTranslucent::RunTestCaseL(const TDesC& aTestName, TTestCase aTestCase, TInt aHorizontalWindows, TInt aVerticalWindows)
	{
	iTestCase = aTestCase;

	ConstructWindowsL(aHorizontalWindows, aVerticalWindows);
	iWsSession.Finish(); // Make sure all windows are drawn in their initial state/positions

	iProfiler->InitResults();

	// Measure time taken to redraw all overlapping translucent windows
	// and repeat KIterationsToTest times
	for (int i = 0; i < KIterationsToTest; i++)
		{
		iProfiler->StartTimer();
		iTopWindow->Window().SetVisible(EFalse); // hide ontop window to cause overlapping translucent windows to be redrawn
		iWsSession.Finish();
		iProfiler->MarkResultSetL();

		iTopWindow->Window().SetVisible(ETrue); // show ontop window
		iWsSession.Finish();
		}

	iProfiler->ResultsAnalysis(aTestName, 0, 0, ScreenDevice()->BitmapDevice().DisplayMode(), KIterationsToTest);
	
	DestroyWindows();
	}

void CTTranslucent::ConstructWindowsL(TInt aHorizontalWindows, TInt aVerticalWindows)
	{
	// Create a yellow opaque window to use as the background for the translucent windows
    iBottomWindow = new (ELeave) CWin(iWsSession, iWindowGroup);
    iBottomWindow->ConstructL(iScreenRect);
    iBottomWindow->Window().SetBackgroundColor(TRgb(255, 255, 0));
    iBottomWindow->Window().SetOrdinalPosition(-1);
    iBottomWindow->Window().Invalidate();
    iBottomWindow->Window().BeginRedraw();
    iBottomWindow->Window().EndRedraw();
    iBottomWindow->Window().Activate();

	// Create some green partially overlapping translucent windows.
	// Each window has roughly the same aspect ratio as the screen.
	// Each window is the same size for every test, regardless of how many windows are being created.

    // Maximum number of windows horizontally and vertically that we want to be able to fit onto
    // the screen. Window size is calculated based on screen width/height and number of windows we want
    // to fit in with the desired overlap.
    // KMaxWindows * KMaxWindows gives the total maximum number of windows that will fit on screen without
    // being clipped.
	const TInt KMaxWindows = 12;
	
	// Calculate 1/4 width of each window
	const TInt KQuarterWidth = iScreenRect.Width() / (KMaxWindows + 3);
	// Calculate 1/4 height of each window
	const TInt KQuarterHeight = iScreenRect.Height() / (KMaxWindows + 3);

	TRect rect(0, 0, 0, 0);
	TInt x = 0;
	TInt y = 0;
	// Calculate size used for all translucent windows
	TSize size(KQuarterWidth*4, KQuarterHeight*4);
	TInt hCount = 0;
	TInt vCount = 0;
	for (hCount = 0; hCount < aHorizontalWindows; hCount++)
		{
		y = 0;
		for (vCount = 0; vCount < aVerticalWindows; vCount++)
			{
			// Construct a vertical column of windows
			CWin* win = new(ELeave) CWin(iWsSession, iWindowGroup);
			CleanupStack::PushL(win);
			iWindowArray.AppendL(win);
			CleanupStack::Pop(win);
			rect.iTl = TPoint(x, y);
			rect.SetSize(size);
			win->ConstructL(rect);
			User::LeaveIfError(win->Window().SetTransparencyAlphaChannel());
			win->Window().SetBackgroundColor(TRgb(0, 255, 0, 16));

			TRect clipRect(size);
			DrawWindowL(win, clipRect);
			win->Window().Activate();
			y+=KQuarterHeight;
			}
		x+=KQuarterWidth;
		}

	// Create a red opaque window ontop of all the others
	iTopWindow = new (ELeave) CWin(iWsSession, iWindowGroup);
	iTopWindow->ConstructL(iScreenRect);
	iTopWindow->Window().SetBackgroundColor(TRgb(255, 0, 0));
	iTopWindow->Window().SetOrdinalPosition(0);
	iTopWindow->Window().Invalidate();
	iTopWindow->Window().BeginRedraw();
	iTopWindow->Window().EndRedraw();
	iTopWindow->Window().Activate();
	}

void CTTranslucent::DestroyWindows()
	{
	iWindowArray.ResetAndDestroy();
    delete iTopWindow;
    iTopWindow = NULL;
    delete iBottomWindow;
	iBottomWindow = NULL;
	}

void CTTranslucent::DrawWindowL(CWin* aWindow, const TRect& aRect)
	{
	iGc->Activate(aWindow->Window());
    iGc->SetClippingRect(aRect);
	aWindow->Window().Invalidate();
	aWindow->Window().BeginRedraw();

	// Draw the whole window
    switch(iTestCase)
    	{
    	case EBlankWindows:
    		break; // Don't draw anything on the blank windows

    	case ETextWindows:
    		{
    		CFont* font;
    		TFontSpec fontSpec;
    		fontSpec.iHeight = 300;
    		User::LeaveIfError(iGc->Device()->GetNearestFontToDesignHeightInTwips(font, fontSpec));
    		iGc->UseFont(font);
    		iGc->DrawText(_L("Text"), TPoint(0,20));
    		iGc->DrawText(_L("window"), TPoint(0,40));
    		iGc->DiscardFont();
    		iGc->Device()->ReleaseFont(font);
    		break;
    		}

    	case EEllipseWindows:
    		iGc->SetPenColor(TRgb(0, 0, 0, 255));
    		iGc->SetBrushColor(TRgb(128, 128, 255, 128));
    		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    		iGc->DrawEllipse(TRect(0,0,79,79));
    		iGc->DrawEllipse(TRect(10,10,69,69));
    		iGc->DrawEllipse(TRect(20,20,59,59));
    		iGc->DrawEllipse(TRect(30,30,49,49));
    		iGc->SetBrushColor(TRgb(255, 0, 0, 150));
    		iGc->SetBrushStyle(CGraphicsContext::EDiamondCrossHatchBrush);
    		iGc->DrawEllipse(TRect(0,0,20,15));
    		iGc->DrawEllipse(TRect(30,10,50,30));
    		break;
    	case EBitmapWindows:
    		iGc->BitBlt(TPoint(0,0), iBitmap);
    		break;
    	}

	aWindow->Window().EndRedraw();
	iGc->Deactivate();
	}

//
// CWin
//
CTTranslucent::CWin::CWin(RWsSession& aWsSession, RWindowGroup& aWindowGroup)
	:iWsSession(aWsSession), iWindowGroup(aWindowGroup)
    {
    }

CTTranslucent::CWin::~CWin()
    {
    iWindow.Close();
    }

void CTTranslucent::CWin::ConstructL (const TRect& aRect)
    {
    iWindow=RWindow(iWsSession);
    User::LeaveIfError(iWindow.Construct(iWindowGroup, (TUint32)this));
    iRect = aRect;
    iWindow.SetExtent(iRect.iTl, iRect.Size());
    }

RWindow& CTTranslucent::CWin::Window()
	{
	return iWindow;
	}
