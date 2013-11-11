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


#include "tfullscreentext.h"
#include "fontdefs.h"
#include "twindow.h"

#include <w32std.h>
#include <coefontprovider.h>


const TInt KIterationsToTest = 25; // number of iterations

_LIT(KSemaphore, "SemFullScreenTextSync"); // Name of the global semaphore
_LIT(KTestStep0009,"GRAPHICS-UI-BENCH-S60-0009");

// Literals for the ini file
_LIT(KSectNameOpenGLVGTest, "FullScreenFontTest");
_LIT(KKeyNameVertical, "Vertical");
_LIT(KKeyNameRightToLeft, "RightToLeft");


CTFullScreenText::CTFullScreenText()
	{
	SetTestStepName(KFullScreenText);
	}

void CTFullScreenText::InitUIL(CCoeEnv* aCoeEnv)
    {
    CFullScreenTextAppUi* appUi = new(ELeave) CFullScreenTextAppUi();
    CleanupStack::PushL(appUi);
    appUi->ConstructL(iDrawVertically, iRightToLeft);
    aCoeEnv->SetAppUi(appUi); // CCoeEnv takes ownership
    CleanupStack::Pop(appUi);
    }

/**
 * Generates events to communicate with the control. Each time the control receives an event
 * it redraws itself. That's necessary because the draw method can't be called directly from
 * a different thread.
 */
void CTFullScreenText::GenerateEventL(TRawEvent::TType aEventType)
    {
    TRawEvent rawEvent;
    rawEvent.Set(aEventType, 0, 0);
    User::LeaveIfError(UserSvr::AddEvent(rawEvent));
    }

TVerdict CTFullScreenText::doTestStepPreambleL()
	{
	// The semaphore has to be created before, otherwise the control can't open it.
    TESTNOERRORL(iSemaphore.CreateGlobal(KSemaphore, 0));	
	// read values from ini file, if keys not found default values apply
	iDrawVertically = EFalse;
	GetBoolFromConfig(KSectNameOpenGLVGTest, KKeyNameVertical, iDrawVertically);
	iRightToLeft = EFalse;
	GetBoolFromConfig(KSectNameOpenGLVGTest, KKeyNameRightToLeft, iRightToLeft);
	// baseclass function needs to be called at the end, otherwise
	// the values from the ini file would be read after InitUIL()
	return CTe_ConeStepBase::doTestStepPreambleL();;
	}

TVerdict CTFullScreenText::doTestStepPostambleL()
    {
    iSemaphore.Close();
    // undo the button state change
    GenerateEventL(TRawEvent::EButton1Up);
    return CTe_ConeStepBase::doTestStepPostambleL();
    }

TVerdict CTFullScreenText::doTestStepL()
    {
    SetTestStepID(KTestStep0009);
    TRAPD(err, FullScreenTextL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0009

@SYMTestCaseDesc
Tests how long it takes to draw text on the full screen. The font is requested
for every redraw.

@SYMTestActions
The control redraws itself everytime it receives a left button down event.
A semaphore is used to synchronise cone thread and test step thread. Depending on the
ini file the text is drawn from left to right, vertically or normal.

@SYMTestExpectedResults
Test should pass and log the framerate.
*/
void CTFullScreenText::FullScreenTextL()
    {
    iProfiler->InitResults();
    for(TInt i = KIterationsToTest; i > 0; --i)
        {
        GenerateEventL(TRawEvent::EButton1Down);
        iSemaphore.Wait();
        }
    iProfiler->MarkResultSetL();
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    // todo: Define how to distinguish between tests with different ini files
    iProfiler->ResultsAnalysisFrameRate(KTestStep0009, 0, 0, 0, KIterationsToTest,
            screenSize.iWidth * screenSize.iHeight);
    }


CGlobalTextControl* CGlobalTextControl::NewLC(const CCoeControl* aParent, TBool aDrawVertically, TBool aRightToLeft)
	{
	CGlobalTextControl* self;
	self = new(ELeave) CGlobalTextControl(aDrawVertically, aRightToLeft);
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	return self;
	}

CGlobalTextControl* CGlobalTextControl::NewL(const CCoeControl* aParent, TBool aDrawVertically, TBool aRightToLeft)
	{
	CGlobalTextControl* self;
	self = CGlobalTextControl::NewLC(aParent, aDrawVertically, aRightToLeft);
	CleanupStack::Pop(self);
	return self;
	}

CGlobalTextControl::CGlobalTextControl(TBool aDrawVertically, TBool aRightToLeft) : 
        iDrawVertically(aDrawVertically), iRightToLeft(aRightToLeft),
        iWsSession(CCoeEnv::Static()->WsSession())
    {
    iMargin.SetAllValuesTo(EInsetMargin);
    }

void CGlobalTextControl::ConstructL(const CCoeControl* aParent)
    {
    User::LeaveIfError(iSemaphore.OpenGlobal(KSemaphore));
    iScreen = new(ELeave) CWsScreenDevice(ControlEnv()->WsSession());
    iScreen->Construct(); // default screen used
    if (iRightToLeft)
        {
        iBidiText = TBidiText::NewL(KRightToLeftText, EMaximumTextLines, TBidiText::ERightToLeft);
        }
    else
        {
        iBidiText = TBidiText::NewL(KFullScreenSampleText, EMaximumTextLines, TBidiText::ELeftToRight);
        }
    if (aParent)
        {
        SetContainerWindowL(*aParent);
        }
    else
        {
        CreateWindowL();
        ActivateL();
        }
    SetRect(TRect(TPoint(0,0), CTWindow::GetDisplaySizeInPixels()));
    }

CGlobalTextControl::~CGlobalTextControl()
	{
    delete iBidiText;
    delete iScreen;
    iSemaphore.Close();
	}

void CGlobalTextControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.SetBrushColor(TRgb(EBackgroundColor));
	gc.Clear(Rect());
	
	// it's recommended to create XCoeTextDrawer on the stack
	XCoeTextDrawer textDrawer(TextDrawer());
	textDrawer->SetAlignment(TGulAlignment(EHCenterVCenter));
	textDrawer->SetTextColor(KRgbBlack);
	textDrawer->SetMargins(iMargin);
	textDrawer->SetLineGapInPixels(EGapBetweenTextLines);
	textDrawer.SetClipRect(aRect);

	// request the font, could also be done during construction
	CFont* font;
	TFontSpec fontSpec;
	fontSpec.iTypeface.iName = KNokiaSeries60Font;
	fontSpec.iHeight=EDesiredFontHeight;
	iScreen->GetNearestFontToDesignHeightInPixels((CFont*&)font, fontSpec);
	
	if (iDrawVertically)
		{
		iBidiText->WrapText(aRect.Height() - ESideBearingsAllowance, *font, NULL, EMaximumTextLines);
		textDrawer.DrawTextVertical(gc, *iBidiText, aRect, *font);
		}
	else
		{
		// If you don't explicitly set the alignment for RightToLeft text, 
		// DrawText's default is to left align regardless of the text direction.
		// Setting it explicitly to Left alignment tells DrawText to right align.
		if (iRightToLeft)
			{
			TGulAlignment alignment(EHLeftVTop);
			textDrawer.SetAlignment(alignment);
			}
		iBidiText->WrapText(aRect.Width() - ESideBearingsAllowance, *font, NULL, EMaximumTextLines);
		textDrawer.DrawText(gc, *iBidiText, aRect, *font);
		}
	iScreen->ReleaseFont(font);	// should be done every time the font is not needed anymore
	}

void CGlobalTextControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
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


CFullScreenTextAppUi::CFullScreenTextAppUi()
	{
	// empty
	}

void CFullScreenTextAppUi::ConstructL(TBool aDrawVertically, TBool aRightToLeft)
    {
    BaseConstructL(ENoAppResourceFile);
    iGlobalTextControl = CGlobalTextControl::NewL(NULL, aDrawVertically, aRightToLeft);
    AddToStackL(iGlobalTextControl);
    }

CFullScreenTextAppUi::~CFullScreenTextAppUi()
	{
	RemoveFromStack(iGlobalTextControl);
	delete iGlobalTextControl;
	}
