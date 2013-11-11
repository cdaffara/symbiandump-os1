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


#include "tpan.h"
#include "te_gesturegenerator.h"
#include "twindow.h"


_LIT(KSemaphore, "SemTPanSync"); // Name of the global semaphore

const TInt KGestureGenerationLimit = 50;

// Test bitmap file location
_LIT(KBigBitmap,"z:\\resource\\apps\\uibench_s60_big.mbm");
_LIT(KTestStep0001,"GRAPHICS-UI-BENCH-S60-0001");

CTPan::CTPan()
	{
	SetTestStepName(KTPan);
	}

TVerdict CTPan::doTestStepPreambleL()
    {
    // The semaphore has to be created before, otherwise the control can't open it.
    TESTNOERRORL(iSemaphore.CreateGlobal(KSemaphore, 0));
    return CTe_ConeStepBase::doTestStepPreambleL();
    }

TVerdict CTPan::doTestStepPostambleL()
    {
    iSemaphore.Close();
    return CTe_ConeStepBase::doTestStepPostambleL(); 
    }

/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTPan::doTestStepL()
	{
	SetTestStepID(KTestStep0001);
	PanBitmapL();
	//RecordTestResultL(); // not possible because of heap panic
	return TestStepResult();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0001

@SYMTestCaseDesc
Tests how long it takes to pan a bitmap in reaction to pointer events.

@SYMTestActions
Create the bitmap and generate the pointer events. Depending on the location of the
pointer the bitmap is drawn to the screen. 

@SYMTestExpectedResults
Test should pass and display the average framerate for the whole test.
*/
void CTPan::PanBitmapL()
    {
    iProfiler->InitResults();

    // Simulate some horizontal drag pointer events
    GestureGenerator::SimulateFlickGestureL(iSemaphore, TPoint(0, 0),           
            TPoint(KGestureGenerationLimit, 0));
    // Simulate some vertical drag pointer events
    GestureGenerator::SimulateFlickGestureL(iSemaphore, TPoint(KGestureGenerationLimit, 0), 
            TPoint(KGestureGenerationLimit, KGestureGenerationLimit));   
    
    iProfiler->MarkResultSetL();
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0001, 0, 0, 0,
            iAppUi->PanControl()->Iterations(), screenSize.iWidth * screenSize.iHeight);
    }

void CTPan::InitUIL(CCoeEnv* aCoeEnv)
	{
	iAppUi = new(ELeave) CPanAppUi();
  	// iAppUi needs to be put on the cleanupstack until CCoeEnv takes ownership of iAppUi
  	CleanupStack::PushL(iAppUi);
  	iAppUi->ConstructL(TRect(CTWindow::GetDisplaySizeInPixels()));
   	CleanupStack::Pop(iAppUi);
   	aCoeEnv->SetAppUi(iAppUi);
  	}

//=============================================================================

CPanAppUi::CPanAppUi()
    {
    // empty
    }

CPanAppUi::~CPanAppUi()
    {
    RemoveFromStack(iPanControl);
    delete iPanControl;
    }

void CPanAppUi::ConstructL(const TRect& aRect)
    {
    BaseConstructL(ENoAppResourceFile);
    iPanControl = CPanControl::NewL(aRect);
    AddToStackL(iPanControl);
    }

CPanControl* CPanAppUi::PanControl()
    {
    return iPanControl;
    }

//=============================================================================

CPanControl* CPanControl::NewL(const TRect& aRect, const CCoeControl* aParent)
    {
    CPanControl* self = CPanControl::NewLC(aRect, aParent);
    CleanupStack::Pop(self);
    return self;
    }
 
CPanControl* CPanControl::NewLC(const TRect& aRect, const CCoeControl* aParent)
    {
    CPanControl* self = new(ELeave) CPanControl();
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aParent);
    return self;
    }
 
CPanControl::CPanControl() : iWsSession(CCoeEnv::Static()->WsSession())
	{
	// empty
    }
 
CPanControl::~CPanControl()
    {
    delete iSourceBitmap;
    iSemaphore.Close();
    }
 
void CPanControl::ConstructL(const TRect& aRect, const CCoeControl* aParent)
    {
    User::LeaveIfError(iSemaphore.OpenGlobal(KSemaphore));
    iSourceBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iSourceBitmap->Load(KBigBitmap, 0));
    
    // No owner, so create an own window
    if(!aParent)     
        {
        CreateWindowL();
        DrawableWindow()->PointerFilter(EPointerFilterDrag, 0);
        SetRect(aRect);
        ActivateL();
        }    
    else
        {
        // use parent window as compound control
        SetContainerWindowL(*aParent);
        SetRect(aRect);
        }
    }
 
void CPanControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if((aPointerEvent.iType == TPointerEvent::EDrag) || (aPointerEvent.iType == TPointerEvent::EButton1Down))
        {
        iCurrentPointerPos = aPointerEvent.iPosition;
        }
	DrawNow(); // Draws the entire control
	iWsSession.Finish(); // Wait until WServ has finished drawing
	iIterations++; // Update frame counter
	iSemaphore.Signal(); // Signal test that control was drawn
	}
 
void CPanControl::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    TRect sourceRect = Rect();
    sourceRect.Move(iCurrentPointerPos);
    gc.DrawBitmap(aRect, iSourceBitmap, sourceRect); 
    }

TInt CPanControl::Iterations()
    {
    return iIterations;
    }

CFbsBitmap* CPanControl::Bitmap()
    {
    return iSourceBitmap;
    }
