// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ttextlistscroll.h"
#include "te_gesturegenerator.h"
#include "twindow.h"

_LIT(KTestStep0011,"GRAPHICS-UI-BENCH-S60-0011");

_LIT(KSemaphore, "SemTextListScrollSync"); // Name of the global semaphore

_LIT(KItemText,"Item number %d. The quick brown fox jumped over the lazy dog. The quick brown fox jumped ");

const TInt KNumItems = 500; // Number of items in the scroll list
const TInt KNumItemsOnScreen = 5;

const TInt KInitialPixelsPerFrame = 0; 
const TInt KAdjustPixelRatePeriod = 200000; // pixel rate is adjusted every 200 ms
const TInt KNumSamples = 10; // 10 data points per test. Each cycle lasts 10 secs.

const TInt KNumberOfIterations = 1000;


CTTextListScroll::CTTextListScroll()
    {
    SetTestStepName(KTTextListScroll);
    }

TVerdict CTTextListScroll::doTestStepPreambleL()
    {
    // The semaphore has to be created before, otherwise the control can't open it.
    TESTNOERRORL(iSemaphore.CreateGlobal(KSemaphore, 0));   
    return CTe_ConeStepBase::doTestStepPreambleL();;
    }

TVerdict CTTextListScroll::doTestStepPostambleL()
    {
    iSemaphore.Close();
    // undo the button state change
    GenerateEventL(TRawEvent::EButton1Up);
    return CTe_ConeStepBase::doTestStepPostambleL();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0011

@SYMTestCaseDesc
Scroll a psuedo list box of text. The look of a list box is created by drawing text and lines
to the screen. This list box is then scrolled forwards and backwards, at various speeds, which 
simulates a user interacting with a device. 

@SYMTestActions
Create two data sets which hold values matching a sine wave pattern. Create a further data set with 
values which will be used to simulates scrolling quickly and then gradually slows down. Use the
GestureGenerator class to create pointer events matching the pattern in the data sets. Draw the list
box to screen within the HandlePointerEventL function.

@SYMTestExpectedResults
Test should pass and list box scrolls in a forward direction at speeeds which resemble a sine wave 
pattern. List box should then scroll backwards and loop back below the starting point.
*/
TVerdict CTTextListScroll::doTestStepL()
    {
    iProfiler->InitResults();
    for (TInt i = 0; i < KNumberOfIterations; i++)
        {
        GenerateEventL(TRawEvent::EButton1Down);
        iSemaphore.Wait();
        }    
    iProfiler->MarkResultSetL();
    TSize windowSize = CTWindow::GetDisplaySizeInPixels();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0011, 0, 0, 0, KNumberOfIterations, windowSize.iWidth * windowSize.iHeight);
    return TestStepResult();
    }

/**
 * Sends an button down event to the text control. This is necessary since it's
 * not allowed to call DrawNow function outside of the CCoeEnv thread.
 */
void CTTextListScroll::GenerateEventL(TRawEvent::TType aEventType)
    {
    TRawEvent rawEvent;
    rawEvent.Set(aEventType, 0, 0);
    TESTNOERRORL(UserSvr::AddEvent(rawEvent));
    }

void CTTextListScroll::InitUIL(CCoeEnv* aCoeEnv)
    {
    iAppUi = new(ELeave) CTextListScrollAppUi();
    CleanupStack::PushL(iAppUi); // needed until CCoeEnv takes ownership
    iAppUi->ConstructL();
    CleanupStack::Pop(iAppUi);
    aCoeEnv->SetAppUi(iAppUi);
    }


CTextList* CTextList::NewLC(const TRect& aRect,const CCoeControl* aParent)
    {
    CTextList* self = new(ELeave) CTextList();
    CleanupStack::PushL(self);
    self->ConstructL(aRect,aParent);
    return self;
    }

CTextList* CTextList::NewL(const TRect& aRect,const CCoeControl* aParent)
    {
    CTextList* self = CTextList::NewLC(aRect, aParent);
    CleanupStack::Pop(self);
    return self;
    }
 
CTextList::CTextList() : iPixelsPerFrame(KInitialPixelsPerFrame),
        iWsSession(CCoeEnv::Static()->WsSession())
    {
    // empty
    }
 
CTextList::~CTextList()
    {
    delete iItemTextArray;        
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    iCurrentDataSet.Close();
    iSemaphore.Close();
    }
 
void CTextList::ConstructL(const TRect& aRect,const CCoeControl* aParent)
    {
    User::LeaveIfError(iSemaphore.OpenGlobal(KSemaphore));
    iItemTextArray = new(ELeave) CDesCArrayFlat(KNumItems);
    TBuf<128> textBuf;
    for(TInt i = 0; i < KNumItems; ++i)
        {
        textBuf.Format(KItemText, i);
        iItemTextArray->AppendL(textBuf);
        }

    // Create a set of values. The amplitude is the max. number of pixels to 
    // scroll the list box by per frame. The frequency is 1 meaning that only
    // one series of values will be created. For example: the list box will
    // gradually scroll faster until the max rate is acheived before starting
    // to slow down again. If the frequency was two then this would occur twice
    // for this cycle of the test.
    // Data sets for each test.
    WaveGenerator::GenerateSineWave(iCurrentDataSet, KNumSamples, 7, 1, ETrue);
    WaveGenerator::GenerateSineWave(iCurrentDataSet, KNumSamples, 5, 1, EFalse);
    // A data set that starts scrolling quickly and gradually slows down.
    iCurrentDataSet.AppendL(8);
    iCurrentDataSet.AppendL(10);
    iCurrentDataSet.AppendL(8);
    iCurrentDataSet.AppendL(7);
    iCurrentDataSet.AppendL(6);
    iCurrentDataSet.AppendL(5);
    iCurrentDataSet.AppendL(4);
    iCurrentDataSet.AppendL(2);
    iCurrentDataSet.AppendL(1);
    iCurrentDataSet.AppendL(0);
    
    // No owner, so create an own window
    if(!aParent)     
        {
        CreateWindowL();
        DrawableWindow()->PointerFilter(EPointerFilterDrag, 0);
        ActivateL();
        }
    // Use parent window
    else
        {
        // This is component in a compound control
        SetContainerWindowL(*aParent);
        } 
    SetRect(aRect);

    iTimer = CPeriodic::NewL(EPriorityNormal);
    TCallBack callback(&CTextList::TimerExpired, (TAny*)this);
    iTimer->Start(KAdjustPixelRatePeriod, KAdjustPixelRatePeriod, callback);
    }

TSize CTextList::MinimumSize()
    {
    return TSize(10,10);
    }

void CTextList::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if(aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        iPos += iPixelsPerFrame;
        DrawNow();
        iWsSession.Flush();
        iWsSession.Finish();
        iSemaphore.Signal();
        } 
    }
 
void CTextList::Draw(const TRect& /*aRect*/) const
    {
    if ((!iItemTextArray) || (iItemTextArray->MdcaCount() <= 0))
        {
        return;
        }
    
    // Set the attributes to draw text
    CWindowGc& gc = SystemGc();
    const CFont* font = CCoeEnv::Static()->NormalFont();
    gc.UseFont(font);
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetPenColor(KRgbBlack);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(KRgbWhite);

    // Only draw visible items.
    // CTextList object (iTextList) is set to height of the screen. i.e. iHeight == screen height 
    TInt itemHeight = Size().iHeight / KNumItemsOnScreen; 

    TInt itemToDraw = iPos / -itemHeight;
    // This accounts for looping around in the same direction multiple times.
    itemToDraw %= KNumItems;

    TInt startPos = iPos + (itemToDraw * itemHeight);
    //startPos should always be offscreen so that item slides into view.
    if (startPos > 0)
        {
        startPos -= itemHeight;
        }
    TRect aTextRect(TPoint(0, startPos), TSize(Size().iWidth, itemHeight));
    TInt aBaselineOffset = (aTextRect.iBr.iY - aTextRect.iTl.iY - font->HeightInPixels()) / 2 + font->AscentInPixels();
            
    for(TInt i = 0; i < KNumItemsOnScreen + 1; ++i)
        {
        if (itemToDraw >= KNumItems)
            {
            itemToDraw = 0;
            }
        else if (itemToDraw < 0)
            {
            itemToDraw = KNumItems + itemToDraw;
            }
        
        ASSERT(itemToDraw >= 0 && itemToDraw < KNumItems);
        TPtrC itemText = iItemTextArray->MdcaPoint(itemToDraw);
        gc.DrawText(itemText, aTextRect, aBaselineOffset, CGraphicsContext::ELeft, 1);
        gc.DrawLine(TPoint(0, (aTextRect.iBr.iY-5)), TPoint(aTextRect.iBr.iX, aTextRect.iBr.iY-5));
        aTextRect.iTl.iY += itemHeight;
        aTextRect.iBr.iY += itemHeight;
        itemToDraw++;
        }
    }

TInt CTextList::TimerExpired(TAny* aPtr)
    {
    CTextList* self = (CTextList*)aPtr;
    TRAPD(err, self->AdjustRateL());
    return err;
    }
    
/**
Adjusts the number of pixels that the list box is scrolled per frame based
on the current cycle of the main test. On the second cycle the list box starts
to scroll backwards.
*/    
void CTextList::AdjustRateL()
    {
    if (iTickCount >= iCurrentDataSet.Count())
        {
        iTickCount = 0;
        }
    iPixelsPerFrame = iCurrentDataSet[iTickCount++];
    }


CTextListScrollAppUi::CTextListScrollAppUi()
    {
    // empty
    }

CTextListScrollAppUi::~CTextListScrollAppUi()
    {
    RemoveFromStack(iTextList);
    delete iTextList;
    }

void CTextListScrollAppUi::ConstructL()
    {
    BaseConstructL(ENoAppResourceFile);
    CWsScreenDevice* aScreenDevice = CCoeEnv::Static()->ScreenDevice();
    iTextList = CTextList::NewL(TRect(TPoint(), TSize(aScreenDevice->SizeInPixels())));
    AddToStackL(iTextList);
    }
