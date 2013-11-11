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


#include "tcomplexfonts.h"
#include "fontdefs.h"
#include "twindow.h"

#include <w32std.h>
#include <coefontprovider.h>


_LIT(KTestStep0010, "GRAPHICS-UI-BENCH-S60-0010");

_LIT(KSemaphore, "SemDrawSync"); // Name of the global semaphore


// constants to define the format of the text
const TInt KHeadingParagraph = 0;
const TInt KClassParagraph = 2;
const TInt KDescriptionHeading = 4;
const TInt KDerivationHeading = 10;
const TInt KFirstDerivation = 12;
const TInt KLastDerivation = 16;

const TInt KHeaderFontSize = 13 * 20; // in TWIPs
const TInt KBodyFontSize = 8 * 20; // in TWIPs
const TInt KBodyLineSpacing = 10; // in TWIPs
const TInt KBodyIndent = 300; // in TWIPs

const TInt KIterationsToTest = 50; // number of iterations


CTComplexFonts::CTComplexFonts()
	{
	SetTestStepName(KTComplexFonts);
	}

void CTComplexFonts::InitUIL(CCoeEnv* aCoeEnv)
    {
    CComplexFontsAppUi* appUi = new(ELeave) CComplexFontsAppUi();
    CleanupStack::PushL(appUi);
    appUi->ConstructL();
    aCoeEnv->SetAppUi(appUi);
    CleanupStack::Pop(appUi);
    }

TVerdict CTComplexFonts::doTestStepPreambleL()
    {
    // The semaphore has to be created before, otherwise the control can't open it.
    TESTNOERRORL(iSemaphore.CreateGlobal(KSemaphore, 0));
    return CTe_ConeStepBase::doTestStepPreambleL();
    }

TVerdict CTComplexFonts::doTestStepPostambleL()
    {
    GenerateEventL(TRawEvent::EButton1Up);
    iSemaphore.Close();
    return CTe_ConeStepBase::doTestStepPostambleL(); 
    }

/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTComplexFonts::doTestStepL()
	{
	SetTestStepName(KTestStep0010);
	TRAPD(err, DrawComplexFontsL());
	if (err != KErrNone)
	    {
	    SetTestStepResult(EAbort);
	    }
	return TestStepResult();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0010

@SYMTestCaseDesc
Tests how long it takes format and draw a CRichText object.

@SYMTestActions
Create a complex font control. Format and draw the text control each time
it receives a left pointer down event. The test step and the drawing of the
control is synchronized with a Semaphore. 

@SYMTestExpectedResults
Test should pass and display the average framerate for the test.
*/
void CTComplexFonts::DrawComplexFontsL()
    {
    iProfiler->InitResults();
    for(TInt i = KIterationsToTest; i > 0; --i)
        {
        GenerateEventL(TRawEvent::EButton1Down);
        iSemaphore.Wait();
        }
    iProfiler->MarkResultSetL();
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0010, 0, 0, 0, KIterationsToTest, screenSize.iWidth * screenSize.iHeight);
    }

/**
 * Generates events to communicate with the control. Each time the control receives an event
 * it redraws itself. That's necessary because the draw method can't be called directly from
 * a different thread.
 */
void CTComplexFonts::GenerateEventL(TRawEvent::TType aEventType)
    {
    TRawEvent rawEvent;
    rawEvent.Set(aEventType, 0, 0);
    User::LeaveIfError(UserSvr::AddEvent(rawEvent));
    }


CComplexFontsAppUi::CComplexFontsAppUi()
    {
    // Empty
    }

void CComplexFontsAppUi::ConstructL()
    {
    BaseConstructL(ENoAppResourceFile);
    iComplexFontsControl = CComplexFontsControl::NewL();
    AddToStackL(iComplexFontsControl);
    }

CComplexFontsAppUi::~CComplexFontsAppUi()
    {
    RemoveFromStack(iComplexFontsControl);
    delete iComplexFontsControl;
    }


CComplexFontsControl* CComplexFontsControl::NewLC(const CCoeControl* aParent)
	{
	CComplexFontsControl* self;
	self = new(ELeave) CComplexFontsControl();
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	return self;
	}

CComplexFontsControl* CComplexFontsControl::NewL(const CCoeControl* aParent)
	{
	CComplexFontsControl* self;
	self = CComplexFontsControl::NewLC(aParent);
	CleanupStack::Pop(self);
	return self;
	}

CComplexFontsControl::CComplexFontsControl() : iWsSession(CCoeEnv::Static()->WsSession())
    {
    // empty
    }

CComplexFontsControl::~CComplexFontsControl()
	{
	delete iTextView;
	delete iLayout;
	delete iRichText;
	delete iCharFormatLayer;
	delete iParaFormatLayer;
	delete iScreen;
	iSemaphore.Close();
	}

/**
 * Constructs the text control with the complex fonts.
 */
void CComplexFontsControl::ConstructL(const CCoeControl* aParent)
    {
    User::LeaveIfError(iSemaphore.OpenGlobal(KSemaphore));
    if (aParent)
        {
        SetContainerWindowL(*aParent);
        }
    else
        {
        CreateWindowL();
        }
    // Create text object, text view and layout.
    iParaFormatLayer = CParaFormatLayer::NewL(); 
    iCharFormatLayer = CCharFormatLayer::NewL();
    // Create an empty rich text object
    iRichText = CRichText::NewL(iParaFormatLayer, iCharFormatLayer);
    
    iScreen = new(ELeave) CWsScreenDevice(ControlEnv()->WsSession());
    User::LeaveIfError(iScreen->Construct()); // default screen used
    
    TPixelsAndRotation pixelsAndRotation;
    iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
    
    // Create the text layout, (required by text view),
    // with the text object and a wrap width (=width of view rect)
    iLayout = CTextLayout::NewL(iRichText,pixelsAndRotation.iPixelSize.iWidth);
    
    // Create text view
    iTextView = CTextView::NewL(iLayout, pixelsAndRotation.iPixelSize,
            iScreen, iScreen, &Window(), 0, // no window group
            &iCoeEnv->WsSession()); // new view
        
    SetRect(TRect(TPoint(0, 0), pixelsAndRotation.iPixelSize));

    // load up text
    iRichText->InsertL(0, KComplexFontsText);

    // break up into paragraphs for CRichText by replacing \n characters in original text
    // with CEditableText::EParagraphDelimiter
    TPtrC complexFontsTextPtr(KComplexFontsText);
    TInt eos = complexFontsTextPtr.Length();
    for (TInt paragraphBreakLocation = 0; paragraphBreakLocation < eos; paragraphBreakLocation++)
        {
        if (complexFontsTextPtr[paragraphBreakLocation] == '\n')
            {
            iRichText->DeleteL(paragraphBreakLocation, 1);
            iRichText->InsertL(paragraphBreakLocation, CEditableText::EParagraphDelimiter);
            }
        }
    
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    // format the first paragraph "Class RWindow" as orange and a bit bigger than the body
    charFormat.iFontPresentation.iTextColor= TLogicalRgb(TRgb(0x0099ff));
    charFormatMask.SetAttrib(EAttColor);
    charFormat.iFontSpec.iHeight = KHeaderFontSize; 
    charFormatMask.SetAttrib(EAttFontHeight);
    
    TInt headerLength = 0;
    TInt headerStart = iRichText->CharPosOfParagraph(headerLength, KHeadingParagraph);
    iRichText->ApplyCharFormatL(charFormat, charFormatMask, headerStart, headerLength);
    charFormatMask.ClearAll();

    // format "Class" as Sans Serif
    charFormat.iFontSpec.iTypeface.iName = KNokiaSeries60Font;
    charFormatMask.SetAttrib(EAttFontTypeface);
    iRichText->ApplyCharFormatL(charFormat, charFormatMask, headerStart, 5);
    charFormatMask.ClearAll();

    // make the rest of the text smaller  
    charFormat.iFontSpec.iHeight = KBodyFontSize;
    charFormatMask.SetAttrib(EAttFontHeight);
    TInt textLength = iRichText->DocumentLength();
    iRichText->ApplyCharFormatL(charFormat, charFormatMask, headerLength, textLength - headerLength);
    charFormatMask.ClearAll();

    TParaFormatMask paraFormatMask;
    // crunch main body a bit closer together to show more on the little screen
    // and indent left margin 
    iParaFormat.iLineSpacingInTwips = KBodyLineSpacing;
    paraFormatMask.SetAttrib(EAttLineSpacing);
    iParaFormat.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;
    paraFormatMask.SetAttrib(EAttLineSpacingControl);
    iParaFormat.iLeftMarginInTwips = KBodyIndent;
    paraFormatMask.SetAttrib(EAttLeftMargin);
    iRichText->ApplyParaFormatL(&iParaFormat, paraFormatMask, headerLength, textLength - headerLength);
    paraFormatMask.ClearAll();

    TInt length = 0;
    TInt start = 0;
    
    // put a grey background on "class RWindow : public RDrawableWindow;"
    start = iRichText->CharPosOfParagraph(length, KClassParagraph);
    iParaFormat.iFillColor = TLogicalRgb(TRgb(0xdddddd));
    paraFormatMask.SetAttrib(EAttFillColor);
    iRichText->ApplyParaFormatL(&iParaFormat, paraFormatMask, start, length);
    paraFormatMask.ClearAll();
    
    // make "Description" and "Derivation" subheadings bold
    charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    charFormatMask.SetAttrib(EAttFontStrokeWeight);
    start = iRichText->CharPosOfParagraph(length, KDescriptionHeading);
    iRichText->ApplyCharFormatL(charFormat, charFormatMask, start, length);
    start = iRichText->CharPosOfParagraph(length, KDerivationHeading);
    iRichText->ApplyCharFormatL(charFormat, charFormatMask, start, length);
    charFormatMask.ClearAll();
    
    // all the paragraphs between and including "Description" and "Derivation" are in sans serif 
    // (the default CRichText font is serif)
    TInt paragraphNumber = 0;

    charFormat.iFontSpec.iTypeface.iName = KNokiaSeries60Font;
    charFormatMask.SetAttrib(EAttFontTypeface);
    for (paragraphNumber = KDescriptionHeading; paragraphNumber <= KDerivationHeading; paragraphNumber++)
        {
        start = iRichText->CharPosOfParagraph(length, paragraphNumber);
        iRichText->ApplyCharFormatL(charFormat, charFormatMask, start, length);
        }
    charFormatMask.ClearAll();
    
    // for each line in the "Derivation" section
    //  progressively indent left margin
    //  make class name blue underlined (it starts two characters after the 
    //    beginning of each paragraph and goes to the space before the '-')
    //  make descriptive text sans serif (from the '-' to the end of the paragraph)
    paraFormatMask.SetAttrib(EAttLeftMargin);
    charFormat.iFontPresentation.iTextColor= TLogicalRgb(TRgb(0xA27316));
    charFormat.iFontPresentation.iUnderline = EUnderlineOn;
    for (paragraphNumber = KFirstDerivation; paragraphNumber <= KLastDerivation; paragraphNumber++)
        {
        iParaFormat.iLeftMarginInTwips = (paragraphNumber - KFirstDerivation + 2)*KBodyIndent;
        start = iRichText->CharPosOfParagraph(length, paragraphNumber);
        iRichText->ApplyParaFormatL(&iParaFormat, paraFormatMask, start, length);
        TInt dash = 0;
        for (dash = start; dash < start + length; dash++)
            {
            if (complexFontsTextPtr[dash] == '-')
                {
                break;
                }
            }
        charFormatMask.SetAttrib(EAttColor);
        charFormatMask.SetAttrib(EAttFontUnderline);
        TInt classNameLength = (dash - 1) - (start + 2);
        iRichText->ApplyCharFormatL(charFormat, charFormatMask, start + 2, classNameLength);
        charFormatMask.ClearAll();
        charFormatMask.SetAttrib(EAttFontTypeface);
        TInt descriptionLength = length - classNameLength - 4;
        iRichText->ApplyCharFormatL(charFormat, charFormatMask, dash, descriptionLength);
        charFormatMask.ClearAll();
        }
    paraFormatMask.ClearAll();
    
    if (!aParent)
        {
        ActivateL();
        }
    }

/**
 * The test meassures how long it takes to format and draw the text view.
 * Most time is spend with formatting the text.
 */
void CComplexFontsControl::Draw(const TRect& aRect) const
	{
	TRAPD(err,iTextView->FormatTextL());
	if (err == KErrNone)
	    {
	    TRAP(err,iTextView->DrawL(aRect));
	    }
	}

/**
 * Draws the text control if a pointer down event is received.
 */
void CComplexFontsControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        DrawNow();
        iWsSession.Flush();
        iWsSession.Finish();
        iSemaphore.Signal();
        }
    }
