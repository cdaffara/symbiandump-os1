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


#include "tscale.h"
#include "te_gesturegenerator.h"
#include "twindow.h"


_LIT(KSemaphore, "SemTScaleSync"); // Name of the global semaphore

const TInt KGestureGenerationLimit = 50;

// Test bitmap file location
_LIT(KBigBitmap,"z:\\resource\\apps\\uibench_s60_big.mbm");
_LIT(KTestStep0007,"GRAPHICS-UI-BENCH-S60-0007");

CTScale::CTScale()
	{
	SetTestStepName(KTScale);	
	}

TVerdict CTScale::doTestStepPreambleL()
    {
    // The semaphore has to be created before, otherwise the control can't open it.
    TESTNOERRORL(iSemaphore.CreateGlobal(KSemaphore, 0));
    return CTe_ConeStepBase::doTestStepPreambleL();
    }

TVerdict CTScale::doTestStepPostambleL()
    {
    iSemaphore.Close();
    return CTe_ConeStepBase::doTestStepPostambleL(); 
    }

/**
   @SYMTestCaseID GRAPHICS-UI-BENCH-S60-0007
   @SYMTestCaseDesc Measures the time for scaling in reaction to pointer events.
   @SYMTestActions Simulate horizontal and vertical drag pointer events and scales a bitmap.
   @SYMTestExpectedResults Measure and display the average framerate for the whole test.
*/
TVerdict CTScale::doTestStepL()
	{
	iProfiler->InitResults();
    // Use CGestureGenerator to simulate some horizontal drag pointer events.
	GestureGenerator::SimulateFlickGestureL(iSemaphore, TPoint(0, 0), 
	    TPoint(KGestureGenerationLimit, 0));
    
    // now some vertical drag
	GestureGenerator::SimulateFlickGestureL(iSemaphore, TPoint(KGestureGenerationLimit, 0), 
    	    TPoint(KGestureGenerationLimit, KGestureGenerationLimit));
    
	iProfiler->MarkResultSetL();
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0007, 0, 0, 0, iAppUi->ScaleControl()->Iterations(), screenSize.iWidth * screenSize.iHeight);
    return TestStepResult();
	} 

void CTScale::InitUIL(CCoeEnv* aCoeEnv)
	{
  	iAppUi = new(ELeave) CScaleAppUi();
  	// iAppUi needs to be put on the cleanupstack until CCoeEnv takes ownership of iAppUi
  	CleanupStack::PushL(iAppUi);
   	iAppUi->ConstructL();
   	CleanupStack::Pop(iAppUi);
   	aCoeEnv->SetAppUi(iAppUi);
  	}


CScaleControl* CScaleControl::NewL(const TRect& aRect,const CCoeControl* aParent)
    {
    CScaleControl* self = CScaleControl::NewLC(aRect,aParent);
    CleanupStack::Pop(self);
    return self;
    }
 
CScaleControl* CScaleControl::NewLC(const TRect& aRect,const CCoeControl* aParent)
    {
    CScaleControl* self = new(ELeave) CScaleControl();
    CleanupStack::PushL(self);
    self->ConstructL(aRect,aParent);
    return self;
    }
 
CScaleControl::CScaleControl() : iWsSession(CCoeEnv::Static()->WsSession())
	{
	// empty
    }
 
CScaleControl::~CScaleControl()
    {
    delete iSourceBitmap;
    iSemaphore.Close();
    }
 
void CScaleControl::ConstructL(const TRect& aRect,const CCoeControl* aParent)
    {
    User::LeaveIfError(iSemaphore.OpenGlobal(KSemaphore));
    // No owner, so create an own window
    if(!aParent)     
        {
        CreateWindowL();
        DrawableWindow()->PointerFilter(EPointerFilterDrag, 0);
        SetRect(aRect);
        ActivateL();
        }
    // Use Parent's window
    else
        {
        // This is component in a compound control
        SetContainerWindowL(*aParent);
        SetRect(aRect);
        }
    
    iSourceBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iSourceBitmap->Load(KBigBitmap, 0));
    iSourceRect = iSourceBitmap->SizeInPixels();
    iSourceRatio = (TReal)iSourceRect.Height()/(TReal)iSourceRect.Width();    
    }
 
/**
 * Compute the new zoom rectangle based on the type of pointer event.
 * Horizontal events make the source rectangle smaller (zooming in)
 * and vertical events make it larger (zooming out).
 * @param aPointerEvent Current Pointer position.
 */
void CScaleControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if(aPointerEvent.iType == TPointerEvent::EDrag)
		{
		if (iCurrentPointerPos.iY == aPointerEvent.iPosition.iY)
			{
			iSourceRect.Shrink(1,0);
			// After shrinking the width, calculate amount to shrink height.
			iSourceRect.Shrink(0, iSourceRect.Height() - (iSourceRect.Width()*iSourceRatio));
			}
		else 
			{
			iSourceRect.Grow(1, 0);
		    iSourceRect.Grow(0, (iSourceRect.Width()*iSourceRatio) - iSourceRect.Height());
			}
		iCurrentPointerPos = aPointerEvent.iPosition;
		}	
	DrawNow(); // Draws the entire control
	iWsSession.Finish(); // Wait until WServ has finished drawing
	iIterations++; // Update frame counter
	iSemaphore.Signal(); // Signal test that control was drawn
	}

/**
 * Scales and draws the bitmap to the screen.
 * @param aRect Region that covered by this control
 */
void CScaleControl::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.DrawBitmap(aRect, iSourceBitmap, iSourceRect); // scale and draw the bitmap
    }

TInt CScaleControl::Iterations()
    {
    return iIterations;
    }


CScaleAppUi::CScaleAppUi()
	{
	// empty
	}

void CScaleAppUi::ConstructL()
    {
    BaseConstructL(ENoAppResourceFile);
    iScale = CScaleControl::NewL(TRect(CTWindow::GetDisplaySizeInPixels()));
    AddToStackL(iScale);
    } 

CScaleAppUi::~CScaleAppUi()
	{
	RemoveFromStack(iScale);
	delete iScale;
	}

CScaleControl* CScaleAppUi::ScaleControl()
    {
    return iScale;
    }
