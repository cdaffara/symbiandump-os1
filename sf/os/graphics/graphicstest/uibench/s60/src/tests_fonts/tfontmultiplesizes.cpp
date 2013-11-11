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


#include "tfontmultiplesizes.h"
#include "fontdefs.h"
#include "twindow.h"

#include <w32std.h>
#include <coefontprovider.h>


_LIT(KTestStep0008,"GRAPHICS-UI-BENCH-S60-0008");
_LIT(KSemaphore, "SemFontMultipleSizesSync"); // Name of the global semaphore
const TInt KNumFontSizes = 5; // Number of different font sizes, also maximum font height


CTFontMultipleSizesTestStep::CTFontMultipleSizesTestStep()
	{
	SetTestStepName(KFontMultipleSizesTestStep);
	}

CTFontMultipleSizesTestStep::~CTFontMultipleSizesTestStep()
    {
    // empty
    }

TVerdict CTFontMultipleSizesTestStep::doTestStepPreambleL()
    {
    // The semaphore has to be created before, otherwise the control can't open it.
    TESTNOERRORL(iSemaphore.CreateGlobal(KSemaphore, 0));
    return CTe_ConeStepBase::doTestStepPreambleL();
    }

TVerdict CTFontMultipleSizesTestStep::doTestStepPostambleL()
    {
    iSemaphore.Close();
    // undo all button state changes
    GenerateEventL(TRawEvent::EButton1Up);
    return CTe_ConeStepBase::doTestStepPostambleL();
    }

TVerdict CTFontMultipleSizesTestStep::doTestStepL()
    {
    SetTestStepID(KTestStep0008);
    TRAPD(err, drawFontMultipleSizesL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0008

@SYMTestCaseDesc
Tests how long it takes to draw some text in different font and height. 

@SYMTestActions
Generate events where font sizes increase and draw the text to the screen.
Generate events where font sizes deacrease and draw the text to the screen. 

@SYMTestExpectedResults
Test should pass and log the average framerate.
*/
void CTFontMultipleSizesTestStep::drawFontMultipleSizesL()
    {
    iProfiler->InitResults();
    // Generate events which increase the font size
    iAppUi->FontSizeControl()->SetIncreaseFontSize(ETrue);
    for (TInt i = 0; i < KNumFontSizes; ++i)
        {
        GenerateEventL(TRawEvent::EButton1Down);
        iSemaphore.Wait();
        }    
    // Generate events which decrease the font size
    iAppUi->FontSizeControl()->SetIncreaseFontSize(EFalse);
    for (TInt i = 0; i < KNumFontSizes; ++i)
        {
        GenerateEventL(TRawEvent::EButton1Down);
        iSemaphore.Wait();
        }
    iProfiler->MarkResultSetL();
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0008, 0, 0, 0, KNumFontSizes * 2, screenSize.iWidth * screenSize.iHeight);
    }

/**
 * Generates events to communicate with the control. Each time the control receives an event
 * it redraws itself. That's necessary because the draw method can't be called directly from
 * a different thread.
 */
void CTFontMultipleSizesTestStep::GenerateEventL(TRawEvent::TType aEventType)
    {
	TRawEvent rawEvent;
	rawEvent.Set(aEventType, 0, 0);
	User::LeaveIfError(UserSvr::AddEvent(rawEvent));
    }

void CTFontMultipleSizesTestStep::InitUIL(CCoeEnv* aCoeEnv)
	{
	iAppUi = new(ELeave) CFontSizeTestAppUi();
	CleanupStack::PushL(iAppUi); // needed until CCoeEnv takes ownership
   	iAppUi->ConstructL();
   	CleanupStack::Pop(iAppUi);
   	aCoeEnv->SetAppUi(iAppUi);
  	}


CFontSizeTestControl* CFontSizeTestControl::NewLC(const CCoeControl* aParent)
	{
	CFontSizeTestControl* self;
	self = new(ELeave) CFontSizeTestControl();
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	return self;
	}

CFontSizeTestControl* CFontSizeTestControl::NewL(const CCoeControl* aParent)
	{
	CFontSizeTestControl* self;
	self = CFontSizeTestControl::NewLC(aParent);
	CleanupStack::Pop(self);
	return self;
	}

CFontSizeTestControl::CFontSizeTestControl() : iIncreaseFontSize(ETrue),
        iBackgroundColor(TRgb(EBackgroundColor)), 
        iWsSession(CCoeEnv::Static()->WsSession()), iFontSize(1)        
    {
    iMargin.SetAllValuesTo(EInsetMargin);
    }

void CFontSizeTestControl::ConstructL(const CCoeControl* aParent)
    {
    User::LeaveIfError(iSemaphore.OpenGlobal(KSemaphore));
    
    iBidiText = TBidiText::NewL(KFullScreenSampleText, EMaximumTextLines, TBidiText::ELeftToRight);
    if (aParent)
        {
        SetContainerWindowL(*aParent);
        }
    else
        {
        CreateWindowL();        
        }
    SetRect(TRect(TPoint(0,0), CTWindow::GetDisplaySizeInPixels()));
    if (!aParent)
        {
        ActivateL();
        }
    }

CFontSizeTestControl::~CFontSizeTestControl()
	{
	delete iBidiText;	
	iSemaphore.Close();	
	}

void CFontSizeTestControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.SetBrushColor(iBackgroundColor);
	
	// get the nearest font matching the desired height
	CFont* font = NULL;
	TFontSpec fontSpec;
	fontSpec.iTypeface.iName = KNokiaSeries60Font;
	fontSpec.iHeight = iFontSize;
	
	CWsScreenDevice* screen = CCoeEnv::Static()->ScreenDevice();
	ASSERT(screen->GetNearestFontToDesignHeightInPixels(font, fontSpec) == KErrNone);
	
	iBidiText->WrapText(aRect.Width() - ESideBearingsAllowance, *font, NULL, EMaximumTextLines);
	
	// it's recommended to create the text drawer in the actual draw function
    XCoeTextDrawer textDrawer(TextDrawer());
    textDrawer->SetMargins(iMargin);
    textDrawer->SetAlignment(TGulAlignment(EHCenterVCenter));
    textDrawer->SetTextColor(KRgbBlack);
    textDrawer->SetLineGapInPixels(EGapBetweenTextLines);
    textDrawer.SetClipRect(aRect);
    textDrawer.DrawText(gc, *iBidiText, aRect, *font);
    
    // Release font, otherwise there are problems deleting iScreen (still holds references)
    screen->ReleaseFont(font);
    	
	// Draw font 1 pixel larger or smaller next time.
	if (iIncreaseFontSize)
	    {
	    ++iFontSize;
	    }
	else
	    {
	    if (iFontSize > 1)
	        {
	        --iFontSize;
	        }
	    }
	}

void CFontSizeTestControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    // Process event generated from test step, forces a redraw.
	if(aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
		DrawNow();
        iWsSession.Flush();
		iWsSession.Finish();
		iSemaphore.Signal();
        }    
    }


CFontSizeTestAppUi::CFontSizeTestAppUi()
	{
	// empty
	}

void CFontSizeTestAppUi::ConstructL()
    {
    BaseConstructL(ENoAppResourceFile);
    iControl = CFontSizeTestControl::NewL();
    AddToStackL(iControl);
    }

CFontSizeTestAppUi::~CFontSizeTestAppUi()
	{
	RemoveFromStack(iControl);
	delete iControl;
	}
